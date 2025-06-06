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
	/// 武器の発生処理
	/// </summary>
	/// <typeparam name="C">武器クラス名</typeparam>
	/// <param name="ownerIn">発生オーナー</param>
	/// <returns>発生できたときはオブジェクト,できないときはnullptr</returns>
	template<class C> C* Spawn( OwnerID owner)
	{
		C* obj = Instantiate<C>();
		if (obj == nullptr)  return nullptr;
		obj->SetOwner(owner);
		return obj;
	}

	/// <summary>
	/// 武器の発生処理
	/// </summary>
	/// <typeparam name="C">武器クラス名</typeparam>
	/// <param name="startIn">発生位置</param>
	/// <param name="targetIn">ターゲット位置</param>
	/// <param name="owner">発生オーナー</param>
	/// <returns>発生できたときはオブジェクト,できないときはnullptr</returns>
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
	/// 武器の発生処理
	/// （一度に多数の発射）
	/// </summary>
	/// <typeparam name="C">武器クラス名</typeparam>
	/// <param name="startIn">発生位置</param>
	/// <param name="targetIn">ターゲット位置</param>
	/// <param name="owner">発生オーナー</param>
	/// <returns>発生できたときはtrue</returns>
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
	/// 武器・火炎放射器の発生処理
	/// </summary>
	/// <param name="startIn">発生位置</param>
	/// <param name="targetIn">ターゲット位置</param>
	/// <param name="owner">発生オーナー</param>
	/// <param name="tag">タグ(in/out)</param>
	/// <returns>発生できたときはオブジェクト,できないときはnullptr</returns>
	Fluid* SpawnFlamethrower(VECTOR3 startIn, VECTOR3 targetIn, OwnerID owner, std::string& tag);


	/// <summary>
	/// メッシュリストからメッシュを得る
	/// </summary>
	/// <param name="str">メッシュ略名</param>
	/// <returns>メッシュアドレス</returns>
	meshstruct* MeshList(std::string str)
	{
		for (meshstruct& ms : meshList)
		{
			if (str == ms.name) return &ms;
		}
		MessageBox(nullptr, "WeaponManager::MeshList()", _T("■□■ 指定のメッシュ名のメッシュはメッシュリストにありません ■□■"), MB_OK);
		return nullptr;
	}

private:
	std::list<meshstruct> meshList;
};