#pragma once
bool isSpeedHackEnabled = false;
bool SnowB = false;
float SnowBsize = 0.0f;
float speedHackMultiplier = 0.0f;
bool isJumpAdjustmentEnabled = false;
float jumpHeightMultiplier = 1.0f;


struct ConfigData {
    // Tab 1 - ESP Settings
    bool Esp;
    bool PlayerLine;
    bool Box;
    bool Health;
    bool Name;
    bool Distance;
    bool Count;
    bool Alert;
    int CrosshairType;
    float AimSize;
    bool Crosshair;
    bool Aimline;

    // Tab 2 - Memory Settings
    bool AimAssist;
    float AimAssistSize;
    bool Kinetic;
    bool Recoil;
    bool Spread;
    bool Reload;
    bool Scope;
    bool Switch;
    bool Shake;
    bool Flash;
    bool Small;
    bool Cost;
    bool Rpd;
    bool Range;
    bool AutoSnipe;
    bool Hit;
    bool Auto;
    bool Fire;
	bool magnet;
	bool loot;
	bool gulag;

    // Tab 3 - Ability Settings
    bool SnowB;
    float SnowBsize;
    bool SlideTackle;
    float SlideRange;
    bool SpeedHack;
    float SpeedMultiplier;
    bool JumpAdjustment;
    float JumpMultiplier;
    bool Spectate;
    bool Outline;
    bool WallHack;
    bool Red;
    bool JetJ;
    bool JetS;
    bool Execute;
    bool Radar1;
    bool Parachute;
    bool Diving;

    // Tab 4 - Aimbot Settings
    bool Aimbot360;
    bool AimSilent;
    int Target;
    int Trigger;
    int By;

    // Tab 5 - Skins Settings
    bool Memento;
    bool Morning;
    bool Amoeba;
    bool Aks;
    bool Quan;
    bool Glorious;
    bool Neon;
    bool Celestial;
    bool Bam;
    bool Demon;
    bool Lotus;
    bool Phoenix;
    bool Qq;
    bool Song;
    bool Siege;
    bool Templar;
    bool Spectre;
    bool Darkside;

    // Tab 6 - Events (No settings to save)

// Tab 7 - Performance Settings
bool FpsLevel;                // Toggle for base frame rate unlock
float FpsLevel1;              // Value for base FPS (e.g., 30–180)
bool showFPSLevelSlider;      // Show/hide base FPS slider

bool FpsLevelUltra;           // Toggle for ultra frame rate unlock
float FpsLevelUltra1;         // Value for ultra FPS (e.g., 60–240)
bool showFPSLevelUltraSlider; // Show/hide ultra FPS slider

bool Frame;                   // Possibly another setting (e.g., VSync)
bool Fpss;                    // Possibly for showing FPS counter
    bool NoSlideFatigue;

};

