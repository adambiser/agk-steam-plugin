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
#include "Utils.h"

/*
NOTE: Cannot use bool as an exported function return type because of AGK2 limitations.  Use int instead.
*/

#define MEMBLOCK_IMAGE_HEADER_LENGTH	12

static SteamPlugin *Steam;

// GLOBALS!
// TODO clean this up!
uint64 g_AppID = 480;
bool g_SteamInitialized = false;
bool g_IsGameOverlayActive = false;


/*
Check to see if the SteamPlugin has been initialized.
If it has not been, return a default value.
*/
#define CheckInitializedPlugin(returnValue)	\
	if (!g_SteamInitialized || !Steam)	\
	{									\
		return returnValue;				\
	}

// Token for passing an empty returnValue into CheckInitializedPlugin();
#define NORETURN
#define NULL_STRING utils::CreateString((const char *)NULL)

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
	CSteamID Handle Methods
*/
std::vector <uint64> m_SteamHandles;

uint64 GetSteamHandle(int handle) // from plugin handle.
{
	// Handles are 1-based!
	if (handle >= 0 && handle < (int)m_SteamHandles.size())
	{
		return m_SteamHandles[handle];
	}
	agk::PluginError("Invalid CSteamID handle.");
	return 0;
}

int GetPluginHandle(uint64 handle)
{
	int index = (int)(std::find(m_SteamHandles.begin(), m_SteamHandles.end(), handle) - m_SteamHandles.begin());
	if (index < (int)m_SteamHandles.size())
	{
		// Handles are 1-based!
		return index;
	}
	m_SteamHandles.push_back(handle);
	return (int)m_SteamHandles.size() - 1;
}

int GetPluginHandle(CSteamID steamID)
{
	return GetPluginHandle(steamID.ConvertToUint64());
}

