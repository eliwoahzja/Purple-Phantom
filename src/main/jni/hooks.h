#pragma once

namespace StretchScreen {

constexpr float kMinAspect = 1.0f;
constexpr float kMaxAspect = 3.0f;

inline bool ValidAspect(float value)
{
    return value >= kMinAspect && value <= kMaxAspect;
}

inline void Apply(float width, float height)
{
    if (width <= 0.0f || height <= 0.0f)
        return;

    Camera *camera = Camera::get_main();
    if (!camera)
        return;

    static Camera *lastCamera = nullptr;
    static float savedAspect = 0.0f;
    static bool applied = false;

    if (camera != lastCamera) {
        lastCamera = camera;
        savedAspect = camera->get_aspect();

        if (!ValidAspect(savedAspect))
            savedAspect = width / height;

        applied = false;
    }

    if (!Config.ExtraMenu.StretchScreen) {
        if (applied) {
            camera->set_aspect(savedAspect);
            applied = false;
        }
        return;
    }

    float target = Config.ExtraMenu.StretchAspect;

    if (!ValidAspect(target))
        target = savedAspect;

    camera->set_aspect(target);
    applied = true;
}

} // namespace StretchScreen
bool (*orig_IsUnlocked)(void *instance);
bool IsUnlocked(void *instance) {
    if (instance != NULL) {
        if (Config.ExtraMenu.Blueprints) {
            return true;
        }
    }
    return orig_IsUnlocked(instance);
}

float ( *orig_GetAssitAimSpeed)(void * instance, Vector3 assistCentorPos, float assistDis, float dis, float angle, bool isPVE, bool gamepadInput);
float GetAssitAimSpeed(void * instance, Vector3 assistCentorPos, float assistDis, float dis, float angle, bool isPVE, bool gamepadInput) {
    if (instance != NULL) {
        if (Config.Aim.AimAssist) {
            return (float)Config.Aim.AimAssistSize;
        }
    }
    return orig_GetAssitAimSpeed(instance, assistCentorPos, assistDis, dis, angle, isPVE, gamepadInput);
}

float ( *orig_GetScaleRecoil)(void * instance);
float GetScaleRecoil(void * instance) {
    if (instance != NULL) {
        if (Config.ExtraMenu.Recoil) {
            return 0.00001f;
        }
    }
    return orig_GetScaleRecoil(instance);
}

float ( *orig_GetRealSpreadModifier)(void * instance);
float GetRealSpreadModifier(void * instance) {
    if (instance != NULL) {
        if (Config.ExtraMenu.Spread) {
            return 0.00001f;
        }
    }
    return orig_GetRealSpreadModifier(instance);
}


float ( *orig_get_ChangeClipTime)(void *instance);
float get_ChangeClipTime(void *instance) {
    if (instance != NULL) {
        if (Config.ExtraMenu.Reload) {
            return 0.0001f;
        }
    }
    return orig_get_ChangeClipTime(instance);
}

float ( *orig_get_ChangeClipLoopTime)(void *instance);
float get_ChangeClipLoopTime(void *instance) {
    if (instance != NULL) {
        if (Config.ExtraMenu.Reload) {
            return 0.0001f;
        }
    }
    return orig_get_ChangeClipLoopTime(instance);
}

void ( *orig_UpdateCameraShake)(void *instance);
void UpdateCameraShake(void *instance) {
    if (instance != NULL) {
        if (Config.ExtraMenu.Shake) {
            return (void) true;
        }
    }
    return orig_UpdateCameraShake(instance);
}

float ( *orig_get_AimingTime)(void *instance);
float get_AimingTime(void *instance) {
    if (instance != NULL) {
        if (Config.ExtraMenu.Scope) {
            return 0.0001f;
        }
    }
    return orig_get_AimingTime(instance);
}

float ( *orig_get_MedicineUseTime)(void *instance);
float get_MedicineUseTime(void *instance) {
    if (instance != NULL) {
        if (Config.ExtraMenu.Medicine) {
            return 0.0001f;
        }
    }
    return orig_get_MedicineUseTime(instance);
}

float ( *orig_get_EquipTime)(void *instance);
float get_EquipTime(void *instance) {
    if (instance != NULL) {
        if (Config.ExtraMenu.Switch) {
            return 0.0001f;
        }
    }
    return orig_get_EquipTime(instance);
}

