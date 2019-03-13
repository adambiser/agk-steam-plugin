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

#ifndef _STEAMPLUGIN_H_
#define _STEAMPLUGIN_H_
#pragma once

#include <steam_api.h>
#include <map>
#include <list>
#include <vector>
#include <mutex>
#include "CCallResultItem.h"
#include "StructClear.h"

#if defined(_WINDOWS)
#if defined(_WIN64)
#pragma comment(lib, "steam_api64.lib")
#else
#pragma comment(lib, "steam_api.lib")
#endif
#endif

enum EAvatarSize
{
	Small = 0,
	Medium = 1,
	Large = 2
};

#define CheckInitialized(steamInterface, returnValue)				\
	if (!g_SteamInitialized || (NULL == steamInterface()))			\
	{																\
		agk::PluginError("Steam API has not been initialized.");	\
		return returnValue;											\
	}

#define SETUP_CALLBACK_LIST(callback, type)	\
public:										\
	bool Has##callback##();					\
	type Get##callback##() { return m_Current##callback##; } \
private:									\
	std::list<##type##> m_##callback##List;	\
	std::mutex m_##callback##Mutex;			\
	type m_Current##callback

// Reports true only once per change.
#define SETUP_CALLBACK_BOOL(callback)		\
private:									\
	bool m_b##callback##;					\
