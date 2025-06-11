#include "Player.h"
#include "../Libs/Imgui/imgui.h"
#include "EnemyManager.h"
#include "MapManager.h"
#include "WeaponManager.h"
#include "Camera.h"
namespace {
	const float Gravity = 0.025f; // �d�͉����x(���̒l)
	const float JumpPower = 0.5f;  // �W�����v�̏���
	const float RotationSpeed = 3; // ��]���x(�x)
	const float MoveSpeed = 0.1f;  // �ړ��X�s�[�h(���[�g�A�j���ȊO)
	const float ForwardPower = 1.0f; // �O�i�X�s�[�h�{��(���[�g�A�j�����܂�)
	const int   MaxNumber = 3;	   // �v���C���[�̐�
	const int   MaxHitPoint = 1000; // �̗�
	const float MaxFlashTime = 5;
	const float MaxDeadTime = 100;
};

Player::Player():
	Cam(nullptr)
{
	animator = new Animator(); // �C���X�^���X���쐬


// Solia
	mesh = new CFbxMesh();
	mesh->Load("Data/Char/Night/Night.mesh");
	mesh->LoadAnimation(aIdle, "Data/Char/Night/Idle.anmx", true);
	mesh->LoadAnimation(aRun, "Data/Char/Night/Walking.anmx", true);
	mesh->LoadAnimation(aRoll, "Data/Char/Night/Roll.anmx", false);
	mesh->LoadAnimation(aAttack1, "Data/Char/Night/AttackLight.anmx", false);
	swordObj = ObjectManager::FindGameObject<WeaponManager>()->Spawn<WeaponSword>(WeaponBase::ePC);	  // ���̔����@�@
	swordObj->SetWeaponSword("Sword", 0, 1, VECTOR3(0, 0, 0), VECTOR3(0.0f, 0.01f, -0.02f), VECTOR3(0.0f, 0.0f, -90.0f));  // ���̃��b�V���A���̈ʒu(0,43�͎��) ,�����̓��b�V���Ŏw��A�A�W���X�g�̈ʒu�Ɗp�x
	swordObj->SetParent(this);
	gunObj = ObjectManager::FindGameObject<WeaponManager>()->Spawn<WeaponGun>(WeaponBase::ePC);	  // �e�̔���
	//gunObj->SetWeaponGun("Pistol", 0, 43, VECTOR3(0,0,0), VECTOR3(0.0f, 0.0f, 0.0f),VECTOR3(180.0f, 0.0f, 90.0f));  // �e�̃��b�V���@�@���̈ʒu(0,43�͎��) ,�e���ʒu�̓��b�V���Ŏw��A�A�W���X�g�̈ʒu�Ɗp�x
	gunObj->SetWeaponGun("", 0, 1, VECTOR3(0, 0, 0));   // �e���b�V���Ȃ��A���̈ʒu(0,43�͎��) ,�e���ʒu��(0,0,0)
	gunObj->SetParent(this);



	animator->SetModel(mesh); // ���̃��f���ŃA�j���[�V��������
	animator->Play(aRun);
	animator->SetPlaySpeed(1.0f);

	meshCol = new MeshCollider();
	meshCol->MakeFromMesh(mesh, animator);

	transform.position = VECTOR3(0, 0, 0);
	transform.rotation = VECTOR3(0, 0, 0);
	state = stNormal;
	atcstate = atIdle;
	speedY = 0;
	number = MaxNumber;
	hitPoint = MaxHitPoint;
	flashTimer = 0;
	tag = "";
	CurrentStamina = StaminaGaugeMax;


	velocity = VECTOR3(0,0,0);

	SetDrawOrder(-10);

	swordObj->SetActive(false);

}

Player::~Player()
{
	SAFE_DELETE(mesh);
	SAFE_DELETE(meshCol);
}

float Player::HpdivMax()
{
	return (float)hitPoint / MaxHitPoint;
}

void Player::SetMaxHp()
{
	hitPoint = MaxHitPoint;
}

SphereCollider Player::Collider()
{
	// ���������߂̃o�E���f�B���O�{�[���Ƃ���
	SphereCollider col;
	col.radius = 0.5f;
	col.center = transform.position + VECTOR3(0, 0.7f, 0);
	return col;
}

