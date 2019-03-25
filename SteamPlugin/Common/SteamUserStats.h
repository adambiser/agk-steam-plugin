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

class CAttachLeaderboardUGCCallResult : public CCallResultItem<LeaderboardUGCSet_t>
{
public:
	CAttachLeaderboardUGCCallResult(SteamLeaderboard_t hSteamLeaderboard, UGCHandle_t hUGC)
	{
		m_CallResultName = "AttachLeaderboardUGC(" 
			+ std::to_string(hSteamLeaderboard) + ", " 
			+ std::to_string(hUGC) + ")";
		m_hSteamAPICall = SteamUserStats()->AttachLeaderboardUGC(hSteamLeaderboard, hUGC);
	}
};

struct PluginLeaderboardFindResult_t : LeaderboardFindResult_t
{
	EResult m_eResult;

	PluginLeaderboardFindResult_t& operator=(const LeaderboardFindResult_t &from)
	{
		m_hSteamLeaderboard = from.m_hSteamLeaderboard;
		m_bLeaderboardFound = from.m_bLeaderboardFound;
		m_eResult = m_bLeaderboardFound ? k_EResultOK : k_EResultFail;
		return *this;
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
		else
		{
			m_CallResultName = "FindOrCreateLeaderboard('" + std::string(pchLeaderboardName) + "', "
				+ std::to_string(eLeaderboardSortMethod) + ", "
				+ std::to_string(eLeaderboardDisplayType) + ")";
			m_hSteamAPICall = SteamUserStats()->FindOrCreateLeaderboard(pchLeaderboardName, eLeaderboardSortMethod, eLeaderboardDisplayType);
		}
	}
	int GetLeaderboardFindResultFound() { return m_Response.m_bLeaderboardFound; }
	SteamLeaderboard_t GetLeaderboardFindResultHandle()	{ return m_Response.m_hSteamLeaderboard; }
};

struct PluginLeaderboardEntry_t : LeaderboardEntry_t
{
	int32 m_Details[k_cLeaderboardDetailsMax];

	PluginLeaderboardEntry_t& operator=(const LeaderboardEntry_t &from)
	{
		m_steamIDUser = from.m_steamIDUser;
		m_nGlobalRank = from.m_nGlobalRank;
		m_nScore = from.m_nScore;
		m_cDetails = from.m_cDetails;
		m_hUGC = from.m_hUGC;
		return *this;
	}
};

struct PluginLeaderboardScoresDownloaded_t : LeaderboardScoresDownloaded_t
{
	EResult m_eResult;
	std::vector<PluginLeaderboardEntry_t> m_Entries;

	PluginLeaderboardScoresDownloaded_t& operator=(const LeaderboardScoresDownloaded_t &from)
	{
		m_hSteamLeaderboard = from.m_hSteamLeaderboard;
		m_cEntryCount = from.m_cEntryCount;
		m_eResult = from.m_hSteamLeaderboardEntries ? k_EResultOK : k_EResultFail;
		return *this;
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
	int GetLeaderboardEntryDetailsCount(int index) { return m_Response.m_Entries[index].m_cDetails; }
	int32 GetLeaderboardEntryDetails(int index, int detailIndex) { return m_Response.m_Entries[index].m_Details[detailIndex]; }
	uint64 GetLeaderboardEntryUGCHandle(int index) { return m_Response.m_Entries[index].m_hUGC; }
protected:
	void OnResponse(LeaderboardScoresDownloaded_t *pCallResult, bool bFailure)
	{
		CCallResultItem::OnResponse(pCallResult, bFailure);
		if (pCallResult->m_hSteamLeaderboardEntries != 0)
		{
			for (int index = 0; index < pCallResult->m_cEntryCount; index++)
			{
				// TODO Add Details support.
				LeaderboardEntry_t entry;
				int32 details[k_cLeaderboardDetailsMax];
				if (SteamUserStats()->GetDownloadedLeaderboardEntry(pCallResult->m_hSteamLeaderboardEntries, index, &entry, details, k_cLeaderboardDetailsMax))
				{
					PluginLeaderboardEntry_t entry2;
					entry2 = entry;
					memcpy_s(entry2.m_Details, k_cLeaderboardDetailsMax, details, k_cLeaderboardDetailsMax);
					m_Response.m_Entries.push_back(entry2);
				}
				else
				{
					// Report error.
					utils::Log(GetName() + ": GetDownloadedLeaderboardEntry failed for index " + std::to_string(index) + ".");
					m_Response.m_eResult = k_EResultFail;
					return;
				}
			}
		}
	}
};

struct PluginLeaderboardScoreUploaded_t : LeaderboardScoreUploaded_t
{
	EResult m_eResult;

