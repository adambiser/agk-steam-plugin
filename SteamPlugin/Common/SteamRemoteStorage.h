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

namespace wrapper
{
	struct RemoteStorageFileReadAsyncComplete_t
	{
		int m_MemblockID;
		EResult m_eResult;

		RemoteStorageFileReadAsyncComplete_t() : m_MemblockID(0), m_eResult((EResult)0) {};

		// This performs the weight of the OnResponse code.
		RemoteStorageFileReadAsyncComplete_t(::RemoteStorageFileReadAsyncComplete_t value) : RemoteStorageFileReadAsyncComplete_t()
		{
			m_eResult = value.m_eResult;
			if (value.m_eResult == k_EResultOK)
			{
				m_MemblockID = agk::CreateMemblock(value.m_cubRead);
				if (!SteamRemoteStorage()->FileReadAsyncComplete(value.m_hFileReadAsync, agk::GetMemblockPtr(m_MemblockID), value.m_cubRead))
				{
					agk::DeleteMemblock(m_MemblockID);
					m_MemblockID = 0;
					agk::Log("FileReadAsyncComplete failed.");
					m_eResult = k_EResultFail;
				}
			}
		}
	};
}

class CFileReadAsyncCallResult2 : public CCallResultItem<RemoteStorageFileReadAsyncComplete_t, wrapper::RemoteStorageFileReadAsyncComplete_t>
{
public:
	CFileReadAsyncCallResult2(const char *pchFile, uint32 nOffset, uint32 cubToRead) :
		m_FileName(pchFile),
		m_nOffset(nOffset),
		m_cubToRead(cubToRead)
	{
		m_CallResultName = "FileReadAsync('" + m_FileName + "', " + std::to_string(m_nOffset) + ", " + std::to_string(m_cubToRead) + ")";
	}
	virtual ~CFileReadAsyncCallResult2()
	{
		if (m_Response.m_MemblockID && agk::GetMemblockExists(m_Response.m_MemblockID))
		{
			agk::DeleteMemblock(m_Response.m_MemblockID);
			m_Response.m_MemblockID = 0;
		}
	}
	std::string GetFileName() { return m_FileName; }
	int GetMemblockID() { return m_Response.m_MemblockID; }
protected:
	SteamAPICall_t CallFunction()
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
		return SteamRemoteStorage()->FileReadAsync(m_pchFile, m_nOffset, m_cubToRead);
	}
private:
	std::string m_FileName;
	uint32 m_nOffset;
	uint32 m_cubToRead;
};

class CFileWriteAsyncCallResult : public CCallResultItem<RemoteStorageFileWriteAsyncComplete_t>
{
public:
	CFileWriteAsyncCallResult(const char *pchFile, const void *pvData, uint32 cubData) :
		m_FileName(pchFile),
		m_pvData(pvData),
		m_cubData(cubData)
	{
		m_CallResultName = "FileWriteAsync("
			+ m_FileName + ", "
			+ "*data, "
			+ std::to_string(m_cubData) + ")";
	}
	std::string GetFileName() { return m_FileName; }
protected:
	SteamAPICall_t CallFunction()
	{
		return SteamRemoteStorage()->FileWriteAsync(m_FileName.c_str(), m_pvData, m_cubData);
	}
private:
	std::string m_FileName;
	const void *m_pvData;
	uint32 m_cubData;
};

#endif // _STEAMREMOTESTORAGE_H_
