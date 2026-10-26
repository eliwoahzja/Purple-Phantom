#pragma once

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cctype>
#include <cstring>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "Fields.h"
#include "MythicAttachmentSystem.h"

namespace OblivionSkinHax {

// Current-build RVAs. These are the current equivalents of the constructor/name/effect
// paths used by the original Fields.h SkinHax loader.
static constexpr uintptr_t kItemInventoryCtor = 0x6971D34;
static constexpr uintptr_t kWeaponConfCtor = 0xAF7E7FC;
static constexpr uintptr_t kWeaponExtraCtor = 0xAF80080;
static constexpr uintptr_t kWeaponFireCtor = 0xAF81990;
static constexpr uintptr_t kWeaponAssetCtor = 0xAF7A080;
static constexpr uintptr_t kMythicArmorCtor = 0x5C90E84;
static constexpr uintptr_t kMythicSightCtor = 0x90DDEEC;
static constexpr uintptr_t kKillEffectCtor = 0x90C54CC;
static constexpr uintptr_t kWeaponSkinCtor = 0xAF8B4D4;
static constexpr uintptr_t kItemResourceCtor = 0x6985AB8;
static constexpr uintptr_t kWeaponNameGetter = 0xAF7FDC8;

static constexpr uintptr_t kCurrentKillEffect = 0xA61AC70;
static constexpr uintptr_t kCurrentFireEffect = 0xA61AAA8;
static constexpr uintptr_t kCurrentBulletTrack = 0xA61AB8C;
static constexpr uintptr_t kCurrentBroadcast = 0xA61ADFC;

using Constructor = void (*)(void*);
using EffectGetter = int (*)(void*);

inline std::mutex gInstanceMutex;
inline std::mutex gDatabaseMutex;
inline std::mutex gSelectionMutex;
inline std::mutex gEffectMutex;

inline std::vector<void*> gInventoryInstances;
inline std::vector<void*> gExtraInstances;
inline std::vector<void*> gFireInstances;
inline std::vector<void*> gConfInstances;
inline std::vector<void*> gAssetInstances;
inline std::vector<void*> gArmorInstances;
inline std::vector<void*> gSightInstances;
inline std::vector<void*> gKillInstances;
inline std::vector<void*> gResourceInstances;

inline Constructor gOrigInventory = nullptr;
inline Constructor gOrigExtra = nullptr;
inline Constructor gOrigFire = nullptr;
inline Constructor gOrigConf = nullptr;
inline Constructor gOrigAsset = nullptr;
inline Constructor gOrigArmor = nullptr;
inline Constructor gOrigSight = nullptr;
inline Constructor gOrigKill = nullptr;
inline Constructor gOrigWeaponSkin = nullptr;
inline Constructor gOrigResource = nullptr;

inline EffectGetter gOrigKillEffect = nullptr;
inline EffectGetter gOrigFireEffect = nullptr;
inline EffectGetter gOrigBulletTrack = nullptr;
inline EffectGetter gOrigBroadcast = nullptr;

inline std::unordered_map<int, int> gActiveKillEffects;
inline std::unordered_map<int, int> gActiveFireEffects;
inline std::unordered_map<int, int> gActiveBulletEffects;
inline std::unordered_map<int, int> gActiveBroadcastEffects;

// gInstalled means the capture phase is installed. Runtime-mutating hooks are
// tracked separately so login/logo can stay capture-only.
inline std::atomic<bool> gInstalled{false};
inline std::atomic<bool> gRuntimeInstalled{false};
inline std::atomic<bool> gWorkerStarted{false};
inline std::atomic<bool> gDatabaseReady{false};
inline std::atomic<bool> gScanRequested{false};
inline size_t gLastUiBuildFingerprint = 0;
inline bool gHasSelectedSkin = false;
inline itemInfo gSelectedSkin{};
inline std::string gSelectedName;
inline int gRarityFilter = 0; // 0 all, 1 mythic, 2 legendary, 3 epic, 4 common
inline bool gSearchKeyboard = false;

static bool Valid(void* p) {
    return p && Tools::IsPtrValid(p);
}

static void Capture(std::vector<void*>& out, void* instance) {
    if (!Valid(instance)) return;
    std::lock_guard<std::mutex> lock(gInstanceMutex);
    if (std::find(out.begin(), out.end(), instance) == out.end()) out.push_back(instance);
}

static void HookInventory(void* p) { if (gOrigInventory) gOrigInventory(p); Capture(gInventoryInstances, p); }
static void HookExtra(void* p) { if (gOrigExtra) gOrigExtra(p); Capture(gExtraInstances, p); }
static void HookFire(void* p) { if (gOrigFire) gOrigFire(p); Capture(gFireInstances, p); }
static void HookConf(void* p) { if (gOrigConf) gOrigConf(p); Capture(gConfInstances, p); }
static void HookAsset(void* p) { if (gOrigAsset) gOrigAsset(p); Capture(gAssetInstances, p); }
static void HookArmor(void* p) { if (gOrigArmor) gOrigArmor(p); Capture(gArmorInstances, p); }
static void HookSight(void* p) { if (gOrigSight) gOrigSight(p); Capture(gSightInstances, p); }
static void HookKill(void* p) { if (gOrigKill) gOrigKill(p); Capture(gKillInstances, p); }
static void HookResource(void* p) { if (gOrigResource) gOrigResource(p); Capture(gResourceInstances, p); }
static void HookWeaponSkin(void* p) {
    if (gOrigWeaponSkin) gOrigWeaponSkin(p);
    if (Valid(p)) CollectWeaponSkinConfig(p);
}

struct Snapshot {
    std::vector<void*> inventory;
    std::vector<void*> extra;
    std::vector<void*> fire;
    std::vector<void*> conf;
    std::vector<void*> asset;
    std::vector<void*> armor;
    std::vector<void*> sight;
    std::vector<void*> kill;
    std::vector<void*> resource;
};

static Snapshot GetSnapshot() {
    std::lock_guard<std::mutex> lock(gInstanceMutex);
    return {gInventoryInstances, gExtraInstances, gFireInstances, gConfInstances,
            gAssetInstances, gArmorInstances, gSightInstances, gKillInstances,
            gResourceInstances};
}

static size_t Fingerprint() {
    std::lock_guard<std::mutex> lock(gInstanceMutex);
    size_t v = gInventoryInstances.size();
    v = v * 1315423911u + gExtraInstances.size();
    v = v * 1315423911u + gFireInstances.size();
    v = v * 1315423911u + gConfInstances.size();
    v = v * 1315423911u + gAssetInstances.size();
    v = v * 1315423911u + gArmorInstances.size();
    v = v * 1315423911u + gSightInstances.size();
    v = v * 1315423911u + gKillInstances.size();
    v = v * 1315423911u + gResourceInstances.size();
    v = v * 1315423911u + SnapshotWeaponSkinConfigs().size();
    return v;
}

static const char* RarityPrefix(int colorId) {
    switch (colorId) {
        case 5: return "[M] ";
        case 4: return "[L] ";
        case 3: return "[E] ";
        default: return "[C] ";
    }
}

static int RarityFromName(const std::string& name) {
    if (name.rfind("[M] ", 0) == 0) return 1;
    if (name.rfind("[L] ", 0) == 0) return 2;
    if (name.rfind("[E] ", 0) == 0) return 3;
    return 4;
}

static const char* RarityName(int rarity) {
    switch (rarity) {
        case 1: return "MYTHIC";
        case 2: return "LEGENDARY";
        case 3: return "EPIC";
        default: return "COMMON";
    }
}

static ImVec4 RarityColor(int rarity) {
    switch (rarity) {
        case 1: return ImVec4(0.95f, 0.16f, 0.18f, 1.00f); // Mythic: red
        case 2: return ImVec4(0.98f, 0.78f, 0.12f, 1.00f); // Legendary: yellow
        case 3: return ImVec4(0.66f, 0.31f, 0.92f, 1.00f); // Epic: purple
        case 4: return ImVec4(0.22f, 0.80f, 0.38f, 1.00f); // Common: green
        default: return ImVec4(0.72f, 0.76f, 0.82f, 1.00f);
    }
}

static ImVec4 WithAlpha(ImVec4 c, float a) {
    c.w = a;
    return c;
}

static bool DrawRarityTabBox(const char* label, int rarity, bool selected, const ImVec2& size) {
    const ImVec4 color = RarityColor(rarity);
    ImGui::PushStyleColor(ImGuiCol_Text, color);
    ImGui::PushStyleColor(ImGuiCol_Button, WithAlpha(color, selected ? 0.28f : 0.10f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, WithAlpha(color, 0.24f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, WithAlpha(color, 0.34f));
    ImGui::PushStyleColor(ImGuiCol_Border, WithAlpha(color, selected ? 0.95f : 0.60f));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f);
    const bool pressed = ImGui::Button(label, size);
    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor(5);
    return pressed;
}

static std::string DisplayName(const std::string& name) {
    if (name.size() >= 4 && name[0] == '[' && name[2] == ']') return name.substr(4);
    return name;
}

// The project's arm64 Il2CppArray wrapper declares max_length as int instead of
// pointer-sized. Logical item i is therefore m_Items[i + 1].
static int ArrayLogicalAt(Array<int>* a, int i) {
    if (!Valid(a)) return 0;
    const int len = a->getLength();
    if (i < 0 || i >= len || len > 4096) return 0;
    return a->m_Items[i + 1];
}

static int ArrayLogicalLast(Array<int>* a) {
    if (!Valid(a) || a->getLength() <= 0) return 0;
    return ArrayLogicalAt(a, a->getLength() - 1);
}


static std::vector<int> SelectedAliases(const itemInfo& skin) {
    std::vector<int> ids;
    auto add = [&](int v) {
        if (v <= 0) return;
        if (std::find(ids.begin(), ids.end(), v) == ids.end()) ids.push_back(v);
    };
    add(skin.WeaponConf[0]);
    add(skin.WeaponExtra[0]);
    add(skin.WeaponConf[2]);
    add(skin.Item2Inventory[0]);
    add(skin.Item2Inventory[3]);

    for (void* ws : SnapshotWeaponSkinConfigs()) {
        if (!Valid(ws)) continue;
        auto* f = reinterpret_cast<WeaponSkinFields*>(reinterpret_cast<uintptr_t>(ws) + 0x10);
        if (!Valid(f)) continue;
        const bool match = (int)f->ID == skin.WeaponConf[2] ||
                           (int)f->WeaponID == skin.WeaponConf[0] ||
                           (int)f->BRWeaponID == skin.WeaponConf[0] ||
                           f->BrSkinWeaponID == skin.WeaponConf[0];
        if (!match) continue;
        add((int)f->ID);
        add((int)f->WeaponID);
        add((int)f->BRWeaponID);
        add(f->BrSkinWeaponID);
    }
    return ids;
}

static void MapRuntimeValue(std::unordered_map<int, int>& map,
                            const std::vector<int>& ids, int value) {
    if (value <= 0) return;
    for (int id : ids) if (id > 0) map[id] = value;
}

static bool BuildDatabase() {
    const Snapshot s = GetSnapshot();
    if (s.conf.empty() || s.inventory.empty()) return false;

    std::unordered_map<int, void*> inventoryById;
    std::unordered_map<int, void*> extraById;
    std::unordered_map<int, void*> assetById;
    std::unordered_map<int, void*> fireById;
    std::unordered_map<int, void*> resourceById;
    std::unordered_map<int, int> armorByIcon;
    std::unordered_map<int, int> sightByIcon;
    std::unordered_map<int, int> killByIcon;

    for (void* v : s.inventory) {
        if (!Valid(v)) continue;
        auto* f = reinterpret_cast<Item2InventoryFields*>(reinterpret_cast<uintptr_t>(v) + 0x20);
        if (Valid(f) && f->ItemID > 0) inventoryById[f->ItemID] = v;
    }
    for (void* v : s.extra) {
        if (!Valid(v)) continue;
        auto* f = reinterpret_cast<WeaponConfExtraFields*>(reinterpret_cast<uintptr_t>(v) + 0x10);
        if (Valid(f) && f->ID > 0) extraById[(int)f->ID] = v;
    }
    for (void* v : s.asset) {
        if (!Valid(v)) continue;
        auto* f = reinterpret_cast<WeaponAssetGroupFields*>(reinterpret_cast<uintptr_t>(v) + 0x40);
        if (Valid(f) && f->Id > 0) assetById[f->Id] = v;
    }
    for (void* v : s.fire) {
        if (!Valid(v)) continue;
        // The original working loader addresses WeaponFireEffectConfig directly:
        // ID is at instance+0x80. Fields.h is still used for the schema, but the
        // raw offsets below preserve the exact working layout for this config.
        const int id = *reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(v) + 0x80);
        if (id > 0) fireById[id] = v;
    }
    for (void* v : s.resource) {
        if (!Valid(v)) continue;
        auto* f = reinterpret_cast<ItemResourceFields*>(reinterpret_cast<uintptr_t>(v) + 0x10);
        if (Valid(f) && f->ID > 0) resourceById[f->ID] = v;
    }
    for (void* v : s.armor) {
        if (!Valid(v)) continue;
        auto* f = reinterpret_cast<MythicArmorFields*>(reinterpret_cast<uintptr_t>(v) + 0x14);
        if (Valid(f) && f->SecondTab > 0 && f->ThirdTab == 5) armorByIcon[f->SecondTab] = f->AssetID;
    }
    for (void* v : s.sight) {
        if (!Valid(v)) continue;
        Array<int>* equip = *reinterpret_cast<Array<int>**>(reinterpret_cast<uintptr_t>(v) + 0x38);
        const int icon = ArrayLogicalLast(equip);
        if (icon > 0) sightByIcon[icon] = *reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(v) + 0x14);
    }
    for (void* v : s.kill) {
        if (!Valid(v)) continue;
        Array<int>* equip = *reinterpret_cast<Array<int>**>(reinterpret_cast<uintptr_t>(v) + 0x90);
        Array<int>* effects = *reinterpret_cast<Array<int>**>(reinterpret_cast<uintptr_t>(v) + 0x10);
        const int icon = ArrayLogicalLast(equip);
        const int effect = ArrayLogicalLast(effects);
        if (icon > 0 && effect > 0) killByIcon[icon] = effect;
    }

