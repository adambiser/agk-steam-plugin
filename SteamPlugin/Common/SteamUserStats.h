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

#include "CCallResultItem.h"

struct PluginLeaderboardFindResult_t : LeaderboardFindResult_t
{
	EResult m_eResult;

	PluginLeaderboardFindResult_t() : m_eResult((EResult)0) {}

	PluginLeaderboardFindResult_t(LeaderboardFindResult_t param) : PluginLeaderboardFindResult_t()
	{
		m_hSteamLeaderboard = param.m_hSteamLeaderboard;
		m_bLeaderboardFound = param.m_bLeaderboardFound;
		// Assign a result based on m_bLeaderboardFound.
		m_eResult = m_bLeaderboardFound ? k_EResultOK : k_EResultFail;
	}
};

class CLeaderboardFindCallResult : public CCallResultItem<LeaderboardFindResult_t, PluginLeaderboardFindResult_t>
{
public:
	CLeaderboardFindCallResult(const char *pchLeaderboardName, ELeaderboardSortMethod eLeaderboardSortMethod = k_ELeaderboardSortMethodNone, ELeaderboardDisplayType eLeaderboardDisplayType = k_ELeaderboardDisplayTypeNone)
	{
		// No sort and/or no display type = find only.
		if (eLeaderboardSortMethod == k_ELeaderboardSortMethodNone || eLeaderboardDisplayType == k_ELeaderboardDisplayTypeNone)
		{
			m_CallResultName = "FindLeaderboard('" + std::string(pchLeaderboardName) + "')";
			m_hSteamAPICall = SteamUserStats()->FindLeaderboard(pchLeaderboardName);
		}
		m_CallResultName = "FindOrCreateLeaderboard('" + std::string(pchLeaderboardName) + "', "
			+ std::to_string(eLeaderboardSortMethod) + ", "
			+ std::to_string(eLeaderboardDisplayType) + ")";
		m_hSteamAPICall = SteamUserStats()->FindOrCreateLeaderboard(pchLeaderboardName, eLeaderboardSortMethod, eLeaderboardDisplayType);
	}
	int GetLeaderboardFindResultFound() { return m_Response.m_bLeaderboardFound; }
	SteamLeaderboard_t GetLeaderboardFindResultHandle()	{ return m_Response.m_hSteamLeaderboard; }
};

struct PluginLeaderboardScoresDownloaded_t : LeaderboardScoresDownloaded_t
{
	EResult m_eResult;
	std::vector<LeaderboardEntry_t> m_Entries;

	PluginLeaderboardScoresDownloaded_t() : m_eResult((EResult)0) {}

	PluginLeaderboardScoresDownloaded_t(LeaderboardScoresDownloaded_t param) : PluginLeaderboardScoresDownloaded_t()
	{
		m_hSteamLeaderboard = param.m_hSteamLeaderboard;
		m_cEntryCount = param.m_cEntryCount;
		if (param.m_hSteamLeaderboardEntries == 0)
		{
			m_eResult = k_EResultFail;
		}
		else
		{
			for (int index = 0; index < param.m_cEntryCount; index++)
			{
				// TODO Add Details support.
				LeaderboardEntry_t entry;
				if (SteamUserStats()->GetDownloadedLeaderboardEntry(param.m_hSteamLeaderboardEntries, index, &entry, NULL, 0))
				{
					m_Entries.push_back(entry);
				}
				else
				{
					// Report error.
					utils::Log("GetDownloadedLeaderboardEntry failed for index " + std::to_string(index) + ".");
					m_eResult = k_EResultFail;
					return;
				}
			}
			m_eResult = k_EResultOK;
		}
	}
};

