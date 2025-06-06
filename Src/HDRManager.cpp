//=============================================================================
//		�g�c�q�`��̃v���O����
//�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@       ver 3.3        2024.3.23
//
//																HDRManager.cpp
//=============================================================================
#include "HDRManager.h"
#include "HDRControl.h"

//------------------------------------------------------------------------
//
//	HDRManager  �R���X�g���N�^	
//
//------------------------------------------------------------------------
HDRManager::HDRManager()
{
	m_pD3D = GameDevice()->m_pD3D;
	m_pShader = GameDevice()->m_pShader;

	ObjectManager::DontDestroy(this);		// ������Ȃ�
	ObjectManager::SetActive(this, false);	// �X�V���Ȃ�
	ObjectManager::SetVisible(this, false);	// �`�揈�����Ȃ�

	SingleInstantiate<HDRControl>();	 // HDR�R���g���[�����쐬����B�폜����Ȃ��B

	m_pVertexBufferHDR = nullptr;

	// HDR�e�N�X�`���p
	m_pHDRMap_Tex = nullptr;
	m_pHDRMap_TexRTV = nullptr;
	m_pHDRMap_TexSRV = nullptr;
	m_pHDRMap_DSTex = nullptr;
	m_pHDRMap_DSTexDSV = nullptr;
	m_pHDRBright_Tex = nullptr;
	m_pHDRBright_TexRTV = nullptr;
	m_pHDRBright_TexSRV = nullptr;
	for (int i = 0; i < HDR_BLOOMTEX_COUNT; i++)
	{
		m_pHDRBloom_Tex[i] = nullptr;
		m_pHDRBloom_TexRTV[i] = nullptr;
		m_pHDRBloom_TexSRV[i] = nullptr;
	}
	m_fLimit = 1.0f;	  // �u���C�g�̂������l
	m_fForce = 1.0f;	  // �u���[���̋���

	DWORD dwWidth  = m_pD3D->m_dwWindowWidth;
	DWORD dwHeight = m_pD3D->m_dwWindowHeight;

	//�o�[�e�b�N�X�o�b�t�@�[�쐬
	//�C�����邱�ƁBz�l���P�ȏ�ɂ��Ȃ��B�N���b�v��Ԃ�z=1�͍ł������Ӗ�����B���������ĕ`�悳��Ȃ��B
	HDRVertex vertices[] =
	{
		VECTOR3(0,              (float)dwHeight, 0), VECTOR2(0,1),      //���_1  ����
		VECTOR3(0,                            0, 0), VECTOR2(0,0),      //���_2�@����    // -- 2024.3.23
		VECTOR3((float)dwWidth, (float)dwHeight, 0), VECTOR2(1,1),      //���_3�@�E��    // -- 2024.3.23
		VECTOR3((float)dwWidth,               0, 0), VECTOR2(1,0),      //���_4�@�E��
	};

	// �o�[�e�b�N�X�o�b�t�@�쐬����
	D3D11_BUFFER_DESC bd;
	//bd.Usage          = D3D11_USAGE_DEFAULT;
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(HDRVertex) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	//bd.CPUAccessFlags = 0;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = vertices;
	if (FAILED(m_pD3D->m_pDevice->CreateBuffer(&bd, &InitData, &m_pVertexBufferHDR)))
	{
		MessageBox(0, _T("HDR.cpp �o�[�e�b�N�X�o�b�t�@�[�쐬���s"), nullptr, MB_OK);
	}


	// ---------------------------------------------------------------------------------------
	// HDR�}�b�v�e�N�X�`���[���쐬
	D3D11_TEXTURE2D_DESC tdesc;
	ZeroMemory(&tdesc, sizeof(D3D11_TEXTURE2D_DESC));
	tdesc.Width = m_pD3D->m_dwWindowWidth;  // HDR�e�N�X�`���̑傫��
	tdesc.Height = m_pD3D->m_dwWindowHeight;
	tdesc.MipLevels = 1;     // �~�b�v�}�b�v�E���x����
	tdesc.ArraySize = 1;
	tdesc.MiscFlags = 0;
	//tdesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT; // �e�N�X�`���E�t�H�[�}�b�g
	tdesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;  // �e�N�X�`���E�t�H�[�}�b�g
	tdesc.SampleDesc.Count = 1;
	tdesc.SampleDesc.Quality = 0;
	tdesc.Usage = D3D11_USAGE_DEFAULT;
	tdesc.BindFlags = D3D11_BIND_RENDER_TARGET |      // �`��^�[�Q�b�g
		D3D11_BIND_SHADER_RESOURCE;       // �V�F�[�_�Ŏg��
	tdesc.CPUAccessFlags = 0;
	m_pD3D->m_pDevice->CreateTexture2D(&tdesc, nullptr, &m_pHDRMap_Tex);

	// HDR�}�b�v�e�N�X�`���[�p�@�����_�[�^�[�Q�b�g�r���[�쐬
	D3D11_RENDER_TARGET_VIEW_DESC DescRT;
	DescRT.Format = tdesc.Format;
	DescRT.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	DescRT.Texture2D.MipSlice = 0;
	m_pD3D->m_pDevice->CreateRenderTargetView(m_pHDRMap_Tex, &DescRT, &m_pHDRMap_TexRTV);

	// HDR�}�b�v�e�N�X�`���p�@�V�F�[�_�[���\�[�X�r���[(SRV)�쐬	
	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	ZeroMemory(&SRVDesc, sizeof(SRVDesc));
	SRVDesc.Format = tdesc.Format;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Texture2D.MipLevels = tdesc.MipLevels;
	m_pD3D->m_pDevice->CreateShaderResourceView(m_pHDRMap_Tex, &SRVDesc, &m_pHDRMap_TexSRV);

	// HDR�}�b�v�e�N�X�`���������_�[�^�[�Q�b�g�ɂ���ۂ̃f�v�X�X�e���V���r���[�p�̃e�N�X�`���[���쐬
	D3D11_TEXTURE2D_DESC descDepth;
	descDepth.Width = m_pD3D->m_dwWindowWidth;
	descDepth.Height = m_pD3D->m_dwWindowHeight;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D32_FLOAT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	m_pD3D->m_pDevice->CreateTexture2D(&descDepth, nullptr, &m_pHDRMap_DSTex);

	//���̃e�N�X�`���[�ɑ΂��f�v�X�X�e���V���r���[(DSV)���쐬
	m_pD3D->m_pDevice->CreateDepthStencilView(m_pHDRMap_DSTex, nullptr, &m_pHDRMap_DSTexDSV);

	// �r���[���쐬������e�N�X�`���͕s�v�Ȃ̂ō폜    // -- 2022.6.29
	SAFE_RELEASE(m_pHDRMap_Tex);
	SAFE_RELEASE(m_pHDRMap_DSTex);

	// ----------------------------------------------------------
	// HDR�u���C�g�e�N�X�`���[���쐬
	ZeroMemory(&tdesc, sizeof(D3D11_TEXTURE2D_DESC));
	tdesc.Width = m_pD3D->m_dwWindowWidth;  // HDR�e�N�X�`���̑傫��
	tdesc.Height = m_pD3D->m_dwWindowHeight;
	tdesc.MipLevels = 1;     // �~�b�v�}�b�v�E���x����
	tdesc.ArraySize = 1;
	tdesc.MiscFlags = 0;
	//tdesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT; // �e�N�X�`���E�t�H�[�}�b�g
	tdesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;  // �e�N�X�`���E�t�H�[�}�b�g
	tdesc.SampleDesc.Count = 1;
	tdesc.SampleDesc.Quality = 0;
	tdesc.Usage = D3D11_USAGE_DEFAULT;
	tdesc.BindFlags = D3D11_BIND_RENDER_TARGET |      // �`��^�[�Q�b�g
		D3D11_BIND_SHADER_RESOURCE;       // �V�F�[�_�Ŏg��
	tdesc.CPUAccessFlags = 0;
	m_pD3D->m_pDevice->CreateTexture2D(&tdesc, nullptr, &m_pHDRBright_Tex);

	// HDR�u���C�g�e�N�X�`���[�p�@�����_�[�^�[�Q�b�g�r���[�쐬
	DescRT.Format = tdesc.Format;
	DescRT.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	DescRT.Texture2D.MipSlice = 0;
	m_pD3D->m_pDevice->CreateRenderTargetView(m_pHDRBright_Tex, &DescRT, &m_pHDRBright_TexRTV);

	// HDR�u���C�g�e�N�X�`���p�@�V�F�[�_�[���\�[�X�r���[(SRV)�쐬	
	ZeroMemory(&SRVDesc, sizeof(SRVDesc));
	SRVDesc.Format = tdesc.Format;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Texture2D.MipLevels = tdesc.MipLevels;
	m_pD3D->m_pDevice->CreateShaderResourceView(m_pHDRBright_Tex, &SRVDesc, &m_pHDRBright_TexSRV);

	// �r���[���쐬������e�N�X�`���͕s�v�Ȃ̂ō폜    // -- 2022.6.29
	SAFE_RELEASE(m_pHDRBright_Tex);

	// ----------------------------------------------------------
	// HDR�u���[���e�N�X�`���[���쐬
	for (int i = 0; i < HDR_BLOOMTEX_COUNT; i++)
	{
		ZeroMemory(&tdesc, sizeof(D3D11_TEXTURE2D_DESC));
		tdesc.Width = m_pD3D->m_dwWindowWidth;  // HDR�e�N�X�`���̑傫��
		tdesc.Height = m_pD3D->m_dwWindowHeight;
		tdesc.MipLevels = 1;     // �~�b�v�}�b�v�E���x����
		tdesc.ArraySize = 1;
		tdesc.MiscFlags = 0;
		//tdesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT; // �e�N�X�`���E�t�H�[�}�b�g
		tdesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;  // �e�N�X�`���E�t�H�[�}�b�g
		tdesc.SampleDesc.Count = 1;
		tdesc.SampleDesc.Quality = 0;
		tdesc.Usage = D3D11_USAGE_DEFAULT;
		tdesc.BindFlags = D3D11_BIND_RENDER_TARGET |      // �`��^�[�Q�b�g
			D3D11_BIND_SHADER_RESOURCE;       // �V�F�[�_�Ŏg��
		tdesc.CPUAccessFlags = 0;
		m_pD3D->m_pDevice->CreateTexture2D(&tdesc, nullptr, &m_pHDRBloom_Tex[i]);

		// HDR�u���[���e�N�X�`���[�p�@�����_�[�^�[�Q�b�g�r���[�쐬
		DescRT.Format = tdesc.Format;
		DescRT.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		DescRT.Texture2D.MipSlice = 0;
		m_pD3D->m_pDevice->CreateRenderTargetView(m_pHDRBloom_Tex[i], &DescRT, &m_pHDRBloom_TexRTV[i]);

		// HDR�u���[���e�N�X�`���p�@�V�F�[�_�[���\�[�X�r���[(SRV)�쐬	
		ZeroMemory(&SRVDesc, sizeof(SRVDesc));
		SRVDesc.Format = tdesc.Format;
		SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		SRVDesc.Texture2D.MipLevels = tdesc.MipLevels;
		m_pD3D->m_pDevice->CreateShaderResourceView(m_pHDRBloom_Tex[i], &SRVDesc, &m_pHDRBloom_TexSRV[i]);

		// �r���[���쐬������e�N�X�`���͕s�v�Ȃ̂ō폜    // -- 2022.6.29
		SAFE_RELEASE(m_pHDRBloom_Tex[i]);
	}

}

