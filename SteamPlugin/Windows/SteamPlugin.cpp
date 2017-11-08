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

#define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <stdio.h>
#include <steam_api.h>
#include "SteamPlugin.h"
#include "..\AGKLibraryCommands.h"

#pragma comment(lib, "steam_api.lib")

SteamPlugin::SteamPlugin() :
	m_AppID(0),
	m_SteamInitialized(false),
	m_RequestingStats(false),
	m_StatsInitialized(false),
	m_StoringStats(false),
	m_StatsStored(false),
	m_AchievementStored(false),
	m_hSteamLeaderboard(0),
	m_FindingLeaderboard(false),
	m_UploadingLeaderboardScore(false),
	m_LeaderboardScoreStored(false),
	m_UploadedScoreChanged(false),
	m_UploadedScore(0),
	m_UploadedGlobalRankNew(0),
	m_UploadedGlobalRankPrevious(0),
	m_DownloadingLeaderboardEntries(false),
	m_NumLeaderboardEntries(0),
	m_CallbackAchievementStored(this, &SteamPlugin::OnAchievementStored),
	m_CallbackUserStatsReceived(this, &SteamPlugin::OnUserStatsReceived),
	m_CallbackUserStatsStored(this, &SteamPlugin::OnUserStatsStored)
{
	// Nothing for now
}

SteamPlugin::~SteamPlugin(void)
{
	this->Shutdown();
}

bool SteamPlugin::Init()
{
	if (!m_SteamInitialized)
	{
		m_SteamInitialized = SteamAPI_Init();
		if (m_SteamInitialized)
		{
			m_AppID = SteamUtils()->GetAppID();
		}
	}
	return m_SteamInitialized;
}

void SteamPlugin::Shutdown()
{
	if (m_SteamInitialized)
	{
		SteamAPI_Shutdown();
		// Reset member variables.
		m_AppID = 0;
		m_SteamInitialized = false;
		m_RequestingStats = false;
		m_StatsInitialized = false;
		m_StoringStats = false;
		m_StatsStored = false;
		m_AchievementStored = false;
		m_hSteamLeaderboard = 0;
		m_FindingLeaderboard = false;
		m_UploadingLeaderboardScore = false;
		m_LeaderboardScoreStored = false;
		m_UploadedScoreChanged = false;
		m_UploadedScore = 0;
		m_UploadedGlobalRankNew = 0;
		m_UploadedGlobalRankPrevious = 0;
		m_DownloadingLeaderboardEntries = false;
		m_NumLeaderboardEntries = 0;
	}
}

bool SteamPlugin::SteamInitialized()
{
	return m_SteamInitialized;
}

int SteamPlugin::GetAppID()
{
	if (!m_SteamInitialized)
	{
		return 0;
	}
	return SteamUtils()->GetAppID();
}

bool SteamPlugin::LoggedOn()
{
	if (!m_SteamInitialized)
	{
		return false;
	}
	return SteamUser()->BLoggedOn();
}

const char *SteamPlugin::GetPersonaName()
{
	if (!m_SteamInitialized)
	{
		return 0;
	}
	return SteamFriends()->GetPersonaName();
}

void SteamPlugin::RunCallbacks()
{
	if (!m_SteamInitialized)
	{
		return;
	}
	SteamAPI_RunCallbacks();
}

bool SteamPlugin::RequestStats()
{
	// Set to false so that we can detect when the callback finishes for this request.
	m_StatsInitialized = false;
	if (NULL == SteamUserStats() || NULL == SteamUser())
	{
		return false;
	}
	if (!SteamUser()->BLoggedOn())
	{
		return false;
	}
	m_RequestingStats = SteamUserStats()->RequestCurrentStats();
	return m_RequestingStats;
}

// Callback for RequestStats.
void SteamPlugin::OnUserStatsReceived(UserStatsReceived_t *pCallback)
{
	if (pCallback->m_nGameID == m_AppID)
	{
		m_RequestingStats = false;
		if (pCallback->m_eResult == k_EResultOK)
		{
			m_StatsInitialized = true;
		}
	}
}

bool SteamPlugin::StoreStats()
{
	if (!m_StatsInitialized)
	{
		return false;
	}
	m_StoringStats = SteamUserStats()->StoreStats();
	return m_StoringStats;
}

