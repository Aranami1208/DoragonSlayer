//=============================================================================
//		ＨＤＲ描画のプログラム
//　　　　　　　　　　　　　　　　　　　　　　　       ver 3.3        2024.3.23
//
//																HDRManager.cpp
//=============================================================================
#include "HDRManager.h"
#include "HDRControl.h"

//------------------------------------------------------------------------
//
//	HDRManager  コンストラクタ	
//
//------------------------------------------------------------------------
HDRManager::HDRManager()
{
	m_pD3D = GameDevice()->m_pD3D;
	m_pShader = GameDevice()->m_pShader;

	ObjectManager::DontDestroy(this);		// 消されない
	ObjectManager::SetActive(this, false);	// 更新しない
	ObjectManager::SetVisible(this, false);	// 描画処理しない

	SingleInstantiate<HDRControl>();	 // HDRコントロールを作成する。削除されない。

	m_pVertexBufferHDR = nullptr;

	// HDRテクスチャ用
	m_pHDRMap_Tex = nullptr;
	m_pHDRMap_TexRTV = nullptr;
	m_pHDRMap_TexSRV = nullptr;
	m_pHDRMap_DSTex = nullptr;
	m_pHDRMap_DSTexDSV = nullptr;
	m_pHDRBright_Tex = nullptr;
	m_pHDRBright_TexRTV = nullptr;
	m_pHDRBright_TexSRV = nullptr;
	for (int i = 0; i < HDR_BLOOMTEX_COUNT; i++)
	{
		m_pHDRBloom_Tex[i] = nullptr;
		m_pHDRBloom_TexRTV[i] = nullptr;
		m_pHDRBloom_TexSRV[i] = nullptr;
	}
	m_fLimit = 1.0f;	  // ブライトのしきい値
	m_fForce = 1.0f;	  // ブルームの強さ

	DWORD dwWidth  = m_pD3D->m_dwWindowWidth;
	DWORD dwHeight = m_pD3D->m_dwWindowHeight;

	//バーテックスバッファー作成
	//気をつけること。z値を１以上にしない。クリップ空間でz=1は最も奥を意味する。したがって描画されない。
	HDRVertex vertices[] =
	{
		VECTOR3(0,              (float)dwHeight, 0), VECTOR2(0,1),      //頂点1  左下
		VECTOR3(0,                            0, 0), VECTOR2(0,0),      //頂点2　左上    // -- 2024.3.23
		VECTOR3((float)dwWidth, (float)dwHeight, 0), VECTOR2(1,1),      //頂点3　右下    // -- 2024.3.23
		VECTOR3((float)dwWidth,               0, 0), VECTOR2(1,0),      //頂点4　右上
	};

	// バーテックスバッファ作成する
	D3D11_BUFFER_DESC bd;
	//bd.Usage          = D3D11_USAGE_DEFAULT;
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(HDRVertex) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	//bd.CPUAccessFlags = 0;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = vertices;
	if (FAILED(m_pD3D->m_pDevice->CreateBuffer(&bd, &InitData, &m_pVertexBufferHDR)))
	{
		MessageBox(0, _T("HDR.cpp バーテックスバッファー作成失敗"), nullptr, MB_OK);
	}


	// ---------------------------------------------------------------------------------------
	// HDRマップテクスチャーを作成
	D3D11_TEXTURE2D_DESC tdesc;
	ZeroMemory(&tdesc, sizeof(D3D11_TEXTURE2D_DESC));
	tdesc.Width = m_pD3D->m_dwWindowWidth;  // HDRテクスチャの大きさ
	tdesc.Height = m_pD3D->m_dwWindowHeight;
	tdesc.MipLevels = 1;     // ミップマップ・レベル数
	tdesc.ArraySize = 1;
	tdesc.MiscFlags = 0;
	//tdesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT; // テクスチャ・フォーマット
	tdesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;  // テクスチャ・フォーマット
	tdesc.SampleDesc.Count = 1;
	tdesc.SampleDesc.Quality = 0;
	tdesc.Usage = D3D11_USAGE_DEFAULT;
	tdesc.BindFlags = D3D11_BIND_RENDER_TARGET |      // 描画ターゲット
		D3D11_BIND_SHADER_RESOURCE;       // シェーダで使う
	tdesc.CPUAccessFlags = 0;
	m_pD3D->m_pDevice->CreateTexture2D(&tdesc, nullptr, &m_pHDRMap_Tex);

	// HDRマップテクスチャー用　レンダーターゲットビュー作成
	D3D11_RENDER_TARGET_VIEW_DESC DescRT;
	DescRT.Format = tdesc.Format;
	DescRT.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	DescRT.Texture2D.MipSlice = 0;
	m_pD3D->m_pDevice->CreateRenderTargetView(m_pHDRMap_Tex, &DescRT, &m_pHDRMap_TexRTV);

	// HDRマップテクスチャ用　シェーダーリソースビュー(SRV)作成	
	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	ZeroMemory(&SRVDesc, sizeof(SRVDesc));
	SRVDesc.Format = tdesc.Format;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Texture2D.MipLevels = tdesc.MipLevels;
	m_pD3D->m_pDevice->CreateShaderResourceView(m_pHDRMap_Tex, &SRVDesc, &m_pHDRMap_TexSRV);

	// HDRマップテクスチャをレンダーターゲットにする際のデプスステンシルビュー用のテクスチャーを作成
	D3D11_TEXTURE2D_DESC descDepth;
	descDepth.Width = m_pD3D->m_dwWindowWidth;
	descDepth.Height = m_pD3D->m_dwWindowHeight;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D32_FLOAT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	m_pD3D->m_pDevice->CreateTexture2D(&descDepth, nullptr, &m_pHDRMap_DSTex);

	//そのテクスチャーに対しデプスステンシルビュー(DSV)を作成
	m_pD3D->m_pDevice->CreateDepthStencilView(m_pHDRMap_DSTex, nullptr, &m_pHDRMap_DSTexDSV);

	// ビューを作成したらテクスチャは不要なので削除    // -- 2022.6.29
	SAFE_RELEASE(m_pHDRMap_Tex);
	SAFE_RELEASE(m_pHDRMap_DSTex);

	// ----------------------------------------------------------
	// HDRブライトテクスチャーを作成
	ZeroMemory(&tdesc, sizeof(D3D11_TEXTURE2D_DESC));
	tdesc.Width = m_pD3D->m_dwWindowWidth;  // HDRテクスチャの大きさ
	tdesc.Height = m_pD3D->m_dwWindowHeight;
	tdesc.MipLevels = 1;     // ミップマップ・レベル数
	tdesc.ArraySize = 1;
	tdesc.MiscFlags = 0;
	//tdesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT; // テクスチャ・フォーマット
	tdesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;  // テクスチャ・フォーマット
	tdesc.SampleDesc.Count = 1;
	tdesc.SampleDesc.Quality = 0;
	tdesc.Usage = D3D11_USAGE_DEFAULT;
	tdesc.BindFlags = D3D11_BIND_RENDER_TARGET |      // 描画ターゲット
		D3D11_BIND_SHADER_RESOURCE;       // シェーダで使う
	tdesc.CPUAccessFlags = 0;
	m_pD3D->m_pDevice->CreateTexture2D(&tdesc, nullptr, &m_pHDRBright_Tex);

	// HDRブライトテクスチャー用　レンダーターゲットビュー作成
	DescRT.Format = tdesc.Format;
	DescRT.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	DescRT.Texture2D.MipSlice = 0;
	m_pD3D->m_pDevice->CreateRenderTargetView(m_pHDRBright_Tex, &DescRT, &m_pHDRBright_TexRTV);

	// HDRブライトテクスチャ用　シェーダーリソースビュー(SRV)作成	
	ZeroMemory(&SRVDesc, sizeof(SRVDesc));
	SRVDesc.Format = tdesc.Format;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Texture2D.MipLevels = tdesc.MipLevels;
	m_pD3D->m_pDevice->CreateShaderResourceView(m_pHDRBright_Tex, &SRVDesc, &m_pHDRBright_TexSRV);

	// ビューを作成したらテクスチャは不要なので削除    // -- 2022.6.29
	SAFE_RELEASE(m_pHDRBright_Tex);

	// ----------------------------------------------------------
	// HDRブルームテクスチャーを作成
	for (int i = 0; i < HDR_BLOOMTEX_COUNT; i++)
	{
		ZeroMemory(&tdesc, sizeof(D3D11_TEXTURE2D_DESC));
		tdesc.Width = m_pD3D->m_dwWindowWidth;  // HDRテクスチャの大きさ
		tdesc.Height = m_pD3D->m_dwWindowHeight;
		tdesc.MipLevels = 1;     // ミップマップ・レベル数
		tdesc.ArraySize = 1;
		tdesc.MiscFlags = 0;
		//tdesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT; // テクスチャ・フォーマット
		tdesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;  // テクスチャ・フォーマット
		tdesc.SampleDesc.Count = 1;
		tdesc.SampleDesc.Quality = 0;
		tdesc.Usage = D3D11_USAGE_DEFAULT;
		tdesc.BindFlags = D3D11_BIND_RENDER_TARGET |      // 描画ターゲット
			D3D11_BIND_SHADER_RESOURCE;       // シェーダで使う
		tdesc.CPUAccessFlags = 0;
		m_pD3D->m_pDevice->CreateTexture2D(&tdesc, nullptr, &m_pHDRBloom_Tex[i]);

		// HDRブルームテクスチャー用　レンダーターゲットビュー作成
		DescRT.Format = tdesc.Format;
		DescRT.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		DescRT.Texture2D.MipSlice = 0;
		m_pD3D->m_pDevice->CreateRenderTargetView(m_pHDRBloom_Tex[i], &DescRT, &m_pHDRBloom_TexRTV[i]);

		// HDRブルームテクスチャ用　シェーダーリソースビュー(SRV)作成	
		ZeroMemory(&SRVDesc, sizeof(SRVDesc));
		SRVDesc.Format = tdesc.Format;
		SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		SRVDesc.Texture2D.MipLevels = tdesc.MipLevels;
		m_pD3D->m_pDevice->CreateShaderResourceView(m_pHDRBloom_Tex[i], &SRVDesc, &m_pHDRBloom_TexSRV[i]);

		// ビューを作成したらテクスチャは不要なので削除    // -- 2022.6.29
		SAFE_RELEASE(m_pHDRBloom_Tex[i]);
	}

}

