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
//		・FBXStaticMesh/FBXSkinMesh用のシェーダー       InitShaderFbx();         Cascade ShadoeMap／環境マッピング対応
//		・ディスプレイスメントマッピング用のシェーダー  InitShaderDisplace();    Cascade ShadoeMap対応
//		・エフェクト用のシェーダー                      InitShaderEffect();
//		・HDR用のシェーダー                             InitShaderHDR();
//		・Fluid用のシェーダー                           InitShaderFluid();
//
//                                                                              Shader.h
// ========================================================================================
#pragma once

//ヘッダーファイルのインクルード
#include <stdio.h>
#include <windows.h>

#include "Main.h"
#include "Direct3D.h"

//シェーダーのバッファ構造体定義

// メッシュシェーダー用のコンスタントバッファーのアプリ側構造体。
// （ワールド行列から射影行列、ライト、カラー）  // -- 2020.1.24
struct CONSTANT_BUFFER_WVLED
{
	MATRIX4X4  mW;             // ワールド行列
	MATRIX4X4  mWVP;           // ワールドから射影までの変換行列
	VECTOR4    vLightDir;      // ライト方向
	VECTOR4    vEyePos;        // 視点
	VECTOR4    vDiffuse;       // ディフューズ色	
	VECTOR4    vDrawInfo;      // 描画関連情報(使用していない)   // -- 2020.12.15
	CONSTANT_BUFFER_WVLED()
	{
		ZeroMemory(this, sizeof(CONSTANT_BUFFER_WVLED));
	}
};

// ディスプレースメントマッピング用の各種データを渡す  // -- 2020.1.24
struct CONSTANT_BUFFER_DISPLACE
{
	VECTOR3    vEyePosInv;    // 各頂点から見た、視点の位置
	float      fMinDistance;  // ポリゴン分割の最小距離
	float      fMaxDistance;  // ポリゴン分割の最大小距離
	int        iMaxDevide;    // 分割最大数
	VECTOR2    vHeight;       // ディスプレースメントマッピング時の盛り上げ高さ
	VECTOR4    vWaveMove;     // 波の移動量(波の処理時のみ)
	VECTOR4    vSpecular;     // 鏡面反射(波の処理時のみ)
	CONSTANT_BUFFER_DISPLACE()
	{
		ZeroMemory(this, sizeof(CONSTANT_BUFFER_DISPLACE));
	}
};

// Cascade Shadowシェーダー用のコンスタントバッファーのアプリ側構造体。
// （ワールド行列から射影行列、ライト、カメラ）  // -- 2020.4.5
struct CONSTANT_BUFFER_WVLLED
{
	MATRIX4X4  mW;             // ワールド行列
	MATRIX4X4  mWVP;           // ワールドから射影までの変換行列
	MATRIX4X4  mWLP[MAX_CASCADE_SHADOW];           // ワールド・”ライトビュー”・プロジェクションの合成
	VECTOR4    vLightDir;      // ライト方向
	VECTOR4    vEyePos;        // カメラ位置	
	VECTOR4    vDiffuse;       // ディフューズ色	
	VECTOR4    vDrawInfo;      // マテリアル関連情報　y:影有り無し   // -- 2020.12.15
	CONSTANT_BUFFER_WVLLED()
	{
		ZeroMemory(this, sizeof(CONSTANT_BUFFER_WVLLED));
	}
};

// マテリアル情報                      // -- 2020.12.15
struct CONSTANT_BUFFER_MATERIAL
{
	VECTOR4    vMatDuffuse;
	VECTOR4    vMatSpecular;
};


//  エフェクト用のコンスタントバッファのアプリ側構造体   //  2017.8.25
struct CONSTANT_BUFFER_EFFECT
{
	MATRIX4X4  mWVP;       // ワールドから射影までの変換行列
	MATRIX4X4  mW;         // ワールド
	MATRIX4X4  mV;         // ビュー
	MATRIX4X4  mP;         // 射影
	VECTOR2    vUVOffset;  // テクスチャ座標のオフセット
	VECTOR2    vUVScale;   // テクスチャ座標の拡縮  // -- 2019.7.17
	float      fAlpha;
	float      fSize;      // パーティクルの大きさ  // -- 2018.8.23
	VECTOR2    Dummy;                               // -- 2019.7.17
	CONSTANT_BUFFER_EFFECT()
	{
		ZeroMemory(this, sizeof(CONSTANT_BUFFER_EFFECT));
	}
};


