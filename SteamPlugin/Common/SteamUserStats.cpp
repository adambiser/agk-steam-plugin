/*
Copyright (c) 2019 Adam Biser <adambiser@gmail.com>

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

#include "Common.h"
#include "SteamUserStats.h"

//AttachLeaderboardUGC

int ClearAchievement(const char *name)
{
	CheckInitialized(false);
	Callbacks()->RegisterUserAchievementStoredCallback();
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

int GetDownloadLeaderboardHandle(int hCallResult)
{
	return GetCallResultValue<CLeaderboardScoresDownloadedCallResult>(hCallResult, &CLeaderboardScoresDownloadedCallResult::GetLeaderboardID);
}

int GetDownloadLeaderboardEntryCount(int hCallResult)
{
	return GetCallResultValue<CLeaderboardScoresDownloadedCallResult>(hCallResult, &CLeaderboardScoresDownloadedCallResult::GetLeaderboardEntryCount);
}

int GetDownloadLeaderboardEntryUser(int hCallResult, int index)
{
	return GetCallResultValue<CLeaderboardScoresDownloadedCallResult>(hCallResult, index, &CLeaderboardScoresDownloadedCallResult::GetLeaderboardEntryUser, __FUNCTION__);
}

int GetDownloadLeaderboardEntryGlobalRank(int hCallResult, int index)
{
	return GetCallResultValue<CLeaderboardScoresDownloadedCallResult>(hCallResult, index, &CLeaderboardScoresDownloadedCallResult::GetLeaderboardEntryGlobalRank, __FUNCTION__);
}

int GetDownloadLeaderboardEntryScore(int hCallResult, int index)
{
	return GetCallResultValue<CLeaderboardScoresDownloadedCallResult>(hCallResult, index, &CLeaderboardScoresDownloadedCallResult::GetLeaderboardEntryScore, __FUNCTION__);
}

// TODO Implement entry details?
//int GetDownloadLeaderboardEntryDetails(int hCallResult, int index)
//{
//	return GetCallResultValue<CLeaderboardScoresDownloadedCallResult>(hCallResult, index, &CLeaderboardScoresDownloadedCallResult::GetLeaderboardEntryDetails, __FUNCTION__);
//}

// TODO UGC
//int GetDownloadLeaderboardEntryUGC(int hCallResult, int index)
//{
//	return GetCallResultValue<CLeaderboardScoresDownloadedCallResult>(hCallResult, index, &CLeaderboardScoresDownloadedCallResult::GetLeaderboardEntryUGCHandle, __FUNCTION__);
//}

int FindLeaderboard(const char *leaderboardName)
{
	CheckInitialized(0);
	return CallResults()->Add(new CLeaderboardFindCallResult(leaderboardName));
}

int FindOrCreateLeaderboard(const char *leaderboardName, int eLeaderboardSortMethod, int eLeaderboardDisplayType)
{
	CheckInitialized(0);
	return CallResults()->Add(new CLeaderboardFindCallResult(leaderboardName, (ELeaderboardSortMethod)eLeaderboardSortMethod, (ELeaderboardDisplayType) eLeaderboardDisplayType));
}

int GetFindLeaderboardFound(int hCallResult)
{
	return GetCallResultValue<CLeaderboardFindCallResult>(hCallResult, &CLeaderboardFindCallResult::GetLeaderboardFindResultFound);
}

int GetFindLeaderboardHandle(int hCallResult)
{
	return GetCallResultValue<CLeaderboardFindCallResult>(hCallResult, &CLeaderboardFindCallResult::GetLeaderboardFindResultHandle);
}

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

//InstallPS3Trophies - ignore

int RequestCurrentStats()
{
	CheckInitialized(false);
	Callbacks()->RegisterUserStatsReceivedCallback();
	return SteamUserStats()->RequestCurrentStats();
}

//RequestGlobalAchievementPercentages
//RequestGlobalStats
//RequestUserStats

int ResetAllStats(int achievementsToo)
{
	CheckInitialized(false);
	Callbacks()->RegisterUserAchievementStoredCallback();
	Callbacks()->RegisterUserStatsStoredCallback();
	return SteamUserStats()->ResetAllStats(achievementsToo != 0);
}

int SetAchievement(const char *name)
{
	CheckInitialized(false);
	Callbacks()->RegisterUserAchievementStoredCallback();
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
	if (g_StoringStats)
	{
		return false;
	}
	g_StoringStats = true;
	Callbacks()->RegisterUserStatsStoredCallback();
	return SteamUserStats()->StoreStats();
}

int IsStoringStats()
{
	return (int)g_StoringStats;
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

int GetUploadLeaderboardScoreSuccess(int hCallResult)
{
	return GetCallResultValue<CLeaderboardScoreUploadedCallResult>(hCallResult, &CLeaderboardScoreUploadedCallResult::GetLeaderboardScoreUploadedSuccess);
}

int GetUploadLeaderboardScoreHandle(int hCallResult)
{
	return GetCallResultValue<CLeaderboardScoreUploadedCallResult>(hCallResult, &CLeaderboardScoreUploadedCallResult::GetLeaderboardScoreUploadedHandle);
}

int GetUploadLeaderboardScoreValue(int hCallResult)
{
	return GetCallResultValue<CLeaderboardScoreUploadedCallResult>(hCallResult, &CLeaderboardScoreUploadedCallResult::GetLeaderboardScoreUploadedScore);
}

int GetUploadLeaderboardScoreChanged(int hCallResult)
{
	return GetCallResultValue<CLeaderboardScoreUploadedCallResult>(hCallResult, &CLeaderboardScoreUploadedCallResult::GetLeaderboardScoreUploadedScoreChanged);
}

int GetUploadLeaderboardScoreRankNew(int hCallResult)
{
	return GetCallResultValue<CLeaderboardScoreUploadedCallResult>(hCallResult, &CLeaderboardScoreUploadedCallResult::GetLeaderboardScoreUploadedRankNew);
}

int GetUploadLeaderboardScoreRankPrevious(int hCallResult)
{
	return GetCallResultValue<CLeaderboardScoreUploadedCallResult>(hCallResult, &CLeaderboardScoreUploadedCallResult::GetLeaderboardScoreUploadedRankPrevious);
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

// Unused
//int GetUserAchievementStoredIsGroup()
//{
//	return Callbacks()->GetUserAchievementStored().m_bGroupAchievement;
//}

char *GetUserAchievementStoredName()
{
	return utils::CreateString(Callbacks()->GetUserAchievementStored().m_rgchAchievementName);
}

int GetUserAchievementStoredCurrentProgress()
{
	return (int)Callbacks()->GetUserAchievementStored().m_nCurProgress;
}

int GetUserAchievementStoredMaxProgress()
{
	return (int)Callbacks()->GetUserAchievementStored().m_nMaxProgress;
}

int HasUserStatsReceivedResponse()
{
	return Callbacks()->HasUserStatsReceivedResponse();
}

// This plugin only reports for the current app id.
//int GetUserStatsReceivedGameAppID()
//{
//	return ((CGameID)(int)Callbacks()->GetUserStatsReceived().m_nGameID).AppID();
//}

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
	if (Callbacks()->HasUserStatsStoredResponse())
	{
		g_StoringStats = false;
		return true;
	}
	return false;
}

int GetUserStatsStoredResult()
{
	return (int)Callbacks()->GetUserStatsStored().m_eResult;
}