//------------------------------------------------------------------------
//
//	HDRManager  デストラクタ	
//
//------------------------------------------------------------------------
HDRManager::~HDRManager()
{
	SAFE_RELEASE(m_pVertexBufferHDR);

	SAFE_RELEASE(m_pHDRMap_Tex);
	SAFE_RELEASE(m_pHDRMap_TexRTV);
	SAFE_RELEASE(m_pHDRMap_TexSRV);
	SAFE_RELEASE(m_pHDRMap_DSTex);
	SAFE_RELEASE(m_pHDRMap_DSTexDSV);

	SAFE_RELEASE(m_pHDRBright_Tex);
	SAFE_RELEASE(m_pHDRBright_TexRTV);
	SAFE_RELEASE(m_pHDRBright_TexSRV);

	for (int i = 0; i < HDR_BLOOMTEX_COUNT; i++)
	{
		SAFE_RELEASE(m_pHDRBloom_Tex[i]);
		SAFE_RELEASE(m_pHDRBloom_TexRTV[i]);
		SAFE_RELEASE(m_pHDRBloom_TexSRV[i]);
	}

}
//------------------------------------------------------------------------
//
//	レンダーターゲットをHDRマップテクスチャーにする	
//
//	引数
//		float BackColor[]：クリヤー色
//
//	戻り値
//		なし
//------------------------------------------------------------------------
void   HDRManager::SetRenderTargetHDR(float BackColor[])
{
	// レンダーターゲットをHDRマップテクスチャーに設定する
	m_pD3D->SetRenderTarget(m_pHDRMap_TexRTV, m_pHDRMap_DSTexDSV);

	// HDRマップテクスチャのクリア
	m_pD3D->ClearRenderTarget(BackColor); // 画面クリア
}

