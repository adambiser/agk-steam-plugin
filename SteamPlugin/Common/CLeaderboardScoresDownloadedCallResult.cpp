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

#include "CLeaderboardScoresDownloadedCallResult.h"

#define CheckEntryIndex(index, returnValue) \
	if (index < 0 || index >= (int)m_Entries.size()) \
	{ \
		utils::PluginError(GetName() + ": Index out of range: " + std::to_string(index)); \
		return returnValue; \
	}

void CLeaderboardScoresDownloadedCallResult::OnDownloadScore(LeaderboardScoresDownloaded_t *pCallResult, bool bIOFailure)
{
	if (!bIOFailure)
	{
		utils::Log(GetName() + ": Succeeded.");
		m_State = Done;
		for (int index = 0; index < pCallResult->m_cEntryCount; index++)
		{
			LeaderboardEntry_t entry;
			SteamUserStats()->GetDownloadedLeaderboardEntry(pCallResult->m_hSteamLeaderboardEntries, index, &entry, NULL, 0);
			m_Entries.push_back(entry);
		}
	}
	else
	{
		utils::Log(GetName() + ": Failed.");
		m_State = ServerError;
	}
}

void CLeaderboardScoresDownloadedCallResult::Call()
{
	m_hSteamAPICall = SteamUserStats()->DownloadLeaderboardEntries(m_hLeaderboard, m_eLeaderboardDataRequest, m_nRangeStart, m_nRangeEnd);
	if (m_hSteamAPICall == k_uAPICallInvalid)
	{
		throw std::string(GetName() + ": Call returned k_uAPICallInvalid.");
	}
	m_CallResult.Set(m_hSteamAPICall, this, &CLeaderboardScoresDownloadedCallResult::OnDownloadScore);
}

int CLeaderboardScoresDownloadedCallResult::GetEntryGlobalRank(int index)
{
	CheckEntryIndex(index, 0);
	return m_Entries[index].m_nGlobalRank;
}

int CLeaderboardScoresDownloadedCallResult::GetEntryScore(int index)
{
	CheckEntryIndex(index, 0);
	return m_Entries[index].m_nScore;
}

CSteamID CLeaderboardScoresDownloadedCallResult::GetEntryUser(int index)
{
	CheckEntryIndex(index, k_steamIDNil);
	return m_Entries[index].m_steamIDUser;
}
