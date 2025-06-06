//=============================================================================
//		�R�c�Q�[���v���O�����@�@�@�@�@                 ver 3.2        2023.1.31
//
//		�Q�[���̃��C������
//																GameMain.cpp
//=============================================================================
#include "GameMain.h"
#include "SceneManager.h"
#include "ObjectManager.h"
#include "ResourceManager.h"
#include "AudioManager.h"

namespace {
	CGameMain* gameMainInstance;
}

// ============================================================================================
//
// CGameMain �Q�[�����C���N���X�̏���
//
// ============================================================================================
//------------------------------------------------------------------------
//
//	�Q�[�����C���N���X�̃R���X�g���N�^	
//
//  �����@�Ȃ�
//
//------------------------------------------------------------------------
CGameMain::CGameMain(CMain*	pMain)
{
	gameMainInstance = this;
	m_pMain = pMain;		// ���C���N���X

	m_pD3D = nullptr;			//	Direct3D�I�u�W�F�N�g
	m_pXAudio = nullptr;		//	XAudio�I�u�W�F�N�g
	m_pDI = nullptr;			//	DirectInputD�I�u�W�F�N�g
	m_pFont = nullptr;			//	�t�H���g�e�N�X�`���[
	m_pShader = nullptr;		//	�V�F�[�_�[
	m_pFbxMeshCtrl = nullptr;	// ���b�V���R���g���[���N���X

	// �J�����E���C�g�E�r���[
	m_vEyePt = VECTOR3(0,0,0);		// �J�����i���_�j�ʒu
	m_vLookatPt = VECTOR3(0,0,0);	// �����ʒu
	m_mView = XMMatrixIdentity();	// �r���[�}�g���b�N�X
	m_mProj = XMMatrixIdentity();	// �v���W�F�N�V�����}�g���b�N�X
	m_mDrawView = XMMatrixIdentity();	// �r���[�}�g���b�N�X
	m_mDrawProj = XMMatrixIdentity();	// �v���W�F�N�V�����}�g���b�N�X
	m_vLightDir = VECTOR3(0,0,0);	// �f�B���N�V���i�����C�g�̕���
	m_vLightEye = VECTOR3(0, 0, 0);	//�f�B���N�V���i�����C�g�̃J�����i���_�j�ʒu
	for (int i = 0; i < MAX_CASCADE_SHADOW; i++)
	{
		m_mLightView[i] = XMMatrixIdentity();   //���C�g�r���[�}�g���b�N�X
		m_mLightProj[i] = XMMatrixIdentity();   //���C�g�v���W�F�N�V�����}�g���b�N�X
	}

}
//------------------------------------------------------------------------
//
//	�Q�[�����C���N���X�̃f�X�g���N�^	
//
//------------------------------------------------------------------------
CGameMain::~CGameMain()
{
	AudioManager::Release();
	ObjectManager::Release();
	SceneManager::Release();
	ResourceManager::Reset();
	MyImgui::ImguiQuit();          // -- 2020.11.15    // MyImgui�̏I������

	SAFE_DELETE(m_pFbxMeshCtrl);    // -- 2021.2.4
	SAFE_DELETE(m_pShader);
	SAFE_DELETE(m_pFont);
	SAFE_DELETE(m_pDI);
	SAFE_DELETE(m_pXAudio);
	SAFE_DELETE(m_pD3D);

	CoUninitialize();   // COM�̉��
}

