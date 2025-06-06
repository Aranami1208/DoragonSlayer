#pragma once
#include "EffectBase.h"
#include <string>
#include <vector>

class EffectManager;
class EffectBillboard : public EffectBase
{
public:
	EffectBillboard();
	~EffectBillboard();
	void Update() override;
	void DrawDepth() override;
	void DrawScreen() override;
	void SetEffectName(std::string name);

private:
	BILLBOARDBASE* billB;
	VECTOR2        uvOffset;
	float          frame;

};