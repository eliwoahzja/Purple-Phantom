#include "Movement/MovementPolicy.h"
#include <array>
#include <atomic>
#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <limits>
#include <thread>

using namespace PhantomMovement;

namespace {
bool SameFloat(float left, float right) {
    return std::memcmp(&left,&right,sizeof(left)) == 0;
}
}

int main() {
    auto settings = GetSettings();
    assert(!settings.noCrouch && !settings.noSprintToFireDelay);
    assert(std::strcmp(GetNoCrouchStatusText(),"Disabled") == 0);
    assert(std::strcmp(GetSprintToFireStatusText(),"Disabled") == 0);

    SetSettings({true,false});
    settings = GetSettings();
    assert(settings.noCrouch && !settings.noSprintToFireDelay);
    assert(std::strstr(GetNoCrouchStatusText(),"Unavailable"));
    assert(std::strcmp(GetSprintToFireStatusText(),"Disabled") == 0);
    SetSettings({false,true});
    settings = GetSettings();
    assert(!settings.noCrouch && settings.noSprintToFireDelay);
    assert(std::strcmp(GetNoCrouchStatusText(),"Disabled") == 0);
    assert(std::strstr(GetSprintToFireStatusText(),"Unavailable"));
    // A setting cannot approve bindings, including in this host-only build.
    InitializeNativeMovement(1);
    assert(std::strstr(GetSprintToFireStatusText(),"Unavailable"));

    // UI/game-thread readers must receive a whole settings snapshot.
    SetSettings({});
    std::atomic<bool> begin{false};
    std::thread writer([&] {
        while (!begin.load(std::memory_order_acquire)) std::this_thread::yield();
        for (unsigned i = 0; i < 50000; ++i) SetSettings({(i&1u)!=0u,(i&1u)!=0u});
    });
    begin.store(true,std::memory_order_release);
    for (unsigned i = 0; i < 50000; ++i) {
        const auto snapshot = GetSettings();
        assert(snapshot.noCrouch == snapshot.noSprintToFireDelay);
    }
    writer.join();
    SetSettings({});

    const LocalContext local{1,2,2,true};
    assert(IsLocalPawn(local));
    assert(IsLocalController(local,10,10));
    assert(!IsLocalController(local,10,11)); // A replaced controller cannot act.
    assert(!IsLocalController(local,0,0));
    const std::array<LocalContext,6> ineligible{{
        {0,2,2,true},  // Match ended.
        {1,3,2,true},  // Another player's pawn.
        {1,2,4,true},  // Old pawn after local respawn.
        {1,0,2,true},
        {1,2,0,true},
        {1,2,2,false}  // Local death.
    }};
    const Settings enabled{true,true};
    for (const auto& context : ineligible) {
        assert(!IsLocalPawn(context));
        assert(!IsLocalController(context,10,10));
        assert(AdjustSprintDelay(enabled,true,context,true,.35f) == .35f);
    }

    // Stand requests resume after a normal crouch transition, then cease when
    // standing. Prone and nonwalking states never receive this request.
    Stance stance{true,false,1,true};
    assert(!ShouldStand(stance));
    stance.transitioning = false;
    assert(ShouldStand(stance));
    stance.crouched = false;
    assert(!ShouldStand(stance));
    stance = {true,true,1,false};
    assert(!ShouldStand(stance));
    stance = {false,true,1,false};
    assert(!ShouldStand(stance));
    for (int state : {-1,0,2,3,4,5,6,10,17,33,std::numeric_limits<int>::max()}) {
        stance = {true,false,state,false};
        assert(!ShouldStand(stance));
    }

    for (float delay : {.01f,.25f,1.f,std::numeric_limits<float>::min(),
                         std::numeric_limits<float>::max()}) {
        assert(AdjustSprintDelay(enabled,true,local,true,delay) == 0.f);
        assert(AdjustSprintDelay(enabled,false,local,true,delay) == delay);
        assert(AdjustSprintDelay(enabled,true,local,false,delay) == delay);
    }
    // Turning the sprint option off restores every native value immediately;
    // the independent no-crouch option cannot alter it.
    const std::array<float,8> nativeValues{{.25f,0.f,-0.f,-1.f,
        std::numeric_limits<float>::infinity(),-std::numeric_limits<float>::infinity(),
        std::numeric_limits<float>::quiet_NaN(),std::numeric_limits<float>::denorm_min()}};
    for (float value : nativeValues) {
        assert(SameFloat(AdjustSprintDelay({},true,local,true,value),value));
        assert(SameFloat(AdjustSprintDelay({true,false},true,local,true,value),value));
        if (!std::isfinite(value) || value <= 0.f)
            assert(SameFloat(AdjustSprintDelay(enabled,true,local,true,value),value));
    }
    assert(!GetSettings().noCrouch && !GetSettings().noSprintToFireDelay);
    std::puts("movement runtime tests passed");
}