    std::vector<itemInfo> built;
    built.reserve(s.conf.size());
    std::unordered_map<std::string, int> duplicates;
    std::unordered_set<int> seenConfIds;

    for (void* conf : s.conf) {
        if (!Valid(conf)) continue;
        auto* cf = reinterpret_cast<WeaponConfFields*>(reinterpret_cast<uintptr_t>(conf) + 0x20);
        if (!Valid(cf)) continue;
        const int baseId = (int)cf->BaseWeaponId;
        const int confId = (int)cf->ID;
        if (baseId <= 0 || confId <= 0) continue;
        auto ii = inventoryById.find(confId);
        if (ii == inventoryById.end()) continue;

        auto* inv = reinterpret_cast<Item2InventoryFields*>(reinterpret_cast<uintptr_t>(ii->second) + 0x20);
        if (!Valid(inv)) continue;

        std::string realName = GetNameString(kWeaponNameGetter, conf);
        if (realName.empty()) continue; // Never expose a fake numeric skin name.
        // Constructor hooks may see a fresh copy of the same config after lobby/match
        // transitions. Keep one browser row per real WeaponConf ID.
        if (!seenConfIds.insert(confId).second) continue;
        std::string tagged = std::string(RarityPrefix((int)cf->ColorID)) + realName;
        int& dup = duplicates[tagged];
        if (dup++ > 0) tagged += " +" + std::to_string(dup - 1);

        const int assetGroupId = inv->WeaponAssetGroupID;
        const int weaponIconId = inv->WeaponIconID;
        int originalFireId = 0;
        int selectedFireId = 0;
        std::array<int, 4> weaponAttach{};
        auto ai = assetById.find(assetGroupId);
        if (ai != assetById.end()) {
            auto* af = reinterpret_cast<WeaponAssetGroupFields*>(reinterpret_cast<uintptr_t>(ai->second) + 0x40);
            if (Valid(af)) {
                originalFireId = af->FireEffectGroupID;
                selectedFireId = originalFireId;
            }
        }

        if (selectedFireId > 0) {
            auto fi = fireById.find(selectedFireId);
            const bool mythic = cf->ColorID == 5;
            if (mythic && fi != fireById.end()) {
                // Original loader checks instance+0x1C before walking the adjacent
                // Mythic fire-effect rows. Keep that exact data path.
                const int smoke = *reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(fi->second) + 0x1C);
                if (smoke == 0) {
                    for (int d = 1; d <= 10; ++d) {
                        auto candidate = fireById.find(originalFireId + d);
                        if (candidate == fireById.end()) continue;
                        const int nextSmoke = *reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(candidate->second) + 0x1C);
                        if (nextSmoke != 0) {
                            selectedFireId = originalFireId + d;
                            break;
                        }
                    }
                }
            }
            auto selected = fireById.find(selectedFireId);
            if (selected != fireById.end()) {
                const uintptr_t fire = reinterpret_cast<uintptr_t>(selected->second);
                weaponAttach = {
                    *reinterpret_cast<int*>(fire + 0x64),
                    *reinterpret_cast<int*>(fire + 0x68),
                    *reinterpret_cast<int*>(fire + 0x6C),
                    *reinterpret_cast<int*>(fire + 0x70)
                };
            }
        }

        int extraAlias = 0;
        int mythicArmor = 0;
        int mythicSight = 0;
        int deadReplay = 0;
        int killEffect = 0;
        auto baseExtra = extraById.find(baseId);
        if (baseExtra != extraById.end()) extraAlias = baseId;
        auto selectedExtra = extraById.find(confId);
        if (selectedExtra != extraById.end()) {
            auto* ef = reinterpret_cast<WeaponConfExtraFields*>(reinterpret_cast<uintptr_t>(selectedExtra->second) + 0x10);
            if (Valid(ef)) {
                deadReplay = ef->DefaultDeadReplayEffectId;
                killEffect = ef->DefaultKillEffectId;
            }
        }
        if (cf->ColorID == 5) {
            auto a = armorByIcon.find(weaponIconId);
            if (a != armorByIcon.end()) mythicArmor = a->second;
            auto si = sightByIcon.find(weaponIconId);
            if (si != sightByIcon.end()) mythicSight = si->second;
            auto ki = killByIcon.find(weaponIconId);
            if (ki != killByIcon.end()) killEffect = ki->second;
        }

        int fx = 0, invModel = 0, rawModel = 0;
        void* mini = nullptr; void* small = nullptr; void* sprite = nullptr; void* square = nullptr;
        auto ri = resourceById.find(confId);
        if (ri != resourceById.end()) {
            auto* rf = reinterpret_cast<ItemResourceFields*>(reinterpret_cast<uintptr_t>(ri->second) + 0x10);
            if (Valid(rf)) {
                fx = rf->FxAssetID; invModel = rf->InventoryModelID; rawModel = rf->ModelAssetIDRaw;
                mini = rf->UIMiniSpriteName; small = rf->UISmallSpriteName;
                sprite = rf->UISpriteName; square = rf->UISquareSpriteName;
            }
        }

        const bool isMythic = cf->ColorID == 5;
        const std::array<int, 31> mythicAttach = BuildMythicAttach(confId, baseId, isMythic);

        built.push_back({
            tagged,
            {baseId, assetGroupId, weaponIconId, baseId + 200},
            {baseId, (int)cf->ColorID, confId, cf->DefaultKillBrocast, cf->DefWeaponSkinID},
            {extraAlias, mythicArmor, mythicSight, deadReplay, killEffect},
            {assetGroupId, selectedFireId, originalFireId},
            weaponAttach,
            mythicAttach,
            {fx, invModel, rawModel},
            {mini, small, sprite, square}
        });
    }

    std::sort(built.begin(), built.end(), [](const itemInfo& a, const itemInfo& b) {
        return ToLower(a.itemName) < ToLower(b.itemName);
    });

    const bool ready = !built.empty();
    {
        std::lock_guard<std::mutex> lock(gDatabaseMutex);
        itemData.swap(built);
        nameCountMap.clear();
    }
    gDatabaseReady.store(ready, std::memory_order_release);
    return gDatabaseReady.load(std::memory_order_acquire);
}

