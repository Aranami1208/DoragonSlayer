#include "BehaviourAction.h"

// Actionƒm[ƒh
// Às‚µ‚Ä‚¢‚­
BehaviourBase::BtState BehaviourAction::Run()
{
    return func(obj);
}