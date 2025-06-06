#pragma once
#include "MapManager.h"

class MapMove : public MapBase {
public:
	MapMove();
	~MapMove();

	void MakeMoveMap(TextReader* txt, int n);
	void Update() override;
	void DrawDepth() override;
	void DrawScreen() override;
	int  checkLimitAndSet(const float minOff, const float maxOff, float& velocityIO, float& upIO);
	bool ActiveOn() { return activeOn; }
	void SetActiveOn(bool on) { activeOn = on; }
	void SetMoveOn(bool on) { moveOn = on; }

private:
	Transform	transformBase;	// ��ʒu
	VECTOR3		velocity;		// ��ʒu����̑���
	std::string tag;			// �^�O��
	int			moveFlag;		// �ړ��敪    1:���s�ړ��@2:��]�@3:�g��k��
	bool		activeOn;		// �����E�\��ON      false:�����E�\���Ȃ��@true:�����E�\������
	bool		moveOn;			// �ړ�ON      false:�ړ���~�@true:�ړ����s
	VECTOR3		up;				// ����
	VECTOR3		minOffset;		// �����ŏ��l
	VECTOR3		maxOffset;		// �����ő�l
	int			changeFlag;		// �����l�����E�l�ɓ��B���ĕ���������ւ��Ƃ�(�o��)�@0:�ړ����@1:�����`�F���W
	int			loop;			// �ړ����J��Ԃ���  0:�`�F���W�̉ӏ��Œ�~���ړ�ON��0:�ړ���~�ɖ߂��B 1:�ړ����J��Ԃ�
};