#include "EnemyManager.h"
#include "MapManager.h"
#include "Player.h"
#include "DataCarrier.h"
#include "DisplayInfo.h"

namespace {
	const float Gravity = 0.025f; // �d�͉����x(���̒l)
	const float MoveSpeed = 0.02f;
	const float RotSpeed = 3.0f;
	const float ReachAngle = 120.0f;       // ����
	const float ReachDistLimit = 10;       // �ߐ�Reach���E�l
	const float AttackDistLimit = 3.3f;       // �U���ڋߌ��E�l
	const int   MaxHitPoint = 700;
	const float MaxFlashTime = 5;
	const float MaxIdleTime = 480;
};

EnemyRS::EnemyRS()
{
	animator = new Animator(); // �C���X�^���X���쐬

	mesh = ObjectManager::FindGameObject<EnemyManager>()->MeshList("RS");	  // EnemyManager�̃��b�V�����Q�Ƃ���
	swordObj = ObjectManager::FindGameObject<WeaponManager>()->Spawn<WeaponSword>(WeaponBase::eENM);	  // ���E���̔���
	swordObj->SetWeaponSword("", 0, 48, VECTOR3(0, 1.7f, 0), VECTOR3(0, -0.03f, 0.74f), VECTOR3(-20.0f, 0.0f, -120.0f));
	swordObj->SetParent(this);


	animator->SetModel(mesh); // ���̃��f���ŃA�j���[�V��������
	animator->Play(aRun);
	animator->SetPlaySpeed(1.0f);

	meshCol = new MeshCollider();
	meshCol->MakeFromMesh(mesh, animator);

	state = stNormal;
	hitPoint = MaxHitPoint;

	flashTimer = 0;

	aroundID = 0;
	targetPlayer = nullptr;

	swordObj->SetActive(false);

	rootNode = nullptr;
	makeBehaviour();   // �r�w�C�r�A�c���[�̍\�z
}

EnemyRS::~EnemyRS()
{
	SAFE_DELETE(meshCol);
	SAFE_DELETE(rootNode);
}

SphereCollider EnemyRS::Collider()
{
	// ���������߂̃o�E���f�B���O�{�[���Ƃ���
	SphereCollider col;
	col.radius = 1.5f;
	col.center = transform.position + VECTOR3(0, col.radius, 0);
	return col;
}

void EnemyRS::DisplayName()
{
	// �G�̓���̃X�e�[�^�X�\��

	char name[] = "���{�b�g�q�r";
	//VECTOR2 dispSize = VECTOR2(3.0f, 1.0f);	// �\���̑傫���@��
	//VECTOR2 dispSize = VECTOR2(2.0f, 0.7f);		// �\���̑傫���@��
	VECTOR2 dispSize = VECTOR2(0.8f, 0.3f);	// �\���̑傫���@��

	// �\���ʒu
	VECTOR3 vEnmPos = transform.position;
	vEnmPos.y += mesh->m_vMax.y + dispSize.y * 0.7f;
	// �g�o�o�[�̕\��
	float fHp = (float)hitPoint / MaxHitPoint;
	CSprite sprite;
	sprite.Draw3D(ObjectManager::FindGameObject<DisplayInfo>()->Image(), vEnmPos, VECTOR2(dispSize.x * fHp, dispSize.y * 0.5f), VECTOR2(320, 96), VECTOR2(125 * fHp, 16));
	// �G�̖��O�\��
	vEnmPos.y += dispSize.y * 0.7f;
	GameDevice()->m_pFont->Draw3D(vEnmPos, name, dispSize, RGB(255, 0, 0), 1.0f, _T("HGP�n�p�p�޼��UB"));
}

void EnemyRS::Update()
{
	VECTOR3 positionOld = transform.position;

	switch (state) {
	case stFlash:
		flashTimer -= 60 * SceneManager::DeltaTime();
		if (flashTimer <= 0) state = stNormal;
		updateNormal();
		break;
	case stNormal:
		updateNormal();
		break;
	case stDamage:
		updateDamage();
		break;
	case stDead:
		updateDead();
		break;
	}

	// ����Enemy�ɂ߂荞�܂Ȃ��悤�ɂ���
	std::list<EnemyBase*> enemys = ObjectManager::FindGameObjects<EnemyBase>();
	for (EnemyBase*& enm : enemys) {
		if (enm->Mesh() == nullptr) continue;
		if (enm == this) continue;
		VECTOR3 push;
		if (HitSphereToSpherePush(enm->Collider(), false, &push)) {
			transform.position += push;
		}
	}

	// �}�b�v�Ƃ̐ڐG����Ǝ��R��������
	transform.position.y += speedY;	   // ���R�����̏���
	speedY -= Gravity * 60 * SceneManager::DeltaTime();
	MapManager* mm = ObjectManager::FindGameObject<MapManager>();
	if (mm->IsCollisionMoveGravity(positionOld, transform.position) != clFall)
	{
		speedY = 0;
	}

	animator->Update();	   // 2024.9.5
}

void EnemyRS::updateNormal()
{
	rootNode->Run();	// �r�w�C�r�A�c���[�̎��s
}

void EnemyRS::updateDamage()
{
	state = stFlash;
	flashTimer = MaxFlashTime;
}
void EnemyRS::updateDead()
{
	animator->MergePlay(aDead);
	if (animator->Finished()) {
		DataCarrier* dc = ObjectManager::FindGameObject<DataCarrier>();
		dc->AddScore(MaxHitPoint/10); // �ő�g�o��1/10���X�R�A�ɉ�����
		swordObj->SetActive(false);	  // ���̍폜����ɐe��������Ƃ��̑΍�
		ObjectManager::Destroy(swordObj);
		DestroyMe();
	}
}


