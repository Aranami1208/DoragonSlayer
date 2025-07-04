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
	float StaminaCost = 15;
	float CurrentStamina;
	float StaminaCoolTime = 30;
	float StaminaCoolCount = 0;

	//回避有効フレーム
	float EffectiveEvationFrame  = 5 ;
	//現在のフレーム
	float CurrentEvationFrame;
	
private:

	Camera* Cam;
	float   speedY; 
	VECTOR3 velocity;
	int     number;
	int     hitPoint;
	float   flashTimer;

	//向く目標のRotation 
	float targetRotY = 0.0f;
	//回転にかける時間
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
	/// 移動処理 T.Aranami 25/06/09
	/// </summary>
	/// <param name="Direction">動く方向(各要素-１〜１の範囲で)</param>
	void move(VECTOR3 Direction);
};