float ( *orig_get_UnequipTime)(void *instance);
float get_UnequipTime(void *instance) {
    if (instance != NULL) {
        if (Config.ExtraMenu.Switch) {
            return 0.0001f;
        }
    }
    return orig_get_UnequipTime(instance);
}

void ( *orig_UpdateCrosshairSize)(void *instance, void *weap);
void UpdateCrosshairSize(void *instance, void *weap) {
    if (instance != NULL) {
        if (Config.ExtraMenu.Small) {
            return (void) true;
        }
    }
    return orig_UpdateCrosshairSize(instance, weap);
}

void (*original_Init)(void* instance);
void hooked_Init(void* instance) {
    if (instance != NULL) {
        if (Config.ExtraMenu.Spectate) {
            return;
        }
    }
    original_Init(instance);
}

void (*orig_InitMaxPickUpDistance)(void *instance);
void InitMaxPickUpDistance(void *instance) {
    if (instance != NULL) {
        if (Config.ExtraMenu.MagnetLoot) {
            *((float *)((uintptr_t)instance + 0x80)) = 5.00f;
        }
    }
 orig_InitMaxPickUpDistance(instance);
}

static bool bIsEmulator = false;
bool IsEmulator(){
	return bIsEmulator;
}

static bool bIsEnableVulkan = false;
bool IsEnableVulkan(){
	return bIsEnableVulkan;
}

bool (*orig_NeedDelayProcess)(void* instance);
bool NeedDelayProcess(void* instance) {
    if (instance != NULL) {
        if (Config.ExtraMenu.Spectate) {
            return false;
        }
    }
    return orig_NeedDelayProcess(instance);
}

bool (*orig_ProcessForDelay)(void* instance, uint8_t* packData, int packDataCount, int sequence, float timeQueued, int aiIndex);
bool ProcessForDelay(void* instance, uint8_t* packData, int packDataCount, int sequence, float timeQueued, int aiIndex) {
    if (instance != NULL) {
        if (Config.ExtraMenu.Spectate) {
            return false;
        }
    }
    return orig_ProcessForDelay(instance, packData, packDataCount, sequence, timeQueued, aiIndex);
}

int (*orig_get_ShotCost)(void *instance);
int get_ShotCost(void *instance) {
    if (instance != NULL) {
        if (Config.ExtraMenu.Cost) {
            return 0;
        }
    }
    return orig_get_ShotCost(instance);
}

float ( *orig_OverHeat)(void *instance);
float OverHeat(void *instance) {
    if (instance != NULL) {
        if (Config.ExtraMenu.Over) {
            return 0.000001f;
        }
    }
    return orig_OverHeat(instance);
}

void ( *orig_OnFlashBangExplode)(void *instance, int weaponItemID, float whiteTime, float whiteAlphaTime, float initIntensity);
void OnFlashBangExplode(void *instance, int weaponItemID, float whiteTime, float whiteAlphaTime, float initIntensity) {
    if (instance != NULL) {
        if (Config.ExtraMenu.Flash) {
            return (void) false;
        }
    }
    return orig_OnFlashBangExplode(instance, weaponItemID, whiteTime, whiteAlphaTime, initIntensity);
}

bool ( *orig_SingleLineCheckPhysics)(void *instance, int hitType, void *hitTarget, void *hitCollider, Vector3 startPos, Vector3 dir, void *impactInfo);
bool SingleLineCheckPhysics(void *instance, int hitType, void *hitTarget, void *hitCollider, Vector3 startPos, Vector3 dir, void *impactInfo) {
    if (instance != NULL) {
        if (Config.ExtraMenu.Hit) {
            return true;
        }
    }
    return orig_SingleLineCheckPhysics(instance, hitType, hitTarget, hitCollider, startPos, dir, impactInfo);
}

float (*orig_GetFastAmmoInterval)(void *instance, float fireInterval);
float GetFastAmmoInterval(void *instance, float fireInterval) {
    if (instance != NULL) {
        if (Config.ExtraMenu.Fire) {
            return 85.0f;

        }
    }
    return orig_GetFastAmmoInterval(instance, fireInterval);
}

float (*orig_GetPickUpTime)(void *instance);
float GetPickUpTime(void *instance) {
    if (instance != NULL) {
        if (Config.ExtraMenu.FastLoot) {
            return 0.0001f; 
        }
    }
    return orig_GetPickUpTime(instance);
}

