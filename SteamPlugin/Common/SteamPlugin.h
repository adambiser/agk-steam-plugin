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
	if (!m_SteamInitialized || (NULL == steamInterface()))			\
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
	uint64 m_AppID;
	bool m_SteamInitialized;
	void ResetSessionVariables();
public:
	void Shutdown();
	bool SteamInitialized() { return m_SteamInitialized; }

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
	bool m_IsGameOverlayActive;
public:
	bool IsGameOverlayActive() { return m_IsGameOverlayActive; }

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

	// Image methods
public:
	int LoadImageFromHandle(int hImage);
	int LoadImageFromHandle(int imageID, int hImage);

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
	int GetNumAchievements();
	const char* GetAchievementAPIName(int index);
	const char *GetAchievementDisplayAttribute(const char *pchName, const char *pchKey);
	// While GetAchievementIcon has an internal callback, there's no need to make them external.
	int GetAchievementIcon(const char *pchName);
	bool GetAchievement(const char *name, bool *pbAchieved);
	bool GetAchievementAndUnlockTime(const char *pchName, bool *pbAchieved, uint32 *punUnlockTime);
	bool SetAchievement(const char *pchName);
	bool IndicateAchievementProgress(const char *pchName, uint32 nCurProgress, uint32 nMaxProgress);
	bool ClearAchievement(const char *pchName);

	// User stats methods.
public:
	bool GetStat(const char *pchName, int32 *pData);
	bool GetStat(const char *pchName, float *pData);
	bool SetStat(const char *pchName, int32 nData);
	bool SetStat(const char *pchName, float fData);
	bool UpdateAvgRateStat(const char *pchName, float flCountThisSession, double dSessionLength);

	// Leaderboard methods: Find
public:
	int FindLeaderboard(const char *pchLeaderboardName);

	// Leaderboard methods: Information
public:
	const char *GetLeaderboardName(SteamLeaderboard_t hLeaderboard);
	int GetLeaderboardEntryCount(SteamLeaderboard_t hLeaderboard);
	ELeaderboardDisplayType GetLeaderboardDisplayType(SteamLeaderboard_t hLeaderboard);
	ELeaderboardSortMethod GetLeaderboardSortMethod(SteamLeaderboard_t hLeaderboard);

	// Leaderboard methods: Upload
public:
	int UploadLeaderboardScore(SteamLeaderboard_t hLeaderboard, ELeaderboardUploadScoreMethod eLeaderboardUploadScoreMethod, int score);

	// Leaderboard methods: Download
public:
	int DownloadLeaderboardEntries(SteamLeaderboard_t hLeaderboard, ELeaderboardDataRequest eLeaderboardDataRequest, int nRangeStart, int nRangeEnd);

	// Lobby methods: List
public:
	void AddRequestLobbyListDistanceFilter(ELobbyDistanceFilter eLobbyDistanceFilter);
	void AddRequestLobbyListFilterSlotsAvailable(int nSlotsAvailable);
	void AddRequestLobbyListNearValueFilter(const char *pchKeyToMatch, int nValueToBeCloseTo);
	void AddRequestLobbyListNumericalFilter(const char *pchKeyToMatch, int nValueToMatch, ELobbyComparison eComparisonType);
	void AddRequestLobbyListResultCountFilter(int cMaxResults);
	void AddRequestLobbyListStringFilter(const char *pchKeyToMatch, const char *pchValueToMatch, ELobbyComparison eComparisonType);
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
	bool SetLobbyJoinable(CSteamID steamIDLobby, bool bLobbyJoinable);
	bool SetLobbyType(CSteamID steamIDLobby, ELobbyType eLobbyType);
	int JoinLobby(CSteamID steamIDLobby);
	bool InviteUserToLobby(CSteamID steamIDLobby, CSteamID steamIDInvitee);
	void LeaveLobby(CSteamID steamIDLobby);

	// Lobby methods: Game server
