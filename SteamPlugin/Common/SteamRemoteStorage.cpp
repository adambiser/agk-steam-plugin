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

#include "DllMain.h"

/* @page ISteamRemoteStorage */

//CommitPublishedFileUpdate - Deprecated 
//CreatePublishedFileUpdateRequest - Deprecated 
//DeletePublishedFile - Deprecated
//EnumeratePublishedFilesByUserAction - Deprecated 
//EnumeratePublishedWorkshopFiles - Deprecated 
//EnumerateUserPublishedFiles - Deprecated 
//EnumerateUserSharedWorkshopFiles - Deprecated 
//EnumerateUserSubscribedFiles - Deprecated 

/*
@desc
Deletes a file from the local disk, and propagates that delete to the cloud.

This is meant to be used when a user actively deletes a file. Use FileForget if you want to remove a file from the Steam Cloud but retain it on the users local disk.

When a file has been deleted it can be re-written with FileWrite to reupload it to the Steam Cloud.
@param filename The name of the file that will be deleted.
@return 1 if the file exists and has been successfully deleted; otherwise, 0 if the file did not exist.
@api ISteamRemoteStorage#FileDelete
*/
extern "C" DLL_EXPORT int CloudFileDelete(const char *filename)
{
	CheckInitialized(false);
	return SteamRemoteStorage()->FileDelete(filename);
}

/*
@desc Checks whether the specified file exists.
@param filename The name of the file.
@return 1 if the file exists; otherwise, 0.
@api ISteamRemoteStorage#FileExists
*/
extern "C" DLL_EXPORT int CloudFileExists(const char *filename)
{
	CheckInitialized(false);
	return SteamRemoteStorage()->FileExists(filename);
}

//FileFetch - deprecated

/*
@desc
Deletes the file from remote storage, but leaves it on the local disk and remains accessible from the API.

When you are out of Cloud space, this can be used to allow calls to FileWrite to keep working without needing to make the user delete files.

How you decide which files to forget are up to you. It could be a simple Least Recently Used (LRU) queue or something more complicated.

Requiring the user to manage their Cloud-ized files for a game, while is possible to do, it is never recommended.
For instance, "Which file would you like to delete so that you may store this new one?" removes a significant advantage of using the Cloud in the first place: its transparency.

Once a file has been deleted or forgotten, calling FileWrite will resynchronize it in the Cloud. Rewriting a forgotten file is the only way to make it persisted again.
@param filename The name of the file that will be forgotten.
@return	1 if the file exists and has been successfully forgotten; otherwise, 0.
@api ISteamRemoteStorage#FileForget
*/
extern "C" DLL_EXPORT int CloudFileForget(const char *filename)
{
	CheckInitialized(false);
	return SteamRemoteStorage()->FileForget(filename);
}

//FilePersist - deprecated

/*
@desc Checks if a specific file is persisted in the steam cloud.
@param filename The name of the file.
@return
1 if the file exists and the file is persisted in the Steam Cloud.
0 if FileForget was called on it and is only available locally.
@api ISteamRemoteStorage#FilePersisted
*/
extern "C" DLL_EXPORT int CloudFilePersisted(const char *filename)
{
	CheckInitialized(false);
	return SteamRemoteStorage()->FilePersisted(filename);
}

/*
@desc Opens a binary file, reads the contents of the file into a memblock, and then closes the file.

**NOTE:** This is a synchronous call and as such is a will block your calling thread on the disk IO, and will also block the SteamAPI,
which can cause other threads in your application to block. To avoid "hitching" due to a busy disk on the client machine using FileReadAsync,
the asynchronous version of this API is recommended.
@param filename The name of the file to read from.
@return
A memblock ID containing the data read from the file.
Returns 0 if the file doesn't exist or the read fails.
@api ISteamRemoteStorage#FileRead
*/
extern "C" DLL_EXPORT int CloudFileRead(const char *filename)
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

#pragma region CFileReadAsyncCallResult
class CFileReadAsyncCallResult : public CCallResultItem<RemoteStorageFileReadAsyncComplete_t>
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
		if (m_MemblockID)
		{
			if (agk::GetMemblockExists(m_MemblockID))
			{
				agk::DeleteMemblock(m_MemblockID);
			}
			m_MemblockID = 0;
		}
	}
	std::string GetFileName() { return m_FileName; }
	int GetMemblockID() { return m_MemblockID; }
