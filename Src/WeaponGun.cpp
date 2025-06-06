#include "WeaponManager.h"
#include "Player.h"
#include "EnemyManager.h"
#include "MapManager.h"
#include "EffectManager.h"

namespace {
	const float BulletWaitTime = 5;
};

WeaponGun::WeaponGun()
{
	mesh = nullptr;
	meshNo = 0;
	boneNo = 0;
	adjustMatrix = XMMatrixIdentity();
	worldMatrix = XMMatrixIdentity();
	offset = VECTOR3(0,0,0);
	bulletTimer = 0; 
}

WeaponGun::~WeaponGun()
{
}

void WeaponGun::SetWeaponGun(std::string name, int meshNoIn, int boneNoIn, VECTOR3 offsetIn, VECTOR3 posIn, VECTOR3 rotIn)
{
	if (name == "")
	{
		mesh = nullptr;
		offset = offsetIn;
	}else{
		mesh = ObjectManager::FindGameObject<WeaponManager>()->MeshList(name)->mesh;	  // WeaponManager�̃��b�V�����Q�Ƃ���
		offset = ObjectManager::FindGameObject<WeaponManager>()->MeshList(name)->offset + offsetIn;	  // WeaponManager�̃I�t�Z�b�g�l�ƈ����l�����Z����
	}
	meshNo = meshNoIn;
	boneNo = boneNoIn;
	adjustMatrix = XMMatrixRotationY(rotIn.y * DegToRad) * XMMatrixRotationX(rotIn.x * DegToRad) * XMMatrixRotationZ(rotIn.z * DegToRad);
	adjustMatrix = adjustMatrix * XMMatrixTranslationFromVector(posIn);
}

void WeaponGun::Start()
{
	if (Parent() == nullptr) DestroyMe();
}

MATRIX4X4 WeaponGun::GetFrameMatrix()
{
	Object3D* pObj = static_cast<Object3D*>(Parent());	// �e�̃I�u�W�F�N�g
	if (pObj == nullptr)  return XMMatrixIdentity();
	MATRIX4X4 world = adjustMatrix * pObj->Mesh()->GetFrameMatrices(pObj->GetAnimator(), pObj->Matrix(), boneNo, meshNo);
	return world;
}

void WeaponGun::Update()
{
	Object3D* pObj = static_cast<Object3D*>(Parent());	// �e�̃I�u�W�F�N�g
	if (pObj == nullptr)  return;
	worldMatrix = adjustMatrix * pObj->Mesh()->GetFrameMatrices(pObj->GetAnimator(), pObj->Matrix(), boneNo, meshNo);
	transform.position = GetPositionVector(worldMatrix);
	startPos = XMVector3TransformCoord(offset, worldMatrix);

	if( bulletTimer > 0 ) bulletTimer -= 60 * SceneManager::DeltaTime();
}

bool WeaponGun::ShotBullet(WeaponBase::OwnerID owner)
{
	bool ret = false;
	if (bulletTimer <= 0)
	{
		Object3D* pObj = static_cast<Object3D*>(Parent());	// �e�̃I�u�W�F�N�g
		WeaponBullet* ws = ObjectManager::FindGameObject<WeaponManager>()->Spawn<WeaponBullet>(owner);
		ws->SetPosition(startPos);
		MATRIX4X4 mat = GetRotateMatrix(pObj->Matrix()) * XMMatrixTranslationFromVector(startPos);	// �e���̈ʒu�Őe�I�u�W�F�N�g�̕����̃}�g���b�N�X
		ws->SetTarget( XMVector3TransformCoord( VECTOR3(0, 0, 1), mat ));
		bulletTimer = BulletWaitTime;
		ret = true;
	}
	return ret;
}

bool WeaponGun::ShotLaser(WeaponBase::OwnerID owner)
{
	Object3D* pObj = static_cast<Object3D*>(Parent());	// �e�̃I�u�W�F�N�g
	WeaponLaser* wl = ObjectManager::FindGameObject<WeaponManager>()->Spawn<WeaponLaser>(owner);
	wl->SetPosition(startPos);
	MATRIX4X4 mat = GetRotateMatrix(pObj->Matrix()) * XMMatrixTranslationFromVector(startPos);	// �e���̈ʒu�Őe�I�u�W�F�N�g�̕����̃}�g���b�N�X

	wl->SetTarget( XMVector3TransformCoord( VECTOR3(0, 0, 1), mat ));
	return true;
}

void WeaponGun::DrawScreen()
{
	if (mesh != nullptr)
	{
		mesh->Render(worldMatrix);
	}

	// ���C���̕\��
	VECTOR3 end = XMVector3TransformCoord(offset, worldMatrix);
	CSprite spr;
	spr.DrawLine3D(transform.position, end, RGB(255, 0, 0));

}
