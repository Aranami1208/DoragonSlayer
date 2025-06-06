#include "BehaviourSelector.h"

// Selectorノード
// すでに実行中(runIdxが０以上)のときは、そこから実行する。
// 実行中で無いときは、子ノードを先頭から順番に実行していく
// 子ノードが一つでもbsTrueを返したら、その時点でbsTrueを返して終了
// 子ノードが全てbsFalseのときはbsFalseを返して終了
// 子ノードが実行中bsRunningのときは、その位置をrunIdxに設定し、bsRunningを返して終了
BehaviourBase::BtState BehaviourSelector::Run()
{
    int i = 0;

    if (runIdx >= 0) i = runIdx;   // 実行中の場合はここから実行する

    for (; i < Child().size(); i++) {   // 子ノードをiの初期値から順番に実行
        BehaviourBase::BtState state = Child()[i]->Run();
        if (state == bsRunning) {   // 子ノード実行中
            runIdx = i;
            return bsRunning;
        }
        else if (state == bsTrue) {   // 子ノード実行結果
            runIdx = -1;
            return bsTrue;
        }
    }
    runIdx = -1;
    return bsFalse;
}