#include "SceneFactory.h"
#include <windows.h>
#include <assert.h>
#include "TitleScene.h"
#include "PlayScene.h"
#include "ClearScene.h"
#include "OverScene.h"
#include "ChangeMapScene.h"
#include "DataCarrier.h"
#include "ShadowControl.h"
#include "HDRManager.h"

SceneBase* SceneFactory::CreateFirst()
{
 	ShadowControl* sc = SingleInstantiate <ShadowControl>();	// ShadowControl�͑S�Ă̍ŏ��ɍ쐬�����B��������Ȃ��BNoDestroy�B
	sc->SetShadow(false);
	SingleInstantiate <HDRManager>();	// HDR�͑S�Ă̍ŏ��ɍ쐬�����B��������Ȃ��BNoDestroy�B
	SingleInstantiate <DataCarrier>();	// DataCarrier�͑S�Ă̍ŏ��ɍ쐬�����B��������Ȃ��BNoDestroy�B

	return new TitleScene();
	//return nullptr;
}

SceneBase * SceneFactory::Create(const std::string & name)
{

	if (name == "TitleScene") {
		ObjectManager::FindGameObject<ShadowControl>()->SetShadow(false);
		return new TitleScene();
	}
	if (name.substr(0, 9) == "PlayScene") {
		ObjectManager::FindGameObject<ShadowControl>()->SetShadow(true);
		return new PlayScene();
	}
	if (name == "ClearScene") {
		ObjectManager::FindGameObject<ShadowControl>()->SetShadow(false);
		return new ClearScene();
	}
	if (name == "OverScene") {
		ObjectManager::FindGameObject<ShadowControl>()->SetShadow(false);
		return new OverScene();
	}
	assert(false);
	return nullptr;
}
