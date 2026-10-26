#pragma once

#define m_UNITYLIB OBFUSCATE("libunity.so")
#define m_ANOGSLIB OBFUSCATE("libanogs.so")
#define m_UNITYLIB "libunity.so"

#define SLEEP_TIME 1000LL / 60LL

uintptr_t Class_Time_set_timeScale;
uintptr_t Class_Time_get_timeScale;



uintptr_t m_unity;
void UpdateAllOffset(){
#define Class_Input__get_touchCount (m_unity + 0x5057E08 ) //	public static int get_touchCount() { }; //RVA:
#define Class_Input__GetTouch (m_unity + 0x5057E18 ) //public static Touch GetTouch(int index) { };
#define Class_Input__get_mousePosition (m_unity + 0x50577C0 ) //public static Vector3 get_mousePosition() { }; //RVA: 

#define Class_Screen__get_width (m_unity + 0x50504BC ) //public static int get_width() { }; 
#define Class_Screen__get_height (m_unity + 0x5050520 ) //public static int get_height() { };
#define Class_Screen__get_density (m_unity + 0x5087930 ) //public static float get_dpi() { };

#define Class_Camera__get_main (m_unity + 0x5024010 ) //public static Camera get_main() { }; /
#define Class_Camera__WorldToScreenPoint (m_unity + 0x5023B54 ) //public Vector3 WorldToScreenPoint(Vector3 position) { };

#define Class_Component__get_transform (m_unity + 0x87192AC ) //public Transform get_transform() { }; 
#define Class_Transform__get_position (m_unity + 0x507C19C ) //public Vector3 get_position() { };

#define Class_Gameplay__get_MatchGame (m_unity + 0x8569B10 ) //public static MatchGame get_MatchGame() { }; 
#define Class_Gameplay__get_LocalPawn (m_unity + 0x856AA08 ) //public static Pawn get_LocalPawn() { }; 
#define Class_Pawn_get_IsFiring (m_unity + 0x8DCFC60 ) //public bool get_IsFiring() { };

#define Class_BaseGame__m_AllVehicles = 0x1A0;

#define Pawn_get_CurrentWeapon (m_unity + 0x518BEA8)
#define Weapon_GetWeaponId (m_unity + 0xB0A9C10) // known-good current build: Weapon.get_WeaponID()

#define Class_Pawn_set_AimRotation (m_unity + 0x51B42E4 ) //public virtual void set_AimRotation(Quaternion value) { }; 
#define Class_Pawn_IsAiming (m_unity + 0x519BB10 ) //public bool IsAiming() { }; 
#define Pawn_m_IsAlive (0x548 ) //protected bool m_IsAlive
    
#define Class_Pawn__m_Mesh (0x628 ) //protected Transform m_Mesh;
#define Class_Pawn__m_HeadBone (0x308 ) //protected Transform m_HeadBone
#define Class_Pawn__m_PlayerInfo (0x5C0 ) //protected PlayerInfo m_PlayerInfo
#define Class_Pawn__m_IsBot (0x5B9 ) //private bool m_IsBot

#define Class_BaseGame__EnemyPawns (0x178 ) //private readonly PawnList EnemyPawns; 
#define Class_PlayerInfo__m_NickName (0x160 ) //protected string m_NickName

#define Class_AttackableTarget__m_AttackableInfo (0x78 ) //private AttackableTargetInfo m_AttackableInfo
#define Class_AttackableTarget__m_Health (0x34 ) //protected float m_Health
#define Class_AttackableTarget__m_MaxHealth (0x38 ) //protected float m_MaxHealth

#define m_IsInEM3Eye (0x170) //protected bool m_IsInEM3Eye
#define Class_Camera_get_aspect (m_unity + 0x5022768)
#define Class_Camera_set_aspect (m_unity + 0x5022784)
#define Class_Time_set_timeScale (m_unity + 0x509B320)
#define Class_Time_get_timeScale (m_unity + 0x5020738)
}