	PluginLeaderboardScoreUploaded_t& operator=(const LeaderboardScoreUploaded_t &from)
	{
		m_bSuccess = from.m_bSuccess;
		m_hSteamLeaderboard = from.m_hSteamLeaderboard;
		m_nScore = from.m_nScore;
		m_bScoreChanged = from.m_bScoreChanged;
		m_nGlobalRankNew = from.m_nGlobalRankNew;
		m_nGlobalRankPrevious = from.m_nGlobalRankPrevious;
		m_eResult = m_bSuccess ? k_EResultOK : k_EResultFail;
		return *this;
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
		m_hSteamAPICall = SteamUserStats()->UploadLeaderboardScore(hLeaderboard, eLeaderboardUploadScoreMethod, nScore, s_Details, s_DetailCount);
		// If the call succeeds, clear the details.
		if (m_hSteamAPICall != k_uAPICallInvalid)
		{
			s_DetailCount = 0;
		}
	}
	int GetLeaderboardScoreUploadedSuccess() { return m_Response.m_bSuccess; }
	uint64 GetLeaderboardScoreUploadedHandle() { return m_Response.m_hSteamLeaderboard; }
	int GetLeaderboardScoreUploadedScore() { return m_Response.m_nScore; }
	int GetLeaderboardScoreUploadedScoreChanged() { return m_Response.m_bScoreChanged; }
	int GetLeaderboardScoreUploadedRankNew() { return m_Response.m_nGlobalRankNew; }
	int GetLeaderboardScoreUploadedRankPrevious() { return m_Response.m_nGlobalRankPrevious; }
	static bool AddDetail(int32 detail) {
		if (s_DetailCount == k_cLeaderboardDetailsMax)
		{
			return false;
		}
		s_Details[s_DetailCount] = detail;
		s_DetailCount++;
		return true;
	}
protected:
	static int32 s_Details[k_cLeaderboardDetailsMax];
	static int32 s_DetailCount;
};
// Normally unsafe to initialize static variables in header, but we're assigning a constant value and only one source file uses this header.
int32 CLeaderboardScoreUploadedCallResult::s_Details[k_cLeaderboardDetailsMax];
int32 CLeaderboardScoreUploadedCallResult::s_DetailCount = 0;

struct PluginNumberOfCurrentPlayers_t : NumberOfCurrentPlayers_t
{
	EResult m_eResult;

	PluginNumberOfCurrentPlayers_t& operator=(const NumberOfCurrentPlayers_t &from)
	{
		this->m_bSuccess = from.m_bSuccess;
		this->m_cPlayers = from.m_cPlayers;
		this->m_eResult = from.m_bSuccess ? k_EResultOK : k_EResultFail;
		return *this;
	}
};

class CNumberOfCurrentPlayersCallResult : public CCallResultItem<NumberOfCurrentPlayers_t, PluginNumberOfCurrentPlayers_t>
{
public:
	CNumberOfCurrentPlayersCallResult()
	{
		m_CallResultName = "GetNumberOfCurrentPlayers()";
		m_hSteamAPICall = SteamUserStats()->GetNumberOfCurrentPlayers();
	}
	int GetNumberOfPlayers() { return m_Response.m_cPlayers; }
};

class CRequestGlobalAchievementPercentagesCallResult : public CCallResultItem<GlobalAchievementPercentagesReady_t>
{
public:
	CRequestGlobalAchievementPercentagesCallResult()
	{
		m_CallResultName = "RequestGlobalAchievementPercentages()";
		m_hSteamAPICall = SteamUserStats()->RequestGlobalAchievementPercentages();
	}
protected:
	void OnResponse(GlobalAchievementPercentagesReady_t *pCallResult, bool bFailure)
	{
		if (pCallResult->m_nGameID != g_AppID)
		{
			utils::Log(GetName() + ":  Received result for another app id: " + std::to_string(pCallResult->m_nGameID));
			return;
		}
		CCallResultItem::OnResponse(pCallResult, bFailure);
	}
};

class CGlobalStatsReceivedCallResult : public CCallResultItem<GlobalStatsReceived_t>
{
public:
	CGlobalStatsReceivedCallResult(int nHistoryDays)
	{
		m_CallResultName = "RequestGlobalStats(" + std::to_string(nHistoryDays) + ")";
		m_hSteamAPICall = SteamUserStats()->RequestGlobalStats(nHistoryDays);
	}
protected:
	void OnResponse(GlobalStatsReceived_t *pCallResult, bool bFailure)
	{
		if (pCallResult->m_nGameID != g_AppID)
		{
			utils::Log(GetName() + ":  Received result for another app id: " + std::to_string(pCallResult->m_nGameID));
			return;
		}
		CCallResultItem::OnResponse(pCallResult, bFailure);
	}
};

#endif // _STEAMUSERSTATS_H_
