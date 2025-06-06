#include "EventManager.h"

EventManager::EventManager()
{
	ObjectManager::DontDestroy(this);		// Ž©‘Ì‚ÍÁ‚³‚ê‚È‚¢
	ObjectManager::SetVisible(this, false);		// Ž©‘Ì‚Í•\Ž¦‚µ‚È‚¢
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
