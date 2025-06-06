#pragma once
#include "Object3D.h"

/// <summary>
/// ビヘイビアツリーの基底クラス
/// </summary>
class BehaviourBase
{
public:
	// 実行情報
	enum BtState
	{
		bsFalse = 0,	// 実行失敗
		bsTrue,			// 実行成功
		bsRunning,		// 実行中
	};

	BehaviourBase();
	virtual ~BehaviourBase();

	/// <summary>
	/// 子ノードの追加
	/// ・SelectorとSequenceノードの追加用
	/// </summary>
	/// <typeparam name="TType">追加するクラス名</typeparam>
	/// <returns>追加したインスタンスのアドレス</returns>
	template <typename TType> TType* AddChild()
	{
		child.push_back(new TType());
		TType* ref = dynamic_cast<TType*>(child.back());
		return ref;
	}

	/// <summary>
	/// 子ノードの追加
	/// ・Actionノードの追加用
	/// </summary>
	/// <typeparam name="TType">追加するクラス名</typeparam>
	/// <param name="objIn">関数のthisアドレス</param>
	/// <param name="funcIn">追加する静的関数名</param>
	/// <returns>追加したインスタンスのアドレス</returns>
	template <typename TType> TType* AddChild(Object3D* objIn, BtState(*funcIn)(Object3D*))
	{
		child.push_back(new TType());
		TType* ref = dynamic_cast<TType*>(child.back());
		ref->SetObj(objIn);
		ref->SetFunc(funcIn);
		return ref;
	}

	virtual BtState Run() {return bsFalse;}
	auto& Child() { return child; }
	void SetObj(Object3D* objIn) { obj = objIn; }
	void SetFunc(BtState(*funcIn)(Object3D*)) { func = funcIn; }

protected:
	int  runIdx;
	Object3D* obj;
	BtState(*func)(Object3D*);
	std::vector<BehaviourBase*> child;
};