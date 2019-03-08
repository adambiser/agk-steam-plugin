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

#include "SteamPlugin.h"
#include "CLeaderboardFindCallResult.h"
#include "CLeaderboardScoresDownloadedCallResult.h"
#include "CLeaderboardScoreUploadedCallResult.h"

#define min(a,b) (a < b) ? a : b;

// Callback for RequestStats.
void SteamPlugin::OnUserStatsReceived(UserStatsReceived_t *pCallback)
{
	if (pCallback->m_nGameID == m_AppID)
	{
		utils::Log("Callback: OnUserStatsReceived " + std::string((pCallback->m_eResult == k_EResultOK) ? "Succeeded." : "Failed.  Result = " + std::to_string(pCallback->m_eResult)));
		if (pCallback->m_eResult == k_EResultOK)
		{
			m_RequestStatsCallbackState = Done;
			m_StatsInitialized = true;
		}
		else
		{
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
		utils::Log("Callback: OnUserStatsStored " + std::string((pCallback->m_eResult == k_EResultOK) ? "Succeeded." : "Failed.  Result = " + std::to_string(pCallback->m_eResult)));
		if (pCallback->m_eResult == k_EResultOK)
		{
			m_StoreStatsCallbackState = Done;
			m_StatsStored = true;
		}
		else
		{
			m_StoreStatsCallbackState = ServerError;
		}
	}
}

// Callback for StoreStats.
void SteamPlugin::OnAchievementStored(UserAchievementStored_t *pCallback)
{
	if (pCallback->m_nGameID == m_AppID)
	{
		agk::Log("Callback: OnAchievementStored");
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
		agk::Log("Callback: OnAchievementIconFetched");
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
	CLeaderboardFindCallResult *callResult = new CLeaderboardFindCallResult(pchLeaderboardName);
	callResult->Run();
	return AddCallResultItem(callResult);
}

SteamLeaderboard_t SteamPlugin::GetLeaderboardHandle(int hCallResult)
{
	if (CLeaderboardFindCallResult *callResult = GetCallResultItem<CLeaderboardFindCallResult>(hCallResult))
	{
		return callResult->GetLeaderboardHandle();
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

int SteamPlugin::UploadLeaderboardScore(SteamLeaderboard_t hLeaderboard, ELeaderboardUploadScoreMethod eLeaderboardUploadScoreMethod, int nScore)
{
	CheckInitialized(SteamUserStats, 0);
	if (!hLeaderboard)
	{
		agk::PluginError("UploadLeaderboardScore: Invalid leaderboard handle.");
		return 0;
	}
	CLeaderboardScoreUploadedCallResult *callResult = new CLeaderboardScoreUploadedCallResult(hLeaderboard, eLeaderboardUploadScoreMethod, nScore);
	callResult->Run();
	return AddCallResultItem(callResult);
}

bool SteamPlugin::LeaderboardScoreStored(int hCallResult)
{
	if (CLeaderboardScoreUploadedCallResult *callResult = GetCallResultItem<CLeaderboardScoreUploadedCallResult>(hCallResult))
	{
		return callResult->LeaderboardScoreStored();
	}
	return false;
}

bool SteamPlugin::LeaderboardScoreChanged(int hCallResult)
{
	if (CLeaderboardScoreUploadedCallResult *callResult = GetCallResultItem<CLeaderboardScoreUploadedCallResult>(hCallResult))
	{
		return callResult->LeaderboardScoreChanged();
	}
	return false;
}

int SteamPlugin::GetLeaderboardUploadedScore(int hCallResult)
{
	if (CLeaderboardScoreUploadedCallResult *callResult = GetCallResultItem<CLeaderboardScoreUploadedCallResult>(hCallResult))
	{
		return callResult->GetLeaderboardUploadedScore();
	}
	return 0;
}

int SteamPlugin::GetLeaderboardGlobalRankNew(int hCallResult)
{
	if (CLeaderboardScoreUploadedCallResult *callResult = GetCallResultItem<CLeaderboardScoreUploadedCallResult>(hCallResult))
	{
		return callResult->GetLeaderboardGlobalRankNew();
	}
	return 0;
}

int SteamPlugin::GetLeaderboardGlobalRankPrevious(int hCallResult)
{
	if (CLeaderboardScoreUploadedCallResult *callResult = GetCallResultItem<CLeaderboardScoreUploadedCallResult>(hCallResult))
	{
		return callResult->GetLeaderboardGlobalRankPrevious();
	}
	return 0;
}

int SteamPlugin::DownloadLeaderboardEntries(SteamLeaderboard_t hLeaderboard, ELeaderboardDataRequest eLeaderboardDataRequest, int nRangeStart, int nRangeEnd)
{
	CheckInitialized(SteamUserStats, 0);
	if (!hLeaderboard)
	{
		agk::PluginError("DownloadLeaderboardEntries: Invalid leaderboard handle.");
		return 0;
	}
	CLeaderboardScoresDownloadedCallResult *callResult = new CLeaderboardScoresDownloadedCallResult(hLeaderboard, eLeaderboardDataRequest, nRangeStart, nRangeEnd);
	callResult->Run();
	return AddCallResultItem(callResult);
}

int SteamPlugin::GetDownloadedLeaderboardEntryCount(int hCallResult)
{
	if (CLeaderboardScoresDownloadedCallResult *callResult = GetCallResultItem<CLeaderboardScoresDownloadedCallResult>(hCallResult))
	{
		return callResult->GetEntryCount();
	}
	return 0;
}

int SteamPlugin::GetDownloadedLeaderboardEntryGlobalRank(int hCallResult, int index)
{
	if (CLeaderboardScoresDownloadedCallResult *callResult = GetCallResultItem<CLeaderboardScoresDownloadedCallResult>(hCallResult))
	{
		return callResult->GetEntryGlobalRank(index);
	}
	return 0;
}

int SteamPlugin::GetDownloadedLeaderboardEntryScore(int hCallResult, int index)
{
	if (CLeaderboardScoresDownloadedCallResult *callResult = GetCallResultItem<CLeaderboardScoresDownloadedCallResult>(hCallResult))
	{
		return callResult->GetEntryScore(index);
	}
	return 0;
}

CSteamID SteamPlugin::GetDownloadedLeaderboardEntryUser(int hCallResult, int index)
{
	if (CLeaderboardScoresDownloadedCallResult *callResult = GetCallResultItem<CLeaderboardScoresDownloadedCallResult>(hCallResult))
	{
		return callResult->GetEntryUser(index);
	}
	return k_steamIDNil;
}
