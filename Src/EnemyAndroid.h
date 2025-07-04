#pragma once 
#include "EnemyBase.h" 
#include "Animator.h" 
#include "WeaponManager.h" 
#include "Player.h" 
#include "Behaviour.h" 

class EnemyAndroid : public EnemyBase
{
public:
	EnemyAndroid();
	~EnemyAndroid();
	void Update() override;
	void DisplayName() override;
	SphereCollider Collider() override;
private:
	void makeBehaviour();
	void updateNormal();
	void updateDamage();
	void updateDead();
	int aroundID;
	WeaponSword* swordObj;
	Player* targetPlayer;

	BehaviourBase* rootNode;   // ビヘイビアツリー・ルートノード 

	/// <summary> 
	/// ビヘイビアツリーで使用するアクション関数 
	/// ・静的関数staticにすること 
	/// </summary> 
	/// <param name="objIn">自インスタンスのアドレス</param> 
	/// <returns>BtState</returns> 
	static BehaviourBase::BtState actionIsReach(Object3D* objIn);
	static BehaviourBase::BtState actionWalk(Object3D* objIn);
	static BehaviourBase::BtState actionIdle(Object3D* objIn);
	static BehaviourBase::BtState actionReach(Object3D* objIn);
	static BehaviourBase::BtState actionAttackSword(Object3D* objIn);
	//地面から炎がはえる攻撃
	static BehaviourBase::BtState actionAttackNeedleFire(Object3D* objIn);

	int FireTimer = 0;
	int FireInterval = 30;
	int FireNum = 3;
	int FireCount = 0;

	VECTOR3 TargetPos = VECTOR3(0,-5000,0);
};