bool SteamPlugin::ResetAllStats(bool bAchievementsToo)
{
	if (!m_StatsInitialized)
	{
		return false;
	}
	m_StoringStats = SteamUserStats()->ResetAllStats(bAchievementsToo);
	return m_StoringStats;
}

// Callback for StoreStats.
void SteamPlugin::OnUserStatsStored(UserStatsStored_t *pCallback)
{
	if (pCallback->m_nGameID == m_AppID)
	{
		m_StoringStats = false;
		if (pCallback->m_eResult == k_EResultOK)
		{
			m_StatsStored = true;
		}
	}
}

// Callback for StoreStats.
void SteamPlugin::OnAchievementStored(UserAchievementStored_t *pCallback)
{
	if (pCallback->m_nGameID == m_AppID)
	{
		m_AchievementStored = true;
	}
}

int SteamPlugin::GetNumAchievements()
{
	if (!m_StatsInitialized)
	{
		return 0;
	}
	return SteamUserStats()->GetNumAchievements();
}

const char *SteamPlugin::GetAchievementID(int index)
{
	if (!m_StatsInitialized)
	{
		return 0;
	}
	return SteamUserStats()->GetAchievementName(index);
}

bool SteamPlugin::GetAchievement(const char *pchName, bool *pbAchieved)
{
	if (!m_StatsInitialized)
	{
		return false;
	}
	return SteamUserStats()->GetAchievement(pchName, pbAchieved);
}

bool SteamPlugin::SetAchievement(const char *pchName)
{
	m_AchievementStored = false;
	if (!m_StatsInitialized)
	{
		return false;
	}
	return SteamUserStats()->SetAchievement(pchName);
}

bool SteamPlugin::IndicateAchievementProgress(const char *pchName, uint32 nCurProgress, uint32 nMaxProgress)
{
	if (!m_StatsInitialized)
	{
		return false;
	}
	return SteamUserStats()->IndicateAchievementProgress(pchName, nCurProgress, nMaxProgress);
}

bool SteamPlugin::ClearAchievement(const char *pchName)
{
	m_AchievementStored = false;
	if (!m_StatsInitialized)
	{
		return false;
	}
	return SteamUserStats()->ClearAchievement(pchName);
}

bool SteamPlugin::GetStat(const char *pchName, int32 *pData)
{
	if (!m_StatsInitialized)
	{
		return false;
	}
	return SteamUserStats()->GetStat(pchName, pData);
}

bool SteamPlugin::GetStat(const char *pchName, float *pData)
{
	if (!m_StatsInitialized)
	{
		return false;
	}
	return SteamUserStats()->GetStat(pchName, pData);
}

bool SteamPlugin::SetStat(const char *pchName, int32 nData)
{
	m_StatsStored = false;
	if (!m_StatsInitialized)
	{
		return false;
	}
	return SteamUserStats()->SetStat(pchName, nData);
}

bool SteamPlugin::SetStat(const char *pchName, float fData)
{
	m_StatsStored = false;
	if (!m_StatsInitialized)
	{
		return false;
	}
	return SteamUserStats()->SetStat(pchName, fData);
}

bool SteamPlugin::UpdateAvgRateStat(const char *pchName, float flCountThisSession, double dSessionLength)
{
	m_StatsStored = false;
	if (!m_StatsInitialized)
	{
		return false;
	}
	return SteamUserStats()->UpdateAvgRateStat(pchName, flCountThisSession, dSessionLength);
}

// Leaderboards
void SteamPlugin::OnFindLeaderboard(LeaderboardFindResult_t *pCallback, bool bIOFailure)
{
	m_FindingLeaderboard = false;
	if (!pCallback->m_bLeaderboardFound || bIOFailure)
	{
		// Did not find the leaderboard.
		return;
	}
	m_hSteamLeaderboard = pCallback->m_hSteamLeaderboard;
}

bool SteamPlugin::FindLeaderboard(const char *pchLeaderboardName)
{
	if (m_FindingLeaderboard)
	{
		// Already waiting for the callback.
		return false;
	}
	if (!m_StatsInitialized)
	{
		return false;
	}
	m_FindingLeaderboard = true;
	m_hSteamLeaderboard = 0;
	SteamAPICall_t hSteamAPICall = SteamUserStats()->FindLeaderboard(pchLeaderboardName);
	m_callResultFindLeaderboard.Set(hSteamAPICall, this, &SteamPlugin::OnFindLeaderboard);
	return true;
}

