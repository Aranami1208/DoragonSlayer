#pragma once
#include "GameMain.h"
#include "DXEffekseerBase.h"
#include "DXEffekseerEffect.h"
#include "TextReader.h"

/// <summary>
/// DirectX11�p��Effekseer���C�u����  �}�l�[�W���[�N���X
///             author : Terutaka Suzuki
/// </summary>
class DXEffekseerEffect;
class DXEffekseerManager :public DXEffekseerBase
{
private:
	struct effectstruct {
		std::string name;
		float radius;
		Effekseer::EffectRef effectR;	//�G�t�F�N�g�̓ǂݍ��ݏ��
		effectstruct()
		{
			radius = 0;
		}
	};
public:
	DXEffekseerManager();
	~DXEffekseerManager();

	Effekseer::EffectRef EffectList(std::string str, float& radius);

	/// <summary>
	/// �G�t�F�N�g�̐����ƍĐ�
	/// </summary>
	/// <param name="name">�G�t�F�N�g��</param>
	/// <param name="posIn">�ʒu</param>
	/// <param name="isLoopIn">���[�v�Đ����邩�B�K��l��false</param>
	/// <returns>�G�t�F�N�g�I�u�W�F�N�g</returns>
	DXEffekseerEffect* Spawn(std::string name, const VECTOR3& posIn, const bool& isLoopIn = false);

	/// <summary>
	/// �G�t�F�N�g�̐����ƍĐ�
	/// </summary>
	/// <param name="name">�G�t�F�N�g��</param>
	/// <param name="posIn">�ʒu</param>
	/// <param name="rotIn">�p�x</param>
	/// <param name="isLoopIn">���[�v�Đ����邩�B�K��l��false</param>
	/// <returns>�G�t�F�N�g�I�u�W�F�N�g</returns>
	DXEffekseerEffect* Spawn(std::string name, const VECTOR3& posIn, const VECTOR3& rotIn, const bool& isLoopIn = false);

	/// <summary>
	/// �G�t�F�N�g�̐����ƍĐ�
	/// �i�@���̊p�x�Ŕ�������j
	/// </summary>
	/// <param name="name">�G�t�F�N�g��</param>
	/// <param name="posIn">�ʒu</param>
	/// <param name="normalIn">�@��</param>
	/// <param name="isLoopIn">���[�v�Đ����邩�B�K��l��false</param>
	/// <returns>�G�t�F�N�g�I�u�W�F�N�g</returns>
	DXEffekseerEffect* SpawnFromNormal(std::string name, const VECTOR3& posIn, const VECTOR3& normalIn, const bool& isLoopIn = false);

	/// <summary>
	/// �G�t�F�N�g�̐����ƍĐ�
	/// (�}�b�v�e�L�X�g���琶��)
	/// </summary>
	/// <param name="txt">�}�b�v�e�L�X�g</param>
	/// <param name="n">�s��</param>
	void MakeEffekseer(TextReader* txt, int n);

private:
	std::list<effectstruct> effectList;	  	//�G�t�F�N�g���X�g
};

