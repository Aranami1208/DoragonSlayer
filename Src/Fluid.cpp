
#include "FluidManager.h"
#include "Fluid.h"


namespace {
	const int   KeepTime = 30;
};
// ---------------------------------------------------------------------------
//
//  ���̃I�u�W�F�N�g�̃R���X�g���N�^
//
// ---------------------------------------------------------------------------
Fluid::Fluid()
{
	Init();		//  ���̃I�u�W�F�N�g�̏�����

	m_bAddFlag = false;						// �����t���O
	m_vAddVoxelPos = VECTOR4(0, 0, 0, 0);	// �����{�N�Z���ʒu
	m_vAddDensity = VECTOR4(0, 0, 0, 0);	// �������x
	m_vAddVelocity = VECTOR4(0, 0, 0, 0);	// �������x
	m_vOption = VECTOR4(0, 0, 0, 0);		// �I�v�V����(x:�\�������̂̑傫���@y:�F�R�[�h�@z:�Ȃ��@w:���������_����)
	m_nAddVoxelWidth = 0;					// �����ӏ����甭��������Ƃ��̃T�C�Y�i�{�N�Z�����j

	m_vOption.x = 1;		// �����l�Ƃ��āA�\�������̂̃T�C�Y��1m�Ƃ���

	m_updateTimer = 0;
	m_keepTimer = 0;

	ObjectManager::SetDrawOrder(this, -200);   // �����F�͒x�߂ɕ`�悷��
}

// ---------------------------------------------------------------------------
//
//  ���̃I�u�W�F�N�g�̃f�X�g���N�^
//
// ---------------------------------------------------------------------------
Fluid::~Fluid()
{
	for (int i = 0; i<2; i++) m_Dns[i].Release();		// ���x
	for (int i = 0; i<2; i++) m_Vlc[i].Release();		// ���x
	m_Vlc_s.Release();		// ���x�̃Z�~�O�����W�A��
	m_Vlc_bs.Release();		// ���x�̋t�����̃Z�~�O�����W�A��
	m_Prs.Release();		// ����
	m_Div.Release();		// ���U
	m_Dns_Tex.Release();	// �`��
}

