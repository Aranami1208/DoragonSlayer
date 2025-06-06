
#include "FluidManager.h"
#include "Fluid.h"


namespace {
	const int   KeepTime = 30;
};
// ---------------------------------------------------------------------------
//
//  流体オブジェクトのコンストラクタ
//
// ---------------------------------------------------------------------------
Fluid::Fluid()
{
	Init();		//  流体オブジェクトの初期化

	m_bAddFlag = false;						// 注入フラグ
	m_vAddVoxelPos = VECTOR4(0, 0, 0, 0);	// 注入ボクセル位置
	m_vAddDensity = VECTOR4(0, 0, 0, 0);	// 注入密度
	m_vAddVelocity = VECTOR4(0, 0, 0, 0);	// 注入速度
	m_vOption = VECTOR4(0, 0, 0, 0);		// オプション(x:表示立方体の大きさ　y:色コード　z:なし　w:発生ランダム率)
	m_nAddVoxelWidth = 0;					// 複数箇所から発生させるときのサイズ（ボクセル数）

	m_vOption.x = 1;		// 初期値として、表示立方体のサイズを1mとする

	m_updateTimer = 0;
	m_keepTimer = 0;

	ObjectManager::SetDrawOrder(this, -200);   // 透明色は遅めに描画する
}

// ---------------------------------------------------------------------------
//
//  流体オブジェクトのデストラクタ
//
// ---------------------------------------------------------------------------
Fluid::~Fluid()
{
	for (int i = 0; i<2; i++) m_Dns[i].Release();		// 密度
	for (int i = 0; i<2; i++) m_Vlc[i].Release();		// 速度
	m_Vlc_s.Release();		// 速度のセミグランジアン
	m_Vlc_bs.Release();		// 速度の逆向きのセミグランジアン
	m_Prs.Release();		// 圧力
	m_Div.Release();		// 発散
	m_Dns_Tex.Release();	// 描画
}

// ---------------------------------------------------------------------------
//
//  流体オブジェクトの初期化
//
// ---------------------------------------------------------------------------
void Fluid::Init()
{
	D3D11_UNORDERED_ACCESS_VIEW_DESC udesc;
	D3D11_BUFFER_DESC bdesc;

	// 3Dテクスチャーの生成
	CreateEmpty3DTexture(FLUID_SIZE + 2, &m_Dns_Tex.pTex3D, &m_Dns_Tex.pSRV);
	for (int i = 0; i<2; i++) CreateEmpty3DTexture(FLUID_SIZE + 2, &m_Dns[i].pTex3D, &m_Dns[i].pSRV);
	for (int i = 0; i<2; i++) CreateEmpty3DTexture(FLUID_SIZE + 2, &m_Vlc[i].pTex3D, &m_Vlc[i].pSRV);
	CreateEmpty3DTexture(FLUID_SIZE + 2, &m_Vlc_s.pTex3D, &m_Vlc_s.pSRV);
	CreateEmpty3DTexture(FLUID_SIZE + 2, &m_Vlc_bs.pTex3D, &m_Vlc_bs.pSRV);

	// ストラクチャードバッファ
	ZeroMemory(&bdesc, sizeof(bdesc));
	bdesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	bdesc.ByteWidth = sizeof(SBUFFER_ELEMENT)*(FLUID_SIZE + 2)*(FLUID_SIZE + 2)*(FLUID_SIZE + 2);
	bdesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	bdesc.StructureByteStride = sizeof(SBUFFER_ELEMENT);
	GameDevice()->m_pD3D->m_pDevice->CreateBuffer(&bdesc, NULL, &m_Prs.pStredBuf);
	GameDevice()->m_pD3D->m_pDevice->CreateBuffer(&bdesc, NULL, &m_Div.pStredBuf);

	// ストラクチャードバッファのUAV
	ZeroMemory(&udesc, sizeof(udesc));
	udesc.Format = DXGI_FORMAT_UNKNOWN;
	udesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	udesc.Texture2D.MipSlice = 0;
	udesc.Buffer.NumElements = (FLUID_SIZE + 2)*(FLUID_SIZE + 2)*(FLUID_SIZE + 2);
	GameDevice()->m_pD3D->m_pDevice->CreateUnorderedAccessView(m_Prs.pStredBuf, &udesc, &m_Prs.pUAV);
	GameDevice()->m_pD3D->m_pDevice->CreateUnorderedAccessView(m_Div.pStredBuf, &udesc, &m_Div.pUAV);

	// 3DテクスチャーのUAV
	ZeroMemory(&udesc, sizeof(udesc));
	udesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	udesc.Texture3D.FirstWSlice = 0;
	udesc.Texture3D.WSize = FLUID_SIZE;
	udesc.Texture3D.MipSlice = 0;
	udesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE3D;

	GameDevice()->m_pD3D->m_pDevice->CreateUnorderedAccessView(m_Dns_Tex.pTex3D, &udesc, &m_Dns_Tex.pUAV);
	for (int i = 0; i<2; i++) GameDevice()->m_pD3D->m_pDevice->CreateUnorderedAccessView(m_Dns[i].pTex3D, &udesc, &m_Dns[i].pUAV);
	for (int i = 0; i<2; i++) GameDevice()->m_pD3D->m_pDevice->CreateUnorderedAccessView(m_Vlc[i].pTex3D, &udesc, &m_Vlc[i].pUAV);
	GameDevice()->m_pD3D->m_pDevice->CreateUnorderedAccessView(m_Vlc_s.pTex3D, &udesc, &m_Vlc_s.pUAV);
	GameDevice()->m_pD3D->m_pDevice->CreateUnorderedAccessView(m_Vlc_bs.pTex3D, &udesc, &m_Vlc_bs.pUAV);

}
// ---------------------------------------------------------------------------
//
//  3Dテクスチャーの生成関数
//
//  int Size                         : 一辺のサイズ
//  ID3D11Texture3D** ppTexture      : 作成された3Dテクスチャー(OUT)
//  ID3D11ShaderResourceView** ppSRV : 作成されたシェーダーリソースビュー(OUT)
//
// ---------------------------------------------------------------------------
HRESULT Fluid::CreateEmpty3DTexture(int Size, ID3D11Texture3D** ppTexture, ID3D11ShaderResourceView** ppSRV)
{
	D3D11_TEXTURE3D_DESC descTex;
	ZeroMemory(&descTex, sizeof(descTex));
	descTex.Height = Size;
	descTex.Width = Size;
	descTex.Depth = Size;
	descTex.MipLevels = 1;
	descTex.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	descTex.Usage = D3D11_USAGE_DEFAULT;
	descTex.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;

	GameDevice()->m_pD3D->m_pDevice->CreateTexture3D(&descTex, NULL, ppTexture);
	GameDevice()->m_pD3D->m_pDevice->CreateShaderResourceView(*ppTexture, NULL, ppSRV);

	return S_OK;
}

