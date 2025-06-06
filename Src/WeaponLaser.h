#pragma once
#include "WeaponBase.h"
#include "Animator.h"


class WeaponLaser : public WeaponBase {
public:
	WeaponLaser();
	~WeaponLaser();
	void Update() override;
	void Start() override;
	void DrawDepth() override;
	void DrawScreen() override;

private:
	float farDistance;
};