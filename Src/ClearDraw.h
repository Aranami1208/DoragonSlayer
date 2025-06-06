#pragma once
#include "Object3D.h"

class ClearDraw : public Object3D
{
public:
	ClearDraw();
	~ClearDraw();
	void Update() override;
	void DrawScreen() override;
private:
	float timer;
	CSpriteImage* image;
	CSpriteImage* imageScore;
	CSprite* sprite;
};
