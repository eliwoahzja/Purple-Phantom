#pragma once
#include "CROWEXE/antiprocobf.hpp"


struct WeaponSkinFields {
    uint8_t ColorID;
    uint8_t GoGetPos;
    bool IsCleanCamo;
    uint8_t SkinType;
    uint BRWeaponID;
    int BrSkinWeaponID;
    int CamoID;
    int ColorSubID;
    uint ID;
    uint WeaponID;
    void *LOCID_Describe;
    void *LOCID_HowToGet;
    void *LOCID_Name;
    Array<int> *SkinID1P;
    Array<int> *SkinID3P;
};
WeaponSkinFields* weaponskinFields;

struct RoleSkinFields {
    uint8_t ClothType; //0x18 
     uint8_t ColorID; //0x19 
     uint8_t GoGetPos; //0x1A 
     uint8_t HatSubMode; //0x1B 
     uint8_t IsCanBeUsedForAllRole; //0x1C 
     bool IsCloseScreenSaver; //0x1D 
     uint8_t SecondTab; //0x1E 
     bool ShowInBag; //0x1F 
     bool ShowRare; //0x20 
     int BRBagAssetID; //0x24 
     int BRClothAssetID; //0x28 
     int BRHatAssetID; //0x2C 
     int BRLeftArmAssetID; //0x30 
     int BRSuitAssetID; //0x34 
     int BRWingAssetID; //0x38 
     int BagAssetID; //0x3C 
     int CamoID; //0x40 
     int ClothAssetID; //0x44 
     int FxAssetIDHandGestureUI; //0x48 
     int FxAssetIDShow; //0x4C 
     int FxAssetID_1P; //0x50 
     int FxAssetID_3P; //0x54 
     int FxAssetID_UI; //0x58 
     int HatAssetID; //0x5C 
     uint ID; //0x60 
     int LeftArm1PAssetID; //0x64 
     int LeftArmAssetID; //0x68 
     int ShowBagAssetID; //0x6C 
     int ShowClothAssetID; //0x70 
     int ShowHatAssetID; //0x74 
     int SuitAssetID; //0x78 
     int WingAssetID; //0x7C 
};
RoleSkinFields* roleskinFields;

struct CharacterModelFields {
    uint8_t BindEffect1PType; //0x10
    bool Female; //0x11
    char _pad_12[2];
     uint AvatarAnimationType; //0x14 
     int BRBagModel; //0x18 
     int BRHeadModel; //0x1C 
     int BRLobby; //0x20 
     int BRModel; //0x24 
     int BindEffect1P; //0x28 
     int ChangeClipEffect1P; //0x2C 
     int DefaultModelID; //0x30 
     int Guarder1P; //0x34 
     int Guarder3P; //0x38 
     int GuarderBagModel; //0x3C 
     int GuarderHeadModel; //0x40 
     int GuarderLobby; //0x44 
     int HeadIgnoreGUPSkin; //0x48 
     int SpReceiveAnimType; //0x4C 
     int Traitor1P; //0x50 
     int Traitor3P; //0x54 
     float* BagOffset; //0x58 
     string* Desc; //0x60 
     float* VehicleOffset; //0x68 
     int* VestOtherModelID; //0x70 
     uintptr_t ItemID; //0x78 
     uintptr_t LitePackDefault; //0x80 
};
CharacterModelFields* characterfields;



