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

#ifndef _STEAMUSERSTATS_H_
#define _STEAMUSERSTATS_H_
#pragma once

#include "CCallbacks.h"

//GlobalAchievementPercentagesReady_t - Call result for RequestGlobalAchievementPercentages
//GlobalStatsReceived_t - Call result for RequestGlobalStats
//LeaderboardFindResult_t - Call result for FindOrCreateLeaderboard, FindLeaderboard
//LeaderboardScoresDownloaded_t - Call result for DownloadLeaderboardEntries, DownloadLeaderboardEntriesForUsers
//LeaderboardScoreUploaded_t - Call result for UploadLeaderboardScore
//LeaderboardUGCSet_t - Call result for AttachLeaderboardUGC
//NumberOfCurrentPlayers_t - Call result for GetNumberOfCurrentPlayers
//PS3TrophiesInstalled_t - ignore

class CUserAchievementIconFetchedCallback : public ListCallbackBase<CUserAchievementIconFetchedCallback, UserAchievementIconFetched_t>
{
public:
	void OnResponse(UserAchievementIconFetched_t *pParam)
	{
		if (pParam->m_nGameID.AppID() != g_AppID)
		{
			return;
		}
		agk::Log("Callback: OnUserAchievementIconFetched");
		// Only store the results for values that are expected.
		std::string name = pParam->m_rgchAchievementName;
		auto search = m_AchievementIconsMap.find(name);
		if (search != m_AchievementIconsMap.end())
		{
			search->second = pParam->m_nIconHandle;
		}
	}
	void Clear(UserAchievementIconFetched_t &value)
	{
		value.m_bAchieved = false;
		value.m_nGameID.Reset();
		value.m_nIconHandle = 0;
		value.m_rgchAchievementName[0] = 0;
	}
	void UnregisterInner()
	{
		ListCallbackBase::UnregisterInner();
		m_AchievementIconsMap.clear();
	}
	int GetAchievementIcon(const char *pchName)
	{
		if (!m_bRegistered)
		{
			Register();
		}
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
			m_AchievementIconsMap[name] = hImage;
		}
		return hImage;
	}
private:
	std::map<std::string, int> m_AchievementIconsMap;
};

class CUserAchievementStoredCallback : public ListCallbackBase<CUserAchievementStoredCallback, UserAchievementStored_t>
{
public:
	void OnResponse(UserAchievementStored_t *pParam)
	{
		if (pParam->m_nGameID != g_AppID)
		{
			return;
		}
		agk::Log("Callback: OnUserAchievementStored");
		StoreResponse(*pParam);
	}
	void Clear(UserAchievementStored_t &value)
	{
		value.m_bGroupAchievement = false;
		value.m_nCurProgress = 0;
		value.m_nGameID = 0;
		value.m_nMaxProgress = 0;
		value.m_rgchAchievementName[0] = 0;
	}
};

class CUserStatsReceivedCallback : public ListCallbackBase<CUserStatsReceivedCallback, UserStatsReceived_t>
{
public:
	void OnResponse(UserStatsReceived_t *pParam)
	{
		if (pParam->m_nGameID != g_AppID)
		{
			utils::Log("Callback: OnUserStatsReceived.  GameID = " + std::to_string(pParam->m_nGameID));
			return;
		}
		utils::Log("Callback: OnUserStatsReceived.  Result = " + std::to_string(pParam->m_eResult) + ", user = " + std::to_string(pParam->m_steamIDUser.ConvertToUint64()));
		if (pParam->m_eResult == k_EResultOK)
		{
			auto it = std::find(m_StatsInitializedUsers.begin(), m_StatsInitializedUsers.end(), pParam->m_steamIDUser);
			if (it == m_StatsInitializedUsers.end())
			{
				m_StatsInitializedUsers.push_back(pParam->m_steamIDUser);
			}
			// If the current user's stats were received, set a flag.
			if (pParam->m_steamIDUser == SteamUser()->GetSteamID())
			{
				m_StatsInitialized = true;
			}
		}
		StoreResponse(*pParam);
	}
	void Clear(UserStatsReceived_t &value)
	{
		value.m_eResult = (EResult)0;
		value.m_nGameID = 0;
		value.m_steamIDUser = k_steamIDNil;
	}
	void UnregisterInner()
	{
		ListCallbackBase::UnregisterInner();
		m_StatsInitialized = false;
		m_StatsInitializedUsers.clear();
	}
	bool StatsInitialized() { return m_StatsInitialized; }
	bool StatsInitializedForUser(CSteamID steamIDUser)
	{
		auto it = std::find(m_StatsInitializedUsers.begin(), m_StatsInitializedUsers.end(), steamIDUser);
		return it != m_StatsInitializedUsers.end();
	}
	void RemoveUser(CSteamID steamIDUser)
	{
		auto it = std::find(m_StatsInitializedUsers.begin(), m_StatsInitializedUsers.end(), steamIDUser);
		if (it != m_StatsInitializedUsers.end())
		{
			m_StatsInitializedUsers.erase(it);
		}
	}
private:
	bool m_StatsInitialized;
	std::vector<CSteamID> m_StatsInitializedUsers;
};

extern CUserStatsReceivedCallback UserStatsReceivedCallback;

class CUserStatsStoredCallback : public ListCallbackBase<CUserStatsStoredCallback, UserStatsStored_t>
{
public:
	CUserStatsStoredCallback() : m_bStoringStats(false) {}
	void OnResponse(UserStatsStored_t *pParam)
	{
		if (pParam->m_nGameID != g_AppID)
		{
			return;
		}
		utils::Log("Callback: OnUserStatsStored.  Result = " + std::to_string(pParam->m_eResult));
		StoreResponse(*pParam);
		m_bStoringStats = false;
	}
	void Clear(UserStatsStored_t &value)
	{
		value.m_eResult = (EResult)0;
		value.m_nGameID = 0;
	}
	bool IsStoringStats() { return m_bStoringStats; }
	void SetStoreStatsFlag() { m_bStoringStats = true; }
private:
	bool m_bStoringStats;
};

class CUserStatsUnloadedCallback : public ListCallbackBase<CUserStatsUnloadedCallback, UserStatsUnloaded_t, CSteamID>
{
public:
	void OnResponse(UserStatsUnloaded_t *pParam)
	{
		utils::Log("Callback: OnUserStatsUnloaded.  User = " + std::to_string(pParam->m_steamIDUser.ConvertToUint64()));
		StoreResponse(pParam->m_steamIDUser);
		UserStatsReceivedCallback.RemoveUser(pParam->m_steamIDUser);
	}
	void Clear(CSteamID &value)
	{
		value = k_steamIDNil;
	}
};


#endif // _STEAMUSERSTATS_H_
