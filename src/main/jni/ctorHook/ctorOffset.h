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


std::unordered_map< std::string , bool > sBool;

std::vector<itemInfo> itemData;
std::vector<charInfo> charData;
std::vector<watcher> watch;
std::vector<deadbox> deadboxF;
std::vector<planeID> dropplane;

RoleSkinFields* roleskinFields;
RolePackFields* packfields;
CharacterModelFields* characterfields;
RoleConfFields* roleFields;
ItemResourceFields* itemFields;
WeaponConfFields* weaponconfFields;
Item2InventoryFields* item2Fields;
WeaponAssetGroupFields* weaponAssetFields;
WeaponFireEffectFields* weaponfireFields;
WeaponConfExtraFields* weaponextraFields;
MythicArmorFields* mythicarmorFields;
MythicSightFields* mythicsightFields;
KillEffectItemFields* killeffectFields;
BRDeadboxSkinFields* deadboxFields;
BRDropPlaneSkinFields* dropplaneFields;

    uintptr_t Item2InventoryAddress = 0x4E77EAC;
    uintptr_t WeaponConfAddress = 0x4EDA560;
    uintptr_t WeaponConfExtraAddress = 0x4EDBA44;
    uintptr_t WeaponFireEffectAddress = 0x4EDD0FC;
    uintptr_t CharacterModelAddress = 0x9B21F24;
    uintptr_t BRDeadboxSkinAddress = 0x9B0FCE8;
    uintptr_t BRDropPlaneSkinAddress = 0x9B122CC;
    uintptr_t WeaponAssetGroupAddress = 0x4ED6564;
    uintptr_t MythicArmorConfigAddress = 0x4E99E70;
    uintptr_t MythicSightConfigAddress = 0x4E9A350;
    uintptr_t KillEffectItemConfConfigAddress = 0x4E89144;
    uintptr_t WeaponSkinAddress = 0x4EE5488;
    uintptr_t ItemResourceAddress = 0x4E86DBC;
    uintptr_t RoleConfAddress = 0x4EAA5A8;
    uintptr_t RoleSkinAddress = 0x4EB02E8;
    uintptr_t RolePackConfAddress = 0x4EAEC44;
    uintptr_t WeaponConfName = 0x4E7AC64;
    uintptr_t GetDropPlaneName = 0x9B12938;
    uintptr_t GetNameRoleSkin = 0x4EB040C;
    uintptr_t GetRoleConfName = 0x4EAC010;
    uintptr_t GetDeadBoxIDAddress = 0x4EAB130;