//------------------------------------------------------------------------
void   HDRManager::SetLimitForce(float limit, float force)
{
	m_fLimit = limit;		// ブライトのしきい値
	m_fForce = force;		// ブルームの強さ
}

//------------------------------------------------------------------------
//
//	HDRレンダリング	
//
//	引数
//		なし
//
//	戻り値
//------------------------------------------------------------------------
void   HDRManager::Render()
{
	float ClearColor[4] = { 0.0f,0.0f,0.0f,1 };

	//   レンダーターゲットをHDRブライトテクスチャーにする
	m_pD3D->SetRenderTarget(m_pHDRBright_TexRTV, nullptr);
	// HDRブライトテクスチャのクリア
	m_pD3D->m_pDeviceContext->ClearRenderTargetView(m_pHDRBright_TexRTV, ClearColor); // 画面クリア

	// HDRブライト
	RenderBright();

	// HDRブルーム
	RenderBloom();

	// レンダーターゲットを通常（バックバッファー）に戻す
	m_pD3D->SetRenderTarget(nullptr, nullptr);
	m_pD3D->ClearRenderTarget(ClearColor); // 画面クリア

	// 透明色の無い通常のブレンディングを設定                  // -- 2020.1.24
	UINT mask = 0xffffffff;
	m_pD3D->m_pDeviceContext->OMSetBlendState(m_pD3D->m_pBlendStateNormal, nullptr, mask);

	// 最終レンダリング
	RenderFinish();

	// 透明色のブレンディングを設定
	m_pD3D->m_pDeviceContext->OMSetBlendState(m_pD3D->m_pBlendStateTrapen, nullptr, mask);

}


