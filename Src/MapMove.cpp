#include "MapManager.h"

namespace {
	const int   MaxDivide = 4096;
	const float MaxHeight = 0.08f;
};

MapMove::MapMove()
{
	ObjectManager::SetDrawOrder(this, 10);   // �������߂ɕ`�悷��

	moveFlag = 0;				// �ړ��敪    1:���s�ړ��@2:��]�@3:�g��k��
	activeOn = true;			// �����E�\��ON   false:�����E�\���Ȃ��@true:�����E�\������
	moveOn = false;				// �ړ�ON      false:�ړ���~�@true:�ړ����s
	up = VECTOR3(0,0,0);		// �ړ�����
	minOffset = VECTOR3(-9999, -9999, -9999);	// �ړ������ŏ��l
	maxOffset = VECTOR3(9999, 9999, 9999);	// �ړ������ő�l
	changeFlag = 0;		// �ړ������E�l�ɓ��B���ĕ���������ւ��Ƃ�(�o��)�@0:�ړ����@1:�����`�F���W
	loop = 1;			// �ړ����J��Ԃ���  0:�`�F���W�̉ӏ��Œ�~���ړ�ON��0:�ړ���~�ɖ߂��B 1:�ړ����J��Ԃ�

	mesh = new CFbxMesh();
	colMoveMesh = new CCollision;
}

MapMove::~MapMove()
{
	SAFE_DELETE(mesh);
	SAFE_DELETE(colMoveMesh);
}

void MapMove::MakeMoveMap(TextReader* txt, int n)
{
	VECTOR3 pos, rot;
	int colmn = txt->GetColumns(n);
	pos.x = txt->GetFloat(n, 1);
	pos.y = txt->GetFloat(n, 2);
	pos.z = txt->GetFloat(n, 3);
	rot.x = 0;
	rot.y = txt->GetFloat(n, 4);
	rot.z = 0;
	transform.position = pos;
	transform.rotation = rot * DegToRad;   // ���W�A���p�ɂ���

	std::string meshName = txt->GetString(n, 18);
	mesh->Load(meshName.c_str());

	if (colmn > 19)
	{
		std::string colName = txt->GetString(n, 19);
		if (colName != "")
		{
			colMoveMesh->AddFbxLoad(colName.c_str());
			colMoveMesh->InitWorldMatrix(transform.matrix());
		}
	}
	if (colmn > 20)	 // �f�B�X�v���C�X�����g�}�b�s���O
	{
		std::string disName = txt->GetString(n, 20);
		if (disName == "Displace")
		{
			mesh->m_iMaxDevide = MaxDivide;
			mesh->m_fHeightMax = MaxHeight;
		}
	}

	transformBase = transform;	// ��ʒu
	velocity = VECTOR3(0, 0, 0);
	tag = txt->GetString(n, 5);	  // �^�O��������
	SetTag(tag);
	if (txt->GetString(n, 7) == "Rot")
	{
		moveFlag = 2;				// �ړ��敪    1:���s�ړ��@2:��]�@3:�g��k��
	}
	else if(txt->GetString(n, 7) == "Scale")
	{
		moveFlag = 3;				// �ړ��敪    1:���s�ړ��@2:��]�@3:�g��k��
	}
	else {
		moveFlag = 1;				// �ړ��敪    1:���s�ړ��@2:��]�@3:�g��k��
	}
	if( txt->GetString(n, 6) == "ActiveOff"){
		activeOn = false;			// �����E�\��ON   false:�����E�\���Ȃ��@true:�����E�\������
	}
	else  if( txt->GetString(n, 6) == "MoveOn"){
		moveOn = true;				// �ړ�ON      false:�ړ���~�@true:�ړ����s
	}

	VECTOR3 work;
	work.x = txt->GetFloat(n, 8);
	work.y = txt->GetFloat(n, 9);
	work.z = txt->GetFloat(n, 10);
	up = work;		// ����

	work.x = txt->GetFloat(n, 11);
	work.y = txt->GetFloat(n, 12);
	work.z = txt->GetFloat(n, 13);
	minOffset = work;	// �����ŏ��l

	work.x = txt->GetFloat(n, 14);
	work.y = txt->GetFloat(n, 15);
	work.z = txt->GetFloat(n, 16);
	maxOffset = work;	// �����ő�l

	if (txt->GetString(n, 17) != "Loop")
	{
		loop = 0;
	}
	else {
		loop = 1;
	}
}

