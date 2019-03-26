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
	CheckInitialized(0);
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
	CheckInitialized(0);
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
	CheckInitialized(NULL_STRING);
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

/*
@desc Check if user borrowed this game via Family Sharing, If true, call GetAppOwner() to get the lender SteamID
@return 1 if the active user borrowed this game via Family Sharing; otherwise 0.
@api ISteamApps#BIsSubscribedFromFamilySharing
*/
extern "C" DLL_EXPORT int IsSubscribedFromFamilySharing()
{
	CheckInitialized(false);
	return SteamApps()->BIsSubscribedFromFamilySharing();
}

/*
@desc Checks if the user is subscribed to the current App ID through a free weekend.
@return 1 if the active user is subscribed to the current App Id via a free weekend; otherwise 0 for any other kind of license.
@api ISteamApps#BIsSubscribedFromFreeWeekend
*/
extern "C" DLL_EXPORT int IsSubscribedFromFreeWeekend()
{
	CheckInitialized(false);
	return SteamApps()->BIsSubscribedFromFreeWeekend();
}

/*
@desc Checks if the user has a VAC ban on their account.
@return 1 when the user has a VAC ban; otherwise 0.
@api ISteamApps#BIsVACBanned
*/
extern "C" DLL_EXPORT int IsVACBanned()
{
	CheckInitialized(false);
	return SteamApps()->BIsVACBanned();
}

/*
@desc Gets the buildid of this app, may change at any time based on backend updates to the game.
@return The App BuildID if installed; otherwise 0.
@api ISteamApps#GetAppBuildId
*/
extern "C" DLL_EXPORT int GetAppBuildID()
{
	CheckInitialized(false);
	return SteamApps()->GetAppBuildId();
}

/*
@desc Gets the install folder for a specific AppID.
This works even if the application is not installed, based on where the game would be installed with the default Steam library location.
@param appID The App ID to get the install dir for.
@return The path the app is installed, or would be installed.
@api ISteamApps#GetAppInstallDir
*/
extern "C" DLL_EXPORT char *GetAppInstallDir(int appID)
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

/*
@desc Gets the Steam ID handle of the original owner of the current app. If it's different from the current user then it is borrowed.
@return A Steam ID handle.
@api ISteamApps#GetAppOwner
*/
extern "C" DLL_EXPORT int GetAppOwner()
{
	CheckInitialized(false);
	return SteamHandles()->GetPluginHandle(SteamApps()->GetAppOwner());
}

/*
@desc Gets a comma separated list of the languages the current app supports.
@return List of languages, separated by commas.
@api ISteamApps#GetAvailableGameLanguages
*/
extern "C" DLL_EXPORT char *GetAvailableGameLanguages()
{
	CheckInitialized(NULL_STRING);
	return utils::CreateString(SteamApps()->GetAvailableGameLanguages());
}

/*
@desc Checks if the user is running from a beta branch, and gets the name of the branch if they are.
@return The name of the beta branch running; otherwise an empty string if not a beta branch or the app is not installed
@api ISteamApps#GetCurrentBetaName
*/
extern "C" DLL_EXPORT char *GetCurrentBetaName()
{
	CheckInitialized(NULL_STRING);
	char name[256];
	if (SteamApps()->GetCurrentBetaName(name, sizeof(name)))
	{
		return utils::CreateString(name);
	}
	return NULL_STRING;
}

/*
@desc Gets the current language that the user has set.
@return The current language if the app is installed; otherwise an empty string.
@api ISteamApps#GetCurrentGameLanguage
*/
extern "C" DLL_EXPORT char *GetCurrentGameLanguage()
{
	CheckInitialized(NULL_STRING);
	return utils::CreateString(SteamApps()->GetCurrentGameLanguage());
}

/*
@desc Gets the number of DLC pieces for the current app.
@return The number of DLC pieces for the current app.
@api ISteamApps#GetDLCCount
*/
extern "C" DLL_EXPORT int GetDLCCount()
{
	CheckInitialized(0);
	return SteamApps()->GetDLCCount();
}