// ---------------------------------------------------------------------------
//
// 流体の発生変数を設定する
//
//   const Transform& trans  : ワールドマトリックス
//   const float& fCubeSize   :	表示用立方体の一辺の大きさ
//   const float& fColorNo    :	流体の色コード（0:炎（黒煙なし）1: 炎（黒煙あり）2:青い炎 3:濃い煙 4:薄い煙）
//   const VECTOR4& vVoxelPos :	設定ボクセル位置
//   const VECTOR4& vDensity  :	密度(密度はxのみ使用する。yzwは使用していない)
//   const VECTOR4& vVelocity :	速度
//   const float&   fRandom   :	速度方向ランダム率(0.0f:一定方向 ～ 1.0f:方向がランダム)
//   const int& nAddVoxelWidth:	複数箇所から発生させるときのサイズ（ボクセル数）。１カ所の場合は０にする。
// ---------------------------------------------------------------------------
void Fluid::Spawn(const Transform& trans, const float& fCubeSize, const float& fColorNo, const VECTOR4& vVoxelPos, const VECTOR4& vDensity, const VECTOR4& vVelocity, const float& fRandom, const int& nAddVoxelWidth)
{
	if (m_bAddFlag)	return;					   // ダブって注入はしない

	m_bAddFlag     = true;                     // 密度と速度が注入された
	transform      = trans;
	m_vOption.x    = fCubeSize;
	m_vOption.y    = fColorNo;
	m_vAddVoxelPos = vVoxelPos;
	m_vAddDensity  = vDensity  * FLUID_TIME_STEP;
	m_vAddVelocity = vVelocity * FLUID_TIME_STEP;
	m_vOption.w    = fRandom;
	m_nAddVoxelWidth = nAddVoxelWidth;
}
// ---------------------------------------------------------------------------
//
// コンピュートシェーダのUAVとSRVを初期化する
//
// ---------------------------------------------------------------------------
void Fluid::Unbind()
{
	ID3D11UnorderedAccessView* ppUAV_NULL[1]={NULL};
	ID3D11ShaderResourceView* ppSRV_NULL[1]={NULL};
	for(int i=0; i<8; i++)
	{
		GameDevice()->m_pD3D->m_pDeviceContext->CSSetUnorderedAccessViews(i,1,ppUAV_NULL,0);
		GameDevice()->m_pD3D->m_pDeviceContext->CSSetShaderResources(i,1,ppSRV_NULL);
	}
}