static void ApplyToCurrentInstances(const itemInfo& skin) {
    const Snapshot s = GetSnapshot();
    const int baseId = skin.WeaponConf[0];
    const int extraId = skin.WeaponExtra[0];
    const int skinId = skin.WeaponConf[2];
    const int itemId = skin.Item2Inventory[0];
    const int lootId = skin.Item2Inventory[3];
    const std::vector<int> aliases = SelectedAliases(skin);
    auto matches = [&](int id) {
        return std::find(aliases.begin(), aliases.end(), id) != aliases.end();
    };

    for (void* v : s.inventory) {
        if (!Valid(v)) continue;
        auto* f = reinterpret_cast<Item2InventoryFields*>(reinterpret_cast<uintptr_t>(v) + 0x20);
        if (!Valid(f)) continue;
        if (f->ItemID == itemId || f->ItemID == lootId || f->ItemID == baseId) {
            f->WeaponAssetGroupID = skin.Item2Inventory[1];
            f->WeaponIconID = skin.Item2Inventory[2];
        }
    }

    void* selectedName = nullptr;
    for (void* v : s.conf) {
        if (!Valid(v)) continue;
        auto* f = reinterpret_cast<WeaponConfFields*>(reinterpret_cast<uintptr_t>(v) + 0x20);
        if (Valid(f) && (int)f->ID == skinId) { selectedName = f->LOCID_Name; break; }
    }
    for (void* v : s.conf) {
        if (!Valid(v)) continue;
        auto* f = reinterpret_cast<WeaponConfFields*>(reinterpret_cast<uintptr_t>(v) + 0x20);
        if (!Valid(f) || !matches((int)f->ID)) continue;
        f->ColorID = (uint8_t)skin.WeaponConf[1];
        f->DefWeaponSkinID = skinId;
        f->DefaultKillBrocast = skin.WeaponConf[3];
        if (selectedName) f->LOCID_Name = selectedName;
    }

    for (void* v : s.extra) {
        if (!Valid(v)) continue;
        auto* f = reinterpret_cast<WeaponConfExtraFields*>(reinterpret_cast<uintptr_t>(v) + 0x10);
        if (!Valid(f) || !matches((int)f->ID)) continue;
        if (skin.WeaponConf[1] == 5) {
            f->DefaultMythicArmor = (uint8_t)skin.WeaponExtra[1];
            f->DefaultMythicSig = (uint8_t)skin.WeaponExtra[2];
        }
        f->DefaultDeadReplayEffectId = skin.WeaponExtra[3];
        f->DefaultKillEffectId = skin.WeaponExtra[4];
    }

    for (void* v : s.asset) {
        if (!Valid(v)) continue;
        auto* f = reinterpret_cast<WeaponAssetGroupFields*>(reinterpret_cast<uintptr_t>(v) + 0x40);
        if (Valid(f) && f->Id == skin.WeaponAsset[0] && skin.WeaponAsset[1] > 0)
            f->FireEffectGroupID = skin.WeaponAsset[1];
    }

    if (skin.WeaponAttach[0] || skin.WeaponAttach[1] || skin.WeaponAttach[2] || skin.WeaponAttach[3]) {
        for (void* v : s.fire) {
            if (!Valid(v)) continue;
            const uintptr_t fire = reinterpret_cast<uintptr_t>(v);
            const int fireId = *reinterpret_cast<int*>(fire + 0x80);
            if (fireId != skin.WeaponAsset[1] && fireId != skin.WeaponAsset[2]) continue;
            *reinterpret_cast<int*>(fire + 0x64) = skin.WeaponAttach[0];
            *reinterpret_cast<int*>(fire + 0x68) = skin.WeaponAttach[1];
            *reinterpret_cast<int*>(fire + 0x6C) = skin.WeaponAttach[2];
            *reinterpret_cast<int*>(fire + 0x70) = skin.WeaponAttach[3];
        }
    }

    for (void* v : s.resource) {
        if (!Valid(v)) continue;
        auto* f = reinterpret_cast<ItemResourceFields*>(reinterpret_cast<uintptr_t>(v) + 0x10);
        if (!Valid(f) || !matches(f->ID)) continue;
        f->FxAssetID = skin.ItemResInt[0];
        f->InventoryModelID = skin.ItemResInt[1];
        f->ModelAssetIDRaw = skin.ItemResInt[2];
        f->UIMiniSpriteName = skin.ItemRes[0];
        f->UISmallSpriteName = skin.ItemRes[1];
        f->UISpriteName = skin.ItemRes[2];
        f->UISquareSpriteName = skin.ItemRes[3];
    }

    {
        std::lock_guard<std::mutex> lock(gEffectMutex);
        gActiveKillEffects.clear();
        gActiveFireEffects.clear();
        gActiveBulletEffects.clear();
        gActiveBroadcastEffects.clear();
        MapRuntimeValue(gActiveKillEffects, aliases, skin.WeaponExtra[4]);
        MapRuntimeValue(gActiveFireEffects, aliases, skin.WeaponAsset[1]);
        // The original Fields/attachment implementation routes the bullet getter through
        // the selected fire-effect group, not a guessed standalone trail ID.
        MapRuntimeValue(gActiveBulletEffects, aliases, skin.WeaponAsset[1]);
        MapRuntimeValue(gActiveBroadcastEffects, aliases, skin.WeaponConf[3]);
    }

    ApplyMythicAttachments({baseId, extraId, skinId, itemId, lootId}, skin.MythicAttach);
}

