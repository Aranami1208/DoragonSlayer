// ========================================================================================
//
// シェーダーの処理                                               ver 3.0        2021.1.11
//
//   シェーダーグラムの読み込みとシェーダーの作成
//   インプットレイアウトの作成
//   コンスタントバッファの作成
//   フォグの処理・影の処理
//
//
//    登録されているシェーダー
//
//		・シンプルなシェーダー                          InitShaderSimple();
//		・スプライト用のシェーダー                      InitShaderSprite();
//		・FBXStaticMesh/FBXSkinMesh用のシェーダー       InitShaderFbx();         Cascade ShadoeMap対応
//		・ディスプレイスメントマッピング用のシェーダー  InitShaderDisplace();    Cascade ShadoeMap対応
//		・エフェクト用のシェーダー                      InitShaderEffect();
//		・HDR用のシェーダー                             InitShaderHDR();
//		・Fluid用のシェーダー                           InitShaderFluid();
//
//                                                                              Shader.cpp
// ========================================================================================

#include "Shader.h"
#include "FbxMesh.h"

//------------------------------------------------------------------------
//
//	シェーダーのコンストラクタ	
//
//  引数　CDirect3D* pD3D
//
//------------------------------------------------------------------------
CShader::CShader(CDirect3D* pD3D)
{
	ZeroMemory(this, sizeof(CShader));
	m_pD3D = pD3D;
}
//------------------------------------------------------------------------
//
//	シェーダーのデストラクタ	
//
//------------------------------------------------------------------------
CShader::~CShader()
{
	// シェーダーの解放  ------------------------------------------

	// 通常用のシンプルなシェーダー
	SAFE_RELEASE(m_pSimple_VertexLayout); 
	SAFE_RELEASE(m_pSimple_VS);
	SAFE_RELEASE(m_pSimple_PS);

	// 3Dスプライト用のシェーダー
	SAFE_RELEASE(m_pSprite3D_VertexLayout);
	SAFE_RELEASE(m_pSprite3D_VS);
	SAFE_RELEASE(m_pSprite3D_PS);
	SAFE_RELEASE(m_pSprite3D_VS_BILL);
	SAFE_RELEASE(m_pSprite3D_PS_ENV);

	// ディスプレースメントマッピング(波)用のシェーダー
	SAFE_RELEASE(m_pDisplaceWave_VertexLayout);
	SAFE_RELEASE(m_pDisplaceWave_VS);
	SAFE_RELEASE(m_pDisplaceWave_HS);
	SAFE_RELEASE(m_pDisplaceWave_DS);
	SAFE_RELEASE(m_pDisplaceWave_PS);

	// ディスプレースメントマッピング(スキンメッシュ)Shadow 用のシェーダー
	SAFE_RELEASE(m_pDisplaceSkinShadowMap_VS);
	SAFE_RELEASE(m_pDisplaceSkinShadowMap_HS);
	SAFE_RELEASE(m_pDisplaceSkinShadowMap_DS);
	SAFE_RELEASE(m_pDisplaceSkinShadowMap_PS);
	SAFE_RELEASE(m_pDisplaceSkinShadowMap_DSDepth);
	SAFE_RELEASE(m_pDisplaceSkinShadowMap_PSDepth);

	// ディスプレースメントマッピング(スタティックメッシュ)Shadow 用のシェーダー 
	SAFE_RELEASE(m_pDisplaceStaticShadowMap_VS);
	SAFE_RELEASE(m_pDisplaceStaticShadowMap_HS);
	SAFE_RELEASE(m_pDisplaceStaticShadowMap_DS);
	SAFE_RELEASE(m_pDisplaceStaticShadowMap_PS);
	SAFE_RELEASE(m_pDisplaceStaticShadowMap_DSDepth);
	SAFE_RELEASE(m_pDisplaceStaticShadowMap_PSDepth);

	// エフェクト用のシェーダー
	SAFE_RELEASE(m_pEffect3D_VertexLayout);
	SAFE_RELEASE(m_pEffect3D_VS_POINT);
	SAFE_RELEASE(m_pEffect3D_GS_POINT);
	SAFE_RELEASE(m_pEffect3D_PS);
	SAFE_RELEASE(m_pEffect3D_VertexLayout_BILL);
	SAFE_RELEASE(m_pEffect3D_VS_BILL);
	SAFE_RELEASE(m_pEffect3D_VS_BILLMESH);              // -- 2019.7.17

	// Fbxモデル　スタティツクメッシュShadowMap用		// -- 2018.7.28
	SAFE_RELEASE(m_pFbxStaticShadowMap_VertexLayout);
	SAFE_RELEASE(m_pFbxStaticShadowMap_VSDepth);
	SAFE_RELEASE(m_pFbxStaticShadowMap_PSDepth);
	SAFE_RELEASE(m_pFbxStaticShadowMap_VS);
	SAFE_RELEASE(m_pFbxStaticShadowMap_PS);

	// Fbxモデル　スキンメッシュ ShadowMap用		// -- 2018.7.28
	SAFE_RELEASE(m_pFbxSkinShadowMap_VertexLayout);
	SAFE_RELEASE(m_pFbxSkinShadowMap_VSDepth);
	SAFE_RELEASE(m_pFbxSkinShadowMap_PSDepth);
	SAFE_RELEASE(m_pFbxSkinShadowMap_VS);
	SAFE_RELEASE(m_pFbxSkinShadowMap_PS);

	// HDR用のシェーダー                       // -- 2019.4.19
	SAFE_RELEASE(m_pHDR_VertexLayout);
	SAFE_RELEASE(m_pHDR_VS);
	SAFE_RELEASE(m_pHDR_PS);
	SAFE_RELEASE(m_pHDR_PS_BRIGHT);
	SAFE_RELEASE(m_pHDR_PS_BLOOM);

	// 流体計算用のシェーダー
	SAFE_RELEASE(m_pFluidSolver_AddSource);
	SAFE_RELEASE(m_pFluidSolver_AdvectBack);
	SAFE_RELEASE(m_pFluidSolver_AdvectDensity);
	SAFE_RELEASE(m_pFluidSolver_AdvectVelocity);
	SAFE_RELEASE(m_pFluidSolver_Boundary1);
	SAFE_RELEASE(m_pFluidSolver_MacCormack);
	SAFE_RELEASE(m_pFluidSolver_Project1);
	SAFE_RELEASE(m_pFluidSolver_Project2);
	SAFE_RELEASE(m_pFluidSolver_Project3);
	SAFE_RELEASE(m_pFluidRender_VertexLayout);
	SAFE_RELEASE(m_pFluidRender_VS);
	SAFE_RELEASE(m_pFluidRender_PS);
	SAFE_RELEASE(m_pFluidTexture_VertexLayout);
	SAFE_RELEASE(m_pFluidTexture_VS);
	SAFE_RELEASE(m_pFluidTexture_PS);


	// コンスタントバッファの解放 ---------------------------

	SAFE_RELEASE(m_pConstantBufferFog);

	SAFE_RELEASE(m_pConstantBufferDisplace);
	SAFE_RELEASE(m_pConstantBufferEffect);
	SAFE_RELEASE(m_pConstantBufferSprite3D);

	SAFE_RELEASE(m_pConstantBufferWVLED);
	SAFE_RELEASE(m_pConstantBufferBone2);
	SAFE_RELEASE(m_pConstantBufferMaterial);      // -- 2020.12.15
	SAFE_RELEASE(m_pConstantBufferWVLLED);

	SAFE_RELEASE(m_pConstantBufferHDR);
	SAFE_RELEASE(m_pConstantBufferHDRBloom);

	SAFE_RELEASE(m_pConstantBufferFluid);  // 流体計算用

}

