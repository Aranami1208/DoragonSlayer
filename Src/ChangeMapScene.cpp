#include "ChangeMapScene.h"
#include "ChangeMapDraw.h"
#include "HDRManager.h"


ChangeMapScene::ChangeMapScene()
{
	Instantiate<ChangeMapDraw>();	 // �`��N���X
	SceneManager::ChangeScene("PlayScene");

	ObjectManager::FindGameObject<HDRManager>()->ResetLimitForce();   // HDR�̃��Z�b�g
}

ChangeMapScene::~ChangeMapScene()
{
}

void ChangeMapScene::Update()
{
}
