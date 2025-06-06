#pragma once
#include "Object3D.h"
#include "Animator.h"

class EnemyBase : public Object3D
{
public:
	EnemyBase();
	virtual ~EnemyBase();

	virtual void DrawScreen() override;
	virtual void DisplayName(){};
	virtual SphereCollider Collider() override;

	virtual void MakeNavigationMap(bool isArea, std::vector<VECTOR3> nvIn);
	virtual VECTOR3 GetNextAreaTarget();
	virtual bool MoveToTarget(VECTOR3 target, float speed = 0.01f, float rotSpeed = 3.0f);
	virtual bool CheckReach(Object3D* object, float angle, float ReachDistLimit);
	virtual void AddDamage(float damage, VECTOR3 pPos);

protected:
	enum AnimID {
		aIdle = 0,
		aRun = 1,
		aDead,
		aAttack1
	};

	enum State {
		stNormal = 0, // �ʏ���(����)
		stDamage,     // �_���[�W
		stDead,       // ���S
		stFlash       // ���G���
	};
	State state;

	std::vector<VECTOR3> navigationMap;
	bool    navigationArea;
	VECTOR3 target;
	float   speedY;
	int     hitPoint;
	float   flashTimer; // ���G����
	float   idleTimer;  // �A�C�h������

};