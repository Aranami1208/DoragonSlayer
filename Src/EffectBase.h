#pragma once
#include "Object3D.h"
#include "Animator.h"

class EffectBase : public Object3D
{
public:
	// ========================================================================================
	//
	// ���_�̍\����
	//
	// ========================================================================================
	struct PARTICLE_VERTEX
	{
		VECTOR3 Pos; //�ʒu
	};
	struct BILLBOARD_VERTEX
	{
		VECTOR3 Pos; //�ʒu
		VECTOR2 UV;  //�e�N�X�`���[���W
	};

	// ========================================================================================
	//
	// ��̃I�u�W�F�N�g�̒��̃p�[�e�B�N���\����
	//
	// ========================================================================================
	struct PART
	{
		VECTOR3     Pos;
		VECTOR3     Dir;
		float       Speed;
		int         BirthFrame;
		PART()
		{
			ZeroMemory(this, sizeof(PART));
		}
	};

	// ========================================================================================
	//
	//	�p�[�e�B�N���z��p�\����
	// 
	// ========================================================================================
	struct PARTICLEBASE
	{
		std::string                 m_name;			   // �ʖ�
		ID3D11ShaderResourceView*	m_pTexture;        // �e�N�X�`���[(�P�̃e�N�X�`���[�ɂ�1�̃p�[�e�B�N���̂�)
		DWORD						m_dwImageWidth;    // �e�N�X�`���[�̕�
		DWORD						m_dwImageHeight;   // �e�N�X�`���[�̍���
		ID3D11Buffer*				m_pVertexBuffer;   // �o�[�e�b�N�X�o�b�t�@
		int							m_nNum;            // ��̃I�u�W�F�N�g���̃p�[�e�B�N����
		float						m_fDestSize;       // �\���T�C�Y(���ƍ����͓���)
		int							m_FrameEnd;        // �p�[�e�B�N����\�����Ă��鎞��
		float						m_fSpeed;          // �p�[�e�B�N���̈ړ��X�s�[�h
		int							m_iBarthFrame;     // �p�[�e�B�N���̊J�n�܂ł̍ő�҂����ԁB�O�͑҂�����
		int							m_ifBound;         // �n�ʂŃo�E���h�����邩�i0:�o�E���h�Ȃ� 1:�n�ʂŃo�E���h�j
		float						m_fAlpha;          // �����x
		int							m_nBlendFlag;      // 0:�ʏ� 1:���Ȕ���
		PARTICLEBASE()
		{
			m_pTexture = nullptr;        // �e�N�X�`���[(�P�̃e�N�X�`���[�ɂ�1�̃p�[�e�B�N���̂�)
			m_dwImageWidth = 0;    // �e�N�X�`���[�̕�
			m_dwImageHeight = 0;   // �e�N�X�`���[�̍���
			m_pVertexBuffer = nullptr;   // �o�[�e�b�N�X�o�b�t�@
			m_nNum = 0;            // ��̃I�u�W�F�N�g���̃p�[�e�B�N����
			m_fDestSize = 0.0f;    // �\���T�C�Y(���ƍ����͓���)
			m_FrameEnd = 0;        // �p�[�e�B�N����\�����Ă��鎞��
			m_fSpeed = 0.0f;       // �p�[�e�B�N���̈ړ��X�s�[�h
			m_iBarthFrame = 0;     // �p�[�e�B�N���̊J�n�܂ł̍ő�҂����ԁB�O�͑҂�����
			m_ifBound = 0;         // �n�ʂŃo�E���h�����邩�i0:�o�E���h�Ȃ� 1:�n�ʂŃo�E���h�j
			m_fAlpha = 1.0f;       // �����x
			m_nBlendFlag = 0;      // 0:�ʏ� 1:���Ȕ���
		}
	};

	// ========================================================================================
	//
	//	�r���{�[�h�\����
	// 
	// ========================================================================================
	struct BILLBOARDBASE
	{
		std::string                 m_name;			   // �ʖ�
		ID3D11ShaderResourceView*	m_pTexture;        // �e�N�X�`���[
		DWORD						m_dwImageWidth;    // �e�N�X�`���[�̕�
		DWORD						m_dwImageHeight;   // �e�N�X�`���[�̍���
		ID3D11Buffer*				m_pVertexBuffer;   // �o�[�e�b�N�X�o�b�t�@
		float						m_fDestWidth;      // �\����
		float						m_fDestHeight;     // �\������
		float						m_fDestCenterX;    // �\�����S�ʒu�w
		float						m_fDestCenterY;    // �\�����S�ʒu�x
		DWORD						m_dwSrcX;          // �p�^�[���̈ʒu�w
		DWORD						m_dwSrcY;          // �p�^�[���̈ʒu�x
		DWORD						m_dwSrcWidth;      // �P�̃p�^�[���̕�
		DWORD						m_dwSrcHeight;     // �P�̃p�^�[���̍���
		DWORD						m_dwNumX;          // �p�^�[���̉��̐�
		DWORD						m_dwNumY;          // �p�^�[���̏c�̐�
		float						m_fAlpha;          // �����x
		int							m_nBlendFlag;      // 0:�ʏ�  1:���Ȕ���
		int							m_nDrawFlag;       // 0:�r���{�[�h  1:�r���{�[�h���b�V��
		BILLBOARDBASE()
		{
			m_pTexture = nullptr;        // �e�N�X�`���[
			m_dwImageWidth = 0;    // �e�N�X�`���[�̕�
			m_dwImageHeight = 0;   // �e�N�X�`���[�̍���
			m_pVertexBuffer = nullptr;   // �o�[�e�b�N�X�o�b�t�@
			m_fDestWidth = 0.0f;   // �\����
			m_fDestHeight = 0.0f;  // �\������
			m_fDestCenterX = 0.0f; // �\�����S�ʒu�w
			m_fDestCenterY = 0.0f; // �\�����S�ʒu�x
			m_dwSrcX = 0;          // �p�^�[���̈ʒu�w
			m_dwSrcY = 0;          // �p�^�[���̈ʒu�x
			m_dwSrcWidth = 0;      // �P�̃p�^�[���̕�
			m_dwSrcHeight = 0;     // �P�̃p�^�[���̍���
			m_dwNumX = 0;          // �p�^�[���̉��̐�
			m_dwNumY = 0;          // �p�^�[���̏c�̐�
			m_fAlpha = 1.0f;       // �����x
			m_nBlendFlag = 0;      // 0:�ʏ�  1:���Ȕ���
			m_nDrawFlag = 0;       // 0:�r���{�[�h  1:�r���{�[�h���b�V��
		}
	};

public:
	EffectBase();
	virtual ~EffectBase();

	void SetNormal(VECTOR3 normalIn) { normal = normalIn; }

	virtual bool LoadBillTexture(const TCHAR* szFName, BILLBOARDBASE* pBillBase);
	virtual bool SetBillSrc(BILLBOARDBASE* pBillBase);
	virtual bool LoadPartTexture(const TCHAR* szFName, PARTICLEBASE* pPartBase);
	virtual bool SetPartSrc(PARTICLEBASE* pPartBase);

protected:
	VECTOR3 normal;
};