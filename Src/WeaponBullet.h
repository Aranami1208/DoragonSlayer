#pragma once
#include "WeaponBase.h"
#include "Animator.h"


class WeaponBullet : public WeaponBase {
public:
	WeaponBullet();
	~WeaponBullet();
	void Update() override;
	void Start() override;
	void DrawScreen() override;

private:
	float farDistance;
};