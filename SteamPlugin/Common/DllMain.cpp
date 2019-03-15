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

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <ctime>
#include <shellapi.h>
#include <vector>
#include <sstream>
#include <psapi.h>
#include "DllMain.h"
#include "..\AGKLibraryCommands.h"
#include "Utils.h"
#include "StructClear.h"
#include "CSteamHandleVector.h"
// CallResult classes.
#include "CCallbacks.h"
#include "CFileReadAsyncCallResult.h"
#include "CFileWriteAsyncCallResult.h"
#include "CLeaderboardFindCallResult.h"
#include "CLeaderboardScoresDownloadedCallResult.h"
#include "CLeaderboardScoreUploadedCallResult.h"
#include "CLobbyCreatedCallResult.h"
#include "CLobbyEnterCallResult.h"
#include "CLobbyMatchListCallResult.h"

/*
NOTE: Cannot use bool as an exported function return type because of AGK2 limitations.  Use int instead.
*/

#define MEMBLOCK_IMAGE_HEADER_LENGTH	12

// GLOBALS!
uint64 g_AppID = 0;
bool g_SteamInitialized = false;
//InputActionSetHandle_t g_hActionSetLayers[STEAM_INPUT_MAX_COUNT];
//int g_nActionSetLayersCount;
InputAnalogActionData_t g_AnalogActionData;
InputDigitalActionData_t g_DigitalActionData;
uint8 g_nMinutesBatteryLeft = 255;
std::mutex g_JoinedLobbiesMutex;
std::vector<CSteamID> g_JoinedLobbies; // Keep track so we don't leave any left open when closing.


/*
Check to see if the SteamPlugin has been initialized.
If it has not been, return a default value.
*/
#define CheckInitialized(returnValue)	\
	if (!g_SteamInitialized)			\
	{									\
		return returnValue;				\
	}

// Token for passing an empty returnValue into CheckInitialized();
#define NORETURN
#define NULL_STRING utils::CreateString((const char *)NULL)

#define Clamp(value, min, max)	\
	if (value < min)			\
	{							\
		value = min;			\
	}							\
	else if (value > max)		\
	{							\
		value = max;			\
	}

///*
//	CSteamID Handle Methods
//*/
//std::vector <uint64> m_SteamHandles;
//
//uint64 SteamHandles()->GetSteamHandle(int handle) // from plugin handle.
//{
//	// Handles are 1-based!
//	if (handle >= 0 && handle < (int)m_SteamHandles.size())
//	{
//		return m_SteamHandles[handle];
//	}
//	agk::PluginError("Invalid CSteamID handle.");
//	return 0;
//}
//
//int SteamHandles()->GetPluginHandle(uint64 handle)
//{
//	int index = (int)(std::find(m_SteamHandles.begin(), m_SteamHandles.end(), handle) - m_SteamHandles.begin());
//	if (index < (int)m_SteamHandles.size())
//	{
//		// Handles are 1-based!
//		return index;
//	}
//	m_SteamHandles.push_back(handle);
//	return (int)m_SteamHandles.size() - 1;
//}
//
//int SteamHandles()->GetPluginHandle(CSteamID steamID)
//{
//	return SteamHandles()->GetPluginHandle(steamID.ConvertToUint64());
//}
//
//void ClearSteamHandlesList()
//{
//	m_SteamHandles.clear();
//	// Handle 0 is always k_steamIDNil.
//	m_SteamHandles.push_back(k_steamIDNil.ConvertToUint64());
//}

/*
	InputHandle_t Handle Methods
*/
int g_InputCount = 0;
InputHandle_t g_InputHandles[STEAM_INPUT_MAX_COUNT];

// Handles are 1-based!
#define ValidateInputHandle(index, returnValue)		\
	index--;										\
	if (index < 0 || index >= g_InputCount)			\
	{												\
		agk::PluginError("Invalid input handle.");	\
		return returnValue;							\
	}

/*
Input Handles
*/
//std::vector <InputActionSetHandle_t> m_ActionSetHandles;
//std::vector <InputDigitalActionHandle_t> m_DigitalActionHandles;
//std::vector <InputAnalogActionHandle_t> m_AnalogActionHandles;

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

//#define ValidateActionHandle(handleType, handleVar, handleIndex, vector, returnValue)	\
//	if (handleIndex <= 0 || handleIndex >= (int)vector.size())	\
//	{															\
//		agk::PluginError("Invalid " #handleType " handle.");		\
//		return returnValue;										\
//	}															\
//	handleType handleVar = vector[handleIndex]
//
//template <typename T>
//inline int GetActionHandleIndex(T handle, std::vector<T> *vector)
//{
//	int index = (int)(std::find(vector->begin(), vector->end(), handle) - vector->begin());
//	if (index < (int)vector->size())
//	{
//		// Handles are 1-based!
//		return index;
//	}
//	vector->push_back(handle);
//	return (int)vector->size() - 1;
//}

//void ClearActionSetHandleList()
//{
//	m_ActionSetHandles.clear();
//	m_DigitalActionHandles.clear();
//	m_AnalogActionHandles.clear();
//	// Handle 0 is always 0.
//	m_ActionSetHandles.push_back(0);
//	m_DigitalActionHandles.push_back(0);
//	m_AnalogActionHandles.push_back(0);
//}
//
/*
	IP Address Parsing Methods
*/
//#define ConvertIPToString(ip) ((ip >> 24) & 0xff) << "." << ((ip >> 16) & 0xff) << "." << ((ip >> 8) & 0xff) << "." << ((ip >> 0) & 0xff)
void ResetVariables()
{
	// Disconnect from any lobbies.
	g_JoinedLobbiesMutex.lock();
	for (size_t index = 0; index < g_JoinedLobbies.size(); index++)
	{
		SteamMatchmaking()->LeaveLobby(g_JoinedLobbies[index]);
	}
	g_JoinedLobbies.clear();
	g_JoinedLobbiesMutex.unlock();
	Callbacks()->ResetSessionVariables();
	// Clear handles
	CallResults()->Clear();
	SteamHandles()->Clear();
	//ClearSteamHandlesList();
	//ClearActionSetHandleList();
	// Input
	Clear(g_AnalogActionData);
	Clear(g_DigitalActionData);
	g_InputCount = 0;
	// Variables
	g_AppID = 0;
	g_SteamInitialized = 0;
	g_JoinedLobbies.clear();
	g_SteamInitialized = false;
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
	if (CCallResultItem *callResult = CallResults()->Get<CCallResultItem>(hCallResult))
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
		agk::Log("Steam API already initialized.");
	}
	agk::Log("Initializing Steam API.");
	ResetVariables();
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

// S_API void S_CALLTYPE SteamAPI_ReleaseCurrentThreadMemory(); - not needed

void RunCallbacks()
{
	CheckInitialized(NORETURN);
	SteamAPI_RunCallbacks();
}

// TODO S_API void S_CALLTYPE SteamAPI_SetMiniDumpComment( const char *pchMsg );

void Shutdown()
{
	ResetVariables();
	SteamAPI_Shutdown();
	//delete Steam;
	//Steam = NULL;
}

// TODO S_API void S_CALLTYPE SteamAPI_WriteMiniDump( uint32 uStructuredExceptionCode, void* pvExceptionInfo, uint32 uBuildID );
#pragma endregion

#pragma region steam_gameserver.h
// TODO
#pragma endregion

#pragma region ISteamApps
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
		//length++; // NULL terminator
		//char *result = agk::utils::CreateString(length);
		//strcpy_s(result, length, folder);
		//return result;
		// TODO Verify this works as expected without using the returned length.
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

//SteamAPICall_t GetFileDetails(const char*pszFileName); // FileDetailsResult_t call result.

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

void InstallDLC(int appID) // Check HasDLCInstalled, GetDLCInstalled
{
	CheckInitialized(NORETURN);
	Callbacks()->EnableDlcInstalledCallback();
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
	if (SteamAppList()->GetAppName(appID, name, sizeof(name)) >= 0)
	{
		return utils::CreateString(name);
	}
	return utils::CreateString("Not found");
}
#pragma endregion

#pragma region ISteamAppTicket
// TODO?
// ISteamAppTicket does not expose a global accessor function. You must access it through ISteamClient::GetISteamGenericInterface.
// uint32 GetAppOwnershipTicketData( uint32 nAppID, void *pvBuffer, uint32 cbBufferLength, uint32 *piAppId, uint32 *piSteamId, uint32 *piSignature, uint32 *pcbSignature );
// See docs for example.
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

#pragma region ISteamController
// As of v1.43 - Use ISteamInput instead of ISteamController.
void ActivateActionSet(int hInput, int hActionSet)
{
	CheckInitialized(NORETURN);
	ValidateInputHandle(hInput, );
	SteamInput()->ActivateActionSet(g_InputHandles[hInput], SteamHandles()->GetSteamHandle(hActionSet));
}

void ActivateActionSetLayer(int hInput, int hActionSetLayer)
{
	CheckInitialized(NORETURN);
	ValidateInputHandle(hInput, );
	SteamInput()->ActivateActionSetLayer(g_InputHandles[hInput], SteamHandles()->GetSteamHandle(hActionSetLayer));
}

void DeactivateActionSetLayer(int hInput, int hActionSetLayer)
{
	CheckInitialized(NORETURN);
	ValidateInputHandle(hInput, );
	SteamInput()->DeactivateActionSetLayer(g_InputHandles[hInput], SteamHandles()->GetSteamHandle(hActionSetLayer));
}

void DeactivateAllActionSetLayers(int hInput)
{
	CheckInitialized(NORETURN);
	ValidateInputHandle(hInput, );
	SteamInput()->DeactivateAllActionSetLayers(g_InputHandles[hInput]);
}

