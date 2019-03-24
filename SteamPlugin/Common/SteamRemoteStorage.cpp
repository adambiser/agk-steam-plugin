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

//CommitPublishedFileUpdate - Deprecated 
//CreatePublishedFileUpdateRequest - Deprecated 
//DeletePublishedFile - Deprecated
//EnumeratePublishedFilesByUserAction - Deprecated 
//EnumeratePublishedWorkshopFiles - Deprecated 
//EnumerateUserPublishedFiles - Deprecated 
//EnumerateUserSharedWorkshopFiles - Deprecated 
//EnumerateUserSubscribedFiles - Deprecated 

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
	// Check for file existence because calling FileReadAsync() on files that have been FileDeleted()
	// will cause Steamworks report an error in the callback, but subsequent FileReadAsync() attempts
	// when the file exists again will return k_uAPICallInvalid until the user logs out and back into 
	// the Steam client.  FileRead() does not have this issue.
	if (!SteamRemoteStorage()->FileExists(filename))
	{
		utils::Log("CloudFileReadAsync: '" + std::string(filename) + "' does not exist.");
		return 0;
	}
	return CallResults()->Add(new CFileReadAsyncCallResult(filename, offset, length));
}

char *GetCloudFileReadAsyncFileName(int hCallResult)
{
	return GetCallResultValue<CFileReadAsyncCallResult>(hCallResult, &CFileReadAsyncCallResult::GetFileName);
}

int GetCloudFileReadAsyncMemblock(int hCallResult)
{
	return GetCallResultValue<CFileReadAsyncCallResult>(hCallResult, &CFileReadAsyncCallResult::GetMemblockID);
}

//FileShare - no information about what this does.

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

int CloudFileWriteStreamCancel(int writeHandle)
{
	return SteamRemoteStorage()->FileWriteStreamCancel(SteamHandles()->GetSteamHandle(writeHandle));
}

int CloudFileWriteStreamClose(int writeHandle)
{
	return SteamRemoteStorage()->FileWriteStreamClose(SteamHandles()->GetSteamHandle(writeHandle));
}

int CloudFileWriteStreamOpen(const char *filename)
{
	return SteamHandles()->GetPluginHandle(SteamRemoteStorage()->FileWriteStreamOpen(filename));
}

int CloudFileWriteStreamWriteChunkEx(int writeHandle, int memblockID, int offset, int length)
{
	if (memblockID == 0 || !agk::GetMemblockExists(memblockID))
	{
		agk::PluginError("Invalid memblock.");
		return 0;
	}
	if (offset + length > agk::GetMemblockSize(memblockID))
	{
		agk::PluginError("Tried to read data beyond memblock bounds.");
		return 0;
	}
	return SteamRemoteStorage()->FileWriteStreamWriteChunk(SteamHandles()->GetSteamHandle(writeHandle), agk::GetMemblockPtr(memblockID) + offset, length);
}

int CloudFileWriteStreamWriteChunk(int writeHandle, int memblockID)
{
	if (memblockID == 0 || !agk::GetMemblockExists(memblockID))
	{
		agk::PluginError("Invalid memblock.");
		return 0;
	}
	return CloudFileWriteStreamWriteChunkEx(writeHandle, memblockID, 0, agk::GetMemblockSize(memblockID));
}

//int GetCachedUGCCount() - no information on this
//{
//	CheckInitialized(0);
//	return SteamRemoteStorage()->GetCachedUGCCount();
//}

//GetCachedUGCHandle - no information on this

int GetCloudFileCount()
{
	CheckInitialized(0);
	return SteamRemoteStorage()->GetFileCount();
}

//GetFileListFromServer - deprecated

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

//GetPublishedFileDetails - deprecated
//GetPublishedItemVoteDetails - deprecated

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

//GetUGCDetails - no info
//GetUGCDownloadProgress - no info
//GetUserPublishedItemVoteDetails - deprecated

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

//PublishVideo - deprecated
//PublishWorkshopFile - deprecated
//ResetFileRequestState - deprecated

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

//SetUserPublishedFileAction - deprecated
//SubscribePublishedFile - deprecated
//SynchronizeToClient - deprecated
//SynchronizeToServer - deprecated

//UGCDownload - no info
//UGCDownloadToLocation - no info
//UGCRead - no info

//UnsubscribePublishedFile - deprecated
//UpdatePublishedFileDescription - deprecated
//UpdatePublishedFileFile - deprecated
//UpdatePublishedFilePreviewFile - deprecated
//UpdatePublishedFileSetChangeDescription - deprecated
//UpdatePublishedFileTags - deprecated
//UpdatePublishedFileTitle - deprecated
//UpdatePublishedFileVisibility - deprecated
//UpdateUserPublishedItemVote - deprecated
