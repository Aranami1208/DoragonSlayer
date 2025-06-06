#include "EffectManager.h"
#include "AudioManager.h"

namespace {
	const int EFFECT_ANIM_SPEED = 5;     // �A�j���[�V�����̑���

}

EffectBillboard::EffectBillboard()
{
	ObjectManager::SetDrawOrder(this, -200);   // �����F�̂��ߒx�߂ɕ`�悷��

	uvOffset = VECTOR2(0,0);
	frame = 0;

	EffectManager* efmObj = ObjectManager::FindGameObject<EffectManager>();
	mesh = efmObj->Mesh();
	billB = efmObj->BillboardList("");

	AudioManager::Audio("SeNitro")->Play();

}
EffectBillboard::~EffectBillboard()
{
}

void EffectBillboard::SetEffectName(std::string name)
{
	billB = ObjectManager::FindGameObject<EffectManager>()->BillboardList(name);
}

void EffectBillboard::Update()
{
	int idx;

	frame += 60 * SceneManager::DeltaTime();
	idx = (int)(frame / EFFECT_ANIM_SPEED);

	if (idx >= billB->m_dwNumX * billB->m_dwNumY)	// �p�^�[���̍Ō�ɒB�����Ƃ�
	{
		DestroyMe();
		return;
	}

	// �A�j���[�V�����̂��߂̃e�N�X�`���I�t�Z�b�g��ݒ肷��
	uvOffset.x = (float)(idx % billB->m_dwNumX * billB->m_dwSrcWidth);   // �w�����̕ψ�
	uvOffset.y = (float)(idx / billB->m_dwNumX * billB->m_dwSrcHeight);  // �x�����̕ψ�

}