int GetActiveActionSetLayerCount(int hInput)
{
	ValidateInputHandle(hInput, 0);
	InputActionSetHandle_t hActionSetLayers[STEAM_INPUT_MAX_COUNT];
	return SteamInput()->GetActiveActionSetLayers(g_InputHandles[hInput], hActionSetLayers);
}

int GetActiveActionSetLayerHandle(int hInput, int index)
{
	ValidateInputHandle(hInput, 0);
	InputActionSetHandle_t hActionSetLayers[STEAM_INPUT_MAX_COUNT];
	int count = SteamInput()->GetActiveActionSetLayers(g_InputHandles[hInput], hActionSetLayers);
	if (index >= 0 && index < count)
	{
		return SteamHandles()->GetPluginHandle(hActionSetLayers[index]);
	}
	return 0;
}

int GetActionSetHandle(const char *actionSetName)
{
	CheckInitialized(0);
	return SteamHandles()->GetPluginHandle(SteamInput()->GetActionSetHandle(actionSetName));
}

int GetAnalogActionDataActive(int hInput, int hAnalogAction)
{
	ValidateInputHandle(hInput, 0);
	return SteamInput()->GetAnalogActionData(g_InputHandles[hInput], SteamHandles()->GetSteamHandle(hAnalogAction)).bActive;
}

int GetAnalogActionDataMode(int hInput, int hAnalogAction)
{
	ValidateInputHandle(hInput, 0);
	return SteamInput()->GetAnalogActionData(g_InputHandles[hInput], SteamHandles()->GetSteamHandle(hAnalogAction)).eMode;
}

float GetAnalogActionDataX(int hInput, int hAnalogAction)
{
	ValidateInputHandle(hInput, 0);
	return SteamInput()->GetAnalogActionData(g_InputHandles[hInput], SteamHandles()->GetSteamHandle(hAnalogAction)).x;
}

float GetAnalogActionDataY(int hInput, int hAnalogAction)
{
	ValidateInputHandle(hInput, 0);
	return SteamInput()->GetAnalogActionData(g_InputHandles[hInput], SteamHandles()->GetSteamHandle(hAnalogAction)).y;
}

int GetAnalogActionHandle(const char *actionName)
{
	CheckInitialized(0);
	return SteamHandles()->GetPluginHandle(SteamInput()->GetAnalogActionHandle(actionName));
}

int GetAnalogActionOriginCount(int hInput, int hActionSet, int hAnalogAction)
{
	ValidateInputHandle(hInput, 0);
	InputActionSetHandle_t actionSetHandle = SteamHandles()->GetSteamHandle(hActionSet);
	InputAnalogActionHandle_t analogActionHandle = SteamHandles()->GetSteamHandle(hAnalogAction);
	EInputActionOrigin origins[STEAM_INPUT_MAX_ORIGINS];
	return SteamInput()->GetAnalogActionOrigins(g_InputHandles[hInput], actionSetHandle, analogActionHandle, origins);
}

int GetAnalogActionOriginValue(int hInput, int hActionSet, int hAnalogAction, int index)
{
	ValidateInputHandle(hInput, 0);
	InputActionSetHandle_t actionSetHandle = SteamHandles()->GetSteamHandle(hActionSet);
	InputAnalogActionHandle_t analogActionHandle = SteamHandles()->GetSteamHandle(hAnalogAction);
	EInputActionOrigin origins[STEAM_INPUT_MAX_ORIGINS];
	int count = SteamInput()->GetAnalogActionOrigins(g_InputHandles[hInput], actionSetHandle, analogActionHandle, origins);
	if (index >= 0 && index < count)
	{
		return origins[index];
	}
	return 0;
}

int GetConnectedControllers()
{
	CheckInitialized(0);
	g_InputCount = SteamInput()->GetConnectedControllers(g_InputHandles);
	return g_InputCount;
}

//GetControllerForGamepadIndex

int GetCurrentActionSet(int hInput)
{
	CheckInitialized(0);
	ValidateInputHandle(hInput, 0);
	return SteamHandles()->GetPluginHandle(SteamInput()->GetCurrentActionSet(g_InputHandles[hInput]));
}

int GetDigitalActionDataActive(int hInput, int hDigitalAction)
{
	ValidateInputHandle(hInput, 0);
	return SteamInput()->GetDigitalActionData(g_InputHandles[hInput], SteamHandles()->GetSteamHandle(hDigitalAction)).bActive;
}

int GetDigitalActionDataState(int hInput, int hDigitalAction)
{
	ValidateInputHandle(hInput, 0);
	return SteamInput()->GetDigitalActionData(g_InputHandles[hInput], SteamHandles()->GetSteamHandle(hDigitalAction)).bState;
}

int GetDigitalActionHandle(const char *actionName)
{
	CheckInitialized(0);
	return SteamHandles()->GetPluginHandle(SteamInput()->GetDigitalActionHandle(actionName));
}

int GetDigitalActionOriginCount(int hInput, int hActionSet, int hDigitalAction)
{
	ValidateInputHandle(hInput, 0);
	InputActionSetHandle_t actionSetHandle = SteamHandles()->GetSteamHandle(hActionSet);
	InputDigitalActionHandle_t digitalActionHandle = SteamHandles()->GetSteamHandle(hDigitalAction);
	EInputActionOrigin origins[STEAM_INPUT_MAX_ORIGINS];
	return SteamInput()->GetDigitalActionOrigins(g_InputHandles[hInput], actionSetHandle, digitalActionHandle, origins);
}

int GetDigitalActionOriginValue(int hInput, int hActionSet, int hDigitalAction, int index)
{
	ValidateInputHandle(hInput, 0);
	InputActionSetHandle_t actionSetHandle = SteamHandles()->GetSteamHandle(hActionSet);
	InputDigitalActionHandle_t digitalActionHandle = SteamHandles()->GetSteamHandle(hDigitalAction);
	EInputActionOrigin origins[STEAM_INPUT_MAX_ORIGINS];
	int count = SteamInput()->GetDigitalActionOrigins(g_InputHandles[hInput], actionSetHandle, digitalActionHandle, origins);
	if (index >= 0 && index < count)
	{
		return origins[index];
	}
	return 0;
}

//GetGamepadIndexForController

char *GetGlyphForActionOrigin(int eOrigin)
{
	CheckInitialized(NULL_STRING);
	return utils::CreateString(SteamInput()->GetGlyphForActionOrigin((EInputActionOrigin)eOrigin));
}

int GetInputTypeForHandle(int hInput)
{
	CheckInitialized(0);
	ValidateInputHandle(hInput, k_ESteamInputType_Unknown);
	return SteamInput()->GetInputTypeForHandle(g_InputHandles[hInput]);
}

float GetMotionDataPosAccelX(int hInput)
{
	ValidateInputHandle(hInput, 0);
	return SteamInput()->GetMotionData(g_InputHandles[hInput]).posAccelX;
}

float GetMotionDataPosAccelY(int hInput)
{
	ValidateInputHandle(hInput, 0);
	return SteamInput()->GetMotionData(g_InputHandles[hInput]).posAccelY;
}

float GetMotionDataPosAccelZ(int hInput)
{
	ValidateInputHandle(hInput, 0);
	return SteamInput()->GetMotionData(g_InputHandles[hInput]).posAccelZ;
}

float GetMotionDataRotQuatW(int hInput)
{
	ValidateInputHandle(hInput, 0);
	return SteamInput()->GetMotionData(g_InputHandles[hInput]).rotQuatW;
}

float GetMotionDataRotQuatX(int hInput)
{
	ValidateInputHandle(hInput, 0);
	return SteamInput()->GetMotionData(g_InputHandles[hInput]).rotQuatX;
}

float GetMotionDataRotQuatY(int hInput)
{
	ValidateInputHandle(hInput, 0);
	return SteamInput()->GetMotionData(g_InputHandles[hInput]).rotQuatY;
}

float GetMotionDataRotQuatZ(int hInput)
{
	ValidateInputHandle(hInput, 0);
	return SteamInput()->GetMotionData(g_InputHandles[hInput]).rotQuatZ;
}

float GetMotionDataRotVelX(int hInput)
{
	ValidateInputHandle(hInput, 0);
	return SteamInput()->GetMotionData(g_InputHandles[hInput]).rotVelX;
}

float GetMotionDataRotVelY(int hInput)
{
	ValidateInputHandle(hInput, 0);
	return SteamInput()->GetMotionData(g_InputHandles[hInput]).rotVelY;

}
float GetMotionDataRotVelZ(int hInput)
{
	ValidateInputHandle(hInput, 0);
	return SteamInput()->GetMotionData(g_InputHandles[hInput]).rotVelZ;
}

char *GetStringForActionOrigin(int eOrigin)
{
	CheckInitialized(NULL_STRING);
	return utils::CreateString(SteamInput()->GetStringForActionOrigin((EInputActionOrigin)eOrigin));
}

int InitSteamInput()
{
	CheckInitialized(false);
	if (SteamInput()->Init())
	{
		// Give the API some time to refresh the inputs.
		RunFrame();
		return true;
	}
	return false;
}

void RunFrame()
{
	SteamInput()->RunFrame();
}

void SetInputLEDColor(int hInput, int red, int green, int blue)
{
	CheckInitialized(NORETURN);
	ValidateInputHandle(hInput, );
	Clamp(red, 0, UINT8_MAX);
	Clamp(green, 0, UINT8_MAX);
	Clamp(blue, 0, UINT8_MAX);
	SteamInput()->SetLEDColor(g_InputHandles[hInput], red, green, blue, k_ESteamInputLEDFlag_SetColor);
}

