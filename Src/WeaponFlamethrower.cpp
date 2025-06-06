#include "WeaponManager.h"
#include "Player.h"
#include "EnemyManager.h"
#include "MapManager.h"
#include "EffectManager.h"

namespace {
	const int   AttackPoint = 300;
	const int   CubeSize = 3;
	const int   ScaleXY = 2;
	const int   ScaleZ = 4;
	const float Radius = 1.0f;
	const float Length = 8.0f;
};

WeaponFlamethrower::WeaponFlamethrower()
{
	tag = "";

	SphereCollider sc;
	for( int i=0; i * (Radius * 1.8f) < Length; i++)
	{
		sc.radius = Radius;
		sphere.emplace_back(sc);
		sphereOffset.emplace_back(VECTOR3(0, 0, i * (Radius * 1.8f)));
	}

	ObjectManager::SetDrawOrder(this, -200);   // 透明色は遅めに描画する
}

WeaponFlamethrower::~WeaponFlamethrower()
{
}


Fluid* WeaponFlamethrower::Spawn(std::string& tagIn)
{
	transform.scale = VECTOR3(ScaleXY, ScaleXY, ScaleZ);  // 火炎放射器の長さの拡大率を作成する
	transform.rotation = GetLookatRotateVector(transform.position, target);	  // 移動方向を設定する
	transform.position = VECTOR3(0.0f, 0.0f, 0.3f) * transform.matrix();

	Fluid* obj = ObjectManager::FindGameObject<FluidManager>()->SetFluidObj(tagIn);
	tag = tagIn;
	return obj;
}


void WeaponFlamethrower::Update()
{

	// 銃から火炎放射を出す処理(流体処理)
	//   const MATRIX4X4& m_mWorld   :	発生位置のワールドマトリックス
	//   const float&     fCubeSize  :	表示用立方体の一辺の大きさ
	//   const float&     fColorNo   :	流体の色コード（0:炎（黒煙なし）1: 炎（黒煙あり）2:青い炎 3:濃い煙 4:薄い煙）
	//   const float&     fDensity   :	発生密度  2.5f 程度
	//   const float&     fVelocity  :	速度      600  程度
	//   const float&     fRandom    :	速度方向ランダム率(0.0f:一定方向 ～ 1.0f:方向がランダム)
	//   const int&    nAddVoxelWidth:	複数箇所から発生させるときのサイズ（ボクセル数）。１カ所の場合は０にする。
	//   std::string    tag:	設定するタグ（ｉｎ／ｏｕｔ）
	Fluid* obj = ObjectManager::FindGameObject<FluidManager>()->SpawnZ(transform, CubeSize, 1, 2.5f, 600, 0.1f, 0, tag);

	// 当たり判定球の設定処理
	int i = 0;
	for (SphereCollider& sc : sphere)
	{
		VECTOR3 scale = transform.scale;
		transform.scale = VECTOR3(1, 1, 1);
		sc.center = sphereOffset[i] * transform.matrix();
		transform.scale = scale;
		i++;
	}
	VECTOR3 end = VECTOR3(0, 0, 3) * transform.matrix();
	
	// 敵やＰＣとのあたり判定
	EffectManager* efm = ObjectManager::FindGameObject<EffectManager>();
	if (owner == ePC)
	{
		// 敵との当たり判定
		std::list<EnemyBase*> enemys = ObjectManager::FindGameObjects<EnemyBase>();
		for (EnemyBase* enm : enemys) {
			
			for (SphereCollider& sc : sphere)
			{
				MeshCollider::CollInfo coll;
				if (enm->HitSphereToMesh(sc, &coll)) {
					enm->AddDamage(AttackPoint, coll.hitPosition);
					//efm->Spawn<EffectBillboard>("Bom3", coll.hitPosition);	   // 爆発効果の発生
					efm->Spawn<EffectParticle>( coll.hitPosition, coll.normal);		   //	パーティクルの発生
					break;
				}
				
			}
		}
	}

	else if (owner == eENM) {
		// Playerとの当たり判定
		Player* pc = ObjectManager::FindGameObject<Player>();
		
		for (SphereCollider& sc : sphere)
		{
			MeshCollider::CollInfo coll;
			if (pc->HitSphereToMesh(sc, &coll)) {
				pc->AddDamage(AttackPoint, coll.hitPosition);
				//efm->Spawn<EffectBillboard>("Bom3", coll.hitPosition);	   // 爆発効果の発生
				efm->Spawn<EffectParticle>( coll.hitPosition, coll.normal);		   //	パーティクルの発生
				break;
			}
		}
		
	}
}

void WeaponFlamethrower::DrawDepth()
{
	// 影を作らない
}
void WeaponFlamethrower::DrawScreen()
{
	Fluid* obj = ObjectManager::FindGameObjectWithTag<Fluid>(tag);
	if (obj != nullptr)
	{
		obj->DrawScreen();
	}

	DestroyMe();   	// １回ですぐ消す
}
