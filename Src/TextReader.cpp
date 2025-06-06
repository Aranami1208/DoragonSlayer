#include "TextReader.h"
#include <fstream>
#include <assert.h>
#include <tchar.h>
#include <codecvt>
#include "MyImgui.h"

namespace {
	const int BUFSIZE = 2048;
};

TextReader::TextReader(tstring filename)
{
	FILE* fp;
	TCHAR	inputBuffer[BUFSIZE];
	TCHAR*  s;
	char	inputBufferU8[BUFSIZE];
	bool u8 = false, ret = false; 
	errno_t error;

	all.clear();

	// --------------------------------------------
	//error = _tfopen_s(&fp, filename.c_str(), _T("r, ccs = UNICODE"));	// �T�[�o�[�X�N���v�g�ǂݍ���(Unicode�œǂݍ���)
	error = _tfopen_s(&fp, filename.c_str(), _T("r"));	// �T�[�o�[�X�N���v�g�ǂݍ���
	if (error == 0 )
	{
		// �t�@�C���̐擪��BOM���`�F�b�N����
		char bom[4];
		fgets(bom, 3 + 1, fp);
		if ((unsigned char)bom[0] == 0xEF && (unsigned char)bom[1] == 0xBB && (unsigned char)bom[2] == 0xBF)
		{
			u8 = true;		// UTF-8�ł���
		}
		else {
			fseek(fp, 0, SEEK_SET);	  // �V�[�N�ʒu��擪�ɖ߂�
		}

		// �ǂݍ���
		while (true)
		{
			if (u8)
			{
				// UTF-8
				if (fgets(inputBufferU8, BUFSIZE, fp) == nullptr)  break;   // �t�@�C���̍Ō�ɒB����܂ň�s�Âǂݍ���
				// TCHAR�ɃR���o�[�g
				MyImgui::ConvertUTF8ToTCHAR(inputBufferU8, inputBuffer);
			}
			else {
				// TCHAR
				if (_fgetts(inputBuffer, BUFSIZE, fp) == nullptr)  break;   // �t�@�C���̍Ō�ɒB����܂ň�s�Âǂݍ���
			}
			ret = true;

			s = _tcsstr(inputBuffer, _T("//"));      // �R�����g�i//�j�̌�̕�������폜����
			if (s != nullptr)
			{
				*s = _T('\0');
			}
			s = _tcsstr(inputBuffer, _T("\n"));      // ������̍Ō��\n���폜����
			if (s != nullptr)
			{
				*s = _T('\0');
			}
			for (int i = 0; i < _tcslen(inputBuffer); i++)		// ���� " ���폜����
			{
				if (inputBuffer[i] == _T('"'))	 inputBuffer[i] = _T(' ');
			}

			if (_tcscmp(inputBuffer, _T("")) == 0 )  continue;	  // ������Ȃ��̍s��ǂݔ�΂�

			int ii;
			for (ii = 0; ii < _tcslen(inputBuffer); ii++)  // �󔒍s��ǂݔ�΂�
			{
				if (inputBuffer[ii] != _T(' ') && inputBuffer[ii] != _T('\t')) break;
			}
			if( ii >= _tcslen(inputBuffer))	  continue;


			// �s���� , �� �^�u �Ő؂蕪����
			LINEREC lineRecord;
			TCHAR str[BUFSIZE] = {};
			int slen = 0;
			bool spt;

			for (int n = 0; n < _tcslen(inputBuffer); n++)
			{
				spt = false;
				while (inputBuffer[n] == _T(' ') || inputBuffer[n] == _T('\t'))	   // �A������󔒂ƃ^�u���P�ɂ܂Ƃ߂�
				{
					spt = true;    // �󔒂��^�u���������Ƃ�
					n++;
				}
				if ( spt && inputBuffer[n] != _T(','))	// �󔒂��^�u����������u �C�v�ȊO�������Ƃ�
				{
					if (slen > 0)		// ���O�ɕ����񂪂������Ƃ�
					{
						str[slen] = _T('\0');
						lineRecord.record.emplace_back(str);	// 1���ڂ��ߍ���
						slen = 0;
					}
					str[slen] = inputBuffer[n];	   // 1������str�ɂ��ߍ���
					slen++;
				}else if (inputBuffer[n] == _T(','))		// �u �C�v�������Ƃ�
				{
					str[slen] = _T('\0');
					lineRecord.record.emplace_back(str);	// ��������1���ڂ��ߍ���
					slen = 0;
				}
				else {
					str[slen] = inputBuffer[n];	   // 1������str�ɂ��ߍ���
					slen++;
				}
			}
			if (slen > 0)
			{
				str[slen] = _T('\0');
				lineRecord.record.emplace_back(str);	// �Ō��1���ڂ��ߍ���
				slen = 0;
			}
			all.emplace_back(lineRecord);		// 1�s���ߍ���
		}
		fclose(fp);
	}
	else {
		MessageBox(0, filename.c_str(), _T("TextReader() �e�L�X�g�ǂݍ��ݏ����@�e�L�X�g�t�@�C��������܂���"), MB_OK);
		assert(false);
	}
}

TextReader::~TextReader()
{
	for (auto rec : all)
		rec.record.clear();
	all.clear();
}

unsigned int TextReader::GetLines()
{
	return (unsigned int)all.size();
}

unsigned int TextReader::GetColumns(unsigned int line)
{

	if (line >= GetLines())
	{
		MessageBox(0, 0, _T("GetColumns() �s���ȍs���w��ł�"), MB_OK);
		assert(false);
	}
	return (unsigned int)all[line].record.size();
}

tstring TextReader::GetString(unsigned int line, unsigned int column)
{
	if (line >= GetLines())
	{
		MessageBox(0, 0, _T("GetString() �s���ȍs���w��ł�"), MB_OK);
		assert(false);
	}
	if (column >= GetColumns(line))
		return _T("");
	return all[line].record[column];
}

int TextReader::GetInt(unsigned int line, unsigned int column)
{
	std::string str = GetString(line, column);
	return std::stoi(str);
}

float TextReader::GetFloat(unsigned int line, unsigned int column)
{
	std::string str = GetString(line, column);
	return std::stof(str);
}
