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

#ifndef _STEAMAPPS_H_
#define _STEAMAPPS_H_
#pragma once

#include "CCallResultItem.h"
#include <sstream>
#include <iomanip>

class CFileDetailsResultCallResult : public CCallResultItem<FileDetailsResult_t>
{
public:
	CFileDetailsResultCallResult(const char *pszFileName) :
		m_FileName(pszFileName)
	{
		m_CallResultName = "GetFileDetails('" + m_FileName + "')";
		m_Response.m_eResult = (EResult)0;
		memset(m_Response.m_FileSHA, 0, sizeof(m_Response.m_FileSHA));
		m_Response.m_ulFileSize = 0;
		m_Response.m_unFlags = 0;
	}
	std::string GetFileSHA1()
	{
		if (m_Response.m_eResult != k_EResultOK)
		{
			return std::string();
		}
		std::ostringstream sha;
		sha << std::uppercase << std::setfill('0') << std::hex;
		for (int x = 0; x < sizeof(m_Response.m_FileSHA); x++)
		{
			sha << std::setw(2) << (int)m_Response.m_FileSHA[x];
		}
		return sha.str();
	}
	int GetFileSize() { return (int)m_Response.m_ulFileSize; }
	int GetFileFlags() { return m_Response.m_unFlags; }
protected:
	SteamAPICall_t CallFunction()
	{
		return SteamApps()->GetFileDetails(m_FileName.c_str());
	}
private:
	std::string m_FileName;
};

#endif // _STEAMAPPS_H_