//------------------------------------------------------------------------
//
//	HDRManager  �f�X�g���N�^	
//
//------------------------------------------------------------------------
HDRManager::~HDRManager()
{
	SAFE_RELEASE(m_pVertexBufferHDR);

	SAFE_RELEASE(m_pHDRMap_Tex);
	SAFE_RELEASE(m_pHDRMap_TexRTV);
	SAFE_RELEASE(m_pHDRMap_TexSRV);
	SAFE_RELEASE(m_pHDRMap_DSTex);
	SAFE_RELEASE(m_pHDRMap_DSTexDSV);

	SAFE_RELEASE(m_pHDRBright_Tex);
	SAFE_RELEASE(m_pHDRBright_TexRTV);
	SAFE_RELEASE(m_pHDRBright_TexSRV);

	for (int i = 0; i < HDR_BLOOMTEX_COUNT; i++)
	{
		SAFE_RELEASE(m_pHDRBloom_Tex[i]);
		SAFE_RELEASE(m_pHDRBloom_TexRTV[i]);
		SAFE_RELEASE(m_pHDRBloom_TexSRV[i]);
	}

}
//------------------------------------------------------------------------
//
//	�����_�[�^�[�Q�b�g��HDR�}�b�v�e�N�X�`���[�ɂ���	
//
//	����
//		float BackColor[]�F�N�����[�F
//
//	�߂�l
//		�Ȃ�
//------------------------------------------------------------------------
void   HDRManager::SetRenderTargetHDR(float BackColor[])
{
	// �����_�[�^�[�Q�b�g��HDR�}�b�v�e�N�X�`���[�ɐݒ肷��
	m_pD3D->SetRenderTarget(m_pHDRMap_TexRTV, m_pHDRMap_DSTexDSV);

	// HDR�}�b�v�e�N�X�`���̃N���A
	m_pD3D->ClearRenderTarget(BackColor); // ��ʃN���A
}

