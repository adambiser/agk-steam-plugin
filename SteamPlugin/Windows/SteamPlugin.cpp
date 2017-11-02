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

#pragma comment(lib, "steam_api.lib")

SteamPlugin::SteamPlugin() :
	m_AppID(0),
	m_SteamInitialized(false),
	m_StatsInitialized(false),
	m_StatsStored(false),
	m_AchievementStored(false),
	m_hSteamLeaderboard(0),
	m_LeaderboardFindResultReceived(false),
	//m_ErrorCode(0),
	m_CallbackAchievementStored(this, &SteamPlugin::OnAchievementStored),
	m_CallbackUserStatsReceived(this, &SteamPlugin::OnUserStatsReceived),
	m_CallbackUserStatsStored(this, &SteamPlugin::OnUserStatsStored),
	m_CallbackLeaderboardFindResult(this, &SteamPlugin::OnLeaderboardFindResult)
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
		m_StatsInitialized = false;
		m_StatsStored = false;
		m_AchievementStored = false;
		m_hSteamLeaderboard = 0;
		m_LeaderboardFindResultReceived = false;
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
	return SteamUserStats()->RequestCurrentStats();
}

// Callback for RequestStats.
void SteamPlugin::OnUserStatsReceived(UserStatsReceived_t *pCallback)
{
	if (pCallback->m_nGameID == m_AppID)
	{
		if (pCallback->m_eResult == k_EResultOK)
		{
			m_StatsInitialized = true;
		}
	}
}

bool SteamPlugin::StatsInitialized()
{
	return m_StatsInitialized;
}

bool SteamPlugin::StoreStats()
{
	if (!m_StatsInitialized)
	{
		return false;
	}
	return SteamUserStats()->StoreStats();
}

bool SteamPlugin::ResetAllStats(bool bAchievementsToo)
{
	if (!m_StatsInitialized)
	{
		return false;
	}
	return SteamUserStats()->ResetAllStats(bAchievementsToo);
}

// Callback for StoreStats.
void SteamPlugin::OnUserStatsStored(UserStatsStored_t *pCallback)
{
	if (pCallback->m_nGameID == m_AppID)
	{
		if (pCallback->m_eResult == k_EResultOK)
		{
			m_StatsStored = true;
		}
	}
}

bool SteamPlugin::StatsStored()
{
	return m_StatsStored;
}

// Callback for StoreStats.
void SteamPlugin::OnAchievementStored(UserAchievementStored_t *pCallback)
{
	if (pCallback->m_nGameID == m_AppID)
	{
		m_AchievementStored = true;
	}
}

bool SteamPlugin::AchievementStored()
{
	return m_AchievementStored;
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

void SteamPlugin::OnLeaderboardFindResult(LeaderboardFindResult_t *pCallback)
{
	m_LeaderboardFindResultReceived = true;
	m_hSteamLeaderboard = pCallback->m_hSteamLeaderboard;
}

SteamAPICall_t SteamPlugin::FindLeaderboard(const char *pchLeaderboardName)
{
	m_LeaderboardFindResultReceived = false;
	if (!m_StatsInitialized)
	{
		return 0;
	}
	return SteamUserStats()->FindLeaderboard(pchLeaderboardName);
}

bool SteamPlugin::LeaderboardFindResultReceived()
{
	return m_LeaderboardFindResultReceived;
}

SteamLeaderboard_t SteamPlugin::GetLeaderboardFindResultHandle()
{
	return m_hSteamLeaderboard;
}

//int GetLeaderboardEntryCount( SteamLeaderboard_t hSteamLeaderboard ) = 0;
//virtual ELeaderboardSortMethod GetLeaderboardSortMethod(SteamLeaderboard_t hSteamLeaderboard) = 0;
//	virtual ELeaderboardDisplayType GetLeaderboardDisplayType( SteamLeaderboard_t hSteamLeaderboard ) = 0;
//CALL_RESULT(LeaderboardScoresDownloaded_t)
//virtual SteamAPICall_t DownloadLeaderboardEntries(SteamLeaderboard_t hSteamLeaderboard, ELeaderboardDataRequest eLeaderboardDataRequest, int nRangeStart, int nRangeEnd) = 0;
//CALL_RESULT(LeaderboardScoresDownloaded_t)
//virtual SteamAPICall_t DownloadLeaderboardEntriesForUsers(SteamLeaderboard_t hSteamLeaderboard,
//	ARRAY_COUNT_D(cUsers, Array of users to retrieve) CSteamID *prgUsers, int cUsers) = 0;


/*
DLL_EXPORT int GetI()
{
    return i + 10;
}

DLL_EXPORT float AddFloat(float a, float b)
{
    return a + b;
}

DLL_EXPORT char* ModifyString(const char* str)
{
	int length = (int) strlen(str) + 1;
	char* str2 = agk::CreateString( length );
	strcpy_s( str2, length, str );
	str2[0] = 'A';
    return str2;
}

DLL_EXPORT void DLLPrint(const char* str)
{
    agk::Print( str );
}

DLL_EXPORT void CreateRedSquare()
{
	agk::CreateSprite( 1, 0 );
	agk::SetSpriteSize( 1, 100, 100 );
	agk::SetSpritePosition( 1, 10, 100 );
	agk::SetSpriteColor( 1, 255,0,0,255 );
}

DLL_EXPORT void LotsOfParams( int a, int b, float c, float d, int e, const char* f, float g )
{
	char str[256];
	sprintf( str, "a: %d, b: %d, c: %f, d: %f, e: %d, f: %s, g: %f", a, b, c, d, e, f, g );
	agk::Print( str );
}
*/