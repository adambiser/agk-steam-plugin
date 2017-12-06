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
	uint64 m_AppID;
	bool m_SteamInitialized;
	ECallbackState m_RequestStatsCallbackState;
	bool m_StatsInitialized;
	ECallbackState m_StoreStatsCallbackState;
	bool m_StatsStored;
	bool m_AchievementStored;
	SteamLeaderboard_t m_hSteamLeaderboard;
	ECallbackState m_FindLeaderboardCallbackState;
	ECallbackState m_UploadLeaderboardScoreCallbackState;
	bool m_LeaderboardScoreStored;
	bool m_LeaderboardScoreChanged;
	int m_LeaderboardUploadedScore;
	int m_LeaderboardGlobalRankNew;
	int m_LeaderboardGlobalRankPrevious;
	ECallbackState m_DownloadLeaderboardEntriesCallbackState;
	int m_DownloadedLeaderboardEntryCount;
	LeaderboardEntry_t m_DownloadedLeaderboardEntries[MAX_LEADERBOARD_ENTRIES];
	int GetFriendAvatar(CSteamID steamID, EAvatarSize size);
	// User stats callbacks
	STEAM_CALLBACK(SteamPlugin, OnAchievementStored, UserAchievementStored_t, m_CallbackAchievementStored);
	STEAM_CALLBACK(SteamPlugin, OnUserStatsReceived, UserStatsReceived_t, m_CallbackUserStatsReceived);
	STEAM_CALLBACK(SteamPlugin, OnUserStatsStored, UserStatsStored_t, m_CallbackUserStatsStored);
	STEAM_CALLBACK(SteamPlugin, OnAvatarImageLoaded, AvatarImageLoaded_t, m_CallbackAvatarImageLoaded);
	STEAM_CALLBACK(SteamPlugin, OnAchievementIconFetched, UserAchievementIconFetched_t, m_CallbackAchievementIconFetched);
	// Leaderboard callbacks
	void OnFindLeaderboard(LeaderboardFindResult_t *pResult, bool bIOFailure);
	CCallResult<SteamPlugin, LeaderboardFindResult_t> m_callResultFindLeaderboard;
	void OnUploadScore(LeaderboardScoreUploaded_t *pResult, bool bIOFailure);
	CCallResult<SteamPlugin, LeaderboardScoreUploaded_t> m_callResultUploadScore;
	void OnDownloadScore(LeaderboardScoresDownloaded_t *pResult, bool bIOFailure);
	CCallResult<SteamPlugin, LeaderboardScoresDownloaded_t> m_callResultDownloadScore;

	std::map<CSteamID, int> avatarsMap;
	std::map<std::string, int> achievementIconsMap;

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
	bool SteamInitialized();
	int GetAppID();
	bool RestartAppIfNecessary(uint32 unOwnAppID);
	bool LoggedOn();
	const char *GetPersonaName();
	void RunCallbacks();
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
	bool GetAchievement(const char *name, bool *pbAchieved);
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
	SteamLeaderboard_t GetLeaderboardHandle() { return m_hSteamLeaderboard; }
	// General information
	const char *GetLeaderboardName(SteamLeaderboard_t hLeaderboard);
	int GetLeaderboardEntryCount(SteamLeaderboard_t hLeaderboard);
	ELeaderboardDisplayType GetLeaderboardDisplayType(SteamLeaderboard_t hLeaderboard);
	ELeaderboardSortMethod GetLeaderboardSortMethod(SteamLeaderboard_t hLeaderboard);
	// Uploading scores
	bool UploadLeaderboardScore(SteamLeaderboard_t hLeaderboard, ELeaderboardUploadScoreMethod eLeaderboardUploadScoreMethod, int score);
	ECallbackState GetUploadLeaderboardScoreCallbackState() { return getCallbackState(&m_UploadLeaderboardScoreCallbackState); }
	bool LeaderboardScoreStored() { return m_LeaderboardScoreStored; }
	bool LeaderboardScoreChanged() { return m_LeaderboardScoreChanged; }
	int GetLeaderboardUploadedScore() { return m_LeaderboardUploadedScore; }
	int GetLeaderboardGlobalRankNew() { return m_LeaderboardGlobalRankNew; }
	int GetLeaderboardGlobalRankPrevious() { return m_LeaderboardGlobalRankPrevious; }
	// Downloading entries.
	bool DownloadLeaderboardEntries(SteamLeaderboard_t hLeaderboard, ELeaderboardDataRequest eLeaderboardDataRequest, int nRangeStart, int nRangeEnd);
	ECallbackState GetDownloadLeaderboardEntriesCallbackState() { return getCallbackState(&m_DownloadLeaderboardEntriesCallbackState); }
	int GetDownloadedLeaderboardEntryCount() { return m_DownloadedLeaderboardEntryCount; }
	int GetDownloadedLeaderboardEntryGlobalRank(int index);
	int GetDownloadedLeaderboardEntryScore(int index);
	const char *GetDownloadedLeaderboardEntryPersonaName(int index);
	int GetDownloadedLeaderboardEntryAvatar(int index, EAvatarSize size);
	// Avatars and images.
	// While GetAvatar, GetAchievementIcon, and GetDownloadedLeaderboardEntryAvatar have internal callbacks, there's no need to make them external.
	int GetAvatar(EAvatarSize size);
	int LoadImageFromHandle(int hImage);
	int LoadImageFromHandle(int imageID, int hImage);
	int GetAchievementIcon(const char *pchName);
};

#endif // STEAMPLUGIN_H_
