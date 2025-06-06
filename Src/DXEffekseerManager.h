#pragma once
#include "GameMain.h"
#include "DXEffekseerBase.h"
#include "DXEffekseerEffect.h"
#include "TextReader.h"

/// <summary>
/// DirectX11用のEffekseerライブラリ  マネージャークラス
///             author : Terutaka Suzuki
/// </summary>
class DXEffekseerEffect;
class DXEffekseerManager :public DXEffekseerBase
{
private:
	struct effectstruct {
		std::string name;
		float radius;
		Effekseer::EffectRef effectR;	//エフェクトの読み込み情報
		effectstruct()
		{
			radius = 0;
		}
	};
public:
	DXEffekseerManager();
	~DXEffekseerManager();

	Effekseer::EffectRef EffectList(std::string str, float& radius);

	/// <summary>
	/// エフェクトの生成と再生
	/// </summary>
	/// <param name="name">エフェクト名</param>
	/// <param name="posIn">位置</param>
	/// <param name="isLoopIn">ループ再生するか。規定値はfalse</param>
	/// <returns>エフェクトオブジェクト</returns>
	DXEffekseerEffect* Spawn(std::string name, const VECTOR3& posIn, const bool& isLoopIn = false);

	/// <summary>
	/// エフェクトの生成と再生
	/// </summary>
	/// <param name="name">エフェクト名</param>
	/// <param name="posIn">位置</param>
	/// <param name="rotIn">角度</param>
	/// <param name="isLoopIn">ループ再生するか。規定値はfalse</param>
	/// <returns>エフェクトオブジェクト</returns>
	DXEffekseerEffect* Spawn(std::string name, const VECTOR3& posIn, const VECTOR3& rotIn, const bool& isLoopIn = false);

	/// <summary>
	/// エフェクトの生成と再生
	/// （法線の角度で発生する）
	/// </summary>
	/// <param name="name">エフェクト名</param>
	/// <param name="posIn">位置</param>
	/// <param name="normalIn">法線</param>
	/// <param name="isLoopIn">ループ再生するか。規定値はfalse</param>
	/// <returns>エフェクトオブジェクト</returns>
	DXEffekseerEffect* SpawnFromNormal(std::string name, const VECTOR3& posIn, const VECTOR3& normalIn, const bool& isLoopIn = false);

	/// <summary>
	/// エフェクトの生成と再生
	/// (マップテキストから生成)
	/// </summary>
	/// <param name="txt">マップテキスト</param>
	/// <param name="n">行数</param>
	void MakeEffekseer(TextReader* txt, int n);

private:
	std::list<effectstruct> effectList;	  	//エフェクトリスト
};

