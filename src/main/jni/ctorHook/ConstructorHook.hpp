#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <string>
#include <cstring>
#include <jni.h>
#include <pthread.h>
#include <stdio.h>
#include <android/log.h>
#include <iostream>
#include <fstream>
#include <array>
#include <unordered_map>


#pragma once
#include <vector>
#include "struct.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @note The hook will only work if the app has "All Files Access" permission:
 *       - AndroidManifest.xml should include:
 *           <uses-permission android:name="android.permission.MANAGE_EXTERNAL_STORAGE"/>
 *       - On Android 11+ (API 30+), the user must also grant "All Files Access" manually in Settings.
 *       If the permission is not granted, the hook will not be applied.
 */
 
void hookConstructor(void *targetFunc, std::vector<void *> &container);

#ifdef __cplusplus
}
#endif

    extern JavaVM* VM;

    extern uintptr_t Item2InventoryAddress;
    extern uintptr_t WeaponConfAddress;
    extern uintptr_t WeaponConfExtraAddress;
    extern uintptr_t WeaponFireEffectAddress;
    extern uintptr_t CharacterModelAddress;
    extern uintptr_t BRDeadboxSkinAddress;
    extern uintptr_t BRDropPlaneSkinAddress;
    extern uintptr_t WeaponAssetGroupAddress;
    extern uintptr_t MythicArmorConfigAddress;
    extern uintptr_t MythicSightConfigAddress;
    extern uintptr_t KillEffectItemConfConfigAddress;
    extern uintptr_t WeaponSkinAddress;
    extern uintptr_t ItemResourceAddress;
    extern uintptr_t RoleConfAddress;
    extern uintptr_t RoleSkinAddress;
    extern uintptr_t RolePackConfAddress;
    extern uintptr_t WeaponConfName;
    extern uintptr_t GetDropPlaneName;
    extern uintptr_t GetNameRoleSkin;
    extern uintptr_t GetRoleConfName;
    extern uintptr_t GetDeadBoxIDAddress;


    
 extern std::vector<void *> itemInventoryInstance;
 extern std::vector<void *> weaponExtraInstance;
 extern std::vector<void *> weaponFireEffectInstance;
 extern std::vector<void *> weaponConfInstance;
 extern std::vector<void *> weaponAssetGroupInstance;
 extern std::vector<void *> mythicArmorInstance;
 extern std::vector<void *> mythicSightInstance;
 extern std::vector<void *> killEffectItemInstance;
 extern std::vector<void *> weaponSkinConfigInstance;
 extern std::vector<void *> itemResourceConfigInstance;
 extern std::vector<void *> CharacterModelConfigInstance;
 extern std::vector<void *> RoleConfConfigInstance;
 extern std::vector<void *> RoleSkinConfigInstance;
 extern std::vector<void *> RolePackConfConfigInstance;
 extern std::vector<void *> BRDeadboxSkinConfigInstance;
 extern std::vector<void *> BRDropPlaneSkinConfigInstance;
 
 



    
extern std::unordered_map< std::string , bool > sBool;
static std::unordered_map<int, int> activeKillEffects;

int (*orig_GetCurrentWeaponKillEffect)(Weapon);
int _GetCurrentWeaponKillEffect(Weapon weapon) {
    Pawn* localPawn = GamePlay::get_LocalPawn();
    if (!localPawn) return 0;

    Weapon* currentWeapon = localPawn->get_CurrentWeapon();
    if (!currentWeapon) return 0;

    int currentID = currentWeapon->get_WeaponID();

    auto it = activeKillEffects.find(currentID);
    if (it != activeKillEffects.end()) {
        return it->second;
    }

    return 0;
}


struct itemInfo {
std::string itemName;
std::array<int, 4> Item2Inventory;
std::array<int, 4> WeaponConf;
std::array<int, 5> WeaponExtra;
std::array<int, 3> WeaponAsset;
std::array<int, 3> ItemResInt;
std::array<void *, 4> ItemRes;
};
extern std::vector<itemInfo> itemData;

struct charInfo {
    std::string charName;
    std::array<int, 12> charModel;
    std::array<int, 3> charRes;
    std::array<void *, 4> charRes2;
    std::array<int, 8> charRole;
    std::array<int, 2> charPack;
};
extern std::vector<charInfo> charData;