void Player::Update()
{
	if (!Cam)
	{
		Cam = ObjectManager::FindGameObject<Camera>();//�J�������擾����
		return;
	}
	

	VECTOR3 positionOld = transform.position;
	velocity = VECTOR3(0, 0, 0);

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
	}

	/*
	ImGui::Begin("Sample");
	ImGui::InputInt("State", (int*)(&state));
	ImGui::InputFloat("SP", &speedY);
	ImGui::End();
	*/

	
	// Enemy�ɂ߂荞�܂Ȃ��悤�ɂ���
	std::list<EnemyBase*> enemys = ObjectManager::FindGameObjects<EnemyBase>();
	for (EnemyBase* &enm : enemys) {
		VECTOR3 push;
		if (enm->Mesh() == nullptr) continue;
		if( HitSphereToSpherePush(enm->Collider(), false, &push)){
			transform.position += push;
		}
	}
	

 	// �}�b�v�Ƃ̐ڐG����Ǝ��R��������
	transform.position.y += speedY;		 // ���R�������x��������
	speedY -= Gravity * 60 * SceneManager::DeltaTime();
	MapManager* mm = ObjectManager::FindGameObject<MapManager>();
	if( mm->IsCollisionMoveGravity(positionOld, transform.position) != clFall )
	{
		speedY = 0;
	}

	animator->Update(); // ���t���[���AUpdate���Ă�		 // -- 2024.9.5
}

void Player::updateNormal()
{
	switch (atcstate) {
	case atIdle:
		updateNormalWalk();
		break;
	case atWalk:
		updateNormalWalk();
		break;
	case atAttack:
		updateNormalAttack();
		break;
	}
}

void Player::updateNormalWalk()
{
	//���[�g�Q(�i�i���ړ��p)
	float root2 = 1/ sqrtf(2.0f);
	VECTOR3 Input(0.0f);

	if (GameDevice()->m_pDI->CheckKey(KD_DAT, DIK_W) || GameDevice()->m_pDI->CheckKey(KD_DAT, DIK_UP)) {
		//�O�i����
		Input.z += 1.0f;
	}
	if (GameDevice()->m_pDI->CheckKey(KD_DAT, DIK_S) || GameDevice()->m_pDI->CheckKey(KD_DAT, DIK_DOWN)) {
		// ���
		Input.z += -1.0f;
	}
	if (GameDevice()->m_pDI->CheckKey(KD_DAT, DIK_A) || GameDevice()->m_pDI->CheckKey(KD_DAT, DIK_LEFT)) {
		//���ɐi��
		Input.x += -1.0f;
	}

	if (GameDevice()->m_pDI->CheckKey(KD_DAT, DIK_D) || GameDevice()->m_pDI->CheckKey(KD_DAT, DIK_RIGHT)) {
		//�E�ɐi��
		Input.x += 1.0f;
	}

	if (GameDevice()->m_pDI->CheckKey(KD_DAT, DIK_LSHIFT)) {
		//����
		Input.x *= 1.5f;

		//�X�^�~�i����
		CurrentStamina = max(0.0f,CurrentStamina - StaminaCost);

		//�Q�[�W���g���؂�����N�[���^�C�����Z�b�g
		if (CurrentStamina < 0 && StaminaCoolCount == 0.0f)
		StaminaCoolCount = StaminaCoolTime;

	}
	else
	{
		StaminaCoolCount = max(0.0f, StaminaCoolCount--);
		if (StaminaCoolCount == 0.0f)
		CurrentStamina = min(StaminaGaugeMax, CurrentStamina + StaminaCost * 0.5f);
	}

	if(Input.Length() == 0.0f) {//�ړ����Ă��Ȃ���΍U�����J�n
		if (atcstate == atAttack)
		{
			animator->MergePlay(aAttack1);
		}
		else {
			animator->MergePlay(aIdle);
		}
	}

	//�΂ߓ��͂̎��͑��x��ς���
	if (Input.x != 0 && Input.z != 0)
	{
		Input.x *= root2;
		Input.z *= root2;
	}

	move(Input);

	//�ړ����Ă����ꍇ�A�L�����N�^�[�̌���������ݒ�
	if (velocity.Length() > 0.0f)
	{
		targetRotY = atan2f(velocity.x, velocity.z);

		float RotDiff = targetRotY - transform.rotation.y;

		//��]�������ŒZ�ɂ��邽�߂̏���
		while (RotDiff > XM_PI) RotDiff -= XM_2PI;
		while (RotDiff < -XM_PI) RotDiff += XM_2PI;

		//1�t���[���ŉ�]�����
		float RotationStep = (1.0f / RotationTime) * SceneManager::DeltaTime();

		if (fabs(RotDiff) < RotationStep)
		{
			transform.rotation.y = targetRotY;
		}
		else
		{
			transform.rotation.y += (RotDiff > 0) ? RotationStep : -RotationStep;
		}
	}


	if ( speedY == 0 && GameDevice()->m_pDI->CheckKey(KD_TRG, DIK_SPACE)) {	  // �W�����v�{�^��
		speedY = JumpPower;
	}

	if (GameDevice()->m_pDI->CheckKey(KD_DAT, DIK_X)) { // �V���b�g�U���{�^��
		gunObj->ShotLaser(WeaponBase::ePC);
	}

	if ( atcstate != atAttack && GameDevice()->m_pDI->CheckMouse(KD_TRG,DIM_LBUTTON)) { // ���U���{�^��
		animator->MergePlay(aAttack1);
		atcstate = atAttack;
		swordObj->SetActive(true);
	}
	
	if (GameDevice()->m_pDI->CheckKey(KD_TRG, DIK_Q)) { // �΂̋ʍU���{�^��
		VECTOR3 startIn = transform.position + VECTOR3(0, 1.0f, 0);
		VECTOR3 targetIn = XMVector3TransformCoord(VECTOR3(0, 1.0f, 30), transform.matrix());
		ObjectManager::FindGameObject<WeaponManager>()->SpawnMany<WeaponFireBall2>(startIn, targetIn, WeaponBase::ePC);
	}
	
	
	if (GameDevice()->m_pDI->CheckKey(KD_DAT, DIK_E)) { // �Ή����ˊ�U���{�^��
		VECTOR3 startIn = transform.position + VECTOR3(0, 1.0f, 0);
		VECTOR3 targetIn = XMVector3TransformCoord(VECTOR3(0, 1.0f, 30), transform.matrix());
		ObjectManager::FindGameObject<WeaponManager>()->SpawnFlamethrower(startIn, targetIn, WeaponBase::ePC, tag);
	}
	

	// -100m�ȉ��ɗ����Ă��܂����Ƃ���+2m�̈ʒu�ɖ߂�����
	if (transform.position.y < -100.0f)
	{
		transform.position.y = 2.0f;    // +2m�̈ʒu
		speedY = 0.0f;        // ���R�����̒�~
	}

	transform.position += velocity;	   // �����x��������
}


