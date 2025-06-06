#pragma once
#include "BehaviourBase.h"

class BehaviourAction : public BehaviourBase
{
public:
	BehaviourAction() {};
	virtual ~BehaviourAction() {};
	BtState Run() override;
};