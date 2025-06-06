#include  "EnemyManager.h"
#include  "Player.h"

namespace {
	const int   AttackPoint = 30;
};

// �R���X�g���N�^
EnemyLava::EnemyLava()
{
	// �o�E���f�B���O�{�b�N�X�̑傫�� �ŏ��l�@VECTOR3 vMin(�ȗ��l VECTOR3(-50.0f, -10.0f, -50.0f))
	boxMin = VECTOR3(-50.0f, -10.0f, -50.0f);
	// �o�E���f�B���O�{�b�N�X�̑傫�� �ő�l�@VECTOR3 vMax(�ȗ��l VECTOR3(50.0f, 0.0f, 50.0f))
	boxMax = VECTOR3(50.0f, 0.0f, 50.0f);

	// ������
	posMin = VECTOR3(0,0,0);
	posMax = VECTOR3(0,0,0);
	lava = nullptr;
}

// �f�X�g���N�^
EnemyLava::~EnemyLava()
{
	SAFE_DELETE(lava);
}

void EnemyLava::Start()
{
	lava = new CWave(boxMax.x-boxMin.x, boxMax.z - boxMin.z, "Data/Map/MapItem/lava03_NM.png", "Data/Map/MapItem/lava03.png");
	lava->m_fMaxDistance = 40.0f;
	lava->m_fMinDistance = 20.0f;
	lava->m_fWaveHeight = 0.15f;
}

void EnemyLava::SetBoundingBox(VECTOR3 minIn, VECTOR3 maxIn)
{
	boxMin = minIn;
	boxMax = maxIn;
}

// �G�n�◬�I�u�W�F�N�g�̍X�V
void	EnemyLava::Update()
{
	// �X�e�[�^�X���Ƃ̏���  --------------------------------
	switch (state)
	{
	case  stFlash: // �G���_���[�W���畜�A�����Ƃ��̏����Bbreak�������Ɏ���NORMAL���s���B
	case  stNormal: // �G���ʏ��Ԃ̂Ƃ��̏���
		updateNormal();
		break;

	case  stDamage: // �G���_���[�W��Ԃ̂Ƃ��̏���
		state = stNormal;
		break;

	case  stDead:	// �G�����S��Ԃ̂Ƃ��̏���
		state = stNormal;
		break;
	}
}
void	EnemyLava::updateNormal()
{
	posMin = boxMin + transform.position;
	posMax = boxMax + transform.position;

	Player* pc = ObjectManager::FindGameObject<Player>();
	if (hitCheck(pc))
	{
		pc->AddDamage(AttackPoint, transform.position);
	}
	std::list<EnemyBase*> els = ObjectManager::FindGameObjects<EnemyBase>();
	for (EnemyBase* &em : els)
	{
		if (em == this) continue;
		if (hitCheck(em))
		{
			em->AddDamage(AttackPoint, transform.position);
		}
	}
}

bool	EnemyLava::hitCheck(Object3D* obj)
{
	if (obj->Position().x >= posMin.x && obj->Position().y >= posMin.y && obj->Position().z >= posMin.z
		&&
		obj->Position().x <= posMax.x && obj->Position().y <= posMax.y + lava->m_fWaveHeight && obj->Position().z <= posMax.z)
	{
		return true;
	}
	return false;
}

void EnemyLava::DrawDepth()
{
	// �e�����Ȃ�
}
void EnemyLava::DrawScreen()
{
	MATRIX4X4 mat = XMMatrixTranslation(posMin.x, posMax.y, posMin.z);
	lava->Render(mat);
}