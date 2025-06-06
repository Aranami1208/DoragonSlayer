#pragma once
#include "Displace.h"
#include "MapManager.h"

class Fluid;
class MapFluid : public MapBase {
public:
	MapFluid();
	~MapFluid();

	void MakeFluidMap(TextReader* txt, int n);
	void Update() override;
	void DrawDepth() override;
	void DrawScreen() override;

private:
	std::string		m_tag;			// 火炎流体オブジェクトのタグ
	Transform 		m_trans;		// 流体の位置
	float			m_fCubeSize;	// 表示用立方体の一辺の大きさ
	float			m_fColorNo;		// 流体の色コード（0:炎（黒煙なし）1: 炎（黒煙あり）2:青い炎 3:濃い煙 4:薄い煙）
	float			m_fDensity;		// 密度
	float			m_fVelocity;	// 速度
	float			m_fRandom;		// 速度方向ランダム率(0.0f:一定方向 ～ 1.0f:方向がランダム)
	int				m_nAddVoxelWidth;	// 複数箇所から発生させるときのサイズ（ボクセル数）。１カ所の場合は０にする
};