#include "PlayScene.h"
#include "Player.h"
#include "MapManager.h"
#include "EnemyManager.h"
#include "WeaponManager.h"
#include "EffectManager.h"
#include "EventManager.h"
#include "FluidManager.h"
#include "Item.h"
#include <fstream>
#include "CsvReader.h"
#include "TextReader.h"
#include <assert.h>
#include "Camera.h"
#include "AudioManager.h"
#include "DisplayInfo.h"
#include "DataCarrier.h"

namespace {
	const std::string FirstScriptName = "Data/Script/MapField.txt";		// 最初に読み込むスクリプト名
	//const std::string FirstScriptName = "Data/Script/MapDungeon.txt";		// 最初に読み込むスクリプト名
};

PlayScene::PlayScene()
{
	// 1つだけ作られるマネージャーオブジェクト。DontDestroy
	SingleInstantiate<FluidManager>();
	SingleInstantiate<EnemyManager>();
	SingleInstantiate<WeaponManager>();
	SingleInstantiate<EffectManager>();
	SingleInstantiate<EventManager>();

	// 毎回作成されるオブジェクト
	Instantiate<MapManager>();

	// スクリプト名が空白だった場合の処理
	DataCarrier* dc = ObjectManager::FindGameObject<DataCarrier>();
	if (dc->ScriptName() == "")
	{
		dc->SetScriptName(FirstScriptName);	   // 最初に読み込むスクリプトを設定する
	}

	// スクリプトファイルを読む
	TextReader* txt = new TextReader(dc->ScriptName());

	for (int i = 0; i < txt->GetLines(); i++) { // １行ずつ読む
		std::string str = txt->GetString(i, 0); // 処理対象が入る
		Object3D* obj = nullptr;

		if (str == "Player") {
			obj = Instantiate<Player>();
			VECTOR3 pos, rot;
			pos.x = txt->GetFloat(i, 1);
			pos.y = txt->GetFloat(i, 2);
			pos.z = txt->GetFloat(i, 3);
			obj->SetPosition(pos.x, pos.y, pos.z);
			rot.x = 0;
			rot.y = txt->GetFloat(i, 4);
			rot.z = 0;
			obj->SetRotation(rot.x, rot.y, rot.z);
		}
		else if (str.substr(0, 3) == "Map")
		{
			ObjectManager::FindGameObject<MapManager>()->MakeMap(txt, i);
		}
		else if (str.substr(0, 5) == "Enemy") {
			ObjectManager::FindGameObject<EnemyManager>()->Spawn(txt, i);
		}
		else if (str.substr(0, 5) == "Event") {
			ObjectManager::FindGameObject<EventManager>()->MakeEvent(txt, i);
		}
		else if (str.substr(0, 4) == "Item") {
			Item* obj = Instantiate<Item>();
			obj->MakeItem(txt, i);
		}
		else {
			assert(false);
		}
	}
	Instantiate<Camera>();
	Instantiate<DisplayInfo>();


	AudioManager::Audio("Bgm1")->Play(AUDIO_LOOP);

	SAFE_DELETE(txt);
}

PlayScene::~PlayScene()
{
}

void PlayScene::Update()
{
/*
	// Enemyが全滅したらClearScene
	if (ObjectManager::FindGameObject<EnemyManager>()->GetNumber() == 0) {
		SceneManager::ChangeScene("ClearScene");
	}
*/	
	if (GameDevice()->m_pDI->CheckKey(KD_TRG, DIK_R)) {
		SceneManager::ChangeScene("ClearScene");
	}
	if (GameDevice()->m_pDI->CheckKey(KD_TRG, DIK_T)) {
		SceneManager::ChangeScene("TitleScene");
	}
}
