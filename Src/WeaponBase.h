#pragma once
#include "Object3D.h"
#include "Animator.h"
#include "EffectBase.h"

class WeaponBase : public Object3D
{
public:
	// �I�[�i�[�̃I�u�W�F�N�g�敪
	const enum OwnerID {
		ePC = 0x00000001,
		eNPC = 0x00000002,
		eENM = 0x00000004,
		eOTHER = 0,
	};

	struct EffectFire
	{
		EffectBase* efObj;
		VECTOR3     offset;
		EffectFire() {
			efObj = nullptr;
			offset = VECTOR3(0, 0, 0);
		}
	};

protected:
	struct meshstruct {
		std::string name;
		CFbxMesh* mesh;
		VECTOR3   offset;
		meshstruct() {
			mesh = nullptr;
			offset = VECTOR3(0, 0, 0);
		}
	};

public:
	WeaponBase();
	virtual ~WeaponBase();
	virtual void SetOwner(OwnerID inowner) { owner = inowner; }

	void SetTarget(VECTOR3 targetIn) { target = targetIn; }

	virtual void SetActive(bool active);

	/// <summary>
	/// ����̃��C���ƑΏۂƂȂ�I�u�W�F�N�g�Ƃ̓����蔻��
	/// ���������Ƃ��͑Ώۂ̃_���[�WAddDamage()��ݒ�
	/// </summary>
	/// <param name="positionOld">��O�̈ʒu�i���ˈʒu�j</param>
	/// <param name="position">���݈ʒu�i���B�ʒu�j</param>
	/// <param name="coll">�q�b�g�����Ƃ��ɓ��������ʒu�̏��(Out)</param>
	/// <param name="AttackPoint">�U����</param>
	/// <returns>���������Ƃ�true</returns>
	virtual bool HitCheckLine(VECTOR3 positionOld, VECTOR3 position, MeshCollider::CollInfo* coll, int AttackPoint);

protected:
	OwnerID   owner;
	VECTOR3   target;

};