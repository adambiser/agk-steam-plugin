#include "SteamPlugin.h"
#include "CLeaderboardFindCallback.h"
#include "CLeaderboardScoresDownloadedCallback.h"

#define min(a,b) (a < b) ? a : b;

// Callback for RequestStats.
void SteamPlugin::OnUserStatsReceived(UserStatsReceived_t *pCallback)
{
	if (pCallback->m_nGameID == m_AppID)
	{
		if (pCallback->m_eResult == k_EResultOK)
		{
			agk::Log("Callback: User Stats Received Succeeded");
			m_RequestStatsCallbackState = Done;
			m_StatsInitialized = true;
		}
		else
		{
			agk::Log("Callback: User Stats Received Failed");
			m_RequestStatsCallbackState = ServerError;
		}
	}
}

bool SteamPlugin::RequestStats()
{
	// Set to false so that we can detect when the callback finishes for this request.
	m_StatsInitialized = false;
	if (!LoggedOn())
	{
		return false;
	}
	CheckInitialized(SteamUserStats, false);
	// Fail when the callback is currently running.
	if (m_RequestStatsCallbackState == Running)
	{
		return false;
	}
	if (!SteamUserStats()->RequestCurrentStats())
	{
		m_RequestStatsCallbackState = ClientError;
		return false;
	}
	m_RequestStatsCallbackState = Running;
	return true;
}

// Callback for StoreStats and ResetAllStats.
void SteamPlugin::OnUserStatsStored(UserStatsStored_t *pCallback)
{
	if (pCallback->m_nGameID == m_AppID)
	{
		if (pCallback->m_eResult == k_EResultOK)
		{
			agk::Log("Callback: User Stats Stored Succeeded");
			m_StoreStatsCallbackState = Done;
			m_StatsStored = true;
		}
		else
		{
			agk::Log("Callback: User Stats Stored Failed");
			m_StoreStatsCallbackState = ServerError;
		}
	}
}

// Callback for StoreStats.
void SteamPlugin::OnAchievementStored(UserAchievementStored_t *pCallback)
{
	if (pCallback->m_nGameID == m_AppID)
	{
		agk::Log("Callback: Achievement Stored");
		m_AchievementStored = true;
	}
}

bool SteamPlugin::StoreStats()
{
	CheckInitialized(SteamUserStats, false);
	// Fail when the callback is currently running.
	if (m_StoreStatsCallbackState == Running)
	{
		return false;
	}
	m_StatsStored = false;
	m_AchievementStored = false;
	if (!SteamUserStats()->StoreStats())
	{
		m_StoreStatsCallbackState = ClientError;
		return false;
	}
	m_StoreStatsCallbackState = Running;
	return true;
}

bool SteamPlugin::ResetAllStats(bool bAchievementsToo)
{
	CheckInitialized(SteamUserStats, false);
	// Fail when the callback is currently running.
	if (m_StoreStatsCallbackState == Running)
	{
		return false;
	}
	m_StatsStored = false;
	m_AchievementStored = false;
	if (!SteamUserStats()->ResetAllStats(bAchievementsToo))
	{
		m_StoreStatsCallbackState = ClientError;
		return false;
	}
	m_StoreStatsCallbackState = Running;
	return true;
}

int SteamPlugin::GetNumAchievements()
{
	CheckInitialized(SteamUserStats, 0);
	return SteamUserStats()->GetNumAchievements();
}

const char *SteamPlugin::GetAchievementAPIName(int index)
{
	CheckInitialized(SteamUserStats, NULL);
	return SteamUserStats()->GetAchievementName(index);
}

const char *SteamPlugin::GetAchievementDisplayAttribute(const char *pchName, const char *pchKey)
{
	CheckInitialized(SteamUserStats, NULL);
	return SteamUserStats()->GetAchievementDisplayAttribute(pchName, pchKey);
}

// Callback for GetAchievementIcon.
void SteamPlugin::OnAchievementIconFetched(UserAchievementIconFetched_t *pParam)
{
	if (pParam->m_nGameID.AppID() == m_AppID)
	{
		agk::Log("Callback: Achievement Icon Fetched");
		// Only store the results for values that are expected.
		std::string name = pParam->m_rgchAchievementName;
		auto search = m_AchievementIconsMap.find(name);
		if (search != m_AchievementIconsMap.end())
		{
			search->second = pParam->m_nIconHandle;
		}
	}
}

