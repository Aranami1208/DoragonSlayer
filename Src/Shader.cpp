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
//		�EFBXStaticMesh/FBXSkinMesh�p�̃V�F�[�_�[       InitShaderFbx();         Cascade ShadoeMap�Ή�
//		�E�f�B�X�v���C�X�����g�}�b�s���O�p�̃V�F�[�_�[  InitShaderDisplace();    Cascade ShadoeMap�Ή�
//		�E�G�t�F�N�g�p�̃V�F�[�_�[                      InitShaderEffect();
//		�EHDR�p�̃V�F�[�_�[                             InitShaderHDR();
//		�EFluid�p�̃V�F�[�_�[                           InitShaderFluid();
//
//                                                                              Shader.cpp
// ========================================================================================

#include "Shader.h"
#include "FbxMesh.h"

//------------------------------------------------------------------------
//
//	�V�F�[�_�[�̃R���X�g���N�^	
//
//  �����@CDirect3D* pD3D
//
//------------------------------------------------------------------------
CShader::CShader(CDirect3D* pD3D)
{
	ZeroMemory(this, sizeof(CShader));
	m_pD3D = pD3D;
}
//------------------------------------------------------------------------
//
//	�V�F�[�_�[�̃f�X�g���N�^	
//
//------------------------------------------------------------------------
CShader::~CShader()
{
	// �V�F�[�_�[�̉��  ------------------------------------------

	// �ʏ�p�̃V���v���ȃV�F�[�_�[
	SAFE_RELEASE(m_pSimple_VertexLayout); 
	SAFE_RELEASE(m_pSimple_VS);
	SAFE_RELEASE(m_pSimple_PS);

	// 3D�X�v���C�g�p�̃V�F�[�_�[
	SAFE_RELEASE(m_pSprite3D_VertexLayout);
	SAFE_RELEASE(m_pSprite3D_VS);
	SAFE_RELEASE(m_pSprite3D_PS);
	SAFE_RELEASE(m_pSprite3D_VS_BILL);
	SAFE_RELEASE(m_pSprite3D_PS_ENV);

	// �f�B�X�v���[�X�����g�}�b�s���O(�g)�p�̃V�F�[�_�[
	SAFE_RELEASE(m_pDisplaceWave_VertexLayout);
	SAFE_RELEASE(m_pDisplaceWave_VS);
	SAFE_RELEASE(m_pDisplaceWave_HS);
	SAFE_RELEASE(m_pDisplaceWave_DS);
	SAFE_RELEASE(m_pDisplaceWave_PS);

	// �f�B�X�v���[�X�����g�}�b�s���O(�X�L�����b�V��)Shadow �p�̃V�F�[�_�[
	SAFE_RELEASE(m_pDisplaceSkinShadowMap_VS);
	SAFE_RELEASE(m_pDisplaceSkinShadowMap_HS);
	SAFE_RELEASE(m_pDisplaceSkinShadowMap_DS);
	SAFE_RELEASE(m_pDisplaceSkinShadowMap_PS);
	SAFE_RELEASE(m_pDisplaceSkinShadowMap_DSDepth);
	SAFE_RELEASE(m_pDisplaceSkinShadowMap_PSDepth);

	// �f�B�X�v���[�X�����g�}�b�s���O(�X�^�e�B�b�N���b�V��)Shadow �p�̃V�F�[�_�[ 
	SAFE_RELEASE(m_pDisplaceStaticShadowMap_VS);
	SAFE_RELEASE(m_pDisplaceStaticShadowMap_HS);
	SAFE_RELEASE(m_pDisplaceStaticShadowMap_DS);
	SAFE_RELEASE(m_pDisplaceStaticShadowMap_PS);
	SAFE_RELEASE(m_pDisplaceStaticShadowMap_DSDepth);
	SAFE_RELEASE(m_pDisplaceStaticShadowMap_PSDepth);

	// �G�t�F�N�g�p�̃V�F�[�_�[
	SAFE_RELEASE(m_pEffect3D_VertexLayout);
	SAFE_RELEASE(m_pEffect3D_VS_POINT);
	SAFE_RELEASE(m_pEffect3D_GS_POINT);
	SAFE_RELEASE(m_pEffect3D_PS);
	SAFE_RELEASE(m_pEffect3D_VertexLayout_BILL);
	SAFE_RELEASE(m_pEffect3D_VS_BILL);
	SAFE_RELEASE(m_pEffect3D_VS_BILLMESH);              // -- 2019.7.17

	// Fbx���f���@�X�^�e�B�c�N���b�V��ShadowMap�p		// -- 2018.7.28
	SAFE_RELEASE(m_pFbxStaticShadowMap_VertexLayout);
	SAFE_RELEASE(m_pFbxStaticShadowMap_VSDepth);
	SAFE_RELEASE(m_pFbxStaticShadowMap_PSDepth);
	SAFE_RELEASE(m_pFbxStaticShadowMap_VS);
	SAFE_RELEASE(m_pFbxStaticShadowMap_PS);

	// Fbx���f���@�X�L�����b�V�� ShadowMap�p		// -- 2018.7.28
	SAFE_RELEASE(m_pFbxSkinShadowMap_VertexLayout);
	SAFE_RELEASE(m_pFbxSkinShadowMap_VSDepth);
	SAFE_RELEASE(m_pFbxSkinShadowMap_PSDepth);
	SAFE_RELEASE(m_pFbxSkinShadowMap_VS);
	SAFE_RELEASE(m_pFbxSkinShadowMap_PS);

	// HDR�p�̃V�F�[�_�[                       // -- 2019.4.19
	SAFE_RELEASE(m_pHDR_VertexLayout);
	SAFE_RELEASE(m_pHDR_VS);
	SAFE_RELEASE(m_pHDR_PS);
	SAFE_RELEASE(m_pHDR_PS_BRIGHT);
	SAFE_RELEASE(m_pHDR_PS_BLOOM);

	// ���̌v�Z�p�̃V�F�[�_�[
	SAFE_RELEASE(m_pFluidSolver_AddSource);
	SAFE_RELEASE(m_pFluidSolver_AdvectBack);
	SAFE_RELEASE(m_pFluidSolver_AdvectDensity);
	SAFE_RELEASE(m_pFluidSolver_AdvectVelocity);
	SAFE_RELEASE(m_pFluidSolver_Boundary1);
	SAFE_RELEASE(m_pFluidSolver_MacCormack);
	SAFE_RELEASE(m_pFluidSolver_Project1);
	SAFE_RELEASE(m_pFluidSolver_Project2);
	SAFE_RELEASE(m_pFluidSolver_Project3);
	SAFE_RELEASE(m_pFluidRender_VertexLayout);
	SAFE_RELEASE(m_pFluidRender_VS);
	SAFE_RELEASE(m_pFluidRender_PS);
	SAFE_RELEASE(m_pFluidTexture_VertexLayout);
	SAFE_RELEASE(m_pFluidTexture_VS);
	SAFE_RELEASE(m_pFluidTexture_PS);


	// �R���X�^���g�o�b�t�@�̉�� ---------------------------

	SAFE_RELEASE(m_pConstantBufferFog);

	SAFE_RELEASE(m_pConstantBufferDisplace);
	SAFE_RELEASE(m_pConstantBufferEffect);
	SAFE_RELEASE(m_pConstantBufferSprite3D);

	SAFE_RELEASE(m_pConstantBufferWVLED);
	SAFE_RELEASE(m_pConstantBufferBone2);
	SAFE_RELEASE(m_pConstantBufferMaterial);      // -- 2020.12.15
	SAFE_RELEASE(m_pConstantBufferWVLLED);

	SAFE_RELEASE(m_pConstantBufferHDR);
	SAFE_RELEASE(m_pConstantBufferHDRBloom);

	SAFE_RELEASE(m_pConstantBufferFluid);  // ���̌v�Z�p

}

