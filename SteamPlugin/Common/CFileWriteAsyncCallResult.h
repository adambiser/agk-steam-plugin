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

#ifndef _CFILEWRITEASYNCCALLRESULT_H_
#define _CFILEWRITEASYNCCALLRESULT_H_
#pragma once

#include "CCallResultItem.h"

class CFileWriteAsyncCallResult : public CCallResultItem
{
public:
	CFileWriteAsyncCallResult(const char *pchFile, const void *pvData, uint32 cubData) :
		CCallResultItem(),
		m_FileName(pchFile),
		m_pvData(pvData),
		m_cubData(cubData) {}
	virtual ~CFileWriteAsyncCallResult(void)
	{
		m_CallResult.Cancel();
	}
	std::string GetName()
	{
		return "FileWriteAsync("
			+ m_FileName + ", "
			+ "*data, "
			//+ std::to_string(m_nOffset) + ", "
			+ std::to_string(m_cubData) + ")";
	}
	std::string GetFileName() { return m_FileName; }
protected:
	void Call();
private:
	CCallResult<CFileWriteAsyncCallResult, RemoteStorageFileWriteAsyncComplete_t> m_CallResult;
	void OnRemoteStorageFileWriteAsyncComplete(RemoteStorageFileWriteAsyncComplete_t *pResult, bool bFailure);
	std::string m_FileName;
	const void *m_pvData;
	uint32 m_cubData;
};

#endif // CFILEWRITEASYNCCALLRESULT_H_