protected:
	int m_MemblockID;
	void OnResponse(RemoteStorageFileReadAsyncComplete_t *pCallResult, bool bFailure)
	{
		CCallResultItem::OnResponse(pCallResult, bFailure);
		if (m_Response.m_eResult == k_EResultOK)
		{
			m_MemblockID = agk::CreateMemblock(m_Response.m_cubRead);
			if (!SteamRemoteStorage()->FileReadAsyncComplete(m_Response.m_hFileReadAsync, agk::GetMemblockPtr(m_MemblockID), m_Response.m_cubRead))
			{
				agk::DeleteMemblock(m_MemblockID);
				m_MemblockID = 0;
				utils::Log(GetName() + ": FileReadAsyncComplete failed.");
				SetResultCode(k_EResultFail);
			}
		}
	}
private:
	std::string m_FileName;
};
#pragma endregion


/*
@desc Starts an asynchronous read from a file.

The offset and amount to read should be valid for the size of the file, as indicated by GetCloudFileSize.
@param filename The name of the file to read from.
@param offset The offset in bytes into the file where the read will start from. 0 if you're reading the whole file in one chunk.
@param length The amount of bytes to read starting from the offset. -1 to read the entire file.
@callback-type callresult
@callback-getters GetCloudFileReadAsyncFileName, GetCloudFileReadAsyncMemblock
@return A [call result handle](Callbacks-and-Call-Results#call-results) on success; otherwise 0.
@api ISteamRemoteStorage#FileReadAsync, ISteamRemoteStorage#RemoteStorageFileReadAsyncComplete_t
*/
extern "C" DLL_EXPORT int CloudFileReadAsync(const char *filename, int offset, int length)
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

/*
@desc Returns the file name for the CloudFileReadAsync call.
@param hCallResult A CloudFileReadAsync call result handle.
@return The file name.
@api ISteamRemoteStorage#FileReadAsync, ISteamRemoteStorage#RemoteStorageFileReadAsyncComplete_t
*/
extern "C" DLL_EXPORT char *GetCloudFileReadAsyncFileName(int hCallResult)
{
	return GetCallResultValue(hCallResult, &CFileReadAsyncCallResult::GetFileName);
}

/*
@desc Returns the memblock of the data returned by the CloudFileReadAsync call.

A call result will delete its memblock in DeleteCallResult() so calling code does not need to do so.
@param hCallResult A CloudFileReadAsync call result handle.
@return A memblock ID.
@api ISteamRemoteStorage#FileReadAsync, ISteamRemoteStorage#RemoteStorageFileReadAsyncComplete_t
*/
extern "C" DLL_EXPORT int GetCloudFileReadAsyncMemblock(int hCallResult)
{
	return GetCallResultValue(hCallResult, &CFileReadAsyncCallResult::GetMemblockID);
}

#pragma region CFileShareCallResult
class CFileShareCallResult : public CCallResultItem<RemoteStorageFileShareResult_t>
{
public:
	CFileShareCallResult(const char *pchFile)
	{
		m_CallResultName = "FileShare('" + std::string(pchFile) + "')";
		m_hSteamAPICall = SteamRemoteStorage()->FileShare(pchFile);
	}
	UGCHandle_t GetUGCHandle() { return m_Response.m_hFile; }
	char *GetFileName() { return m_Response.m_rgchFilename; }
};
#pragma endregion

/*
@desc Shares a file with users and features.
@param filename The file to share.
@callback-type callresult
@callback-getters GetCloudFileShareUGCHandle, GetCloudFileShareFileName
@return A [call result handle](Callbacks-and-Call-Results#call-results) on success; otherwise 0.
@api ISteamRemoteStorage#FileShare, ISteamRemoteStorage#RemoteStorageFileShareResult_t
*/
extern "C" DLL_EXPORT int CloudFileShare(const char *filename)
{
	CheckInitialized(0);
	return CallResults()->Add(new CFileShareCallResult(filename));
}

