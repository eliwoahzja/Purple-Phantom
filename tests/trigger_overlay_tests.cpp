#include "Trigger/TriggerOverlay.h"
#include <algorithm>
#include <cassert>
#include <cstdio>
#include <limits>

int main() {
    ImGui::CreateContext();
    auto& io = ImGui::GetIO();
    io.IniFilename = nullptr;
    io.DisplaySize = ImVec2(1280, 720);
    unsigned char* pixels = nullptr;
    int width = 0, height = 0;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
    ImGui::NewFrame();
    auto* draw = ImGui::GetBackgroundDrawList();
    PhantomTrigger::SetSettings({});
    PhantomTrigger::DrawTriggerDot(draw, 1280, 720);
    assert(draw->VtxBuffer.empty());
    // The guide works independently of trigger activation or any game/ESP data.
    PhantomTrigger::SetSettings({false, PhantomTrigger::Mode::Both, true});
    PhantomTrigger::DrawTriggerDot(draw, 1280, 720);
    assert(!draw->VtxBuffer.empty());
    float minX = 1280, maxX = 0, minY = 720, maxY = 0;
    for (const auto& vertex : draw->VtxBuffer) {
        minX = std::min(minX, vertex.pos.x); maxX = std::max(maxX, vertex.pos.x);
        minY = std::min(minY, vertex.pos.y); maxY = std::max(maxY, vertex.pos.y);
    }
    assert(std::fabs((minX + maxX) / 2 - 640) < .01f);
    assert(std::fabs((minY + maxY) / 2 - 360) < .01f);
    assert(maxX - minX <= 10 && maxY - minY <= 10);
    const int count = draw->VtxBuffer.Size;
    PhantomTrigger::SetSettings({true, PhantomTrigger::Mode::ADSOnly, false});
    PhantomTrigger::DrawTriggerDot(draw, 1280, 720);
    assert(draw->VtxBuffer.Size == count);
    PhantomTrigger::SetSettings({true, PhantomTrigger::Mode::ADSOnly, true});
    PhantomTrigger::DrawTriggerDot(nullptr, 1280, 720);
    PhantomTrigger::DrawTriggerDot(draw, 0, 720);
    PhantomTrigger::DrawTriggerDot(draw, 1280, -1);
    PhantomTrigger::DrawTriggerDot(draw, std::numeric_limits<float>::quiet_NaN(), 720);
    assert(draw->VtxBuffer.Size == count);
    assert(PhantomTrigger::GetRuntimeStatus() ==
           PhantomTrigger::RuntimeStatus::WaitingForBinaryVerification);
    ImGui::EndFrame();
    ImGui::DestroyContext();
    PhantomTrigger::SetSettings({});
    std::puts("trigger overlay tests passed");
}
