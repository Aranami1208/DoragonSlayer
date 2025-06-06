#include "WeaponManager.h"
#include "Player.h"
#include "EnemyManager.h"
#include "MapManager.h"
#include "EffectManager.h"
#include "AudioManager.h"

namespace {
	const float LifeDistance = 30.0f;
	const int   AttackPoint = 50;
};

WeaponLaser::WeaponLaser()
{
	mesh = ObjectManager::FindGameObject<WeaponManager>()->MeshList("Laser")->mesh;	  // WeaponManager�̃��b�V�����Q�Ƃ���
	farDistance = LifeDistance;		  // ����������ݒ肷��
	AudioManager::Audio("SeLaser")->Play();

}

WeaponLaser::~WeaponLaser()
{
}

void WeaponLaser::Start()
{
	transform.rotation = GetLookatRotateVector(transform.position, target);	  // �ړ�������ݒ肷��
}

void WeaponLaser::Update()
{
	VECTOR3 foward = VECTOR3(0, 0, LifeDistance);
	VECTOR3 positionNew = XMVector3TransformCoord(foward, transform.matrix());	  // ���[�U�[�̐�[�ʒu

	EffectManager* efm = ObjectManager::FindGameObject<EffectManager>();

	MeshCollider::CollInfo coll;
	if (HitCheckLine(transform.position, positionNew, &coll, AttackPoint))  // �����蔻��Ƒ���ւ̃_���[�W�Z�b�g
	{
		efm->Spawn<EffectParticle>( coll.hitPosition, coll.normal);		   //	�p�[�e�B�N���̔���
	}
	else {
		// �}�b�v�Ƃ̓����蔻��
		MapManager* mm = ObjectManager::FindGameObject<MapManager>();
		if (mm != nullptr) {
			VECTOR3 hit, normal;
			if (mm->IsCollisionLay(transform.position, positionNew, hit, normal))
			{
				efm->Spawn<EffectParticle>( hit, normal);
			}
		}
	}
}

void WeaponLaser::DrawDepth()
{
	// �e�����Ȃ�
}
void WeaponLaser::DrawScreen()
{	VECTOR3 save = GameDevice()->m_vLightDir;
	GameDevice()->m_vLightDir = VECTOR3(0, 0, 0);  // �A�e�̂Ȃ��`��ɂ���

	transform.scale = VECTOR3(1, 1, LifeDistance);	 // Z������LifeDistance�{�������΂�

	Object3D::DrawScreen();

	transform.scale = VECTOR3(1, 1, 1);	 	 // �P�{�ɖ߂�

	GameDevice()->m_vLightDir = save;		// �A�e�̂���`��ɖ߂�

	DestroyMe();     	// �P��ł�������
}