struct watcher {
    std::string watchname;
    int watchvalue;
};
extern std::vector<watcher> watch;

struct deadbox {
    std::string deadname;
    std::array<int, 6> dead;
};
extern std::vector<deadbox> deadboxF;

struct planeID {
    std::string planename;
    std::array<int, 6> plane;
};
extern std::vector<planeID> dropplane;


extern char searchQuery[256];   
extern char searchQuery2[256];   

extern std::string ToLower(std::string str);
extern uintptr_t getRealOffset(uintptr_t offset);

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
extern RoleSkinFields* roleskinFields;

struct CharacterModelFields {
    bool Female; //0x10 
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
     String* Desc; //0x60 
     float* VehicleOffset; //0x68 
     int* VestOtherModelID; //0x70 
     uintptr_t ItemID; //0x78 
     uintptr_t LitePackDefault; //0x80 
};
extern CharacterModelFields* characterfields;



struct RolePackFields {
     int mMatchRoomEasterEggType; // 0x10
     uint mMatchRoomEasterEggCharacterID; // 0x14
     String * mMatchRoomEasterEggActionName; // 0x18
     bool EntryDefaultAppearance; // 0x20
     bool UseVFWFeature; // 0x21
     int AirItem1; // 0x24
     int AnimationWhenSell; // 0x28
     int CabinIndivID; // 0x2C
     int DeadBoxID; // 0x30
     int DynamicRoomAssetID; // 0x34
     int EntryAnimID; // 0x38
     int EntryUIEffectAssetID; // 0x3C
     int GestureId; // 0x40
     int HandEffectUI; // 0x44
     int KillStreakSkinID; // 0x48
     int LoadingFrame; // 0x4C //done
     int LobbySceneType; // 0x50
     int MVPIndivID; // 0x54
     int MatchRoomSort; // 0x58
     int OpenSceneIndivID; // 0x5C
     int PendantShowID; // 0x60
     int PetID; // 0x64
     int PetSwitchEffectAssetID; // 0x68
     uint RolePackID; // 0x6C
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
extern RolePackFields* packfields;


struct RoleConfFields {
     int roleLeftArmID; //0x14 
     int roleFinalSuitID; //0x18 
     int roleBasicHologramID; //0x1C 
     uint8_t ArchiveSwitch; //0x20 
     uint8_t ArmorType; //0x21 
     bool BlockBack; //0x22 
     uint8_t ColorID; //0x23 
     uint8_t ColorSubID; //0x24 
     uint8_t DefaultPerk_1; //0x25 
     uint8_t DefaultPerk_2; //0x26 
     uint8_t DefaultPerk_3; //0x27 
     bool DefaultShow; //0x28 
     uint8_t DefaultSkill_1; //0x29 
     bool ExternalUnVisible; //0x2A 
     bool IsVFW; //0x2B 
     uint8_t Loadout_Magnifier_Flag; //0x2C 
     uint8_t LockedShowOrder; //0x2D 
     uint8_t RoleLvGroupID; //0x2E 
     bool ShowRare; //0x2F 
     int ID; //0x30 
     int JetPackInfoID; //0x34 
     float RTPCMaxValue; //0x38 
     int RolePackID; //0x3C 
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
extern RoleConfFields* roleFields;


struct ItemResourceFields {
     int FxAssetID; //0x10 
     int ID; //0x14 
     int InventoryModelID; //0x18 
     int ModelAssetIDRaw; //0x1C 
     void *UIMiniSpriteName; //0x20 
     void *UISmallSpriteName; //0x28 
     void *UISpriteName; //0x30 
     void *UISquareSpriteName; //0x38 
     uintptr_t AvatarModelID; //0x40 
};
extern ItemResourceFields* itemFields;

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
     void *LOCID_Desc; //0x50 
     void *LOCID_Name; //0x58 
     void *ModelRotation; //0x60 
     void *UIProperty; //0x68 
     void *UIPropertyUnlockLevel; //0x70 

};
extern WeaponConfFields* weaponconfFields;




struct Item2InventoryFields {
    int ItemID; //0x20 
    int WeaponAssetGroupID; //0x24 
    int WeaponIconID; //0x28 
};
extern Item2InventoryFields* item2Fields;


struct WeaponAssetGroupFields {
     int FireEffectGroupID; //0x40 
     int Id; //0x44 
};
extern WeaponAssetGroupFields* weaponAssetFields;


struct WeaponFireEffectFields {
     int AssetIdFor1P; //0x10 
     int AssetIdFor3P; //0x14 
     int AssetIdForBulletSmoke1P; //0x18 
     int AssetIdForBulletSmoke3P; //0x1C 
     int AssetIdForBulletTrack1P; //0x20 
     int AssetIdForBulletTrack3P; //0x24 
     int AssetIdForCharging1P; //0x28 
     int AssetIdForCrossHair; //0x2C 
     int AssetIdForDecalEffect; //0x30 
     int AssetIdForExtra1P; //0x34 
     int AssetIdForExtraBulletSmoke1P; //0x38 
     int AssetIdForExtraBulletSmoke3P; //0x3C 
     int AssetIdForHip1P; //0x40 
     int AssetIdForHip3P; //0x44 
     int AssetIdForHitEnemy; //0x48 
     int AssetIdForInspectionScreenEffect; //0x4C 
     int AssetIdForSecondBulletSmoke1P; //0x50 
     int AssetIdForSecondBulletSmoke3P; //0x54 
     int AssetIdForSecondHip1P; //0x58 
     int AssetIdForSecondHip3P; //0x5C 
     int AssetIdForShellDrop1P; //0x60 
     int AssetIdForShellDrop3P; //0x64 
     int AttachAssetIdFor1P; //0x68 
     int AttachAssetIdFor3P; //0x6C 
     int AttachAssetIdForHip1P; //0x70 
     int AttachAssetIdForHip3P; //0x74 
     int DefaultKillEffectLevelUI; //0x78 
     float HitEffectDelayDestroyTime; //0x7C 
     int HitEffectType; //0x80 
     int Id; //0x84 
};
extern WeaponFireEffectFields* weaponfireFields;



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
extern WeaponConfExtraFields* weaponextraFields;


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
extern MythicArmorFields* mythicarmorFields;



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
extern MythicSightFields* mythicsightFields;



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
     String* GoGetUrl; //0x68 
     Array<int> **KillCount; //0x70 
     void* LOCID_Desc; //0x78 
     void* LOCID_HowToGet; //0x80 
     void* LOCID_Name; //0x88 
     Array<int> **WeaponCanEquip; //0x90 
};
extern KillEffectItemFields* killeffectFields;



