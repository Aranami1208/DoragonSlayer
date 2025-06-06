#include "EffectManager.h"
#include "EffectBase.h"
#include "Player.h"

EffectBase::EffectBase()
{
}

EffectBase::~EffectBase()
{
}

bool EffectBase::LoadBillTexture(const TCHAR* szFName, BILLBOARDBASE* pBillBase)
{
	//�e�N�X�`���[�ǂݍ���	
	if (FAILED(GameDevice()->m_pD3D->CreateShaderResourceViewFromFile(szFName, &pBillBase->m_pTexture,
						pBillBase->m_dwImageWidth, pBillBase->m_dwImageHeight, 3)))
	{
		MessageBox(0, _T("�r���{�[�h�@�e�N�X�`���[��ǂݍ��߂܂���"), szFName, MB_OK);
		return false;
	}

	return true;
}

bool EffectBase::SetBillSrc(BILLBOARDBASE* pBillBase)
{
	// �r���{�[�h�p�̔|���S�����쐬���A�o�[�e�b�N�X�o�b�t�@���쐬����

	//�o�[�e�b�N�X�o�b�t�@�[�쐬(�������̍��W�ō쐬)
	BILLBOARD_VERTEX vertices[] =
	{
		VECTOR3(-pBillBase->m_fDestWidth / 2,  pBillBase->m_fDestHeight / 2, 0), VECTOR2((float)(pBillBase->m_dwSrcX + pBillBase->m_dwSrcWidth) / pBillBase->m_dwImageWidth, (float)pBillBase->m_dwSrcY / pBillBase->m_dwImageHeight),                     //���_1  ����
		VECTOR3(-pBillBase->m_fDestWidth / 2, -pBillBase->m_fDestHeight / 2, 0), VECTOR2((float)(pBillBase->m_dwSrcX + pBillBase->m_dwSrcWidth) / pBillBase->m_dwImageWidth, (float)(pBillBase->m_dwSrcY + pBillBase->m_dwSrcHeight) / pBillBase->m_dwImageHeight),   //���_2�@����	  // -- 2024.3.23
		VECTOR3(pBillBase->m_fDestWidth / 2,  pBillBase->m_fDestHeight / 2, 0), VECTOR2((float)pBillBase->m_dwSrcX / pBillBase->m_dwImageWidth,                (float)pBillBase->m_dwSrcY / pBillBase->m_dwImageHeight),                        //���_2�@�E��					      // -- 2024.3.23
		VECTOR3(pBillBase->m_fDestWidth / 2, -pBillBase->m_fDestHeight / 2, 0), VECTOR2((float)pBillBase->m_dwSrcX / pBillBase->m_dwImageWidth,                (float)(pBillBase->m_dwSrcY + pBillBase->m_dwSrcHeight) / pBillBase->m_dwImageHeight),      //���_4�@�E��
	};

	// �Q�x�ڈȍ~�ɏ���������邱�Ƃ̑΍�
	SAFE_RELEASE(pBillBase->m_pVertexBuffer);

	// �o�[�e�b�N�X�o�b�t�@�[�쐬
	D3D11_BUFFER_DESC bd;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(BILLBOARD_VERTEX) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = vertices;
	if (FAILED(GameDevice()->m_pD3D->m_pDevice->CreateBuffer(&bd, &InitData, &pBillBase->m_pVertexBuffer)))
	{
		return false;
	}

	return true;
}


bool EffectBase::LoadPartTexture(const TCHAR* szFName, PARTICLEBASE* pPartBase)
{
	//�e�N�X�`���[�ǂݍ���	
	if (FAILED(GameDevice()->m_pD3D->CreateShaderResourceViewFromFile(szFName, &pPartBase->m_pTexture,
		pPartBase->m_dwImageWidth, pPartBase->m_dwImageHeight, 3)))
	{
		MessageBox(0, _T("�p�[�e�B�N���@�e�N�X�`���[��ǂݍ��߂܂���"), szFName, MB_OK);
		return false;
	}

	return true;
}

bool EffectBase::SetPartSrc(PARTICLEBASE* pPartBase)
{
	//�o�[�e�b�N�X
	PARTICLE_VERTEX vertices[] =
	{
		VECTOR3(0.0f, 0.0f, 0.0f)
	};

	// �Q�x�ڈȍ~�ɏ���������邱�Ƃ̑΍�
	SAFE_RELEASE(pPartBase->m_pVertexBuffer);

	// �o�[�e�b�N�X�o�b�t�@�[�쐬
	D3D11_BUFFER_DESC bd;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(PARTICLE_VERTEX) * 1;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = vertices;
	if (FAILED(GameDevice()->m_pD3D->m_pDevice->CreateBuffer(&bd, &InitData, &pPartBase->m_pVertexBuffer)))
	{
		return false;
	}

	return true;

}
