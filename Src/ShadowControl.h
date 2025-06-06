#pragma once
#include "Object3D.h"

class ShadowControl : public Object3D {
public:
	ShadowControl();
	~ShadowControl();
	void DrawDepth() override;
	void DrawScreen() override;
	void SetShadow(bool sdw);

private:


};