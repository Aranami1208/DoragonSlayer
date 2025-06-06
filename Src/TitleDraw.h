#pragma once
#include "Object3D.h"

class TitleDraw : public Object3D
{
public:
	TitleDraw();
	~TitleDraw();
	void Update() override;
	void DrawScreen() override;
private:
	float timer;
	CSpriteImage* image;
	CSprite* sprite;
};
