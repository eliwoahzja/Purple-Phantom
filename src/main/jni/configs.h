#pragma once
#include "Patches.h"
#include "Hacks/Feature/Init_Hacks.h"

void ApplyPatches() {
    if (Config.ExtraMenu.Range)     Patches.range.Modify();     else Patches.range.Restore();
    if (Config.ExtraMenu.Kinetic)   Patches.kinetic.Modify();   else Patches.kinetic.Restore();
    if (Config.ExtraMenu.Outline)   Patches.outline.Modify();   else Patches.outline.Restore();
    if (Config.ExtraMenu.Fps)       Patches.fps.Modify();       else Patches.fps.Restore();
    if (Config.ExtraMenu.Frame)     Patches.frame.Modify();     else Patches.frame.Restore();
    if (Config.ExtraMenu.Rpd)       Patches.rpd.Modify();       else Patches.rpd.Restore();
    if (Config.ExtraMenu.Grapss)    Patches.grap.Modify();      else Patches.grap.Restore();
    if (Config.ExtraMenu.WallHack)   Patches.A1.Modify();        else Patches.A1.Restore();
    if (Config.ExtraMenu.AutoSnipe) Patches.autosnipe.Modify(); else Patches.autosnipe.Restore();
    if (Config.ExtraMenu.Revive)    Patches.revive.Modify();    else Patches.revive.Restore();
    if (Config.ExtraMenu.Diving)    Patches.wing.Modify();      else Patches.wing.Restore();
    if (Config.ExtraMenu.Tutorial)  Patches.tutorial.Modify();  else Patches.tutorial.Restore();
    if (Config.ExtraMenu.Cost)      Patches.cost.Modify();      else Patches.cost.Restore();
    if (Config.ExtraMenu.Red)       Patches.red.Modify();       else Patches.red.Restore();
    if (Config.ExtraMenu.JetS)      Patches.jets.Modify();      else Patches.jets.Restore();
    if (Config.ExtraMenu.NoSlideFatigue) {
        Patches.NoSlideFatigue.Modify();
        Patches.NoSlideFatiguePve.Modify();
    } else {
        Patches.NoSlideFatigue.Restore();
        Patches.NoSlideFatiguePve.Restore();
    }
    if (Config.ExtraMenu.JetJ)      Patches.jetj.Modify();      else Patches.jetj.Restore();
    if (Config.ExtraMenu.Fire)      Patches.fire.Modify();      else Patches.fire.Restore();
    if (Config.ExtraMenu.MagnetLoot)Patches.magnet.Modify();    else Patches.magnet.Restore();
    if (Config.ExtraMenu.TestGulag) Patches.gulag.Modify();     else Patches.gulag.Restore();
    if (Config.ExtraMenu.FastLoot)  Patches.loot.Modify();      else Patches.loot.Restore();
}
