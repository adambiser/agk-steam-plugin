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

#include "CLeaderboardScoreUploadedCallResult.h"

void CLeaderboardScoreUploadedCallResult::OnUploadScore(LeaderboardScoreUploaded_t *pCallResult, bool bIOFailure)
{
	m_LeaderboardScoreUploaded = *pCallResult;
	// Factor in bIOFailure.
	if (m_LeaderboardScoreUploaded.m_bSuccess && !bIOFailure)
	{
		utils::Log(GetName() + ": Succeeded.");
		m_State = Done;
		m_eResult = k_EResultOK;
	}
	else
	{
		utils::Log(GetName() + ": Failed.");
		m_State = ServerError;
		m_eResult = k_EResultFail;
	}
}

void CLeaderboardScoreUploadedCallResult::Call()
{
	m_hSteamAPICall = SteamUserStats()->UploadLeaderboardScore(m_hLeaderboard, m_eLeaderboardUploadScoreMethod, m_nScore, NULL, 0);
	if (m_hSteamAPICall == k_uAPICallInvalid)
	{
		throw std::string(GetName() + ": Call returned k_uAPICallInvalid.");
	}
	m_CallResult.Set(m_hSteamAPICall, this, &CLeaderboardScoreUploadedCallResult::OnUploadScore);
}

std::string CLeaderboardScoreUploadedCallResult::GetResultJSON()
{
	return ToJSON(m_LeaderboardScoreUploaded);
}
