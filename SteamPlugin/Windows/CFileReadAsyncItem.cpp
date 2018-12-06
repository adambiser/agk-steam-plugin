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

#define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers
#include "CFileReadAsyncItem.h"
#include "..\AGKLibraryCommands.h"
#include <sstream>

CFileReadAsyncItem::CFileReadAsyncItem() :
	m_CallbackState(Idle),
	m_Result((EResult)0),
	m_MemblockID(0)
{
}

CFileReadAsyncItem::~CFileReadAsyncItem(void)
{
}

bool CFileReadAsyncItem::Call(const char *pchFile, uint32 nOffset, uint32 cubToRead)
{
	if (m_CallbackState == Running)
	{
		return false;
	}
	// Check for file existence because calling FileReadAsync() on files that have been FileDeleted 
	// can cause Steamworks to start a CallResult that never finishes and FileReadAsync() can no longer
	// be used the file until the user logs out and back into the Steam client.
	// FileRead() does not have this issue.
	if (!SteamRemoteStorage()->FileExists(pchFile))
	{
		return false;
	}
	m_Filename = pchFile;
	std::ostringstream msg;
	msg << "FileReadAsync: " << m_Filename << ", nOffset: " << nOffset << ", cubToRead: " << cubToRead;
	agk::Log(msg.str().c_str());
	try
	{
		SteamAPICall_t hSteamAPICall = SteamRemoteStorage()->FileReadAsync(pchFile, nOffset, cubToRead);
		if (hSteamAPICall == k_uAPICallInvalid)
		{
			msg.str(std::string());
			msg << "FileReadAsync returned k_uAPICallInvalid:\npchFile: " << m_Filename << "\nnOffset: " << nOffset << "\ncubToRead: " << cubToRead;
			agk::PluginError(msg.str().c_str());
			m_CallbackState = ClientError;
			return false;
		}
		m_CallResult.Set(hSteamAPICall, this, &CFileReadAsyncItem::OnRemoteStorageFileReadAsyncComplete);
		m_CallbackState = Running;
		agk::Log("CFileReadAsyncItem is running.");
		return true;
	}
	catch (...)
	{
		agk::Log("CFileReadAsyncItem had an error.");
		m_CallbackState = ClientError;
		return false;
	}
}

void CFileReadAsyncItem::OnRemoteStorageFileReadAsyncComplete(RemoteStorageFileReadAsyncComplete_t *pResult, bool bFailure)
{
	m_Result = pResult->m_eResult;
	std::ostringstream msg;
	msg << "OnRemoteStorageFileReadAsyncComplete: " << m_Filename << ".  Result = " << pResult->m_eResult;
	agk::Log(msg.str().c_str());
	if (pResult->m_eResult == k_EResultOK) // && !bFailure)
	{
		m_MemblockID = agk::CreateMemblock(pResult->m_cubRead);
		if (SteamRemoteStorage()->FileReadAsyncComplete(pResult->m_hFileReadAsync, agk::GetMemblockPtr(m_MemblockID), pResult->m_cubRead))
		{
			agk::Log("FileReadAsyncComplete retrieved data.");
			m_CallbackState = Done;
		}
		else
		{
			agk::PluginError("ERROR: FileReadAsyncComplete failed.");
			agk::DeleteMemblock(m_MemblockID);
			m_MemblockID = 0;
			m_CallbackState = ServerError;
		}
	}
	else
	{
		m_CallbackState = ServerError;
	}
}