//------------------------------------------------------------------------
//
//	�e��V�F�[�_�[�̍쐬	
//
//  �E�V�F�[�_�[�ƃR���X�^���g�o�b�t�@���쐬����
//  �E�e�N�X�`���[�T���v���[�ƃu�����h�X�e�[�g���쐬����
//
//  �����@�Ȃ�
//
//	�߂�l HRESULT
//		S_OK	= ����
//		E_FAIL	= �ُ�
//
//------------------------------------------------------------------------
HRESULT CShader::InitShader()
{
	//  �ʏ탁�b�V���p�̃V�F�[�_�[�쐬
	InitShaderSimple();

	//  Fbx�X�^�e�B�b�N�E�X�L�����b�V���p�̃V�F�[�_�[�쐬
	InitShaderFbx();

	//  �f�B�X�v���[�X�����g�}�b�s���O�p�̃V�F�[�_�[�쐬
	InitShaderDisplace();

	//  �G�t�F�N�g�p�̃V�F�[�_�[�쐬
	InitShaderEffect();

	//  �X�v���C�g�p�̃V�F�[�_�[�쐬
	InitShaderSprite();

	//  HDR�p�̃V�F�[�_�[�쐬
	InitShaderHDR();

	//  ���̌v�Z�p�̃V�F�[�_�[�쐬
	InitShaderFluid();

	//  �R���X�^���g�o�b�t�@�쐬
	InitShaderConstant();

	return S_OK;
}
//------------------------------------------------------------------------
//
//  �ʏ�p(Simple Shader)�̃V�F�[�_�[�쐬
//
//  �����@�Ȃ�
//
//	�߂�l HRESULT
//		S_OK	= ����
//		E_FAIL	= �ُ�
//
//------------------------------------------------------------------------
HRESULT CShader::InitShaderSimple()
{

	// ���_�C���v�b�g���C�A�E�g���`
	UINT numElements = 0;
	D3D11_INPUT_ELEMENT_DESC Simplelayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	numElements = sizeof(Simplelayout) / sizeof(Simplelayout[0]);

	// �o�[�e�b�N�X�V�F�[�_�E�s�N�Z���V�F�[�_�E���_�C���v�b�g���C�A�E�g�̍쐬
	MakeShader(_T("VS"), _T("Simple_VS.cso"), (void**)&m_pSimple_VS, Simplelayout, numElements, &m_pSimple_VertexLayout);
	MakeShader(_T("PS"), _T("Simple_PS.cso"), (void**)&m_pSimple_PS);

	return S_OK;
}