private:
	STEAM_CALLBACK(SteamPlugin, OnLobbyGameCreated, LobbyGameCreated_t, m_CallbackLobbyGameCreated);
	SETUP_CALLBACK_LIST(LobbyGameCreated, LobbyGameCreated_t);
public:
	bool GetLobbyGameServer(CSteamID steamIDLobby, uint32 *punGameServerIP, uint16 *punGameServerPort, CSteamID *psteamIDGameServer);
	void SetLobbyGameServer(CSteamID steamIDLobby, uint32 unGameServerIP, uint16 unGameServerPort, CSteamID steamIDGameServer); // Triggers a LobbyGameCreated_t callback.
	//uint32 GetPublicIP();

	// Lobby methods: Data
private:
	STEAM_CALLBACK(SteamPlugin, OnLobbyDataUpdate, LobbyDataUpdate_t, m_CallResultLobbyDataUpdate);
	SETUP_CALLBACK_LIST(LobbyDataUpdate, LobbyDataUpdate_t);
public:
	const char *GetLobbyData(CSteamID steamIDLobby, const char *pchKey);
	int GetLobbyDataCount(CSteamID steamIDLobby);
	bool GetLobbyDataByIndex(CSteamID steamIDLobby, int iLobbyData, char *pchKey, int cchKeyBufferSize, char *pchValue, int cchValueBufferSize);
	void SetLobbyData(CSteamID steamIDLobby, const char *pchKey, const char *pchValue);
	bool DeleteLobbyData(CSteamID steamIDLobby, const char *pchKey);
	bool RequestLobbyData(CSteamID steamIDLobby);
	const char *GetLobbyMemberData(CSteamID steamIDLobby, CSteamID steamIDUser, const char *pchKey);
	void SetLobbyMemberData(CSteamID steamIDLobby, const char *pchKey, const char *pchValue);

	// Lobby methods: Members and Status
private:
	STEAM_CALLBACK(SteamPlugin, OnLobbyChatUpdate, LobbyChatUpdate_t, m_CallbackLobbyChatUpdate);
	SETUP_CALLBACK_LIST(LobbyChatUpdate, LobbyChatUpdate_t);
public:
	CSteamID GetLobbyOwner(CSteamID steamIDLobby);
	bool SetLobbyOwner(CSteamID steamIDLobby, CSteamID steamIDNewOwner);
	int GetLobbyMemberLimit(CSteamID steamIDLobby);
	bool SetLobbyMemberLimit(CSteamID steamIDLobby, int cMaxMembers);
	int GetNumLobbyMembers(CSteamID steamIDLobby);
	CSteamID GetLobbyMemberByIndex(CSteamID steamIDLobby, int iMember);

	// Lobby methods: Chat messages
private:
	STEAM_CALLBACK(SteamPlugin, OnLobbyChatMessage, LobbyChatMsg_t, m_CallbackLobbyChatMessage);
	SETUP_CALLBACK_LIST(LobbyChatMessage, plugin::LobbyChatMsg_t);
public:
	bool SendLobbyChatMessage(CSteamID steamIDLobby, const char *pvMsgBody);
	// Lobby methods: Favorite games
	int AddFavoriteGame(AppId_t nAppID, uint32 nIP, uint16 nConnPort, uint16 nQueryPort, uint32 unFlags, uint32 rTime32LastPlayedOnServer);
	int GetFavoriteGameCount();
	bool GetFavoriteGame(int iGame, AppId_t *pnAppID, uint32 *pnIP, uint16 *pnConnPort, uint16 *pnQueryPort, uint32 *punFlags, uint32 *pRTime32LastPlayedOnServer);
	bool RemoveFavoriteGame(AppId_t nAppID, uint32 nIP, uint16 nConnPort, uint16 nQueryPort, uint32 unFlags);

	// Music methods
