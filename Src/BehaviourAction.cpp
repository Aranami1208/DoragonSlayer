#include "BehaviourAction.h"

// Action�m�[�h
// ���s���Ă���
BehaviourBase::BtState BehaviourAction::Run()
{
    return func(obj);
}