#include "ClearScene.h"
#include "ClearDraw.h"
#include "ResultScore.h"
#include "AudioManager.h"
#include "HDRManager.h"
#include "DataCarrier.h"

ClearScene::ClearScene()
{
	Instantiate<ClearDraw>();	 // �N�����[�V�[���̕`��N���X
	Instantiate<ResultScore>();
	AudioManager::Audio("Bgm1")->Stop();

	ObjectManager::FindGameObject<HDRManager>()->ResetLimitForce();   // HDR�̃��Z�b�g
}

ClearScene::~ClearScene()
{
}

void ClearScene::Update()
{
	if (GameDevice()->m_pDI->CheckKey(KD_TRG, DIK_T))
	{
		ObjectManager::FindGameObject<DataCarrier>()->SetScriptName("");   	 // �ŏ��̃X�N���v�g����J�n
		ObjectManager::FindGameObject<DataCarrier>()->ClearScore();			 // �X�R�A���N�����[����
		SceneManager::ChangeScene("TitleScene");
	}
	else if (GameDevice()->m_pDI->CheckKey(KD_TRG, DIK_RETURN))
	{
		PostQuitMessage(0);
	}
}