void MapMove::Update()
{
	colMoveMesh->SaveWorldMatrix();   // ���[���h�}�g���b�N�X��O�̑Ҕ�

	if (colMoveMesh != nullptr && moveOn)   // �ړ��}�b�v�̍X�V����
	{
		changeFlag = 0;      // �`�F���W�t���O�̃��Z�b�g
		if (moveFlag == 1)   // ���s�ړ��̏���
		{
			velocity += up * 60 * SceneManager::DeltaTime();    // �ړ������̏���
			if (up.x != 0)  // ���E�ɓ�����
			{
				if (checkLimitAndSet(minOffset.x, maxOffset.x, velocity.x, up.x) == 1) changeFlag = 1;
			}
			if (up.y != 0)  // �㉺�ɓ�����
			{
				if (checkLimitAndSet(minOffset.y, maxOffset.y, velocity.y, up.y) == 1) changeFlag = 1;
			}
			if (up.z != 0)  // �O��ɓ�����
			{
				if (checkLimitAndSet(minOffset.z, maxOffset.z, velocity.z, up.z) == 1) changeFlag = 1;
			}
			transform.position = transformBase.position+ velocity;
		}
		else if (moveFlag == 2) {   // ��]�̏���
			velocity += up * 60 * SceneManager::DeltaTime();    // ��]�����̏���
			if (up.x != 0)  // �w��]
			{
				if (checkLimitAndSet(minOffset.x, maxOffset.x, velocity.x, up.x) == 1) changeFlag = 1;
			}
			else if (up.y != 0)  // �x��]
			{
				if (checkLimitAndSet(minOffset.y, maxOffset.y, velocity.y, up.y) == 1) changeFlag = 1;
			}
			else if (up.z != 0)  // �y��]
			{
				if (checkLimitAndSet(minOffset.z, maxOffset.z, velocity.z, up.z) == 1) changeFlag = 1;
			}
			transform.rotation = transformBase.rotation + velocity * DegToRad;   // ���W�A���p�ɂ���
		}
		else if (moveFlag == 3) {   // �g��k���̏���
			velocity += up * 60 * SceneManager::DeltaTime();    // �g�k�����̏���
			VECTOR3 vScaleNow = transformBase.scale + velocity;     // ���݂̊g�嗦�𓾂�
			// �K��̊g��k���ɒB������
			if (up.x != 0 )
			{
				if (checkLimitAndSet(minOffset.x, maxOffset.x, vScaleNow.x, up.x) == 1) changeFlag = 1;
			}
			if (up.y != 0 )
			{
				if (checkLimitAndSet(minOffset.y, maxOffset.y, vScaleNow.y, up.y) == 1) changeFlag = 1;
			}
			if (up.z != 0 )
			{
				if (checkLimitAndSet(minOffset.z, maxOffset.z, vScaleNow.z, up.z) == 1) changeFlag = 1;
			}
			velocity = vScaleNow - transformBase.scale;
			transform.scale = transformBase.scale + velocity;
		}
		// ���[�v�����Ȃ��Ƃ��̒�~���f�B���]�̃^�C�~���O�Œ�~����
		if (loop == 0 && changeFlag == 1)
		{
			moveOn = false;    // �ړ����Ȃ�
		}
		colMoveMesh->SetWorldMatrix(transform.matrix());
	}
}

int  MapMove::checkLimitAndSet(const float minOff, const float maxOff, float& velocityIO, float& upIO)
{
	int change = 0;

	if (velocityIO < minOff )
	{
		velocityIO = minOff;
		if (upIO < 0) upIO *= -1;
		change = 1;
	}else if( velocityIO > maxOff)
	{
		velocityIO = maxOff;
		if (upIO > 0) upIO *= -1;
		change = 1;
	}
	return change;
}

void MapMove::DrawDepth()
{
	if (!activeOn) return;
	Object3D::DrawDepth();
}
void MapMove::DrawScreen()
{
	if (!activeOn) return;
	Object3D::DrawScreen();
}
