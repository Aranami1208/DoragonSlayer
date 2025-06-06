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

	ObjectManager::SetDrawOrder(this, -200);   // �����F�͒x�߂ɕ`�悷��
}

WeaponFlamethrower::~WeaponFlamethrower()
{
}


Fluid* WeaponFlamethrower::Spawn(std::string& tagIn)
{
	transform.scale = VECTOR3(ScaleXY, ScaleXY, ScaleZ);  // �Ή����ˊ�̒����̊g�嗦���쐬����
	transform.rotation = GetLookatRotateVector(transform.position, target);	  // �ړ�������ݒ肷��
	transform.position = VECTOR3(0.0f, 0.0f, 0.3f) * transform.matrix();

	Fluid* obj = ObjectManager::FindGameObject<FluidManager>()->SetFluidObj(tagIn);
	tag = tagIn;
	return obj;
}


void WeaponFlamethrower::Update()
{

	// �e����Ή����˂��o������(���̏���)
	//   const MATRIX4X4& m_mWorld   :	�����ʒu�̃��[���h�}�g���b�N�X
	//   const float&     fCubeSize  :	�\���p�����̂̈�ӂ̑傫��
	//   const float&     fColorNo   :	���̂̐F�R�[�h�i0:���i�����Ȃ��j1: ���i��������j2:���� 3:�Z���� 4:�������j
	//   const float&     fDensity   :	�������x  2.5f ���x
	//   const float&     fVelocity  :	���x      600  ���x
	//   const float&     fRandom    :	���x���������_����(0.0f:������ �` 1.0f:�����������_��)
	//   const int&    nAddVoxelWidth:	�����ӏ����甭��������Ƃ��̃T�C�Y�i�{�N�Z�����j�B�P�J���̏ꍇ�͂O�ɂ���B
	//   std::string    tag:	�ݒ肷��^�O�i�����^�������j
	Fluid* obj = ObjectManager::FindGameObject<FluidManager>()->SpawnZ(transform, CubeSize, 1, 2.5f, 600, 0.1f, 0, tag);

	// �����蔻�苅�̐ݒ菈��
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
	
	// �G��o�b�Ƃ̂����蔻��
	EffectManager* efm = ObjectManager::FindGameObject<EffectManager>();
	if (owner == ePC)
	{
		// �G�Ƃ̓����蔻��
		std::list<EnemyBase*> enemys = ObjectManager::FindGameObjects<EnemyBase>();
		for (EnemyBase* enm : enemys) {
			
			for (SphereCollider& sc : sphere)
			{
				MeshCollider::CollInfo coll;
				if (enm->HitSphereToMesh(sc, &coll)) {
					enm->AddDamage(AttackPoint, coll.hitPosition);
					//efm->Spawn<EffectBillboard>("Bom3", coll.hitPosition);	   // �������ʂ̔���
					efm->Spawn<EffectParticle>( coll.hitPosition, coll.normal);		   //	�p�[�e�B�N���̔���
					break;
				}
				
			}
		}
	}

	else if (owner == eENM) {
		// Player�Ƃ̓����蔻��
		Player* pc = ObjectManager::FindGameObject<Player>();
		
		for (SphereCollider& sc : sphere)
		{
			MeshCollider::CollInfo coll;
			if (pc->HitSphereToMesh(sc, &coll)) {
				pc->AddDamage(AttackPoint, coll.hitPosition);
				//efm->Spawn<EffectBillboard>("Bom3", coll.hitPosition);	   // �������ʂ̔���
				efm->Spawn<EffectParticle>( coll.hitPosition, coll.normal);		   //	�p�[�e�B�N���̔���
				break;
			}
		}
		
	}
}

void WeaponFlamethrower::DrawDepth()
{
	// �e�����Ȃ�
}
void WeaponFlamethrower::DrawScreen()
{
	Fluid* obj = ObjectManager::FindGameObjectWithTag<Fluid>(tag);
	if (obj != nullptr)
	{
		obj->DrawScreen();
	}

	DestroyMe();   	// �P��ł�������
}