void ResetInputLEDColor(int hInput)
{
	CheckInitialized(NORETURN);
	ValidateInputHandle(hInput, );
	SteamInput()->SetLEDColor(g_InputHandles[hInput], 0, 0, 0, k_ESteamInputLEDFlag_RestoreUserDefault);
}

//ShowAnalogActionOrigins - deprecated

int ShowBindingPanel(int hInput)
{
	CheckInitialized(false);
	ValidateInputHandle(hInput, false);
	return SteamInput()->ShowBindingPanel(g_InputHandles[hInput]);
}

//ShowDigitalActionOrigins - deprecated

int ShutdownSteamInput()
{
	CheckInitialized(0);
	return SteamInput()->Shutdown();
}

void StopAnalogActionMomentum(int hInput, int hAnalogAction)
{
	CheckInitialized(NORETURN);
	ValidateInputHandle(hInput, );
	InputAnalogActionHandle_t analogActionHandle = SteamHandles()->GetSteamHandle(hAnalogAction);
	//ValidateActionHandle(InputAnalogActionHandle_t, analogActionHandle, hAnalogAction, m_AnalogActionHandles, );
	SteamInput()->StopAnalogActionMomentum(g_InputHandles[hInput], analogActionHandle);
}

void TriggerInputHapticPulse(int hInput, int eTargetPad, int duration)
{
	CheckInitialized(NORETURN);
	ValidateInputHandle(hInput, );
	Clamp(duration, 0, USHRT_MAX);
	SteamInput()->TriggerHapticPulse(g_InputHandles[hInput], (ESteamControllerPad)eTargetPad, (unsigned short)duration);
}

void TriggerInputRepeatedHapticPulse(int hInput, int eTargetPad, int onDuration, int offDuration, int repeat)
{
	CheckInitialized(NORETURN);
	ValidateInputHandle(hInput, );
	Clamp(onDuration, 0, USHRT_MAX);
	Clamp(offDuration, 0, USHRT_MAX);
	Clamp(repeat, 0, USHRT_MAX);
	SteamInput()->TriggerRepeatedHapticPulse(g_InputHandles[hInput], (ESteamControllerPad)eTargetPad, (unsigned short)onDuration, (unsigned short)offDuration, (unsigned short)repeat, 0);
}

void TriggerInputVibration(int hInput, int leftSpeed, int rightSpeed)
{
	CheckInitialized(NORETURN);
	ValidateInputHandle(hInput, );
	Clamp(leftSpeed, 0, USHRT_MAX);
	Clamp(rightSpeed, 0, USHRT_MAX);
	SteamInput()->TriggerVibration(g_InputHandles[hInput], (unsigned short)leftSpeed, (unsigned short)rightSpeed);
}

// New for v1.43
int GetActionOriginFromXboxOrigin(int hInput, int eOrigin)
{
	ValidateInputHandle(hInput, k_EInputActionOrigin_None);
	CheckInitialized(k_EInputActionOrigin_None);
	return SteamInput()->GetActionOriginFromXboxOrigin(g_InputHandles[hInput], (EXboxOrigin)eOrigin);
}

char *GetStringForXboxOrigin(int eOrigin)
{
	CheckInitialized(NULL_STRING);
	return utils::CreateString(SteamInput()->GetStringForXboxOrigin((EXboxOrigin)eOrigin));
}

char *GetGlyphForXboxOrigin(int eOrigin)
{
	CheckInitialized(NULL_STRING);
	return utils::CreateString(SteamInput()->GetGlyphForXboxOrigin((EXboxOrigin)eOrigin));
}

int TranslateActionOrigin(int eDestinationInputType, int eSourceOrigin)
{
	CheckInitialized(k_EInputActionOrigin_None);
	return SteamInput()->TranslateActionOrigin((ESteamInputType)eDestinationInputType,(EInputActionOrigin)eSourceOrigin);
}
#pragma endregion

#pragma region ISteamFriends
void ActivateGameOverlay(const char *dialogName)
{
	CheckInitialized(NORETURN);
	SteamFriends()->ActivateGameOverlay(dialogName);
}

void ActivateGameOverlayInviteDialog(int hLobbySteamID)
{
	CheckInitialized(NORETURN);
	SteamFriends()->ActivateGameOverlayInviteDialog(SteamHandles()->GetSteamHandle(hLobbySteamID));
}

void ActivateGameOverlayToStore(int appID, int flag)
{
	CheckInitialized(NORETURN);
	SteamFriends()->ActivateGameOverlayToStore(appID, (EOverlayToStoreFlag)flag);
}

void ActivateGameOverlayToUser(const char *dialogName, int hSteamID)
{
	CheckInitialized(NORETURN);
	SteamFriends()->ActivateGameOverlayToUser(dialogName, SteamHandles()->GetSteamHandle(hSteamID));
}

void ActivateGameOverlayToWebPage(const char *url)
{
	CheckInitialized(NORETURN);
	SteamFriends()->ActivateGameOverlayToWebPage(url, k_EActivateGameOverlayToWebPageMode_Default);
}

// TODO Merge?
void ActivateGameOverlayToWebPageModal(const char *url)
{
	CheckInitialized(NORETURN);
	SteamFriends()->ActivateGameOverlayToWebPage(url, k_EActivateGameOverlayToWebPageMode_Modal);
}

//ClearRichPresence
//CloseClanChatWindowInSteam
//DownloadClanActivityCounts
//EnumerateFollowingList
//GetChatMemberByIndex
//GetClanActivityCounts
//GetClanByIndex
//GetClanChatMemberCount
//GetClanChatMessage
//GetClanCount
//GetClanName
//GetClanOfficerByIndex
//GetClanOfficerCount
//GetClanOwner
//GetClanTag
//GetCoplayFriend
//GetCoplayFriendCount
//GetFollowerCount
//GetFriendByIndex - see GetFriendListJSON
//GetFriendCoplayGame
//GetFriendCoplayTime

int GetFriendCount(int friendFlags)
{
	return SteamFriends()->GetFriendCount((EFriendFlags)friendFlags);
}

// This is an array of integers.  Speed is not important.  Returning JSON seems easiest.
char *GetFriendListJSON(int friendFlags)
{
	std::string json("[");
	for (int index = 0; index < SteamFriends()->GetFriendCount((EFriendFlags)friendFlags); index++)
	{
		if (index > 0)
		{
			json += ", ";
		}
		json += std::to_string(SteamHandles()->GetPluginHandle(SteamFriends()->GetFriendByIndex(index, (EFriendFlags)friendFlags)));
	}
	json += "]";
	return utils::CreateString(json);
}

//GetFriendCountFromSource
//GetFriendFromSourceByIndex
//GetFriendGamePlayed

int GetFriendGamePlayedGameAppID(int hUserSteamID)
{
	FriendGameInfo_t pFriendGameInfo;
	if (SteamFriends()->GetFriendGamePlayed(SteamHandles()->GetSteamHandle(hUserSteamID), &pFriendGameInfo))
	{
		return pFriendGameInfo.m_gameID.AppID();
	}
	return 0;
}

char *GetFriendGamePlayedIP(int hUserSteamID)
{
	FriendGameInfo_t pFriendGameInfo;
	if (SteamFriends()->GetFriendGamePlayed(SteamHandles()->GetSteamHandle(hUserSteamID), &pFriendGameInfo))
	{
		return utils::CreateString(utils::ToIPString(pFriendGameInfo.m_unGameIP));
	}	
	return NULL_STRING;
}

int GetFriendGamePlayedConnectionPort(int hUserSteamID)
{
	FriendGameInfo_t pFriendGameInfo;
	if (SteamFriends()->GetFriendGamePlayed(SteamHandles()->GetSteamHandle(hUserSteamID), &pFriendGameInfo))
	{
		return pFriendGameInfo.m_usGamePort;
	}
	return 0;
}

int GetFriendGamePlayedQueryPort(int hUserSteamID)
{
	FriendGameInfo_t pFriendGameInfo;
	if (SteamFriends()->GetFriendGamePlayed(SteamHandles()->GetSteamHandle(hUserSteamID), &pFriendGameInfo))
	{
		return pFriendGameInfo.m_usQueryPort;
	}
	return 0;
}

int GetFriendGamePlayedLobby(int hUserSteamID)
{
	FriendGameInfo_t pFriendGameInfo;
	if (SteamFriends()->GetFriendGamePlayed(SteamHandles()->GetSteamHandle(hUserSteamID), &pFriendGameInfo))
	{
		return SteamHandles()->GetPluginHandle(pFriendGameInfo.m_steamIDLobby);
	}
	return 0;
}

//GetFriendMessage

char *GetFriendPersonaName(int hUserSteamID)
{
	CheckInitialized(NULL_STRING);
	// TODO m_PersonaStateChangedEnabled = true;
	return utils::CreateString(SteamFriends()->GetFriendPersonaName(SteamHandles()->GetSteamHandle(hUserSteamID)));
}

//GetFriendPersonaNameHistory

int GetFriendPersonaState(int hUserSteamID)
{
	CheckInitialized(-1);
	// TODO m_PersonaStateChangedEnabled = true;
	return SteamFriends()->GetFriendPersonaState(SteamHandles()->GetSteamHandle(hUserSteamID));
}

int GetFriendRelationship(int hUserSteamID)
{
	CheckInitialized(-1);
	return SteamFriends()->GetFriendRelationship(SteamHandles()->GetSteamHandle(hUserSteamID));
}

//GetFriendRichPresence
//GetFriendRichPresenceKeyByIndex
//GetFriendRichPresenceKeyCount

int GetFriendsGroupCount()
{
	CheckInitialized(0);
	return SteamFriends()->GetFriendsGroupCount();
}

