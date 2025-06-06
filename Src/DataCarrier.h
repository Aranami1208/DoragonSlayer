#pragma once
#include "Object3D.h"

/// <summary>
/// ゲームの進行に伴う様々なデータを保存するクラス
/// １．スクリプト名
/// ２．スコア値
/// </summary>
class DataCarrier : public Object3D
{
public:
	DataCarrier();
	~DataCarrier();

	void Start() override;
	void Update() override;

	void AddScore(int inScore) { score += inScore; }
	void ClearScore() { score = 0; }
	int  Score() { return score; }
	void SetScriptName(std::string name) { currentScriptName = name; }
	std::string ScriptName() { return currentScriptName; }

private:
	std::string currentScriptName;		// 現在実行中のスクリプト名
	int score;		// スコア

};