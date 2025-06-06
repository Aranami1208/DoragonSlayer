#pragma once
#include "Animator.h"

#include "WeaponBase.h"
#include <string>
#include <list>

#include "WeaponFireBall.h"
#include "WeaponFireBall2.h"
#include "WeaponFlamethrower.h"
#include "WeaponSword.h"
#include "WeaponGun.h"
#include "WeaponBullet.h"
#include "WeaponLaser.h"

class WeaponManager : public WeaponBase
{
public:
	WeaponManager();
	~WeaponManager();

	/// <summary>
	/// ����̔�������
	/// </summary>
	/// <typeparam name="C">����N���X��</typeparam>
	/// <param name="ownerIn">�����I�[�i�[</param>
	/// <returns>�����ł����Ƃ��̓I�u�W�F�N�g,�ł��Ȃ��Ƃ���nullptr</returns>
	template<class C> C* Spawn( OwnerID owner)
	{
		C* obj = Instantiate<C>();
		if (obj == nullptr)  return nullptr;
		obj->SetOwner(owner);
		return obj;
	}

	/// <summary>
	/// ����̔�������
	/// </summary>
	/// <typeparam name="C">����N���X��</typeparam>
	/// <param name="startIn">�����ʒu</param>
	/// <param name="targetIn">�^�[�Q�b�g�ʒu</param>
	/// <param name="owner">�����I�[�i�[</param>
	/// <returns>�����ł����Ƃ��̓I�u�W�F�N�g,�ł��Ȃ��Ƃ���nullptr</returns>
	template<class C> C* Spawn(VECTOR3 startIn, VECTOR3 targetIn, OwnerID owner)
	{
		C* obj = Instantiate<C>();
		if (obj == nullptr)  return nullptr;
		obj->SetPosition(startIn);
		obj->SetTarget(targetIn);
		obj->SetOwner(owner);
		return obj;
	}

	/// <summary>
	/// ����̔�������
	/// �i��x�ɑ����̔��ˁj
	/// </summary>
	/// <typeparam name="C">����N���X��</typeparam>
	/// <param name="startIn">�����ʒu</param>
	/// <param name="targetIn">�^�[�Q�b�g�ʒu</param>
	/// <param name="owner">�����I�[�i�[</param>
	/// <returns>�����ł����Ƃ���true</returns>
	template<class C> bool SpawnMany(VECTOR3 startIn, VECTOR3 targetIn, OwnerID owner, int num = 5, float offset = 1.0f)
	{
		for (int i = 0; i < num; i++)
		{
			C* obj = Instantiate<C>();
			if (obj == nullptr)  return false;
			obj->SetPosition(startIn + VECTOR3( Randomf(-offset*0.5f, offset*0.5f), Randomf(-offset*0.5f, offset*0.5f), Randomf(-offset*0.5f, offset*0.5f)));
			obj->SetTarget(targetIn + VECTOR3( Randomf(-offset*0.8f, offset*0.8f), Randomf(-offset*0.8f, offset*0.8f),Randomf(-offset*0.8f, offset*0.8f)));
			obj->SetOwner(owner);
		}
		return true;
	}

	/// <summary>
	/// ����E�Ή����ˊ�̔�������
	/// </summary>
	/// <param name="startIn">�����ʒu</param>
	/// <param name="targetIn">�^�[�Q�b�g�ʒu</param>
	/// <param name="owner">�����I�[�i�[</param>
	/// <param name="tag">�^�O(in/out)</param>
	/// <returns>�����ł����Ƃ��̓I�u�W�F�N�g,�ł��Ȃ��Ƃ���nullptr</returns>
	Fluid* SpawnFlamethrower(VECTOR3 startIn, VECTOR3 targetIn, OwnerID owner, std::string& tag);


	/// <summary>
	/// ���b�V�����X�g���烁�b�V���𓾂�
	/// </summary>
	/// <param name="str">���b�V������</param>
	/// <returns>���b�V���A�h���X</returns>
	meshstruct* MeshList(std::string str)
	{
		for (meshstruct& ms : meshList)
		{
			if (str == ms.name) return &ms;
		}
		MessageBox(nullptr, "WeaponManager::MeshList()", _T("������ �w��̃��b�V�����̃��b�V���̓��b�V�����X�g�ɂ���܂��� ������"), MB_OK);
		return nullptr;
	}

private:
	std::list<meshstruct> meshList;
};