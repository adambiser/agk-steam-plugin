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

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "DllMain.h"
#include <shellapi.h>
#include <psapi.h>

/*
NOTE: Cannot use bool as an exported function return type because of AGK2 limitations.  Use int instead.
*/

// GLOBALS!
// Move into Callbacks().
uint64 g_AppID = 0;
bool g_SteamInitialized;
bool g_StoringStats;
std::mutex g_JoinedLobbiesMutex;
std::vector<CSteamID> g_JoinedLobbies; // Keep track so we don't leave any left open when closing.

void ResetSession()
{
	// Disconnect from any lobbies.
	g_JoinedLobbiesMutex.lock();
	for (size_t index = 0; index < g_JoinedLobbies.size(); index++)
	{
		SteamMatchmaking()->LeaveLobby(g_JoinedLobbies[index]);
	}
	g_JoinedLobbies.clear();
	g_JoinedLobbiesMutex.unlock();
	Callbacks()->Reset();
	// Clear handles
	CallResults()->Clear();
	SteamHandles()->Clear();
	// Clear motion data
	Clear(g_InputAnalogActionData);
	Clear(g_InputDigitalActionData);
	Clear(g_InputMotionData);
	// Variables
	g_AppID = 0;
	g_InputCount = 0;
	g_SteamInitialized = false;
	g_StoringStats = false;
	ClearMostAchievedAchievementInfo();
}

#pragma region Additional Methods
char *GetCommandLineArgsJSON()
{
	std::ostringstream json;
	json << "[";
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
			char arg[MAX_PATH];
			wcstombs_s(&size, arg, szArglist[i], MAX_PATH);
			json << "\"" << utils::EscapeJSON(arg) << "\"";
		}
		// Free memory.
		LocalFree(szArglist);
	}
	json << "]";
	return utils::CreateString(json);
}

#define STEAM_REGISTRY_SUBKEY TEXT("Software\\Valve\\Steam")
#define STEAM_REGISTRY_VALUE TEXT("SteamPath")

char *GetSteamPath()
{
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
	return NULL_STRING;
}

int IsSteamEmulated()
{
	HANDLE hProcess = GetCurrentProcess();
	HMODULE hModules[1024];
	DWORD cbNeeded;

	bool emulated = false;
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
	return emulated;
}

#undef SetFileAttributes
int SetFileAttributes(const char *filename, int attributes)
{
	return SetFileAttributesA(filename, attributes);
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

#pragma region CallResult Methods
void DeleteCallResult(int hCallResult)
{
	CheckInitialized(NORETURN);
	CallResults()->Delete(hCallResult);
}

int GetCallResultCode(int hCallResult)
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
int Init()
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

int IsSteamInitialized()
{
	return g_SteamInitialized;
}

int RestartAppIfNecessary(int ownAppID)
{
	return SteamAPI_RestartAppIfNecessary(ownAppID);
}

// S_API void S_CALLTYPE SteamAPI_ReleaseCurrentThreadMemory(); - not doable

void RunCallbacks()
{
	CheckInitialized(NORETURN);
	SteamAPI_RunCallbacks();
}

// TODO S_API void S_CALLTYPE SteamAPI_SetMiniDumpComment( const char *pchMsg ); - not doable

void Shutdown()
{
	ResetSession();
	SteamAPI_Shutdown();
}

// TODO S_API void S_CALLTYPE SteamAPI_WriteMiniDump( uint32 uStructuredExceptionCode, void* pvExceptionInfo, uint32 uBuildID );
#pragma endregion

#pragma region steam_gameserver.h
// TODO?
#pragma endregion

#pragma region ISteamAppTicket
// This should never be needed in most cases.
#pragma endregion

#pragma region ISteamClient
//TODO? bool BReleaseSteamPipe( HSteamPipe hSteamPipe );
//TODO? bool BShutdownIfAllPipesClosed();
//TODO? HSteamUser ConnectToGlobalUser( HSteamPipe hSteamPipe );
//TODO? HSteamUser CreateLocalUser( HSteamPipe *phSteamPipe, EAccountType eAccountType );
//TODO? HSteamPipe CreateSteamPipe();
//TODO? uint32 GetIPCCallCount(); - this is also in SteamUtils!
//TODO? void ReleaseUser( HSteamPipe hSteamPipe, HSteamUser hUser );
//TODO? void SetLocalIPBinding( uint32 unIP, uint16 usPort );
//TODO? void SetWarningMessageHook( SteamAPIWarningMessageHook_t pFunction ); - this is also in SteamUtils!
#pragma endregion

#pragma region ISteamGameCoordinator
// deprecated
#pragma endregion

#pragma region ISteamGameServer
// TODO? 
#pragma endregion

#pragma region ISteamGameServerStats
// TODO?
#pragma endregion

#pragma region ISteamHTMLSurface
// TODO?
#pragma endregion

#pragma region ISteamHTTP
// TODO?
#pragma endregion

#pragma region ISteamInventory
// TODO?
#pragma endregion

#pragma region ISteamMatchmakingServers
// No callbacks.
#pragma endregion

#pragma region ISteamMusicRemote
// TODO?
#pragma endregion

#pragma region ISteamNetworking
// TODO?
#pragma endregion

#pragma region ISteamNetworkingSockets
// TODO?
#pragma endregion

#pragma region ISteamNetworkingUtils
// TODO?
#pragma endregion

#pragma region ISteamParties
// TODO?
#pragma endregion

#pragma region ISteamScreenshots
// TODO?
#pragma endregion

#pragma region ISteamUGC
// TODO?
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
#pragma endregion

#pragma region ISteamUnifiedMessages
// Deprecated
#pragma endregion

#pragma region ISteamVideo
// TODO?
#pragma endregion

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
