#pragma once
#include "WeaponBase.h"
#include "Animator.h"

class WeaponGun : public WeaponBase {
public:
	WeaponGun();
	~WeaponGun();
	void Update() override;
	void DrawScreen() override;
	void Start() override;

	/// <summary>
	/// ����e�I�u�W�F�N�g�Ɋe��ݒ�l��ݒ肷��
	/// </summary>
	/// <param name="name">���b�V�����B���b�V�����Ȃ��Ƃ���""</param>
	/// <param name="meshNoIn">���b�V���ԍ�</param>
	/// <param name="boneNoIn">�{�[���ԍ�</param>
	/// <param name="offsetIn">�I�t�Z�b�g</param>
	/// <param name="posIn">�ړ������l</param>
	/// <param name="rotIn">��]�����l</param>
	void SetWeaponGun(std::string name, int meshNoIn, int boneNoIn, VECTOR3 offsetIn = VECTOR3(0,0,0),
											VECTOR3 posIn=VECTOR3(0,0,0), VECTOR3 rotIn=VECTOR3(0,0,0));

	/// <summary>
	/// �e�𔭎˂���
	/// </summary>
	/// <param name="owner">�I�[�i�[</param>
	/// <returns>���˂ł�����true</returns>
	bool ShotBullet(WeaponBase::OwnerID owner);

	/// <summary>
	/// ���[�U�[�𔭎˂���
	/// </summary>
	/// <param name="owner">�I�[�i�[</param>
	/// <returns>���˂ł�����true</returns>
	bool ShotLaser(WeaponBase::OwnerID owner);

	/// <summary>
	/// ����e�̂��̃t���[���ʒu�ł̃��[���h�}�g���b�N�X�𓾂�
	/// </summary>
	/// <returns>����e�̃��[���h�}�g���b�N�X</returns>
	MATRIX4X4 GetFrameMatrix();

private:
	int  meshNo;
	int  boneNo;
	MATRIX4X4 adjustMatrix;
	MATRIX4X4 worldMatrix;
	VECTOR3 offset;
	VECTOR3 startPos;
	float bulletTimer;

};