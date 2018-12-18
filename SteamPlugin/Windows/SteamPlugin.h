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

#ifndef STEAMPLUGIN_H_
#define STEAMPLUGIN_H_

#include <steam_api.h>
#include <map>
#include <list>
#include <vector>
//#include <thread>
#include <mutex>
#include "DllMain.h"
#include "CFileWriteAsyncItem.h"
#include "CFileReadAsyncItem.h"

#define MAX_LEADERBOARD_ENTRIES 10
#define MAX_GAMEPAD_TEXT_INPUT_LENGTH	512

enum EAvatarSize
{
	Small = 0,
	Medium = 1,
	Large = 2
};

class SteamPlugin
{
private:
	// General methods.
	uint64 m_AppID;
	bool m_SteamInitialized;
	// App/DLC methods
	bool m_HasNewLaunchQueryParameters;
	STEAM_CALLBACK(SteamPlugin, OnNewLaunchQueryParameters, NewLaunchQueryParameters_t, m_CallbackNewLaunchQueryParameters);
	bool m_OnDlcInstalledEnabled;
	std::list<AppId_t> m_DlcInstalledList;
	AppId_t m_NewDlcInstalled;
	STEAM_CALLBACK(SteamPlugin, OnDlcInstalled, DlcInstalled_t, m_CallbackDlcInstalled);
	// Overlay methods
	bool m_IsGameOverlayActive;
	STEAM_CALLBACK(SteamPlugin, OnGameOverlayActivated, GameOverlayActivated_t, m_CallbackGameOverlayActivated);
	// User/Friend methods
	bool m_AvatarImageLoadedEnabled; // Added so games that don't load friend avatars don't waste memory storing things it never uses.
	std::list<CSteamID> m_AvatarImageLoadedUsers;
	CSteamID m_AvatarUser;
	bool m_PersonaStateChangedEnabled;
	std::list<PersonaStateChange_t> m_PersonaStateChangeList;
	PersonaStateChange_t m_PersonaStateChange;
	STEAM_CALLBACK(SteamPlugin, OnPersonaStateChanged, PersonaStateChange_t, m_CallbackPersonaStateChanged);
	STEAM_CALLBACK(SteamPlugin, OnAvatarImageLoaded, AvatarImageLoaded_t, m_CallbackAvatarImageLoaded);
	// Image methods
	// General user stats methods.
	ECallbackState m_RequestStatsCallbackState;
	STEAM_CALLBACK(SteamPlugin, OnUserStatsReceived, UserStatsReceived_t, m_CallbackUserStatsReceived);
	bool m_StatsInitialized;
	ECallbackState m_StoreStatsCallbackState;
	STEAM_CALLBACK(SteamPlugin, OnUserStatsStored, UserStatsStored_t, m_CallbackUserStatsStored);
	STEAM_CALLBACK(SteamPlugin, OnAchievementStored, UserAchievementStored_t, m_CallbackAchievementStored);
	bool m_StatsStored;
	bool m_AchievementStored;
	// Achievements methods.
	STEAM_CALLBACK(SteamPlugin, OnAchievementIconFetched, UserAchievementIconFetched_t, m_CallbackAchievementIconFetched);
	std::map<std::string, int> m_AchievementIconsMap;
	// User stats methods.
	// Leaderboard methods: Find
	ECallbackState m_FindLeaderboardCallbackState;
	void OnFindLeaderboard(LeaderboardFindResult_t *pResult, bool bIOFailure);
	CCallResult<SteamPlugin, LeaderboardFindResult_t> m_CallResultFindLeaderboard;
	LeaderboardFindResult_t m_LeaderboardFindResult;
	// Leaderboard methods: Information
	// Leaderboard methods: Upload
	ECallbackState m_UploadLeaderboardScoreCallbackState;
	void OnUploadScore(LeaderboardScoreUploaded_t *pResult, bool bIOFailure);
	CCallResult<SteamPlugin, LeaderboardScoreUploaded_t> m_CallResultUploadScore;
	LeaderboardScoreUploaded_t m_LeaderboardScoreUploaded;
	// Leaderboard methods: Download
	ECallbackState m_DownloadLeaderboardEntriesCallbackState;
	void OnDownloadScore(LeaderboardScoresDownloaded_t *pResult, bool bIOFailure);
	CCallResult<SteamPlugin, LeaderboardScoresDownloaded_t> m_CallResultDownloadScore;
	int m_DownloadedLeaderboardEntryCount;
	LeaderboardEntry_t m_DownloadedLeaderboardEntries[MAX_LEADERBOARD_ENTRIES];
	// Lobby methods: List
	ECallbackState m_LobbyMatchListCallbackState;
	void OnLobbyMatchList(LobbyMatchList_t *pLobbyMatchList, bool bIOFailure);
	CCallResult<SteamPlugin, LobbyMatchList_t> m_CallResultLobbyMatchList;
	LobbyMatchList_t m_LobbyMatchList;
	// Lobby methods: Data
	STEAM_CALLBACK(SteamPlugin, OnLobbyDataUpdated, LobbyDataUpdate_t, m_CallResultLobbyDataUpdate);
	// TODO Replace with LobbyDataUpdateInfo_t variable.
	CSteamID m_LobbyDataUpdatedLobby;
	CSteamID m_LobbyDataUpdatedID;
	struct LobbyDataUpdateInfo_t
	{
		CSteamID lobby;
		CSteamID changedID;
	};
	std::list<LobbyDataUpdateInfo_t> m_LobbyDataUpdated;
	// Lobby methods: Owner methods
	// Lobby methods: Create
	ECallbackState m_LobbyCreateCallbackState;
	void OnLobbyCreated(LobbyCreated_t *pParam, bool bIOFailure);
	CCallResult<SteamPlugin, LobbyCreated_t> m_CallResultLobbyCreate;
	CSteamID m_LobbyCreatedID;
	EResult m_LobbyCreatedResult;
	// Lobby methods: Join, Create, Leave
	std::vector<CSteamID> m_JoinedLobbies; // Keep track so we don't leave any left open when closing.
	ECallbackState m_LobbyEnterCallbackState;
	void OnLobbyEnter(LobbyEnter_t *pParam, bool bIOFailure);
	CCallResult<SteamPlugin, LobbyEnter_t> m_CallResultLobbyEnter;
	STEAM_CALLBACK(SteamPlugin, OnLobbyEnter, LobbyEnter_t, m_MainCallResultLobbyEnter);
	CSteamID m_LobbyEnterID;
	bool m_LobbyEnterBlocked;
	EChatRoomEnterResponse m_LobbyEnterResponse;
	STEAM_CALLBACK(SteamPlugin, OnGameLobbyJoinRequested, GameLobbyJoinRequested_t, m_CallbackGameLobbyJoinRequested);
	GameLobbyJoinRequested_t m_GameLobbyJoinRequestedInfo;
	// Lobby methods: Game server
	STEAM_CALLBACK(SteamPlugin, OnLobbyGameCreated, LobbyGameCreated_t, m_CallbackLobbyGameCreated);
	LobbyGameCreated_t m_LobbyGameCreatedInfo;
	// Lobby methods: Members and Status
	STEAM_CALLBACK(SteamPlugin, OnLobbyChatUpdated, LobbyChatUpdate_t, m_CallbackLobbyChatUpdated);
	// TODO Replace with ChatUpdateInfo_t variable.
	CSteamID m_LobbyChatUpdateUserChanged;
	EChatMemberStateChange m_LobbyChatUpdateUserState;
	CSteamID m_LobbyChatUpdateUserMakingChange;
	struct ChatUpdateInfo_t
	{
		CSteamID userChanged;
		EChatMemberStateChange userState;
		CSteamID userMakingChange;
	};
	std::list<ChatUpdateInfo_t> m_ChatUpdates;
	// Lobby methods: Chat messages
	STEAM_CALLBACK(SteamPlugin, OnLobbyChatMessage, LobbyChatMsg_t, m_CallbackLobbyChatMessage);
	// TODO Replace with ChatMessageInfo_t variable.
	CSteamID m_LobbyChatMessageUser; // Holds next chat message user after HasLobbyChatMessage call.
	char m_LobbyChatMessageText[4096];
	struct ChatMessageInfo_t
	{
		CSteamID user;
		char text[4096];
	};
	std::list<ChatMessageInfo_t> m_ChatMessages;
	// Music methods
	STEAM_CALLBACK(SteamPlugin, OnPlaybackStatusHasChanged, PlaybackStatusHasChanged_t, m_CallbackPlaybackStatusHasChanged);
	bool m_PlaybackStatusHasChanged;
	STEAM_CALLBACK(SteamPlugin, OnVolumeHasChanged, VolumeHasChanged_t, m_CallbackVolumeHasChanged);
	bool m_VolumeHasChanged;
	// Util methods
	bool m_HasIPCountryChanged;
	STEAM_CALLBACK(SteamPlugin, OnIPCountryChanged, IPCountry_t, m_CallbackIPCountryChanged);
	bool m_HasLowBatteryWarning;
	uint8 m_nMinutesBatteryLeft;
	STEAM_CALLBACK(SteamPlugin, OnLowBatteryPower, LowBatteryPower_t, m_CallbackLowBatteryPower);
	bool m_IsSteamShuttingDown;
	STEAM_CALLBACK(SteamPlugin, OnSteamShutdown, SteamShutdown_t, m_CallbackSteamShutdown);
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
	CFileWriteAsyncItem *GetFileWriteAsyncItem(std::string filename);
	std::map<std::string, CFileWriteAsyncItem*> m_FileWriteAsyncItemMap;
	// Cloud method: FileReadAsync
	CFileReadAsyncItem *GetFileReadAsyncItem(std::string filename);
	std::map<std::string, CFileReadAsyncItem*> m_FileReadAsyncItemMap;
	// Controller
	//int m_ControllerCount;
	//ControllerHandle_t m_ControllerHandles[STEAM_CONTROLLER_MAX_COUNT];

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
	// mutex
	std::mutex lobbyDataUpdateMutex;

public:
	SteamPlugin();
	virtual ~SteamPlugin(void);
	// General methods.
	bool Init();
	void Shutdown();
	bool SteamInitialized() { return m_SteamInitialized; }
	bool RestartAppIfNecessary(uint32 unOwnAppID);
	int GetAppID();
	int GetAppName(AppId_t nAppID, char *pchName, int cchNameMax);
	bool LoggedOn();
	void RunCallbacks();
	// App/DLC methods
	bool GetDLCDataByIndex(int iDLC, AppId_t *pAppID, bool *pbAvailable, char *pchName, int cchNameBufferSize);
	bool IsAppInstalled(AppId_t appID);
	bool IsCybercafe();
	bool IsDlcInstalled(AppId_t appID);
	bool IsLowViolence();
	bool IsSubscribed();
	bool IsSubscribedApp(AppId_t appID);
	bool IsSubscribedFromFreeWeekend();
	bool IsVACBanned();
	int GetAppBuildId();
	uint32 GetAppInstallDir(AppId_t appID, char *pchFolder, uint32 cchFolderBufferSize);
	CSteamID GetAppOwner();
	const char * GetAvailableGameLanguages();
	bool GetCurrentBetaName(char *pchName, int cchNameBufferSize);
	const char * GetCurrentGameLanguage();
	int GetDLCCount();
	bool GetDlcDownloadProgress(AppId_t nAppID, uint64 *punBytesDownloaded, uint64 *punBytesTotal);
	uint32 GetEarliestPurchaseUnixTime(AppId_t nAppID);
	//SteamAPICall_t GetFileDetails(const char*pszFileName); // FileDetailsResult_t call result.
	uint32 GetInstalledDepots(AppId_t appID, DepotId_t *pvecDepots, uint32 cMaxDepots);
	const char * GetLaunchQueryParam(const char *pchKey);
	bool HasNewLaunchQueryParameters();
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
	void ActivateGameOverlayToWebPage(const char *pchURL);
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
	bool FindLeaderboard(const char *pchLeaderboardName);
	ECallbackState GetFindLeaderboardCallbackState() { return getCallbackState(&m_FindLeaderboardCallbackState); }
	SteamLeaderboard_t GetLeaderboardHandle() { return m_LeaderboardFindResult.m_hSteamLeaderboard; }
	// General information
	const char *GetLeaderboardName(SteamLeaderboard_t hLeaderboard);
	int GetLeaderboardEntryCount(SteamLeaderboard_t hLeaderboard);
	ELeaderboardDisplayType GetLeaderboardDisplayType(SteamLeaderboard_t hLeaderboard);
	ELeaderboardSortMethod GetLeaderboardSortMethod(SteamLeaderboard_t hLeaderboard);
	// Uploading scores
	bool UploadLeaderboardScore(SteamLeaderboard_t hLeaderboard, ELeaderboardUploadScoreMethod eLeaderboardUploadScoreMethod, int score);
	ECallbackState GetUploadLeaderboardScoreCallbackState() { return getCallbackState(&m_UploadLeaderboardScoreCallbackState); }
	bool LeaderboardScoreStored() { return m_LeaderboardScoreUploaded.m_bSuccess != 0; }
	bool LeaderboardScoreChanged() { return m_LeaderboardScoreUploaded.m_bScoreChanged != 0; }
	int GetLeaderboardUploadedScore() { return m_LeaderboardScoreUploaded.m_nScore; }
	int GetLeaderboardGlobalRankNew() { return m_LeaderboardScoreUploaded.m_nGlobalRankNew; }
	int GetLeaderboardGlobalRankPrevious() { return m_LeaderboardScoreUploaded.m_nGlobalRankPrevious; }
	// Downloading entries.
	bool DownloadLeaderboardEntries(SteamLeaderboard_t hLeaderboard, ELeaderboardDataRequest eLeaderboardDataRequest, int nRangeStart, int nRangeEnd);
	ECallbackState GetDownloadLeaderboardEntriesCallbackState() { return getCallbackState(&m_DownloadLeaderboardEntriesCallbackState); }
	int GetDownloadedLeaderboardEntryCount() { return m_DownloadedLeaderboardEntryCount; }
	int GetDownloadedLeaderboardEntryGlobalRank(int index);
	int GetDownloadedLeaderboardEntryScore(int index);
	CSteamID GetDownloadedLeaderboardEntryUser(int index);
	// Lobby methods: List
	void AddRequestLobbyListDistanceFilter(ELobbyDistanceFilter eLobbyDistanceFilter);
	void AddRequestLobbyListFilterSlotsAvailable(int nSlotsAvailable);
	void AddRequestLobbyListNearValueFilter(const char *pchKeyToMatch, int nValueToBeCloseTo);
	void AddRequestLobbyListNumericalFilter(const char *pchKeyToMatch, int nValueToMatch, ELobbyComparison eComparisonType);
	void AddRequestLobbyListResultCountFilter(int cMaxResults);
	void AddRequestLobbyListStringFilter(const char *pchKeyToMatch, const char *pchValueToMatch, ELobbyComparison eComparisonType);
	bool RequestLobbyList();
	CSteamID GetLobbyByIndex(int iLobby);
	ECallbackState GetLobbyMatchListCallbackState() { return getCallbackState(&m_LobbyMatchListCallbackState); }
	int GetLobbyMatchListCount() { return m_LobbyMatchList.m_nLobbiesMatching; }
	// Lobby methods: Create, Join, Leave
	bool CreateLobby(ELobbyType eLobbyType, int cMaxMembers);
	ECallbackState GetLobbyCreateCallbackState() { return getCallbackState(&m_LobbyCreateCallbackState); }
	CSteamID GetLobbyCreatedID() { return m_LobbyCreatedID; }
	EResult GetLobbyCreatedResult() { return m_LobbyCreatedResult; }
	//bool SetLinkedLobby(CSteamID steamIDLobby, CSteamID steamIDLobbyDependent);
	bool SetLobbyJoinable(CSteamID steamIDLobby, bool bLobbyJoinable);
	bool SetLobbyType(CSteamID steamIDLobby, ELobbyType eLobbyType);
	bool JoinLobby(CSteamID steamIDLobby);
	ECallbackState GetLobbyEnterCallbackState() { return getCallbackState(&m_LobbyEnterCallbackState); }
	CSteamID GetLobbyEnterID() { return m_LobbyEnterID; }
	bool GetLobbyEnterBlocked() { return m_LobbyEnterBlocked; }
	uint32 GetLobbyEnterResponse() { return m_LobbyEnterResponse; }
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
	bool HasLobbyDataUpdated();
	CSteamID GetLobbyDataUpdatedLobby() { return m_LobbyDataUpdatedLobby; }
	CSteamID GetLobbyDataUpdatedID() { return m_LobbyDataUpdatedID; }
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
	CSteamID GetLobbyChatUpdateUserChanged() { return m_LobbyChatUpdateUserChanged; }
	EChatMemberStateChange GetLobbyChatUpdateUserState() { return m_LobbyChatUpdateUserState; }
	CSteamID GetLobbyChatUpdateUserMakingChange() { return m_LobbyChatUpdateUserMakingChange; }
	// Lobby methods: Chat messages
	bool HasLobbyChatMessage();
	CSteamID GetLobbyChatMessageUser() { return m_LobbyChatMessageUser; }
	void GetLobbyChatMessageText(char *msg) { strcpy_s(msg, strlen(m_LobbyChatMessageText) + 1, m_LobbyChatMessageText); }
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
	bool FileReadAsync(const char *pchFile, uint32 nOffset, uint32 cubToRead);
	ECallbackState GetFileReadAsyncCallbackState(const char *pchFile);
	EResult GetFileReadAsyncResult(const char *pchFile);
	int GetFileReadAsyncMemblock(const char *pchFile);
	//SteamAPICall_t FileShare(const char *pchFile);
	bool FileWrite(const char *pchFile, const void *pvData, int32 cubData);
	bool FileWriteAsync(const char *pchFile, const void *pvData, uint32 cubData);
	ECallbackState GetFileWriteAsyncCallbackState(const char *pchFile);
	EResult GetFileWriteAsyncResult(const char *pchFile);
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
	Controller Information
	*/
	bool InitSteamController();
	bool ShutdownSteamController();
	int GetConnectedControllers(ControllerHandle_t *handlesOut);
	ESteamInputType GetInputTypeForHandle(ControllerHandle_t controllerHandle);
	void RunFrame();
	bool ShowBindingPanel(ControllerHandle_t controllerHandle);
	/*
	Controller Action Sets and Layers
	*/
	void ActivateActionSet(ControllerHandle_t controllerHandle, ControllerActionSetHandle_t actionSetHandle);
	ControllerActionSetHandle_t GetActionSetHandle(const char *pszActionSetName);
	ControllerActionSetHandle_t GetCurrentActionSet(ControllerHandle_t controllerHandle);
	void ActivateActionSetLayer(ControllerHandle_t controllerHandle, ControllerActionSetHandle_t hActionSetLayer);
	void DeactivateActionSetLayer(ControllerHandle_t controllerHandle, ControllerActionSetHandle_t hActionSetLayer);
	void DeactivateAllActionSetLayers(ControllerHandle_t controllerHandle);
	int GetActiveActionSetLayers(ControllerHandle_t controllerHandle, ControllerActionSetHandle_t *handlesOut);
	/*
	Controller Actions and Motion
	*/
	ControllerAnalogActionData_t m_AnalogActionData;
	void GetAnalogActionData(ControllerHandle_t controllerHandle, ControllerAnalogActionHandle_t analogActionHandle);
	ControllerAnalogActionHandle_t GetAnalogActionHandle(const char *pszActionName);
	void StopAnalogActionMomentum(ControllerHandle_t controllerHandle, ControllerAnalogActionHandle_t analogActionHandle);
	ControllerDigitalActionData_t m_DigitalActionData;
	void GetDigitalActionData(ControllerHandle_t controllerHandle, ControllerDigitalActionHandle_t digitalActionHandle);
	ControllerDigitalActionHandle_t GetDigitalActionHandle(const char *pszActionName);
	/*
	Controller Action Origins.
	*/
	int GetAnalogActionOrigins(ControllerHandle_t controllerHandle, ControllerActionSetHandle_t actionSetHandle, ControllerAnalogActionHandle_t analogActionHandle, EControllerActionOrigin *originsOut);
	int GetDigitalActionOrigins(ControllerHandle_t controllerHandle, ControllerActionSetHandle_t actionSetHandle, ControllerDigitalActionHandle_t digitalActionHandle, EControllerActionOrigin *originsOut);
	const char *GetGlyphForActionOrigin(EControllerActionOrigin eOrigin);
	const char *GetStringForActionOrigin(EControllerActionOrigin eOrigin);
	/*
	Controller Effects
	*/
	void SetLEDColor(ControllerHandle_t controllerHandle, uint8 nColorR, uint8 nColorG, uint8 nColorB, unsigned int nFlags);
	void TriggerHapticPulse(ControllerHandle_t controllerHandle, ESteamControllerPad eTargetPad, unsigned short duration);
	void TriggerRepeatedHapticPulse(ControllerHandle_t controllerHandle, ESteamControllerPad eTargetPad, unsigned short onDuration, unsigned short offDuration, unsigned short repeat, unsigned int flags);
	void TriggerVibration(ControllerHandle_t controllerHandle, unsigned short usLeftSpeed, unsigned short usRightSpeed);
};

#endif // STEAMPLUGIN_H_
