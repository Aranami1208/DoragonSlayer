#pragma once
#include "EnemyManager.h"
#include  "Displace.h"

//======================================================================
// �G�n�◬�@�I�u�W�F�N�g�N���X
//======================================================================
class EnemyLava : public EnemyBase
{
public:
	EnemyLava();	// �R���X�g���N�^
	virtual	~EnemyLava();	// �f�X�g���N�^
	void Start() override;
	void Update() override;
	void DrawDepth() override;
	void DrawScreen() override;

	void SetBoundingBox(VECTOR3, VECTOR3);

private:
	CWave* lava;
	VECTOR3 boxMin;
	VECTOR3 boxMax;
	VECTOR3 posMin;
	VECTOR3 posMax;
	void updateNormal();
	bool hitCheck(Object3D*);
};