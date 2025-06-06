#pragma once
#include "EffectBase.h"
#include <string>
#include <vector>

class EffectManager;
class EffectParticle : public EffectBase
{
public:
	EffectParticle();
	~EffectParticle();
	void Update() override;
	void Start() override;
	void DrawDepth() override;
	void DrawScreen() override;

	void SetEffectName(std::string name);
	void SetNormal(VECTOR3 normalIn) { normal = normalIn; }

private:
	PARTICLEBASE*  partB;
	int            maxParticle;
	PART*          ptArray;     // 一つのオブジェクトの中のパーティクル配列
	float          frame;
	VECTOR3        normal;

	void renderParticle(MATRIX4X4 W, MATRIX4X4 V, MATRIX4X4 P);

};