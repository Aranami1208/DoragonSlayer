#include "ShadowControl.h"
#include "HDRManager.h"

ShadowControl::ShadowControl()
{
	ObjectManager::DontDestroy(this);		// 消されない
	ObjectManager::SetActive(this, false);		// 更新しない
	ObjectManager::SetDrawOrder(this, 100000);	// 一番最初に描画処理する
}

ShadowControl::~ShadowControl()
{
}

void ShadowControl::SetShadow(bool sdw)
{
	if (sdw)
	{
		ObjectManager::SetDrawTimes(MAX_CASCADE_SHADOW + 1);
	}
	else {
		ObjectManager::SetDrawTimes(1);
	}
}

// シャドウマップへの書き込みの初期処理
void ShadowControl::DrawDepth()
{
	// ブレンドステートマスク
	UINT mask = 0xffffffff;

	//ビューポートの設定(深度テクスチャー用のビューポート)
	D3D11_VIEWPORT vp;
	vp.Width = GameDevice()->m_pD3D->m_dwDepthTexWidth;
	vp.Height = GameDevice()->m_pD3D->m_dwDepthTexHeight;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	GameDevice()->m_pD3D->m_pDeviceContext->RSSetViewports(1, &vp);

	// レンダーターゲットを深度テクスチャーに変更
	float ClearColor[4] = { 0,0,0,1 };// クリア色作成　RGBAの順

	// 透明色の無い通常のブレンディングを設定                  // -- 2020.9.13
	// （深度マップテクスチャへ書き込むときは、透明色を考慮したブレンディングは不可）
	// （なぜなら、書き込む値は色ではなく深度値のため。深度値自体をブレンディングしてしまうため）
	// （なお、ピクセルシェーダの中で透明色は切り捨てているので、透明色には影ができない）
	GameDevice()->m_pD3D->m_pDeviceContext->OMSetBlendState(GameDevice()->m_pD3D->m_pBlendStateNormal, nullptr, mask);

	// カスケードシャドウごとにレンダリングする
	int cn = ObjectManager::DrawCounter();

	GameDevice()->m_pD3D->SetRenderTarget(GameDevice()->m_pD3D->m_pDepthMap_TexRTV[cn], GameDevice()->m_pD3D->m_pDepthMap_DSTexDSV[cn]);
	GameDevice()->m_pD3D->ClearRenderTarget(ClearColor);

	// 光源位置にカメラを置く
	GameDevice()->m_mView = GameDevice()->m_mLightView[cn];
	GameDevice()->m_mProj = GameDevice()->m_mLightProj[cn];
}

// 画面への書き込みの初期処理
void ShadowControl::DrawScreen()
{

	//ビューポートの設定を元に戻す
	D3D11_VIEWPORT vp;
	vp.Width = GameDevice()->m_pD3D->m_dwWindowWidth;
	vp.Height = GameDevice()->m_pD3D->m_dwWindowHeight;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	GameDevice()->m_pD3D->m_pDeviceContext->RSSetViewports(1, &vp);

	// 透明色のブレンディングを設定
	UINT mask = 0xffffffff;
	GameDevice()->m_pD3D->m_pDeviceContext->OMSetBlendState(GameDevice()->m_pD3D->m_pBlendStateTrapen, nullptr, mask);

	// カメラを通常位置に戻す
	GameDevice()->m_mView = GameDevice()->m_mDrawView;
	GameDevice()->m_mProj = GameDevice()->m_mDrawProj;

	// HDRリミットが1.0より小さいときは、レンダーターゲットをHDRマップテクスチャにする
	float ClearColor[4] = { 0,0,0,1 };// クリア色作成　RGBAの順
	if (ObjectManager::FindGameObject<HDRManager>()->Limit() < 1.0f)
	{
		ObjectManager::FindGameObject<HDRManager>()->SetRenderTargetHDR(ClearColor);   // HDRマップテクスチャに切り替えて画面クリヤー
	}
	else {
		GameDevice()->m_pD3D->SetRenderTarget(nullptr, nullptr);      //レンダーターゲットを通常（バックバッファー）に戻す
		GameDevice()->m_pD3D->ClearRenderTarget(ClearColor);    // 通常の画面クリヤー
	}
}
