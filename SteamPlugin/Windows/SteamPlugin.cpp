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
	m_RequestStatsCallbackState(Idle),
	m_StatsInitialized(false),
	m_StoreStatsCallbackState(Idle),
	m_StatsStored(false),
	m_AchievementStored(false),
	m_hSteamLeaderboard(0),
	m_FindLeaderboardCallbackState(Idle),
	m_UploadLeaderboardScoreCallbackState(Idle),
	m_LeaderboardScoreStored(false),
	m_LeaderboardScoreChanged(false),
	m_LeaderboardUploadedScore(0),
	m_LeaderboardGlobalRankNew(0),
	m_LeaderboardGlobalRankPrevious(0),
	m_DownloadLeaderboardEntriesCallbackState(Idle),
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
		m_RequestStatsCallbackState = Idle;
		m_StatsInitialized = false;
		m_StoreStatsCallbackState = Idle;
		m_StatsStored = false;
		m_AchievementStored = false;
		m_hSteamLeaderboard = 0;
		m_FindLeaderboardCallbackState = Idle;
		m_UploadLeaderboardScoreCallbackState = Idle;
		m_LeaderboardScoreStored = false;
		m_LeaderboardScoreChanged = false;
		m_LeaderboardUploadedScore = 0;
		m_LeaderboardGlobalRankNew = 0;
		m_LeaderboardGlobalRankPrevious = 0;
		m_DownloadLeaderboardEntriesCallbackState = Idle;
		m_NumLeaderboardEntries = 0;
	}
}

bool SteamPlugin::SteamInitialized()
{
	return m_SteamInitialized;
}

bool SteamPlugin::RestartAppIfNecessary(uint32 unOwnAppID)
{
	return SteamAPI_RestartAppIfNecessary(unOwnAppID);
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

// Callback for RequestStats.
void SteamPlugin::OnUserStatsReceived(UserStatsReceived_t *pCallback)
{
	if (pCallback->m_nGameID == m_AppID)
	{
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

bool SteamPlugin::StoreStats()
{
	if (!m_StatsInitialized)
	{
		return false;
	}
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
	if (!m_StatsInitialized)
	{
		return false;
	}
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

// Callback for StoreStats.
void SteamPlugin::OnUserStatsStored(UserStatsStored_t *pCallback)
{
	if (pCallback->m_nGameID == m_AppID)
	{
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
	if (!m_StatsInitialized)
	{
		return false;
	}
	return SteamUserStats()->SetStat(pchName, nData);
}

bool SteamPlugin::SetStat(const char *pchName, float fData)
{
	if (!m_StatsInitialized)
	{
		return false;
	}
	return SteamUserStats()->SetStat(pchName, fData);
}

bool SteamPlugin::UpdateAvgRateStat(const char *pchName, float flCountThisSession, double dSessionLength)
{
	if (!m_StatsInitialized)
	{
		return false;
	}
	return SteamUserStats()->UpdateAvgRateStat(pchName, flCountThisSession, dSessionLength);
}

// Leaderboards
void SteamPlugin::OnFindLeaderboard(LeaderboardFindResult_t *pCallback, bool bIOFailure)
{
	if (pCallback->m_bLeaderboardFound && !bIOFailure)
	{
		m_FindLeaderboardCallbackState = Done;
		m_hSteamLeaderboard = pCallback->m_hSteamLeaderboard;
	}
	else
	{
		// Did not find the leaderboard.
		m_FindLeaderboardCallbackState = ServerError;
	}
}

bool SteamPlugin::FindLeaderboard(const char *pchLeaderboardName)
{
	if (!m_StatsInitialized)
	{
		return false;
	}
	// Fail when the callback is currently running.
	if (m_FindLeaderboardCallbackState == Running)
	{
		return false;
	}
	try
	{
		m_hSteamLeaderboard = 0;
		SteamAPICall_t hSteamAPICall = SteamUserStats()->FindLeaderboard(pchLeaderboardName);
		m_callResultFindLeaderboard.Set(hSteamAPICall, this, &SteamPlugin::OnFindLeaderboard);
		m_FindLeaderboardCallbackState = Running;
		return true;
	}
	catch (...)
	{
		m_FindLeaderboardCallbackState = ClientError;
		return false;
	}
}

void SteamPlugin::OnUploadScore(LeaderboardScoreUploaded_t *pCallback, bool bIOFailure)
{
	if (pCallback->m_bSuccess && !bIOFailure)
	{
		m_UploadLeaderboardScoreCallbackState = Done;
		m_LeaderboardScoreStored = true;
	}
	else
	{
		m_UploadLeaderboardScoreCallbackState = ServerError;
	}
	// Report these values as Steam reports them.
	m_LeaderboardScoreChanged = (pCallback->m_bScoreChanged != 0);
	m_LeaderboardUploadedScore = pCallback->m_nScore;
	m_LeaderboardGlobalRankNew = pCallback->m_nGlobalRankNew;
	m_LeaderboardGlobalRankPrevious = pCallback->m_nGlobalRankPrevious;
}

bool SteamPlugin::UploadLeaderboardScore(SteamLeaderboard_t hLeaderboard, ELeaderboardUploadScoreMethod eLeaderboardUploadScoreMethod, int score)
{
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
		m_LeaderboardScoreStored = false;
		m_LeaderboardScoreChanged = false;
		m_LeaderboardUploadedScore = 0;
		m_LeaderboardGlobalRankNew = 0;
		m_LeaderboardGlobalRankPrevious = 0;
		SteamAPICall_t hSteamAPICall = SteamUserStats()->UploadLeaderboardScore(hLeaderboard, eLeaderboardUploadScoreMethod, score, NULL, 0);
		m_callResultUploadScore.Set(hSteamAPICall, this, &SteamPlugin::OnUploadScore);
		m_UploadLeaderboardScoreCallbackState = Running;
		return true;
	}
	catch (...)
	{
		m_UploadLeaderboardScoreCallbackState = ClientError;
		return false;
	}
}

void SteamPlugin::OnDownloadScore(LeaderboardScoresDownloaded_t *pCallback, bool bIOFailure)
{
	if (!bIOFailure)
	{
		m_DownloadLeaderboardEntriesCallbackState = Done;
		m_NumLeaderboardEntries = min(pCallback->m_cEntryCount, MAX_LEADERBOARD_ENTRIES);
		for (int index = 0; index < m_NumLeaderboardEntries; index++)
		{
			SteamUserStats()->GetDownloadedLeaderboardEntry(pCallback->m_hSteamLeaderboardEntries, index, &m_leaderboardEntries[index], NULL, 0);
		}
	}
	else
	{
		m_DownloadLeaderboardEntriesCallbackState = ServerError;
	}
}

bool SteamPlugin::DownloadLeaderboardEntries(SteamLeaderboard_t hLeaderboard, ELeaderboardDataRequest eLeaderboardDataRequest, int nRangeStart, int nRangeEnd)
{
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
		m_NumLeaderboardEntries = 0;
		SteamAPICall_t hSteamAPICall = SteamUserStats()->DownloadLeaderboardEntries(hLeaderboard, eLeaderboardDataRequest, nRangeStart, nRangeEnd);
		m_callResultDownloadScore.Set(hSteamAPICall, this, &SteamPlugin::OnDownloadScore);
		m_DownloadLeaderboardEntriesCallbackState = Running;
		return true;
	}
	catch (...)
	{
		m_DownloadLeaderboardEntriesCallbackState = ClientError;
		return false;
	}
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
