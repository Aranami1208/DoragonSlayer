#pragma once
/// <summary>
/// ���ׂẴI�[�f�B�I�I�u�W�F�N�g���Ǘ����܂�
/// �V���O���g���ɂ��Ă���
/// </summary>
/// <author>T.Suzuki</author>

#include <list>
#include <string>
#include "XAudio.h"

/// <summary>
/// �I�[�f�B�I�}�l�[�W���[
/// (�I�[�f�B�I�̊Ǘ����s��)
/// </summary>
namespace AudioManager {
	/// <summary>
	/// �I�[�f�B�I�}�l�[�W���[�̊J�n����
	/// </summary>
	void Start();

	/// <summary>
	/// �I�[�f�B�I�}�l�[�W���[�̏I������
	///  �iaudiolist��S�č폜����j
	/// </summary>
	void Release();

	/// <summary>
	///  audiolist��ǉ�����
	/// </summary>
	/// <param name="name">�I�[�f�B�I�ʖ�</param>
	/// <param name="audioName">�I�[�f�B�I�t�@�C����</param>
	/// <param name="n">�d�ˍ��킹�Đ����B�ȗ��l�P</param>
	void Push(const std::string name, const TCHAR* audioName, int n = 1);

	/// <summary>
	/// audioObjects ����I�[�f�B�I�ʖ����L�[�ɂ��ăI�[�f�B�I�\�[�X��T��
	/// </summary>
	/// <param name="name">�I�[�f�B�I�ʖ�</param>
	/// <returns>�I�[�f�B�I�\�[�X</returns>
	CXAudioSource* Audio(std::string name);

	/// <summary>
	/// audioObjects ����I�[�f�B�I�ʖ����L�[�ɂ��ăI�[�f�B�I�\�[�X���폜����
	/// </summary>
	/// <param name="name">�I�[�f�B�I�ʖ�</param>
	bool Delete(std::string name);
};
