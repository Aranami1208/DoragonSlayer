
#include "FluidManager.h"
#include "Fluid.h"

// ---------------------------------------------------------------------------
//
// 流体マネージャのコンストラクタ
//
//   なお、オブジェクトのdeleteはCBaseProcのデストラクタで行うため不要
//
// ---------------------------------------------------------------------------
FluidManager::FluidManager()
{
	ObjectManager::DontDestroy(this);		// 自体は消されない
	tagNo = 0;	// タグＮＯの初期値

	Init();   // 初期化処理
}
// ---------------------------------------------------------------------------
//
//  流体マネージャのデストラクタ
//
// ---------------------------------------------------------------------------
FluidManager::~FluidManager()
{
	SAFE_RELEASE(m_pCubeVBuffer);
	SAFE_RELEASE(m_pCubeIBuffer);
	SAFE_RELEASE(m_pFrontFace);
	SAFE_RELEASE(m_pBackFace);
	SAFE_RELEASE(m_pSampleLinearFluid);

	m_TexA.Release();
	m_TexB.Release();
}
/*
// ---------------------------------------------------------------------------
//
// 流体マネージャに流体オブジェクトを追加する
//
//   他のオブジェクトから呼ばれ、そのオブジェクト専用の流体オブジェクトを
//   作成するときに使用する
//   流体マネージャに流体オブジェクトを追加し、親オブジェクトアドレスを設定する
//   戻り値には、追加したオブジェクト番号が返る
//
//   引数
//		CBaseObj* pOyaObj  :  親オブジェクト(他のオブジェクト)
//
//   戻り値
//		int  作成したオブジェクト番号
// ---------------------------------------------------------------------------
Object3D* FluidManager::AddObj(Object3D* pOyaObj)
{
	Fluid* obj = Instantiate<Fluid>();
	if (obj == nullptr)  return nullptr;

	return obj;
}
*/
// ---------------------------------------------------------------------------
//
//  流体マネージャの初期化
//
// ---------------------------------------------------------------------------
void FluidManager::Init()
{
	D3D11_RENDER_TARGET_VIEW_DESC rdesc;
	D3D11_TEXTURE2D_DESC desc;
	D3D11_SHADER_RESOURCE_VIEW_DESC sdesc;
	D3D11_BUFFER_DESC bdesc;
	D3D11_SUBRESOURCE_DATA InitData;

	// 1パス用　テクスチャーAのレンダーターゲット関連作成 ------------------
	// テクスチャーの作成
	ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
	desc.Width = WINDOW_WIDTH;
	desc.Height = WINDOW_HEIGHT;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	GameDevice()->m_pD3D->m_pDevice->CreateTexture2D(&desc, NULL, &m_TexA.pTex2D);

	// そのテクスチャーに対しレンダーターゲットビュー(RTV)を作成	
	ZeroMemory(&rdesc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
	rdesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	rdesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rdesc.Texture2DArray.MipSlice = 0;
	GameDevice()->m_pD3D->m_pDevice->CreateRenderTargetView(m_TexA.pTex2D, &rdesc, &m_TexA.pRTV);

	// そのテクスチャーに対しシェーダーリソースビュー(SRV)を作成		
	ZeroMemory(&sdesc, sizeof(sdesc));
	sdesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sdesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	sdesc.Texture2D.MipLevels = 1;
	GameDevice()->m_pD3D->m_pDevice->CreateShaderResourceView(m_TexA.pTex2D, &sdesc, &m_TexA.pSRV);

	// そのテクスチャーのレンダーターゲット化に伴うデプスステンシルテクスチャー作成
	desc.Width = WINDOW_WIDTH;
	desc.Height = WINDOW_HEIGHT;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_D32_FLOAT;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	GameDevice()->m_pD3D->m_pDevice->CreateTexture2D(&desc, NULL, &m_TexA.pDSTex2D);

	// デプスステンシルテクスチャーのDSVを作成
	GameDevice()->m_pD3D->m_pDevice->CreateDepthStencilView(m_TexA.pDSTex2D, NULL, &m_TexA.pDSV);


	// 2パス用　テクスチャーBのレンダーターゲット関連作成 ----------------------
	// テクスチャーの作成
	ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
	desc.Width = WINDOW_WIDTH;
	desc.Height = WINDOW_HEIGHT;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	GameDevice()->m_pD3D->m_pDevice->CreateTexture2D(&desc, NULL, &m_TexB.pTex2D);

	// そのテクスチャーに対しレンダーターゲットビュー(RTV)を作成	
	ZeroMemory(&rdesc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
	rdesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	rdesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rdesc.Texture2DArray.MipSlice = 0;
	GameDevice()->m_pD3D->m_pDevice->CreateRenderTargetView(m_TexB.pTex2D, &rdesc, &m_TexB.pRTV);

	// そのテクスチャーに対しシェーダーリソースビュー(SRV)を作成		
	ZeroMemory(&sdesc, sizeof(sdesc));
	sdesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sdesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	sdesc.Texture2D.MipLevels = 1;
	GameDevice()->m_pD3D->m_pDevice->CreateShaderResourceView(m_TexB.pTex2D, &sdesc, &m_TexB.pSRV);

	// そのテクスチャーのレンダーターゲット化に伴うデプスステンシルテクスチャー作成
	desc.Width = WINDOW_WIDTH;
	desc.Height = WINDOW_HEIGHT;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_D32_FLOAT;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	GameDevice()->m_pD3D->m_pDevice->CreateTexture2D(&desc, NULL, &m_TexB.pDSTex2D);

	// デプスステンシルテクスチャーのDSVを作成
	GameDevice()->m_pD3D->m_pDevice->CreateDepthStencilView(m_TexB.pDSTex2D, NULL, &m_TexB.pDSV);


	// 場の３Ｄテクスチャを表示するための立方体を作成

	// バーテックスバッファを作成する
	MakeCubeVertexBuffer(1);   // とりあえず１ｍ四方の立方体を作成する

	// インデックスバッファm_pCubeIBuffer作成
	// ・三角形の頂点が右回りを表とする		  // -- 2024.3.23
	int IndexData[] =
	{
		2,1,0,
		2,3,1,
		1,4,0,
		1,5,4,
		3,5,1,
		3,7,5,
		3,2,6,
		7,3,6,
		2,0,4,
		6,2,4,
		5,6,4,
		7,6,5
	};
	bdesc.Usage = D3D11_USAGE_DEFAULT;
	bdesc.ByteWidth = sizeof(int) * 12 * 3;
	bdesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bdesc.CPUAccessFlags = 0;
	bdesc.MiscFlags = 0;
	InitData.pSysMem = IndexData;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;
	if (FAILED(GameDevice()->m_pD3D->m_pDevice->CreateBuffer(&bdesc, &InitData, &m_pCubeIBuffer)))
		return;


	// テクスチャー用サンプラー作成
	D3D11_SAMPLER_DESC SamDesc;
	ZeroMemory(&SamDesc, sizeof(D3D11_SAMPLER_DESC));
	SamDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	SamDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	SamDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	SamDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	SamDesc.BorderColor[0] = 0; SamDesc.BorderColor[1] = 0; SamDesc.BorderColor[2] = 0; SamDesc.BorderColor[3] = 0;
	GameDevice()->m_pD3D->m_pDevice->CreateSamplerState(&SamDesc, &m_pSampleLinearFluid);


	// ラスタライズ設定
	D3D11_RASTERIZER_DESC rdc;
	ZeroMemory(&rdc, sizeof(rdc));
	rdc.CullMode = D3D11_CULL_BACK;		// 後ろ向きの三角形を描画しません
	rdc.FillMode = D3D11_FILL_SOLID;
	rdc.FrontCounterClockwise = FALSE;	// 三角形の頂点がレンダーターゲット上で右回りならば三角形は前向き	 // -- 2024.3.23
	GameDevice()->m_pD3D->m_pDevice->CreateRasterizerState(&rdc, &m_pFrontFace);

	rdc.CullMode = D3D11_CULL_FRONT;	// 前向きの三角形を描画しません
	rdc.FillMode = D3D11_FILL_SOLID;
	rdc.FrontCounterClockwise = FALSE;	// 三角形の頂点がレンダーターゲット上で右回りならば三角形は前向き	 // -- 2024.3.23
	GameDevice()->m_pD3D->m_pDevice->CreateRasterizerState(&rdc, &m_pBackFace);

}

// ---------------------------------------------------------------------------
//
// 流体を上方向（Ｙ方向）に発生させる
//
//   主に、火が燃えているような、上方向に発生する炎や煙を発生させる
//
//   const Transform& trans     :	発生位置のワールドマトリックス
//   const float&     fCubeSize  :	表示用立方体の一辺の大きさ
//   const float&     fColorNo   :	流体の色コード（0:炎（黒煙なし）1: 炎（黒煙あり）2:青い炎 3:濃い煙 4:薄い煙）
//   const float&     fDensity   :	発生密度
//   const float&     fVelocity  :	速度
//   const float&     fRandom    :	速度方向ランダム率(0.0f:一定方向 ～ 1.0f:方向がランダム)
//   const int&    nAddVoxelWidth:	複数箇所から発生させるときのサイズ（ボクセル数）。１カ所の場合は０にする。
//   std::string&     tag        :	設定するタグ(in/out)
//
// 戻り値　Fluid* : 発生したオブジェクトアドレス。発生できなかったときはnullptr
//
// ---------------------------------------------------------------------------
Fluid* FluidManager::SpawnY(const Transform& trans, const float& fCubeSize, const float& fColorNo, const float& fDensity, const float& fVelocity, const float& fRandom, const int& nAddVoxelWidth, std::string& tag)
{
	Fluid* obj = SetFluidObj(tag);
	if (obj == nullptr)  return nullptr;

	// 流体を発生させる
	obj->Spawn( trans, fCubeSize, fColorNo, VECTOR4(FLUID_SIZE / 2, 6, FLUID_SIZE / 2, 2),  // VoxelPos  x,y,z と 発生半径
												VECTOR4(fDensity, 0, 0, 0), VECTOR4(0, fVelocity, 0, 0), fRandom, nAddVoxelWidth);

	return obj;
}

// ---------------------------------------------------------------------------
//
// 流体を奥方向（Ｚ方向）に発生させる
//
//   主に、火炎放射器のような、前方向に発生する炎や煙を発生させる
//
//   const Transform& trans     :	発生位置のワールドマトリックス
//   const float&     fCubeSize  :	表示用立方体の一辺の大きさ
//   const float&     fColorNo   :	流体の色コード（0:炎（黒煙なし）1: 炎（黒煙あり）2:青い炎 3:濃い煙 4:薄い煙）
//   const float&     fDensity   :	発生密度
//   const float&     fVelocity  :	速度
//   const float&     fRandom    :	速度方向ランダム率(0.0f:一定方向 ～ 1.0f:方向がランダム)
//   const int&    nAddVoxelWidth:	複数箇所から発生させるときのサイズ（ボクセル数）。１カ所の場合は０にする。
//   std::string&      tag        :	設定するタグ(in/out)
//
// 戻り値　Fluid* : 発生したオブジェクトアドレス。発生できなかったときはnullptr
//
// ---------------------------------------------------------------------------
Fluid*  FluidManager::SpawnZ(const Transform& trans, const float& fCubeSize, const float& fColorNo, const float& fDensity, const float& fVelocity, const float& fRandom, const int& nAddVoxelWidth, std::string& tag)
{
	Fluid* obj = SetFluidObj(tag);
	if (obj == nullptr)  return nullptr;

	// 流体を発生させる
	obj->Spawn( trans, fCubeSize, fColorNo, VECTOR4(FLUID_SIZE / 2, FLUID_SIZE / 2, 6, 2),  // VoxelPos  x,y,z と 発生半径
												VECTOR4(fDensity, 0, 0, 0), VECTOR4(0, 0, fVelocity, 0), fRandom, nAddVoxelWidth);

	return obj;
}

// ---------------------------------------------------------------------------
//
// 流体を奥方向（Ｚ方向）に発生させる
//
//   主に、火炎放射器のような、前方向に発生する炎や煙を発生させる
//
//   const Transform& trans     :	発生位置のワールドマトリックス
//   const float&     fCubeSize  :	表示用立方体の一辺の大きさ
//   const float&     fColorNo   :	流体の色コード（0:炎（黒煙なし）1: 炎（黒煙あり）2:青い炎 3:濃い煙 4:薄い煙）
//   const float&     fDensity   :	発生密度
//   const float&     fVelocity  :	速度
//   const float&     fRandom    :	速度方向ランダム率(0.0f:一定方向 ～ 1.0f:方向がランダム)
//   const int&    nAddVoxelWidth:	複数箇所から発生させるときのサイズ（ボクセル数）。１カ所の場合は０にする。
//   std::string&      tag        :	設定するタグ(in/out)
//
// 戻り値　Fluid* : 発生したオブジェクトアドレス。発生できなかったときはnullptr
//
// ---------------------------------------------------------------------------
Fluid* FluidManager::SetFluidObj( std::string& tag)
{
	Fluid* obj = nullptr;
	if (tag == "")
	{
		obj = Instantiate<Fluid>();
		tagNo++;
		tag = std::to_string(tagNo);
		obj->SetTag(tag);
	}
	else {
		obj = ObjectManager::FindGameObjectWithTag<Fluid>(tag);
		if (obj == nullptr)
		{
			obj = Instantiate<Fluid>();
			obj->SetTag(tag);
		}
	}
	if (obj == nullptr)  return nullptr;
	return obj;
}

//------------------------------------------------------------------------
// 表示用立方体の頂点バッファm_pCubeVBufferを作成・更新する関数
// 
// 引数 const float& fSiz : 一辺の大きさ
// 
//------------------------------------------------------------------------
HRESULT FluidManager::MakeCubeVertexBuffer(const float& fSize)
{
	D3D11_BUFFER_DESC bdesc;
	D3D11_SUBRESOURCE_DATA InitData;

	// 一辺１ｍの基本立方体
	// ・原点は、左下手前
	VECTOR3 VertexConst[] =
	{
		VECTOR3(0,0,1),
		VECTOR3(0,0,0),
		VECTOR3(0,1,1),
		VECTOR3(0,1,0),
		VECTOR3(1,0,1),
		VECTOR3(1,0,0),
		VECTOR3(1,1,1),
		VECTOR3(1,1,0),
	};

	// 一辺fSizeメートルの表示用立方体
	VECTOR3 VertexData[8];
	for (int i = 0; i < 8; i++)
	{
		VertexData[i] = VertexConst[i] * fSize;	// 基本立方体をfSize倍にする
	}

	if (!m_pCubeVBuffer)
	{
		// まだ作成されていないため、頂点バッファm_pCubeVBufferを新規作成する
		//bdesc.Usage = D3D11_USAGE_DEFAULT;
		bdesc.Usage = D3D11_USAGE_DYNAMIC;
		bdesc.ByteWidth = sizeof(VECTOR3) * 8;
		bdesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		//bdesc.CPUAccessFlags = 0;
		bdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bdesc.MiscFlags = 0;
		InitData.pSysMem = VertexData;
		if (FAILED(GameDevice()->m_pD3D->m_pDevice->CreateBuffer(&bdesc, &InitData, &m_pCubeVBuffer)))
			return E_FAIL;
	}
	else {
		// すでに作成済みのため、頂点バッファm_pCubeVBufferの書き換えをする
		D3D11_MAPPED_SUBRESOURCE msr;
		if (SUCCEEDED(GameDevice()->m_pD3D->m_pDeviceContext->Map(m_pCubeVBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr)))
		{
			memcpy(msr.pData, VertexData, sizeof(VECTOR3) * 8);
			GameDevice()->m_pD3D->m_pDeviceContext->Unmap(m_pCubeVBuffer, 0);
		}
		else {
			return E_FAIL;
		}
	}

	// 立方体の大きさを設定
	m_fCubeSize = fSize;

	return S_OK;
}


//------------------------------------------------------------------------
//
//	流体を画面に描画する処理
//
// 引数
//      const MATRIX4X4& mWorld                 ワールドマトリックス
//      const VECTOR4& vOption                  オプション 
//      const ID3D11ShaderResourceView** ppTex3D_SRV  3Dテクスチャ
//      const MATRIX4X4& mView                  ビューマトリックス
//      const MATRIX4X4& mProj                  プロジェクションマトリックス
//      const VECTOR3&   vLight                 光源ベクトル
//      const VECTOR3&   vEye                   視点ベクトル(使っていない)
//
//	戻り値 HRESULT
//		S_OK	= 正常
//		E_FAIL	= 異常
//
//------------------------------------------------------------------------
void FluidManager::Draw(const MATRIX4X4& mWorld, const VECTOR4& vOption, ID3D11ShaderResourceView** ppTex3D_SRV, const MATRIX4X4& mView, const MATRIX4X4& mProj, const VECTOR3& vLight, const VECTOR3& vEye)
{

	// 画面クリア
	float ClearColor[4] = { 0,0,0,0 };//クリア色作成　RGBAの順

	// 表示用立方体の大きさが変わっているか
	if ( m_fCubeSize != vOption.x)
	{
		MakeCubeVertexBuffer(vOption.x);   // バーテックスバッファを更新する
		m_fCubeSize = vOption.x;
	}

	// ブレンドステート透明色
	// (ここをm_pBlendStateNormalにすると、場が表示される。但しこのときはこのメソッドの最後でm_pBlendStateTrapenに戻すこと)
	GameDevice()->m_pD3D->m_pDeviceContext->OMSetBlendState(GameDevice()->m_pD3D->m_pBlendStateTrapen, NULL, 0xffffffff);

	D3D11_MAPPED_SUBRESOURCE pData;
	CONSTANT_BUFFER_WVLED cb = {};
	UINT stride = 0;
	UINT offset = 0;

	// サンプラーステートをセットする
	GameDevice()->m_pD3D->m_pDeviceContext->PSSetSamplers(0, 1, &m_pSampleLinearFluid);

	// 現在のレンダーターゲットを保存しておく
	ID3D11RenderTargetView*   pTargetRTVSave = GameDevice()->m_pD3D->m_pTarget_TexRTV;
	ID3D11DepthStencilView*   pTargetDSVSave = GameDevice()->m_pD3D->m_pTarget_DSTexDSV;

	// パス１  テクスチャーAにレンダリング ------------------------------------------------

	// レンダーターゲットをテクスチャーAに変更
	GameDevice()->m_pD3D->SetRenderTarget(m_TexA.pRTV, m_TexA.pDSV);
	GameDevice()->m_pD3D->ClearRenderTarget(ClearColor);  // 画面クリヤー

	// フロントフェイスのみレンダリングするようにカリングモードをセット
	GameDevice()->m_pD3D->m_pDeviceContext->RSSetState(m_pFrontFace);

	// 使用するシェーダーのセット	
	GameDevice()->m_pD3D->m_pDeviceContext->VSSetShader(GameDevice()->m_pShader->m_pFluidTexture_VS, NULL, 0);
	GameDevice()->m_pD3D->m_pDeviceContext->PSSetShader(GameDevice()->m_pShader->m_pFluidTexture_PS, NULL, 0);

	// シェーダーのコンスタントバッファーに各種データを渡す
	if (SUCCEEDED(GameDevice()->m_pD3D->m_pDeviceContext->Map(GameDevice()->m_pShader->m_pConstantBufferWVLED, 0, D3D11_MAP_WRITE_DISCARD, 0, &pData)))
	{
		// ワールド、カメラ、射影行列を渡す
		MATRIX4X4 m = XMMatrixTranspose(mWorld*mView*mProj);
		cb.mWVP = m;
		cb.mW = XMMatrixTranspose(mWorld);
		cb.vEyePos = vEye;
		cb.vDiffuse = vOption;
		cb.vDrawInfo = VECTOR4(0, 0, 0, 0);    // つかっていない    // -- 2021.1.11
		memcpy_s(pData.pData, pData.RowPitch, (void*)(&cb), sizeof(cb));
		GameDevice()->m_pD3D->m_pDeviceContext->Unmap(GameDevice()->m_pShader->m_pConstantBufferWVLED, 0);
	}
	// 現在のシェーダーにこのコンスタントバッファーをセット
	GameDevice()->m_pD3D->m_pDeviceContext->VSSetConstantBuffers(0, 1, &GameDevice()->m_pShader->m_pConstantBufferWVLED);
	GameDevice()->m_pD3D->m_pDeviceContext->PSSetConstantBuffers(0, 1, &GameDevice()->m_pShader->m_pConstantBufferWVLED);
	// 頂点インプットレイアウトをセット
	GameDevice()->m_pD3D->m_pDeviceContext->IASetInputLayout(GameDevice()->m_pShader->m_pFluidTexture_VertexLayout);
	// プリミティブ・トポロジーをセット
	GameDevice()->m_pD3D->m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// 頂点バッファをセット
	stride = sizeof(VECTOR3);
	offset = 0;
	GameDevice()->m_pD3D->m_pDeviceContext->IASetVertexBuffers(0, 1, &m_pCubeVBuffer, &stride, &offset);
	// インデックスバッファをセット
	GameDevice()->m_pD3D->m_pDeviceContext->IASetIndexBuffer(m_pCubeIBuffer, DXGI_FORMAT_R32_UINT, 0);
	// レンダリング
	GameDevice()->m_pD3D->m_pDeviceContext->DrawIndexed(12 * 3, 0, 0);


	// パス２ テクスチャーBにレンダリング ---------------------------------------------------

	// レンダーターゲットをテクスチャーBに変更
	GameDevice()->m_pD3D->SetRenderTarget(m_TexB.pRTV, m_TexB.pDSV);
	GameDevice()->m_pD3D->ClearRenderTarget(ClearColor);  // 画面クリヤー

	// バックフェイスのみレンダリングするようにカリングモードをセット
	GameDevice()->m_pD3D->m_pDeviceContext->RSSetState(m_pBackFace);

	// 使用するシェーダーのセット	
	GameDevice()->m_pD3D->m_pDeviceContext->VSSetShader(GameDevice()->m_pShader->m_pFluidTexture_VS, NULL, 0);
	GameDevice()->m_pD3D->m_pDeviceContext->PSSetShader(GameDevice()->m_pShader->m_pFluidTexture_PS, NULL, 0);

	// シェーダーのコンスタントバッファーに各種データを渡す
	if (SUCCEEDED(GameDevice()->m_pD3D->m_pDeviceContext->Map(GameDevice()->m_pShader->m_pConstantBufferWVLED, 0, D3D11_MAP_WRITE_DISCARD, 0, &pData)))
	{
		// ワールド、カメラ、射影行列を渡す
		MATRIX4X4 m = XMMatrixTranspose(mWorld*mView*mProj);
		cb.mWVP = m;
		cb.mW = XMMatrixTranspose(mWorld);
		cb.vEyePos = vEye;
		cb.vDiffuse = vOption;
		cb.vDrawInfo = VECTOR4(0, 0, 0, 0);    // つかっていない    // -- 2021.1.11
		memcpy_s(pData.pData, pData.RowPitch, (void*)(&cb), sizeof(cb));
		GameDevice()->m_pD3D->m_pDeviceContext->Unmap(GameDevice()->m_pShader->m_pConstantBufferWVLED, 0);
	}
	// 現在のシェーダーにこのコンスタントバッファーをセット
	GameDevice()->m_pD3D->m_pDeviceContext->VSSetConstantBuffers(0, 1, &GameDevice()->m_pShader->m_pConstantBufferWVLED);
	GameDevice()->m_pD3D->m_pDeviceContext->PSSetConstantBuffers(0, 1, &GameDevice()->m_pShader->m_pConstantBufferWVLED);
	// 頂点インプットレイアウトをセット
	GameDevice()->m_pD3D->m_pDeviceContext->IASetInputLayout(GameDevice()->m_pShader->m_pFluidTexture_VertexLayout);
	// プリミティブ・トポロジーをセット
	GameDevice()->m_pD3D->m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// 頂点バッファをセット
	stride = sizeof(VECTOR3);
	offset = 0;
	GameDevice()->m_pD3D->m_pDeviceContext->IASetVertexBuffers(0, 1, &m_pCubeVBuffer, &stride, &offset);
	// インデックスバッファをセット
	GameDevice()->m_pD3D->m_pDeviceContext->IASetIndexBuffer(m_pCubeIBuffer, DXGI_FORMAT_R32_UINT, 0);
	// レンダリング
	GameDevice()->m_pD3D->m_pDeviceContext->DrawIndexed(12 * 3, 0, 0);


	// パス３  流体のレンダリング ----------------------------------------------

	// レンダーターゲットを保存しておいたターゲット（バックバッファー等）に戻す
	GameDevice()->m_pD3D->SetRenderTarget(pTargetRTVSave, pTargetDSVSave);

	// フロントフェイスのみレンダリングするようにカリングモードを戻す
	GameDevice()->m_pD3D->m_pDeviceContext->RSSetState(m_pFrontFace);

	// 使用するシェーダーのセット	
	GameDevice()->m_pD3D->m_pDeviceContext->VSSetShader(GameDevice()->m_pShader->m_pFluidRender_VS, NULL, 0);
	GameDevice()->m_pD3D->m_pDeviceContext->PSSetShader(GameDevice()->m_pShader->m_pFluidRender_PS, NULL, 0);

	// パス１とパス２で作成したテクスチャー2枚をシェーダーにセット
	GameDevice()->m_pD3D->m_pDeviceContext->PSSetShaderResources(0, 1, &m_TexA.pSRV);
	GameDevice()->m_pD3D->m_pDeviceContext->PSSetShaderResources(1, 1, &m_TexB.pSRV);

	// コンピュートシェーダーで作成した、3Dテクスチャーをシェーダーにセット
	GameDevice()->m_pD3D->m_pDeviceContext->PSSetShaderResources(2, 1, ppTex3D_SRV);

	// シェーダーのコンスタントバッファーに各種データを渡す
	if (SUCCEEDED(GameDevice()->m_pD3D->m_pDeviceContext->Map(GameDevice()->m_pShader->m_pConstantBufferWVLED, 0, D3D11_MAP_WRITE_DISCARD, 0, &pData)))
	{
		// ワールド、カメラ、射影行列を渡す
		MATRIX4X4 m = XMMatrixTranspose(mWorld*mView*mProj);
		cb.mWVP = m;
		cb.mW = XMMatrixTranspose(mWorld);
		cb.vEyePos = vEye;
		cb.vDiffuse = vOption;
		memcpy_s(pData.pData, pData.RowPitch, (void*)(&cb), sizeof(cb));
		GameDevice()->m_pD3D->m_pDeviceContext->Unmap(GameDevice()->m_pShader->m_pConstantBufferWVLED, 0);
	}
	// 現在のシェーダーにこのコンスタントバッファーをセット
	GameDevice()->m_pD3D->m_pDeviceContext->VSSetConstantBuffers(0, 1, &GameDevice()->m_pShader->m_pConstantBufferWVLED);
	GameDevice()->m_pD3D->m_pDeviceContext->PSSetConstantBuffers(0, 1, &GameDevice()->m_pShader->m_pConstantBufferWVLED);
	// 頂点インプットレイアウトをセット
	GameDevice()->m_pD3D->m_pDeviceContext->IASetInputLayout(GameDevice()->m_pShader->m_pFluidRender_VertexLayout);
	// プリミティブ・トポロジーをセット
	GameDevice()->m_pD3D->m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// 頂点バッファをセット
	stride = sizeof(VECTOR3);
	offset = 0;
	GameDevice()->m_pD3D->m_pDeviceContext->IASetVertexBuffers(0, 1, &m_pCubeVBuffer, &stride, &offset);
	// インデックスバッファをセット
	GameDevice()->m_pD3D->m_pDeviceContext->IASetIndexBuffer(m_pCubeIBuffer, DXGI_FORMAT_R32_UINT, 0);

	// レンダリング
	GameDevice()->m_pD3D->m_pDeviceContext->DrawIndexed(12 * 3, 0, 0);

	// 透明色のブレンドに戻す
	//GameDevice()->m_pD3D->m_pDeviceContext->OMSetBlendState(GameDevice()->m_pD3D->m_pBlendStateTrapen, NULL, 0xffffffff);

}


