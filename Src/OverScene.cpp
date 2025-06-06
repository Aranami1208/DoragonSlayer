#include "OverScene.h"
#include "OverDraw.h"
#include "ResultScore.h"
#include "AudioManager.h"
#include "HDRManager.h"
#include "DataCarrier.h"

OverScene::OverScene()
{
	Instantiate<OverDraw>();	  	 // �I�[�o�[�V�[���̕`��N���X
	Instantiate<ResultScore>();
	AudioManager::Audio("Bgm1")->Stop();

	ObjectManager::FindGameObject<HDRManager>()->ResetLimitForce();   // HDR�̃��Z�b�g
}

OverScene::~OverScene()
{
}

void OverScene::Update()
{
	if (GameDevice()->m_pDI->CheckKey(KD_TRG, DIK_T))
	{
		//ObjectManager::FindGameObject<DataCarrier>()->SetScriptName("");	 // �ŏ��̃X�N���v�g����J�n
		//ObjectManager::FindGameObject<DataCarrier>()->ClearScore();		 // �X�R�A���N�����[����
		SceneManager::ChangeScene("TitleScene");
	}
	else if (GameDevice()->m_pDI->CheckKey(KD_TRG, DIK_RETURN))
	{
		PostQuitMessage(0);
	}
}
