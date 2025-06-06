#pragma once
#include "Object3D.h"
#include "TextReader.h"
#include "MapBase.h"

#include "MapStage.h"
#include "MapMove.h"
#include "MapSky.h"
#include "MapWave.h"
#include "MapTree.h"
#include "MapFluid.h"

class MapManager : public MapBase
{
public:
	MapManager();
	virtual ~MapManager();

	/// <summary>
	/// �X�N���v�g�e�L�X�g�̎w��ɂ��}�b�v���쐬����
	/// </summary>
	/// <param name="txt">�e�L�X�g���[�_�[�I�u�W�F�N�g</param>
	/// <param name="n">�w��s�ʒu</param>
	void MakeMap(TextReader* txt, int n );

	/// <summary>
	/// �ʏ�}�b�v�E�ړ��}�b�v�Ɛ����Ƃ̓����蔻����s��
	/// �n�_�����ԋ߂��|���S���̓����蔻�����Ԃ��܂�
	/// �|���S���̕\�ʂ̂ݔ��肵�A���ʂ͔��肵�܂���
	/// </summary>
	/// <param name="startIn">�����̎n�_</param>
	/// <param name="endIn">�����̏I�_</param>
	/// <param name="vHit">���������ʒu������ꏊ(Out)</param>
	/// <param name="vNormal">���������ʒu�̖@��������ꏊ(Out)</param>
	/// <returns>�������Ă����true</returns>
	bool IsCollisionLay(const VECTOR3& startIn, const VECTOR3& endIn, VECTOR3& hit, VECTOR3& normal);

	/// <summary>
	/// �ʏ�}�b�v�E�ړ��}�b�v�Ƌ��̂Ƃ̓����蔻����s��
	/// �ړ��J�n�����ԋ߂��|���S���̓����蔻�����Ԃ��܂�
	/// </summary>
	/// <param name="startIn">�ړ��J�n�_</param>
	/// <param name="endIn">�ړ��I���_</param>
	/// <param name="fRadius">���a</param>
	/// <param name="vHit">���������ʒu������ꏊ(Out)</param>
	/// <param name="vNormal">���������ʒu�̖@��������ꏊ(Out)</param>
	/// <returns>�������Ă����true</returns>
	bool IsCollisionSphere(const VECTOR3& startIn, const VECTOR3& endIn, const float& radius, VECTOR3& hit, VECTOR3& normal);

	/// <summary>
	/// �ʏ�}�b�v�E�ړ��}�b�v�Ƌ��̂̈ړ������Ƃ̏d�͂��������������蔻����s��
	/// �ړ��J�n�_�����ԋ߂��|���S���œ����蔻����s���܂�
	/// �������Ă����ꍇ�A���̂��|���S���̊O�ɏo��悤�Ɉړ��I���_position�̒l���ύX����܂�
	/// </summary>
	/// <param name="positionOld">�ړ��O�_</param>
	/// <param name="position">�ړ���_(In/Out)</param>
	/// <param name="fRadius">���a</param>
	/// <returns>�R���W��������̌���</returns>
	CollRet IsCollisionMoveGravity(const VECTOR3& positionOld, VECTOR3& position, float radius = 0.2f);

	/// �ʏ�}�b�v�E�ړ��}�b�v�Ƌ��̂̈ړ������Ƃ̏d�͂��������������蔻����s��
	/// �ړ��J�n�_�����ԋ߂��|���S���œ����蔻����s���܂�
	/// �������Ă����ꍇ�A���̂��|���S���̊O�ɏo��悤�Ɉړ��I���_position�̒l���ύX����܂�
	/// </summary>
	/// <param name="positionOld">�ړ��O�_</param>
	/// <param name="position">�ړ���_(In/Out)</param>
	/// <param name="vHit">���������ʒu������ꏊ(Out)</param>
	/// <param name="vNormal">���������ʒu�̖@��������ꏊ(Out)</param>
	/// <param name="fRadius">���a</param>
	/// <returns>�R���W��������̌���</returns>
	CollRet IsCollisionMoveGravity(const VECTOR3& positionOld, VECTOR3& position, VECTOR3& hit, VECTOR3& normal, float radius = 0.2f);

	/// <summary>
	/// �ʏ�}�b�v�E�ړ��}�b�v�Ƌ��̂̈ړ������Ƃ̓����蔻����s��
	/// �ړ��J�n�_�����ԋ߂��|���S���œ����蔻����s���܂�
	/// �������Ă����ꍇ�A���̂��|���S���̊O�ɏo��悤�Ɉړ��I���_position�̒l���ύX����܂�
	/// </summary>
	/// <param name="positionOld">�ړ��O�_</param>
	/// <param name="position">�ړ���_(In/Out)</param>
	/// <param name="fRadius">���a</param>
	/// <returns>�������Ă����true</returns>
	bool IsCollisionMove(const VECTOR3& positionOld, VECTOR3& position, float radius = 0.2f);

	/// <summary>
	/// �ʏ�}�b�v�E�ړ��}�b�v�Ƌ��̂̈ړ������Ƃ̓����蔻����s��
	/// �ړ��J�n�_�����ԋ߂��|���S���̓����蔻�����Ԃ��܂�
	/// �������Ă����ꍇ�A���̂��|���S���̊O�ɏo��悤�Ɉړ��I���_position�̒l���ύX����܂�
	/// </summary>
	/// <param name="positionOld">�ړ��O�_</param>
	/// <param name="position">�ړ���_(In/Out)</param>
	/// <param name="vHit">���������ʒu������ꏊ(Out)</param>
	/// <param name="vNormal">���������ʒu�̖@��������ꏊ(Out)</param>
	/// <param name="fRadius">���a</param>
	/// <returns>�������Ă����true</returns>
	bool IsCollisionMove(const VECTOR3& positionOld, VECTOR3& position, VECTOR3& hit, VECTOR3& normal, float radius = 0.2f);

	CCollision* ColMesh() { return colMesh; }

private:
	CCollision* colMesh;	// �ÓI�R���W�������܂Ƃ߂��R���W�������b�V��

};