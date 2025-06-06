#include "BehaviourBase.h"

BehaviourBase::BehaviourBase()
{
	runIdx = -1;
	obj = nullptr;
	func = nullptr;
}
BehaviourBase::~BehaviourBase()
{
	for (auto& chi : child)
	{
		SAFE_DELETE(chi);
	}
}
