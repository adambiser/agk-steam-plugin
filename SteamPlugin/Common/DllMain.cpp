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
#include "AGKUtils.h"

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

std::string EscapeJSON(const std::string &input)
{
	std::ostringstream output;
	for (auto c = input.cbegin(); c != input.cend(); c++)
	{
		switch (*c) {
		case '"':
			output << "\\\"";
			break;
		case '\\':
			output << "\\\\";
			break;
		case '\b':
			output << "\\b";
			break;
		case '\f':
			output << "\\f";
			break;
		case '\n':
			output << "\\n";
			break;
		case '\r':
			output << "\\r";
			break;
		case '\t':
			output << "\\t";
			break;
		default:
			if ('\x00' <= *c && *c <= '\x1f')
			{
				char buffer[8];
				sprintf_s(buffer, "\\u%04x", *c);
				output << buffer;
			}
			else
			{
				output << *c;
			}
			break;
		}
	}
	return output.str();
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

//#define ConvertIPToString(ip) ((ip >> 24) & 0xff) << "." << ((ip >> 16) & 0xff) << "." << ((ip >> 8) & 0xff) << "." << ((ip >> 0) & 0xff)
#define ConvertIPToString(ip) std::to_string((ip >> 24) & 0xff) + "." + std::to_string((ip >> 16) & 0xff) + "." + std::to_string((ip >> 8) & 0xff) + "." + std::to_string((ip >> 0) & 0xff)

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
			return utils::CreateString(name);
		}
	}
	return utils::CreateString("NULL");
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
	Steam->RunCallbacks();
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
			json << "\"" << EscapeJSON(arg) << "\"";
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

char *GetCallResultJSON(int hCallResult)
{
	if (CCallResultItem *callResult = Steam->GetCallResultItem<CCallResultItem>(hCallResult))
	{
		return utils::CreateString(callResult->GetResultJSON());
	}
	return NULL_STRING;
};

// App/DLC methods
int GetDLCCount()
{
	CheckInitializedPlugin(0);
	return Steam->GetDLCCount();
}

char *GetDLCDataJSON()
{
	std::string json("[");
	if (Steam)
	{
		for (int index = 0; index < Steam->GetDLCCount(); index++)
		{
			if (index > 0)
			{
				json += ", ";
			}
			AppId_t appID;
			bool available;
			char name[128];
			bool success = Steam->GetDLCDataByIndex(index, &appID, &available, name, sizeof(name));
			json += "{";
			if (success)
			{
				json += "\"AppID\": " + std::to_string(appID) + ", "
					"\"Available\": " + std::to_string(available) + ", "
					"\"Name\": \"" + EscapeJSON(name) + "\"";
			}
			else
			{
				// Send an empty entry to indicate failure for this index.
				json += "\"AppID\": 0, "
					"\"Available\": 0, "
					"\"Name\": \"\"";
			}
			json += "}";
		}
	}
	json += "]";
	return utils::CreateString(json);
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
	return GetPluginHandle(Steam->GetAppOwner());
}

char *GetAvailableGameLanguages()
{
	CheckInitializedPlugin(NULL_STRING);
	return utils::CreateString(Steam->GetAvailableGameLanguages());
}

char *GetCurrentBetaName()
{
	CheckInitializedPlugin(NULL_STRING);
	char name[256];
	if (Steam->GetCurrentBetaName(name, sizeof(name)))
	{
		return utils::CreateString(name);
	}
	return NULL_STRING;
}

char *GetCurrentGameLanguage()
{
	CheckInitializedPlugin(NULL_STRING);
	return utils::CreateString(Steam->GetCurrentGameLanguage());
}

char *GetDLCDownloadProgressJSON(int appID)
{
	std::string json("{");
	if (Steam)
	{
		uint64 downloaded;
		uint64 total;
		bool downloading = Steam->GetDlcDownloadProgress(appID, &downloaded, &total);
		if (downloading)
		{
			json += "\"AppID\": " + std::to_string(appID) + ", "
				"\"BytesDownloaded\": " + std::to_string(downloaded) + ", "
				"\"BytesTotal\": " + std::to_string(total);
		}
	}
	json += "}";
	return utils::CreateString(json);
}

int GetEarliestPurchaseUnixTime(int appID)
{
	CheckInitializedPlugin(false);
	return Steam->GetEarliestPurchaseUnixTime(appID);
}

