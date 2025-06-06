#pragma once
#include "Object3D.h"

class Camera : public Object3D {
public:
	Camera();
	~Camera();
	void Update() override;

	VECTOR3 GetForwardVector();
	VECTOR3 GetRotation();

private:
	VECTOR3 lookPosition;

	int viewType; // 視点切り替えの番号

	POINT MoucePoint;//今のフレームのマウス位置

	POINT MouceDiff;//全開のフレームとの差分

	VECTOR3 ForwardVector;//カメラの前方ベクトル

	float changeTime; // 視点切り替え時間を数える
	VECTOR3 changePosStart; // 視点移動の開始点
	VECTOR3 changePosGoal; // 視点移動の終了点 
	VECTOR3 changeLookStart; // 注視点移動の開始点
	VECTOR3 changeLookGoal; // 注視点移動の終了点 
};