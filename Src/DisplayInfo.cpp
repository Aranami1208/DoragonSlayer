#include "DisplayInfo.h"
#include "DataCarrier.h"
#include "Player.h"

namespace {
	const int ScoreMax = 1000;   // �X�R�A�o�[�̕\����̃X�R�A�̍ő�l
}

DisplayInfo::DisplayInfo()
{
	image = new CSpriteImage(_T("Data/Image/foregrd3.png"));
	sprite = new CSprite();

	SetPriority(-9999); 	// �Ō����O�ɏ�������
	SetDrawOrder(-100001);	// ��ԍŌ�(HDR����)�ɕ`�悷��
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
	// �e�����Ȃ�
}
void DisplayInfo::DrawScreen()
{
	float h = 0;
	TCHAR str[256];
	int   DestX, DestY;

	// �X�e�[�^�X�o�[�̕\��
	Player* obj = ObjectManager::FindGameObject<Player>();
	h = (float)obj->HpdivMax();
	if (h < 0) h = 0;

	// �g�o�Ǝc�@���̕\��
	DestX = 10;
	DestY = 10;
	sprite->Draw(image, DestX, DestY, 0, 0, 213, 31);
	sprite->Draw(image, DestX + 59, DestY + 6, 59, 32, (DWORD)(144 * h), 6);

	_stprintf_s(str, _T("%d"), obj->Number());
	GameDevice()->m_pFont->Draw(DestX + 6, DestY + 15, str, 16, RGB(255, 0, 0));
	_stprintf_s(str, _T("%06d"), obj->HitPoint());
	GameDevice()->m_pFont->Draw(DestX + 26, DestY + 16, str, 12, RGB(0, 0, 0));

	// �X�R�A�g�̕\��
	DestX = WINDOW_WIDTH - 213 - 10;
	DestY = 10;
	sprite->Draw(image, DestX, DestY, 0, 112, 213, 28);
	// �X�R�A�o�[�̕\��
	int score = ObjectManager::FindGameObject<DataCarrier>()->Score();  // �X�R�A
	h = (float)score / ScoreMax;
	if (h < 0) h = 0;
	if (h > 1) h = 1;
	sprite->Draw(image, DestX + 59, DestY + 6, 59, 142, (DWORD)(144 * h), 6);
	// �X�R�A���l�̕\��
	_stprintf_s(str, _T("%06d"), score);
	GameDevice()->m_pFont->Draw(DestX + 8, DestY + 17, str, 11, RGB(0, 0, 0));

}