//SteamAPICall_t GetFileDetails(const char*pszFileName); // FileDetailsResult_t call result.

char *GetInstalledDepotsJSON(int appID, int maxDepots)
{
	std::string json("[");
	if (Steam)
	{
		DepotId_t *depots = new DepotId_t[maxDepots];
		int count = Steam->GetInstalledDepots(appID, depots, maxDepots);
		for (int x = 0; x < count; x++)
		{
			if (x > 0)
			{
				json += ", ";
			}
			json += std::to_string(depots[x]);
		}
		delete[] depots;
	}
	json += "]";
	return utils::CreateString(json);
}

char *GetLaunchQueryParam(const char *key)
{
	CheckInitializedPlugin(NULL_STRING);
	return utils::CreateString(Steam->GetLaunchQueryParam(key));
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
	Steam->ActivateGameOverlayInviteDialog(GetSteamHandle(hLobbySteamID));
}

void ActivateGameOverlayToStore(int appID, int flag)
{
	CheckInitializedPlugin(NORETURN);
	Steam->ActivateGameOverlayToStore(appID, (EOverlayToStoreFlag)flag);
}

void ActivateGameOverlayToUser(const char *dialogName, int hSteamID)
{
	CheckInitializedPlugin(NORETURN);
	Steam->ActivateGameOverlayToUser(dialogName, GetSteamHandle(hSteamID));
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
	return utils::CreateString(Steam->GetPersonaName());
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

int HasPersonaStateChanged()
{
	CheckInitializedPlugin(false);
	return Steam->HasPersonaStateChanged();
}

char *GetPersonaStateChangedJSON()
{
	CheckInitializedPlugin(0);
	PersonaStateChange_t value = Steam->GetPersonaStateChange();
	return utils::CreateString(std::string("{"
		"\"SteamID\": " + std::to_string(GetPluginHandle(value.m_ulSteamID)) + ","
		"\"ChangeFlags\": " + std::to_string(value.m_nChangeFlags) + "}"));
}

int RequestUserInformation(int hUserSteamID, int requireNameOnly)
{
	CheckInitializedPlugin(false);
	return Steam->RequestUserInformation(GetSteamHandle(hUserSteamID), requireNameOnly != 0);
}

int HasAvatarImageLoaded()
{
	CheckInitializedPlugin(false);
	return Steam->HasAvatarImageLoaded();
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
		for (int x = 0; x < Steam->GetFriendCount((EFriendFlags)friendFlags); x++)
		{
			if (x > 0)
			{
				json +=  ", ";
			}
			json += std::to_string(GetPluginHandle(Steam->GetFriendByIndex(x, (EFriendFlags)friendFlags)));
		}
	}
	json += "]";
	return utils::CreateString(json);
}

char *GetFriendGamePlayedJSON(int hUserSteamID)
{
	std::string json("{");
	if (Steam)
	{
		FriendGameInfo_t pFriendGameInfo;
		bool ingame = Steam->GetFriendGamePlayed(GetSteamHandle(hUserSteamID), &pFriendGameInfo);
		json += "\"InGame\": " + std::to_string(ingame);
		if (ingame)
		{
			json += ", "
				"\"GameAppID\": " + std::to_string(pFriendGameInfo.m_gameID.AppID()) + ", "
				"\"GameIP\": \"" + ConvertIPToString(pFriendGameInfo.m_unGameIP) + "\", "
				"\"GamePort\": " + std::to_string(pFriendGameInfo.m_usGamePort) + ", "
				"\"QueryPort\": " + std::to_string(pFriendGameInfo.m_usQueryPort) + ", "
				"\"SteamIDLobby\": " + std::to_string(GetPluginHandle(pFriendGameInfo.m_steamIDLobby));
		}
	}
	json += "}";
	return utils::CreateString(json);
}

char *GetFriendPersonaName(int hUserSteamID)
{
	CheckInitializedPlugin(NULL_STRING);
	return utils::CreateString(Steam->GetFriendPersonaName(GetSteamHandle(hUserSteamID)));
}

int GetFriendPersonaState(int hUserSteamID)
{
	CheckInitializedPlugin(-1);
	return Steam->GetFriendPersonaState(GetSteamHandle(hUserSteamID));
}

