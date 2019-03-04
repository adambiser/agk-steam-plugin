#include "SteamPlugin.h"

// Cloud methods: Information
bool SteamPlugin::IsCloudEnabledForAccount()
{
	CheckInitialized(SteamRemoteStorage, false);
	return SteamRemoteStorage()->IsCloudEnabledForAccount();
}

bool SteamPlugin::IsCloudEnabledForApp()
{
	CheckInitialized(SteamRemoteStorage, false);
	return SteamRemoteStorage()->IsCloudEnabledForApp();
}

void SteamPlugin::SetCloudEnabledForApp(bool bEnabled)
{
	CheckInitialized(SteamRemoteStorage, );
	SteamRemoteStorage()->SetCloudEnabledForApp(bEnabled);
}

// Cloud Methods: Files
bool SteamPlugin::FileDelete(const char *pchFile)
{
	CheckInitialized(SteamRemoteStorage, false);
	return SteamRemoteStorage()->FileDelete(pchFile);
}

bool SteamPlugin::FileExists(const char *pchFile)
{
	CheckInitialized(SteamRemoteStorage, false);
	return SteamRemoteStorage()->FileExists(pchFile);
}

bool SteamPlugin::FileForget(const char *pchFile)
{
	CheckInitialized(SteamRemoteStorage, false);
	return SteamRemoteStorage()->FileForget(pchFile);
}

bool SteamPlugin::FilePersisted(const char *pchFile)
{
	CheckInitialized(SteamRemoteStorage, false);
	return SteamRemoteStorage()->FilePersisted(pchFile);
}

int32 SteamPlugin::FileRead(const char *pchFile, void *pvData, int32 cubDataToRead)
{
	CheckInitialized(SteamRemoteStorage, 0);
	return SteamRemoteStorage()->FileRead(pchFile, pvData, cubDataToRead);
}

CFileReadAsyncItem *SteamPlugin::GetFileReadAsyncItem(std::string filename)
{
	auto search = m_FileReadAsyncItemMap.find(filename);
	if (search != m_FileReadAsyncItemMap.end())
	{
		return search->second;
	}
	return NULL;
}

bool SteamPlugin::FileReadAsync(const char *pchFile, uint32 nOffset, uint32 cubToRead)
{
	CheckInitialized(SteamRemoteStorage, false);
	std::string filename = pchFile;
	CFileReadAsyncItem *item = GetFileReadAsyncItem(filename);
	if (!item)
	{
		item = new CFileReadAsyncItem();
		m_FileReadAsyncItemMap.insert_or_assign(filename, item);
	}
	return item->Call(pchFile, nOffset, cubToRead);
}

ECallbackState SteamPlugin::GetFileReadAsyncCallbackState(const char *pchFile)
{
	std::string filename = pchFile;
	CFileReadAsyncItem *item = GetFileReadAsyncItem(filename);
	if (item)
	{
		return getCallbackState(&item->m_CallbackState);
	}
	return Idle;
}

EResult SteamPlugin::GetFileReadAsyncResult(const char *pchFile)
{
	std::string filename = pchFile;
	CFileReadAsyncItem *item = GetFileReadAsyncItem(filename);
	if (item)
	{
		return item->m_Result;
	}
	return (EResult)0;
}

int SteamPlugin::GetFileReadAsyncMemblock(const char *pchFile)
{
	std::string filename = pchFile;
	CFileReadAsyncItem *item = GetFileReadAsyncItem(filename);
	if (item)
	{
		return item->m_MemblockID;
	}
	return 0;
}

//SteamAPICall_t SteamPlugin::FileShare(const char *pchFile);

bool SteamPlugin::FileWrite(const char *pchFile, const void *pvData, int32 cubData)
{
	CheckInitialized(SteamRemoteStorage, false);
	return SteamRemoteStorage()->FileWrite(pchFile, pvData, cubData);
}

CFileWriteAsyncItem *SteamPlugin::GetFileWriteAsyncItem(std::string filename)
{
	auto search = m_FileWriteAsyncItemMap.find(filename);
	if (search != m_FileWriteAsyncItemMap.end())
	{
		return search->second;
	}
	return NULL;
}

