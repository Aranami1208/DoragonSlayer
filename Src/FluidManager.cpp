
#include "FluidManager.h"
#include "Fluid.h"

// ---------------------------------------------------------------------------
//
// ���̃}�l�[�W���̃R���X�g���N�^
//
//   �Ȃ��A�I�u�W�F�N�g��delete��CBaseProc�̃f�X�g���N�^�ōs�����ߕs�v
//
// ---------------------------------------------------------------------------
FluidManager::FluidManager()
{
	ObjectManager::DontDestroy(this);		// ���̂͏�����Ȃ�
	tagNo = 0;	// �^�O�m�n�̏����l

	Init();   // ����������
}
// ---------------------------------------------------------------------------
//
//  ���̃}�l�[�W���̃f�X�g���N�^
//
// ---------------------------------------------------------------------------
FluidManager::~FluidManager()
{
	SAFE_RELEASE(m_pCubeVBuffer);
	SAFE_RELEASE(m_pCubeIBuffer);
	SAFE_RELEASE(m_pFrontFace);
	SAFE_RELEASE(m_pBackFace);
	SAFE_RELEASE(m_pSampleLinearFluid);

	m_TexA.Release();
	m_TexB.Release();
}
/*
// ---------------------------------------------------------------------------
//
// ���̃}�l�[�W���ɗ��̃I�u�W�F�N�g��ǉ�����
//
//   ���̃I�u�W�F�N�g����Ă΂�A���̃I�u�W�F�N�g��p�̗��̃I�u�W�F�N�g��
//   �쐬����Ƃ��Ɏg�p����
//   ���̃}�l�[�W���ɗ��̃I�u�W�F�N�g��ǉ����A�e�I�u�W�F�N�g�A�h���X��ݒ肷��
//   �߂�l�ɂ́A�ǉ������I�u�W�F�N�g�ԍ����Ԃ�
//
//   ����
//		CBaseObj* pOyaObj  :  �e�I�u�W�F�N�g(���̃I�u�W�F�N�g)
//
//   �߂�l
//		int  �쐬�����I�u�W�F�N�g�ԍ�
// ---------------------------------------------------------------------------
Object3D* FluidManager::AddObj(Object3D* pOyaObj)
{
	Fluid* obj = Instantiate<Fluid>();
	if (obj == nullptr)  return nullptr;

	return obj;
}
*/
// ---------------------------------------------------------------------------
//
//  ���̃}�l�[�W���̏�����
//
// ---------------------------------------------------------------------------
void FluidManager::Init()
{
	D3D11_RENDER_TARGET_VIEW_DESC rdesc;
	D3D11_TEXTURE2D_DESC desc;
	D3D11_SHADER_RESOURCE_VIEW_DESC sdesc;
	D3D11_BUFFER_DESC bdesc;
	D3D11_SUBRESOURCE_DATA InitData;

	// 1�p�X�p�@�e�N�X�`���[A�̃����_�[�^�[�Q�b�g�֘A�쐬 ------------------
	// �e�N�X�`���[�̍쐬
	ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
	desc.Width = WINDOW_WIDTH;
	desc.Height = WINDOW_HEIGHT;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	GameDevice()->m_pD3D->m_pDevice->CreateTexture2D(&desc, NULL, &m_TexA.pTex2D);

	// ���̃e�N�X�`���[�ɑ΂������_�[�^�[�Q�b�g�r���[(RTV)���쐬	
	ZeroMemory(&rdesc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
	rdesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	rdesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rdesc.Texture2DArray.MipSlice = 0;
	GameDevice()->m_pD3D->m_pDevice->CreateRenderTargetView(m_TexA.pTex2D, &rdesc, &m_TexA.pRTV);

	// ���̃e�N�X�`���[�ɑ΂��V�F�[�_�[���\�[�X�r���[(SRV)���쐬		
	ZeroMemory(&sdesc, sizeof(sdesc));
	sdesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sdesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	sdesc.Texture2D.MipLevels = 1;
	GameDevice()->m_pD3D->m_pDevice->CreateShaderResourceView(m_TexA.pTex2D, &sdesc, &m_TexA.pSRV);

	// ���̃e�N�X�`���[�̃����_�[�^�[�Q�b�g���ɔ����f�v�X�X�e���V���e�N�X�`���[�쐬
	desc.Width = WINDOW_WIDTH;
	desc.Height = WINDOW_HEIGHT;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_D32_FLOAT;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	GameDevice()->m_pD3D->m_pDevice->CreateTexture2D(&desc, NULL, &m_TexA.pDSTex2D);

	// �f�v�X�X�e���V���e�N�X�`���[��DSV���쐬
	GameDevice()->m_pD3D->m_pDevice->CreateDepthStencilView(m_TexA.pDSTex2D, NULL, &m_TexA.pDSV);


	// 2�p�X�p�@�e�N�X�`���[B�̃����_�[�^�[�Q�b�g�֘A�쐬 ----------------------
	// �e�N�X�`���[�̍쐬
	ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
	desc.Width = WINDOW_WIDTH;
	desc.Height = WINDOW_HEIGHT;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	GameDevice()->m_pD3D->m_pDevice->CreateTexture2D(&desc, NULL, &m_TexB.pTex2D);

	// ���̃e�N�X�`���[�ɑ΂������_�[�^�[�Q�b�g�r���[(RTV)���쐬	
	ZeroMemory(&rdesc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
	rdesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	rdesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rdesc.Texture2DArray.MipSlice = 0;
	GameDevice()->m_pD3D->m_pDevice->CreateRenderTargetView(m_TexB.pTex2D, &rdesc, &m_TexB.pRTV);

	// ���̃e�N�X�`���[�ɑ΂��V�F�[�_�[���\�[�X�r���[(SRV)���쐬		
	ZeroMemory(&sdesc, sizeof(sdesc));
	sdesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sdesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	sdesc.Texture2D.MipLevels = 1;
	GameDevice()->m_pD3D->m_pDevice->CreateShaderResourceView(m_TexB.pTex2D, &sdesc, &m_TexB.pSRV);

	// ���̃e�N�X�`���[�̃����_�[�^�[�Q�b�g���ɔ����f�v�X�X�e���V���e�N�X�`���[�쐬
	desc.Width = WINDOW_WIDTH;
	desc.Height = WINDOW_HEIGHT;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_D32_FLOAT;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	GameDevice()->m_pD3D->m_pDevice->CreateTexture2D(&desc, NULL, &m_TexB.pDSTex2D);

	// �f�v�X�X�e���V���e�N�X�`���[��DSV���쐬
	GameDevice()->m_pD3D->m_pDevice->CreateDepthStencilView(m_TexB.pDSTex2D, NULL, &m_TexB.pDSV);


	// ��̂R�c�e�N�X�`����\�����邽�߂̗����̂��쐬

	// �o�[�e�b�N�X�o�b�t�@���쐬����
	MakeCubeVertexBuffer(1);   // �Ƃ肠�����P���l���̗����̂��쐬����

	// �C���f�b�N�X�o�b�t�@m_pCubeIBuffer�쐬
	// �E�O�p�`�̒��_���E����\�Ƃ���		  // -- 2024.3.23
	int IndexData[] =
	{
		2,1,0,
		2,3,1,
		1,4,0,
		1,5,4,
		3,5,1,
		3,7,5,
		3,2,6,
		7,3,6,
		2,0,4,
		6,2,4,
		5,6,4,
		7,6,5
	};
	bdesc.Usage = D3D11_USAGE_DEFAULT;
	bdesc.ByteWidth = sizeof(int) * 12 * 3;
	bdesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bdesc.CPUAccessFlags = 0;
	bdesc.MiscFlags = 0;
	InitData.pSysMem = IndexData;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;
	if (FAILED(GameDevice()->m_pD3D->m_pDevice->CreateBuffer(&bdesc, &InitData, &m_pCubeIBuffer)))
		return;


	// �e�N�X�`���[�p�T���v���[�쐬
	D3D11_SAMPLER_DESC SamDesc;
	ZeroMemory(&SamDesc, sizeof(D3D11_SAMPLER_DESC));
	SamDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	SamDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	SamDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	SamDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	SamDesc.BorderColor[0] = 0; SamDesc.BorderColor[1] = 0; SamDesc.BorderColor[2] = 0; SamDesc.BorderColor[3] = 0;
	GameDevice()->m_pD3D->m_pDevice->CreateSamplerState(&SamDesc, &m_pSampleLinearFluid);


	// ���X�^���C�Y�ݒ�
	D3D11_RASTERIZER_DESC rdc;
	ZeroMemory(&rdc, sizeof(rdc));
	rdc.CullMode = D3D11_CULL_BACK;		// �������̎O�p�`��`�悵�܂���
	rdc.FillMode = D3D11_FILL_SOLID;
	rdc.FrontCounterClockwise = FALSE;	// �O�p�`�̒��_�������_�[�^�[�Q�b�g��ŉE���Ȃ�ΎO�p�`�͑O����	 // -- 2024.3.23
	GameDevice()->m_pD3D->m_pDevice->CreateRasterizerState(&rdc, &m_pFrontFace);

	rdc.CullMode = D3D11_CULL_FRONT;	// �O�����̎O�p�`��`�悵�܂���
	rdc.FillMode = D3D11_FILL_SOLID;
	rdc.FrontCounterClockwise = FALSE;	// �O�p�`�̒��_�������_�[�^�[�Q�b�g��ŉE���Ȃ�ΎO�p�`�͑O����	 // -- 2024.3.23
	GameDevice()->m_pD3D->m_pDevice->CreateRasterizerState(&rdc, &m_pBackFace);

}

// ---------------------------------------------------------------------------
//
// ���̂�������i�x�����j�ɔ���������
//
//   ��ɁA�΂��R���Ă���悤�ȁA������ɔ������鉊�≌�𔭐�������
//
//   const Transform& trans     :	�����ʒu�̃��[���h�}�g���b�N�X
//   const float&     fCubeSize  :	�\���p�����̂̈�ӂ̑傫��
//   const float&     fColorNo   :	���̂̐F�R�[�h�i0:���i�����Ȃ��j1: ���i��������j2:���� 3:�Z���� 4:�������j
//   const float&     fDensity   :	�������x
//   const float&     fVelocity  :	���x
//   const float&     fRandom    :	���x���������_����(0.0f:������ �` 1.0f:�����������_��)
//   const int&    nAddVoxelWidth:	�����ӏ����甭��������Ƃ��̃T�C�Y�i�{�N�Z�����j�B�P�J���̏ꍇ�͂O�ɂ���B
//   std::string&     tag        :	�ݒ肷��^�O(in/out)
//
// �߂�l�@Fluid* : ���������I�u�W�F�N�g�A�h���X�B�����ł��Ȃ������Ƃ���nullptr
//
// ---------------------------------------------------------------------------
Fluid* FluidManager::SpawnY(const Transform& trans, const float& fCubeSize, const float& fColorNo, const float& fDensity, const float& fVelocity, const float& fRandom, const int& nAddVoxelWidth, std::string& tag)
{
	Fluid* obj = SetFluidObj(tag);
	if (obj == nullptr)  return nullptr;

	// ���̂𔭐�������
	obj->Spawn( trans, fCubeSize, fColorNo, VECTOR4(FLUID_SIZE / 2, 6, FLUID_SIZE / 2, 2),  // VoxelPos  x,y,z �� �������a
												VECTOR4(fDensity, 0, 0, 0), VECTOR4(0, fVelocity, 0, 0), fRandom, nAddVoxelWidth);

	return obj;
}

// ---------------------------------------------------------------------------
//
// ���̂��������i�y�����j�ɔ���������
//
//   ��ɁA�Ή����ˊ�̂悤�ȁA�O�����ɔ������鉊�≌�𔭐�������
//
//   const Transform& trans     :	�����ʒu�̃��[���h�}�g���b�N�X
//   const float&     fCubeSize  :	�\���p�����̂̈�ӂ̑傫��
//   const float&     fColorNo   :	���̂̐F�R�[�h�i0:���i�����Ȃ��j1: ���i��������j2:���� 3:�Z���� 4:�������j
//   const float&     fDensity   :	�������x
//   const float&     fVelocity  :	���x
//   const float&     fRandom    :	���x���������_����(0.0f:������ �` 1.0f:�����������_��)
//   const int&    nAddVoxelWidth:	�����ӏ����甭��������Ƃ��̃T�C�Y�i�{�N�Z�����j�B�P�J���̏ꍇ�͂O�ɂ���B
//   std::string&      tag        :	�ݒ肷��^�O(in/out)
//
// �߂�l�@Fluid* : ���������I�u�W�F�N�g�A�h���X�B�����ł��Ȃ������Ƃ���nullptr
//
// ---------------------------------------------------------------------------
Fluid*  FluidManager::SpawnZ(const Transform& trans, const float& fCubeSize, const float& fColorNo, const float& fDensity, const float& fVelocity, const float& fRandom, const int& nAddVoxelWidth, std::string& tag)
{
	Fluid* obj = SetFluidObj(tag);
	if (obj == nullptr)  return nullptr;

	// ���̂𔭐�������
	obj->Spawn( trans, fCubeSize, fColorNo, VECTOR4(FLUID_SIZE / 2, FLUID_SIZE / 2, 6, 2),  // VoxelPos  x,y,z �� �������a
												VECTOR4(fDensity, 0, 0, 0), VECTOR4(0, 0, fVelocity, 0), fRandom, nAddVoxelWidth);

	return obj;
}

// ---------------------------------------------------------------------------
//
// ���̂��������i�y�����j�ɔ���������
//
//   ��ɁA�Ή����ˊ�̂悤�ȁA�O�����ɔ������鉊�≌�𔭐�������
//
//   const Transform& trans     :	�����ʒu�̃��[���h�}�g���b�N�X
//   const float&     fCubeSize  :	�\���p�����̂̈�ӂ̑傫��
//   const float&     fColorNo   :	���̂̐F�R�[�h�i0:���i�����Ȃ��j1: ���i��������j2:���� 3:�Z���� 4:�������j
//   const float&     fDensity   :	�������x
//   const float&     fVelocity  :	���x
//   const float&     fRandom    :	���x���������_����(0.0f:������ �` 1.0f:�����������_��)
//   const int&    nAddVoxelWidth:	�����ӏ����甭��������Ƃ��̃T�C�Y�i�{�N�Z�����j�B�P�J���̏ꍇ�͂O�ɂ���B
//   std::string&      tag        :	�ݒ肷��^�O(in/out)
//
// �߂�l�@Fluid* : ���������I�u�W�F�N�g�A�h���X�B�����ł��Ȃ������Ƃ���nullptr
//
// ---------------------------------------------------------------------------
Fluid* FluidManager::SetFluidObj( std::string& tag)
{
	Fluid* obj = nullptr;
	if (tag == "")
	{
		obj = Instantiate<Fluid>();
		tagNo++;
		tag = std::to_string(tagNo);
		obj->SetTag(tag);
	}
	else {
		obj = ObjectManager::FindGameObjectWithTag<Fluid>(tag);
		if (obj == nullptr)
		{
			obj = Instantiate<Fluid>();
			obj->SetTag(tag);
		}
	}
	if (obj == nullptr)  return nullptr;
	return obj;
}

//------------------------------------------------------------------------
// �\���p�����̂̒��_�o�b�t�@m_pCubeVBuffer���쐬�E�X�V����֐�
// 
// ���� const float& fSiz : ��ӂ̑傫��
// 
//------------------------------------------------------------------------
HRESULT FluidManager::MakeCubeVertexBuffer(const float& fSize)
{
	D3D11_BUFFER_DESC bdesc;
	D3D11_SUBRESOURCE_DATA InitData;

	// ��ӂP���̊�{������
	// �E���_�́A������O
	VECTOR3 VertexConst[] =
	{
		VECTOR3(0,0,1),
		VECTOR3(0,0,0),
		VECTOR3(0,1,1),
		VECTOR3(0,1,0),
		VECTOR3(1,0,1),
		VECTOR3(1,0,0),
		VECTOR3(1,1,1),
		VECTOR3(1,1,0),
	};

	// ���fSize���[�g���̕\���p������
	VECTOR3 VertexData[8];
	for (int i = 0; i < 8; i++)
	{
		VertexData[i] = VertexConst[i] * fSize;	// ��{�����̂�fSize�{�ɂ���
	}

	if (!m_pCubeVBuffer)
	{
		// �܂��쐬����Ă��Ȃ����߁A���_�o�b�t�@m_pCubeVBuffer��V�K�쐬����
		//bdesc.Usage = D3D11_USAGE_DEFAULT;
		bdesc.Usage = D3D11_USAGE_DYNAMIC;
		bdesc.ByteWidth = sizeof(VECTOR3) * 8;
		bdesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		//bdesc.CPUAccessFlags = 0;
		bdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bdesc.MiscFlags = 0;
		InitData.pSysMem = VertexData;
		if (FAILED(GameDevice()->m_pD3D->m_pDevice->CreateBuffer(&bdesc, &InitData, &m_pCubeVBuffer)))
			return E_FAIL;
	}
	else {
		// ���łɍ쐬�ς݂̂��߁A���_�o�b�t�@m_pCubeVBuffer�̏�������������
		D3D11_MAPPED_SUBRESOURCE msr;
		if (SUCCEEDED(GameDevice()->m_pD3D->m_pDeviceContext->Map(m_pCubeVBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr)))
		{
			memcpy(msr.pData, VertexData, sizeof(VECTOR3) * 8);
			GameDevice()->m_pD3D->m_pDeviceContext->Unmap(m_pCubeVBuffer, 0);
		}
		else {
			return E_FAIL;
		}
	}

	// �����̂̑傫����ݒ�
	m_fCubeSize = fSize;

	return S_OK;
}


//------------------------------------------------------------------------
//
//	���̂���ʂɕ`�悷�鏈��
//
// ����
//      const MATRIX4X4& mWorld                 ���[���h�}�g���b�N�X
//      const VECTOR4& vOption                  �I�v�V���� 
//      const ID3D11ShaderResourceView** ppTex3D_SRV  3D�e�N�X�`��
//      const MATRIX4X4& mView                  �r���[�}�g���b�N�X
//      const MATRIX4X4& mProj                  �v���W�F�N�V�����}�g���b�N�X
//      const VECTOR3&   vLight                 �����x�N�g��
//      const VECTOR3&   vEye                   ���_�x�N�g��(�g���Ă��Ȃ�)
//
//	�߂�l HRESULT
//		S_OK	= ����
//		E_FAIL	= �ُ�
//
//------------------------------------------------------------------------
void FluidManager::Draw(const MATRIX4X4& mWorld, const VECTOR4& vOption, ID3D11ShaderResourceView** ppTex3D_SRV, const MATRIX4X4& mView, const MATRIX4X4& mProj, const VECTOR3& vLight, const VECTOR3& vEye)
{

	// ��ʃN���A
	float ClearColor[4] = { 0,0,0,0 };//�N���A�F�쐬�@RGBA�̏�

	// �\���p�����̂̑傫�����ς���Ă��邩
	if ( m_fCubeSize != vOption.x)
	{
		MakeCubeVertexBuffer(vOption.x);   // �o�[�e�b�N�X�o�b�t�@���X�V����
		m_fCubeSize = vOption.x;
	}

	// �u�����h�X�e�[�g�����F
	// (������m_pBlendStateNormal�ɂ���ƁA�ꂪ�\�������B�A�����̂Ƃ��͂��̃��\�b�h�̍Ō��m_pBlendStateTrapen�ɖ߂�����)
	GameDevice()->m_pD3D->m_pDeviceContext->OMSetBlendState(GameDevice()->m_pD3D->m_pBlendStateTrapen, NULL, 0xffffffff);

	D3D11_MAPPED_SUBRESOURCE pData;
	CONSTANT_BUFFER_WVLED cb = {};
	UINT stride = 0;
	UINT offset = 0;

	// �T���v���[�X�e�[�g���Z�b�g����
	GameDevice()->m_pD3D->m_pDeviceContext->PSSetSamplers(0, 1, &m_pSampleLinearFluid);

	// ���݂̃����_�[�^�[�Q�b�g��ۑ����Ă���
	ID3D11RenderTargetView*   pTargetRTVSave = GameDevice()->m_pD3D->m_pTarget_TexRTV;
	ID3D11DepthStencilView*   pTargetDSVSave = GameDevice()->m_pD3D->m_pTarget_DSTexDSV;

	// �p�X�P  �e�N�X�`���[A�Ƀ����_�����O ------------------------------------------------

	// �����_�[�^�[�Q�b�g���e�N�X�`���[A�ɕύX
	GameDevice()->m_pD3D->SetRenderTarget(m_TexA.pRTV, m_TexA.pDSV);
	GameDevice()->m_pD3D->ClearRenderTarget(ClearColor);  // ��ʃN�����[

	// �t�����g�t�F�C�X�̂݃����_�����O����悤�ɃJ�����O���[�h���Z�b�g
	GameDevice()->m_pD3D->m_pDeviceContext->RSSetState(m_pFrontFace);

	// �g�p����V�F�[�_�[�̃Z�b�g	
	GameDevice()->m_pD3D->m_pDeviceContext->VSSetShader(GameDevice()->m_pShader->m_pFluidTexture_VS, NULL, 0);
	GameDevice()->m_pD3D->m_pDeviceContext->PSSetShader(GameDevice()->m_pShader->m_pFluidTexture_PS, NULL, 0);

	// �V�F�[�_�[�̃R���X�^���g�o�b�t�@�[�Ɋe��f�[�^��n��
	if (SUCCEEDED(GameDevice()->m_pD3D->m_pDeviceContext->Map(GameDevice()->m_pShader->m_pConstantBufferWVLED, 0, D3D11_MAP_WRITE_DISCARD, 0, &pData)))
	{
		// ���[���h�A�J�����A�ˉe�s���n��
		MATRIX4X4 m = XMMatrixTranspose(mWorld*mView*mProj);
		cb.mWVP = m;
		cb.mW = XMMatrixTranspose(mWorld);
		cb.vEyePos = vEye;
		cb.vDiffuse = vOption;
		cb.vDrawInfo = VECTOR4(0, 0, 0, 0);    // �����Ă��Ȃ�    // -- 2021.1.11
		memcpy_s(pData.pData, pData.RowPitch, (void*)(&cb), sizeof(cb));
		GameDevice()->m_pD3D->m_pDeviceContext->Unmap(GameDevice()->m_pShader->m_pConstantBufferWVLED, 0);
	}
	// ���݂̃V�F�[�_�[�ɂ��̃R���X�^���g�o�b�t�@�[���Z�b�g
	GameDevice()->m_pD3D->m_pDeviceContext->VSSetConstantBuffers(0, 1, &GameDevice()->m_pShader->m_pConstantBufferWVLED);
	GameDevice()->m_pD3D->m_pDeviceContext->PSSetConstantBuffers(0, 1, &GameDevice()->m_pShader->m_pConstantBufferWVLED);
	// ���_�C���v�b�g���C�A�E�g���Z�b�g
	GameDevice()->m_pD3D->m_pDeviceContext->IASetInputLayout(GameDevice()->m_pShader->m_pFluidTexture_VertexLayout);
	// �v���~�e�B�u�E�g�|���W�[���Z�b�g
	GameDevice()->m_pD3D->m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// ���_�o�b�t�@���Z�b�g
	stride = sizeof(VECTOR3);
	offset = 0;
	GameDevice()->m_pD3D->m_pDeviceContext->IASetVertexBuffers(0, 1, &m_pCubeVBuffer, &stride, &offset);
	// �C���f�b�N�X�o�b�t�@���Z�b�g
	GameDevice()->m_pD3D->m_pDeviceContext->IASetIndexBuffer(m_pCubeIBuffer, DXGI_FORMAT_R32_UINT, 0);
	// �����_�����O
	GameDevice()->m_pD3D->m_pDeviceContext->DrawIndexed(12 * 3, 0, 0);


	// �p�X�Q �e�N�X�`���[B�Ƀ����_�����O ---------------------------------------------------

	// �����_�[�^�[�Q�b�g���e�N�X�`���[B�ɕύX
	GameDevice()->m_pD3D->SetRenderTarget(m_TexB.pRTV, m_TexB.pDSV);
	GameDevice()->m_pD3D->ClearRenderTarget(ClearColor);  // ��ʃN�����[

	// �o�b�N�t�F�C�X�̂݃����_�����O����悤�ɃJ�����O���[�h���Z�b�g
	GameDevice()->m_pD3D->m_pDeviceContext->RSSetState(m_pBackFace);

	// �g�p����V�F�[�_�[�̃Z�b�g	
	GameDevice()->m_pD3D->m_pDeviceContext->VSSetShader(GameDevice()->m_pShader->m_pFluidTexture_VS, NULL, 0);
	GameDevice()->m_pD3D->m_pDeviceContext->PSSetShader(GameDevice()->m_pShader->m_pFluidTexture_PS, NULL, 0);

	// �V�F�[�_�[�̃R���X�^���g�o�b�t�@�[�Ɋe��f�[�^��n��
	if (SUCCEEDED(GameDevice()->m_pD3D->m_pDeviceContext->Map(GameDevice()->m_pShader->m_pConstantBufferWVLED, 0, D3D11_MAP_WRITE_DISCARD, 0, &pData)))
	{
		// ���[���h�A�J�����A�ˉe�s���n��
		MATRIX4X4 m = XMMatrixTranspose(mWorld*mView*mProj);
		cb.mWVP = m;
		cb.mW = XMMatrixTranspose(mWorld);
		cb.vEyePos = vEye;
		cb.vDiffuse = vOption;
		cb.vDrawInfo = VECTOR4(0, 0, 0, 0);    // �����Ă��Ȃ�    // -- 2021.1.11
		memcpy_s(pData.pData, pData.RowPitch, (void*)(&cb), sizeof(cb));
		GameDevice()->m_pD3D->m_pDeviceContext->Unmap(GameDevice()->m_pShader->m_pConstantBufferWVLED, 0);
	}
	// ���݂̃V�F�[�_�[�ɂ��̃R���X�^���g�o�b�t�@�[���Z�b�g
	GameDevice()->m_pD3D->m_pDeviceContext->VSSetConstantBuffers(0, 1, &GameDevice()->m_pShader->m_pConstantBufferWVLED);
	GameDevice()->m_pD3D->m_pDeviceContext->PSSetConstantBuffers(0, 1, &GameDevice()->m_pShader->m_pConstantBufferWVLED);
	// ���_�C���v�b�g���C�A�E�g���Z�b�g
	GameDevice()->m_pD3D->m_pDeviceContext->IASetInputLayout(GameDevice()->m_pShader->m_pFluidTexture_VertexLayout);
	// �v���~�e�B�u�E�g�|���W�[���Z�b�g
	GameDevice()->m_pD3D->m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// ���_�o�b�t�@���Z�b�g
	stride = sizeof(VECTOR3);
	offset = 0;
	GameDevice()->m_pD3D->m_pDeviceContext->IASetVertexBuffers(0, 1, &m_pCubeVBuffer, &stride, &offset);
	// �C���f�b�N�X�o�b�t�@���Z�b�g
	GameDevice()->m_pD3D->m_pDeviceContext->IASetIndexBuffer(m_pCubeIBuffer, DXGI_FORMAT_R32_UINT, 0);
	// �����_�����O
	GameDevice()->m_pD3D->m_pDeviceContext->DrawIndexed(12 * 3, 0, 0);


	// �p�X�R  ���̂̃����_�����O ----------------------------------------------

	// �����_�[�^�[�Q�b�g��ۑ����Ă������^�[�Q�b�g�i�o�b�N�o�b�t�@�[���j�ɖ߂�
	GameDevice()->m_pD3D->SetRenderTarget(pTargetRTVSave, pTargetDSVSave);

	// �t�����g�t�F�C�X�̂݃����_�����O����悤�ɃJ�����O���[�h��߂�
	GameDevice()->m_pD3D->m_pDeviceContext->RSSetState(m_pFrontFace);

	// �g�p����V�F�[�_�[�̃Z�b�g	
	GameDevice()->m_pD3D->m_pDeviceContext->VSSetShader(GameDevice()->m_pShader->m_pFluidRender_VS, NULL, 0);
	GameDevice()->m_pD3D->m_pDeviceContext->PSSetShader(GameDevice()->m_pShader->m_pFluidRender_PS, NULL, 0);

	// �p�X�P�ƃp�X�Q�ō쐬�����e�N�X�`���[2�����V�F�[�_�[�ɃZ�b�g
	GameDevice()->m_pD3D->m_pDeviceContext->PSSetShaderResources(0, 1, &m_TexA.pSRV);
	GameDevice()->m_pD3D->m_pDeviceContext->PSSetShaderResources(1, 1, &m_TexB.pSRV);

	// �R���s���[�g�V�F�[�_�[�ō쐬�����A3D�e�N�X�`���[���V�F�[�_�[�ɃZ�b�g
	GameDevice()->m_pD3D->m_pDeviceContext->PSSetShaderResources(2, 1, ppTex3D_SRV);

	// �V�F�[�_�[�̃R���X�^���g�o�b�t�@�[�Ɋe��f�[�^��n��
	if (SUCCEEDED(GameDevice()->m_pD3D->m_pDeviceContext->Map(GameDevice()->m_pShader->m_pConstantBufferWVLED, 0, D3D11_MAP_WRITE_DISCARD, 0, &pData)))
	{
		// ���[���h�A�J�����A�ˉe�s���n��
		MATRIX4X4 m = XMMatrixTranspose(mWorld*mView*mProj);
		cb.mWVP = m;
		cb.mW = XMMatrixTranspose(mWorld);
		cb.vEyePos = vEye;
		cb.vDiffuse = vOption;
		memcpy_s(pData.pData, pData.RowPitch, (void*)(&cb), sizeof(cb));
		GameDevice()->m_pD3D->m_pDeviceContext->Unmap(GameDevice()->m_pShader->m_pConstantBufferWVLED, 0);
	}
	// ���݂̃V�F�[�_�[�ɂ��̃R���X�^���g�o�b�t�@�[���Z�b�g
	GameDevice()->m_pD3D->m_pDeviceContext->VSSetConstantBuffers(0, 1, &GameDevice()->m_pShader->m_pConstantBufferWVLED);
	GameDevice()->m_pD3D->m_pDeviceContext->PSSetConstantBuffers(0, 1, &GameDevice()->m_pShader->m_pConstantBufferWVLED);
	// ���_�C���v�b�g���C�A�E�g���Z�b�g
	GameDevice()->m_pD3D->m_pDeviceContext->IASetInputLayout(GameDevice()->m_pShader->m_pFluidRender_VertexLayout);
	// �v���~�e�B�u�E�g�|���W�[���Z�b�g
	GameDevice()->m_pD3D->m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// ���_�o�b�t�@���Z�b�g
	stride = sizeof(VECTOR3);
	offset = 0;
	GameDevice()->m_pD3D->m_pDeviceContext->IASetVertexBuffers(0, 1, &m_pCubeVBuffer, &stride, &offset);
	// �C���f�b�N�X�o�b�t�@���Z�b�g
	GameDevice()->m_pD3D->m_pDeviceContext->IASetIndexBuffer(m_pCubeIBuffer, DXGI_FORMAT_R32_UINT, 0);

	// �����_�����O
	GameDevice()->m_pD3D->m_pDeviceContext->DrawIndexed(12 * 3, 0, 0);

	// �����F�̃u�����h�ɖ߂�
	//GameDevice()->m_pD3D->m_pDeviceContext->OMSetBlendState(GameDevice()->m_pD3D->m_pBlendStateTrapen, NULL, 0xffffffff);

}