//------------------------------------------------------------------------
void   HDRManager::SetLimitForce(float limit, float force)
{
	m_fLimit = limit;		// �u���C�g�̂������l
	m_fForce = force;		// �u���[���̋���
}

//------------------------------------------------------------------------
//
//	HDR�����_�����O	
//
//	����
//		�Ȃ�
//
//	�߂�l
//------------------------------------------------------------------------
void   HDRManager::Render()
{
	float ClearColor[4] = { 0.0f,0.0f,0.0f,1 };

	//   �����_�[�^�[�Q�b�g��HDR�u���C�g�e�N�X�`���[�ɂ���
	m_pD3D->SetRenderTarget(m_pHDRBright_TexRTV, nullptr);
	// HDR�u���C�g�e�N�X�`���̃N���A
	m_pD3D->m_pDeviceContext->ClearRenderTargetView(m_pHDRBright_TexRTV, ClearColor); // ��ʃN���A

	// HDR�u���C�g
	RenderBright();

	// HDR�u���[��
	RenderBloom();

	// �����_�[�^�[�Q�b�g��ʏ�i�o�b�N�o�b�t�@�[�j�ɖ߂�
	m_pD3D->SetRenderTarget(nullptr, nullptr);
	m_pD3D->ClearRenderTarget(ClearColor); // ��ʃN���A

	// �����F�̖����ʏ�̃u�����f�B���O��ݒ�                  // -- 2020.1.24
	UINT mask = 0xffffffff;
	m_pD3D->m_pDeviceContext->OMSetBlendState(m_pD3D->m_pBlendStateNormal, nullptr, mask);

	// �ŏI�����_�����O
	RenderFinish();

	// �����F�̃u�����f�B���O��ݒ�
	m_pD3D->m_pDeviceContext->OMSetBlendState(m_pD3D->m_pBlendStateTrapen, nullptr, mask);

}


