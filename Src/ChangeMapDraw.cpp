#include "ChangeMapDraw.h"
#include "GameMain.h"

ChangeMapDraw::ChangeMapDraw()
{
	image = new CSpriteImage("Data/Image/Clear3.png");
	sprite = new CSprite;
}

ChangeMapDraw::~ChangeMapDraw()
{
	SAFE_DELETE(image);
	SAFE_DELETE( sprite);
}

void ChangeMapDraw::Update()
{
}

void ChangeMapDraw::DrawScreen()
{
	sprite->Draw(image, 0, 0, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	GameDevice()->m_pFont->Draw(480, 250, "Now ChangeMap!!", 48, RGB(255, 0, 0));
}