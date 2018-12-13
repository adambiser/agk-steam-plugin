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
#include "CFileWriteAsyncItem.h"
#include "..\AGKLibraryCommands.h"
#include <sstream>

CFileWriteAsyncItem::CFileWriteAsyncItem() :
	m_CallbackState(Idle),
	m_Result((EResult)0)
{
}

CFileWriteAsyncItem::~CFileWriteAsyncItem(void)
{
}

bool CFileWriteAsyncItem::Call(const char *pchFile, const void *pvData, uint32 cubData)
{
	if (m_CallbackState == Running)
	{
		return false;
	}
	m_Filename = pchFile;
	std::ostringstream msg;
	msg << "FileWriteAsync: " << m_Filename << ", cubData: " << cubData;
	agk::Log(msg.str().c_str());
	try
	{
		SteamAPICall_t hSteamAPICall = SteamRemoteStorage()->FileWriteAsync(pchFile, pvData, cubData);
		if (hSteamAPICall == k_uAPICallInvalid)
		{
			msg.str(std::string());
			msg << "FileWriteAsync returned k_uAPICallInvalid.\n pchFile: " << m_Filename << "\n cubData: " << cubData;
			agk::PluginError(msg.str().c_str());
			m_CallbackState = ClientError;
			return false;
		}
		m_CallResult.Set(hSteamAPICall, this, &CFileWriteAsyncItem::OnRemoteStorageFileWriteAsyncComplete);
		m_CallbackState = Running;
		return true;
	}
	catch (...)
	{
		msg.str(std::string());
		msg << "FileWriteAsync returned k_uAPICallInvalid.\n pchFile: " << m_Filename << "\n cubData: " << cubData;
		agk::PluginError(msg.str().c_str());
		m_CallbackState = ClientError;
		return false;
	}
}

void CFileWriteAsyncItem::OnRemoteStorageFileWriteAsyncComplete(RemoteStorageFileWriteAsyncComplete_t *pResult, bool bFailure)
{
	m_Result = pResult->m_eResult;
	std::ostringstream msg;
	msg << "OnRemoteStorageFileWriteAsyncComplete: " << m_Filename << ".  Result = " << pResult->m_eResult;
	agk::Log(msg.str().c_str());
	if (pResult->m_eResult == k_EResultOK && !bFailure)
	{
		m_CallbackState = Done;
	}
	else
	{
		m_CallbackState = ServerError;
	}
}
