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
// Cloud Methods: Files
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

//UGCHandle_t SteamPlugin::GetCachedUGCHandle(int32 iCachedContent);
//bool SteamPlugin::GetUGCDetails(UGCHandle_t hContent, AppId_t *pnAppID, char **ppchName, int32 *pnFileSizeInBytes, CSteamID *pSteamIDOwner);
//bool SteamPlugin::GetUGCDownloadProgress(UGCHandle_t hContent, int32 *pnBytesDownloaded, int32 *pnBytesExpected);
//SteamAPICall_t SteamPlugin::UGCDownload(UGCHandle_t hContent, uint32 unPriority);
//SteamAPICall_t SteamPlugin::UGCDownloadToLocation(UGCHandle_t hContent, const char *pchLocation, uint32 unPriority);
//int32 SteamPlugin::UGCRead(UGCHandle_t hContent, void *pvData, int32 cubDataToRead, uint32 cOffset, EUGCReadAction eAction);

