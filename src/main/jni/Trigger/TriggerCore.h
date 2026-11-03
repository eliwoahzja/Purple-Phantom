#pragma once

#include <cstdint>

namespace PhantomTrigger {

enum class Mode : std::uint8_t { Both, ADSOnly };

// Burst values are usable only after the native adapter verifies cancellation.
enum class FireMode : std::uint8_t {
    Unsupported,
    Automatic,
    Single,
    BurstAutomatic,
    BurstSingle
};

struct Settings {
    bool enabled = false;
    Mode mode = Mode::Both;
    bool showDot = false;
};

// These are adapter-issued generation tokens, never managed-object addresses.
struct ContextIdentity {
    std::uint64_t match = 0;
    std::uint64_t pawn = 0;
    std::uint64_t weapon = 0;
    std::uint64_t component = 0;
    std::uint64_t inputMode = 0;

    constexpr bool operator==(const ContextIdentity& other) const noexcept {
        return match == other.match && pawn == other.pawn &&
               weapon == other.weapon && component == other.component &&
               inputMode == other.inputMode;
    }

    constexpr bool operator!=(const ContextIdentity& other) const noexcept {
        return !(*this == other);
    }
};

struct Frame {
    ContextIdentity identity{};
    std::uint64_t updateId = 0;
    bool contextValid = false;
    bool bindingsReady = false;
    // True for any physical press/down/up: native input owns this update.
    bool manualActive = false;
    bool aiming = false;
    bool fullyAimed = false;
    bool ammoAvailable = false;
    bool hardBlocked = false;
    // Validated this update against body geometry, world occlusion, and smoke.
    bool targetValid = false;
    bool shotReady = false;
    std::uint64_t shotSerial = 0;
    FireMode fireMode = FireMode::Unsupported;
};

struct Intent {
    bool held = false;
    bool down = false;
    bool up = false;
    // True means leave the original native input completely untouched.
    bool passthrough = true;
};

// One controller per local input stream. Step runs once before native input
// consumption; shotSerial acknowledges an actual accepted shot from that stream.
class Controller {
public:
    bool HasOwnedInput() const noexcept { return phase_ == Phase::Held; }
    void Reset() noexcept {
        *this = Controller{};
    }

    Intent Step(const Settings& settings, const Frame& frame) noexcept {
        if (!hasContext_ || identity_ != frame.identity) {
            // Never send the old owner's release into a different generation.
            Reset();
            hasContext_ = true;
            identity_ = frame.identity;
            mode_ = settings.mode;
            fireMode_ = frame.fireMode;
        } else if (hasUpdate_ && frame.updateId <= lastUpdate_) {
            // Repeated callbacks may not consume an edge or advance rearming.
            return {};
        }

        hasUpdate_ = true;
        lastUpdate_ = frame.updateId;

        const bool modeChanged = mode_ != settings.mode ||
                                 fireMode_ != frame.fireMode;
        mode_ = settings.mode;
        fireMode_ = frame.fireMode;

        if (frame.manualActive) {
            // A synthetic up here could cancel the user's physical fire.
            phase_ = Phase::Idle;
            return {};
        }

        if (modeChanged || !Eligible(settings, frame)) {
            return ReleaseOwned();
        }

        if (IsAutomatic(frame.fireMode)) {
            const bool wasHeld = IsOwned();
            phase_ = Phase::Held;
            // Native cadence can temporarily reject a shot without dropping
            // sustained input; it remains responsible for accepting shots.
            return {true, !wasHeld, false, false};
        }

        if (phase_ == Phase::Held) {
            if (frame.shotSerial == submittedShotSerial_) {
                return {true, false, false, false};
            }
            phase_ = Phase::Rearm;
            return {false, false, true, false};
        }

        // A preceding release was consumed by a distinct native update before
        // this call. Cadence readiness is only needed for a fresh single tap.
        if (phase_ == Phase::Rearm) {
            phase_ = Phase::Idle;
        }
        if (!frame.shotReady) {
            return {};
        }

        submittedShotSerial_ = frame.shotSerial;
        phase_ = Phase::Held;
        return {true, true, false, false};
    }

private:
    enum class Phase : std::uint8_t { Idle, Held, Rearm };

    static bool IsAutomatic(FireMode mode) noexcept {
        return mode == FireMode::Automatic || mode == FireMode::BurstAutomatic;
    }

    static bool IsSupported(FireMode mode) noexcept {
        return IsAutomatic(mode) || mode == FireMode::Single ||
               mode == FireMode::BurstSingle;
    }

    static bool Eligible(const Settings& settings, const Frame& frame) noexcept {
        const bool validMode = settings.mode == Mode::Both ||
                               settings.mode == Mode::ADSOnly;
        const bool adsReady = settings.mode == Mode::Both ||
                              (frame.aiming && frame.fullyAimed);
        return settings.enabled && validMode && frame.contextValid &&
               frame.bindingsReady && frame.ammoAvailable && !frame.hardBlocked &&
               frame.targetValid && IsSupported(frame.fireMode) && adsReady;
    }

    bool IsOwned() const noexcept {
        return phase_ == Phase::Held;
    }

    Intent ReleaseOwned() noexcept {
        const bool wasHeld = IsOwned();
        phase_ = Phase::Idle;
        return wasHeld ? Intent{false, false, true, false} : Intent{};
    }

    ContextIdentity identity_{};
    std::uint64_t lastUpdate_ = 0;
    std::uint64_t submittedShotSerial_ = 0;
    Mode mode_ = Mode::Both;
    FireMode fireMode_ = FireMode::Unsupported;
    Phase phase_ = Phase::Idle;
    bool hasContext_ = false;
    bool hasUpdate_ = false;
};

} // namespace PhantomTrigger