// ---------------------------------------------------------------------------
//
//  流体を発生させる(密度と速度を場に注入する)
//
//  ①　コンスタントバッファに密度、速度を設定する
//  ②　流体の発生処理を実行する
//
//  VECTOR3 vOffset  : 位置のオフセット
//
// ---------------------------------------------------------------------------
void Fluid::AddSource(VECTOR3 vOffset)
{
	// コンスタントバッファに密度、速度を設定する
	D3D11_MAPPED_SUBRESOURCE pData;
	CONSTANT_BUFFER_FLUID cb = {};
	GameDevice()->m_pD3D->m_pDeviceContext->Map(GameDevice()->m_pShader->m_pConstantBufferFluid, 0, D3D11_MAP_WRITE_DISCARD, 0, &pData);
	cb.vAddVoxelPos = m_vAddVoxelPos + vOffset;
	cb.vAddDensity = m_vAddDensity;
	cb.vAddVelocity = m_vAddVelocity;
	cb.vOption = m_vOption;       // オプション(x:表示立方体の大きさ　y:色コード　z:なし　w:発生ランダム率)
	memcpy_s(pData.pData, pData.RowPitch, (void*)(&cb), sizeof(cb));
	GameDevice()->m_pD3D->m_pDeviceContext->Unmap(GameDevice()->m_pShader->m_pConstantBufferFluid, 0);

	GameDevice()->m_pD3D->m_pDeviceContext->CSSetShaderResources(0, 1, &m_Dns[1].pSRV);
	GameDevice()->m_pD3D->m_pDeviceContext->CSSetUnorderedAccessViews(0, 1, &m_Dns[0].pUAV, 0);
	GameDevice()->m_pD3D->m_pDeviceContext->CSSetShaderResources(1, 1, &m_Vlc[0].pSRV);
	GameDevice()->m_pD3D->m_pDeviceContext->CSSetUnorderedAccessViews(1, 1, &m_Vlc[1].pUAV, 0);

	GameDevice()->m_pD3D->m_pDeviceContext->CSSetShader(GameDevice()->m_pShader->m_pFluidSolver_AddSource, NULL, 0);  // AddSourceを呼び出す
	GameDevice()->m_pD3D->m_pDeviceContext->CSSetConstantBuffers(0, 1, &GameDevice()->m_pShader->m_pConstantBufferFluid);
	GameDevice()->m_pD3D->m_pDeviceContext->Dispatch(1, 1, 1);
	Unbind();
}

