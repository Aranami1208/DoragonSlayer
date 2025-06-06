#include "BehaviourSequence.h"

// Sequence�m�[�h
// ���łɎ��s��(runIdx���O�ȏ�)�̂Ƃ��́A����������s����B
// ���s���Ŗ����Ƃ��́A�q�m�[�h��擪���珇�ԂɎ��s���Ă���
// �q�m�[�h���P�ł�bsFalse��Ԃ�����A���̎��_��bsFalse��Ԃ��ďI��
// �q�m�[�h���S��bsTrue�̂Ƃ���bsTrue��Ԃ��ďI��
// �q�m�[�h�����s��bsRunning�̂Ƃ��́A���̈ʒu��runIdx�ɐݒ肵�AbsRunning��Ԃ��ďI��
BehaviourBase::BtState BehaviourSequence::Run()
{
    int i = 0;

    if (runIdx >= 0) i = runIdx;   // ���s���̏ꍇ�͂���������s����

    for (; i < Child().size(); i++) {   // �q�m�[�h��i�̏����l���珇�ԂɎ��s
        BehaviourBase::BtState state = Child()[i]->Run();
        if (state == bsRunning) {   // �q�m�[�h���s��
            runIdx = i;
            return bsRunning;
        }
        else if (state == bsFalse) {   // �q�m�[�h���s����
            runIdx = -1;
            return bsFalse;
        }
    }
    runIdx = -1;
    return bsTrue;
}