//------------------------------------------------------------------------
//
//	各種シェーダーの作成	
//
//  ・シェーダーとコンスタントバッファを作成する
//  ・テクスチャーサンプラーとブレンドステートを作成する
//
//  引数　なし
//
//	戻り値 HRESULT
//		S_OK	= 正常
//		E_FAIL	= 異常
//
//------------------------------------------------------------------------
HRESULT CShader::InitShader()
{
	//  通常メッシュ用のシェーダー作成
	InitShaderSimple();

	//  Fbxスタティック・スキンメッシュ用のシェーダー作成
	InitShaderFbx();

	//  ディスプレースメントマッピング用のシェーダー作成
	InitShaderDisplace();

	//  エフェクト用のシェーダー作成
	InitShaderEffect();

	//  スプライト用のシェーダー作成
	InitShaderSprite();

	//  HDR用のシェーダー作成
	InitShaderHDR();

	//  流体計算用のシェーダー作成
	InitShaderFluid();

	//  コンスタントバッファ作成
	InitShaderConstant();

	return S_OK;
}
//------------------------------------------------------------------------
//
//  通常用(Simple Shader)のシェーダー作成
//
//  引数　なし
//
//	戻り値 HRESULT
//		S_OK	= 正常
//		E_FAIL	= 異常
//
//------------------------------------------------------------------------
HRESULT CShader::InitShaderSimple()
{

	// 頂点インプットレイアウトを定義
	UINT numElements = 0;
	D3D11_INPUT_ELEMENT_DESC Simplelayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	numElements = sizeof(Simplelayout) / sizeof(Simplelayout[0]);

	// バーテックスシェーダ・ピクセルシェーダ・頂点インプットレイアウトの作成
	MakeShader(_T("VS"), _T("Simple_VS.cso"), (void**)&m_pSimple_VS, Simplelayout, numElements, &m_pSimple_VertexLayout);
	MakeShader(_T("PS"), _T("Simple_PS.cso"), (void**)&m_pSimple_PS);

	return S_OK;
}