void EffectBillboard::DrawDepth()
{
	 // �e�����Ȃ�
}
void EffectBillboard::DrawScreen()
{

	//�g�p����V�F�[�_�[�̓o�^	
	GameDevice()->m_pD3D->m_pDeviceContext->VSSetShader(GameDevice()->m_pShader->m_pEffect3D_VS_BILLMESH, NULL, 0);
	GameDevice()->m_pD3D->m_pDeviceContext->PSSetShader(GameDevice()->m_pShader->m_pEffect3D_PS, NULL, 0);

	//�r���{�[�h�́A���_���������[���h�g�����X�t�H�[�������߂�
	MATRIX4X4 mWorld = GetLookatMatrix(transform.position, GameDevice()->m_vEyePt);
	// �`�撆�S�ʒu�̈ړ�������
	MATRIX4X4 mPosUp = XMMatrixTranslation(billB->m_fDestCenterX - billB->m_fDestWidth / 2,
							billB->m_fDestCenterY - billB->m_fDestHeight / 2, 0);
	mWorld = mPosUp * mWorld;
	// ���b�V���̑傫���Ɋg�傷��ݒ������i���X�̃��b�V���͂P���l���̔����j
	MATRIX4X4 mScale = XMMatrixScaling(billB->m_fDestWidth, billB->m_fDestHeight, 
							(billB->m_fDestWidth + billB->m_fDestHeight) / 2);
	mWorld = mScale * mWorld;

	UINT mask = 0xffffffff;
	if (billB->m_nBlendFlag == 1)   // ���Z�����F�w��
	{
		// ���Z�����F�̃u�����f�B���O��ݒ�
		GameDevice()->m_pD3D->m_pDeviceContext->OMSetBlendState(GameDevice()->m_pD3D->m_pBlendStateAdd, NULL, mask);
	}

	//�V�F�[�_�[�̃R���X�^���g�o�b�t�@�[�Ɋe��f�[�^��n��	
	D3D11_MAPPED_SUBRESOURCE pData;
	CONSTANT_BUFFER_EFFECT cb;
	if (SUCCEEDED(GameDevice()->m_pD3D->m_pDeviceContext->Map(GameDevice()->m_pShader->m_pConstantBufferEffect, 0, D3D11_MAP_WRITE_DISCARD, 0, &pData)))
	{
		//���[���h�A�J�����A�ˉe�s��A�e�N�X�`���[�I�t�Z�b�g��n��
		cb.mWVP = XMMatrixTranspose(mWorld * GameDevice()->m_mView * GameDevice()->m_mProj);

		cb.vUVOffset.x = uvOffset.x /  billB->m_dwImageWidth;			// �e�N�X�`���A�j���[�V�����̃I�t�Z�b�g
		cb.vUVOffset.y = uvOffset.y /  billB->m_dwImageHeight;			// �e�N�X�`���A�j���[�V�����̃I�t�Z�b�g
		cb.vUVScale.x = (float) billB->m_dwSrcWidth /  billB->m_dwImageWidth;		// �e�N�X�`���A�j���[�V������1/����
		cb.vUVScale.y = (float) billB->m_dwSrcHeight /  billB->m_dwImageHeight;	// �e�N�X�`���A�j���[�V������1/�c��

		cb.fAlpha =  billB->m_fAlpha;

		memcpy_s(pData.pData, pData.RowPitch, (void*)(&cb), sizeof(cb));
		cb.mWVP = XMMatrixTranspose(mWorld * GameDevice()->m_mView * GameDevice()->m_mProj);
		cb.mWVP = XMMatrixTranspose(mWorld * GameDevice()->m_mView * GameDevice()->m_mProj);
		cb.mWVP = XMMatrixTranspose(mWorld * GameDevice()->m_mView * GameDevice()->m_mProj);
		cb.mWVP = XMMatrixTranspose(mWorld * GameDevice()->m_mView * GameDevice()->m_mProj);
		GameDevice()->m_pD3D->m_pDeviceContext->Unmap(GameDevice()->m_pShader->m_pConstantBufferEffect, 0);
	}

	//���̃R���X�^���g�o�b�t�@�[���ǂ̃V�F�[�_�[�Ŏg����
	GameDevice()->m_pD3D->m_pDeviceContext->VSSetConstantBuffers(0, 1, &GameDevice()->m_pShader->m_pConstantBufferEffect);
	GameDevice()->m_pD3D->m_pDeviceContext->PSSetConstantBuffers(0, 1, &GameDevice()->m_pShader->m_pConstantBufferEffect);

	//���_�C���v�b�g���C�A�E�g���Z�b�g
	GameDevice()->m_pD3D->m_pDeviceContext->IASetInputLayout(GameDevice()->m_pShader->m_pFbxStaticShadowMap_VertexLayout);

	//�v���~�e�B�u�E�g�|���W�[���Z�b�g
	GameDevice()->m_pD3D->m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//�e�N�X�`���[�T���v���[���V�F�[�_�[�ɓn��
	GameDevice()->m_pD3D->m_pDeviceContext->PSSetSamplers(0, 1, &GameDevice()->m_pD3D->m_pSampleLinear);

	// ���b�V���̕`�揇�����肷��m_dwRenderIdxArray�̐ݒ�
	mesh->SetRenderIdxArray(mWorld, GameDevice()->m_vEyePt);

	// ���b�V���̐������e�N�X�`���[�A�o�[�e�b�N�X�o�b�t�@�A�C���f�b�N�X�o�b�t�@���Z�b�g���āA�����_�����O����
	for (DWORD mi = 0; mi < mesh->m_dwMeshNum; mi++)
	{
		DWORD i = mesh->m_RenderOrder[mi].Idx;		  // -- 2025.3.15

		//�e�N�X�`���[���V�F�[�_�[�ɓn��
		GameDevice()->m_pD3D->m_pDeviceContext->PSSetShaderResources(0, 1, & billB->m_pTexture);

		// �o�[�e�b�N�X�o�b�t�@�[���Z�b�g
		UINT stride = sizeof(StaticVertexNormal);
		UINT offset = 0;
		GameDevice()->m_pD3D->m_pDeviceContext->IASetVertexBuffers(0, 1, &mesh->m_pMeshArray[i].m_pVertexBuffer, &stride, &offset);

		//�C���f�b�N�X�o�b�t�@�[���Z�b�g
		GameDevice()->m_pD3D->m_pDeviceContext->IASetIndexBuffer(mesh->m_pMeshArray[i].m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

		//�v���~�e�B�u�������_�����O
		// �i�C���f�b�N�X�̐����w�肵�ă����_�����O�j
		GameDevice()->m_pD3D->m_pDeviceContext->DrawIndexed(mesh->m_pMeshArray[i].m_dwIndicesNum, 0, 0);
	}

	if ( billB->m_nBlendFlag == 1)   // ���Z�����F�w��
	{
		// �ʏ�̃u�����f�B���O�ɖ߂�
		GameDevice()->m_pD3D->m_pDeviceContext->OMSetBlendState(GameDevice()->m_pD3D->m_pBlendStateTrapen, NULL, mask);
	}

}
