#include "MovementRuntime.h"
#include "MovementPolicy.h"
#include <atomic>
#include <cstring>

#if defined(__ANDROID__) && defined(__aarch64__) && !defined(STUDIO_STANDALONE_PREVIEW)
#include "../Trigger/NativeProfile.h"
#include "../IL2CppSDKGenerator/Dobby/dobby.h"
#endif

namespace PhantomMovement {
namespace {
constexpr unsigned NoCrouchBit = 1u, NoSprintDelayBit = 2u;
std::atomic<unsigned> requested{0u};
std::atomic<const char*> crouchStatus{"Unavailable: native verification pending"};
std::atomic<const char*> sprintStatus{"Unavailable: native verification pending"};

#if defined(__ANDROID__) && defined(__aarch64__) && !defined(STUDIO_STANDALONE_PREVIEW)
using Address = std::uintptr_t;
using Method = const void*;
using Getter = Address(*)(void*,Method);
using Predicate = bool(*)(Address,Method);
using CrouchFunction = void(*)(void*,Method);
using UpdateFunction = void(*)(void*,float,Method);
using DelayFunction = float(*)(void*,Method);
using PhantomTrigger::NativeProfileDetail::CodeCheck;
#include "NativeMovementChecks.inc"

Address library = 0;
std::atomic<bool> crouchInstalled{false}, sprintInstalled{false};
CrouchFunction originalCrouch = nullptr;
UpdateFunction originalUpdate = nullptr;
DelayFunction originalDelay = nullptr;
thread_local unsigned updateDepth = 0;

template<class Function> Function At(Address rva) noexcept {
    return reinterpret_cast<Function>(library+rva);
}
template<class Value> Value Field(Address object, std::size_t offset) noexcept {
    Value value{};
    if (object) std::memcpy(&value,reinterpret_cast<const void*>(object+offset),sizeof(value));
    return value;
}
template<std::size_t Count> bool Patched(const int (&ids)[Count]) noexcept {
    // Native bitmap query takes w0 directly, without an IL2CPP static receiver.
    for (int id : ids) if (At<bool(*)(int)>(0x3295318)(id)) return true;
    return false;
}

LocalContext PawnContext(Address pawn, Address match) noexcept {
    // These old IL2CPP static methods reserve a leading null receiver.
    const auto localPawn = match ? At<Getter>(0x856AA08)(nullptr,nullptr) : 0;
    const bool alive = pawn && pawn == localPawn && Field<bool>(pawn,0x548);
    return {match,pawn,localPawn,alive};
}

bool LocalController(void* controller, Address& pawn) noexcept {
    pawn = 0;
    if (!controller) return false;
    const auto match = At<Getter>(0x8569B10)(nullptr,nullptr);
    if (!match) return false; // LocalPC requires an active gameplay object.
    const auto localController = At<Getter>(0x856A8A8)(nullptr,nullptr);
    if (reinterpret_cast<Address>(controller) != localController) return false;
    pawn = Field<Address>(localController,0x78);
    return IsLocalController(PawnContext(pawn,match),reinterpret_cast<Address>(controller),localController);
}

bool CrouchCompatible() noexcept {
    constexpr int ids[]{0xE4B1,0xE49F,0x85AF};
    const bool compatible = !Patched(ids);
    crouchStatus.store(compatible ? "Ready" : "Unavailable: native stance hotfix is active",
                       std::memory_order_release);
    return compatible;
}

void CheckCrouch(void* controller, Method metadata) {
    Address pawn = 0;
    if (crouchInstalled.load(std::memory_order_acquire) && GetSettings().noCrouch &&
        LocalController(controller,pawn) && CrouchCompatible()) return;
    originalCrouch(controller,metadata);
}

void UpdateLocalPawn(void* controller, float deltaTime, Method metadata) {
    ++updateDepth;
    originalUpdate(controller,deltaTime,metadata);
    Address pawn = 0;
    if (updateDepth == 1 && crouchInstalled.load(std::memory_order_acquire) &&
        GetSettings().noCrouch && LocalController(controller,pawn) && CrouchCompatible()) {
        Stance stance{Field<bool>(pawn,0x4C8),Field<bool>(pawn,0x4C9),Field<int>(pawn,0x510),false};
        if (ShouldStand(stance)) {
            stance.transitioning = At<Predicate>(0x51B463C)(pawn,nullptr);
            if (ShouldStand(stance)) {
                // Preserve normal clearance checks and camera transitions.
                // Never write stance flags or force the pawn through a ceiling.
                At<void(*)(Address,bool,bool,Method)>(0x518BC48)(pawn,false,false,nullptr);
            }
        }
    }
    --updateDepth;
}

float SprintDelay(void* weapon, Method metadata) {
    // Retain native metadata initialization and owner/property evaluation.
    const float nativeDelay = originalDelay(weapon,metadata);
    const auto settings = GetSettings();
    if (!sprintInstalled.load(std::memory_order_acquire) || !settings.noSprintToFireDelay ||
        !weapon || !std::isfinite(nativeDelay) || nativeDelay <= 0.f) return nativeDelay;
    const auto address = reinterpret_cast<Address>(weapon);
    const auto match = At<Getter>(0x8569B10)(nullptr,nullptr);
    const auto context = PawnContext(Field<Address>(address,0x90),match);
    if (!IsLocalPawn(context)) return nativeDelay;
    constexpr int ids[]{0x1258A,0x1258B,0x1258C};
    const bool compatible = !Patched(ids);
    sprintStatus.store(compatible ? "Ready; native firing interval remains" :
        "Unavailable: native sprint timing hotfix is active",std::memory_order_release);
    if (!compatible) return nativeDelay;
    const bool ordinaryGun = At<Predicate>(0xC364D3C)(address,nullptr) &&
                             At<Predicate>(0xC365324)(address,nullptr);
    return AdjustSprintDelay(settings,compatible,context,ordinaryGun,nativeDelay);
}

template<std::size_t Count>
bool Validate(const CodeCheck (&checks)[Count], std::atomic<const char*>& status) noexcept {
    const char* failure = nullptr;
    const bool valid = PhantomTrigger::NativeProfileDetail::ValidateEntrypoints(
        library,checks,Count,failure);
    if (!valid) status.store(failure,std::memory_order_release);
    return valid;
}

void InstallCrouch() noexcept {
    if (!Validate(commonChecks,crouchStatus) || !Validate(crouchChecks,crouchStatus)) return;
    struct Hook { Address rva; void* replacement; void** original; };
    const Hook hooks[]{
        {0x666AE64,reinterpret_cast<void*>(&CheckCrouch),reinterpret_cast<void**>(&originalCrouch)},
        {0x666894C,reinterpret_cast<void*>(&UpdateLocalPawn),reinterpret_cast<void**>(&originalUpdate)}
    };
    unsigned installed = 0;
    for (const auto& hook : hooks) {
        const int result = DobbyHook(reinterpret_cast<void*>(library+hook.rva),hook.replacement,hook.original);
        if (result != 0 || !*hook.original) {
            if (result == 0) DobbyDestroy(reinterpret_cast<void*>(library+hook.rva));
            while (installed) DobbyDestroy(reinterpret_cast<void*>(library+hooks[--installed].rva));
            crouchStatus.store("Unavailable: crouch hook installation failed",std::memory_order_release);
            return;
        }
        ++installed;
    }
    crouchInstalled.store(true,std::memory_order_release);
    crouchStatus.store("Ready",std::memory_order_release);
}

void InstallSprint() noexcept {
    if (!Validate(commonChecks,sprintStatus) || !Validate(sprintChecks,sprintStatus)) return;
    const int result = DobbyHook(reinterpret_cast<void*>(library+0x513CBC4),
        reinterpret_cast<void*>(&SprintDelay),reinterpret_cast<void**>(&originalDelay));
    if (result != 0 || !originalDelay) {
        if (result == 0) DobbyDestroy(reinterpret_cast<void*>(library+0x513CBC4));
        sprintStatus.store("Unavailable: sprint timing hook installation failed",std::memory_order_release);
        return;
    }
    sprintInstalled.store(true,std::memory_order_release);
    sprintStatus.store("Ready; native firing interval remains",std::memory_order_release);
}
#endif
} // namespace

Settings GetSettings() noexcept {
    const auto bits = requested.load(std::memory_order_acquire);
    return {(bits&NoCrouchBit) != 0u,(bits&NoSprintDelayBit) != 0u};
}
void SetSettings(const Settings& settings) noexcept {
    requested.store((settings.noCrouch ? NoCrouchBit : 0u) |
                    (settings.noSprintToFireDelay ? NoSprintDelayBit : 0u),std::memory_order_release);
}
const char* GetNoCrouchStatusText() noexcept {
    return GetSettings().noCrouch ? crouchStatus.load(std::memory_order_acquire) : "Disabled";
}
const char* GetSprintToFireStatusText() noexcept {
    return GetSettings().noSprintToFireDelay ? sprintStatus.load(std::memory_order_acquire) : "Disabled";
}
void InitializeNativeMovement(std::uintptr_t base) noexcept {
#if defined(__ANDROID__) && defined(__aarch64__) && !defined(STUDIO_STANDALONE_PREVIEW)
    static std::atomic<bool> attempted{false};
    if (!base || attempted.exchange(true,std::memory_order_acq_rel)) return;
    library = base;
    InstallCrouch();
    InstallSprint();
#else
    (void)base;
#endif
}
} // namespace PhantomMovement