/*
In the Steam API, GetAchievementIcon reports failure and "no icon" the same way.
Instead this checks some failure conditions so that it can return 0 for failure and -1 when waiting on a callback.
*/
int SteamPlugin::GetAchievementIcon(const char *pchName)
{
	CheckInitialized(SteamUserStats, 0);
	// See if we're waiting for a callback result for this icon.
	std::string name = pchName;
	auto search = m_AchievementIconsMap.find(name);
	if (search != m_AchievementIconsMap.end())
	{
		// If we have a result from the callback, remove it from the wait list.
		if ((search->second) != -1)
		{
			m_AchievementIconsMap.erase(search);
		}
		return search->second;
	}
	int hImage = SteamUserStats()->GetAchievementIcon(pchName);
	if (hImage == 0)
	{
		// Check to see if this is a valid achievement name.
		bool pbAchieved;
		if (!SteamUserStats()->GetAchievement(pchName, &pbAchieved))
		{
			// Not valid, return 0 for no icon.
			return 0;
		}
		hImage = -1; // Use -1 to indicate that we're waiting on a callback to provide the handle.
		//m_AchievementIconsMap.insert_or_assign(name, hImage);
		m_AchievementIconsMap[name] = hImage;
	}
	return hImage;
}

bool SteamPlugin::GetAchievement(const char *pchName, bool *pbAchieved)
{
	CheckInitialized(SteamUserStats, false);
	return SteamUserStats()->GetAchievement(pchName, pbAchieved);
}

bool SteamPlugin::SetAchievement(const char *pchName)
{
	CheckInitialized(SteamUserStats, false);
	return SteamUserStats()->SetAchievement(pchName);
}

bool SteamPlugin::GetAchievementAndUnlockTime(const char *pchName, bool *pbAchieved, uint32 *punUnlockTime)
{
	CheckInitialized(SteamUserStats, false);
	return SteamUserStats()->GetAchievementAndUnlockTime(pchName, pbAchieved, punUnlockTime);
}

bool SteamPlugin::IndicateAchievementProgress(const char *pchName, uint32 nCurProgress, uint32 nMaxProgress)
{
	CheckInitialized(SteamUserStats, false);
	return SteamUserStats()->IndicateAchievementProgress(pchName, nCurProgress, nMaxProgress);
}

bool SteamPlugin::ClearAchievement(const char *pchName)
{
	CheckInitialized(SteamUserStats, false);
	return SteamUserStats()->ClearAchievement(pchName);
}

bool SteamPlugin::GetStat(const char *pchName, int32 *pData)
{
	CheckInitialized(SteamUserStats, false);
	return SteamUserStats()->GetStat(pchName, pData);
}

bool SteamPlugin::GetStat(const char *pchName, float *pData)
{
	CheckInitialized(SteamUserStats, false);
	return SteamUserStats()->GetStat(pchName, pData);
}

bool SteamPlugin::SetStat(const char *pchName, int32 nData)
{
	CheckInitialized(SteamUserStats, false);
	return SteamUserStats()->SetStat(pchName, nData);
}

bool SteamPlugin::SetStat(const char *pchName, float fData)
{
	CheckInitialized(SteamUserStats, false);
	return SteamUserStats()->SetStat(pchName, fData);
}

bool SteamPlugin::UpdateAvgRateStat(const char *pchName, float flCountThisSession, double dSessionLength)
{
	CheckInitialized(SteamUserStats, false);
	return SteamUserStats()->UpdateAvgRateStat(pchName, flCountThisSession, dSessionLength);
}

int SteamPlugin::FindLeaderboard(const char *pchLeaderboardName)
{
	CheckInitialized(SteamUserStats, false);
	CLeaderboardFindCallback *callback = new CLeaderboardFindCallback(pchLeaderboardName);
	callback->Run();
	return AddCallbackItem(callback);
}

SteamLeaderboard_t SteamPlugin::GetLeaderboardHandle(int hCallback)
{
	CLeaderboardFindCallback *callback = GetCallbackItem<CLeaderboardFindCallback>(hCallback);
	if (callback)
	{
		return callback->GetLeaderboardHandle();
	}
	return 0;
}

const char *SteamPlugin::GetLeaderboardName(SteamLeaderboard_t hLeaderboard)
{
	if (!m_StatsInitialized || !hLeaderboard)
	{
		return NULL;
	}
	return SteamUserStats()->GetLeaderboardName(hLeaderboard);
}

int SteamPlugin::GetLeaderboardEntryCount(SteamLeaderboard_t hLeaderboard)
{
	if (!m_StatsInitialized || !hLeaderboard)
	{
		return 0;
	}
	return SteamUserStats()->GetLeaderboardEntryCount(hLeaderboard);
}

ELeaderboardDisplayType SteamPlugin::GetLeaderboardDisplayType(SteamLeaderboard_t hLeaderboard)
{
	if (!m_StatsInitialized || !hLeaderboard)
	{
		return k_ELeaderboardDisplayTypeNone;
	}
	return SteamUserStats()->GetLeaderboardDisplayType(hLeaderboard);
}