//------------------------------------------------------------------------
//
//	�A�v���P�[�V�����̏���������	
//
//  �����@�Ȃ�
//
//	�߂�l HRESULT
//		S_OK	= ����
//		E_FAIL	= �ُ�
//
//------------------------------------------------------------------------
HRESULT CGameMain::Init()
{
	// �b�n�l�̏�����
	//CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	CoInitialize(nullptr);

	// Direct3D�̏�����
	m_pD3D = new CDirect3D;
	if (FAILED(m_pD3D->InitD3D(m_pMain->m_hWnd, WINDOW_WIDTH, WINDOW_HEIGHT))) {
		MessageBox(0, _T("Direct3D���������o���܂���"), nullptr, MB_OK);
		return E_FAIL;
	}

	// �V�F�[�_�[�̏�����
	m_pShader = new CShader(m_pD3D);
	if (FAILED(m_pShader->InitShader())) {
		MessageBox(0, _T("Shader�𐶐��E�������o���܂���"), nullptr, MB_OK);
		return E_FAIL;
	}

	// XAudio2�̏�����
	m_pXAudio = new CXAudio;
	if (FAILED(m_pXAudio->InitAudio(m_pMain->m_hWnd))) {
		MessageBox(0, _T("XAudio2���������o���܂���"), nullptr, MB_OK);
		return E_FAIL;
	}

	// DirectInput�̏�����
	m_pDI = new CDirectInput;
	if (!(m_pDI->StartDirectInput(m_pMain->m_hInstance, m_pMain->m_hWnd, 
                    INIT_KEYBOARD | INIT_MOUSE | INIT_JOYSTICK, WINDOW_WIDTH, WINDOW_HEIGHT))) {
		MessageBox(0, _T("DirectInput���������o���܂���"), nullptr, MB_OK);
		return E_FAIL;
	}

	// �t�H���g�e�N�X�`���[�̏�����
	m_pFont = new CFontTexture(m_pShader);

	// MyImgui�̏�����
	MyImgui::ImguiInit(m_pMain->m_hWnd, m_pD3D, WINDOW_WIDTH, WINDOW_HEIGHT);           // -- 2020.11.15

	// Fbx���b�V���R���g���[���N���X
	m_pFbxMeshCtrl = new CFbxMeshCtrl(m_pShader);                       // -- 2021.2.4

	// ----------------------------------------------------------------------------------------
	// �ϐ��̏�����
	m_vLightDir = normalize( VECTOR3(0.8f, 1, -1) );  // �������W�̏����l�B���K������

	// �v���W�F�N�V�����g�����X�t�H�[���i�ˉe�ϊ��j�̏����l
	//m_mProj = XMMatrixPerspectiveFovLH((FLOAT)(XM_PI / 4), (FLOAT)WINDOW_WIDTH / (FLOAT)WINDOW_HEIGHT, 0.1f, 1000.0);
	m_mProj = XMMatrixPerspectiveFovLH(XMConvertToRadians(38.0f), (FLOAT)WINDOW_WIDTH / (FLOAT)WINDOW_HEIGHT, 0.1f, 1000.0);
	m_mDrawProj = m_mProj;

	// ���C�g�v���W�F�N�V�����g�����X�t�H�[���i���ˉe�i���s���e�j�jm_mLightProj�̏����l  // -- 2020.4.5
	SetLightProj();

	// �r���[�g�����X�t�H�[���i���_���W�ϊ��j�̏����l
	VECTOR3 vUpVec(0.0f, 1.0f, 0.0f);//����ʒu
	m_vEyePt = VECTOR3(0.0f, 4.0f, -5.0f);
	m_vLookatPt = VECTOR3(0.0f, 2.0f, 0.0f);
	m_mView = XMMatrixLookAtLH(m_vEyePt, m_vLookatPt, vUpVec);
	m_mDrawView = m_mView;

	// ���C�g���_����̃r���[�g�����X�t�H�[��  m_vLightEye��m_mLightView�̏����l    // -- 2020.4.5
	// �E�Ȃ��A���_�ʒu�E�����_�ʒu��Camera.cpp�Ŗ����[�v���ƂɃJ�����ʒu�ɏ]���ĕύX���Ă���BSetLightView()�����s���Ă���B
	SetLightView();

	// �e�}�l�[�W���[�̍ŏ��ɂP�񂾂����s���鏈��
	AudioManager::Start();
	SceneManager::Start();
	ObjectManager::Start();

	// ���t���b�V�����[�g��ݒ肷��
	MainControl::UseFrameTimer(1.0f / 60.0f);

	return S_OK;
}

