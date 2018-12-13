/*
Copyright (c) 2018 Adam Biser <adambiser@gmail.com>

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

#ifndef CFILEREADASYNCITEM_H_
#define CFILEREADASYNCITEM_H_

#include "DllMain.h"
#include <steam_api.h>
#include <string>

class CFileReadAsyncItem
{
private:
	std::string m_Filename;
	CCallResult<CFileReadAsyncItem, RemoteStorageFileReadAsyncComplete_t> m_CallResult;
	void OnRemoteStorageFileReadAsyncComplete(RemoteStorageFileReadAsyncComplete_t *pResult, bool bFailure);
public:
	CFileReadAsyncItem();
	virtual ~CFileReadAsyncItem(void);
	// Callback values.
	ECallbackState m_CallbackState;
	EResult m_Result;
	int m_MemblockID;
	// Method call.
	bool Call(const char *pchFile, uint32 nOffset, uint32 cubToRead);
};

#endif // CFILEREADASYNCITEM_H_
