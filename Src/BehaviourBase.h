#pragma once
#include "Object3D.h"

/// <summary>
/// �r�w�C�r�A�c���[�̊��N���X
/// </summary>
class BehaviourBase
{
public:
	// ���s���
	enum BtState
	{
		bsFalse = 0,	// ���s���s
		bsTrue,			// ���s����
		bsRunning,		// ���s��
	};

	BehaviourBase();
	virtual ~BehaviourBase();

	/// <summary>
	/// �q�m�[�h�̒ǉ�
	/// �ESelector��Sequence�m�[�h�̒ǉ��p
	/// </summary>
	/// <typeparam name="TType">�ǉ�����N���X��</typeparam>
	/// <returns>�ǉ������C���X�^���X�̃A�h���X</returns>
	template <typename TType> TType* AddChild()
	{
		child.push_back(new TType());
		TType* ref = dynamic_cast<TType*>(child.back());
		return ref;
	}

	/// <summary>
	/// �q�m�[�h�̒ǉ�
	/// �EAction�m�[�h�̒ǉ��p
	/// </summary>
	/// <typeparam name="TType">�ǉ�����N���X��</typeparam>
	/// <param name="objIn">�֐���this�A�h���X</param>
	/// <param name="funcIn">�ǉ�����ÓI�֐���</param>
	/// <returns>�ǉ������C���X�^���X�̃A�h���X</returns>
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