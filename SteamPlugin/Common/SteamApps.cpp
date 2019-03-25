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
#include "SteamApps.h"

/* @page ISteamApps */

/*
@desc Returns the App ID of a DLC by index.
@param index The index.  Should be between 0 and GetDLCCount() - 1.
@return The AppID if the index is valid; otherwise 0.
@api ISteamApps#GetDLCCount, ISteamApps#BGetDLCDataByIndex
*/
extern "C" DLL_EXPORT int GetDLCDataByIndexAppID(int index)
{
	AppId_t appID;
	bool available;
	char name[128];
	if (SteamApps()->BGetDLCDataByIndex(index, &appID, &available, name, sizeof(name)))
	{
		return appID;
	}
	return 0;
}

/*
@desc Returns whether the DLC at the given index is available.
@param index The index.  Should be between 0 and GetDLCCount() - 1.
@return 1 if available; otherwise 0.
@api ISteamApps#GetDLCCount, ISteamApps#BGetDLCDataByIndex
*/
extern "C" DLL_EXPORT int GetDLCDataByIndexAvailable(int index)
{
	AppId_t appID;
	bool available;
	char name[128];
	if (SteamApps()->BGetDLCDataByIndex(index, &appID, &available, name, sizeof(name)))
	{
		return available;
	}
	return 0;
}

/*
@desc Returns the name of the DLC at the given index.
@param index The index.  Should be between 0 and GetDLCCount() - 1.
@return The DLC name if the index is valid; otherwise and empty string.
@api ISteamApps#GetDLCCount, ISteamApps#BGetDLCDataByIndex
*/
extern "C" DLL_EXPORT char *GetDLCDataByIndexName(int index)
{
	AppId_t appID;
	bool available;
	char name[128];
	if (SteamApps()->BGetDLCDataByIndex(index, &appID, &available, name, sizeof(name)))
	{
		return utils::CreateString(name);
	}
	return NULL_STRING;
}

/*
@desc Checks if a specific app is installed.
This only works for base applications, not DLC. Use IsDLCInstalled for DLC instead.
@param appID The App ID of the application to check.
@return 1 if the app is installed; otherwise 0.
@api ISteamApps#BIsAppInstalled
*/
extern "C" DLL_EXPORT int IsAppInstalled(int appID)
{
	CheckInitialized(false);
	return SteamApps()->BIsAppInstalled(appID);
}

/*
@desc Checks whether the current App ID is for Cyber Cafes.
@return 1 if a cyber cafe; otherwise 0.
@api ISteamApps#BIsCybercafe*/
extern "C" DLL_EXPORT int IsCybercafe()
{
	CheckInitialized(false);
	return SteamApps()->BIsCybercafe();
}

/*
@desc Checks if the user owns a specific DLC and if the DLC is installed
@param appID The App ID of the DLC to check.
@return 1 when the user owns a DLC and it is installed; otherwise 0.
@api ISteamApps#BIsDlcInstalled
*/
extern "C" DLL_EXPORT int IsDLCInstalled(int appID)
{
	CheckInitialized(false);
	return SteamApps()->BIsDlcInstalled(appID);
}

/*
@desc Checks if the license owned by the user provides low violence depots.
@return 1 if the license owned by the user provides low violence depots; otherwise 0.
@api ISteamApps#BIsLowViolence
*/
extern "C" DLL_EXPORT int IsLowViolence()
{
	CheckInitialized(false);
	return SteamApps()->BIsLowViolence();
}

/*
@desc Checks if the active user is subscribed to the current App ID.
@return 1 if the active user owns the current AppId; otherwise 0.
@api ISteamApps#BIsSubscribed
*/
extern "C" DLL_EXPORT int IsSubscribed()
{
	CheckInitialized(false);
	return SteamApps()->BIsSubscribed();
}

/*
@desc Checks if the active user is subscribed to a specified AppId.
Only use this if you need to check ownership of another game related to yours, a demo for example.
@param appID The App ID to check.
@return 1 if the active user is subscribed to the specified App ID; otherwise 0.
@api ISteamApps#BIsSubscribedApp
*/
extern "C" DLL_EXPORT int IsSubscribedApp(int appID)
{
	CheckInitialized(false);
	return SteamApps()->BIsSubscribedApp(appID);
}

int IsSubscribedFromFamilySharing()
{
	CheckInitialized(false);
	return SteamApps()->BIsSubscribedFromFamilySharing();
}

int IsSubscribedFromFreeWeekend()
{
	CheckInitialized(false);
	return SteamApps()->BIsSubscribedFromFreeWeekend();
}

int IsVACBanned()
{
	CheckInitialized(false);
	return SteamApps()->BIsVACBanned();
}

int GetAppBuildID()
{
	CheckInitialized(false);
	return SteamApps()->GetAppBuildId();
}

