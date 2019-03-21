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

namespace wrapper
{
	struct LeaderboardFindResult_t : ::LeaderboardFindResult_t
	{
		EResult m_eResult;

		LeaderboardFindResult_t() : m_eResult((EResult)0) {}

		LeaderboardFindResult_t(::LeaderboardFindResult_t param) : LeaderboardFindResult_t()
		{
			m_hSteamLeaderboard = param.m_hSteamLeaderboard;
			m_bLeaderboardFound = param.m_bLeaderboardFound;
			// Assigned a result based on m_bLeaderboardFound.
			m_eResult = m_bLeaderboardFound ? k_EResultOK : k_EResultFail;
		}
	};

	struct LeaderboardScoresDownloaded_t : ::LeaderboardScoresDownloaded_t
	{
		EResult m_eResult;
		std::vector<LeaderboardEntry_t> m_Entries;

		LeaderboardScoresDownloaded_t() : m_eResult((EResult)0) {}

		LeaderboardScoresDownloaded_t(::LeaderboardScoresDownloaded_t param) : LeaderboardScoresDownloaded_t()
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

	struct LeaderboardScoreUploaded_t : ::LeaderboardScoreUploaded_t
	{
		EResult m_eResult;

		LeaderboardScoreUploaded_t() : m_eResult((EResult)0) {}

		LeaderboardScoreUploaded_t(::LeaderboardScoreUploaded_t param) : LeaderboardScoreUploaded_t()
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
}

class CLeaderboardFindCallResult : public CCallResultItem<LeaderboardFindResult_t, wrapper::LeaderboardFindResult_t>
{
public:
	CLeaderboardFindCallResult(std::string leaderboardName) :
		m_LeaderboardName(leaderboardName)
	{
		m_CallResultName = "FindLeaderboard('" + m_LeaderboardName + "')";
		m_Response.m_bLeaderboardFound = 0;
		m_Response.m_hSteamLeaderboard = 0;
	}
	int GetLeaderboardFindResultFound() { return m_Response.m_bLeaderboardFound; }
	SteamLeaderboard_t GetLeaderboardFindResultHandle()	{ return m_Response.m_hSteamLeaderboard; }
protected:
	SteamAPICall_t Call()
	{
		return SteamUserStats()->FindLeaderboard(m_LeaderboardName.c_str());;
	}
private:
	std::string m_LeaderboardName;
};

class CLeaderboardScoresDownloadedCallResult : public CCallResultItem<LeaderboardScoresDownloaded_t, wrapper::LeaderboardScoresDownloaded_t>
{
public:
	CLeaderboardScoresDownloadedCallResult(SteamLeaderboard_t hLeaderboard, ELeaderboardDataRequest eLeaderboardDataRequest, int nRangeStart, int nRangeEnd) :
		m_hLeaderboard(hLeaderboard),
		m_eLeaderboardDataRequest(eLeaderboardDataRequest),
		m_nRangeStart(nRangeStart),
		m_nRangeEnd(nRangeEnd)
	{
		m_CallResultName = "DownloadLeaderboardEntries("
			+ std::to_string(m_hLeaderboard) + ", "
			+ std::to_string(m_eLeaderboardDataRequest) + ", "
			+ std::to_string(m_nRangeStart) + ", "
			+ std::to_string(m_nRangeEnd) + ")";
	}
	uint64 GetLeaderboardID() { return m_Response.m_hSteamLeaderboard; }
	int GetLeaderboardEntryCount() { return (int)m_Response.m_Entries.size(); }
	bool IsValidIndex(int index) { return (index >= 0) && (index < (int)m_Response.m_Entries.size()); }
	uint64 GetLeaderboardEntryUser(int index) { return m_Response.m_Entries[index].m_steamIDUser.ConvertToUint64(); }
	int GetLeaderboardEntryGlobalRank(int index) { return m_Response.m_Entries[index].m_nGlobalRank; }
	int GetLeaderboardEntryScore(int index) { return m_Response.m_Entries[index].m_nScore; }
	int GetLeaderboardEntryDetails(int index) { return m_Response.m_Entries[index].m_cDetails; }
	uint64 GetLeaderboardEntryUGCHandle(int index) { return m_Response.m_Entries[index].m_hUGC; }
protected:
	SteamAPICall_t Call()
	{
		return SteamUserStats()->DownloadLeaderboardEntries(m_hLeaderboard, m_eLeaderboardDataRequest, m_nRangeStart, m_nRangeEnd);
	}
private:
	SteamLeaderboard_t m_hLeaderboard;
	ELeaderboardDataRequest m_eLeaderboardDataRequest;
	int m_nRangeStart;
	int m_nRangeEnd;
};

class CLeaderboardScoreUploadedCallResult : public CCallResultItem<LeaderboardScoreUploaded_t, wrapper::LeaderboardScoreUploaded_t>
{
public:
	CLeaderboardScoreUploadedCallResult(SteamLeaderboard_t hLeaderboard, ELeaderboardUploadScoreMethod m_eLeaderboardUploadScoreMethod, int nScore) :
		m_hLeaderboard(hLeaderboard),
		m_eLeaderboardUploadScoreMethod(m_eLeaderboardUploadScoreMethod),
		m_nScore(nScore)
	{
		m_CallResultName = "UploadLeaderboardScore("
			+ std::to_string(m_hLeaderboard) + ", "
			+ std::to_string(m_eLeaderboardUploadScoreMethod) + ", "
			+ std::to_string(m_nScore) + ")";
		m_Response.m_bScoreChanged = 0;
		m_Response.m_bSuccess = 0;
		m_Response.m_hSteamLeaderboard = 0;
		m_Response.m_nGlobalRankNew = 0;
		m_Response.m_nGlobalRankPrevious = 0;
		m_Response.m_nScore = 0;
	}
	int GetLeaderboardScoreUploadedSuccess() { return m_Response.m_bSuccess; }
	uint64 GetLeaderboardScoreUploadedHandle() { return m_Response.m_hSteamLeaderboard; }
	int GetLeaderboardScoreUploadedScore() { return m_Response.m_nScore; }
	int GetLeaderboardScoreUploadedScoreChanged() { return m_Response.m_bScoreChanged; }
	int GetLeaderboardScoreUploadedRankNew() { return m_Response.m_nGlobalRankNew; }
	int GetLeaderboardScoreUploadedRankPrevious() { return m_Response.m_nGlobalRankPrevious; }
protected:
	SteamAPICall_t Call()
	{
		return SteamUserStats()->UploadLeaderboardScore(m_hLeaderboard, m_eLeaderboardUploadScoreMethod, m_nScore, NULL, 0);
	}
private:
	SteamLeaderboard_t m_hLeaderboard;
	ELeaderboardUploadScoreMethod m_eLeaderboardUploadScoreMethod;
	int m_nScore;
};

#endif // _STEAMUSERSTATS_H_
