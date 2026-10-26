#pragma once
#include "FTools/Iconcpp.h"
#include "FTools/Font.h"
#include "FTools/Icon.h"
#include <cmath>
#include <iostream>
#include <malloc.h>
#include <errno.h>
#include <stdarg.h>
#include <cstdio>   // for print

class ESP;                        
static ESP* m_Canvas = nullptr;

char extra[230];
int atas, bawah, kiri, kanan;

int32_t ToColor(float* col) {
    return ImGui::ColorConvertFloat4ToU32(*(ImVec4*)(col));
}

// ✅ Global color defines (macros, no syntax issues ever)
#define RED   0xFF0000FF
#define GREEN 0xFF00FF00
#define BLUE  0xFFFF0000

int totalBots, totalEnemies;
bool isEspReady;



Vector2 pushToScreenBorder(Vector2 Pos, Vector2 screen, int borders, int offset) {
    int x = (int)Pos.x;
    int y = (int)Pos.y;
    if ((borders & 1) == 1) {
        y = 0 - offset;
    }
    if ((borders & 2) == 2) {
        x = (int)screen.x + offset;
    }
    if ((borders & 4) == 4) {
        y = (int)screen.y + offset;
    }
    if ((borders & 8) == 8) {
        x = 0 - offset;
    }
    return Vector2(x, y);
}
int isOutsideSafezone(Vector2 pos, Vector2 screen) {
    Vector2 mSafezoneTopLeft(screen.x * 0.04f, screen.y * 0.04f);
    Vector2 mSafezoneBottomRight(screen.x * 0.96f, screen.y * 0.96f);

    int result = 0;
    if (pos.y < mSafezoneTopLeft.y) {
        result |= 1;
    }
    if (pos.x > mSafezoneBottomRight.x) {
        result |= 2;
    }
    if (pos.y > mSafezoneBottomRight.y) {
        result |= 4;
    }
    if (pos.x < mSafezoneTopLeft.x) {
        result |= 8;
    }
    return result;
}

static inline void ApplySpeedhack() {
    static float lastAppliedScale = 1.0f;
    const float targetScale = (speedHackMultiplier <= 0.0f) ? 1.0f : speedHackMultiplier;
    if (fabsf(targetScale - lastAppliedScale) <= 0.001f) return;

    auto Time_set_timeScale = reinterpret_cast<void (*)(float)>(Class_Time_set_timeScale);
    if (!Time_set_timeScale) return;

    Time_set_timeScale(targetScale);
    lastAppliedScale = targetScale;
}