ELeaderboardSortMethod SteamPlugin::GetLeaderboardSortMethod(SteamLeaderboard_t hLeaderboard)
{
	if (!m_StatsInitialized || !hLeaderboard)
	{
		return k_ELeaderboardSortMethodNone;
	}
	return SteamUserStats()->GetLeaderboardSortMethod(hLeaderboard);
}

// Callback for UploadLeaderboardScore.
void SteamPlugin::OnUploadScore(LeaderboardScoreUploaded_t *pCallback, bool bIOFailure)
{
	if (pCallback->m_bSuccess && !bIOFailure)
	{
		agk::Log("Callback: Upload Leaderboard Score Succeeded");
		m_UploadLeaderboardScoreCallbackState = Done;
	}
	else
	{
		agk::Log("Callback: Upload Leaderboard Score Failed");
		m_UploadLeaderboardScoreCallbackState = ServerError;
	}
	m_LeaderboardScoreUploaded = *pCallback;
	// Factor in bIOFailure.
	m_LeaderboardScoreUploaded.m_bSuccess = m_LeaderboardScoreUploaded.m_bSuccess && !bIOFailure;
}

bool SteamPlugin::UploadLeaderboardScore(SteamLeaderboard_t hLeaderboard, ELeaderboardUploadScoreMethod eLeaderboardUploadScoreMethod, int score)
{
	CheckInitialized(SteamUserStats, false);
	if (!hLeaderboard)
	{
		m_UploadLeaderboardScoreCallbackState = ClientError;
		return false;
	}
	// Fail when the callback is currently running.
	if (m_UploadLeaderboardScoreCallbackState == Running)
	{
		return false;
	}
	try
	{
		// Clear result structure.
		m_LeaderboardScoreUploaded.m_bScoreChanged = false;
		m_LeaderboardScoreUploaded.m_bSuccess = false;
		m_LeaderboardScoreUploaded.m_hSteamLeaderboard = 0;
		m_LeaderboardScoreUploaded.m_nGlobalRankNew = 0;
		m_LeaderboardScoreUploaded.m_nGlobalRankPrevious = 0;
		m_LeaderboardScoreUploaded.m_nScore = 0;
		// Call it.
		SteamAPICall_t hSteamAPICall = SteamUserStats()->UploadLeaderboardScore(hLeaderboard, eLeaderboardUploadScoreMethod, score, NULL, 0);
		m_CallResultUploadScore.Set(hSteamAPICall, this, &SteamPlugin::OnUploadScore);
		m_UploadLeaderboardScoreCallbackState = Running;
		return true;
	}
	catch (...)
	{
		m_UploadLeaderboardScoreCallbackState = ClientError;
		return false;
	}
}

int SteamPlugin::DownloadLeaderboardEntries(SteamLeaderboard_t hLeaderboard, ELeaderboardDataRequest eLeaderboardDataRequest, int nRangeStart, int nRangeEnd)
{
	CheckInitialized(SteamUserStats, 0);
	if (!hLeaderboard)
	{
		agk::PluginError("Invalid leaderboard handle: 0");
		return 0;
	}
	CLeaderboardScoresDownloadedCallback *callback = new CLeaderboardScoresDownloadedCallback(hLeaderboard, eLeaderboardDataRequest, nRangeStart, nRangeEnd);
	callback->Run();
	return AddCallbackItem(callback);
}

int SteamPlugin::GetDownloadedLeaderboardEntryCount(int hCallback)
{
	CLeaderboardScoresDownloadedCallback *callback = GetCallbackItem<CLeaderboardScoresDownloadedCallback>(hCallback);
	if (callback)
	{
		return callback->GetEntryCount();
	}
	return 0;
}

int SteamPlugin::GetDownloadedLeaderboardEntryGlobalRank(int hCallback, int index)
{
	CLeaderboardScoresDownloadedCallback *callback = GetCallbackItem<CLeaderboardScoresDownloadedCallback>(hCallback);
	if (callback)
	{
		return callback->GetEntryGlobalRank(index);
	}
	return 0;
}

int SteamPlugin::GetDownloadedLeaderboardEntryScore(int hCallback, int index)
{
	CLeaderboardScoresDownloadedCallback *callback = GetCallbackItem<CLeaderboardScoresDownloadedCallback>(hCallback);
	if (callback)
	{
		return callback->GetEntryScore(index);
	}
	return 0;
}

CSteamID SteamPlugin::GetDownloadedLeaderboardEntryUser(int hCallback, int index)
{
	CLeaderboardScoresDownloadedCallback *callback = GetCallbackItem<CLeaderboardScoresDownloadedCallback>(hCallback);
	if (callback)
	{
		return callback->GetEntryUser(index);
	}
	return k_steamIDNil;
}
