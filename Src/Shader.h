// ========================================================================================
//
// �V�F�[�_�[�̏���                                               ver 3.0        2021.1.11
//
//   �V�F�[�_�[�O�����̓ǂݍ��݂ƃV�F�[�_�[�̍쐬
//   �C���v�b�g���C�A�E�g�̍쐬
//   �R���X�^���g�o�b�t�@�̍쐬
//   �t�H�O�̏����E�e�̏���
//
//
//    �o�^����Ă���V�F�[�_�[
//
//		�E�V���v���ȃV�F�[�_�[                          InitShaderSimple();
//		�E�X�v���C�g�p�̃V�F�[�_�[                      InitShaderSprite();
//		�EFBXStaticMesh/FBXSkinMesh�p�̃V�F�[�_�[       InitShaderFbx();         Cascade ShadoeMap�^���}�b�s���O�Ή�
//		�E�f�B�X�v���C�X�����g�}�b�s���O�p�̃V�F�[�_�[  InitShaderDisplace();    Cascade ShadoeMap�Ή�
//		�E�G�t�F�N�g�p�̃V�F�[�_�[                      InitShaderEffect();
//		�EHDR�p�̃V�F�[�_�[                             InitShaderHDR();
//		�EFluid�p�̃V�F�[�_�[                           InitShaderFluid();
//
//                                                                              Shader.h
// ========================================================================================
#pragma once

//�w�b�_�[�t�@�C���̃C���N���[�h
#include <stdio.h>
#include <windows.h>

#include "Main.h"
#include "Direct3D.h"

//�V�F�[�_�[�̃o�b�t�@�\���̒�`

// ���b�V���V�F�[�_�[�p�̃R���X�^���g�o�b�t�@�[�̃A�v�����\���́B
// �i���[���h�s�񂩂�ˉe�s��A���C�g�A�J���[�j  // -- 2020.1.24
struct CONSTANT_BUFFER_WVLED
{
	MATRIX4X4  mW;             // ���[���h�s��
	MATRIX4X4  mWVP;           // ���[���h����ˉe�܂ł̕ϊ��s��
	VECTOR4    vLightDir;      // ���C�g����
	VECTOR4    vEyePos;        // ���_
	VECTOR4    vDiffuse;       // �f�B�t���[�Y�F	
	VECTOR4    vDrawInfo;      // �`��֘A���(�g�p���Ă��Ȃ�)   // -- 2020.12.15
	CONSTANT_BUFFER_WVLED()
	{
		ZeroMemory(this, sizeof(CONSTANT_BUFFER_WVLED));
	}
};

// �f�B�X�v���[�X�����g�}�b�s���O�p�̊e��f�[�^��n��  // -- 2020.1.24
struct CONSTANT_BUFFER_DISPLACE
{
	VECTOR3    vEyePosInv;    // �e���_���猩���A���_�̈ʒu
	float      fMinDistance;  // �|���S�������̍ŏ�����
	float      fMaxDistance;  // �|���S�������̍ő召����
	int        iMaxDevide;    // �����ő吔
	VECTOR2    vHeight;       // �f�B�X�v���[�X�����g�}�b�s���O���̐���グ����
	VECTOR4    vWaveMove;     // �g�̈ړ���(�g�̏������̂�)
	VECTOR4    vSpecular;     // ���ʔ���(�g�̏������̂�)
	CONSTANT_BUFFER_DISPLACE()
	{
		ZeroMemory(this, sizeof(CONSTANT_BUFFER_DISPLACE));
	}
};

// Cascade Shadow�V�F�[�_�[�p�̃R���X�^���g�o�b�t�@�[�̃A�v�����\���́B
// �i���[���h�s�񂩂�ˉe�s��A���C�g�A�J�����j  // -- 2020.4.5
struct CONSTANT_BUFFER_WVLLED
{
	MATRIX4X4  mW;             // ���[���h�s��
	MATRIX4X4  mWVP;           // ���[���h����ˉe�܂ł̕ϊ��s��
	MATRIX4X4  mWLP[MAX_CASCADE_SHADOW];           // ���[���h�E�h���C�g�r���[�h�E�v���W�F�N�V�����̍���
	VECTOR4    vLightDir;      // ���C�g����
	VECTOR4    vEyePos;        // �J�����ʒu	
	VECTOR4    vDiffuse;       // �f�B�t���[�Y�F	
	VECTOR4    vDrawInfo;      // �}�e���A���֘A���@y:�e�L�薳��   // -- 2020.12.15
	CONSTANT_BUFFER_WVLLED()
	{
		ZeroMemory(this, sizeof(CONSTANT_BUFFER_WVLLED));
	}
};

