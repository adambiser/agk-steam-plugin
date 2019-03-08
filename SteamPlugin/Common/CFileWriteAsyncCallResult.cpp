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

#include "CFileWriteAsyncCallResult.h"

void CFileWriteAsyncCallResult::OnRemoteStorageFileWriteAsyncComplete(RemoteStorageFileWriteAsyncComplete_t *pResult, bool bFailure)
{
	if (pResult->m_eResult == k_EResultOK && !bFailure)
	{
		utils::Log(GetName() + ": Succeeded.");
		m_State = Done;
	}
	else
	{
		utils::Log(GetName() + ": Failed with result " + std::to_string(pResult->m_eResult) + ".");
		m_State = ServerError;
	}
}

void CFileWriteAsyncCallResult::Call()
{
	m_hSteamAPICall = SteamRemoteStorage()->FileWriteAsync(m_FileName.c_str(), m_pvData, m_cubData);
	if (m_hSteamAPICall == k_uAPICallInvalid)
	{
		//throw std::string(GetName() + ": File does not exist.");
		throw std::string(GetName() + ": Call returned k_uAPICallInvalid.");
	}
	m_CallResult.Set(m_hSteamAPICall, this, &CFileWriteAsyncCallResult::OnRemoteStorageFileWriteAsyncComplete);
}
