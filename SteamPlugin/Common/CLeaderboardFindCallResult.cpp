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

#include "CLeaderboardFindCallResult.h"

void CLeaderboardFindCallResult::OnFindLeaderboard(LeaderboardFindResult_t *pCallResult, bool bIOFailure)
{
	m_LeaderboardFindResult = *pCallResult;
	if (!bIOFailure)
	{
		utils::Log(GetName() + ": Succeeded.");
		m_eResult = k_EResultOK;
	}
	else
	{
		utils::Log(GetName() + ": Failed.");
		m_eResult = k_EResultFail;
	}
}

LeaderboardFindResult_t CLeaderboardFindCallResult::GetLeaderboardFindResult()
{
	return m_LeaderboardFindResult;
}

void CLeaderboardFindCallResult::Call()
{
	m_hSteamAPICall = SteamUserStats()->FindLeaderboard(m_LeaderboardName.c_str());
	if (m_hSteamAPICall == k_uAPICallInvalid)
	{
		throw std::string(GetName() + ": Call returned k_uAPICallInvalid.");
	}
	m_CallResult.Set(m_hSteamAPICall, this, &CLeaderboardFindCallResult::OnFindLeaderboard);
}
