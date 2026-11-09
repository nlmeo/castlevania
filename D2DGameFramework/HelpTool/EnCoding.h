#pragma once
#ifndef __MY_ENCODING_H__
#define __MY_ENCODING_H__

#include <string>
#include <windows.h>

namespace HelpTool
{
	/**
	 * @brief 编码转换命名空间
	 *
	 * 提供Unicode宽字符与ASCII多字节字符之间的转换功能。
	 */
	namespace EnCodeing
	{
		/**
		 * @brief 将Unicode宽字符转换为ASCII多字节字符
		 *
		 * @param wp 宽字符字符串
		 * @return ASCII字符串
		 */
		inline std::string W2C(const wchar_t* wp)
		{
			if (wp == nullptr) { return std::string(); }
			int wlen = (int)wcslen(wp);
			int slen = WideCharToMultiByte(CP_ACP, 0, wp, wlen, nullptr, 0, nullptr, nullptr);
			char* _char = new char[slen + 1];
			WideCharToMultiByte(CP_ACP, 0, wp, wlen, _char, slen, nullptr, nullptr);
			_char[slen] = '\0';
			std::string retstr = _char;
			delete[] _char;
			return retstr;
		}

		/**
		 * @brief 将ASCII多字节字符转换为Unicode宽字符
		 *
		 * @param sc ASCII字符串
		 * @return 宽字符字符串
		 */
		inline std::wstring C2W(const char* sc)
		{
			int slen = (int)strlen(sc);
			int wlen = MultiByteToWideChar(CP_UTF8, 0, sc, slen, nullptr, 0);
			wchar_t* sw = new wchar_t[wlen + 1];
			MultiByteToWideChar(CP_UTF8, 0, sc, slen, sw, wlen);
			sw[wlen] = 0;
			std::wstring retstr = sw;
			delete[] sw;
			return retstr;
		}
	}
}

#endif