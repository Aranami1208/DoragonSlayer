#include "Camera.h"
#include "Player.h"
#include "MapManager.h"
//                                      ������_�@�@�@�@
static const VECTOR3 CameraPos = { VECTOR3(0, 2, -5)};
static const VECTOR3 LookPos =   { VECTOR3(0, 1,  5)};
static const float CHANGE_TIME_LIMIT = 0.5f; // �b

Camera::Camera()
{
	ObjectManager::SetVisible(this, false);		// ���͕̂\�����Ȃ�
	SetPriority(-10000); 	 // �Ō�ɏ�������
	viewType = 0;
	changeTime = CHANGE_TIME_LIMIT; // �؂�ւ�����
	LONG HalfHeight = 1080 / 2;
	LONG HalfWidth = 1920 / 2;

	MoucePoint.x = HalfWidth;
	MoucePoint.y = HalfHeight;

	SetCursorPos(HalfWidth, HalfHeight);
}

Camera::~Camera()
{
}

void Camera::Update()
{
	
	//���͂��擾 
	CDirectInput* DInput = GameDevice()->m_pDI;

	DInput->ShowMouseCursor(false);

	POINT CurrentPoint;

	GetCursorPos(&CurrentPoint);

	int HalfHeight = 1080 / 2;
	int HalfWidth = 1920 / 2;


	MouceDiff.x = HalfWidth - CurrentPoint.x;
	MouceDiff.y = HalfHeight - CurrentPoint.y;
	
	MoucePoint = CurrentPoint;
	SetCursorPos(HalfWidth, HalfHeight);

	VECTOR3 MoveRate = VECTOR3(0.01f,0.005f,0.0f);



	//�J�������[�e�[�V����
	transform.rotation = transform.rotation +VECTOR3(-MouceDiff.y * MoveRate.y, -MouceDiff.x * MoveRate.x, 0);

	

	// �v���C���[�̍s������߂�
	Player* player = ObjectManager::FindGameObject<Player>();
	MATRIX4X4 rot = XMMatrixRotationRollPitchYawFromVector(transform.rotation);
	//MATRIX4X4 trans = XMMatrixTranslation( player->Position().x, 0.0f, player->Position().z);
	MATRIX4X4 trans = XMMatrixTranslationFromVector( player->Position());
	MATRIX4X4 m = rot * trans;
	// �v���C���[����]�E�ړ����ĂȂ����̃J�����ʒu��
	// �v���C���[�̉�]�E�ړ��s����|����ƁA


	

	if (changeTime >= CHANGE_TIME_LIMIT) {
		transform.position = CameraPos * m;
		lookPosition = LookPos * m;
	}
	else { // ���_�؂�ւ���
		//changeTime += 1.0f / 60.0f;
		changeTime += 1.0f * SceneManager::DeltaTime();
		float timeRate = changeTime / CHANGE_TIME_LIMIT; // 0.0�`1.0
		float rate = timeRate;
		VECTOR3 position = (changePosGoal - changePosStart) * rate + changePosStart;
		VECTOR3 look = (changeLookGoal - changeLookStart) * rate + changeLookStart;
		transform.position = position * m;
		lookPosition = look * m;
	}


	// �J�������ǂɂ߂荞�܂Ȃ��悤�ɂ���
	VECTOR3 start = player->Position() + VECTOR3(0, 1.5f, 0);
	VECTOR3 end = transform.position;
	// start����end�Ɍ������x�N�g�������A������0.2��������
	VECTOR3 camVec = end - start;
	camVec = XMVector3Normalize(camVec) * (camVec.Length() + 0.2f);
	end = start + camVec;

	VECTOR3 hit, normal;
	if (ObjectManager::FindGameObject<MapManager>()->IsCollisionLay(start, end, hit, normal)) {
		end = hit;
	}
	//end����0.02��O�ɒu��;
	transform.position = XMVector3Normalize(camVec) * ((end - start).Length() - 0.02f) + start;

	//�����Ă���x�N�g�������߂�
	ForwardVector = lookPosition - transform.position;

	// ------------------------------------------------------------------
	// �J�������W��GameMain�ɐݒ肷��
	GameDevice()->m_vEyePt = transform.position; // �J�������W
	GameDevice()->m_vLookatPt = lookPosition; // �����_
	GameDevice()->m_mView = XMMatrixLookAtLH( 	    // �r���[�}�g���b�N�X
								transform.position,
								lookPosition,
								VECTOR3(0, 1, 0));
	GameDevice()->m_mDrawView = GameDevice()->m_mView;

	// ------------------------------------------------------------------------
	// ���C�g���_����̃r���[�g�����X�t�H�[���iShadowMap�p�j���C�g���_���o�b�̈ʒu�ɍ��킹��
	GameDevice()->SetLightView();


	// ------------------------------------------------------------------
	// ���_����̋����̂Q���DrawObject�ɐݒ肷��
	// ����́A���_����̋����̍~���ɕ`�悵��������
	std::list<Object3D*> objList = ObjectManager::FindGameObjects<Object3D>();
	for (Object3D*& obj : objList)
	{
		if (obj != this)
		{
			float distQ = magnitudeSQ(obj->Position() - transform.position);
			ObjectManager::SetEyeDist(obj, distQ);	 // ���_����̋����̂Q���DrawObject�ɐݒ�
		}
	}

}


VECTOR3 Camera::GetForwardVector()
{
	return XMVector3Normalize(ForwardVector);
}

VECTOR3 Camera::GetRotation()
{
	return transform.rotation;
}