//------------------------------------------------------------------------
//
//	�Q�[���̃��[�v����	
//
//  �����@�Ȃ�
//
//	�߂�l �Ȃ�
//
//------------------------------------------------------------------------
void CGameMain::Update()
{
	MyImgui::ImguiNewFrame();   // MyImgui�`��O����

	m_pDI->GetInput();			// ���͏��̎擾
	m_pFont->Refresh();         // �t�H���g���̃��t���b�V��

	if (m_pDI->CheckKey(KD_TRG, DIK_F4))  ChangeScreenMode();   // �t���X�N���[���̐؂�ւ�       // -- 2018.12.14

	SceneManager::Update();
	ObjectManager::Update();

}

//------------------------------------------------------------------------
//
//	�Q�[���̃��C������	
//
//  �����@�Ȃ�
//
//	�߂�l �Ȃ�
//
//------------------------------------------------------------------------
void CGameMain::GameMain()
{
}

void CGameMain::Draw()
{
	//��ʃN���A�i���ۂ͒P�F�ŉ�ʂ�h��Ԃ������j
	float ClearColor[4] = { 0,0,0,1 };// �N���A�F�쐬�@RGBA�̏�
	m_pD3D->ClearRenderTarget(ClearColor); // ��ʃN���A

	SceneManager::Draw();
	ObjectManager::Draw();

	MyImgui::ImguiRender();      // MyImgui���`�揈��    // -- 2020.11.15

	//��ʍX�V�i�o�b�N�o�b�t�@���t�����g�o�b�t�@�Ɂj
	m_pD3D->m_pSwapChain->Present(1, 0);                   // Vsync
	//m_pD3D->m_pSwapChain->Present(0, 0);                   // Vsync�Ȃ�
}

//------------------------------------------------------------------------
//
//	�A�v���P�[�V�����̏I������	
//
//  �����@�Ȃ�
//
//	�߂�l �Ȃ�
//
//------------------------------------------------------------------------
void CGameMain::Quit()
{
	;
}

//------------------------------------------------------------------------    // -- 2020.4.5
//
// ���C�g�v���W�F�N�V�����g�����X�t�H�[���i���ˉe�i���s���e�j�jm_mLightProj�̐ݒ�
//
// �E���C�g�͕��s���̂��߃��C�g�v���W�F�N�V�����́A�������e�ł͂Ȃ����ˉe�i���s���e�j�ƂȂ�
// �E�{���A�ߕ��ʂ̏c�� = ZNear * tan(Y�����̎���p/2) * 2�@�ƂȂ邪�A
// �E�ߎ��_ZNear��1.0m�̂Ƃ��́A�ߕ��ʂ̏c�������̂܂ܕ\����ʂ̏c���ƂȂ�
//
//  �����@�Ȃ� 
//	�߂�l �Ȃ�
//
//------------------------------------------------------------------------
void CGameMain::SetLightProj()
{
	float ZNear = 1.0f;
	float NearH, NearW;
	for (int i = 0; i < MAX_CASCADE_SHADOW; i++)
	{
		if (i == 0)
		{
			// �J�X�P�[�h[0]�̉�ʏc��
			NearH = 44.0f;
		}
		else if (i == 1)
		{
			// �J�X�P�[�h[1]�̉�ʏc��
			NearH = 200.0f;
		}
		// �ߕ��ʂ̉��� = �ߕ��ʂ̏c�� * �A�X�y�N�g��B�\����ʂ̉����ƂȂ�
		NearW = NearH * (float)m_pD3D->m_dwDepthTexWidth / (float)m_pD3D->m_dwDepthTexHeight;
		m_mLightProj[i] = XMMatrixOrthographicLH(NearW, NearH, ZNear, 1000.0f);  // ���C�g�v���W�F�N�V�����i���ˉe�i���s���e�j�j
	}
}


