#pragma once
#include "WeaponBase.h"
#include "Animator.h"
#include "Fluid.h"

class WeaponFlamethrower : public WeaponBase {
public:
	WeaponFlamethrower();
	~WeaponFlamethrower();
	void Update() override;
	Fluid* Spawn( std::string& tag);
	void DrawDepth() override;
	void DrawScreen() override;

private:
	std::string tag;
	std::vector<SphereCollider> sphere;
	std::vector<VECTOR3> sphereOffset;
};