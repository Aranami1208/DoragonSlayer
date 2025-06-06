#include "WeaponManager.h"
#include "Player.h"
#include "EnemyManager.h"
#include "MapManager.h"
#include "EffectManager.h"
#include "AudioManager.h"

namespace {
	const float MoveSpeed = 2.0f;
	const float LifeDistance = 50.0f;
	const int   AttackPoint = 50;
};

WeaponBullet::WeaponBullet()
{
	mesh = ObjectManager::FindGameObject<WeaponManager>()->MeshList("Bullet")->mesh;	  // WeaponManagerのメッシュを参照する
	farDistance = LifeDistance;		  // 生存距離を設定する
	AudioManager::Audio("SeShot")->Play();
}

WeaponBullet::~WeaponBullet()
{
}

void WeaponBullet::Start()
{
	transform.rotation = GetLookatRotateVector(transform.position, target);	  // 移動方向を設定する
}

void WeaponBullet::Update()
{
	VECTOR3 positionOld = transform.position;

	VECTOR3 foward = VECTOR3(0, 0, MoveSpeed * 60 * SceneManager::DeltaTime());
	transform.position = XMVector3TransformCoord(foward, transform.matrix());

	EffectManager* efm = ObjectManager::FindGameObject<EffectManager>();

	MeshCollider::CollInfo coll;
	if (HitCheckLine(positionOld, transform.position, &coll, AttackPoint))  // 当たり判定と相手へのダメージセット
	{
		efm->Spawn<EffectParticle>( coll.hitPosition, coll.normal);		   //	パーティクルの発生
		DestroyMe();
	}
	else {
		// マップとの当たり判定
		MapManager* mm = ObjectManager::FindGameObject<MapManager>();
		if (mm != nullptr) {
			VECTOR3 hit, normal;
			if (mm->IsCollisionLay(positionOld, transform.position, hit, normal))
			{
				efm->Spawn<EffectParticle>( hit, normal);
				DestroyMe();
			}
		}
	}

	farDistance -= MoveSpeed * 60 * SceneManager::DeltaTime();
	if (farDistance <= 0)
	{
		DestroyMe();     	// 自分の削除
	}
}

void WeaponBullet::DrawScreen()
{
	//VECTOR3 save = GameDevice()->m_vLightDir;
	//GameDevice()->m_vLightDir = VECTOR3(0, 0, 0);  // 陰影のない描画にする

	Object3D::DrawScreen();

	//GameDevice()->m_vLightDir = save;		// 陰影のある描画に戻す
}