void Player::updateNormalAttack()
{
	if (animator->Finished()) {
		atcstate = atIdle;
		swordObj->SetActive(false);
	}
}

void Player::updateDamage()
{
	state = stFlash;
	flashTimer = MaxFlashTime;
}

void Player::updateDead()
{
	if (animator->Finished())
	{
		if (--number <= 0) // �o�b����l���炷
		{
			// �o�b���S�Ď��S������Q�[���I�[�o�[��ʂ�
			SceneManager::ChangeScene("OverScene");
		}
		else {
			// ���A����
			hitPoint = MaxHitPoint;
			state = stFlash;
			flashTimer = MaxDeadTime;
		}
	}
}

void Player::move(VECTOR3 Direction)
{
	//�ړ����͂�����Ă��Ȃ���Ώ������Ȃ�
	if (Direction.Length() == 0.0f) return;

	VECTOR3 vec;

	// ���[�g�{�[���A�j���[�V�������s�����ǂ������[�g�A�j���[�V�����^�C�v���m�F����
	if (mesh->GetRootAnimType(animator->PlayingID()) == eRootAnimNone)
	{
		vec = Direction * MoveSpeed * 60 * SceneManager::DeltaTime();
	}
	else {
		// ���[�g�{�[���A�j���[�V�����ł̑O�i�ړ��l
		vec = GetPositionVector(mesh->GetRootAnimUpMatrices(animator)) * 60 * SceneManager::DeltaTime();
	}
	vec *= ForwardPower;	// �O�i�̃X�s�[�h�{�����|����
	MATRIX4X4 rotY = XMMatrixRotationY(Cam->GetRotation().y); // Y�̉�]�s��
	velocity += vec * rotY; // �L�����̌����Ă���ւ̈ړ���

	//�ړ��ʂ����Z
	transform.position += velocity;

	//�U�����͂���Ă���ꍇ�́AMergePlay
	if (atcstate == atAttack)
	{
		animator->MergePlay(aAttack1);
	}
	else {
		animator->MergePlay(aRun);
	}
}

void Player::AddDamage(float damage, VECTOR3 pPos)
{
	if (state != stNormal)	  return;  	  // �����ԈȊO�͓����蔻��͂Ȃ��i���G��ԁj

	hitPoint -= damage;
	if (hitPoint > 0) {
		// �܂��g�o���c���Ă���Ƃ�
		VECTOR3 push = transform.position - pPos;	// ��ԕ����̃x�N�g�������
		push.y = 0;
		push = XMVector3Normalize(push) * 0.4f;	// ���̃x�N�g���̒������ړ�������������
		transform.position += push;	// transform.position�Ɉړ��x�N�g����������
		transform.rotation.y = atan2f(-push.x, -push.z);   // �ړ�������������
		state = stDamage; 	// �_���[�W��Ԃɂ���
	}
	else {
		// �g�o���O�ɂȂ����Ƃ�
		//animator->Play(aDead);
		state = stDead;	 	// ���S��Ԃɂ���
	}
}

void Player::DrawScreen()
{
	Object3D::DrawScreen(); // �p�����̕`��֐����Ă�


}


