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

	int viewType; // ���_�؂�ւ��̔ԍ�

	POINT MoucePoint;//���̃t���[���̃}�E�X�ʒu

	POINT MouceDiff;//�S�J�̃t���[���Ƃ̍���

	VECTOR3 ForwardVector;//�J�����̑O���x�N�g��

	float changeTime; // ���_�؂�ւ����Ԃ𐔂���
	VECTOR3 changePosStart; // ���_�ړ��̊J�n�_
	VECTOR3 changePosGoal; // ���_�ړ��̏I���_ 
	VECTOR3 changeLookStart; // �����_�ړ��̊J�n�_
	VECTOR3 changeLookGoal; // �����_�ړ��̏I���_ 
};