#include "NativeDriver.h"
#include "NativeCancellation.h"
#include <atomic>
#include <cmath>
#include <cstring>

#if defined(__aarch64__)
#include "NativeQueries.h"
#include "../IL2CppSDKGenerator/Dobby/dobby.h"
#endif

namespace PhantomTrigger {
namespace {
std::atomic<RuntimeStatus> status{RuntimeStatus::WaitingForBinaryVerification};
std::atomic<const char*> reason{"Native profile verification pending"};
std::atomic<FireMode> detected{FireMode::Unsupported};
std::atomic<std::uint64_t> viewport{0};
std::atomic<bool> viewportValid{false};
std::atomic<std::uintptr_t> observedComponent{0};
std::atomic<std::uint64_t> committedShots{0};

#if defined(__aarch64__)
std::uintptr_t library = 0;
std::atomic<unsigned> installation{0};
std::atomic<std::uint64_t> lifecycle{1};
using Metadata = const void*;
using UpdateFunction = void(*)(void*, float, Metadata);
using ButtonFunction = void(*)(void*, void*, Metadata);
using StartMatchFunction = void(*)(void*, void*, Metadata);
using EndMatchFunction = void(*)(void*, Metadata);
UpdateFunction originalUpdate = nullptr;
ButtonFunction originalButtons = nullptr;
StartMatchFunction originalStart = nullptr;
EndMatchFunction originalEnd = nullptr;

template<class Function> Function At(std::uintptr_t rva) noexcept {
    return reinterpret_cast<Function>(library + rva);
}
template<class Value> Value Field(std::uintptr_t object, std::size_t offset) noexcept {
    Value value{};
    std::memcpy(&value, reinterpret_cast<const void*>(object + offset), sizeof(value));
    return value;
}
using Getter = std::uintptr_t(*)(void*, Metadata);
using ObjectGetter = std::uintptr_t(*)(std::uintptr_t, Metadata);
using BoolGetter = bool(*)(std::uintptr_t, Metadata);
using FloatGetter = float(*)(std::uintptr_t, Metadata);
using BoolSetter = void(*)(void*, bool, Metadata);

struct LocalState {
    Controller controller;
    std::uintptr_t scope = 0;
    unsigned depth = 0;
    bool seen = false;
    std::uint64_t update = 0;
    std::uint64_t epoch = 0;
    std::uint64_t generations[5]{1,1,1,1,1};
    std::uintptr_t previous[5]{};
    Frame lastFrame{};
    bool hasFrame = false;
    NativeCancellation cancellation;
    bool stopAfterUpdate = false;
    bool syntheticInput = false;
};
thread_local LocalState local;

bool InputHotfixActive() noexcept {
    using IsPatched = bool(*)(int);
    // Verified native C++ bitmap query, not an IL2CPP managed static method.
    constexpr int ids[]{0x11f06,0x11f41,0x11f36,0x11f21,0x11f1e,0x11f24,
                        0x11f1b,0x11f1c,0x11f1d,0x11f1f,0x11f20,0x11f0f,
                        0x9118,0x955d};
    for (int id : ids) if (At<IsPatched>(0x3295318)(id)) return true;
    return false;
}

bool HardStateBlocked(std::uintptr_t weapon) noexcept {
    const auto machine = Field<std::uintptr_t>(weapon, 0x170);
    if (!machine) return true;
    const auto current = Field<std::uintptr_t>(machine, 0x18);
    if (!current) return true;
    const int type = Field<int>(current, 0x20);
    switch (type) {
        // Ordinary cadence/bolt states retain held intent. Equipment, reload,
        // charging/preparation, empty, melee, secondary, and unknown states do not.
        case 3: case 4: case 6: case 7: case 11: case 12:
        case 16: case 17: case 19: case 20: case 33: case 34:
        case 35: case 36: case 37: case 38: case 39: case 45:
            return false;
        default: return true;
    }
}

bool PhysicalFire(std::uintptr_t param) noexcept {
    constexpr std::size_t fields[]{0x18,0x1a,0x1b,0x1c,0x1e,0x1f,0x20,0x24,0x25};
    for (auto offset : fields) if (Field<bool>(param, offset)) return true;
    // ADS press/down/up are deliberately not treated as physical FIRE: the
    // trigger may fire while the player holds the independent ADS button.
    return false;
}

ContextIdentity Identity(std::uintptr_t match, std::uintptr_t pawn,
                         std::uintptr_t weapon, std::uintptr_t component,
                         std::uintptr_t controller) noexcept {
    const std::uintptr_t now[]{match,pawn,weapon,component,controller};
    const auto epoch = lifecycle.load(std::memory_order_acquire);
    if (local.epoch != epoch) {
        local.controller.Reset();
        local.cancellation.Reset();
        local.epoch = epoch;
        for (auto& generation : local.generations) ++generation;
    }
    for (unsigned i = 0; i < 5; ++i) {
        if (local.previous[i] != now[i]) ++local.generations[i];
        local.previous[i] = now[i];
    }
    return {local.generations[0],local.generations[1],local.generations[2],
            local.generations[3],local.generations[4]};
}

void Merge(void* param, const Intent& intent) noexcept {
    if (intent.passthrough) return;
    // No aim/ADS flags, weapon timing, view angles, or shared result reset.
    At<BoolSetter>(0xA81F820)(param, intent.held, nullptr);
    At<BoolSetter>(0xA81F7E8)(param, intent.down, nullptr);
    const bool up = intent.up;
    std::memcpy(static_cast<unsigned char*>(param) + 0x1a, &up, sizeof(up));
    At<BoolSetter>(0xA81F898)(param, intent.down, nullptr);
    At<BoolSetter>(0xA81F8A8)(param, intent.up, nullptr);
}

bool PrepareViewRay(bool needed, ViewRay& ray) noexcept {
    float centerX = .5f, centerY = .5f;
    const bool ready = needed && BuildNativeRay(library,ray,centerX,centerY);
    if (ready) {
        std::uint32_t x = 0, y = 0;
        std::memcpy(&x,&centerX,sizeof(x)); std::memcpy(&y,&centerY,sizeof(y));
        viewport.store((static_cast<std::uint64_t>(x)<<32)|y,std::memory_order_relaxed);
    }
    viewportValid.store(ready,std::memory_order_release);
    return ready;
}

void Buttons(void* unit, void* param, Metadata metadata) {
    originalButtons(unit, param, metadata);
    if (installation.load(std::memory_order_acquire) != 2 ||
        !local.scope || local.depth != 1 || local.seen || !unit || !param) return;
    const auto controller = local.scope;
    if (At<Getter>(0xBB503EC)(nullptr,nullptr) != controller ||
        Field<std::uintptr_t>(controller, 0x50) != reinterpret_cast<std::uintptr_t>(unit)) return;
    local.seen = true;
    const auto settings = GetSettings();
    if (!settings.enabled && !local.controller.HasOwnedInput() &&
        !local.cancellation.HasPendingCleanup()) {
        local.controller.Reset();
        local.hasFrame = false;
        status.store(RuntimeStatus::Disabled,std::memory_order_release);
        reason.store("Disabled",std::memory_order_release);
        detected.store(FireMode::Unsupported,std::memory_order_release);
        observedComponent.store(0,std::memory_order_release);
        ViewRay ray;
        PrepareViewRay(settings.showDot,ray);
        return;
    }
    const auto pawn = Field<std::uintptr_t>(controller, 0x70);
    const auto weapon = Field<std::uintptr_t>(controller, 0x68);
    const auto match = At<Getter>(0x8569B10)(nullptr,nullptr);
    if (!match || !pawn || !weapon || pawn != At<Getter>(0x856AA08)(nullptr,nullptr) ||
        !Field<bool>(pawn,0x548)) {
        local.controller.Reset();
        local.cancellation.Reset();
        detected.store(FireMode::Unsupported, std::memory_order_release);
        status.store(RuntimeStatus::WaitingForContext, std::memory_order_release);
        viewportValid.store(false, std::memory_order_release);
        observedComponent.store(0,std::memory_order_release);
        return;
    }
    if (InputHotfixActive()) {
        // Revoke only an input stream this controller actually owned. Native
        // physical activity keeps priority even during compatibility loss.
        const bool sameOwner = local.previous[0] == match && local.previous[1] == pawn &&
            local.previous[2] == weapon && local.previous[4] == controller &&
            local.epoch == lifecycle.load(std::memory_order_acquire);
        if (local.hasFrame && sameOwner) {
            auto stopped = local.lastFrame;
            stopped.updateId = local.update;
            stopped.contextValid = false;
            stopped.manualActive = PhysicalFire(reinterpret_cast<std::uintptr_t>(param));
            const auto revoke = local.controller.Step(GetSettings(),stopped);
            // A patched input path may interpret Up as Fire. Revoke through
            // StopFire only; never synthesize an unknown release edge.
            local.stopAfterUpdate = !revoke.passthrough && revoke.up;
        }
        local.controller.Reset();
        NativeDriverFailure("Unsupported native input hotfix");
        return;
    }
    const auto stateMachine = Field<std::uintptr_t>(weapon,0x170);
    if (!stateMachine || !Field<std::uintptr_t>(stateMachine,0x18)) {
        local.controller.Reset();
        local.cancellation.Reset();
        observedComponent.store(0,std::memory_order_release);
        viewportValid.store(false,std::memory_order_release);
        status.store(RuntimeStatus::WaitingForContext,std::memory_order_release);
        reason.store("Waiting for an initialized weapon",std::memory_order_release);
        return;
    }
    const auto component = At<ObjectGetter>(0x50EB1E4)(weapon, nullptr);
    if (!component) {
        local.controller.Reset();
        local.cancellation.Reset();
        observedComponent.store(0,std::memory_order_release);
        viewportValid.store(false,std::memory_order_release);
        status.store(RuntimeStatus::WaitingForContext,std::memory_order_release);
        reason.store("Waiting for a weapon fire component",std::memory_order_release);
        return;
    }
    observedComponent.store(component,std::memory_order_release);
    Frame frame;
    frame.identity = Identity(match,pawn,weapon,component,controller);
    frame.updateId = local.update;
    frame.contextValid = component != 0;
    frame.bindingsReady = true;
    const auto nativeSettings = At<ObjectGetter>(0xBB4A5BC)(controller,nullptr);
    const bool nativeAuto = !nativeSettings ||
        At<BoolGetter>(0xC34A0B8)(nativeSettings,nullptr);
    frame.manualActive = nativeAuto || PhysicalFire(reinterpret_cast<std::uintptr_t>(param));
    frame.hardBlocked = !Field<bool>(pawn,0x548) || HardStateBlocked(weapon) ||
        nativeAuto || !At<BoolGetter>(0xBB4BD5C)(controller,nullptr);
    frame.ammoAvailable = At<BoolGetter>(0x50EB73C)(weapon,nullptr);
    frame.aiming = At<BoolGetter>(0x519BB10)(pawn,nullptr);
    const float percentage = At<FloatGetter>(0x5113B4C)(weapon,nullptr);
    frame.fullyAimed = frame.aiming && std::isfinite(percentage) && percentage >= 1.f;
    frame.shotReady = At<BoolGetter>(0x5126CC4)(weapon,nullptr);
    // The game's per-fire-group ShotFired counter can reset to the same value
    // between presses. Observe committed local bullet creation instead.
    frame.shotSerial = committedShots.load(std::memory_order_acquire);
    const bool burst = At<BoolGetter>(0x50EBB28)(weapon,nullptr);
    const auto klass = Field<std::uintptr_t>(weapon,0);
    const auto prepare = klass ? Field<std::uintptr_t>(klass,0xD70) : 0;
    const bool knownPrepare = prepare == library + 0x511AB0C ||
                              prepare == library + 0xC3DC644;
    const bool potentiallyRelease = At<BoolGetter>(0x50EE190)(weapon,nullptr) ||
                                   prepare == library + 0xC3DC644;
    const bool release = !knownPrepare || (potentiallyRelease &&
        At<BoolGetter>(0xBB4DF2C)(controller,nullptr));
    const bool ordinaryGun = At<BoolGetter>(0xC364D3C)(weapon,nullptr) &&
        At<BoolGetter>(0xC365324)(weapon,nullptr);
    if (component && ordinaryGun && !burst && !release) {
        frame.fireMode = At<BoolGetter>(0x50EBA5C)(weapon,nullptr)
            ? FireMode::Single : FireMode::Automatic;
    }
    if (installation.load(std::memory_order_acquire) != 2) {
        local.controller.Reset();
        return;
    }
    detected.store(frame.fireMode,std::memory_order_release);
    status.store(RuntimeStatus::Ready,std::memory_order_release);
    reason.store(nativeAuto ? "Native auto-fire controls are unsupported" :
        (!knownPrepare ? "Unsupported weapon input handler" :
         (release ? "Release-to-fire mode is unsupported" :
          (burst ? "Burst mode is unsupported" : "Ready"))),std::memory_order_release);
    ViewRay ray;
    const bool rayReady = PrepareViewRay(settings.showDot ||
        NeedsTargetQuery(settings,frame,local.controller),ray);
    frame.contextValid = frame.contextValid && rayReady;
    NativeTargetQueries queries(library,match,pawn);
    const auto intent = EvaluateInput(settings,frame,ray,queries,local.controller);
    const auto decision = local.cancellation.Resolve(frame.identity,frame.manualActive,release,intent);
    if (decision.resetController) local.controller.Reset();
    local.stopAfterUpdate = decision.stopResult;
    local.syntheticInput = !decision.input.passthrough;
    local.lastFrame = frame;
    local.hasFrame = true;
    if (installation.load(std::memory_order_acquire) == 2) Merge(param,decision.input);
    else local.controller.Reset();
}

void Update(void* controller, float deltaTime, Metadata metadata) {
    if (installation.load(std::memory_order_acquire) != 2) {
        originalUpdate(controller,deltaTime,metadata);
        return;
    }
    ++local.depth;
    if (local.depth != 1) {
        originalUpdate(controller,deltaTime,metadata);
        --local.depth;
        return;
    }
    local.scope = reinterpret_cast<std::uintptr_t>(controller);
    local.seen = false;
    local.stopAfterUpdate = false;
    local.syntheticInput = false;
    ++local.update;
    originalUpdate(controller,deltaTime,metadata);
    const bool sameOwner = local.seen && local.hasFrame && !local.lastFrame.manualActive &&
        local.previous[1] == Field<std::uintptr_t>(local.scope,0x70) &&
        local.previous[2] == Field<std::uintptr_t>(local.scope,0x68) &&
        local.epoch == lifecycle.load(std::memory_order_acquire);
    // Weapon.PrepareInputParam runs after the button hook and can change the
    // release-fire marker. Catch that transition before the caller consumes an
    // Up as a shot, or starts treating a held input as release-to-fire. This
    // adapter never supplies a release-triggered shot.
    if (sameOwner && local.syntheticInput) {
        const auto param = Field<std::uintptr_t>(local.scope,0x38);
        if (param && Field<bool>(param,0x14) &&
            At<BoolGetter>(0xBB4DF2C)(local.scope,nullptr)) {
            const Intent unsafeUp{false,false,true,false};
            local.cancellation.Resolve(local.lastFrame.identity,false,true,unsafeUp);
            local.controller.Reset();
            local.stopAfterUpdate = true;
        }
    }
    if (local.stopAfterUpdate && sameOwner) {
        // UpdateFireInput produces this result; its caller consumes it after
        // this hook returns. Stop only our source without a release-fire edge.
        const auto result = Field<std::uintptr_t>(local.scope,0x40);
        if (result) {
            auto* object = reinterpret_cast<void*>(result);
            At<BoolSetter>(0xA81FA84)(object,false,nullptr);
            At<BoolSetter>(0xA81FB30)(object,true,nullptr);
            const bool noEdge = false;
            std::memcpy(reinterpret_cast<unsigned char*>(result)+0x15,&noEdge,sizeof(noEdge));
            std::memcpy(reinterpret_cast<unsigned char*>(result)+0x16,&noEdge,sizeof(noEdge));
        }
    }
    local.scope = 0;
    if (!local.seen) {
        local.controller.Reset();
        observedComponent.store(0,std::memory_order_release);
        detected.store(FireMode::Unsupported,std::memory_order_release);
        viewportValid.store(false,std::memory_order_release);
        status.store(RuntimeStatus::WaitingForContext,std::memory_order_release);
        reason.store("Waiting for the native local input path",std::memory_order_release);
    }
    --local.depth;
}
void Start(void* controller, void* match, Metadata metadata) {
    lifecycle.fetch_add(1,std::memory_order_acq_rel);
    observedComponent.store(0,std::memory_order_release);
    originalStart(controller,match,metadata);
}
void End(void* controller, Metadata metadata) {
    lifecycle.fetch_add(1,std::memory_order_acq_rel);
    viewportValid.store(false,std::memory_order_release);
    observedComponent.store(0,std::memory_order_release);
    detected.store(FireMode::Unsupported,std::memory_order_release);
    if (installation.load(std::memory_order_acquire) == 2)
        status.store(RuntimeStatus::WaitingForContext,std::memory_order_release);
    originalEnd(controller,metadata);
}
#endif
} // namespace

bool InstallNativeDriver(std::uintptr_t base) noexcept {
#if defined(__aarch64__)
    unsigned expected = 0;
    if (!base || !installation.compare_exchange_strong(expected,1)) return expected == 2;
    library = base;
    struct Hook { std::uintptr_t rva; void* replacement; void** original; };
    const Hook hooks[]{
        {0xBB4AE14,reinterpret_cast<void*>(&Update),reinterpret_cast<void**>(&originalUpdate)},
        {0xBB4B66C,reinterpret_cast<void*>(&Buttons),reinterpret_cast<void**>(&originalButtons)},
        {0xBB4F2B4,reinterpret_cast<void*>(&Start),reinterpret_cast<void**>(&originalStart)},
        {0xBB4F3A4,reinterpret_cast<void*>(&End),reinterpret_cast<void**>(&originalEnd)}
    };
    unsigned installed = 0;
    for (const auto& hook : hooks) {
        const int result = DobbyHook(reinterpret_cast<void*>(base+hook.rva),hook.replacement,hook.original);
        if (result != 0 || !*hook.original) {
            if (result == 0) DobbyDestroy(reinterpret_cast<void*>(base+hook.rva));
            while (installed) DobbyDestroy(reinterpret_cast<void*>(base+hooks[--installed].rva));
            installation.store(3,std::memory_order_release);
            NativeDriverFailure("Native input hook installation failed");
            return false;
        }
        ++installed;
    }
    installation.store(2,std::memory_order_release);
    status.store(RuntimeStatus::WaitingForContext,std::memory_order_release);
    reason.store("Waiting for local gameplay input",std::memory_order_release);
    return true;
#else
    (void)base;
    return false;
#endif
}
RuntimeStatus NativeStatus() noexcept { return status.load(std::memory_order_acquire); }
const char* NativeStatusText() noexcept { return reason.load(std::memory_order_acquire); }
FireMode NativeDetectedMode() noexcept { return detected.load(std::memory_order_acquire); }
bool NativeViewportCenter(float& x, float& y) noexcept {
    x = y = .5f;
    if (!viewportValid.load(std::memory_order_acquire)) return false;
    const auto bits = viewport.load(std::memory_order_relaxed);
    const auto xBits = static_cast<std::uint32_t>(bits>>32);
    const auto yBits = static_cast<std::uint32_t>(bits);
    std::memcpy(&x,&xBits,sizeof(x)); std::memcpy(&y,&yBits,sizeof(y));
    return true;
}
void NativeDriverFailure(const char* message) noexcept {
#if defined(__aarch64__)
    installation.store(3,std::memory_order_release);
#endif
    reason.store(message ? message : "Native compatibility verification failed",std::memory_order_release);
    status.store(RuntimeStatus::InvalidNativeState,std::memory_order_release);
    detected.store(FireMode::Unsupported,std::memory_order_release);
    viewportValid.store(false,std::memory_order_release);
    observedComponent.store(0,std::memory_order_release);
}
void NotifyNativeShot(std::uintptr_t component) noexcept {
    if (component && component == observedComponent.load(std::memory_order_acquire))
        committedShots.fetch_add(1,std::memory_order_acq_rel);
}
} // namespace PhantomTrigger