// ---------------------------------------------------------------------------
//
//  ���̃I�u�W�F�N�g�̏�����
//
// ---------------------------------------------------------------------------
void Fluid::Init()
{
	D3D11_UNORDERED_ACCESS_VIEW_DESC udesc;
	D3D11_BUFFER_DESC bdesc;

	// 3D�e�N�X�`���[�̐���
	CreateEmpty3DTexture(FLUID_SIZE + 2, &m_Dns_Tex.pTex3D, &m_Dns_Tex.pSRV);
	for (int i = 0; i<2; i++) CreateEmpty3DTexture(FLUID_SIZE + 2, &m_Dns[i].pTex3D, &m_Dns[i].pSRV);
	for (int i = 0; i<2; i++) CreateEmpty3DTexture(FLUID_SIZE + 2, &m_Vlc[i].pTex3D, &m_Vlc[i].pSRV);
	CreateEmpty3DTexture(FLUID_SIZE + 2, &m_Vlc_s.pTex3D, &m_Vlc_s.pSRV);
	CreateEmpty3DTexture(FLUID_SIZE + 2, &m_Vlc_bs.pTex3D, &m_Vlc_bs.pSRV);

	// �X�g���N�`���[�h�o�b�t�@
	ZeroMemory(&bdesc, sizeof(bdesc));
	bdesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	bdesc.ByteWidth = sizeof(SBUFFER_ELEMENT)*(FLUID_SIZE + 2)*(FLUID_SIZE + 2)*(FLUID_SIZE + 2);
	bdesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	bdesc.StructureByteStride = sizeof(SBUFFER_ELEMENT);
	GameDevice()->m_pD3D->m_pDevice->CreateBuffer(&bdesc, NULL, &m_Prs.pStredBuf);
	GameDevice()->m_pD3D->m_pDevice->CreateBuffer(&bdesc, NULL, &m_Div.pStredBuf);

	// �X�g���N�`���[�h�o�b�t�@��UAV
	ZeroMemory(&udesc, sizeof(udesc));
	udesc.Format = DXGI_FORMAT_UNKNOWN;
	udesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	udesc.Texture2D.MipSlice = 0;
	udesc.Buffer.NumElements = (FLUID_SIZE + 2)*(FLUID_SIZE + 2)*(FLUID_SIZE + 2);
	GameDevice()->m_pD3D->m_pDevice->CreateUnorderedAccessView(m_Prs.pStredBuf, &udesc, &m_Prs.pUAV);
	GameDevice()->m_pD3D->m_pDevice->CreateUnorderedAccessView(m_Div.pStredBuf, &udesc, &m_Div.pUAV);

	// 3D�e�N�X�`���[��UAV
	ZeroMemory(&udesc, sizeof(udesc));
	udesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	udesc.Texture3D.FirstWSlice = 0;
	udesc.Texture3D.WSize = FLUID_SIZE;
	udesc.Texture3D.MipSlice = 0;
	udesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE3D;

	GameDevice()->m_pD3D->m_pDevice->CreateUnorderedAccessView(m_Dns_Tex.pTex3D, &udesc, &m_Dns_Tex.pUAV);
	for (int i = 0; i<2; i++) GameDevice()->m_pD3D->m_pDevice->CreateUnorderedAccessView(m_Dns[i].pTex3D, &udesc, &m_Dns[i].pUAV);
	for (int i = 0; i<2; i++) GameDevice()->m_pD3D->m_pDevice->CreateUnorderedAccessView(m_Vlc[i].pTex3D, &udesc, &m_Vlc[i].pUAV);
	GameDevice()->m_pD3D->m_pDevice->CreateUnorderedAccessView(m_Vlc_s.pTex3D, &udesc, &m_Vlc_s.pUAV);
	GameDevice()->m_pD3D->m_pDevice->CreateUnorderedAccessView(m_Vlc_bs.pTex3D, &udesc, &m_Vlc_bs.pUAV);

}
// ---------------------------------------------------------------------------
//
//  3D�e�N�X�`���[�̐����֐�
//
//  int Size                         : ��ӂ̃T�C�Y
//  ID3D11Texture3D** ppTexture      : �쐬���ꂽ3D�e�N�X�`���[(OUT)
//  ID3D11ShaderResourceView** ppSRV : �쐬���ꂽ�V�F�[�_�[���\�[�X�r���[(OUT)
//
// ---------------------------------------------------------------------------
HRESULT Fluid::CreateEmpty3DTexture(int Size, ID3D11Texture3D** ppTexture, ID3D11ShaderResourceView** ppSRV)
{
	D3D11_TEXTURE3D_DESC descTex;
	ZeroMemory(&descTex, sizeof(descTex));
	descTex.Height = Size;
	descTex.Width = Size;
	descTex.Depth = Size;
	descTex.MipLevels = 1;
	descTex.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	descTex.Usage = D3D11_USAGE_DEFAULT;
	descTex.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;

	GameDevice()->m_pD3D->m_pDevice->CreateTexture3D(&descTex, NULL, ppTexture);
	GameDevice()->m_pD3D->m_pDevice->CreateShaderResourceView(*ppTexture, NULL, ppSRV);

	return S_OK;
}