//------------------------------------------------------------------------
//
//	�����_�����O   �u���C�g	
//
//	����
//		�Ȃ�
//
//	�߂�l
//		�Ȃ�
//------------------------------------------------------------------------
void   HDRManager::RenderBright()
{

	//�g�p����V�F�[�_�[�̃Z�b�g
	m_pD3D->m_pDeviceContext->VSSetShader(m_pShader->m_pHDR_VS, nullptr, 0);
	m_pD3D->m_pDeviceContext->PSSetShader(m_pShader->m_pHDR_PS_BRIGHT, nullptr, 0);

	//���̃R���X�^���g�o�b�t�@�[���ǂ̃V�F�[�_�[�Ŏg����
	m_pD3D->m_pDeviceContext->VSSetConstantBuffers(0, 1, &m_pShader->m_pConstantBufferHDR);
	m_pD3D->m_pDeviceContext->PSSetConstantBuffers(0, 1, &m_pShader->m_pConstantBufferHDR);
	//���_�C���v�b�g���C�A�E�g���Z�b�g
	m_pD3D->m_pDeviceContext->IASetInputLayout(m_pShader->m_pHDR_VertexLayout);

	//�v���~�e�B�u�E�g�|���W�[���Z�b�g
	m_pD3D->m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// �T���v���[���Z�b�g
	m_pD3D->m_pDeviceContext->PSSetSamplers(0, 1, &m_pD3D->m_pSampleLinear);

	//�o�[�e�b�N�X�o�b�t�@�[���Z�b�g
	UINT stride = sizeof(HDRVertex);
	UINT offset = 0;
	m_pD3D->m_pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBufferHDR, &stride, &offset);

	//�V�F�[�_�[�̃R���X�^���g�o�b�t�@�[�Ɋe��f�[�^��n��
	D3D11_MAPPED_SUBRESOURCE pData;
	CONSTANT_BUFFER_HDR     cb;
	if (SUCCEEDED(m_pD3D->m_pDeviceContext->Map(m_pShader->m_pConstantBufferHDR, 0, D3D11_MAP_WRITE_DISCARD, 0, &pData)))
	{

		//�r���[�|�[�g�T�C�Y��n���i�N���C�A���g�̈�̉��Əc�j
		cb.ViewPort.x = m_pD3D->m_dwWindowWidth;
		cb.ViewPort.y = m_pD3D->m_dwWindowHeight;
		cb.Info = VECTOR2(m_fLimit, 0);    // �u���C�g�̂������l
		memcpy_s(pData.pData, pData.RowPitch, (void*)(&cb), sizeof(cb));
		m_pD3D->m_pDeviceContext->Unmap(m_pShader->m_pConstantBufferHDR, 0);
	}

	//�e�N�X�`���[���V�F�[�_�[�ɓn��
	m_pD3D->m_pDeviceContext->PSSetShaderResources(0, 1, &m_pHDRMap_TexSRV);

	//�v���~�e�B�u�������_�����O
	m_pD3D->m_pDeviceContext->Draw(4, 0);

}

