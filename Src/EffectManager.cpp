#include "EffectManager.h"

EffectManager::EffectManager()
{
	ObjectManager::DontDestroy(this);		// ���̂͏�����Ȃ�
	ObjectManager::SetVisible(this, false);		// ���͕̂\�����Ȃ�

	// �����r���{�[�h�X�^�e�B�b�N���b�V���̓ǂݍ���
	mesh = new CFbxMesh();
	mesh->Load("Data/Item/BillSphere.mesh");
	meshCol = nullptr;

	// ------------------------------------------------------------------------------------------
	// �r���{�[�h�\���̃��X�g�̐ݒ�
	BILLBOARDBASE bb;

	// ���r���{�[�h "sparklen3" (�擪�v�f:�ȗ��l)
	billboardList.push_back(bb);
	billboardList.back().m_name = "sparklen3";
	LoadBillTexture("Data/Image/sparklen3.png", &billboardList.back());
	billboardList.back().m_fDestWidth = 0.7f;      // �\����
	billboardList.back().m_fDestHeight = 0.7f;     // �\������
	billboardList.back().m_fDestCenterX = billboardList.back().m_fDestWidth / 2;  // �\�����S�ʒu�w(�^��)
	billboardList.back().m_fDestCenterY = billboardList.back().m_fDestHeight / 2; // �\�����S�ʒu�x(�^��)
	billboardList.back().m_dwSrcX = 0;             // �p�^�[���̈ʒu�@�w���W
	billboardList.back().m_dwSrcY = 0;             // �p�^�[���̈ʒu�@�x���W
	billboardList.back().m_dwSrcWidth = 85;        // �p�^�[���̕�
	billboardList.back().m_dwSrcHeight = 85;       // �p�^�[���̍���
	billboardList.back().m_dwNumX = 3;             // �A�j���[�V����������p�^�[���̐��@�w����
	billboardList.back().m_dwNumY = 1;             // �A�j���[�V����������p�^�[���̐��@�x����
	billboardList.back().m_fAlpha = 0.9f;          // �����x
	billboardList.back().m_nBlendFlag = 1;         // �u�����h�X�e�[�g�t���O(0:�ʏ�`��@1:���Z�����F�`��)
	billboardList.back().m_nDrawFlag = 0;          // �`��t���O(0:�r���{�[�h�̂�)
	SetBillSrc(&billboardList.back());                 // �o�[�e�b�N�X�o�b�t�@�̍쐬

	// �����r���{�[�h	"Bom3"
	billboardList.push_back(bb);
	billboardList.back().m_name = "Bom3";
	LoadBillTexture("Data/Image/Bom3.png", &billboardList.back());
	billboardList.back().m_fDestWidth = 4.0f;      // �\����
	billboardList.back().m_fDestHeight = 4.0f;     // �\������
	billboardList.back().m_fDestCenterX = billboardList.back().m_fDestWidth / 2;  // �\�����S�ʒu�w(�^��)
	billboardList.back().m_fDestCenterY = billboardList.back().m_fDestHeight / 2; // �\�����S�ʒu�x(�^��)
	billboardList.back().m_dwSrcX = 0;             // �p�^�[���̈ʒu�@�w���W
	billboardList.back().m_dwSrcY = 0;             // �p�^�[���̈ʒu�@�x���W
	billboardList.back().m_dwSrcWidth = 64;        // �p�^�[���̕�
	billboardList.back().m_dwSrcHeight = 64;       // �p�^�[���̍���
	billboardList.back().m_dwNumX = 4;             // �A�j���[�V����������p�^�[���̐��@�w����
	billboardList.back().m_dwNumY = 4;             // �A�j���[�V����������p�^�[���̐��@�x����
	billboardList.back().m_fAlpha = 0.9f;          // �����x
	billboardList.back().m_nBlendFlag = 1;         // �u�����h�X�e�[�g�t���O(0:�ʏ�`��@1:���Z�����F�`��)
	billboardList.back().m_nDrawFlag = 1;          // �`��t���O(0:�r���{�[�h�@1:�r���{�[�h���b�V��)
	SetBillSrc(&billboardList.back());                 // �o�[�e�b�N�X�o�b�t�@�̍쐬

	// �����r���{�[�h2	  "Bom4"
	billboardList.push_back(bb);
	billboardList.back().m_name = "Bom4";
	LoadBillTexture("Data/Image/Bom4.png", &billboardList.back());
	billboardList.back().m_fDestWidth = 4.0f;      // �\����
	billboardList.back().m_fDestHeight = 4.0f;     // �\������
	billboardList.back().m_fDestCenterX = billboardList.back().m_fDestWidth / 2;  // �\�����S�ʒu�w(�^��)
	billboardList.back().m_fDestCenterY = billboardList.back().m_fDestHeight;   // �\�����S�ʒu�x(���̒[)
	billboardList.back().m_dwSrcX = 0;             // �p�^�[���̈ʒu�@�w���W
	billboardList.back().m_dwSrcY = 0;             // �p�^�[���̈ʒu�@�x���W
	billboardList.back().m_dwSrcWidth = 128;       // �p�^�[���̕�
	billboardList.back().m_dwSrcHeight = 128;      // �p�^�[���̍���
	billboardList.back().m_dwNumX = 4;             // �A�j���[�V����������p�^�[���̐��@�w����
	billboardList.back().m_dwNumY = 4;             // �A�j���[�V����������p�^�[���̐��@�x����
	billboardList.back().m_fAlpha = 0.9f;          // �����x
	billboardList.back().m_nBlendFlag = 0;         // �u�����h�X�e�[�g�t���O(0:�ʏ�`��@1:���Z�����F�`��)
	billboardList.back().m_nDrawFlag = 1;          // �`��t���O(0:�r���{�[�h�@1:�r���{�[�h���b�V��)
	SetBillSrc(&billboardList.back());                 // �o�[�e�b�N�X�o�b�t�@�̍쐬

	// �����r���{�[�h3	  "Effect01"
	billboardList.push_back(bb);
	billboardList.back().m_name = "Effect01";
	LoadBillTexture("Data/Image/Effect01.png", &billboardList.back());
	billboardList.back().m_fDestWidth = 4.0f;      // �\����
	billboardList.back().m_fDestHeight = 4.0f;     // �\������
	billboardList.back().m_fDestCenterX = billboardList.back().m_fDestWidth / 2;  // �\�����S�ʒu�w(�^��)
	billboardList.back().m_fDestCenterY = billboardList.back().m_fDestHeight;   // �\�����S�ʒu�x(���̒[)
	billboardList.back().m_dwSrcX = 0;             // �p�^�[���̈ʒu�@�w���W
	billboardList.back().m_dwSrcY = 0;             // �p�^�[���̈ʒu�@�x���W
	billboardList.back().m_dwSrcWidth = 128;       // �p�^�[���̕�
	billboardList.back().m_dwSrcHeight = 128;      // �p�^�[���̍���
	billboardList.back().m_dwNumX = 4;             // �A�j���[�V����������p�^�[���̐��@�w����
	billboardList.back().m_dwNumY = 4;             // �A�j���[�V����������p�^�[���̐��@�x����
	billboardList.back().m_fAlpha = 0.9f;          // �����x
	billboardList.back().m_nBlendFlag = 0;         // �u�����h�X�e�[�g�t���O(0:�ʏ�`��@1:���Z�����F�`��)
	billboardList.back().m_nDrawFlag = 1;          // �`��t���O(0:�r���{�[�h�@1:�r���{�[�h���b�V��)
	SetBillSrc(&billboardList.back());                 // �o�[�e�b�N�X�o�b�t�@�̍쐬

	// ------------------------------------------------------------------------------------------
	// �p�[�e�B�N���\���̂̐ݒ�
	PARTICLEBASE pb;

	// �Ήԃp�[�e�B�N�� "particle3"(�擪�v�f:�ȗ��l)
	particleList.push_back(pb);
	particleList.back().m_name = "particle3";
	LoadPartTexture(_T("Data/Image/particle3.png"), &particleList.back());// �p�[�e�B�N���e�N�X�`��
	particleList.back().m_nNum = 100;             // ��̃I�u�W�F�N�g���̃p�[�e�B�N�����BPARTICLE_NUM_MAX�ȉ��ł��邱�ƁB
	particleList.back().m_fDestSize = 0.1f;       // �\���T�C�Y(��̃p�[�e�B�N���̑傫��)
	particleList.back().m_FrameEnd = 60;          // �p�[�e�B�N����\�����Ă��鎞��
	particleList.back().m_fSpeed = 0.015f;        // �p�[�e�B�N���̈ړ��X�s�[�h�B�����_��
	particleList.back().m_iBarthFrame = 20;       // �p�[�e�B�N���̊J�n�܂ł̍ő�҂����ԁB�����_���ŊJ�n�B�O�͑҂�����
	particleList.back().m_ifBound = 0;            // �n�ʂŃo�E���h�����邩�i0:�o�E���h�Ȃ� 1:�n�ʂŃo�E���h�j
	particleList.back().m_fAlpha = 0.9f;          // �����x
	particleList.back().m_nBlendFlag = 1;         // �u�����h�X�e�[�g�t���O(0:�ʏ�`��@1:���Z�����F�`��)
	SetPartSrc(&particleList.back());             // �o�[�e�b�N�X�o�b�t�@�̍쐬

	// �Ήԃp�[�e�B�N���E��  "particle2"
	particleList.push_back(pb);
	particleList.back().m_name = "particle2";
	LoadPartTexture(_T("Data/Image/particle2.png"), &particleList.back());// �p�[�e�B�N���e�N�X�`��
	particleList.back().m_nNum = 10;              // ��̃I�u�W�F�N�g���̃p�[�e�B�N�����BPARTICLE_NUM_MAX�ȉ��ł��邱�ƁB
	particleList.back().m_fDestSize = 0.5f;       // �\���T�C�Y(��̃p�[�e�B�N���̑傫��)
	particleList.back().m_FrameEnd = 60;          // �p�[�e�B�N����\�����Ă��鎞��
	particleList.back().m_fSpeed = 0.015f;        // �p�[�e�B�N���̈ړ��X�s�[�h�B�����_��
	particleList.back().m_iBarthFrame = 0;        // �p�[�e�B�N���̊J�n�܂ł̍ő�҂����ԁB�����_���ŊJ�n�B�O�͑҂�����
	particleList.back().m_ifBound = 0;            // �n�ʂŃo�E���h�����邩�i0:�o�E���h�Ȃ� 1:�n�ʂŃo�E���h�j
	particleList.back().m_fAlpha = 0.9f;          // �����x
	particleList.back().m_nBlendFlag = 1;         // �u�����h�X�e�[�g�t���O(0:�ʏ�`��@1:���Z�����F�`��)
	SetPartSrc(&particleList.back());             // �o�[�e�b�N�X�o�b�t�@�̍쐬

}

