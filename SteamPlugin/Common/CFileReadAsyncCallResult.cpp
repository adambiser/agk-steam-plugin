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

#include "CFileReadAsyncCallResult.h"

void CFileReadAsyncCallResult::OnRemoteStorageFileReadAsyncComplete(RemoteStorageFileReadAsyncComplete_t *pResult, bool bFailure)
{
	m_eResult = pResult->m_eResult;
	if (pResult->m_eResult == k_EResultOK) // && !bFailure)
	{
		utils::Log(GetName() + ": Succeeded.");
		m_MemblockID = agk::CreateMemblock(pResult->m_cubRead);
		if (!SteamRemoteStorage()->FileReadAsyncComplete(pResult->m_hFileReadAsync, agk::GetMemblockPtr(m_MemblockID), pResult->m_cubRead))
		{
			utils::Log(GetName() + ": FileReadAsyncComplete failed.");
			agk::DeleteMemblock(m_MemblockID);
			m_MemblockID = 0;
			m_eResult = k_EResultFail;
		}
	}
	else
	{
		utils::Log(GetName() + ": Failed with result " + std::to_string(pResult->m_eResult) + ".");
	}
}
void CFileReadAsyncCallResult::Call()
{
	const char *m_pchFile = m_FileName.c_str();
	// Check for file existence because calling FileReadAsync() on files that have been FileDeleted()
	// will cause Steamworks report an error in the callback, but subsequent FileReadAsync() attempts
	// when the file exists again will return k_uAPICallInvalid until the user logs out and back into 
	// the Steam client.  FileRead() does not have this issue.
	if (!SteamRemoteStorage()->FileExists(m_pchFile))
	{
		throw std::string(GetName() + ": File does not exist.");
	}
	m_hSteamAPICall = SteamRemoteStorage()->FileReadAsync(m_pchFile, m_nOffset, m_cubToRead);
	if (m_hSteamAPICall == k_uAPICallInvalid)
	{
		throw std::string(GetName() + ": Call returned k_uAPICallInvalid.");
	}
	m_CallResult.Set(m_hSteamAPICall, this, &CFileReadAsyncCallResult::OnRemoteStorageFileReadAsyncComplete);
}
