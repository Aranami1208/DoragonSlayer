#pragma once
#include "Object3D.h"

class ResultScore : public Object3D {
public:
	ResultScore();
	~ResultScore();
	void Update() override;
	void DrawScreen() override;
private:
	int viewScore; // �\������X�R�A
	float timer;
	int   scoreUpNum;   // �����l
};