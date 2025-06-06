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
	/// 効果の発生処理
	/// </summary>
	/// <typeparam name="C">効果クラス名</typeparam>
	/// <param name="str">エフェクト名</param>
	/// <returns>発生できたときはオブジェクト,できないときはnullptr</returns>
	template<class C> C* Spawn(std::string str = "")
	{
		C* obj = Instantiate<C>();
		if (obj == nullptr)  return nullptr;
		obj->SetEffectName(str);
		return obj;
	}

	/// <summary>
	/// 効果の発生処理
	/// </summary>
	/// <typeparam name="C">効果クラス名</typeparam>
	/// <param name="startIn">発生位置</param>
	/// <returns>発生できたときはオブジェクト,できないときはnullptr</returns>
	template<class C> C* Spawn(VECTOR3 startIn)
	{
		C* obj = Instantiate<C>();
		if (obj == nullptr)  return nullptr;
		obj->SetPosition(startIn);
		obj->SetEffectName("");
		return obj;
	}

	/// <summary>
	/// 効果の発生処理
	/// </summary>
	/// <typeparam name="C">効果クラス名</typeparam>
	/// <param name="str">エフェクト名</param>
	/// <param name="startIn">発生位置</param>
	/// <returns>発生できたときはオブジェクト,できないときはnullptr</returns>
	template<class C> C* Spawn(std::string str, VECTOR3 startIn)
	{
		C* obj = Instantiate<C>();
		if (obj == nullptr)  return nullptr;
		obj->SetPosition(startIn);
		obj->SetEffectName(str);
		return obj;
	}

	/// <summary>
	/// 効果の発生処理
	/// </summary>
	/// <typeparam name="C">効果クラス名</typeparam>
	/// <param name="startIn">発生位置</param>
	/// <param name="normalIn">法線</param>
	/// <returns>発生できたときはオブジェクト,できないときはnullptr</returns>
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
	/// 効果の発生処理
	/// </summary>
	/// <typeparam name="C">効果クラス名</typeparam>
	/// <param name="str">エフェクト名</param>
	/// <param name="startIn">発生位置</param>
	/// <param name="normalIn">法線</param>
	/// <returns>発生できたときはオブジェクト,できないときはnullptr</returns>
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