uintptr_t GetClosestTarget() {
    uintptr_t result = 0;
    const float MAX_AIM_DISTANCE = 20.0f; // 50 meter limit
    float MinDist = MAX_AIM_DISTANCE; // Start with max allowed distance

    auto Gameplay_get_MatchGame = (uintptr_t(*)())(Class_Gameplay__get_MatchGame);
    auto get_MatchGame = Gameplay_get_MatchGame();

    if (Tools::IsPtrValid((void*)get_MatchGame)) {
        auto Gameplay_get_LocalPawn = (uintptr_t(*)())(Class_Gameplay__get_LocalPawn);
        auto LocalPawn = Gameplay_get_LocalPawn();
        
        if (LocalPawn) {
            Vector3 MyPos{0, 0, 0};
            auto local_m_Mesh = *(Transform**)(LocalPawn + Class_Pawn__m_Mesh);
            
            if (local_m_Mesh) {
                MyPos = local_m_Mesh->get_position();

                auto EnemyPawns = *(List<uintptr_t>**)(get_MatchGame + Class_BaseGame__EnemyPawns);
                if (EnemyPawns) {
                    auto Items = EnemyPawns->getItems();
                    if (Items) {
                        for (int i = 0; i < EnemyPawns->getSize(); i++) {
                            auto Pawn = Items[i];
                            if (Pawn) {
                                auto isAlive = *(bool*)((uintptr_t)Pawn + Pawn_m_IsAlive);
                                if (!isAlive)
                                    continue;

                                auto m_Mesh = *(Transform**)(Pawn + Class_Pawn__m_Mesh);
                                if (!m_Mesh)
                                    continue;

                                Vector3 RootPos = m_Mesh->get_position();
                                float Distance = Vector3::Distance(MyPos, RootPos);

                                if (Distance < MinDist && Distance <= MAX_AIM_DISTANCE) {
                                    result = Pawn;
                                    MinDist = Distance;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return result;
}

bool isInsideFOV(int x, int y) {
    if (!Config.Aim.size)
        return true;

    int circle_x = get_width() / 2;
    int circle_y = get_height() / 2;
    int rad = Config.Aim.size;
    return (x - circle_x) * (x - circle_x) + (y - circle_y) * (y - circle_y) <= rad * rad;
}

uintptr_t GetInsideFOVTarget() {
    uintptr_t result = 0;
    float MaxDist = std::numeric_limits<float>::infinity();
    auto Gameplay_get_MatchGame = (uintptr_t( *)())(Class_Gameplay__get_MatchGame);
    auto get_MatchGame = Gameplay_get_MatchGame();
    if (Tools::IsPtrValid((void *)get_MatchGame)) {
        auto Gameplay_get_LocalPawn = (uintptr_t( *)())(Class_Gameplay__get_LocalPawn);
        auto LocalPawn = Gameplay_get_LocalPawn();
        if (LocalPawn) {
            Vector3 MyPos {0, 0, 0};

            auto local_m_Mesh = * (Transform * *)(LocalPawn + Class_Pawn__m_Mesh);
            if (local_m_Mesh) {
                MyPos = local_m_Mesh->get_position();
            }

            auto EnemyPawns = * (List<uintptr_t> * *)(get_MatchGame + Class_BaseGame__EnemyPawns);
            if (EnemyPawns) {
                auto Items = EnemyPawns->getItems();
                if (Items) {
                    for (int i = 0; i < EnemyPawns->getSize(); i++) {
                        auto Pawn = Items[i];
                        if (Pawn) {
                            auto isAlive = * (bool *)((uintptr_t)Pawn + Pawn_m_IsAlive);
                            if (!isAlive)
                                continue;

                            auto m_HeadBone = * (Transform * *)(Pawn + Class_Pawn__m_HeadBone);
                            if (!m_HeadBone)
                                continue;

                            auto HeadSc = WorldToScreenPoint(m_HeadBone->get_position());

                            Vector2 v2Middle = Vector2((float)(get_width() / 2), (float)(get_height() / 2));
                            Vector2 v2Loc = Vector2(HeadSc.x, HeadSc.y);

                            if (isInsideFOV((int)HeadSc.x, (int)HeadSc.y)) {
                                float Distance = Vector2::Distance(v2Middle, v2Loc);

                                if (Distance < MaxDist) {
                                    result = Pawn;
                                    MaxDist = Distance;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return result;
}

void ( * oWeaponFireComponent_Instant_CreateBulletLine)(uintptr_t thiz, Vector3 startPos, Vector3 dir, bool isDualFire);
void WeaponFireComponent_Instant_CreateBulletLine(uintptr_t thiz, Vector3 startPos, Vector3 dir, bool isDualFire) {
    if (Config.Aim.AimSilent) {

        auto Gameplay_get_MatchGame = (uintptr_t( *)())(Class_Gameplay__get_MatchGame);
        auto get_MatchGame = Gameplay_get_MatchGame();
        if (Tools::IsPtrValid((void *)get_MatchGame)) {
            auto Gameplay_get_LocalPawn = (uintptr_t( *)())(Class_Gameplay__get_LocalPawn);
            auto LocalPawn = Gameplay_get_LocalPawn();
            if (LocalPawn) {
                bool bTriggerReady = Config.Aim.Trigger == EAimTrigger::None;
                if (Config.Aim.Trigger == EAimTrigger::Shooting) {
                    auto Pawn_get_IsFiring = (bool ( *)(uintptr_t))(Class_Pawn_get_IsFiring);
                    bTriggerReady = Pawn_get_IsFiring(LocalPawn);
                } else if (Config.Aim.Trigger == EAimTrigger::Scoping) {
                    auto Pawn_IsAiming = (bool ( *)(uintptr_t))(Class_Pawn_IsAiming);
                    bTriggerReady = Pawn_IsAiming(LocalPawn);
                }
                if (bTriggerReady) {
                    uintptr_t Target = 0;
                    if (Config.Aim.By == EAim::Distance) {
                        Target = GetClosestTarget();
                    }
                    if (Config.Aim.By == EAim::Crosshair) {
                        Target = GetInsideFOVTarget();
                    }
                    if (Target) {
                        Vector3 targetPos;
                        if (Config.Aim.Target == EAimTarget::Heads) {
                            auto m_HeadBone = * (Transform * *)(Target + Class_Pawn__m_HeadBone);

                            if (!m_HeadBone)
                                return oWeaponFireComponent_Instant_CreateBulletLine(thiz, startPos, dir, isDualFire);
                            targetPos = m_HeadBone->get_position();
                        }
                        if (Config.Aim.Target == EAimTarget::Chests) {
                            auto m_HeadBone = * (Transform * *)(Target + Class_Pawn__m_HeadBone);
                            if (!m_HeadBone)
                                return oWeaponFireComponent_Instant_CreateBulletLine(thiz, startPos, dir, isDualFire);
                            targetPos = m_HeadBone->get_position();
                            targetPos.y -= 0.2f;
                        }

                        if (Config.Aim.Target == EAimTarget::Body) {
                            auto m_HeadBone = * (Transform * *)(Target + Class_Pawn__m_HeadBone);
                            if (!m_HeadBone)
                                return oWeaponFireComponent_Instant_CreateBulletLine(thiz, startPos, dir, isDualFire);
                            targetPos = m_HeadBone->get_position();
                            targetPos.y -= 0.4f;
                        }

                        auto main = Camera::get_main();
                        if (main) {
                            auto mainView = ((Component *)main)->get_transform();
                            if (mainView) {
                                dir = targetPos - mainView->get_position();
                            }
                        }
                    }
                }
            }
        }
    }
    return oWeaponFireComponent_Instant_CreateBulletLine(thiz, startPos, dir, isDualFire);
}

void ( * oCreateBulletProjectile)(void * thiz, Vector3 startPos, Vector3 dir, void * weaponImpact, int itemID, int flySmokeAssetID, bool enableVirtualStartPos, Vector3 virtualStartPos);
void CreateBulletProjectile(void * thiz, Vector3 startPos, Vector3 dir, void * weaponImpact, int itemID, int flySmokeAssetID, bool enableVirtualStartPos, Vector3 virtualStartPos) {
    if (Config.Aim.AimSilent) {
        auto Gameplay_get_MatchGame = (uintptr_t( *)())(Class_Gameplay__get_MatchGame);
        auto get_MatchGame = Gameplay_get_MatchGame();
        if (Tools::IsPtrValid((void *)get_MatchGame)) {
            auto Gameplay_get_LocalPawn = (uintptr_t( *)())(Class_Gameplay__get_LocalPawn);
            auto LocalPawn = Gameplay_get_LocalPawn();
            if (LocalPawn) {
                bool bTriggerReady = Config.Aim.Trigger == EAimTrigger::None;
                if (Config.Aim.Trigger == EAimTrigger::Shooting) {
                    auto Pawn_get_IsFiring = (bool ( *)(uintptr_t))(Class_Pawn_get_IsFiring);
                    bTriggerReady = Pawn_get_IsFiring(LocalPawn);
                } else if (Config.Aim.Trigger == EAimTrigger::Scoping) {
                    auto Pawn_IsAiming = (bool ( *)(uintptr_t))(Class_Pawn_IsAiming);
                    bTriggerReady = Pawn_IsAiming(LocalPawn);
                }
                if (bTriggerReady) {
                    uintptr_t Target = 0;
                    if (Config.Aim.By == EAim::Distance) {
                        Target = GetClosestTarget();
                    }
                    if (Config.Aim.By == EAim::Crosshair) {
                        Target = GetInsideFOVTarget();
                    }
                    if (Target) {
                        Vector3 targetPos;
                        if (Config.Aim.Target == EAimTarget::Heads) {
                            auto m_HeadBone = * (Transform * *)(Target + Class_Pawn__m_HeadBone);
                            if (!m_HeadBone)
                                return oCreateBulletProjectile(thiz, startPos, dir, weaponImpact, itemID, flySmokeAssetID, enableVirtualStartPos, virtualStartPos);
                            targetPos = m_HeadBone->get_position();
                        }
                        if (Config.Aim.Target == EAimTarget::Chests) {
                            auto m_HeadBone = * (Transform * *)(Target + Class_Pawn__m_HeadBone);
                            if (!m_HeadBone)
                                return oCreateBulletProjectile(thiz, startPos, dir, weaponImpact, itemID, flySmokeAssetID, enableVirtualStartPos, virtualStartPos);
                            targetPos = m_HeadBone->get_position();
                            targetPos.y -= 0.2f;
                        }

                        if (Config.Aim.Target == EAimTarget::Body) {
                            auto m_HeadBone = * (Transform * *)(Target + Class_Pawn__m_HeadBone);
                            if (!m_HeadBone)
                                return oCreateBulletProjectile(thiz, startPos, dir, weaponImpact, itemID, flySmokeAssetID, enableVirtualStartPos, virtualStartPos);
                            targetPos = m_HeadBone->get_position();
                            targetPos.y -= 0.4f;
                        }

                        auto main = Camera::get_main();
                        if (main) {
                            auto mainView = ((Component *)main)->get_transform();
                            if (mainView) {
                                dir = targetPos - mainView->get_position();
                            }
                        }
                    }
                }
            }
        }
    }
    return oCreateBulletProjectile(thiz, startPos, dir, weaponImpact, itemID, flySmokeAssetID, enableVirtualStartPos, virtualStartPos);
}

bool isOutsideScreen(ImVec2 pos, ImVec2 screen) {
    if (pos.y < 0) {
        return true;
    }
    if (pos.x > screen.x) {
        return true;
    }
    if (pos.y > screen.y) {
        return true;
    }
    return pos.x < 0;
}

ImVec2 pushToScreenBorder(ImVec2 Pos, ImVec2 screen, int offset) {
    int x = (int)Pos.x;
    int y = (int)Pos.y;
    if (Pos.y < 0) {
        // top
        y = -offset;
    }
    if (Pos.x > screen.x) {
        // right
        x = (int)screen.x + offset;
    }
    if (Pos.y > screen.y) {
        // bottom
        y = (int)screen.y + offset;
    }
    if (Pos.x < 0) {
        // left
        x = -offset;
    }
    return ImVec2(x, y);
}

void drawLine(long color, float thickness, Vector3 start, Vector3 end) {
    ImGui::GetBackgroundDrawList()->PathLineTo(ImVec2(start.x, start.y));
    ImGui::GetBackgroundDrawList()->PathLineTo(ImVec2(end.x, end.y));
    ImGui::GetBackgroundDrawList()->PathStroke(color, 0, thickness);
}

void Box4Line(ImDrawList * draw, float thicc, int x, int y, int w, int h, int color) {
        int iw = w / 4;
        int ih = h / 4;
        // top
        draw->AddRect(ImVec2(x, y), ImVec2(x + iw, y), color, thicc);
        draw->AddRect(ImVec2(x + w - iw, y), ImVec2(x + w, y), color, thicc);
        draw->AddRect(ImVec2(x, y), ImVec2(x, y + ih), color, thicc);
        draw->AddRect(ImVec2(x + w - 1, y), ImVec2(x + w - 1, y + ih), color, thicc);
    ;
        // bottom
        draw->AddRect(ImVec2(x, y + h), ImVec2(x + iw, y + h), color, thicc);
        draw->AddRect(ImVec2(x + w - iw, y + h), ImVec2(x + w, y + h), color, thicc);
        draw->AddRect(ImVec2(x, y + h - ih), ImVec2(x, y + h), color, thicc);
        draw->AddRect(ImVec2(x + w - 1, y + h - ih), ImVec2(x + w - 1, y + h), color, thicc);
}

void DrawAimLine(ImDrawList* draw, int sWidth, int sHeight) {
    uintptr_t Target = 0;

    if (Config.Aim.By == EAim::Distance) {
        Target = GetClosestTarget();
    } else if (Config.Aim.By == EAim::Crosshair) {
        Target = GetInsideFOVTarget();
    }

    if (Target) {
        Vector3 targetPos;
        auto m_HeadBone = *(Transform**)(Target + Class_Pawn__m_HeadBone);
        if (!m_HeadBone) return;

        if (Config.Aim.Target == EAimTarget::Heads) {
            targetPos = m_HeadBone->get_position();
        } else if (Config.Aim.Target == EAimTarget::Chests) {
            targetPos = m_HeadBone->get_position();
            targetPos.y -= 0.2f;
        } else if (Config.Aim.Target == EAimTarget::Body) {
            targetPos = m_HeadBone->get_position();
            targetPos.y -= 0.4f;
        }

        auto HeadSc = WorldToScreenPoint(targetPos);
        if (HeadSc.z > 0) {
            ImVec2 center(sWidth / 2, sHeight / 2);
            draw->AddLine(center, ImVec2(HeadSc.x, sHeight - HeadSc.y), ToColor(Config.sColorsESPPLAYER.LinePLAYER), Config.sESPMenuLineScale.lineSize);

            // Draw ESP based on targetPos here using the same logic you have for drawing ESP.
        }
    }
}

// ===============================
// Vector & Matrix Definitions
// ===============================
struct Vector4 {
    float x, y, z, w;
};

struct Matrix4x4 {
    float _11, _12, _13, _14;
    float _21, _22, _23, _24;
    float _31, _32, _33, _34;
    float _41, _42, _43, _44;
};

struct Player {
    Vector3 HeadPos;
    Vector3 RootPos;
    int health;
    bool isEnemy;
};

// ===============================
// Mock drawLine (replace with ImGui / renderer call)
// ===============================
inline void DrawLine(float x1, float y1, float x2, float y2, unsigned int color) {
    ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x1, y1), ImVec2(x2, y2), color, 1.5f);
}


// ===============================
// WorldToScreen conversion
// ===============================
inline Vector2 WorldToScreen(Vector3 pos, const Matrix4x4& matrix, int width, int height) {
    float x = pos.x * matrix._11 + pos.y * matrix._21 + pos.z * matrix._31 + matrix._41;
    float y = pos.x * matrix._12 + pos.y * matrix._22 + pos.z * matrix._32 + matrix._42;
    float w = pos.x * matrix._14 + pos.y * matrix._24 + pos.z * matrix._34 + matrix._44;

    if (w < 0.1f)
        return { -1, -1 };

    float invW = 1.0f / w;
    x *= invW;
    y *= invW;

    Vector2 screen;
    screen.x = (width / 2.0f * x) + (width / 2.0f);
    screen.y = (height / 2.0f * -y) + (height / 2.0f);
    return screen;
}


// ===============================
// ESP Box Drawer
// ===============================
inline void DrawESPBox(Vector2 screenPos, float height, float width) {
    DrawLine(screenPos.x - width / 2, screenPos.y, screenPos.x + width / 2, screenPos.y, RED);
    DrawLine(screenPos.x - width / 2, screenPos.y + height, screenPos.x + width / 2, screenPos.y + height, RED);
    DrawLine(screenPos.x - width / 2, screenPos.y, screenPos.x - width / 2, screenPos.y + height, RED);
    DrawLine(screenPos.x + width / 2, screenPos.y, screenPos.x + width / 2, screenPos.y + height, RED);
}

void DrawESP(ImDrawList * draw, int sWidth, int sHeight, float density) {
ApplySpeedhack();
    if (Config.ESPMenu.Esp) {

        auto mainCamera = Camera::get_main(); // CAMERA NG PLAYER WHICH IS AKO / POV KO SYEMPRE.
        if (!mainCamera) return;  // IF YUNG CAMERA KO AY HINDI ATTACHED SAKIN THEN RETURN PARA WALANG CRASH.

        auto Gameplay_get_MatchGame = (uintptr_t (*)())(Class_Gameplay__get_MatchGame);
        auto get_MatchGame = Gameplay_get_MatchGame();

        if (!get_MatchGame) return; // Exit early if no match game is active.

        auto Gameplay_get_LocalPawn = (uintptr_t (*)())(Class_Gameplay__get_LocalPawn);
        auto LocalPawn = Gameplay_get_LocalPawn();
        if (!LocalPawn) return; // Exit early if local pawn is invalid.

        Vector3 MyPos{0, 0, 0};
        auto local_m_Mesh = *(Transform **)(LocalPawn + Class_Pawn__m_Mesh);
        if (local_m_Mesh) {
            MyPos = local_m_Mesh->get_position();
        }

        if (!isEspReady) {
            totalBots = 0;
            totalEnemies = 0;

            auto EnemyPawns = *(List<uintptr_t> **)(get_MatchGame + Class_BaseGame__EnemyPawns);
            if (!EnemyPawns || EnemyPawns->getSize() == 0) return; // Exit early if no enemy pawns.

            for (int i = 0; i < EnemyPawns->getSize(); i++) {
                auto Pawn = EnemyPawns->getItems()[i];
                if (!Pawn) continue; // Skip invalid pawn entries.

                auto m_PlayerInfo = *(uintptr_t *)(Pawn + Class_Pawn__m_PlayerInfo);
                if (!m_PlayerInfo) continue; // Skip if player info is missing.

                auto m_AttackableInfo = *(uintptr_t *)(m_PlayerInfo + Class_AttackableTarget__m_AttackableInfo);

                // Directly fetch health values.
                int CurHP = (int)*(float *)(m_AttackableInfo + Class_AttackableTarget__m_Health);
                int MaxHP = (int)*(float *)(m_AttackableInfo + Class_AttackableTarget__m_MaxHealth);

                auto m_HeadBone = *(Transform **)(Pawn + Class_Pawn__m_HeadBone);
                auto m_Mesh = *(Transform **)(Pawn + Class_Pawn__m_Mesh);

                if (!m_HeadBone || !m_Mesh) continue; // Skip invalid components.

                bool isBot = *(bool *)((uintptr_t)Pawn + Class_Pawn__m_IsBot);
                if (isBot) {
                    totalBots++;
                } else {
                    totalEnemies++;
                }

                auto HeadSc = WorldToScreenPoint(m_HeadBone->get_position());
                auto RootSc = WorldToScreenPoint(m_Mesh->get_position());

                float Distance = Vector3::Distance(MyPos, m_HeadBone->get_position());
                if (Distance > 500 || HeadSc.z <= 0 || RootSc.z <= 0) continue; // Skip out-of-range or invalid entities.


                bool isAlive = * (bool *)((uintptr_t)Pawn + Pawn_m_IsAlive);
                if (isAlive) {
                    if (HeadSc.z > 0 && RootSc.z > 0) {

                        if (Config.ESPMenu.Alert && isOutsideScreen({HeadSc.x, sHeight - HeadSc.y}, {sWidth, sHeight})) {
                            ImVec2 hintDotRenderPos = pushToScreenBorder({HeadSc.x, sHeight - HeadSc.y}, {sWidth, sHeight}, (int)((sHeight / (float)1080 * 100) / 3));
                            ImVec2 hintTextRenderPos = pushToScreenBorder({HeadSc.x, sHeight - HeadSc.y}, {sWidth, sHeight}, -(int)((sHeight / (float)1080 * 36)));

                            std::string strDistance = to_string((int)Distance) + "M";

                            draw->AddCircleFilled({hintDotRenderPos.x, hintDotRenderPos.y}, (sHeight / (float)1080 * 100), ImColor(255, 128, 0, 128), 0);
                            draw->AddText(nullptr, 25.0f, {hintTextRenderPos.x - 20.0f, hintTextRenderPos.y - 15.0f}, ImColor(255, 255, 255, 255), strDistance.c_str());
                        }

                        if (Config.ESPMenu.isPlayerLine) {
                            float scaleSizeLine = Config.sESPMenuLineScale.lineSize;
                            if (Config.ESPMenu.Target == LineTarget::Top) {
                                if (isBot) {
                                    if (isAlive) {
                                        draw->AddLine(ImVec2(sWidth / 2, 80), ImVec2(HeadSc.x, sHeight - HeadSc.y), ToColor(Config.sColorsESPBOT.LineBOT), scaleSizeLine);
                                    }
                                } else {
                                    if (isAlive) {
                                        draw->AddLine(ImVec2(sWidth / 2, 80), ImVec2(HeadSc.x, sHeight - HeadSc.y), ToColor(Config.sColorsESPPLAYER.LinePLAYER), scaleSizeLine);
                                    }
                                }
                            }

                            if (Config.ESPMenu.Target == LineTarget::Center) {
                                if (isBot) {
                                    if (isAlive) {
                                        draw->AddLine(ImVec2(sWidth / 2, sHeight / 2), ImVec2(HeadSc.x, sHeight - HeadSc.y), ToColor(Config.sColorsESPBOT.LineBOT), scaleSizeLine);
                                    }
                                } else {
                                    if (isAlive) {
                                        draw->AddLine(ImVec2(sWidth / 2, sHeight / 2), ImVec2(HeadSc.x, sHeight - HeadSc.y), ToColor(Config.sColorsESPPLAYER.LinePLAYER), scaleSizeLine);
                                    }
                                }
                            }
                            if (Config.ESPMenu.Target == LineTarget::Bottom) {
                                if (isBot) {
                                    if (isAlive) {
                                        draw->AddLine(ImVec2(sWidth / 2, sHeight), ImVec2(HeadSc.x, sHeight - RootSc.y), ToColor(Config.sColorsESPBOT.LineBOT), scaleSizeLine);
                                    }
                                } else {
                                    if (isAlive) {
                                        draw->AddLine(ImVec2(sWidth / 2, sHeight), ImVec2(HeadSc.x, sHeight - RootSc.y), ToColor(Config.sColorsESPPLAYER.LinePLAYER), scaleSizeLine);
                                    }
                                }
                            }
                        }

                        if (Config.ESPMenu.Crosshair | Config.Aim.By == EAim::Crosshair) {
                            if (Config.ESPMenu.CrosshairType == CrosshairTarget::Circle) {
                                ImVec2 center(get_width() / 2, get_height() / 2);
                                float radius = Config.Aim.size;
                                ImU32 color = ToColor(Config.sColorsESPOTHERS.PovOTHERS);
                                bool isAlive = * (bool *)((uintptr_t)Pawn + Pawn_m_IsAlive);
                                if (isAlive) {
                                    draw->AddCircle(center, radius, color, 60, 1.5f);
                                }
                            }
                            if (Config.ESPMenu.CrosshairType == CrosshairTarget::Plus) {
                                ImVec2 center(get_width() / 2, get_height() / 2);
                                float radius = Config.Aim.size;
                                ImU32 color = ToColor(Config.sColorsESPOTHERS.PovOTHERS);
                                bool isAlive = * (bool *)((uintptr_t)Pawn + Pawn_m_IsAlive);
                                if (isAlive) {
                                    draw->AddLine(ImVec2(center.x - radius, center.y), ImVec2(center.x + radius, center.y), color, 1.5f);
                                    draw->AddLine(ImVec2(center.x, center.y - radius), ImVec2(center.x, center.y + radius), color, 1.5f);
                                }
                            }
                            if (Config.ESPMenu.CrosshairType == CrosshairTarget::Ekis) {
                                ImVec2 center(get_width() / 2, get_height() / 2);
                                float radius = Config.Aim.size;
                                ImU32 color = ToColor(Config.sColorsESPOTHERS.PovOTHERS);
                                bool isAlive = * (bool *)((uintptr_t)Pawn + Pawn_m_IsAlive);
                                if (isAlive) {
                                    draw->AddLine(ImVec2(center.x - radius, center.y - radius), ImVec2(center.x + radius, center.y + radius), color, 1.5f);
                                    draw->AddLine(ImVec2(center.x + radius, center.y - radius), ImVec2(center.x - radius, center.y + radius), color, 1.5f);
                                }

                            }
                        }

if (Config.ESPMenu.Box) {
    // Get head and root screen positions from existing code
    Vector2 headScreen = { HeadSc.x, sHeight - HeadSc.y };
    Vector2 rootScreen = { RootSc.x, sHeight - RootSc.y };

    // Validate coordinates
    if (HeadSc.z > 0 && RootSc.z > 0) {
        float boxHeight = fabsf(rootScreen.y - headScreen.y);
        float boxWidth  = boxHeight * 0.65f;

        // Top-left and bottom-right corners
        ImVec2 topLeft(headScreen.x - boxWidth / 2, headScreen.y);
        ImVec2 bottomRight(headScreen.x + boxWidth / 2, headScreen.y + boxHeight);

        // Pick color depending on bot or player
        ImU32 boxColor = isBot
            ? ToColor(Config.sColorsESPBOT.BoxBOT)
            : ToColor(Config.sColorsESPPLAYER.BoxPLAYER);

        // Draw box
        draw->AddRect(topLeft, bottomRight, boxColor, 0, 0, 2.0f);
    }
}


                        if (Config.ESPMenu.Health) {
                            auto m_AttackableInfo = * (uintptr_t *)(Pawn + Class_AttackableTarget__m_AttackableInfo);
                            if (m_AttackableInfo) {
                                int CurHP = (int) * (float *)(m_AttackableInfo + Class_AttackableTarget__m_Health);
                                int MaxHP = (int) * (float *)(m_AttackableInfo + Class_AttackableTarget__m_MaxHealth);

                                long HPColor = IM_COL32(std::min(((510 * (MaxHP - CurHP)) / MaxHP), 255), std::min(((510 * CurHP) / MaxHP), 255), 0, 155);

                                float boxWidth = density / 1.6f;
                                boxWidth -= std::min(((boxWidth / 2) / 00.0f) * Distance,
                                                     boxWidth / 2);
                                float boxHeight = boxWidth * 0.07f;

                                ImVec2 vStart = {HeadSc.x - (boxWidth / 2),
                                                 sHeight - HeadSc.y - (boxHeight * 2.1f)
                                                };

                                ImVec2 vEndFilled = {vStart.x + (CurHP * boxWidth / MaxHP),
                                                     vStart.y + boxHeight
                                                    };
                                ImVec2 vEndRect = {vStart.x + boxWidth, vStart.y + boxHeight};

                                draw->AddRectFilled(vStart, vEndFilled, HPColor);
                                draw->AddRect(vStart, vEndRect, IM_COL32(0, 0, 0, 155));
                            }
                        }



                        if (Config.ESPMenu.Name) {
                            auto m_NickName = *(String**)(m_PlayerInfo + Class_PlayerInfo__m_NickName);

                            int NameColor;
                            char ws[0xFF] = {0};

                            if (m_NickName && m_NickName->CString()) {
                                snprintf(ws, sizeof(ws), OBFUSCATE(" %s "), m_NickName->CString());
                            } else {
                                snprintf(ws, sizeof(ws), OBFUSCATE(" UNKNOWN "));
                            }

                            float boxWidth = density / 1.8f;
                            boxWidth -= std::min(((boxWidth / 2) / 00.0f) * Distance, boxWidth / 2);
                            float boxHeight = boxWidth * 0.19f;
                            std::string name;

                            if (isBot) {
                                NameColor = IM_COL32(0, 255, 0, 255);
                                name += "   BOT";
                                draw->AddText(NULL, ((float)density / 30.0f), {
                                    HeadSc.x - (boxWidth / 3),
                                    sHeight - HeadSc.y - (boxHeight * 1.83f)
                                },
                                ToColor(Config.sColorsESPBOT.NameBOT), name.c_str());
                            } else {
                                NameColor = IM_COL32(255, 255, 100, 255);
                                name += std::string(" ") + ws;

                                draw->AddText(NULL, ((float)density / 30.0f), {
                                    HeadSc.x - (boxWidth / 3),
                                    sHeight - HeadSc.y - (boxHeight * 1.83f)
                                },
                                ToColor(Config.sColorsESPPLAYER.NamePLAYER), name.c_str());
                            }
                        }


                        if (Config.ESPMenu.Distance) {
                            float boxWidth = density / 1.8f;
                            boxWidth -= std::min(((boxWidth / 2) / 00.0f) * Distance, boxWidth / 2);
                            float boxHeight = boxWidth * 0.19f;

                            std::string strDistance = to_string((int)Distance) + "M";


                            if (isBot) {
                                draw->AddText(NULL, ((float)density / 30.0f), {HeadSc.x + (boxWidth / 3), sHeight - HeadSc.y - (boxHeight * 1.83f)}, ToColor(Config.sColorsESPBOT.DistanceBOT), strDistance.c_str());
                            } else {
                                draw->AddText(NULL, ((float)density / 30.0f), {HeadSc.x + (boxWidth / 3), sHeight - HeadSc.y - (boxHeight * 1.83f)}, ToColor(Config.sColorsESPPLAYER.DistancePLAYER), strDistance.c_str());
                            }
                        }
                    }







if (Config.ESPMenu.Count && Config.ESPMenu.Esp) {
    int totalBots = 0;
    int totalEnemies = 0;
    
    // Only count if we have valid enemy data
    if (EnemyPawns != nullptr && EnemyPawns->getSize() > 0) {
        for (int i = 0; i < EnemyPawns->getSize(); i++) {
            auto Pawn = EnemyPawns->getItems()[i];
            if (!Pawn) continue;
            
            bool isAlive = *(bool*)((uintptr_t)Pawn + Pawn_m_IsAlive);
            if (!isAlive) continue;
            
            bool isBot = *(bool*)((uintptr_t)Pawn + Class_Pawn__m_IsBot);
            isBot ? totalBots++ : totalEnemies++;
        }
    }

    // Only draw if we have enemies or ESP is active
    if ((totalBots > 0 || totalEnemies > 0) && Config.ESPMenu.Esp) {
        ImDrawList* drawList = ImGui::GetForegroundDrawList();
        if (!drawList) return;

        // Display setup
        const float rectWidth = 100.0f;
        const float rectHeight = 40.0f;
        const float spacing = 20.0f;
        const float rounding = 10.0f;
        const float startY = 40.0f;
        
        // Calculate positions
        float centerX = sWidth / 2.0f;
        ImVec2 botRectMin = { centerX - rectWidth - spacing, startY };
        ImVec2 botRectMax = { centerX - spacing, startY + rectHeight };
        ImVec2 playerRectMin = { centerX + spacing, startY };
        ImVec2 playerRectMax = { centerX + rectWidth + spacing, startY + rectHeight };

        // Draw boxes
        drawList->AddRect(botRectMin, botRectMax, IM_COL32_WHITE, rounding, 0, 2.0f);
        drawList->AddRect(playerRectMin, playerRectMax, IM_COL32_WHITE, rounding, 0, 2.0f);

        // Draw icons and text
        const float iconOffset = 10.0f;
        const float textOffset = 40.0f;
        const float verticalOffset = 8.0f;
        
        // Bot display
        ImVec2 botIconPos = { botRectMin.x + iconOffset, botRectMin.y + verticalOffset };
        ImVec2 botTextPos = { botRectMax.x - textOffset, botRectMin.y + verticalOffset };
        drawList->AddText(nullptr, 22.0f, botIconPos, IM_COL32_WHITE, ICON_FA_DESKTOP);
        drawList->AddText(botTextPos, IM_COL32_WHITE, std::to_string(totalBots).c_str());

        // Player display
        ImVec2 playerIconPos = { playerRectMin.x + iconOffset, playerRectMin.y + verticalOffset };
        ImVec2 playerTextPos = { playerRectMax.x - textOffset, playerRectMin.y + verticalOffset };
        drawList->AddText(nullptr, 22.0f, playerIconPos, IM_COL32_WHITE, ICON_FA_USER);
        drawList->AddText(playerTextPos, IM_COL32_WHITE, std::to_string(totalEnemies).c_str());
    }
}
                                 




                    if (Config.ESPMenu.Aimline) {
                        DrawAimLine(draw, sWidth, sHeight);
                    }

                }
            }
        }
    }
}





void *AimBot_Func() {

    if (m_unity && Config.Aim.Aimbot360) {
        auto Gameplay_get_MatchGame = (uintptr_t ( *)())(Class_Gameplay__get_MatchGame);
        auto baseGame = Gameplay_get_MatchGame();
        if (Tools::IsPtrValid((void *) baseGame)) {
            auto Gameplay_get_LocalPawn = (uintptr_t ( *)())(Class_Gameplay__get_LocalPawn);
            auto LocalPawn = Gameplay_get_LocalPawn();
            if (LocalPawn) {
                if (true) {
                    bool bTriggerReady =  Config.Aim.Trigger == EAimTrigger::None;
                    if (Config.Aim.Trigger == EAimTrigger::Shooting) {
                        auto Pawn_get_IsFiring = (bool ( *)(uintptr_t))(Class_Pawn_get_IsFiring);
                        bTriggerReady = Pawn_get_IsFiring(LocalPawn);
                    } else if (Config.Aim.Trigger == EAimTrigger::Scoping) {
                        auto Pawn_IsAiming = (bool ( *)(uintptr_t))(Class_Pawn_IsAiming);
                        bTriggerReady = Pawn_IsAiming(LocalPawn);
                    }
                    if (bTriggerReady) {
                        uintptr_t Target = 0;
                        if (Config.Aim.By == EAim::Distance) {
                            Target = GetClosestTarget();
                        }
                        if (Config.Aim.By == EAim::Crosshair) {
                            Target = GetInsideFOVTarget();
                        }

                        if (Target) {
                            Vector3 targetPos;
                            if (Config.Aim.Target == EAimTarget::Heads) {
                                auto m_HeadBone = * (Transform * *)(Target + Class_Pawn__m_HeadBone);
                                if (!m_HeadBone)
                                    return nullptr;
                                targetPos = m_HeadBone->get_position();
                            }
                            if (Config.Aim.Target == EAimTarget::Chests) {
                                auto m_HeadBone = * (Transform * *)(Target + Class_Pawn__m_HeadBone);
                                if (!m_HeadBone)
                                    return nullptr;
                                targetPos = m_HeadBone->get_position();
                                targetPos.y -= 0.2f;
                            }
                            if (Config.Aim.Target == EAimTarget::Body) {
                                auto m_HeadBone = * (Transform * *)(Target + Class_Pawn__m_HeadBone);
                                if (!m_HeadBone)
                                    return nullptr;
                                targetPos = m_HeadBone->get_position();
                                targetPos.y -= 0.4f;
                            }

                            auto main = Camera::get_main();
                            if (main) {
                                auto mainView = ((Component *) main)->get_transform();
                                if (mainView) {
                                    auto Pawn_set_AimRotation = (void ( *)(uintptr_t, Quaternion))(Class_Pawn_set_AimRotation);
                                    Pawn_set_AimRotation(LocalPawn, Quaternion::LookRotation(targetPos - mainView->get_position(), Vector3::Up()));
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return nullptr;
}