bool ( *orig_IsAimTargetCanAutoFire)(void *instance, void *weap);
bool IsAimTargetCanAutoFire(void *instance, void *weap) {
    if (instance != NULL) {
        if (Config.ExtraMenu.Auto) {
            return true;
        }
    }
    return orig_IsAimTargetCanAutoFire(instance, weap);
}

bool (*orig_AdvanceUAV)(void *instance);
bool getAdvanceUAV(void *instance) {
    if (instance != NULL) {
        if (Config.ExtraMenu.Radar1) {
            return true;
        }
    }
    return orig_AdvanceUAV(instance);
}

void ( *orig_OpenParachute)(void *instance, bool isAuto);
void OpenParachute(void *instance, bool isAuto) {
    if (instance != NULL) {
        if (Config.ExtraMenu.Parachute) {
            return (void) true;
        }
    }
    return orig_OpenParachute(instance, isAuto);
}

float (*orig_get_AccelerationForwardSpeedUp)(void *instance);
float get_AccelerationForwardSpeedUp(void *instance) {
    if (instance != NULL) {
        if (Config.ExtraMenu.Diving) {
            return 200.0f;

        }
    }
    return orig_get_AccelerationForwardSpeedUp(instance);
}

float (*orig_get_MaxVelocityForwardSpeedUp)(void *instance);
float get_MaxVelocityForwardSpeedUp(void *instance) {
    if (instance != NULL) {
        if (Config.ExtraMenu.Diving) {
            return 200.0f;

        }
    }
    return orig_get_MaxVelocityForwardSpeedUp(instance);
}

float (*orig_GetMaxJumpHeight)(void *);
float hook_GetMaxJumpHeight(void *instance) {
    float orig_ = orig_GetMaxJumpHeight(instance);
    return isJumpAdjustmentEnabled ? orig_ * jumpHeightMultiplier : orig_;
}


float (*o_GetSuperSlideRate)(void *ins);
float h_GetSuperSlideRate(void *ins) {
    if (SlideTackle) {
        return SlideRange + 256;
    }

    return o_GetSuperSlideRate(ins);
}

float (*o_get_SlideTackleAcclerationSpeed)(void *ins);
float h_get_SlideTackleAcclerationSpeed(void *ins) {
    if (SlideTackle) {
        return SlideRange + 1;
    }

    return o_get_SlideTackleAcclerationSpeed(ins);
}

float (*o_PawnGetMaxSpeed)(void *ins);
float h_PawnGetMaxSpeed(void *ins) {
    if (SlideTackle) {
        return SlideRange + 256;
    }

    return o_PawnGetMaxSpeed(ins);
}

void (*o_TickLocalPlayer)(void *ins, float deltaTime);
void h_TickLocalPlayer(void *ins, float deltaTime) {
    if (!SlideTackle) {
        o_TickLocalPlayer(ins, deltaTime);
    }
}

float (*o_get_SlideTackleSpeed)(void *ins);
float h_get_SlideTackleSpeed(void *ins) {
    if (SlideTackle) {
        return SlideRange;
    }

    return o_get_SlideTackleSpeed(ins);
}

float ( *orig_GetWeaponMoveScale)(void *instance);
float GetWeaponMoveScale(void *instance) {
    if (instance != NULL) {
        if (Config.ExtraMenu.Speed) {
            return 2.0f;
        }
    }
    return orig_GetWeaponMoveScale(instance);
}


bool (*orig_IsWidgetVisible)(void *instance, void *rec);
bool IsWidgetVisible(void *instance, void *rect) {
    if (instance != NULL) {
        if (Config.ExtraMenu.Fps) {
            return true;
        }
    }
    return orig_IsWidgetVisible(instance, rect);
}


inline int (*original_Fpslevel)(void*) = nullptr;
inline int hooked_Fpslevel(void* ins) {
    return (ins && Config.ExtraMenu.EnhanceFPS) ? 7 : original_Fpslevel(ins);
}

inline int (*original_Fpslevel1)(void*) = nullptr;
inline int hooked_Fpslevel1(void* ins) {
    return (ins && Config.ExtraMenu.EnhanceFPS) ? 7 : original_Fpslevel1(ins);
}

inline int (*orig_GetFrameRateValue)(void*, int, int) = nullptr;
inline int GetFrameRateValue(void* ins, int f, int m) {
    if (ins) {
        return 360;
    }
    return orig_GetFrameRateValue(ins, f, m);
}

