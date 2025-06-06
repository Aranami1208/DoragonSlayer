#include "TitleScene.h"
#include "TitleDraw.h"
#include "GameMain.h"
#include "AudioManager.h"
#include "DataCarrier.h"
#include "HDRManager.h"

TitleScene::TitleScene()
{
	Instantiate<TitleDraw>();	// �^�C�g���`��N���X����

	DataCarrier* dc = ObjectManager::FindGameObject<DataCarrier>();
	dc->ClearScore();	  // �X�R�A���N�����[����

	ObjectManager::FindGameObject<HDRManager>()->ResetLimitForce();   // HDR�̃��Z�b�g
}

TitleScene::~TitleScene()
{
}

void TitleScene::Update()
{
	if (GameDevice()->m_pDI->CheckKey(KD_TRG, DIK_RETURN))
	{
		SceneManager::ChangeScene("PlayScene");
	}
}
