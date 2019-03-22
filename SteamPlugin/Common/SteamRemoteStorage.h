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

#ifndef _STEAMREMOTESTORAGE_H_
#define _STEAMREMOTESTORAGE_H_
#pragma once

#include "CCallResultItem.h"

struct PluginFileReadAsyncComplete_t : RemoteStorageFileReadAsyncComplete_t
{
	int m_MemblockID;

	PluginFileReadAsyncComplete_t& operator=(const RemoteStorageFileReadAsyncComplete_t &from)
	{
		m_cubRead = from.m_cubRead;
		m_eResult = from.m_eResult;
		m_hFileReadAsync = from.m_hFileReadAsync;
		m_nOffset = from.m_nOffset;
		m_MemblockID = 0;
		return *this;
	}
};

class CFileReadAsyncCallResult : public CCallResultItem<RemoteStorageFileReadAsyncComplete_t, PluginFileReadAsyncComplete_t>
{
public:
	CFileReadAsyncCallResult(const char *pchFile, uint32 nOffset, uint32 cubToRead) :
		m_FileName(pchFile)
	{
		m_CallResultName = "FileReadAsync('" + std::string(pchFile) + "', " + std::to_string(nOffset) + ", " + std::to_string(cubToRead) + ")";
		// Check for file existence because calling FileReadAsync() on files that have been FileDeleted()
		// will cause Steamworks report an error in the callback, but subsequent FileReadAsync() attempts
		// when the file exists again will return k_uAPICallInvalid until the user logs out and back into 
		// the Steam client.  FileRead() does not have this issue.
		if (!SteamRemoteStorage()->FileExists(pchFile))
		{
			utils::Log(GetName() + ": File does not exist.");
			return;
		}
		m_hSteamAPICall = SteamRemoteStorage()->FileReadAsync(pchFile, nOffset, cubToRead);
	}
	virtual ~CFileReadAsyncCallResult()
	{
		if (m_Response.m_MemblockID)
		{
			if (agk::GetMemblockExists(m_Response.m_MemblockID))
			{
				agk::DeleteMemblock(m_Response.m_MemblockID);
			}
			m_Response.m_MemblockID = 0;
		}
	}
	std::string GetFileName() { return m_FileName; }
	int GetMemblockID() { return m_Response.m_MemblockID; }
protected:
	void OnResponse(RemoteStorageFileReadAsyncComplete_t *pCallResult, bool bFailure)
	{
		CCallResultItem::OnResponse(pCallResult, bFailure);
		if (m_Response.m_eResult == k_EResultOK)
		{
			m_Response.m_MemblockID = agk::CreateMemblock(m_Response.m_cubRead);
			if (!SteamRemoteStorage()->FileReadAsyncComplete(m_Response.m_hFileReadAsync, agk::GetMemblockPtr(m_Response.m_MemblockID), m_Response.m_cubRead))
			{
				agk::DeleteMemblock(m_Response.m_MemblockID);
				m_Response.m_MemblockID = 0;
				utils::Log(GetName() + ": FileReadAsyncComplete failed.");
				m_Response.m_eResult = k_EResultFail;
			}
		}
	}
private:
	std::string m_FileName;
};

class CFileWriteAsyncCallResult : public CCallResultItem<RemoteStorageFileWriteAsyncComplete_t>
{
public:
	CFileWriteAsyncCallResult(const char *pchFile, const void *pvData, uint32 cubData) :
		m_FileName(pchFile)
	{
		m_CallResultName = "FileWriteAsync('" + std::string(pchFile) + "', *data, " + std::to_string(cubData) + ")";
		m_hSteamAPICall = SteamRemoteStorage()->FileWriteAsync(pchFile, pvData, cubData);
	}
	std::string GetFileName() { return m_FileName; }
private:
	std::string m_FileName;
};

#endif // _STEAMREMOTESTORAGE_H_