inline int (*orig_GetUltraFrameRateFinalFPS)(void*) = nullptr;
inline int GetUltraFrameRateFinalFPS(void* g) {
    return (g && Config.ExtraMenu.EnhanceFPS) ? 360 : orig_GetUltraFrameRateFinalFPS(g);
}

inline bool (*original_Extremequality)(void*) = nullptr;
inline bool hooked_Extremequality(void* ins) {
    return (ins && Config.ExtraMenu.EnhanceFPS) ? true : original_Extremequality(ins);
}
typedef void (*SetUltraFrameRateDeviceInfo_t)(void*, bool, int, int, int, bool);
inline SetUltraFrameRateDeviceInfo_t orig_SetUltraFrameRateDeviceInfo = nullptr;

inline void hooked_SetUltraFrameRateDeviceInfo(void* thiz, bool enableUltraFrameRate, int ultraFrameRate, int ultraFrameRateBR, int ultraFrameRateQualityLimit, bool customizedFrameRate) {
    if (thiz != NULL) {
        enableUltraFrameRate = true;
        ultraFrameRate = 5000;
        ultraFrameRateBR = 5000;
        ultraFrameRateQualityLimit = 5000;
        customizedFrameRate = true;
    }
    orig_SetUltraFrameRateDeviceInfo(thiz, enableUltraFrameRate, ultraFrameRate, ultraFrameRateBR, ultraFrameRateQualityLimit, customizedFrameRate);
} 

//-- Vulkan Support
int (*orig_GetGulagTicketCount)(void *instance, uint playerId);
int get_GulagTicketCount(void *instance, uint playerId) {
    if (instance != NULL) {
        if (Config.ExtraMenu.TestGulag) {
            *(uint*)((uintptr_t)instance + 0x10) = 2;
            
            int originalCount = orig_GetGulagTicketCount(instance, playerId);
            return originalCount * 2;
        }
    }
    return orig_GetGulagTicketCount(instance, playerId);
}



bool ( *orig_SameTeamFromPawn)(void *instance, void *pawn1, void *pawn2);
bool SameTeamFromPawn(void *instance, void *pawn1, void *pawn2) {
    if (instance != NULL) {
        if (Config.ExtraMenu.Radar2) {
            return true;
        }
    }
    return orig_SameTeamFromPawn(instance, pawn1, pawn2);
}

// Hook for CheckTargetIsValid_DyingInAzurGameMode
bool (*orig_CheckTargetIsValid_DyingInAzurGameMode)(void* instance, void* targetPawn);
bool CheckTargetIsValid_DyingInAzurGameMode_Hook(void* instance, void* targetPawn) {
    if (instance != nullptr && targetPawn != nullptr) {
        if (isExecute) {
            return true;
        }
    }
    return orig_CheckTargetIsValid_DyingInAzurGameMode(instance, targetPawn);
}

// Hook for CheckTargetIsValid_PhysState
bool (*orig_CheckTargetIsValid_PhysState)(void* instance, void* targetPawn);
bool CheckTargetIsValid_PhysState_Hook(void* instance, void* targetPawn) {
    if (instance != nullptr && targetPawn != nullptr) {
        if (isExecute) {
            return true;
        }
    }
    return orig_CheckTargetIsValid_PhysState(instance, targetPawn);
}

// Hook for CheckTargetIsValid_ManualParameter
bool (*orig_CheckTargetIsValid_ManualParameter)(void* instance, void* tempPawn, float* distance);
bool CheckTargetIsValid_ManualParameter_Hook(void* instance, void* tempPawn, float* distance) {
    if (instance != nullptr && tempPawn != nullptr) {
        if (isExecute) {
            return true;
        }
    }
    return orig_CheckTargetIsValid_ManualParameter(instance, tempPawn, distance);
}

// Hook for CheckTargetIsValid_Ult
bool (*orig_CheckTargetIsValid_Ult)(void* instance, void* targetPawn);
bool CheckTargetIsValid_Ult_Hook(void* instance, void* targetPawn) {
    if (instance != nullptr && targetPawn != nullptr) {
        if (isExecute) {
            return true;
        }
    }
    return orig_CheckTargetIsValid_Ult(instance, targetPawn);
}



bool (*orig_CalculateObstacleSide)(void* instance, float* backDis, float* beforeDis);
bool CalculateObstacleSide_Hook(void* instance, float* backDis, float* beforeDis) {
    if (instance != nullptr) {
        if (isExecute) {
            return false;
        }
    }
    return orig_CalculateObstacleSide(instance, backDis, beforeDis);
}

