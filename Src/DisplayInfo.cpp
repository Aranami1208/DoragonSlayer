#include "DisplayInfo.h"
#include "DataCarrier.h"
#include "Player.h"

namespace {
	const int ScoreMax = 1000;   // スコアバーの表示上のスコアの最大値
}

DisplayInfo::DisplayInfo()
{
	image = new CSpriteImage(_T("Data/Image/foregrd3.png"));
	sprite = new CSprite();

	SetPriority(-9999); 	// 最後より一つ前に処理する
	SetDrawOrder(-100001);	// 一番最後(HDRより後)に描画する
}

DisplayInfo::~DisplayInfo()
{
	SAFE_DELETE(image);
	SAFE_DELETE(sprite);
}

void DisplayInfo::Update()
{
}

void DisplayInfo::DrawDepth()
{
	// 影を作らない
}
void DisplayInfo::DrawScreen()
{
	float h = 0;
	TCHAR str[256];
	int   DestX, DestY;

	// ステータスバーの表示
	Player* obj = ObjectManager::FindGameObject<Player>();
	h = (float)obj->HpdivMax();
	if (h < 0) h = 0;

	// ＨＰと残機数の表示
	DestX = 10;
	DestY = 10;
	sprite->Draw(image, DestX, DestY, 0, 0, 213, 31);
	sprite->Draw(image, DestX + 59, DestY + 6, 59, 32, (DWORD)(144 * h), 6);

	_stprintf_s(str, _T("%d"), obj->Number());
	GameDevice()->m_pFont->Draw(DestX + 6, DestY + 15, str, 16, RGB(255, 0, 0));
	_stprintf_s(str, _T("%06d"), obj->HitPoint());
	GameDevice()->m_pFont->Draw(DestX + 26, DestY + 16, str, 12, RGB(0, 0, 0));

	// スコア枠の表示
	DestX = WINDOW_WIDTH - 213 - 10;
	DestY = 10;
	sprite->Draw(image, DestX, DestY, 0, 112, 213, 28);
	// スコアバーの表示
	int score = ObjectManager::FindGameObject<DataCarrier>()->Score();  // スコア
	h = (float)score / ScoreMax;
	if (h < 0) h = 0;
	if (h > 1) h = 1;
	sprite->Draw(image, DestX + 59, DestY + 6, 59, 142, (DWORD)(144 * h), 6);
	// スコア数値の表示
	_stprintf_s(str, _T("%06d"), score);
	GameDevice()->m_pFont->Draw(DestX + 8, DestY + 17, str, 11, RGB(0, 0, 0));

}
