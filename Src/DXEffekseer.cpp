#include "DXEffekseer.h"
#include "Direct3D.h"
#include "Main.h"
#include <algorithm>

namespace {
	constexpr float Z_NEAR = 0.1f;		//���_�����ԋ߂��A�`�悳��鋗��
	constexpr float Z_FAR = 1000.0f;	//���_�����ԉ����A�`�悳��鋗��
	const int p_fps = 60;
};

DXEffekseer::DXEffekseer()
{
	ObjectManager::DontDestroy(this);		// ������Ȃ�
	ObjectManager::SetPriority(this, -10001);	// �J�������x�߂ɍX�V��������
	ObjectManager::SetVisible(this, false);		// ���͕̂\�����Ȃ�

	//�G�t�F�N�g�}�l�[�W���[�̍쐬(�G�t�F�N�g�̍ő�C���X�^���X��)
	effekseerManager = Effekseer::Manager::Create(8000);

	//�`��f�o�C�X�̍쐬
	graphicsDevice = EffekseerRendererDX11::CreateGraphicsDevice(GameDevice()->m_pD3D->m_pDevice, GameDevice()->m_pD3D->m_pDeviceContext);
	//�G�t�F�N�g�����_���̍쐬
	effekseerRenderer = EffekseerRendererDX11::Renderer::Create(graphicsDevice, 8000);

	//�`�惂�W���[���̐ݒ�
	effekseerManager->SetSpriteRenderer(effekseerRenderer->CreateSpriteRenderer());
	effekseerManager->SetRibbonRenderer(effekseerRenderer->CreateRibbonRenderer());
	effekseerManager->SetRingRenderer(effekseerRenderer->CreateRingRenderer());
	effekseerManager->SetTrackRenderer(effekseerRenderer->CreateTrackRenderer());
	effekseerManager->SetModelRenderer(effekseerRenderer->CreateModelRenderer());

	//�e�N�X�`���A���f���A�J�[�u�A�}�e���A�����[�_�[�̐ݒ�
	effekseerManager->SetTextureLoader(effekseerRenderer->CreateTextureLoader());
	effekseerManager->SetModelLoader(effekseerRenderer->CreateModelLoader());
	effekseerManager->SetMaterialLoader(effekseerRenderer->CreateMaterialLoader());
	effekseerManager->SetCurveLoader(Effekseer::MakeRefPtr<Effekseer::CurveLoader>());

	//���W�n�̐ݒ�
	effekseerManager->SetCoordinateSystem(Effekseer::CoordinateSystem::LH);

	//�`�拗���̐ݒ�
	drawParameter.ZNear = Z_NEAR;
	drawParameter.ZFar = Z_FAR;

	//�^�C�}�[�̏�����
	timer = 0;
}

DXEffekseer::~DXEffekseer()
{
	
}

/// <summary>
/// �J�����ʒu��}�l�[�W���[�̍X�V������
/// �����ł͕ϐ���Effekseer�d�l�ɕύX���ē�����Update�ɓn���Ă���
/// </summary>
/// <param name="mView">�r���[�ϊ��s��</param>
/// <param name="mProj">�ˉe�ϊ��s��</param>
void DXEffekseer::Update()
{
	MATRIX4X4 mView = GameDevice()->m_mView;
	MATRIX4X4 mProj = GameDevice()->m_mProj;

	//�r���[�ϊ��s���ݒ�
	Effekseer::Matrix44 viewMatrix;

	for (unsigned int i = 0; i < 4; i++)
	{
		for (unsigned int j = 0; j < 4; j++)
		{
			viewMatrix.Values[i][j] = mView.m[i][j];
		}
	}
	//Effekseer�����ł́A�r���[�ϊ��s��͋t�s��ŊǗ�����Ă���
	//mView�͌��X�t�s��̂��߁A���̂܂܂ŗǂ�

	//�ˉe�ϊ��s���ݒ�
	Effekseer::Matrix44 projectionMatrix;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			projectionMatrix.Values[i][j] = mProj.m[i][j];
		}
	}

	ManagerUpdate(viewMatrix, projectionMatrix);
}

/// <summary>
/// �J�����ʒu��}�l�[�W���[�̍X�V
/// </summary>
/// <param name="viewMatrix">�r���[�ϊ��s��</param>
/// <param name="projectionMatrix">�ˉe�ϊ��s��</param>
void DXEffekseer::ManagerUpdate(Effekseer::Matrix44 viewMatrix, Effekseer::Matrix44 projectionMatrix)
{
	//�J�����ʒu�̐ݒ�
	Effekseer::Matrix44 inverseViewMatrix;
	Effekseer::Matrix44::Inverse(inverseViewMatrix, viewMatrix);
	layerParameter.ViewerPosition = Effekseer::Vector3D(inverseViewMatrix.Values[3][0], inverseViewMatrix.Values[3][1], inverseViewMatrix.Values[3][2]);
	effekseerManager->SetLayerParameter(0, layerParameter);

	//�}�l�[�W���[�̍X�V
	effekseerManager->Update(updateParameter);
	//���Ԃ��X�V����
	effekseerRenderer->SetTime((float)timer / p_fps);

	//�ˉe�ϊ��s���ݒ�
	effekseerRenderer->SetProjectionMatrix(projectionMatrix);
	//�r���[�ϊ��s���ݒ�
	effekseerRenderer->SetCameraMatrix(viewMatrix);

	//�G�t�F�N�g�̕`��J�n
	effekseerRenderer->BeginRendering();

	//�G�t�F�N�g�̕`��
	drawParameter.ViewProjectionMatrix = effekseerRenderer->GetCameraProjectionMatrix();
	effekseerManager->Draw(drawParameter);

	//�G�t�F�N�g�̕`��I��
	effekseerRenderer->EndRendering();

	//�o�ߎ���
	timer++;
}

void DXEffekseer::BeginEffekseerDraw()
{
	//�G�t�F�N�g�̕`��J�n
	effekseerRenderer->BeginRendering();
	//ViewProjectionMatrix�̐ݒ�
	drawParameter.ViewProjectionMatrix = effekseerRenderer->GetCameraProjectionMatrix();
}

void DXEffekseer::EndEffekseerDraw()
{
	//�G�t�F�N�g�̕`��I��
	effekseerRenderer->EndRendering();
}

//�G�t�F�N�g�̕`��
void DXEffekseer::DrawEffect(const Effekseer::Handle &handle)
{
	effekseerManager->DrawHandle(handle, drawParameter);
}