/*
@desc Gets the bytes downloaded of the download progress for DLC.

_Note: Steamworks returns an unsigned 64-bit integer, but AppGameKit only supports signed 32-bit integers.  Files larger than 2 GB will cause problems._
@param appID The App ID of the DLC to monitor.
@return A JSON string of the download progress.
@api ISteamApps#GetDlcDownloadProgress
*/
extern "C" DLL_EXPORT int GetDLCDownloadProgressBytesDownloaded(int appID)
{
	CheckInitialized(0);
	uint64 bytesDownloaded;
	uint64 bytesTotal;
	if (SteamApps()->GetDlcDownloadProgress(appID, &bytesDownloaded, &bytesTotal))
	{
		return (int)bytesDownloaded;
	}
	return -1;
}

/*
@desc Gets the bytes total of the download progress for DLC.

_Note: Steamworks returns an unsigned 64-bit integer, but AppGameKit only supports signed 32-bit integers.  Files larger than 2 GB will cause problems._
@param appID The App ID of the DLC to monitor.
@return A JSON string of the download progress.
@api ISteamApps#GetDlcDownloadProgress
*/
extern "C" DLL_EXPORT int GetDLCDownloadProgressBytesTotal(int appID)
{
	CheckInitialized(0);
	uint64 bytesDownloaded;
	uint64 bytesTotal;
	if (SteamApps()->GetDlcDownloadProgress(appID, &bytesDownloaded, &bytesTotal))
	{
		return (int)bytesTotal;
	}
	return -1;
}

/*
@desc Gets the time of purchase of the specified app in Unix epoch format (time since Jan 1st, 1970).
@param appID The App ID to get the purchase time for.
@return A Unix epoch time.
@api ISteamApps#GetEarliestPurchaseUnixTime
*/
extern "C" DLL_EXPORT int GetEarliestPurchaseUnixTime(int appID)
{
	CheckInitialized(0);
	return SteamApps()->GetEarliestPurchaseUnixTime(appID);
}

/*
@desc Asynchronously retrieves metadata details about a specific file in the depot manifest.
@param filename The filename in the current depot.
@callback-type callresult
@callback-getters GetFileDetailsSHA1, GetFileDetailsSize
@return A[call result handle](Callbacks - and-Call - Results#call - results) on success; otherwise 0.
@api ISteamApps#GetFileDetails
*/
extern "C" DLL_EXPORT int GetFileDetails(const char *filename)
{
	CheckInitialized(0);
	return CallResults()->Add(new CFileDetailsResultCallResult(filename));
}

/*
@desc Returns the SHA1 hash for the file.
@param hCallResult A GetFileDetails call result handle.
@return A 40 character hexidecimal string or an empty string if there was an error.
@api ISteamApps#GetFileDetails, ISteamApps#FileDetailsResult_t
*/
extern "C" DLL_EXPORT char *GetFileDetailsSHA1(int hCallResult)
{
	CheckInitialized(NULL_STRING);
	return GetCallResultValue<CFileDetailsResultCallResult>(hCallResult, &CFileDetailsResultCallResult::GetFileSHA1);
}

/*
@desc Returns the size of the file.
@param hCallResult A GetFileDetails call result handle.
@return The file size.
@api ISteamApps#GetFileDetails, ISteamApps#FileDetailsResult_t
*/
extern "C" DLL_EXPORT int GetFileDetailsSize(int hCallResult)
{
	CheckInitialized(0);
	return GetCallResultValue<CFileDetailsResultCallResult>(hCallResult, &CFileDetailsResultCallResult::GetFileSize);
}

// Not explained in the documentation.  Assuming flags are unused.
///*
//@desc Returns a flag value that is not explained in the Steamworks SDK documentation.
//@param hCallResult A GetFileDetails call result handle.
//@return A flag value.
//@api ISteamApps#GetFileDetails, ISteamApps#FileDetailsResult_t
//*/
//extern "C" DLL_EXPORT int GetFileDetailsFlags(int hCallResult)
//{
//	return GetCallResultValue<CFileDetailsResultCallResult>(hCallResult, &CFileDetailsResultCallResult::GetFileFlags);
//}

