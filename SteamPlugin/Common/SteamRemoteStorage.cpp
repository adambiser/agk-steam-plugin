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

#include "SteamPlugin.h"
#include "CFileReadAsyncCallResult.h"
#include "CFileWriteAsyncCallResult.h"

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

int SteamPlugin::FileReadAsync(const char *pchFile, uint32 nOffset, uint32 cubToRead)
{
	CheckInitialized(SteamRemoteStorage, false);
	CFileReadAsyncCallResult *callResult = new CFileReadAsyncCallResult(pchFile, nOffset, cubToRead);
	callResult->Run();
	return AddCallResultItem(callResult);
}

std::string SteamPlugin::GetFileReadAsyncFileName(int hCallResult)
{
	if (CFileReadAsyncCallResult *callResult = GetCallResultItem<CFileReadAsyncCallResult>(hCallResult))
	{
		return callResult->GetFileName();
	}
	return std::string("");
}

int SteamPlugin::GetFileReadAsyncMemblock(int hCallResult)
{
	if (CFileReadAsyncCallResult *callResult = GetCallResultItem<CFileReadAsyncCallResult>(hCallResult))
	{
		return callResult->GetMemblockID();
	}
	return 0;
}

//SteamAPICall_t SteamPlugin::FileShare(const char *pchFile);

bool SteamPlugin::FileWrite(const char *pchFile, const void *pvData, int32 cubData)
{
	CheckInitialized(SteamRemoteStorage, false);
	return SteamRemoteStorage()->FileWrite(pchFile, pvData, cubData);
}

int SteamPlugin::FileWriteAsync(const char *pchFile, const void *pvData, uint32 cubData)
{
	CheckInitialized(SteamRemoteStorage, false);
	CFileWriteAsyncCallResult *callResult = new CFileWriteAsyncCallResult(pchFile, pvData, cubData);
	callResult->Run();
	return AddCallResultItem(callResult);
}

std::string SteamPlugin::GetFileWriteAsyncFileName(int hCallResult)
{
	if (CFileWriteAsyncCallResult *callResult = GetCallResultItem<CFileWriteAsyncCallResult>(hCallResult))
	{
		return callResult->GetFileName();
	}
	return std::string("");
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