struct RolePackFields {
     int mMatchRoomEasterEggType; // 0x10
     uint mMatchRoomEasterEggCharacterID; // 0x14
     String * mMatchRoomEasterEggActionName; // 0x18
     bool DisableCustomize; // 0x20
  bool EntryDefaultAppearance; // 0x21
  bool SkipWinnerCircleAnimation; // 0x22
  bool UseDynamicLobbyScene; // 0x23
  bool UseVFWFeature; // 0x24
     int AirItem1; // 0x28
     int AnimationWhenSell; // 0x2C
     int CabinIndivID; // 0x30
     int DeadBoxID; // 0x34
     int DynamicRoomAssetID; // 0x38
     int EntryAnimID; // 0x3C
     int EntryUIEffectAssetID; // 0x40
     int EntryVideoAnimID; // 0x44
     float EntryVideoPlayTime; // 0x48
     int EntryVideoTagAnimID; // 0x4C
     int GestureId; // 0x50
     int HandEffectUI; // 0x54
     int KillStreakSkinID; // 0x58
     int LoadingFrame; // 0x5C
     int LobbySceneType; // 0x60
     int MVPIndivID; // 0x64
     int MatchRoomSort; // 0x68
     int OpenSceneIndivID; // 0x6C
     int PendantShowID; // 0x70
     int PetID; // 0x74
     int PetSwitchEffectAssetID; // 0x78
     uint RolePackID; // 0x7C
     int SkillSkinID; // 0x70
     int SkillSkinUI; // 0x74
     int SpecialSceneFrameGroupAssetID; // 0x78
     int VTOLShowAssetID; // 0x7C
     String * AirPoseBR; // 0x80
     String * EntryAnim; // 0x88
     String * EntryIdleAnim; // 0x90
     void* EntryIdleItems; // 0x98
     void* EntryItems; // 0xA0
     void* EntryItemsAnim; // 0xA8
     void* EntryItemsIdleAnim; // 0xB0
     String * EntryUIAnimationStateName; // 0xB8
};
RolePackFields* packfields;


struct RoleConfFields {
     int roleLeftArmID; //0x14 
     int roleFinalSuitID; //0x18 
     int roleBasicHologramID; //0x1C 
     uint8_t ArchiveSwitch; //0x20
     uint8_t ArmorType; //0x21
     uint8_t ColorID; //0x22
     uint8_t ColorSubID; //0x23
     uint8_t DefaultPerk_1; //0x24
     uint8_t DefaultPerk_2; //0x25
     uint8_t DefaultPerk_3; //0x26
     bool DefaultShow; //0x27
     uint8_t DefaultSkill_1; //0x28
     bool ExternalUnVisible; //0x29
     bool IsVFW; //0x2A
     uint8_t Loadout_Magnifier_Flag; //0x2B
     uint8_t RoleLvGroupID; //0x2C
     bool ShowInBag; //0x2D
     uint8_t ShowOrder; //0x2E
     bool ShowRare; //0x2F
     int BlockAppearance; //0x30
     uint ID; //0x34
     int JetPackInfoID; //0x38
     int KeepInCatalog; //0x3C
     float RTPCMaxValue; //0x40
     int RolePackID; //0x44 
     void *AudioSwitch; //0x40 
    void *BRBankNameEX; //0x48 
    void *BasicAudioBank; //0x50 
    void *BasicAudioBank2; //0x58 
    void *BasicAudioBank3; //0x60 
    void *DefaultAudioBank; //0x68 
    void *DefaultAudioBank2; //0x70 
    void *DefaultAudioEventEx; //0x78 
    void *FoleyAudioBank; //0x80 
    void *FoleyAudioNameEx; //0x88 
    void *LOCID_Desc; //0x90 
    void *LOCID_Name; //0x98 
    void *LitePackageKeyName; //0xA0 
    void *MPBankNameEx; //0xA8 
    void *PVEBankNameEX; //0xB0 
    void *RTPCName; //0xB8 
    void *RoleSkinBox; //0xC0 
    void *SpecialAudioBank; //0xC8 
    void *SpecialAudioBank2; //0xD0 
    void *SpecialAudioEventEx; //0xD8 
    void *SpecialTips; //0xE0 
    void *WinnerCircle; //0xE8 
};
RoleConfFields* roleFields;


struct ItemResourceFields {
   void *UISmallSpriteName;   // 0x10
    void *UIMiniSpriteName; // 0x12
         void *UISpriteName;  // 0x14
    void *UISquareSpriteName; // 0x16
    int FxAssetID;                    // 0x18
    int ID;                           // 0x1C
    int InventoryModelID;             // 0x20
    int ModelAssetIDRaw;              // 0x24
    uintptr_t AvatarModelID;          // 0x28
};
ItemResourceFields* itemFields;

