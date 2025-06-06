#pragma once
#include <string>
#include <vector>

/// <summary>
// string�̕ʖ�
/// </summary>
typedef
#ifndef UNICODE  
	std::string
#else
	std::wstring
#endif
tstring;

/// <summary>
/// Text��ǂރN���X
/// </summary>
class TextReader {
public:
	TextReader(tstring filename);
	~TextReader();

	/// <summary>
	/// Text�t�@�C���̍s����Ԃ�
	/// </summary>
	/// <returns>�s��</returns>
	unsigned int GetLines();

	/// <summary>
	/// Text�̌���(���ڐ�)��Ԃ�
	/// �s�ɂ���Č���(���ڐ�)���ς��̂ŁA�����ōs���w�肷��
	/// �Ȃ��A�s�ԍ��́A�擪�s��0����
	/// �܂��A�󔒍s��擪//�̍s�͍s���Ɋ܂߂Ȃ�
	/// </summary>
	/// <param name="line">�s�ԍ�</param>
	/// <returns>���̍s�̌���(���ڐ�)</returns>
	unsigned int GetColumns(unsigned int line);

	/// <summary>
	/// �w��ʒu�̕�������擾����
	/// </summary>
	/// <param name="line">�s</param>
	/// <param name="column">��</param>
	/// <returns>������</returns>
	tstring GetString(unsigned int line, unsigned int column);

	/// <summary>
	/// �w��ʒu�̓��e��int�Ŏ擾����
	/// </summary>
	/// <param name="line">�s</param>
	/// <param name="column">��</param>
	/// <returns>�����l</returns>
	int GetInt(unsigned int line, unsigned int column);

	/// <summary>
	/// �w��ʒu�̓��e��float�Ŏ擾����
	/// </summary>
	/// <param name="line">�s</param>
	/// <param name="column">��</param>
	/// <returns>�����l</returns>
	float GetFloat(unsigned int line, unsigned int column);
private:
	struct LINEREC {
		std::vector<tstring> record;
	};
	std::vector<LINEREC> all;
};
