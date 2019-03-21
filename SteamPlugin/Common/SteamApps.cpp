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

#define MAX_PATH	260

int IsAppInstalled(int appID)
{
	CheckInitialized(false);
	return SteamApps()->BIsAppInstalled(appID);
}

int IsCybercafe()
{
	CheckInitialized(false);
	return SteamApps()->BIsCybercafe();
}

int IsDLCInstalled(int appID)
{
	CheckInitialized(false);
	return SteamApps()->BIsDlcInstalled(appID);
}

int IsLowViolence()
{
	CheckInitialized(false);
	return SteamApps()->BIsLowViolence();
}

int IsSubscribed()
{
	CheckInitialized(false);
	return SteamApps()->BIsSubscribed();
}

int IsSubscribedApp(int appID)
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
	char folder[MAX_PATH];
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

int GetDLCDataByIndexAppID(int index)
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

int GetDLCDataByIndexAvailable(int index)
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

char *GetDLCDataByIndexName(int index)
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
#pragma endregion

#pragma region ISteamAppsList
// Restricted interface.
char *GetAppName(int appID)
{
	char name[256];
	int length = SteamAppList()->GetAppName(appID, name, sizeof(name));
	// Length of 0 means there is no app of that id.
	if (length == 0)
	{
		return utils::CreateString("Not found");
	}
	// Length > 0 means the name was found.
	if (length > 0)
	{
		name[length] = 0;
		return utils::CreateString(name);
	}
	// Length < 0 means the name needs to be cached.
	return agk::CreateString(0);
}
