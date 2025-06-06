#pragma once
#include "Object3D.h"

class OverDraw : public Object3D
{
public:
	OverDraw();
	~OverDraw();
	void Update() override;
	void DrawScreen() override;
private: 
	float timer;
	CSpriteImage* image;
	CSpriteImage* imageScore;
	CSprite* sprite;
};
