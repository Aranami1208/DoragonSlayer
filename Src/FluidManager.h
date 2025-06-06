#pragma once

#include "GameMain.h"
#include "Object3D.h"

// �}�N����` ----------------------------------------------

// ���̃I�u�W�F�N�g�̃I�u�W�F�N�g��
#define FLUID_OBJ_MAX 4

// �[����������̈�ӂ̃T�C�Y�i��ӂ̃T�C�Y-2�j�B��̒[���܂߂�ƈ�ӂ�FLUID_SIZE+2�ƂȂ�
// �i�V�F�[�_�[FluidSolver.hlsli���ɂ��w�肠��j
#define FLUID_SIZE 64
//#define FLUID_SIZE 100

// �[����������̈�ӂ̃T�C�Y�̕�����
// �i�V�F�[�_�[FluidSolver.hlsli���ɂ��w�肠��j
// �E�R���s���[�g�V�F�[�_�͂b�o�t���Ƃf�o�t���ŃX���b�h�w�萔���߂������������ǂ��̂ŁA
// �@�Ⴆ�΂U�S�̂Ƃ��́A�W���W�i�b�o�t�W�A�f�o�t�W�j�Ŏw�肷��
#define FLUID_RSIZE 8
//#define FLUID_RSIZE 10


// ��������
// �i�V�F�[�_�[FluidSolver.hlsli���ɂ��w�肠��j
//#define FLUID_TIME_STEP 0.01f
#define FLUID_TIME_STEP 0.04f

// SD_RESOURCE���m�����ւ���}�N��
// �E�|�C���^�[�����ւ���B���͈̂ړ����Ȃ��B
#define FLUID_SWAP_SDRESOURCE(a,b) {SD_RESOURCE tmp;memcpy(&tmp,&a,sizeof(SD_RESOURCE));memcpy(&a,&b,sizeof(SD_RESOURCE));memcpy(&b,&tmp,sizeof(SD_RESOURCE));}

// �V�F�[�_�[���\�[�X�\����
struct SD_RESOURCE
{
	ID3D11Texture2D*			pTex2D;
	ID3D11Texture2D*			pDSTex2D;
	ID3D11Texture3D*			pTex3D;
	ID3D11Buffer*				pStredBuf;
	ID3D11RenderTargetView*		pRTV;
	ID3D11ShaderResourceView*	pSRV;	
	ID3D11DepthStencilView*		pDSV;
	ID3D11UnorderedAccessView*	pUAV;

	SD_RESOURCE()
	{
		ZeroMemory(this, sizeof(SD_RESOURCE));
	}

	void Release()
	{
		SAFE_RELEASE(pTex2D);
		SAFE_RELEASE(pDSTex2D);
		SAFE_RELEASE(pTex3D);
		SAFE_RELEASE(pStredBuf);
		SAFE_RELEASE(pRTV);
		SAFE_RELEASE(pSRV);
		SAFE_RELEASE(pDSV);
		SAFE_RELEASE(pUAV);
	}
};

//�X�g���N�`���[�h�o�b�t�@�̗v�f�\����
struct SBUFFER_ELEMENT
{
	float f;
	float f0;
};

// --------------------------------------------------------
//
//	���̌v�Z�}�l�[�W���N���X
//
// --------------------------------------------------------
class Fluid;
class FluidManager : public Object3D
{
protected:
	SD_RESOURCE			m_TexA;		// �R�c�e�N�X�`���̕`�掞�Ƀ����_�[�^�[�Q�b�g�Ƃ��Ďg�p
	SD_RESOURCE			m_TexB;		// �R�c�e�N�X�`���̕`�掞�Ƀ����_�[�^�[�Q�b�g�Ƃ��Ďg�p

	float				m_fCubeSize;		// �\���p�����́@��ӂ̑傫��
	ID3D11Buffer*		m_pCubeVBuffer;		// �\���p�����́@�o�[�e�b�N�X�o�b�t�@
	ID3D11Buffer*		m_pCubeIBuffer;		// �\���p�����́@�C���f�b�N�X�o�b�t�@

	ID3D11SamplerState* m_pSampleLinearFluid;	// �\���p�T���v���[�X�e�[�g

	ID3D11RasterizerState* m_pFrontFace;	// �\�ʂ̂ݕ\��
	ID3D11RasterizerState* m_pBackFace;		// ���ʂ̂ݕ\��

	int tagNo;	   // �^�O�m�n

public:
	FluidManager();
	virtual	~FluidManager();
	void Init();

	Fluid* SpawnY(const Transform& trans, const float& fCubeSize, const float& fColorNo, const float& fDensity, const float& fVelocity, const float&  fRandom, const int& nAddVoxelWidth, std::string& tag);
	Fluid* SpawnZ(const Transform& trans, const float& fCubeSize, const float& fColorNo, const float& fDensity, const float& fVelocity, const float&  fRandom, const int& nAddVoxelWidth, std::string& tag);

	HRESULT MakeCubeVertexBuffer(const float& fSize);
	void  Draw(const MATRIX4X4& mWorld, const VECTOR4& vOption, ID3D11ShaderResourceView** ppSRV, const MATRIX4X4& mView, const MATRIX4X4& mProj, const VECTOR3& vLight, const VECTOR3& vEye);

	Fluid*  SetFluidObj(std::string& tag);

	float GetCubeSize() { return m_fCubeSize; }
};