struct WeaponConfFields {
     uint8_t Accuracy; //0x20 
     bool CanEquipOptic; //0x21 
     uint8_t ColorID; //0x22 
     uint8_t ColorSubID; //0x23 
     uint8_t Control; //0x24 
     uint8_t Damage; //0x25 
     bool ExternalUnVisible; //0x26 
     uint8_t FireRate; //0x27 
     uint8_t LOCID_HowToGet; //0x28 
     uint8_t MaxAttach; //0x29 
     uint8_t MaxSkill; //0x2A 
     uint8_t Mobility; //0x2B 
     uint8_t Range; //0x2C 
     bool ShowRare; //0x2D 
     uint8_t SuperShowGroup; //0x2E 
     uint8_t WeaponBluePrintType; //0x2F 
     uint8_t WinnerCircleAnimID; //0x30 
     uint BaseWeaponId; //0x34 
     int DefWeaponSkinID; //0x38 
     int DefaultKillBrocast; //0x3C 
     uint ID; //0x40 
     int WinnerCircleWeaponMountType; //0x44 
     void *EnableCamoBeta; //0x48
     void *ChangeColorStr; //0x50  
     void *EnableCamoBeta2; //0x58  
     void *LOCID_Desc; //0x60  
     void *LOCID_Name; //0x68  
     void *ModelRotation; //0x70 
     void *UIProperty; //0x78 
     void *UIPropertyUnlockLevel; //0x80 

};
WeaponConfFields* weaponconfFields;




struct Item2InventoryFields {
    int ItemID; //0x20 
    int WeaponAssetGroupID; //0x24 
    int WeaponIconID; //0x28 
};
Item2InventoryFields* item2Fields;


struct WeaponAssetGroupFields {
     int FireEffectGroupID; //0x40 
     int Id; //0x44 
};
WeaponAssetGroupFields* weaponAssetFields;



struct WeaponFireEffectFields {
    bool ShellDropCanOverrideByMagazine; // 0x10 
    int AssetIdFor1P;                   // 0x14
    int AssetIdFor3P;                   // 0x18
    int AssetIdForBulletSmoke1P;        // 0x1C
    int AssetIdForBulletSmoke3P;        // 0x20
    int AssetIdForBulletTrack1P;        // 0x24
    int AssetIdForBulletTrack3P;        // 0x28
    int AssetIdForCharging1P;           // 0x2C
    int AssetIdForCrossHair;            // 0x30
    int AssetIdForDecalEffect;          // 0x34
    int AssetIdForExtra1P;              // 0x38
    int AssetIdForExtraBulletSmoke1P;   // 0x3C
    int AssetIdForExtraBulletSmoke3P;   // 0x40
    int AssetIdForHip1P;                // 0x44
    int AssetIdForHip3P;                // 0x48
    int AssetIdForHitEnemy;             // 0x4C
    int AssetIdForInspectionScreenEffect; // 0x50
    int AssetIdForSecondBulletSmoke1P;  // 0x54
    int AssetIdForSecondBulletSmoke3P;  // 0x58
    int AssetIdForSecondHip1P;          // 0x5C
    int AssetIdForSecondHip3P;          // 0x60
    
    int AttachAssetIdFor1P;             // 0x64
    int AttachAssetIdFor3P;             // 0x68
    int AttachAssetIdForHip1P;          // 0x6C
    int AttachAssetIdForHip3P;          // 0x70
    int DefaultKillEffectLevelUI;       // 0x74
    float HitEffectDelayDestroyTime;    // 0x78
    int HitEffectType;                  // 0x7C
    int Id;                             // 0x80
};
WeaponFireEffectFields* weaponfireFields;



struct WeaponConfExtraFields {
     uint8_t DefaultMythicArmor; //0x10 
     uint8_t DefaultMythicSig; //0x11 
     uint8_t GoGetPos; //0x12 
     uint8_t SecondTab; //0x13 
     uint8_t ThirdTab; //0x14 
     int DefaultDeadReplayEffectId; //0x18 
     int DefaultKillEffectId; //0x1C 
     int FireEffectGroupID; //0x20 
     uint ID; //0x24 
};
WeaponConfExtraFields* weaponextraFields;


