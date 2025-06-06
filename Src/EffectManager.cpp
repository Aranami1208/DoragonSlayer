#include "EffectManager.h"

EffectManager::EffectManager()
{
	ObjectManager::DontDestroy(this);		// 自体は消されない
	ObjectManager::SetVisible(this, false);		// 自体は表示しない

	// 半球ビルボードスタティックメッシュの読み込み
	mesh = new CFbxMesh();
	mesh->Load("Data/Item/BillSphere.mesh");
	meshCol = nullptr;

	// ------------------------------------------------------------------------------------------
	// ビルボード構造体リストの設定
	BILLBOARDBASE bb;

	// 炎ビルボード "sparklen3" (先頭要素:省略値)
	billboardList.push_back(bb);
	billboardList.back().m_name = "sparklen3";
	LoadBillTexture("Data/Image/sparklen3.png", &billboardList.back());
	billboardList.back().m_fDestWidth = 0.7f;      // 表示幅
	billboardList.back().m_fDestHeight = 0.7f;     // 表示高さ
	billboardList.back().m_fDestCenterX = billboardList.back().m_fDestWidth / 2;  // 表示中心位置Ｘ(真ん中)
	billboardList.back().m_fDestCenterY = billboardList.back().m_fDestHeight / 2; // 表示中心位置Ｙ(真ん中)
	billboardList.back().m_dwSrcX = 0;             // パターンの位置　Ｘ座標
	billboardList.back().m_dwSrcY = 0;             // パターンの位置　Ｙ座標
	billboardList.back().m_dwSrcWidth = 85;        // パターンの幅
	billboardList.back().m_dwSrcHeight = 85;       // パターンの高さ
	billboardList.back().m_dwNumX = 3;             // アニメーションさせるパターンの数　Ｘ方向
	billboardList.back().m_dwNumY = 1;             // アニメーションさせるパターンの数　Ｙ方向
	billboardList.back().m_fAlpha = 0.9f;          // 透明度
	billboardList.back().m_nBlendFlag = 1;         // ブレンドステートフラグ(0:通常描画　1:加算合成色描画)
	billboardList.back().m_nDrawFlag = 0;          // 描画フラグ(0:ビルボードのみ)
	SetBillSrc(&billboardList.back());                 // バーテックスバッファの作成

	// 爆発ビルボード	"Bom3"
	billboardList.push_back(bb);
	billboardList.back().m_name = "Bom3";
	LoadBillTexture("Data/Image/Bom3.png", &billboardList.back());
	billboardList.back().m_fDestWidth = 4.0f;      // 表示幅
	billboardList.back().m_fDestHeight = 4.0f;     // 表示高さ
	billboardList.back().m_fDestCenterX = billboardList.back().m_fDestWidth / 2;  // 表示中心位置Ｘ(真ん中)
	billboardList.back().m_fDestCenterY = billboardList.back().m_fDestHeight / 2; // 表示中心位置Ｙ(真ん中)
	billboardList.back().m_dwSrcX = 0;             // パターンの位置　Ｘ座標
	billboardList.back().m_dwSrcY = 0;             // パターンの位置　Ｙ座標
	billboardList.back().m_dwSrcWidth = 64;        // パターンの幅
	billboardList.back().m_dwSrcHeight = 64;       // パターンの高さ
	billboardList.back().m_dwNumX = 4;             // アニメーションさせるパターンの数　Ｘ方向
	billboardList.back().m_dwNumY = 4;             // アニメーションさせるパターンの数　Ｙ方向
	billboardList.back().m_fAlpha = 0.9f;          // 透明度
	billboardList.back().m_nBlendFlag = 1;         // ブレンドステートフラグ(0:通常描画　1:加算合成色描画)
	billboardList.back().m_nDrawFlag = 1;          // 描画フラグ(0:ビルボード　1:ビルボードメッシュ)
	SetBillSrc(&billboardList.back());                 // バーテックスバッファの作成

	// 爆発ビルボード2	  "Bom4"
	billboardList.push_back(bb);
	billboardList.back().m_name = "Bom4";
	LoadBillTexture("Data/Image/Bom4.png", &billboardList.back());
	billboardList.back().m_fDestWidth = 4.0f;      // 表示幅
	billboardList.back().m_fDestHeight = 4.0f;     // 表示高さ
	billboardList.back().m_fDestCenterX = billboardList.back().m_fDestWidth / 2;  // 表示中心位置Ｘ(真ん中)
	billboardList.back().m_fDestCenterY = billboardList.back().m_fDestHeight;   // 表示中心位置Ｙ(下の端)
	billboardList.back().m_dwSrcX = 0;             // パターンの位置　Ｘ座標
	billboardList.back().m_dwSrcY = 0;             // パターンの位置　Ｙ座標
	billboardList.back().m_dwSrcWidth = 128;       // パターンの幅
	billboardList.back().m_dwSrcHeight = 128;      // パターンの高さ
	billboardList.back().m_dwNumX = 4;             // アニメーションさせるパターンの数　Ｘ方向
	billboardList.back().m_dwNumY = 4;             // アニメーションさせるパターンの数　Ｙ方向
	billboardList.back().m_fAlpha = 0.9f;          // 透明度
	billboardList.back().m_nBlendFlag = 0;         // ブレンドステートフラグ(0:通常描画　1:加算合成色描画)
	billboardList.back().m_nDrawFlag = 1;          // 描画フラグ(0:ビルボード　1:ビルボードメッシュ)
	SetBillSrc(&billboardList.back());                 // バーテックスバッファの作成

	// 爆発ビルボード3	  "Effect01"
	billboardList.push_back(bb);
	billboardList.back().m_name = "Effect01";
	LoadBillTexture("Data/Image/Effect01.png", &billboardList.back());
	billboardList.back().m_fDestWidth = 4.0f;      // 表示幅
	billboardList.back().m_fDestHeight = 4.0f;     // 表示高さ
	billboardList.back().m_fDestCenterX = billboardList.back().m_fDestWidth / 2;  // 表示中心位置Ｘ(真ん中)
	billboardList.back().m_fDestCenterY = billboardList.back().m_fDestHeight;   // 表示中心位置Ｙ(下の端)
	billboardList.back().m_dwSrcX = 0;             // パターンの位置　Ｘ座標
	billboardList.back().m_dwSrcY = 0;             // パターンの位置　Ｙ座標
	billboardList.back().m_dwSrcWidth = 128;       // パターンの幅
	billboardList.back().m_dwSrcHeight = 128;      // パターンの高さ
	billboardList.back().m_dwNumX = 4;             // アニメーションさせるパターンの数　Ｘ方向
	billboardList.back().m_dwNumY = 4;             // アニメーションさせるパターンの数　Ｙ方向
	billboardList.back().m_fAlpha = 0.9f;          // 透明度
	billboardList.back().m_nBlendFlag = 0;         // ブレンドステートフラグ(0:通常描画　1:加算合成色描画)
	billboardList.back().m_nDrawFlag = 1;          // 描画フラグ(0:ビルボード　1:ビルボードメッシュ)
	SetBillSrc(&billboardList.back());                 // バーテックスバッファの作成

	// ------------------------------------------------------------------------------------------
	// パーティクル構造体の設定
	PARTICLEBASE pb;

	// 火花パーティクル "particle3"(先頭要素:省略値)
	particleList.push_back(pb);
	particleList.back().m_name = "particle3";
	LoadPartTexture(_T("Data/Image/particle3.png"), &particleList.back());// パーティクルテクスチャ
	particleList.back().m_nNum = 100;             // 一つのオブジェクト中のパーティクル数。PARTICLE_NUM_MAX以下であること。
	particleList.back().m_fDestSize = 0.1f;       // 表示サイズ(一つのパーティクルの大きさ)
	particleList.back().m_FrameEnd = 60;          // パーティクルを表示している時間
	particleList.back().m_fSpeed = 0.015f;        // パーティクルの移動スピード。ランダム
	particleList.back().m_iBarthFrame = 20;       // パーティクルの開始までの最大待ち時間。ランダムで開始。０は待ち無し
	particleList.back().m_ifBound = 0;            // 地面でバウンドさせるか（0:バウンドなし 1:地面でバウンド）
	particleList.back().m_fAlpha = 0.9f;          // 透明度
	particleList.back().m_nBlendFlag = 1;         // ブレンドステートフラグ(0:通常描画　1:加算合成色描画)
	SetPartSrc(&particleList.back());             // バーテックスバッファの作成

	// 火花パーティクル・大  "particle2"
	particleList.push_back(pb);
	particleList.back().m_name = "particle2";
	LoadPartTexture(_T("Data/Image/particle2.png"), &particleList.back());// パーティクルテクスチャ
	particleList.back().m_nNum = 10;              // 一つのオブジェクト中のパーティクル数。PARTICLE_NUM_MAX以下であること。
	particleList.back().m_fDestSize = 0.5f;       // 表示サイズ(一つのパーティクルの大きさ)
	particleList.back().m_FrameEnd = 60;          // パーティクルを表示している時間
	particleList.back().m_fSpeed = 0.015f;        // パーティクルの移動スピード。ランダム
	particleList.back().m_iBarthFrame = 0;        // パーティクルの開始までの最大待ち時間。ランダムで開始。０は待ち無し
	particleList.back().m_ifBound = 0;            // 地面でバウンドさせるか（0:バウンドなし 1:地面でバウンド）
	particleList.back().m_fAlpha = 0.9f;          // 透明度
	particleList.back().m_nBlendFlag = 1;         // ブレンドステートフラグ(0:通常描画　1:加算合成色描画)
	SetPartSrc(&particleList.back());             // バーテックスバッファの作成

}