//------------------------------------------------------------------------
//
//  Fbx���f���@�X�^�e�B�c�N���X�L�����b�V���p�̃V�F�[�_�[�쐬
//
//  �����@�Ȃ�
//
//	�߂�l HRESULT
//		S_OK	= ����
//		E_FAIL	= �ُ�
//
//------------------------------------------------------------------------
HRESULT CShader::InitShaderFbx()
{

	// -------------------------------------------------------------------
	// 
	// �X�^�e�B�b�N���b�V��  FbxStaticMesh
	// 
	// -------------------------------------------------------------------
	// ���_�C���v�b�g���C�A�E�g���`
	UINT numElements = 0;
	D3D11_INPUT_ELEMENT_DESC FbxStaticNM_layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },		// �v32byte
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT,0,32, D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT,0,44, D3D11_INPUT_PER_VERTEX_DATA,0 }, // �v56byte
	};
	numElements = sizeof(FbxStaticNM_layout) / sizeof(FbxStaticNM_layout[0]);

	// --------------------------------------------------------------------------  // -- 2018.7.28
	//
	//	�X�^�e�B�b�N���b�V���@�V���h�E�}�b�v�̃V�F�[�_�[
	//
	// -------------------------------------------------------------------------- 
	// --------------------------------------------------------------------- 
	// �@�@FbxStaticShadow
	// ---------------------------------------------------------------------

	MakeShader(_T("VS"), _T("FbxStaticShadowMap_VS.cso"), (void**)&m_pFbxStaticShadowMap_VS, FbxStaticNM_layout, numElements, &m_pFbxStaticShadowMap_VertexLayout);
	MakeShader(_T("PS"), _T("FbxStaticShadowMap_PS.cso"), (void**)&m_pFbxStaticShadowMap_PS);

	// --------------------------------------------------------------------- // -- 2018.7.28
	// �A�@FbxStaticShadowDepth
	// ---------------------------------------------------------------------

	MakeShader(_T("VS"), _T("FbxStaticShadowMap_VSDepth.cso"), (void**)&m_pFbxStaticShadowMap_VSDepth);
	MakeShader(_T("PS"), _T("FbxStaticShadowMap_PSDepth.cso"), (void**)&m_pFbxStaticShadowMap_PSDepth);


	// -------------------------------------------------------------------
	// 
	// �X�L�����b�V��  FbxSkinMesh
	// 
	// -------------------------------------------------------------------
	// ���_�C���v�b�g���C�A�E�g���`
	numElements = 0;
	D3D11_INPUT_ELEMENT_DESC FbxSkinNM_layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BONE_INDEX", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BONE_WEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 64, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 76, D3D11_INPUT_PER_VERTEX_DATA, 0 }, // �v76Byte
	};
	numElements = sizeof(FbxSkinNM_layout) / sizeof(FbxSkinNM_layout[0]);


	// --------------------------------------------------------------------------  // -- 2018.7.28
	//
	//	�X�L�����b�V���@�V���h�E�}�b�v�̃V�F�[�_�[
	//
	// -------------------------------------------------------------------------- 

	// ------------------------------------------------  // -- 2018.7.28
	// �@�@FbxSkinShadow
	// ------------------------------------------------
	MakeShader(_T("VS"), _T("FbxSkinShadowMap_VS.cso"), (void**)&m_pFbxSkinShadowMap_VS, FbxSkinNM_layout, numElements, &m_pFbxSkinShadowMap_VertexLayout);
	MakeShader(_T("PS"), _T("FbxSkinShadowMap_PS.cso"), (void**)&m_pFbxSkinShadowMap_PS);

	// ---------------------------------------------------------  // -- 2018.7.28
	// �A�@FbxSkinShadowDepth
	// ---------------------------------------------------------  
	MakeShader(_T("VS"), _T("FbxSkinShadowMap_VSDepth.cso"), (void**)&m_pFbxSkinShadowMap_VSDepth);
	MakeShader(_T("PS"), _T("FbxSkinShadowMap_PSDepth.cso"), (void**)&m_pFbxSkinShadowMap_PSDepth);


	return S_OK;
}
//------------------------------------------------------------------------
//
//  �f�B�X�v���[�X�����g�}�b�s���O�p�̃V�F�[�_�[�쐬
//
//  �����@�Ȃ�
//
//	�߂�l HRESULT
//		S_OK	= ����
//		E_FAIL	= �ُ�
//
//------------------------------------------------------------------------
HRESULT CShader::InitShaderDisplace()
{

	// -----------------------------------------------------------------------------------------------
	// �g�̃f�B�X�v���C�X�}�b�s���O DisplaceWave
	// -----------------------------------------------------------------------------------------------
	// ���_�C���v�b�g���C�A�E�g���`
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = sizeof(layout) / sizeof(layout[0]);

	// �o�[�e�b�N�X�V�F�[�_�E�s�N�Z���V�F�[�_�E���_�C���v�b�g���C�A�E�g�̍쐬
	MakeShader(_T("VS"), _T("DisplaceWave_VS.cso"), (void**)&m_pDisplaceWave_VS, layout, numElements, &m_pDisplaceWave_VertexLayout);
	MakeShader(_T("HS"), _T("DisplaceWave_HS.cso"), (void**)&m_pDisplaceWave_HS);
	MakeShader(_T("DS"), _T("DisplaceWave_DS.cso"), (void**)&m_pDisplaceWave_DS);
	MakeShader(_T("PS"), _T("DisplaceWave_PS.cso"), (void**)&m_pDisplaceWave_PS);


	// -----------------------------------------------------------------------------------------------
	//
	// �X�^�e�B�b�N���b�V���̃f�B�X�v���C�X�����g�}�b�s���O DisplaceStaticMesh
	//
	// -----------------------------------------------------------------------------------------------

	// -----------------------------------------------------------------------------------------------
	//
	// �X�^�e�B�b�N���b�V���̃f�B�X�v���C�X�}�b�s���O  ShadowMap�p   DisplaceStaticShadowMap
	//
	// -----------------------------------------------------------------------------------------------
	// ---------------------------------------------------------------------------
	// �@�@ShadowMap�p
	// ---------------------------------------------------------------------------
	// �o�[�e�b�N�X�V�F�[�_�E�s�N�Z���V�F�[�_�E�n���V�F�[�_�E�h���C���V�F�[�_�̍쐬
	//  !!!!! ���_�C���v�b�g���C�A�E�g�́A�X�^�e�B�b�N���b�V���̃��C�A�E�g���g�p	
	MakeShader(_T("VS"), _T("DisplaceStaticShadowMap_VS.cso"), (void**)&m_pDisplaceStaticShadowMap_VS);
	MakeShader(_T("HS"), _T("DisplaceStaticShadowMap_HS.cso"), (void**)&m_pDisplaceStaticShadowMap_HS);
	MakeShader(_T("DS"), _T("DisplaceStaticShadowMap_DS.cso"), (void**)&m_pDisplaceStaticShadowMap_DS);
	MakeShader(_T("PS"), _T("DisplaceStaticShadowMap_PS.cso"), (void**)&m_pDisplaceStaticShadowMap_PS);

	// ---------------------------------------------------------------------------
	// �A�@ShadowDepth�p
	// ---------------------------------------------------------------------------
	MakeShader(_T("DS"), _T("DisplaceStaticShadowMap_DSDepth.cso"), (void**)&m_pDisplaceStaticShadowMap_DSDepth);
	MakeShader(_T("PS"), _T("DisplaceStaticShadowMap_PSDepth.cso"), (void**)&m_pDisplaceStaticShadowMap_PSDepth);


	// -----------------------------------------------------------------------------------------------
	//
	// �X�L�����b�V���̃f�B�X�v���C�X�}�b�s���O�p
	//
	// -----------------------------------------------------------------------------------------------

	// -----------------------------------------------------------------------------------------------
	//
	// �X�L�����b�V���̃f�B�X�v���C�X�}�b�s���O  ShadowMap�p   DisplaceSkinShadowMap
	//
	// -----------------------------------------------------------------------------------------------
	// -----------------------------------------------------------------------------------------------
	// �@�@ShadowMap�p
	// -----------------------------------------------------------------------------------------------
	MakeShader(_T("VS"), _T("DisplaceSkinShadowMap_VS.cso"), (void**)&m_pDisplaceSkinShadowMap_VS);
	MakeShader(_T("HS"), _T("DisplaceSkinShadowMap_HS.cso"), (void**)&m_pDisplaceSkinShadowMap_HS);
	MakeShader(_T("DS"), _T("DisplaceSkinShadowMap_DS.cso"), (void**)&m_pDisplaceSkinShadowMap_DS);
	MakeShader(_T("PS"), _T("DisplaceSkinShadowMap_PS.cso"), (void**)&m_pDisplaceSkinShadowMap_PS);

	// -----------------------------------------------------------------------------------------------
	// �A�@ShadowDepth�p
	// -----------------------------------------------------------------------------------------------
	MakeShader(_T("DS"), _T("DisplaceSkinShadowMap_DSDepth.cso"), (void**)&m_pDisplaceSkinShadowMap_DSDepth);
	MakeShader(_T("PS"), _T("DisplaceSkinShadowMap_PSDepth.cso"), (void**)&m_pDisplaceSkinShadowMap_PSDepth);

	return S_OK;
}

