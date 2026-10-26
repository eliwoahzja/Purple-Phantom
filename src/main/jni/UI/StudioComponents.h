#pragma once
#include "../ImGui/imgui.h"
#include "../ImGui/imgui_internal.h"
#include "StudioFonts.h"
#include "ControlDescriptions.h"
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <cmath>

namespace StudioUI {
static ImFont* BodyFont = nullptr;
static ImFont* TitleFont = nullptr;
static float Scale = 1.0f;
static float BottomInset = 0;
static ImVec2 UsableDisplaySize() {
    const ImVec2 size=ImGui::GetIO().DisplaySize;
    return ImVec2(size.x,std::max(0.f,size.y-std::max(0.f,BottomInset)));
}
static bool GridOpen = false;
static bool ExclusiveDrag = false;
static int CurrentIcon = 0;
static const char* PendingSection = nullptr;
static ImGuiID ScrollOwner = 0;
static ImVec2 ScrollStart, ScrollLast;
static bool ScrollDragging = false;
static float S(float n) { return n * Scale; }
static ImVec2 V(float x, float y) { return ImVec2(S(x), S(y)); }
static ImVec2 Add(ImVec2 a, ImVec2 b) { return ImVec2(a.x+b.x, a.y+b.y); }
static float Clamp(float v, float lo, float hi) { return std::max(lo, std::min(hi, v)); }
static ImU32 Ink() { return IM_COL32(248,250,255,255); }
static ImU32 Muted() { return IM_COL32(187,196,212,255); }
static ImU32 Accent(int alpha=255) {
    ImVec4 c=ImGui::GetStyleColorVec4(ImGuiCol_CheckMark);
    c.w=alpha/255.f;
    return ImGui::ColorConvertFloat4ToU32(c);
}
static void ResetGestures() {
    ScrollOwner=0; ScrollDragging=false; ExclusiveDrag=false;
}
static void Initialize(ImGuiIO& io) {
    ImFontConfig config;
    config.FontDataOwnedByAtlas=false;
    config.OversampleH=2; config.OversampleV=1;
    BodyFont=io.Fonts->AddFontFromMemoryTTF((void*)RegularFontData,sizeof(RegularFontData),22,&config);
    TitleFont=io.Fonts->AddFontFromMemoryTTF((void*)HeadingFontData,sizeof(HeadingFontData),30,&config);
    if(!BodyFont) BodyFont=io.Fonts->AddFontDefault();
    if(!TitleFont) TitleFont=BodyFont;
    io.FontDefault=BodyFont;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    GridOpen=false;
    ResetGestures();
}
struct Theme {
    ImGuiStyle previous;
    explicit Theme(const float* accent) : previous(ImGui::GetStyle()) {
        const ImVec2 display=ImGui::GetIO().DisplaySize;
        const bool portrait=display.y>display.x;
        Scale=Clamp(portrait?std::min(display.x/440.f,display.y/900.f):
                    std::min(display.x/1040.f,display.y/700.f),.65f,2.5f);
        ImGuiStyle style;
        ImGui::StyleColorsDark(&style);
        style.WindowPadding=ImVec2(18,18);
        style.FramePadding=ImVec2(12,9);
        style.ItemSpacing=ImVec2(12,10);
        style.ItemInnerSpacing=ImVec2(8,6);
        style.CellPadding=ImVec2(6,6);
        style.WindowRounding=32; style.ChildRounding=24;
        style.FrameRounding=16; style.PopupRounding=24;
        style.ScrollbarSize=5; style.ScrollbarRounding=8;
        style.GrabMinSize=12; style.GrabRounding=12;
        style.WindowBorderSize=0; style.ChildBorderSize=0;
        style.WindowMenuButtonPosition=ImGuiDir_None;
        auto* c=style.Colors;
        const ImVec4 a(Clamp(accent[0],0,1),Clamp(accent[1],0,1),Clamp(accent[2],0,1),1);
        c[ImGuiCol_Text]=ImVec4(.97f,.98f,1,1);
        c[ImGuiCol_TextDisabled]=ImVec4(.73f,.77f,.83f,1);
        c[ImGuiCol_WindowBg]=ImVec4(0,0,0,0);
        c[ImGuiCol_ChildBg]=ImVec4(0,0,0,0);
        c[ImGuiCol_PopupBg]=ImVec4(.075f,.025f,.12f,.99f);
        c[ImGuiCol_Border]=ImVec4(.82f,.34f,1,.48f);
        c[ImGuiCol_FrameBg]=ImVec4(.48f,.16f,.68f,.20f);
        c[ImGuiCol_FrameBgHovered]=ImVec4(.78f,.25f,.95f,.28f);
        c[ImGuiCol_FrameBgActive]=ImVec4(.84f,.25f,1,.38f);
        c[ImGuiCol_Button]=ImVec4(.55f,.12f,.76f,.22f);
        c[ImGuiCol_ButtonHovered]=ImVec4(.80f,.18f,.96f,.34f);
        c[ImGuiCol_ButtonActive]=ImVec4(a.x,a.y,a.z,.6f);
        c[ImGuiCol_Header]=ImVec4(.55f,.12f,.76f,.20f);
        c[ImGuiCol_HeaderHovered]=ImVec4(.80f,.18f,.96f,.32f);
        c[ImGuiCol_HeaderActive]=ImVec4(a.x,a.y,a.z,.4f);
        c[ImGuiCol_CheckMark]=a; c[ImGuiCol_SliderGrab]=a;
        c[ImGuiCol_SliderGrabActive]=a; c[ImGuiCol_NavHighlight]=ImVec4(.95f,.72f,1,1);
        c[ImGuiCol_TextSelectedBg]=ImVec4(a.x,a.y,a.z,.4f);
        c[ImGuiCol_ScrollbarBg]=ImVec4(0,0,0,0);
        c[ImGuiCol_ScrollbarGrab]=ImVec4(.78f,.42f,1,.42f);
        c[ImGuiCol_ScrollbarGrabHovered]=ImVec4(.86f,.42f,1,.68f);
        c[ImGuiCol_ScrollbarGrabActive]=ImVec4(.92f,.55f,1,.88f);
        style.ScaleAllSizes(Scale);
        ImGui::GetStyle()=style;
        ImGui::PushFont(BodyFont);
    }
    ~Theme() { ImGui::PopFont(); ImGui::GetStyle()=previous; }
};
static void Text(ImDrawList* d, ImVec2 p, float size, ImU32 color, const char* value,
                 float wrap=0, bool heading=false) {
    d->AddText(heading?TitleFont:BodyFont,S(size),p,color,value,nullptr,wrap);
}
static float TextHeight(const char* value,float size,float width,bool heading=false) {
    return (heading?TitleFont:BodyFont)->CalcTextSizeA(S(size),FLT_MAX,width,value).y;
}
// No scene readback, extra textures or offscreen framebuffer is needed for glass.
static void Glass(ImDrawList* d,ImVec2 p,ImVec2 size,float radius=26,bool active=false,bool hovered=false) {
    const ImVec2 end=Add(p,size);
    const float r=S(radius);
    d->AddRectFilled(Add(p,V(0,4)),Add(end,V(0,4)),IM_COL32(0,0,0,32),r);
    d->AddRectFilled(p,end,IM_COL32(63,25,91,hovered?142:112),r);
    if(active) d->AddRectFilled(p,end,Accent(108),r);
    // Fade the reflection across the rounded mesh, including antialiased edges.
    const int first=d->VtxBuffer.Size;
    d->AddRectFilled(p,end,IM_COL32(221,178,255,255),r);
    for(int i=first;i<d->VtxBuffer.Size;++i) {
        ImDrawVert& vertex=d->VtxBuffer[i];
        const float t=Clamp((vertex.pos.y-p.y)/std::max(1.f,size.y),0,1);
        const int edgeAlpha=(vertex.col>>IM_COL32_A_SHIFT)&255;
        const int alpha=(int)(edgeAlpha*(hovered?28.f:20.f)*(1-t)*(1-t)/255.f);
        vertex.col=(vertex.col&~IM_COL32_A_MASK)|((ImU32)alpha<<IM_COL32_A_SHIFT);
    }
    d->AddRect(p,end,IM_COL32(221,119,255,active?190:125),r,0,S(1));
    d->PathArcTo(ImVec2(p.x+r,p.y+r),r-S(1),3.14159265f,4.71238898f,12);
    d->PathLineTo(ImVec2(end.x-r,p.y+S(1)));
    d->PathStroke(IM_COL32(248,218,255,active?190:125),0,S(1));
}
static bool Tap(const char* id,ImVec2 size) {
    ImGui::PushStyleColor(ImGuiCol_HeaderHovered,ImVec4(0,0,0,0));
    ImGui::PushStyleColor(ImGuiCol_HeaderActive,ImVec4(0,0,0,0));
    const bool pressed=ImGui::Selectable(id,false,ImGuiSelectableFlags_DontClosePopups,size);
    ImGui::PopStyleColor(2);
    // A release after dragging a tile is a scroll, never a toggle.
    return pressed && !(ImGui::IsMouseReleased(0) &&
        ImGui::GetIO().MouseDragMaxDistanceSqr[0]>S(8)*S(8));
}
static bool Button(const char* label,ImVec2 size=ImVec2(0,0),bool primary=false) {
    if(primary) ImGui::PushStyleColor(ImGuiCol_Button,ImGui::GetStyleColorVec4(ImGuiCol_CheckMark));
    const bool clicked=ImGui::Button(label,size);
    if(primary) ImGui::PopStyleColor();
    if(ImGui::IsItemHovered() && Description(label)[0]) ImGui::SetTooltip("%s",Description(label));
    return clicked && !(ImGui::IsMouseReleased(0) && ImGui::GetIO().MouseDragMaxDistanceSqr[0]>S(8)*S(8));
}
static void Icon(ImDrawList* d, ImVec2 p, float size, int kind, ImU32 color) {
    const float u=size/24.f;
    auto v=[&](float x,float y){return ImVec2(p.x+x*u,p.y+y*u);};
    const float line=std::max(1.f,2*u);
    if(kind==0) { // eye
        d->AddBezierCubic(v(2,12),v(7,3),v(17,3),v(22,12),color,line);
        d->AddBezierCubic(v(2,12),v(7,21),v(17,21),v(22,12),color,line);
        d->AddCircle(v(12,12),4*u,color,16,line);
    } else if(kind==1) { // movement
        d->AddCircleFilled(v(14,4),2*u,color,12);
        d->AddLine(v(13,8),v(10,14),color,line);
        d->AddLine(v(13,8),v(7,9),color,line);
        d->AddLine(v(13,8),v(18,13),color,line);
        d->AddLine(v(10,14),v(5,21),color,line);
        d->AddLine(v(10,14),v(16,16),color,line);
        d->AddLine(v(16,16),v(19,22),color,line);
    } else if(kind==2) { // crosshair
        d->AddCircle(v(12,12),7*u,color,20,line);
        d->AddCircleFilled(v(12,12),2*u,color,12);
        d->AddLine(v(12,1),v(12,6),color,line); d->AddLine(v(12,18),v(12,23),color,line);
        d->AddLine(v(1,12),v(6,12),color,line); d->AddLine(v(18,12),v(23,12),color,line);
    } else if(kind==3) { // stacked memory blocks
        for(int i=0;i<3;++i) {
            const float x=i==0?12.f:(i==1?6.f:18.f), y=i==0?3.f:12.f;
            ImVec2 points[]={v(x,y),v(x+5,y+3),v(x+5,y+9),v(x,y+12),v(x-5,y+9),v(x-5,y+3)};
            d->AddPolyline(points,6,color,ImDrawFlags_Closed,line);
            d->AddLine(v(x-5,y+3),v(x,y+6),color,line);
            d->AddLine(v(x+5,y+3),v(x,y+6),color,line);
            d->AddLine(v(x,y+6),v(x,y+12),color,line);
        }
    } else { // gear
        d->AddCircle(v(12,12),6*u,color,16,line*1.6f);
        d->AddCircle(v(12,12),2*u,color,12,line);
        for(int i=0;i<8;++i) {
            const float a=i*3.14159265f/4;
            d->AddLine(v(12+7*cosf(a),12+7*sinf(a)),v(12+10*cosf(a),12+10*sinf(a)),color,line*1.8f);
        }
    }
}
static void EndGrid() {
    if(GridOpen) { ImGui::EndTable(); GridOpen=false; }
}
static void NextCell(bool fullWidth=false) {
    if(!GridOpen) {
        const int columns=!fullWidth && ImGui::GetContentRegionAvail().x>=S(570)?2:1;
        GridOpen=ImGui::BeginTable("##glass_grid",columns,ImGuiTableFlags_SizingStretchSame);
    }
    if(GridOpen) ImGui::TableNextColumn();
}
static void Section(const char* title) {
    EndGrid();
    ImGui::PushID(title);
    const ImVec2 p=ImGui::GetCursorScreenPos();
    Text(ImGui::GetWindowDrawList(),Add(p,V(6,12)),12,Muted(),title);
    ImGui::Dummy(V(1,38));
    if(PendingSection && std::strcmp(PendingSection,title)==0) {
        ImGui::SetScrollHereY(0); PendingSection=nullptr;
    }
    ImGui::PopID();
}
static bool Toggle(const char* label,bool* value) {
    const bool master=std::strcmp(label,"Enable ESP")==0 || std::strcmp(label,"Aimlock")==0;
    if(master) EndGrid();
    NextCell(master); ImGui::PushID(label);
    const ImVec2 p=ImGui::GetCursorScreenPos();
    const float width=ImGui::GetContentRegionAvail().x;
    const float textWidth=width-S(master?148:30);
    const float labelHeight=TextHeight(label,17,textWidth,true);
    const float descriptionHeight=TextHeight(Description(label),13,textWidth);
    const float height=std::max(S(master?90:120),S(master?36:64)+labelHeight+descriptionHeight);
    const bool pressed=Tap("##toggle",ImVec2(width,height));
    if(pressed) *value=!*value;
    auto* d=ImGui::GetWindowDrawList();
    Glass(d,p,ImVec2(width,height),26,*value,ImGui::IsItemHovered());
    if(ImGui::IsItemFocused()) d->AddRect(p,Add(p,ImVec2(width,height)),Ink(),S(26),0,S(2));
    d->AddCircleFilled(Add(p,V(30,29)),S(17),*value?Accent():IM_COL32(215,225,241,33),28);
    Icon(d,Add(p,V(19,18)),S(22),CurrentIcon,Ink());
    const ImVec2 track(p.x+width-S(60),p.y+S(17));
    const ImGuiID animation=ImGui::GetID("##knob");
    auto* storage=ImGui::GetStateStorage();
    float t=storage->GetFloat(animation,*value?1.f:0.f);
    t += ((*value?1.f:0.f)-t)*std::min(1.f,ImGui::GetIO().DeltaTime*18.f);
    storage->SetFloat(animation,t);
    d->AddRectFilled(track,Add(track,V(44,25)),*value?Accent():IM_COL32(255,255,255,44),S(13));
    d->AddCircleFilled(Add(track,V(12.5f+19*t,12.5f)),S(10),Ink(),24);
    Text(d,Add(p,V(master?63:15,master?18:52)),17,Ink(),label,textWidth,true);
    Text(d,ImVec2(p.x+S(master?63:15),p.y+S(master?24:57)+labelHeight),13,Muted(),Description(label),textWidth);
    if(ImGui::IsItemHovered()) ImGui::SetTooltip("%s\n%s",label,Description(label));
    ImGui::PopID();
    if(master) EndGrid();
    return pressed;
}
static void Sun(ImDrawList* d,ImVec2 center,float radius,ImU32 color) {
    d->AddCircleFilled(center,radius*.42f,color,24);
    for(int i=0;i<8;++i) {
        const float a=i*3.14159265f/4;
        d->AddLine(ImVec2(center.x+std::cos(a)*radius*.69f,center.y+std::sin(a)*radius*.69f),
                   ImVec2(center.x+std::cos(a)*radius,center.y+std::sin(a)*radius),color,S(2));
    }
}
static bool Slider(const char* label,float* value,float low,float high,const char* format="%.1f") {
    NextCell(); ImGui::PushID(label);
    const ImVec2 p=ImGui::GetCursorScreenPos();
    const float width=ImGui::GetContentRegionAvail().x;
    const float textWidth=std::max(S(40),width-S(125));
    const float labelHeight=TextHeight(label,17,textWidth,true);
    const float descHeight=TextHeight(Description(label),13,textWidth);
    const float height=std::max(S(206),S(144)+labelHeight+descHeight);
    auto* d=ImGui::GetWindowDrawList();
    Glass(d,p,ImVec2(width,height),28);
    const ImVec2 pill=Add(p,V(14,15));
    const ImVec2 pillSize(S(78),height-S(30));
    Glass(d,pill,pillSize,39);
    ImGui::SetCursorScreenPos(pill);
    const ImVec4 clear(0,0,0,0);
    ImGui::PushStyleColor(ImGuiCol_FrameBg,clear);
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered,clear);
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive,clear);
    ImGui::PushStyleColor(ImGuiCol_SliderGrab,clear);
    ImGui::PushStyleColor(ImGuiCol_SliderGrabActive,clear);
    ImGui::PushStyleColor(ImGuiCol_Text,clear);
    ImGui::PushStyleVar(ImGuiStyleVar_GrabMinSize,0.f);
    const bool changed=ImGui::VSliderFloat("##value",pillSize,value,low,high,format,ImGuiSliderFlags_AlwaysClamp);
    ImGui::PopStyleVar(); ImGui::PopStyleColor(6);
    ExclusiveDrag |= ImGui::IsItemActive();
    const float fraction=high>low && std::isfinite(*value)?Clamp((*value-low)/(high-low),0,1):0;
    const float fillY=pill.y+pillSize.y*(1-fraction);
    if(fraction>0) {
        d->PushClipRect(ImVec2(pill.x,fillY),Add(pill,pillSize),true);
        d->AddRectFilled(pill,Add(pill,pillSize),IM_COL32(248,249,253,255),S(39));
        d->PopClipRect();
    }
    Sun(d,ImVec2(pill.x+pillSize.x*.5f,pill.y+pillSize.y-S(36)),S(16),
        fraction>36.f*Scale/pillSize.y?IM_COL32(215,165,53,255):Ink());
    if(ImGui::IsItemActive() || ImGui::IsItemFocused())
        d->AddRect(pill,Add(pill,pillSize),Ink(),S(39),0,S(2));
    char number[64], range[80];
    std::snprintf(number,sizeof(number),format,*value);
    std::snprintf(range,sizeof(range),"%g - %g",low,high);
    const float x=p.x+S(110);
    Text(d,ImVec2(x,p.y+S(20)),17,Ink(),label,textWidth,true);
    Text(d,ImVec2(x,p.y+S(30)+labelHeight),30,Ink(),number,textWidth,true);
    Text(d,ImVec2(x,p.y+S(68)+labelHeight),12,Muted(),range,textWidth);
    Text(d,ImVec2(x,p.y+S(91)+labelHeight),13,Muted(),Description(label),textWidth);
    Text(d,ImVec2(x,p.y+height-S(44)),12,Muted(),"Swipe up / down\nto adjust",textWidth);
    ImGui::SetCursorScreenPos(p); ImGui::Dummy(ImVec2(width,height));
    ImGui::PopID(); return changed;
}
template<class Enum> static void Choice(const char* label,Enum& value,const char* const* items,int count) {
    NextCell(); ImGui::PushID(label);
    const ImVec2 p=ImGui::GetCursorScreenPos();
    const float width=ImGui::GetContentRegionAvail().x;
    const float descriptionHeight=TextHeight(Description(label),13,width-S(30));
    const float height=S(96)+descriptionHeight;
    Glass(ImGui::GetWindowDrawList(),p,ImVec2(width,height),26);
    Text(ImGui::GetWindowDrawList(),Add(p,V(15,15)),17,Ink(),label,width-S(30),true);
    Text(ImGui::GetWindowDrawList(),Add(p,V(15,41)),13,Muted(),Description(label),width-S(30));
    ImGui::SetCursorScreenPos(ImVec2(p.x+S(15),p.y+S(49)+descriptionHeight));
    ImGui::SetNextItemWidth(width-S(30));
    int selected=static_cast<int>(value);
    if(ImGui::Combo("##value",&selected,items,count)) value=static_cast<Enum>(selected);
    ImGui::SetCursorScreenPos(p); ImGui::Dummy(ImVec2(width,height));
    ImGui::PopID();
}
static void Color(const char* label,float* value) {
    NextCell(); ImGui::PushID(label);
    const ImVec2 p=ImGui::GetCursorScreenPos();
    const float width=ImGui::GetContentRegionAvail().x;
    const float height=std::max(S(92),S(51)+TextHeight(Description(label),13,width-S(95)));
    Glass(ImGui::GetWindowDrawList(),p,ImVec2(width,height),26);
    Text(ImGui::GetWindowDrawList(),Add(p,V(15,18)),16,Ink(),label,width-S(95),true);
    Text(ImGui::GetWindowDrawList(),Add(p,V(15,44)),13,Muted(),Description(label),width-S(95));
    ImGui::SetCursorScreenPos(ImVec2(p.x+width-S(62),p.y+S(21)));
    float rgb[3]={0,0,0};
    if(value) {
        const bool byteRange=value[0]>1 || value[1]>1 || value[2]>1;
        for(int i=0;i<3;++i) rgb[i]=Clamp(value[i]/(byteRange?255.f:1.f),0,1);
    }
    ImGui::BeginDisabled(value==nullptr);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding,V(10,10));
    if(ImGui::ColorEdit3("##value",rgb,ImGuiColorEditFlags_NoInputs|ImGuiColorEditFlags_NoLabel) && value)
        for(int i=0;i<3;++i) value[i]=rgb[i];
    ImGui::PopStyleVar(); ImGui::EndDisabled();
    ImGui::SetCursorScreenPos(p); ImGui::Dummy(ImVec2(width,height));
    ImGui::PopID();
}
static void Heading(const char* title,const char* subtitle) {
    const ImVec2 p=ImGui::GetCursorScreenPos();
    const float width=ImGui::GetContentRegionAvail().x;
    const float titleHeight=TextHeight(title,28,width,true);
    const float subHeight=TextHeight(subtitle,14,width);
    Text(ImGui::GetWindowDrawList(),p,28,Ink(),title,width,true);
    Text(ImGui::GetWindowDrawList(),ImVec2(p.x,p.y+titleHeight+S(8)),14,Muted(),subtitle,width);
    ImGui::Dummy(ImVec2(width,titleHeight+subHeight+S(18)));
}
// Run after widgets, so a pill slider or popup can claim its own gesture.
static void ContentScroll() {
    auto* window=ImGui::GetCurrentWindow();
    auto& io=ImGui::GetIO();
    if(ImGui::IsMouseClicked(0) && ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem) && !ExclusiveDrag) {
        ScrollOwner=window->ID; ScrollStart=ScrollLast=io.MousePos; ScrollDragging=false;
    }
    if(ScrollOwner==window->ID) {
        if(ExclusiveDrag || GImGui->ActiveId==ImGui::GetWindowScrollbarID(window,ImGuiAxis_Y)) {
            ScrollOwner=0; ScrollDragging=false;
        } else if(ImGui::IsMouseDown(0)) {
            if(std::fabs(io.MousePos.y-ScrollStart.y)>S(8)) ScrollDragging=true;
            if(ScrollDragging) {
                ImGui::ClearActiveID();
                const float scroll=window->ScrollTarget.y<FLT_MAX?window->ScrollTarget.y:window->Scroll.y;
                ImGui::SetScrollY(window,Clamp(scroll+ScrollLast.y-io.MousePos.y,0,window->ScrollMax.y));
            }
            ScrollLast=io.MousePos;
        } else { ScrollOwner=0; ScrollDragging=false; }
    }
}
} // namespace StudioUI