/*
@desc Gets a list of all installed depots for a given App ID in mount order.
@param appID The App to list the depots for.
@param maxDepots The maximum number of depots to obtain.
@return A JSON string of an integer array.
*/
extern "C" DLL_EXPORT char *GetInstalledDepotsJSON(int appID, int maxDepots)
{
	std::string json("[");
	if (g_SteamInitialized)
	{
		DepotId_t *depots = new DepotId_t[maxDepots];
		int count = SteamApps()->GetInstalledDepots(appID, depots, maxDepots);
		for (int index = 0; index < count; index++)
		{
			if (index > 0)
			{
				json += ", ";
			}
			json += std::to_string(depots[index]);
		}
		delete[] depots;
	}
	json += "]";
	return utils::CreateString(json);
}

/*
@desc Gets the command line if game was launched via Steam URL, e.g. steam://run/<appid>//<command line>/.
@return The command line if launched via Steam URL.
@api ISteamApps#GetLaunchCommandLine
*/
extern "C" DLL_EXPORT char *GetLaunchCommandLine()
{
	CheckInitialized(NULL_STRING);
	char cmd[2084];
	int length = SteamApps()->GetLaunchCommandLine(cmd, sizeof(cmd));
	cmd[length] = 0;
	return utils::CreateString(cmd);
}

/*
@desc Gets the associated launch parameter if the game is run via steam://run/<appid>/?param1=value1;param2=value2;param3=value3 etc.
@param key The launch key to test for. Ex: param1
@return The key value or an empty string if the key doesn't exist.
@api ISteamApps#GetLaunchQueryParam
*/
extern "C" DLL_EXPORT char *GetLaunchQueryParam(const char *key)
{
	CheckInitialized(NULL_STRING);
	return utils::CreateString(SteamApps()->GetLaunchQueryParam(key));
}

/*
@desc Install a DLC.  HasDLCInstalledResponse will report true when a DLC has finished installing.
@param appID The App ID of the DLC you want to install.
@api ISteamApps#InstallDLC
*/
extern "C" DLL_EXPORT void InstallDLC(int appID)
{
	CheckInitialized(NORETURN);
	Callbacks()->RegisterDlcInstalledCallback();
	SteamApps()->InstallDLC(appID);
}

/*
@desc Allows you to force verify game content on next launch.

If you detect the game is out-of-date (for example, by having the client detect a version mismatch with a server),
you can call use MarkContentCorrupt to force a verify, show a message to the user, and then quit.
@param missingFilesOnly Only scan for missing files, don't verify the checksum of each file.
@return A boolean as 0 or 1.  Steam API doesn't indicate what this means.
@api ISteamApps#MarkContentCorrupt
*/
extern "C" DLL_EXPORT int MarkContentCorrupt(int missingFilesOnly)
{
	CheckInitialized(false);
	return SteamApps()->MarkContentCorrupt(missingFilesOnly != 0);
}

// RequestAllProofOfPurchaseKeys - deprecated
// RequestAppProofOfPurchaseKey - deprecated

/*
@desc Uninstall a DLC.

_Note: This appears to take place after exiting the app._
@param appID The App ID of the DLC you want to uninstall.
@api ISteamApps#UninstallDLC
*/
extern "C" DLL_EXPORT void UninstallDLC(int appID)
{
	CheckInitialized(NORETURN);
	SteamApps()->UninstallDLC(appID);
}

// Callbacks
/*
@desc Triggered after the current user gains ownership of DLC and that DLC is installed.
@callback-type list
@callback-getters	GetDLCInstalledAppID
@return 1 when the callback has more responses to process; otherwise 0.
@api ISteamApps#DlcInstalled_t
*/
extern "C" DLL_EXPORT int HasDLCInstalledResponse()
{
	CheckInitialized(false);
	return Callbacks()->HasDlcInstalledResponse();
}

/*
@desc Returns the App ID for the current LobbyGameCreated_t callback response.
@return An App ID.
@api ISteamApps#DlcInstalled_t
*/
extern "C" DLL_EXPORT int GetDLCInstalledAppID()
{
	CheckInitialized(0);
	return Callbacks()->GetDlcInstalled().m_nAppID;
}

/*
@desc Posted after the user executes a steam url with query parameters while running.
Once a post has been reported, this method returns 0 until another post arrives.
@callback-type bool
@return 1 when the user executes a steam url with query parameters while running; otherwise 0.
@api ISteamApps#NewLaunchQueryParameters_t
*/
extern "C" DLL_EXPORT int HasNewUrlLaunchParametersResponse()
{
	CheckInitialized(false);
	return Callbacks()->HasNewUrlLaunchParametersResponse();
}
