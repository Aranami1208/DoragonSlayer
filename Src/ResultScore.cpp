#include "ResultScore.h"
#include "DataCarrier.h"

namespace {
	const float scoreViewTime = 0.5f;
	const float scoreUpTime = 0.5f;
	const float scoreDoneTime = 3.0f;	  // スコアのカウント時間（秒）
	const VECTOR2 DrawPosition = VECTOR2(580, 300);
};

ResultScore::ResultScore()
{
	viewScore = 0;
	timer = 0.0f;
	DataCarrier* dc = ObjectManager::FindGameObject<DataCarrier>();
	scoreUpNum = (dc->Score() / (SceneManager::DeltaTime() * 60 * 60)) / scoreDoneTime;
	if (scoreUpNum < 1)  scoreUpNum = 1;
}

ResultScore::~ResultScore()
{
}

void ResultScore::Update()
{
	timer += SceneManager::DeltaTime();
	DataCarrier* dc = ObjectManager::FindGameObject<DataCarrier>();
	if (timer >= scoreUpTime+scoreViewTime) {
		if (viewScore < dc->Score())
			viewScore += scoreUpNum;
	}
	if (viewScore >= dc->Score()) {
		viewScore = dc->Score();
	}
}

void ResultScore::DrawScreen()
{
	if (timer >= scoreViewTime) {
		char str[64]; // 文字列を用意
		GameDevice()->m_pFont->Draw(DrawPosition.x, DrawPosition.y, "スコア", 32, RGB(255, 255, 255));
		sprintf_s<64>(str, "SCORE: %6d", viewScore);
		GameDevice()->m_pFont->Draw(DrawPosition.x, DrawPosition.y + 100, str, 32, RGB(255, 255, 255));
	}
}