/*
@desc Returns the UGC handle for the CloudFileShare call.
@param hCallResult A CloudFileShare call result handle.
@return A UGC handle.
@api ISteamRemoteStorage#FileShare, ISteamRemoteStorage#RemoteStorageFileShareResult_t
*/
extern "C" DLL_EXPORT int GetCloudFileShareUGCHandle(int hCallResult)
{
	return GetCallResultValue(hCallResult, &CFileShareCallResult::GetUGCHandle);
}

/*
@desc Returns the file name for the CloudFileShare call.
@param hCallResult A CloudFileShare call result handle.
@return The file name.
@api ISteamRemoteStorage#FileShare, ISteamRemoteStorage#RemoteStorageFileShareResult_t
*/
extern "C" DLL_EXPORT char *GetCloudFileShareFileName(int hCallResult)
{
	return GetCallResultValue(hCallResult, &CFileShareCallResult::GetFileName);
}

/*
@desc
Creates a new file, writes the memblock to the file, and then closes the file. If the target file already exists, it is overwritten.

**NOTE:** This is a synchronous call and as such is a will block your calling thread on the disk IO, and will also block the SteamAPI,
which can cause other threads in your application to block. To avoid "hitching" due to a busy disk on the client machine using FileWriteAsync,
the asynchronous version of this API is recommended.
@param filename The name of the file to write to.
@param memblockID A memblock of data to write to the file.
@return 1 if the write was successful.

Otherwise, 0 under the following conditions:
* The file you're trying to write is larger than 100MiB as defined by k_unMaxCloudFileChunkSize.
* The memblock is empty.
* You tried to write to an invalid path or filename.
* The current user's Steam Cloud storage quota has been exceeded. They may have run out of space, or have too many files.
* Steam could not write to the disk, the location might be read-only.
@api ISteamRemoteStorage#FileWrite
*/
extern "C" DLL_EXPORT int CloudFileWrite(const char *filename, int memblockID)
{
	CheckInitialized(false);
	if (agk::GetMemblockExists(memblockID))
	{
		return SteamRemoteStorage()->FileWrite(filename, agk::GetMemblockPtr(memblockID), agk::GetMemblockSize(memblockID));
	}
	return false;
}

#pragma region CFileWriteAsyncCallResult
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
#pragma endregion

/*
@desc Creates a new file and asynchronously writes the raw byte data to the Steam Cloud, and then closes the file. If the target file already exists, it is overwritten.

The data in memblock is copied and the memblock can be deleted immediately after calling this method.
@param filename The name of the file to write to.
@param memblockID The memblock containing the data to write to the file.
@callback-type callresult
@callback-getters GetCloudFileWriteAsyncFileName
@return A [call result handle](Callbacks-and-Call-Results#call-results) on success; otherwise 0.
@api ISteamRemoteStorage#FileWriteAsync, ISteamRemoteStorage#RemoteStorageFileWriteAsyncComplete_t
*/
extern "C" DLL_EXPORT int CloudFileWriteAsync(const char *filename, int memblockID)
{
	CheckInitialized(0);
	if (agk::GetMemblockExists(memblockID))
	{
		// The data buffer passed in to FileWriteAsync is immediately copied.  Safe to delete memblock immediately after call.
		return CallResults()->Add(new CFileWriteAsyncCallResult(filename, agk::GetMemblockPtr(memblockID), agk::GetMemblockSize(memblockID)));
	}
	return 0;
}

/*
@desc Returns the file name for the CloudFileWriteAsync call.
@param hCallResult A CloudFileReadAsync [call result handle](Callbacks-and-Call-Results#call-results).
@return The file name.
*/
extern "C" DLL_EXPORT char *GetCloudFileWriteAsyncFileName(int hCallResult)
{
	return GetCallResultValue(hCallResult, &CFileWriteAsyncCallResult::GetFileName);
}

/*
@desc Cancels a file write stream that was started by FileWriteStreamOpen.

This trashes all of the data written and closes the write stream, but if there was an existing file with this name, it remains untouched.
@param writeHandle The file write stream to cancel.
@return 1 or 0.  Steamworks SDK docs don't explain.
@api ISteamRemoteStorage#FileWriteStreamCancel
*/
extern "C" DLL_EXPORT int CloudFileWriteStreamCancel(int writeHandle)
{
	CheckInitialized(0);
	return SteamRemoteStorage()->FileWriteStreamCancel(SteamHandles()->GetSteamHandle(writeHandle));
}

