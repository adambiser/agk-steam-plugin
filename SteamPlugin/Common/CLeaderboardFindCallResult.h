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

#ifndef _CLEADERBOARDFINDCALLRESULT_H_
#define _CLEADERBOARDFINDCALLRESULT_H_
#pragma once

#include "CCallResultItem.h"

class CLeaderboardFindCallResult : public CCallResultItem
{
public:
	CLeaderboardFindCallResult(std::string leaderboardName) :
		CCallResultItem(),
		m_LeaderboardName(leaderboardName)
	{
		m_CallResultName = "FindLeaderboard(" + m_LeaderboardName + ")";
		m_LeaderboardFindResult.m_bLeaderboardFound = 0;
		m_LeaderboardFindResult.m_hSteamLeaderboard = 0;
	}
	virtual ~CLeaderboardFindCallResult(void)
	{
		m_CallResult.Cancel();
	}
	LeaderboardFindResult_t GetLeaderboardFindResult();
protected:
	void Call();
private:
	CCallResult<CLeaderboardFindCallResult, LeaderboardFindResult_t> m_CallResult;
	void OnFindLeaderboard(LeaderboardFindResult_t *pCallResult, bool bIOFailure);
	std::string m_LeaderboardName;
	LeaderboardFindResult_t m_LeaderboardFindResult;
};

#endif // _CLEADERBOARDFINDCALLRESULT_H_
