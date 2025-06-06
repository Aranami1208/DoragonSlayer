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
/// DirectX11用のEffekseerライブラリ
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

	// アクセス関数
	Effekseer::ManagerRef GetEffekseerManager() { return effekseerManager; };

private:
	//描画
	EffekseerRendererDX11::RendererRef effekseerRenderer;
	//マネージャー
	Effekseer::ManagerRef effekseerManager;
	//描画デバイス
	Effekseer::Backend::GraphicsDeviceRef graphicsDevice;

	Effekseer::Manager::LayerParameter layerParameter;		//SetLayerParameter関数へレイヤーごとに設定するパラメーター

	Effekseer::Manager::UpdateParameter updateParameter;	//マネージャーが更新されるときのパラメーター(経過フレーム、更新回数、非同期処理に関する制御が可能)

	Effekseer::Manager::DrawParameter drawParameter;		//DrawHandleに使用するパラメーター

	int timer;
};