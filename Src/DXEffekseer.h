#pragma once

#include "MyMath.h"
#include "Object3D.h"
#include "DXEffekseerBase.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "xaudio2.lib")

#ifndef _DEBUG
#pragma comment(lib, "Effekseer.lib")
#else
#pragma comment(lib, "Effekseerd.lib")
#endif
#include <Effekseer.h>

#ifndef _DEBUG
#pragma comment(lib, "EffekseerRendererDX11.lib")
#else
#pragma comment(lib, "EffekseerRendererDX11d.lib")
#endif
#include <EffekseerRendererDX11.h>

class CDirect3D;

/// <summary>
/// DirectX11�p��Effekseer���C�u����
///             author : Kaito Suzuki(student)
///             editor : Terutaka Suzuki
/// </summary>
class DXEffekseer : public Object3D
{
public:
	DXEffekseer();
	virtual ~DXEffekseer();

	void Update() override;

	void ManagerUpdate(Effekseer::Matrix44 inverseViewMatrix, Effekseer::Matrix44 projectionMatrix);

	void BeginEffekseerDraw();
	void EndEffekseerDraw();

	void DrawEffect(const Effekseer::Handle &);

	// �A�N�Z�X�֐�
	Effekseer::ManagerRef GetEffekseerManager() { return effekseerManager; };

private:
	//�`��
	EffekseerRendererDX11::RendererRef effekseerRenderer;
	//�}�l�[�W���[
	Effekseer::ManagerRef effekseerManager;
	//�`��f�o�C�X
	Effekseer::Backend::GraphicsDeviceRef graphicsDevice;

	Effekseer::Manager::LayerParameter layerParameter;		//SetLayerParameter�֐��փ��C���[���Ƃɐݒ肷��p�����[�^�[

	Effekseer::Manager::UpdateParameter updateParameter;	//�}�l�[�W���[���X�V�����Ƃ��̃p�����[�^�[(�o�߃t���[���A�X�V�񐔁A�񓯊������Ɋւ��鐧�䂪�\)

	Effekseer::Manager::DrawParameter drawParameter;		//DrawHandle�Ɏg�p����p�����[�^�[

	int timer;
};