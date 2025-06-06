#pragma once
#include "Object3D.h"
#include "Animator.h"

class EffectBase : public Object3D
{
public:
	// ========================================================================================
	//
	// 頂点の構造体
	//
	// ========================================================================================
	struct PARTICLE_VERTEX
	{
		VECTOR3 Pos; //位置
	};
	struct BILLBOARD_VERTEX
	{
		VECTOR3 Pos; //位置
		VECTOR2 UV;  //テクスチャー座標
	};

	// ========================================================================================
	//
	// 一つのオブジェクトの中のパーティクル構造体
	//
	// ========================================================================================
	struct PART
	{
		VECTOR3     Pos;
		VECTOR3     Dir;
		float       Speed;
		int         BirthFrame;
		PART()
		{
			ZeroMemory(this, sizeof(PART));
		}
	};

	// ========================================================================================
	//
	//	パーティクル配列用構造体
	// 
	// ========================================================================================
	struct PARTICLEBASE
	{
		std::string                 m_name;			   // 別名
		ID3D11ShaderResourceView*	m_pTexture;        // テクスチャー(１つのテクスチャーには1つのパーティクルのみ)
		DWORD						m_dwImageWidth;    // テクスチャーの幅
		DWORD						m_dwImageHeight;   // テクスチャーの高さ
		ID3D11Buffer*				m_pVertexBuffer;   // バーテックスバッファ
		int							m_nNum;            // 一つのオブジェクト中のパーティクル数
		float						m_fDestSize;       // 表示サイズ(幅と高さは同一)
		int							m_FrameEnd;        // パーティクルを表示している時間
		float						m_fSpeed;          // パーティクルの移動スピード
		int							m_iBarthFrame;     // パーティクルの開始までの最大待ち時間。０は待ち無し
		int							m_ifBound;         // 地面でバウンドさせるか（0:バウンドなし 1:地面でバウンド）
		float						m_fAlpha;          // 透明度
		int							m_nBlendFlag;      // 0:通常 1:自己発光
		PARTICLEBASE()
		{
			m_pTexture = nullptr;        // テクスチャー(１つのテクスチャーには1つのパーティクルのみ)
			m_dwImageWidth = 0;    // テクスチャーの幅
			m_dwImageHeight = 0;   // テクスチャーの高さ
			m_pVertexBuffer = nullptr;   // バーテックスバッファ
			m_nNum = 0;            // 一つのオブジェクト中のパーティクル数
			m_fDestSize = 0.0f;    // 表示サイズ(幅と高さは同一)
			m_FrameEnd = 0;        // パーティクルを表示している時間
			m_fSpeed = 0.0f;       // パーティクルの移動スピード
			m_iBarthFrame = 0;     // パーティクルの開始までの最大待ち時間。０は待ち無し
			m_ifBound = 0;         // 地面でバウンドさせるか（0:バウンドなし 1:地面でバウンド）
			m_fAlpha = 1.0f;       // 透明度
			m_nBlendFlag = 0;      // 0:通常 1:自己発光
		}
	};

	// ========================================================================================
	//
	//	ビルボード構造体
	// 
	// ========================================================================================
	struct BILLBOARDBASE
	{
		std::string                 m_name;			   // 別名
		ID3D11ShaderResourceView*	m_pTexture;        // テクスチャー
		DWORD						m_dwImageWidth;    // テクスチャーの幅
		DWORD						m_dwImageHeight;   // テクスチャーの高さ
		ID3D11Buffer*				m_pVertexBuffer;   // バーテックスバッファ
		float						m_fDestWidth;      // 表示幅
		float						m_fDestHeight;     // 表示高さ
		float						m_fDestCenterX;    // 表示中心位置Ｘ
		float						m_fDestCenterY;    // 表示中心位置Ｙ
		DWORD						m_dwSrcX;          // パターンの位置Ｘ
		DWORD						m_dwSrcY;          // パターンの位置Ｙ
		DWORD						m_dwSrcWidth;      // １つのパターンの幅
		DWORD						m_dwSrcHeight;     // １つのパターンの高さ
		DWORD						m_dwNumX;          // パターンの横の数
		DWORD						m_dwNumY;          // パターンの縦の数
		float						m_fAlpha;          // 透明度
		int							m_nBlendFlag;      // 0:通常  1:自己発光
		int							m_nDrawFlag;       // 0:ビルボード  1:ビルボードメッシュ
		BILLBOARDBASE()
		{
			m_pTexture = nullptr;        // テクスチャー
			m_dwImageWidth = 0;    // テクスチャーの幅
			m_dwImageHeight = 0;   // テクスチャーの高さ
			m_pVertexBuffer = nullptr;   // バーテックスバッファ
			m_fDestWidth = 0.0f;   // 表示幅
			m_fDestHeight = 0.0f;  // 表示高さ
			m_fDestCenterX = 0.0f; // 表示中心位置Ｘ
			m_fDestCenterY = 0.0f; // 表示中心位置Ｙ
			m_dwSrcX = 0;          // パターンの位置Ｘ
			m_dwSrcY = 0;          // パターンの位置Ｙ
			m_dwSrcWidth = 0;      // １つのパターンの幅
			m_dwSrcHeight = 0;     // １つのパターンの高さ
			m_dwNumX = 0;          // パターンの横の数
			m_dwNumY = 0;          // パターンの縦の数
			m_fAlpha = 1.0f;       // 透明度
			m_nBlendFlag = 0;      // 0:通常  1:自己発光
			m_nDrawFlag = 0;       // 0:ビルボード  1:ビルボードメッシュ
		}
	};

public:
	EffectBase();
	virtual ~EffectBase();

	void SetNormal(VECTOR3 normalIn) { normal = normalIn; }

	virtual bool LoadBillTexture(const TCHAR* szFName, BILLBOARDBASE* pBillBase);
	virtual bool SetBillSrc(BILLBOARDBASE* pBillBase);
	virtual bool LoadPartTexture(const TCHAR* szFName, PARTICLEBASE* pPartBase);
	virtual bool SetPartSrc(PARTICLEBASE* pPartBase);

protected:
	VECTOR3 normal;
};