EffectManager::~EffectManager()
{
	SAFE_DELETE(mesh);
	for (BILLBOARDBASE& bb : billboardList)
	{
		SAFE_RELEASE(bb.m_pTexture);
		SAFE_RELEASE(bb.m_pVertexBuffer);
	}
	for (PARTICLEBASE& pb : particleList)
	{
		SAFE_RELEASE(pb.m_pTexture);
		SAFE_RELEASE(pb.m_pVertexBuffer);
	}
}


EffectBase::BILLBOARDBASE* EffectManager::BillboardList(std::string str)
{
	if (str == "")
	{
		return &billboardList.front();	 // �擪�v�f
	}
	for (BILLBOARDBASE& bb : billboardList)
	{
		if (str == bb.m_name) return &bb;
	}
	std::string msg = "EffectManager::BillboardList()   " + str;
	MessageBox(nullptr, msg.c_str(), _T("������ �w��̃G�t�F�N�g���̃G�t�F�N�g�̓r���{�[�h���X�g�ɂ���܂��� ������"), MB_OK);
	return nullptr;
}

EffectBase::PARTICLEBASE* EffectManager::ParticleList(std::string str)
{
	if (str == "")
	{
		return &particleList.front();	 // �擪�v�f
	}
	for (PARTICLEBASE& pb : particleList)
	{
		if (str == pb.m_name) return &pb;
	}
	std::string msg = "EffectManager::ParticleList()   " + str;
	MessageBox(nullptr, msg.c_str(), _T("������ �w��̃G�t�F�N�g���̃G�t�F�N�g�̓p�[�e�B�N�����X�g�ɂ���܂��� ������"), MB_OK);
	return nullptr;
}