// ---------------------------------------------------------------------------
//
//  流体オブジェクトの更新関数(ソルバー)
//
//  (ソルバーとは流体計算モデル等を解くためのプログラムのことである)
//
// ---------------------------------------------------------------------------
void Fluid::Update()
{
	m_updateTimer += 60 * SceneManager::DeltaTime();
	if (m_updateTimer < 0.99f) return;				   // 誤差対応のため1ではなく0.99とする
	m_updateTimer = 0;

	// 密度、速度を注入  ----------------------------------------
	if (m_bAddFlag)  // 密度、速度が注入されたとき
	{
		VECTOR3 vOffset;

		m_keepTimer = KeepTime;	 // 保持時間を設定

		//  流体を発生させる(密度と速度を場に注入する)
		AddSource();

		// 発生させる場所が複数箇所のとき、中心点以外に4カ所から発生させる
		if (m_nAddVoxelWidth > 0)
		{
			// Ｙ方向発生かＺ方向発生か
			if (m_vAddVoxelPos.y >= FLUID_SIZE / 2 - 1)
			{
				vOffset = VECTOR3(1, 1, 0);  // Z方向発生
			}
			else {
				vOffset = VECTOR3(1, 0, 1);  // Y方向発生
			}
			for (int i = 0; i < 4; i++)  // 4カ所から発生
			{
				//  流体を発生させる
				AddSource( VECTOR3( vOffset.x * (i%2*2-1) * m_nAddVoxelWidth, 
									vOffset.y * (i/2*2-1) * m_nAddVoxelWidth,
									vOffset.z * (i/2*2-1) * m_nAddVoxelWidth));
			}
		}
	}

	// 速度ソルバー  ----------------------------------------------
	
	// SD_RESOURCEを入れ替える
	FLUID_SWAP_SDRESOURCE(m_Vlc[0],m_Vlc[1]);

	// Project1
	GameDevice()->m_pD3D->m_pDeviceContext->CSSetUnorderedAccessViews(4, 1, &m_Div.pUAV, 0);
	GameDevice()->m_pD3D->m_pDeviceContext->CSSetUnorderedAccessViews(3, 1, &m_Prs.pUAV, 0);
	GameDevice()->m_pD3D->m_pDeviceContext->CSSetShaderResources(1,1,&m_Vlc[0].pSRV);
	GameDevice()->m_pD3D->m_pDeviceContext->CSSetShader(GameDevice()->m_pShader->m_pFluidSolver_Project1,NULL,0);
	GameDevice()->m_pD3D->m_pDeviceContext->Dispatch(FLUID_RSIZE, FLUID_RSIZE, FLUID_RSIZE);
	Unbind();

	// project2
	GameDevice()->m_pD3D->m_pDeviceContext->CSSetUnorderedAccessViews(3, 1, &m_Prs.pUAV, 0);
	GameDevice()->m_pD3D->m_pDeviceContext->CSSetUnorderedAccessViews(4, 1, &m_Div.pUAV, 0);
	GameDevice()->m_pD3D->m_pDeviceContext->CSSetShader(GameDevice()->m_pShader->m_pFluidSolver_Project2, NULL, 0);
	for (int i = 0; i < 5;i++)  GameDevice()->m_pD3D->m_pDeviceContext->Dispatch(FLUID_RSIZE, FLUID_RSIZE, FLUID_RSIZE);
	Unbind();

	// project3
	GameDevice()->m_pD3D->m_pDeviceContext->CSSetShaderResources(1,1,&m_Vlc[0].pSRV);
	GameDevice()->m_pD3D->m_pDeviceContext->CSSetUnorderedAccessViews(1,1,&m_Vlc[1].pUAV,0);
	GameDevice()->m_pD3D->m_pDeviceContext->CSSetUnorderedAccessViews(3, 1, &m_Prs.pUAV, 0);
	GameDevice()->m_pD3D->m_pDeviceContext->CSSetShader(GameDevice()->m_pShader->m_pFluidSolver_Project3, NULL, 0);
	GameDevice()->m_pD3D->m_pDeviceContext->Dispatch(FLUID_RSIZE, FLUID_RSIZE, FLUID_RSIZE);
	Unbind();

	// SD_RESOURCEを入れ替える
	FLUID_SWAP_SDRESOURCE(m_Vlc[0],m_Vlc[1]);

	// boundary
	GameDevice()->m_pD3D->m_pDeviceContext->CSSetShaderResources(1,1,&m_Vlc[0].pSRV);
	GameDevice()->m_pD3D->m_pDeviceContext->CSSetUnorderedAccessViews(1,1,&m_Vlc[1].pUAV,0);
	GameDevice()->m_pD3D->m_pDeviceContext->CSSetShader(GameDevice()->m_pShader->m_pFluidSolver_Boundary1, NULL, 0);
	GameDevice()->m_pD3D->m_pDeviceContext->Dispatch(FLUID_RSIZE, FLUID_RSIZE, 1);
	Unbind();

	// AdvectVelocity
	GameDevice()->m_pD3D->m_pDeviceContext->CSSetShaderResources(1,1,&m_Vlc[0].pSRV);
	GameDevice()->m_pD3D->m_pDeviceContext->CSSetUnorderedAccessViews(2,1,&m_Vlc_s.pUAV,0);  // MacCormackを使用するとき
	//GameDevice()->m_pD3D->m_pDeviceContext->CSSetUnorderedAccessViews(1,1,&m_Vlc[1].pUAV,0);  // MacCormackを使用しないとき(FluidSolver.hlsliも変更すること)
	GameDevice()->m_pD3D->m_pDeviceContext->CSSetShader(GameDevice()->m_pShader->m_pFluidSolver_AdvectVelocity, NULL, 0);
	GameDevice()->m_pD3D->m_pDeviceContext->Dispatch(FLUID_RSIZE, FLUID_RSIZE, FLUID_RSIZE);
	Unbind();

	
	// Advect Back(MacCormackを使用するとき)
	GameDevice()->m_pD3D->m_pDeviceContext->CSSetShaderResources(1,1, &m_Vlc[0].pSRV);
	GameDevice()->m_pD3D->m_pDeviceContext->CSSetShaderResources(2,1, &m_Vlc_s.pSRV);
	GameDevice()->m_pD3D->m_pDeviceContext->CSSetUnorderedAccessViews(3, 1, &m_Vlc_bs.pUAV, 0);
	GameDevice()->m_pD3D->m_pDeviceContext->CSSetShader(GameDevice()->m_pShader->m_pFluidSolver_AdvectBack, NULL, 0);
	GameDevice()->m_pD3D->m_pDeviceContext->Dispatch(FLUID_RSIZE, FLUID_RSIZE, FLUID_RSIZE);
	Unbind();

	// MacCormack(MacCormackを使用するとき)
	GameDevice()->m_pD3D->m_pDeviceContext->CSSetShaderResources(0,1, &m_Dns[0].pSRV);
	GameDevice()->m_pD3D->m_pDeviceContext->CSSetShaderResources(1,1, &m_Vlc[0].pSRV);
	GameDevice()->m_pD3D->m_pDeviceContext->CSSetUnorderedAccessViews(1, 1, &m_Vlc[1].pUAV, 0);
	GameDevice()->m_pD3D->m_pDeviceContext->CSSetShaderResources(2,1, &m_Vlc_s.pSRV);
	GameDevice()->m_pD3D->m_pDeviceContext->CSSetShaderResources(3,1, &m_Vlc_bs.pSRV);
	GameDevice()->m_pD3D->m_pDeviceContext->CSSetShader(GameDevice()->m_pShader->m_pFluidSolver_MacCormack, NULL, 0);
	GameDevice()->m_pD3D->m_pDeviceContext->Dispatch(FLUID_RSIZE, FLUID_RSIZE, FLUID_RSIZE);
	Unbind();
	

	// 密度ソルバー  ------------------------------------------
	// AdvectDensity
	GameDevice()->m_pD3D->m_pDeviceContext->CSSetShaderResources(1,1,&m_Vlc[0].pSRV);
	GameDevice()->m_pD3D->m_pDeviceContext->CSSetUnorderedAccessViews(5, 1, &m_Dns_Tex.pUAV, 0);	
	GameDevice()->m_pD3D->m_pDeviceContext->CSSetShaderResources(0,1,&m_Dns[0].pSRV);
	GameDevice()->m_pD3D->m_pDeviceContext->CSSetUnorderedAccessViews(0,1,&m_Dns[1].pUAV,0);

	// SD_RESOURCEを入れ替える
	FLUID_SWAP_SDRESOURCE(m_Dns[0],m_Dns[1]);

	GameDevice()->m_pD3D->m_pDeviceContext->CSSetShader(GameDevice()->m_pShader->m_pFluidSolver_AdvectDensity, NULL, 0);
	GameDevice()->m_pD3D->m_pDeviceContext->Dispatch(FLUID_RSIZE, FLUID_RSIZE, FLUID_RSIZE);
	Unbind();


	// シェーダーリセット処理 ---------------------------------
	GameDevice()->m_pD3D->m_pDeviceContext->CSSetShader(NULL,NULL,0);

	m_keepTimer--;
	if (m_bAddFlag == false && m_keepTimer <= 0)
	{

		DestroyMe();  // 注入フラグがセットされなくて保持時間が過ぎたときは、削除する

		// ここに、リソースのクリヤー処理を入れる

	}
	m_bAddFlag = false;  // 注入フラグをリセットする

}

