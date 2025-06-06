#include "EffectManager.h"
#include "AudioManager.h"

namespace {
	const int EFFECT_ANIM_SPEED = 5;     // �A�j���[�V�����̑���
	const int EFFECT_MAX = 500;

}

EffectBillfire::EffectBillfire()
{
	ObjectManager::SetDrawOrder(this, -200);   // �����F�̂��ߒx�߂ɕ`�悷��

	uvOffset = VECTOR2(0,0);
	frame = 0;

	EffectManager* efmObj = ObjectManager::FindGameObject<EffectManager>();
	mesh = efmObj->Mesh();
	billB = efmObj->BillboardList("");

	AudioManager::Audio("SeNitro")->Play();

}
EffectBillfire::~EffectBillfire()
{
}

void EffectBillfire::SetEffectName(std::string name)
{
	billB = ObjectManager::FindGameObject<EffectManager>()->BillboardList(name);
}

void EffectBillfire::Update()
{
	int idx;

	frame += 60 * SceneManager::DeltaTime();
	idx = (int)(frame / EFFECT_ANIM_SPEED);

	if (idx >= billB->m_dwNumX * billB->m_dwNumY)	// �p�^�[���̍Ō�ɒB�����Ƃ�
	{
		if (Parent() == nullptr)
		{
			DestroyMe();
			return;
		}
		else {
			frame = 0;	  // �e������Ƃ��́A�폜�͐e���s��
		}
	}

	// �A�j���[�V�����̂��߂̃e�N�X�`���I�t�Z�b�g��ݒ肷��
	uvOffset.x = (float)(idx % billB->m_dwNumX * billB->m_dwSrcWidth);   // �w�����̕ψ�
	uvOffset.y = (float)(idx / billB->m_dwNumX * billB->m_dwSrcHeight);  // �x�����̕ψ�

}

void EffectBillfire::DrawDepth()
{
	// �e�����Ȃ�
}
void EffectBillfire::DrawScreen()
{

	//�g�p����V�F�[�_�[�̃Z�b�g
	GameDevice()->m_pD3D->m_pDeviceContext->VSSetShader(GameDevice()->m_pShader->m_pEffect3D_VS_BILL, NULL, 0);
	GameDevice()->m_pD3D->m_pDeviceContext->PSSetShader(GameDevice()->m_pShader->m_pEffect3D_PS, NULL, 0);

	//�r���{�[�h�́A���_���������[���h�g�����X�t�H�[�������߂�
	MATRIX4X4 mWorld = GetLookatMatrix(transform.position, GameDevice()->m_vEyePt);
	// �`�撆�S�ʒu�̈ړ�������
	MATRIX4X4 mPosUp = XMMatrixTranslation(billB->m_fDestCenterX - billB->m_fDestWidth / 2,
		billB->m_fDestCenterY - billB->m_fDestHeight / 2, 0);
	mWorld = mPosUp * mWorld;

	//�o�[�e�b�N�X�o�b�t�@�[���Z�b�g
	UINT stride = sizeof(BILLBOARD_VERTEX);
	UINT offset = 0;
	ID3D11Buffer* pVertexBuffer = billB->m_pVertexBuffer;   // �o�[�e�b�N�X�o�b�t�@
	GameDevice()->m_pD3D->m_pDeviceContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);

	UINT mask = 0xffffffff;
	if (billB->m_nBlendFlag == 1)   // ���Z�����F�w��
	{
		// ���Z�����F�̃u�����f�B���O��ݒ�
		GameDevice()->m_pD3D->m_pDeviceContext->OMSetBlendState(GameDevice()->m_pD3D->m_pBlendStateAdd, NULL, mask);
	}

	//�V�F�[�_�[�̃R���X�^���g�o�b�t�@�[�Ɋe��f�[�^��n��
	D3D11_MAPPED_SUBRESOURCE pData;
	CONSTANT_BUFFER_EFFECT cb;
	ZeroMemory(&cb, sizeof(cb));

	if (SUCCEEDED(GameDevice()->m_pD3D->m_pDeviceContext->Map(GameDevice()->m_pShader->m_pConstantBufferEffect, 0, D3D11_MAP_WRITE_DISCARD, 0, &pData)))
	{
		//���[���h�A�J�����A�ˉe�s��A�e�N�X�`���[�I�t�Z�b�g��n��
		cb.mWVP = XMMatrixTranspose(mWorld * GameDevice()->m_mView * GameDevice()->m_mProj);

		cb.vUVOffset.x = uvOffset.x / billB->m_dwImageWidth;		// �e�N�X�`���A�j���[�V�����̃I�t�Z�b�g
		cb.vUVOffset.y = uvOffset.y / billB->m_dwImageHeight;		// �e�N�X�`���A�j���[�V�����̃I�t�Z�b�g
		cb.vUVScale.x = 1;
		cb.vUVScale.y = 1;
		cb.fAlpha = billB->m_fAlpha;

		memcpy_s(pData.pData, pData.RowPitch, (void*)(&cb), sizeof(cb));
		GameDevice()->m_pD3D->m_pDeviceContext->Unmap(GameDevice()->m_pShader->m_pConstantBufferEffect, 0);
	}
	//���̃R���X�^���g�o�b�t�@�[���ǂ̃V�F�[�_�[�Ŏg����
	GameDevice()->m_pD3D->m_pDeviceContext->VSSetConstantBuffers(0, 1, &GameDevice()->m_pShader->m_pConstantBufferEffect);
	GameDevice()->m_pD3D->m_pDeviceContext->PSSetConstantBuffers(0, 1, &GameDevice()->m_pShader->m_pConstantBufferEffect);
	//���_�C���v�b�g���C�A�E�g���Z�b�g
	GameDevice()->m_pD3D->m_pDeviceContext->IASetInputLayout(GameDevice()->m_pShader->m_pEffect3D_VertexLayout_BILL);
	//�v���~�e�B�u�E�g�|���W�[���Z�b�g
	GameDevice()->m_pD3D->m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	//�e�N�X�`���[���V�F�[�_�[�ɓn��
	GameDevice()->m_pD3D->m_pDeviceContext->PSSetSamplers(0, 1, &GameDevice()->m_pD3D->m_pSampleLinear);
	GameDevice()->m_pD3D->m_pDeviceContext->PSSetShaderResources(0, 1, &billB->m_pTexture);
	//�v���~�e�B�u�������_�����O
	GameDevice()->m_pD3D->m_pDeviceContext->Draw(4, 0);

	if (billB->m_nBlendFlag == 1)   // ���Z�����F�w��
	{
		// �ʏ�̃u�����f�B���O�ɖ߂�
		GameDevice()->m_pD3D->m_pDeviceContext->OMSetBlendState(GameDevice()->m_pD3D->m_pBlendStateTrapen, NULL, mask);
	}

}