bool SteamPlugin::FileWriteAsync(const char *pchFile, const void *pvData, uint32 cubData)
{
	CheckInitialized(SteamRemoteStorage, false);
	std::string filename = pchFile;
	CFileWriteAsyncItem *item = GetFileWriteAsyncItem(filename);
	if (!item)
	{
		item = new CFileWriteAsyncItem();
		m_FileWriteAsyncItemMap.insert_or_assign(filename, item);
	}
	return item->Call(pchFile, pvData, cubData);
}

ECallbackState SteamPlugin::GetFileWriteAsyncCallbackState(const char *pchFile)
{
	std::string filename = pchFile;
	CFileWriteAsyncItem *item = GetFileWriteAsyncItem(filename);
	if (item)
	{
		return getCallbackState(&item->m_CallbackState);
	}
	return Idle;
}

EResult SteamPlugin::GetFileWriteAsyncResult(const char *pchFile)
{
	std::string filename = pchFile;
	CFileWriteAsyncItem *item = GetFileWriteAsyncItem(filename);
	if (item)
	{
		return item->m_Result;
	}
	return (EResult)0;
}

//bool SteamPlugin::FileWriteStreamCancel(UGCFileWriteStreamHandle_t writeHandle);
//bool SteamPlugin::FileWriteStreamClose(UGCFileWriteStreamHandle_t writeHandle);
//UGCFileWriteStreamHandle_t SteamPlugin::FileWriteStreamOpen(const char *pchFile);
//bool SteamPlugin::FileWriteStreamWriteChunk(UGCFileWriteStreamHandle_t writeHandle, const void *pvData, int32 cubData);

int32 SteamPlugin::GetFileCount()
{
	CheckInitialized(SteamRemoteStorage, 0);
	return SteamRemoteStorage()->GetFileCount();
}

const char * SteamPlugin::GetFileNameAndSize(int iFile, int32 *pnFileSizeInBytes)
{
	CheckInitialized(SteamRemoteStorage, NULL);
	return SteamRemoteStorage()->GetFileNameAndSize(iFile, pnFileSizeInBytes);
}

int32 SteamPlugin::GetFileSize(const char *pchFile)
{
	CheckInitialized(SteamRemoteStorage, 0);
	return SteamRemoteStorage()->GetFileSize(pchFile);
}

int64 SteamPlugin::GetFileTimestamp(const char *pchFile)
{
	CheckInitialized(SteamRemoteStorage, 0);
	return SteamRemoteStorage()->GetFileTimestamp(pchFile);
}

bool SteamPlugin::GetQuota(uint64 *pnTotalBytes, uint64 *puAvailableBytes)
{
	CheckInitialized(SteamRemoteStorage, false);
	return SteamRemoteStorage()->GetQuota(pnTotalBytes, puAvailableBytes);
}

ERemoteStoragePlatform SteamPlugin::GetSyncPlatforms(const char *pchFile)
{
	CheckInitialized(SteamRemoteStorage, k_ERemoteStoragePlatformNone);
	return SteamRemoteStorage()->GetSyncPlatforms(pchFile);
}

bool SteamPlugin::SetSyncPlatforms(const char *pchFile, ERemoteStoragePlatform eRemoteStoragePlatform)
{
	CheckInitialized(SteamRemoteStorage, false);
	return SteamRemoteStorage()->SetSyncPlatforms(pchFile, eRemoteStoragePlatform);
}

// User-Generated Content
int32 SteamPlugin::GetCachedUGCCount()
{
	CheckInitialized(SteamRemoteStorage, 0);
	return SteamRemoteStorage()->GetCachedUGCCount();
}
//UGCHandle_t SteamPlugin::GetCachedUGCHandle(int32 iCachedContent);
//bool SteamPlugin::GetUGCDetails(UGCHandle_t hContent, AppId_t *pnAppID, char **ppchName, int32 *pnFileSizeInBytes, CSteamID *pSteamIDOwner);
//bool SteamPlugin::GetUGCDownloadProgress(UGCHandle_t hContent, int32 *pnBytesDownloaded, int32 *pnBytesExpected);
//SteamAPICall_t SteamPlugin::UGCDownload(UGCHandle_t hContent, uint32 unPriority);
//SteamAPICall_t SteamPlugin::UGCDownloadToLocation(UGCHandle_t hContent, const char *pchLocation, uint32 unPriority);
//int32 SteamPlugin::UGCRead(UGCHandle_t hContent, void *pvData, int32 cubDataToRead, uint32 cOffset, EUGCReadAction eAction);