//　3Dスプライトシェーダー用のコンスタントバッファーのアプリ側構造体 
struct CONSTANT_BUFFER_SPRITE
{
	MATRIX4X4  mWVP;
	MATRIX4X4  mW;
	float      ViewPortWidth;
	float      ViewPortHeight;
	VECTOR2    vUVOffset;
	VECTOR4    vColor;         // カラー情報。半透明の割合を指定する
	VECTOR4    vMatInfo;       // マテリアル関連情報　x:テクスチャ有り無し。DrawRect()、DrawLine()で使用。
	CONSTANT_BUFFER_SPRITE()
	{
		ZeroMemory(this, sizeof(CONSTANT_BUFFER_SPRITE));
	}
};


// フォグに関する情報を渡す      // -- 2019.3.5
struct CONSTANT_BUFFER_FOG
{
	VECTOR4 vFogInfo;      // x:フォグ有無(0:無し 1:指数フォグ 2:グランドフォグ)  y:フォグ量  z: フォグ密度  w:フォグ高さ
	VECTOR4 vFogColor;     // フォグの色
};

// HDR用のコンスタントバッファーのアプリ側構造体   // -- 2019.4.19
struct CONSTANT_BUFFER_HDR
{
	VECTOR2    ViewPort;
	VECTOR2    Info;
};

// 流体計算用コンスタントバッファ    // -- 2019.12.31
struct CONSTANT_BUFFER_FLUID
{
	VECTOR4 vAddVoxelPos;	// 注入ボクセル位置
	VECTOR4 vAddDensity;	// 注入密度
	VECTOR4 vAddVelocity;	// 注入速度
	VECTOR4 vOption;		// 各種オプション
};

//
// CShaderクラス
//
class CShader
{
public:
	// Direct3D11
	CDirect3D*              m_pD3D;

	// シェーダー
	// 通常用のシンプルなシェーダー
	ID3D11InputLayout*      m_pSimple_VertexLayout;
	ID3D11VertexShader*     m_pSimple_VS;
	ID3D11PixelShader*      m_pSimple_PS;

	// 3Dスプライト用のシェーダー
	ID3D11InputLayout*      m_pSprite3D_VertexLayout;
	ID3D11VertexShader*     m_pSprite3D_VS;
	ID3D11PixelShader*      m_pSprite3D_PS;
	ID3D11VertexShader*     m_pSprite3D_VS_BILL;
	ID3D11PixelShader*      m_pSprite3D_PS_ENV;		// -- 2019.6.8

	// ディスプレースメントマッピング(波)用のシェーダー
	ID3D11InputLayout*      m_pDisplaceWave_VertexLayout;
	ID3D11VertexShader*     m_pDisplaceWave_VS;
	ID3D11HullShader*       m_pDisplaceWave_HS;
	ID3D11DomainShader*     m_pDisplaceWave_DS;
	ID3D11PixelShader*      m_pDisplaceWave_PS;

	// ディスプレースメントマッピング(スキンメッシュ)Shadow 用のシェーダー  // -- 2020.1.24
	ID3D11VertexShader* m_pDisplaceSkinShadowMap_VS;
	ID3D11HullShader* m_pDisplaceSkinShadowMap_HS;
	ID3D11DomainShader* m_pDisplaceSkinShadowMap_DS;
	ID3D11PixelShader* m_pDisplaceSkinShadowMap_PS;
	ID3D11DomainShader* m_pDisplaceSkinShadowMap_DSDepth;
	ID3D11PixelShader* m_pDisplaceSkinShadowMap_PSDepth;

	// ディスプレースメントマッピング(スタティックメッシュ)Shadow 用のシェーダー  // -- 2018.7.28
	ID3D11VertexShader* m_pDisplaceStaticShadowMap_VS;
	ID3D11HullShader* m_pDisplaceStaticShadowMap_HS;
	ID3D11DomainShader* m_pDisplaceStaticShadowMap_DS;
	ID3D11PixelShader* m_pDisplaceStaticShadowMap_PS;
	ID3D11DomainShader* m_pDisplaceStaticShadowMap_DSDepth;
	ID3D11PixelShader* m_pDisplaceStaticShadowMap_PSDepth;

	// エフェクト用のシェーダー
	ID3D11InputLayout*      m_pEffect3D_VertexLayout;
	ID3D11VertexShader*     m_pEffect3D_VS_POINT;
	ID3D11GeometryShader*   m_pEffect3D_GS_POINT;
	ID3D11PixelShader*      m_pEffect3D_PS;
	ID3D11InputLayout*      m_pEffect3D_VertexLayout_BILL;
	ID3D11VertexShader*     m_pEffect3D_VS_BILL;
	ID3D11VertexShader*     m_pEffect3D_VS_BILLMESH;  // -- 2019.7.17

