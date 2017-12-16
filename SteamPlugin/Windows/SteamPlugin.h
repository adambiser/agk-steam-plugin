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

#define MAX_LEADERBOARD_ENTRIES 10

enum ECallbackState
{
	ServerError = -2,
	ClientError = -1,
	Idle = 0,
	Running = 1,
	Done = 2
};

enum EAvatarSize
{
	Small = 0,
	Medium = 1,
	Large = 2
};

class SteamPlugin {
private:
	// General methods.
	uint64 m_AppID;
	bool m_SteamInitialized;
	// User/Friend methods
	bool m_AvatarImageLoadedEnabled; // Added so games that don't load friend avatars don't waste memory storing things it never uses.
	std::list<CSteamID> m_AvatarImageLoadedUsers;
	CSteamID m_AvatarUser;
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
	// Lobby methods: Join
	std::vector<CSteamID> m_JoinedLobbies; // Keep track so we leave any left open when closing.
	ECallbackState m_LobbyEnterCallbackState;
	void OnLobbyEnter(LobbyEnter_t *pParam, bool bIOFailure);
	CCallResult<SteamPlugin, LobbyEnter_t> m_CallResultLobbyEnter;
	STEAM_CALLBACK(SteamPlugin, OnLobbyEnter, LobbyEnter_t, m_MainCallResultLobbyEnter);
	CSteamID m_LobbyEnterID;
	bool m_LobbyEnterBlocked;
	EChatRoomEnterResponse m_LobbyEnterResponse;
	// Lobby methods: Leave
	// Lobby methods: Members
	// Lobby methods: Member status
	STEAM_CALLBACK(SteamPlugin, OnLobbyChatUpdated, LobbyChatUpdate_t, m_CallbackLobbyChatUpdated);
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
	CSteamID m_LobbyChatMessageUser; // Holds next chat message user after HasLobbyChatMessage call.
	char m_LobbyChatMessageText[4096];
	struct ChatMessageInfo_t
	{
		CSteamID user;
		char text[4096];
	};
	std::list<ChatMessageInfo_t> m_ChatMessages;

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
	bool LoggedOn();
	void RunCallbacks();
	void ActivateGameOverlay(const char *pchDialog);
	// User/Friend methods
	const char *GetPersonaName();
	CSteamID GetSteamID();
	bool HasAvatarImageLoaded();
	CSteamID GetAvatarImageLoadedUser() { return m_AvatarUser; }
	int GetFriendAvatar(CSteamID steamID, EAvatarSize size);
	const char *GetFriendPersonaName(CSteamID steamID);
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
	const char* GetAchievementID(int index);
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
	bool RequestLobbyList();
	CSteamID GetLobbyByIndex(int iLobby);
	ECallbackState GetLobbyMatchListCallbackState() { return getCallbackState(&m_LobbyMatchListCallbackState); }
	int GetLobbyMatchListCount() { return m_LobbyMatchList.m_nLobbiesMatching; }
	// Lobby methods: Data
	int GetLobbyDataCount(CSteamID steamIDLobby);
	bool GetLobbyDataByIndex(CSteamID steamIDLobby, int iLobbyData, char *pchKey, int cchKeyBufferSize, char *pchValue, int cchValueBufferSize);
	const char *GetLobbyData(CSteamID steamIDLobby, const char *pchKey);
	bool RequestLobbyData(CSteamID steamIDLobby);
	bool HasLobbyDataUpdated();
	CSteamID GetLobbyDataUpdatedLobby() { return m_LobbyDataUpdatedLobby; }
	CSteamID GetLobbyDataUpdatedID() { return m_LobbyDataUpdatedID; }
	const char *GetLobbyMemberData(CSteamID steamIDLobby, CSteamID steamIDUser, const char *pchKey);
	void SetLobbyMemberData(CSteamID steamIDLobby, const char *pchKey, const char *pchValue);
	// Lobby methods: Owner methods
	bool DeleteLobbyData(CSteamID steamIDLobby, const char *pchKey);
	void SetLobbyData(CSteamID steamIDLobby, const char *pchKey, const char *pchValue);
	//bool SetLinkedLobby(CSteamID steamIDLobby, CSteamID steamIDLobbyDependent);
	//void SetLobbyGameServer(CSteamID steamIDLobby, uint32 unGameServerIP, uint16 unGameServerPort, CSteamID steamIDGameServer);
	//bool SetLobbyJoinable(CSteamID steamIDLobby, bool bLobbyJoinable);
	//bool SetLobbyMemberLimit(CSteamID steamIDLobby, int cMaxMembers);
	//bool SetLobbyOwner(CSteamID steamIDLobby, CSteamID steamIDNewOwner); // Triggers a LobbyDataUpdate_t callback.
	//bool SetLobbyType(CSteamID steamIDLobby, ELobbyType eLobbyType);
	// Lobby methods: Create
	bool CreateLobby(ELobbyType eLobbyType, int cMaxMembers);
	ECallbackState GetLobbyCreateCallbackState() { return getCallbackState(&m_LobbyCreateCallbackState); }
	CSteamID GetLobbyCreatedID() { return m_LobbyCreatedID; }
	EResult GetLobbyCreatedResult() { return m_LobbyCreatedResult; }
	// Lobby methods: Join
	bool JoinLobby(CSteamID steamIDLobby);
	ECallbackState GetLobbyEnterCallbackState() { return getCallbackState(&m_LobbyEnterCallbackState); }
	CSteamID GetLobbyEnterID() { return m_LobbyEnterID; }
	bool GetLobbyEnterBlocked() { return m_LobbyEnterBlocked; }
	uint32 GetLobbyEnterResponse() { return m_LobbyEnterResponse; }
	// Lobby methods: Leave
	void LeaveLobby(CSteamID steamIDLobby);
	// Lobby methods:  members
	CSteamID GetLobbyOwner(CSteamID steamIDLobby);
	int GetLobbyMemberLimit(CSteamID steamIDLobby);
	int GetNumLobbyMembers(CSteamID steamIDLobby);
	CSteamID GetLobbyMemberByIndex(CSteamID steamIDLobby, int iMember);
	// Lobby methods: Member status
	bool HasLobbyChatUpdate();
	CSteamID GetLobbyChatUpdateUserChanged() { return m_LobbyChatUpdateUserChanged; }
	EChatMemberStateChange GetLobbyChatUpdateUserState() { return m_LobbyChatUpdateUserState; }
	CSteamID GetLobbyChatUpdateUserMakingChange() { return m_LobbyChatUpdateUserMakingChange; }
	// Lobby methods: Chat messages
	bool HasLobbyChatMessage();
	CSteamID GetLobbyChatMessageUser() { return m_LobbyChatMessageUser; }
	void GetLobbyChatMessageText(char *msg) { strcpy_s(msg, strlen(m_LobbyChatMessageText) + 1, m_LobbyChatMessageText); }
	bool SendLobbyChatMessage(CSteamID steamIDLobby, const char *pvMsgBody);
};

#endif // STEAMPLUGIN_H_
