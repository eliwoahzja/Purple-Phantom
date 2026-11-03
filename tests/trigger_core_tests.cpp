#include "Trigger/TriggerCore.h"

#include <array>
#include <cstdio>
#include <cstdlib>
#include <type_traits>

using namespace PhantomTrigger;

namespace {

const char* currentTest = "startup";

[[noreturn]] void Fail(const char* expression, int line) {
    std::fprintf(stderr, "%s:%d: %s\n", currentTest, line, expression);
    std::abort();
}

#define CHECK(expression) ((expression) ? static_cast<void>(0) : Fail(#expression, __LINE__))

Settings Enabled() {
    return {true, Mode::Both, false};
}

Frame Ready(FireMode mode = FireMode::Automatic) {
    Frame frame;
    frame.identity = {1, 2, 3, 4, 5};
    frame.updateId = 1;
    frame.contextValid = true;
    frame.bindingsReady = true;
    frame.ammoAvailable = true;
    frame.targetValid = true;
    frame.shotReady = true;
    frame.fireMode = mode;
    return frame;
}

void Passthrough(const Intent& intent) {
    CHECK(intent.passthrough);
    CHECK(!intent.held && !intent.down && !intent.up);
}

void Down(const Intent& intent) {
    CHECK(!intent.passthrough);
    CHECK(intent.held && intent.down && !intent.up);
}

void Hold(const Intent& intent) {
    CHECK(!intent.passthrough);
    CHECK(intent.held && !intent.down && !intent.up);
}

void Up(const Intent& intent) {
    CHECK(!intent.passthrough);
    CHECK(!intent.held && !intent.down && intent.up);
}

void DefaultAndAutomaticCadence() {
    currentTest = "defaults and automatic cadence";
    Controller controller;
    Frame frame = Ready();
    Passthrough(controller.Step({}, frame));
    ++frame.updateId;
    Down(controller.Step(Enabled(), frame));
    for (int i = 0; i < 100; ++i) {
        ++frame.updateId;
        frame.shotReady = (i % 5) == 0;
        frame.shotSerial += frame.shotReady ? 1 : 0;
        Hold(controller.Step(Enabled(), frame));
    }
    ++frame.updateId;
    frame.targetValid = false;
    Up(controller.Step(Enabled(), frame));
    ++frame.updateId;
    Passthrough(controller.Step(Enabled(), frame));
    ++frame.updateId;
    frame.targetValid = true;
    frame.shotReady = false;
    Down(controller.Step(Enabled(), frame));
}

void EachHardGate() {
    currentTest = "each hard gate";
    // Every gate must inhibit acquisition, release owned input once, and permit
    // immediate fresh acquisition when that same gate clears.
    for (int gate = 0; gate != 9; ++gate) {
        Controller controller;
        Settings settings = Enabled();
        Frame frame = Ready();
        const auto block = [gate](Settings& config, Frame& value) {
            switch (gate) {
                case 0: config.enabled = false; break;
                case 1: value.contextValid = false; break;
                case 2: value.bindingsReady = false; break;
                case 3: value.ammoAvailable = false; break;
                case 4: value.hardBlocked = true; break;
                case 5: value.targetValid = false; break;
                case 6: value.fireMode = FireMode::Unsupported; break;
                case 7: config.mode = Mode::ADSOnly; break;
                case 8: config.mode = static_cast<Mode>(255); break;
                default: std::abort();
            }
        };
        block(settings, frame);
        Passthrough(controller.Step(settings, frame));
        settings = Enabled();
        frame = Ready();
        ++frame.updateId;
        // A mode change itself consumes one update even without ownership.
        if (gate >= 6) {
            Passthrough(controller.Step(settings, frame));
            ++frame.updateId;
        }
        Down(controller.Step(settings, frame));
        block(settings, frame);
        ++frame.updateId;
        Up(controller.Step(settings, frame));
        ++frame.updateId;
        Passthrough(controller.Step(settings, frame));
        settings = Enabled();
        Frame fresh = Ready();
        fresh.updateId = frame.updateId + 1;
        if (gate >= 6) {
            Passthrough(controller.Step(settings, fresh));
            ++fresh.updateId;
        }
        Down(controller.Step(settings, fresh));
    }
}

void ADSAndDotIndependence() {
    currentTest = "ADS and dot independence";
    Controller controller;
    Settings settings{true, Mode::ADSOnly, false};
    Frame frame = Ready();
    Passthrough(controller.Step(settings, frame));
    frame.aiming = true;
    ++frame.updateId;
    Passthrough(controller.Step(settings, frame));
    frame.fullyAimed = true;
    ++frame.updateId;
    Down(controller.Step(settings, frame));
    settings.showDot = true;
    ++frame.updateId;
    Hold(controller.Step(settings, frame));
    settings.showDot = false;
    ++frame.updateId;
    Hold(controller.Step(settings, frame));
    frame.aiming = false;
    ++frame.updateId;
    Up(controller.Step(settings, frame));
    ++frame.updateId;
    Passthrough(controller.Step(settings, frame));

    settings.mode = Mode::Both;
    frame.fullyAimed = false;
    ++frame.updateId;
    Passthrough(controller.Step(settings, frame));
    ++frame.updateId;
    Down(controller.Step(settings, frame));
    frame.aiming = true;
    frame.fullyAimed = true;
    ++frame.updateId;
    Hold(controller.Step(settings, frame));
}

void ManualInputOwnsEveryTransition() {
    currentTest = "manual input owns every transition";
    for (FireMode mode : {FireMode::Automatic, FireMode::Single}) {
        Controller controller;
        Frame frame = Ready(mode);
        Down(controller.Step(Enabled(), frame));
        frame.manualActive = true;
        ++frame.updateId;
        Passthrough(controller.Step(Enabled(), frame));
        frame.targetValid = false;
        frame.shotSerial = 1;
        ++frame.updateId;
        Passthrough(controller.Step(Enabled(), frame));
        frame.manualActive = false;
        ++frame.updateId;
        Passthrough(controller.Step(Enabled(), frame));
        frame.targetValid = true;
        ++frame.updateId;
        Down(controller.Step(Enabled(), frame));
        frame.manualActive = true;
        frame.ammoAvailable = false;
        ++frame.updateId;
        Passthrough(controller.Step({}, frame));
        frame.manualActive = false;
        ++frame.updateId;
        Passthrough(controller.Step({}, frame));
    }
}

void GenerationsAndReset() {
    currentTest = "generation changes and reset";
    using Token = std::uint64_t ContextIdentity::*;
    constexpr std::array<Token, 5> tokens{
        &ContextIdentity::match, &ContextIdentity::pawn, &ContextIdentity::weapon,
        &ContextIdentity::component, &ContextIdentity::inputMode};
    for (Token token : tokens) {
        Controller controller;
        Frame frame = Ready(FireMode::Single);
        Down(controller.Step(Enabled(), frame));
        ++(frame.identity.*token);
        frame.updateId = 0; // Each new generation owns its update sequence.
        frame.targetValid = false;
        Passthrough(controller.Step(Enabled(), frame));
        frame.targetValid = true;
        ++frame.updateId;
        Down(controller.Step(Enabled(), frame));
        frame.shotSerial = 1;
        ++frame.updateId;
        Up(controller.Step(Enabled(), frame));
        controller.Reset();
        frame.updateId = 0;
        Down(controller.Step(Enabled(), frame));
    }
}

void DuplicateAndOutOfOrderUpdates() {
    currentTest = "duplicate and out of order updates";
    Controller controller;
    Frame frame = Ready(FireMode::Single);
    frame.updateId = 10;
    Down(controller.Step(Enabled(), frame));
    Passthrough(controller.Step(Enabled(), frame));
    frame.shotSerial = 1;
    Passthrough(controller.Step(Enabled(), frame));
    frame.updateId = 9;
    frame.targetValid = false;
    Passthrough(controller.Step(Enabled(), frame));
    frame.updateId = 11;
    frame.targetValid = true;
    Up(controller.Step(Enabled(), frame));
    Passthrough(controller.Step(Enabled(), frame));
    frame.updateId = 10;
    Passthrough(controller.Step(Enabled(), frame));
    frame.updateId = 12;
    Down(controller.Step(Enabled(), frame));
}

void SingleAcknowledgementAndRearm() {
    currentTest = "single acknowledgement and rearm";
    Controller controller;
    Frame frame = Ready(FireMode::Single);
    frame.shotReady = false;
    Passthrough(controller.Step(Enabled(), frame));
    frame.shotReady = true;
    ++frame.updateId;
    Down(controller.Step(Enabled(), frame));
    for (int i = 0; i < 100; ++i) {
        ++frame.updateId;
        frame.shotReady = (i % 2) == 0;
        Hold(controller.Step(Enabled(), frame));
    }
    ++frame.shotSerial;
    ++frame.updateId;
    Up(controller.Step(Enabled(), frame));
    frame.shotReady = false;
    ++frame.updateId;
    Passthrough(controller.Step(Enabled(), frame));
    frame.shotReady = true;
    ++frame.updateId;
    Down(controller.Step(Enabled(), frame));
    frame.targetValid = false;
    ++frame.shotSerial;
    ++frame.updateId;
    Up(controller.Step(Enabled(), frame));
    frame.targetValid = true;
    ++frame.updateId;
    Down(controller.Step(Enabled(), frame));
    ++frame.updateId;
    Hold(controller.Step(Enabled(), frame));
}

void RepeatedSinglesAtNativeCadence() {
    currentTest = "repeated singles at native cadence";
    Controller controller;
    Frame frame = Ready(FireMode::Single);
    std::uint64_t acceptedShots = 0;
    std::uint64_t lastPressUpdate = 0;
    std::uint64_t lastReleaseUpdate = 0;
    for (std::uint64_t tick = 1; tick <= 120; ++tick) {
        frame.updateId = tick;
        frame.shotSerial = acceptedShots;
        frame.shotReady = (tick % 3) == 1;
        const Intent intent = controller.Step(Enabled(), frame);
        if (intent.down) {
            CHECK(frame.shotReady);
            CHECK(lastPressUpdate == 0 || lastReleaseUpdate > lastPressUpdate);
            CHECK(lastReleaseUpdate < tick);
            ++acceptedShots; // Simulated native acceptance, seen next update.
            lastPressUpdate = tick;
        }
        if (intent.up) {
            CHECK(lastPressUpdate != 0 && lastPressUpdate < tick);
            lastReleaseUpdate = tick;
        }
    }
    CHECK(acceptedShots == 40);
}

void ModeChangesAndBurstAdmission() {
    currentTest = "mode changes and validated burst admission";
    Controller controller;
    Frame frame = Ready();
    Down(controller.Step(Enabled(), frame));
    frame.fireMode = FireMode::Single;
    ++frame.updateId;
    Up(controller.Step(Enabled(), frame));
    ++frame.updateId;
    Down(controller.Step(Enabled(), frame));
    Settings ads{true, Mode::ADSOnly, false};
    frame.aiming = true;
    frame.fullyAimed = true;
    ++frame.updateId;
    Up(controller.Step(ads, frame));
    ++frame.updateId;
    Down(controller.Step(ads, frame));

    for (FireMode mode : {FireMode::BurstAutomatic, FireMode::BurstSingle}) {
        controller.Reset();
        frame = Ready(mode);
        Down(controller.Step(Enabled(), frame));
        frame.targetValid = false;
        ++frame.updateId;
        Up(controller.Step(Enabled(), frame));
        ++frame.updateId;
        Passthrough(controller.Step(Enabled(), frame));
    }
    controller.Reset();
    frame = Ready(static_cast<FireMode>(255));
    Passthrough(controller.Step(Enabled(), frame));
}

void EligibilityMatrixAndIntentInvariants() {
    currentTest = "eligibility matrix and intent invariants";
    // Exercise combinations rather than isolated if-branches: no invalid
    // current frame may acquire input, even when its other fields are true.
    for (unsigned mask = 0; mask < 256; ++mask) {
        Controller controller;
        Frame frame = Ready();
        Settings settings{(mask & 1) != 0, Mode::ADSOnly, (mask & 128) != 0};
        frame.contextValid = (mask & 2) != 0;
        frame.bindingsReady = (mask & 4) != 0;
        frame.ammoAvailable = (mask & 8) != 0;
        frame.targetValid = (mask & 16) != 0;
        frame.aiming = (mask & 32) != 0;
        frame.fullyAimed = (mask & 64) != 0;
        const Intent intent = controller.Step(settings, frame);
        if ((mask & 127) == 127) {
            Down(intent);
        } else {
            Passthrough(intent);
        }
        CHECK(!(intent.down && intent.up));
        CHECK(!intent.down || intent.held);
        CHECK(!intent.up || !intent.held);
    }
}

} // namespace

static_assert(std::is_trivially_copyable<Controller>::value,
              "The controller must remain allocation-free value state.");
static_assert(noexcept(Controller{}.Step(Settings{}, Frame{})),
              "The gameplay hot path must not throw.");

int main() {
    DefaultAndAutomaticCadence();
    EachHardGate();
    ADSAndDotIndependence();
    ManualInputOwnsEveryTransition();
    GenerationsAndReset();
    DuplicateAndOutOfOrderUpdates();
    SingleAcknowledgementAndRearm();
    RepeatedSinglesAtNativeCadence();
    ModeChangesAndBurstAdmission();
    EligibilityMatrixAndIntentInvariants();
    std::puts("Trigger core tests passed.");
}