EffectManager::~EffectManager()
{
	SAFE_DELETE(mesh);
	for (BILLBOARDBASE& bb : billboardList)
	{
		SAFE_RELEASE(bb.m_pTexture);
		SAFE_RELEASE(bb.m_pVertexBuffer);
	}
	for (PARTICLEBASE& pb : particleList)
	{
		SAFE_RELEASE(pb.m_pTexture);
		SAFE_RELEASE(pb.m_pVertexBuffer);
	}
}


EffectBase::BILLBOARDBASE* EffectManager::BillboardList(std::string str)
{
	if (str == "")
	{
		return &billboardList.front();	 // 先頭要素
	}
	for (BILLBOARDBASE& bb : billboardList)
	{
		if (str == bb.m_name) return &bb;
	}
	std::string msg = "EffectManager::BillboardList()   " + str;
	MessageBox(nullptr, msg.c_str(), _T("■□■ 指定のエフェクト名のエフェクトはビルボードリストにありません ■□■"), MB_OK);
	return nullptr;
}

EffectBase::PARTICLEBASE* EffectManager::ParticleList(std::string str)
{
	if (str == "")
	{
		return &particleList.front();	 // 先頭要素
	}
	for (PARTICLEBASE& pb : particleList)
	{
		if (str == pb.m_name) return &pb;
	}
	std::string msg = "EffectManager::ParticleList()   " + str;
	MessageBox(nullptr, msg.c_str(), _T("■□■ 指定のエフェクト名のエフェクトはパーティクルリストにありません ■□■"), MB_OK);
	return nullptr;
}