private:
	STEAM_CALLBACK(SteamPlugin, OnPlaybackStatusHasChanged, PlaybackStatusHasChanged_t, m_CallbackPlaybackStatusHasChanged);
	SETUP_CALLBACK_BOOL(MusicPlaybackStatusChanged);
	STEAM_CALLBACK(SteamPlugin, OnVolumeHasChanged, VolumeHasChanged_t, m_CallbackVolumeHasChanged);
	SETUP_CALLBACK_BOOL(MusicVolumeChanged);
public:
	bool IsMusicEnabled();
	bool IsMusicPlaying();
	AudioPlayback_Status GetMusicPlaybackStatus();
	float GetMusicVolume();
	void PauseMusic();
	void PlayMusic();
	void PlayNextSong();
	void PlayPreviousSong();
	void SetMusicVolume(float flVolume);

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
	uint8 GetCurrentBatteryPower();
	uint32 GetIPCCallCount();
	const char *GetIPCountry();
	uint32 GetSecondsSinceAppActive();
	uint32 GetSecondsSinceComputerActive();
	uint32 GetServerRealTime();
	const char *GetSteamUILanguage();
	bool IsOverlayEnabled();
	void SetOverlayNotificationInset(int nHorizontalInset, int nVerticalInset);
	void SetOverlayNotificationPosition(ENotificationPosition eNotificationPosition);
	uint8 GetMinutesBatteryLeft() { return m_nMinutesBatteryLeft; }
	void SetWarningMessageHook();

	// Big Picture methods
private:
	STEAM_CALLBACK(SteamPlugin, OnGamepadTextInputDismissed, GamepadTextInputDismissed_t, m_CallbackGamepadTextInputDismissed);
	SETUP_CALLBACK_LIST(GamepadTextInputDismissed, plugin::GamepadTextInputDismissed_t);
public:
	bool IsSteamInBigPictureMode();
	bool ShowGamepadTextInput(EGamepadTextInputMode eInputMode, EGamepadTextInputLineMode eLineInputMode, const char *pchDescription, uint32 unCharMax, const char *pchExistingText);

	// VR methods
public:
	bool IsSteamRunningInVR();
	void StartVRDashboard();
	void SetVRHeadsetStreamingEnabled(bool bEnabled);
	bool IsVRHeadsetStreamingEnabled();

	// Cloud methods : Information
public:
	bool IsCloudEnabledForAccount();
	bool IsCloudEnabledForApp();
	void SetCloudEnabledForApp(bool bEnabled);
	bool GetQuota(uint64 *pnTotalBytes, uint64 *puAvailableBytes);

	// Cloud Methods: Files
public:
	bool FileDelete(const char *pchFile);
	bool FileExists(const char *pchFile);
	bool FileForget(const char *pchFile);
	bool FilePersisted(const char *pchFile);
	int32 FileRead(const char *pchFile, void *pvData, int32 cubDataToRead);
	int FileReadAsync(const char *pchFile, uint32 nOffset, uint32 cubToRead);
	std::string GetFileReadAsyncFileName(int hCallResult);
	int GetFileReadAsyncMemblock(int hCallResult);
	//SteamAPICall_t FileShare(const char *pchFile);
	bool FileWrite(const char *pchFile, const void *pvData, int32 cubData);
	int FileWriteAsync(const char *pchFile, const void *pvData, uint32 cubData);
	std::string GetFileWriteAsyncFileName(int hCallResult);
	//bool FileWriteStreamCancel(UGCFileWriteStreamHandle_t writeHandle);
	//bool FileWriteStreamClose(UGCFileWriteStreamHandle_t writeHandle);
	//UGCFileWriteStreamHandle_t FileWriteStreamOpen(const char *pchFile);
	//bool FileWriteStreamWriteChunk(UGCFileWriteStreamHandle_t writeHandle, const void *pvData, int32 cubData);
	int32 GetFileCount();
	const char * GetFileNameAndSize(int iFile, int32 *pnFileSizeInBytes);
	int32 GetFileSize(const char *pchFile);
	int64 GetFileTimestamp(const char *pchFile);
	ERemoteStoragePlatform GetSyncPlatforms(const char *pchFile);
	bool SetSyncPlatforms(const char *pchFile, ERemoteStoragePlatform eRemoteStoragePlatform);

	// User-Generated Content
