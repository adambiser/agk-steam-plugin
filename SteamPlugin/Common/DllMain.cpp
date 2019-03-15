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
// TODO clean this up!
uint64 g_AppID = 0;
bool g_SteamInitialized = false;
InputAnalogActionData_t g_AnalogActionData;
InputMotionData_t g_MotionData;
InputDigitalActionData_t g_DigitalActionData;
uint8 g_nMinutesBatteryLeft = 255;
std::mutex g_JoinedLobbiesMutex;
std::vector<CSteamID> g_JoinedLobbies; // Keep track so we don't leave any left open when closing.


/*
Check to see if the SteamPlugin has been initialized.
If it has not been, return a default value.
*/
#define CheckInitializedPlugin(returnValue)	\
	if (!g_SteamInitialized)				\
	{										\
		return returnValue;					\
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
	ClearSteamHandlesList();
	ClearActionSetHandleList();
	// Input
	Clear(g_AnalogActionData);
	Clear(g_DigitalActionData);
	Clear(g_MotionData);
	m_InputCount = 0;
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
//	CheckInitializedPlugin(NULL_STRING);
//	uint32 ip = Callbacks()->GetPublicIP();
//	std::ostringstream ipc;
//	ipc << ConvertIPToString(ip);
//	return utils::CreateString(ipc);
//}
#pragma endregion

#pragma region CallResult Methods
void DeleteCallResult(int hCallResult)
{
	CheckInitializedPlugin(NORETURN);
	CallResults()->Delete(hCallResult);
}

int GetCallResultCode(int hCallResult)
{
	CheckInitializedPlugin(0);
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
		RequestStats();
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
	CheckInitializedPlugin(NORETURN);
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
	return NULL_STRING;
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

int GetDLCCount()
{
	CheckInitializedPlugin(0);
	return SteamApps()->GetDLCCount();
}

//TODO no JSON
char *GetDLCDataJSON()
{
	std::string json("[");
	if (IsSteamInitialized())
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

// TODO Split into parts
char *GetDLCDownloadProgressJSON(int appID)
{
	if (IsSteamInitialized())
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

// TODO add by index.
char *GetInstalledDepotsJSON(int appID, int maxDepots)
{
	if (IsSteamInitialized())
	{
		//DepotId_t *depots = new DepotId_t[maxDepots];
		//int count = SteamApps()->GetInstalledDepots(appID, depots, maxDepots);
		//std::string json = ToJSON(depots, count);
		//delete[] depots;
		//return utils::CreateString(json);
	}
	return utils::CreateString("[]");
}

char *GetLaunchCommandLine()
{
	CheckInitializedPlugin(NULL_STRING);
	char cmd[2084];
	int length = SteamApps()->GetLaunchCommandLine(cmd, 2084);
	cmd[length] = 0;
	return utils::CreateString(cmd);
}

char *GetLaunchQueryParam(const char *key)
{
	CheckInitializedPlugin(NULL_STRING);
	return utils::CreateString(SteamApps()->GetLaunchQueryParam(key));
}

void InstallDLC(int appID) // Check HasNewDLCInstalled, GetNewDLCInstalled
{
	CheckInitializedPlugin(NORETURN);
	Callbacks()->EnableOnDlcInstalledCallback();
	SteamApps()->InstallDLC(appID);
}

int MarkContentCorrupt(int missingFilesOnly)
{
	CheckInitializedPlugin(false);
	return SteamApps()->MarkContentCorrupt(missingFilesOnly != 0);
}

// RequestAllProofOfPurchaseKeys - deprecated
// RequestAppProofOfPurchaseKey - deprecated

void UninstallDLC(int appID)
{
	CheckInitializedPlugin(NORETURN);
	SteamApps()->UninstallDLC(appID);
}

// Callbacks
int HasNewDLCInstalled()
{
	CheckInitializedPlugin(false);
	return Callbacks()->HasOnDlcInstalled();
}

int GetNewDLCInstalled()
{
	CheckInitializedPlugin(0);
	return Callbacks()->GetOnDlcInstalled().m_nAppID;
}

int HasNewUrlLaunchParameters()
{
	CheckInitializedPlugin(false);
	return Callbacks()->HasOnNewUrlLaunchParameters();
}
#pragma endregion

#pragma region ISteamAppsList
// Restricted interface.
char *GetAppName(int appID)
{
	char name[256];
	if (SteamAppList()->GetAppName(appID, name, sizeof(name)))
	{
		return utils::CreateString(name);
	}
	return utils::CreateString("NULL");
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
	CheckInitializedPlugin(NORETURN);
	ValidateInputHandle(hInput, );
	ValidateActionHandle(InputActionSetHandle_t, actionSetHandle, hActionSet, m_ActionSetHandles, );
	SteamInput()->ActivateActionSet(m_InputHandles[hInput], actionSetHandle);
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

//GetActiveActionSetLayers
char *GetActiveActionSetLayersJSON(int hInput)
{
	CheckInitializedPlugin(utils::CreateString("[]"));
	ValidateInputHandle(hInput, utils::CreateString("[]"));
	std::ostringstream json;
	json << "[";
	if (IsSteamInitialized())
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

int GetActionSetHandle(const char *actionSetName)
{
	CheckInitializedPlugin(0);
	return GetActionHandleIndex(SteamInput()->GetActionSetHandle(actionSetName), &m_ActionSetHandles);
}

void GetAnalogActionData(int hInput, int hAnalogAction)
{
	CheckInitializedPlugin(NORETURN);
	ValidateInputHandle(hInput, );
	ValidateActionHandle(InputAnalogActionHandle_t, analogActionHandle, hAnalogAction, m_AnalogActionHandles, );
	g_AnalogActionData = SteamInput()->GetAnalogActionData(m_InputHandles[hInput], analogActionHandle);
}

int GetAnalogActionDataActive() { return g_AnalogActionData.bActive; }
int GetAnalogActionDataMode() { return g_AnalogActionData.eMode; }
float GetAnalogActionDataX() { return g_AnalogActionData.x; }
float GetAnalogActionDataY() { return g_AnalogActionData.y; }

int GetAnalogActionHandle(const char *actionName)
{
	CheckInitializedPlugin(0);
	return GetActionHandleIndex(SteamInput()->GetAnalogActionHandle(actionName), &m_AnalogActionHandles);
}

//GetAnalogActionOrigins
char *GetAnalogActionOriginsJSON(int hInput, int hActionSet, int hAnalogAction)
{
	ValidateInputHandle(hInput, utils::CreateString("[]"));
	ValidateActionHandle(InputActionSetHandle_t, actionSetHandle, hActionSet, m_ActionSetHandles, utils::CreateString("[]"));
	ValidateActionHandle(InputAnalogActionHandle_t, analogActionHandle, hAnalogAction, m_AnalogActionHandles, utils::CreateString("[]"));
	std::ostringstream json;
	json << "[";
	if (IsSteamInitialized())
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

int GetConnectedControllers()
{
	CheckInitializedPlugin(0);
	m_InputCount = SteamInput()->GetConnectedControllers(m_InputHandles);
	return m_InputCount;
}

//GetControllerForGamepadIndex

int GetCurrentActionSet(int hInput)
{
	CheckInitializedPlugin(0);
	ValidateInputHandle(hInput, false);
	return GetActionHandleIndex(SteamInput()->GetCurrentActionSet(m_InputHandles[hInput]), &m_ActionSetHandles);
}

void GetDigitalActionData(int hInput, int hDigitalAction)
{
	CheckInitializedPlugin(NORETURN);
	ValidateInputHandle(hInput, );
	ValidateActionHandle(InputDigitalActionHandle_t, digitalActionHandle, hDigitalAction, m_DigitalActionHandles, );
	g_DigitalActionData = SteamInput()->GetDigitalActionData(m_InputHandles[hInput], digitalActionHandle);
}

int GetDigitalActionDataActive() { return g_DigitalActionData.bActive; }
int GetDigitalActionDataState() { return g_DigitalActionData.bState; }

int GetDigitalActionHandle(const char *actionName)
{
	CheckInitializedPlugin(0);
	return GetActionHandleIndex(SteamInput()->GetDigitalActionHandle(actionName), &m_DigitalActionHandles);
}

char *GetDigitalActionOriginsJSON(int hInput, int hActionSet, int hDigitalAction)
{
	ValidateInputHandle(hInput, utils::CreateString("[]"));
	ValidateActionHandle(InputActionSetHandle_t, actionSetHandle, hActionSet, m_ActionSetHandles, utils::CreateString("[]"));
	ValidateActionHandle(InputDigitalActionHandle_t, digitalActionHandle, hDigitalAction, m_DigitalActionHandles, utils::CreateString("[]"));
	std::ostringstream json;
	json << "[";
	if (IsSteamInitialized())
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

//GetGamepadIndexForController

char *GetGlyphForActionOrigin(int eOrigin)
{
	CheckInitializedPlugin(NULL_STRING);
	return utils::CreateString(SteamInput()->GetGlyphForActionOrigin((EInputActionOrigin)eOrigin));
}

int GetInputTypeForHandle(int hInput)
{
	CheckInitializedPlugin(0);
	ValidateInputHandle(hInput, k_ESteamInputType_Unknown);
	return SteamInput()->GetInputTypeForHandle(m_InputHandles[hInput]);
}

void GetMotionData(int hInput)
{
	CheckInitializedPlugin(NORETURN);
	ValidateInputHandle(hInput, );
	g_MotionData = SteamInput()->GetMotionData(m_InputHandles[hInput]);
}

float GetMotionDataPosAccelX() { return g_MotionData.posAccelX; }
float GetMotionDataPosAccelY() { return g_MotionData.posAccelY; }
float GetMotionDataPosAccelZ() { return g_MotionData.posAccelZ; }
float GetMotionDataRotQuatW() { return g_MotionData.rotQuatW; }
float GetMotionDataRotQuatX() { return g_MotionData.rotQuatX; }
float GetMotionDataRotQuatY() { return g_MotionData.rotQuatY; }
float GetMotionDataRotQuatZ() { return g_MotionData.rotQuatZ; }
float GetMotionDataRotVelX() { return g_MotionData.rotVelX; }
float GetMotionDataRotVelY() { return g_MotionData.rotVelY; }
float GetMotionDataRotVelZ() { return g_MotionData.rotVelZ; }

char *GetStringForActionOrigin(int eOrigin)
{
	CheckInitializedPlugin(NULL_STRING);
	return utils::CreateString(SteamInput()->GetStringForActionOrigin((EInputActionOrigin)eOrigin));
}

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

void RunFrame()
{
	CheckInitializedPlugin(NORETURN);
	SteamInput()->RunFrame();
}

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

//ShowAnalogActionOrigins - deprecated

int ShowBindingPanel(int hInput)
{
	CheckInitializedPlugin(false);
	ValidateInputHandle(hInput, false);
	return SteamInput()->ShowBindingPanel(m_InputHandles[hInput]);
}

//ShowDigitalActionOrigins - deprecated

int ShutdownSteamInput()
{
	CheckInitializedPlugin(0);
	return SteamInput()->Shutdown();
}

void StopAnalogActionMomentum(int hInput, int hAnalogAction)
{
	CheckInitializedPlugin(NORETURN);
	ValidateInputHandle(hInput, );
	ValidateActionHandle(InputAnalogActionHandle_t, analogActionHandle, hAnalogAction, m_AnalogActionHandles, );
	SteamInput()->StopAnalogActionMomentum(m_InputHandles[hInput], analogActionHandle);
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

// New for v1.43
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
#pragma endregion

#pragma region ISteamFriends
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

// TODO Merge?
void ActivateGameOverlayToWebPageModal(const char *url)
{
	CheckInitializedPlugin(NORETURN);
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
//GetFriendByIndex
//GetFriendCoplayGame
//GetFriendCoplayTime
//GetFriendCount
char *GetFriendListJSON(int friendFlags)
{
	std::string json("[");
	if (IsSteamInitialized())
	{
		for (int x = 0; x < SteamFriends()->GetFriendCount((EFriendFlags)friendFlags); x++)
		{
			if (x > 0)
			{
				json += ", ";
			}
			json += std::to_string(GetPluginHandle(SteamFriends()->GetFriendByIndex(x, (EFriendFlags)friendFlags)));
		}
	}
	json += "]";
	return utils::CreateString(json);
}

//GetFriendCountFromSource
//GetFriendFromSourceByIndex
//GetFriendGamePlayed
char *GetFriendGamePlayedJSON(int hUserSteamID)
{
	// TODO m_PersonaStateChangedEnabled = true;
	if (IsSteamInitialized())
	{
		FriendGameInfo_t pFriendGameInfo;
		bool ingame = SteamFriends()->GetFriendGamePlayed(GetSteamHandle(hUserSteamID), &pFriendGameInfo);
		//return utils::CreateString(ToJSON(ingame, pFriendGameInfo));
	}
	return utils::CreateString("{}");
}

//GetFriendMessage

char *GetFriendPersonaName(int hUserSteamID)
{
	CheckInitializedPlugin(NULL_STRING);
	// TODO m_PersonaStateChangedEnabled = true;
	return utils::CreateString(SteamFriends()->GetFriendPersonaName(GetSteamHandle(hUserSteamID)));
}

//GetFriendPersonaNameHistory

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

//GetFriendRichPresence
//GetFriendRichPresenceKeyByIndex
//GetFriendRichPresenceKeyCount

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

//GetFriendsGroupMembersList
char *GetFriendsGroupMembersListJSON(int hFriendsGroupID) // return a json array of SteamID handles
{
	if (IsSteamInitialized())
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

/*
NOTE: Steam docs say
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

// NOTE: The Steam API appears to implicitly call RequestUserInformation when needed.
int GetFriendAvatar(int hUserSteamID, int size)
{
	CheckInitializedPlugin(0);
	CSteamID steamID = GetSteamHandle(hUserSteamID);
	Callbacks()->EnableOnAvatarImageLoadedCallback();
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
	CheckInitializedPlugin(NULL_STRING);
	return utils::CreateString(SteamFriends()->GetPersonaName());
}

//GetPersonaState

char *GetPlayerNickname(int hUserSteamID)
{
	CheckInitializedPlugin(NULL_STRING);
	return utils::CreateString(SteamFriends()->GetPlayerNickname(GetSteamHandle(hUserSteamID)));
}

//GetUserRestrictions

int HasFriend(int hUserSteamID, int friendFlags)
{
	CheckInitializedPlugin(false);
	return SteamFriends()->HasFriend(GetSteamHandle(hUserSteamID), (EFriendFlags)friendFlags);
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
	CheckInitializedPlugin(false);
	return SteamFriends()->RequestUserInformation(GetSteamHandle(hUserSteamID), requireNameOnly != 0);
}

//SendClanChatMessage
//SetInGameVoiceSpeaking
//SetListenForFriendsMessages
//SetPersonaName
//SetPlayedWith
//SetRichPresence

//Callbacks
int HasAvatarImageLoaded()
{
	CheckInitializedPlugin(false);
	return Callbacks()->HasOnAvatarImageLoaded();
}

int GetAvatarImageLoadedUser()
{
	CheckInitializedPlugin(0);
	return GetPluginHandle(Callbacks()->GetOnAvatarImageLoaded());
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
	CheckInitializedPlugin(false);
	return Callbacks()->IsGameOverlayActive();
}

//GameRichPresenceJoinRequested_t
//GameServerChangeRequested_t
//JoinClanChatRoomCompletionResult_t

int HasPersonaStateChange()
{
	CheckInitializedPlugin(false);
	return Callbacks()->HasOnPersonaStateChange();
}

char *GetPersonaStateChangeJSON()
{
	CheckInitializedPlugin(0);
	//return utils::CreateString(ToJSON(Callbacks()->GetPersonaStateChange()));
	return utils::CreateString("{}");
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

//AddRequestLobbyListCompatibleMembersFilter

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

//CheckForPSNGameBootInvite - deprecated

int CreateLobby(int eLobbyType, int maxMembers)
{
	CheckInitializedPlugin(0);
	return CallResults()->Add(new CLobbyCreatedCallResult((ELobbyType)eLobbyType, maxMembers));
}

int DeleteLobbyData(int hLobbySteamID, const char *key)
{
	CheckInitializedPlugin(false);
	return SteamMatchmaking()->DeleteLobbyData(GetSteamHandle(hLobbySteamID), key);
}

//GetFavoriteGame
// Leave this as by-index since AddFavoriteGame appears to return an index.
char *GetFavoriteGameJSON(int index)
{
	if (IsSteamInitialized())
	{
		plugin::FavoriteGame_t info;
		if (SteamMatchmaking()->GetFavoriteGame(index, &info.m_AppID, &info.m_nIP, &info.m_nConnPort, &info.m_nQueryPort, &info.m_unFlags, &info.m_rTime32LastPlayedOnServer))
		{
			//return utils::CreateString(ToJSON(info));
		}
	}
	return utils::CreateString("{}");
}

int GetFavoriteGameCount()
{
	CheckInitializedPlugin(0);
	return SteamMatchmaking()->GetFavoriteGameCount();
}

//GetLobbyByIndex - used in callback
//GetLobbyChatEntry - used in callback

char *GetLobbyData(int hLobbySteamID, const char *key)
{
	CheckInitializedPlugin(NULL_STRING);
	return utils::CreateString(SteamMatchmaking()->GetLobbyData(GetSteamHandle(hLobbySteamID), key));
}

//GetLobbyDataByIndex
//GetLobbyDataCount
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

//GetLobbyGameServer
char *GetLobbyGameServerJSON(int hLobbySteamID)
{
	if (IsSteamInitialized())
	{
		plugin::LobbyGameServer_t server;
		if (SteamMatchmaking()->GetLobbyGameServer(GetSteamHandle(hLobbySteamID), &server.m_unGameServerIP, &server.m_unGameServerPort, &server.m_ulSteamIDGameServer))
		{
			//return utils::CreateString(ToJSON(server));
		}
	}
	return utils::CreateString("{}");
}

int GetLobbyMemberByIndex(int hLobbySteamID, int index)
{
	CheckInitializedPlugin(0);
	return GetPluginHandle(SteamMatchmaking()->GetLobbyMemberByIndex(GetSteamHandle(hLobbySteamID), index));
}

char *GetLobbyMemberData(int hLobbySteamID, int hUserSteamID, const char *key)
{
	CheckInitializedPlugin(NULL_STRING);
	return utils::CreateString(SteamMatchmaking()->GetLobbyMemberData(GetSteamHandle(hLobbySteamID), GetSteamHandle(hUserSteamID), key));
}

int GetLobbyMemberLimit(int hLobbySteamID)
{
	CheckInitializedPlugin(0);
	return SteamMatchmaking()->GetLobbyMemberLimit(GetSteamHandle(hLobbySteamID));
}

int GetLobbyOwner(int hLobbySteamID)
{
	CheckInitializedPlugin(0);
	return GetPluginHandle(SteamMatchmaking()->GetLobbyOwner(GetSteamHandle(hLobbySteamID)));
}

int GetNumLobbyMembers(int hLobbySteamID)
{
	CheckInitializedPlugin(0);
	return SteamMatchmaking()->GetNumLobbyMembers(GetSteamHandle(hLobbySteamID));
}

int InviteUserToLobby(int hLobbySteamID, int hInviteeSteamID)
{
	CheckInitializedPlugin(false);
	return SteamMatchmaking()->InviteUserToLobby(GetSteamHandle(hLobbySteamID), GetSteamHandle(hInviteeSteamID));
}

int JoinLobby(int hLobbySteamID)
{
	CheckInitializedPlugin(false);
	return CallResults()->Add(new CLobbyEnterCallResult(GetSteamHandle(hLobbySteamID)));
}

void LeaveLobby(int hLobbySteamID)
{
	CheckInitializedPlugin(NORETURN);
	CSteamID steamIDLobby = GetSteamHandle(hLobbySteamID);
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

int RequestLobbyData(int hLobbySteamID)
{
	CheckInitializedPlugin(false);
	return SteamMatchmaking()->RequestLobbyData(GetSteamHandle(hLobbySteamID));
}

int RequestLobbyList()
{
	CheckInitializedPlugin(0);
	return CallResults()->Add(new CLobbyMatchListCallResult());
}

int SendLobbyChatMessage(int hLobbySteamID, const char *msg)
{
	CheckInitializedPlugin(0);
	return SteamMatchmaking()->SendLobbyChatMsg(GetSteamHandle(hLobbySteamID), msg, (int)strlen(msg) + 1);
}

//int SetLinkedLobby(int hLobbySteamID, int hLobbyDependentSteamID)
//{
//	CheckInitializedPlugin(false);
//	return Callbacks()->SetLinkedLobby(GetSteamHandle(hLobbySteamID), GetSteamHandle(hLobbyDependentSteamID));
//}

void SetLobbyData(int hLobbySteamID, const char *key, const char *value)
{
	CheckInitializedPlugin(NORETURN);
	SteamMatchmaking()->SetLobbyData(GetSteamHandle(hLobbySteamID), key, value);
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

int SetLobbyJoinable(int hLobbySteamID, int lobbyJoinable)
{
	CheckInitializedPlugin(false);
	return SteamMatchmaking()->SetLobbyJoinable(GetSteamHandle(hLobbySteamID), lobbyJoinable != 0);
}

void SetLobbyMemberData(int hLobbySteamID, const char *key, const char *value)
{
	CheckInitializedPlugin(NORETURN);
	return SteamMatchmaking()->SetLobbyMemberData(GetSteamHandle(hLobbySteamID), key, value);
}

int SetLobbyMemberLimit(int hLobbySteamID, int maxMembers)
{
	CheckInitializedPlugin(0);
	return SteamMatchmaking()->SetLobbyMemberLimit(GetSteamHandle(hLobbySteamID), maxMembers);
}

int SetLobbyOwner(int hLobbySteamID, int hNewOwnerSteamID)
{
	CheckInitializedPlugin(0);
	return SteamMatchmaking()->SetLobbyOwner(GetSteamHandle(hLobbySteamID), GetSteamHandle(hNewOwnerSteamID));
}

int SetLobbyType(int hLobbySteamID, int eLobbyType)
{
	CheckInitializedPlugin(false);
	return SteamMatchmaking()->SetLobbyType(GetSteamHandle(hLobbySteamID), (ELobbyType)eLobbyType);
}

// Callbacks
int HasLobbyChatMessage()
{
	CheckInitializedPlugin(false);
	return Callbacks()->HasOnLobbyChatMessage();
}

// TODO JSON
int GetLobbyChatMessageUser()
{
	CheckInitializedPlugin(0);
	return GetPluginHandle(Callbacks()->GetOnLobbyChatMessage().m_ulSteamIDUser);
}

char *GetLobbyChatMessageText()
{
	CheckInitializedPlugin(NULL_STRING);
	return utils::CreateString(Callbacks()->GetOnLobbyChatMessage().m_chChatEntry);
}

int HasLobbyChatUpdate()
{
	CheckInitializedPlugin(false);
	return Callbacks()->HasOnLobbyChatUpdate();
}

int GetLobbyChatUpdateLobby()
{
	CheckInitializedPlugin(0);
	return GetPluginHandle(Callbacks()->GetOnLobbyChatUpdate().m_ulSteamIDLobby);
}

int GetLobbyChatUpdateUserChanged()
{
	CheckInitializedPlugin(0);
	return GetPluginHandle(Callbacks()->GetOnLobbyChatUpdate().m_ulSteamIDUserChanged);
}

int GetLobbyChatUpdateUserState()
{
	CheckInitializedPlugin(0);
	return Callbacks()->GetOnLobbyChatUpdate().m_rgfChatMemberStateChange;
}

int GetLobbyChatUpdateUserMakingChange()
{
	CheckInitializedPlugin(0);
	return GetPluginHandle(Callbacks()->GetOnLobbyChatUpdate().m_ulSteamIDMakingChange);
}

int HasLobbyDataUpdateResponse()
{
	CheckInitializedPlugin(false);
	return Callbacks()->HasOnLobbyDataUpdate();
}

char *GetLobbyDataUpdateResponseJSON()
{
	CheckInitializedPlugin(NULL_STRING);
	//return utils::CreateString(ToJSON(Callbacks()->GetLobbyDataUpdate()));
	return utils::CreateString("{}");
}

int HasLobbyEnterResponse()
{
	CheckInitializedPlugin(false);
	return Callbacks()->HasOnLobbyEnter();
}

char *GetLobbyEnterResponseJSON()
{
	CheckInitializedPlugin(NULL_STRING);
	// Must be the same as in CLobbyEnterCallResult.
	return utils::CreateString("{}"); // ToJSON(Callbacks()->GetLobbyEnter()));
}

int HasGameLobbyJoinRequest()
{
	CheckInitializedPlugin(0);
	return Callbacks()->HasOnGameLobbyJoinRequested();
}

int GetGameLobbyJoinRequestedLobby()
{
	CheckInitializedPlugin(0);
	return GetPluginHandle(Callbacks()->GetOnGameLobbyJoinRequested().m_steamIDLobby);
}

int HasLobbyGameCreated()
{
	CheckInitializedPlugin(false);
	return Callbacks()->HasOnLobbyGameCreated();
}

char *GetLobbyGameCreatedJSON()
{
	if (IsSteamInitialized())
	{
		//return utils::CreateString(ToJSON(Callbacks()->GetLobbyGameCreated()));
	}
	return utils::CreateString("{}");
}
#pragma endregion

#pragma region ISteamMatchmakingServers
// No callbacks.
#pragma endregion

#pragma region ISteamMusic
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

//Callbacks
int HasMusicPlaybackStatusChanged()
{
	CheckInitializedPlugin(0);
	return Callbacks()->HasOnPlaybackStatusHasChanged();
}

int HasMusicVolumeChanged()
{
	CheckInitializedPlugin(0);
	return Callbacks()->HasOnVolumeHasChanged();
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
	CheckInitializedPlugin(false);
	return SteamRemoteStorage()->FileDelete(filename);
}

int CloudFileExists(const char *filename)
{
	CheckInitializedPlugin(false);
	return SteamRemoteStorage()->FileExists(filename);
}

//FileFetch - deprecated

int CloudFileForget(const char *filename)
{
	CheckInitializedPlugin(false);
	return SteamRemoteStorage()->FileForget(filename);
}

//FilePersist - deprecated
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
	return CallResults()->Add(new CFileReadAsyncCallResult(filename, offset, length));
}

char *GetCloudFileReadAsyncFileName(int hCallResult)
{
	CheckInitializedPlugin(NULL_STRING);
	if (CFileReadAsyncCallResult *callResult = CallResults()->Get<CFileReadAsyncCallResult>(hCallResult))
	{
		return utils::CreateString(callResult->GetFileName());
	}
	return NULL_STRING;
}

int GetCloudFileReadAsyncMemblock(int hCallResult)
{
	CheckInitializedPlugin(0);
	if (CFileReadAsyncCallResult *callResult = CallResults()->Get<CFileReadAsyncCallResult>(hCallResult))
	{
		return callResult->GetMemblockID();
	}
	return 0;
}

//FileShare
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
		return CallResults()->Add(new CFileWriteAsyncCallResult(filename, agk::GetMemblockPtr(memblockID), agk::GetMemblockSize(memblockID)));
	}
	return 0;
}

char *GetCloudFileWriteAsyncFileName(int hCallResult)
{
	CheckInitializedPlugin(NULL_STRING);
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
	CheckInitializedPlugin(0);
	return SteamRemoteStorage()->GetCachedUGCCount();
}

//GetCachedUGCHandle

int GetCloudFileCount()
{
	CheckInitializedPlugin(0);
	return SteamRemoteStorage()->GetFileCount();
}

//GetFileListFromServer

//GetFileNameAndSize
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
	CheckInitializedPlugin(0);
	return SteamRemoteStorage()->GetFileSize(filename);
}

int GetCloudFileTimestamp(const char *filename)
{
	CheckInitializedPlugin(0);
	// NOTE: Dangerous conversion int64 to int.
	return (int)SteamRemoteStorage()->GetFileTimestamp(filename);
}

//GetPublishedFileDetails
//GetPublishedItemVoteDetails

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
	if (IsSteamInitialized())
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

//GetUGCDetails
//GetUGCDownloadProgress
//GetUserPublishedItemVoteDetails

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

//PublishVideo
//PublishWorkshopFile
//ResetFileRequestState

void SetCloudEnabledForApp(int enabled)
{
	CheckInitializedPlugin(NORETURN);
	return SteamRemoteStorage()->SetCloudEnabledForApp(enabled != 0);
}

int SetCloudFileSyncPlatforms(const char *filename, int eRemoteStoragePlatform)
{
	CheckInitializedPlugin(false);
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
	CheckInitializedPlugin(0);
	return GetPluginHandle(SteamUser()->GetSteamID());
}

// Not in the API, but grouping this near GetSteamID().
int IsSteamIDValid(int hSteamID)
{
	CheckInitializedPlugin(false);
	CSteamID steamID = GetSteamHandle(hSteamID);
	return steamID.IsValid();
}

// Not in the API, but grouping this near GetSteamID().
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

// Not in the API, but grouping this near GetSteamID().
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

// Not in the API, but grouping this near GetSteamID().
char *GetSteamID64(int hUserSteamID)
{
	CheckInitializedPlugin(NULL_STRING);
	char id64[21]; // Max value is 18,446,744,073,709,551,615
	_i64toa(GetSteamHandle(hUserSteamID), id64, 10);
	return utils::CreateString(id64);
}

// Not in the API, but grouping this near GetSteamID().
int GetHandleFromSteamID64(const char *steamID64)
{
	CheckInitializedPlugin(0);
	uint64 id = _atoi64(steamID64);
	return GetPluginHandle(CSteamID(id));
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
	CheckInitializedPlugin(false);
	return SteamUserStats()->ClearAchievement(name);
}

int DownloadLeaderboardEntries(int hLeaderboard, int eLeaderboardDataRequest, int rangeStart, int rangeEnd)
{
	CheckInitializedPlugin(0);
	SteamLeaderboard_t leaderboard = GetSteamHandle(hLeaderboard);
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
	CheckInitializedPlugin(false);
	return CallResults()->Add(new CLeaderboardFindCallResult(leaderboardName));
}

//FindOrCreateLeaderboard

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

//GetAchievementAchievedPercent

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
	return Callbacks()->GetAchievementIcon(name);
}

char *GetAchievementAPIName(int index)
{
	CheckInitializedPlugin(NULL_STRING);
	return utils::CreateString(SteamUserStats()->GetAchievementName(index));
}

//GetGlobalStat
//GetGlobalStatHistory

int GetLeaderboardDisplayType(int hLeaderboard)
{
	CheckInitializedPlugin(0);
	return SteamUserStats()->GetLeaderboardDisplayType(GetSteamHandle(hLeaderboard));
}

int GetLeaderboardEntryCount(int hLeaderboard)
{
	CheckInitializedPlugin(0);
	return SteamUserStats()->GetLeaderboardEntryCount(GetSteamHandle(hLeaderboard));
}

char *GetLeaderboardName(int hLeaderboard)
{
	CheckInitializedPlugin(NULL_STRING);
	return utils::CreateString(SteamUserStats()->GetLeaderboardName(GetSteamHandle(hLeaderboard)));
}

int GetLeaderboardSortMethod(int hLeaderboard)
{
	CheckInitializedPlugin(0);
	return SteamUserStats()->GetLeaderboardSortMethod(GetSteamHandle(hLeaderboard));
}

//GetMostAchievedAchievementInfo
//GetNextMostAchievedAchievementInfo

int GetNumAchievements()
{
	CheckInitializedPlugin(0);
	return SteamUserStats()->GetNumAchievements();
}

//GetNumberOfCurrentPlayers

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

//GetTrophySpaceRequiredBeforeInstall - deprecated
//GetUserAchievement
//GetUserAchievementAndUnlockTime
//GetUserStat
//GetUserStatsData

int IndicateAchievementProgress(const char *name, int curProgress, int maxProgress)
{
	CheckInitializedPlugin(false);
	return SteamUserStats()->IndicateAchievementProgress(name, curProgress, maxProgress);
}

//InstallPS3Trophies

int RequestStats()
{
	CheckInitializedPlugin(false);
	Callbacks()->EnableOnUserStatsReceivedCallback();
	return SteamUserStats()->RequestCurrentStats();
}

//RequestGlobalAchievementPercentages
//RequestGlobalStats
//RequestUserStats

int ResetAllStats(int achievementsToo)
{
	CheckInitializedPlugin(false);
	return SteamUserStats()->ResetAllStats(achievementsToo != 0);
}

int SetAchievement(const char *name)
{
	CheckInitializedPlugin(false);
	return SteamUserStats()->SetAchievement(name);
}

int SetStatInt(const char *name, int value)
{
	CheckInitializedPlugin(false);
	return SteamUserStats()->SetStat(name, value);
}

int SetStatFloat(const char *name, float value)
{
	CheckInitializedPlugin(false);
	return SteamUserStats()->SetStat(name, value);
}

//SetUserStatsData

int StoreStats()
{
	CheckInitializedPlugin(false);
	return SteamUserStats()->StoreStats();
}

int UpdateAvgRateStat(const char *name, float countThisSession, float sessionLength)
{
	CheckInitializedPlugin(false);
	return SteamUserStats()->UpdateAvgRateStat(name, countThisSession, (double)sessionLength);
}

int UploadLeaderboardScore(int hLeaderboard, ELeaderboardUploadScoreMethod eLeaderboardUploadScoreMethod, int score)
{
	CheckInitializedPlugin(0);
	SteamLeaderboard_t leaderboard = GetSteamHandle(hLeaderboard);
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

int HasNewUserAchievementStored()
{
	return Callbacks()->HasOnUserAchievementStored();
}

int HasNewUserStatsReceived()
{
	return Callbacks()->HasOnUserStatsReceived();
}

int GetNewUserStatsReceivedGameID()
{
	return (int)Callbacks()->GetOnUserStatsReceived().m_nGameID;
}

int GetNewUserStatsReceivedResult()
{
	return (int)Callbacks()->GetOnUserStatsReceived().m_eResult;
}

int GetNewUserStatsReceivedUser()
{
	return GetPluginHandle(Callbacks()->GetOnUserStatsReceived().m_steamIDUser);
}

int StatsInitialized()
{
	return Callbacks()->StatsInitialized();
}

int HasNewUserStatsStored()
{
	return Callbacks()->HasOnUserStatsStored();
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
	CheckInitializedPlugin(0);
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
	CheckInitializedPlugin(0);
	return LoadImageFromHandle(0, hImage);
}

void LoadImageIDFromHandle(int imageID, int hImage)
{
	CheckInitializedPlugin(NORETURN);
	LoadImageFromHandle(imageID, hImage);
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

//IsAPICallCompleted

int IsOverlayEnabled()
{
	CheckInitializedPlugin(0);
	return SteamUtils()->IsOverlayEnabled();
}

int IsSteamInBigPictureMode()
{
	CheckInitializedPlugin(0);
	return SteamUtils()->IsSteamInBigPictureMode();
}

int IsSteamRunningInVR()
{
	CheckInitializedPlugin(false);
	return SteamUtils()->IsSteamRunningInVR();
}

int IsVRHeadsetStreamingEnabled()
{
	CheckInitializedPlugin(false);
	return SteamUtils()->IsVRHeadsetStreamingEnabled();
}

void SetOverlayNotificationInset(int horizontalInset, int verticalInset)
{
	CheckInitializedPlugin(NORETURN);
	SteamUtils()->SetOverlayNotificationInset(horizontalInset, verticalInset);
}

void SetOverlayNotificationPosition(int eNotificationPosition)
{
	CheckInitializedPlugin(NORETURN);
	SteamUtils()->SetOverlayNotificationPosition((ENotificationPosition)eNotificationPosition);
}

void SetVRHeadsetStreamingEnabled(int enabled)
{
	CheckInitializedPlugin(NORETURN);
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
	CheckInitializedPlugin(NORETURN);
	SteamUtils()->SetWarningMessageHook(&SteamAPIDebugTextHook);
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

void StartVRDashboard()
{
	CheckInitializedPlugin(NORETURN);
	SteamUtils()->StartVRDashboard();
}

//Callbacks
//CheckFileSignature_t

int HasGamepadTextInputDismissedInfo()
{
	CheckInitializedPlugin(0);
	return Callbacks()->HasOnGamepadTextInputDismissed();
}

char *GetGamepadTextInputDismissedInfoJSON()
{
	if (IsSteamInitialized())
	{
		//return utils::CreateString(ToJSON(Callbacks()->GetGamepadTextInputDismissed()));
	}
	return utils::CreateString("{}");
}

int HasIPCountryChanged()
{
	CheckInitializedPlugin(false);
	return Callbacks()->HasOnIPCountryChanged();
}

int HasLowBatteryWarning()
{
	CheckInitializedPlugin(false);
	return Callbacks()->HasOnLowBatteryPower();
}

int GetMinutesBatteryLeft()
{
	CheckInitializedPlugin(255);
	return Callbacks()->GetMinutesBatteryLeft();
}

//SteamAPICallCompleted_t

int IsSteamShuttingDown()
{
	CheckInitializedPlugin(false);
	return Callbacks()->HasOnSteamShutdown();
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
