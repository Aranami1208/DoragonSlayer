#pragma once
#include "Animator.h"

#include "EffectBase.h"
#include <string>
#include <list>

#include "EffectBillboard.h"
#include "EffectBillfire.h"
#include "EffectParticle.h"


class EffectManager : public EffectBase
{
public:
	EffectManager();
	~EffectManager();

	/// <summary>
	/// ���ʂ̔�������
	/// </summary>
	/// <typeparam name="C">���ʃN���X��</typeparam>
	/// <param name="str">�G�t�F�N�g��</param>
	/// <returns>�����ł����Ƃ��̓I�u�W�F�N�g,�ł��Ȃ��Ƃ���nullptr</returns>
	template<class C> C* Spawn(std::string str = "")
	{
		C* obj = Instantiate<C>();
		if (obj == nullptr)  return nullptr;
		obj->SetEffectName(str);
		return obj;
	}

	/// <summary>
	/// ���ʂ̔�������
	/// </summary>
	/// <typeparam name="C">���ʃN���X��</typeparam>
	/// <param name="startIn">�����ʒu</param>
	/// <returns>�����ł����Ƃ��̓I�u�W�F�N�g,�ł��Ȃ��Ƃ���nullptr</returns>
	template<class C> C* Spawn(VECTOR3 startIn)
	{
		C* obj = Instantiate<C>();
		if (obj == nullptr)  return nullptr;
		obj->SetPosition(startIn);
		obj->SetEffectName("");
		return obj;
	}

	/// <summary>
	/// ���ʂ̔�������
	/// </summary>
	/// <typeparam name="C">���ʃN���X��</typeparam>
	/// <param name="str">�G�t�F�N�g��</param>
	/// <param name="startIn">�����ʒu</param>
	/// <returns>�����ł����Ƃ��̓I�u�W�F�N�g,�ł��Ȃ��Ƃ���nullptr</returns>
	template<class C> C* Spawn(std::string str, VECTOR3 startIn)
	{
		C* obj = Instantiate<C>();
		if (obj == nullptr)  return nullptr;
		obj->SetPosition(startIn);
		obj->SetEffectName(str);
		return obj;
	}

	/// <summary>
	/// ���ʂ̔�������
	/// </summary>
	/// <typeparam name="C">���ʃN���X��</typeparam>
	/// <param name="startIn">�����ʒu</param>
	/// <param name="normalIn">�@��</param>
	/// <returns>�����ł����Ƃ��̓I�u�W�F�N�g,�ł��Ȃ��Ƃ���nullptr</returns>
	template<class C> C* Spawn(VECTOR3 startIn, VECTOR3 normalIn)
	{
		C* obj = Instantiate<C>();
		if (obj == nullptr)  return nullptr;
		obj->SetPosition(startIn);
		obj->SetNormal(normalIn);
		obj->SetEffectName("");
		return obj;
	}

	/// <summary>
	/// ���ʂ̔�������
	/// </summary>
	/// <typeparam name="C">���ʃN���X��</typeparam>
	/// <param name="str">�G�t�F�N�g��</param>
	/// <param name="startIn">�����ʒu</param>
	/// <param name="normalIn">�@��</param>
	/// <returns>�����ł����Ƃ��̓I�u�W�F�N�g,�ł��Ȃ��Ƃ���nullptr</returns>
	template<class C> C* Spawn(std::string str, VECTOR3 startIn, VECTOR3 normalIn)
	{
		C* obj = Instantiate<C>();
		if (obj == nullptr)  return nullptr;
		obj->SetPosition(startIn);
		obj->SetNormal(normalIn);
		obj->SetEffectName(str);
		return obj;
	}

	BILLBOARDBASE* BillboardList(std::string str);
	PARTICLEBASE* ParticleList(std::string str);

private:
	std::list<BILLBOARDBASE> billboardList;
	std::list<PARTICLEBASE>  particleList;

};