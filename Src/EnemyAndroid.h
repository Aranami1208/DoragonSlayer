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

	BehaviourBase* rootNode;   // �r�w�C�r�A�c���[�E���[�g�m�[�h 

	/// <summary> 
	/// �r�w�C�r�A�c���[�Ŏg�p����A�N�V�����֐� 
	/// �E�ÓI�֐�static�ɂ��邱�� 
	/// </summary> 
	/// <param name="objIn">���C���X�^���X�̃A�h���X</param> 
	/// <returns>BtState</returns> 
	static BehaviourBase::BtState actionIsReach(Object3D* objIn);
	static BehaviourBase::BtState actionWalk(Object3D* objIn);
	static BehaviourBase::BtState actionIdle(Object3D* objIn);
	static BehaviourBase::BtState actionReach(Object3D* objIn);
	static BehaviourBase::BtState actionAttackSword(Object3D* objIn);
};