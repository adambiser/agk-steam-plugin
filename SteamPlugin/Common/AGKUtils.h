/*
Copyright (c) 2019 Adam Biser <adambiser@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#ifndef _AGKUTILS_H_
#define _AGKUTILS_H_
#pragma once

#include "../AGKLibraryCommands.h"
#include <string>
#include <sstream>

// From winnt.h
#ifndef _MAC
typedef wchar_t WCHAR;    // wc,   16-bit UNICODE character
#else
typedef unsigned short WCHAR;    // wc,   16-bit UNICODE character
#endif

namespace utils
{
	/*
	Converts a const char* to agk:string.
	*/
	static inline char *CreateString(const char* text)
	{
		if (text)
		{
			int length = (int)strlen(text) + 1;
			char *result = agk::CreateString(length);
			strcpy_s(result, length, text);
			return result;
		}
		char *result = agk::CreateString(1);
		strcpy_s(result, 1, "");
		return result;
	}

	static inline char *CreateString(std::string text)
	{
		return CreateString(text.c_str());
	}

	static inline char *CreateString(const WCHAR* text)
	{
		size_t length = wcslen(text) + 1;
		char *result = agk::CreateString((int)length);
		size_t size;
		wcstombs_s(&size, result, length, text, length);
		return result;
	}

	static inline char *CreateString(std::ostringstream& stream)
	{
		return CreateString(stream.str());
	}

	static inline void Log(std::string msg)
	{
		agk::Log(msg.c_str());
	}

	static inline void PluginError(std::string msg)
	{
		agk::PluginError(msg.c_str());
	}
};

#endif // _AGKUTILS_H_
