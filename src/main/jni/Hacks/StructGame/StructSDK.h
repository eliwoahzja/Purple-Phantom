#pragma once

class Camera {
public:

    static Camera *get_main() {
        return reinterpret_cast<Camera *(__fastcall *)()>(
            Class_Camera__get_main
        )();
    }

    float get_aspect()
    {
        using Fn = float (__fastcall *)(Camera *);
        return reinterpret_cast<Fn>(Class_Camera_get_aspect)(this);
    }

    void set_aspect(float value)
    {
        using Fn = void (__fastcall *)(Camera *, float);
        reinterpret_cast<Fn>(Class_Camera_set_aspect)(this, value);
    }
};
Vector3 WorldToScreenPoint(Vector3 position) {
	return reinterpret_cast<Vector3 (__fastcall *)(Camera *, Vector3)>(Class_Camera__WorldToScreenPoint)(Camera::get_main(), position);
}


class Transform {
	public:
	Vector3 get_position() {
		return reinterpret_cast<Vector3(__fastcall *)(Transform *)>(Class_Transform__get_position)(this);
	}
	
};


class Component {
	public:
	Transform *get_transform() {
		return reinterpret_cast<Transform *(__fastcall *)(Component *)>(Class_Component__get_transform)(this);
	}
	
};

int get_width() {
	return reinterpret_cast<int(__fastcall *)()>(Class_Screen__get_width)();
}

int get_height() {
	return reinterpret_cast<int(__fastcall *)()>(Class_Screen__get_height)();
}

float get_density() {
	return reinterpret_cast<float(__fastcall *)()>(Class_Screen__get_density)();
}

// SkinHax runtime compatibility accessors. These mirror the known-good baseline
// and keep effect/attachment alias resolution on the same live weapon identity.
class Weapon {
public:
    int get_WeaponID() {
        using GetWeaponIDFunc = int (*)(Weapon*);
        auto fn = reinterpret_cast<GetWeaponIDFunc>(Weapon_GetWeaponId);
        return fn ? fn(this) : 0;
    }
};

class Pawn {
public:
    Weapon* get_CurrentWeapon() {
        using GetCurrentWeaponFunc = Weapon* (*)(Pawn*);
        auto fn = reinterpret_cast<GetCurrentWeaponFunc>(Pawn_get_CurrentWeapon);
        return fn ? fn(this) : nullptr;
    }
};

class GamePlay {
public:
    static Pawn* get_LocalPawn() {
        using GetLocalPawnFunc = Pawn* (*)();
        auto fn = reinterpret_cast<GetLocalPawnFunc>(Class_Gameplay__get_LocalPawn);
        return fn ? fn() : nullptr;
    }
};