/*
@desc Closes a file write stream that was started by FileWriteStreamOpen.

This flushes the stream to the disk, overwriting the existing file if there was one.
@param writeHandle The file write stream to close.
@return 1 if the file write stream was successfully closed; otherwise 0.
@api ISteamRemoteStorage#FileWriteStreamClose
*/
extern "C" DLL_EXPORT int CloudFileWriteStreamClose(int writeHandle)
{
	CheckInitialized(0);
	return SteamRemoteStorage()->FileWriteStreamClose(SteamHandles()->GetSteamHandle(writeHandle));
}

/*
@desc Creates a new file output stream allowing you to stream out data to the Steam Cloud file in chunks.
If the target file already exists, it is not overwritten until FileWriteStreamClose has been called.
@param filename The name of the file to write to.
@return A file write stream handle or -1 if there's a problem.
@api ISteamRemoteStorage#FileWriteStreamOpen
*/
extern "C" DLL_EXPORT int CloudFileWriteStreamOpen(const char *filename)
{
	CheckInitialized(-1);
	return SteamHandles()->GetPluginHandle(SteamRemoteStorage()->FileWriteStreamOpen(filename));
}

/*
@desc Writes a blob of data from a memblock to the file write stream.
Data size is restricted to 100 * 1024 * 1024 bytes.
@param writeHandle The file write stream to write to.
@param memblockID A memblock containing the data to write.
@param offset The offset within the memblock of the data to write.
@param length The length of the data to write.
@return 1 if the data was successfully written to the file write stream; otherwise 0;
@api ISteamRemoteStorage#FileWriteStreamWriteChunk
@plugin-name CloudFileWriteStreamWriteChunk
*/
extern "C" DLL_EXPORT int CloudFileWriteStreamWriteChunkEx(int writeHandle, int memblockID, int offset, int length)
{
	CheckInitialized(false);
	if (memblockID == 0 || !agk::GetMemblockExists(memblockID))
	{
		agk::PluginError("CloudFileWriteStreamWriteChunk: Invalid memblock.");
		return 0;
	}
	if (offset + length > agk::GetMemblockSize(memblockID))
	{
		agk::PluginError("CloudFileWriteStreamWriteChunk: Tried to read data beyond memblock bounds.");
		return 0;
	}
	return SteamRemoteStorage()->FileWriteStreamWriteChunk(SteamHandles()->GetSteamHandle(writeHandle), agk::GetMemblockPtr(memblockID) + offset, length);
}

/*
@desc Writes the entire contents of a memblock to the file write stream.
Data size is restricted to 100 * 1024 * 1024 bytes.
@param writeHandle The file write stream to write to.
@param memblockID A memblock containing the data to write.
@return 1 if the data was successfully written to the file write stream; otherwise 0;
@api ISteamRemoteStorage#FileWriteStreamWriteChunk
*/
extern "C" DLL_EXPORT int CloudFileWriteStreamWriteChunk(int writeHandle, int memblockID)
{
	return CloudFileWriteStreamWriteChunkEx(writeHandle, memblockID, 0, agk::GetMemblockSize(memblockID));
}

extern "C" DLL_EXPORT int GetCloudCachedUGCCount() // no information on this
{
	CheckInitialized(0);
	return SteamRemoteStorage()->GetCachedUGCCount();
}

extern "C" DLL_EXPORT int GetCloudCachedUGCHandle(int index) // no information on this
{
	CheckInitialized(0);
	return SteamHandles()->GetPluginHandle(SteamRemoteStorage()->GetCachedUGCHandle(index));
}

/*
@desc Gets the total number of local files synchronized by Steam Cloud.
@return The number of files present for the current user, including files in subfolders.
@api ISteamRemoteStorage#GetFileCount
*/
extern "C" DLL_EXPORT int GetCloudFileCount()
{
	CheckInitialized(0);
	return SteamRemoteStorage()->GetFileCount();
}

//GetFileListFromServer - deprecated

/*
@desc Gets the file name at the given index
@param index The index of the file, this should be between 0 and GetFileCount.
@return The name of the file at the given index.  An empty string if the file doesn't exist.
@api ISteamRemoteStorage#GetFileCount, ISteamRemoteStorage#GetFileNameAndSize
*/
extern "C" DLL_EXPORT char *GetCloudFileName(int index)
{
	CheckInitialized(NULL_STRING);
	int32 pnFileSizeInBytes;
	return utils::CreateString(SteamRemoteStorage()->GetFileNameAndSize(index, &pnFileSizeInBytes));
}

