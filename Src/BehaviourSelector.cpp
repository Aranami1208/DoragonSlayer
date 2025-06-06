#include "BehaviourSelector.h"

// Selector�m�[�h
// ���łɎ��s��(runIdx���O�ȏ�)�̂Ƃ��́A����������s����B
// ���s���Ŗ����Ƃ��́A�q�m�[�h��擪���珇�ԂɎ��s���Ă���
// �q�m�[�h����ł�bsTrue��Ԃ�����A���̎��_��bsTrue��Ԃ��ďI��
// �q�m�[�h���S��bsFalse�̂Ƃ���bsFalse��Ԃ��ďI��
// �q�m�[�h�����s��bsRunning�̂Ƃ��́A���̈ʒu��runIdx�ɐݒ肵�AbsRunning��Ԃ��ďI��
BehaviourBase::BtState BehaviourSelector::Run()
{
    int i = 0;

    if (runIdx >= 0) i = runIdx;   // ���s���̏ꍇ�͂���������s����

    for (; i < Child().size(); i++) {   // �q�m�[�h��i�̏����l���珇�ԂɎ��s
        BehaviourBase::BtState state = Child()[i]->Run();
        if (state == bsRunning) {   // �q�m�[�h���s��
            runIdx = i;
            return bsRunning;
        }
        else if (state == bsTrue) {   // �q�m�[�h���s����
            runIdx = -1;
            return bsTrue;
        }
    }
    runIdx = -1;
    return bsFalse;
}