//------------------------------------------------------------------------
//
//  Fbxモデル　スタティツク＆スキンメッシュ用のシェーダー作成
//
//  引数　なし
//
//	戻り値 HRESULT
//		S_OK	= 正常
//		E_FAIL	= 異常
//
//------------------------------------------------------------------------
HRESULT CShader::InitShaderFbx()
{

	// -------------------------------------------------------------------
	// 
	// スタティックメッシュ  FbxStaticMesh
	// 
	// -------------------------------------------------------------------
	// 頂点インプットレイアウトを定義
	UINT numElements = 0;
	D3D11_INPUT_ELEMENT_DESC FbxStaticNM_layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },		// 計32byte
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT,0,32, D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT,0,44, D3D11_INPUT_PER_VERTEX_DATA,0 }, // 計56byte
	};
	numElements = sizeof(FbxStaticNM_layout) / sizeof(FbxStaticNM_layout[0]);

	// --------------------------------------------------------------------------  // -- 2018.7.28
	//
	//	スタティックメッシュ　シャドウマップのシェーダー
	//
	// -------------------------------------------------------------------------- 
	// --------------------------------------------------------------------- 
	// ①　FbxStaticShadow
	// ---------------------------------------------------------------------

	MakeShader(_T("VS"), _T("FbxStaticShadowMap_VS.cso"), (void**)&m_pFbxStaticShadowMap_VS, FbxStaticNM_layout, numElements, &m_pFbxStaticShadowMap_VertexLayout);
	MakeShader(_T("PS"), _T("FbxStaticShadowMap_PS.cso"), (void**)&m_pFbxStaticShadowMap_PS);

	// --------------------------------------------------------------------- // -- 2018.7.28
	// ②　FbxStaticShadowDepth
	// ---------------------------------------------------------------------

	MakeShader(_T("VS"), _T("FbxStaticShadowMap_VSDepth.cso"), (void**)&m_pFbxStaticShadowMap_VSDepth);
	MakeShader(_T("PS"), _T("FbxStaticShadowMap_PSDepth.cso"), (void**)&m_pFbxStaticShadowMap_PSDepth);


	// -------------------------------------------------------------------
	// 
	// スキンメッシュ  FbxSkinMesh
	// 
	// -------------------------------------------------------------------
	// 頂点インプットレイアウトを定義
	numElements = 0;
	D3D11_INPUT_ELEMENT_DESC FbxSkinNM_layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BONE_INDEX", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BONE_WEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 64, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 76, D3D11_INPUT_PER_VERTEX_DATA, 0 }, // 計76Byte
	};
	numElements = sizeof(FbxSkinNM_layout) / sizeof(FbxSkinNM_layout[0]);


	// --------------------------------------------------------------------------  // -- 2018.7.28
	//
	//	スキンメッシュ　シャドウマップのシェーダー
	//
	// -------------------------------------------------------------------------- 

	// ------------------------------------------------  // -- 2018.7.28
	// ①　FbxSkinShadow
	// ------------------------------------------------
	MakeShader(_T("VS"), _T("FbxSkinShadowMap_VS.cso"), (void**)&m_pFbxSkinShadowMap_VS, FbxSkinNM_layout, numElements, &m_pFbxSkinShadowMap_VertexLayout);
	MakeShader(_T("PS"), _T("FbxSkinShadowMap_PS.cso"), (void**)&m_pFbxSkinShadowMap_PS);

	// ---------------------------------------------------------  // -- 2018.7.28
	// ②　FbxSkinShadowDepth
	// ---------------------------------------------------------  
	MakeShader(_T("VS"), _T("FbxSkinShadowMap_VSDepth.cso"), (void**)&m_pFbxSkinShadowMap_VSDepth);
	MakeShader(_T("PS"), _T("FbxSkinShadowMap_PSDepth.cso"), (void**)&m_pFbxSkinShadowMap_PSDepth);


	return S_OK;
}
//------------------------------------------------------------------------
//
//  ディスプレースメントマッピング用のシェーダー作成
//
//  引数　なし
//
//	戻り値 HRESULT
//		S_OK	= 正常
//		E_FAIL	= 異常
//
//------------------------------------------------------------------------
HRESULT CShader::InitShaderDisplace()
{

	// -----------------------------------------------------------------------------------------------
	// 波のディスプレイスマッピング DisplaceWave
	// -----------------------------------------------------------------------------------------------
	// 頂点インプットレイアウトを定義
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = sizeof(layout) / sizeof(layout[0]);

	// バーテックスシェーダ・ピクセルシェーダ・頂点インプットレイアウトの作成
	MakeShader(_T("VS"), _T("DisplaceWave_VS.cso"), (void**)&m_pDisplaceWave_VS, layout, numElements, &m_pDisplaceWave_VertexLayout);
	MakeShader(_T("HS"), _T("DisplaceWave_HS.cso"), (void**)&m_pDisplaceWave_HS);
	MakeShader(_T("DS"), _T("DisplaceWave_DS.cso"), (void**)&m_pDisplaceWave_DS);
	MakeShader(_T("PS"), _T("DisplaceWave_PS.cso"), (void**)&m_pDisplaceWave_PS);


	// -----------------------------------------------------------------------------------------------
	//
	// スタティックメッシュのディスプレイスメントマッピング DisplaceStaticMesh
	//
	// -----------------------------------------------------------------------------------------------

	// -----------------------------------------------------------------------------------------------
	//
	// スタティックメッシュのディスプレイスマッピング  ShadowMap用   DisplaceStaticShadowMap
	//
	// -----------------------------------------------------------------------------------------------
	// ---------------------------------------------------------------------------
	// ①　ShadowMap用
	// ---------------------------------------------------------------------------
	// バーテックスシェーダ・ピクセルシェーダ・ハルシェーダ・ドメインシェーダの作成
	//  !!!!! 頂点インプットレイアウトは、スタティックメッシュのレイアウトを使用	
	MakeShader(_T("VS"), _T("DisplaceStaticShadowMap_VS.cso"), (void**)&m_pDisplaceStaticShadowMap_VS);
	MakeShader(_T("HS"), _T("DisplaceStaticShadowMap_HS.cso"), (void**)&m_pDisplaceStaticShadowMap_HS);
	MakeShader(_T("DS"), _T("DisplaceStaticShadowMap_DS.cso"), (void**)&m_pDisplaceStaticShadowMap_DS);
	MakeShader(_T("PS"), _T("DisplaceStaticShadowMap_PS.cso"), (void**)&m_pDisplaceStaticShadowMap_PS);

	// ---------------------------------------------------------------------------
	// ②　ShadowDepth用
	// ---------------------------------------------------------------------------
	MakeShader(_T("DS"), _T("DisplaceStaticShadowMap_DSDepth.cso"), (void**)&m_pDisplaceStaticShadowMap_DSDepth);
	MakeShader(_T("PS"), _T("DisplaceStaticShadowMap_PSDepth.cso"), (void**)&m_pDisplaceStaticShadowMap_PSDepth);


	// -----------------------------------------------------------------------------------------------
	//
	// スキンメッシュのディスプレイスマッピング用
	//
	// -----------------------------------------------------------------------------------------------

	// -----------------------------------------------------------------------------------------------
	//
	// スキンメッシュのディスプレイスマッピング  ShadowMap用   DisplaceSkinShadowMap
	//
	// -----------------------------------------------------------------------------------------------
	// -----------------------------------------------------------------------------------------------
	// ①　ShadowMap用
	// -----------------------------------------------------------------------------------------------
	MakeShader(_T("VS"), _T("DisplaceSkinShadowMap_VS.cso"), (void**)&m_pDisplaceSkinShadowMap_VS);
	MakeShader(_T("HS"), _T("DisplaceSkinShadowMap_HS.cso"), (void**)&m_pDisplaceSkinShadowMap_HS);
	MakeShader(_T("DS"), _T("DisplaceSkinShadowMap_DS.cso"), (void**)&m_pDisplaceSkinShadowMap_DS);
	MakeShader(_T("PS"), _T("DisplaceSkinShadowMap_PS.cso"), (void**)&m_pDisplaceSkinShadowMap_PS);

	// -----------------------------------------------------------------------------------------------
	// ②　ShadowDepth用
	// -----------------------------------------------------------------------------------------------
	MakeShader(_T("DS"), _T("DisplaceSkinShadowMap_DSDepth.cso"), (void**)&m_pDisplaceSkinShadowMap_DSDepth);
	MakeShader(_T("PS"), _T("DisplaceSkinShadowMap_PSDepth.cso"), (void**)&m_pDisplaceSkinShadowMap_PSDepth);

	return S_OK;
}