// �}�e���A�����                      // -- 2020.12.15
struct CONSTANT_BUFFER_MATERIAL
{
	VECTOR4    vMatDuffuse;
	VECTOR4    vMatSpecular;
};


//  �G�t�F�N�g�p�̃R���X�^���g�o�b�t�@�̃A�v�����\����   //  2017.8.25
struct CONSTANT_BUFFER_EFFECT
{
	MATRIX4X4  mWVP;       // ���[���h����ˉe�܂ł̕ϊ��s��
	MATRIX4X4  mW;         // ���[���h
	MATRIX4X4  mV;         // �r���[
	MATRIX4X4  mP;         // �ˉe
	VECTOR2    vUVOffset;  // �e�N�X�`�����W�̃I�t�Z�b�g
	VECTOR2    vUVScale;   // �e�N�X�`�����W�̊g�k  // -- 2019.7.17
	float      fAlpha;
	float      fSize;      // �p�[�e�B�N���̑傫��  // -- 2018.8.23
	VECTOR2    Dummy;                               // -- 2019.7.17
	CONSTANT_BUFFER_EFFECT()
	{
		ZeroMemory(this, sizeof(CONSTANT_BUFFER_EFFECT));
	}
};


//�@3D�X�v���C�g�V�F�[�_�[�p�̃R���X�^���g�o�b�t�@�[�̃A�v�����\���� 
struct CONSTANT_BUFFER_SPRITE
{
	MATRIX4X4  mWVP;
	MATRIX4X4  mW;
	float      ViewPortWidth;
	float      ViewPortHeight;
	VECTOR2    vUVOffset;
	VECTOR4    vColor;         // �J���[���B�������̊������w�肷��
	VECTOR4    vMatInfo;       // �}�e���A���֘A���@x:�e�N�X�`���L�薳���BDrawRect()�ADrawLine()�Ŏg�p�B
	CONSTANT_BUFFER_SPRITE()
	{
		ZeroMemory(this, sizeof(CONSTANT_BUFFER_SPRITE));
	}
};


// �t�H�O�Ɋւ������n��      // -- 2019.3.5
struct CONSTANT_BUFFER_FOG
{
	VECTOR4 vFogInfo;      // x:�t�H�O�L��(0:���� 1:�w���t�H�O 2:�O�����h�t�H�O)  y:�t�H�O��  z: �t�H�O���x  w:�t�H�O����
	VECTOR4 vFogColor;     // �t�H�O�̐F
};

// HDR�p�̃R���X�^���g�o�b�t�@�[�̃A�v�����\����   // -- 2019.4.19
struct CONSTANT_BUFFER_HDR
{
	VECTOR2    ViewPort;
	VECTOR2    Info;
};

// ���̌v�Z�p�R���X�^���g�o�b�t�@    // -- 2019.12.31
struct CONSTANT_BUFFER_FLUID
{
	VECTOR4 vAddVoxelPos;	// �����{�N�Z���ʒu
	VECTOR4 vAddDensity;	// �������x
	VECTOR4 vAddVelocity;	// �������x
	VECTOR4 vOption;		// �e��I�v�V����
};

//
// CShader�N���X
//
class CShader
{
public:
	// Direct3D11
	CDirect3D*              m_pD3D;

	// �V�F�[�_�[
	// �ʏ�p�̃V���v���ȃV�F�[�_�[
	ID3D11InputLayout*      m_pSimple_VertexLayout;
	ID3D11VertexShader*     m_pSimple_VS;
	ID3D11PixelShader*      m_pSimple_PS;

