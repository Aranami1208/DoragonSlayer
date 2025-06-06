#pragma once
#include "GameMain.h"
#include "DXEffekseer.h"
#include "DXEffekseerManager.h"

class DXEffekseer;
class DXEffekseerManager;

/// <summary>
/// DirectX11�p��Effekseer���C�u����  �G�t�F�N�g�N���X
///             author : Kaito Suzuki(student)
///             editor : Terutaka Suzuki
/// </summary>
class DXEffekseerEffect :public DXEffekseerBase
{
public:

	void SetEffect(std::string name);

	/// <summary>
	/// �G�t�F�N�g�̐����ƍĐ�
	/// </summary>
	/// <param name="name">�G�t�F�N�g��</param>
	/// <param name="posIn">�ʒu</param>
	/// <param name="isLoopIn">���[�v�Đ����邩�B�K��l��false</param>
	void Spawn(std::string name, const VECTOR3&, const bool& isLoopIn = false);

	/// <summary>
	/// �G�t�F�N�g�̐����ƍĐ�
	/// </summary>
	/// <param name="name">�G�t�F�N�g��</param>
	/// <param name="posIn">�ʒu</param>
	/// <param name="rotIn">�p�x</param>
	/// <param name="isLoopIn">���[�v�Đ����邩�B�K��l��false</param>
	void Spawn(std::string name, const VECTOR3&, const VECTOR3&, const bool& isLoopIn = false);

	/// <summary>
	/// �G�t�F�N�g�̐����ƍĐ�
	/// �i�@���̊p�x�Ŕ�������j
	/// </summary>
	/// <param name="name">�G�t�F�N�g��</param>
	/// <param name="posIn">�ʒu</param>
	/// <param name="normalIn">�@��</param>
	/// <param name="isLoopIn">���[�v�Đ����邩�B�K��l��false</param>
	void SpawnFromNormal(std::string name, const VECTOR3&, const VECTOR3&, const bool& isLoopIn = false);

	void Play( const bool &);
	void Stop();
	void SetDelayTime(const unsigned int&);
	void Update() override;
	void DrawScreen() override;
	VECTOR3 GetNormalRotateVector(const VECTOR3&);

	DXEffekseerEffect();
	~DXEffekseerEffect();
private:
	DXEffekseer* effek;
	DXEffekseerManager* effManager;
};

