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
	mesh = ObjectManager::FindGameObject<WeaponManager>()->MeshList("Bullet")->mesh;	  // WeaponManager�̃��b�V�����Q�Ƃ���
	farDistance = LifeDistance;		  // ����������ݒ肷��
	AudioManager::Audio("SeShot")->Play();
}

WeaponBullet::~WeaponBullet()
{
}

void WeaponBullet::Start()
{
	transform.rotation = GetLookatRotateVector(transform.position, target);	  // �ړ�������ݒ肷��
}

void WeaponBullet::Update()
{
	VECTOR3 positionOld = transform.position;

	VECTOR3 foward = VECTOR3(0, 0, MoveSpeed * 60 * SceneManager::DeltaTime());
	transform.position = XMVector3TransformCoord(foward, transform.matrix());

	EffectManager* efm = ObjectManager::FindGameObject<EffectManager>();

	MeshCollider::CollInfo coll;
	if (HitCheckLine(positionOld, transform.position, &coll, AttackPoint))  // �����蔻��Ƒ���ւ̃_���[�W�Z�b�g
	{
		efm->Spawn<EffectParticle>( coll.hitPosition, coll.normal);		   //	�p�[�e�B�N���̔���
		DestroyMe();
	}
	else {
		// �}�b�v�Ƃ̓����蔻��
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
		DestroyMe();     	// �����̍폜
	}
}

void WeaponBullet::DrawScreen()
{
	//VECTOR3 save = GameDevice()->m_vLightDir;
	//GameDevice()->m_vLightDir = VECTOR3(0, 0, 0);  // �A�e�̂Ȃ��`��ɂ���

	Object3D::DrawScreen();

	//GameDevice()->m_vLightDir = save;		// �A�e�̂���`��ɖ߂�
}
