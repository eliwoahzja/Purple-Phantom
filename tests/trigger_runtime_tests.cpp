#include "Trigger/TriggerRuntime.h"
#include "Trigger/TriggerDump1657.h"
#include <cassert>
#include <cstdio>
#include <limits>
#include <initializer_list>

using namespace PhantomTrigger;

struct Queries final : TargetQueries {
    BodyHit body{true, true, true, true, true, 7, {0, 0, 100}, 100};
    bool available = true;
    Occlusion world = Occlusion::Clear, smoke = Occlusion::Clear;
    int traces = 0, worlds = 0, smokes = 0;
    bool TraceBody(const ContextIdentity&, const ViewRay&, BodyHit& hit) noexcept override {
        ++traces; hit = body; return available;
    }
    Occlusion CheckWorld(const ContextIdentity&, const ViewRay&, const BodyHit&) noexcept override {
        ++worlds; return world;
    }
    Occlusion CheckSmoke(const ContextIdentity&, const ViewRay&, const BodyHit&) noexcept override {
        ++smokes; return smoke;
    }
};

int main() {
    const ViewRay ray{{0, 0, 0}, {0, 0, 1}, 500};
    const ContextIdentity identity{1, 2, 3, 4, 5};
    Queries q;
    assert(EvaluateTarget(q, identity, ray) == TargetResult::Eligible);
    assert(q.traces == 1 && q.worlds == 1 && q.smokes == 1);

    for (auto member : {&BodyHit::hit, &BodyHit::enemy, &BodyHit::alive,
                        &BodyHit::detailed, &BodyHit::nearest}) {
        Queries miss;
        miss.body.*member = false;
        assert(EvaluateTarget(miss, identity, ray) == TargetResult::NoEligibleBody);
        assert(miss.worlds == 0 && miss.smokes == 0);
    }
    q.body.point.x = .1f; // Ten centimeters beside the exact ray is a miss.
    assert(EvaluateTarget(q, identity, ray) == TargetResult::InvalidHit);
    q.body.point = {0, 0, 100};
    q.body.distance = 200; // Stale hit metadata must not qualify.
    assert(EvaluateTarget(q, identity, ray) == TargetResult::InvalidHit);
    q.body.distance = 100;
    q.body.targetGeneration = 0;
    assert(EvaluateTarget(q, identity, ray) == TargetResult::NoEligibleBody);
    q = Queries{};
    q.available = false;
    assert(EvaluateTarget(q, identity, ray) == TargetResult::QueryUnavailable);
    for (auto state : {Occlusion::Blocked, Occlusion::Unavailable}) {
        q = Queries{};
        q.world = state;
        assert(EvaluateTarget(q, identity, ray) == (state == Occlusion::Blocked
            ? TargetResult::WorldBlocked : TargetResult::QueryUnavailable));
        assert(q.smokes == 0);
        q = Queries{};
        q.smoke = state;
        assert(EvaluateTarget(q, identity, ray) == (state == Occlusion::Blocked
            ? TargetResult::SmokeBlocked : TargetResult::QueryUnavailable));
    }
    for (float invalid : {0.f, -1.f, std::numeric_limits<float>::infinity(),
                          std::numeric_limits<float>::quiet_NaN()}) {
        auto invalidRay = ray;
        invalidRay.maxDistance = invalid;
        q = Queries{};
        assert(EvaluateTarget(q, identity, invalidRay) == TargetResult::InvalidRay);
        assert(q.traces == 0);
    }
    auto invalidRay = ray;
    invalidRay.direction = {0, 0, 0};
    assert(EvaluateTarget(q, identity, invalidRay) == TargetResult::InvalidRay);

    Settings settings{true, Mode::Both, false};
    Frame frame;
    frame.identity = identity;
    frame.updateId = 1;
    frame.contextValid = frame.bindingsReady = frame.ammoAvailable = frame.shotReady = true;
    frame.fireMode = FireMode::Automatic;
    Controller controller;
    q = Queries{};
    assert(EvaluateInput(settings, frame, ray, q, controller).down);
    ++frame.updateId;
    q.smoke = Occlusion::Blocked;
    frame.targetValid = true; // Caller cannot override the new smoke result.
    auto release = EvaluateInput(settings, frame, ray, q, controller);
    assert(release.up && !release.held);
    ++frame.updateId;
    assert(EvaluateInput(settings, frame, ray, q, controller).passthrough);
    ++frame.updateId;
    q.smoke = Occlusion::Clear;
    assert(EvaluateInput(settings, frame, ray, q, controller).down);
    ++frame.updateId;
    q = Queries{};
    frame.manualActive = true;
    auto native = EvaluateInput(settings, frame, ray, q, controller);
    assert(native.passthrough && !native.up && q.traces == 0);
    frame.manualActive = false;
    for (auto member : {&Frame::contextValid, &Frame::bindingsReady, &Frame::ammoAvailable}) {
        ++frame.updateId;
        frame.*member = false;
        q = Queries{};
        assert(EvaluateInput(settings, frame, ray, q, controller).passthrough);
        assert(q.traces == 0);
        frame.*member = true;
    }
    settings.mode = Mode::ADSOnly;
    ++frame.updateId;
    q = Queries{};
    assert(EvaluateInput(settings, frame, ray, q, controller).passthrough);
    assert(q.traces == 0);
    ++frame.updateId;
    frame.aiming = frame.fullyAimed = true;
    assert(EvaluateInput(settings, frame, ray, q, controller).down);

    SetSettings({true, Mode::ADSOnly, true});
    const auto read = GetSettings();
    assert(read.enabled && read.mode == Mode::ADSOnly && read.showDot);
    assert(ApprovedBinaryProfileCount == 0);
    assert(GetRuntimeStatus() == RuntimeStatus::WaitingForBinaryVerification);
    assert(GetDetectedFireMode() == FireMode::Unsupported);
    float x = 0, y = 0;
    assert(!GetViewportCenter(x, y) && x == .5f && y == .5f);
    SetSettings({true, static_cast<Mode>(255), true});
    assert(!GetSettings().enabled); // Invalid settings cannot broaden ADS to Both.
    SetSettings({});
    std::puts("trigger runtime tests passed");
}