//------------------------------------------------------------------------
//
//	�����_�����O   �u���[��	
//
//	����
//		�Ȃ�
//
//	�߂�l
//		�Ȃ�
//------------------------------------------------------------------------
void   HDRManager::RenderBloom()
{
	float ClearColor[4] = { 0.0f,0.0f,0.0f,1 };

	//�g�p����V�F�[�_�[�̃Z�b�g
	m_pD3D->m_pDeviceContext->VSSetShader(m_pShader->m_pHDR_VS, nullptr, 0);
	m_pD3D->m_pDeviceContext->PSSetShader(m_pShader->m_pHDR_PS_BLOOM, nullptr, 0);

	//���̃R���X�^���g�o�b�t�@�[���ǂ̃V�F�[�_�[�Ŏg����
	m_pD3D->m_pDeviceContext->VSSetConstantBuffers(0, 1, &m_pShader->m_pConstantBufferHDR);
	m_pD3D->m_pDeviceContext->PSSetConstantBuffers(0, 1, &m_pShader->m_pConstantBufferHDR);

	//�V�F�[�_�[�̃R���X�^���g�o�b�t�@�[�Ɋe��f�[�^��n��
	D3D11_MAPPED_SUBRESOURCE pData;
	CONSTANT_BUFFER_HDR   cb;
	if (SUCCEEDED(m_pD3D->m_pDeviceContext->Map(m_pShader->m_pConstantBufferHDR, 0, D3D11_MAP_WRITE_DISCARD, 0, &pData)))
	{
		//�r���[�|�[�g�T�C�Y��n���i�N���C�A���g�̈�̉��Əc�j
		cb.ViewPort.x = m_pD3D->m_dwWindowWidth;
		cb.ViewPort.y = m_pD3D->m_dwWindowHeight;
		cb.Info = VECTOR2(0, 0);
		memcpy_s(pData.pData, pData.RowPitch, (void*)(&cb), sizeof(cb));
		m_pD3D->m_pDeviceContext->Unmap(m_pShader->m_pConstantBufferHDR, 0);
	}

	//���_�C���v�b�g���C�A�E�g���Z�b�g
	m_pD3D->m_pDeviceContext->IASetInputLayout(m_pShader->m_pHDR_VertexLayout);

	//�v���~�e�B�u�E�g�|���W�[���Z�b�g
	m_pD3D->m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// �T���v���[���Z�b�g
	m_pD3D->m_pDeviceContext->PSSetSamplers(0, 1, &m_pD3D->m_pSampleLinear);

	//�o�[�e�b�N�X�o�b�t�@�[���Z�b�g
	UINT stride = sizeof(HDRVertex);
	UINT offset = 0;
	m_pD3D->m_pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBufferHDR, &stride, &offset);

	//���̃R���X�^���g�o�b�t�@�[���ǂ̃V�F�[�_�[�Ŏg����
	m_pD3D->m_pDeviceContext->VSSetConstantBuffers(1, 1, &m_pShader->m_pConstantBufferHDRBloom);
	m_pD3D->m_pDeviceContext->PSSetConstantBuffers(1, 1, &m_pShader->m_pConstantBufferHDRBloom);


	// �u���[���̏���
	// �u���[���e�N�X�`���z��ɏ��Ԃɏ������݂Ȃ���u���[�������������Ă���
	// �ŏ��A�u���C�g�e�N�X�`����ǂݍ���ŁA�u���[���e�N�X�`���̂O�Ԃɏ�������
	// ���Ƀu���[���e�N�X�`���̂O�Ԃ�ǂݍ���ŁA�u���[���e�N�X�`���̂P�Ԃɏ�������
	// ���̂悤�ɏ��Ԃɏ������݂Ȃ���A�u���[�������������Ă����B
	// �ŏI���ʂ́A�Ō�̃u���[���e�N�X�`�� HDR_BLOOMTEX_COUNT-1�� �ɏ������܂�Ă���
	for (int j = 0; j < HDR_BLOOMTEX_COUNT; j++)
	{
		// �����_�[�^�[�Q�b�g��HDR�u���[���e�N�X�`���[�ɂ���
		m_pD3D->SetRenderTarget( m_pHDRBloom_TexRTV[j], nullptr);

		// HDR�u���[���e�N�X�`���̃N���A
		m_pD3D->m_pDeviceContext->ClearRenderTargetView(m_pHDRBloom_TexRTV[j], ClearColor); // ��ʃN���A

		VECTOR4  cbh[15] = {};  // �I�t�Z�b�gx,y���E�G�C�gw
		float total = 0;

		// �u���[���s�N�Z���̃T���v�����O
		// ������1�s�N�Z���{�㉺�܂��͍��E��7�s�N�Z���ÂA���v�P�T�s�N�Z��
		for (int i = 0; i < 15; i++)
		{
			float p = (i - (15 - 1)*0.5f)*0.0008f;   // ���S�_����̃T���v�����O����
			cbh[i] = (j % 2 == 0) ? VECTOR4(p, 0, 0, 0) : VECTOR4(0, p, 0, 0);  // �s�N�Z���I�t�Z�b�g �P�񂲂Ƃɉ��Əc��؂�ւ��� x,y
			cbh[i].w = exp(-p*p / 2) / sqrt(3.14159f * 2);       // �K�E�X���z�i���K���z�j�ɂ���ăE�F�C�g�l w �����v���Ă���
			total += cbh[i].w;
		}

		for (int i = 0; i < 15; i++) cbh[i].w /= total;  // �E�F�C�g�lw���쐬����

		if (SUCCEEDED(m_pD3D->m_pDeviceContext->Map(m_pShader->m_pConstantBufferHDRBloom, 0, D3D11_MAP_WRITE_DISCARD, 0, &pData)))
		{
			memcpy_s(pData.pData, pData.RowPitch, (void*)(&cbh), sizeof(cbh));
			m_pD3D->m_pDeviceContext->Unmap(m_pShader->m_pConstantBufferHDRBloom, 0);
		}

		// �e�N�X�`���[���V�F�[�_�[�ɓn��
		if (j == 0)
		{
			// �ŏ��́A�u���C�g�e�N�X�`����n��
			m_pD3D->m_pDeviceContext->PSSetShaderResources(0, 1, &m_pHDRBright_TexSRV);
		}
		else {
			// �Q��ڂ���́A��O�̃u���[���e�N�X�`����n��
			m_pD3D->m_pDeviceContext->PSSetShaderResources(0, 1, &m_pHDRBloom_TexSRV[j - 1]);
		}

		//�v���~�e�B�u�������_�����O
		m_pD3D->m_pDeviceContext->Draw(4, 0);

	}

}

