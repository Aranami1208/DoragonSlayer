#include "TitleDraw.h"
#include "GameMain.h"

TitleDraw::TitleDraw()
{
	timer = 0;

	image = new CSpriteImage("Data/Image/Title.png");
	sprite = new CSprite;
}

TitleDraw::~TitleDraw()
{
	SAFE_DELETE(image);
	SAFE_DELETE(sprite);
}

void TitleDraw::Update()
{
}

void TitleDraw::DrawScreen()
{
	sprite->Draw(image, 0, 0, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

	GameDevice()->m_pFont->Draw(183, 63, "‚R‚cƒAƒNƒVƒ‡ƒ“ƒQ[ƒ€", 90, RGB(0, 0, 0), 1.0f, "HGP‘n‰pŠpÎß¯Ìß‘Ì");
	GameDevice()->m_pFont->Draw(180, 60, "‚R‚cƒAƒNƒVƒ‡ƒ“ƒQ[ƒ€", 90, RGB(255, 0, 0), 1.0f, "HGP‘n‰pŠpÎß¯Ìß‘Ì");

	timer += 60 * SceneManager::DeltaTime();
	if ( (int)timer % 8 < 4 )
	{
		GameDevice()->m_pFont->Draw(500, 700, "Push Enter Key!!", 40, RGB(255, 0, 0), 1.0f, "HGP‘n‰pŠpÎß¯Ìß‘Ì");
	}
	if (timer > 8) timer = 0;
}