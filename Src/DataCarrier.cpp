#include "DataCarrier.h"


DataCarrier::DataCarrier()
{
	ObjectManager::DontDestroy(this);		// DataCarrierは消されない
	ObjectManager::SetVisible(this, false);		// DataCarrierは表示しない

	currentScriptName = "";
	score = 0;
}

DataCarrier::~DataCarrier()
{
}

void DataCarrier::Start()
{

}

void DataCarrier::Update()
{

}

