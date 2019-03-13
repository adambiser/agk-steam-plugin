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

#ifndef _CLEADERBOARDSCOREUPLOADEDCALLRESULT_H_
#define _CLEADERBOARDSCOREUPLOADEDCALLRESULT_H_
#pragma once

#include "CCallResultItem.h"

class CLeaderboardScoreUploadedCallResult : public CCallResultItem
{
public:
	CLeaderboardScoreUploadedCallResult(SteamLeaderboard_t hLeaderboard, ELeaderboardUploadScoreMethod m_eLeaderboardUploadScoreMethod, int nScore) :
		CCallResultItem(),
		m_hLeaderboard(hLeaderboard),
		m_eLeaderboardUploadScoreMethod(m_eLeaderboardUploadScoreMethod),
		m_nScore(nScore)
	{
		//m_LeaderboardScoreUploaded.m_bScoreChanged = 0;
		//m_LeaderboardScoreUploaded.m_bSuccess = 0;
		//m_LeaderboardScoreUploaded.m_hSteamLeaderboard = 0;
		//m_LeaderboardScoreUploaded.m_nGlobalRankNew = 0;
		//m_LeaderboardScoreUploaded.m_nGlobalRankPrevious = 0;
		//m_LeaderboardScoreUploaded.m_nScore = 0;
	}
	virtual ~CLeaderboardScoreUploadedCallResult(void)
	{
		m_CallResult.Cancel();
	}
	std::string GetName()
	{
		return "UploadLeaderboardScore("
			+ std::to_string(m_hLeaderboard) + ", "
			+ std::to_string(m_eLeaderboardUploadScoreMethod) + ", "
			+ std::to_string(m_nScore) + ")";
	}
protected:
	void Call();
private:
	CCallResult<CLeaderboardScoreUploadedCallResult, LeaderboardScoreUploaded_t> m_CallResult;
	void OnUploadScore(LeaderboardScoreUploaded_t *pCallResult, bool bIOFailure);
	SteamLeaderboard_t m_hLeaderboard;
	ELeaderboardUploadScoreMethod m_eLeaderboardUploadScoreMethod;
	int m_nScore;
	LeaderboardScoreUploaded_t m_LeaderboardScoreUploaded;
};

#endif // _CLEADERBOARDSCOREUPLOADEDCALLRESULT_H_