struct MythicArmorFields {
     int AssetID; //0x14 
     int ColorID; //0x18 
     int ColorSubID; //0x1C 
     int GoGetPos; //0x20 
     uint ID; //0x24 
     int MythicArmorLevel; //0x28 
     int SecondTab; //0x2C 
     int ThirdTab; //0x30 
};
MythicArmorFields* mythicarmorFields;



struct MythicSightFields {
     bool ShowInBag; //0x10 
     char pad_11[3];
     int AssetID; //0x14 
     int ColorID; //0x18 
     int ColorSubID; //0x1C 
     uint ID; //0x20 
     int RelateMythicSight; //0x24 
     void* LOCID_Desc; //0x28 
     void* LOCID_Name; //0x30 
     Array<int> **WeaponCanEquip; //0x38
};
MythicSightFields* mythicsightFields;



struct KillEffectItemFields {
     Array<int> **RealAssetIDs; //0x10 
     bool IsAutoDemolition; //0x18 
     bool ShowInBag; //0x19 
     int ColorID; //0x1C 
     int ColorSubID; //0x20 
     uint CurrencyID; //0x24 
     int GoGetPos; //0x28 
     uint ID; //0x2C 
     int SecondTab; //0x30 
     int SellPrice; //0x34 
     int SellProtection; //0x38 
     int ThirdTab; //0x3C 
     Array<int> **AssetID; //0x40 
     Array<int> **AssetID_CN; //0x48 
     Array<int> **AssetID_GA; //0x50 
     Array<int> **AssetID_KR; //0x58 
     Array<int> **AssetID_VNG; //0x60 
     string* GoGetUrl; //0x68 
     Array<int> **KillCount; //0x70 
     void* LOCID_Desc; //0x78 
     void* LOCID_HowToGet; //0x80 
     void* LOCID_Name; //0x88 
     Array<int> **WeaponCanEquip; //0x90 
};
KillEffectItemFields* killeffectFields;



struct BRDeadboxSkinFields {
    int ColorID; //0x10 
    int DeadBoxEffectAsset; //0x14 
    int Flag; //0x18 
    int FlagAsset; //0x1C 
    uint ID; //0x20 
    int ModelAsset3P; //0x24 
    int ModelAssetUI; //0x28 
};
BRDeadboxSkinFields* deadboxFields;


struct BRDropPlaneSkinFields {
     int ColorID; //0x10 
     uint ID; //0x14 
     int ModelAsset1P; //0x18 
     int ModelAsset3P; //0x1C 
     int ModelAssetCutScene; //0x20 
     int ModelAssetUI; //0x24 
     int Priority; //0x28 
};
BRDropPlaneSkinFields* dropplaneFields;
static char searchQuery[256] = "";

bool loadskinhack = false;
bool loadCharacter = false;
bool isLoad = false;

std::string lastKnownName = "";
int emptyNameCount = 0;
int g_selectedTargetCharIndex = 0;
void* lastKnownLocId = nullptr;

struct itemInfo {
    std::string itemName;
    std::array<int, 4> Item2Inventory;
    std::array<int, 5> WeaponConf;
    std::array<int, 5> WeaponExtra;
    std::array<int, 3> WeaponAsset;
    // Fire-effect attachment assets used by the original SkinHax path.
    std::array<int, 4> WeaponAttach;
    // Per-attachment-type mythic appearances supplied by the updated attachment engine.
    std::array<int, 31> MythicAttach;
    std::array<int, 3> ItemResInt;
    std::array<void *, 4> ItemRes;
};
std::vector<itemInfo> itemData;

std::unordered_map<std::string, int> nameCountMap;
std::unordered_map<std::string, int> nameCountChar;