static void SelectSkin(const itemInfo& skin) {
    {
        std::lock_guard<std::mutex> lock(gSelectionMutex);
        gSelectedSkin = skin;
        gSelectedName = skin.itemName;
        gHasSelectedSkin = true;
    }
    ApplyToCurrentInstances(skin);
}

static int CurrentWeaponId() {
    Pawn* pawn = GamePlay::get_LocalPawn();
    if (!pawn) return 0;
    Weapon* weapon = pawn->get_CurrentWeapon();
    return weapon ? weapon->get_WeaponID() : 0;
}

static int ResolveEffect(void* instance, EffectGetter original,
                         const std::unordered_map<int, int>& values) {
    const int id = CurrentWeaponId();
    std::lock_guard<std::mutex> lock(gEffectMutex);
    auto it = values.find(id);
    if (it != values.end() && it->second > 0) return it->second;
    return original ? original(instance) : 0;
}

static int HookKillEffect(void* p) { return ResolveEffect(p, gOrigKillEffect, gActiveKillEffects); }
static int HookFireEffect(void* p) { return ResolveEffect(p, gOrigFireEffect, gActiveFireEffects); }
static int HookBulletEffect(void* p) { return ResolveEffect(p, gOrigBulletTrack, gActiveBulletEffects); }
static int HookBroadcast(void* p) { return ResolveEffect(p, gOrigBroadcast, gActiveBroadcastEffects); }

