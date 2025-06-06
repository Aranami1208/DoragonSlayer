#include "HDRManager.h"
#include "HDRControl.h"

HDRControl::HDRControl()
{
	ObjectManager::DontDestroy(this);		// 消されない
	ObjectManager::SetActive(this, false);		// 更新しない
	ObjectManager::SetDrawOrder(this, -100000);	// 一番最後に描画処理する
}

HDRControl::~HDRControl()
{
}
void HDRControl::DrawDepth()
{
	// 影の処理はしない
}
void HDRControl::DrawScreen()
{
	// HDRリミットが1.0より小さいときは、HDRマップテクスチャーからHDRレンダリング
	// レンダーターゲットをバックバッファに戻す
	if (ObjectManager::FindGameObject<HDRManager>()->Limit() < 1.0f)
	{
		ObjectManager::FindGameObject<HDRManager>()->Render();   // HDRレンダリング
	}
}
