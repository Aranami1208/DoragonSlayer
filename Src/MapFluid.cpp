#include "MapManager.h"
#include "FluidManager.h"
#include "Fluid.h"

MapFluid::MapFluid()
{
	ObjectManager::SetDrawOrder(this, -200);   // 透明色は遅めに描画する

	m_tag = "";					// 火炎流体のタグ。初期値は""(未定)
	m_fCubeSize = 3.0f;			// 表示用立方体の一辺の大きさ
	m_fColorNo = 1;				// 流体の色コード（０～４）
	m_fDensity = 2.0f;			// 密度
	m_fVelocity = 300;			// 速度
	m_fRandom = 0.3f;			// 速度方向ランダム率
	m_nAddVoxelWidth = 0;		// 複数箇所から発生させるときのサイズ（ボクセル数）。１カ所の場合は０にする
}

MapFluid::~MapFluid()
{
	;
}

void MapFluid::MakeFluidMap(TextReader* txt, int n)
{
	VECTOR3 pos;
	pos.x = txt->GetFloat(n, 1);
	pos.y = txt->GetFloat(n, 2);
	pos.z = txt->GetFloat(n, 3);
	transform.position = pos;
	transform.rotation = VECTOR3(0,0,0);
	m_fCubeSize = txt->GetFloat(n, 4);			// 表示用立方体の一辺の大きさ
	m_fColorNo = txt->GetFloat(n,5);			// 流体の色コード（０～４）
	m_fDensity = txt->GetFloat(n,6);			// 密度
	m_fVelocity = txt->GetFloat(n,7);			// 速度
	m_fRandom = txt->GetFloat(n,8);			// 速度方向ランダム率
	m_nAddVoxelWidth = txt->GetFloat(n,9);		// 複数箇所から発生させるときのサイズ（ボクセル数）。１カ所の場合は０にする

}
void MapFluid::Update()
{
	ObjectManager::FindGameObject<FluidManager>()->SpawnY(transform, m_fCubeSize, m_fColorNo, 
										m_fDensity, m_fVelocity, m_fRandom, m_nAddVoxelWidth, m_tag);
}

void MapFluid::DrawDepth()
{
	// 影を作らない
}
void MapFluid::DrawScreen()
{
	Fluid* obj = ObjectManager::FindGameObjectWithTag<Fluid>(m_tag);
	if (obj != nullptr)
	{
		obj->DrawScreen();
	}
}