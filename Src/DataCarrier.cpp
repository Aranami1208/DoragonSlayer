#include "DataCarrier.h"


DataCarrier::DataCarrier()
{
	ObjectManager::DontDestroy(this);		// DataCarrier‚ÍÁ‚³‚ê‚È‚¢
	ObjectManager::SetVisible(this, false);		// DataCarrier‚Í•\¦‚µ‚È‚¢

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