bool (*orig_CalculateObstacleTop)(void* instance, Vector3 pawnPos);
bool CalculateObstacleTop_Hook(void* instance, Vector3 pawnPos) {
    if (instance != nullptr) {
        if (isExecute) {
            return false;
        }
    }
    return orig_CalculateObstacleTop(instance, pawnPos);
}

int (*oCheckTargetIsValid)(void *ins, void* tempPawn, float distance);
int CheckTargetIsValid(void *ins, void* tempPawn, float distance) {
    if ( ins != nullptr ) {
        if (isExecute) {
            return (int)true;
        }
    }
    return oCheckTargetIsValid(ins, tempPawn, distance);
}

//Vsync
void (*orig_SetEnableVSync)(bool enabled);
void my_SetEnableVSync(bool enabled) {
    orig_SetEnableVSync(false);
}

bool (*oCheckCanExcution_Extra)(void *ins);
bool CheckCanExcution_Extra(void *ins) {
    if ( ins != nullptr) {
        if (isExecute) {
            return true;
        }
    }
    return oCheckCanExcution_Extra(ins);
}


bool (*o_CheckExecution_ObstacleAround)(void *ins);
bool CheckExecution_ObstacleAround(void *ins) {
    if (ins!=nullptr) {
        if (isExecute) {
            return true;
        }
    }
    return o_CheckExecution_ObstacleAround(ins);
}
static bool bIsTutorialEnabled = false;
bool IsTutorialEnabled() {
    return bIsTutorialEnabled;
}

float (*get_m_PhysSkisMaxSpeed)(void* instance);
float hooked_get_m_PhysSkisMaxSpeed(void* instance) {
    if (SnowB) {
        return SnowBsize;
    }
    return get_m_PhysSkisMaxSpeed(instance);

}


float (*original_CalcFinalMoveScale)(void* instance);
float hooked_CalcFinalMoveScale(void* instance) {
    if (instance == nullptr) {
        return original_CalcFinalMoveScale(instance);
    }

    if (isSpeedHackEnabled) {
        if (speedHackMultiplier > 0.0f && speedHackMultiplier <= 100.0f) {
            return speedHackMultiplier;
        }
    }

    return original_CalcFinalMoveScale(instance);
}

static bool bIs180FPS = false;
bool Is180FPS() {
    return bIs180FPS;
}

static bool bIs180FPSON = false;
void Is180FPSON(bool value) {
    bIs180FPS = value;
}

static bool bIs180FPSSupport = false;
bool Is180FPSSupport() {
    return bIs180FPSSupport;
}

static bool bIsDynamicFrame = false;
bool IsDynamicFrame() {
    return bIsDynamicFrame;
}

static bool bIsDynamicFrameON = false;
void IsDynamicFrameON(bool value) {
    bIsDynamicFrame = value;
}

static bool bIsDynamicFrameSupport = false;
bool IsDynamicFrameSupport() {
    return bIsDynamicFrameSupport;
}


void SPAMSINT0() {

    printf("SPAMSINT0 triggered!\n");
}

size_t hook_strlen(const char *thread)
{
    if (strstr(thread, "eglSwapBuffers"))
    {
        SPAMSINT0();
    }
    return strlen(thread);
}



inline float read_cpu_temp() {
    const char* paths[] = {
        "/sys/class/thermal/thermal_zone0/temp",
        "/sys/class/thermal/thermal_zone1/temp",
        "/sys/class/thermal/thermal_zone2/temp",
        "/sys/class/thermal/thermal_zone3/temp"
    };
    for (auto& path : paths) {
        std::ifstream file(path);
        if (file.is_open()) {
            float temp;
            file >> temp;
            file.close();
            if (temp > 1000.0f) temp /= 1000.0f;  
            if (temp > 0.0f && temp < 100.0f) return temp;
        }
    }
    return 35.0f; // fallback bahog belat
}


inline int adaptive_ultra_fps() {
    float temp = read_cpu_temp();


    const float cool = 40.0f;
    const float warm = 48.0f;
    const float hot  = 58.0f;
    const float danger = 65.0f;

    int fps;
    if (temp <= cool) fps = 120;             
    else if (temp <= warm) fps = 120;        
    else if (temp <= hot) fps = 120;         
    else if (temp <= danger) fps = 60;       
    else fps = 30;                           

    return fps;
}




bool (*orig_bypass)(void *ins);
bool hook_bypass(void *ins) {
    return false;
}
