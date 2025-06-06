#pragma once
#include "Displace.h"
#include "MapManager.h"

class Fluid;
class MapFluid : public MapBase {
public:
	MapFluid();
	~MapFluid();

	void MakeFluidMap(TextReader* txt, int n);
	void Update() override;
	void DrawDepth() override;
	void DrawScreen() override;

private:
	std::string		m_tag;			// �Ή����̃I�u�W�F�N�g�̃^�O
	Transform 		m_trans;		// ���̂̈ʒu
	float			m_fCubeSize;	// �\���p�����̂̈�ӂ̑傫��
	float			m_fColorNo;		// ���̂̐F�R�[�h�i0:���i�����Ȃ��j1: ���i��������j2:���� 3:�Z���� 4:�������j
	float			m_fDensity;		// ���x
	float			m_fVelocity;	// ���x
	float			m_fRandom;		// ���x���������_����(0.0f:������ �` 1.0f:�����������_��)
	int				m_nAddVoxelWidth;	// �����ӏ����甭��������Ƃ��̃T�C�Y�i�{�N�Z�����j�B�P�J���̏ꍇ�͂O�ɂ���
};