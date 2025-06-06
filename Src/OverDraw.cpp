#include "OverDraw.h"
#include "GameMain.h"

OverDraw::OverDraw()
{
	timer = 0;

	image = new CSpriteImage("Data/Image/Over.png");
	imageScore = new CSpriteImage("Data/Image/ClearScore.png");
	sprite = new CSprite;

}

OverDraw::~OverDraw()
{
	SAFE_DELETE(image);
	SAFE_DELETE(imageScore);
	SAFE_DELETE(sprite);
}

void OverDraw::Update()
{
}

void OverDraw::DrawScreen()
{
	sprite->Draw(image, 0, 0, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

	GameDevice()->m_pFont->Draw(363, 63, "ƒQ[ƒ€ƒI[ƒo[", 90, RGB(0, 0, 0), 1.0f, "HGP‘n‰pŠpÎß¯Ìß‘Ì");
	GameDevice()->m_pFont->Draw(360, 60, "ƒQ[ƒ€ƒI[ƒo[", 90, RGB(255, 0, 0), 1.0f, "HGP‘n‰pŠpÎß¯Ìß‘Ì");
	sprite->Draw(imageScore, WINDOW_WIDTH / 2 - 128, WINDOW_HEIGHT / 2 - 108, 0, 0, 256, 256);

	timer += 60 * SceneManager::DeltaTime();
	if ((int)timer % 8 < 4)
	{
		GameDevice()->m_pFont->Draw(500, 700, "Continue [T] Key!!", 40, RGB(255, 0, 0), 1.0f, "HGP‘n‰pŠpÎß¯Ìß‘Ì");
	}
	if (timer > 8) timer = 0;
}