void saveAllConfig() {
    ConfigData config{};
    
    // Tab 1
    config.Esp = Config.ESPMenu.Esp;
    config.PlayerLine = Config.ESPMenu.isPlayerLine;
    config.Box = Config.ESPMenu.Box;
    config.Health = Config.ESPMenu.Health;
    config.Name = Config.ESPMenu.Name;
    config.Distance = Config.ESPMenu.Distance;
    config.Count = Config.ESPMenu.Count;
    config.Alert = Config.ESPMenu.Alert;
    config.CrosshairType = static_cast<int>(Config.ESPMenu.CrosshairType);
    config.AimSize = Config.Aim.size;
    config.Crosshair = Config.ESPMenu.Crosshair;
    config.Aimline = Config.ESPMenu.Aimline;

    // Tab 2
    config.AimAssist = Config.Aim.AimAssist;
    config.AimAssistSize = Config.Aim.AimAssistSize;
    config.Kinetic = Config.ExtraMenu.Kinetic;
    config.Recoil = Config.ExtraMenu.Recoil;
    config.Spread = Config.ExtraMenu.Spread;
    config.Reload = Config.ExtraMenu.Reload;
    config.Scope = Config.ExtraMenu.Scope;
    config.Switch = Config.ExtraMenu.Switch;
    config.Shake = Config.ExtraMenu.Shake;
    config.Flash = Config.ExtraMenu.Flash;
    config.Small = Config.ExtraMenu.Small;
    config.Cost = Config.ExtraMenu.Cost;
    config.Rpd = Config.ExtraMenu.Rpd;
    config.Range = Config.ExtraMenu.Range;
    config.AutoSnipe = Config.ExtraMenu.AutoSnipe;
    config.Hit = Config.ExtraMenu.Hit;
    config.Auto = Config.ExtraMenu.Auto;
    config.Fire = Config.ExtraMenu.Fire;

    // Tab 3
    config.SnowB = SnowB;
    config.SnowBsize = SnowBsize;
    config.SlideTackle = SlideTackle;
    config.SlideRange = SlideRange;
    config.SpeedHack = isSpeedHackEnabled;
    config.SpeedMultiplier = speedHackMultiplier;
    config.JumpAdjustment = isJumpAdjustmentEnabled;
    config.JumpMultiplier = jumpHeightMultiplier;
    config.Spectate = Config.ExtraMenu.Spectate;
    config.Outline = Config.ExtraMenu.Outline;
    config.WallHack = Config.ExtraMenu.WallHack;
    config.Red = Config.ExtraMenu.Red;
    config.JetJ = Config.ExtraMenu.JetJ;
    config.JetS = Config.ExtraMenu.JetS;
    config.NoSlideFatigue = Config.ExtraMenu.NoSlideFatigue;
    config.Execute = isExecute;
    config.Radar1 = Config.ExtraMenu.Radar1;
    config.Parachute = Config.ExtraMenu.Parachute;
    config.Diving = Config.ExtraMenu.Diving;

    // Tab 4
    config.Aimbot360 = Config.Aim.Aimbot360;
    config.AimSilent = Config.Aim.AimSilent;
    config.Target = static_cast<int>(Config.Aim.Target);
    config.Trigger = static_cast<int>(Config.Aim.Trigger);
    config.By = static_cast<int>(Config.Aim.By);

    // Tab 5
    config.Memento = Config.ExtraMenu.Memento;
    config.Morning = Config.ExtraMenu.Morning;
    config.Amoeba = Config.ExtraMenu.Amoeba;
    config.Aks = Config.ExtraMenu.Aks;
    config.Quan = Config.ExtraMenu.Quan;
    config.Glorious = Config.ExtraMenu.Glorious;
    config.Neon = Config.ExtraMenu.Neon;
    config.Celestial = Config.ExtraMenu.Celestial;
    config.Bam = Config.ExtraMenu.Bam;
    config.Demon = Config.ExtraMenu.Demon;
    config.Lotus = Config.ExtraMenu.Lotus;
    config.Phoenix = Config.ExtraMenu.Phoenix;
    config.Qq = Config.ExtraMenu.Qq;
    config.Song = Config.ExtraMenu.Song;
    config.Siege = Config.ExtraMenu.Siege;
    config.Templar = Config.ExtraMenu.Templar;
    config.Spectre = Config.ExtraMenu.Spectre;
    config.Darkside = Config.ExtraMenu.Darkside;

    // Tab 7
    config.FpsLevel = Config.Aim.FpsLevel;
    config.FpsLevel1 = Config.Aim.FpsLevel1;
    config.Frame = Config.ExtraMenu.Frame;
    config.Fpss = Config.ExtraMenu.Fpss;

    std::ofstream file("/storage/emulated/0/Download/wc_config.dat", std::ios::binary);
    if (file.is_open()) {
        file.write(reinterpret_cast<char*>(&config), sizeof(ConfigData));
        file.close();
        ImGui::OpenPopup("Config Saved");
    } else {
        ImGui::OpenPopup("Config Save Error");
    }
}