/*
@desc Gets the file size at the given index
@param index The index of the file, this should be between 0 and GetFileCount.
@return The name of the file at the given index.  An empty string if the file doesn't exist.
@api ISteamRemoteStorage#GetFileCount, ISteamRemoteStorage#GetFileNameAndSize
@plugin-name GetCloudFileSize
*/
extern "C" DLL_EXPORT int GetCloudFileSizeByIndex(int index)
{
	CheckInitialized(0);
	int32 pnFileSizeInBytes;
	if (SteamRemoteStorage()->GetFileNameAndSize(index, &pnFileSizeInBytes))
	{
		return pnFileSizeInBytes;
	}
	return 0;
}

/*
@desc Gets the specified file's size in bytes.
@param filename 	The name of the file.
@return The size of the file in bytes. Returns 0 if the file does not exist.
@api ISteamRemoteStorage#GetFileSize
*/
extern "C" DLL_EXPORT int GetCloudFileSize(const char *filename)
{
	CheckInitialized(0);
	return SteamRemoteStorage()->GetFileSize(filename);
}

/*
@desc Gets the specified file's last modified timestamp in Unix epoch format (seconds since Jan 1st 1970).
@param filename The name of the file.
@return The last modified timestamp in Unix epoch format.
@api ISteamRemoteStorage#GetFileTimestamp
*/
extern "C" DLL_EXPORT int GetCloudFileTimestamp(const char *filename)
{
	CheckInitialized(0);
	// NOTE: Dangerous conversion int64 to int.
	return (int)SteamRemoteStorage()->GetFileTimestamp(filename);
}

//GetPublishedFileDetails - deprecated
//GetPublishedItemVoteDetails - deprecated

/*
@desc Gets the number of bytes available in the user's Steam Cloud storage.
@return An integer.
@api ISteamRemoteStorage#GetQuota
*/
extern "C" DLL_EXPORT int GetCloudQuotaAvailable()
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

/*
@desc Gets the total amount of bytes the user has access to in the user's Steam Cloud storage.
@return An integer.
@api ISteamRemoteStorage#GetQuota
*/
extern "C" DLL_EXPORT int GetCloudQuotaTotal()
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

/*
@desc Obtains the platforms that the specified file will syncronize to.
@param filename The name of the file.
@return Bitfield containing the platforms that the file was set to with SetSyncPlatforms.
@api ISteamRemoteStorage#GetSyncPlatforms, ISteamRemoteStorage#ERemoteStoragePlatform
*/
extern "C" DLL_EXPORT int GetCloudFileSyncPlatforms(const char *filename)
{
	CheckInitialized(0);
	return SteamRemoteStorage()->GetSyncPlatforms(filename);
}

extern "C" DLL_EXPORT int GetCloudUGCDetailsAppID(int hUGC) // no info
{
	CheckInitialized(0);
	UGCHandle_t hContent = SteamHandles()->GetSteamHandle(hUGC);
	AppId_t nAppID;
	char *pchName = NULL;
	int32 nFileSizeInBytes;
	CSteamID steamIDOwner;
	if (SteamRemoteStorage()->GetUGCDetails(hContent, &nAppID, &pchName, &nFileSizeInBytes, &steamIDOwner))
	{
		return nAppID;
	}
	return 0;
}

extern "C" DLL_EXPORT char *GetCloudUGCDetailsFileName(int hUGC) // no info
{
	CheckInitialized(NULL_STRING);
	UGCHandle_t hContent = SteamHandles()->GetSteamHandle(hUGC);
	AppId_t nAppID;
	char *pchName = NULL;
	int32 nFileSizeInBytes;
	CSteamID steamIDOwner;
	if (SteamRemoteStorage()->GetUGCDetails(hContent, &nAppID, &pchName, &nFileSizeInBytes, &steamIDOwner))
	{
		return utils::CreateString(pchName);
	}
	return NULL_STRING;
}