//------------------------------------------------------------------------
//
//	レンダリング   ブライト	
//
//	引数
//		なし
//
//	戻り値
//		なし
//------------------------------------------------------------------------
void   HDRManager::RenderBright()
{

	//使用するシェーダーのセット
	m_pD3D->m_pDeviceContext->VSSetShader(m_pShader->m_pHDR_VS, nullptr, 0);
	m_pD3D->m_pDeviceContext->PSSetShader(m_pShader->m_pHDR_PS_BRIGHT, nullptr, 0);

	//このコンスタントバッファーをどのシェーダーで使うか
	m_pD3D->m_pDeviceContext->VSSetConstantBuffers(0, 1, &m_pShader->m_pConstantBufferHDR);
	m_pD3D->m_pDeviceContext->PSSetConstantBuffers(0, 1, &m_pShader->m_pConstantBufferHDR);
	//頂点インプットレイアウトをセット
	m_pD3D->m_pDeviceContext->IASetInputLayout(m_pShader->m_pHDR_VertexLayout);

	//プリミティブ・トポロジーをセット
	m_pD3D->m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// サンプラーをセット
	m_pD3D->m_pDeviceContext->PSSetSamplers(0, 1, &m_pD3D->m_pSampleLinear);

	//バーテックスバッファーをセット
	UINT stride = sizeof(HDRVertex);
	UINT offset = 0;
	m_pD3D->m_pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBufferHDR, &stride, &offset);

	//シェーダーのコンスタントバッファーに各種データを渡す
	D3D11_MAPPED_SUBRESOURCE pData;
	CONSTANT_BUFFER_HDR     cb;
	if (SUCCEEDED(m_pD3D->m_pDeviceContext->Map(m_pShader->m_pConstantBufferHDR, 0, D3D11_MAP_WRITE_DISCARD, 0, &pData)))
	{

		//ビューポートサイズを渡す（クライアント領域の横と縦）
		cb.ViewPort.x = m_pD3D->m_dwWindowWidth;
		cb.ViewPort.y = m_pD3D->m_dwWindowHeight;
		cb.Info = VECTOR2(m_fLimit, 0);    // ブライトのしきい値
		memcpy_s(pData.pData, pData.RowPitch, (void*)(&cb), sizeof(cb));
		m_pD3D->m_pDeviceContext->Unmap(m_pShader->m_pConstantBufferHDR, 0);
	}

	//テクスチャーをシェーダーに渡す
	m_pD3D->m_pDeviceContext->PSSetShaderResources(0, 1, &m_pHDRMap_TexSRV);

	//プリミティブをレンダリング
	m_pD3D->m_pDeviceContext->Draw(4, 0);

}