int GetFriendRelationship(int hUserSteamID)
{
	CheckInitializedPlugin(-1);
	return Steam->GetFriendRelationship(GetSteamHandle(hUserSteamID));
}

int GetFriendSteamLevel(int hUserSteamID)
{
	CheckInitializedPlugin(0);
	return Steam->GetFriendSteamLevel(GetSteamHandle(hUserSteamID));
}

char *GetPlayerNickname(int hUserSteamID)
{
	CheckInitializedPlugin(NULL_STRING);
	return utils::CreateString(Steam->GetPlayerNickname(GetSteamHandle(hUserSteamID)));
}

int HasFriend(int hUserSteamID, int friendFlags)
{
	CheckInitializedPlugin(false);
	return Steam->HasFriend(GetSteamHandle(hUserSteamID), (EFriendFlags)friendFlags);
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
	std::string json("[");
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
					json += ",";
				}
				json += std::to_string(GetPluginHandle(friends[x]));
			}
		}
	}
	json += "]";
	return utils::CreateString(json);
}

char *GetFriendsGroupName(int hFriendsGroupID)
{
	CheckInitializedPlugin(NULL_STRING);
	return utils::CreateString(Steam->GetFriendsGroupName(hFriendsGroupID));
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
	return utils::CreateString(Steam->GetAchievementAPIName(index));
}

char *GetAchievementDisplayName(const char *name)
{
	CheckInitializedPlugin(NULL_STRING);
	return utils::CreateString(Steam->GetAchievementDisplayAttribute(name, "name"));
}