	// Fbxモデル　スタティツクメッシュShadowMap用		// -- 2018.7.28
	ID3D11InputLayout* m_pFbxStaticShadowMap_VertexLayout;
	ID3D11VertexShader* m_pFbxStaticShadowMap_VSDepth;
	ID3D11PixelShader* m_pFbxStaticShadowMap_PSDepth;
	ID3D11VertexShader* m_pFbxStaticShadowMap_VS;
	ID3D11PixelShader* m_pFbxStaticShadowMap_PS;

	// Fbxモデル　スキンメッシュ ShadowMap用		// -- 2018.7.28
	ID3D11InputLayout* m_pFbxSkinShadowMap_VertexLayout;
	ID3D11VertexShader* m_pFbxSkinShadowMap_VSDepth;
	ID3D11PixelShader* m_pFbxSkinShadowMap_PSDepth;
	ID3D11VertexShader* m_pFbxSkinShadowMap_VS;
	ID3D11PixelShader* m_pFbxSkinShadowMap_PS;

	// HDR用のシェーダー
	ID3D11InputLayout* m_pHDR_VertexLayout;
	ID3D11VertexShader* m_pHDR_VS;
	ID3D11PixelShader* m_pHDR_PS;
	ID3D11PixelShader* m_pHDR_PS_BRIGHT;
	ID3D11PixelShader* m_pHDR_PS_BLOOM;

	// 流体計算のシェーダー
	ID3D11ComputeShader* m_pFluidSolver_AddSource;
	ID3D11ComputeShader* m_pFluidSolver_AdvectBack;
	ID3D11ComputeShader* m_pFluidSolver_AdvectDensity;
	ID3D11ComputeShader* m_pFluidSolver_AdvectVelocity;
	ID3D11ComputeShader* m_pFluidSolver_Boundary1;
	ID3D11ComputeShader* m_pFluidSolver_MacCormack;
	ID3D11ComputeShader* m_pFluidSolver_Project1;
	ID3D11ComputeShader* m_pFluidSolver_Project2;
	ID3D11ComputeShader* m_pFluidSolver_Project3;
	ID3D11InputLayout* m_pFluidRender_VertexLayout;
	ID3D11VertexShader* m_pFluidRender_VS;
	ID3D11PixelShader* m_pFluidRender_PS;
	ID3D11InputLayout* m_pFluidTexture_VertexLayout;
	ID3D11VertexShader* m_pFluidTexture_VS;
	ID3D11PixelShader* m_pFluidTexture_PS;


	// コンスタントバッファ  ------------------------------------------

	//コンスタントバッファーディスプレイスメントマッピング用
	ID3D11Buffer* m_pConstantBufferDisplace;

	//コンスタントバッファーエフェクト用
	ID3D11Buffer* m_pConstantBufferEffect;

	//コンスタントバッファー 3Dスプライト用
	ID3D11Buffer* m_pConstantBufferSprite3D;

	// コンスタントバッファー　メッシュ 変換行列・カラー渡し用
	ID3D11Buffer* m_pConstantBufferWVLED;

	// コンスタントバッファー ボーン行列渡し用
	ID3D11Buffer* m_pConstantBufferBone2;   // -- 2018.4.14 ボーン行列用コンスタントバッファ

	// マテリアル情報　渡し用
	ID3D11Buffer* m_pConstantBufferMaterial;    // -- 2020.12.15

	// コンスタントバッファー　メッシュ 変換行列・カラー渡し(シャドウマップ用)
	ID3D11Buffer* m_pConstantBufferWVLLED;   // -- 2018.7.28

	// コンスタントバッファー  フォグ情報渡し用
	ID3D11Buffer* m_pConstantBufferFog;

	// コンスタントバッファー HDR用
	ID3D11Buffer* m_pConstantBufferHDR;
	ID3D11Buffer* m_pConstantBufferHDRBloom;

	// コンスタントバッファー  流体計算用
	ID3D11Buffer* m_pConstantBufferFluid;


public:
	HRESULT InitShader();
	HRESULT InitShaderSimple();
	HRESULT InitShaderSprite();

	HRESULT InitShaderFbx();
	HRESULT InitShaderDisplace();
	HRESULT InitShaderEffect();
	HRESULT InitShaderHDR();
	HRESULT InitShaderFluid();

	HRESULT InitShaderConstant();

	void    SetConstantBufferFog(const int& m_nFog, const float& m_fFogVolume, const float& m_fFogDensity, const float& m_fFogHeight, const VECTOR4& m_vFogColor);

	HRESULT MakeShader(const TCHAR ProfileName[], const TCHAR FileName[], void** ppShader, D3D11_INPUT_ELEMENT_DESC Fluid_layout[] = nullptr, UINT numElements = 0, ID3D11InputLayout** ppInputLayout = nullptr);
	HRESULT MakeConstantBuffer(UINT size, ID3D11Buffer**  ppConstantBuffer);

	CShader(CDirect3D* pD3D);
	~CShader();

};