// ---------------------------------------------------------------------------
//
// ���̂̔����ϐ���ݒ肷��
//
//   const Transform& trans  : ���[���h�}�g���b�N�X
//   const float& fCubeSize   :	�\���p�����̂̈�ӂ̑傫��
//   const float& fColorNo    :	���̂̐F�R�[�h�i0:���i�����Ȃ��j1: ���i��������j2:���� 3:�Z���� 4:�������j
//   const VECTOR4& vVoxelPos :	�ݒ�{�N�Z���ʒu
//   const VECTOR4& vDensity  :	���x(���x��x�̂ݎg�p����Byzw�͎g�p���Ă��Ȃ�)
//   const VECTOR4& vVelocity :	���x
//   const float&   fRandom   :	���x���������_����(0.0f:������ �` 1.0f:�����������_��)
//   const int& nAddVoxelWidth:	�����ӏ����甭��������Ƃ��̃T�C�Y�i�{�N�Z�����j�B�P�J���̏ꍇ�͂O�ɂ���B
// ---------------------------------------------------------------------------
void Fluid::Spawn(const Transform& trans, const float& fCubeSize, const float& fColorNo, const VECTOR4& vVoxelPos, const VECTOR4& vDensity, const VECTOR4& vVelocity, const float& fRandom, const int& nAddVoxelWidth)
{
	if (m_bAddFlag)	return;					   // �_�u���Ē����͂��Ȃ�

	m_bAddFlag     = true;                     // ���x�Ƒ��x���������ꂽ
	transform      = trans;
	m_vOption.x    = fCubeSize;
	m_vOption.y    = fColorNo;
	m_vAddVoxelPos = vVoxelPos;
	m_vAddDensity  = vDensity  * FLUID_TIME_STEP;
	m_vAddVelocity = vVelocity * FLUID_TIME_STEP;
	m_vOption.w    = fRandom;
	m_nAddVoxelWidth = nAddVoxelWidth;
}
// ---------------------------------------------------------------------------
//
// �R���s���[�g�V�F�[�_��UAV��SRV������������
//
// ---------------------------------------------------------------------------
void Fluid::Unbind()
{
	ID3D11UnorderedAccessView* ppUAV_NULL[1]={NULL};
	ID3D11ShaderResourceView* ppSRV_NULL[1]={NULL};
	for(int i=0; i<8; i++)
	{
		GameDevice()->m_pD3D->m_pDeviceContext->CSSetUnorderedAccessViews(i,1,ppUAV_NULL,0);
		GameDevice()->m_pD3D->m_pDeviceContext->CSSetShaderResources(i,1,ppSRV_NULL);
	}
}

// ---------------------------------------------------------------------------
//
//  ���̂𔭐�������(���x�Ƒ��x����ɒ�������)
//
//  �@�@�R���X�^���g�o�b�t�@�ɖ��x�A���x��ݒ肷��
//  �A�@���̂̔������������s����
//
//  VECTOR3 vOffset  : �ʒu�̃I�t�Z�b�g
//
// ---------------------------------------------------------------------------
void Fluid::AddSource(VECTOR3 vOffset)
{
	// �R���X�^���g�o�b�t�@�ɖ��x�A���x��ݒ肷��
	D3D11_MAPPED_SUBRESOURCE pData;
	CONSTANT_BUFFER_FLUID cb = {};
	GameDevice()->m_pD3D->m_pDeviceContext->Map(GameDevice()->m_pShader->m_pConstantBufferFluid, 0, D3D11_MAP_WRITE_DISCARD, 0, &pData);
	cb.vAddVoxelPos = m_vAddVoxelPos + vOffset;
	cb.vAddDensity = m_vAddDensity;
	cb.vAddVelocity = m_vAddVelocity;
	cb.vOption = m_vOption;       // �I�v�V����(x:�\�������̂̑傫���@y:�F�R�[�h�@z:�Ȃ��@w:���������_����)
	memcpy_s(pData.pData, pData.RowPitch, (void*)(&cb), sizeof(cb));
	GameDevice()->m_pD3D->m_pDeviceContext->Unmap(GameDevice()->m_pShader->m_pConstantBufferFluid, 0);

	GameDevice()->m_pD3D->m_pDeviceContext->CSSetShaderResources(0, 1, &m_Dns[1].pSRV);
	GameDevice()->m_pD3D->m_pDeviceContext->CSSetUnorderedAccessViews(0, 1, &m_Dns[0].pUAV, 0);
	GameDevice()->m_pD3D->m_pDeviceContext->CSSetShaderResources(1, 1, &m_Vlc[0].pSRV);
	GameDevice()->m_pD3D->m_pDeviceContext->CSSetUnorderedAccessViews(1, 1, &m_Vlc[1].pUAV, 0);

	GameDevice()->m_pD3D->m_pDeviceContext->CSSetShader(GameDevice()->m_pShader->m_pFluidSolver_AddSource, NULL, 0);  // AddSource���Ăяo��
	GameDevice()->m_pD3D->m_pDeviceContext->CSSetConstantBuffers(0, 1, &GameDevice()->m_pShader->m_pConstantBufferFluid);
	GameDevice()->m_pD3D->m_pDeviceContext->Dispatch(1, 1, 1);
	Unbind();
}

