#pragma once

#include "TriggerRuntime.h"
#include "../ImGui/imgui.h"
#include <cmath>

namespace PhantomTrigger {

// Draw once per render frame. The runtime publishes the game viewport center;
// this renderer never calls Unity or reads native game objects.
inline void DrawTriggerDot(ImDrawList* draw,float width,float height) {
    if(!draw || !GetSettings().showDot || !std::isfinite(width) ||
       !std::isfinite(height) || width<=0.f || height<=0.f)
        return;

    float normalizedX=.5f,normalizedY=.5f;
    GetViewportCenter(normalizedX,normalizedY);
    if(!std::isfinite(normalizedX) || !std::isfinite(normalizedY) ||
       normalizedX<0.f || normalizedX>1.f || normalizedY<0.f || normalizedY>1.f) {
        normalizedX=.5f;
        normalizedY=.5f;
    }

    const ImVec2 center(normalizedX*width,normalizedY*height);
    draw->AddCircleFilled(center,4.f,IM_COL32(0,0,0,255),16);
    draw->AddCircleFilled(center,3.f,IM_COL32(255,255,255,255),16);
}

} // namespace PhantomTrigger
