#include "ChangeMapScene.h"
#include "ChangeMapDraw.h"
#include "HDRManager.h"


ChangeMapScene::ChangeMapScene()
{
	Instantiate<ChangeMapDraw>();	 // 描画クラス
	SceneManager::ChangeScene("PlayScene");

	ObjectManager::FindGameObject<HDRManager>()->ResetLimitForce();   // HDRのリセット
}

ChangeMapScene::~ChangeMapScene()
{
}

void ChangeMapScene::Update()
{
}