class CLeaderboardScoresDownloadedCallResult : public CCallResultItem<LeaderboardScoresDownloaded_t, PluginLeaderboardScoresDownloaded_t>
{
public:
	CLeaderboardScoresDownloadedCallResult(SteamLeaderboard_t hLeaderboard, ELeaderboardDataRequest eLeaderboardDataRequest, int nRangeStart, int nRangeEnd)
	{
		m_CallResultName = "DownloadLeaderboardEntries("
			+ std::to_string(hLeaderboard) + ", "
			+ std::to_string(eLeaderboardDataRequest) + ", "
			+ std::to_string(nRangeStart) + ", "
			+ std::to_string(nRangeEnd) + ")";
		m_hSteamAPICall = SteamUserStats()->DownloadLeaderboardEntries(hLeaderboard, eLeaderboardDataRequest, nRangeStart, nRangeEnd);
	}
	uint64 GetLeaderboardID() { return m_Response.m_hSteamLeaderboard; }
	int GetLeaderboardEntryCount() { return (int)m_Response.m_Entries.size(); }
	bool IsValidIndex(int index) { return (index >= 0) && (index < (int)m_Response.m_Entries.size()); }
	uint64 GetLeaderboardEntryUser(int index) { return m_Response.m_Entries[index].m_steamIDUser.ConvertToUint64(); }
	int GetLeaderboardEntryGlobalRank(int index) { return m_Response.m_Entries[index].m_nGlobalRank; }
	int GetLeaderboardEntryScore(int index) { return m_Response.m_Entries[index].m_nScore; }
	int GetLeaderboardEntryDetails(int index) { return m_Response.m_Entries[index].m_cDetails; }
	uint64 GetLeaderboardEntryUGCHandle(int index) { return m_Response.m_Entries[index].m_hUGC; }
};

struct PluginLeaderboardScoreUploaded_t : LeaderboardScoreUploaded_t
{
	EResult m_eResult;

	PluginLeaderboardScoreUploaded_t() : m_eResult((EResult)0) {}

	PluginLeaderboardScoreUploaded_t(LeaderboardScoreUploaded_t param) : PluginLeaderboardScoreUploaded_t()
	{
		m_bSuccess = param.m_bSuccess;
		m_hSteamLeaderboard = param.m_hSteamLeaderboard;
		m_nScore = param.m_nScore;
		m_bScoreChanged = param.m_bScoreChanged;
		m_nGlobalRankNew = param.m_nGlobalRankNew;
		m_nGlobalRankPrevious = param.m_nGlobalRankPrevious;
		m_eResult = m_bSuccess ? k_EResultOK : k_EResultFail;
	}
};

class CLeaderboardScoreUploadedCallResult : public CCallResultItem<LeaderboardScoreUploaded_t, PluginLeaderboardScoreUploaded_t>
{
public:
	CLeaderboardScoreUploadedCallResult(SteamLeaderboard_t hLeaderboard, ELeaderboardUploadScoreMethod eLeaderboardUploadScoreMethod, int nScore)
	{
		m_CallResultName = "UploadLeaderboardScore(" 
			+ std::to_string(hLeaderboard) + ", " 
			+ std::to_string(eLeaderboardUploadScoreMethod) + ", " 
			+ std::to_string(nScore) + ")";
		m_hSteamAPICall = SteamUserStats()->UploadLeaderboardScore(hLeaderboard, eLeaderboardUploadScoreMethod, nScore, NULL, 0);
	}
	int GetLeaderboardScoreUploadedSuccess() { return m_Response.m_bSuccess; }
	uint64 GetLeaderboardScoreUploadedHandle() { return m_Response.m_hSteamLeaderboard; }
	int GetLeaderboardScoreUploadedScore() { return m_Response.m_nScore; }
	int GetLeaderboardScoreUploadedScoreChanged() { return m_Response.m_bScoreChanged; }
	int GetLeaderboardScoreUploadedRankNew() { return m_Response.m_nGlobalRankNew; }
	int GetLeaderboardScoreUploadedRankPrevious() { return m_Response.m_nGlobalRankPrevious; }
};

class CGlobalStatsReceivedCallResult : public CCallResultItem<GlobalStatsReceived_t>
{
public:
	CGlobalStatsReceivedCallResult(int nHistoryDays)
	{
		m_CallResultName = "RequestGlobalStats(" + std::to_string(nHistoryDays) + ")";
		m_hSteamAPICall = SteamUserStats()->RequestGlobalStats(nHistoryDays);
	}
private:
	void OnResponse(GlobalStatsReceived_t *pCallResult, bool bFailure)
	{
		if (pCallResult->m_nGameID != g_AppID)
		{
			utils::Log(GetName() + ":  Received stats for another app id: " + std::to_string(pCallResult->m_nGameID));
			return;
		}
		m_Response = *pCallResult;
		SetResultCode(m_Response.m_eResult, bFailure);
		utils::Log(GetName() + ":  Result code = " + std::to_string(m_Response.m_eResult));
	}
};

#endif // _STEAMUSERSTATS_H_
