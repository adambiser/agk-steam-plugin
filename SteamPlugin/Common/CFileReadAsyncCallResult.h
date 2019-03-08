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

#ifndef _CFILEREADASYNCCALLRESULT_H_
#define _CFILEREADASYNCCALLRESULT_H_
#pragma once

#include "CCallResultItem.h"

class CFileReadAsyncCallResult : public CCallResultItem
{
public:
	CFileReadAsyncCallResult(const char *pchFile, uint32 nOffset, uint32 cubToRead) :
		CCallResultItem(),
		m_FileName(pchFile),
		m_nOffset(nOffset),
		m_cubToRead(cubToRead),
		m_MemblockID(0) {}
	virtual ~CFileReadAsyncCallResult(void)
	{
		if (m_MemblockID && agk::GetMemblockExists(m_MemblockID))
		{
			agk::DeleteMemblock(m_MemblockID);
		}
		m_MemblockID = 0;
		m_CallResult.Cancel();
	}
	std::string GetName()
	{
		return "FileReadAsync(" 
			+ m_FileName + ", "
			+ std::to_string(m_nOffset) + ", "
			+ std::to_string(m_cubToRead) + ")";
	}
	std::string GetFileName() { return m_FileName; }
	int GetMemblockID() { return m_MemblockID; }
protected:
	void Call();
private:
	CCallResult<CFileReadAsyncCallResult, RemoteStorageFileReadAsyncComplete_t> m_CallResult;
	void OnRemoteStorageFileReadAsyncComplete(RemoteStorageFileReadAsyncComplete_t *pResult, bool bFailure);
	std::string m_FileName;
	uint32 m_nOffset;
	uint32 m_cubToRead;
	int m_MemblockID;
};

#endif // _CFILEREADASYNCCALLRESULT_H_
