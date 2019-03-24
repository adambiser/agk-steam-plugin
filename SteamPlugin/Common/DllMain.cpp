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

// SteamAPI_ReleaseCurrentThreadMemory - AppGameKit is single threaded.

void RunCallbacks()
{
	CheckInitialized(NORETURN);
	SteamAPI_RunCallbacks();
}

// SteamAPI_SetMiniDumpComment - not doable, AppGameKit does not provide EXCEPTION_POINTERS

void Shutdown()
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

// ISteamNetworkingSockets
//	TODO: Research this.

// ISteamNetworkingUtils
//	TODO: Research this.

// ISteamParties
//	TODO: Research this.

// ISteamScreenshots
//	TODO: Research this.

// ISteamUGC
//	TODO: Research this.

// ISteamUnifiedMessages
//	SKIP: Deprecated

// ISteamVideo
//	TODO: Research this.

// SteamEncryptedAppTicket - Encrypted application tickets.
//	SKIP? Would anyone using AppGameKit need this?

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
