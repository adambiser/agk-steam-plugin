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

#include "stdafx.h"
#if defined(_WINDOWS)
#include <windows.h>
#include <shellapi.h>
#include <psapi.h>
#endif

/*
NOTE: Cannot use bool as an exported function return type because of AGK2 limitations.  Use int instead.
*/

// GLOBALS!
uint32 g_AppID = 0;
bool g_SteamInitialized;

void ResetSession()
{
	CallbackBase::ResetAll();
	CallResults()->Clear();
	SteamHandles()->Clear();
	// Disconnect from any lobbies and clan chats.
	LobbyManager()->Reset();
	ClanChatManager()->Reset();
	// Clear input data
	ResetSteamInput();
	// Variables
	g_AppID = 0;
	g_SteamInitialized = false;
	ClearMostAchievedAchievementInfo();
}

#pragma region Additional Methods
/* @page Additional Methods

_Note: These commands are not part of the Steamworks SDK but are included as a convenience._
*/

#include <stdio.h>
#if defined(GNUC)
//#include <sys/stat.h>

//#define STEAM_RUNTIME_SUBPATH "/ubuntu12_32/steam-runtime/i386"
int g_argc;
char **g_argv;

__attribute__((constructor))
void OnStartup(int argc, char **argv)
{
	// Store the command line arguments for later.
	g_argc = argc;
	g_argv = argv;
	// This doesn't work...
	//std::cout << "OnStartup";
	//// Set steam runtime environment variable.
	//struct stat sb;
	//if (stat("$HOME/.local/share/Steam/steam.sh", &sb) == 0)
	//{
	//	setenv("STEAM_RUNTIME", "$HOME/.local/share/Steam" STEAM_RUNTIME_SUBPATH, 0); // Don't overwrite.
	//}
	//else if (stat("$HOME/.steam/steam.sh", &sb) == 0)
	//{
	//	setenv("STEAM_RUNTIME", "$HOME/.steam" STEAM_RUNTIME_SUBPATH, 0); // Don't overwrite.
	//}
	//char *pEnv;
	//if ((pEnv = getenv("STEAM_RUNTIME")) == NULL)
	//{
	//	// No runtime?  Abort...
	//	return;
	//}
	//std::string sSTEAM_RUNTIME(pEnv);
	//std::cout << "Steam runtime: " << sSTEAM_RUNTIME;
	//// Show the Steam Overlay - https://partner.steamgames.com/doc/store/application/platforms/linux#FAQ
	//std::string sLD_PRELOAD = sSTEAM_RUNTIME + "/../../../ubuntu12_64/gameoverlayrenderer.so";
	//setenv("LD_PRELOAD", sLD_PRELOAD.c_str(), 0); // Don't overwrite.
	//// Set up LD_LIBRARY_PATH/
	//std::string sLD_LIBRARY_PATH;
	//if ((pEnv = getenv("LD_LIBRARY_PATH")) != NULL)
	//{
	//	// Has a value, just use it.
	//	sLD_LIBRARY_PATH.assign(pEnv);
	//}
	//else
	//{
	//	// No value.  Create it.
	//	sLD_LIBRARY_PATH.assign(sSTEAM_RUNTIME + "/lib/i386-linux-gnu:" + sSTEAM_RUNTIME + "/usr/lib/i386-linux-gnu:" + sSTEAM_RUNTIME + "/usr/lib");
	//}
	//// Add the current directory to the library path to pick up libsteam_api.so
	//sLD_LIBRARY_PATH.append(":.");
	//setenv("LD_LIBRARY_PATH", sLD_LIBRARY_PATH.c_str(), 1);  // Overwrite.
}
#endif

/*
@desc Returns the command line arguments.
@return The command line arguments as a JSON string of an array of strings.
*/
extern "C" DLL_EXPORT char *GetCommandLineArgsJSON()
{
	std::ostringstream json;
	json << "[";
#if defined(_WINDOWS)
	int nArgs;
	LPWSTR *szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);
	if (NULL != szArglist)
	{
		for (int i = 0; i < nArgs; i++)
		{
			if (i > 0)
			{
				json << ",";
			}
			size_t size;
			char arg[_MAX_PATH];
			wcstombs_s(&size, arg, szArglist[i], _MAX_PATH);
			json << "\"" << utils::EscapeJSON(arg) << "\"";
		}
		// Free memory.
		LocalFree(szArglist);
	}
