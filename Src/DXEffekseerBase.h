#include "Effekseer.h"
#include "Object3D.h"
#include <string>

#pragma once

/// <summary>
/// DirectX11用のEffekseerライブラリ  基底クラス
///             author : Kaito Suzuki(student)
///             editor : Terutaka Suzuki
/// </summary>
class DXEffekseerBase : public Object3D
{
public:
	const bool Active() const { return isActive; };
	const Effekseer::Handle Handle() const { return EffectHandle; };

	virtual void Death() final;
	virtual void DrawDepth() override;		// 影の描画
	virtual void DrawScreen() override;		// オブジェクトの描画

	virtual void LoadEffect(const TCHAR *);
	virtual Effekseer::EffectRef LoadEffectEffectR(const TCHAR *);

	void SetEffectLocation(const VECTOR3 &argPos);
	void SetEffectRotation(const float angle);
	void SetEffectRotation(const VECTOR3 &angle);
	void SetEffectScale(const VECTOR3 &scale);
	void SetActive(const bool &activeBool) { isActive = activeBool; };
	void SetDontDestroy(const bool& dontDestroyBool) { dontDestroy = dontDestroyBool; }

	float Radius() { return radius; }

	//コンストラクタ
	DXEffekseerBase();
	virtual ~DXEffekseerBase();

protected:
	bool isActive;					// そのエフェクトはアクティブ？
	bool isLoop;					// ループするエフェクト？
	bool isDelaySpawned;			// 遅れてSpawnさせるオブジェクト？
	bool dontDestroy;				// 終了後、このオブジェクトを削除しないか。しないときtrue
	unsigned int Timer;				// Spawnしてから経過した時間
	unsigned int DeathTime;			// Spawn～Deathまでの時間
	unsigned int DelaySpawnTime;	// Spawnするまでの時間(遅らせることができる)
	Effekseer::EffectRef EffectR;	// エフェクトの読み込み情報
	Effekseer::Handle EffectHandle;	// エフェクトのインスタンスハンドル
	float radius;					// 当たり判定の半径
};