//------------------------------------------------------------------------
//
//	�ŏI�����_�����O	
//
//	����
//		�Ȃ�
//
//	�߂�l
//		�Ȃ�
//------------------------------------------------------------------------
void   HDRManager::RenderFinish()
{
	//�g�p����V�F�[�_�[�̃Z�b�g
	m_pD3D->m_pDeviceContext->VSSetShader(m_pShader->m_pHDR_VS, nullptr, 0);
	m_pD3D->m_pDeviceContext->PSSetShader(m_pShader->m_pHDR_PS, nullptr, 0);

	//���̃R���X�^���g�o�b�t�@�[���ǂ̃V�F�[�_�[�Ŏg����
	m_pD3D->m_pDeviceContext->VSSetConstantBuffers(0, 1, &m_pShader->m_pConstantBufferHDR);
	m_pD3D->m_pDeviceContext->PSSetConstantBuffers(0, 1, &m_pShader->m_pConstantBufferHDR);
	//���_�C���v�b�g���C�A�E�g���Z�b�g
	m_pD3D->m_pDeviceContext->IASetInputLayout(m_pShader->m_pHDR_VertexLayout);

	//�v���~�e�B�u�E�g�|���W�[���Z�b�g
	m_pD3D->m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// �T���v���[���Z�b�g
	m_pD3D->m_pDeviceContext->PSSetSamplers(0, 1, &m_pD3D->m_pSampleLinear);

	// Z�o�b�t�@�𖳌���
	m_pD3D->SetZBuffer(FALSE);

	//�o�[�e�b�N�X�o�b�t�@�[���Z�b�g
	UINT stride = sizeof(HDRVertex);
	UINT offset = 0;
	m_pD3D->m_pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBufferHDR, &stride, &offset);

	//�V�F�[�_�[�̃R���X�^���g�o�b�t�@�[�Ɋe��f�[�^��n��
	D3D11_MAPPED_SUBRESOURCE pData;
	CONSTANT_BUFFER_HDR     cb;
	if (SUCCEEDED(m_pD3D->m_pDeviceContext->Map(m_pShader->m_pConstantBufferHDR, 0, D3D11_MAP_WRITE_DISCARD, 0, &pData)))
	{

		//�r���[�|�[�g�T�C�Y��n���i�N���C�A���g�̈�̉��Əc�j
		cb.ViewPort.x = m_pD3D->m_dwWindowWidth;
		cb.ViewPort.y = m_pD3D->m_dwWindowHeight;
		cb.Info = VECTOR2(0, m_fForce);            // �u���[���̋����@1.0f�ȏ�
		memcpy_s(pData.pData, pData.RowPitch, (void*)(&cb), sizeof(cb));
		m_pD3D->m_pDeviceContext->Unmap(m_pShader->m_pConstantBufferHDR, 0);
	}

	//�e�N�X�`���[���V�F�[�_�[�ɓn��
	m_pD3D->m_pDeviceContext->PSSetShaderResources(0, 1, &m_pHDRMap_TexSRV);   // �g�c�q�}�b�v�e�N�X�`��
	m_pD3D->m_pDeviceContext->PSSetShaderResources(1, 1, &m_pHDRBloom_TexSRV[HDR_BLOOMTEX_COUNT-1]);   // �ŏI��HDR�u���[���e�N�X�`��

	//�v���~�e�B�u�������_�����O
	m_pD3D->m_pDeviceContext->Draw(4, 0);

	//Z�o�b�t�@��L����
	m_pD3D->SetZBuffer(TRUE);

}