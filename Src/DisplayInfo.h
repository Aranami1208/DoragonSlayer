#pragma once
#include "Object3D.h"

/// <summary>
/// 画面に各種情報を表示する処理
/// </summary>
class DisplayInfo : public Object3D
{
public:
	DisplayInfo();
	~DisplayInfo();

	void Update() override;
	void DrawDepth() override;
	void DrawScreen() override;
	CSpriteImage* Image(){return image;}
private:
	CSpriteImage* image;
	CSprite* sprite;
};