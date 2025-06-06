#include "WeaponManager.h"
#include "Player.h"
#include "EnemyManager.h"
#include "MapManager.h"
#include "EffectManager.h"

namespace {
	const int   AttackPoint = 200;
};

WeaponSword::WeaponSword()
{
	mesh = nullptr;
	meshNo = 0;
	boneNo = 0;
	adjustMatrix = XMMatrixIdentity();
	worldMatrix = XMMatrixIdentity();
	offset = VECTOR3(0,0,0);

}

WeaponSword::~WeaponSword()
{
}

void WeaponSword::SetWeaponSword(std::string name, int meshNoIn, int boneNoIn, VECTOR3 offsetIn, VECTOR3 posIn, VECTOR3 rotIn)
{
	if (name == "")
	{
		mesh = nullptr;
		offset = offsetIn;
	}else{
		mesh = ObjectManager::FindGameObject<WeaponManager>()->MeshList(name)->mesh;	  // WeaponManagerのメッシュを参照する
		offset = ObjectManager::FindGameObject<WeaponManager>()->MeshList(name)->offset + offsetIn;	  // WeaponManagerのオフセット値と引数値を加算する
	}
	meshNo = meshNoIn;
	boneNo = boneNoIn;
	adjustMatrix = XMMatrixRotationY(rotIn.y * DegToRad) * XMMatrixRotationX(rotIn.x * DegToRad) * XMMatrixRotationZ(rotIn.z * DegToRad);
	adjustMatrix = adjustMatrix * XMMatrixTranslationFromVector(posIn);
}

void WeaponSword::Start()
{
	if (Parent() == nullptr) DestroyMe();

}

MATRIX4X4 WeaponSword::GetFrameMatrix()
{
	Object3D* pObj = static_cast<Object3D*>(Parent());	// 親のオブジェクト
	if (pObj == nullptr)  return XMMatrixIdentity();
	MATRIX4X4 world = adjustMatrix * pObj->Mesh()->GetFrameMatrices(pObj->GetAnimator(), pObj->Matrix(), boneNo, meshNo);
	return world;
}

void WeaponSword::Update()
{
	EffectManager* efm = ObjectManager::FindGameObject<EffectManager>();

	Object3D* pObj = static_cast<Object3D*>(Parent());	// 親のオブジェクト
	if (pObj == nullptr)  return;
	worldMatrix = adjustMatrix * pObj->Mesh()->GetFrameMatrices(pObj->GetAnimator(), pObj->Matrix(), boneNo, meshNo);
	transform.position = GetPositionVector(worldMatrix);
	VECTOR3 end = XMVector3TransformCoord(offset, worldMatrix);

	MeshCollider::CollInfo coll;
	if (HitCheckLine(transform.position, end, &coll, AttackPoint))  // 当たり判定と相手へのダメージセット
	{
		efm->Spawn<EffectParticle>( coll.hitPosition, coll.normal);		   //	パーティクルの発生
	}
	else {
		// マップとの当たり判定
		MapManager* mm = ObjectManager::FindGameObject<MapManager>();
		if (mm != nullptr) {
			VECTOR3 hit, normal;
			if (mm->IsCollisionLay(transform.position, end, hit, normal))
			{
				efm->Spawn<EffectParticle>( hit, normal);
			}
		}
	}
}

void WeaponSword::DrawScreen()
{
	if (mesh != nullptr)
	{
		mesh->Render(worldMatrix);
	}

	// 剣のラインの表示
	VECTOR3 end = XMVector3TransformCoord(offset, worldMatrix);
	CSprite spr;
	spr.DrawLine3D(transform.position, end, RGB(255, 0, 0));

}
