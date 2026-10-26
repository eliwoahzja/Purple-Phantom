#pragma once

class Weapon {
public:

    int get_WeaponID() {
        using GetWeaponIDFunc = int (*)(Weapon*);
        static GetWeaponIDFunc _Weapon__get_WeaponID = (GetWeaponIDFunc)(_Weapon__get_WeaponID);
        return _Weapon__get_WeaponID ? _Weapon__get_WeaponID(this) : 0;
    }
    
}; 


class Pawn {
public:

    Weapon* get_CurrentWeapon() {
        using GetCurrentWeaponFunc = Weapon* (*)(Pawn*);
        static GetCurrentWeaponFunc _Pawn__get_CurrentWeapon = (GetCurrentWeaponFunc)(_Pawn__get_CurrentWeapon);
        return _Pawn__get_CurrentWeapon ? _Pawn__get_CurrentWeapon(this) : nullptr;
    }
    
};

class GamePlay {
public:
    static Pawn* get_LocalPawn() {
        using GetLocalPawnFunc = Pawn*(*)();
        static GetLocalPawnFunc _GamePlay__get_LocalPawn = (GetLocalPawnFunc)(_GamePlay__get_LocalPawn);
        return _GamePlay__get_LocalPawn ? _GamePlay__get_LocalPawn() : nullptr;
    }

};