#elif defined(GNUC)
	for (int i = 0; i < g_argc; i++)
	{
		if (i > 0)
		{
			json << ",";
		}
		json << "\"" << utils::EscapeJSON(g_argv[i]) << "\"";
}
#endif
	json << "]";
	return utils::CreateString(json);
}

#if defined(_WINDOWS)
#define STEAM_REGISTRY_SUBKEY TEXT("Software\\Valve\\Steam")
#define STEAM_REGISTRY_VALUE TEXT("SteamPath")
#elif defined(GNUC)
#include <sys/stat.h>
#endif

/*
@desc On Windows, this returns the path of the folder containing the Steam client EXE as found in the registry
by reading the value of SteamPath in HKEY_CURRENT_USER\Software\Valve\Steam.
The path may contain slashes instead of backslashes and the trailing slash may or may not be included.

On Linux, this returns the value of the "STEAM_RUNTIME" environment variable.
@return The path at which the Steam client is installed.
*/
extern "C" DLL_EXPORT char *GetSteamPath()
{
#if defined(_WINDOWS)
	HKEY hKey = 0;
	if (RegOpenKeyEx(HKEY_CURRENT_USER, STEAM_REGISTRY_SUBKEY, 0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS)
	{
		DWORD dwType = REG_SZ;
		TCHAR szValue[1024];
		DWORD dwDataSize = sizeof(szValue) / sizeof(TCHAR);
		// Query string value
		LSTATUS result = RegQueryValueEx(hKey, STEAM_REGISTRY_VALUE, NULL, &dwType, (LPBYTE)(&szValue), &dwDataSize);
		RegCloseKey(hKey);
		if (result == ERROR_SUCCESS)
		{
			return utils::CreateString(szValue);
		}
	}
#elif defined(GNUC)
	char *pPath;
	if ((pPath = getenv("STEAM_RUNTIME")) != NULL)
	{
		char sPath[_MAX_PATH] = "";
		strncpy(sPath, pPath, strlen(pPath));
		// Chop off "/ubuntu12_32/steam-runtime/i386"
		*(strrchr(sPath, '/')) = 0;
		*(strrchr(sPath, '/')) = 0;
		*(strrchr(sPath, '/')) = 0;
		return utils::CreateString(sPath);
	}
	//struct stat sb;
	//if (stat("$HOME/.local/share/Steam/steam.sh", &sb) == 0)
	//{
	//	return utils::CreateString("$HOME/.local/share/Steam");
	//}
	//if (stat("$HOME/.steam/steam.sh", &sb) == 0)
	//{
	//	return utils::CreateString("$HOME/.steam");
	//}
#endif
	return NULL_STRING;
}

/*
@desc Attempts to detect when Steam is being emulated.
Emulation is sometimes used with pirated games, but it can also be used for valid reasons.

Note: This only works on Windows.
@return 1 when Steam emulation is detected, otherwise 0.
*/
extern "C" DLL_EXPORT int IsSteamEmulated()
{
	bool emulated = false;
#if defined(_WINDOWS)
	HANDLE hProcess = GetCurrentProcess();
	HMODULE hModules[1024];
	DWORD cbNeeded;

	if (EnumProcessModules(hProcess, hModules, sizeof(hModules), &cbNeeded))
	{
		for (unsigned int i = 0; i < (cbNeeded / sizeof(HMODULE)); i++)
		{
			TCHAR szModName[MAX_PATH];
			if (GetModuleFileNameEx(hProcess, hModules[i], szModName, sizeof(szModName) / sizeof(TCHAR)))
			{
#if UNICODE
				std::wstring s = szModName;
				if (s.find(TEXT("SmartSteamEmu.dll")) != std::wstring::npos)
#else
				std::string s = szModName;
				if (s.find("SmartSteamEmu.dll") != std::string::npos)
#endif
				{
					agk::Log("SmartSteamEmu emulation detected.");
					emulated = true;
				}
			}
		}
	}
#endif
	return emulated;
}

#undef SetFileAttributes
#if defined(_WINDOWS)
/*
@desc Sets the attributes of a file.
This is only included to help with development because the AppGameKit IDE deletes Steam files in the project folder when the interpreter exits.

Note: This only works on Windows.
@param filename The name of the file whose attributes are to be set.
@param attributes The file attributes to set for the file.
@return 1 if successful; otherwise, 0.
*/
extern "C" DLL_EXPORT int SetFileAttributes(const char *filename, int attributes)
{
	return SetFileAttributesA(filename, attributes);
#else
extern "C" DLL_EXPORT int SetFileAttributes(const char * /*filename*/, int /*attributes*/)
{
	return 0;
#endif
}

//char *GetPublicIP()
//{
//	CheckInitialized(NULL_STRING);
//	uint32 ip = Callbacks()->GetPublicIP();
//	std::ostringstream ipc;
//	ipc << ConvertIPToString(ip);
//	return utils::CreateString(ipc);
//}
#pragma endregion

#pragma region Call Result Methods
/* @page Call Result Methods

Some Steamworks methods are asynchronous commands that return call result handles.

Use GetCallResultCode to check the status of a call result and DeleteCallResult once the call result has been processed.
*/
/*
@desc Deletes a [call result](Callbacks-and-Call-Results#call-results) and its data.
@param hCallResult The call result handle to delete.
*/
extern "C" DLL_EXPORT void DeleteCallResult(int hCallResult)
{
	CheckInitialized(NORETURN);
	CallResults()->Delete(hCallResult);
}

/*
@desc Returns the result code of the given [call result](Callbacks-and-Call-Results#call-results).
@param hCallResult The call result handle to check.
@return An EResult code.
@url https://partner.steamgames.com/doc/api/steam_api#EResult
*/
extern "C" DLL_EXPORT int GetCallResultCode(int hCallResult)
{
	CheckInitialized(0);
	if (CCallResultItemBase *callResult = CallResults()->Get<CCallResultItemBase>(hCallResult))
	{
		return callResult->GetResultCode();
	}
	return 0;
}
#pragma endregion

#pragma region steam_api.h
/* @page steam_api.h */

/*
@desc
Initializes the Steam API.  This method should be called before attempting to do anything else with this plugin.
This also calls RequestCurrentStats() internally so calling code does not need to do so. The result of the RequestCurrentStats call has no effect on the return value.
@return 1 when Steam API initialization succeeds; otherwise 0.
@url https://partner.steamgames.com/doc/api/steam_api#SteamAPI_Init
*/
extern "C" DLL_EXPORT int Init()
{
	if (g_SteamInitialized)
	{
		agk::PluginError("Steam API already initialized.");
		return false;
	}
	agk::Log("Initializing Steam API.");
	ResetSession();
	g_SteamInitialized = SteamAPI_Init();
	if (g_SteamInitialized)
	{
		g_AppID = SteamUtils()->GetAppID();
		utils::Log("AppID: " + std::to_string(g_AppID));
		RequestCurrentStats();
		return true;
	}
	agk::Log("Failed to initialize Steam API.");
	return false;
}

/*
@desc Returns whether the Steam API is currently initialized.
@return 1 when the Steam API is initialized; otherwise 0.
@url https://partner.steamgames.com/doc/api/steam_api#SteamAPI_Init
*/
extern "C" DLL_EXPORT int SteamInitialized()
{
	return g_SteamInitialized;
}

/*
@desc
_When used, this command should be called near the beginning of the starting process and should be called before Init._

Tests to see if the game has been started through the Steam Client.  Effectively serves as a basic DRM by forcing the game to be started by the Steam Client.
@param ownAppID The Steam AppID of your game.
@return
1 when a new instance of the game has been started by Steam Client and this instance should close as soon as possible;
otherwise 0 when the game is already running from the Steam Client.

0 will also be reported if the steam_appid.txt file exists.
@url https://partner.steamgames.com/doc/api/steam_api#SteamAPI_RestartAppIfNecessary
*/
extern "C" DLL_EXPORT int RestartAppIfNecessary(int ownAppID)
{
	return SteamAPI_RestartAppIfNecessary(ownAppID);
}

// SteamAPI_ReleaseCurrentThreadMemory - AppGameKit is single threaded.

/*
@desc
Allows asynchronous Steam API calls to handle any new results.
Should be called each frame sync.
@url https://partner.steamgames.com/doc/api/steam_api#SteamAPI_RunCallbacks
*/
extern "C" DLL_EXPORT void RunCallbacks()
{
	CheckInitialized(NORETURN);
	SteamAPI_RunCallbacks();
}

// SteamAPI_SetMiniDumpComment - not doable, AppGameKit does not provide EXCEPTION_POINTERS

/*
@desc Shuts down the plugin and frees memory.
@url https://partner.steamgames.com/doc/api/steam_api#SteamAPI_Shutdown
*/
extern "C" DLL_EXPORT void Shutdown()
{
	ResetSession();
	SteamAPI_Shutdown();
}

// SteamAPI_WriteMiniDump - not doable, AppGameKit does not provide EXCEPTION_POINTERS
#pragma endregion

// steam_gameserver.h
//	TODO: Research this.

// steamnetworkingtypes.h
//	TODO: Research this.

// ISteamAppTicket
//	SKIP: This should never be needed in most cases.

// ISteamClient
// SKIP: Only needed for a more complex versioning scheme or a multiplexed gameserver.

// ISteamGameCoordinator
//	SKIP: deprecated

// ISteamGameServer
//	TODO: Research this.

// ISteamGameServerStats
//	TODO: Research this.

// ISteamHTMLSurface
//	TODO: Research this.

// ISteamHTTP
//	TODO: Research this.

// ISteamInventory
//	TODO: Research this.

// ISteamMatchmakingServers
//	TODO: Research this.

// ISteamMusicRemote
//	TODO: Research this.

// ISteamNetworking
//	TODO: Research this.

// ISteamNetworkingSockets - New to SDK v1.44
//	TODO: Research this.

// ISteamNetworkingUtils - New to SDK v1.44
//	TODO: Research this.

// ISteamParties - New to SDK v1.43
//	TODO: Research this.

// ISteamScreenshots
//	TODO: Research this.

// ISteamUGC
//	TODO: Research this.
//AddAppDependencyResult_t - Call result for AddAppDependency
//AddUGCDependencyResult_t - Call result for AddDependency
//CreateItemResult_t - Call result for CreateItem
//DownloadItemResult_t - Call result for DownloadItem
//GetAppDependenciesResult_t - Call result for GetAppDependencies
//DeleteItemResult_t - Call result for DeleteItem
//GetUserItemVoteResult_t - Call result for GetUserItemVote
//ItemInstalled_t - Callback.  Check m_unAppID for this game's appid!
//RemoveAppDependencyResult_t - Call result for RemoveAppDependency.
//RemoveUGCDependencyResult_t - Call result for RemoveDependency
//SetUserItemVoteResult_t - Call result for SetUserItemVote
//StartPlaytimeTrackingResult_t - Call result for StartPlaytimeTracking
//SteamUGCQueryCompleted_t - Call result for SendQueryUGCRequest
//StopPlaytimeTrackingResult_t - Call result for StopPlaytimeTracking, StopPlaytimeTrackingForAllItems
//SubmitItemUpdateResult_t - Call result for SubmitItemUpdate
//UserFavoriteItemsListChanged_t - Call result for AddItemToFavorites, RemoveItemFromFavorites

// ISteamUnifiedMessages
//	SKIP: Deprecated

// ISteamVideo
//	TODO: Research this.

// SteamEncryptedAppTicket - Encrypted application tickets.
//	SKIP? Would anyone using AppGameKit need this?

#if defined(_WINDOWS)
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		Shutdown();
		break;
	}
	return TRUE;
}
#endif