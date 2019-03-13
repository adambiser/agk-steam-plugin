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

#ifndef _CLEADERBOARDSCORESDOWNLOADEDCALLRESULT_H_
#define _CLEADERBOARDSCORESDOWNLOADEDCALLRESULT_H_
#pragma once

#include "CCallResultItem.h"
#include <vector>

class CLeaderboardScoresDownloadedCallResult : public CCallResultItem
{
public:
	CLeaderboardScoresDownloadedCallResult(SteamLeaderboard_t hLeaderboard, ELeaderboardDataRequest eLeaderboardDataRequest, int nRangeStart, int nRangeEnd) :
		CCallResultItem(),
		m_hLeaderboard(hLeaderboard),
		m_eLeaderboardDataRequest(eLeaderboardDataRequest),
		m_nRangeStart(nRangeStart),
		m_nRangeEnd(nRangeEnd) {}
	virtual ~CLeaderboardScoresDownloadedCallResult(void)
	{
		m_CallResult.Cancel();
	}
	std::string GetName() { 
		return "DownloadLeaderboardEntries("
			+ std::to_string(m_hLeaderboard) + ", "
			+ std::to_string(m_eLeaderboardDataRequest) + ", "
			+ std::to_string(m_nRangeStart) + ", "
			+ std::to_string(m_nRangeEnd) + ")";
	}
protected:
	void Call();
private:
	CCallResult<CLeaderboardScoresDownloadedCallResult, LeaderboardScoresDownloaded_t> m_CallResult;
	void OnDownloadScore(LeaderboardScoresDownloaded_t *pCallResult, bool bIOFailure);
	SteamLeaderboard_t m_hLeaderboard;
	ELeaderboardDataRequest m_eLeaderboardDataRequest;
	int m_nRangeStart;
	int m_nRangeEnd;
	std::vector<LeaderboardEntry_t> m_Entries;
};

#endif // _CLEADERBOARDSCORESDOWNLOADEDCALLRESULT_H_
