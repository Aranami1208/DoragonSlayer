#include "EventBase.h"
#include "Player.h"
#include "MapManager.h"
#include "Item.h"

namespace {
	const int DelayTime = 60;	 // �폜�܂ł̑҂�����
};

EventBase::EventBase()
{
	tag = "";
	needEnter = false;
	onceOn = false;
	eventcycle = ecStandby;
	countTime = 0;
}

EventBase::~EventBase()
{
}

void EventBase::Update()
{
	switch (eventcycle)
	{
	case ecStandby:
		// �X�^���o�C��ԂȂ̂�Player�Ƃ̐ڐG������s��
		if (HitCheck())
		{
			if (needEnter)
			{
				// Enter�L�[���K�v�ȂƂ���ecWait�ɂ���
				eventcycle = ecWait;
			}
			else {
				// Enter�L�[�s�v�ȂƂ��́A���s��ecEnd�ɂ���
				UpdateEvent();
				countTime = DelayTime;	 // �폜�҂����Ԃ�ݒ�
				eventcycle = ecEnd;
			}
		}
		break;

	case ecWait:
		// ������Enter�L�[�҂��Ǝ��s�B���s��ecEnd�ɂ���
		if (GameDevice()->m_pDI->CheckKey(KD_TRG, DIK_RETURN))
		{
			UpdateEvent();
			countTime = DelayTime; 	 // �폜�҂����Ԃ�ݒ�
			eventcycle = ecEnd;
		}
		else {
			if (!HitCheck())
			{
				// �ڐG�����ꂽ�̂ŁAecStandby�ɖ߂�B
				eventcycle = ecStandby;
				countTime = 0;
			}
		}
		break;

	case ecEnd:
		if (countTime == 0) UpdateOnce();  // ��x����̂Ƃ��A�폜�҂����ԂɒB�����̂ō폜����
		if (!HitCheck())
		{
			// �ڐG�����ꂽ
			if( countTime > 0 )	 UpdateOnce(); // ��x����̂Ƃ��A�܂��폜���ԂɒB���Ă��Ȃ������̂ō폜����
			countTime = 0;
			eventcycle = ecStandby;	// ecStandby�ɖ߂�B
		}
		else {
			countTime--;
		}
		break;
	}
}
void EventBase::UpdateOnce()
{
	if (onceOn)	   // ��x����̂Ƃ�
	{
		DeleteEvent();	// �C�x���g�Ώۂ̃I�u�W�F�N�g���폜�܂��͔�\����
		DestroyMe();	// �������폜
	}
}

bool EventBase::HitCheck()
{
	if (tag == "")	return false;

	Player* pc = ObjectManager::FindGameObject<Player>();

	// �}�b�v�Ƃ̐ڐG����
	MapStage* ms = ObjectManager::FindGameObjectWithTag<MapStage>(tag);
	if (ms != nullptr)
	{
		if (ms->Mesh() == nullptr) return false;
		if (ms->MeshCol() == nullptr)
		{
			ms->CreateMeshcolliderFromMesh(ms->Mesh());  // ���b�V���R���C�_�[�𐶐�����
		}
		if (ms->HitSphereToMesh(pc->Collider()))
		{
			return true;
		}
		return false;
	}
	else {
		MapTree* mt = ObjectManager::FindGameObjectWithTag<MapTree>(tag);
		if (mt != nullptr)
		{
			if (mt->Mesh() == nullptr) return false;
			if (mt->MeshCol() == nullptr)
			{
				mt->CreateMeshcolliderFromMesh(mt->Mesh());  // ���b�V���R���C�_�[�𐶐�����
			}
			if (mt->HitSphereToMesh(pc->Collider()))
			{
				return true;
			}
			return false;
		}
	}

	// �ړ��}�b�v�Ƃ̐ڐG����
	MapMove* mm = ObjectManager::FindGameObjectWithTag<MapMove>(tag);
	if (mm != nullptr)
	{
		if (mm->ActiveOn())
		{
			VECTOR3 hit, normal;
			VECTOR3 posStart = VECTOR3(0, pc->Collider().radius, -pc->Collider().radius) * pc->Matrix();
			VECTOR3 posEnd = VECTOR3(0, -pc->Collider().radius, pc->Collider().radius) * pc->Matrix();
			if (mm->ColMoveMesh()->IsCollisionSphere(posStart, posEnd, pc->Collider().radius, hit, normal))
			{
				return true;
			}
		}
		return false;
	}

	// Item�Ƃ̐ڐG����
	Item* it = ObjectManager::FindGameObjectWithTag<Item>(tag);
	if (it != nullptr)
	{
		if (it->MeshCol() != nullptr)
		{
 			if (it->HitSphereToMesh(pc->Collider()))
			{
				return true;
			}
			return false;
		}
		else if (it->Radius() > 0)
		{
			if (magnitude(pc->Position() - it->Position()) < it->Radius())
			{
				return true;
			}
			return false;
		}
	}

	return false;
}

void EventBase::DeleteEvent()
{
	if (tag == "")	return;

	// �X�e�[�W�}�b�v�Ǝ��}�b�v�̍폜�͍s��Ȃ�!!
	
	// �ړ��}�b�v�̔�\��������
	MapMove* mm = ObjectManager::FindGameObjectWithTag<MapMove>(tag);
	if (mm != nullptr)
	{
		mm->SetActiveOn(false);
	}

	// Item�̍폜����
	Item* it = ObjectManager::FindGameObjectWithTag<Item>(tag);
	if (it != nullptr)
	{
		it->DestroyMe();
	}

}


SphereCollider EventBase::Collider()
{
	SphereCollider col;
	col.radius = 1.5f;
	col.center = transform.position + VECTOR3( 0, col.radius, 0 );
	return col;
}

void EventBase::DrawScreen()
{
	Object3D::DrawScreen();

	if (eventcycle == ecWait)  // ������Enter�L�[�҂��̕\���B
	{
		DrawPushEnter();
	}
}

// Enter�L�[�҂��̕\��
void EventBase::DrawPushEnter()
{
	Player* pc = ObjectManager::FindGameObject<Player>();
	VECTOR3 pos = pc->Position();
	pos.y += pc->Collider().center.y + pc->Collider().radius + 0.7f;
	GameDevice()->m_pFont->Draw3D(pos, "Push Enter Key!!", VECTOR2(1.0f, 0.2f), RGB(255, 0, 0), 1.0f, _T("HGP�n�p�p�޼��UB"));
}