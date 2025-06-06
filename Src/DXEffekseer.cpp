#include "DXEffekseer.h"
#include "Direct3D.h"
#include "Main.h"
#include <algorithm>

namespace {
	constexpr float Z_NEAR = 0.1f;		//視点から一番近い、描画される距離
	constexpr float Z_FAR = 1000.0f;	//視点から一番遠い、描画される距離
	const int p_fps = 60;
};

DXEffekseer::DXEffekseer()
{
	ObjectManager::DontDestroy(this);		// 消されない
	ObjectManager::SetPriority(this, -10001);	// カメラより遅めに更新処理する
	ObjectManager::SetVisible(this, false);		// 自体は表示しない

	//エフェクトマネージャーの作成(エフェクトの最大インスタンス数)
	effekseerManager = Effekseer::Manager::Create(8000);

	//描画デバイスの作成
	graphicsDevice = EffekseerRendererDX11::CreateGraphicsDevice(GameDevice()->m_pD3D->m_pDevice, GameDevice()->m_pD3D->m_pDeviceContext);
	//エフェクトレンダラの作成
	effekseerRenderer = EffekseerRendererDX11::Renderer::Create(graphicsDevice, 8000);

	//描画モジュールの設定
	effekseerManager->SetSpriteRenderer(effekseerRenderer->CreateSpriteRenderer());
	effekseerManager->SetRibbonRenderer(effekseerRenderer->CreateRibbonRenderer());
	effekseerManager->SetRingRenderer(effekseerRenderer->CreateRingRenderer());
	effekseerManager->SetTrackRenderer(effekseerRenderer->CreateTrackRenderer());
	effekseerManager->SetModelRenderer(effekseerRenderer->CreateModelRenderer());

	//テクスチャ、モデル、カーブ、マテリアルローダーの設定
	effekseerManager->SetTextureLoader(effekseerRenderer->CreateTextureLoader());
	effekseerManager->SetModelLoader(effekseerRenderer->CreateModelLoader());
	effekseerManager->SetMaterialLoader(effekseerRenderer->CreateMaterialLoader());
	effekseerManager->SetCurveLoader(Effekseer::MakeRefPtr<Effekseer::CurveLoader>());

	//座標系の設定
	effekseerManager->SetCoordinateSystem(Effekseer::CoordinateSystem::LH);

	//描画距離の設定
	drawParameter.ZNear = Z_NEAR;
	drawParameter.ZFar = Z_FAR;

	//タイマーの初期化
	timer = 0;
}

DXEffekseer::~DXEffekseer()
{
	
}

/// <summary>
/// カメラ位置やマネージャーの更新をする
/// ここでは変数をEffekseer仕様に変更して内部のUpdateに渡している
/// </summary>
/// <param name="mView">ビュー変換行列</param>
/// <param name="mProj">射影変換行列</param>
void DXEffekseer::Update()
{
	MATRIX4X4 mView = GameDevice()->m_mView;
	MATRIX4X4 mProj = GameDevice()->m_mProj;

	//ビュー変換行列を設定
	Effekseer::Matrix44 viewMatrix;

	for (unsigned int i = 0; i < 4; i++)
	{
		for (unsigned int j = 0; j < 4; j++)
		{
			viewMatrix.Values[i][j] = mView.m[i][j];
		}
	}
	//Effekseer内部では、ビュー変換行列は逆行列で管理されている
	//mViewは元々逆行列のため、このままで良い

	//射影変換行列を設定
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
/// カメラ位置やマネージャーの更新
/// </summary>
/// <param name="viewMatrix">ビュー変換行列</param>
/// <param name="projectionMatrix">射影変換行列</param>
void DXEffekseer::ManagerUpdate(Effekseer::Matrix44 viewMatrix, Effekseer::Matrix44 projectionMatrix)
{
	//カメラ位置の設定
	Effekseer::Matrix44 inverseViewMatrix;
	Effekseer::Matrix44::Inverse(inverseViewMatrix, viewMatrix);
	layerParameter.ViewerPosition = Effekseer::Vector3D(inverseViewMatrix.Values[3][0], inverseViewMatrix.Values[3][1], inverseViewMatrix.Values[3][2]);
	effekseerManager->SetLayerParameter(0, layerParameter);

	//マネージャーの更新
	effekseerManager->Update(updateParameter);
	//時間を更新する
	effekseerRenderer->SetTime((float)timer / p_fps);

	//射影変換行列を設定
	effekseerRenderer->SetProjectionMatrix(projectionMatrix);
	//ビュー変換行列を設定
	effekseerRenderer->SetCameraMatrix(viewMatrix);

	//エフェクトの描画開始
	effekseerRenderer->BeginRendering();

	//エフェクトの描画
	drawParameter.ViewProjectionMatrix = effekseerRenderer->GetCameraProjectionMatrix();
	effekseerManager->Draw(drawParameter);

	//エフェクトの描画終了
	effekseerRenderer->EndRendering();

	//経過時間
	timer++;
}

void DXEffekseer::BeginEffekseerDraw()
{
	//エフェクトの描画開始
	effekseerRenderer->BeginRendering();
	//ViewProjectionMatrixの設定
	drawParameter.ViewProjectionMatrix = effekseerRenderer->GetCameraProjectionMatrix();
}

void DXEffekseer::EndEffekseerDraw()
{
	//エフェクトの描画終了
	effekseerRenderer->EndRendering();
}

//エフェクトの描画
void DXEffekseer::DrawEffect(const Effekseer::Handle &handle)
{
	effekseerManager->DrawHandle(handle, drawParameter);
}
