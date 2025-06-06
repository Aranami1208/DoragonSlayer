#pragma once
#include "EnemyManager.h"
#include  "Displace.h"

//======================================================================
// 敵溶岩流　オブジェクトクラス
//======================================================================
class EnemyLava : public EnemyBase
{
public:
	EnemyLava();	// コンストラクタ
	virtual	~EnemyLava();	// デストラクタ
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