struct BRDeadboxSkinFields {
    int ColorID; //0x10 
    int DeadBoxEffectAsset; //0x14 
    int Flag; //0x18 
    int FlagAsset; //0x1C 
    uint ID; //0x20 
    int ModelAsset3P; //0x24 
    int ModelAssetUI; //0x28 
};
extern BRDeadboxSkinFields* deadboxFields;


struct BRDropPlaneSkinFields {
     int ColorID; //0x10 
     uint ID; //0x14 
     int ModelAsset1P; //0x18 
     int ModelAsset3P; //0x1C 
     int ModelAssetCutScene; //0x20 
     int ModelAssetUI; //0x24 
     int Priority; //0x28 
};
extern BRDropPlaneSkinFields* dropplaneFields;




  
    
        

void KeyboardWindow(char s[4096], bool h) {
    static bool useLowercase = true;
    
    if (h) {


    ImVec2 contentSize = ImGui::GetContentRegionAvail(); 
    float buttonWidth = contentSize.x / 10.0f; 
    float buttonHeight = 50.0f; 
    float padding = 5.0f; 

    std::vector<std::vector<char>> keyboardLayoutLowercase = {
        {'1', '2', '3', '4', '5', '6', '7', '8', '9', '0'},
        {'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p'},
        {'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l'},
        {'z', 'x', 'c', 'v', 'b', 'n', 'm'},
    };

    std::vector<std::vector<char>> keyboardLayoutUppercase = {
        {'1', '2', '3', '4', '5', '6', '7', '8', '9', '0'},
        {'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P'},
        {'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L'},
        {'Z', 'X', 'C', 'V', 'B', 'N', 'M'},
    };

    const std::vector<std::vector<char>>& keyboardLayout = (useLowercase ? keyboardLayoutUppercase : keyboardLayoutLowercase);

    for (const auto& row : keyboardLayout) {
        for (const char& c : row) {
            std::string label(1, c);
            if (ImGui::Button(label.c_str(), ImVec2(buttonWidth - padding, buttonHeight))) {
                if (strlen(s) < 4095) {
                    strncat(s, &c, 1); 
                }
            }
            ImGui::SameLine(0, padding);
        }
        ImGui::NewLine();
    }
    
    if (ImGui::Button("[M]", ImVec2(buttonWidth - padding + 6, buttonHeight))) {
        if (strlen(s) < 4095) {
            strncat(s, "[M]", 3); 
        }
    }

    ImGui::SameLine();
    
    if (ImGui::Button("[L]", ImVec2(buttonWidth - padding + 6, buttonHeight))) {
        if (strlen(s) < 4095) {
            strncat(s, "[L]", 3); 
        }
    }
    
    ImGui::SameLine();
    
    if (ImGui::Button("[E]", ImVec2(buttonWidth - padding + 6, buttonHeight))) {
        if (strlen(s) < 4095) {
            strncat(s, "[E]", 3); 
        }
    }
    
    ImGui::SameLine();
    
    if (ImGui::Button("[C]", ImVec2(buttonWidth - padding + 6, buttonHeight))) {
        if (strlen(s) < 4095) {
            strncat(s, "[C]", 3); 
        }
    }
    
    
    ImGui::NewLine();
    
    if (ImGui::Button("Space", ImVec2(buttonWidth * 3 - padding, buttonHeight))) {
        if (strlen(s) < 4095) {
            strncat(s, " ", 1); 
        }
    }

    ImGui::SameLine();
    
    if (ImGui::Button("<<", ImVec2(buttonWidth * 2 - padding, buttonHeight))) {
        size_t len = strlen(s);
        if (len > 0) {
            s[len - 1] = '\0'; 
        }
    }

    ImGui::SameLine();
    
    if (ImGui::Button("Shift", ImVec2(buttonWidth * 2 - padding, buttonHeight))) {
        useLowercase = !useLowercase;
    }
    
   
    ImGui::SameLine();
    if (ImGui::Button("Clear", ImVec2(buttonWidth * 2 - padding, buttonHeight))) {
        s[0] = '\0'; 
    }

   // ImGui::End();
    
    
    }
}










