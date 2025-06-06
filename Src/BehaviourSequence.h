#pragma once
#include "BehaviourBase.h"

class BehaviourSequence : public BehaviourBase
{
public:
	BehaviourSequence(){};
	~BehaviourSequence() {};

	BtState Run() override;

};