	// 3D�X�v���C�g�p�̃V�F�[�_�[
	ID3D11InputLayout*      m_pSprite3D_VertexLayout;
	ID3D11VertexShader*     m_pSprite3D_VS;
	ID3D11PixelShader*      m_pSprite3D_PS;
	ID3D11VertexShader*     m_pSprite3D_VS_BILL;
	ID3D11PixelShader*      m_pSprite3D_PS_ENV;		// -- 2019.6.8

	// �f�B�X�v���[�X�����g�}�b�s���O(�g)�p�̃V�F�[�_�[
	ID3D11InputLayout*      m_pDisplaceWave_VertexLayout;
	ID3D11VertexShader*     m_pDisplaceWave_VS;
	ID3D11HullShader*       m_pDisplaceWave_HS;
	ID3D11DomainShader*     m_pDisplaceWave_DS;
	ID3D11PixelShader*      m_pDisplaceWave_PS;

	// �f�B�X�v���[�X�����g�}�b�s���O(�X�L�����b�V��)Shadow �p�̃V�F�[�_�[  // -- 2020.1.24
	ID3D11VertexShader* m_pDisplaceSkinShadowMap_VS;
	ID3D11HullShader* m_pDisplaceSkinShadowMap_HS;
	ID3D11DomainShader* m_pDisplaceSkinShadowMap_DS;
	ID3D11PixelShader* m_pDisplaceSkinShadowMap_PS;
	ID3D11DomainShader* m_pDisplaceSkinShadowMap_DSDepth;
	ID3D11PixelShader* m_pDisplaceSkinShadowMap_PSDepth;

	// �f�B�X�v���[�X�����g�}�b�s���O(�X�^�e�B�b�N���b�V��)Shadow �p�̃V�F�[�_�[  // -- 2018.7.28
	ID3D11VertexShader* m_pDisplaceStaticShadowMap_VS;
	ID3D11HullShader* m_pDisplaceStaticShadowMap_HS;
	ID3D11DomainShader* m_pDisplaceStaticShadowMap_DS;
	ID3D11PixelShader* m_pDisplaceStaticShadowMap_PS;
	ID3D11DomainShader* m_pDisplaceStaticShadowMap_DSDepth;
	ID3D11PixelShader* m_pDisplaceStaticShadowMap_PSDepth;

	// �G�t�F�N�g�p�̃V�F�[�_�[
	ID3D11InputLayout*      m_pEffect3D_VertexLayout;
	ID3D11VertexShader*     m_pEffect3D_VS_POINT;
	ID3D11GeometryShader*   m_pEffect3D_GS_POINT;
	ID3D11PixelShader*      m_pEffect3D_PS;
	ID3D11InputLayout*      m_pEffect3D_VertexLayout_BILL;
	ID3D11VertexShader*     m_pEffect3D_VS_BILL;
	ID3D11VertexShader*     m_pEffect3D_VS_BILLMESH;  // -- 2019.7.17

	// Fbx���f���@�X�^�e�B�c�N���b�V��ShadowMap�p		// -- 2018.7.28
	ID3D11InputLayout* m_pFbxStaticShadowMap_VertexLayout;
	ID3D11VertexShader* m_pFbxStaticShadowMap_VSDepth;
	ID3D11PixelShader* m_pFbxStaticShadowMap_PSDepth;
	ID3D11VertexShader* m_pFbxStaticShadowMap_VS;
	ID3D11PixelShader* m_pFbxStaticShadowMap_PS;

	// Fbx���f���@�X�L�����b�V�� ShadowMap�p		// -- 2018.7.28
	ID3D11InputLayout* m_pFbxSkinShadowMap_VertexLayout;
	ID3D11VertexShader* m_pFbxSkinShadowMap_VSDepth;
	ID3D11PixelShader* m_pFbxSkinShadowMap_PSDepth;
	ID3D11VertexShader* m_pFbxSkinShadowMap_VS;
	ID3D11PixelShader* m_pFbxSkinShadowMap_PS;

	// HDR�p�̃V�F�[�_�[
	ID3D11InputLayout* m_pHDR_VertexLayout;
	ID3D11VertexShader* m_pHDR_VS;
	ID3D11PixelShader* m_pHDR_PS;
	ID3D11PixelShader* m_pHDR_PS_BRIGHT;
	ID3D11PixelShader* m_pHDR_PS_BLOOM;