//------------------------------------------------------------------------
//
//	レンダリング   ブルーム	
//
//	引数
//		なし
//
//	戻り値
//		なし
//------------------------------------------------------------------------
void   HDRManager::RenderBloom()
{
	float ClearColor[4] = { 0.0f,0.0f,0.0f,1 };

	//使用するシェーダーのセット
	m_pD3D->m_pDeviceContext->VSSetShader(m_pShader->m_pHDR_VS, nullptr, 0);
	m_pD3D->m_pDeviceContext->PSSetShader(m_pShader->m_pHDR_PS_BLOOM, nullptr, 0);

	//このコンスタントバッファーをどのシェーダーで使うか
	m_pD3D->m_pDeviceContext->VSSetConstantBuffers(0, 1, &m_pShader->m_pConstantBufferHDR);
	m_pD3D->m_pDeviceContext->PSSetConstantBuffers(0, 1, &m_pShader->m_pConstantBufferHDR);

	//シェーダーのコンスタントバッファーに各種データを渡す
	D3D11_MAPPED_SUBRESOURCE pData;
	CONSTANT_BUFFER_HDR   cb;
	if (SUCCEEDED(m_pD3D->m_pDeviceContext->Map(m_pShader->m_pConstantBufferHDR, 0, D3D11_MAP_WRITE_DISCARD, 0, &pData)))
	{
		//ビューポートサイズを渡す（クライアント領域の横と縦）
		cb.ViewPort.x = m_pD3D->m_dwWindowWidth;
		cb.ViewPort.y = m_pD3D->m_dwWindowHeight;
		cb.Info = VECTOR2(0, 0);
		memcpy_s(pData.pData, pData.RowPitch, (void*)(&cb), sizeof(cb));
		m_pD3D->m_pDeviceContext->Unmap(m_pShader->m_pConstantBufferHDR, 0);
	}

	//頂点インプットレイアウトをセット
	m_pD3D->m_pDeviceContext->IASetInputLayout(m_pShader->m_pHDR_VertexLayout);

	//プリミティブ・トポロジーをセット
	m_pD3D->m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// サンプラーをセット
	m_pD3D->m_pDeviceContext->PSSetSamplers(0, 1, &m_pD3D->m_pSampleLinear);

	//バーテックスバッファーをセット
	UINT stride = sizeof(HDRVertex);
	UINT offset = 0;
	m_pD3D->m_pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBufferHDR, &stride, &offset);

	//このコンスタントバッファーをどのシェーダーで使うか
	m_pD3D->m_pDeviceContext->VSSetConstantBuffers(1, 1, &m_pShader->m_pConstantBufferHDRBloom);
	m_pD3D->m_pDeviceContext->PSSetConstantBuffers(1, 1, &m_pShader->m_pConstantBufferHDRBloom);


	// ブルームの処理
	// ブルームテクスチャ配列に順番に書き込みながらブルームを完成させていく
	// 最初、ブライトテクスチャを読み込んで、ブルームテクスチャの０番に書き込む
	// 次にブルームテクスチャの０番を読み込んで、ブルームテクスチャの１番に書き込む
	// このように順番に書き込みながら、ブルームを完成させていく。
	// 最終結果は、最後のブルームテクスチャ HDR_BLOOMTEX_COUNT-1番 に書き込まれている
	for (int j = 0; j < HDR_BLOOMTEX_COUNT; j++)
	{
		// レンダーターゲットをHDRブルームテクスチャーにする
		m_pD3D->SetRenderTarget( m_pHDRBloom_TexRTV[j], nullptr);

		// HDRブルームテクスチャのクリア
		m_pD3D->m_pDeviceContext->ClearRenderTargetView(m_pHDRBloom_TexRTV[j], ClearColor); // 画面クリア

		VECTOR4  cbh[15] = {};  // オフセットx,y＆ウエイトw
		float total = 0;

		// ブルームピクセルのサンプリング
		// 中央の1ピクセル＋上下または左右の7ピクセルづつ、合計１５ピクセル
		for (int i = 0; i < 15; i++)
		{
			float p = (i - (15 - 1)*0.5f)*0.0008f;   // 中心点からのサンプリング距離
			cbh[i] = (j % 2 == 0) ? VECTOR4(p, 0, 0, 0) : VECTOR4(0, p, 0, 0);  // ピクセルオフセット １回ごとに横と縦を切り替える x,y
			cbh[i].w = exp(-p*p / 2) / sqrt(3.14159f * 2);       // ガウス分布（正規分布）によってウェイト値 w を合計していく
			total += cbh[i].w;
		}

		for (int i = 0; i < 15; i++) cbh[i].w /= total;  // ウェイト値wを作成する

		if (SUCCEEDED(m_pD3D->m_pDeviceContext->Map(m_pShader->m_pConstantBufferHDRBloom, 0, D3D11_MAP_WRITE_DISCARD, 0, &pData)))
		{
			memcpy_s(pData.pData, pData.RowPitch, (void*)(&cbh), sizeof(cbh));
			m_pD3D->m_pDeviceContext->Unmap(m_pShader->m_pConstantBufferHDRBloom, 0);
		}

		// テクスチャーをシェーダーに渡す
		if (j == 0)
		{
			// 最初は、ブライトテクスチャを渡す
			m_pD3D->m_pDeviceContext->PSSetShaderResources(0, 1, &m_pHDRBright_TexSRV);
		}
		else {
			// ２回目からは、一つ前のブルームテクスチャを渡す
			m_pD3D->m_pDeviceContext->PSSetShaderResources(0, 1, &m_pHDRBloom_TexSRV[j - 1]);
		}

		//プリミティブをレンダリング
		m_pD3D->m_pDeviceContext->Draw(4, 0);

	}

}