int GetFriendsGroupIDByIndex(int index)
{
	CheckInitialized(k_FriendsGroupID_Invalid);
	return SteamFriends()->GetFriendsGroupIDByIndex(index);
}

int GetFriendsGroupMembersCount(int hFriendsGroupID)
{
	CheckInitialized(0);
	return SteamFriends()->GetFriendsGroupMembersCount(hFriendsGroupID);
}

// This is an array of integers.  Speed is not important.  Returning JSON seems easiest.
char *GetFriendsGroupMembersListJSON(int hFriendsGroupID)
{
	std::string json("[");
	if (IsSteamInitialized())
	{
		int count = SteamFriends()->GetFriendsGroupMembersCount(hFriendsGroupID);
		if (count > 0)
		{
			CSteamID *pSteamIDMembers = new CSteamID[count];
			SteamFriends()->GetFriendsGroupMembersList(hFriendsGroupID, pSteamIDMembers, count);
			for (int index = 0; index < count; index++)
			{
				if (index > 0)
				{
					json += ", ";
				}
				json += std::to_string(SteamHandles()->GetPluginHandle(pSteamIDMembers[index]));
			}
			delete[] pSteamIDMembers;
		}
	}
	json += "]";
	return utils::CreateString(json);
}

char *GetFriendsGroupName(int hFriendsGroupID)
{
	CheckInitialized(NULL_STRING);
	return utils::CreateString(SteamFriends()->GetFriendsGroupName(hFriendsGroupID));
}

/*
NOTE: Steam docs say
"If the Steam level is not immediately available for the specified user then this returns 0 and queues it to be downloaded from the Steam servers.
When it gets downloaded a PersonaStateChange_t callback will be posted with m_nChangeFlags including k_EPersonaChangeSteamLevel."

HOWEVER, this doesn't actually appear to be the case.  GetFriendSteamLevel returns 0, but the callback never seems to fire.
Possible solution is to just keep requesting the level when 0 is returned.
*/
int GetFriendSteamLevel(int hUserSteamID)
{
	CheckInitialized(0);
	// TODO m_PersonaStateChangedEnabled = true;
	return SteamFriends()->GetFriendSteamLevel(SteamHandles()->GetSteamHandle(hUserSteamID));
}

// NOTE: The Steam API appears to implicitly call RequestUserInformation when needed.
int GetFriendAvatar(int hUserSteamID, int size)
{
	CheckInitialized(0);
	CSteamID steamID = SteamHandles()->GetSteamHandle(hUserSteamID);
	Callbacks()->EnableAvatarImageLoadedCallback();
	//SteamFriends()->RequestUserInformation(steamID, false);
	int hImage = 0;
	switch ((EAvatarSize)size)
	{
	case Small:
		hImage = SteamFriends()->GetSmallFriendAvatar(steamID);
		break;
	case Medium:
		hImage = SteamFriends()->GetMediumFriendAvatar(steamID);
		break;
	case Large:
		hImage = SteamFriends()->GetLargeFriendAvatar(steamID);
		break;
	default:
		agk::PluginError("Invalid avatar size.");
		return 0;
	}
	return hImage;
}

char *GetPersonaName()
{
	CheckInitialized(NULL_STRING);
	return utils::CreateString(SteamFriends()->GetPersonaName());
}

//GetPersonaState

char *GetPlayerNickname(int hUserSteamID)
{
	CheckInitialized(NULL_STRING);
	return utils::CreateString(SteamFriends()->GetPlayerNickname(SteamHandles()->GetSteamHandle(hUserSteamID)));
}

//GetUserRestrictions

int HasFriend(int hUserSteamID, int friendFlags)
{
	CheckInitialized(false);
	return SteamFriends()->HasFriend(SteamHandles()->GetSteamHandle(hUserSteamID), (EFriendFlags)friendFlags);
}

//InviteUserToGame
//IsClanChatAdmin
//IsClanPublic
//IsClanOfficialGameGroup
//IsClanChatWindowOpenInSteam
//IsFollowing
//IsUserInSource
//JoinClanChatRoom
//LeaveClanChatRoom
//OpenClanChatWindowInSteam
//ReplyToFriendMessage
//RequestClanOfficerList
//RequestFriendRichPresence

int RequestUserInformation(int hUserSteamID, int requireNameOnly)
{
	CheckInitialized(false);
	return SteamFriends()->RequestUserInformation(SteamHandles()->GetSteamHandle(hUserSteamID), requireNameOnly != 0);
}

//SendClanChatMessage
//SetInGameVoiceSpeaking
//SetListenForFriendsMessages
//SetPersonaName
//SetPlayedWith
//SetRichPresence

//Callbacks
int HasAvatarImageLoadedResponse()
{
	CheckInitialized(false);
	return Callbacks()->HasAvatarImageLoadedResponse();
}

int GetAvatarImageLoadedUser()
{
	CheckInitialized(0);
	return SteamHandles()->GetPluginHandle(Callbacks()->GetAvatarImageLoaded());
}

//ClanOfficerListResponse_t
//DownloadClanActivityCountsResult_t
//FriendRichPresenceUpdate_t
//FriendsEnumerateFollowingList_t
//FriendsGetFollowerCount_t
//FriendsIsFollowing_t
//GameConnectedChatJoin_t
//GameConnectedChatLeave_t
//GameConnectedClanChatMsg_t
//GameConnectedFriendChatMsg_t
//GameLobbyJoinRequested_t

int IsGameOverlayActive()
{
	CheckInitialized(false);
	return Callbacks()->IsGameOverlayActive();
}

//GameRichPresenceJoinRequested_t
//GameServerChangeRequested_t
//JoinClanChatRoomCompletionResult_t

int HasPersonaStateChangeResponse()
{
	CheckInitialized(false);
	return Callbacks()->HasPersonaStateChangeResponse();
}

int GetPersonaStateChangeFlags()
{
	CheckInitialized(0);
	return Callbacks()->GetPersonaStateChange().m_nChangeFlags;
}

int GetPersonaStateChangeSteamID()
{
	CheckInitialized(0);
	return SteamHandles()->GetPluginHandle(Callbacks()->GetPersonaStateChange().m_ulSteamID);
}

//SetPersonaNameResponse_t
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

