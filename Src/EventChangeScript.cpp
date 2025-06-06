#include "EventManager.h"
#include "PlayScene.h"
#include "DataCarrier.h"
#include "AudioManager.h"


EventChangeScript::EventChangeScript()
{
	scriptName = "";
}

EventChangeScript::~EventChangeScript()
{
}

void EventChangeScript::MakeChangeScript(TextReader* txt, int n)
{
	tag = txt->GetString(n, 1);
	scriptName = txt->GetString(n, 2);
	if (txt->GetColumns(n) > 3)
	{
		if (txt->GetString(n, 3) == "Once") onceOn = true;
	}
	if (txt->GetColumns(n) > 4)
	{
		if (txt->GetString(n, 4) == "Enter") needEnter = true;
	}
}

void EventChangeScript::UpdateEvent()
{
	DataCarrier* dc = ObjectManager::FindGameObject<DataCarrier>();
	PlayScene* ps = ObjectManager::FindGameObject<PlayScene>();

	dc->SetScriptName(scriptName);   // �f�[�^�[�L�����A�ɃX�N���v�g����ݒ�
	SceneManager::ChangeScene("PlayScene" + scriptName);   // PlayScene���`�F���W����

}