public:
	int32 GetCachedUGCCount();

	// Input Information
private:
	//int m_InputCount;
	//InputHandle_t m_InputHandles[STEAM_INPUT_MAX_COUNT];
public:
	bool InitSteamInput();
	int GetConnectedControllers(InputHandle_t *handlesOut);
	ESteamInputType GetInputTypeForHandle(InputHandle_t inputHandle);
	void RunFrame();
	bool ShowBindingPanel(InputHandle_t inputHandle);

	// Input Action Sets and Layers
public:
	void ActivateActionSet(InputHandle_t inputHandle, InputActionSetHandle_t actionSetHandle);
	InputActionSetHandle_t GetActionSetHandle(const char *pszActionSetName);
	InputActionSetHandle_t GetCurrentActionSet(InputHandle_t inputHandle);
	void ActivateActionSetLayer(InputHandle_t inputHandle, InputActionSetHandle_t hActionSetLayer);
	void DeactivateActionSetLayer(InputHandle_t inputHandle, InputActionSetHandle_t hActionSetLayer);
	void DeactivateAllActionSetLayers(InputHandle_t inputHandle);
	int GetActiveActionSetLayers(InputHandle_t inputHandle, InputActionSetHandle_t *handlesOut);

	// Input Actions and Motion
public:
	InputAnalogActionData_t m_AnalogActionData;
	void GetAnalogActionData(InputHandle_t inputHandle, InputAnalogActionHandle_t analogActionHandle);
	InputAnalogActionHandle_t GetAnalogActionHandle(const char *pszActionName);
	void StopAnalogActionMomentum(InputHandle_t inputHandle, InputAnalogActionHandle_t analogActionHandle);
	InputDigitalActionData_t m_DigitalActionData;
	void GetDigitalActionData(InputHandle_t inputHandle, InputDigitalActionHandle_t digitalActionHandle);
	InputDigitalActionHandle_t GetDigitalActionHandle(const char *pszActionName);
	InputMotionData_t m_MotionData;
	void GetMotionData(InputHandle_t inputHandle);

	// Input Action Origins.
public:
	int GetAnalogActionOrigins(InputHandle_t inputHandle, InputActionSetHandle_t actionSetHandle, InputAnalogActionHandle_t analogActionHandle, EInputActionOrigin *originsOut);
	int GetDigitalActionOrigins(InputHandle_t inputHandle, InputActionSetHandle_t actionSetHandle, InputDigitalActionHandle_t digitalActionHandle, EInputActionOrigin *originsOut);
	const char *GetGlyphForActionOrigin(EInputActionOrigin eOrigin);
	const char *GetStringForActionOrigin(EInputActionOrigin eOrigin);
	EInputActionOrigin GetActionOriginFromXboxOrigin(InputHandle_t inputHandle, EXboxOrigin eOrigin);
	const char *GetStringForXboxOrigin(EXboxOrigin eOrigin);
	const char *GetGlyphForXboxOrigin(EXboxOrigin eOrigin);
	EInputActionOrigin TranslateActionOrigin(ESteamInputType eDestinationInputType, EInputActionOrigin eSourceOrigin);

	// Input Effects
public:
	void SetLEDColor(InputHandle_t inputHandle, uint8 nColorR, uint8 nColorG, uint8 nColorB, unsigned int nFlags);
	void TriggerHapticPulse(InputHandle_t inputHandle, ESteamControllerPad eTargetPad, unsigned short duration);
	void TriggerRepeatedHapticPulse(InputHandle_t inputHandle, ESteamControllerPad eTargetPad, unsigned short onDuration, unsigned short offDuration, unsigned short repeat, unsigned int flags);
	void TriggerVibration(InputHandle_t inputHandle, unsigned short usLeftSpeed, unsigned short usRightSpeed);
};

#endif // _STEAMPLUGIN_H_
