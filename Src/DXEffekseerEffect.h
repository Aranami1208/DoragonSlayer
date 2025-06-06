#pragma once
#include "GameMain.h"
#include "DXEffekseer.h"
#include "DXEffekseerManager.h"

class DXEffekseer;
class DXEffekseerManager;

/// <summary>
/// DirectX11用のEffekseerライブラリ  エフェクトクラス
///             author : Kaito Suzuki(student)
///             editor : Terutaka Suzuki
/// </summary>
class DXEffekseerEffect :public DXEffekseerBase
{
public:

	void SetEffect(std::string name);

	/// <summary>
	/// エフェクトの生成と再生
	/// </summary>
	/// <param name="name">エフェクト名</param>
	/// <param name="posIn">位置</param>
	/// <param name="isLoopIn">ループ再生するか。規定値はfalse</param>
	void Spawn(std::string name, const VECTOR3&, const bool& isLoopIn = false);

	/// <summary>
	/// エフェクトの生成と再生
	/// </summary>
	/// <param name="name">エフェクト名</param>
	/// <param name="posIn">位置</param>
	/// <param name="rotIn">角度</param>
	/// <param name="isLoopIn">ループ再生するか。規定値はfalse</param>
	void Spawn(std::string name, const VECTOR3&, const VECTOR3&, const bool& isLoopIn = false);

	/// <summary>
	/// エフェクトの生成と再生
	/// （法線の角度で発生する）
	/// </summary>
	/// <param name="name">エフェクト名</param>
	/// <param name="posIn">位置</param>
	/// <param name="normalIn">法線</param>
	/// <param name="isLoopIn">ループ再生するか。規定値はfalse</param>
	void SpawnFromNormal(std::string name, const VECTOR3&, const VECTOR3&, const bool& isLoopIn = false);

	void Play( const bool &);
	void Stop();
	void SetDelayTime(const unsigned int&);
	void Update() override;
	void DrawScreen() override;
	VECTOR3 GetNormalRotateVector(const VECTOR3&);

	DXEffekseerEffect();
	~DXEffekseerEffect();
private:
	DXEffekseer* effek;
	DXEffekseerManager* effManager;
};

