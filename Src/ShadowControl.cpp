#include "ShadowControl.h"
#include "HDRManager.h"

ShadowControl::ShadowControl()
{
	ObjectManager::DontDestroy(this);		// ������Ȃ�
	ObjectManager::SetActive(this, false);		// �X�V���Ȃ�
	ObjectManager::SetDrawOrder(this, 100000);	// ��ԍŏ��ɕ`�揈������
}

ShadowControl::~ShadowControl()
{
}

void ShadowControl::SetShadow(bool sdw)
{
	if (sdw)
	{
		ObjectManager::SetDrawTimes(MAX_CASCADE_SHADOW + 1);
	}
	else {
		ObjectManager::SetDrawTimes(1);
	}
}

// �V���h�E�}�b�v�ւ̏������݂̏�������
void ShadowControl::DrawDepth()
{
	// �u�����h�X�e�[�g�}�X�N
	UINT mask = 0xffffffff;

	//�r���[�|�[�g�̐ݒ�(�[�x�e�N�X�`���[�p�̃r���[�|�[�g)
	D3D11_VIEWPORT vp;
	vp.Width = GameDevice()->m_pD3D->m_dwDepthTexWidth;
	vp.Height = GameDevice()->m_pD3D->m_dwDepthTexHeight;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	GameDevice()->m_pD3D->m_pDeviceContext->RSSetViewports(1, &vp);

	// �����_�[�^�[�Q�b�g��[�x�e�N�X�`���[�ɕύX
	float ClearColor[4] = { 0,0,0,1 };// �N���A�F�쐬�@RGBA�̏�

	// �����F�̖����ʏ�̃u�����f�B���O��ݒ�                  // -- 2020.9.13
	// �i�[�x�}�b�v�e�N�X�`���֏������ނƂ��́A�����F���l�������u�����f�B���O�͕s�j
	// �i�Ȃ��Ȃ�A�������ޒl�͐F�ł͂Ȃ��[�x�l�̂��߁B�[�x�l���̂��u�����f�B���O���Ă��܂����߁j
	// �i�Ȃ��A�s�N�Z���V�F�[�_�̒��œ����F�͐؂�̂ĂĂ���̂ŁA�����F�ɂ͉e���ł��Ȃ��j
	GameDevice()->m_pD3D->m_pDeviceContext->OMSetBlendState(GameDevice()->m_pD3D->m_pBlendStateNormal, nullptr, mask);

	// �J�X�P�[�h�V���h�E���ƂɃ����_�����O����
	int cn = ObjectManager::DrawCounter();

	GameDevice()->m_pD3D->SetRenderTarget(GameDevice()->m_pD3D->m_pDepthMap_TexRTV[cn], GameDevice()->m_pD3D->m_pDepthMap_DSTexDSV[cn]);
	GameDevice()->m_pD3D->ClearRenderTarget(ClearColor);

	// �����ʒu�ɃJ������u��
	GameDevice()->m_mView = GameDevice()->m_mLightView[cn];
	GameDevice()->m_mProj = GameDevice()->m_mLightProj[cn];
}

// ��ʂւ̏������݂̏�������
void ShadowControl::DrawScreen()
{

	//�r���[�|�[�g�̐ݒ�����ɖ߂�
	D3D11_VIEWPORT vp;
	vp.Width = GameDevice()->m_pD3D->m_dwWindowWidth;
	vp.Height = GameDevice()->m_pD3D->m_dwWindowHeight;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	GameDevice()->m_pD3D->m_pDeviceContext->RSSetViewports(1, &vp);

	// �����F�̃u�����f�B���O��ݒ�
	UINT mask = 0xffffffff;
	GameDevice()->m_pD3D->m_pDeviceContext->OMSetBlendState(GameDevice()->m_pD3D->m_pBlendStateTrapen, nullptr, mask);

	// �J������ʏ�ʒu�ɖ߂�
	GameDevice()->m_mView = GameDevice()->m_mDrawView;
	GameDevice()->m_mProj = GameDevice()->m_mDrawProj;

	// HDR���~�b�g��1.0��菬�����Ƃ��́A�����_�[�^�[�Q�b�g��HDR�}�b�v�e�N�X�`���ɂ���
	float ClearColor[4] = { 0,0,0,1 };// �N���A�F�쐬�@RGBA�̏�
	if (ObjectManager::FindGameObject<HDRManager>()->Limit() < 1.0f)
	{
		ObjectManager::FindGameObject<HDRManager>()->SetRenderTargetHDR(ClearColor);   // HDR�}�b�v�e�N�X�`���ɐ؂�ւ��ĉ�ʃN�����[
	}
	else {
		GameDevice()->m_pD3D->SetRenderTarget(nullptr, nullptr);      //�����_�[�^�[�Q�b�g��ʏ�i�o�b�N�o�b�t�@�[�j�ɖ߂�
		GameDevice()->m_pD3D->ClearRenderTarget(ClearColor);    // �ʏ�̉�ʃN�����[
	}
}