//------------------------------------------------------------------------
//
//	最終レンダリング	
//
//	引数
//		なし
//
//	戻り値
//		なし
//------------------------------------------------------------------------
void   HDRManager::RenderFinish()
{
	//使用するシェーダーのセット
	m_pD3D->m_pDeviceContext->VSSetShader(m_pShader->m_pHDR_VS, nullptr, 0);
	m_pD3D->m_pDeviceContext->PSSetShader(m_pShader->m_pHDR_PS, nullptr, 0);

	//このコンスタントバッファーをどのシェーダーで使うか
	m_pD3D->m_pDeviceContext->VSSetConstantBuffers(0, 1, &m_pShader->m_pConstantBufferHDR);
	m_pD3D->m_pDeviceContext->PSSetConstantBuffers(0, 1, &m_pShader->m_pConstantBufferHDR);
	//頂点インプットレイアウトをセット
	m_pD3D->m_pDeviceContext->IASetInputLayout(m_pShader->m_pHDR_VertexLayout);

	//プリミティブ・トポロジーをセット
	m_pD3D->m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// サンプラーをセット
	m_pD3D->m_pDeviceContext->PSSetSamplers(0, 1, &m_pD3D->m_pSampleLinear);

	// Zバッファを無効化
	m_pD3D->SetZBuffer(FALSE);

	//バーテックスバッファーをセット
	UINT stride = sizeof(HDRVertex);
	UINT offset = 0;
	m_pD3D->m_pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBufferHDR, &stride, &offset);

	//シェーダーのコンスタントバッファーに各種データを渡す
	D3D11_MAPPED_SUBRESOURCE pData;
	CONSTANT_BUFFER_HDR     cb;
	if (SUCCEEDED(m_pD3D->m_pDeviceContext->Map(m_pShader->m_pConstantBufferHDR, 0, D3D11_MAP_WRITE_DISCARD, 0, &pData)))
	{

		//ビューポートサイズを渡す（クライアント領域の横と縦）
		cb.ViewPort.x = m_pD3D->m_dwWindowWidth;
		cb.ViewPort.y = m_pD3D->m_dwWindowHeight;
		cb.Info = VECTOR2(0, m_fForce);            // ブルームの強さ　1.0f以上
		memcpy_s(pData.pData, pData.RowPitch, (void*)(&cb), sizeof(cb));
		m_pD3D->m_pDeviceContext->Unmap(m_pShader->m_pConstantBufferHDR, 0);
	}

	//テクスチャーをシェーダーに渡す
	m_pD3D->m_pDeviceContext->PSSetShaderResources(0, 1, &m_pHDRMap_TexSRV);   // ＨＤＲマップテクスチャ
	m_pD3D->m_pDeviceContext->PSSetShaderResources(1, 1, &m_pHDRBloom_TexSRV[HDR_BLOOMTEX_COUNT-1]);   // 最終のHDRブルームテクスチャ

	//プリミティブをレンダリング
	m_pD3D->m_pDeviceContext->Draw(4, 0);

	//Zバッファを有効化
	m_pD3D->SetZBuffer(TRUE);

}