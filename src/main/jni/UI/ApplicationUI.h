#pragma once
#include "StudioComponents.h"
#include <string>

// UI only: the caller continues to own all values and feature callbacks.
namespace StudioUI {
static int Page=0;
static std::string AuthError;
static bool NavigationOpen=false;
static float NavigationProgress=0;
static const char* Pages[]={"Visuals","Movement","Aim","Memory","System","SkinHax"};
static const char* PageDetails[]={"Player overlays and visibility", "Movement and travel controls",
    "Assistance and targeting", "Weapon handling and items", "Performance and appearance",
    "Weapon skins and Mythic attachments"};
static const char* Sections[][4]={
    {"OVERLAY","ADDITIONAL VISIBILITY","OVERLAY COLORS",nullptr},
    {"SNOWBOARD","SLIDE","SPEED","JUMP"},
    {"ASSISTANCE","ADDITIONAL CONTROLS",nullptr,nullptr},
    {"WEAPON HANDLING","MOVEMENT AND ITEMS",nullptr,nullptr},
    {"PERFORMANCE","WORKSPACE","RENDERING",nullptr},
    {"WEAPON SKINS",nullptr,nullptr,nullptr}
};
static void Visuals() {
    Section("OVERLAY");
    Toggle("Enable ESP", &Config.ESPMenu.Esp);
    Toggle("Player lines", &Config.ESPMenu.isPlayerLine);
    Toggle("Player boxes", &Config.ESPMenu.Box);
    Toggle("Health", &Config.ESPMenu.Health);
    Toggle("Names", &Config.ESPMenu.Name);
    Toggle("Distance", &Config.ESPMenu.Distance);
    Toggle("Off-screen alerts", &Config.ESPMenu.Alert);
    Toggle("Vehicles", &Config.ESPMenu.Vehicles);
    static const char* types[]={"Old type","Classic","Modern"};
    Choice("ESP presentation",Config.WIDGETS_EXPERT.EUI,types,3);
    Section("ADDITIONAL VISIBILITY");
    Toggle("Spectate without delay",&Config.ExtraMenu.Spectate);
    Toggle("Wallhack glow",&checkboxStates["wallhack"]);
    Toggle("Multiplayer radar",&Config.ExtraMenu.Radar1);
    Toggle("Royale tags",&checkboxStates["BRTAGS"]);
    Section("OVERLAY COLORS");
    if(ImGui::CollapsingHeader("Player colors")) {
        Color("Player box",Config.sColorsESPPLAYER.BoxPLAYER);
        Color("Player line",Config.sColorsESPPLAYER.LinePLAYER);
        Color("Player name",Config.sColorsESPPLAYER.NamePLAYER);
        Color("Player health",Config.sColorsESPPLAYER.HealthPLAYER);
        Color("Player distance",Config.sColorsESPPLAYER.DistancePLAYER);
        EndGrid();
    }
    if(ImGui::CollapsingHeader("Bot colors")) {
        Color("Bot box",Config.sColorsESPBOT.BoxBOT);
        Color("Bot line",Config.sColorsESPBOT.LineBOT);
        Color("Bot name",Config.sColorsESPBOT.NameBOT);
        Color("Bot health",Config.sColorsESPBOT.HealthBOT);
        Color("Bot distance",Config.sColorsESPBOT.DistanceBOT);
        EndGrid();
    }
}
static void Movement() {
    Section("SNOWBOARD");
    if(Toggle("Snowboard boost",&SnowB)) SnowBshow=SnowB;
    if(SnowBshow) Slider("Boost",&SnowBsize,0,100);
    Section("SLIDE");
    if(Toggle("Long slide",&SlideTackle)) SlideShow=SlideTackle;
    if(SlideShow) Slider("Slide range",&SlideRange,0,30);
    Section("SPEED");
if (Toggle("Speed hack", &isSpeedHackEnabled))
    showSpeedHackSlider = isSpeedHackEnabled;
if (showSpeedHackSlider)
    Slider("Speed multiplier", &Config.ExtraMenu.SpeedhackValue, speed_min, speed_max, "%.1fx");
    Section("JUMP");
    if(Toggle("High jump",&isJumpAdjustmentEnabled) && !isJumpAdjustmentEnabled) jumpHeightMultiplier=1;
    if(isJumpAdjustmentEnabled) Slider("Jump multiplier",&jumpHeightMultiplier,.5f,5,"%.1fx");
}
static void AimControls() {
    Section("ASSISTANCE");
    if(Toggle("Aimlock",&Config.Aim.AimAssist)) Config.Aim.showAimAssistSlider=Config.Aim.AimAssist;
    Slider("Smoothness",&Config.Aim.AimAssistSize,0,100,"%.0f");
    Slider("Field of view",&Config.Aim.size,0,500,"%.0f");
    static const char* targets[]={"Head","Chest"};
    // The backend defines exactly these three activation values.
    static const char* triggers[]={"Always","Fire","Scope"};
    Choice("Target",Config.Aim.Target,targets,2);
    Choice("Activation",Config.Aim.Trigger,triggers,3);
    Section("ADDITIONAL CONTROLS");
    // These existing keys intentionally retain the original label-to-state mapping.
    Toggle("Aimbot 360",&Config.Aim.Aimbot360);
    Toggle("Bullet track",&Config.Aim.AimSilent);
    Toggle("Fast revival",&Config.ExtraMenu.Revive);
    Toggle("Fast medicine",&Config.ExtraMenu.Medicine);
}
static void MemoryControls() {
    Section("WEAPON HANDLING");
    Toggle("Weapon kinetic",&Config.ExtraMenu.Kinetic);
    Toggle("No recoil",&Config.ExtraMenu.Recoil);
    Toggle("No spread",&Config.ExtraMenu.Spread);
    Toggle("No reload",&Config.ExtraMenu.Reload);
    Toggle("No scope",&Config.ExtraMenu.Scope);
    Toggle("No switch",&Config.ExtraMenu.Switch);
    Toggle("No shake",&Config.ExtraMenu.Shake);
    Toggle("Unlimited bullets",&checkboxStates["AMMO"]);
    Toggle("No overheat",&Config.ExtraMenu.Rpd);
    Toggle("Long range",&Config.ExtraMenu.Range);
    Toggle("Instant shot",&Config.ExtraMenu.AutoSnipe);
    Toggle("Big hitbox",&Config.ExtraMenu.Hit);
    Toggle("HIDE RECORD",&HIDEESP);
    Toggle("Quick firing",&Config.ExtraMenu.Auto);
    Section("MOVEMENT AND ITEMS");
    Toggle("Jet boost + slide",&Config.ExtraMenu.JetS);
    Toggle("No slide fatigue",&Config.ExtraMenu.NoSlideFatigue);
    Toggle("Long execute",&isExecute);
    Toggle("No parachute",&Config.ExtraMenu.Parachute);
    Toggle("Sky diving speed",&Config.ExtraMenu.Diving);
    Toggle("Two Gulag tickets",&Config.ExtraMenu.TestGulag);
    Toggle("Magnet loot",&Config.ExtraMenu.MagnetLoot);
    Toggle("Fast loot",&Config.ExtraMenu.FastLoot);
    Toggle("Jet boost + jump",&Config.ExtraMenu.JetJ);
}
static void SystemControls() {
    Section("PERFORMANCE");
    Toggle("Blueprints",&Config.ExtraMenu.Blueprints);
    Toggle("Anti FPS drop",&checkboxStates["anti"]);
    Toggle("Maximum frame rate",&checkboxStates["FRAMERATE"]);

    Section("WORKSPACE");
    Toggle("Show floating control",&showFloatingIcon);
    Color("Interface accent",menu);

    Section("RENDERING");

    Toggle("Stretch Screen",&Config.ExtraMenu.StretchScreen);

if (Config.ExtraMenu.StretchScreen)
    ImGui::SliderFloat(
        "Stretch Aspect",
        &Config.ExtraMenu.StretchAspect,
        0.5f,
        3.0f,
        "%.2f"
    );

    const float fps=ImGui::GetIO().Framerate;
    ImGui::Text("%.0f FPS   /   %.2f ms per frame",
                fps,
                fps>0?1000.0f/fps:0);

    ImGui::TextWrapped(
        "The home bar always stays available. Swipe up or tap it to browse tabs and sections."
    );
}


static void SkinHaxControls() {
    // DrawPanel owns its search, rarity boxes, list and apply action. Keep it out
    // of the generic two-column settings grid so the weapon list gets full width.
    EndGrid();
#ifdef STUDIO_STANDALONE_PREVIEW
    Section("WEAPON SKINS");
    ImGui::TextWrapped("SkinHax runtime data is available in the production game build.");
#else
    OblivionSkinHax::DrawPanel(Scale);
#endif
}

static int EnabledCount(int page) {
    switch(page) {
        case 0: return Config.ESPMenu.Esp+Config.ESPMenu.isPlayerLine+Config.ESPMenu.Box+
            Config.ESPMenu.Health+Config.ESPMenu.Name+Config.ESPMenu.Distance+Config.ESPMenu.Alert+
            Config.ESPMenu.Vehicles+Config.ExtraMenu.Spectate+checkboxStates["wallhack"]+
            Config.ExtraMenu.Radar1+checkboxStates["BRTAGS"];
        case 1: return SnowB+SlideTackle+isSpeedHackEnabled+isJumpAdjustmentEnabled;
        case 2: return Config.Aim.AimAssist+Config.Aim.Aimbot360+Config.Aim.AimSilent+Config.ExtraMenu.Revive+Config.ExtraMenu.Medicine;
        case 3: return Config.ExtraMenu.Kinetic+Config.ExtraMenu.Recoil+Config.ExtraMenu.Spread+
            Config.ExtraMenu.Reload+Config.ExtraMenu.Scope+Config.ExtraMenu.Switch+Config.ExtraMenu.Shake+
            checkboxStates["AMMO"]+Config.ExtraMenu.Rpd+Config.ExtraMenu.Range+Config.ExtraMenu.AutoSnipe+
            Config.ExtraMenu.Hit+Config.ExtraMenu.Auto+Config.ExtraMenu.JetS+isExecute+
            Config.ExtraMenu.Parachute+Config.ExtraMenu.Diving+Config.ExtraMenu.TestGulag+
            Config.ExtraMenu.MagnetLoot+Config.ExtraMenu.FastLoot+Config.ExtraMenu.JetJ;
case 4: return checkboxStates["highend"]
    + checkboxStates["anti"]
    + checkboxStates["FRAMERATE"]
    + showFloatingIcon
    + Config.ExtraMenu.StretchScreen;
        case 5:
#ifdef STUDIO_STANDALONE_PREVIEW
            return 0;
#else
            return OblivionSkinHax::gHasSelectedSkin ? 1 : 0;
#endif
        default: return 0;
    }
}
static void PreviewBackdrop() {
#ifdef STUDIO_STANDALONE_PREVIEW
    auto* d=ImGui::GetBackgroundDrawList();
    const ImVec2 size=ImGui::GetIO().DisplaySize;
    d->AddRectFilledMultiColor(ImVec2(0,0),size,IM_COL32(22,28,39,255),IM_COL32(33,40,45,255),
                              IM_COL32(13,19,25,255),IM_COL32(22,21,33,255));
    struct Light { float x,y,r; int red,green,blue; };
    const Light lights[]={{.15f,.35f,.46f,112,60,172},{.78f,.35f,.43f,10,113,123},
                          {.81f,.79f,.41f,174,52,88},{.30f,.86f,.3f,16,125,105}};
    const float side=std::min(size.x,size.y);
    for(const auto& light:lights) {
        for(int ring=26;ring>0;--ring) {
            const float t=ring/26.f;
            d->AddCircleFilled(ImVec2(size.x*light.x,size.y*light.y),side*light.r*t,
                IM_COL32(light.red,light.green,light.blue,5),64);
        }
    }
#endif
}
#ifndef STUDIO_STANDALONE_PREVIEW
static void LoginPanel() {
    const ImVec2 display=UsableDisplaySize();
    const ImVec2 size(std::min(S(460),display.x-S(24)),std::min(S(380),display.y-S(30)));
    ImGui::SetNextWindowSize(size,ImGuiCond_Always);
    ImGui::SetNextWindowPos(ImVec2(display.x*.5f,display.y*.5f),ImGuiCond_Always,ImVec2(.5f,.5f));
    if(ImGui::Begin("##glass_access",nullptr,ImGuiWindowFlags_NoDecoration|ImGuiWindowFlags_NoSavedSettings)) {
        ImGui::SetWindowFontScale(Scale*15.f/22.f);
        auto* d=ImGui::GetWindowDrawList();
        d->AddRectFilled(ImGui::GetWindowPos(),Add(ImGui::GetWindowPos(),size),IM_COL32(19,24,32,238),S(32));
        Glass(d,ImGui::GetWindowPos(),size,32);
        Heading("VOID", "Enter your license key to open the control center.");
        ImGui::TextUnformatted("License key");
        ImGui::SetNextItemWidth(-1);
        ImGui::InputTextWithHint("##license","Paste or enter your key",s,sizeof(s));
        const float half=(ImGui::GetContentRegionAvail().x-S(12))*.5f;
        if(Button("Paste key",ImVec2(half,S(44)))) {
            const std::string key=getClipboard();
            std::snprintf(s,sizeof(s),"%s",key.c_str());
        }
        ImGui::SameLine(0,S(12));
        if(Button("Continue",ImVec2(half,S(44)),true)) {
            AuthError=Login(s);
            if(AuthError=="OK") isLogin=bValid && g_Auth==g_Token;
            if(AuthError=="OK" && !isLogin) AuthError="Session verification failed.";
        }
        ImGui::TextWrapped("Paste copies your key. Continue verifies access.");
        if(!AuthError.empty() && AuthError!="OK") {
            ImGui::PushStyleColor(ImGuiCol_Text,ImVec4(1,.62f,.60f,1));
            ImGui::TextWrapped("%s",AuthError.c_str()); ImGui::PopStyleColor();
        }
    }
    ImGui::End();
}
#endif
static void DrawPage() {
    CurrentIcon=Page; ExclusiveDrag=false;
    switch(Page) {
        case 0: Visuals(); break;
        case 1: Movement(); break;
        case 2: AimControls(); break;
        case 3: MemoryControls(); break;
        case 4: SystemControls(); break;
        case 5: SkinHaxControls(); break;
    }
    EndGrid();
}
static void ControlCenter() {
    const ImVec2 display=UsableDisplaySize();
    const ImVec2 size(std::min(S(940),display.x-S(24)),display.y-S(68));
    const ImVec2 pos((display.x-size.x)*.5f,S(12));
    ImGui::SetNextWindowPos(pos,ImGuiCond_Always);
    ImGui::SetNextWindowSize(size,ImGuiCond_Always);
    ImGuiWindowFlags flags=ImGuiWindowFlags_NoDecoration|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoSavedSettings;
    if(NavigationOpen || NavigationProgress>.01f) flags|=ImGuiWindowFlags_NoInputs;
    if(ImGui::Begin("##white_crowns",nullptr,flags)) {
        ImGui::SetWindowFontScale(Scale*15.f/22.f);
        auto* d=ImGui::GetWindowDrawList();
        d->PushClipRect(pos,Add(pos,size),false);
        d->AddRectFilled(pos,Add(pos,size),IM_COL32(19,24,32,142),S(32));
        Glass(d,pos,size,32);
        d->PopClipRect();
        const float headerHeight=S(size.x<S(580)?133:118);
        const bool narrow=size.x<S(580);
        Text(d,Add(pos,V(24,21)),12,Muted(),"VOID  /  CONTROL CENTER");
        const float nameWidth=size.x-S(142);
        Text(d,Add(pos,V(24,43)),31,Ink(),Pages[Page],nameWidth,true);
        char subtitle[120];
        std::snprintf(subtitle,sizeof(subtitle),"%s  /  %d active",PageDetails[Page],EnabledCount(Page));
        Text(d,Add(pos,V(24,84)),13,Muted(),subtitle,size.x-S(48));
        // The circular buttons have separate 44-point touch targets.
        ImGui::SetCursorPos(ImVec2(size.x-S(119),S(40)));
        if(Tap("##browse",V(44,44))) NavigationOpen=true;
        Glass(d,ImVec2(pos.x+size.x-S(119),pos.y+S(40)),V(44,44),22,false,ImGui::IsItemHovered());
        for(int i=0;i<4;++i) d->AddCircleFilled(ImVec2(pos.x+size.x-S(104)+S(i%2*13),pos.y+S(55+i/2*13)),S(3),Ink(),12);
        if(ImGui::IsItemHovered()) ImGui::SetTooltip("Browse tabs and sections");
        ImGui::SetCursorPos(ImVec2(size.x-S(67),S(40)));
        if(Tap("##close",V(44,44))) { show=false; NavigationOpen=false; ResetGestures(); }
        Glass(d,ImVec2(pos.x+size.x-S(67),pos.y+S(40)),V(44,44),22,false,ImGui::IsItemHovered());
        d->AddLine(ImVec2(pos.x+size.x-S(51),pos.y+S(56)),ImVec2(pos.x+size.x-S(39),pos.y+S(68)),Ink(),S(2));
        d->AddLine(ImVec2(pos.x+size.x-S(39),pos.y+S(56)),ImVec2(pos.x+size.x-S(51),pos.y+S(68)),Ink(),S(2));
        if(ImGui::IsItemHovered()) ImGui::SetTooltip("Hide controls. Swipe up on the home bar to return.");
        ImGui::SetCursorPos(V(narrow?12:18,headerHeight/Scale));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding,V(5,0));
        ImGui::PushID(Page);
        if(ImGui::BeginChild("##settings",ImVec2(size.x-S(narrow?24:36),std::max(S(30),size.y-headerHeight-S(14))),false,
                            ImGuiWindowFlags_AlwaysUseWindowPadding|ImGuiWindowFlags_NoBackground)) {
            DrawPage();
            if(!NavigationOpen && NavigationProgress<.01f) ContentScroll();
        }
        ImGui::EndChild(); ImGui::PopID(); ImGui::PopStyleVar();
    }
    ImGui::End();
}
static void NavigationSheet() {
    const float target=NavigationOpen?1.f:0.f;
    NavigationProgress+=(target-NavigationProgress)*std::min(1.f,ImGui::GetIO().DeltaTime*19.f);
    if(std::fabs(NavigationProgress-target)<.005f) NavigationProgress=target;
    if(NavigationProgress<=.001f) return;
    const ImVec2 display=UsableDisplaySize();
    const bool narrow=display.x<S(600);
    const ImVec2 size(std::min(S(790),display.x-S(24)),std::min(S(narrow?485:380),display.y-S(100)));
    const ImVec2 p((display.x-size.x)*.5f,display.y-S(62)-size.y+(1-NavigationProgress)*size.y);
    ImGui::SetNextWindowPos(ImVec2(0,0)); ImGui::SetNextWindowSize(display);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding,ImVec2(0,0));
    const ImGuiWindowFlags flags=ImGuiWindowFlags_NoDecoration|ImGuiWindowFlags_NoMove|
        ImGuiWindowFlags_NoSavedSettings|ImGuiWindowFlags_NoBackground;
    if(ImGui::Begin("##navigation_backdrop",nullptr,flags)) {
        ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(0,0),display,IM_COL32(3,6,12,(int)(160*NavigationProgress)));
        if(ImGui::InvisibleButton("##dismiss",display)) NavigationOpen=false;
    }
    ImGui::End(); ImGui::PopStyleVar();
    ImGui::SetNextWindowPos(p); ImGui::SetNextWindowSize(size);
    if(ImGui::Begin("##glass_navigation",nullptr,flags)) {
        ImGui::SetWindowFontScale(Scale*15.f/22.f);
        auto* d=ImGui::GetWindowDrawList();
        d->PushClipRect(p,Add(p,size),false);
        d->AddRectFilled(p,Add(p,size),IM_COL32(29,36,48,249),S(32));
        Glass(d,p,size,32);
        d->PopClipRect();
        ImGui::SetCursorPos(V(0,0));
        const bool handle=ImGui::InvisibleButton("##sheet_handle",ImVec2(size.x,S(32)));
        if((ImGui::IsItemActive() && ImGui::GetMouseDragDelta(0).y>S(28)) || handle) NavigationOpen=false;
        d->AddRectFilled(ImVec2(p.x+size.x*.5f-S(23),p.y+S(12)),ImVec2(p.x+size.x*.5f+S(23),p.y+S(16)),IM_COL32(238,244,255,160),S(2));
        Text(d,Add(p,V(22,39)),25,Ink(),"Navigation",0,true);
        Text(d,Add(p,V(22,72)),13,Muted(),"Choose a tab, then jump to a section.",size.x-S(44));
        ImGui::SetCursorPos(V(16,102));
        if(ImGui::BeginChild("##nav_content",ImVec2(size.x-S(32),size.y-S(116)),false)) {
            auto* tileDraw=ImGui::GetWindowDrawList();
            const int columns=narrow?3:5;
            if(ImGui::BeginTable("##tabs",columns,ImGuiTableFlags_SizingStretchSame)) {
                for(int i=0;i<6;++i) {
                    ImGui::TableNextColumn(); ImGui::PushID(i);
                    const ImVec2 tile=ImGui::GetCursorScreenPos();
                    const ImVec2 tileSize(ImGui::GetContentRegionAvail().x,S(86));
                    if(Tap("##tab",tileSize)) { Page=i; PendingSection=nullptr; ResetGestures(); }
                    Glass(tileDraw,tile,tileSize,23,Page==i,ImGui::IsItemHovered());
                    Icon(tileDraw,ImVec2(tile.x+tileSize.x*.5f-S(13),tile.y+S(12)),S(26),i,Ink());
                    const float textWidth=BodyFont->CalcTextSizeA(S(14),FLT_MAX,0,Pages[i]).x;
                    Text(tileDraw,ImVec2(tile.x+(tileSize.x-textWidth)*.5f,tile.y+S(49)),14,Ink(),Pages[i]);
                    if(Page==i) tileDraw->AddCircleFilled(ImVec2(tile.x+tileSize.x*.5f,tile.y+S(75)),S(2),Ink(),12);
                    ImGui::PopID();
                }
                ImGui::EndTable();
            }
            ImGui::Spacing();
            ImGui::Text("%s sections",Pages[Page]);
            if(ImGui::BeginTable("##sections",narrow?1:2,ImGuiTableFlags_SizingStretchSame)) {
                for(int i=0;i<4 && Sections[Page][i];++i) {
                    ImGui::TableNextColumn();
                    if(Button(Sections[Page][i],ImVec2(-1,S(42)))) {
                        PendingSection=Sections[Page][i]; NavigationOpen=false;
                    }
                }
                ImGui::EndTable();
            }
            if(Button("Open selected tab",ImVec2(-1,S(44)),true)) NavigationOpen=false;
            ExclusiveDrag=false; ContentScroll();
        }
        ImGui::EndChild();
    }
    ImGui::End();
}
static void HomeBar() {
    const ImVec2 display=UsableDisplaySize();
    const ImVec2 size(std::min(S(230),display.x),S(44));
    const ImVec2 p((display.x-size.x)*.5f,display.y-size.y-S(4));
    ImGui::SetNextWindowPos(p); ImGui::SetNextWindowSize(size);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding,ImVec2(0,0));
    if(ImGui::Begin("##home_bar",nullptr,ImGuiWindowFlags_NoDecoration|ImGuiWindowFlags_NoMove|
                    ImGuiWindowFlags_NoSavedSettings|ImGuiWindowFlags_NoBackground)) {
        ImGui::BringWindowToDisplayFront(ImGui::GetCurrentWindow());
        const bool pressed=ImGui::InvisibleButton("##home_gesture",size);
        // Signed delta distinguishes an upward swipe from a tap/downward drag.
        const bool up=ImGui::IsItemActive() && ImGui::GetMouseDragDelta(0).y<-S(20);
        if(up || (pressed && ImGui::GetIO().MouseDragMaxDistanceSqr[0]<S(8)*S(8))) {
            show=true; NavigationOpen=true;
        }
        auto* d=ImGui::GetWindowDrawList();
        const float half=std::min(S(66),size.x*.35f);
        const float y=p.y+S(31);
        d->AddRectFilled(ImVec2(display.x*.5f-half,y+S(2)),ImVec2(display.x*.5f+half,y+S(7)),IM_COL32(0,0,0,110),S(3));
        d->AddRectFilled(ImVec2(display.x*.5f-half,y),ImVec2(display.x*.5f+half,y+S(4)),Ink(),S(2));
        if(showFloatingIcon || !show) {
            const char* hint="Swipe up for controls";
            const float width=BodyFont->CalcTextSizeA(S(11),FLT_MAX,0,hint).x;
            Text(d,ImVec2((display.x-width)*.5f,p.y+S(4)),11,Muted(),hint);
        }
        if(ImGui::IsItemHovered()) ImGui::SetTooltip("Swipe up or tap to open navigation");
    }
    ImGui::End(); ImGui::PopStyleVar();
}
static void Draw() {
    if(UsableDisplaySize().x<100 || UsableDisplaySize().y<140) return;
    Theme theme(menu);
    PreviewBackdrop();
#ifndef STUDIO_STANDALONE_PREVIEW
    if(!isLogin) { LoginPanel(); return; }
#endif
    if(ImGui::IsKeyPressed(ImGuiKey_Escape)) {
        if(NavigationOpen) NavigationOpen=false;
        else show=false;
    }
    if(ImGui::IsKeyPressed(ImGuiKey_Home) && !ImGui::GetIO().WantTextInput) {
        show=true; NavigationOpen=true;
    }
    if(show) ControlCenter();
    else { NavigationOpen=false; NavigationProgress=0; }
    NavigationSheet();
    HomeBar();
}
} // namespace StudioUI