// ---------------------------------------------------------------------------
//
//  ���̃I�u�W�F�N�g�̍X�V�֐�(�\���o�[)
//
//  (�\���o�[�Ƃ͗��̌v�Z���f�������������߂̃v���O�����̂��Ƃł���)
//
// ---------------------------------------------------------------------------
void Fluid::Update()
{
	m_updateTimer += 60 * SceneManager::DeltaTime();
	if (m_updateTimer < 0.99f) return;				   // �덷�Ή��̂���1�ł͂Ȃ�0.99�Ƃ���
	m_updateTimer = 0;

	// ���x�A���x�𒍓�  ----------------------------------------
	if (m_bAddFlag)  // ���x�A���x���������ꂽ�Ƃ�
	{
		VECTOR3 vOffset;

		m_keepTimer = KeepTime;	 // �ێ����Ԃ�ݒ�

		//  ���̂𔭐�������(���x�Ƒ��x����ɒ�������)
		AddSource();

		// ����������ꏊ�������ӏ��̂Ƃ��A���S�_�ȊO��4�J�����甭��������
		if (m_nAddVoxelWidth > 0)
		{
			// �x�����������y����������
			if (m_vAddVoxelPos.y >= FLUID_SIZE / 2 - 1)
			{
				vOffset = VECTOR3(1, 1, 0);  // Z��������
			}
			else {
				vOffset = VECTOR3(1, 0, 1);  // Y��������
			}
			for (int i = 0; i < 4; i++)  // 4�J�����甭��
			{
				//  ���̂𔭐�������
				AddSource( VECTOR3( vOffset.x * (i%2*2-1) * m_nAddVoxelWidth, 
									vOffset.y * (i/2*2-1) * m_nAddVoxelWidth,
									vOffset.z * (i/2*2-1) * m_nAddVoxelWidth));
			}
		}
	}

	// ���x�\���o�[  ----------------------------------------------
	
	// SD_RESOURCE�����ւ���
	FLUID_SWAP_SDRESOURCE(m_Vlc[0],m_Vlc[1]);

	// Project1
	GameDevice()->m_pD3D->m_pDeviceContext->CSSetUnorderedAccessViews(4, 1, &m_Div.pUAV, 0);
	GameDevice()->m_pD3D->m_pDeviceContext->CSSetUnorderedAccessViews(3, 1, &m_Prs.pUAV, 0);
	GameDevice()->m_pD3D->m_pDeviceContext->CSSetShaderResources(1,1,&m_Vlc[0].pSRV);
	GameDevice()->m_pD3D->m_pDeviceContext->CSSetShader(GameDevice()->m_pShader->m_pFluidSolver_Project1,NULL,0);
	GameDevice()->m_pD3D->m_pDeviceContext->Dispatch(FLUID_RSIZE, FLUID_RSIZE, FLUID_RSIZE);
	Unbind();

	// project2
	GameDevice()->m_pD3D->m_pDeviceContext->CSSetUnorderedAccessViews(3, 1, &m_Prs.pUAV, 0);
	GameDevice()->m_pD3D->m_pDeviceContext->CSSetUnorderedAccessViews(4, 1, &m_Div.pUAV, 0);
	GameDevice()->m_pD3D->m_pDeviceContext->CSSetShader(GameDevice()->m_pShader->m_pFluidSolver_Project2, NULL, 0);
	for (int i = 0; i < 5;i++)  GameDevice()->m_pD3D->m_pDeviceContext->Dispatch(FLUID_RSIZE, FLUID_RSIZE, FLUID_RSIZE);
	Unbind();

	// project3
	GameDevice()->m_pD3D->m_pDeviceContext->CSSetShaderResources(1,1,&m_Vlc[0].pSRV);
	GameDevice()->m_pD3D->m_pDeviceContext->CSSetUnorderedAccessViews(1,1,&m_Vlc[1].pUAV,0);
	GameDevice()->m_pD3D->m_pDeviceContext->CSSetUnorderedAccessViews(3, 1, &m_Prs.pUAV, 0);
	GameDevice()->m_pD3D->m_pDeviceContext->CSSetShader(GameDevice()->m_pShader->m_pFluidSolver_Project3, NULL, 0);
	GameDevice()->m_pD3D->m_pDeviceContext->Dispatch(FLUID_RSIZE, FLUID_RSIZE, FLUID_RSIZE);
	Unbind();

	// SD_RESOURCE�����ւ���
	FLUID_SWAP_SDRESOURCE(m_Vlc[0],m_Vlc[1]);

	// boundary
	GameDevice()->m_pD3D->m_pDeviceContext->CSSetShaderResources(1,1,&m_Vlc[0].pSRV);
	GameDevice()->m_pD3D->m_pDeviceContext->CSSetUnorderedAccessViews(1,1,&m_Vlc[1].pUAV,0);
	GameDevice()->m_pD3D->m_pDeviceContext->CSSetShader(GameDevice()->m_pShader->m_pFluidSolver_Boundary1, NULL, 0);
	GameDevice()->m_pD3D->m_pDeviceContext->Dispatch(FLUID_RSIZE, FLUID_RSIZE, 1);
	Unbind();

	// AdvectVelocity
	GameDevice()->m_pD3D->m_pDeviceContext->CSSetShaderResources(1,1,&m_Vlc[0].pSRV);
	GameDevice()->m_pD3D->m_pDeviceContext->CSSetUnorderedAccessViews(2,1,&m_Vlc_s.pUAV,0);  // MacCormack���g�p����Ƃ�
	//GameDevice()->m_pD3D->m_pDeviceContext->CSSetUnorderedAccessViews(1,1,&m_Vlc[1].pUAV,0);  // MacCormack���g�p���Ȃ��Ƃ�(FluidSolver.hlsli���ύX���邱��)
	GameDevice()->m_pD3D->m_pDeviceContext->CSSetShader(GameDevice()->m_pShader->m_pFluidSolver_AdvectVelocity, NULL, 0);
	GameDevice()->m_pD3D->m_pDeviceContext->Dispatch(FLUID_RSIZE, FLUID_RSIZE, FLUID_RSIZE);
	Unbind();

	
	// Advect Back(MacCormack���g�p����Ƃ�)
	GameDevice()->m_pD3D->m_pDeviceContext->CSSetShaderResources(1,1, &m_Vlc[0].pSRV);
	GameDevice()->m_pD3D->m_pDeviceContext->CSSetShaderResources(2,1, &m_Vlc_s.pSRV);
	GameDevice()->m_pD3D->m_pDeviceContext->CSSetUnorderedAccessViews(3, 1, &m_Vlc_bs.pUAV, 0);
	GameDevice()->m_pD3D->m_pDeviceContext->CSSetShader(GameDevice()->m_pShader->m_pFluidSolver_AdvectBack, NULL, 0);
	GameDevice()->m_pD3D->m_pDeviceContext->Dispatch(FLUID_RSIZE, FLUID_RSIZE, FLUID_RSIZE);
	Unbind();

	// MacCormack(MacCormack���g�p����Ƃ�)
	GameDevice()->m_pD3D->m_pDeviceContext->CSSetShaderResources(0,1, &m_Dns[0].pSRV);
	GameDevice()->m_pD3D->m_pDeviceContext->CSSetShaderResources(1,1, &m_Vlc[0].pSRV);
	GameDevice()->m_pD3D->m_pDeviceContext->CSSetUnorderedAccessViews(1, 1, &m_Vlc[1].pUAV, 0);
	GameDevice()->m_pD3D->m_pDeviceContext->CSSetShaderResources(2,1, &m_Vlc_s.pSRV);
	GameDevice()->m_pD3D->m_pDeviceContext->CSSetShaderResources(3,1, &m_Vlc_bs.pSRV);
	GameDevice()->m_pD3D->m_pDeviceContext->CSSetShader(GameDevice()->m_pShader->m_pFluidSolver_MacCormack, NULL, 0);
	GameDevice()->m_pD3D->m_pDeviceContext->Dispatch(FLUID_RSIZE, FLUID_RSIZE, FLUID_RSIZE);
	Unbind();
	

	// ���x�\���o�[  ------------------------------------------
	// AdvectDensity
	GameDevice()->m_pD3D->m_pDeviceContext->CSSetShaderResources(1,1,&m_Vlc[0].pSRV);
	GameDevice()->m_pD3D->m_pDeviceContext->CSSetUnorderedAccessViews(5, 1, &m_Dns_Tex.pUAV, 0);	
	GameDevice()->m_pD3D->m_pDeviceContext->CSSetShaderResources(0,1,&m_Dns[0].pSRV);
	GameDevice()->m_pD3D->m_pDeviceContext->CSSetUnorderedAccessViews(0,1,&m_Dns[1].pUAV,0);

	// SD_RESOURCE�����ւ���
	FLUID_SWAP_SDRESOURCE(m_Dns[0],m_Dns[1]);

	GameDevice()->m_pD3D->m_pDeviceContext->CSSetShader(GameDevice()->m_pShader->m_pFluidSolver_AdvectDensity, NULL, 0);
	GameDevice()->m_pD3D->m_pDeviceContext->Dispatch(FLUID_RSIZE, FLUID_RSIZE, FLUID_RSIZE);
	Unbind();


	// �V�F�[�_�[���Z�b�g���� ---------------------------------
	GameDevice()->m_pD3D->m_pDeviceContext->CSSetShader(NULL,NULL,0);

	m_keepTimer--;
	if (m_bAddFlag == false && m_keepTimer <= 0)
	{

		DestroyMe();  // �����t���O���Z�b�g����Ȃ��ĕێ����Ԃ��߂����Ƃ��́A�폜����

		// �����ɁA���\�[�X�̃N�����[����������

	}
	m_bAddFlag = false;  // �����t���O�����Z�b�g����

}