//------------------------------------------------------------------------
//
//  エフェクト用のシェーダー作成
//
//  引数　なし
//
//	戻り値 HRESULT
//		S_OK	= 正常
//		E_FAIL	= 異常
//
//------------------------------------------------------------------------
HRESULT CShader::InitShaderEffect()
{

	// -------------------------------------------------------------------
	// 
	// パーティクルのシェーダー
	// 
	// -------------------------------------------------------------------
	// 頂点インプットレイアウトを定義
	UINT numElements = 0;
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	numElements = sizeof(layout) / sizeof(layout[0]);

	// バーテックスシェーダ・ジオメトリシェーダ・頂点インプットレイアウトの作成
	MakeShader(_T("VS"), _T("Effect3D_VS_POINT.cso"), (void**)&m_pEffect3D_VS_POINT, layout, numElements, &m_pEffect3D_VertexLayout);
	MakeShader(_T("GS"), _T("Effect3D_GS_POINT.cso"), (void**)&m_pEffect3D_GS_POINT);


	// -----------------------------------------------------------------------------------------------------
	// 
	// ビルボードのシェーダー
	// 
	// -----------------------------------------------------------------------------------------------------
	//頂点インプットレイアウトを定義
	D3D11_INPUT_ELEMENT_DESC layoutbill[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	numElements = sizeof(layoutbill) / sizeof(layoutbill[0]);

	// バーテックスシェーダ・頂点インプットレイアウトの作成
	MakeShader(_T("VS"), _T("Effect3D_VS_BILL.cso"), (void**)&m_pEffect3D_VS_BILL, layoutbill, numElements, &m_pEffect3D_VertexLayout_BILL);

	// -----------------------------------------------------------------------------------------------------
	// 
	// ビルボードメッシュのシェーダー
	// 
	// -----------------------------------------------------------------------------------------------------
	// バーテックスシェーダの作成
	MakeShader(_T("VS"), _T("Effect3D_VS_BILLMESH.cso"), (void**)&m_pEffect3D_VS_BILLMESH);

	// -----------------------------------------------------------------------------------------------------
	// 
	// 共通のシェーダー
	// 
	// -----------------------------------------------------------------------------------------------------
	// ピクセルシェーダの作成
	MakeShader(_T("PS"), _T("Effect3D_PS.cso"), (void**)&m_pEffect3D_PS);

	return S_OK;
}

//------------------------------------------------------------------------
//
//  スプライト用のシェーダー作成
//
//  引数　なし
//
//	戻り値 HRESULT
//		S_OK	= 正常
//		E_FAIL	= 異常
//
//------------------------------------------------------------------------
HRESULT CShader::InitShaderSprite()
{
	// 頂点インプットレイアウトを定義	
	D3D11_INPUT_ELEMENT_DESC layout_sprite[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = sizeof(layout_sprite) / sizeof(layout_sprite[0]);

	// バーテックスシェーダ・ピクセルシェーダ・頂点インプットレイアウトの作成
	MakeShader(_T("VS"), _T("Sprite3D_VS.cso"), (void**)&m_pSprite3D_VS, layout_sprite, numElements, &m_pSprite3D_VertexLayout);
	MakeShader(_T("VS"), _T("Sprite3D_VS_BILL.cso"), (void**)&m_pSprite3D_VS_BILL);
	MakeShader(_T("PS"), _T("Sprite3D_PS.cso"), (void**)&m_pSprite3D_PS);

	return S_OK;
}

//------------------------------------------------------------------------ // -- 2019.4.19
//
// HDR用のシェーダー作成
//
//  引数　なし
//
//	戻り値 HRESULT
//		S_OK	= 正常
//		E_FAIL	= 異常
//
//------------------------------------------------------------------------
HRESULT CShader::InitShaderHDR()
{

	// 頂点インプットレイアウトを定義
	UINT numElements = 0;
	D3D11_INPUT_ELEMENT_DESC layout_HDR[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	numElements = sizeof(layout_HDR) / sizeof(layout_HDR[0]);

	// バーテックスシェーダ・ピクセルシェーダ・頂点インプットレイアウトの作成
	MakeShader(_T("VS"), _T("HDR_VS.cso"), (void**)&m_pHDR_VS, layout_HDR, numElements, &m_pHDR_VertexLayout);
	MakeShader(_T("PS"), _T("HDR_PS.cso"), (void**)&m_pHDR_PS);
	MakeShader(_T("PS"), _T("HDR_PS_BRIGHT.cso"), (void**)&m_pHDR_PS_BRIGHT);
	MakeShader(_T("PS"), _T("HDR_PS_BLOOM.cso"), (void**)&m_pHDR_PS_BLOOM);

	return S_OK;

}

//------------------------------------------------------------------------
//
//  流体計算用のシェーダー作成
//
//  引数　なし
//
//	戻り値 HRESULT
//		S_OK	= 正常
//		E_FAIL	= 異常
//
//------------------------------------------------------------------------
HRESULT CShader::InitShaderFluid()
{

	// 頂点インプットレイアウトを定義
	UINT numElements = 0;
	D3D11_INPUT_ELEMENT_DESC Fluid_layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	numElements = sizeof(Fluid_layout) / sizeof(Fluid_layout[0]);

	//
	// 各種シェーダーの作成
	//
	// FluidTexture
	MakeShader(_T("VS"), _T("FluidTexture_VS.cso"), (void**)&m_pFluidTexture_VS, Fluid_layout, numElements, &m_pFluidTexture_VertexLayout);
	MakeShader(_T("PS"), _T("FluidTexture_PS.cso"), (void**)&m_pFluidTexture_PS);

	// FluidRender
	MakeShader(_T("VS"), _T("FluidRender_VS.cso"), (void**)&m_pFluidRender_VS, Fluid_layout, numElements, &m_pFluidRender_VertexLayout);
	MakeShader(_T("PS"), _T("FluidRender_PS.cso"), (void**)&m_pFluidRender_PS);

	// FluidSolver
	MakeShader(_T("CS"), _T("FluidSolver_AddSource.cso"), (void**)&m_pFluidSolver_AddSource);
	MakeShader(_T("CS"), _T("FluidSolver_AdvectBack.cso"), (void**)&m_pFluidSolver_AdvectBack);
	MakeShader(_T("CS"), _T("FluidSolver_AdvectDensity.cso"), (void**)&m_pFluidSolver_AdvectDensity);
	MakeShader(_T("CS"), _T("FluidSolver_AdvectVelocity.cso"), (void**)&m_pFluidSolver_AdvectVelocity);
	MakeShader(_T("CS"), _T("FluidSolver_Boundary1.cso"), (void**)&m_pFluidSolver_Boundary1);
	MakeShader(_T("CS"), _T("FluidSolver_MacCormack.cso"), (void**)&m_pFluidSolver_MacCormack);
	MakeShader(_T("CS"), _T("FluidSolver_Project1.cso"), (void**)&m_pFluidSolver_Project1);
	MakeShader(_T("CS"), _T("FluidSolver_Project2.cso"), (void**)&m_pFluidSolver_Project2);
	MakeShader(_T("CS"), _T("FluidSolver_Project3.cso"), (void**)&m_pFluidSolver_Project3);

	return S_OK;
}

//------------------------------------------------------------------------
//
//  シェーダーの作成関数
//
//  引数	TCHAR ProfileName[]	作成するシェーダー種類
//								(VS,PS,GS,HS,DS,CS)
//			TCHAR FileName[]	ＨＬＳＬファイル名
//			void** ppShader		作成するシェーダー(OUT)
//			D3D11_INPUT_ELEMENT_DESC Fluid_layout[]	頂点レイアウト定義(省略可)
//			UINT numElements						頂点レイアウトエレメント数(省略可)
//			ID3D11InputLayout** ppInputLayout		作成する頂点レイアウト(OUT)(省略可)
//
//	戻り値 HRESULT
//		S_OK	= 正常
//		E_FAIL	= 異常
//
//------------------------------------------------------------------------
HRESULT CShader::MakeShader(const TCHAR ProfileName[], const TCHAR FileName[], void** ppShader, D3D11_INPUT_ELEMENT_DESC Fluid_layout[], UINT numElements, ID3D11InputLayout** ppInputLayout)
{
	// コンパイル済みシェーダの読み込み配列
	BYTE* pCso = nullptr;
	DWORD dwCsoSize = 0;

	// コンパイル済みシェーダの読み込みをする
	m_pD3D->ReadCso(FileName, &pCso, &dwCsoSize);

	// シェーダー種類ごとの作成処理
	if (_tcscmp(ProfileName, _T("VS")) == 0)	// バーテックスシェーダー
	{
		if (FAILED(m_pD3D->m_pDevice->CreateVertexShader(pCso, dwCsoSize, nullptr, (ID3D11VertexShader**)ppShader)))
		{
			SAFE_DELETE_ARRAY(pCso);
			MessageBox(0, _T("バーテックスシェーダー作成失敗"), FileName, MB_OK);
			return E_FAIL;
		}
		if (ppInputLayout)	// 頂点インプットレイアウトを作成するとき
		{
			// 頂点インプットレイアウトを作成
			if (FAILED(m_pD3D->m_pDevice->CreateInputLayout(Fluid_layout, numElements, pCso, dwCsoSize, ppInputLayout)))
			{
				SAFE_DELETE_ARRAY(pCso);
				MessageBox(0, _T("インプット レイアウト作成失敗"), FileName, MB_OK);
				return E_FAIL;
			}
		}
	}
	else if (_tcscmp(ProfileName, _T("PS")) == 0)	// ピクセルシェーダー
	{
		if (FAILED(m_pD3D->m_pDevice->CreatePixelShader(pCso, dwCsoSize, nullptr, (ID3D11PixelShader**)ppShader)))
		{
			SAFE_DELETE_ARRAY(pCso);
			MessageBox(0, _T("ピクセルシェーダー作成失敗"), FileName, MB_OK);
			return E_FAIL;
		}
	}
	else if (_tcscmp(ProfileName, _T("GS")) == 0)	// ジオメトリシェーダー
	{
		if (FAILED(m_pD3D->m_pDevice->CreateGeometryShader(pCso, dwCsoSize, nullptr, (ID3D11GeometryShader**)ppShader)))
		{
			SAFE_DELETE_ARRAY(pCso);
			MessageBox(0, _T("ジオメトリシェーダー作成失敗"), FileName, MB_OK);
			return E_FAIL;
		}
	}
	else if (_tcscmp(ProfileName, _T("HS")) == 0)	// ハルシェーダー
	{
		if (FAILED(m_pD3D->m_pDevice->CreateHullShader(pCso, dwCsoSize, nullptr, (ID3D11HullShader**)ppShader)))
		{
			SAFE_DELETE_ARRAY(pCso);
			MessageBox(0, _T("ハルシェーダー作成失敗"), FileName, MB_OK);
			return E_FAIL;
		}
	}
	else if (_tcscmp(ProfileName, _T("DS")) == 0)	// ドメインシェーダー
	{
		if (FAILED(m_pD3D->m_pDevice->CreateDomainShader(pCso, dwCsoSize, nullptr, (ID3D11DomainShader**)ppShader)))
		{
			SAFE_DELETE_ARRAY(pCso);
			MessageBox(0, _T("ドメインシェーダー作成失敗"), FileName, MB_OK);
			return E_FAIL;
		}
	}
	else if (_tcscmp(ProfileName, _T("CS")) == 0)	// コンピュートシェーダ
	{
		if (FAILED(m_pD3D->m_pDevice->CreateComputeShader(pCso, dwCsoSize, nullptr, (ID3D11ComputeShader**)ppShader)))
		{
			SAFE_DELETE_ARRAY(pCso);
			MessageBox(0, _T("コンピュートシェーダ作成失敗"), FileName, MB_OK);
			return E_FAIL;
		}
	}
	else {
		SAFE_DELETE_ARRAY(pCso);
		MessageBox(0, _T("シェーダ種類指定エラー"), ProfileName, MB_OK);
		return E_FAIL;
	}

	SAFE_DELETE_ARRAY(pCso);
	return S_OK;

}

//------------------------------------------------------------------------
//
//  各種コンスタントバッファー作成
//
//  引数　なし
//
//	戻り値 HRESULT
//		S_OK	= 正常
//		E_FAIL	= 異常
//
//------------------------------------------------------------------------
HRESULT CShader::InitShaderConstant()
{


	// フォグ情報渡し用 コンスタントバッファー作成
	MakeConstantBuffer(sizeof(CONSTANT_BUFFER_FOG), &m_pConstantBufferFog);

	// ディスプレイスメントマッピング用 コンスタントバッファー作成
	MakeConstantBuffer(sizeof(CONSTANT_BUFFER_DISPLACE), &m_pConstantBufferDisplace);

	// エフェクト用 コンスタントバッファー作成
	MakeConstantBuffer(sizeof(CONSTANT_BUFFER_EFFECT), &m_pConstantBufferEffect);

	// スプライト用 コンスタントバッファー作成　ここでは変換行列渡し用
	MakeConstantBuffer(sizeof(CONSTANT_BUFFER_SPRITE), &m_pConstantBufferSprite3D);

	// Fbxコンスタントバッファー作成　ここでは変換行列渡し用
	MakeConstantBuffer(sizeof(CONSTANT_BUFFER_WVLED), &m_pConstantBufferWVLED);

	// Fbxコンスタントバッファー作成　ここではボーン行列渡し用
	MakeConstantBuffer(sizeof(MATRIX4X4) * MAX_BONES, &m_pConstantBufferBone2);

	// コンスタントバッファー作成　マテリアル渡し用                                    // -- 2020.12.15
	MakeConstantBuffer(sizeof(CONSTANT_BUFFER_MATERIAL), &m_pConstantBufferMaterial);

	// Fbxコンスタントバッファー作成　ShadowMap用
	MakeConstantBuffer(sizeof(CONSTANT_BUFFER_WVLLED), &m_pConstantBufferWVLLED);

	// HDR用　コンスタントバッファー作成
	MakeConstantBuffer(sizeof(CONSTANT_BUFFER_HDR), &m_pConstantBufferHDR);

	// HDR Bloom用　コンスタントバッファー作成
	MakeConstantBuffer(sizeof(VECTOR4) * 15, &m_pConstantBufferHDRBloom);

	// 流体計算用コンスタントバッファー作成
	MakeConstantBuffer(sizeof(CONSTANT_BUFFER_FLUID), &m_pConstantBufferFluid);

	return S_OK;
}

//------------------------------------------------------------------------
//
//  コンスタントバッファーの作成関数
//
//  引数	UINT	size						作成するコンスタントバッファーのサイズ
//			ID3D11Buffer**  pppConstantBuffer	作成するコンスタントバッファー(OUT)
//
//	戻り値 HRESULT
//		S_OK	= 正常
//		E_FAIL	= 異常
//
//------------------------------------------------------------------------
HRESULT CShader::MakeConstantBuffer(UINT size, ID3D11Buffer**  ppConstantBuffer)
{
	D3D11_BUFFER_DESC cb = { 0 };

	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cb.ByteWidth = size;
	cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cb.MiscFlags = 0;
	cb.StructureByteStride = 0;
	cb.Usage = D3D11_USAGE_DYNAMIC;

	if (FAILED(m_pD3D->m_pDevice->CreateBuffer(&cb, nullptr, ppConstantBuffer)))
	{
		MessageBox(0, _T("コンスタントバッファー 作成失敗"), nullptr, MB_OK);
		return E_FAIL;
	}
	return S_OK;
}


//------------------------------------------------------------------------  // -- 2019.4.14
//
//	シェーダにコンスタントバッファＦｏｇを設定する処理
//
// 引数
//   const int&      nFog;          // フォグ有無(0:無し 1:指数フォグ 2:グランドフォグ)
//   const float&    fFogVolume;    // フォグ量（指数）
//   const float&    fFogDensity;   // フォグ密度
//   const float&    fFogHeight;    // フォグ高さ
//   const VECTOR4&  vFogColor;     // フォグの色
//
//	戻り値 HRESULT
//		S_OK	= 正常
//		E_FAIL	= 異常
//
//------------------------------------------------------------------------
void CShader::SetConstantBufferFog(const int& nFog, const float& fFogVolume, const float& fFogDensity, const float& fFogHeight, const VECTOR4& vFogColor)
{
	//シェーダーのコンスタントバッファーFOGに各種データを渡す
	D3D11_MAPPED_SUBRESOURCE pData;
	CONSTANT_BUFFER_FOG cbf;

	if (SUCCEEDED(m_pD3D->m_pDeviceContext->Map(m_pConstantBufferFog, 0, D3D11_MAP_WRITE_DISCARD, 0, &pData)))
	{
		// フォグ情報を渡す
		// x:フォグ有無(0:無し 1:指数フォグ 2:グランドフォグ)  y:フォグ量  z: フォグ密度  w:フォグ高さ
		cbf.vFogInfo.x = (float)nFog;
		cbf.vFogInfo.y = fFogVolume;
		cbf.vFogInfo.z = fFogDensity;
		cbf.vFogInfo.w = fFogHeight;

		// フォグの色を渡す
		cbf.vFogColor = vFogColor;

		memcpy_s(pData.pData, pData.RowPitch, (void*)(&cbf), sizeof(cbf));
		m_pD3D->m_pDeviceContext->Unmap(m_pConstantBufferFog, 0);
	}

}