struct charInfo {
    std::string charName;
    std::array<int, 12> charModel;
    std::array<int, 3> charRes;
    std::array<void *, 4> charRes2;
    std::array<int, 8> charRole;
    std::array<void *, 1> charRole2;
    std::array<int, 6> charPack;
};

std::vector<charInfo> charData;

struct TargetChar {
    std::string name;
    int traitor1p;
    int traitor3p;
    int itemID;
    int roleID;
    int rolepackID;
};

std::vector<TargetChar> g_targetCharacters;

struct watcher {
    std::string watchname;
    int watchvalue;
};
std::vector<watcher> watch;

struct deadbox {
    std::string deadname;
    std::array<int, 6> dead;
};
std::vector<deadbox> deadboxF;

struct planeID {
    std::string planename;
    std::array<int, 6> plane;
};
std::vector<planeID> dropplane;

uintptr_t location = 0;

uintptr_t getRealOffset(uintptr_t offset) {
    while (location <= 0) {
        location = Tools::GetBaseAddress("libunity.so");
        if (location <= 0) {
            usleep(1000); 
        }
    }
    return location + offset;
}

std::string GetNameString(uintptr_t off, void *getadd) {
    auto getC = (String *(*)(void *)) (getRealOffset(off));
    if (Tools::IsPtrValid((void *) getC)) {
    auto getV = getC(getadd);
    if (!getV || !Tools::IsPtrValid((void*)getV)) return "";
    const char* text = getV->CString();
    return text ? std::string(text) : std::string();
    }
    return "";
}

std::string ToLower(std::string str) {
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    return str;
}

std::string GetBaseWeaponName(const std::string& fullName)
{
    size_t dashPos = fullName.find(" - ");
    if (dashPos == std::string::npos)
        return fullName;
    std::string base = fullName.substr(0, dashPos);
    size_t tagEnd = base.find("] ");
    if (tagEnd != std::string::npos && tagEnd + 2 < base.length())
        base = base.substr(tagEnd + 2);
    return base;
}

void KeyboardWindow(char* s, size_t capacity, bool h) {
    static bool useLowercase = true;
    if (!h || !s || capacity == 0) return;

    auto appendTextSafe = [&](const char* text) {
        const size_t len = strlen(s);
        const size_t add = strlen(text);
        if (len + add + 1 <= capacity) {
            strncat(s, text, capacity - len - 1);
        }
    };

    // Compact layout so every row stays visible on landscape phones.
    const ImVec2 contentSize = ImGui::GetContentRegionAvail();
    const float padding = 3.0f;
    const float buttonHeight = 25.0f;
    const float buttonWidth = (contentSize.x - (padding * 9.0f)) / 10.0f;

    const std::vector<std::vector<char>> keyboardLayoutLowercase = {
        {'1', '2', '3', '4', '5', '6', '7', '8', '9', '0'},
        {'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p'},
        {'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l'},
        {'z', 'x', 'c', 'v', 'b', 'n', 'm'},
    };

    const std::vector<std::vector<char>> keyboardLayoutUppercase = {
        {'1', '2', '3', '4', '5', '6', '7', '8', '9', '0'},
        {'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P'},
        {'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L'},
        {'Z', 'X', 'C', 'V', 'B', 'N', 'M'},
    };

    const std::vector<std::vector<char>>& keyboardLayout =
        useLowercase ? keyboardLayoutLowercase : keyboardLayoutUppercase;

    for (const auto& row : keyboardLayout) {
        for (size_t keyIndex = 0; keyIndex < row.size(); ++keyIndex) {
            const char c = row[keyIndex];
            char label[2] = { c, '\0' };
            ImGui::PushID((int)(c + keyIndex * 97));
            if (ImGui::Button(label, ImVec2(buttonWidth, buttonHeight))) {
                char add[2] = { c, '\0' };
                appendTextSafe(add);
            }
            ImGui::PopID();
            if (keyIndex + 1 < row.size()) ImGui::SameLine(0, padding);
        }
    }

    // Keep rarity shortcuts and the two most-used actions on one compact row.
    const char* tags[] = { "[M]", "[L]", "[E]", "[C]" };
    for (int t = 0; t < 4; ++t) {
        if (ImGui::Button(tags[t], ImVec2(buttonWidth, buttonHeight))) appendTextSafe(tags[t]);
        ImGui::SameLine(0, padding);
    }

    if (ImGui::Button(DEOBF("SPACE"), ImVec2(buttonWidth * 3.0f + padding * 2.0f, buttonHeight))) {
        appendTextSafe(" ");
    }
    ImGui::SameLine(0, padding);

    if (ImGui::Button(useLowercase ? DEOBF("SHIFT") : DEOBF("shift"),
                      ImVec2(buttonWidth * 2.0f + padding, buttonHeight))) {
        useLowercase = !useLowercase;
    }
}


