/*
Copyright (c) 2017 Adam Biser <adambiser@gmail.com>

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

#define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <ctime>
#include <shellapi.h>
#include <vector>
#include <sstream>
#include <psapi.h>
#include "DllMain.h"
#include "SteamPlugin.h"
#include "..\AGKLibraryCommands.h"

/*
NOTE: Cannot use bool as an exported function return type because of AGK2 limitations.  Use int instead.
*/

static SteamPlugin *Steam;

/*
Check to see if the SteamPlugin has been initialized.
If it has not been, return a default value.
*/
#define CheckInitializedPlugin(returnValue)	\
	if (!Steam)							\
	{									\
		return returnValue;				\
	}

// Token for passing an empty returnValue into CheckInitializedPlugin();
#define NORETURN
#define NULL_STRING CreateString((const char *)NULL)

#define Clamp(value, min, max)			\
	if (value < min)					\
	{									\
		value = min;					\
	}									\
	else if (value > max)				\
	{									\
		value = max;					\
	}

/*
Converts a const char* to agk:string.
*/
char *CreateString(const char* text)
{
	if (text)
	{
		int length = (int)strlen(text) + 1;
		char *result = agk::CreateString(length);
		strcpy_s(result, length, text);
		return result;
	}
	char *result = agk::CreateString(1);
	strcpy_s(result, 1, "");
	return result;
}

// This version also deletes the given text pointer.
//char *CreateStringEx(const char* text)
//{
//	char *result = CreateString(text);
//	delete[] text;
//	text = NULL;
//	return result;
//}

char *CreateString(std::string text)
{
	return CreateString(text.c_str());
}

char *CreateString(const WCHAR* text)
{
	size_t length = wcslen(text) + 1;
	char *result = agk::CreateString((int)length);
	size_t size;
	wcstombs_s(&size, result, length, text, length);
	return result;
}

/*
	CSteamID Handle Methods
*/
std::vector <CSteamID> m_CSteamIDs;

CSteamID GetSteamID(int handle)
{
	// Handles are 1-based!
	if (handle >= 0 && handle < (int)m_CSteamIDs.size())
	{
		return m_CSteamIDs[handle];
	}
	agk::PluginError("Invalid CSteamID handle.");
	return k_steamIDNil;
}

int GetSteamIDHandle(CSteamID steamID)
{
	int index = (int)(std::find(m_CSteamIDs.begin(), m_CSteamIDs.end(), steamID) - m_CSteamIDs.begin());
	if (index < (int) m_CSteamIDs.size())
	{
		// Handles are 1-based!
		return index;
	}
	m_CSteamIDs.push_back(steamID);
	return (int)m_CSteamIDs.size() - 1;
}

void ClearSteamIDHandleList()
{
	m_CSteamIDs.clear();
	// Handle 0 is always k_steamIDNil.
	m_CSteamIDs.push_back(k_steamIDNil);
}

/*
	InputHandle_t Handle Methods
*/
int m_InputCount = 0;
InputHandle_t m_InputHandles[STEAM_INPUT_MAX_COUNT];

// Handles are 1-based!
#define ValidateInputHandle(index, returnValue)		\
	index--;										\
	if (index < 0 || index >= m_InputCount)			\
	{												\
		agk::PluginError("Invalid input handle.");	\
		return returnValue;							\
	}

/*
CSteamID Handle Methods
*/
std::vector <InputActionSetHandle_t> m_ActionSetHandles;
std::vector <InputDigitalActionHandle_t> m_DigitalActionHandles;
std::vector <InputAnalogActionHandle_t> m_AnalogActionHandles;

//template <typename T>
//inline T GetActionHandle(int handle, std::vector<T> *vector)
//{
//	// Handles are 1-based!
//	if (handle >= 0 && handle < (int)vector->size())
//	{
//		return (*vector)[handle];
//	}
//	agk::PluginError("Invalid handle.");
//	return 0;
//}

