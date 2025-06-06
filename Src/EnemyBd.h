#pragma once
#include "EnemyBase.h"
#include "Animator.h"
#include "Player.h"
#include "Behaviour.h"

class EnemyBd : public EnemyBase {
public:
	EnemyBd();
	~EnemyBd();
	void Update() override;
	void DisplayName() override;
	SphereCollider Collider() override;

private:
	void makeBehaviour();
	void updateNormal();
	void updateDamage();
	void updateDead();

	int aroundID;
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
	static BehaviourBase::BtState actionAttackFireBall(Object3D* objIn);
	static BehaviourBase::BtState actionAttackFlame(Object3D* objIn);
	static BehaviourBase::BtState actionAttackSelect(Object3D* objIn);

};