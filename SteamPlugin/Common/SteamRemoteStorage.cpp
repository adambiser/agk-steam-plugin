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

#include "Common.h"
#include "SteamRemoteStorage.h"

struct RemoteStorageFileReadAsyncCompleteResponse
{
	int m_MemblockID;
	EResult m_eResult;

	RemoteStorageFileReadAsyncCompleteResponse() : m_MemblockID(0), m_eResult((EResult)0) {};

	// This performs the weight of the OnResponse code.
	RemoteStorageFileReadAsyncCompleteResponse(RemoteStorageFileReadAsyncComplete_t value)
	{
		m_MemblockID = 0;
		m_eResult = value.m_eResult;
		if (value.m_eResult == k_EResultOK)
		{
			agk::Log("Creating memblock...");
			utils ::Log("Length: " + std::to_string(value.m_cubRead));
			m_MemblockID = agk::CreateMemblock(value.m_cubRead);
			utils::Log("memblock: " + std::to_string(m_MemblockID));
			if (!SteamRemoteStorage()->FileReadAsyncComplete(value.m_hFileReadAsync, agk::GetMemblockPtr(m_MemblockID), value.m_cubRead))
			{
				agk::DeleteMemblock(m_MemblockID);
				m_MemblockID = 0;
				agk::Log("FileReadAsyncComplete failed.");
				m_eResult = k_EResultFail;
			}
		}
	}

	~RemoteStorageFileReadAsyncCompleteResponse()
	{
		//if (m_MemblockID && agk::GetMemblockExists(m_MemblockID))
		//{
			agk::Log("struct Destroyed!");
		//	agk::DeleteMemblock(m_MemblockID);
		//	m_MemblockID = 0;
		//}
	}
};