extern "C" DLL_EXPORT int GetCloudUGCDetailsFileSize(int hUGC) // no info
{
	CheckInitialized(0);
	UGCHandle_t hContent = SteamHandles()->GetSteamHandle(hUGC);
	AppId_t nAppID;
	char *pchName = NULL;
	int32 nFileSizeInBytes;
	CSteamID steamIDOwner;
	if (SteamRemoteStorage()->GetUGCDetails(hContent, &nAppID, &pchName, &nFileSizeInBytes, &steamIDOwner))
	{
		return nFileSizeInBytes;
	}
	return 0;
}

extern "C" DLL_EXPORT int GetCloudUGCDetailsOwner(int hUGC) // no info
{
	CheckInitialized(0);
	UGCHandle_t hContent = SteamHandles()->GetSteamHandle(hUGC);
	AppId_t nAppID;
	char *pchName = NULL;
	int32 nFileSizeInBytes;
	CSteamID steamIDOwner;
	if (SteamRemoteStorage()->GetUGCDetails(hContent, &nAppID, &pchName, &nFileSizeInBytes, &steamIDOwner))
	{
		return SteamHandles()->GetPluginHandle(steamIDOwner);
	}
	return 0;
}

extern "C" DLL_EXPORT int GetCloudUGCDownloadProgressBytesDownloaded(int hUGC) // no info
{
	CheckInitialized(0);
	UGCHandle_t hContent = SteamHandles()->GetSteamHandle(hUGC);
	int nBytesDownloaded;
	int nBytesExpected;
	if (SteamRemoteStorage()->GetUGCDownloadProgress(hContent, &nBytesDownloaded, &nBytesExpected))
	{
		return nBytesDownloaded;
	}
	return 0;
}

extern "C" DLL_EXPORT int GetCloudUGCDownloadProgressBytesExpected(int hUGC) // no info
{
	CheckInitialized(0);
	UGCHandle_t hContent = SteamHandles()->GetSteamHandle(hUGC);
	int nBytesDownloaded;
	int nBytesExpected;
	if (SteamRemoteStorage()->GetUGCDownloadProgress(hContent, &nBytesDownloaded, &nBytesExpected))
	{
		return nBytesExpected;
	}
	return 0;
}

//GetUserPublishedItemVoteDetails - deprecated

/*
@desc
Checks if the account wide Steam Cloud setting is enabled for this user; or if they disabled it in the Settings->Cloud dialog.

Ensure that you are also checking IsCloudEnabledForApp, as these two options are mutually exclusive.
@return
1 if Steam Cloud is enabled for this account; otherwise, 0.
@api
ISteamRemoteStorage#IsCloudEnabledForAccount

https://partner.steamgames.com/doc/api/ISteamRemoteStorage#IsCloudEnabledForAccount
*/
extern "C" DLL_EXPORT int IsCloudEnabledForAccount()
{
	CheckInitialized(false);
	return SteamRemoteStorage()->IsCloudEnabledForAccount();
}

/*
@desc
Checks if the per game Steam Cloud setting is enabled for this user; or if they disabled it in the Game Properties->Update dialog.

Ensure that you are also checking IsCloudEnabledForAccount, as these two options are mutually exclusive.

It's generally recommended that you allow the user to toggle this setting within your in-game options, you can toggle it with SetCloudEnabledForApp.
@return
1 if Steam Cloud is enabled for this app; otherwise, 0.
@api
ISteamRemoteStorage#IsCloudEnabledForApp
*/
extern "C" DLL_EXPORT int IsCloudEnabledForApp()
{
	CheckInitialized(false);
	return SteamRemoteStorage()->IsCloudEnabledForApp();
}

//PublishVideo - deprecated
//PublishWorkshopFile - deprecated
//ResetFileRequestState - deprecated

/*
@desc
Toggles whether the Steam Cloud is enabled for your application.

This setting can be queried with IsCloudEnabledForApp.

**NOTE:** This must only ever be called as the direct result of the user explicitly requesting that it's enabled or not.
This is typically accomplished with a checkbox within your in-game options.
@param enabled 1 to enabled or 0 to disable the Steam Cloud for this application.
@api
ISteamRemoteStorage#SetCloudEnabledForApp
*/
extern "C" DLL_EXPORT void SetCloudEnabledForApp(int enabled)
{
	CheckInitialized(NORETURN);
	return SteamRemoteStorage()->SetCloudEnabledForApp(enabled != 0);
}