void SteamPlugin::OnUploadScore(LeaderboardScoreUploaded_t *pCallback, bool bIOFailure)
{
	m_UploadingLeaderboardScore = false;
	m_LeaderboardScoreStored = (pCallback->m_bSuccess && !bIOFailure);
	if (m_LeaderboardScoreStored)
	{
		m_UploadedScoreChanged = (pCallback->m_bScoreChanged != 0);
		m_UploadedScore = pCallback->m_nScore;
		m_UploadedGlobalRankNew = pCallback->m_nGlobalRankNew;
		m_UploadedGlobalRankPrevious = pCallback->m_nGlobalRankPrevious;
	}
}

bool SteamPlugin::UploadLeaderboardScore(SteamLeaderboard_t hLeaderboard, int score)
{
	if (m_UploadingLeaderboardScore)
	{
		// Already waiting for the callback.
		return false;
	}
	if (!hLeaderboard)
	{
		return false;
	}
	m_UploadingLeaderboardScore = true;
	m_LeaderboardScoreStored = false;
	m_UploadedScoreChanged = false;
	m_UploadedScore = 0;
	m_UploadedGlobalRankNew = 0;
	m_UploadedGlobalRankPrevious = 0;
	SteamAPICall_t hSteamAPICall = SteamUserStats()->UploadLeaderboardScore(hLeaderboard, k_ELeaderboardUploadScoreMethodKeepBest, score, NULL, 0);
	m_callResultUploadScore.Set(hSteamAPICall, this, &SteamPlugin::OnUploadScore);
	return true;
}

void SteamPlugin::OnDownloadScore(LeaderboardScoresDownloaded_t *pCallback, bool bIOFailure)
{
	m_DownloadingLeaderboardEntries = false;
	if (!bIOFailure)
	{
		m_NumLeaderboardEntries = min(pCallback->m_cEntryCount, MAX_LEADERBOARD_ENTRIES);
		for (int index = 0; index < m_NumLeaderboardEntries; index++)
		{
			SteamUserStats()->GetDownloadedLeaderboardEntry(pCallback->m_hSteamLeaderboardEntries, index, &m_leaderboardEntries[index], NULL, 0);
		}
	}
}

bool SteamPlugin::DownloadLeaderboardEntries(SteamLeaderboard_t hLeaderboard, ELeaderboardDataRequest eLeaderboardDataRequest, int nRangeStart, int nRangeEnd)
{
	if (m_DownloadingLeaderboardEntries)
	{
		// Already waiting for the callback.
		return false;
	}
	if (!hLeaderboard)
	{
		return false;
	}
	m_DownloadingLeaderboardEntries = true;
	SteamAPICall_t hSteamAPICall = SteamUserStats()->DownloadLeaderboardEntries(hLeaderboard, eLeaderboardDataRequest, nRangeStart, nRangeEnd);
	m_callResultDownloadScore.Set(hSteamAPICall, this, &SteamPlugin::OnDownloadScore);
	return true;
}

int SteamPlugin::GetLeaderboardEntryGlobalRank(int index)
{
	if (index < 0 || index >= m_NumLeaderboardEntries)
	{
		agk::PluginError("GetLeaderboardEntryGlobalRank index out of bounds.");
		return 0;
	}
	return m_leaderboardEntries[index].m_nGlobalRank;
}

int SteamPlugin::GetLeaderboardEntryScore(int index)
{
	if (index < 0 || index >= m_NumLeaderboardEntries)
	{
		agk::PluginError("GetLeaderboardEntryScore index out of bounds.");
		return 0;
	}
	return m_leaderboardEntries[index].m_nScore;
}

const char *SteamPlugin::GetLeaderboardEntryPersonaName(int index)
{
	if (index < 0 || index >= m_NumLeaderboardEntries)
	{
		agk::PluginError("GetLeaderboardEntryScore index out of bounds.");
		return 0;
	}
	return SteamFriends()->GetFriendPersonaName(m_leaderboardEntries[index].m_steamIDUser);;
}