static bool InstallOne(uintptr_t base, uintptr_t rva, void* hook, void** orig) {
    void* target = reinterpret_cast<void*>(base + rva);
    if (!Valid(target)) return false;
    DobbyHook(target, hook, orig);
    return true;
}

static void Worker() {
    size_t previousFingerprint = 0;
    size_t lastBuiltFingerprint = 0;
    int stablePasses = 0;

    while (gInstalled.load(std::memory_order_acquire)) {
        std::this_thread::sleep_for(std::chrono::seconds(1));

        // Constructor hooks are capture-only. Do not walk Fields.h objects until
        // the SkinHax UI has explicitly requested a scan. This avoids touching
        // temporary login/loading objects during the game's early boot phase.
        if (!gScanRequested.load(std::memory_order_acquire)) continue;

        const size_t current = Fingerprint();
        if (current == 0) {
            stablePasses = 0;
            previousFingerprint = 0;
            continue;
        }

        if (current == previousFingerprint)
            ++stablePasses;
        else
            stablePasses = 0;
        previousFingerprint = current;

        // One unchanged one-second snapshot is enough after capture-only hooks
        // have already been collecting since startup. The UI also performs one
        // immediate build when it first sees a complete snapshot.
        if (stablePasses < 1 || current == lastBuiltFingerprint) continue;

        if (BuildDatabase())
            lastBuiltFingerprint = current;

        itemInfo selected;
        bool have = false;
        {
            std::lock_guard<std::mutex> lock(gSelectionMutex);
            have = gHasSelectedSkin;
            if (have) selected = gSelectedSkin;
        }
        if (have) ApplyToCurrentInstances(selected);
    }
}