#define ValidateActionHandle(handleType, handleVar, handleIndex, vector, returnValue)	\
	if (handleIndex <= 0 || handleIndex >= (int)vector.size())	\
	{															\
		agk::PluginError("Invalid " #handleType " handle.");		\
		return returnValue;										\
	}															\
	handleType handleVar = vector[handleIndex]

template <typename T>
inline int GetActionHandleIndex(T handle, std::vector<T> *vector)
{
	int index = (int)(std::find(vector->begin(), vector->end(), handle) - vector->begin());
	if (index < (int)vector->size())
	{
		// Handles are 1-based!
		return index;
	}
	vector->push_back(handle);
	return (int)vector->size() - 1;
}

void ClearActionSetHandleList()
{
	m_ActionSetHandles.clear();
	m_DigitalActionHandles.clear();
	m_AnalogActionHandles.clear();
	// Handle 0 is always 0.
	m_ActionSetHandles.push_back(0);
	m_DigitalActionHandles.push_back(0);
	m_AnalogActionHandles.push_back(0);
}

/*
	IP Address Parsing Methods
*/
bool ParseIP(const char *ipv4, uint32 *ip)
{
	int ip1, ip2, ip3, ip4;
	sscanf(ipv4, "%d.%d.%d.%d", &ip1, &ip2, &ip3, &ip4);
	if ((ip1 < 0 || ip1 > 255)
		|| (ip2 < 0 || ip2 > 255)
		|| (ip3 < 0 || ip3 > 255)
		|| (ip4 < 0 || ip4 > 255)
		)
	{
		agk::PluginError("Could not parse IP address.");
		return false;
	}
	*ip = (ip1 << 24) | (ip2 << 16) | (ip3 << 8) | (ip4);
	return true;
}

#define ConvertIPToString(ip) ((ip >> 24) & 0xff) << "." << ((ip >> 16) & 0xff) << "." << ((ip >> 8) & 0xff) << "." << ((ip >> 0) & 0xff)

void ResetVariables()
{

	ClearSteamIDHandleList();
	ClearActionSetHandleList();
	m_InputCount = 0;
}

/*
This method should be called before attempting to do anything else with this plugin.
Returns 1 when the Steam API has been initialized.  Otherwise 0 is returned.
Note that this also calls RequestStats, so calling code does not need to do so.
The result of the RequestStats call has no effect on the return value.
*/
int Init()
{
	if (!Steam)
	{
		Steam = new SteamPlugin();
	}
	ResetVariables();
	if (Steam)
	{
		if (Steam->Init())
		{
			Steam->RequestStats();
			return true;
		}
	}
	return false;
}

/*
Shuts down the plugin and frees memory.
*/
void Shutdown()
{
	ResetVariables();
	delete Steam;
	Steam = NULL;
}

/*
Gets whether the Steam API has been initialized.
When it has, this method returns 1.  Otherwise 0 is returned.

*/
int SteamInitialized()
{
	CheckInitializedPlugin(false);
	return Steam->SteamInitialized();
}

int RestartAppIfNecessary(int ownAppID)
{
	bool doDelete = false;
	bool result = false;
	if (!Steam)
	{
		doDelete = true;
		Steam = new SteamPlugin();
	}
	if (Steam)
	{
		result = Steam->RestartAppIfNecessary(ownAppID);
	}
	if (doDelete)
	{
		delete Steam;
		Steam = NULL;
	}
	return result;
}

/*
Returns the AppID or 0 if the Steam API has not been not initialized or the AppID cannot be found.
*/
int GetAppID()
{
	CheckInitializedPlugin(0);
	return Steam->GetAppID();
}

char *GetAppName(int appID)
{
	if (Steam)
	{
		char name[256];
		if (Steam->GetAppName(appID, name, sizeof(name)))
		{
			return CreateString(name);
		}
	}
	return CreateString("NULL");
}

/*
Returns 1 when logged into Steam.  Otherise returns 0.
*/
int LoggedOn()
{
	CheckInitializedPlugin(false);
	return Steam->LoggedOn();
}

int IsSteamIDValid(int hSteamID)
{
	CheckInitializedPlugin(false);
	CSteamID steamID = GetSteamID(hSteamID);
	return steamID.IsValid();
}

int GetHandleFromSteamID64(const char *steamID64)
{
	CheckInitializedPlugin(0);
	uint64 id = _atoi64(steamID64);
	return GetSteamIDHandle(CSteamID(id));
}

/*
Call every frame to allow the Steam API callbacks to process.
Should be used in conjunction with RequestStats and StoreStats to ensure the callbacks finish.
*/
void RunCallbacks()
{
	CheckInitializedPlugin(NORETURN);
	Steam->RunCallbacks();
}

char *GetCommandLineArgsJSON()
{
	std::ostringstream json;
	json << "[";
	LPWSTR *szArglist;
	int nArgs;
	szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);
	if (NULL != szArglist)
	{
		char arg[MAX_PATH];
		for (int i = 0; i < nArgs; i++)
		{
			if (i > 0)
			{
				json << ",";
			}
			size_t size;
			wcstombs_s(&size, arg, szArglist[i], MAX_PATH);
			json << "\"" << arg << "\"";
		}
		// Free memory.
		LocalFree(szArglist);
	}
	json << "]";
	return CreateString(json.str());
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
			return CreateString(szValue);
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

// App/DLC methods
char *GetDLCDataJSON()
{
	std::ostringstream json;
	json << "[";
	if (Steam)
	{
		for (int index = 0; index < Steam->GetDLCCount(); index++)
		{
			if (index > 0)
			{
				json << ",";
			}
			AppId_t appID;
			bool available;
			char name[128];
			bool success = Steam->GetDLCDataByIndex(index, &appID, &available, name, sizeof(name));
			json << "{";
			if (success)
			{
				json << "\"AppID\": " << appID;
				json << ", \"Available\": " << available;
				json << ", \"Name\": \"" << name << "\"";
			}
			json << "}";
			//else
			//{
			//	// Send an empty entry to indicate failure for this index.
			//	json << "{";
			//	json << "\"AppID\": 0";
			//	json << ", \"Available\": 0";
			//	json << ", \"Name\": \"\"";
			//	json << "}";
			//}
		}
	}
	json << "]";
	return CreateString(json.str());
}

char *GetDLCDataByIndexJSON(int index)
{
	std::ostringstream json;
	json << "{";
	if (Steam)
	{
		AppId_t appID;
		bool available;
		char name[128];
		bool success = Steam->GetDLCDataByIndex(index, &appID, &available, name, sizeof(name));
		if (success)
		{
			json << "\"AppID\": " << appID;
			json << ", \"Available\": " << available;
			json << ", \"Name\": \"" << name << "\"";
		}
	}
	json << "}";
	return CreateString(json.str());
}

int IsAppInstalled(int appID)
{
	CheckInitializedPlugin(false);
	return Steam->IsAppInstalled(appID);
}

int IsCybercafe()
{
	CheckInitializedPlugin(false);
	return Steam->IsCybercafe();
}

int IsDLCInstalled(int appID)
{
	CheckInitializedPlugin(false);
	return Steam->IsDlcInstalled(appID);
}

int IsLowViolence()
{
	CheckInitializedPlugin(false);
	return Steam->IsLowViolence();
}

int IsSubscribed()
{
	CheckInitializedPlugin(false);
	return Steam->IsSubscribed();
}

int IsSubscribedApp(int appID)
{
	CheckInitializedPlugin(false);
	return Steam->IsSubscribedApp(appID);
}

int IsSubscribedFromFamilySharing()
{
	CheckInitializedPlugin(false);
	return Steam->IsSubscribedFromFamilySharing();
}

int IsSubscribedFromFreeWeekend()
{
	CheckInitializedPlugin(false);
	return Steam->IsSubscribedFromFreeWeekend();
}

int IsVACBanned()
{
	CheckInitializedPlugin(false);
	return Steam->IsVACBanned();
}

int GetAppBuildID()
{
	CheckInitializedPlugin(false);
	return Steam->GetAppBuildId();
}

char *GetAppInstallDir(int appID)
{
	CheckInitializedPlugin(NULL_STRING);
	char folder[MAX_PATH];
	uint32 length = Steam->GetAppInstallDir(appID, folder, sizeof(folder));
	if (length)
	{
		//length++; // NULL terminator
		//char *result = agk::CreateString(length);
		//strcpy_s(result, length, folder);
		//return result;
		// TODO Verify this works as expected without using the returned length.
		return CreateString(folder);
	}
	else
	{
		return NULL_STRING;
	}
}

int GetAppOwner()
{
	CheckInitializedPlugin(false);
	return GetSteamIDHandle(Steam->GetAppOwner());
}

char *GetAvailableGameLanguages()
{
	CheckInitializedPlugin(NULL_STRING);
	return CreateString(Steam->GetAvailableGameLanguages());
}

char *GetCurrentBetaName()
{
	CheckInitializedPlugin(NULL_STRING);
	char name[256];
	if (Steam->GetCurrentBetaName(name, sizeof(name)))
	{
		return CreateString(name);
	}
	return NULL_STRING;
}

char *GetCurrentGameLanguage()
{
	CheckInitializedPlugin(NULL_STRING);
	return CreateString(Steam->GetCurrentGameLanguage());
}

int GetDLCCount()
{
	CheckInitializedPlugin(false);
	return Steam->GetDLCCount();
}

char *GetDLCDownloadProgressJSON(int appID)
{
	std::ostringstream json;
	json << "{";
	if (Steam)
	{
		uint64 downloaded;
		uint64 total;
		bool downloading = Steam->GetDlcDownloadProgress(appID, &downloaded, &total);
		if (downloading)
		{
			json << "\"AppID\": " << appID;
			json << ", \"BytesDownloaded\": " << downloaded;
			json << ", \"BytesTotal\": " << total;
		}
	}
	json << "}";
	return CreateString(json.str());
}

int GetEarliestPurchaseUnixTime(int appID)
{
	CheckInitializedPlugin(false);
	return Steam->GetEarliestPurchaseUnixTime(appID);
}

//SteamAPICall_t GetFileDetails(const char*pszFileName); // FileDetailsResult_t call result.

char *GetInstalledDepotsJSON(int appID, int maxDepots)
{
	std::ostringstream json;
	json << "[";
	if (Steam)
	{
		DepotId_t *depots = new DepotId_t[maxDepots];
		int count = Steam->GetInstalledDepots(appID, depots, maxDepots);
		for (int x = 0; x < count; x++)
		{
			if (x > 0)
			{
				json << ",";
			}
			json << depots[x];
		}
		delete[] depots;
	}
	json << "]";
	return CreateString(json.str());
}

char * GetLaunchQueryParam(const char *key)
{
	CheckInitializedPlugin(NULL_STRING);
	return CreateString(Steam->GetLaunchQueryParam(key));
}

int HasNewLaunchQueryParameters()
{
	CheckInitializedPlugin(false);
	return Steam->HasNewLaunchQueryParameters();
}

char *GetLaunchCommandLine()
{
	CheckInitializedPlugin(NULL_STRING);
	char cmd[2084];
	int length = Steam->GetLaunchCommandLine(cmd, 2084);
	cmd[length] = 0;
	return CreateString(cmd);
}

int HasNewDLCInstalled()
{
	CheckInitializedPlugin(false);
	return Steam->HasNewDlcInstalled();
}

int GetNewDLCInstalled()
{
	CheckInitializedPlugin(0);
	return Steam->GetNewDlcInstalled();
}

void InstallDLC(int appID) // Triggers a DlcInstalled_t callback.
{
	CheckInitializedPlugin(NORETURN);
	Steam->InstallDLC(appID);
}

int MarkContentCorrupt(int missingFilesOnly)
{
	CheckInitializedPlugin(false);
	return Steam->MarkContentCorrupt(missingFilesOnly != 0);
}

void UninstallDLC(int appID)
{
	CheckInitializedPlugin(NORETURN);
	Steam->UninstallDLC(appID);
}

// Overlay methods
int IsGameOverlayActive()
{
	CheckInitializedPlugin(false);
	return Steam->IsGameOverlayActive();
}

void ActivateGameOverlay(const char *dialogName)
{
	CheckInitializedPlugin(NORETURN);
	Steam->ActivateGameOverlay(dialogName);
}

void ActivateGameOverlayInviteDialog(int hLobbySteamID)
{
	CheckInitializedPlugin(NORETURN);
	Steam->ActivateGameOverlayInviteDialog(GetSteamID(hLobbySteamID));
}

void ActivateGameOverlayToStore(int appID, int flag)
{
	CheckInitializedPlugin(NORETURN);
	Steam->ActivateGameOverlayToStore(appID, (EOverlayToStoreFlag)flag);
}

void ActivateGameOverlayToUser(const char *dialogName, int hSteamID)
{
	CheckInitializedPlugin(NORETURN);
	Steam->ActivateGameOverlayToUser(dialogName, GetSteamID(hSteamID));
}

void ActivateGameOverlayToWebPage(const char *url)
{
	CheckInitializedPlugin(NORETURN);
	Steam->ActivateGameOverlayToWebPage(url, k_EActivateGameOverlayToWebPageMode_Default);
}

void ActivateGameOverlayToWebPageModal(const char *url)
{
	CheckInitializedPlugin(NORETURN);
	Steam->ActivateGameOverlayToWebPage(url, k_EActivateGameOverlayToWebPageMode_Modal);
}

char *GetPersonaName()
{
	CheckInitializedPlugin(NULL_STRING);
	return CreateString(Steam->GetPersonaName());
}

int GetSteamID()
{
	CheckInitializedPlugin(0);
	return GetSteamIDHandle(Steam->GetSteamID());
}

int GetAccountID(int hUserSteamID)
{
	CheckInitializedPlugin(0);
	CSteamID steamID = GetSteamID(hUserSteamID);
	if (steamID != k_steamIDNil)
	{
		return steamID.GetAccountID();
	}
	return 0;
}

char *GetSteamID3(int hUserSteamID)
{
	CheckInitializedPlugin(NULL_STRING);
	CSteamID steamID = GetSteamID(hUserSteamID);
	if (steamID != k_steamIDNil)
	{
		std::ostringstream steam3;
		steam3 << "[";
		// Source: https://developer.valvesoftware.com/wiki/SteamID#Types_of_Steam_Accounts
		switch (steamID.GetEAccountType())
		{
		case k_EAccountTypeInvalid:			// I
		case k_EAccountTypeIndividual:		// U
		case k_EAccountTypeMultiseat:		// M
		case k_EAccountTypeGameServer:		// G
		case k_EAccountTypeAnonGameServer:	// A
		case k_EAccountTypePending:			// P
		case k_EAccountTypeContentServer:	// C
		case k_EAccountTypeClan:			// g
		case k_EAccountTypeChat:			// T / L / c
		case k_EAccountTypeAnonUser:		// a
			steam3 << "IUMGAPCgT?a"[steamID.GetEAccountType()];
			break;
		case k_EAccountTypeConsoleUser: // 9 = P2P, but this doesn't look right.  Fake anyway, just do the default below.
		default:
			steam3 << steamID.GetEAccountType();
			break;
		}
		steam3 << ":" << steamID.GetEUniverse() << ":" << steamID.GetAccountID();
		switch (steamID.GetEAccountType())
		{
		case k_EAccountTypeAnonGameServer:
		case k_EAccountTypeAnonUser:
			steam3 << ":" << steamID.GetUnAccountInstance();
			break;
		}
		steam3 << "]";
		return CreateString(steam3.str());
	}
	return NULL_STRING;
}

char *GetSteamID64(int hUserSteamID)
{
	CheckInitializedPlugin(NULL_STRING);
	char id64[21]; // Max value is 18,446,744,073,709,551,615
	_i64toa(GetSteamID(hUserSteamID).ConvertToUint64(), id64, 10);
	return CreateString(id64);
}

int HasPersonaStateChanged()
{
	CheckInitializedPlugin(false);
	return Steam->HasPersonaStateChanged();
}

int GetPersonaStateChangedUser()
{
	CheckInitializedPlugin(0);
	return GetSteamIDHandle(Steam->GetPersonaStateChangedUser());
}

int GetPersonaStateChangedFlags()
{
	CheckInitializedPlugin(0);
	return Steam->GetPersonaStateChangedFlags();
}

int RequestUserInformation(int hUserSteamID, int requireNameOnly)
{
	CheckInitializedPlugin(false);
	return Steam->RequestUserInformation(GetSteamID(hUserSteamID), requireNameOnly != 0);
}

int HasAvatarImageLoaded()
{
	CheckInitializedPlugin(false);
	return Steam->HasAvatarImageLoaded();
}

int GetAvatarImageLoadedUser()
{
	CheckInitializedPlugin(0);
	return GetSteamIDHandle(Steam->GetAvatarImageLoadedUser());
}

int GetFriendAvatar(int hUserSteamID, int size)
{
	CheckInitializedPlugin(0);
	return Steam->GetFriendAvatar(GetSteamID(hUserSteamID), (EAvatarSize)size);
}

int GetFriendCount(int friendFlags)
{
	CheckInitializedPlugin(-1);
	return Steam->GetFriendCount((EFriendFlags)friendFlags);
}

int GetFriendByIndex(int index, int friendFlags)
{
	CheckInitializedPlugin(0);
	return GetSteamIDHandle(Steam->GetFriendByIndex(index, (EFriendFlags)friendFlags));
}

char *GetFriendListJSON(int friendFlags)
{
	std::ostringstream json;
	json << "[";
	if (Steam)
	{
		int friendCount = Steam->GetFriendCount((EFriendFlags)friendFlags);
		for (int x = 0; x < friendCount; x++)
		{
			if (x > 0)
			{
				json << ",";
			}
			json << GetSteamIDHandle(Steam->GetFriendByIndex(x, (EFriendFlags)friendFlags));
		}
	}
	json << "]";
	return CreateString(json.str());
}

char *GetFriendGamePlayedJSON(int hUserSteamID)
{
	std::ostringstream json;
	json << "{";
	if (Steam)
	{
		FriendGameInfo_t pFriendGameInfo;
		bool ingame = Steam->GetFriendGamePlayed(GetSteamID(hUserSteamID), &pFriendGameInfo);
		json << "\"InGame\": " << (int)ingame;
		if (ingame)
		{
			json << ", \"GameAppID\": " << pFriendGameInfo.m_gameID.AppID();
			json << ", \"GameIP\": \"" << ConvertIPToString(pFriendGameInfo.m_unGameIP) << "\"";
			json << ", \"GamePort\": " << pFriendGameInfo.m_usGamePort;
			json << ", \"QueryPort\": " << pFriendGameInfo.m_usQueryPort;
			json << ", \"SteamIDLobby\": " << GetSteamIDHandle(pFriendGameInfo.m_steamIDLobby);
		}
	}
	json << "}";
	return CreateString(json.str());
}

char *GetFriendPersonaName(int hUserSteamID)
{
	CheckInitializedPlugin(NULL_STRING);
	return CreateString(Steam->GetFriendPersonaName(GetSteamID(hUserSteamID)));
}

int GetFriendPersonaState(int hUserSteamID)
{
	CheckInitializedPlugin(-1);
	return Steam->GetFriendPersonaState(GetSteamID(hUserSteamID));
}

int GetFriendRelationship(int hUserSteamID)
{
	CheckInitializedPlugin(-1);
	return Steam->GetFriendRelationship(GetSteamID(hUserSteamID));
}

int GetFriendSteamLevel(int hUserSteamID)
{
	CheckInitializedPlugin(0);
	return Steam->GetFriendSteamLevel(GetSteamID(hUserSteamID));
}

char *GetPlayerNickname(int hUserSteamID)
{
	CheckInitializedPlugin(NULL_STRING);
	return CreateString(Steam->GetPlayerNickname(GetSteamID(hUserSteamID)));
}

int HasFriend(int hUserSteamID, int friendFlags)
{
	CheckInitializedPlugin(false);
	return Steam->HasFriend(GetSteamID(hUserSteamID), (EFriendFlags)friendFlags);
}

int GetFriendsGroupCount()
{
	CheckInitializedPlugin(0);
	return Steam->GetFriendsGroupCount();
}

int GetFriendsGroupIDByIndex(int index)
{
	CheckInitializedPlugin(k_FriendsGroupID_Invalid);
	return Steam->GetFriendsGroupIDByIndex(index);
}

int GetFriendsGroupMembersCount(int hFriendsGroupID)
{
	CheckInitializedPlugin(0);
	return Steam->GetFriendsGroupMembersCount(hFriendsGroupID);
}

char *GetFriendsGroupMembersListJSON(int hFriendsGroupID) // return a json array of SteamID handles
{
	std::ostringstream json;
	json << "[";
	if (Steam)
	{
		int memberCount = Steam->GetFriendsGroupMembersCount(hFriendsGroupID);
		if (memberCount > 0)
		{
			std::vector<CSteamID> friends(memberCount);
			Steam->GetFriendsGroupMembersList(hFriendsGroupID, friends.data(), memberCount);
			for (int x = 0; x < memberCount; x++)
			{
				if (x > 0)
				{
					json << ",";
				}
				json << GetSteamIDHandle(friends[x]);
			}
		}
	}
	json << "]";
	return CreateString(json.str());
}

char *GetFriendsGroupName(int hFriendsGroupID)
{
	CheckInitializedPlugin(NULL_STRING);
	return CreateString(Steam->GetFriendsGroupName(hFriendsGroupID));
}

int LoadImageFromHandle(int hImage)
{
	CheckInitializedPlugin(0);
	return Steam->LoadImageFromHandle(hImage);
}

void LoadImageIDFromHandle(int imageID, int hImage)
{
	CheckInitializedPlugin(NORETURN);
	Steam->LoadImageFromHandle(imageID, hImage);
}

/*
Requests the user stats.
Returns 1 if the process succeeds.  Otherwise, returns 0.
Use StatsInitialized() to determine when user stats are initialized.
*/
int RequestStats()
{
	CheckInitializedPlugin(false);
	return Steam->RequestStats();
}

int GetRequestStatsCallbackState()
{
	CheckInitializedPlugin(ClientError);
	return Steam->GetRequestStatsCallbackState();
}

/*
Returns 1 when user stats are initialized.  Otherwise, returns 0.
*/
int StatsInitialized()
{
	CheckInitializedPlugin(false);
	return Steam->StatsInitialized();
}

/*
Stores the user stats and achievements in Steam.
Returns 1 if the process succeeds.  Otherwise returns 0.
Use StatsStored() to determine when user stats have been stored.
*/
int StoreStats()
{
	CheckInitializedPlugin(false);
	return Steam->StoreStats();
}

/*
Resets all status and optionally the achievements.
Should generally only be used while testing.
*/
int ResetAllStats(int achievementsToo)
{
	CheckInitializedPlugin(false);
	return Steam->ResetAllStats(achievementsToo != 0);
}

int GetStoreStatsCallbackState()
{
	CheckInitializedPlugin(ClientError);
	return Steam->GetStoreStatsCallbackState();
}

/*
Returns 1 when user stats have been stored since the last StoreStats call.  Otherwise, returns 0.
*/
int StatsStored()
{
	CheckInitializedPlugin(false);
	return Steam->StatsStored();
}

/*
Returns 1 when an achievement has been stored since the last StoreStats call.  Otherwise, returns 0.
*/
int AchievementStored()
{
	CheckInitializedPlugin(false);
	return Steam->AchievementStored();
}

/*
Returns the number of achievements for the app or 0 if user stats have not been initialized.
Use StatsInitialized() to determine when user stats are initialized before calling this method.
*/
int GetNumAchievements()
{
	CheckInitializedPlugin(0);
	return Steam->GetNumAchievements();
}

/*
Gets the achievement API name for the given achievement index.
If user stats have not been initialized, an empty string is returned.
Use StatsInitialized() to determine when user stats are initialized before calling this method.
*/
char *GetAchievementAPIName(int index)
{
	CheckInitializedPlugin(NULL_STRING);
	return CreateString(Steam->GetAchievementAPIName(index));
}

char *GetAchievementDisplayName(const char *name)
{
	CheckInitializedPlugin(NULL_STRING);
	return CreateString(Steam->GetAchievementDisplayAttribute(name, "name"));
}

char *GetAchievementDisplayDesc(const char *name)
{
	CheckInitializedPlugin(NULL_STRING);
	return CreateString(Steam->GetAchievementDisplayAttribute(name, "desc"));
}

int GetAchievementDisplayHidden(const char *name)
{
	CheckInitializedPlugin(false);
	return (strcmp(Steam->GetAchievementDisplayAttribute(name, "hidden"), "1") == 0);
}

int GetAchievementIcon(const char *name)
{
	CheckInitializedPlugin(0);
	return Steam->GetAchievementIcon(name);
}

/*
Returns 1 if the user has achieved this achievement.
If user stats have not been initialized 0 is returned.
Use StatsInitialized() to determine when user stats are initialized before calling this method.
If the call fails, 0 is returned and the error is reported to the AGK interpreter.
*/
int GetAchievement(const char *name)
{
	CheckInitializedPlugin(false);
	bool result = false;
	if (Steam->GetAchievement(name, &result))
	{
		return result;
	}
	agk::PluginError("GetAchievement failed.");
	return false;
}

int GetAchievementUnlockTime(const char *name)
{
	CheckInitializedPlugin(0);
	bool pbAchieved;
	uint32 punUnlockTime;
	if (SteamUserStats()->GetAchievementAndUnlockTime(name, &pbAchieved, &punUnlockTime))
	{
		if (pbAchieved)
		{
			return punUnlockTime;
		}
	}
	return 0;
}

/*
Marks an achievement as achieved.
Returns 1 if the process succeeds.  Otherwise, returns 0.
Use StatsInitialized() to determine when user stats are initialized before calling this method.
*/
int SetAchievement(const char *name)
{
	CheckInitializedPlugin(false);
	return Steam->SetAchievement(name);
}

/*
Raises a notification about achievemnt progress for progress stat achievements.
The notification only shows when current progress is less than the max.
SetStat still needs to be used to set the progress stat value.
*/
int IndicateAchievementProgress(const char *name, int curProgress, int maxProgress)
{
	CheckInitializedPlugin(false);
	return Steam->IndicateAchievementProgress(name, curProgress, maxProgress);
}

/*
Clears an achievement.
Returns 1 if the process succeeds.  Otherwise, returns 0.
Use StatsInitialized() to determine when user stats are initialized before calling this method.
*/
int ClearAchievement(const char *name)
{
	CheckInitializedPlugin(false);
	return Steam->ClearAchievement(name);
}

/*
Gets an integer user stat value.
If there is a problem, 0 is returned and the error is reported to the AGK interpreter.
*/
int GetStatInt(const char *name)
{
	CheckInitializedPlugin(0);
	int result = 0;
	if (Steam->GetStat(name, &result)) {
		return result;
	}
	agk::PluginError("GetStat failed.");
	return 0;
}

/*
Gets a float user stat value.
If there is a problem, 0 is returned and the error is reported to the AGK interpreter.
*/
float GetStatFloat(const char *name)
{
	CheckInitializedPlugin(0.0);
	float result = 0.0;
	if (Steam->GetStat(name, &result)) {
		return result;
	}
	agk::PluginError("GetStat failed.");
	return 0.0;
}

/*
Sets an integer user stat value.
Returns 1 if the call succeeds.  Otherwise returns 0.
*/
int SetStatInt(const char *name, int value)
{
	CheckInitializedPlugin(false);
	return Steam->SetStat(name, value);
}

/*
Sets a float user stat value.
Returns 1 if the call succeeds.  Otherwise returns 0.
*/
int SetStatFloat(const char *name, float value)
{
	CheckInitializedPlugin(false);
	return Steam->SetStat(name, value);
}

/*
Updates an average rate user stat.  Steam takes care of the averaging.  Use GetStatFloat to get the result.
Returns 1 if the call succeeds.  Otherwise returns 0.
*/
int UpdateAvgRateStat(const char *name, float countThisSession, float sessionLength)
{
	CheckInitializedPlugin(false);
	return Steam->UpdateAvgRateStat(name, countThisSession, (double)sessionLength);
}

int FindLeaderboard(const char *leaderboardName)
{
	CheckInitializedPlugin(false);
	return Steam->FindLeaderboard(leaderboardName);
}

int GetFindLeaderboardCallbackState()
{
	CheckInitializedPlugin(ClientError);
	return Steam->GetFindLeaderboardCallbackState();
}

int GetLeaderboardHandle()
{
	CheckInitializedPlugin(0);
	return (int) Steam->GetLeaderboardHandle();
}

char *GetLeaderboardName(int hLeaderboard)
{
	CheckInitializedPlugin(NULL_STRING);
	return CreateString(Steam->GetLeaderboardName(hLeaderboard));
}

int GetLeaderboardEntryCount(int hLeaderboard)
{
	CheckInitializedPlugin(0);
	return Steam->GetLeaderboardEntryCount(hLeaderboard);
}

int GetLeaderboardDisplayType(int hLeaderboard)
{
	CheckInitializedPlugin(0);
	return Steam->GetLeaderboardDisplayType(hLeaderboard);
}

int GetLeaderboardSortMethod(int hLeaderboard)
{
	CheckInitializedPlugin(0);
	return Steam->GetLeaderboardSortMethod(hLeaderboard);
}

// TODO Adde force parameter and allow for a data memblock of integers.
int UploadLeaderboardScore(int hLeaderboard, int score)
{
	CheckInitializedPlugin(false);
	return Steam->UploadLeaderboardScore((SteamLeaderboard_t)hLeaderboard, k_ELeaderboardUploadScoreMethodKeepBest, score);
}

int UploadLeaderboardScoreForceUpdate(int hLeaderboard, int score)
{
	CheckInitializedPlugin(false);
	return Steam->UploadLeaderboardScore((SteamLeaderboard_t)hLeaderboard, k_ELeaderboardUploadScoreMethodForceUpdate, score);
}

int GetUploadLeaderboardScoreCallbackState()
{
	CheckInitializedPlugin(ClientError);
	return Steam->GetUploadLeaderboardScoreCallbackState();
}

int LeaderboardScoreStored()
{
	CheckInitializedPlugin(false);
	return Steam->LeaderboardScoreStored();
}

int LeaderboardScoreChanged()
{
	CheckInitializedPlugin(false);
	return Steam->LeaderboardScoreChanged();
}

int GetLeaderboardUploadedScore()
{
	CheckInitializedPlugin(0);
	return Steam->GetLeaderboardUploadedScore();
}

int GetLeaderboardGlobalRankNew()
{
	CheckInitializedPlugin(0);
	return Steam->GetLeaderboardGlobalRankNew();
}

int GetLeaderboardGlobalRankPrevious()
{
	CheckInitializedPlugin(0);
	return Steam->GetLeaderboardGlobalRankPrevious();
}

int DownloadLeaderboardEntries(int hLeaderboard, int eLeaderboardDataRequest, int nRangeStart, int nRangeEnd)
{
	CheckInitializedPlugin(false);
	return Steam->DownloadLeaderboardEntries(hLeaderboard, (ELeaderboardDataRequest) eLeaderboardDataRequest, nRangeStart, nRangeEnd);
}

int GetDownloadLeaderboardEntriesCallbackState()
{
	CheckInitializedPlugin(ClientError);
	return Steam->GetDownloadLeaderboardEntriesCallbackState();
}

int GetDownloadedLeaderboardEntryCount()
{
	CheckInitializedPlugin(0);
	return Steam->GetDownloadedLeaderboardEntryCount();
}

int GetDownloadedLeaderboardEntryGlobalRank(int index)
{
	CheckInitializedPlugin(0);
	return Steam->GetDownloadedLeaderboardEntryGlobalRank(index);
}

int GetDownloadedLeaderboardEntryScore(int index)
{
	CheckInitializedPlugin(0);
	return Steam->GetDownloadedLeaderboardEntryScore(index);
}

int GetDownloadedLeaderboardEntryUser(int index)
{
	CheckInitializedPlugin(0);
	return GetSteamIDHandle(Steam->GetDownloadedLeaderboardEntryUser(index));
}

void AddRequestLobbyListDistanceFilter(int eLobbyDistanceFilter)
{
	CheckInitializedPlugin(NORETURN);
	return Steam->AddRequestLobbyListDistanceFilter((ELobbyDistanceFilter)eLobbyDistanceFilter);
}

void AddRequestLobbyListFilterSlotsAvailable(int slotsAvailable)
{
	CheckInitializedPlugin(NORETURN);
	return Steam->AddRequestLobbyListFilterSlotsAvailable(slotsAvailable);
}

void AddRequestLobbyListNearValueFilter(const char *keyToMatch, int valueToBeCloseTo)
{
	CheckInitializedPlugin(NORETURN);
	return Steam->AddRequestLobbyListNearValueFilter(keyToMatch, valueToBeCloseTo);
}

void AddRequestLobbyListNumericalFilter(const char *keyToMatch, int valueToMatch, int eComparisonType)
{
	CheckInitializedPlugin(NORETURN);
	return Steam->AddRequestLobbyListNumericalFilter(keyToMatch, valueToMatch, (ELobbyComparison)eComparisonType);
}

void AddRequestLobbyListResultCountFilter(int maxResults)
{
	CheckInitializedPlugin(NORETURN);
	return Steam->AddRequestLobbyListResultCountFilter(maxResults);
}

void AddRequestLobbyListStringFilter(const char *keyToMatch, const char *valueToMatch, int eComparisonType)
{
	CheckInitializedPlugin(NORETURN);
	return Steam->AddRequestLobbyListStringFilter(keyToMatch, valueToMatch, (ELobbyComparison)eComparisonType);
}

int RequestLobbyList()
{
	CheckInitializedPlugin(0);
	return Steam->RequestLobbyList();
}

int GetLobbyMatchListCallbackState()
{
	CheckInitializedPlugin(ClientError);
	return Steam->GetLobbyMatchListCallbackState();
}

int GetLobbyMatchListCount() {
	CheckInitializedPlugin(0);
	return Steam->GetLobbyMatchListCount();
}

int GetLobbyByIndex(int index)
{
	CheckInitializedPlugin(0);
	return GetSteamIDHandle(Steam->GetLobbyByIndex(index));
}

int CreateLobby(int eLobbyType, int maxMembers)
{
	CheckInitializedPlugin(false);
	return Steam->CreateLobby((ELobbyType)eLobbyType, maxMembers);
}

int GetLobbyCreateCallbackState()
{
	CheckInitializedPlugin(0);
	return Steam->GetLobbyCreateCallbackState();
}

int GetLobbyCreatedID()
{
	CheckInitializedPlugin(0);
	return GetSteamIDHandle(Steam->GetLobbyCreatedID());
}

int GetLobbyCreatedResult()
{
	CheckInitializedPlugin(0);
	return Steam->GetLobbyCreatedResult();
}

//int SetLinkedLobby(int hLobbySteamID, int hLobbyDependentSteamID)
//{
//	CheckInitializedPlugin(false);
//	return Steam->SetLinkedLobby(GetSteamID(hLobbySteamID), GetSteamID(hLobbyDependentSteamID));
//}

int SetLobbyJoinable(int hLobbySteamID, int lobbyJoinable)
{
	CheckInitializedPlugin(false);
	return Steam->SetLobbyJoinable(GetSteamID(hLobbySteamID), lobbyJoinable != 0);
}

int SetLobbyType(int hLobbySteamID, int eLobbyType)
{
	CheckInitializedPlugin(false);
	return Steam->SetLobbyType(GetSteamID(hLobbySteamID), (ELobbyType)eLobbyType);
}

int JoinLobby(int hLobbySteamID)
{
	CheckInitializedPlugin(false);
	return Steam->JoinLobby(GetSteamID(hLobbySteamID));
}

int GetLobbyEnterCallbackState()
{
	CheckInitializedPlugin(ClientError);
	return Steam->GetLobbyEnterCallbackState();
}

int GetLobbyEnterID()
{
	CheckInitializedPlugin(0);
	return GetSteamIDHandle(Steam->GetLobbyEnterID());
}

int GetLobbyEnterBlocked()
{
	CheckInitializedPlugin(false);
	return Steam->GetLobbyEnterBlocked();
}

int GetLobbyEnterResponse()
{
	CheckInitializedPlugin(0);
	return Steam->GetLobbyEnterResponse();
}

int InviteUserToLobby(int hLobbySteamID, int hInviteeSteamID)
{
	CheckInitializedPlugin(false);
	return Steam->InviteUserToLobby(GetSteamID(hLobbySteamID), GetSteamID(hInviteeSteamID));
}

int HasGameLobbyJoinRequest()
{
	CheckInitializedPlugin(0);
	return Steam->HasGameLobbyJoinRequest();
}

int GetGameLobbyJoinRequestedLobby()
{
	CheckInitializedPlugin(0);
	return GetSteamIDHandle(Steam->GetGameLobbyJoinRequestedLobby());
}

void LeaveLobby(int hLobbySteamID)
{
	CheckInitializedPlugin(NORETURN);
	Steam->LeaveLobby(GetSteamID(hLobbySteamID));
}

// Lobby methods: Game server
char *GetLobbyGameServerJSON(int hLobbySteamID)
{
	std::ostringstream json;
	json << "{";
	if (Steam)
	{
		uint32 unGameServerIP;
		uint16 unGameServerPort;
		CSteamID steamIDGameServer;
		if (Steam->GetLobbyGameServer(GetSteamID(hLobbySteamID), &unGameServerIP, &unGameServerPort, &steamIDGameServer))
		{
			json << "\"hLobby\": " << hLobbySteamID; // Add this to match LobbyGameCreated_t.
			json << ", \"IP\": \"" << ConvertIPToString(unGameServerIP) << "\"";
			json << ", \"Port\": " << unGameServerPort;
			json << ", \"hGameServer\": " << GetSteamIDHandle(steamIDGameServer);
		}
	}
	json << "}";
	return CreateString(json.str());
}

int SetLobbyGameServer(int hLobbySteamID, const char *gameServerIP, int gameServerPort, int hGameServerSteamID)
{
	CheckInitializedPlugin(false);
	if (gameServerPort < 0 || gameServerPort > 0xffff)
	{
		agk::PluginError("SetLobbyGameServer: Invalid game server port.");
		return false;
	}
	uint32 ip;
	if (!ParseIP(gameServerIP, &ip))
	{
		return false;
	}
	Steam->SetLobbyGameServer(GetSteamID(hLobbySteamID), ip, gameServerPort, GetSteamID(hGameServerSteamID));
	return true;
}

int HasLobbyGameCreated()
{
	CheckInitializedPlugin(false);
	return Steam->HasLobbyGameCreated();
}

char *GetLobbyGameCreatedJSON()
{
	std::ostringstream json;
	json << "{";
	if (Steam)
	{
		LobbyGameCreated_t gameServer = Steam->GetLobbyGameCreated();
		if (gameServer.m_ulSteamIDLobby != 0)
		{
			json << "\"hLobby\": " << GetSteamIDHandle(gameServer.m_ulSteamIDLobby);
			json << ", \"IP\": \"" << ConvertIPToString(gameServer.m_unIP) << "\"";
			json << ", \"Port\": " << gameServer.m_usPort;
			json << ", \"hGameServer\": " << GetSteamIDHandle(gameServer.m_ulSteamIDGameServer);
		}
	}
	json << "}";
	return CreateString(json.str());
}

// Lobby methods: Data
char *GetLobbyData(int hLobbySteamID, const char *key)
{
	CheckInitializedPlugin(NULL_STRING);
	return CreateString(Steam->GetLobbyData(GetSteamID(hLobbySteamID), key));
}

int GetLobbyDataCount(int hLobbySteamID)
{
	CheckInitializedPlugin(0);
	return Steam->GetLobbyDataCount(GetSteamID(hLobbySteamID));
}

char *GetLobbyDataByIndexJSON(int hLobbySteamID, int index)
{
	std::ostringstream json;
	json << "{";
	if (Steam)
	{
		char key[k_nMaxLobbyKeyLength];
		char value[k_cubChatMetadataMax];
		if (Steam->GetLobbyDataByIndex(GetSteamID(hLobbySteamID), index, key, k_nMaxLobbyKeyLength, value, k_cubChatMetadataMax))
		{
			json << "\"Key\": \"" << key << "\"";
			json << ", \"Value\": \"" << value << "\"";
		}
	}
	json << "}";
	return CreateString(json.str());
}

char *GetLobbyDataJSON(int hLobbySteamID)
{
	CheckInitializedPlugin(NULL_STRING);
	CSteamID steamIDLobby = GetSteamID(hLobbySteamID);
	std::ostringstream json;
	json << "[";
	char key[k_nMaxLobbyKeyLength];
	char value[k_cubChatMetadataMax];
	for (int index = 0; index < Steam->GetLobbyDataCount(steamIDLobby); index++)
	{
		if (Steam->GetLobbyDataByIndex(steamIDLobby, index, key, k_nMaxLobbyKeyLength, value, k_cubChatMetadataMax))
		{
			if (index > 0)
			{
				json << ", ";
			}
			json << "{\"Key\": \"" << key << "\"";
			json << ", \"Value\": \"" << value << "\"}";
		}
	}
	json << "]";
	return CreateString(json.str());
}

void SetLobbyData(int hLobbySteamID, const char *key, const char *value)
{
	CheckInitializedPlugin(NORETURN);
	Steam->SetLobbyData(GetSteamID(hLobbySteamID), key, value);
}

int DeleteLobbyData(int hLobbySteamID, const char *key)
{
	CheckInitializedPlugin(false);
	return Steam->DeleteLobbyData(GetSteamID(hLobbySteamID), key);
}

int RequestLobbyData(int hLobbySteamID)
{
	CheckInitializedPlugin(false);
	return Steam->RequestLobbyData(GetSteamID(hLobbySteamID));
}

int HasLobbyDataUpdated()
{
	CheckInitializedPlugin(false);
	return Steam->HasLobbyDataUpdated();
}

int GetLobbyDataUpdatedLobby()
{
	CheckInitializedPlugin(0);
	return GetSteamIDHandle(Steam->GetLobbyDataUpdatedLobby());
}

int GetLobbyDataUpdatedID()
{
	CheckInitializedPlugin(0);
	return GetSteamIDHandle(Steam->GetLobbyDataUpdatedID());
}

char *GetLobbyMemberData(int hLobbySteamID, int hUserSteamID, const char *key)
{
	CheckInitializedPlugin(NULL_STRING);
	return CreateString(Steam->GetLobbyMemberData(GetSteamID(hLobbySteamID), GetSteamID(hUserSteamID), key));
}

void SetLobbyMemberData(int hLobbySteamID, const char *key, const char *value)
{
	CheckInitializedPlugin(NORETURN);
	return Steam->SetLobbyMemberData(GetSteamID(hLobbySteamID), key, value);
}

int GetLobbyOwner(int hLobbySteamID)
{
	CheckInitializedPlugin(0);
	return GetSteamIDHandle(Steam->GetLobbyOwner(GetSteamID(hLobbySteamID)));
}

int SetLobbyOwner(int hLobbySteamID, int hNewOwnerSteamID)
{
	CheckInitializedPlugin(0);
	return Steam->SetLobbyOwner(GetSteamID(hLobbySteamID), GetSteamID(hNewOwnerSteamID));
}

int GetLobbyMemberLimit(int hLobbySteamID)
{
	CheckInitializedPlugin(0);
	return Steam->GetLobbyMemberLimit(GetSteamID(hLobbySteamID));
}

int SetLobbyMemberLimit(int hLobbySteamID, int maxMembers)
{
	CheckInitializedPlugin(0);
	return Steam->SetLobbyMemberLimit(GetSteamID(hLobbySteamID), maxMembers);
}

int GetNumLobbyMembers(int hLobbySteamID)
{
	CheckInitializedPlugin(0);
	return Steam->GetNumLobbyMembers(GetSteamID(hLobbySteamID));
}

int GetLobbyMemberByIndex(int hLobbySteamID, int index)
{
	CheckInitializedPlugin(0);
	return GetSteamIDHandle(Steam->GetLobbyMemberByIndex(GetSteamID(hLobbySteamID), index));
}

int HasLobbyChatUpdate()
{
	CheckInitializedPlugin(false);
	return Steam->HasLobbyChatUpdate();
}

int GetLobbyChatUpdateUserChanged()
{
	CheckInitializedPlugin(0);
	return GetSteamIDHandle(Steam->GetLobbyChatUpdateUserChanged());
}

int GetLobbyChatUpdateUserState()
{
	CheckInitializedPlugin(0);
	return Steam->GetLobbyChatUpdateUserState();
}

int GetLobbyChatUpdateUserMakingChange()
{
	CheckInitializedPlugin(0);
	return GetSteamIDHandle(Steam->GetLobbyChatUpdateUserMakingChange());
}

int HasLobbyChatMessage()
{
	CheckInitializedPlugin(false);
	return Steam->HasLobbyChatMessage();
}

int GetLobbyChatMessageUser()
{
	CheckInitializedPlugin(0);
	return GetSteamIDHandle(Steam->GetLobbyChatMessageUser());
}

char *GetLobbyChatMessageText()
{
	CheckInitializedPlugin(NULL_STRING);
	char msg[4096];
	Steam->GetLobbyChatMessageText(msg);
	return CreateString(msg);
}

int SendLobbyChatMessage(int hLobbySteamID, const char *msg)
{
	CheckInitializedPlugin(0);
	return Steam->SendLobbyChatMessage(GetSteamID(hLobbySteamID), msg);
}

// Lobby methods: Favorite games
int AddFavoriteGame(int appID, const char *ipv4, int connectPort, int queryPort, int flags) //, int time32LastPlayedOnServer)
{
	CheckInitializedPlugin(0);
	if (connectPort < 0 || connectPort > 0xffff)
	{
		agk::PluginError("AddFavoriteGame: Invalid connection port.");
		return 0;
	}
	if (queryPort < 0 || queryPort > 0xffff)
	{
		agk::PluginError("AddFavoriteGame: Invalid query port.");
		return 0;
	}
	uint32 ip;
	if (!ParseIP(ipv4, &ip))
	{
		return 0;
	}
	std::time_t now = std::time(0);
	//agk::Message(agk::Str((int)now));
	return Steam->AddFavoriteGame(appID, (uint32)ip, connectPort, queryPort, flags, (uint32)now);
}

int GetFavoriteGameCount()
{
	CheckInitializedPlugin(0);
	return Steam->GetFavoriteGameCount();
}

char *GetFavoriteGameJSON(int index)
{
	std::ostringstream json;
	json << "{";
	if (Steam)
	{
		AppId_t nAppID;
		uint32 nIP;
		uint16 nConnPort;
		uint16 nQueryPort;
		uint32 unFlags;
		uint32 rTime32LastPlayedOnServer;
		if (Steam->GetFavoriteGame(index, &nAppID, &nIP, &nConnPort, &nQueryPort, &unFlags, &rTime32LastPlayedOnServer))
		{
			json << "\"AppID\": " << nAppID;
			json << ", \"IP\": \"" << ConvertIPToString(nIP) << "\"";
			json << ", \"ConnectPort\": " << nConnPort;
			json << ", \"QueryPort\": " << nQueryPort;
			json << ", \"Flags\": " << unFlags;
			json << ", \"TimeLastPlayedOnServer\": " << (int)rTime32LastPlayedOnServer;
		}
	}
	json << "}";
	return CreateString(json.str());
}

int RemoveFavoriteGame(int appID, const char *ipv4, int connectPort, int queryPort, int flags)
{
	CheckInitializedPlugin(0);
	if (connectPort < 0 || connectPort > 0xffff)
	{
		agk::PluginError("RemoveFavoriteGame: Invalid connection port.");
		return 0;
	}
	if (queryPort < 0 || queryPort > 0xffff)
	{
		agk::PluginError("RemoveFavoriteGame: Invalid query port.");
		return 0;
	}
	uint32 ip;
	if (!ParseIP(ipv4, &ip))
	{
		return 0;
	}
	return Steam->RemoveFavoriteGame(appID, ip, connectPort, queryPort, flags);
}

//char *GetPublicIP()
//{
//	CheckInitializedPlugin(NULL_STRING);
//	uint32 ip = Steam->GetPublicIP();
//	std::ostringstream ipc;
//	ipc << ConvertIPToString(ip);
//	return CreateString(ipc.str());
//}

int IsMusicEnabled()
{
	CheckInitializedPlugin(0);
	return Steam->IsMusicEnabled();
}

int IsMusicPlaying()
{
	CheckInitializedPlugin(0);
	return Steam->IsMusicPlaying();
}

int GetMusicPlaybackStatus()
{
	CheckInitializedPlugin(0);
	return Steam->GetMusicPlaybackStatus();
}

float GetMusicVolume()
{
	CheckInitializedPlugin(0.0);
	return Steam->GetMusicVolume();
}

void PauseMusic()
{
	CheckInitializedPlugin(NORETURN);
	Steam->PauseMusic();
}

void PlayMusic()
{
	CheckInitializedPlugin(NORETURN);
	Steam->PlayMusic();
}

void PlayNextSong()
{
	CheckInitializedPlugin(NORETURN);
	Steam->PlayNextSong();
}

void PlayPreviousSong()
{
	CheckInitializedPlugin(NORETURN);
	Steam->PlayPreviousSong();
}

void SetMusicVolume(float volume)
{
	CheckInitializedPlugin(NORETURN);
	Steam->SetMusicVolume(volume);
}

int HasMusicPlaybackStatusChanged()
{
	CheckInitializedPlugin(0);
	return Steam->HasMusicPlaybackStatusChanged();
}

int HasMusicVolumeChanged()
{
	CheckInitializedPlugin(0);
	return Steam->HasMusicVolumeChanged();
}

// Util methods
int GetCurrentBatteryPower()
{
	CheckInitializedPlugin(0);
	return Steam->GetCurrentBatteryPower();
}

int GetIPCCallCount()
{
	CheckInitializedPlugin(0);
	return Steam->GetIPCCallCount();
}

char *GetIPCountry()
{
	CheckInitializedPlugin(NULL_STRING);
	return CreateString(Steam->GetIPCountry());
}

int GetSecondsSinceAppActive()
{
	CheckInitializedPlugin(0);
	return Steam->GetSecondsSinceAppActive();
}

int GetSecondsSinceComputerActive()
{
	CheckInitializedPlugin(0);
	return Steam->GetSecondsSinceComputerActive();
}

int GetServerRealTime()
{
	CheckInitializedPlugin(0);
	return Steam->GetServerRealTime();
}

char *GetSteamUILanguage()
{
	CheckInitializedPlugin(NULL_STRING);
	return CreateString(Steam->GetSteamUILanguage());
}

int IsOverlayEnabled()
{
	CheckInitializedPlugin(0);
	return Steam->IsOverlayEnabled();
}

void SetOverlayNotificationInset(int horizontalInset, int verticalInset)
{
	CheckInitializedPlugin(NORETURN);
	Steam->SetOverlayNotificationInset(horizontalInset, verticalInset);
}

void SetOverlayNotificationPosition(int eNotificationPosition)
{
	CheckInitializedPlugin(NORETURN);
	Steam->SetOverlayNotificationPosition((ENotificationPosition) eNotificationPosition);
}

int HasIPCountryChanged()
{
	CheckInitializedPlugin(false);
	return Steam->HasIPCountryChanged();
}

int HasLowBatteryWarning()
{
	CheckInitializedPlugin(false);
	return Steam->HasLowBatteryWarning();
}

int GetMinutesBatteryLeft()
{
	CheckInitializedPlugin(255);
	return Steam->GetMinutesBatteryLeft();
}

int IsSteamShuttingDown()
{
	CheckInitializedPlugin(false);
	return Steam->IsSteamShuttingDown();
}

void SetWarningMessageHook()
{
	CheckInitializedPlugin(NORETURN);
	Steam->SetWarningMessageHook();
}

// Big Picture methods
int IsSteamInBigPictureMode()
{
	CheckInitializedPlugin(0);
	return Steam->IsSteamInBigPictureMode();
}

int HasGamepadTextInputDismissedInfo()
{
	CheckInitializedPlugin(0);
	return Steam->HasGamepadTextInputDismissedInfo();
}

char *GetGamepadTextInputDismissedInfoJSON()
{
	std::ostringstream json;
	json << "{";
	if (Steam)
	{
		if (Steam->HasGamepadTextInputDismissedInfo())
		{
			bool submitted;
			char text[MAX_GAMEPAD_TEXT_INPUT_LENGTH];
			Steam->GetGamepadTextInputDismissedInfo(&submitted, text);
			json << "\"Submitted\": " << (int)submitted;
			json << ", \"Text\": \"" << text << "\"";
		}
	}
	json << "}";
	return CreateString(json.str());
}

int ShowGamepadTextInput(int eInputMode, int eLineInputMode, const char *description, int charMax, const char *existingText)
{
	CheckInitializedPlugin(false);
	return Steam->ShowGamepadTextInput((EGamepadTextInputMode)eInputMode, (EGamepadTextInputLineMode)eLineInputMode, description, charMax, existingText);
}

// VR Methods
int IsSteamRunningInVR()
{
	CheckInitializedPlugin(false);
	return Steam->IsSteamRunningInVR();
}

void StartVRDashboard()
{
	CheckInitializedPlugin(NORETURN);
	Steam->StartVRDashboard();
}

void SetVRHeadsetStreamingEnabled(int enabled)
{
	CheckInitializedPlugin(NORETURN);
	Steam->SetVRHeadsetStreamingEnabled(enabled != 0);
}

int IsVRHeadsetStreamingEnabled()
{
	CheckInitializedPlugin(false);
	return Steam->IsVRHeadsetStreamingEnabled();
}

// Cloud methods: Information
int IsCloudEnabledForAccount()
{
	CheckInitializedPlugin(false);
	return Steam->IsCloudEnabledForAccount();
}

int IsCloudEnabledForApp()
{
	CheckInitializedPlugin(false);
	return Steam->IsCloudEnabledForApp();
}

void SetCloudEnabledForApp(int enabled)
{
	CheckInitializedPlugin(NORETURN);
	return Steam->SetCloudEnabledForApp(enabled != 0);
}

// Cloud methods: Files
int CloudFileDelete(const char *filename)
{
	CheckInitializedPlugin(false);
	return Steam->FileDelete(filename);
}

int CloudFileExists(const char *filename)
{
	CheckInitializedPlugin(false);
	return Steam->FileExists(filename);
}

int CloudFileForget(const char *filename)
{
	CheckInitializedPlugin(false);
	return Steam->FileForget(filename);
}

int CloudFilePersisted(const char *filename)
{
	CheckInitializedPlugin(false);
	return Steam->FilePersisted(filename);
}

int CloudFileRead(const char *filename)
{
	CheckInitializedPlugin(0);
	int fileSize = Steam->GetFileSize(filename);
	unsigned int memblock = agk::CreateMemblock(fileSize);
	if (Steam->FileRead(filename, agk::GetMemblockPtr(memblock), fileSize))
	{
		return memblock;
	}
	agk::DeleteMemblock(memblock);
	return 0;
}

int CloudFileReadAsync(const char *filename, int offset, int length)
{
	CheckInitializedPlugin(false);
	if (length == -1)
	{
		length = Steam->GetFileSize(filename);
	}
	return Steam->FileReadAsync(filename, offset, length);
}

int GetCloudFileReadAsyncCallbackState(const char *filename)
{
	CheckInitializedPlugin(ClientError);
	return Steam->GetFileReadAsyncCallbackState(filename);
}

int GetCloudFileReadAsyncResult(const char *filename)
{
	CheckInitializedPlugin(0);
	return Steam->GetFileReadAsyncResult(filename);
}

int GetCloudFileReadAsyncMemblock(const char *filename)
{
	CheckInitializedPlugin(0);
	return Steam->GetFileReadAsyncMemblock(filename);
}


//SteamAPICall_t CloudFileShare(const char *filename);

int CloudFileWrite(const char *filename, int memblockID)
{
	CheckInitializedPlugin(false);
	if (agk::GetMemblockExists(memblockID))
	{
		return Steam->FileWrite(filename, agk::GetMemblockPtr(memblockID), agk::GetMemblockSize(memblockID));
	}
	return false;
}

int CloudFileWriteAsync(const char *filename, int memblockID)
{
	CheckInitializedPlugin(false);
	if (agk::GetMemblockExists(memblockID))
	{
		return Steam->FileWriteAsync(filename, agk::GetMemblockPtr(memblockID), agk::GetMemblockSize(memblockID));
	}
	return false;
}

int GetCloudFileWriteAsyncCallbackState(const char *filename)
{
	CheckInitializedPlugin(ClientError);
	return Steam->GetFileWriteAsyncCallbackState(filename);
}

int GetCloudFileWriteAsyncResult(const char *filename)
{
	CheckInitializedPlugin(0);
	return (int)Steam->GetFileWriteAsyncResult(filename);
}

//bool CloudFileWriteStreamCancel(UGCFileWriteStreamHandle_t writeHandle);
//bool CloudFileWriteStreamClose(UGCFileWriteStreamHandle_t writeHandle);
//UGCFileWriteStreamHandle_t CloudFileWriteStreamOpen(const char *filename);
//bool CloudFileWriteStreamWriteChunk(UGCFileWriteStreamHandle_t writeHandle, const void *pvData, int32 cubData);

int GetCloudFileCount()
{
	CheckInitializedPlugin(0);
	return Steam->GetFileCount();
}

char * GetCloudFileName(int index)
{
	CheckInitializedPlugin(NULL_STRING);
	int32 pnFileSizeInBytes;
	return CreateString(Steam->GetFileNameAndSize(index, &pnFileSizeInBytes));
}

char *GetCloudFileListJSON()
{
	std::ostringstream json;
	json << "[";
	if (Steam)
	{
		for (int index = 0; index < Steam->GetFileCount(); index++)
		{
			if (index > 0)
			{
				json << ", ";
			}
			json << "{";
			int32 pnFileSizeInBytes;
			const char *name = Steam->GetFileNameAndSize(index, &pnFileSizeInBytes);
			if (name)
			{
				json << "\"Name\": \"" << name << "\", ";
				json << "\"Size\": " << pnFileSizeInBytes;
			}
			else
			{
				json << "\"Name\": \"?\", ";
				json << "\"Size\": 0";
			}
			json << "}";
		}
	}
	json << "]";
	return CreateString(json.str());
}

int GetCloudFileSize(const char *filename)
{
	CheckInitializedPlugin(0);
	return Steam->GetFileSize(filename);
}

int GetCloudFileTimestamp(const char *filename)
{
	CheckInitializedPlugin(0);
	// NOTE: Dangerous conversion int64 to int.
	return (int) Steam->GetFileTimestamp(filename);
}

int GetCloudQuotaAvailable()
{
	CheckInitializedPlugin(0);
	uint64 pnTotalBytes;
	uint64 puAvailableBytes;
	if (Steam->GetQuota(&pnTotalBytes, &puAvailableBytes))
	{
		// Note: Potential issue converting uint64 to int if the quote gets to be more than 2GB.
		return (int)puAvailableBytes;
	}
	return 0;
}

int GetCloudQuotaTotal()
{
	CheckInitializedPlugin(0);
	uint64 pnTotalBytes;
	uint64 puAvailableBytes;
	if (Steam->GetQuota(&pnTotalBytes, &puAvailableBytes))
	{
		// Note: Potential issue converting uint64 to int if the quote gets to be more than 2GB.
		return (int)pnTotalBytes;
	}
	return 0;
}

char *GetCloudQuotaJSON()
{
	std::ostringstream json;
	json << "{";
	if (Steam)
	{
		uint64 pnTotalBytes;
		uint64 puAvailableBytes;
		if (Steam->GetQuota(&pnTotalBytes, &puAvailableBytes))
		{
			json << "\"Available\": " << puAvailableBytes << ", ";
			json << "\"Total\": " << pnTotalBytes;
		}
	}
	json << "}";
	return CreateString(json.str());
}

int GetCloudFileSyncPlatforms(const char *filename)
{
	CheckInitializedPlugin(0);
	return Steam->GetSyncPlatforms(filename);
}

int SetCloudFileSyncPlatforms(const char *filename, int eRemoteStoragePlatform)
{
	CheckInitializedPlugin(false);
	return Steam->SetSyncPlatforms(filename, (ERemoteStoragePlatform) eRemoteStoragePlatform);
}

// User-Generated Content

int GetCachedUGCCount()
{
	CheckInitializedPlugin(0);
	return Steam->GetCachedUGCCount();
}

//UGCHandle_t GetCachedUGCHandle(int32 iCachedContent);
//bool GetUGCDetails(UGCHandle_t hContent, AppId_t *pnAppID, const char **pname, int32 *pnFileSizeInBytes, CSteamID *pSteamIDOwner);
//bool GetUGCDownloadProgress(UGCHandle_t hContent, int32 *pnBytesDownloaded, int32 *pnBytesExpected);
//SteamAPICall_t UGCDownload(UGCHandle_t hContent, uint32 unPriority);
//SteamAPICall_t UGCDownloadToLocation(UGCHandle_t hContent, const char *location, uint32 unPriority);
//int32 UGCRead(UGCHandle_t hContent, void *pvData, int32 cubDataToRead, uint32 cOffset, EUGCReadAction eAction);

/*
Input Information
*/
int InitSteamInput()
{
	CheckInitializedPlugin(0);
	return Steam->InitSteamInput();
}

int ShutdownSteamInput()
{
	CheckInitializedPlugin(0);
	return Steam->ShutdownSteamInput();
}

int GetConnectedControllers()
{
	CheckInitializedPlugin(0);
	m_InputCount = Steam->GetConnectedControllers(m_InputHandles);
	return m_InputCount;
}

int GetInputTypeForHandle(int hInput)
{
	CheckInitializedPlugin(0);
	ValidateInputHandle(hInput, k_ESteamInputType_Unknown);
	return Steam->GetInputTypeForHandle(m_InputHandles[hInput]);
}

void RunFrame()
{
	CheckInitializedPlugin(NORETURN);
	Steam->RunFrame();
}

int ShowBindingPanel(int hInput)
{
	CheckInitializedPlugin(false);
	ValidateInputHandle(hInput, false);
	return Steam->ShowBindingPanel(m_InputHandles[hInput]);
}

/*
Input Action Sets and Layers
*/

void ActivateActionSet(int hInput, int hActionSet)
{
	CheckInitializedPlugin(NORETURN);
	ValidateInputHandle(hInput, );
	ValidateActionHandle(InputActionSetHandle_t, actionSetHandle, hActionSet, m_ActionSetHandles, );
	Steam->ActivateActionSet(m_InputHandles[hInput], actionSetHandle);
}

int GetActionSetHandle(const char *actionSetName)
{
	CheckInitializedPlugin(0);
	return GetActionHandleIndex(Steam->GetActionSetHandle(actionSetName), &m_ActionSetHandles);
}

int GetCurrentActionSet(int hInput)
{
	CheckInitializedPlugin(0);
	ValidateInputHandle(hInput, false);
	return GetActionHandleIndex(Steam->GetCurrentActionSet(m_InputHandles[hInput]), &m_ActionSetHandles);
}

void ActivateActionSetLayer(int hInput, int hActionSetLayer)
{
	CheckInitializedPlugin(NORETURN);
	ValidateInputHandle(hInput, );
	ValidateActionHandle(InputActionSetHandle_t, actionSetLayerHandle, hActionSetLayer, m_ActionSetHandles, );
	Steam->ActivateActionSetLayer(m_InputHandles[hInput], actionSetLayerHandle);
}

void DeactivateActionSetLayer(int hInput, int hActionSetLayer)
{
	CheckInitializedPlugin(NORETURN);
	ValidateInputHandle(hInput, );
	ValidateActionHandle(InputActionSetHandle_t, actionSetLayerHandle, hActionSetLayer, m_ActionSetHandles, );
	Steam->DeactivateActionSetLayer(m_InputHandles[hInput], actionSetLayerHandle);
}

void DeactivateAllActionSetLayers(int hInput)
{
	CheckInitializedPlugin(NORETURN);
	ValidateInputHandle(hInput, );
	Steam->DeactivateAllActionSetLayers(m_InputHandles[hInput]);
}

char *GetActiveActionSetLayersJSON(int hInput)
{
	CheckInitializedPlugin(CreateString("[]"));
	ValidateInputHandle(hInput, CreateString("[]"));
	std::ostringstream json;
	json << "[";
	if (Steam)
	{
		// TODO The API document has incorrect information.  Is STEAM_INPUT_MAX_COUNT correct?
		InputActionSetHandle_t *handlesOut = new InputActionSetHandle_t[STEAM_INPUT_MAX_COUNT];
		int count = Steam->GetActiveActionSetLayers(m_InputHandles[hInput], handlesOut);
		for (int index = 0; index < count; index++)
		{
			if (index > 0)
			{
				json << ",";
			}
			json << GetActionHandleIndex(handlesOut[index], &m_ActionSetHandles);
		}
		delete[] handlesOut;
	}
	json << "]";
	return CreateString(json.str());
}

/*
Input Actions and Motion
*/
void GetAnalogActionData(int hInput, int hAnalogAction)
{
	CheckInitializedPlugin(NORETURN);
	ValidateInputHandle(hInput, );
	ValidateActionHandle(InputAnalogActionHandle_t, analogActionHandle, hAnalogAction, m_AnalogActionHandles, );
	Steam->GetAnalogActionData(m_InputHandles[hInput], analogActionHandle);
}

int GetAnalogActionDataActive()
{
	CheckInitializedPlugin(0);
	return Steam->m_AnalogActionData.bActive;
}

int GetAnalogActionDataMode()
{
	CheckInitializedPlugin(0);
	return Steam->m_AnalogActionData.eMode;
}

// TODO Add a per-Input dead zone.
float GetAnalogActionDataX()
{
	CheckInitializedPlugin(0);
	return Steam->m_AnalogActionData.x;
}

float GetAnalogActionDataY()
{
	CheckInitializedPlugin(0);
	return Steam->m_AnalogActionData.y;
}

int GetAnalogActionHandle(const char *actionName)
{
	CheckInitializedPlugin(0);
	return GetActionHandleIndex(Steam->GetAnalogActionHandle(actionName), &m_AnalogActionHandles);
}

void StopAnalogActionMomentum(int hInput, int hAnalogAction)
{
	CheckInitializedPlugin(NORETURN);
	ValidateInputHandle(hInput, );
	ValidateActionHandle(InputAnalogActionHandle_t, analogActionHandle, hAnalogAction, m_AnalogActionHandles, );
	Steam->StopAnalogActionMomentum(m_InputHandles[hInput], analogActionHandle);
}

void GetDigitalActionData(int hInput, int hDigitalAction)
{
	CheckInitializedPlugin(NORETURN);
	ValidateInputHandle(hInput, );
	ValidateActionHandle(InputDigitalActionHandle_t, digitalActionHandle, hDigitalAction, m_DigitalActionHandles, );
	Steam->GetDigitalActionData(m_InputHandles[hInput], digitalActionHandle);
}

int GetDigitalActionDataActive()
{
	CheckInitializedPlugin(false);
	return Steam->m_DigitalActionData.bActive;
}

int GetDigitalActionDataState()
{
	CheckInitializedPlugin(false);
	return Steam->m_DigitalActionData.bState;
}

int GetDigitalActionHandle(const char *actionName)
{
	CheckInitializedPlugin(0);
	return GetActionHandleIndex(Steam->GetDigitalActionHandle(actionName), &m_DigitalActionHandles);
}

void GetMotionData(int hInput)
{
	CheckInitializedPlugin(NORETURN);
	ValidateInputHandle(hInput, );
	Steam->GetMotionData(m_InputHandles[hInput]);
}

float GetMotionDataPosAccelX()
{
	CheckInitializedPlugin(0);
	return Steam->m_MotionData.posAccelX;
}

float GetMotionDataPosAccelY()
{
	CheckInitializedPlugin(0);
	return Steam->m_MotionData.posAccelY;
}

float GetMotionDataPosAccelZ()
{
	CheckInitializedPlugin(0);
	return Steam->m_MotionData.posAccelZ;
}

float GetMotionDataRotQuatW()
{
	CheckInitializedPlugin(0);
	return Steam->m_MotionData.rotQuatW;
}

float GetMotionDataRotQuatX()
{
	CheckInitializedPlugin(0);
	return Steam->m_MotionData.rotQuatX;
}

float GetMotionDataRotQuatY()
{
	CheckInitializedPlugin(0);
	return Steam->m_MotionData.rotQuatY;
}

float GetMotionDataRotQuatZ()
{
	CheckInitializedPlugin(0);
	return Steam->m_MotionData.rotQuatZ;
}

float GetMotionDataRotVelX()
{
	CheckInitializedPlugin(0);
	return Steam->m_MotionData.rotVelX;
}

float GetMotionDataRotVelY()
{
	CheckInitializedPlugin(0);
	return Steam->m_MotionData.rotVelY;
}

float GetMotionDataRotVelZ()
{
	CheckInitializedPlugin(0);
	return Steam->m_MotionData.rotVelZ;
}

/*
Input Action Origins
*/

char *GetAnalogActionOriginsJSON(int hInput, int hActionSet, int hAnalogAction)
{
	ValidateInputHandle(hInput, CreateString("[]"));
	ValidateActionHandle(InputActionSetHandle_t, actionSetHandle, hActionSet, m_ActionSetHandles, CreateString("[]"));
	ValidateActionHandle(InputAnalogActionHandle_t, analogActionHandle, hAnalogAction, m_AnalogActionHandles, CreateString("[]"));
	std::ostringstream json;
	json << "[";
	if (Steam)
	{
		EInputActionOrigin *origins = new EInputActionOrigin[STEAM_INPUT_MAX_ORIGINS];
		int count = Steam->GetAnalogActionOrigins(m_InputHandles[hInput], actionSetHandle, analogActionHandle, origins);
		for (int index = 0; index < count; index++)
		{
			if (index > 0)
			{
				json << ",";
			}
			json << origins[index];
		}
		delete[] origins;
	}
	json << "]";
	return CreateString(json.str());
}

char *GetDigitalActionOriginsJSON(int hInput, int hActionSet, int hDigitalAction)
{
	ValidateInputHandle(hInput, CreateString("[]"));
	ValidateActionHandle(InputActionSetHandle_t, actionSetHandle, hActionSet, m_ActionSetHandles, CreateString("[]"));
	ValidateActionHandle(InputDigitalActionHandle_t, digitalActionHandle, hDigitalAction, m_DigitalActionHandles, CreateString("[]"));
	std::ostringstream json;
	json << "[";
	if (Steam)
	{
		EInputActionOrigin *origins = new EInputActionOrigin[STEAM_INPUT_MAX_ORIGINS];
		int count = Steam->GetDigitalActionOrigins(m_InputHandles[hInput], actionSetHandle, digitalActionHandle, origins);
		for (int index = 0; index < count; index++)
		{
			if (index > 0)
			{
				json << ",";
			}
			json << origins[index];
		}
		delete[] origins;
	}
	json << "]";
	return CreateString(json.str());
}

char *GetGlyphForActionOrigin(int eOrigin)
{
	CheckInitializedPlugin(NULL_STRING);
	return CreateString(Steam->GetGlyphForActionOrigin((EInputActionOrigin)eOrigin));
}

char *GetStringForActionOrigin(int eOrigin)
{
	CheckInitializedPlugin(NULL_STRING);
	return CreateString(Steam->GetStringForActionOrigin((EInputActionOrigin)eOrigin));
}

int GetActionOriginFromXboxOrigin(int hInput, int eOrigin)
{
	ValidateInputHandle(hInput, k_EInputActionOrigin_None);
	CheckInitializedPlugin(k_EInputActionOrigin_None);
	return Steam->GetActionOriginFromXboxOrigin(m_InputHandles[hInput], (EXboxOrigin)eOrigin);
}

char *GetStringForXboxOrigin(int eOrigin)
{
	CheckInitializedPlugin(NULL_STRING);
	return CreateString(Steam->GetStringForXboxOrigin((EXboxOrigin)eOrigin));
}

char *GetGlyphForXboxOrigin(int eOrigin)
{
	CheckInitializedPlugin(NULL_STRING);
	return CreateString(Steam->GetGlyphForXboxOrigin((EXboxOrigin)eOrigin));
}

int TranslateActionOrigin(int eDestinationInputType, int eSourceOrigin)
{
	CheckInitializedPlugin(k_EInputActionOrigin_None);
	return Steam->TranslateActionOrigin((ESteamInputType)eDestinationInputType,(EInputActionOrigin)eSourceOrigin);
}

/*
Input Effects
*/
void SetInputLEDColor(int hInput, int red, int green, int blue)
{
	CheckInitializedPlugin(NORETURN);
	ValidateInputHandle(hInput, );
	Clamp(red, 0, UINT8_MAX);
	Clamp(green, 0, UINT8_MAX);
	Clamp(blue, 0, UINT8_MAX);
	Steam->SetLEDColor(m_InputHandles[hInput], red, green, blue, k_ESteamInputLEDFlag_SetColor);
}

void ResetInputLEDColor(int hInput)
{
	CheckInitializedPlugin(NORETURN);
	ValidateInputHandle(hInput, );
	Steam->SetLEDColor(m_InputHandles[hInput], 0, 0, 0, k_ESteamInputLEDFlag_RestoreUserDefault);
}

void TriggerInputHapticPulse(int hInput, int eTargetPad, int duration)
{
	CheckInitializedPlugin(NORETURN);
	ValidateInputHandle(hInput, );
	Clamp(duration, 0, USHRT_MAX);
	Steam->TriggerHapticPulse(m_InputHandles[hInput], (ESteamControllerPad)eTargetPad, (unsigned short)duration);
}

void TriggerInputRepeatedHapticPulse(int hInput, int eTargetPad, int onDuration, int offDuration, int repeat)
{
	CheckInitializedPlugin(NORETURN);
	ValidateInputHandle(hInput, );
	Clamp(onDuration, 0, USHRT_MAX);
	Clamp(offDuration, 0, USHRT_MAX);
	Clamp(repeat, 0, USHRT_MAX);
	Steam->TriggerRepeatedHapticPulse(m_InputHandles[hInput], (ESteamControllerPad)eTargetPad, (unsigned short)onDuration, (unsigned short)offDuration, (unsigned short)repeat, 0);
}

void TriggerInputVibration(int hInput, int leftSpeed, int rightSpeed)
{
	CheckInitializedPlugin(NORETURN);
	ValidateInputHandle(hInput, );
	Clamp(leftSpeed, 0, USHRT_MAX);
	Clamp(rightSpeed, 0, USHRT_MAX);
	Steam->TriggerVibration(m_InputHandles[hInput], (unsigned short)leftSpeed, (unsigned short)rightSpeed);
}

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
