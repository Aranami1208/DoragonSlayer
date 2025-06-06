#pragma once
#include "Object3D.h"

class ChangeMapDraw : public Object3D
{
public:
	ChangeMapDraw();
	~ChangeMapDraw();
	void Update() override;
	void DrawScreen() override;
private:
	CSpriteImage* image;
	CSprite* sprite;
};