inline bool InstallCaptureOnly() {
    if (gInstalled.exchange(true)) return true;
    const uintptr_t base = Tools::GetBaseAddress("libunity.so");
    if (!base) { gInstalled.store(false); return false; }

    // Capture-only constructor hooks must be installed early or the one-time
    // WeaponConf/Inventory constructors are already gone by the time the user
    // opens the SkinHax page. These hooks call the original first and only retain
    // the config pointer; they do not parse Fields.h or modify game state here.
    InstallOne(base, kItemInventoryCtor, (void*)HookInventory, (void**)&gOrigInventory);
    InstallOne(base, kWeaponConfCtor, (void*)HookConf, (void**)&gOrigConf);
    InstallOne(base, kWeaponExtraCtor, (void*)HookExtra, (void**)&gOrigExtra);
    InstallOne(base, kWeaponFireCtor, (void*)HookFire, (void**)&gOrigFire);
    InstallOne(base, kWeaponAssetCtor, (void*)HookAsset, (void**)&gOrigAsset);
    InstallOne(base, kMythicArmorCtor, (void*)HookArmor, (void**)&gOrigArmor);
    InstallOne(base, kMythicSightCtor, (void*)HookSight, (void**)&gOrigSight);
    InstallOne(base, kKillEffectCtor, (void*)HookKill, (void**)&gOrigKill);
    // One owner only: MythicAttachmentSystem shares this captured vector.
    InstallOne(base, kWeaponSkinCtor, (void*)HookWeaponSkin, (void**)&gOrigWeaponSkin);
    InstallOne(base, kItemResourceCtor, (void*)HookResource, (void**)&gOrigResource);

    // Collect the attachment config constructors early too, but keep every
    // runtime/model mutation hook deferred until SkinHax is actually opened.
    InstallMythicAttachmentCollectorsFresh(base);

    if (!gWorkerStarted.exchange(true)) std::thread(Worker).detach();
    return true;
}

