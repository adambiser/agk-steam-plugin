#include "SteamPlugin.h"

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
		m_AchievementIconsMap.insert_or_assign(name, hImage);
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

// Callback for FindLeaderboard.
void SteamPlugin::OnFindLeaderboard(LeaderboardFindResult_t *pCallback, bool bIOFailure)
{
	if (pCallback->m_bLeaderboardFound && !bIOFailure)
	{
		agk::Log("Callback: Find Leaderboard Succeeded");
		m_FindLeaderboardCallbackState = Done;
	}
	else
	{
		// Did not find the leaderboard.
		agk::Log("Callback: Find Leaderboard Failed");
		m_FindLeaderboardCallbackState = ServerError;
	}
	m_LeaderboardFindResult = *pCallback;
}

// TODO Change this to behave like CFileReadAsyncItem does so multiple leaderboards can be searched at once.
bool SteamPlugin::FindLeaderboard(const char *pchLeaderboardName)
{
	CheckInitialized(SteamUserStats, false);
	// Fail when the callback is currently running.
	if (m_FindLeaderboardCallbackState == Running)
	{
		return false;
	}
	try
	{
		// Clear result structure.
		m_LeaderboardFindResult.m_bLeaderboardFound = 0;
		m_LeaderboardFindResult.m_hSteamLeaderboard = 0;
		SteamAPICall_t hSteamAPICall = SteamUserStats()->FindLeaderboard(pchLeaderboardName);
		m_CallResultFindLeaderboard.Set(hSteamAPICall, this, &SteamPlugin::OnFindLeaderboard);
		m_FindLeaderboardCallbackState = Running;
		return true;
	}
	catch (...)
	{
		m_FindLeaderboardCallbackState = ClientError;
		return false;
	}
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

// Callback for DownloadLeaderboardEntries.
void SteamPlugin::OnDownloadScore(LeaderboardScoresDownloaded_t *pCallback, bool bIOFailure)
{
	if (!bIOFailure)
	{
		agk::Log("Callback: Download Leaderboard Entries Succeeded");
		m_DownloadLeaderboardEntriesCallbackState = Done;
		for (int index = 0; index < pCallback->m_cEntryCount; index++)
		{
			LeaderboardEntry_t entry;
			SteamUserStats()->GetDownloadedLeaderboardEntry(pCallback->m_hSteamLeaderboardEntries, index, &entry, NULL, 0);
			m_DownloadedLeaderboardEntries.push_back(entry);
		}
	}
	else
	{
		agk::Log("Callback: Download Leaderboard Entries Failed");
		m_DownloadLeaderboardEntriesCallbackState = ServerError;
	}
}

bool SteamPlugin::DownloadLeaderboardEntries(SteamLeaderboard_t hLeaderboard, ELeaderboardDataRequest eLeaderboardDataRequest, int nRangeStart, int nRangeEnd)
{
	CheckInitialized(SteamUserStats, false);
	if (!hLeaderboard)
	{
		m_DownloadLeaderboardEntriesCallbackState = ClientError;
		return false;
	}
	// Fail when the callback is currently running.
	if (m_DownloadLeaderboardEntriesCallbackState == Running)
	{
		return false;
	}
	try
	{
		m_DownloadedLeaderboardEntries.clear();
		SteamAPICall_t hSteamAPICall = SteamUserStats()->DownloadLeaderboardEntries(hLeaderboard, eLeaderboardDataRequest, nRangeStart, nRangeEnd);
		m_CallResultDownloadScore.Set(hSteamAPICall, this, &SteamPlugin::OnDownloadScore);
		m_DownloadLeaderboardEntriesCallbackState = Running;
		return true;
	}
	catch (...)
	{
		m_DownloadLeaderboardEntriesCallbackState = ClientError;
		return false;
	}
}

int SteamPlugin::GetDownloadedLeaderboardEntryGlobalRank(int index)
{
	if (index < 0 || index >= (int)m_DownloadedLeaderboardEntries.size())
	{
		agk::PluginError("GetDownloadedLeaderboardEntryGlobalRank index out of bounds.");
		return 0;
	}
	return m_DownloadedLeaderboardEntries[index].m_nGlobalRank;
}

int SteamPlugin::GetDownloadedLeaderboardEntryScore(int index)
{
	if (index < 0 || index >= (int)m_DownloadedLeaderboardEntries.size())
	{
		agk::PluginError("GetDownloadedLeaderboardEntryScore index out of bounds.");
		return 0;
	}
	return m_DownloadedLeaderboardEntries[index].m_nScore;
}

CSteamID SteamPlugin::GetDownloadedLeaderboardEntryUser(int index)
{
	if (index < 0 || index >= (int)m_DownloadedLeaderboardEntries.size())
	{
		agk::PluginError("GetDownloadedLeaderboardEntryAvatar index out of bounds.");
		return k_steamIDNil;
	}
	return m_DownloadedLeaderboardEntries[index].m_steamIDUser;
}