//------------------------------------------------------------------------    // -- 2020.4.5
//
// ���C�g�r���[�g�����X�t�H�[���̐ݒ�
//
// ShadowMap�p��m_vLightEye��m_mLightView�̐ݒ�
// ���C�g���_����̃r���[�g�����X�t�H�[��
// �E���C�g���_�ʒu�́A�e�𐶐�����S�Ă̕��̂����ɂ��邱�ƁB�����ł�200���Ƃ���B
//
//  �����@�Ȃ�
//	�߂�l �Ȃ�
//
//------------------------------------------------------------------------
void CGameMain::SetLightView()
{
	VECTOR3 vUpVec(0.0f, 1.0f, 0.0f);//����ʒu

	// ���C�g���_����̃r���[�g�����X�t�H�[���iShadowMap�p�j���C�g���_���o�b�̈ʒu�ɍ��킹��    // -- 2020.4.4
	m_vLightEye = m_vLookatPt + m_vLightDir * 200.0f;	        // ���s���e�̏ꍇ���C�g���_�ʒu��200���Ƃ���
	for (int i = 0; i < MAX_CASCADE_SHADOW; i++)
	{
		VECTOR3 vOffset;
		if (i == 0)
		{
			// �J�X�P�[�h�O�Ԗڂ̓J��������P�T���������
			vOffset = GetPositionVector(XMMatrixTranslation(0, 0, 15) * GetRotateMatrix(GetLookatMatrix(m_vEyePt, m_vLookatPt)));
		}
		else {
			// �J�X�P�[�h�P�Ԗڂ̓J��������P�O�O���������
			vOffset = GetPositionVector(XMMatrixTranslation(0, 0, 100) * GetRotateMatrix(GetLookatMatrix(m_vEyePt, m_vLookatPt)));
		}
		m_mLightView[i] = XMMatrixLookAtLH(m_vLightEye + vOffset, m_vLookatPt + vOffset, vUpVec);
	}
}



//------------------------------------------------------------------------    // -- 2018.12.14
//
//	�E�B���h�E���[�h�ƃt���X�N���[�����[�h��؂�ւ��鏈��	
//
//  �����@���[�h  0:�E�C���h�E���[�h�@1:�t���X�N���[���@-1:���[�h�؂�ւ��i�ȗ��l�j
//
//	�߂�l �Ȃ�
//
//------------------------------------------------------------------------
HRESULT CGameMain::ChangeScreenMode(int nMode)
{
	HRESULT Ret = S_OK;
	BOOL bFullScreen;  // �t���X�N���[�����[�h���H

	//	���݂̃X�N���[�����[�h�𓾂�
	m_pD3D->m_pSwapChain->GetFullscreenState(&bFullScreen, nullptr);

	switch (nMode)
	{
	case 0:    // �E�C���h�E���[�h�ɂ���
		if( bFullScreen ) Ret = m_pD3D->m_pSwapChain->SetFullscreenState(false, nullptr);
		break;
	case 1:    // �t���X�N���[�����[�h�ɂ���
		if( !bFullScreen ) Ret = m_pD3D->m_pSwapChain->SetFullscreenState(true, nullptr);
		break;	
	default:   // ���݂̃��[�h��؂�ւ���
		if (bFullScreen)
		{
			Ret = m_pD3D->m_pSwapChain->SetFullscreenState(false, nullptr);
		}
		else {
			Ret = m_pD3D->m_pSwapChain->SetFullscreenState(true, nullptr);
		}
	}

	return Ret;
}

void CGameMain::SetWindowName(const char* name)
{
	SetWindowText(m_pMain->m_hWnd, name);
}

CGameMain* GameDevice()
{
	return gameMainInstance;
}
