#include "EnemyManager.h"
#include "EnemyBase.h"
#include "Player.h"
#include "DataCarrier.h"


EnemyBase::EnemyBase()
{
	state = stNormal;
	speedY = 0;
	navigationArea = false;
	target = VECTOR3(0, 0, 0);
	hitPoint = 0;
	flashTimer= 0;
	idleTimer= 0;
}

EnemyBase::~EnemyBase()
{
	navigationMap.clear();
	navigationMap.shrink_to_fit();
}

void EnemyBase::DrawScreen()
{
	Object3D::DrawScreen();
	DisplayName();
}

void EnemyBase::MakeNavigationMap(bool isArea, std::vector<VECTOR3> nvIn)
{
	navigationArea = isArea;
	navigationMap.clear();
	navigationMap.shrink_to_fit();
	for (const VECTOR3& nv : nvIn)
	{
		navigationMap.emplace_back(nv);
	}
	if (navigationMap.size() > 0)
	{
		if (navigationArea)
		{
			transform.position = GetNextAreaTarget();
			target = transform.position;
		}
		else {
			transform.position = navigationMap[0];
			target = transform.position;
		}
	}
}

void EnemyBase::AddDamage(float damage, VECTOR3 pPos)
{
	if (state != stNormal )	  return;	  // �����ԈȊO�͓����蔻��͂Ȃ��i���G��ԁj

	hitPoint -= damage;
	if (hitPoint > 0) {
		// �܂��g�o���c���Ă���Ƃ�
		VECTOR3 push = transform.position - pPos;	// ��ԕ����̃x�N�g�������
		push.y = 0;
		push = XMVector3Normalize(push) * 0.4f;	// ���̃x�N�g���̒������ړ�������������
		transform.position += push;	// ������transform.position�Ɉړ��x�N�g����������
		transform.rotation.y = atan2f(-push.x, -push.z);   // �ړ�������������
		state = stDamage;  	// �_���[�W��Ԃɂ���
	}
	else {
		// �g�o���O�ɂȂ����Ƃ�
		state = stDead;  	// ���S��Ԃɂ���
	}
}

VECTOR3 EnemyBase::GetNextAreaTarget()
{
	VECTOR3 target = VECTOR3(0,0,0);

	// ���̖ړI�n�����߂�Ƃ��̍Œ዗�������߂�
	// navigationMap[0]��Min���AnavigationMap[1]��Max�������Ă���
	float lx = navigationMap[1].x - navigationMap[0].x;	        // �w�����̒���
	float lz = navigationMap[1].z - navigationMap[0].z;	        // �y�����̒���
	float len = (lx < lz) ? lx : lz;  // �������̒��������߂�
	len = len * 0.5f * 0.99f;        // �������̒����̔���������߂�
	if (len > 10.0f) len = 10.0f;    // �ő�10�����Œ዗���Ƃ���B

	// ���̖ړI�n�𗐐��ŋ��߂�
	do {
		target.x = (float)Random(navigationMap[0].x, navigationMap[1].x);	// ���̖ړI�n�itarget�j���Z�b�g����
		target.z = (float)Random(navigationMap[0].z, navigationMap[1].z);
	} while (magnitude(target - transform.position) < len);  // �ړI�n�����ݒn����len�ȓ��̂Ƃ��͂�蒼��

	return target;
}

bool EnemyBase::MoveToTarget(VECTOR3 target, float speed, float rotSpeed)
{
	const float NearLimit = 0.5f;

	VECTOR3 toTarget = target - transform.position;
	if (magnitude(toTarget) <= NearLimit) return true;	 // �ړI�n�ɓ��B

	// �����̐��ʂ̃x�N�g�������
	MATRIX4X4 myRot = XMMatrixRotationY(transform.rotation.y);
	VECTOR3 front = VECTOR3(0, 0, 1) * myRot;
	//transform.position += front; // �Ƃ肠����

	float forward = speed;

	// ���[�g�{�[���A�j���[�V�������s�����ǂ������[�g�A�j���[�V�����^�C�v���m�F����
	if (mesh->GetRootAnimType(animator->PlayingID()) != eRootAnimNone)
	{
		// ���[�g�{�[���A�j���[�V�����ł̑O�i�ړ��l
		forward = GetPositionVector(mesh->GetRootAnimUpMatrices(animator)).z;
	}
	transform.position += front * forward * 60 * SceneManager::DeltaTime(); // �Ƃ肠����
	VECTOR3 right = VECTOR3(1, 0, 0) * myRot;

	// �v���C���[�ւ̃x�N�g���̒����P�̂���
	VECTOR3 toTarget1 = XMVector3Normalize(toTarget);
	// ���ς���� �����ꂪcos��
	float ip = Dot(front, toTarget1);
	float rSpeed = rotSpeed * DegToRad * 60 * SceneManager::DeltaTime();
	if (ip >= cosf(rSpeed)) {
		transform.rotation.y = atan2f(toTarget.x, toTarget.z);
	}
	else if (Dot(right, toTarget) > 0) {
		transform.rotation.y += rSpeed; // �E�ɋȂ���
	}
	else {
		transform.rotation.y -= rSpeed; // ���ɋȂ���
	}

	return false;
}

bool EnemyBase::CheckReach(Object3D* player, float angle, float ReachDistLimit)
{
	auto* playObj = dynamic_cast<Player*>(player);
	if (playObj != nullptr && !playObj->IsNormal())	 // �v���C���[��Normal�ȊO�̂Ƃ��͑ΏۂƂ��Ȃ�
	{
		return false;
	}

	VECTOR3 toPlayer = player->Position() - transform.position;	   // �������猩���v���C���[�̈ʒu

	if (toPlayer.Length() < ReachDistLimit / 3)  return true; // �ߐڃ��~�b�g��1/3�܂ŋ߂Â����Ƃ��͖������ŋߐځB

	if (toPlayer.Length() < ReachDistLimit) {
		// �����̐��ʂ̃x�N�g�������
		VECTOR3 front = VECTOR3(0, 0, 1) * XMMatrixRotationY(transform.rotation.y);
		// �v���C���[�ւ̃x�N�g���̒����P�̂���
		VECTOR3 toPlayer1 = XMVector3Normalize(toPlayer);
		// ���ς���� �����ꂪcos��
		float ip = Dot(front, toPlayer1);
		if (ip >= cosf(angle * DegToRad)) {
			// ����ɓ�����
			return true;
		}
	}
	return false;
}


SphereCollider EnemyBase::Collider()
{
	SphereCollider col;
	col.radius = 1.5f;
	col.center = transform.position + VECTOR3( 0, col.radius, 0 );
	return col;
}

