#include "DataCarrier.h"


DataCarrier::DataCarrier()
{
	ObjectManager::DontDestroy(this);		// DataCarrier�͏�����Ȃ�
	ObjectManager::SetVisible(this, false);		// DataCarrier�͕\�����Ȃ�

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