char *GetAppInstallDir(int appID)
{
	CheckInitialized(NULL_STRING);
	char folder[_MAX_PATH];
	uint32 length = SteamApps()->GetAppInstallDir(appID, folder, sizeof(folder));
	if (length)
	{
		folder[length] = 0;
		return utils::CreateString(folder);
	}
	return NULL_STRING;
}

int GetAppOwner()
{
	CheckInitialized(false);
	return SteamHandles()->GetPluginHandle(SteamApps()->GetAppOwner());
}

char *GetAvailableGameLanguages()
{
	CheckInitialized(NULL_STRING);
	return utils::CreateString(SteamApps()->GetAvailableGameLanguages());
}

char *GetCurrentBetaName()
{
	CheckInitialized(NULL_STRING);
	char name[256];
	if (SteamApps()->GetCurrentBetaName(name, sizeof(name)))
	{
		return utils::CreateString(name);
	}
	return NULL_STRING;
}

char *GetCurrentGameLanguage()
{
	CheckInitialized(NULL_STRING);
	return utils::CreateString(SteamApps()->GetCurrentGameLanguage());
}

int GetDLCCount()
{
	CheckInitialized(0);
	return SteamApps()->GetDLCCount();
}

int GetDLCDownloadProgressBytesDownloaded(int appID)
{
	uint64 bytesDownloaded;
	uint64 bytesTotal;
	if (SteamApps()->GetDlcDownloadProgress(appID, &bytesDownloaded, &bytesTotal))
	{
		return (int)bytesDownloaded;
	}
	return -1;
}

int GetDLCDownloadProgressBytesTotal(int appID)
{
	uint64 bytesDownloaded;
	uint64 bytesTotal;
	if (SteamApps()->GetDlcDownloadProgress(appID, &bytesDownloaded, &bytesTotal))
	{
		return (int)bytesTotal;
	}
	return -1;
}

int GetEarliestPurchaseUnixTime(int appID)
{
	CheckInitialized(false);
	return SteamApps()->GetEarliestPurchaseUnixTime(appID);
}

int GetFileDetails(const char *filename)
{
	CheckInitialized(0);
	return CallResults()->Add(new CFileDetailsResultCallResult(filename));
}

char *GetFileDetailsSHA1(int hCallResult)
{
	return GetCallResultValue<CFileDetailsResultCallResult>(hCallResult, &CFileDetailsResultCallResult::GetFileSHA1);
}

int GetFileDetailsSize(int hCallResult)
{
	return GetCallResultValue<CFileDetailsResultCallResult>(hCallResult, &CFileDetailsResultCallResult::GetFileSize);
}

// Not explained in the documentation.  Assuming flags are unused.
//int GetFileDetailsFlags(int hCallResult)
//{
//	return GetCallResultValue<CFileDetailsResultCallResult>(hCallResult, &CFileDetailsResultCallResult::GetFileFlags);
//}

// This is an array of integers.  Speed is not important.  Returning JSON seems easiest.
char *GetInstalledDepotsJSON(int appID, int maxDepots)
{
	DepotId_t *depots = new DepotId_t[maxDepots];
	int count = SteamApps()->GetInstalledDepots(appID, depots, maxDepots);
	std::string json("[");
	for (int index = 0; index < count; index++)
	{
		if (index > 0)
		{
			json += ", ";
		}
		json += std::to_string(depots[index]);
	}
	json += "]";
	delete[] depots;
	return utils::CreateString(json);
}

char *GetLaunchCommandLine()
{
	CheckInitialized(NULL_STRING);
	char cmd[2084];
	int length = SteamApps()->GetLaunchCommandLine(cmd, sizeof(cmd));
	cmd[length] = 0;
	return utils::CreateString(cmd);
}

char *GetLaunchQueryParam(const char *key)
{
	CheckInitialized(NULL_STRING);
	return utils::CreateString(SteamApps()->GetLaunchQueryParam(key));
}

void InstallDLC(int appID)
{
	CheckInitialized(NORETURN);
	Callbacks()->RegisterDlcInstalledCallback();
	SteamApps()->InstallDLC(appID);
}

int MarkContentCorrupt(int missingFilesOnly)
{
	CheckInitialized(false);
	return SteamApps()->MarkContentCorrupt(missingFilesOnly != 0);
}

// RequestAllProofOfPurchaseKeys - deprecated
// RequestAppProofOfPurchaseKey - deprecated

void UninstallDLC(int appID)
{
	CheckInitialized(NORETURN);
	SteamApps()->UninstallDLC(appID);
}

// Callbacks
int HasDLCInstalledResponse()
{
	CheckInitialized(false);
	return Callbacks()->HasDlcInstalledResponse();
}

int GetDLCInstalledAppID()
{
	CheckInitialized(0);
	return Callbacks()->GetDlcInstalled().m_nAppID;
}

int HasNewUrlLaunchParametersResponse()
{
	CheckInitialized(false);
	return Callbacks()->HasNewUrlLaunchParametersResponse();
}