// �r�w�C�r�A�c���[�̍\�z
void EnemyRS::makeBehaviour()
{
	rootNode = new BehaviourSequence();
	auto* node1 = rootNode->AddChild<BehaviourSelector>();
	auto* node2 = node1->AddChild<BehaviourSequence>();
	node2->AddChild<BehaviourAction>(this, actionIsReach);
	auto* node3 = node2->AddChild<BehaviourSelector>();
	node3->AddChild<BehaviourAction>(this, actionReach);
	node3->AddChild<BehaviourAction>(this, actionAttackSword);
	auto* node4 = node1->AddChild<BehaviourSelector>();
	node4->AddChild<BehaviourAction>(this, actionIdle);
	node4->AddChild<BehaviourAction>(this, actionWalk);
}


// �r�w�C�r�A�c���[�̃A�N�V�����֐�
// (static�֐��̂��ߎ��C���X�^���X�A�h���Xthis������objIn�Ŏ󂯎��)

BehaviourBase::BtState EnemyRS::actionIsReach(Object3D* objIn)
{
	auto* my = dynamic_cast<EnemyRS*>(objIn);

	// plyer������ɓ��������̃`�F�b�N
	Player* player = ObjectManager::FindGameObject<Player>();
	if (my->CheckReach(player, ReachAngle, ReachDistLimit))
	{
		my->targetPlayer = player;	// �^�[�Q�b�g�ƂȂ�o�b
		return BehaviourBase::bsTrue;	  // ����ɓ�������bsTrue
	}
	else {
		my->targetPlayer = nullptr;
	}
	return BehaviourBase::bsFalse;	  // ����ɓ���Ȃ�������bsFalse
}

BehaviourBase::BtState EnemyRS::actionIdle(Object3D* objIn)
{
	auto* my = dynamic_cast<EnemyRS*>(objIn);

	my->idleTimer -= 60 * SceneManager::DeltaTime();
	if (my->idleTimer <= 0 || my->targetPlayer != nullptr)
	{
		return BehaviourBase::bsFalse;	  // �A�C�h�����I�������bsFalse
	}
	my->animator->MergePlay(aIdle);
	return BehaviourBase::bsTrue;		  // �A�C�h������bsTrue
}

BehaviourBase::BtState EnemyRS::actionWalk(Object3D* objIn)   // ���[�g�ɏ]���Ĉړ�
{
	auto* my = dynamic_cast<EnemyRS*>(objIn);

	if (my->navigationArea)
	{
		if (my->MoveToTarget(my->target, MoveSpeed, RotSpeed)) // �G���A�̖ړI�n�ֈړ�
		{
			my->target = my->GetNextAreaTarget();
			my->animator->MergePlay(aIdle);
			my->idleTimer = MaxIdleTime;
			return BehaviourBase::bsFalse;	 // ���B������bsFalse
		}
	}
	else {
		my->target = my->navigationMap[my->aroundID];  // ���[�g�̖ړI�n
		if (my->MoveToTarget(my->target, MoveSpeed, RotSpeed)) // ���[�g�̖ړI�n�ֈړ�
		{
			my->aroundID = (my->aroundID + 1) % my->navigationMap.size();	 // ���B�����玟�̖ڕW�l��
			my->animator->MergePlay(aIdle);
			my->idleTimer = MaxIdleTime;
			return BehaviourBase::bsFalse;	 // ���B������bsFalse
		}
	}
	my->animator->MergePlay(aRun);
	return BehaviourBase::bsTrue;	   	 // �ړ���(���B���Ȃ�������)��bsTrue
}

BehaviourBase::BtState EnemyRS::actionReach(Object3D* objIn)   // Player�Ɍ������Ĉړ�
{
	auto* my = dynamic_cast<EnemyRS*>(objIn);

	if (my->targetPlayer == nullptr) {	// �^�[�Q�b�g�ƂȂ�o�b�������Ƃ�(���ʂ͂��蓾�Ȃ�)
		return BehaviourBase::bsTrue;
	}

	VECTOR3 toPlayer = my->targetPlayer->Position() - my->transform.position;	   // �������猩���v���C���[�̈ʒu
	if (magnitude(toPlayer) < AttackDistLimit)	 // �U�������ɓ�������
	{
		return BehaviourBase::bsFalse;		 // �U���Ɉڂ�Ƃ���bsFalse
	}
	else {
		// Reach�����̊O�ɏo����A���ɖ߂�
		if (toPlayer.Length() >= ReachDistLimit) {
			my->animator->MergePlay(aRun);
			return BehaviourBase::bsTrue;	 // Reach�I���Ƃ���bsTrue
		}
		my->MoveToTarget(my->targetPlayer->Position(), MoveSpeed, RotSpeed);   // �v���C���[�Ɍ�����
	}
	my->animator->MergePlay(aRun);
	return BehaviourBase::bsTrue;  		 // Reach�p���̂Ƃ���bsTrue
}

BehaviourBase::BtState EnemyRS::actionAttackSword(Object3D* objIn)	 // �U������
{
	auto* my = dynamic_cast<EnemyRS*>(objIn);

	my->swordObj->SetActive(true);
	my->animator->MergePlay(aAttack1);

	if (my->animator->Finished()) // �U���A�j���[�V�������I���H
	{
		my->animator->Play(aRun);
		my->swordObj->SetActive(false);
		return BehaviourBase::bsTrue;	  // �U���A�j���[�V����������ɏI�������Ƃ���bsTrue
	}

	return BehaviourBase::bsRunning;   // �U�����̂Ƃ���bsRunning
}