	// ���̌v�Z�̃V�F�[�_�[
	ID3D11ComputeShader* m_pFluidSolver_AddSource;
	ID3D11ComputeShader* m_pFluidSolver_AdvectBack;
	ID3D11ComputeShader* m_pFluidSolver_AdvectDensity;
	ID3D11ComputeShader* m_pFluidSolver_AdvectVelocity;
	ID3D11ComputeShader* m_pFluidSolver_Boundary1;
	ID3D11ComputeShader* m_pFluidSolver_MacCormack;
	ID3D11ComputeShader* m_pFluidSolver_Project1;
	ID3D11ComputeShader* m_pFluidSolver_Project2;
	ID3D11ComputeShader* m_pFluidSolver_Project3;
	ID3D11InputLayout* m_pFluidRender_VertexLayout;
	ID3D11VertexShader* m_pFluidRender_VS;
	ID3D11PixelShader* m_pFluidRender_PS;
	ID3D11InputLayout* m_pFluidTexture_VertexLayout;
	ID3D11VertexShader* m_pFluidTexture_VS;
	ID3D11PixelShader* m_pFluidTexture_PS;


	// �R���X�^���g�o�b�t�@  ------------------------------------------

	//�R���X�^���g�o�b�t�@�[�f�B�X�v���C�X�����g�}�b�s���O�p
	ID3D11Buffer* m_pConstantBufferDisplace;

	//�R���X�^���g�o�b�t�@�[�G�t�F�N�g�p
	ID3D11Buffer* m_pConstantBufferEffect;

	//�R���X�^���g�o�b�t�@�[ 3D�X�v���C�g�p
	ID3D11Buffer* m_pConstantBufferSprite3D;

	// �R���X�^���g�o�b�t�@�[�@���b�V�� �ϊ��s��E�J���[�n���p
	ID3D11Buffer* m_pConstantBufferWVLED;

	// �R���X�^���g�o�b�t�@�[ �{�[���s��n���p
	ID3D11Buffer* m_pConstantBufferBone2;   // -- 2018.4.14 �{�[���s��p�R���X�^���g�o�b�t�@

	// �}�e���A�����@�n���p
	ID3D11Buffer* m_pConstantBufferMaterial;    // -- 2020.12.15

	// �R���X�^���g�o�b�t�@�[�@���b�V�� �ϊ��s��E�J���[�n��(�V���h�E�}�b�v�p)
	ID3D11Buffer* m_pConstantBufferWVLLED;   // -- 2018.7.28

	// �R���X�^���g�o�b�t�@�[  �t�H�O���n���p
	ID3D11Buffer* m_pConstantBufferFog;

	// �R���X�^���g�o�b�t�@�[ HDR�p
	ID3D11Buffer* m_pConstantBufferHDR;
	ID3D11Buffer* m_pConstantBufferHDRBloom;

	// �R���X�^���g�o�b�t�@�[  ���̌v�Z�p
	ID3D11Buffer* m_pConstantBufferFluid;


public:
	HRESULT InitShader();
	HRESULT InitShaderSimple();
	HRESULT InitShaderSprite();

	HRESULT InitShaderFbx();
	HRESULT InitShaderDisplace();
	HRESULT InitShaderEffect();
	HRESULT InitShaderHDR();
	HRESULT InitShaderFluid();

	HRESULT InitShaderConstant();

	void    SetConstantBufferFog(const int& m_nFog, const float& m_fFogVolume, const float& m_fFogDensity, const float& m_fFogHeight, const VECTOR4& m_vFogColor);

	HRESULT MakeShader(const TCHAR ProfileName[], const TCHAR FileName[], void** ppShader, D3D11_INPUT_ELEMENT_DESC Fluid_layout[] = nullptr, UINT numElements = 0, ID3D11InputLayout** ppInputLayout = nullptr);
	HRESULT MakeConstantBuffer(UINT size, ID3D11Buffer**  ppConstantBuffer);

	CShader(CDirect3D* pD3D);
	~CShader();

};
