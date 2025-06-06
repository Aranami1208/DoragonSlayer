#include "EffectManager.h"

namespace {
	const int EFFECT_NUM_MAX = 100;    // MAX :�P�̃I�u�W�F�N�g�̒��̔������B

}

EffectParticle::EffectParticle()
{
	ObjectManager::SetDrawOrder(this, -200);   // �����F�̂��ߒx�߂ɕ`�悷��

	partB = ObjectManager::FindGameObject<EffectManager>()->ParticleList("");

	maxParticle = EFFECT_NUM_MAX;
	ptArray = new PART[EFFECT_NUM_MAX];     // ��̃I�u�W�F�N�g�̒��̃p�[�e�B�N���z��
	frame = 0;
	normal = VECTOR3(0, 0, 0);
}
EffectParticle::~EffectParticle()
{
	SAFE_DELETE_ARRAY(ptArray);
}

void EffectParticle::SetEffectName(std::string name)
{
	partB = ObjectManager::FindGameObject<EffectManager>()->ParticleList(name);
}

void EffectParticle::Start()
{

	maxParticle = partB->m_nNum;  // �p�[�e�B�N���\���̂̃p�[�e�B�N������ݒ肷��
	if (maxParticle > EFFECT_NUM_MAX) maxParticle = EFFECT_NUM_MAX; // �������v�f�����I�[�o�[���Ă�����v�f���ɒ�������
	frame = 0;

	// �ړ������B�@�������𒆐S�Ƃ��ă����_�������ɔ���������
	VECTOR3 vDist, vMin, vMax;
	vDist.x = (1.0f - fabsf(normal.x)) / 2;
	vDist.y = (1.0f - fabsf(normal.y)) / 2;
	vDist.z = (1.0f - fabsf(normal.z)) / 2;

	if (normal.x < 0)
	{
		vMin.x = normal.x - vDist.x;
		vMax.x = 0.0f + vDist.x;
	}
	else {
		vMin.x = 0.0f - vDist.x;
		vMax.x = normal.x + vDist.x;
	}
	if (normal.y < 0)
	{
		vMin.y = normal.y - vDist.y;
		vMax.y = 0.0f + vDist.y;
	}
	else {
		vMin.y = 0.0f - vDist.y;
		vMax.y = normal.y + vDist.y;
	}
	if (normal.z < 0)
	{
		vMin.z = normal.z - vDist.z;
		vMax.z = 0.0f + vDist.z;
	}
	else {
		vMin.z = 0.0f - vDist.z;
		vMax.z = normal.z + vDist.z;
	}

	// �p�[�e�B�N���̊J�n
	for (int i = 0; i < maxParticle; i++)
	{
		ptArray[i].Pos = transform.position;
		ptArray[i].Dir.x = ((float)rand() / (float)RAND_MAX) * (vMax.x - vMin.x) + vMin.x;
		ptArray[i].Dir.y = ((float)rand() / (float)RAND_MAX) * (vMax.y - vMin.y) + vMin.y;
		ptArray[i].Dir.z = ((float)rand() / (float)RAND_MAX) * (vMax.z - vMin.z) + vMin.z;
		ptArray[i].Dir = normalize(ptArray[i].Dir);
		ptArray[i].Speed = (5 + ((float)rand() / (float)RAND_MAX)) * partB->m_fSpeed;      // �ړ��X�s�[�h�B�����_��
		ptArray[i].BirthFrame = (int)(((float)rand() / RAND_MAX) * partB->m_iBarthFrame);  // �J�n�܂ł̑҂����ԁB�����_��
	}
}
void EffectParticle::Update()
{
	frame += 60 * SceneManager::DeltaTime();

	for (int i = 0; i < maxParticle; i++)
	{
		if (frame > ptArray[i].BirthFrame) // ��̃p�[�e�B�N�����\���J�n���ԂɂȂ����Ƃ�
		{
			// �ړ�
			ptArray[i].Pos = ptArray[i].Pos + ptArray[i].Dir * ptArray[i].Speed * 60 * SceneManager::DeltaTime();

			// �d�͂̌v�Z
			ptArray[i].Dir = ptArray[i].Dir + VECTOR3(0, -0.049f * 60 * SceneManager::DeltaTime(), 0);

			// �n�ʂł̃o�E���h
			if (partB->m_ifBound == 1)
			{
				if (ptArray[i].Pos.y < 0)
				{
					ptArray[i].Dir.y = -ptArray[i].Dir.y;
					ptArray[i].Speed /= (1.8f * 60 * SceneManager::DeltaTime());
				}
			}
		}
	}

	if (frame >= partB->m_FrameEnd) // �\�����ԏI����
	{
		DestroyMe();
	}

}

void EffectParticle::DrawDepth()
{
	// �e�����Ȃ�
}

