#pragma once
#include "Object3D.h"

class ResultScore : public Object3D {
public:
	ResultScore();
	~ResultScore();
	void Update() override;
	void DrawScreen() override;
private:
	int viewScore; // 表示するスコア
	float timer;
	int   scoreUpNum;   // 増分値
};