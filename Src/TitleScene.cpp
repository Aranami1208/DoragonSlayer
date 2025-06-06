#include "TitleScene.h"
#include "TitleDraw.h"
#include "GameMain.h"
#include "AudioManager.h"
#include "DataCarrier.h"
#include "HDRManager.h"

TitleScene::TitleScene()
{
	Instantiate<TitleDraw>();	// タイトル描画クラス生成

	DataCarrier* dc = ObjectManager::FindGameObject<DataCarrier>();
	dc->ClearScore();	  // スコアをクリヤーする

	ObjectManager::FindGameObject<HDRManager>()->ResetLimitForce();   // HDRのリセット
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