public:										\
	bool Has##callback##()					\
	{										\
		if (m_b##callback##)				\
		{									\
			m_b##callback## = false;		\
			return true;					\
		}									\
		return false;						\
	}

// Adds the result to the end of the callback list.
#define StoreCallbackResult(callback, result)	\
	m_##callback##Mutex.lock();					\
	m_##callback##List.push_back(result);		\
	m_##callback##Mutex.unlock()

// Moves the front value of the list into the current value variable.
#define GetNextCallbackResult(callback)						\
	m_##callback##Mutex.lock();								\
	if (m_##callback##List.size() > 0)						\
	{														\
		m_Current##callback = m_##callback##List.front();	\
		m_##callback##List.pop_front();						\
		m_##callback##Mutex.unlock();						\
		return true;										\
	}														\
	Clear(m_Current##callback##);							\
	m_##callback##Mutex.unlock();							\
	return false

// Clears the callback list and current value variable.
#define ClearCallbackList(callback) \
	m_##callback##Mutex.lock();		\
	m_##callback##List.clear();		\
	Clear(m_Current##callback##);	\
	m_##callback##Mutex.unlock()

class SteamPlugin
{
public:
	SteamPlugin();
	virtual ~SteamPlugin(void);

	// General methods.
private:
	void ResetSessionVariables();
public:
	void Shutdown();

	// TODO Remove this.
	// Return to Idle after reporting Done.
	ECallbackState getCallbackState(ECallbackState *callbackState)
	{
		if (*callbackState == Done)
		{
			*callbackState = Idle;
			return Done;
		}
		return *callbackState;
	}

	// CallResults
private:
	std::map<int, CCallResultItem*> m_CallResultItems;
	int m_CurrentCallResultHandle;
	int AddCallResultItem(CCallResultItem *callResult);
public:
	// Template methods MUST be defined in the header file!
	template <class T> T *GetCallResultItem(int hCallResult)
	{
		auto search = m_CallResultItems.find(hCallResult);
		if (search == m_CallResultItems.end())
		{
			utils::PluginError("Invalid call result handle: " + std::to_string(hCallResult));
			return NULL;
		}
		T *callResult = dynamic_cast<T*>(m_CallResultItems[hCallResult]);
		if (!callResult)
		{
			utils::PluginError("Call result handle " + std::to_string(hCallResult) + " was not the expected call result type.");
			return NULL;
		}
		return callResult;
	}
	void DeleteCallResultItem(int hCallResult);
	ECallbackState GetCallResultItemState(int hCallResult);

	// App/DLC methods
private:
	STEAM_CALLBACK(SteamPlugin, OnNewLaunchQueryParameters, NewUrlLaunchParameters_t, m_CallbackNewLaunchQueryParameters);
	SETUP_CALLBACK_BOOL(NewLaunchQueryParameters);
	STEAM_CALLBACK(SteamPlugin, OnDlcInstalled, DlcInstalled_t, m_CallbackDlcInstalled);
	SETUP_CALLBACK_LIST(NewDlcInstalled, DlcInstalled_t);
	bool m_OnDlcInstalledEnabled;
public:

	// Overlay methods
private:
	STEAM_CALLBACK(SteamPlugin, OnGameOverlayActivated, GameOverlayActivated_t, m_CallbackGameOverlayActivated);

	// User/Friend methods
private:
	STEAM_CALLBACK(SteamPlugin, OnPersonaStateChange, PersonaStateChange_t, m_CallbackPersonaStateChange);
	SETUP_CALLBACK_LIST(PersonaStateChange, PersonaStateChange_t);
	bool m_PersonaStateChangedEnabled;
	STEAM_CALLBACK(SteamPlugin, OnAvatarImageLoaded, AvatarImageLoaded_t, m_CallbackAvatarImageLoaded);
	SETUP_CALLBACK_LIST(AvatarImageLoadedUser, CSteamID);
	bool m_AvatarImageLoadedEnabled; // Added so games that don't load friend avatars don't waste memory storing things it never uses.
public:
	//EPersonaState GetPersonaState();
	//uint32 GetUserRestrictions();
	CSteamID GetSteamID();
	//steamUser->GetPlayerSteamLevel()
	//m_PersonaStateChange
	int GetFriendAvatar(CSteamID steamID, EAvatarSize size);

	// General user stats methods.
private:
	STEAM_CALLBACK(SteamPlugin, OnUserStatsReceived, UserStatsReceived_t, m_CallbackUserStatsReceived);
	ECallbackState m_RequestStatsCallbackState;
	bool m_StatsInitialized;
	STEAM_CALLBACK(SteamPlugin, OnUserStatsStored, UserStatsStored_t, m_CallbackUserStatsStored);
	STEAM_CALLBACK(SteamPlugin, OnAchievementStored, UserAchievementStored_t, m_CallbackAchievementStored);
	ECallbackState m_StoreStatsCallbackState;
	bool m_StatsStored;
	bool m_AchievementStored;
public:
	ECallbackState GetRequestStatsCallbackState() { return getCallbackState(&m_RequestStatsCallbackState); }
	bool StatsInitialized() { return m_StatsInitialized; }
	bool StoreStats();
	bool ResetAllStats(bool bAchievementsToo);
	ECallbackState GetStoreStatsCallbackState() { return getCallbackState(&m_StoreStatsCallbackState); }
	bool StatsStored() { return m_StatsStored; }
	bool AchievementStored() { return m_AchievementStored; }

	// Achievements methods.
private:
	STEAM_CALLBACK(SteamPlugin, OnAchievementIconFetched, UserAchievementIconFetched_t, m_CallbackAchievementIconFetched);
	std::map<std::string, int> m_AchievementIconsMap;
public:
	// While GetAchievementIcon has an internal callback, there's no need to make them external.
	int GetAchievementIcon(const char *pchName);

	// Leaderboard methods: Find
public:
	int FindLeaderboard(const char *pchLeaderboardName);

	// Leaderboard methods: Upload
public:
	int UploadLeaderboardScore(SteamLeaderboard_t hLeaderboard, ELeaderboardUploadScoreMethod eLeaderboardUploadScoreMethod, int score);

	// Leaderboard methods: Download
public:
	int DownloadLeaderboardEntries(SteamLeaderboard_t hLeaderboard, ELeaderboardDataRequest eLeaderboardDataRequest, int nRangeStart, int nRangeEnd);

	// Lobby methods: List
public:
	int RequestLobbyList();
		
	// Lobby methods: Create, Join, Leave
private:
	STEAM_CALLBACK(SteamPlugin, OnLobbyEnter, LobbyEnter_t, m_MainCallResultLobbyEnter);
	SETUP_CALLBACK_LIST(LobbyEnter, LobbyEnter_t);
	std::vector<CSteamID> m_JoinedLobbies; // Keep track so we don't leave any left open when closing.
	STEAM_CALLBACK(SteamPlugin, OnGameLobbyJoinRequested, GameLobbyJoinRequested_t, m_CallbackGameLobbyJoinRequested);
	SETUP_CALLBACK_LIST(GameLobbyJoinRequested, GameLobbyJoinRequested_t);
public:
	int CreateLobby(ELobbyType eLobbyType, int cMaxMembers);
	//bool SetLinkedLobby(CSteamID steamIDLobby, CSteamID steamIDLobbyDependent);
	int JoinLobby(CSteamID steamIDLobby);
	void LeaveLobby(CSteamID steamIDLobby);

	// Lobby methods: Game server
private:
	STEAM_CALLBACK(SteamPlugin, OnLobbyGameCreated, LobbyGameCreated_t, m_CallbackLobbyGameCreated);
	SETUP_CALLBACK_LIST(LobbyGameCreated, LobbyGameCreated_t);
public:

	// Lobby methods: Data
private:
	STEAM_CALLBACK(SteamPlugin, OnLobbyDataUpdate, LobbyDataUpdate_t, m_CallResultLobbyDataUpdate);
	SETUP_CALLBACK_LIST(LobbyDataUpdate, LobbyDataUpdate_t);
public:

	// Lobby methods: Members and Status
private:
	STEAM_CALLBACK(SteamPlugin, OnLobbyChatUpdate, LobbyChatUpdate_t, m_CallbackLobbyChatUpdate);
	SETUP_CALLBACK_LIST(LobbyChatUpdate, LobbyChatUpdate_t);
public:

	// Lobby methods: Chat messages
private:
	STEAM_CALLBACK(SteamPlugin, OnLobbyChatMessage, LobbyChatMsg_t, m_CallbackLobbyChatMessage);
	SETUP_CALLBACK_LIST(LobbyChatMessage, plugin::LobbyChatMsg_t);
public:

	// Music methods
private:
	STEAM_CALLBACK(SteamPlugin, OnPlaybackStatusHasChanged, PlaybackStatusHasChanged_t, m_CallbackPlaybackStatusHasChanged);
	SETUP_CALLBACK_BOOL(MusicPlaybackStatusChanged);
	STEAM_CALLBACK(SteamPlugin, OnVolumeHasChanged, VolumeHasChanged_t, m_CallbackVolumeHasChanged);
	SETUP_CALLBACK_BOOL(MusicVolumeChanged);

	// Util methods
private:
	STEAM_CALLBACK(SteamPlugin, OnIPCountryChanged, IPCountry_t, m_CallbackIPCountryChanged);
	SETUP_CALLBACK_BOOL(IPCountryChanged);
	STEAM_CALLBACK(SteamPlugin, OnLowBatteryPower, LowBatteryPower_t, m_CallbackLowBatteryPower);
	SETUP_CALLBACK_BOOL(LowBatteryWarning);
	uint8 m_nMinutesBatteryLeft;
	STEAM_CALLBACK(SteamPlugin, OnSteamShutdown, SteamShutdown_t, m_CallbackSteamShutdown);
	SETUP_CALLBACK_BOOL(SteamShutdownNotification);
public:
	uint8 GetMinutesBatteryLeft() { return m_nMinutesBatteryLeft; }

	// Big Picture methods
private:
	STEAM_CALLBACK(SteamPlugin, OnGamepadTextInputDismissed, GamepadTextInputDismissed_t, m_CallbackGamepadTextInputDismissed);
	SETUP_CALLBACK_LIST(GamepadTextInputDismissed, plugin::GamepadTextInputDismissed_t);

	// Cloud Methods: Files
public:
	int FileReadAsync(const char *pchFile, uint32 nOffset, uint32 cubToRead);
	std::string GetFileReadAsyncFileName(int hCallResult);
	int GetFileReadAsyncMemblock(int hCallResult);
	//SteamAPICall_t FileShare(const char *pchFile);
	int FileWriteAsync(const char *pchFile, const void *pvData, uint32 cubData);
	std::string GetFileWriteAsyncFileName(int hCallResult);
	//bool FileWriteStreamCancel(UGCFileWriteStreamHandle_t writeHandle);
	//bool FileWriteStreamClose(UGCFileWriteStreamHandle_t writeHandle);
	//UGCFileWriteStreamHandle_t FileWriteStreamOpen(const char *pchFile);
	//bool FileWriteStreamWriteChunk(UGCFileWriteStreamHandle_t writeHandle, const void *pvData, int32 cubData);

	// Input Information
private:
	//int m_InputCount;
	//InputHandle_t m_InputHandles[STEAM_INPUT_MAX_COUNT];
public:

	// Input Action Sets and Layers
public:

	// Input Actions and Motion
public:
	InputAnalogActionData_t m_AnalogActionData;
	void GetAnalogActionData(InputHandle_t inputHandle, InputAnalogActionHandle_t analogActionHandle);
	InputDigitalActionData_t m_DigitalActionData;
	void GetDigitalActionData(InputHandle_t inputHandle, InputDigitalActionHandle_t digitalActionHandle);
	InputMotionData_t m_MotionData;
	void GetMotionData(InputHandle_t inputHandle);

};

#endif // _STEAMPLUGIN_H_
