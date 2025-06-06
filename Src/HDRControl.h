#pragma once
#include "Object3D.h"

class HDRControl : public Object3D {
public:
	HDRControl();
	~HDRControl();
	void DrawDepth() override;
	void DrawScreen() override;

private:


};