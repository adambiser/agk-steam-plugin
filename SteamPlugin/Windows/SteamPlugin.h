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

class SteamPlugin {
private:
	uint64 m_AppID;
	bool m_SteamInitialized;
	bool m_StatsInitialized;
	bool m_StatsStored;
	bool m_AchievementStored;
	SteamLeaderboard_t m_hSteamLeaderboard;
	bool m_LeaderboardFindResultReceived;
	//int m_ErrorCode;
	STEAM_CALLBACK(SteamPlugin, OnAchievementStored, UserAchievementStored_t, m_CallbackAchievementStored);
	STEAM_CALLBACK(SteamPlugin, OnUserStatsReceived, UserStatsReceived_t, m_CallbackUserStatsReceived);
	STEAM_CALLBACK(SteamPlugin, OnUserStatsStored, UserStatsStored_t, m_CallbackUserStatsStored);
	

public:
	SteamPlugin();
	virtual ~SteamPlugin(void);
	// General methods.
	bool Init();
	void Shutdown();
	bool SteamInitialized();
	int GetAppID();
	bool LoggedOn();
	void RunCallbacks();
	// General user stats methods.
	bool RequestStats();
	bool StatsInitialized();
	bool StoreStats();
	bool ResetAllStats(bool bAchievementsToo);
	bool StatsStored();
	bool AchievementStored();
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
	int m_nLeaderboardEntries;
	LeaderboardEntry_t m_leaderboardEntries[10];
	SteamAPICall_t FindLeaderboard(const char *pchLeaderboardName);
	void OnFindLeaderboard(LeaderboardFindResult_t *pResult, bool bIOFailure);
	CCallResult<LeaderboardFindResult_t, bool> m_callResultFindLeaderboard;
	//void OnUploadScore(LeaderboardScoreUploaded_t *pResult, bool bIOFailure);
	//CCallResult<LeaderboardScoreUploaded_t, bool> m_callResultUploadScore;
	//void OnDownloadScore(LeaderboardScoresDownloaded_t *pResult, bool bIOFailure);
	//CCallResult<LeaderboardScoresDownloaded_t, bool> m_callResultDownloadScore;
};

#endif // STEAMPLUGIN_H_