//template class CCallResultEx<RemoteStorageFileReadAsyncComplete_t, RemoteStorageFileReadAsyncCompleteResponse>;
class CFileReadAsyncCallResult2 : public CCallResultEx<RemoteStorageFileReadAsyncComplete_t, RemoteStorageFileReadAsyncCompleteResponse>
{
public:
	CFileReadAsyncCallResult2(const char *pchFile, uint32 nOffset, uint32 cubToRead) :
		CCallResultEx<RemoteStorageFileReadAsyncComplete_t, RemoteStorageFileReadAsyncCompleteResponse>(),
		m_FileName(pchFile),
		m_nOffset(nOffset),
		m_cubToRead(cubToRead)
	{
		m_CallResultName = "FileReadAsync("
			+ m_FileName + ", "
			+ std::to_string(m_nOffset) + ", "
			+ std::to_string(m_cubToRead) + ")";
	}
	virtual ~CFileReadAsyncCallResult2()
	{
		if (m_Response.m_MemblockID && agk::GetMemblockExists(m_Response.m_MemblockID))
		{
			agk::Log("Memblock Destroyed!");
			//agk::DeleteMemblock(m_Response.m_MemblockID);
			m_Response.m_MemblockID = 0;
		}
		agk::Log("CFileReadAsyncCallResult2 Destroyed!");
	}
protected:
	SteamAPICall_t CallFunction()
	{
		utils::Log(GetName() + ": calling.");
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


//void CFileReadAsyncCallResult::OnRemoteStorageFileReadAsyncComplete(RemoteStorageFileReadAsyncComplete_t *pResult, bool bFailure)
//{
//	SetResultCode(pResult->m_eResult, bFailure);
//	if (pResult->m_eResult == k_EResultOK) // && !bFailure)
//	{
//		utils::Log(GetName() + ": Succeeded.");
//		m_MemblockID = agk::CreateMemblock(pResult->m_cubRead);
//		if (!SteamRemoteStorage()->FileReadAsyncComplete(pResult->m_hFileReadAsync, agk::GetMemblockPtr(m_MemblockID), pResult->m_cubRead))
//		{
//			utils::Log(GetName() + ": FileReadAsyncComplete failed.");
//			agk::DeleteMemblock(m_MemblockID);
//			m_MemblockID = 0;
//			SetResultCode(k_EResultFail);
//		}
//	}
//	else
//	{
//		utils::Log(GetName() + ": Failed with result " + std::to_string(pResult->m_eResult) + ".");
//	}
//}
//void CFileReadAsyncCallResult::Call()
//{
//	const char *m_pchFile = m_FileName.c_str();
//	// Check for file existence because calling FileReadAsync() on files that have been FileDeleted()
//	// will cause Steamworks report an error in the callback, but subsequent FileReadAsync() attempts
//	// when the file exists again will return k_uAPICallInvalid until the user logs out and back into 
//	// the Steam client.  FileRead() does not have this issue.
//	if (!SteamRemoteStorage()->FileExists(m_pchFile))
//	{
//		throw std::string(GetName() + ": File does not exist.");
//	}
//	m_hSteamAPICall = SteamRemoteStorage()->FileReadAsync(m_pchFile, m_nOffset, m_cubToRead);
//	if (m_hSteamAPICall == k_uAPICallInvalid)
//	{
//		throw std::string(GetName() + ": Call returned k_uAPICallInvalid.");
//	}
//	m_CallResult.Set(m_hSteamAPICall, this, &CFileReadAsyncCallResult::OnRemoteStorageFileReadAsyncComplete);
//}

void CFileWriteAsyncCallResult::OnRemoteStorageFileWriteAsyncComplete(RemoteStorageFileWriteAsyncComplete_t *pResult, bool bFailure)
{
	SetResultCode(pResult->m_eResult);
	if (pResult->m_eResult == k_EResultOK && !bFailure)
	{
		utils::Log(GetName() + ": Succeeded.");
	}
	else
	{
		utils::Log(GetName() + ": Failed with result " + std::to_string(pResult->m_eResult) + ".");
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

//CommitPublishedFileUpdate
//CreatePublishedFileUpdateRequest
//DeletePublishedFile
//EnumeratePublishedFilesByUserAction
//EnumeratePublishedWorkshopFiles
//EnumerateUserPublishedFiles
//EnumerateUserSharedWorkshopFiles
//EnumerateUserSubscribedFiles

int CloudFileDelete(const char *filename)
{
	CheckInitialized(false);
	return SteamRemoteStorage()->FileDelete(filename);
}

int CloudFileExists(const char *filename)
{
	CheckInitialized(false);
	return SteamRemoteStorage()->FileExists(filename);
}

//FileFetch - deprecated

int CloudFileForget(const char *filename)
{
	CheckInitialized(false);
	return SteamRemoteStorage()->FileForget(filename);
}

//FilePersist - deprecated

int CloudFilePersisted(const char *filename)
{
	CheckInitialized(false);
	return SteamRemoteStorage()->FilePersisted(filename);
}

int CloudFileRead(const char *filename)
{
	CheckInitialized(0);
	int fileSize = SteamRemoteStorage()->GetFileSize(filename);
	unsigned int memblock = agk::CreateMemblock(fileSize);
	if (SteamRemoteStorage()->FileRead(filename, agk::GetMemblockPtr(memblock), fileSize))
	{
		return memblock;
	}
	agk::DeleteMemblock(memblock);
	return 0;
}

int CloudFileReadAsync(const char *filename, int offset, int length)
{
	CheckInitialized(0);
	if (length == -1)
	{
		length = SteamRemoteStorage()->GetFileSize(filename);
	}
	return CallResults()->Add(new CFileReadAsyncCallResult2(filename, offset, length));
}

char *GetCloudFileReadAsyncFileName(int hCallResult)
{
	if (auto *callResult = CallResults()->Get<CFileReadAsyncCallResult2>(hCallResult))
	{
		//return callResult->Ge
	}
	return NULL_STRING;
	//return GetCallResultValue<CFileReadAsyncCallResult>(hCallResult, &CFileReadAsyncCallResult::GetResponse);
}

int GetCloudFileReadAsyncMemblock(int hCallResult)
{
	if (auto *callResult = CallResults()->Get<CFileReadAsyncCallResult2>(hCallResult))
	{
		return callResult->GetResponse().m_MemblockID;
	}
	return 0;
	//return GetCallResultValue<CFileReadAsyncCallResult>(hCallResult, &CFileReadAsyncCallResult::GetMemblockID);
}

//FileShare
int CloudFileWrite(const char *filename, int memblockID)
{
	CheckInitialized(false);
	if (agk::GetMemblockExists(memblockID))
	{
		return SteamRemoteStorage()->FileWrite(filename, agk::GetMemblockPtr(memblockID), agk::GetMemblockSize(memblockID));
	}
	return false;
}

int CloudFileWriteAsync(const char *filename, int memblockID)
{
	CheckInitialized(0);
	if (agk::GetMemblockExists(memblockID))
	{
		// The data buffer passed in to FileWriteAsync is immediately copied.  Safe to delete memblock immediately after call.
		return CallResults()->Add(new CFileWriteAsyncCallResult(filename, agk::GetMemblockPtr(memblockID), agk::GetMemblockSize(memblockID)));
	}
	return 0;
}

char *GetCloudFileWriteAsyncFileName(int hCallResult)
{
	return GetCallResultValue<CFileWriteAsyncCallResult>(hCallResult, &CFileWriteAsyncCallResult::GetFileName);
}

//FileWriteStreamCancel
//FileWriteStreamClose
//FileWriteStreamOpen
//FileWriteStreamWriteChunk

int GetCachedUGCCount()
{
	CheckInitialized(0);
	return SteamRemoteStorage()->GetCachedUGCCount();
}

//GetCachedUGCHandle

int GetCloudFileCount()
{
	CheckInitialized(0);
	return SteamRemoteStorage()->GetFileCount();
}

//GetFileListFromServer

//GetFileNameAndSize
char *GetCloudFileName(int index)
{
	CheckInitialized(NULL_STRING);
	int32 pnFileSizeInBytes;
	return utils::CreateString(SteamRemoteStorage()->GetFileNameAndSize(index, &pnFileSizeInBytes));
}

int GetCloudFileSizeByIndex(int index)
{
	CheckInitialized(0);
	int32 pnFileSizeInBytes;
	if (SteamRemoteStorage()->GetFileNameAndSize(index, &pnFileSizeInBytes))
	{
		return pnFileSizeInBytes;
	}
	return 0;
}

int GetCloudFileSize(const char *filename)
{
	CheckInitialized(0);
	return SteamRemoteStorage()->GetFileSize(filename);
}

int GetCloudFileTimestamp(const char *filename)
{
	CheckInitialized(0);
	// NOTE: Dangerous conversion int64 to int.
	return (int)SteamRemoteStorage()->GetFileTimestamp(filename);
}

//GetPublishedFileDetails
//GetPublishedItemVoteDetails

int GetCloudQuotaAvailable()
{
	CheckInitialized(0);
	uint64 pnTotalBytes;
	uint64 puAvailableBytes;
	if (SteamRemoteStorage()->GetQuota(&pnTotalBytes, &puAvailableBytes))
	{
		// Note: Potential issue converting uint64 to int if the quote gets to be more than 2GB.
		return (int)puAvailableBytes;
	}
	return 0;
}

int GetCloudQuotaTotal()
{
	CheckInitialized(0);
	uint64 pnTotalBytes;
	uint64 puAvailableBytes;
	if (SteamRemoteStorage()->GetQuota(&pnTotalBytes, &puAvailableBytes))
	{
		// Note: Potential issue converting uint64 to int if the quote gets to be more than 2GB.
		return (int)pnTotalBytes;
	}
	return 0;
}

int GetCloudFileSyncPlatforms(const char *filename)
{
	CheckInitialized(0);
	return SteamRemoteStorage()->GetSyncPlatforms(filename);
}

//GetUGCDetails
//GetUGCDownloadProgress
//GetUserPublishedItemVoteDetails

int IsCloudEnabledForAccount()
{
	CheckInitialized(false);
	return SteamRemoteStorage()->IsCloudEnabledForAccount();
}

int IsCloudEnabledForApp()
{
	CheckInitialized(false);
	return SteamRemoteStorage()->IsCloudEnabledForApp();
}

//PublishVideo
//PublishWorkshopFile
//ResetFileRequestState

void SetCloudEnabledForApp(int enabled)
{
	CheckInitialized(NORETURN);
	return SteamRemoteStorage()->SetCloudEnabledForApp(enabled != 0);
}

int SetCloudFileSyncPlatforms(const char *filename, int eRemoteStoragePlatform)
{
	CheckInitialized(false);
	return SteamRemoteStorage()->SetSyncPlatforms(filename, (ERemoteStoragePlatform)eRemoteStoragePlatform);
}

//SetUserPublishedFileAction
//SubscribePublishedFile
//SynchronizeToClient
//SynchronizeToServer
//UGCDownload
//UGCDownloadToLocation
//UGCRead
//UnsubscribePublishedFile
//UpdatePublishedFileDescription
//UpdatePublishedFileFile
//UpdatePublishedFilePreviewFile
//UpdatePublishedFileSetChangeDescription
//UpdatePublishedFileTags
//UpdatePublishedFileTitle
//UpdatePublishedFileVisibility
//UpdateUserPublishedItemVote