void ClearSteamHandlesList()
{
	m_SteamHandles.clear();
	// Handle 0 is always k_steamIDNil.
	m_SteamHandles.push_back(k_steamIDNil.ConvertToUint64());
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
Input Handles
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
//#define ConvertIPToString(ip) ((ip >> 24) & 0xff) << "." << ((ip >> 16) & 0xff) << "." << ((ip >> 8) & 0xff) << "." << ((ip >> 0) & 0xff)

void ResetVariables()
{
	ClearSteamHandlesList();
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
	// TODO Remove
	if (!Steam)
	{
		Steam = new SteamPlugin();
	}
	if (!g_SteamInitialized)
	{
		ResetVariables();
		g_SteamInitialized = SteamAPI_Init();
		if (g_SteamInitialized)
		{
			g_AppID = SteamUtils()->GetAppID();
			RequestStats();
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
	SteamAPI_Shutdown();
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
	return g_SteamInitialized;
}

int RestartAppIfNecessary(int ownAppID)
{
	return SteamAPI_RestartAppIfNecessary(ownAppID);
}

/*
Returns the AppID or 0 if the Steam API has not been not initialized or the AppID cannot be found.
*/
int GetAppID()
{
	return SteamUtils()->GetAppID();
}

char *GetAppName(int appID)
{
	char name[256];
	if (SteamAppList()->GetAppName(appID, name, sizeof(name)))
	{
		return utils::CreateString(name);
	}
	return utils::CreateString("NULL");
}

/*
Returns 1 when logged into Steam.  Otherise returns 0.
*/
int LoggedOn()
{
	return SteamUser()->BLoggedOn();
}

int IsSteamIDValid(int hSteamID)
{
	CheckInitializedPlugin(false);
	CSteamID steamID = GetSteamHandle(hSteamID);
	return steamID.IsValid();
}

int GetHandleFromSteamID64(const char *steamID64)
{
	CheckInitializedPlugin(0);
	uint64 id = _atoi64(steamID64);
	return GetPluginHandle(CSteamID(id));
}

/*
Call every frame to allow the Steam API callbacks to process.
Should be used in conjunction with RequestStats and StoreStats to ensure the callbacks finish.
*/
void RunCallbacks()
{
	CheckInitializedPlugin(NORETURN);
	SteamAPI_RunCallbacks();
}

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

void DeleteCallResult(int hCallResult)
{
	CheckInitializedPlugin(NORETURN);
	Steam->DeleteCallResultItem(hCallResult);
}

int GetCallResultState(int hCallResult)
{
	CheckInitializedPlugin(ClientError);
	return Steam->GetCallResultItemState(hCallResult);
}

int GetCallResultCode(int hCallResult)
{
	CheckInitializedPlugin(0);
	if (CCallResultItem *callResult = Steam->GetCallResultItem<CCallResultItem>(hCallResult))
	{
		return callResult->GetResultCode();
	}
	return 0;
}

//char *GetCallResultJSON(int hCallResult)
//{
//	if (CCallResultItem *callResult = Steam->GetCallResultItem<CCallResultItem>(hCallResult))
//	{
//		return utils::CreateString(callResult->GetResultJSON());
//	}
//	return utils::CreateString("{}");
//};

// App/DLC methods
int GetDLCCount()
{
	CheckInitializedPlugin(0);
	return SteamApps()->GetDLCCount();
}

char *GetDLCDataJSON()
{
	std::string json("[");
	if (Steam)
	{
		for (int index = 0; index < SteamApps()->GetDLCCount(); index++)
		{
			if (index > 0)
			{
				json += ", ";
			}
			plugin::DLCData_t dlc;
			bool success = SteamApps()->BGetDLCDataByIndex(index, &dlc.m_AppID, &dlc.m_bAvailable, dlc.m_chName, sizeof(dlc.m_chName));
			if (!success)
			{
				dlc.Clear();
			}
			//json += ToJSON(dlc);
		}
	}
	json += "]";
	return utils::CreateString(json);
}

int IsAppInstalled(int appID)
{
	CheckInitializedPlugin(false);
	return SteamApps()->BIsAppInstalled(appID);
}

int IsCybercafe()
{
	CheckInitializedPlugin(false);
	return SteamApps()->BIsCybercafe();
}

int IsDLCInstalled(int appID)
{
	CheckInitializedPlugin(false);
	return SteamApps()->BIsDlcInstalled(appID);
}

int IsLowViolence()
{
	CheckInitializedPlugin(false);
	return SteamApps()->BIsLowViolence();
}

int IsSubscribed()
{
	CheckInitializedPlugin(false);
	return SteamApps()->BIsSubscribed();
}

int IsSubscribedApp(int appID)
{
	CheckInitializedPlugin(false);
	return SteamApps()->BIsSubscribedApp(appID);
}

int IsSubscribedFromFamilySharing()
{
	CheckInitializedPlugin(false);
	return SteamApps()->BIsSubscribedFromFamilySharing();
}

int IsSubscribedFromFreeWeekend()
{
	CheckInitializedPlugin(false);
	return SteamApps()->BIsSubscribedFromFreeWeekend();
}

int IsVACBanned()
{
	CheckInitializedPlugin(false);
	return SteamApps()->BIsVACBanned();
}

int GetAppBuildID()
{
	CheckInitializedPlugin(false);
	return SteamApps()->GetAppBuildId();
}

char *GetAppInstallDir(int appID)
{
	CheckInitializedPlugin(NULL_STRING);
	char folder[MAX_PATH];
	uint32 length = SteamApps()->GetAppInstallDir(appID, folder, sizeof(folder));
	if (length)
	{
		//length++; // NULL terminator
		//char *result = agk::utils::CreateString(length);
		//strcpy_s(result, length, folder);
		//return result;
		// TODO Verify this works as expected without using the returned length.
		return utils::CreateString(folder);
	}
	else
	{
		return NULL_STRING;
	}
}

int GetAppOwner()
{
	CheckInitializedPlugin(false);
	return GetPluginHandle(SteamApps()->GetAppOwner());
}

char *GetAvailableGameLanguages()
{
	CheckInitializedPlugin(NULL_STRING);
	return utils::CreateString(SteamApps()->GetAvailableGameLanguages());
}

char *GetCurrentBetaName()
{
	CheckInitializedPlugin(NULL_STRING);
	char name[256];
	if (SteamApps()->GetCurrentBetaName(name, sizeof(name)))
	{
		return utils::CreateString(name);
	}
	return NULL_STRING;
}

char *GetCurrentGameLanguage()
{
	CheckInitializedPlugin(NULL_STRING);
	return utils::CreateString(SteamApps()->GetCurrentGameLanguage());
}

char *GetDLCDownloadProgressJSON(int appID)
{
	if (Steam)
	{
		plugin::DownloadProgress_t progress;
		bool downloading = SteamApps()->GetDlcDownloadProgress(appID, &progress.m_unBytesDownloaded, &progress.m_unBytesTotal);
		if (!downloading)
		{
			progress.Clear();
		}
		//return utils::CreateString(ToJSON(progress));
	}
	return utils::CreateString("{}");
}

int GetEarliestPurchaseUnixTime(int appID)
{
	CheckInitializedPlugin(false);
	return SteamApps()->GetEarliestPurchaseUnixTime(appID);
}

//SteamAPICall_t GetFileDetails(const char*pszFileName); // FileDetailsResult_t call result.

char *GetInstalledDepotsJSON(int appID, int maxDepots)
{
	if (Steam)
	{
		//DepotId_t *depots = new DepotId_t[maxDepots];
		//int count = SteamApps()->GetInstalledDepots(appID, depots, maxDepots);
		//std::string json = ToJSON(depots, count);
		//delete[] depots;
		//return utils::CreateString(json);
	}
	return utils::CreateString("[]");
}

char *GetLaunchQueryParam(const char *key)
{
	CheckInitializedPlugin(NULL_STRING);
	return utils::CreateString(SteamApps()->GetLaunchQueryParam(key));
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
	int length = SteamApps()->GetLaunchCommandLine(cmd, 2084);
	cmd[length] = 0;
	return utils::CreateString(cmd);
}

int HasNewDLCInstalled()
{
	CheckInitializedPlugin(false);
	return Steam->HasNewDlcInstalled();
}

int GetNewDLCInstalled()
{
	CheckInitializedPlugin(0);
	return Steam->GetNewDlcInstalled().m_nAppID;
}

void InstallDLC(int appID) // Triggers a DlcInstalled_t callback.
{
	CheckInitializedPlugin(NORETURN);
	//TODO m_OnDlcInstalledEnabled = true;
	SteamApps()->InstallDLC(appID);
}

int MarkContentCorrupt(int missingFilesOnly)
{
	CheckInitializedPlugin(false);
	return SteamApps()->MarkContentCorrupt(missingFilesOnly != 0);
}

void UninstallDLC(int appID)
{
	CheckInitializedPlugin(NORETURN);
	SteamApps()->UninstallDLC(appID);
}

// Overlay methods
int IsGameOverlayActive()
{
	CheckInitializedPlugin(false);
	return g_IsGameOverlayActive;
}

void ActivateGameOverlay(const char *dialogName)
{
	CheckInitializedPlugin(NORETURN);
	SteamFriends()->ActivateGameOverlay(dialogName);
}

void ActivateGameOverlayInviteDialog(int hLobbySteamID)
{
	CheckInitializedPlugin(NORETURN);
	SteamFriends()->ActivateGameOverlayInviteDialog(GetSteamHandle(hLobbySteamID));
}

void ActivateGameOverlayToStore(int appID, int flag)
{
	CheckInitializedPlugin(NORETURN);
	SteamFriends()->ActivateGameOverlayToStore(appID, (EOverlayToStoreFlag)flag);
}

void ActivateGameOverlayToUser(const char *dialogName, int hSteamID)
{
	CheckInitializedPlugin(NORETURN);
	SteamFriends()->ActivateGameOverlayToUser(dialogName, GetSteamHandle(hSteamID));
}

void ActivateGameOverlayToWebPage(const char *url)
{
	CheckInitializedPlugin(NORETURN);
	SteamFriends()->ActivateGameOverlayToWebPage(url, k_EActivateGameOverlayToWebPageMode_Default);
}

void ActivateGameOverlayToWebPageModal(const char *url)
{
	CheckInitializedPlugin(NORETURN);
	SteamFriends()->ActivateGameOverlayToWebPage(url, k_EActivateGameOverlayToWebPageMode_Modal);
}

char *GetPersonaName()
{
	CheckInitializedPlugin(NULL_STRING);
	return utils::CreateString(SteamFriends()->GetPersonaName());
}

int GetSteamID()
{
	CheckInitializedPlugin(0);
	return GetPluginHandle(Steam->GetSteamID());
}

int GetAccountID(int hUserSteamID)
{
	CheckInitializedPlugin(0);
	CSteamID steamID = GetSteamHandle(hUserSteamID);
	if (steamID != k_steamIDNil)
	{
		return steamID.GetAccountID();
	}
	return 0;
}

char *GetSteamID3(int hUserSteamID)
{
	CheckInitializedPlugin(NULL_STRING);
	CSteamID steamID = GetSteamHandle(hUserSteamID);
	if (steamID != k_steamIDNil)
	{
		std::string steam3("[");
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
			steam3 += "IUMGAPCgT?a"[steamID.GetEAccountType()];
			break;
		case k_EAccountTypeConsoleUser: // 9 = P2P, but this doesn't look right.  Fake anyway, just do the default below.
		default:
			steam3 += std::to_string(steamID.GetEAccountType());
			break;
		}
		steam3 += ":" + std::to_string(steamID.GetEUniverse()) + ":" + std::to_string(steamID.GetAccountID());
		switch (steamID.GetEAccountType())
		{
		case k_EAccountTypeAnonGameServer:
		case k_EAccountTypeAnonUser:
			steam3 += ":" + std::to_string(steamID.GetUnAccountInstance());
			break;
		}
		steam3 += "]";
		return utils::CreateString(steam3);
	}
	return NULL_STRING;
}

char *GetSteamID64(int hUserSteamID)
{
	CheckInitializedPlugin(NULL_STRING);
	char id64[21]; // Max value is 18,446,744,073,709,551,615
	_i64toa(GetSteamHandle(hUserSteamID), id64, 10);
	return utils::CreateString(id64);
}

int HasPersonaStateChange()
{
	CheckInitializedPlugin(false);
	return Steam->HasPersonaStateChange();
}

char *GetPersonaStateChangeJSON()
{
	CheckInitializedPlugin(0);
	//return utils::CreateString(ToJSON(Steam->GetPersonaStateChange()));
	return utils::CreateString("{}");
}

int RequestUserInformation(int hUserSteamID, int requireNameOnly)
{
	CheckInitializedPlugin(false);
	return SteamFriends()->RequestUserInformation(GetSteamHandle(hUserSteamID), requireNameOnly != 0);
}

int HasAvatarImageLoaded()
{
	CheckInitializedPlugin(false);
	return Steam->HasAvatarImageLoadedUser();
}

int GetAvatarImageLoadedUser()
{
	CheckInitializedPlugin(0);
	return GetPluginHandle(Steam->GetAvatarImageLoadedUser());
}

int GetFriendAvatar(int hUserSteamID, int size)
{
	CheckInitializedPlugin(0);
	return Steam->GetFriendAvatar(GetSteamHandle(hUserSteamID), (EAvatarSize)size);
}

char *GetFriendListJSON(int friendFlags)
{
	std::string json("[");
	if (Steam)
	{
		for (int x = 0; x < SteamFriends()->GetFriendCount((EFriendFlags)friendFlags); x++)
		{
			if (x > 0)
			{
				json +=  ", ";
			}
			json += std::to_string(GetPluginHandle(SteamFriends()->GetFriendByIndex(x, (EFriendFlags)friendFlags)));
		}
	}
	json += "]";
	return utils::CreateString(json);
}

char *GetFriendGamePlayedJSON(int hUserSteamID)
{
	// TODO m_PersonaStateChangedEnabled = true;
	if (Steam)
	{
		FriendGameInfo_t pFriendGameInfo;
		bool ingame = SteamFriends()->GetFriendGamePlayed(GetSteamHandle(hUserSteamID), &pFriendGameInfo);
		//return utils::CreateString(ToJSON(ingame, pFriendGameInfo));
	}
	return utils::CreateString("{}");
}

char *GetFriendPersonaName(int hUserSteamID)
{
	CheckInitializedPlugin(NULL_STRING);
	// TODO m_PersonaStateChangedEnabled = true;
	return utils::CreateString(SteamFriends()->GetFriendPersonaName(GetSteamHandle(hUserSteamID)));
}

int GetFriendPersonaState(int hUserSteamID)
{
	CheckInitializedPlugin(-1);
	// TODO m_PersonaStateChangedEnabled = true;
	return SteamFriends()->GetFriendPersonaState(GetSteamHandle(hUserSteamID));
}

int GetFriendRelationship(int hUserSteamID)
{
	CheckInitializedPlugin(-1);
	return SteamFriends()->GetFriendRelationship(GetSteamHandle(hUserSteamID));
}

/*
NOTE:
Steam docs say
"If the Steam level is not immediately available for the specified user then this returns 0 and queues it to be downloaded from the Steam servers.
When it gets downloaded a PersonaStateChange_t callback will be posted with m_nChangeFlags including k_EPersonaChangeSteamLevel."

HOWEVER, this doesn't actually appear to be the case.  GetFriendSteamLevel returns 0, but the callback never seems to fire.
Possible solution is to just keep requesting the level when 0 is returned.
*/
int GetFriendSteamLevel(int hUserSteamID)
{
	CheckInitializedPlugin(0);
	// TODO m_PersonaStateChangedEnabled = true;
	return SteamFriends()->GetFriendSteamLevel(GetSteamHandle(hUserSteamID));
}

char *GetPlayerNickname(int hUserSteamID)
{
	CheckInitializedPlugin(NULL_STRING);
	return utils::CreateString(SteamFriends()->GetPlayerNickname(GetSteamHandle(hUserSteamID)));
}

int HasFriend(int hUserSteamID, int friendFlags)
{
	CheckInitializedPlugin(false);
	return SteamFriends()->HasFriend(GetSteamHandle(hUserSteamID), (EFriendFlags)friendFlags);
}

int GetFriendsGroupCount()
{
	CheckInitializedPlugin(0);
	return SteamFriends()->GetFriendsGroupCount();
}

int GetFriendsGroupIDByIndex(int index)
{
	CheckInitializedPlugin(k_FriendsGroupID_Invalid);
	return SteamFriends()->GetFriendsGroupIDByIndex(index);
}

int GetFriendsGroupMembersCount(int hFriendsGroupID)
{
	CheckInitializedPlugin(0);
	return SteamFriends()->GetFriendsGroupMembersCount(hFriendsGroupID);
}

char *GetFriendsGroupMembersListJSON(int hFriendsGroupID) // return a json array of SteamID handles
{
	if (Steam)
	{
		int memberCount = SteamFriends()->GetFriendsGroupMembersCount(hFriendsGroupID);
		if (memberCount > 0)
		{
			CSteamID *pSteamIDMembers = new CSteamID[memberCount];
			SteamFriends()->GetFriendsGroupMembersList(hFriendsGroupID, pSteamIDMembers, memberCount);
			//std::string json = ToJSON(pSteamIDMembers, memberCount);
			delete[] pSteamIDMembers;
			//return utils::CreateString(json);
		}
	}
	return utils::CreateString("[]");
}

char *GetFriendsGroupName(int hFriendsGroupID)
{
	CheckInitializedPlugin(NULL_STRING);
	return utils::CreateString(SteamFriends()->GetFriendsGroupName(hFriendsGroupID));
}

int LoadImageFromHandle(int imageID, int hImage)
{
	if (hImage == -1 || hImage == 0)
	{
		return 0;
	}
	uint32 width;
	uint32 height;
	if (!SteamUtils()->GetImageSize(hImage, &width, &height))
	{
		agk::PluginError("GetImageSize failed.");
		return 0;
	}
	// Get the actual raw RGBA data from Steam and turn it into a texture in our game engine
	const int imageSizeInBytes = width * height * 4;
	uint8 *imageBuffer = new uint8[imageSizeInBytes];
	if (SteamUtils()->GetImageRGBA(hImage, imageBuffer, imageSizeInBytes))
	{
		unsigned int memID = agk::CreateMemblock(imageSizeInBytes + MEMBLOCK_IMAGE_HEADER_LENGTH);
		agk::SetMemblockInt(memID, 0, width);
		agk::SetMemblockInt(memID, 4, height);
		agk::SetMemblockInt(memID, 8, 32); // bitdepth always 32
		memcpy_s(agk::GetMemblockPtr(memID) + MEMBLOCK_IMAGE_HEADER_LENGTH, imageSizeInBytes, imageBuffer, imageSizeInBytes);
		if (imageID)
		{
			agk::CreateImageFromMemblock(imageID, memID);
		}
		else
		{
			imageID = agk::CreateImageFromMemblock(memID);
		}
		agk::DeleteMemblock(memID);
	}
	else
	{
		imageID = 0;
		agk::PluginError("GetImageRGBA failed.");
	}
	// Free memory.
	delete[] imageBuffer;
	return imageID;
}

int LoadImageFromHandle(int hImage)
{
	CheckInitializedPlugin(0);
	return LoadImageFromHandle(0, hImage);
}

void LoadImageIDFromHandle(int imageID, int hImage)
{
	CheckInitializedPlugin(NORETURN);
	LoadImageFromHandle(imageID, hImage);
}

/*
Requests the user stats.
Returns 1 if the process succeeds.  Otherwise, returns 0.
Use StatsInitialized() to determine when user stats are initialized.
*/
int RequestStats()
{
	CheckInitializedPlugin(false);
	// Set to false so that we can detect when the callback finishes for this request.
	//m_StatsInitialized = false;
	//if (!LoggedOn())
	//{
	//	return false;
	//}
	// Fail when the callback is currently running.
	//if (m_RequestStatsCallbackState == Running)
	//{
	//	return false;
	//}
	return SteamUserStats()->RequestCurrentStats();
	//{
	//	//m_RequestStatsCallbackState = ClientError;
	//	return false;
	//}
	////m_RequestStatsCallbackState = Running;
	//return true;
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
	return SteamUserStats()->GetNumAchievements();
}

/*
Gets the achievement API name for the given achievement index.
If user stats have not been initialized, an empty string is returned.
Use StatsInitialized() to determine when user stats are initialized before calling this method.
*/
char *GetAchievementAPIName(int index)
{
	CheckInitializedPlugin(NULL_STRING);
	return utils::CreateString(SteamUserStats()->GetAchievementName(index));
}

char *GetAchievementDisplayName(const char *name)
{
	CheckInitializedPlugin(NULL_STRING);
	return utils::CreateString(SteamUserStats()->GetAchievementDisplayAttribute(name, "name"));
}

char *GetAchievementDisplayDesc(const char *name)
{
	CheckInitializedPlugin(NULL_STRING);
	return utils::CreateString(SteamUserStats()->GetAchievementDisplayAttribute(name, "desc"));
}

int GetAchievementDisplayHidden(const char *name)
{
	CheckInitializedPlugin(false);
	return (strcmp(SteamUserStats()->GetAchievementDisplayAttribute(name, "hidden"), "1") == 0);
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
	if (SteamUserStats()->GetAchievement(name, &result))
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
	return SteamUserStats()->SetAchievement(name);
}

/*
Raises a notification about achievemnt progress for progress stat achievements.
The notification only shows when current progress is less than the max.
SetStat still needs to be used to set the progress stat value.
*/
int IndicateAchievementProgress(const char *name, int curProgress, int maxProgress)
{
	CheckInitializedPlugin(false);
	return SteamUserStats()->IndicateAchievementProgress(name, curProgress, maxProgress);
}

/*
Clears an achievement.
Returns 1 if the process succeeds.  Otherwise, returns 0.
Use StatsInitialized() to determine when user stats are initialized before calling this method.
*/
int ClearAchievement(const char *name)
{
	CheckInitializedPlugin(false);
	return SteamUserStats()->ClearAchievement(name);
}

/*
Gets an integer user stat value.
If there is a problem, 0 is returned and the error is reported to the AGK interpreter.
*/
int GetStatInt(const char *name)
{
	CheckInitializedPlugin(0);
	int result = 0;
	if (SteamUserStats()->GetStat(name, &result)) {
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
	if (SteamUserStats()->GetStat(name, &result)) {
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
	return SteamUserStats()->SetStat(name, value);
}

/*
Sets a float user stat value.
Returns 1 if the call succeeds.  Otherwise returns 0.
*/
int SetStatFloat(const char *name, float value)
{
	CheckInitializedPlugin(false);
	return SteamUserStats()->SetStat(name, value);
}

/*
Updates an average rate user stat.  Steam takes care of the averaging.  Use GetStatFloat to get the result.
Returns 1 if the call succeeds.  Otherwise returns 0.
*/
int UpdateAvgRateStat(const char *name, float countThisSession, float sessionLength)
{
	CheckInitializedPlugin(false);
	return SteamUserStats()->UpdateAvgRateStat(name, countThisSession, (double)sessionLength);
}

int FindLeaderboard(const char *leaderboardName)
{
	CheckInitializedPlugin(false);
	return Steam->FindLeaderboard(leaderboardName);
}

char *GetLeaderboardName(int hLeaderboard)
{
	CheckInitializedPlugin(NULL_STRING);
	return utils::CreateString(SteamUserStats()->GetLeaderboardName(GetSteamHandle(hLeaderboard)));
}

int GetLeaderboardEntryCount(int hLeaderboard)
{
	CheckInitializedPlugin(0);
	return SteamUserStats()->GetLeaderboardEntryCount(GetSteamHandle(hLeaderboard));
}

int GetLeaderboardDisplayType(int hLeaderboard)
{
	CheckInitializedPlugin(0);
	return SteamUserStats()->GetLeaderboardDisplayType(GetSteamHandle(hLeaderboard));
}

int GetLeaderboardSortMethod(int hLeaderboard)
{
	CheckInitializedPlugin(0);
	return SteamUserStats()->GetLeaderboardSortMethod(GetSteamHandle(hLeaderboard));
}

// TODO Add force parameter? 
// TODO Allow for a data memblock of integers.
int UploadLeaderboardScore(int hLeaderboard, int score)
{
	CheckInitializedPlugin(0);
	return Steam->UploadLeaderboardScore(GetSteamHandle(hLeaderboard), k_ELeaderboardUploadScoreMethodKeepBest, score);
}

int UploadLeaderboardScoreForceUpdate(int hLeaderboard, int score)
{
	CheckInitializedPlugin(0);
	return Steam->UploadLeaderboardScore(GetSteamHandle(hLeaderboard), k_ELeaderboardUploadScoreMethodForceUpdate, score);
}

int DownloadLeaderboardEntries(int hLeaderboard, int eLeaderboardDataRequest, int nRangeStart, int nRangeEnd)
{
	CheckInitializedPlugin(0);
	return Steam->DownloadLeaderboardEntries(GetSteamHandle(hLeaderboard), (ELeaderboardDataRequest)eLeaderboardDataRequest, nRangeStart, nRangeEnd);
}

void AddRequestLobbyListDistanceFilter(int eLobbyDistanceFilter)
{
	CheckInitializedPlugin(NORETURN);
	return SteamMatchmaking()->AddRequestLobbyListDistanceFilter((ELobbyDistanceFilter)eLobbyDistanceFilter);
}

void AddRequestLobbyListFilterSlotsAvailable(int slotsAvailable)
{
	CheckInitializedPlugin(NORETURN);
	return SteamMatchmaking()->AddRequestLobbyListFilterSlotsAvailable(slotsAvailable);
}

void AddRequestLobbyListNearValueFilter(const char *keyToMatch, int valueToBeCloseTo)
{
	CheckInitializedPlugin(NORETURN);
	return SteamMatchmaking()->AddRequestLobbyListNearValueFilter(keyToMatch, valueToBeCloseTo);
}

void AddRequestLobbyListNumericalFilter(const char *keyToMatch, int valueToMatch, int eComparisonType)
{
	CheckInitializedPlugin(NORETURN);
	return SteamMatchmaking()->AddRequestLobbyListNumericalFilter(keyToMatch, valueToMatch, (ELobbyComparison)eComparisonType);
}

void AddRequestLobbyListResultCountFilter(int maxResults)
{
	CheckInitializedPlugin(NORETURN);
	return SteamMatchmaking()->AddRequestLobbyListResultCountFilter(maxResults);
}

void AddRequestLobbyListStringFilter(const char *keyToMatch, const char *valueToMatch, int eComparisonType)
{
	CheckInitializedPlugin(NORETURN);
	return SteamMatchmaking()->AddRequestLobbyListStringFilter(keyToMatch, valueToMatch, (ELobbyComparison)eComparisonType);
}

int RequestLobbyList()
{
	CheckInitializedPlugin(0);
	return Steam->RequestLobbyList();
}

int CreateLobby(int eLobbyType, int maxMembers)
{
	CheckInitializedPlugin(0);
	return Steam->CreateLobby((ELobbyType)eLobbyType, maxMembers);
}

//int SetLinkedLobby(int hLobbySteamID, int hLobbyDependentSteamID)
//{
//	CheckInitializedPlugin(false);
//	return Steam->SetLinkedLobby(GetSteamHandle(hLobbySteamID), GetSteamHandle(hLobbyDependentSteamID));
//}

int SetLobbyJoinable(int hLobbySteamID, int lobbyJoinable)
{
	CheckInitializedPlugin(false);
	return SteamMatchmaking()->SetLobbyJoinable(GetSteamHandle(hLobbySteamID), lobbyJoinable != 0);
}

int SetLobbyType(int hLobbySteamID, int eLobbyType)
{
	CheckInitializedPlugin(false);
	return SteamMatchmaking()->SetLobbyType(GetSteamHandle(hLobbySteamID), (ELobbyType)eLobbyType);
}

int JoinLobby(int hLobbySteamID)
{
	CheckInitializedPlugin(false);
	return Steam->JoinLobby(GetSteamHandle(hLobbySteamID));
}

int HasLobbyEnterResponse()
{
	CheckInitializedPlugin(false);
	return Steam->HasLobbyEnter();
}

char *GetLobbyEnterResponseJSON()
{
	CheckInitializedPlugin(NULL_STRING);
	// Must be the same as in CLobbyEnterCallResult.
	return utils::CreateString("{}"); // ToJSON(Steam->GetLobbyEnter()));
}

int InviteUserToLobby(int hLobbySteamID, int hInviteeSteamID)
{
	CheckInitializedPlugin(false);
	return SteamMatchmaking()->InviteUserToLobby(GetSteamHandle(hLobbySteamID), GetSteamHandle(hInviteeSteamID));
}

int HasGameLobbyJoinRequest()
{
	CheckInitializedPlugin(0);
	return Steam->HasGameLobbyJoinRequested();
}

int GetGameLobbyJoinRequestedLobby()
{
	CheckInitializedPlugin(0);
	return GetPluginHandle(Steam->GetGameLobbyJoinRequested().m_steamIDLobby);
}

void LeaveLobby(int hLobbySteamID)
{
	CheckInitializedPlugin(NORETURN);
	Steam->LeaveLobby(GetSteamHandle(hLobbySteamID));
}

// Lobby methods: Game server
char *GetLobbyGameServerJSON(int hLobbySteamID)
{
	if (Steam)
	{
		plugin::LobbyGameServer_t server;
		if (SteamMatchmaking()->GetLobbyGameServer(GetSteamHandle(hLobbySteamID), &server.m_unGameServerIP, &server.m_unGameServerPort, &server.m_ulSteamIDGameServer))
		{
			//return utils::CreateString(ToJSON(server));
		}
	}
	return utils::CreateString("{}");
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
	if (!utils::ParseIP(gameServerIP, &ip))
	{
		return false;
	}
	SteamMatchmaking()->SetLobbyGameServer(GetSteamHandle(hLobbySteamID), ip, gameServerPort, GetSteamHandle(hGameServerSteamID));
	return true;
}

int HasLobbyGameCreated()
{
	CheckInitializedPlugin(false);
	return Steam->HasLobbyGameCreated();
}

char *GetLobbyGameCreatedJSON()
{
	if (Steam)
	{
		//return utils::CreateString(ToJSON(Steam->GetLobbyGameCreated()));
	}
	return utils::CreateString("{}");
}

// Lobby methods: Data
char *GetLobbyData(int hLobbySteamID, const char *key)
{
	CheckInitializedPlugin(NULL_STRING);
	return utils::CreateString(SteamMatchmaking()->GetLobbyData(GetSteamHandle(hLobbySteamID), key));
}

char *GetLobbyDataJSON(int hLobbySteamID)
{
	CheckInitializedPlugin(NULL_STRING);
	CSteamID steamIDLobby = GetSteamHandle(hLobbySteamID);
	std::string json("[");
	for (int index = 0; index < SteamMatchmaking()->GetLobbyDataCount(steamIDLobby); index++)
	{
		char key[k_nMaxLobbyKeyLength];
		char value[k_cubChatMetadataMax];
		if (SteamMatchmaking()->GetLobbyDataByIndex(steamIDLobby, index, key, k_nMaxLobbyKeyLength, value, k_cubChatMetadataMax))
		{
			if (index > 0)
			{
				json += ", ";
			}
			//json += ToJSON(key, value);
		}
		else
		{
			utils::PluginError("GetLobbyDataByIndex: Error reading index " + std::to_string(index) + " of lobby " + std::to_string(hLobbySteamID));
			return NULL_STRING;
		}
	}
	json += "]";
	return utils::CreateString(json);
}

void SetLobbyData(int hLobbySteamID, const char *key, const char *value)
{
	CheckInitializedPlugin(NORETURN);
	SteamMatchmaking()->SetLobbyData(GetSteamHandle(hLobbySteamID), key, value);
}

int DeleteLobbyData(int hLobbySteamID, const char *key)
{
	CheckInitializedPlugin(false);
	return SteamMatchmaking()->DeleteLobbyData(GetSteamHandle(hLobbySteamID), key);
}

int RequestLobbyData(int hLobbySteamID)
{
	CheckInitializedPlugin(false);
	return SteamMatchmaking()->RequestLobbyData(GetSteamHandle(hLobbySteamID));
}

int HasLobbyDataUpdateResponse()
{
	CheckInitializedPlugin(false);
	return Steam->HasLobbyDataUpdate();
}

char *GetLobbyDataUpdateResponseJSON()
{
	CheckInitializedPlugin(NULL_STRING);
	//return utils::CreateString(ToJSON(Steam->GetLobbyDataUpdate()));
	return utils::CreateString("{}");
}

char *GetLobbyMemberData(int hLobbySteamID, int hUserSteamID, const char *key)
{
	CheckInitializedPlugin(NULL_STRING);
	return utils::CreateString(SteamMatchmaking()->GetLobbyMemberData(GetSteamHandle(hLobbySteamID), GetSteamHandle(hUserSteamID), key));
}

void SetLobbyMemberData(int hLobbySteamID, const char *key, const char *value)
{
	CheckInitializedPlugin(NORETURN);
	return SteamMatchmaking()->SetLobbyMemberData(GetSteamHandle(hLobbySteamID), key, value);
}

int GetLobbyOwner(int hLobbySteamID)
{
	CheckInitializedPlugin(0);
	return GetPluginHandle(SteamMatchmaking()->GetLobbyOwner(GetSteamHandle(hLobbySteamID)));
}

int SetLobbyOwner(int hLobbySteamID, int hNewOwnerSteamID)
{
	CheckInitializedPlugin(0);
	return SteamMatchmaking()->SetLobbyOwner(GetSteamHandle(hLobbySteamID), GetSteamHandle(hNewOwnerSteamID));
}

int GetLobbyMemberLimit(int hLobbySteamID)
{
	CheckInitializedPlugin(0);
	return SteamMatchmaking()->GetLobbyMemberLimit(GetSteamHandle(hLobbySteamID));
}

int SetLobbyMemberLimit(int hLobbySteamID, int maxMembers)
{
	CheckInitializedPlugin(0);
	return SteamMatchmaking()->SetLobbyMemberLimit(GetSteamHandle(hLobbySteamID), maxMembers);
}

int GetNumLobbyMembers(int hLobbySteamID)
{
	CheckInitializedPlugin(0);
	return SteamMatchmaking()->GetNumLobbyMembers(GetSteamHandle(hLobbySteamID));
}

int GetLobbyMemberByIndex(int hLobbySteamID, int index)
{
	CheckInitializedPlugin(0);
	return GetPluginHandle(SteamMatchmaking()->GetLobbyMemberByIndex(GetSteamHandle(hLobbySteamID), index));
}

int HasLobbyChatUpdate()
{
	CheckInitializedPlugin(false);
	return Steam->HasLobbyChatUpdate();
}

// TODO JSON
int GetLobbyChatUpdateLobby()
{
	CheckInitializedPlugin(0);
	return GetPluginHandle(Steam->GetLobbyChatUpdate().m_ulSteamIDLobby);
}

int GetLobbyChatUpdateUserChanged()
{
	CheckInitializedPlugin(0);
	return GetPluginHandle(Steam->GetLobbyChatUpdate().m_ulSteamIDUserChanged);
}

int GetLobbyChatUpdateUserState()
{
	CheckInitializedPlugin(0);
	return Steam->GetLobbyChatUpdate().m_rgfChatMemberStateChange;
}

int GetLobbyChatUpdateUserMakingChange()
{
	CheckInitializedPlugin(0);
	return GetPluginHandle(Steam->GetLobbyChatUpdate().m_ulSteamIDMakingChange);
}

int HasLobbyChatMessage()
{
	CheckInitializedPlugin(false);
	return Steam->HasLobbyChatMessage();
}

// TODO JSON
int GetLobbyChatMessageUser()
{
	CheckInitializedPlugin(0);
	return GetPluginHandle(Steam->GetLobbyChatMessage().m_ulSteamIDUser);
}

char *GetLobbyChatMessageText()
{
	CheckInitializedPlugin(NULL_STRING);
	return utils::CreateString(Steam->GetLobbyChatMessage().m_chChatEntry);
}

int SendLobbyChatMessage(int hLobbySteamID, const char *msg)
{
	CheckInitializedPlugin(0);
	return SteamMatchmaking()->SendLobbyChatMsg(GetSteamHandle(hLobbySteamID), msg, (int)strlen(msg) + 1);
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
	if (!utils::ParseIP(ipv4, &ip))
	{
		return 0;
	}
	std::time_t now = std::time(0);
	//agk::Message(agk::Str((int)now));
	return SteamMatchmaking()->AddFavoriteGame(appID, ip, connectPort, queryPort, flags, (uint32)now);
}

int GetFavoriteGameCount()
{
	CheckInitializedPlugin(0);
	return SteamMatchmaking()->GetFavoriteGameCount();
}

// Leave this as by-index since AddFavoriteGame appears to return an index.
char *GetFavoriteGameJSON(int index)
{
	if (Steam)
	{
		plugin::FavoriteGame_t info;
		if (SteamMatchmaking()->GetFavoriteGame(index, &info.m_AppID, &info.m_nIP, &info.m_nConnPort, &info.m_nQueryPort, &info.m_unFlags, &info.m_rTime32LastPlayedOnServer))
		{
			//return utils::CreateString(ToJSON(info));
		}
	}
	return utils::CreateString("{}");
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
	if (!utils::ParseIP(ipv4, &ip))
	{
		return 0;
	}
	return SteamMatchmaking()->RemoveFavoriteGame(appID, ip, connectPort, queryPort, flags);
}

//char *GetPublicIP()
//{
//	CheckInitializedPlugin(NULL_STRING);
//	uint32 ip = Steam->GetPublicIP();
//	std::ostringstream ipc;
//	ipc << ConvertIPToString(ip);
//	return utils::CreateString(ipc);
//}

int IsMusicEnabled()
{
	CheckInitializedPlugin(0);
	return SteamMusic()->BIsEnabled();
}

int IsMusicPlaying()
{
	CheckInitializedPlugin(0);
	return SteamMusic()->BIsPlaying();
}

int GetMusicPlaybackStatus()
{
	CheckInitializedPlugin(AudioPlayback_Undefined);
	return SteamMusic()->GetPlaybackStatus();
}

float GetMusicVolume()
{
	CheckInitializedPlugin(0.0);
	return SteamMusic()->GetVolume();
}

void PauseMusic()
{
	CheckInitializedPlugin(NORETURN);
	SteamMusic()->Pause();
}

void PlayMusic()
{
	CheckInitializedPlugin(NORETURN);
	SteamMusic()->Play();
}

void PlayNextSong()
{
	CheckInitializedPlugin(NORETURN);
	SteamMusic()->PlayNext();
}

void PlayPreviousSong()
{
	CheckInitializedPlugin(NORETURN);
	SteamMusic()->PlayPrevious();
}

void SetMusicVolume(float volume)
{
	CheckInitializedPlugin(NORETURN);
	SteamMusic()->SetVolume(volume);
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
	return SteamUtils()->GetCurrentBatteryPower();
}

int GetIPCCallCount()
{
	CheckInitializedPlugin(0);
	return SteamUtils()->GetIPCCallCount();
}

char *GetIPCountry()
{
	CheckInitializedPlugin(NULL_STRING);
	return utils::CreateString(SteamUtils()->GetIPCountry());
}

int GetSecondsSinceAppActive()
{
	CheckInitializedPlugin(0);
	return SteamUtils()->GetSecondsSinceAppActive();
}

int GetSecondsSinceComputerActive()
{
	CheckInitializedPlugin(0);
	return SteamUtils()->GetSecondsSinceComputerActive();
}

int GetServerRealTime()
{
	CheckInitializedPlugin(0);
	return SteamUtils()->GetServerRealTime();
}

char *GetSteamUILanguage()
{
	CheckInitializedPlugin(NULL_STRING);
	return utils::CreateString(SteamUtils()->GetSteamUILanguage());
}

int IsOverlayEnabled()
{
	CheckInitializedPlugin(0);
	return SteamUtils()->IsOverlayEnabled();
}

void SetOverlayNotificationInset(int horizontalInset, int verticalInset)
{
	CheckInitializedPlugin(NORETURN);
	SteamUtils()->SetOverlayNotificationInset(horizontalInset, verticalInset);
}

void SetOverlayNotificationPosition(int eNotificationPosition)
{
	CheckInitializedPlugin(NORETURN);
	SteamUtils()->SetOverlayNotificationPosition((ENotificationPosition) eNotificationPosition);
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
	return Steam->HasSteamShutdownNotification();
}

extern "C" void __cdecl SteamAPIDebugTextHook(int nSeverity, const char *pchDebugText)
{
	agk::Log(pchDebugText);
}

void SetWarningMessageHook()
{
	CheckInitializedPlugin(NORETURN);
	SteamUtils()->SetWarningMessageHook(&SteamAPIDebugTextHook);
}

// Big Picture methods
int IsSteamInBigPictureMode()
{
	CheckInitializedPlugin(0);
	return SteamUtils()->IsSteamInBigPictureMode();
}

int HasGamepadTextInputDismissedInfo()
{
	CheckInitializedPlugin(0);
	return Steam->HasGamepadTextInputDismissed();
}

char *GetGamepadTextInputDismissedInfoJSON()
{
	if (Steam)
	{
		//return utils::CreateString(ToJSON(Steam->GetGamepadTextInputDismissed()));
	}
	return utils::CreateString("{}");
}

int ShowGamepadTextInput(int eInputMode, int eLineInputMode, const char *description, int charMax, const char *existingText)
{
	CheckInitializedPlugin(false);
	if (charMax > MAX_GAMEPAD_TEXT_INPUT_LENGTH)
	{
		agk::Log("ShowGamepadTextInput: Maximum text length exceeds plugin limit.");
		charMax = MAX_GAMEPAD_TEXT_INPUT_LENGTH;
	}
	return SteamUtils()->ShowGamepadTextInput((EGamepadTextInputMode)eInputMode, (EGamepadTextInputLineMode)eLineInputMode, description, charMax, existingText);
}

// VR Methods
int IsSteamRunningInVR()
{
	CheckInitializedPlugin(false);
	return SteamUtils()->IsSteamRunningInVR();
}

void StartVRDashboard()
{
	CheckInitializedPlugin(NORETURN);
	SteamUtils()->StartVRDashboard();
}

void SetVRHeadsetStreamingEnabled(int enabled)
{
	CheckInitializedPlugin(NORETURN);
	SteamUtils()->SetVRHeadsetStreamingEnabled(enabled != 0);
}

int IsVRHeadsetStreamingEnabled()
{
	CheckInitializedPlugin(false);
	return SteamUtils()->IsVRHeadsetStreamingEnabled();
}

// Cloud methods: Information
int IsCloudEnabledForAccount()
{
	CheckInitializedPlugin(false);
	return SteamRemoteStorage()->IsCloudEnabledForAccount();
}

int IsCloudEnabledForApp()
{
	CheckInitializedPlugin(false);
	return SteamRemoteStorage()->IsCloudEnabledForApp();
}

void SetCloudEnabledForApp(int enabled)
{
	CheckInitializedPlugin(NORETURN);
	return SteamRemoteStorage()->SetCloudEnabledForApp(enabled != 0);
}

// Cloud methods: Files
int CloudFileDelete(const char *filename)
{
	CheckInitializedPlugin(false);
	return SteamRemoteStorage()->FileDelete(filename);
}

int CloudFileExists(const char *filename)
{
	CheckInitializedPlugin(false);
	return SteamRemoteStorage()->FileExists(filename);
}

int CloudFileForget(const char *filename)
{
	CheckInitializedPlugin(false);
	return SteamRemoteStorage()->FileForget(filename);
}

int CloudFilePersisted(const char *filename)
{
	CheckInitializedPlugin(false);
	return SteamRemoteStorage()->FilePersisted(filename);
}

int CloudFileRead(const char *filename)
{
	CheckInitializedPlugin(0);
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
	CheckInitializedPlugin(0);
	if (length == -1)
	{
		length = SteamRemoteStorage()->GetFileSize(filename);
	}
	return Steam->FileReadAsync(filename, offset, length);
}

char *GetCloudFileReadAsyncFileName(int hCallResult)
{
	CheckInitializedPlugin(NULL_STRING);
	return utils::CreateString(Steam->GetFileReadAsyncFileName(hCallResult));
}

int GetCloudFileReadAsyncMemblock(int hCallResult)
{
	CheckInitializedPlugin(0);
	return Steam->GetFileReadAsyncMemblock(hCallResult);
}

//SteamAPICall_t CloudFileShare(const char *filename);

int CloudFileWrite(const char *filename, int memblockID)
{
	CheckInitializedPlugin(false);
	if (agk::GetMemblockExists(memblockID))
	{
		return SteamRemoteStorage()->FileWrite(filename, agk::GetMemblockPtr(memblockID), agk::GetMemblockSize(memblockID));
	}
	return false;
}

int CloudFileWriteAsync(const char *filename, int memblockID)
{
	CheckInitializedPlugin(0);
	if (agk::GetMemblockExists(memblockID))
	{
		// The data buffer passed in to FileWriteAsync is immediately copied.  Safe to delete memblock immediately after call.
		return Steam->FileWriteAsync(filename, agk::GetMemblockPtr(memblockID), agk::GetMemblockSize(memblockID));
	}
	return false;
}

char *GetCloudFileWriteAsyncFileName(int hCallResult)
{
	CheckInitializedPlugin(NULL_STRING);
	return utils::CreateString(Steam->GetFileWriteAsyncFileName(hCallResult));
}

//bool CloudFileWriteStreamCancel(UGCFileWriteStreamHandle_t writeHandle);
//bool CloudFileWriteStreamClose(UGCFileWriteStreamHandle_t writeHandle);
//UGCFileWriteStreamHandle_t CloudFileWriteStreamOpen(const char *filename);
//bool CloudFileWriteStreamWriteChunk(UGCFileWriteStreamHandle_t writeHandle, const void *pvData, int32 cubData);

int GetCloudFileCount()
{
	CheckInitializedPlugin(0);
	return SteamRemoteStorage()->GetFileCount();
}

// Leave the by-index since this is pretty simple.
char *GetCloudFileName(int index)
{
	CheckInitializedPlugin(NULL_STRING);
	int32 pnFileSizeInBytes;
	return utils::CreateString(SteamRemoteStorage()->GetFileNameAndSize(index, &pnFileSizeInBytes));
}

char *GetCloudFileListJSON()
{
	std::string json("[");
	//if (Steam)
	//{
	//	for (int index = 0; index < SteamRemoteStorage()->GetFileCount(); index++)
	//	{
	//		if (index > 0)
	//		{
	//			json += ", ";
	//		}
	//		json += "{";
	//		int32 pnFileSizeInBytes;
	//		const char *name = SteamRemoteStorage()->GetFileNameAndSize(index, &pnFileSizeInBytes);
	//		if (name)
	//		{
	//			json += "\"Name\": \"" + EscapeJSON(name) + "\", "
	//				"\"Size\": " + std::to_string(pnFileSizeInBytes);
	//		}
	//		else
	//		{
	//			json += "\"Name\": \"?\", "
	//				"\"Size\": 0";
	//		}
	//		json += "}";
	//	}
	//}
	json += "]";
	return utils::CreateString(json);
}

int GetCloudFileSize(const char *filename)
{
	CheckInitializedPlugin(0);
	return SteamRemoteStorage()->GetFileSize(filename);
}

int GetCloudFileTimestamp(const char *filename)
{
	CheckInitializedPlugin(0);
	// NOTE: Dangerous conversion int64 to int.
	return (int)SteamRemoteStorage()->GetFileTimestamp(filename);
}

int GetCloudQuotaAvailable()
{
	CheckInitializedPlugin(0);
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
	CheckInitializedPlugin(0);
	uint64 pnTotalBytes;
	uint64 puAvailableBytes;
	if (SteamRemoteStorage()->GetQuota(&pnTotalBytes, &puAvailableBytes))
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
		if (SteamRemoteStorage()->GetQuota(&pnTotalBytes, &puAvailableBytes))
		{
			json << "\"Available\": " << puAvailableBytes << ", ";
			json << "\"Total\": " << pnTotalBytes;
		}
	}
	json << "}";
	return utils::CreateString(json);
}

int GetCloudFileSyncPlatforms(const char *filename)
{
	CheckInitializedPlugin(0);
	return SteamRemoteStorage()->GetSyncPlatforms(filename);
}

int SetCloudFileSyncPlatforms(const char *filename, int eRemoteStoragePlatform)
{
	CheckInitializedPlugin(false);
	return SteamRemoteStorage()->SetSyncPlatforms(filename, (ERemoteStoragePlatform) eRemoteStoragePlatform);
}

// User-Generated Content

int GetCachedUGCCount()
{
	CheckInitializedPlugin(0);
	return SteamRemoteStorage()->GetCachedUGCCount();
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
	CheckInitializedPlugin(false);
	if (SteamInput()->Init())
	{
		// Give the API some time to refresh the inputs.
		SteamInput()->RunFrame();
		return true;
	}
	return false;
}

int ShutdownSteamInput()
{
	CheckInitializedPlugin(0);
	return SteamInput()->Shutdown();
}

int GetConnectedControllers()
{
	CheckInitializedPlugin(0);
	m_InputCount = SteamInput()->GetConnectedControllers(m_InputHandles);
	return m_InputCount;
}

int GetInputTypeForHandle(int hInput)
{
	CheckInitializedPlugin(0);
	ValidateInputHandle(hInput, k_ESteamInputType_Unknown);
	return SteamInput()->GetInputTypeForHandle(m_InputHandles[hInput]);
}

void RunFrame()
{
	CheckInitializedPlugin(NORETURN);
	SteamInput()->RunFrame();
}

int ShowBindingPanel(int hInput)
{
	CheckInitializedPlugin(false);
	ValidateInputHandle(hInput, false);
	return SteamInput()->ShowBindingPanel(m_InputHandles[hInput]);
}

/*
Input Action Sets and Layers
*/

void ActivateActionSet(int hInput, int hActionSet)
{
	CheckInitializedPlugin(NORETURN);
	ValidateInputHandle(hInput, );
	ValidateActionHandle(InputActionSetHandle_t, actionSetHandle, hActionSet, m_ActionSetHandles, );
	SteamInput()->ActivateActionSet(m_InputHandles[hInput], actionSetHandle);
}

int GetActionSetHandle(const char *actionSetName)
{
	CheckInitializedPlugin(0);
	return GetActionHandleIndex(SteamInput()->GetActionSetHandle(actionSetName), &m_ActionSetHandles);
}

int GetCurrentActionSet(int hInput)
{
	CheckInitializedPlugin(0);
	ValidateInputHandle(hInput, false);
	return GetActionHandleIndex(SteamInput()->GetCurrentActionSet(m_InputHandles[hInput]), &m_ActionSetHandles);
}

void ActivateActionSetLayer(int hInput, int hActionSetLayer)
{
	CheckInitializedPlugin(NORETURN);
	ValidateInputHandle(hInput, );
	ValidateActionHandle(InputActionSetHandle_t, actionSetLayerHandle, hActionSetLayer, m_ActionSetHandles, );
	SteamInput()->ActivateActionSetLayer(m_InputHandles[hInput], actionSetLayerHandle);
}

void DeactivateActionSetLayer(int hInput, int hActionSetLayer)
{
	CheckInitializedPlugin(NORETURN);
	ValidateInputHandle(hInput, );
	ValidateActionHandle(InputActionSetHandle_t, actionSetLayerHandle, hActionSetLayer, m_ActionSetHandles, );
	SteamInput()->DeactivateActionSetLayer(m_InputHandles[hInput], actionSetLayerHandle);
}

void DeactivateAllActionSetLayers(int hInput)
{
	CheckInitializedPlugin(NORETURN);
	ValidateInputHandle(hInput, );
	SteamInput()->DeactivateAllActionSetLayers(m_InputHandles[hInput]);
}

char *GetActiveActionSetLayersJSON(int hInput)
{
	CheckInitializedPlugin(utils::CreateString("[]"));
	ValidateInputHandle(hInput, utils::CreateString("[]"));
	std::ostringstream json;
	json << "[";
	if (Steam)
	{
		// TODO The API document has incorrect information.  Is STEAM_INPUT_MAX_COUNT correct?
		InputActionSetHandle_t *handlesOut = new InputActionSetHandle_t[STEAM_INPUT_MAX_COUNT];
		int count = SteamInput()->GetActiveActionSetLayers(m_InputHandles[hInput], handlesOut);
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
	return utils::CreateString(json);
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
	return GetActionHandleIndex(SteamInput()->GetAnalogActionHandle(actionName), &m_AnalogActionHandles);
}

void StopAnalogActionMomentum(int hInput, int hAnalogAction)
{
	CheckInitializedPlugin(NORETURN);
	ValidateInputHandle(hInput, );
	ValidateActionHandle(InputAnalogActionHandle_t, analogActionHandle, hAnalogAction, m_AnalogActionHandles, );
	SteamInput()->StopAnalogActionMomentum(m_InputHandles[hInput], analogActionHandle);
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
	return GetActionHandleIndex(SteamInput()->GetDigitalActionHandle(actionName), &m_DigitalActionHandles);
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
	ValidateInputHandle(hInput, utils::CreateString("[]"));
	ValidateActionHandle(InputActionSetHandle_t, actionSetHandle, hActionSet, m_ActionSetHandles, utils::CreateString("[]"));
	ValidateActionHandle(InputAnalogActionHandle_t, analogActionHandle, hAnalogAction, m_AnalogActionHandles, utils::CreateString("[]"));
	std::ostringstream json;
	json << "[";
	if (Steam)
	{
		EInputActionOrigin *origins = new EInputActionOrigin[STEAM_INPUT_MAX_ORIGINS];
		int count = SteamInput()->GetAnalogActionOrigins(m_InputHandles[hInput], actionSetHandle, analogActionHandle, origins);
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
	return utils::CreateString(json);
}

char *GetDigitalActionOriginsJSON(int hInput, int hActionSet, int hDigitalAction)
{
	ValidateInputHandle(hInput, utils::CreateString("[]"));
	ValidateActionHandle(InputActionSetHandle_t, actionSetHandle, hActionSet, m_ActionSetHandles, utils::CreateString("[]"));
	ValidateActionHandle(InputDigitalActionHandle_t, digitalActionHandle, hDigitalAction, m_DigitalActionHandles, utils::CreateString("[]"));
	std::ostringstream json;
	json << "[";
	if (Steam)
	{
		EInputActionOrigin *origins = new EInputActionOrigin[STEAM_INPUT_MAX_ORIGINS];
		int count = SteamInput()->GetDigitalActionOrigins(m_InputHandles[hInput], actionSetHandle, digitalActionHandle, origins);
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
	return utils::CreateString(json);
}

char *GetGlyphForActionOrigin(int eOrigin)
{
	CheckInitializedPlugin(NULL_STRING);
	return utils::CreateString(SteamInput()->GetGlyphForActionOrigin((EInputActionOrigin)eOrigin));
}

char *GetStringForActionOrigin(int eOrigin)
{
	CheckInitializedPlugin(NULL_STRING);
	return utils::CreateString(SteamInput()->GetStringForActionOrigin((EInputActionOrigin)eOrigin));
}

int GetActionOriginFromXboxOrigin(int hInput, int eOrigin)
{
	ValidateInputHandle(hInput, k_EInputActionOrigin_None);
	CheckInitializedPlugin(k_EInputActionOrigin_None);
	return SteamInput()->GetActionOriginFromXboxOrigin(m_InputHandles[hInput], (EXboxOrigin)eOrigin);
}

char *GetStringForXboxOrigin(int eOrigin)
{
	CheckInitializedPlugin(NULL_STRING);
	return utils::CreateString(SteamInput()->GetStringForXboxOrigin((EXboxOrigin)eOrigin));
}

char *GetGlyphForXboxOrigin(int eOrigin)
{
	CheckInitializedPlugin(NULL_STRING);
	return utils::CreateString(SteamInput()->GetGlyphForXboxOrigin((EXboxOrigin)eOrigin));
}

int TranslateActionOrigin(int eDestinationInputType, int eSourceOrigin)
{
	CheckInitializedPlugin(k_EInputActionOrigin_None);
	return SteamInput()->TranslateActionOrigin((ESteamInputType)eDestinationInputType,(EInputActionOrigin)eSourceOrigin);
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
	SteamInput()->SetLEDColor(m_InputHandles[hInput], red, green, blue, k_ESteamInputLEDFlag_SetColor);
}

void ResetInputLEDColor(int hInput)
{
	CheckInitializedPlugin(NORETURN);
	ValidateInputHandle(hInput, );
	SteamInput()->SetLEDColor(m_InputHandles[hInput], 0, 0, 0, k_ESteamInputLEDFlag_RestoreUserDefault);
}

void TriggerInputHapticPulse(int hInput, int eTargetPad, int duration)
{
	CheckInitializedPlugin(NORETURN);
	ValidateInputHandle(hInput, );
	Clamp(duration, 0, USHRT_MAX);
	SteamInput()->TriggerHapticPulse(m_InputHandles[hInput], (ESteamControllerPad)eTargetPad, (unsigned short)duration);
}

void TriggerInputRepeatedHapticPulse(int hInput, int eTargetPad, int onDuration, int offDuration, int repeat)
{
	CheckInitializedPlugin(NORETURN);
	ValidateInputHandle(hInput, );
	Clamp(onDuration, 0, USHRT_MAX);
	Clamp(offDuration, 0, USHRT_MAX);
	Clamp(repeat, 0, USHRT_MAX);
	SteamInput()->TriggerRepeatedHapticPulse(m_InputHandles[hInput], (ESteamControllerPad)eTargetPad, (unsigned short)onDuration, (unsigned short)offDuration, (unsigned short)repeat, 0);
}

void TriggerInputVibration(int hInput, int leftSpeed, int rightSpeed)
{
	CheckInitializedPlugin(NORETURN);
	ValidateInputHandle(hInput, );
	Clamp(leftSpeed, 0, USHRT_MAX);
	Clamp(rightSpeed, 0, USHRT_MAX);
	SteamInput()->TriggerVibration(m_InputHandles[hInput], (unsigned short)leftSpeed, (unsigned short)rightSpeed);
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
