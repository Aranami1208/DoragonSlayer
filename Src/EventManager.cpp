#include "EventManager.h"

EventManager::EventManager()
{
	ObjectManager::DontDestroy(this);		// ���̂͏�����Ȃ�
	ObjectManager::SetVisible(this, false);		// ���͕̂\�����Ȃ�
	mesh = nullptr;
	meshCol = nullptr;
}

EventManager::~EventManager()
{
}


void EventManager::MakeEvent(TextReader* txt, int n)
{
	std::string str = txt->GetString(n, 0);

	if (str == "EventRescue")
	{
		Instantiate<EventRescue>()->MakeRescue(txt, n);
	}else if (str == "EventChangeScript")
	{
		Instantiate<EventChangeScript>()->MakeChangeScript(txt, n);
	}else if (str == "EventMoveMap")
	{
		Instantiate<EventMoveMap>()->MakeMoveMap(txt, n);
	}
}
