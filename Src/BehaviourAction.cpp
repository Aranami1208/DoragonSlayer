#include "BehaviourAction.h"

// Actionノード
// 実行していく
BehaviourBase::BtState BehaviourAction::Run()
{
    return func(obj);
}