bool loadAllConfig() {
    ConfigData config{};
    std::ifstream file("/storage/emulated/0/Download/wc_config.dat", std::ios::binary);
    if (file.is_open()) {
        file.read(reinterpret_cast<char*>(&config), sizeof(ConfigData));
        file.close();

        // Tab 1
        Config.ESPMenu.Esp = config.Esp;
        Config.ESPMenu.isPlayerLine = config.PlayerLine;
        Config.ESPMenu.Box = config.Box;
        Config.ESPMenu.Health = config.Health;
        Config.ESPMenu.Name = config.Name;
        Config.ESPMenu.Distance = config.Distance;
        Config.ESPMenu.Count = config.Count;
        Config.ESPMenu.Alert = config.Alert;
        Config.ESPMenu.CrosshairType = static_cast<decltype(Config.ESPMenu.CrosshairType)>(config.CrosshairType);
        Config.Aim.size = config.AimSize;
        Config.ESPMenu.Crosshair = config.Crosshair;
        Config.ESPMenu.Aimline = config.Aimline;

        // Tab 2
        Config.Aim.AimAssist = config.AimAssist;
        Config.Aim.AimAssistSize = config.AimAssistSize;
        Config.Aim.showAimAssistSlider = config.AimAssist;
        Config.ExtraMenu.Kinetic = config.Kinetic;
        Config.ExtraMenu.Recoil = config.Recoil;
        Config.ExtraMenu.Spread = config.Spread;
        Config.ExtraMenu.Reload = config.Reload;
        Config.ExtraMenu.Scope = config.Scope;
        Config.ExtraMenu.Switch = config.Switch;
        Config.ExtraMenu.Shake = config.Shake;
        Config.ExtraMenu.Flash = config.Flash;
        Config.ExtraMenu.Small = config.Small;
        Config.ExtraMenu.Cost = config.Cost;
        Config.ExtraMenu.Rpd = config.Rpd;
        Config.ExtraMenu.Range = config.Range;
        Config.ExtraMenu.AutoSnipe = config.AutoSnipe;
        Config.ExtraMenu.Hit = config.Hit;
        Config.ExtraMenu.Auto = config.Auto;
        Config.ExtraMenu.Fire = config.Fire;

        // Tab 3
        SnowB = config.SnowB;
        SnowBshow = config.SnowB;
        SnowBsize = config.SnowBsize;
        SlideTackle = config.SlideTackle;
        SlideShow = config.SlideTackle;
        SlideRange = config.SlideRange;
        isSpeedHackEnabled = config.SpeedHack;
        showSpeedHackSlider = config.SpeedHack;
        speedHackMultiplier = config.SpeedMultiplier;
        isJumpAdjustmentEnabled = config.JumpAdjustment;
        jumpHeightMultiplier = config.JumpMultiplier;
        Config.ExtraMenu.Spectate = config.Spectate;
        Config.ExtraMenu.Outline = config.Outline;
        Config.ExtraMenu.WallHack = config.WallHack;
        Config.ExtraMenu.Red = config.Red;
        Config.ExtraMenu.JetJ = config.JetJ;
        Config.ExtraMenu.JetS = config.JetS;
        Config.ExtraMenu.NoSlideFatigue = config.NoSlideFatigue;
        isExecute = config.Execute;
        Config.ExtraMenu.Radar1 = config.Radar1;
        Config.ExtraMenu.Parachute = config.Parachute;
        Config.ExtraMenu.Diving = config.Diving;

        // Tab 4
        Config.Aim.Aimbot360 = config.Aimbot360;
        Config.Aim.AimSilent = config.AimSilent;
        Config.Aim.Target = static_cast<decltype(Config.Aim.Target)>(config.Target);
        Config.Aim.Trigger = static_cast<decltype(Config.Aim.Trigger)>(config.Trigger);
        Config.Aim.By = static_cast<decltype(Config.Aim.By)>(config.By);

        // Tab 5
        Config.ExtraMenu.Memento = config.Memento;
        Config.ExtraMenu.Morning = config.Morning;
        Config.ExtraMenu.Amoeba = config.Amoeba;
        Config.ExtraMenu.Aks = config.Aks;
        Config.ExtraMenu.Quan = config.Quan;
        Config.ExtraMenu.Glorious = config.Glorious;
        Config.ExtraMenu.Neon = config.Neon;
        Config.ExtraMenu.Celestial = config.Celestial;
        Config.ExtraMenu.Bam = config.Bam;
        Config.ExtraMenu.Demon = config.Demon;
        Config.ExtraMenu.Lotus = config.Lotus;
        Config.ExtraMenu.Phoenix = config.Phoenix;
        Config.ExtraMenu.Qq = config.Qq;
        Config.ExtraMenu.Song = config.Song;
        Config.ExtraMenu.Siege = config.Siege;
        Config.ExtraMenu.Templar = config.Templar;
        Config.ExtraMenu.Spectre = config.Spectre;
        Config.ExtraMenu.Darkside = config.Darkside;

        // Tab 7
        Config.Aim.FpsLevel = config.FpsLevel;
        Config.Aim.showFPSLevelSlider = config.FpsLevel;
        Config.Aim.FpsLevel1 = config.FpsLevel1;
        Config.ExtraMenu.Frame = config.Frame;
        Config.ExtraMenu.Fpss = config.Fpss;

        return true;
    }
    return false;
}
