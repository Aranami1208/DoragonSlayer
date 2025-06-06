#pragma once

#include "FluidManager.h"
// --------------------------------------------------------
//
//	流体計算オブジェクトクラス
//
// --------------------------------------------------------
class Fluid : public Object3D
{
protected:
	SD_RESOURCE			m_Dns[2];	// 密度
	SD_RESOURCE			m_Vlc[2];	// 速度
	SD_RESOURCE			m_Vlc_s;	// 速度のセミグランジアン
	SD_RESOURCE			m_Vlc_bs;	// 速度の逆向きのセミグランジアン
	SD_RESOURCE			m_Prs;		// 圧力
	SD_RESOURCE			m_Div;		// 発散
	SD_RESOURCE			m_Dns_Tex;	// 描画

	bool				m_bAddFlag;		// 注入フラグ
	VECTOR4				m_vAddVoxelPos;	// 注入ボクセル位置
	VECTOR4				m_vAddDensity;	// 注入密度
	VECTOR4				m_vAddVelocity;	// 注入速度
	VECTOR4				m_vOption;	    // オプション(x:表示立方体の大きさ　y:色コード　z:なし　w:発生ランダム率)
	int					m_nAddVoxelWidth;	// 複数箇所から発生させるときのサイズ（ボクセル数）

	float				m_updateTimer;
	float				m_keepTimer;
public:
	Fluid();
	virtual	~Fluid();
	void Init();
	void Update() override;
	void DrawDepth() override;
	void DrawScreen() override;

	void Unbind();
	HRESULT CreateEmpty3DTexture(int Size,ID3D11Texture3D** ppTexture,ID3D11ShaderResourceView** ppSRV);

	void Spawn(const Transform& trans, const float& fCubeSize, const float& fColorNo, const VECTOR4&, const VECTOR4&, const VECTOR4&, const float& fRandom, const int& nAddVoxelWidth);
	void AddSource(VECTOR3 vOffset = VECTOR3(0, 0, 0));

};