// Floating keyboard used by search pages.  It is intentionally detached from the
// tab child region so the operator/skin list is never squeezed or clipped and
// every keyboard row remains visible on 16:9 / 20:9 landscape displays.
void DrawDetachedSearchKeyboard(const char* windowId, const char* title, char* buffer,
                                size_t capacity, bool* openState) {
    if (!openState || !*openState || !buffer || capacity == 0) return;

    ImGuiIO& kbIO = ImGui::GetIO();
    float kbW = kbIO.DisplaySize.x * 0.60f;
    if (kbW < 700.0f) kbW = 700.0f;
    if (kbW > 920.0f) kbW = 920.0f;
    if (kbW > kbIO.DisplaySize.x - 20.0f) kbW = kbIO.DisplaySize.x - 20.0f;

    // Compact enough to display every key row, while still being detached.
    float kbH = 238.0f;
    if (kbH > kbIO.DisplaySize.y - 16.0f) kbH = kbIO.DisplaySize.y - 16.0f;

    const ImVec2 kbPos((kbIO.DisplaySize.x - kbW) * 0.5f,
                       kbIO.DisplaySize.y - kbH - 8.0f);
    ImGui::SetNextWindowPos(kbPos, ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(kbW, kbH), ImGuiCond_Always);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 10.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.0f, 8.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(3.0f, 3.0f));
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.030f, 0.028f, 0.038f, 0.990f));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.40f, 0.20f, 0.58f, 1.0f));

    const ImGuiWindowFlags kbFlags = ImGuiWindowFlags_NoTitleBar |
                                     ImGuiWindowFlags_NoResize |
                                     ImGuiWindowFlags_NoCollapse |
                                     ImGuiWindowFlags_NoSavedSettings |
                                     ImGuiWindowFlags_NoScrollbar |
                                     ImGuiWindowFlags_NoScrollWithMouse;

    if (ImGui::Begin(windowId, openState, kbFlags)) {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.72f, 0.43f, 0.95f, 1.0f));
        ImGui::TextUnformatted(title ? title : DEOBF("SEARCH KEYBOARD"));
        ImGui::PopStyleColor();

        // DELETE / CLEAR are kept in the fixed header so they can never disappear
        // below the phone's visible area.
        const float actionW = 72.0f;
        const float gap = 4.0f;
        const float totalActionsW = actionW * 3.0f + gap * 2.0f;
        ImGui::SameLine();
        ImGui::SetCursorPosX(ImGui::GetWindowWidth() - totalActionsW - 10.0f);

        if (ImGui::Button(DEOBF("DELETE"), ImVec2(actionW, 25.0f))) {
            const size_t len = strlen(buffer);
            if (len > 0) buffer[len - 1] = '\0';
        }
        ImGui::SameLine(0, gap);
        if (ImGui::Button(DEOBF("CLEAR"), ImVec2(actionW, 25.0f))) {
            buffer[0] = '\0';
        }
        ImGui::SameLine(0, gap);
        if (ImGui::Button(DEOBF("HIDE"), ImVec2(actionW, 25.0f))) {
            *openState = false;
        }

        ImGui::Separator();
        KeyboardWindow(buffer, capacity, true);
    }
    ImGui::End();

    ImGui::PopStyleColor(2);
    ImGui::PopStyleVar(4);
}
