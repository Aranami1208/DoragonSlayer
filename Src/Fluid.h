#pragma once

#include "FluidManager.h"
// --------------------------------------------------------
//
//	���̌v�Z�I�u�W�F�N�g�N���X
//
// --------------------------------------------------------
class Fluid : public Object3D
{
protected:
	SD_RESOURCE			m_Dns[2];	// ���x
	SD_RESOURCE			m_Vlc[2];	// ���x
	SD_RESOURCE			m_Vlc_s;	// ���x�̃Z�~�O�����W�A��
	SD_RESOURCE			m_Vlc_bs;	// ���x�̋t�����̃Z�~�O�����W�A��
	SD_RESOURCE			m_Prs;		// ����
	SD_RESOURCE			m_Div;		// ���U
	SD_RESOURCE			m_Dns_Tex;	// �`��

	bool				m_bAddFlag;		// �����t���O
	VECTOR4				m_vAddVoxelPos;	// �����{�N�Z���ʒu
	VECTOR4				m_vAddDensity;	// �������x
	VECTOR4				m_vAddVelocity;	// �������x
	VECTOR4				m_vOption;	    // �I�v�V����(x:�\�������̂̑傫���@y:�F�R�[�h�@z:�Ȃ��@w:���������_����)
	int					m_nAddVoxelWidth;	// �����ӏ����甭��������Ƃ��̃T�C�Y�i�{�N�Z�����j

	float				m_updateTimer;
	float				m_keepTimer;
public:
	Fluid();
	virtual	~Fluid();
	void Init();
	void Update() override;
	void DrawDepth() override;
	void DrawScreen() override;

	void Unbind();
	HRESULT CreateEmpty3DTexture(int Size,ID3D11Texture3D** ppTexture,ID3D11ShaderResourceView** ppSRV);

	void Spawn(const Transform& trans, const float& fCubeSize, const float& fColorNo, const VECTOR4&, const VECTOR4&, const VECTOR4&, const float& fRandom, const int& nAddVoxelWidth);
	void AddSource(VECTOR3 vOffset = VECTOR3(0, 0, 0));

};
