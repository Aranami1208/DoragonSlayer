#pragma once
#include <string>
#include <vector>

/// <summary>
// stringの別名
/// </summary>
typedef
#ifndef UNICODE  
	std::string
#else
	std::wstring
#endif
tstring;

/// <summary>
/// Textを読むクラス
/// </summary>
class TextReader {
public:
	TextReader(tstring filename);
	~TextReader();

	/// <summary>
	/// Textファイルの行数を返す
	/// </summary>
	/// <returns>行数</returns>
	unsigned int GetLines();

	/// <summary>
	/// Textの桁数(項目数)を返す
	/// 行によって桁数(項目数)が変わるので、引数で行を指定する
	/// なお、行番号は、先頭行を0する
	/// また、空白行や先頭//の行は行数に含めない
	/// </summary>
	/// <param name="line">行番号</param>
	/// <returns>その行の桁数(項目数)</returns>
	unsigned int GetColumns(unsigned int line);

	/// <summary>
	/// 指定位置の文字列を取得する
	/// </summary>
	/// <param name="line">行</param>
	/// <param name="column">桁</param>
	/// <returns>文字列</returns>
	tstring GetString(unsigned int line, unsigned int column);

	/// <summary>
	/// 指定位置の内容をintで取得する
	/// </summary>
	/// <param name="line">行</param>
	/// <param name="column">桁</param>
	/// <returns>整数値</returns>
	int GetInt(unsigned int line, unsigned int column);

	/// <summary>
	/// 指定位置の内容をfloatで取得する
	/// </summary>
	/// <param name="line">行</param>
	/// <param name="column">桁</param>
	/// <returns>小数値</returns>
	float GetFloat(unsigned int line, unsigned int column);
private:
	struct LINEREC {
		std::vector<tstring> record;
	};
	std::vector<LINEREC> all;
};
