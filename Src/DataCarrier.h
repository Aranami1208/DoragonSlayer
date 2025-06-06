#pragma once
#include "Object3D.h"

/// <summary>
/// �Q�[���̐i�s�ɔ����l�X�ȃf�[�^��ۑ�����N���X
/// �P�D�X�N���v�g��
/// �Q�D�X�R�A�l
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
	std::string currentScriptName;		// ���ݎ��s���̃X�N���v�g��
	int score;		// �X�R�A

};