/*
@desc
Allows you to specify which operating systems a file will be synchronized to.

Use this if you have a multiplatform game but have data which is incompatible between platforms.

Files default to k_ERemoteStoragePlatformAll when they are first created. You can use the bitwise OR operator to specify multiple platforms.
@param filename The name of the file.
@param eRemoteStoragePlatform The platforms that the file will be syncronized to.
@param-api eRemoteStoragePlatform ISteamRemoteStorage#ERemoteStoragePlatform
@return 1 if the file exists, otherwise 0.
@api ISteamRemoteStorage#SetSyncPlatforms
*/
extern "C" DLL_EXPORT int SetCloudFileSyncPlatforms(const char *filename, int eRemoteStoragePlatform)
{
	CheckInitialized(false);
	return SteamRemoteStorage()->SetSyncPlatforms(filename, (ERemoteStoragePlatform)eRemoteStoragePlatform);
}

//SetUserPublishedFileAction - deprecated
//SubscribePublishedFile - deprecated
//SynchronizeToClient - deprecated
//SynchronizeToServer - deprecated

#pragma region CDownloadUGCResult
class CDownloadUGCResult : public CCallResultItem<RemoteStorageDownloadUGCResult_t>
{
public:
	CDownloadUGCResult(UGCHandle_t hContent, const char *pchLocation, uint32 unPriority)
	{
		if (pchLocation == NULL)
		{
			m_CallResultName = "UGCDownload(" + std::to_string(hContent) + ", " + std::to_string(unPriority) + ")";
			m_hSteamAPICall = SteamRemoteStorage()->UGCDownload(hContent, unPriority);
		}
		else
		{
			m_CallResultName = "UGCDownloadToLocation("
				+ std::to_string(hContent) + ", "
				"'" + pchLocation + "', "
				+ std::to_string(unPriority) + ")";
			m_hSteamAPICall = SteamRemoteStorage()->UGCDownloadToLocation(hContent, pchLocation, unPriority);
		}
	}
};
#pragma endregion

extern "C" DLL_EXPORT int CloudUGCDownload(int hUGC, int priority)
{
	CheckInitialized(0);
	return CallResults()->Add(new CDownloadUGCResult(SteamHandles()->GetSteamHandle(hUGC), NULL, priority));
}

extern "C" DLL_EXPORT int CloudUGCDownloadToLocation(int hUGC, const char *location, int priority)
{
	CheckInitialized(0);
	return CallResults()->Add(new CDownloadUGCResult(SteamHandles()->GetSteamHandle(hUGC), location, priority));
}

/*
@plugin-name CloudUGCRead
*/
extern "C" DLL_EXPORT int CloudUGCReadEx(int hUGC, int memblockID, int dstOffset, int length, int srcOffset, int eAction)
{
	CheckInitialized(0);
	if (memblockID == 0 || !agk::GetMemblockExists(memblockID))
	{
		agk::PluginError("CloudUGCRead: Invalid memblock.");
		return 0;
	}
	if (dstOffset + length > agk::GetMemblockSize(memblockID))
	{
		agk::PluginError("CloudUGCRead: Tried to write data beyond memblock bounds.");
		return 0;
	}
	return SteamRemoteStorage()->UGCRead(SteamHandles()->GetSteamHandle(hUGC), agk::GetMemblockPtr(memblockID) + dstOffset, length, srcOffset, (EUGCReadAction)eAction);
}

extern "C" DLL_EXPORT int CloudUGCRead(int hUGC, int memblockID, int srcOffset, int eAction)
{
	return CloudUGCReadEx(hUGC, memblockID, 0, agk::GetMemblockSize(memblockID), srcOffset, eAction);
}

//UnsubscribePublishedFile - deprecated
//UpdatePublishedFileDescription - deprecated
//UpdatePublishedFileFile - deprecated
//UpdatePublishedFilePreviewFile - deprecated
//UpdatePublishedFileSetChangeDescription - deprecated
//UpdatePublishedFileTags - deprecated
//UpdatePublishedFileTitle - deprecated
//UpdatePublishedFileVisibility - deprecated
//UpdateUserPublishedItemVote - deprecated
