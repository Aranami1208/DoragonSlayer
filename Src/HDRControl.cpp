#include "HDRManager.h"
#include "HDRControl.h"

HDRControl::HDRControl()
{
	ObjectManager::DontDestroy(this);		// ������Ȃ�
	ObjectManager::SetActive(this, false);		// �X�V���Ȃ�
	ObjectManager::SetDrawOrder(this, -100000);	// ��ԍŌ�ɕ`�揈������
}

HDRControl::~HDRControl()
{
}
void HDRControl::DrawDepth()
{
	// �e�̏����͂��Ȃ�
}
void HDRControl::DrawScreen()
{
	// HDR���~�b�g��1.0��菬�����Ƃ��́AHDR�}�b�v�e�N�X�`���[����HDR�����_�����O
	// �����_�[�^�[�Q�b�g���o�b�N�o�b�t�@�ɖ߂�
	if (ObjectManager::FindGameObject<HDRManager>()->Limit() < 1.0f)
	{
		ObjectManager::FindGameObject<HDRManager>()->Render();   // HDR�����_�����O
	}
}
