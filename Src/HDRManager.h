//=============================================================================
//		�g�c�q�`��̃v���O����
//�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@       ver 3.3        2024.3.23
//
//																HDRManager.h
//=============================================================================
#pragma once

#include "Object3D.h"

#include "Direct3D.h"
#include "Shader.h"


//�x����\��
#pragma warning(disable : 4005)
#pragma warning(disable : 4244)
#pragma warning(disable : 4018)

// HDRManager
#define HDR_BLOOMTEX_COUNT  4

//
// HDRManager�p���_�̍\����
//
struct HDRVertex
{
	VECTOR3 Pos;   //�ʒu
	VECTOR2 UV;	   //�e�N�X�`���[���W
};

//
// HDRManager�N���X
//
class CShader;
class HDRManager : public Object3D
{
private:
	CDirect3D*            m_pD3D;  // Direct3D11
	CShader*              m_pShader;
	ID3D11Buffer*         m_pVertexBufferHDR;

	// HDR�e�N�X�`���p
	ID3D11Texture2D*          m_pHDRMap_Tex;
	ID3D11RenderTargetView*   m_pHDRMap_TexRTV;
	ID3D11ShaderResourceView* m_pHDRMap_TexSRV;
	ID3D11Texture2D*          m_pHDRMap_DSTex;
	ID3D11DepthStencilView*   m_pHDRMap_DSTexDSV;

	ID3D11Texture2D*          m_pHDRBright_Tex;
	ID3D11RenderTargetView*   m_pHDRBright_TexRTV;
	ID3D11ShaderResourceView* m_pHDRBright_TexSRV;

	ID3D11Texture2D*          m_pHDRBloom_Tex[HDR_BLOOMTEX_COUNT];
	ID3D11RenderTargetView*   m_pHDRBloom_TexRTV[HDR_BLOOMTEX_COUNT];
	ID3D11ShaderResourceView* m_pHDRBloom_TexSRV[HDR_BLOOMTEX_COUNT];

	float                     m_fLimit;		// �u���C�g�̂������l
	float                     m_fForce;		// �u���[���̋���

	void    RenderBright();
	void    RenderBloom();
	void    RenderFinish();

public:
	// ���\�b�h
	HDRManager();
	~HDRManager();

	void    SetRenderTargetHDR(float BackColor[]);
	float   Limit() { return m_fLimit; }
	void    SetLimitForce(float, float);
	void    ResetLimitForce() { m_fLimit = 1.0f; m_fForce = 1.0f; }
	void    Render();
	
};