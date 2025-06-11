#pragma once

#include "Object3D.h"
#include "Animator.h"
#include "WeaponManager.h"

class Camera;

class Player : public Object3D {
public:
	Player();
	~Player();
	void Update() override;
	void DrawScreen() override;

	SphereCollider Collider() override;
	void AddDamage(float damage, VECTOR3 pPos);
	int Number() { return number; }
	int HitPoint() { return hitPoint; }
	float HpdivMax();
	void SetMaxHp();
	bool IsNormal() { if (state == stNormal) return true; else return false; }

	float StaminaGaugeMax = 5000;
	float StaminaCost = 60;
	float CurrentStamina;
	float StaminaCoolTime = 60;
	float StaminaCoolCount = 0;

	
private:

	Camera* Cam;
	float   speedY; 
	VECTOR3 velocity;
	int     number;
	int     hitPoint;
	float   flashTimer;

	//Œü‚­–Ú•W‚ÌRotation 
	float targetRotY = 0.0f;
	//‰ñ“]‚É‚©‚¯‚éŠÔ
	float RotationTime = 0.15f;

	WeaponSword* swordObj;
	WeaponGun*   gunObj;
	std::string tag;

	enum AnimID {
		aIdle = 0,
		aRun = 1,
		aRoll,
		aAttack1,
		aAttack2,
		aAttack3,
	};

	enum State {
		stNormal = 0,
		stDamage,
		stDead,
		stFlash
	};

	const enum AtcState {
		atIdle = 0,
		atWalk,
		atAttack,
	};
	State state;
	AtcState atcstate;

	void updateNormal();
	void updateNormalWalk();
	void updateNormalAttack();
	void updateDamage();
	void updateDead();

	/// <summary>
	/// ˆÚ“®ˆ— T.Aranami 25/06/09
	/// </summary>
	/// <param name="Direction">“®‚­•ûŒü(Še—v‘f-‚P`‚P‚Ì”ÍˆÍ‚Å)</param>
	void move(VECTOR3 Direction);
};