char *GetAchievementDisplayDesc(const char *name)
{
	CheckInitializedPlugin(NULL_STRING);
	return utils::CreateString(Steam->GetAchievementDisplayAttribute(name, "desc"));
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

char *GetLeaderboardName(int hLeaderboard)
{
	CheckInitializedPlugin(NULL_STRING);
	return utils::CreateString(Steam->GetLeaderboardName(GetSteamHandle(hLeaderboard)));
}

int GetLeaderboardEntryCount(int hLeaderboard)
{
	CheckInitializedPlugin(0);
	return Steam->GetLeaderboardEntryCount(GetSteamHandle(hLeaderboard));
}

int GetLeaderboardDisplayType(int hLeaderboard)
{
	CheckInitializedPlugin(0);
	return Steam->GetLeaderboardDisplayType(GetSteamHandle(hLeaderboard));
}

int GetLeaderboardSortMethod(int hLeaderboard)
{
	CheckInitializedPlugin(0);
	return Steam->GetLeaderboardSortMethod(GetSteamHandle(hLeaderboard));
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
	return Steam->SetLobbyJoinable(GetSteamHandle(hLobbySteamID), lobbyJoinable != 0);
}

int SetLobbyType(int hLobbySteamID, int eLobbyType)
{
	CheckInitializedPlugin(false);
	return Steam->SetLobbyType(GetSteamHandle(hLobbySteamID), (ELobbyType)eLobbyType);
}

int JoinLobby(int hLobbySteamID)
{
	CheckInitializedPlugin(false);
	return Steam->JoinLobby(GetSteamHandle(hLobbySteamID));
}

int HasLobbyEnterResponse()
{
	CheckInitializedPlugin(false);
	return Steam->HasLobbyEnterResponse();
}

char *GetLobbyEnterResponseJSON()
{
	CheckInitializedPlugin(NULL_STRING);
	LobbyEnter_t lobbyEnter = Steam->GetLobbyEnterResponse();
	// Must be the same as in CLobbyEnterCallResult.
	return utils::CreateString(std::string("{"
		"\"SteamIDLobby\": " + std::to_string(GetPluginHandle(lobbyEnter.m_ulSteamIDLobby)) + ", "
		"\"Locked\": " + std::to_string(lobbyEnter.m_bLocked) + ", "
		"\"ChatRoomEnterResponse\": " + std::to_string(lobbyEnter.m_EChatRoomEnterResponse) + ", "
		"\"ChatPermissions\": " + std::to_string(lobbyEnter.m_rgfChatPermissions) + "}"));
}

int InviteUserToLobby(int hLobbySteamID, int hInviteeSteamID)
{
	CheckInitializedPlugin(false);
	return Steam->InviteUserToLobby(GetSteamHandle(hLobbySteamID), GetSteamHandle(hInviteeSteamID));
}

int HasGameLobbyJoinRequest()
{
	CheckInitializedPlugin(0);
	return Steam->HasGameLobbyJoinRequest();
}

int GetGameLobbyJoinRequestedLobby()
{
	CheckInitializedPlugin(0);
	return GetPluginHandle(Steam->GetGameLobbyJoinRequestedLobby());
}

void LeaveLobby(int hLobbySteamID)
{
	CheckInitializedPlugin(NORETURN);
	Steam->LeaveLobby(GetSteamHandle(hLobbySteamID));
}

// Lobby methods: Game server
char *GetLobbyGameServerJSON(int hLobbySteamID)
{
	std::string json("{");
	if (Steam)
	{
		uint32 unGameServerIP;
		uint16 unGameServerPort;
		CSteamID steamIDGameServer;
		if (Steam->GetLobbyGameServer(GetSteamHandle(hLobbySteamID), &unGameServerIP, &unGameServerPort, &steamIDGameServer))
		{
			json += "\"SteamIDLobby\": " + std::to_string(hLobbySteamID) + ", " // Add this to match LobbyGameCreated_t.
				"\"IP\": \"" + ConvertIPToString(unGameServerIP) +  "\", "
				"\"Port\": " + std::to_string(unGameServerPort) + ", "
				"\"SteamIDGameServer\": " + std::to_string(GetPluginHandle(steamIDGameServer));
		}
	}
	json += "}";
	return utils::CreateString(json);
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
	Steam->SetLobbyGameServer(GetSteamHandle(hLobbySteamID), ip, gameServerPort, GetSteamHandle(hGameServerSteamID));
	return true;
}

int HasLobbyGameCreated()
{
	CheckInitializedPlugin(false);
	return Steam->HasLobbyGameCreated();
}

char *GetLobbyGameCreatedJSON()
{
	std::string json("{");
	if (Steam)
	{
		LobbyGameCreated_t gameServer = Steam->GetLobbyGameCreated();
		//if (gameServer.m_ulSteamIDLobby != 0)
		//{
			json += "\"SteamIDLobby\": " + std::to_string(GetPluginHandle(gameServer.m_ulSteamIDLobby)) + ", "
				"\"SteamIDGameServer\": " + std::to_string(GetPluginHandle(gameServer.m_ulSteamIDGameServer)) + ", "
				"\"IP\": \"" + ConvertIPToString(gameServer.m_unIP) + "\", "
				"\"Port\": " + std::to_string(gameServer.m_usPort);
		//}
	}
	json += "}";
	return utils::CreateString(json);
}

// Lobby methods: Data
char *GetLobbyData(int hLobbySteamID, const char *key)
{
	CheckInitializedPlugin(NULL_STRING);
	return utils::CreateString(Steam->GetLobbyData(GetSteamHandle(hLobbySteamID), key));
}

char *GetLobbyDataJSON(int hLobbySteamID)
{
	CheckInitializedPlugin(NULL_STRING);
	CSteamID steamIDLobby = GetSteamHandle(hLobbySteamID);
	std::string json("[");
	for (int index = 0; index < Steam->GetLobbyDataCount(steamIDLobby); index++)
	{
		char key[k_nMaxLobbyKeyLength];
		char value[k_cubChatMetadataMax];
		if (Steam->GetLobbyDataByIndex(steamIDLobby, index, key, k_nMaxLobbyKeyLength, value, k_cubChatMetadataMax))
		{
			if (index > 0)
			{
				json += ", ";
			}
			json += "{"
				"\"Key\": \"" + EscapeJSON(key) + "\", "
				"\"Value\": \"" + EscapeJSON(value) + "\"}";
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
	Steam->SetLobbyData(GetSteamHandle(hLobbySteamID), key, value);
}

int DeleteLobbyData(int hLobbySteamID, const char *key)
{
	CheckInitializedPlugin(false);
	return Steam->DeleteLobbyData(GetSteamHandle(hLobbySteamID), key);
}

int RequestLobbyData(int hLobbySteamID)
{
	CheckInitializedPlugin(false);
	return Steam->RequestLobbyData(GetSteamHandle(hLobbySteamID));
}

int HasLobbyDataUpdateResponse()
{
	CheckInitializedPlugin(false);
	return Steam->HasLobbyDataUpdateResponse();
}

char *GetLobbyDataUpdateResponseJSON()
{
	CheckInitializedPlugin(NULL_STRING);
	LobbyDataUpdate_t update = Steam->GetLobbyDataUpdateResponse();
	// Must be the same as in CLobbyEnterCallResult.
	return utils::CreateString(std::string("{"
		"\"SteamIDLobby\": " + std::to_string(GetPluginHandle(update.m_ulSteamIDLobby)) + ", "
		"\"SteamIDMember\": " + std::to_string(GetPluginHandle(update.m_ulSteamIDMember)) + ", "
		"\"Success\": " + std::to_string(update.m_bSuccess) + "}"));
}

char *GetLobbyMemberData(int hLobbySteamID, int hUserSteamID, const char *key)
{
	CheckInitializedPlugin(NULL_STRING);
	return utils::CreateString(Steam->GetLobbyMemberData(GetSteamHandle(hLobbySteamID), GetSteamHandle(hUserSteamID), key));
}

void SetLobbyMemberData(int hLobbySteamID, const char *key, const char *value)
{
	CheckInitializedPlugin(NORETURN);
	return Steam->SetLobbyMemberData(GetSteamHandle(hLobbySteamID), key, value);
}

int GetLobbyOwner(int hLobbySteamID)
{
	CheckInitializedPlugin(0);
	return GetPluginHandle(Steam->GetLobbyOwner(GetSteamHandle(hLobbySteamID)));
}

int SetLobbyOwner(int hLobbySteamID, int hNewOwnerSteamID)
{
	CheckInitializedPlugin(0);
	return Steam->SetLobbyOwner(GetSteamHandle(hLobbySteamID), GetSteamHandle(hNewOwnerSteamID));
}

int GetLobbyMemberLimit(int hLobbySteamID)
{
	CheckInitializedPlugin(0);
	return Steam->GetLobbyMemberLimit(GetSteamHandle(hLobbySteamID));
}

int SetLobbyMemberLimit(int hLobbySteamID, int maxMembers)
{
	CheckInitializedPlugin(0);
	return Steam->SetLobbyMemberLimit(GetSteamHandle(hLobbySteamID), maxMembers);
}

int GetNumLobbyMembers(int hLobbySteamID)
{
	CheckInitializedPlugin(0);
	return Steam->GetNumLobbyMembers(GetSteamHandle(hLobbySteamID));
}

int GetLobbyMemberByIndex(int hLobbySteamID, int index)
{
	CheckInitializedPlugin(0);
	return GetPluginHandle(Steam->GetLobbyMemberByIndex(GetSteamHandle(hLobbySteamID), index));
}

int HasLobbyChatUpdate()
{
	CheckInitializedPlugin(false);
	return Steam->HasLobbyChatUpdate();
}

int GetLobbyChatUpdateLobby()
{
	CheckInitializedPlugin(0);
	return GetPluginHandle(Steam->GetLobbyChatUpdateLobby());
}

int GetLobbyChatUpdateUserChanged()
{
	CheckInitializedPlugin(0);
	return GetPluginHandle(Steam->GetLobbyChatUpdateUserChanged());
}

int GetLobbyChatUpdateUserState()
{
	CheckInitializedPlugin(0);
	return Steam->GetLobbyChatUpdateUserState();
}

int GetLobbyChatUpdateUserMakingChange()
{
	CheckInitializedPlugin(0);
	return GetPluginHandle(Steam->GetLobbyChatUpdateUserMakingChange());
}

int HasLobbyChatMessage()
{
	CheckInitializedPlugin(false);
	return Steam->HasLobbyChatMessage();
}

int GetLobbyChatMessageUser()
{
	CheckInitializedPlugin(0);
	return GetPluginHandle(Steam->GetLobbyChatMessageUser());
}

char *GetLobbyChatMessageText()
{
	CheckInitializedPlugin(NULL_STRING);
	char msg[4096];
	Steam->GetLobbyChatMessageText(msg);
	return utils::CreateString(msg);
}

int SendLobbyChatMessage(int hLobbySteamID, const char *msg)
{
	CheckInitializedPlugin(0);
	return Steam->SendLobbyChatMessage(GetSteamHandle(hLobbySteamID), msg);
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
	return Steam->AddFavoriteGame(appID, ip, connectPort, queryPort, flags, (uint32)now);
}

int GetFavoriteGameCount()
{
	CheckInitializedPlugin(0);
	return Steam->GetFavoriteGameCount();
}

// Leave this as by-index since AddFavoriteGame appears to return an index.
char *GetFavoriteGameJSON(int index)
{
	std::string json("{");
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
			json += "\"AppID\": " + std::to_string(nAppID) + ", "
				"\"IP\": \"" + ConvertIPToString(nIP) + "\", "
				"\"ConnPort\": " + std::to_string(nConnPort) + ", "
				"\"QueryPort\": " + std::to_string(nQueryPort) + ", "
				"\"Flags\": " + std::to_string(unFlags) + ", "
				"\"TimeLastPlayedOnServer\": " + std::to_string(rTime32LastPlayedOnServer);
		}
	}
	json += "}";
	return utils::CreateString(json);
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
//	return utils::CreateString(ipc);
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
	return utils::CreateString(Steam->GetIPCountry());
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
	return utils::CreateString(Steam->GetSteamUILanguage());
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
	std::string json("{");
	if (Steam)
	{
		if (Steam->HasGamepadTextInputDismissedInfo())
		{
			bool submitted;
			char text[MAX_GAMEPAD_TEXT_INPUT_LENGTH];
			Steam->GetGamepadTextInputDismissedInfo(&submitted, text);
			json += "\"Submitted\": " + std::to_string(submitted) + ", "
				"\"Text\": \"" + EscapeJSON(text) + "\"";
		}
	}
	json += "}";
	return utils::CreateString(json);
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
	CheckInitializedPlugin(0);
	if (length == -1)
	{
		length = Steam->GetFileSize(filename);
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
		return Steam->FileWrite(filename, agk::GetMemblockPtr(memblockID), agk::GetMemblockSize(memblockID));
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
	return Steam->GetFileCount();
}

// Leave the by-index since this is pretty simple.
char *GetCloudFileName(int index)
{
	CheckInitializedPlugin(NULL_STRING);
	int32 pnFileSizeInBytes;
	return utils::CreateString(Steam->GetFileNameAndSize(index, &pnFileSizeInBytes));
}

char *GetCloudFileListJSON()
{
	std::string json("[");
	if (Steam)
	{
		for (int index = 0; index < Steam->GetFileCount(); index++)
		{
			if (index > 0)
			{
				json += ", ";
			}
			json += "{";
			int32 pnFileSizeInBytes;
			const char *name = Steam->GetFileNameAndSize(index, &pnFileSizeInBytes);
			if (name)
			{
				json += "\"Name\": \"" + EscapeJSON(name) + "\", "
					"\"Size\": " + std::to_string(pnFileSizeInBytes);
			}
			else
			{
				json += "\"Name\": \"?\", "
					"\"Size\": 0";
			}
			json += "}";
		}
	}
	json += "]";
	return utils::CreateString(json);
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
	return utils::CreateString(json);
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
	CheckInitializedPlugin(utils::CreateString("[]"));
	ValidateInputHandle(hInput, utils::CreateString("[]"));
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
	ValidateInputHandle(hInput, utils::CreateString("[]"));
	ValidateActionHandle(InputActionSetHandle_t, actionSetHandle, hActionSet, m_ActionSetHandles, utils::CreateString("[]"));
	ValidateActionHandle(InputAnalogActionHandle_t, analogActionHandle, hAnalogAction, m_AnalogActionHandles, utils::CreateString("[]"));
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
	return utils::CreateString(json);
}

char *GetGlyphForActionOrigin(int eOrigin)
{
	CheckInitializedPlugin(NULL_STRING);
	return utils::CreateString(Steam->GetGlyphForActionOrigin((EInputActionOrigin)eOrigin));
}

char *GetStringForActionOrigin(int eOrigin)
{
	CheckInitializedPlugin(NULL_STRING);
	return utils::CreateString(Steam->GetStringForActionOrigin((EInputActionOrigin)eOrigin));
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
	return utils::CreateString(Steam->GetStringForXboxOrigin((EXboxOrigin)eOrigin));
}

char *GetGlyphForXboxOrigin(int eOrigin)
{
	CheckInitializedPlugin(NULL_STRING);
	return utils::CreateString(Steam->GetGlyphForXboxOrigin((EXboxOrigin)eOrigin));
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