void EffectParticle::DrawScreen()
{
	//�g�p����V�F�[�_�[�̃Z�b�g
	GameDevice()->m_pD3D->m_pDeviceContext->VSSetShader(GameDevice()->m_pShader->m_pEffect3D_VS_POINT, NULL, 0);
	GameDevice()->m_pD3D->m_pDeviceContext->GSSetShader(GameDevice()->m_pShader->m_pEffect3D_GS_POINT, NULL, 0);
	GameDevice()->m_pD3D->m_pDeviceContext->PSSetShader(GameDevice()->m_pShader->m_pEffect3D_PS, NULL, 0);

	//�o�[�e�b�N�X�o�b�t�@�[���Z�b�g
	UINT stride = sizeof(PARTICLE_VERTEX);
	UINT offset = 0;
	GameDevice()->m_pD3D->m_pDeviceContext->IASetVertexBuffers(0, 1, &partB->m_pVertexBuffer, &stride, &offset);

	// ���Z�����F�̃u�����f�B���O��ݒ�
	UINT mask = 0xffffffff;
	if (partB->m_nBlendFlag == 1)   // ���Z�����F�w��
	{
		GameDevice()->m_pD3D->m_pDeviceContext->OMSetBlendState(GameDevice()->m_pD3D->m_pBlendStateAdd, NULL, mask);
	}

	// �p�[�e�B�N���P�����P���|�C���g�X�v���C�g�Ƃ���m_MaxParticle���`��
	for (int i = 0; i < maxParticle; i++)
	{
		// �`��J�n�O�̂��̂ƕ`��t���[���A�E�g�̂��͕̂`�悵�Ȃ�
		if (partB->m_FrameEnd < frame + (partB->m_iBarthFrame - ptArray[i].BirthFrame)) continue;

		//�X�̃p�[�e�B�N���́A���_���������[���h�g�����X�t�H�[�������߂�
		MATRIX4X4 mWorld = GetLookatMatrix(ptArray[i].Pos, GameDevice()->m_vEyePt);

		renderParticle(mWorld, GameDevice()->m_mView, GameDevice()->m_mProj);  // �p�[�e�B�N���������_�����O
	}

	if (partB->m_nBlendFlag == 1)   // ���Z�����F�w��
	{
		// �ʏ�̃u�����f�B���O�ɖ߂�
		GameDevice()->m_pD3D->m_pDeviceContext->OMSetBlendState(GameDevice()->m_pD3D->m_pBlendStateTrapen, NULL, mask);
	}

	//�W�I���g���V�F�[�_�[�̃��Z�b�g
	GameDevice()->m_pD3D->m_pDeviceContext->GSSetShader(NULL, NULL, 0);

	return;

}

void EffectParticle::renderParticle(MATRIX4X4 W, MATRIX4X4 V, MATRIX4X4 P)
{
	//�V�F�[�_�[�̃R���X�^���g�o�b�t�@�[�Ɋe��f�[�^��n��
	D3D11_MAPPED_SUBRESOURCE pData;
	CONSTANT_BUFFER_EFFECT cb;
	ZeroMemory(&cb, sizeof(cb));
	if (SUCCEEDED(GameDevice()->m_pD3D->m_pDeviceContext->Map(GameDevice()->m_pShader->m_pConstantBufferEffect, 0, D3D11_MAP_WRITE_DISCARD, 0, &pData)))
	{
		//���[���h�A�J�����A�ˉe�s���n�� // -- 2017.8.25
		cb.mW = XMMatrixTranspose(W);
		cb.mV = XMMatrixTranspose(V);
		cb.mP = XMMatrixTranspose(P);

		cb.fAlpha = partB->m_fAlpha;            // �p�[�e�B�N���̓����x
		cb.fSize = partB->m_fDestSize;          // �p�[�e�B�N���̃T�C�Y 
		memcpy_s(pData.pData, pData.RowPitch, (void*)(&cb), sizeof(cb));
		GameDevice()->m_pD3D->m_pDeviceContext->Unmap(GameDevice()->m_pShader->m_pConstantBufferEffect, 0);
	}

	//���̃R���X�^���g�o�b�t�@�[���ǂ̃V�F�[�_�[�Ŏg����
	GameDevice()->m_pD3D->m_pDeviceContext->VSSetConstantBuffers(0, 1, &GameDevice()->m_pShader->m_pConstantBufferEffect);
	GameDevice()->m_pD3D->m_pDeviceContext->GSSetConstantBuffers(0, 1, &GameDevice()->m_pShader->m_pConstantBufferEffect);
	GameDevice()->m_pD3D->m_pDeviceContext->PSSetConstantBuffers(0, 1, &GameDevice()->m_pShader->m_pConstantBufferEffect);
	//���_�C���v�b�g���C�A�E�g���Z�b�g
	GameDevice()->m_pD3D->m_pDeviceContext->IASetInputLayout(GameDevice()->m_pShader->m_pEffect3D_VertexLayout);
	//�v���~�e�B�u�E�g�|���W�[���Z�b�g
	GameDevice()->m_pD3D->m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	//�e�N�X�`���[���V�F�[�_�[�ɓn��
	GameDevice()->m_pD3D->m_pDeviceContext->PSSetSamplers(0, 1, &GameDevice()->m_pD3D->m_pSampleLinear);
	GameDevice()->m_pD3D->m_pDeviceContext->PSSetShaderResources(0, 1, &partB->m_pTexture);
	//�v���~�e�B�u�������_�����O
	GameDevice()->m_pD3D->m_pDeviceContext->Draw(1, 0);

}
