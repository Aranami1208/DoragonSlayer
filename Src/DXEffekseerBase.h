#include "Effekseer.h"
#include "Object3D.h"
#include <string>

#pragma once

/// <summary>
/// DirectX11�p��Effekseer���C�u����  ���N���X
///             author : Kaito Suzuki(student)
///             editor : Terutaka Suzuki
/// </summary>
class DXEffekseerBase : public Object3D
{
public:
	const bool Active() const { return isActive; };
	const Effekseer::Handle Handle() const { return EffectHandle; };

	virtual void Death() final;
	virtual void DrawDepth() override;		// �e�̕`��
	virtual void DrawScreen() override;		// �I�u�W�F�N�g�̕`��

	virtual void LoadEffect(const TCHAR *);
	virtual Effekseer::EffectRef LoadEffectEffectR(const TCHAR *);

	void SetEffectLocation(const VECTOR3 &argPos);
	void SetEffectRotation(const float angle);
	void SetEffectRotation(const VECTOR3 &angle);
	void SetEffectScale(const VECTOR3 &scale);
	void SetActive(const bool &activeBool) { isActive = activeBool; };
	void SetDontDestroy(const bool& dontDestroyBool) { dontDestroy = dontDestroyBool; }

	float Radius() { return radius; }

	//�R���X�g���N�^
	DXEffekseerBase();
	virtual ~DXEffekseerBase();

protected:
	bool isActive;					// ���̃G�t�F�N�g�̓A�N�e�B�u�H
	bool isLoop;					// ���[�v����G�t�F�N�g�H
	bool isDelaySpawned;			// �x���Spawn������I�u�W�F�N�g�H
	bool dontDestroy;				// �I����A���̃I�u�W�F�N�g���폜���Ȃ����B���Ȃ��Ƃ�true
	unsigned int Timer;				// Spawn���Ă���o�߂�������
	unsigned int DeathTime;			// Spawn�`Death�܂ł̎���
	unsigned int DelaySpawnTime;	// Spawn����܂ł̎���(�x�点�邱�Ƃ��ł���)
	Effekseer::EffectRef EffectR;	// �G�t�F�N�g�̓ǂݍ��ݏ��
	Effekseer::Handle EffectHandle;	// �G�t�F�N�g�̃C���X�^���X�n���h��
	float radius;					// �����蔻��̔��a
};