//------------------------------------------------------------------------
//
//  �G�t�F�N�g�p�̃V�F�[�_�[�쐬
//
//  �����@�Ȃ�
//
//	�߂�l HRESULT
//		S_OK	= ����
//		E_FAIL	= �ُ�
//
//------------------------------------------------------------------------
HRESULT CShader::InitShaderEffect()
{

	// -------------------------------------------------------------------
	// 
	// �p�[�e�B�N���̃V�F�[�_�[
	// 
	// -------------------------------------------------------------------
	// ���_�C���v�b�g���C�A�E�g���`
	UINT numElements = 0;
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	numElements = sizeof(layout) / sizeof(layout[0]);

	// �o�[�e�b�N�X�V�F�[�_�E�W�I���g���V�F�[�_�E���_�C���v�b�g���C�A�E�g�̍쐬
	MakeShader(_T("VS"), _T("Effect3D_VS_POINT.cso"), (void**)&m_pEffect3D_VS_POINT, layout, numElements, &m_pEffect3D_VertexLayout);
	MakeShader(_T("GS"), _T("Effect3D_GS_POINT.cso"), (void**)&m_pEffect3D_GS_POINT);


	// -----------------------------------------------------------------------------------------------------
	// 
	// �r���{�[�h�̃V�F�[�_�[
	// 
	// -----------------------------------------------------------------------------------------------------
	//���_�C���v�b�g���C�A�E�g���`
	D3D11_INPUT_ELEMENT_DESC layoutbill[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	numElements = sizeof(layoutbill) / sizeof(layoutbill[0]);

	// �o�[�e�b�N�X�V�F�[�_�E���_�C���v�b�g���C�A�E�g�̍쐬
	MakeShader(_T("VS"), _T("Effect3D_VS_BILL.cso"), (void**)&m_pEffect3D_VS_BILL, layoutbill, numElements, &m_pEffect3D_VertexLayout_BILL);

	// -----------------------------------------------------------------------------------------------------
	// 
	// �r���{�[�h���b�V���̃V�F�[�_�[
	// 
	// -----------------------------------------------------------------------------------------------------
	// �o�[�e�b�N�X�V�F�[�_�̍쐬
	MakeShader(_T("VS"), _T("Effect3D_VS_BILLMESH.cso"), (void**)&m_pEffect3D_VS_BILLMESH);

	// -----------------------------------------------------------------------------------------------------
	// 
	// ���ʂ̃V�F�[�_�[
	// 
	// -----------------------------------------------------------------------------------------------------
	// �s�N�Z���V�F�[�_�̍쐬
	MakeShader(_T("PS"), _T("Effect3D_PS.cso"), (void**)&m_pEffect3D_PS);

	return S_OK;
}

//------------------------------------------------------------------------
//
//  �X�v���C�g�p�̃V�F�[�_�[�쐬
//
//  �����@�Ȃ�
//
//	�߂�l HRESULT
//		S_OK	= ����
//		E_FAIL	= �ُ�
//
//------------------------------------------------------------------------
HRESULT CShader::InitShaderSprite()
{
	// ���_�C���v�b�g���C�A�E�g���`	
	D3D11_INPUT_ELEMENT_DESC layout_sprite[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = sizeof(layout_sprite) / sizeof(layout_sprite[0]);

	// �o�[�e�b�N�X�V�F�[�_�E�s�N�Z���V�F�[�_�E���_�C���v�b�g���C�A�E�g�̍쐬
	MakeShader(_T("VS"), _T("Sprite3D_VS.cso"), (void**)&m_pSprite3D_VS, layout_sprite, numElements, &m_pSprite3D_VertexLayout);
	MakeShader(_T("VS"), _T("Sprite3D_VS_BILL.cso"), (void**)&m_pSprite3D_VS_BILL);
	MakeShader(_T("PS"), _T("Sprite3D_PS.cso"), (void**)&m_pSprite3D_PS);

	return S_OK;
}

//------------------------------------------------------------------------ // -- 2019.4.19
//
// HDR�p�̃V�F�[�_�[�쐬
//
//  �����@�Ȃ�
//
//	�߂�l HRESULT
//		S_OK	= ����
//		E_FAIL	= �ُ�
//
//------------------------------------------------------------------------
HRESULT CShader::InitShaderHDR()
{

	// ���_�C���v�b�g���C�A�E�g���`
	UINT numElements = 0;
	D3D11_INPUT_ELEMENT_DESC layout_HDR[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	numElements = sizeof(layout_HDR) / sizeof(layout_HDR[0]);

	// �o�[�e�b�N�X�V�F�[�_�E�s�N�Z���V�F�[�_�E���_�C���v�b�g���C�A�E�g�̍쐬
	MakeShader(_T("VS"), _T("HDR_VS.cso"), (void**)&m_pHDR_VS, layout_HDR, numElements, &m_pHDR_VertexLayout);
	MakeShader(_T("PS"), _T("HDR_PS.cso"), (void**)&m_pHDR_PS);
	MakeShader(_T("PS"), _T("HDR_PS_BRIGHT.cso"), (void**)&m_pHDR_PS_BRIGHT);
	MakeShader(_T("PS"), _T("HDR_PS_BLOOM.cso"), (void**)&m_pHDR_PS_BLOOM);

	return S_OK;

}

//------------------------------------------------------------------------
//
//  ���̌v�Z�p�̃V�F�[�_�[�쐬
//
//  �����@�Ȃ�
//
//	�߂�l HRESULT
//		S_OK	= ����
//		E_FAIL	= �ُ�
//
//------------------------------------------------------------------------
HRESULT CShader::InitShaderFluid()
{

	// ���_�C���v�b�g���C�A�E�g���`
	UINT numElements = 0;
	D3D11_INPUT_ELEMENT_DESC Fluid_layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	numElements = sizeof(Fluid_layout) / sizeof(Fluid_layout[0]);

	//
	// �e��V�F�[�_�[�̍쐬
	//
	// FluidTexture
	MakeShader(_T("VS"), _T("FluidTexture_VS.cso"), (void**)&m_pFluidTexture_VS, Fluid_layout, numElements, &m_pFluidTexture_VertexLayout);
	MakeShader(_T("PS"), _T("FluidTexture_PS.cso"), (void**)&m_pFluidTexture_PS);

	// FluidRender
	MakeShader(_T("VS"), _T("FluidRender_VS.cso"), (void**)&m_pFluidRender_VS, Fluid_layout, numElements, &m_pFluidRender_VertexLayout);
	MakeShader(_T("PS"), _T("FluidRender_PS.cso"), (void**)&m_pFluidRender_PS);

	// FluidSolver
	MakeShader(_T("CS"), _T("FluidSolver_AddSource.cso"), (void**)&m_pFluidSolver_AddSource);
	MakeShader(_T("CS"), _T("FluidSolver_AdvectBack.cso"), (void**)&m_pFluidSolver_AdvectBack);
	MakeShader(_T("CS"), _T("FluidSolver_AdvectDensity.cso"), (void**)&m_pFluidSolver_AdvectDensity);
	MakeShader(_T("CS"), _T("FluidSolver_AdvectVelocity.cso"), (void**)&m_pFluidSolver_AdvectVelocity);
	MakeShader(_T("CS"), _T("FluidSolver_Boundary1.cso"), (void**)&m_pFluidSolver_Boundary1);
	MakeShader(_T("CS"), _T("FluidSolver_MacCormack.cso"), (void**)&m_pFluidSolver_MacCormack);
	MakeShader(_T("CS"), _T("FluidSolver_Project1.cso"), (void**)&m_pFluidSolver_Project1);
	MakeShader(_T("CS"), _T("FluidSolver_Project2.cso"), (void**)&m_pFluidSolver_Project2);
	MakeShader(_T("CS"), _T("FluidSolver_Project3.cso"), (void**)&m_pFluidSolver_Project3);

	return S_OK;
}

//------------------------------------------------------------------------
//
//  �V�F�[�_�[�̍쐬�֐�
//
//  ����	TCHAR ProfileName[]	�쐬����V�F�[�_�[���
//								(VS,PS,GS,HS,DS,CS)
//			TCHAR FileName[]	�g�k�r�k�t�@�C����
//			void** ppShader		�쐬����V�F�[�_�[(OUT)
//			D3D11_INPUT_ELEMENT_DESC Fluid_layout[]	���_���C�A�E�g��`(�ȗ���)
//			UINT numElements						���_���C�A�E�g�G�������g��(�ȗ���)
//			ID3D11InputLayout** ppInputLayout		�쐬���钸�_���C�A�E�g(OUT)(�ȗ���)
//
//	�߂�l HRESULT
//		S_OK	= ����
//		E_FAIL	= �ُ�
//
//------------------------------------------------------------------------
HRESULT CShader::MakeShader(const TCHAR ProfileName[], const TCHAR FileName[], void** ppShader, D3D11_INPUT_ELEMENT_DESC Fluid_layout[], UINT numElements, ID3D11InputLayout** ppInputLayout)
{
	// �R���p�C���ς݃V�F�[�_�̓ǂݍ��ݔz��
	BYTE* pCso = nullptr;
	DWORD dwCsoSize = 0;

	// �R���p�C���ς݃V�F�[�_�̓ǂݍ��݂�����
	m_pD3D->ReadCso(FileName, &pCso, &dwCsoSize);

	// �V�F�[�_�[��ނ��Ƃ̍쐬����
	if (_tcscmp(ProfileName, _T("VS")) == 0)	// �o�[�e�b�N�X�V�F�[�_�[
	{
		if (FAILED(m_pD3D->m_pDevice->CreateVertexShader(pCso, dwCsoSize, nullptr, (ID3D11VertexShader**)ppShader)))
		{
			SAFE_DELETE_ARRAY(pCso);
			MessageBox(0, _T("�o�[�e�b�N�X�V�F�[�_�[�쐬���s"), FileName, MB_OK);
			return E_FAIL;
		}
		if (ppInputLayout)	// ���_�C���v�b�g���C�A�E�g���쐬����Ƃ�
		{
			// ���_�C���v�b�g���C�A�E�g���쐬
			if (FAILED(m_pD3D->m_pDevice->CreateInputLayout(Fluid_layout, numElements, pCso, dwCsoSize, ppInputLayout)))
			{
				SAFE_DELETE_ARRAY(pCso);
				MessageBox(0, _T("�C���v�b�g ���C�A�E�g�쐬���s"), FileName, MB_OK);
				return E_FAIL;
			}
		}
	}
	else if (_tcscmp(ProfileName, _T("PS")) == 0)	// �s�N�Z���V�F�[�_�[
	{
		if (FAILED(m_pD3D->m_pDevice->CreatePixelShader(pCso, dwCsoSize, nullptr, (ID3D11PixelShader**)ppShader)))
		{
			SAFE_DELETE_ARRAY(pCso);
			MessageBox(0, _T("�s�N�Z���V�F�[�_�[�쐬���s"), FileName, MB_OK);
			return E_FAIL;
		}
	}
	else if (_tcscmp(ProfileName, _T("GS")) == 0)	// �W�I���g���V�F�[�_�[
	{
		if (FAILED(m_pD3D->m_pDevice->CreateGeometryShader(pCso, dwCsoSize, nullptr, (ID3D11GeometryShader**)ppShader)))
		{
			SAFE_DELETE_ARRAY(pCso);
			MessageBox(0, _T("�W�I���g���V�F�[�_�[�쐬���s"), FileName, MB_OK);
			return E_FAIL;
		}
	}
	else if (_tcscmp(ProfileName, _T("HS")) == 0)	// �n���V�F�[�_�[
	{
		if (FAILED(m_pD3D->m_pDevice->CreateHullShader(pCso, dwCsoSize, nullptr, (ID3D11HullShader**)ppShader)))
		{
			SAFE_DELETE_ARRAY(pCso);
			MessageBox(0, _T("�n���V�F�[�_�[�쐬���s"), FileName, MB_OK);
			return E_FAIL;
		}
	}
	else if (_tcscmp(ProfileName, _T("DS")) == 0)	// �h���C���V�F�[�_�[
	{
		if (FAILED(m_pD3D->m_pDevice->CreateDomainShader(pCso, dwCsoSize, nullptr, (ID3D11DomainShader**)ppShader)))
		{
			SAFE_DELETE_ARRAY(pCso);
			MessageBox(0, _T("�h���C���V�F�[�_�[�쐬���s"), FileName, MB_OK);
			return E_FAIL;
		}
	}
	else if (_tcscmp(ProfileName, _T("CS")) == 0)	// �R���s���[�g�V�F�[�_
	{
		if (FAILED(m_pD3D->m_pDevice->CreateComputeShader(pCso, dwCsoSize, nullptr, (ID3D11ComputeShader**)ppShader)))
		{
			SAFE_DELETE_ARRAY(pCso);
			MessageBox(0, _T("�R���s���[�g�V�F�[�_�쐬���s"), FileName, MB_OK);
			return E_FAIL;
		}
	}
	else {
		SAFE_DELETE_ARRAY(pCso);
		MessageBox(0, _T("�V�F�[�_��ގw��G���["), ProfileName, MB_OK);
		return E_FAIL;
	}

	SAFE_DELETE_ARRAY(pCso);
	return S_OK;

}

//------------------------------------------------------------------------
//
//  �e��R���X�^���g�o�b�t�@�[�쐬
//
//  �����@�Ȃ�
//
//	�߂�l HRESULT
//		S_OK	= ����
//		E_FAIL	= �ُ�
//
//------------------------------------------------------------------------
HRESULT CShader::InitShaderConstant()
{


	// �t�H�O���n���p �R���X�^���g�o�b�t�@�[�쐬
	MakeConstantBuffer(sizeof(CONSTANT_BUFFER_FOG), &m_pConstantBufferFog);

	// �f�B�X�v���C�X�����g�}�b�s���O�p �R���X�^���g�o�b�t�@�[�쐬
	MakeConstantBuffer(sizeof(CONSTANT_BUFFER_DISPLACE), &m_pConstantBufferDisplace);

	// �G�t�F�N�g�p �R���X�^���g�o�b�t�@�[�쐬
	MakeConstantBuffer(sizeof(CONSTANT_BUFFER_EFFECT), &m_pConstantBufferEffect);

	// �X�v���C�g�p �R���X�^���g�o�b�t�@�[�쐬�@�����ł͕ϊ��s��n���p
	MakeConstantBuffer(sizeof(CONSTANT_BUFFER_SPRITE), &m_pConstantBufferSprite3D);

	// Fbx�R���X�^���g�o�b�t�@�[�쐬�@�����ł͕ϊ��s��n���p
	MakeConstantBuffer(sizeof(CONSTANT_BUFFER_WVLED), &m_pConstantBufferWVLED);

	// Fbx�R���X�^���g�o�b�t�@�[�쐬�@�����ł̓{�[���s��n���p
	MakeConstantBuffer(sizeof(MATRIX4X4) * MAX_BONES, &m_pConstantBufferBone2);

	// �R���X�^���g�o�b�t�@�[�쐬�@�}�e���A���n���p                                    // -- 2020.12.15
	MakeConstantBuffer(sizeof(CONSTANT_BUFFER_MATERIAL), &m_pConstantBufferMaterial);

	// Fbx�R���X�^���g�o�b�t�@�[�쐬�@ShadowMap�p
	MakeConstantBuffer(sizeof(CONSTANT_BUFFER_WVLLED), &m_pConstantBufferWVLLED);

	// HDR�p�@�R���X�^���g�o�b�t�@�[�쐬
	MakeConstantBuffer(sizeof(CONSTANT_BUFFER_HDR), &m_pConstantBufferHDR);

	// HDR Bloom�p�@�R���X�^���g�o�b�t�@�[�쐬
	MakeConstantBuffer(sizeof(VECTOR4) * 15, &m_pConstantBufferHDRBloom);

	// ���̌v�Z�p�R���X�^���g�o�b�t�@�[�쐬
	MakeConstantBuffer(sizeof(CONSTANT_BUFFER_FLUID), &m_pConstantBufferFluid);

	return S_OK;
}

//------------------------------------------------------------------------
//
//  �R���X�^���g�o�b�t�@�[�̍쐬�֐�
//
//  ����	UINT	size						�쐬����R���X�^���g�o�b�t�@�[�̃T�C�Y
//			ID3D11Buffer**  pppConstantBuffer	�쐬����R���X�^���g�o�b�t�@�[(OUT)
//
//	�߂�l HRESULT
//		S_OK	= ����
//		E_FAIL	= �ُ�
//
//------------------------------------------------------------------------
HRESULT CShader::MakeConstantBuffer(UINT size, ID3D11Buffer**  ppConstantBuffer)
{
	D3D11_BUFFER_DESC cb = { 0 };

	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cb.ByteWidth = size;
	cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cb.MiscFlags = 0;
	cb.StructureByteStride = 0;
	cb.Usage = D3D11_USAGE_DYNAMIC;

	if (FAILED(m_pD3D->m_pDevice->CreateBuffer(&cb, nullptr, ppConstantBuffer)))
	{
		MessageBox(0, _T("�R���X�^���g�o�b�t�@�[ �쐬���s"), nullptr, MB_OK);
		return E_FAIL;
	}
	return S_OK;
}


//------------------------------------------------------------------------  // -- 2019.4.14
//
//	�V�F�[�_�ɃR���X�^���g�o�b�t�@�e������ݒ肷�鏈��
//
// ����
//   const int&      nFog;          // �t�H�O�L��(0:���� 1:�w���t�H�O 2:�O�����h�t�H�O)
//   const float&    fFogVolume;    // �t�H�O�ʁi�w���j
//   const float&    fFogDensity;   // �t�H�O���x
//   const float&    fFogHeight;    // �t�H�O����
//   const VECTOR4&  vFogColor;     // �t�H�O�̐F
//
//	�߂�l HRESULT
//		S_OK	= ����
//		E_FAIL	= �ُ�
//
//------------------------------------------------------------------------
void CShader::SetConstantBufferFog(const int& nFog, const float& fFogVolume, const float& fFogDensity, const float& fFogHeight, const VECTOR4& vFogColor)
{
	//�V�F�[�_�[�̃R���X�^���g�o�b�t�@�[FOG�Ɋe��f�[�^��n��
	D3D11_MAPPED_SUBRESOURCE pData;
	CONSTANT_BUFFER_FOG cbf;

	if (SUCCEEDED(m_pD3D->m_pDeviceContext->Map(m_pConstantBufferFog, 0, D3D11_MAP_WRITE_DISCARD, 0, &pData)))
	{
		// �t�H�O����n��
		// x:�t�H�O�L��(0:���� 1:�w���t�H�O 2:�O�����h�t�H�O)  y:�t�H�O��  z: �t�H�O���x  w:�t�H�O����
		cbf.vFogInfo.x = (float)nFog;
		cbf.vFogInfo.y = fFogVolume;
		cbf.vFogInfo.z = fFogDensity;
		cbf.vFogInfo.w = fFogHeight;

		// �t�H�O�̐F��n��
		cbf.vFogColor = vFogColor;

		memcpy_s(pData.pData, pData.RowPitch, (void*)(&cbf), sizeof(cbf));
		m_pD3D->m_pDeviceContext->Unmap(m_pConstantBufferFog, 0);
	}

}