#pragma region ISteamMatchmaking
int AddFavoriteGame(int appID, const char *ipv4, int connectPort, int queryPort, int flags) //, int time32LastPlayedOnServer)
{
	CheckInitialized(0);
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

//AddRequestLobbyListCompatibleMembersFilter

void AddRequestLobbyListDistanceFilter(int eLobbyDistanceFilter)
{
	CheckInitialized(NORETURN);
	return SteamMatchmaking()->AddRequestLobbyListDistanceFilter((ELobbyDistanceFilter)eLobbyDistanceFilter);
}

void AddRequestLobbyListFilterSlotsAvailable(int slotsAvailable)
{
	CheckInitialized(NORETURN);
	return SteamMatchmaking()->AddRequestLobbyListFilterSlotsAvailable(slotsAvailable);
}

void AddRequestLobbyListNearValueFilter(const char *keyToMatch, int valueToBeCloseTo)
{
	CheckInitialized(NORETURN);
	return SteamMatchmaking()->AddRequestLobbyListNearValueFilter(keyToMatch, valueToBeCloseTo);
}

void AddRequestLobbyListNumericalFilter(const char *keyToMatch, int valueToMatch, int eComparisonType)
{
	CheckInitialized(NORETURN);
	return SteamMatchmaking()->AddRequestLobbyListNumericalFilter(keyToMatch, valueToMatch, (ELobbyComparison)eComparisonType);
}

void AddRequestLobbyListResultCountFilter(int maxResults)
{
	CheckInitialized(NORETURN);
	return SteamMatchmaking()->AddRequestLobbyListResultCountFilter(maxResults);
}

void AddRequestLobbyListStringFilter(const char *keyToMatch, const char *valueToMatch, int eComparisonType)
{
	CheckInitialized(NORETURN);
	return SteamMatchmaking()->AddRequestLobbyListStringFilter(keyToMatch, valueToMatch, (ELobbyComparison)eComparisonType);
}

//CheckForPSNGameBootInvite - deprecated

int CreateLobby(int eLobbyType, int maxMembers)
{
	CheckInitialized(0);
	return CallResults()->Add(new CLobbyCreatedCallResult((ELobbyType)eLobbyType, maxMembers));
}

int DeleteLobbyData(int hLobbySteamID, const char *key)
{
	CheckInitialized(false);
	return SteamMatchmaking()->DeleteLobbyData(SteamHandles()->GetSteamHandle(hLobbySteamID), key);
}

//GetFavoriteGame
// Leave this as by-index since AddFavoriteGame appears to return an index.
int GetFavoriteGameAppID(int index)
{
	AppId_t nAppID;
	uint32 nIP;
	uint16 nConnPort;
	uint16 nQueryPort;
	uint32 nFlags;
	uint32 rTime32LastPlayedOnServer;
	if (SteamMatchmaking()->GetFavoriteGame(index, &nAppID, &nIP, &nConnPort, &nQueryPort, &nFlags, &rTime32LastPlayedOnServer))
	{
		return nAppID;
	}
	return 0;
}

char *GetFavoriteGameIP(int index)
{
	AppId_t nAppID;
	uint32 nIP;
	uint16 nConnPort;
	uint16 nQueryPort;
	uint32 nFlags;
	uint32 rTime32LastPlayedOnServer;
	if (SteamMatchmaking()->GetFavoriteGame(index, &nAppID, &nIP, &nConnPort, &nQueryPort, &nFlags, &rTime32LastPlayedOnServer))
	{
		return utils::CreateString(utils::ToIPString(nIP));
	}
	return NULL_STRING;
}

int GetFavoriteGameConnectionPort(int index)
{
	AppId_t nAppID;
	uint32 nIP;
	uint16 nConnPort;
	uint16 nQueryPort;
	uint32 nFlags;
	uint32 rTime32LastPlayedOnServer;
	if (SteamMatchmaking()->GetFavoriteGame(index, &nAppID, &nIP, &nConnPort, &nQueryPort, &nFlags, &rTime32LastPlayedOnServer))
	{
		return nConnPort;
	}
	return 0;
}

int GetFavoriteGameQueryPort(int index)
{
	AppId_t nAppID;
	uint32 nIP;
	uint16 nConnPort;
	uint16 nQueryPort;
	uint32 nFlags;
	uint32 rTime32LastPlayedOnServer;
	if (SteamMatchmaking()->GetFavoriteGame(index, &nAppID, &nIP, &nConnPort, &nQueryPort, &nFlags, &rTime32LastPlayedOnServer))
	{
		return nQueryPort;
	}
	return 0;
}

int GetFavoriteGameFlags(int index)
{
	AppId_t nAppID;
	uint32 nIP;
	uint16 nConnPort;
	uint16 nQueryPort;
	uint32 nFlags;
	uint32 rTime32LastPlayedOnServer;
	if (SteamMatchmaking()->GetFavoriteGame(index, &nAppID, &nIP, &nConnPort, &nQueryPort, &nFlags, &rTime32LastPlayedOnServer))
	{
		return nFlags;
	}
	return 0;
}

int GetFavoriteGameUnixTimeLastPlayedOnServer(int index)
{
	AppId_t nAppID;
	uint32 nIP;
	uint16 nConnPort;
	uint16 nQueryPort;
	uint32 nFlags;
	uint32 rTime32LastPlayedOnServer;
	if (SteamMatchmaking()->GetFavoriteGame(index, &nAppID, &nIP, &nConnPort, &nQueryPort, &nFlags, &rTime32LastPlayedOnServer))
	{
		return nFlags;
	}
	return 0;
}

int GetFavoriteGameCount()
{
	CheckInitialized(0);
	return SteamMatchmaking()->GetFavoriteGameCount();
}

//GetLobbyByIndex - used in callback
//GetLobbyChatEntry - used in callback

char *GetLobbyData(int hLobbySteamID, const char *key)
{
	CheckInitialized(NULL_STRING);
	return utils::CreateString(SteamMatchmaking()->GetLobbyData(SteamHandles()->GetSteamHandle(hLobbySteamID), key));
}

//GetLobbyDataByIndex - see GetLobbyDataJSON
//GetLobbyDataCount - see GetLobbyDataJSON

// The only time GetLobbyDataCount and GetLobbyDataByIndex seem useful is to iterate all of the lobby data.
// Returning the Key/Value list as JSON seems nicer than having to use GetLobbyDataCount, GetLobbyDataByIndexKey, and GetLobbyDataByIndexValue.
char *GetLobbyDataJSON(int hLobbySteamID)
{
	CheckInitialized(NULL_STRING);
	CSteamID steamIDLobby = SteamHandles()->GetSteamHandle(hLobbySteamID);
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
			json += "{"
				"\"Key\": \"" + utils::EscapeJSON(key) + "\", "
				"\"Value\": \"" + utils::EscapeJSON(value) + "\""
				"}";
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

char *GetLobbyGameServerIP(int hLobbySteamID)
{
	uint32 nIP;
	uint16 nPort;
	CSteamID ulSteamID;
	if (SteamMatchmaking()->GetLobbyGameServer(SteamHandles()->GetSteamHandle(hLobbySteamID), &nIP, &nPort, &ulSteamID))
	{
		return utils::CreateString(utils::ToIPString(nIP));
	}
	return NULL_STRING;
}

int GetLobbyGameServerPort(int hLobbySteamID)
{
	uint32 nIP;
	uint16 nPort;
	CSteamID ulSteamID;
	if (SteamMatchmaking()->GetLobbyGameServer(SteamHandles()->GetSteamHandle(hLobbySteamID), &nIP, &nPort, &ulSteamID))
	{
		return nPort;
	}
	return 0;
}

int GetLobbyGameServerSteamID(int hLobbySteamID)
{
	uint32 nIP;
	uint16 nPort;
	CSteamID ulSteamID;
	if (SteamMatchmaking()->GetLobbyGameServer(SteamHandles()->GetSteamHandle(hLobbySteamID), &nIP, &nPort, &ulSteamID))
	{
		return SteamHandles()->GetPluginHandle(ulSteamID);
	}
	return 0;
}

int GetLobbyMemberByIndex(int hLobbySteamID, int index)
{
	CheckInitialized(0);
	return SteamHandles()->GetPluginHandle(SteamMatchmaking()->GetLobbyMemberByIndex(SteamHandles()->GetSteamHandle(hLobbySteamID), index));
}

char *GetLobbyMemberData(int hLobbySteamID, int hUserSteamID, const char *key)
{
	CheckInitialized(NULL_STRING);
	return utils::CreateString(SteamMatchmaking()->GetLobbyMemberData(SteamHandles()->GetSteamHandle(hLobbySteamID), SteamHandles()->GetSteamHandle(hUserSteamID), key));
}

int GetLobbyMemberLimit(int hLobbySteamID)
{
	CheckInitialized(0);
	return SteamMatchmaking()->GetLobbyMemberLimit(SteamHandles()->GetSteamHandle(hLobbySteamID));
}

int GetLobbyOwner(int hLobbySteamID)
{
	CheckInitialized(0);
	return SteamHandles()->GetPluginHandle(SteamMatchmaking()->GetLobbyOwner(SteamHandles()->GetSteamHandle(hLobbySteamID)));
}

int GetNumLobbyMembers(int hLobbySteamID)
{
	CheckInitialized(0);
	return SteamMatchmaking()->GetNumLobbyMembers(SteamHandles()->GetSteamHandle(hLobbySteamID));
}

int InviteUserToLobby(int hLobbySteamID, int hInviteeSteamID)
{
	CheckInitialized(false);
	return SteamMatchmaking()->InviteUserToLobby(SteamHandles()->GetSteamHandle(hLobbySteamID), SteamHandles()->GetSteamHandle(hInviteeSteamID));
}

int JoinLobby(int hLobbySteamID)
{
	CheckInitialized(false);
	return CallResults()->Add(new CLobbyEnterCallResult(SteamHandles()->GetSteamHandle(hLobbySteamID)));
}

void LeaveLobby(int hLobbySteamID)
{
	CheckInitialized(NORETURN);
	CSteamID steamIDLobby = SteamHandles()->GetSteamHandle(hLobbySteamID);
	g_JoinedLobbiesMutex.lock();
	SteamMatchmaking()->LeaveLobby(steamIDLobby);
	// Remove from list of joined lobbies.
	auto it = std::find(g_JoinedLobbies.begin(), g_JoinedLobbies.end(), steamIDLobby);
	if (it != g_JoinedLobbies.end())
	{
		g_JoinedLobbies.erase(it);
	}
	g_JoinedLobbiesMutex.unlock();
}

int RemoveFavoriteGame(int appID, const char *ipv4, int connectPort, int queryPort, int flags)
{
	CheckInitialized(0);
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

int RequestLobbyData(int hLobbySteamID)
{
	CheckInitialized(false);
	return SteamMatchmaking()->RequestLobbyData(SteamHandles()->GetSteamHandle(hLobbySteamID));
}

int RequestLobbyList()
{
	CheckInitialized(0);
	return CallResults()->Add(new CLobbyMatchListCallResult());
}

int SendLobbyChatMessage(int hLobbySteamID, const char *msg)
{
	CheckInitialized(0);
	return SteamMatchmaking()->SendLobbyChatMsg(SteamHandles()->GetSteamHandle(hLobbySteamID), msg, (int)strlen(msg) + 1);
}

//int SetLinkedLobby(int hLobbySteamID, int hLobbyDependentSteamID)
//{
//	CheckInitialized(false);
//	return Callbacks()->SetLinkedLobby(SteamHandles()->GetSteamHandle(hLobbySteamID), SteamHandles()->GetSteamHandle(hLobbyDependentSteamID));
//}

void SetLobbyData(int hLobbySteamID, const char *key, const char *value)
{
	CheckInitialized(NORETURN);
	SteamMatchmaking()->SetLobbyData(SteamHandles()->GetSteamHandle(hLobbySteamID), key, value);
}

int SetLobbyGameServer(int hLobbySteamID, const char *gameServerIP, int gameServerPort, int hGameServerSteamID)
{
	CheckInitialized(false);
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
	SteamMatchmaking()->SetLobbyGameServer(SteamHandles()->GetSteamHandle(hLobbySteamID), ip, gameServerPort, SteamHandles()->GetSteamHandle(hGameServerSteamID));
	return true;
}

int SetLobbyJoinable(int hLobbySteamID, int lobbyJoinable)
{
	CheckInitialized(false);
	return SteamMatchmaking()->SetLobbyJoinable(SteamHandles()->GetSteamHandle(hLobbySteamID), lobbyJoinable != 0);
}

void SetLobbyMemberData(int hLobbySteamID, const char *key, const char *value)
{
	CheckInitialized(NORETURN);
	return SteamMatchmaking()->SetLobbyMemberData(SteamHandles()->GetSteamHandle(hLobbySteamID), key, value);
}

int SetLobbyMemberLimit(int hLobbySteamID, int maxMembers)
{
	CheckInitialized(0);
	return SteamMatchmaking()->SetLobbyMemberLimit(SteamHandles()->GetSteamHandle(hLobbySteamID), maxMembers);
}

int SetLobbyOwner(int hLobbySteamID, int hNewOwnerSteamID)
{
	CheckInitialized(0);
	return SteamMatchmaking()->SetLobbyOwner(SteamHandles()->GetSteamHandle(hLobbySteamID), SteamHandles()->GetSteamHandle(hNewOwnerSteamID));
}

int SetLobbyType(int hLobbySteamID, int eLobbyType)
{
	CheckInitialized(false);
	return SteamMatchmaking()->SetLobbyType(SteamHandles()->GetSteamHandle(hLobbySteamID), (ELobbyType)eLobbyType);
}

// Callbacks
int HasLobbyChatMessageResponse()
{
	CheckInitialized(false);
	return Callbacks()->HasLobbyChatMessageResponse();
}

int GetLobbyChatMessageUser()
{
	CheckInitialized(0);
	return SteamHandles()->GetPluginHandle(Callbacks()->GetLobbyChatMessage().m_ulSteamIDUser);
}

char *GetLobbyChatMessageText()
{
	CheckInitialized(NULL_STRING);
	return utils::CreateString(Callbacks()->GetLobbyChatMessage().m_chChatEntry);
}

int HasLobbyChatUpdateResponse()
{
	CheckInitialized(false);
	return Callbacks()->HasLobbyChatUpdateResponse();
}

int GetLobbyChatUpdateLobby()
{
	CheckInitialized(0);
	return SteamHandles()->GetPluginHandle(Callbacks()->GetLobbyChatUpdate().m_ulSteamIDLobby);
}

int GetLobbyChatUpdateUserChanged()
{
	CheckInitialized(0);
	return SteamHandles()->GetPluginHandle(Callbacks()->GetLobbyChatUpdate().m_ulSteamIDUserChanged);
}

int GetLobbyChatUpdateUserState()
{
	CheckInitialized(0);
	return Callbacks()->GetLobbyChatUpdate().m_rgfChatMemberStateChange;
}

int GetLobbyChatUpdateUserMakingChange()
{
	CheckInitialized(0);
	return SteamHandles()->GetPluginHandle(Callbacks()->GetLobbyChatUpdate().m_ulSteamIDMakingChange);
}

int HasLobbyDataUpdateResponse()
{
	CheckInitialized(false);
	return Callbacks()->HasLobbyDataUpdateResponse();
}

int GetLobbyDataUpdateLobby()
{
	return SteamHandles()->GetPluginHandle(Callbacks()->GetLobbyDataUpdate().m_ulSteamIDLobby);
}

int GetLobbyDataUpdateMember()
{
	return SteamHandles()->GetPluginHandle(Callbacks()->GetLobbyDataUpdate().m_ulSteamIDMember);
}

int GetLobbyDataUpdateSuccess()
{
	return SteamHandles()->GetPluginHandle(Callbacks()->GetLobbyDataUpdate().m_bSuccess);
}

int HasLobbyEnterResponse()
{
	CheckInitialized(false);
	return Callbacks()->HasLobbyEnterResponse();
}

int GetLobbyEnterChatPermissions()
{
	return Callbacks()->GetLobbyEnter().m_rgfChatPermissions;
}

int GetLobbyEnterChatRoomEnterResponse()
{
	return Callbacks()->GetLobbyEnter().m_EChatRoomEnterResponse;
}

int GetLobbyEnterLobby()
{
	return SteamHandles()->GetPluginHandle(Callbacks()->GetLobbyEnter().m_ulSteamIDLobby);
}

int GetLobbyEnterLocked()
{
	return Callbacks()->GetLobbyEnter().m_bLocked;
}

int HasGameLobbyJoinRequestedResponse()
{
	return Callbacks()->HasGameLobbyJoinRequestedResponse();
}

int GetGameLobbyJoinRequestedLobby()
{
	return SteamHandles()->GetPluginHandle(Callbacks()->GetGameLobbyJoinRequested().m_steamIDLobby);
}

int HasLobbyGameCreatedResponse()
{
	return Callbacks()->HasLobbyGameCreatedResponse();
}

int GetLobbyGameCreatedGameServer()
{
	return SteamHandles()->GetPluginHandle(Callbacks()->GetLobbyGameCreated().m_ulSteamIDGameServer);
}

int GetLobbyGameCreatedLobby()
{
	return SteamHandles()->GetPluginHandle(Callbacks()->GetLobbyGameCreated().m_ulSteamIDLobby);
}

char *GetLobbyGameCreatedIP()
{
	return utils::CreateString(utils::ToIPString(Callbacks()->GetLobbyGameCreated().m_unIP));
}

int GetLobbyGameCreatedPort()
{
	return Callbacks()->GetLobbyGameCreated().m_usPort;
}
#pragma endregion

#pragma region ISteamMatchmakingServers
// No callbacks.
#pragma endregion

#pragma region ISteamMusic
int IsMusicEnabled()
{
	CheckInitialized(0);
	return SteamMusic()->BIsEnabled();
}

int IsMusicPlaying()
{
	CheckInitialized(0);
	return SteamMusic()->BIsPlaying();
}

int GetMusicPlaybackStatus()
{
	CheckInitialized(AudioPlayback_Undefined);
	return SteamMusic()->GetPlaybackStatus();
}

float GetMusicVolume()
{
	CheckInitialized(0.0);
	return SteamMusic()->GetVolume();
}

void PauseMusic()
{
	CheckInitialized(NORETURN);
	SteamMusic()->Pause();
}

void PlayMusic()
{
	CheckInitialized(NORETURN);
	SteamMusic()->Play();
}

void PlayNextSong()
{
	CheckInitialized(NORETURN);
	SteamMusic()->PlayNext();
}

void PlayPreviousSong()
{
	CheckInitialized(NORETURN);
	SteamMusic()->PlayPrevious();
}

void SetMusicVolume(float volume)
{
	CheckInitialized(NORETURN);
	SteamMusic()->SetVolume(volume);
}

//Callbacks
int HasMusicPlaybackStatusChangedResponse()
{
	CheckInitialized(0);
	return Callbacks()->HasPlaybackStatusHasChangedResponse();
}

int HasMusicVolumeChangedResponse()
{
	CheckInitialized(0);
	return Callbacks()->HasVolumeHasChangedResponse();
}
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

#pragma region ISteamRemoteStorage
//CommitPublishedFileUpdate
//CreatePublishedFileUpdateRequest
//DeletePublishedFile
//EnumeratePublishedFilesByUserAction
//EnumeratePublishedWorkshopFiles
//EnumerateUserPublishedFiles
//EnumerateUserSharedWorkshopFiles
//EnumerateUserSubscribedFiles

int CloudFileDelete(const char *filename)
{
	CheckInitialized(false);
	return SteamRemoteStorage()->FileDelete(filename);
}

int CloudFileExists(const char *filename)
{
	CheckInitialized(false);
	return SteamRemoteStorage()->FileExists(filename);
}

//FileFetch - deprecated

int CloudFileForget(const char *filename)
{
	CheckInitialized(false);
	return SteamRemoteStorage()->FileForget(filename);
}

//FilePersist - deprecated
int CloudFilePersisted(const char *filename)
{
	CheckInitialized(false);
	return SteamRemoteStorage()->FilePersisted(filename);
}

int CloudFileRead(const char *filename)
{
	CheckInitialized(0);
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
	CheckInitialized(0);
	if (length == -1)
	{
		length = SteamRemoteStorage()->GetFileSize(filename);
	}
	return CallResults()->Add(new CFileReadAsyncCallResult(filename, offset, length));
}

char *GetCloudFileReadAsyncFileName(int hCallResult)
{
	CheckInitialized(NULL_STRING);
	if (CFileReadAsyncCallResult *callResult = CallResults()->Get<CFileReadAsyncCallResult>(hCallResult))
	{
		return utils::CreateString(callResult->GetFileName());
	}
	return NULL_STRING;
}

int GetCloudFileReadAsyncMemblock(int hCallResult)
{
	CheckInitialized(0);
	if (CFileReadAsyncCallResult *callResult = CallResults()->Get<CFileReadAsyncCallResult>(hCallResult))
	{
		return callResult->GetMemblockID();
	}
	return 0;
}

//FileShare
int CloudFileWrite(const char *filename, int memblockID)
{
	CheckInitialized(false);
	if (agk::GetMemblockExists(memblockID))
	{
		return SteamRemoteStorage()->FileWrite(filename, agk::GetMemblockPtr(memblockID), agk::GetMemblockSize(memblockID));
	}
	return false;
}

int CloudFileWriteAsync(const char *filename, int memblockID)
{
	CheckInitialized(0);
	if (agk::GetMemblockExists(memblockID))
	{
		// The data buffer passed in to FileWriteAsync is immediately copied.  Safe to delete memblock immediately after call.
		return CallResults()->Add(new CFileWriteAsyncCallResult(filename, agk::GetMemblockPtr(memblockID), agk::GetMemblockSize(memblockID)));
	}
	return 0;
}

char *GetCloudFileWriteAsyncFileName(int hCallResult)
{
	CheckInitialized(NULL_STRING);
	if (CFileWriteAsyncCallResult *callResult = CallResults()->Get<CFileWriteAsyncCallResult>(hCallResult))
	{
		return utils::CreateString(callResult->GetFileName());
	}
	return NULL_STRING;
}

//FileWriteStreamCancel
//FileWriteStreamClose
//FileWriteStreamOpen
//FileWriteStreamWriteChunk

int GetCachedUGCCount()
{
	CheckInitialized(0);
	return SteamRemoteStorage()->GetCachedUGCCount();
}

//GetCachedUGCHandle

int GetCloudFileCount()
{
	CheckInitialized(0);
	return SteamRemoteStorage()->GetFileCount();
}

//GetFileListFromServer

//GetFileNameAndSize
// Leave the by-index since this is pretty simple.
char *GetCloudFileName(int index)
{
	CheckInitialized(NULL_STRING);
	int32 pnFileSizeInBytes;
	return utils::CreateString(SteamRemoteStorage()->GetFileNameAndSize(index, &pnFileSizeInBytes));
}

char *GetCloudFileListJSON()
{
	std::string json("[");
	//if (IsSteamInitialized())
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
	CheckInitialized(0);
	return SteamRemoteStorage()->GetFileSize(filename);
}

int GetCloudFileTimestamp(const char *filename)
{
	CheckInitialized(0);
	// NOTE: Dangerous conversion int64 to int.
	return (int)SteamRemoteStorage()->GetFileTimestamp(filename);
}

//GetPublishedFileDetails
//GetPublishedItemVoteDetails

int GetCloudQuotaAvailable()
{
	CheckInitialized(0);
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
	CheckInitialized(0);
	uint64 pnTotalBytes;
	uint64 puAvailableBytes;
	if (SteamRemoteStorage()->GetQuota(&pnTotalBytes, &puAvailableBytes))
	{
		// Note: Potential issue converting uint64 to int if the quote gets to be more than 2GB.
		return (int)pnTotalBytes;
	}
	return 0;
}

int GetCloudFileSyncPlatforms(const char *filename)
{
	CheckInitialized(0);
	return SteamRemoteStorage()->GetSyncPlatforms(filename);
}

//GetUGCDetails
//GetUGCDownloadProgress
//GetUserPublishedItemVoteDetails

int IsCloudEnabledForAccount()
{
	CheckInitialized(false);
	return SteamRemoteStorage()->IsCloudEnabledForAccount();
}

int IsCloudEnabledForApp()
{
	CheckInitialized(false);
	return SteamRemoteStorage()->IsCloudEnabledForApp();
}

//PublishVideo
//PublishWorkshopFile
//ResetFileRequestState

void SetCloudEnabledForApp(int enabled)
{
	CheckInitialized(NORETURN);
	return SteamRemoteStorage()->SetCloudEnabledForApp(enabled != 0);
}

int SetCloudFileSyncPlatforms(const char *filename, int eRemoteStoragePlatform)
{
	CheckInitialized(false);
	return SteamRemoteStorage()->SetSyncPlatforms(filename, (ERemoteStoragePlatform)eRemoteStoragePlatform);
}

//SetUserPublishedFileAction
//SubscribePublishedFile
//SynchronizeToClient
//SynchronizeToServer
//UGCDownload
//UGCDownloadToLocation
//UGCRead
//UnsubscribePublishedFile
//UpdatePublishedFileDescription
//UpdatePublishedFileFile
//UpdatePublishedFilePreviewFile
//UpdatePublishedFileSetChangeDescription
//UpdatePublishedFileTags
//UpdatePublishedFileTitle
//UpdatePublishedFileVisibility
//UpdateUserPublishedItemVote
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

#pragma region ISteamUser
//AdvertiseGame
//BeginAuthSession
//BIsBehindNAT
//BIsPhoneIdentifying
//BIsPhoneRequiringVerification
//BIsPhoneVerified
//BIsTwoFactorEnabled

int LoggedOn()
{
	return SteamUser()->BLoggedOn();
}

//CancelAuthTicket
//DecompressVoice
//EndAuthSession
//GetAuthSessionTicket
//GetAvailableVoice
//GetEncryptedAppTicket
//GetGameBadgeLevel
//GetHSteamUser
//GetPlayerSteamLevel

int GetSteamID()
{
	CheckInitialized(0);
	return SteamHandles()->GetPluginHandle(SteamUser()->GetSteamID());
}

// Not in the API, but grouping this near GetSteamID().
int IsSteamIDValid(int hSteamID)
{
	CheckInitialized(false);
	CSteamID steamID = SteamHandles()->GetSteamHandle(hSteamID);
	return steamID.IsValid();
}

// Not in the API, but grouping this near GetSteamID().
int GetAccountID(int hUserSteamID)
{
	CheckInitialized(0);
	CSteamID steamID = SteamHandles()->GetSteamHandle(hUserSteamID);
	if (steamID != k_steamIDNil)
	{
		return steamID.GetAccountID();
	}
	return 0;
}

// Not in the API, but grouping this near GetSteamID().
char *GetSteamID3(int hUserSteamID)
{
	CheckInitialized(NULL_STRING);
	CSteamID steamID = SteamHandles()->GetSteamHandle(hUserSteamID);
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

// Not in the API, but grouping this near GetSteamID().
char *GetSteamID64(int hUserSteamID)
{
	CheckInitialized(NULL_STRING);
	char id64[21]; // Max value is 18,446,744,073,709,551,615
	_i64toa(SteamHandles()->GetSteamHandle(hUserSteamID), id64, 10);
	return utils::CreateString(id64);
}

// Not in the API, but grouping this near GetSteamID().
int GetHandleFromSteamID64(const char *steamID64)
{
	CheckInitialized(0);
	uint64 id = _atoi64(steamID64);
	return SteamHandles()->GetPluginHandle(CSteamID(id));
}

//GetUserDataFolder
//GetVoice
//GetVoiceOptimalSampleRate
//InitiateGameConnection
//RequestEncryptedAppTicket
//RequestStoreAuthURL
//StartVoiceRecording
//StopVoiceRecording
//TerminateGameConnection
//TrackAppUsageEvent
//UserHasLicenseForApp

//Callbacks
//ClientGameServerDeny_t
//EncryptedAppTicketResponse_t - Call result for RequestEncryptedAppTicket.
//GameWebCallback_t
//GetAuthSessionTicketResponse_t - Call result for GetAuthSessionTicket.
//IPCFailure_t
//LicensesUpdated_t
//MicroTxnAuthorizationResponse_t
//SteamServerConnectFailure_t
//SteamServersConnected_t
//SteamServersDisconnected_t
//StoreAuthURLResponse_t - Call result for RequestStoreAuthURL
//ValidateAuthTicketResponse_t - Call result for BeginAuthSession
#pragma endregion

#pragma region ISteamUserStats
//AttachLeaderboardUGC
int ClearAchievement(const char *name)
{
	CheckInitialized(false);
	return SteamUserStats()->ClearAchievement(name);
}

int DownloadLeaderboardEntries(int hLeaderboard, int eLeaderboardDataRequest, int rangeStart, int rangeEnd)
{
	CheckInitialized(0);
	SteamLeaderboard_t leaderboard = SteamHandles()->GetSteamHandle(hLeaderboard);
	if (!leaderboard)
	{
		agk::PluginError("DownloadLeaderboardEntries: Invalid leaderboard handle.");
		return 0;
	}
	return CallResults()->Add(new CLeaderboardScoresDownloadedCallResult(leaderboard, (ELeaderboardDataRequest)eLeaderboardDataRequest, rangeStart, rangeEnd));
}

//DownloadLeaderboardEntriesForUsers

int FindLeaderboard(const char *leaderboardName)
{
	CheckInitialized(false);
	return CallResults()->Add(new CLeaderboardFindCallResult(leaderboardName));
}

//FindOrCreateLeaderboard

int GetAchievement(const char *name)
{
	CheckInitialized(false);
	bool result = false;
	if (SteamUserStats()->GetAchievement(name, &result))
	{
		return result;
	}
	agk::PluginError("GetAchievement failed.");
	return false;
}

//GetAchievementAchievedPercent

int GetAchievementUnlockTime(const char *name)
{
	CheckInitialized(0);
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

char *GetAchievementDisplayName(const char *name)
{
	CheckInitialized(NULL_STRING);
	return utils::CreateString(SteamUserStats()->GetAchievementDisplayAttribute(name, "name"));
}

char *GetAchievementDisplayDesc(const char *name)
{
	CheckInitialized(NULL_STRING);
	return utils::CreateString(SteamUserStats()->GetAchievementDisplayAttribute(name, "desc"));
}

int GetAchievementDisplayHidden(const char *name)
{
	CheckInitialized(false);
	return (strcmp(SteamUserStats()->GetAchievementDisplayAttribute(name, "hidden"), "1") == 0);
}

int GetAchievementIcon(const char *name)
{
	CheckInitialized(0);
	return Callbacks()->GetAchievementIcon(name);
}

char *GetAchievementAPIName(int index)
{
	CheckInitialized(NULL_STRING);
	return utils::CreateString(SteamUserStats()->GetAchievementName(index));
}

//GetGlobalStat
//GetGlobalStatHistory

int GetLeaderboardDisplayType(int hLeaderboard)
{
	CheckInitialized(0);
	return SteamUserStats()->GetLeaderboardDisplayType(SteamHandles()->GetSteamHandle(hLeaderboard));
}

int GetLeaderboardEntryCount(int hLeaderboard)
{
	CheckInitialized(0);
	return SteamUserStats()->GetLeaderboardEntryCount(SteamHandles()->GetSteamHandle(hLeaderboard));
}

char *GetLeaderboardName(int hLeaderboard)
{
	CheckInitialized(NULL_STRING);
	return utils::CreateString(SteamUserStats()->GetLeaderboardName(SteamHandles()->GetSteamHandle(hLeaderboard)));
}

int GetLeaderboardSortMethod(int hLeaderboard)
{
	CheckInitialized(0);
	return SteamUserStats()->GetLeaderboardSortMethod(SteamHandles()->GetSteamHandle(hLeaderboard));
}

//GetMostAchievedAchievementInfo
//GetNextMostAchievedAchievementInfo

int GetNumAchievements()
{
	CheckInitialized(0);
	return SteamUserStats()->GetNumAchievements();
}

//GetNumberOfCurrentPlayers

int GetStatInt(const char *name)
{
	CheckInitialized(0);
	int result = 0;
	if (SteamUserStats()->GetStat(name, &result)) {
		return result;
	}
	agk::PluginError("GetStat failed.");
	return 0;
}

float GetStatFloat(const char *name)
{
	CheckInitialized(0.0);
	float result = 0.0;
	if (SteamUserStats()->GetStat(name, &result)) {
		return result;
	}
	agk::PluginError("GetStat failed.");
	return 0.0;
}

//GetTrophySpaceRequiredBeforeInstall - deprecated
//GetUserAchievement
//GetUserAchievementAndUnlockTime
//GetUserStat
//GetUserStatsData

int IndicateAchievementProgress(const char *name, int curProgress, int maxProgress)
{
	CheckInitialized(false);
	return SteamUserStats()->IndicateAchievementProgress(name, curProgress, maxProgress);
}

//InstallPS3Trophies

int RequestCurrentStats()
{
	CheckInitialized(false);
	Callbacks()->EnableUserStatsReceivedCallback();
	return SteamUserStats()->RequestCurrentStats();
}

//RequestGlobalAchievementPercentages
//RequestGlobalStats
//RequestUserStats

int ResetAllStats(int achievementsToo)
{
	CheckInitialized(false);
	return SteamUserStats()->ResetAllStats(achievementsToo != 0);
}

int SetAchievement(const char *name)
{
	CheckInitialized(false);
	return SteamUserStats()->SetAchievement(name);
}

int SetStatInt(const char *name, int value)
{
	CheckInitialized(false);
	return SteamUserStats()->SetStat(name, value);
}

int SetStatFloat(const char *name, float value)
{
	CheckInitialized(false);
	return SteamUserStats()->SetStat(name, value);
}

//SetUserStatsData

int StoreStats()
{
	CheckInitialized(false);
	return SteamUserStats()->StoreStats();
}

int UpdateAvgRateStat(const char *name, float countThisSession, float sessionLength)
{
	CheckInitialized(false);
	return SteamUserStats()->UpdateAvgRateStat(name, countThisSession, (double)sessionLength);
}

int UploadLeaderboardScore(int hLeaderboard, ELeaderboardUploadScoreMethod eLeaderboardUploadScoreMethod, int score)
{
	CheckInitialized(0);
	SteamLeaderboard_t leaderboard = SteamHandles()->GetSteamHandle(hLeaderboard);
	if (!leaderboard)
	{
		agk::PluginError("UploadLeaderboardScore: Invalid leaderboard handle.");
		return 0;
	}
	return CallResults()->Add(new CLeaderboardScoreUploadedCallResult(leaderboard, eLeaderboardUploadScoreMethod, score));
}

int UploadLeaderboardScore(int hLeaderboard, int score)
{
	return UploadLeaderboardScore(hLeaderboard, k_ELeaderboardUploadScoreMethodKeepBest, score);
}

int UploadLeaderboardScoreForceUpdate(int hLeaderboard, int score)
{
	return UploadLeaderboardScore(hLeaderboard, k_ELeaderboardUploadScoreMethodForceUpdate, score);
}

//Callbacks
//GlobalAchievementPercentagesReady_t - RequestGlobalAchievementPercentages
//GlobalStatsReceived_t - RequestGlobalStats
//LeaderboardFindResult_t - Call result for FindOrCreateLeaderboard, FindLeaderboard
//LeaderboardScoresDownloaded_t - Call result for DownloadLeaderboardEntries, DownloadLeaderboardEntriesForUsers
//LeaderboardScoreUploaded_t - Call result for UploadLeaderboardScore
//LeaderboardUGCSet_t - Call result for AttachLeaderboardUGC
//NumberOfCurrentPlayers_t - GetNumberOfCurrentPlayers
//PS3TrophiesInstalled_t - ignore

int HasUserAchievementStoredResponse()
{
	return Callbacks()->HasUserAchievementStoredResponse();
}

int HasUserStatsReceivedResponse()
{
	return Callbacks()->HasUserStatsReceivedResponse();
}

int GetUserStatsReceivedGameAppID()
{
	return ((CGameID)(int)Callbacks()->GetUserStatsReceived().m_nGameID).AppID();
}

int GetUserStatsReceivedResult()
{
	return (int)Callbacks()->GetUserStatsReceived().m_eResult;
}

int GetUserStatsReceivedUser()
{
	return SteamHandles()->GetPluginHandle(Callbacks()->GetUserStatsReceived().m_steamIDUser);
}

int StatsInitialized()
{
	return Callbacks()->StatsInitialized();
}

int HasUserStatsStoredResponse()
{
	return Callbacks()->HasUserStatsStoredResponse();
}
#pragma endregion

#pragma region ISteamUtils
//BOverlayNeedsPresent
//CheckFileSignature
//GetAPICallFailureReason
//GetAPICallResult

int GetAppID()
{
	return SteamUtils()->GetAppID();
}

//GetConnectedUniverse
//GetCSERIPPort

int GetCurrentBatteryPower()
{
	CheckInitialized(0);
	return SteamUtils()->GetCurrentBatteryPower();
}

//GetEnteredGamepadTextInput - used in callback
//GetEnteredGamepadTextLength - used in callback

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
	CheckInitialized(0);
	return LoadImageFromHandle(0, hImage);
}

void LoadImageIDFromHandle(int imageID, int hImage)
{
	CheckInitialized(NORETURN);
	LoadImageFromHandle(imageID, hImage);
}

int GetIPCCallCount()
{
	CheckInitialized(0);
	return SteamUtils()->GetIPCCallCount();
}

char *GetIPCountry()
{
	CheckInitialized(NULL_STRING);
	return utils::CreateString(SteamUtils()->GetIPCountry());
}

int GetSecondsSinceAppActive()
{
	CheckInitialized(0);
	return SteamUtils()->GetSecondsSinceAppActive();
}

int GetSecondsSinceComputerActive()
{
	CheckInitialized(0);
	return SteamUtils()->GetSecondsSinceComputerActive();
}

int GetServerRealTime()
{
	CheckInitialized(0);
	return SteamUtils()->GetServerRealTime();
}

char *GetSteamUILanguage()
{
	CheckInitialized(NULL_STRING);
	return utils::CreateString(SteamUtils()->GetSteamUILanguage());
}

//IsAPICallCompleted

int IsOverlayEnabled()
{
	CheckInitialized(0);
	return SteamUtils()->IsOverlayEnabled();
}

int IsSteamInBigPictureMode()
{
	CheckInitialized(0);
	return SteamUtils()->IsSteamInBigPictureMode();
}

int IsSteamRunningInVR()
{
	CheckInitialized(false);
	return SteamUtils()->IsSteamRunningInVR();
}

int IsVRHeadsetStreamingEnabled()
{
	CheckInitialized(false);
	return SteamUtils()->IsVRHeadsetStreamingEnabled();
}

void SetOverlayNotificationInset(int horizontalInset, int verticalInset)
{
	CheckInitialized(NORETURN);
	SteamUtils()->SetOverlayNotificationInset(horizontalInset, verticalInset);
}

void SetOverlayNotificationPosition(int eNotificationPosition)
{
	CheckInitialized(NORETURN);
	SteamUtils()->SetOverlayNotificationPosition((ENotificationPosition)eNotificationPosition);
}

void SetVRHeadsetStreamingEnabled(int enabled)
{
	CheckInitialized(NORETURN);
	SteamUtils()->SetVRHeadsetStreamingEnabled(enabled != 0);
}

extern "C" void __cdecl SteamAPIDebugTextHook(int nSeverity, const char *pchDebugText)
{
	switch (nSeverity)
	{
	case 1: // warning
		utils::Log(std::string("Warning: ") + pchDebugText);
		break;
	case 0: // message
	default: // undefined severity
		agk::Log(pchDebugText);
		break;
	}
}

void SetWarningMessageHook()
{
	CheckInitialized(NORETURN);
	SteamUtils()->SetWarningMessageHook(&SteamAPIDebugTextHook);
}

int ShowGamepadTextInput(int eInputMode, int eLineInputMode, const char *description, int charMax, const char *existingText)
{
	CheckInitialized(false);
	if (charMax > MAX_GAMEPAD_TEXT_INPUT_LENGTH)
	{
		agk::Log("ShowGamepadTextInput: Maximum text length exceeds plugin limit.");
		charMax = MAX_GAMEPAD_TEXT_INPUT_LENGTH;
	}
	return SteamUtils()->ShowGamepadTextInput((EGamepadTextInputMode)eInputMode, (EGamepadTextInputLineMode)eLineInputMode, description, charMax, existingText);
}

void StartVRDashboard()
{
	CheckInitialized(NORETURN);
	SteamUtils()->StartVRDashboard();
}

//Callbacks
//CheckFileSignature_t

int HasGamepadTextInputDismissedResponse()
{
	CheckInitialized(0);
	return Callbacks()->HasGamepadTextInputDismissedResponse();
}

// TODO
char *GetGamepadTextInputDismissedInfoJSON()
{
	if (IsSteamInitialized())
	{
		//return utils::CreateString(ToJSON(Callbacks()->GetGamepadTextInputDismissed()));
	}
	return utils::CreateString("{}");
}

int HasIPCountryChangedResponse()
{
	CheckInitialized(false);
	return Callbacks()->HasIPCountryChangedResponse();
}

int HasLowBatteryWarningResponse()
{
	CheckInitialized(false);
	return Callbacks()->HasLowBatteryPowerResponse();
}

int GetMinutesBatteryLeft()
{
	CheckInitialized(255);
	return Callbacks()->GetMinutesBatteryLeft();
}

//SteamAPICallCompleted_t

int IsSteamShuttingDown()
{
	CheckInitialized(false);
	return Callbacks()->HasSteamShutdownResponse();
}

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
