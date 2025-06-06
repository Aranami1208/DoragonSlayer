#include "WeaponManager.h"
#include "Player.h"
#include "EnemyManager.h"
#include "MapManager.h"
#include "EffectManager.h"

namespace {
	const float MoveSpeed = 1.0f;
	const float LifeDistance = 60.0f;
	const int   WeaponFireMax = 10;
	const int   AttackPoint = 200;
};

WeaponFireBall2::WeaponFireBall2()
{
	mesh = ObjectManager::FindGameObject<WeaponManager>()->MeshList("FireBall2")->mesh;	  // WeaponManager�̃��b�V�����Q�Ƃ���
	farDistance = LifeDistance;		  // ����������ݒ肷��
}

WeaponFireBall2::~WeaponFireBall2()
{
	for (EffectFire& ef : efList)
	{
		ObjectManager::Destroy(ef.efObj); 	// �΂̋ʂ��͂މ��̍폜
	}
}

void WeaponFireBall2::Start()
{
	transform.rotation = GetLookatRotateVector(transform.position, target);	  // �ړ�������ݒ肷��

	EffectManager* efm = ObjectManager::FindGameObject<EffectManager>();
	EffectFire ef;
	for (int i = 0; i < WeaponFireMax; i++)		// �΂̋ʂ��͂މ�
	{
		efList.push_back(ef);
		efList.back().efObj = efm->Spawn<EffectBillfire>();
		efList.back().offset.x = Randomf(-0.2f, 0.2f);
		efList.back().offset.y = Randomf(-0.2f, 0.2f);
		efList.back().offset.z = Randomf(-0.4f, 0.4f);
		efList.back().efObj->SetParent(this);
	}
}

void WeaponFireBall2::Update()
{
	VECTOR3 positionOld = transform.position;

	VECTOR3 foward = VECTOR3(0, 0, MoveSpeed * 60 * SceneManager::DeltaTime());
	transform.position = XMVector3TransformCoord(foward, transform.matrix());

	EffectManager* efm = ObjectManager::FindGameObject<EffectManager>();

	MeshCollider::CollInfo coll;
	if (HitCheckLine(positionOld, transform.position, &coll, AttackPoint))  // �����蔻��Ƒ���ւ̃_���[�W�Z�b�g
	{
		efm->Spawn<EffectBillboard>("Bom3", coll.hitPosition);	   // �������ʂ̔���
		//efm->Spawn<EffectParticle>( coll.hitPosition, coll.normal);		   //	�p�[�e�B�N���̔���
		DestroyMe();
	}
	else {
		// �}�b�v�Ƃ̓����蔻��
		MapManager* mm = ObjectManager::FindGameObject<MapManager>();
		if (mm != nullptr) {
			VECTOR3 hit, normal;
			if (mm->IsCollisionLay(positionOld, transform.position, hit, normal))
			{
				efm->Spawn<EffectBillboard>("Bom3", hit);
				//efm->Spawn<EffectParticle>( hit, normal);

				DestroyMe();
			}
		}
	}

	// �΂̋ʂ��͂މ��̍X�V
	for ( EffectFire &ef : efList)
	{
		ef.efObj->SetPosition(transform.position + ef.offset);
	}

	farDistance -= MoveSpeed * 60 * SceneManager::DeltaTime();
	if (farDistance <= 0)
	{
		DestroyMe();     	// �����̍폜
	}
}

void WeaponFireBall2::DrawDepth()
{
	// �e�����Ȃ�
}
void WeaponFireBall2::DrawScreen()
{
	VECTOR3 save = GameDevice()->m_vLightDir;
	GameDevice()->m_vLightDir = VECTOR3(0, 0, 0);  // �A�e�̂Ȃ��`��ɂ���

	Object3D::DrawScreen();

	GameDevice()->m_vLightDir = save;		// �A�e�̂���`��ɖ߂�
}
