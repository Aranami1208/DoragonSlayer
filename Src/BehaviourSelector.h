#pragma once
#include "BehaviourBase.h"

class BehaviourSelector : public BehaviourBase
{
public:
	BehaviourSelector(){};
	~BehaviourSelector() {};

	BtState Run() override;

};