//------------------------------------------------------------------------
//
//	���̂���ʂɃ����_�����O���鏈��
//
// ����
//      const MATRIX4X4& mWorld               ���[���h�}�g���b�N�X
//      const VECTOR4X4& vOption              �I�v�V����(x�����o�ɕ\���p�����̂̃T�C�Y�������Ă���)
//      const MATRIX4X4& mProj                �v���W�F�N�V�����}�g���b�N�X
//      const VECTOR3&   vLight               �����x�N�g��
//      const VECTOR3&   vEye                 ���_�x�N�g��(�g���Ă��Ȃ�)
//
//	�߂�l HRESULT
//		S_OK	= ����
//		E_FAIL	= �ُ�
//
//------------------------------------------------------------------------
void Fluid::DrawDepth()
{
}
void Fluid::DrawScreen()
{
	// �\���ʒu�̒���
	// ���̂̔����ꏊ�����_mWorldStart�Ƃ���
	VECTOR4   vOffset     = m_vAddVoxelPos / FLUID_SIZE * m_vOption.x;
	MATRIX4X4 mWorldStart = XMMatrixTranslation( -vOffset.x, -vOffset.y, -vOffset.z) * transform.matrix();

	//	���̂���ʂɕ`�悷�鏈��
	ObjectManager::FindGameObject<FluidManager>()->Draw(mWorldStart, m_vOption, &m_Dns_Tex.pSRV, GameDevice()->m_mView, GameDevice()->m_mProj, GameDevice()->m_vLightDir, GameDevice()->m_vEyePt);
}