inline bool InstallRuntimeOnly() {
    if (gRuntimeInstalled.exchange(true)) return true;
    if (!InstallCaptureOnly()) {
        gRuntimeInstalled.store(false);
        return false;
    }
    const uintptr_t base = Tools::GetBaseAddress("libunity.so");
    if (!base) { gRuntimeInstalled.store(false); return false; }

    InstallOne(base, kCurrentKillEffect, (void*)HookKillEffect, (void**)&gOrigKillEffect);
    InstallOne(base, kCurrentFireEffect, (void*)HookFireEffect, (void**)&gOrigFireEffect);
    InstallOne(base, kCurrentBulletTrack, (void*)HookBulletEffect, (void**)&gOrigBulletTrack);
    InstallOne(base, kCurrentBroadcast, (void*)HookBroadcast, (void**)&gOrigBroadcast);
    InstallMythicAttachmentRuntimeFresh(base);
    return true;
}

inline bool Install() {
    return InstallCaptureOnly() && InstallRuntimeOnly();
}

inline std::vector<itemInfo> DatabaseSnapshot() {
    std::lock_guard<std::mutex> lock(gDatabaseMutex);
    return itemData;
}

inline void DrawPanel(float scale) {
    // Constructor/config capture should already be armed from the normal hook
    // thread. Keep this fallback for late injection or unusual startup ordering.
    if (!gInstalled.load(std::memory_order_acquire) && !InstallCaptureOnly()) {
        ImGui::TextDisabled("SkinHax waiting for libunity.so...");
        return;
    }

    // Only the runtime-mutating effect/attachment hooks are lazy. This preserves
    // the crash-safe login path without missing the one-shot skin constructors.
    if (!gRuntimeInstalled.load(std::memory_order_acquire) && !InstallRuntimeOnly()) {
        ImGui::TextDisabled("SkinHax runtime hooks are not ready yet...");
        return;
    }
    gScanRequested.store(true, std::memory_order_release);

    // The capture phase has been running since startup, so in the normal lobby
    // path the snapshot is already complete. Build once immediately instead of
    // showing a fake multi-second loading state. If data is still arriving, the
    // worker rebuilds on the next stabilized fingerprint.
    const size_t uiFingerprint = Fingerprint();
    if (!gDatabaseReady.load(std::memory_order_acquire) &&
        uiFingerprint != 0 && uiFingerprint != gLastUiBuildFingerprint) {
        gLastUiBuildFingerprint = uiFingerprint;
        BuildDatabase();
    }

    const std::vector<itemInfo> db = DatabaseSnapshot();
    const float buttonH = std::max(36.0f, 42.0f * scale);
    ImGui::SetNextItemWidth(-145.0f * scale);
    if (ImGui::InputText("##SKINHAX_SEARCH", searchQuery, sizeof(searchQuery))) {}
    if (ImGui::IsItemClicked() || ImGui::IsItemActive()) gSearchKeyboard = true;
    ImGui::SameLine();
    if (ImGui::Button("KEYBOARD", ImVec2(130.0f * scale, buttonH))) gSearchKeyboard = !gSearchKeyboard;

    int rarityCounts[5] = {(int)db.size(), 0, 0, 0, 0};
    for (const auto& skin : db) {
        const int rarity = RarityFromName(skin.itemName);
        if (rarity >= 1 && rarity <= 4) ++rarityCounts[rarity];
    }
    std::array<std::string, 5> tabs = {
        "ALL (" + std::to_string(rarityCounts[0]) + ")",
        "MYTHIC (" + std::to_string(rarityCounts[1]) + ")",
        "LEGENDARY (" + std::to_string(rarityCounts[2]) + ")",
        "EPIC (" + std::to_string(rarityCounts[3]) + ")",
        "COMMON (" + std::to_string(rarityCounts[4]) + ")"
    };
    const float tabW = std::max(80.0f, (ImGui::GetContentRegionAvail().x - 4.0f * 4.0f) / 5.0f);
    for (int i = 0; i < 5; ++i) {
        if (i) ImGui::SameLine(0.0f, 4.0f);
        // Each rarity is its own bordered color box. ALL stays neutral while
        // Mythic/Legendary/Epic/Common use red/yellow/purple/green.
        if (DrawRarityTabBox(tabs[i].c_str(), i, gRarityFilter == i, ImVec2(tabW, buttonH)))
            gRarityFilter = i;
    }

    std::string query = ToLower(std::string(searchQuery));
    std::vector<const itemInfo*> filtered;
    filtered.reserve(db.size());
    for (const auto& skin : db) {
        const int rarity = RarityFromName(skin.itemName);
        if (gRarityFilter != 0 && rarity != gRarityFilter) continue;
        const std::string clean = DisplayName(skin.itemName);
        if (!query.empty() && ToLower(clean).find(query) == std::string::npos) continue;
        filtered.push_back(&skin);
    }

    if (db.empty())
        ImGui::TextDisabled("Loading WeaponConf / inventory skin data...");
    else
        ImGui::TextDisabled("%d named weapon skins loaded | %d shown", (int)db.size(), (int)filtered.size());
    ImGui::Separator();

    const float actionsH = 62.0f * scale;
    if (ImGui::BeginChild("##SKINHAX_LIST", ImVec2(0, -actionsH), true)) {
        for (const itemInfo* skin : filtered) {
            if (!skin) continue;
            const int rarity = RarityFromName(skin->itemName);
            const std::string clean = DisplayName(skin->itemName);
            const bool selected = gSelectedName == skin->itemName;
            const ImVec4 rarityColor = RarityColor(rarity);
            ImGui::PushID(skin->WeaponConf[2]);
            ImGui::PushStyleColor(ImGuiCol_Header, WithAlpha(rarityColor, selected ? 0.26f : 0.10f));
            ImGui::PushStyleColor(ImGuiCol_HeaderHovered, WithAlpha(rarityColor, 0.20f));
            ImGui::PushStyleColor(ImGuiCol_HeaderActive, WithAlpha(rarityColor, 0.30f));
            const bool rowPressed = ImGui::Selectable(
                clean.c_str(), selected, 0, ImVec2(0, std::max(42.0f, 48.0f * scale)));
            const ImVec2 rowMin = ImGui::GetItemRectMin();
            const ImVec2 rowMax = ImGui::GetItemRectMax();
            ImGui::PopStyleColor(3);

            // Explicit border makes every weapon entry a separate rarity box instead
            // of one flat mixed list. The fill stays translucent for readability.
            ImGui::GetWindowDrawList()->AddRect(
                rowMin, rowMax, ImGui::ColorConvertFloat4ToU32(WithAlpha(rarityColor, 0.85f)),
                5.0f, 0, 1.0f);

            if (rowPressed) {
                std::lock_guard<std::mutex> lock(gSelectionMutex);
                gSelectedSkin = *skin;
                gSelectedName = skin->itemName;
            }
            ImGui::SameLine(ImGui::GetWindowWidth() - 155.0f * scale);
            ImGui::TextColored(rarityColor, "%s", RarityName(rarity));
            ImGui::PopID();
        }
    }
    ImGui::EndChild();

    bool canApply = false;
    itemInfo pending;
    {
        std::lock_guard<std::mutex> lock(gSelectionMutex);
        canApply = !gSelectedName.empty();
        if (canApply) pending = gSelectedSkin;
    }
    if (!canApply) ImGui::BeginDisabled();
    if (ImGui::Button("APPLY SELECTED SKIN", ImVec2(-1, buttonH))) SelectSkin(pending);
    if (!canApply) ImGui::EndDisabled();

    DrawDetachedSearchKeyboard("##SKINHAX_KEYBOARD", "SEARCH WEAPON SKIN",
                               searchQuery, sizeof(searchQuery), &gSearchKeyboard);
}

} // namespace OblivionSkinHax
