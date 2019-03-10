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

#if defined(_WINDOWS)
#if defined(_WIN64)
#pragma comment(lib, "steam_api64.lib")
#else
#pragma comment(lib, "steam_api.lib")
#endif
#endif

#define MAX_GAMEPAD_TEXT_INPUT_LENGTH	512
#define MAX_CHAT_MESSAGE_LENGTH			4096

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

class SteamPlugin
{
private:
	// General methods.
	uint64 m_AppID;
	bool m_SteamInitialized;
	// CallResults
	std::map<int, CCallResultItem*> m_CallResultItems;
	int m_CurrentCallResultHandle;
	int AddCallResultItem(CCallResultItem *callResult);
	// Template methods MUST be defined in the header file!
public:
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
private:
	// App/DLC methods
	STEAM_CALLBACK(SteamPlugin, OnNewLaunchQueryParameters, NewUrlLaunchParameters_t, m_CallbackNewLaunchQueryParameters);
	bool m_HasNewLaunchQueryParameters;
	STEAM_CALLBACK(SteamPlugin, OnDlcInstalled, DlcInstalled_t, m_CallbackDlcInstalled);
	bool m_OnDlcInstalledEnabled;
	std::list<AppId_t> m_DlcInstalledList;
	std::mutex m_DlcInstalledMutex;
	AppId_t m_NewDlcInstalled;
	// Overlay methods
	STEAM_CALLBACK(SteamPlugin, OnGameOverlayActivated, GameOverlayActivated_t, m_CallbackGameOverlayActivated);
	bool m_IsGameOverlayActive;
	// User/Friend methods
	STEAM_CALLBACK(SteamPlugin, OnPersonaStateChanged, PersonaStateChange_t, m_CallbackPersonaStateChanged);
	STEAM_CALLBACK(SteamPlugin, OnAvatarImageLoaded, AvatarImageLoaded_t, m_CallbackAvatarImageLoaded);
	bool m_AvatarImageLoadedEnabled; // Added so games that don't load friend avatars don't waste memory storing things it never uses.
	std::list<CSteamID> m_AvatarImageLoadedUsers;
	CSteamID m_AvatarUser;
	bool m_PersonaStateChangedEnabled;
	std::list<PersonaStateChange_t> m_PersonaStateChangeList;
	PersonaStateChange_t m_PersonaStateChange;
	// Image methods
	// General user stats methods.
	STEAM_CALLBACK(SteamPlugin, OnUserStatsReceived, UserStatsReceived_t, m_CallbackUserStatsReceived);
	ECallbackState m_RequestStatsCallbackState;
	bool m_StatsInitialized;
	STEAM_CALLBACK(SteamPlugin, OnUserStatsStored, UserStatsStored_t, m_CallbackUserStatsStored);
	STEAM_CALLBACK(SteamPlugin, OnAchievementStored, UserAchievementStored_t, m_CallbackAchievementStored);
	ECallbackState m_StoreStatsCallbackState;
	bool m_StatsStored;
	bool m_AchievementStored;
	// Achievements methods.
	STEAM_CALLBACK(SteamPlugin, OnAchievementIconFetched, UserAchievementIconFetched_t, m_CallbackAchievementIconFetched);
	std::map<std::string, int> m_AchievementIconsMap;
	// User stats methods.
	// Leaderboard methods: Find
	// Leaderboard methods: Information
	// Leaderboard methods: Upload
	// Leaderboard methods: Download
	// Lobby methods: List
	// Lobby methods: Data
	STEAM_CALLBACK(SteamPlugin, OnLobbyDataUpdated, LobbyDataUpdate_t, m_CallResultLobbyDataUpdate);
	std::list<LobbyDataUpdate_t> m_LobbyDataUpdatedList;
	std::mutex m_LobbyDataUpdatedMutex;
	LobbyDataUpdate_t m_CurrentLobbyDataUpdate;
	void ClearCurrentLobbyDataUpdate()
	{
		m_CurrentLobbyDataUpdate.m_bSuccess = 0;
		m_CurrentLobbyDataUpdate.m_ulSteamIDLobby = 0;
		m_CurrentLobbyDataUpdate.m_ulSteamIDMember = 0;
	}
	// Lobby methods: Owner methods
	// Lobby methods: Create
	// Lobby methods: Join, Create, Leave
	STEAM_CALLBACK(SteamPlugin, OnLobbyEnter, LobbyEnter_t, m_MainCallResultLobbyEnter);
	std::vector<CSteamID> m_JoinedLobbies; // Keep track so we don't leave any left open when closing.
	std::list<LobbyEnter_t> m_LobbyEnterList; // For callback reporting.
	std::mutex m_LobbyEnterMutex;
	LobbyEnter_t m_CurrentLobbyEnter;
	void ClearCurrentLobbyEnter()
	{
		m_CurrentLobbyEnter.m_bLocked = 0;
		m_CurrentLobbyEnter.m_EChatRoomEnterResponse = 0;
		m_CurrentLobbyEnter.m_rgfChatPermissions = 0;
		m_CurrentLobbyEnter.m_ulSteamIDLobby = 0;
	}
	STEAM_CALLBACK(SteamPlugin, OnGameLobbyJoinRequested, GameLobbyJoinRequested_t, m_CallbackGameLobbyJoinRequested);
	GameLobbyJoinRequested_t m_GameLobbyJoinRequestedInfo;
	// Lobby methods: Game server
	STEAM_CALLBACK(SteamPlugin, OnLobbyGameCreated, LobbyGameCreated_t, m_CallbackLobbyGameCreated);
	LobbyGameCreated_t m_LobbyGameCreatedInfo;
	// Lobby methods: Members and Status
	STEAM_CALLBACK(SteamPlugin, OnLobbyChatUpdated, LobbyChatUpdate_t, m_CallbackLobbyChatUpdated);
	std::list<LobbyChatUpdate_t> m_LobbyChatUpdatedList;
	std::mutex m_LobbyChatUpdatedMutex;
	LobbyChatUpdate_t m_CurrentLobbyChatUpdate;
	void ClearCurrentLobbyChatUpdate()
	{
		m_CurrentLobbyChatUpdate.m_rgfChatMemberStateChange = (EChatMemberStateChange)0;
		m_CurrentLobbyChatUpdate.m_ulSteamIDLobby = 0;
		m_CurrentLobbyChatUpdate.m_ulSteamIDMakingChange = 0;
		m_CurrentLobbyChatUpdate.m_ulSteamIDUserChanged = 0;
	}
	// Lobby methods: Chat messages
	STEAM_CALLBACK(SteamPlugin, OnLobbyChatMessage, LobbyChatMsg_t, m_CallbackLobbyChatMessage);
	struct ChatMessageInfo_t
	{
		CSteamID user;
		char text[MAX_CHAT_MESSAGE_LENGTH];
	};
	std::list<ChatMessageInfo_t> m_LobbyChatMessageList;
	std::timed_mutex m_LobbyChatMessageMutex;
	ChatMessageInfo_t m_CurrentLobbyChatMessage;
	void ClearCurrentLobbyChatMessage()
	{
		m_CurrentLobbyChatMessage.user = k_steamIDNil;
		*m_CurrentLobbyChatMessage.text = NULL;
	}
	// Music methods
	STEAM_CALLBACK(SteamPlugin, OnPlaybackStatusHasChanged, PlaybackStatusHasChanged_t, m_CallbackPlaybackStatusHasChanged);
	bool m_PlaybackStatusHasChanged;
	STEAM_CALLBACK(SteamPlugin, OnVolumeHasChanged, VolumeHasChanged_t, m_CallbackVolumeHasChanged);
	bool m_VolumeHasChanged;
	// Util methods
	STEAM_CALLBACK(SteamPlugin, OnIPCountryChanged, IPCountry_t, m_CallbackIPCountryChanged);
	bool m_HasIPCountryChanged;
	STEAM_CALLBACK(SteamPlugin, OnLowBatteryPower, LowBatteryPower_t, m_CallbackLowBatteryPower);
	bool m_HasLowBatteryWarning;
	uint8 m_nMinutesBatteryLeft;
	STEAM_CALLBACK(SteamPlugin, OnSteamShutdown, SteamShutdown_t, m_CallbackSteamShutdown);
	bool m_IsSteamShuttingDown;
	// Big Picture methods
	STEAM_CALLBACK(SteamPlugin, OnGamepadTextInputDismissed, GamepadTextInputDismissed_t, m_CallbackGamepadTextInputDismissed);
	struct GamepadTextInputDismissedInfo_t
	{
		bool dismissed;
		bool submitted;
		char text[MAX_GAMEPAD_TEXT_INPUT_LENGTH];
	};
	GamepadTextInputDismissedInfo_t m_GamepadTextInputDismissedInfo;
	// Cloud method: FileWriteAsync
	// Cloud method: FileReadAsync
	// Input
	//int m_InputCount;
	//InputHandle_t m_InputHandles[STEAM_INPUT_MAX_COUNT];

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

public:
	SteamPlugin();
	virtual ~SteamPlugin(void);
	// General methods.
	bool Init();
	void Shutdown();
	bool SteamInitialized() { return m_SteamInitialized; }
	bool RestartAppIfNecessary(uint32 unOwnAppID)
	{
		return SteamAPI_RestartAppIfNecessary(unOwnAppID);
	}
	int GetAppID();
	int GetAppName(AppId_t nAppID, char *pchName, int cchNameMax)
	{
		CheckInitialized(SteamAppList, 0);
		return SteamAppList()->GetAppName(nAppID, pchName, cchNameMax);
	}
	bool LoggedOn();
	void RunCallbacks()
	{
		if (!m_SteamInitialized)
		{
			return;
		}
		SteamAPI_RunCallbacks();
	}
	void DeleteCallResultItem(int hCallResult);
	ECallbackState GetCallResultItemState(int hCallResult);
	// App/DLC methods
	bool GetDLCDataByIndex(int iDLC, AppId_t *pAppID, bool *pbAvailable, char *pchName, int cchNameBufferSize);
	bool IsAppInstalled(AppId_t appID);
	bool IsCybercafe();
	bool IsDlcInstalled(AppId_t appID);
	bool IsLowViolence();
	bool IsSubscribed();
	bool IsSubscribedApp(AppId_t appID);
	bool IsSubscribedFromFamilySharing();
	bool IsSubscribedFromFreeWeekend();
	bool IsVACBanned();
	int GetAppBuildId();
	uint32 GetAppInstallDir(AppId_t appID, char *pchFolder, uint32 cchFolderBufferSize);
	CSteamID GetAppOwner();
	const char *GetAvailableGameLanguages();
	bool GetCurrentBetaName(char *pchName, int cchNameBufferSize);
	const char *GetCurrentGameLanguage();
	int GetDLCCount();
	bool GetDlcDownloadProgress(AppId_t nAppID, uint64 *punBytesDownloaded, uint64 *punBytesTotal);
	uint32 GetEarliestPurchaseUnixTime(AppId_t nAppID);
	//SteamAPICall_t GetFileDetails(const char*pszFileName); // FileDetailsResult_t call result.
	uint32 GetInstalledDepots(AppId_t appID, DepotId_t *pvecDepots, uint32 cMaxDepots);
	const char *GetLaunchQueryParam(const char *pchKey);
	bool HasNewLaunchQueryParameters();
	int GetLaunchCommandLine(char *pchCommandLine, int cubCommandLine);
	bool HasNewDlcInstalled();
	AppId_t GetNewDlcInstalled() { return m_NewDlcInstalled; }
	void InstallDLC(AppId_t nAppID); // Triggers a DlcInstalled_t callback.
	bool MarkContentCorrupt(bool bMissingFilesOnly);
	void UninstallDLC(AppId_t nAppID);
	// Overlay methods
	bool IsGameOverlayActive() { return m_IsGameOverlayActive; }
	void ActivateGameOverlay(const char *pchDialog);
	void ActivateGameOverlayInviteDialog(CSteamID steamIDLobby);
	void ActivateGameOverlayToStore(AppId_t nAppID, EOverlayToStoreFlag eFlag);
	void ActivateGameOverlayToUser(const char *pchDialog, CSteamID steamID);
	void ActivateGameOverlayToWebPage(const char *pchURL, EActivateGameOverlayToWebPageMode eMode);
	// User/Friend methods
	const char *GetPersonaName();
	//EPersonaState GetPersonaState();
	//uint32 GetUserRestrictions();
	CSteamID GetSteamID();
	//steamUser->GetPlayerSteamLevel()
	//m_PersonaStateChange
	bool HasPersonaStateChanged();
	CSteamID GetPersonaStateChangedUser() { return m_PersonaStateChange.m_ulSteamID; }
	EPersonaChange GetPersonaStateChangedFlags() { return (EPersonaChange)m_PersonaStateChange.m_nChangeFlags; }
	bool RequestUserInformation(CSteamID steamIDUser, bool bRequireNameOnly);
	bool HasAvatarImageLoaded();
	CSteamID GetAvatarImageLoadedUser() { return m_AvatarUser; }
	int GetFriendAvatar(CSteamID steamID, EAvatarSize size);
	int GetFriendCount(EFriendFlags iFriendFlags);
	CSteamID GetFriendByIndex(int iFriend, EFriendFlags iFriendFlags);
	bool GetFriendGamePlayed(CSteamID steamIDFriend, FriendGameInfo_t *pFriendGameInfo);
	const char *GetFriendPersonaName(CSteamID steamID);
	EPersonaState GetFriendPersonaState(CSteamID steamIDFriend);
	EFriendRelationship GetFriendRelationship(CSteamID steamIDFriend);
	int GetFriendSteamLevel(CSteamID steamIDFriend);
	const char *GetPlayerNickname(CSteamID steamIDPlayer);
	bool HasFriend(CSteamID steamIDFriend, EFriendFlags iFriendFlags);
	//bool InviteUserToGame(CSteamID steamIDFriend, const char *pchConnectString);
	// Friend group methods
	int GetFriendsGroupCount();
	FriendsGroupID_t GetFriendsGroupIDByIndex(int iFriendGroup);
	int GetFriendsGroupMembersCount(FriendsGroupID_t friendsGroupID);
	void GetFriendsGroupMembersList(FriendsGroupID_t friendsGroupID, CSteamID *pOutSteamIDMembers, int nMembersCount);
	const char * GetFriendsGroupName(FriendsGroupID_t friendsGroupID);
	// Image methods
	int LoadImageFromHandle(int hImage);
	int LoadImageFromHandle(int imageID, int hImage);
	// General user stats methods.
	bool RequestStats();
	ECallbackState GetRequestStatsCallbackState() { return getCallbackState(&m_RequestStatsCallbackState); }
	bool StatsInitialized() { return m_StatsInitialized; }
	bool StoreStats();
	bool ResetAllStats(bool bAchievementsToo);
	ECallbackState GetStoreStatsCallbackState() { return getCallbackState(&m_StoreStatsCallbackState); }
	bool StatsStored() { return m_StatsStored; }
	bool AchievementStored() { return m_AchievementStored; }
	// Achievements methods.
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
	bool GetStat(const char *pchName, int32 *pData);
	bool GetStat(const char *pchName, float *pData);
	bool SetStat(const char *pchName, int32 nData);
	bool SetStat(const char *pchName, float fData);
	bool UpdateAvgRateStat(const char *pchName, float flCountThisSession, double dSessionLength);
	// Leaderboards
	int FindLeaderboard(const char *pchLeaderboardName);
	SteamLeaderboard_t GetLeaderboardHandle(int hCallResult);
	// General information
	const char *GetLeaderboardName(SteamLeaderboard_t hLeaderboard);
	int GetLeaderboardEntryCount(SteamLeaderboard_t hLeaderboard);
	ELeaderboardDisplayType GetLeaderboardDisplayType(SteamLeaderboard_t hLeaderboard);
	ELeaderboardSortMethod GetLeaderboardSortMethod(SteamLeaderboard_t hLeaderboard);
	// Uploading scores
	int UploadLeaderboardScore(SteamLeaderboard_t hLeaderboard, ELeaderboardUploadScoreMethod eLeaderboardUploadScoreMethod, int score);
	bool LeaderboardScoreStored(int hCallResult);
	bool LeaderboardScoreChanged(int hCallResult);
	int GetLeaderboardUploadedScore(int hCallResult);
	int GetLeaderboardGlobalRankNew(int hCallResult);
	int GetLeaderboardGlobalRankPrevious(int hCallResult);
	// Downloading entries.
	int DownloadLeaderboardEntries(SteamLeaderboard_t hLeaderboard, ELeaderboardDataRequest eLeaderboardDataRequest, int nRangeStart, int nRangeEnd);
	int GetDownloadedLeaderboardEntryCount(int hCallResult);
	int GetDownloadedLeaderboardEntryGlobalRank(int hCallResult, int index);
	int GetDownloadedLeaderboardEntryScore(int hCallResult, int index);
	CSteamID GetDownloadedLeaderboardEntryUser(int hCallResult, int index);
	// Lobby methods: List
	void AddRequestLobbyListDistanceFilter(ELobbyDistanceFilter eLobbyDistanceFilter);
	void AddRequestLobbyListFilterSlotsAvailable(int nSlotsAvailable);
	void AddRequestLobbyListNearValueFilter(const char *pchKeyToMatch, int nValueToBeCloseTo);
	void AddRequestLobbyListNumericalFilter(const char *pchKeyToMatch, int nValueToMatch, ELobbyComparison eComparisonType);
	void AddRequestLobbyListResultCountFilter(int cMaxResults);
	void AddRequestLobbyListStringFilter(const char *pchKeyToMatch, const char *pchValueToMatch, ELobbyComparison eComparisonType);
	int RequestLobbyList();
	// Lobby methods: Create, Join, Leave
	int CreateLobby(ELobbyType eLobbyType, int cMaxMembers);
	//EResult GetLobbyCreatedResult(int hCallResult);
	//bool SetLinkedLobby(CSteamID steamIDLobby, CSteamID steamIDLobbyDependent);
	bool SetLobbyJoinable(CSteamID steamIDLobby, bool bLobbyJoinable);
	bool SetLobbyType(CSteamID steamIDLobby, ELobbyType eLobbyType);
	int JoinLobby(CSteamID steamIDLobby);
	bool HasLobbyEnterResponse();
	LobbyEnter_t GetLobbyEnterResponse() { return m_CurrentLobbyEnter; }
	bool InviteUserToLobby(CSteamID steamIDLobby, CSteamID steamIDInvitee);
	bool HasGameLobbyJoinRequest() { return m_GameLobbyJoinRequestedInfo.m_steamIDLobby != k_steamIDNil; }
	CSteamID GetGameLobbyJoinRequestedLobby();
	void LeaveLobby(CSteamID steamIDLobby);
	// Lobby methods: Game server
	bool GetLobbyGameServer(CSteamID steamIDLobby, uint32 *punGameServerIP, uint16 *punGameServerPort, CSteamID *psteamIDGameServer);
	void SetLobbyGameServer(CSteamID steamIDLobby, uint32 unGameServerIP, uint16 unGameServerPort, CSteamID steamIDGameServer); // Triggers a LobbyGameCreated_t callback.
	bool HasLobbyGameCreated() { return m_LobbyGameCreatedInfo.m_ulSteamIDLobby != 0; };
	LobbyGameCreated_t GetLobbyGameCreated();
	// Lobby methods: Data
	const char *GetLobbyData(CSteamID steamIDLobby, const char *pchKey);
	int GetLobbyDataCount(CSteamID steamIDLobby);
	bool GetLobbyDataByIndex(CSteamID steamIDLobby, int iLobbyData, char *pchKey, int cchKeyBufferSize, char *pchValue, int cchValueBufferSize);
	void SetLobbyData(CSteamID steamIDLobby, const char *pchKey, const char *pchValue);
	bool DeleteLobbyData(CSteamID steamIDLobby, const char *pchKey);
	bool RequestLobbyData(CSteamID steamIDLobby);
	bool HasLobbyDataUpdateResponse();
	LobbyDataUpdate_t GetLobbyDataUpdateResponse() { return m_CurrentLobbyDataUpdate; }
	const char *GetLobbyMemberData(CSteamID steamIDLobby, CSteamID steamIDUser, const char *pchKey);
	void SetLobbyMemberData(CSteamID steamIDLobby, const char *pchKey, const char *pchValue);
	// Lobby methods: members and status
	CSteamID GetLobbyOwner(CSteamID steamIDLobby);
	bool SetLobbyOwner(CSteamID steamIDLobby, CSteamID steamIDNewOwner);
	int GetLobbyMemberLimit(CSteamID steamIDLobby);
	bool SetLobbyMemberLimit(CSteamID steamIDLobby, int cMaxMembers);
	int GetNumLobbyMembers(CSteamID steamIDLobby);
	CSteamID GetLobbyMemberByIndex(CSteamID steamIDLobby, int iMember);
	bool HasLobbyChatUpdate();
	CSteamID GetLobbyChatUpdateLobby() { return m_CurrentLobbyChatUpdate.m_ulSteamIDLobby; }
	CSteamID GetLobbyChatUpdateUserChanged() { return m_CurrentLobbyChatUpdate.m_ulSteamIDUserChanged; }
	EChatMemberStateChange GetLobbyChatUpdateUserState() { return (EChatMemberStateChange)m_CurrentLobbyChatUpdate.m_rgfChatMemberStateChange; }
	CSteamID GetLobbyChatUpdateUserMakingChange() { return m_CurrentLobbyChatUpdate.m_ulSteamIDMakingChange; }
	// Lobby methods: Chat messages
	bool HasLobbyChatMessage();
	CSteamID GetLobbyChatMessageUser() { return m_CurrentLobbyChatMessage.user; }
	void GetLobbyChatMessageText(char *msg) { strcpy_s(msg, MAX_CHAT_MESSAGE_LENGTH, m_CurrentLobbyChatMessage.text); }
	bool SendLobbyChatMessage(CSteamID steamIDLobby, const char *pvMsgBody);
	// Lobby methods: Favorite games
	int AddFavoriteGame(AppId_t nAppID, uint32 nIP, uint16 nConnPort, uint16 nQueryPort, uint32 unFlags, uint32 rTime32LastPlayedOnServer);
	int GetFavoriteGameCount();
	bool GetFavoriteGame(int iGame, AppId_t *pnAppID, uint32 *pnIP, uint16 *pnConnPort, uint16 *pnQueryPort, uint32 *punFlags, uint32 *pRTime32LastPlayedOnServer);
	bool RemoveFavoriteGame(AppId_t nAppID, uint32 nIP, uint16 nConnPort, uint16 nQueryPort, uint32 unFlags);
	// Game server
	//uint32 GetPublicIP();
	// Music methods
	bool IsMusicEnabled();
	bool IsMusicPlaying();
	AudioPlayback_Status GetMusicPlaybackStatus();
	float GetMusicVolume();
	void PauseMusic();
	void PlayMusic();
	void PlayNextSong();
	void PlayPreviousSong();
	void SetMusicVolume(float flVolume);
	bool HasMusicPlaybackStatusChanged();
	bool HasMusicVolumeChanged();
	// Utils
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
	bool HasIPCountryChanged();
	bool HasLowBatteryWarning();
	uint8 GetMinutesBatteryLeft() { return m_nMinutesBatteryLeft; }
	bool IsSteamShuttingDown();
	void SetWarningMessageHook();
	// Big Picture methods
	bool IsSteamInBigPictureMode();
	bool HasGamepadTextInputDismissedInfo();
	void GetGamepadTextInputDismissedInfo(bool *pbSubmitted, char *pchText);
	bool ShowGamepadTextInput(EGamepadTextInputMode eInputMode, EGamepadTextInputLineMode eLineInputMode, const char *pchDescription, uint32 unCharMax, const char *pchExistingText);
	// VR methods
	bool IsSteamRunningInVR();
	void StartVRDashboard();
	void SetVRHeadsetStreamingEnabled(bool bEnabled);
	bool IsVRHeadsetStreamingEnabled();
	// Cloud methods : Information
	bool IsCloudEnabledForAccount();
	bool IsCloudEnabledForApp();
	void SetCloudEnabledForApp(bool bEnabled);
	// Cloud Methods: Files
	bool FileDelete(const char *pchFile);
	bool FileExists(const char *pchFile);
	bool FileForget(const char *pchFile);
	bool FilePersisted(const char *pchFile);
	int32 FileRead(const char *pchFile, void *pvData, int32 cubDataToRead);
	int FileReadAsync(const char *pchFile, uint32 nOffset, uint32 cubToRead);
	//EResult GetFileReadAsyncResult(int hCallResult);
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
	bool GetQuota(uint64 *pnTotalBytes, uint64 *puAvailableBytes);
	ERemoteStoragePlatform GetSyncPlatforms(const char *pchFile);
	bool SetSyncPlatforms(const char *pchFile, ERemoteStoragePlatform eRemoteStoragePlatform);
	// User-Generated Content
	int32 GetCachedUGCCount();
	//UGCHandle_t GetCachedUGCHandle(int32 iCachedContent);
	//bool GetUGCDetails(UGCHandle_t hContent, AppId_t *pnAppID, char **ppchName, int32 *pnFileSizeInBytes, CSteamID *pSteamIDOwner);
	//bool GetUGCDownloadProgress(UGCHandle_t hContent, int32 *pnBytesDownloaded, int32 *pnBytesExpected);
	//SteamAPICall_t UGCDownload(UGCHandle_t hContent, uint32 unPriority);
	//SteamAPICall_t UGCDownloadToLocation(UGCHandle_t hContent, const char *pchLocation, uint32 unPriority);
	//int32 UGCRead(UGCHandle_t hContent, void *pvData, int32 cubDataToRead, uint32 cOffset, EUGCReadAction eAction);
	/*
	Input Information
	*/
	bool InitSteamInput();
	bool ShutdownSteamInput();
	int GetConnectedControllers(InputHandle_t *handlesOut);
	ESteamInputType GetInputTypeForHandle(InputHandle_t inputHandle);
	void RunFrame();
	bool ShowBindingPanel(InputHandle_t inputHandle);
	/*
	Input Action Sets and Layers
	*/
	void ActivateActionSet(InputHandle_t inputHandle, InputActionSetHandle_t actionSetHandle);
	InputActionSetHandle_t GetActionSetHandle(const char *pszActionSetName);
	InputActionSetHandle_t GetCurrentActionSet(InputHandle_t inputHandle);
	void ActivateActionSetLayer(InputHandle_t inputHandle, InputActionSetHandle_t hActionSetLayer);
	void DeactivateActionSetLayer(InputHandle_t inputHandle, InputActionSetHandle_t hActionSetLayer);
	void DeactivateAllActionSetLayers(InputHandle_t inputHandle);
	int GetActiveActionSetLayers(InputHandle_t inputHandle, InputActionSetHandle_t *handlesOut);
	/*
	Input Actions and Motion
	*/
	InputAnalogActionData_t m_AnalogActionData;
	void GetAnalogActionData(InputHandle_t inputHandle, InputAnalogActionHandle_t analogActionHandle);
	InputAnalogActionHandle_t GetAnalogActionHandle(const char *pszActionName);
	void StopAnalogActionMomentum(InputHandle_t inputHandle, InputAnalogActionHandle_t analogActionHandle);
	InputDigitalActionData_t m_DigitalActionData;
	void GetDigitalActionData(InputHandle_t inputHandle, InputDigitalActionHandle_t digitalActionHandle);
	InputDigitalActionHandle_t GetDigitalActionHandle(const char *pszActionName);
	InputMotionData_t m_MotionData;
	void GetMotionData(InputHandle_t inputHandle);
	void ClearInputData()
	{
		m_AnalogActionData.bActive = false;
		m_AnalogActionData.eMode = k_EInputSourceMode_None;
		m_AnalogActionData.x = 0;
		m_AnalogActionData.y = 0;
		m_DigitalActionData.bActive = false;
		m_DigitalActionData.bState = false;
		m_MotionData.posAccelX = 0;
		m_MotionData.posAccelY = 0;
		m_MotionData.posAccelZ = 0;
		m_MotionData.rotQuatW = 0;
		m_MotionData.rotQuatX = 0;
		m_MotionData.rotQuatY = 0;
		m_MotionData.rotQuatZ = 0;
		m_MotionData.rotVelX = 0;
		m_MotionData.rotVelY = 0;
		m_MotionData.rotVelZ = 0;
	}
	/*
	Input Action Origins.
	*/
	int GetAnalogActionOrigins(InputHandle_t inputHandle, InputActionSetHandle_t actionSetHandle, InputAnalogActionHandle_t analogActionHandle, EInputActionOrigin *originsOut);
	int GetDigitalActionOrigins(InputHandle_t inputHandle, InputActionSetHandle_t actionSetHandle, InputDigitalActionHandle_t digitalActionHandle, EInputActionOrigin *originsOut);
	const char *GetGlyphForActionOrigin(EInputActionOrigin eOrigin);
	const char *GetStringForActionOrigin(EInputActionOrigin eOrigin);
	EInputActionOrigin GetActionOriginFromXboxOrigin(InputHandle_t inputHandle, EXboxOrigin eOrigin);
	const char *GetStringForXboxOrigin(EXboxOrigin eOrigin);
	const char *GetGlyphForXboxOrigin(EXboxOrigin eOrigin);
	EInputActionOrigin TranslateActionOrigin(ESteamInputType eDestinationInputType, EInputActionOrigin eSourceOrigin);
	/*
	Input Effects
	*/
	void SetLEDColor(InputHandle_t inputHandle, uint8 nColorR, uint8 nColorG, uint8 nColorB, unsigned int nFlags);
	void TriggerHapticPulse(InputHandle_t inputHandle, ESteamControllerPad eTargetPad, unsigned short duration);
	void TriggerRepeatedHapticPulse(InputHandle_t inputHandle, ESteamControllerPad eTargetPad, unsigned short onDuration, unsigned short offDuration, unsigned short repeat, unsigned int flags);
	void TriggerVibration(InputHandle_t inputHandle, unsigned short usLeftSpeed, unsigned short usRightSpeed);
};

#endif // _STEAMPLUGIN_H_
