#pragma once

#include "GameMain.h"
#include "Object3D.h"

// マクロ定義 ----------------------------------------------

// 流体オブジェクトのオブジェクト数
#define FLUID_OBJ_MAX 4

// 端を除いた場の一辺のサイズ（一辺のサイズ-2）。場の端を含めると一辺はFLUID_SIZE+2となる
// （シェーダーFluidSolver.hlsli中にも指定あり）
#define FLUID_SIZE 64
//#define FLUID_SIZE 100

// 端を除いた場の一辺のサイズの平方根
// （シェーダーFluidSolver.hlsli中にも指定あり）
// ・コンピュートシェーダはＣＰＵ側とＧＰＵ側でスレッド指定数が近い方が効率が良いので、
// 　例えば６４のときは、８＊８（ＣＰＵ８、ＧＰＵ８）で指定する
#define FLUID_RSIZE 8
//#define FLUID_RSIZE 10


// 微小時間
// （シェーダーFluidSolver.hlsli中にも指定あり）
//#define FLUID_TIME_STEP 0.01f
#define FLUID_TIME_STEP 0.04f

// SD_RESOURCE同士を入れ替えるマクロ
// ・ポインターを入れ替える。実体は移動しない。
#define FLUID_SWAP_SDRESOURCE(a,b) {SD_RESOURCE tmp;memcpy(&tmp,&a,sizeof(SD_RESOURCE));memcpy(&a,&b,sizeof(SD_RESOURCE));memcpy(&b,&tmp,sizeof(SD_RESOURCE));}

// シェーダーリソース構造体
struct SD_RESOURCE
{
	ID3D11Texture2D*			pTex2D;
	ID3D11Texture2D*			pDSTex2D;
	ID3D11Texture3D*			pTex3D;
	ID3D11Buffer*				pStredBuf;
	ID3D11RenderTargetView*		pRTV;
	ID3D11ShaderResourceView*	pSRV;	
	ID3D11DepthStencilView*		pDSV;
	ID3D11UnorderedAccessView*	pUAV;

	SD_RESOURCE()
	{
		ZeroMemory(this, sizeof(SD_RESOURCE));
	}

	void Release()
	{
		SAFE_RELEASE(pTex2D);
		SAFE_RELEASE(pDSTex2D);
		SAFE_RELEASE(pTex3D);
		SAFE_RELEASE(pStredBuf);
		SAFE_RELEASE(pRTV);
		SAFE_RELEASE(pSRV);
		SAFE_RELEASE(pDSV);
		SAFE_RELEASE(pUAV);
	}
};

//ストラクチャードバッファの要素構造体
struct SBUFFER_ELEMENT
{
	float f;
	float f0;
};

// --------------------------------------------------------
//
//	流体計算マネージャクラス
//
// --------------------------------------------------------
class Fluid;
class FluidManager : public Object3D
{
protected:
	SD_RESOURCE			m_TexA;		// ３Ｄテクスチャの描画時にレンダーターゲットとして使用
	SD_RESOURCE			m_TexB;		// ３Ｄテクスチャの描画時にレンダーターゲットとして使用

	float				m_fCubeSize;		// 表示用立方体　一辺の大きさ
	ID3D11Buffer*		m_pCubeVBuffer;		// 表示用立方体　バーテックスバッファ
	ID3D11Buffer*		m_pCubeIBuffer;		// 表示用立方体　インデックスバッファ

	ID3D11SamplerState* m_pSampleLinearFluid;	// 表示用サンプラーステート

	ID3D11RasterizerState* m_pFrontFace;	// 表面のみ表示
	ID3D11RasterizerState* m_pBackFace;		// 裏面のみ表示

	int tagNo;	   // タグＮＯ

public:
	FluidManager();
	virtual	~FluidManager();
	void Init();

	Fluid* SpawnY(const Transform& trans, const float& fCubeSize, const float& fColorNo, const float& fDensity, const float& fVelocity, const float&  fRandom, const int& nAddVoxelWidth, std::string& tag);
	Fluid* SpawnZ(const Transform& trans, const float& fCubeSize, const float& fColorNo, const float& fDensity, const float& fVelocity, const float&  fRandom, const int& nAddVoxelWidth, std::string& tag);

	HRESULT MakeCubeVertexBuffer(const float& fSize);
	void  Draw(const MATRIX4X4& mWorld, const VECTOR4& vOption, ID3D11ShaderResourceView** ppSRV, const MATRIX4X4& mView, const MATRIX4X4& mProj, const VECTOR3& vLight, const VECTOR3& vEye);

	Fluid*  SetFluidObj(std::string& tag);

	float GetCubeSize() { return m_fCubeSize; }
};