//------------------------------------------------------------------------
//
//	流体を画面にレンダリングする処理
//
// 引数
//      const MATRIX4X4& mWorld               ワールドマトリックス
//      const VECTOR4X4& vOption              オプション(xメンバに表示用立方体のサイズが入っている)
//      const MATRIX4X4& mProj                プロジェクションマトリックス
//      const VECTOR3&   vLight               光源ベクトル
//      const VECTOR3&   vEye                 視点ベクトル(使っていない)
//
//	戻り値 HRESULT
//		S_OK	= 正常
//		E_FAIL	= 異常
//
//------------------------------------------------------------------------
void Fluid::DrawDepth()
{
}
void Fluid::DrawScreen()
{
	// 表示位置の調整
	// 流体の発生場所を原点mWorldStartとする
	VECTOR4   vOffset     = m_vAddVoxelPos / FLUID_SIZE * m_vOption.x;
	MATRIX4X4 mWorldStart = XMMatrixTranslation( -vOffset.x, -vOffset.y, -vOffset.z) * transform.matrix();

	//	流体を画面に描画する処理
	ObjectManager::FindGameObject<FluidManager>()->Draw(mWorldStart, m_vOption, &m_Dns_Tex.pSRV, GameDevice()->m_mView, GameDevice()->m_mProj, GameDevice()->m_vLightDir, GameDevice()->m_vEyePt);
}