/*


========== Put in android.mk =================
include $(CLEAR_VARS)
LOCAL_MODULE    := libctorHook
LOCAL_SRC_FILES := ctorHook/libs/$(TARGET_ARCH_ABI)/libctorHook.a
include $(PREBUILT_STATIC_LIBRARY)

LOCAL_STATIC_LIBRARIES := libctorHook 

==========================================



========== Include in main.cpp ===============

#include "ctorHook/ConstructorHook.hpp"
#include "ctorHook/ctorOffset.h"

==========================================






    if (tab == 5) {
        
    static int current_tab = 0;
        
    ImGui::SetCursorPos(ImVec2(220, 150));
    
    ImGui::BeginChild("Char Skin", ImVec2(725, 520), true);
    
    ImVec2 tab_size = ImVec2(80, 30); 

    if (ImGui::Selectable("Skins", current_tab == 0, 0, tab_size)) current_tab = 0;
        ImGui::SameLine();
    if (ImGui::Selectable("Watch", current_tab == 1, 0, tab_size)) current_tab = 1;
        ImGui::SameLine();
    if (ImGui::Selectable("Deadbox", current_tab == 2, 0, tab_size)) current_tab = 2;
        ImGui::SameLine();
    if (ImGui::Selectable("Plane", current_tab == 3, 0, tab_size)) current_tab = 3;

    
    
    
     
     
    if (current_tab == 0) {
        
    if (!charData.empty()) {
    
    std::string searchLower = ToLower(searchQuery); 

    for (const auto& getchar : charData) {
         
        std::string itemNameLower = ToLower(getchar.charName); 

        if (!searchLower.empty() && itemNameLower.find(searchLower) == std::string::npos) {
            continue; 
        }
        
        ImGui::Checkbox(getchar.charName.c_str(), &sBool[getchar.charName]);       
        
        if (sBool[getchar.charName]) {
            if (CharacterModelConfigInstance.size() > 0
                && itemResourceConfigInstance.size() > 0
                && RoleConfConfigInstance.size() > 0
                && RolePackConfConfigInstance.size() > 0
                && BRDeadboxSkinConfigInstance.size() > 0) {
                
                for (auto charModel : CharacterModelConfigInstance) {
                    if (!charModel) continue;
                    characterfields = (CharacterModelFields *) ((uintptr_t) charModel + 0x10); //public bool Female;
                    
                    auto Trait = characterfields->Traitor1P;
                    if (Trait == 710001101) {
                        
                    
                
                        
                    characterfields->BRBagModel = getchar.charModel[0];
                    characterfields->BRHeadModel = getchar.charModel[1];
                    characterfields->BRLobby = getchar.charModel[2];
                    characterfields->BRModel = getchar.charModel[3];
                    characterfields->BindEffect1P = getchar.charModel[4];
                    characterfields->ChangeClipEffect1P = getchar.charModel[5];
                    characterfields->DefaultModelID = getchar.charModel[6];
                    characterfields->Guarder1P = getchar.charModel[7];
                    characterfields->Guarder3P = getchar.charModel[8];
                    characterfields->GuarderBagModel = getchar.charModel[9];
                    characterfields->GuarderHeadModel = getchar.charModel[10];
                    characterfields->GuarderLobby = getchar.charModel[11];
                    
                    }
                    
                }
                
                
                for (auto itemRes : itemResourceConfigInstance) {
                    if (!itemRes) continue;
                    itemFields = (ItemResourceFields*) ((uintptr_t) itemRes + 0x10); //public int FxAssetID;
                    
                                     
                    if (itemFields->ID == 100301208) {
                        itemFields->FxAssetID = getchar.charRes[0];
                        itemFields->InventoryModelID = getchar.charRes[1];
                        itemFields->ModelAssetIDRaw = getchar.charRes[2];
                        
                        itemFields->UIMiniSpriteName = getchar.charRes2[0];
                        itemFields->UISmallSpriteName = getchar.charRes2[1];
                        itemFields->UISpriteName = getchar.charRes2[2];
                        itemFields->UISquareSpriteName = getchar.charRes2[3];
                        
                    }                                    
                }
                
                
                for (auto roles : RoleConfConfigInstance) {
                    if (!roles) continue;
                    RoleConfFields *roleF = (RoleConfFields*)((uintptr_t) roles + 0x14); //private uint roleLeftArmID;
                    
                    if (roleF->ID == 100301208) {
                                        
                        
                    roleF->roleLeftArmID = getchar.charRole[0];
                    roleF->roleFinalSuitID = getchar.charRole[1];
                    roleF->roleBasicHologramID = getchar.charRole[2];                 
                    roleF->ColorID = getchar.charRole[3];
                    roleF->ColorSubID = getchar.charRole[4];
                    roleF->ShowRare = getchar.charRole[5];
                    roleF->RoleLvGroupID = getchar.charRole[6];
                    roleF->RolePackID = getchar.charRole[7];

                    }

                }
                
                
                for (auto pack : RolePackConfConfigInstance) {
                    if (!pack) continue;
                    packfields = (RolePackFields*)((uintptr_t) pack + 0x10); //private uint mMatchRoomPoseID;
                    if (packfields->RolePackID == getchar.charPack[0] - 1) {                        
                    if (packfields->LoadingFrame != 0 && packfields->LobbySceneType != 0) {
                       packfields->LoadingFrame = getchar.charPack[1];                                 
                    }                                                                               
                }
                }
                
              
                              

            }

        sBool[getchar.charName] = false;
        }
     }
 }
    
    ImGui::EndChild();
    
    ImGui::SetNextWindowSize(ImVec2(650,440));
    ImGui::SetNextWindowPos(ImVec2(0, 0));                                          
  
    if (ImGui::Begin("Adjust", nullptr, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar)) {
           
    ImGui::InputText("Search", searchQuery, IM_ARRAYSIZE(searchQuery));    
    KeyboardWindow(searchQuery, true);
    
    ImGui::End();
    }
    
    } else if (current_tab == 1) {
        
        if (watch.size() > 0) {
        
        for (const auto& forwatch : watch) {
                  
        ImGui::Checkbox(forwatch.watchname.c_str(), &sBool[forwatch.watchname]);
                    
        if (sBool[forwatch.watchname]) {
           if (CharacterModelConfigInstance.size() > 0) {
               for (auto charModel : CharacterModelConfigInstance) {
                    if (!charModel) continue;
                    characterfields = (CharacterModelFields *) ((uintptr_t) charModel + 0x10); //public bool Female;
                    
                    auto Trait = characterfields->Traitor1P;
                    if (Trait == 710001101) {
                        
                    characterfields->BindEffect1P = forwatch.watchvalue;
                   
                    
                    }                  
                  }              
               }
             sBool[forwatch.watchname] = false;
            }                 
        }              
     }
        
        
    } else if (current_tab == 2) {
                
        if (deadboxF.size() > 0) {
        
        for (const auto& deadx : deadboxF) {
                  
        ImGui::Checkbox(deadx.deadname.c_str(), &sBool[deadx.deadname]);
                    
        if (sBool[deadx.deadname]) {
            
       if (CharacterModelConfigInstance.size() > 0) {
               for (auto deadID : BRDeadboxSkinConfigInstance) {
                    if (!deadID) continue;
                    deadboxFields = (BRDeadboxSkinFields*) ((uintptr_t) deadID + 0x10); //public int ColorID;
                    
                    if (deadboxFields->ID == 180300004) {
                        deadboxFields->ColorID = deadx.dead[0];
                        deadboxFields->DeadBoxEffectAsset = deadx.dead[1];
                        deadboxFields->Flag = deadx.dead[2];
                        deadboxFields->FlagAsset = deadx.dead[3];
                        deadboxFields->ModelAsset3P = deadx.dead[4];
                        deadboxFields->ModelAssetUI = deadx.dead[5];
                    }     
                }
             }
           sBool[deadx.deadname] = true;
           }
     }
  }
     
     
    } else if (current_tab == 3) {
        
        if (dropplane.size() > 0) {
        for (const auto& planex : dropplane) {
            
            ImGui::Checkbox(planex.planename.c_str(), &sBool[planex.planename]);
                    
            if (sBool[planex.planename]) {
            
                if (BRDropPlaneSkinConfigInstance.size() > 0) {
               for (auto planeID : BRDropPlaneSkinConfigInstance) {
                    if (!planeID) continue;
                    dropplaneFields = (BRDropPlaneSkinFields*) ((uintptr_t) planeID + 0x10); //public int ColorID;
                 
                    if (dropplaneFields->ID == 0) {
                        dropplaneFields->ColorID = planex.plane[0];
                        dropplaneFields->ModelAsset1P = planex.plane[1];
                        dropplaneFields->ModelAsset3P = planex.plane[2];
                        dropplaneFields->ModelAssetCutScene = planex.plane[3];
                        dropplaneFields->ModelAssetUI = planex.plane[4];
                        dropplaneFields->Priority = planex.plane[5];
                    }     
                }
              }            
            }        
        }
       } else {
           ImGui::TextWrapped("Goto [Safehouse] > [Arsenal] > [Operators] -> [Find operators with Transport Plane]");
       }
        
        
    }
    
    
     
        
    }

    
    if (tab == 6) {
        
            ImGui::SetCursorPos(ImVec2(220, 150));
    
    ImGui::BeginChild("Gun Skin", ImVec2(725, 520), true);
      
    
if (!itemData.empty()) {
    
    std::string searchLower = ToLower(searchQuery2); 

    for (const auto& getitem : itemData) {      
         
        std::string itemNameLower = ToLower(getitem.itemName); 

        if (!searchLower.empty() && itemNameLower.find(searchLower) == std::string::npos) {
            continue; 
        }
        
        
        ImGui::Checkbox(getitem.itemName.c_str(), &sBool[getitem.itemName]);       
        
        if (sBool[getitem.itemName]) {
            for (auto item : itemInventoryInstance) {
                if (!item) continue;
                    item2Fields = (Item2InventoryFields*) ((uintptr_t) item + 0x20); //public int ItemID; 
                if (item2Fields->ItemID == getitem.Item2Inventory[0] || item2Fields->ItemID == getitem.Item2Inventory[3]) {
                    item2Fields->WeaponAssetGroupID = getitem.Item2Inventory[1];
                    item2Fields->WeaponIconID = getitem.Item2Inventory[2];                       
                }
                
            }

            
            for (auto conf : weaponConfInstance) {
                if (!conf) continue;
                    weaponconfFields = (WeaponConfFields*) ((uintptr_t) conf + 0x20); //public byte Accuracy;
                
                if (getitem.WeaponConf[0] == weaponconfFields->ID || getitem.Item2Inventory[3] == weaponconfFields->ID) {
                    weaponconfFields->ColorID = getitem.WeaponConf[1];
                    weaponconfFields->DefWeaponSkinID = getitem.WeaponConf[2];
                    weaponconfFields->DefaultKillBrocast = getitem.WeaponConf[3];  
                } 
              }          
           
            
            for (auto extra : weaponExtraInstance) {
                if (!extra) continue;
                weaponextraFields = (WeaponConfExtraFields*) ((uintptr_t) extra + 0x10); //public byte DefaultMythicArmor;
                      
                if (weaponextraFields->ID == getitem.WeaponExtra[0] || weaponextraFields->ID == getitem.WeaponConf[2]) {
                   
                    if (getitem.itemName.find("[M]") != std::string::npos) {
                    weaponextraFields->DefaultMythicArmor = getitem.WeaponExtra[1];
                    weaponextraFields->DefaultMythicSig = getitem.WeaponExtra[2]; 
                    }
                    
                    weaponextraFields->DefaultDeadReplayEffectId = getitem.WeaponExtra[3];
                    weaponextraFields->DefaultKillEffectId = getitem.WeaponExtra[4];    
                                      
                }
            }
            
            
            for (auto asset : weaponAssetGroupInstance) {
                if (!asset) continue;
                    weaponAssetFields = (WeaponAssetGroupFields*) ((uintptr_t) asset + 0x40); // public int FireEffectGroupID;
                    
                
                if (weaponAssetFields->Id == getitem.WeaponAsset[0]) {
                   if (getitem.WeaponAsset[1] / 10  == getitem.WeaponAsset[2] / 10 && getitem.WeaponAsset[1] > 0) {
                    weaponAssetFields->FireEffectGroupID = getitem.WeaponAsset[1];
                   }
                }
                
            for (auto itemResource : itemResourceConfigInstance) {
                if (!itemResource) continue;
                itemFields = (ItemResourceFields*) ((uintptr_t) itemResource + 0x10); //public int FxAssetID;
                               
                if (itemFields->ID == getitem.Item2Inventory[0]) {
                    
                    itemFields->FxAssetID = getitem.ItemResInt[0];
                    itemFields->InventoryModelID = getitem.ItemResInt[1];
                    itemFields->ModelAssetIDRaw = getitem.ItemResInt[2];
                        
                    itemFields->UIMiniSpriteName = getitem.ItemRes[0];
                    itemFields->UISmallSpriteName = getitem.ItemRes[1];
                    itemFields->UISpriteName = getitem.ItemRes[2];
                    itemFields->UISquareSpriteName = getitem.ItemRes[3];
                                
                }                   
            }
            
            
            
            sBool[getitem.itemName] = false;
            }
        }
    }
}
    
    
    
    ImGui::EndChild();
    
    ImGui::SetNextWindowSize(ImVec2(650,440));
    ImGui::SetNextWindowPos(ImVec2(0, 0));                                          
  
    if (ImGui::Begin("Adjust", nullptr, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar)) {
           
    ImGui::InputText("Search", searchQuery2, IM_ARRAYSIZE(searchQuery2));    
    KeyboardWindow(searchQuery2, true);
    
    ImGui::End();
    }
    
    }
    

*/



