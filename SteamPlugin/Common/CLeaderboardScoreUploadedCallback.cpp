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

#include "CLeaderboardScoreUploadedCallback.h"
#include "AGKUtils.h"

#define CheckEntryIndex(index, returnValue) \
	if (index < 0 || index >= (int)m_Entries.size()) \
	{ \
		utils::PluginError(GetName() + ": Index out of range: " + std::to_string(index)); \
		return returnValue; \
	}

void CLeaderboardScoreUploadedCallback::OnUploadScore(LeaderboardScoreUploaded_t *pCallback, bool bIOFailure)
{
	m_LeaderboardScoreUploaded = *pCallback;
	// Factor in bIOFailure.
	m_LeaderboardScoreUploaded.m_bSuccess = m_LeaderboardScoreUploaded.m_bSuccess && !bIOFailure;
	if (m_LeaderboardScoreUploaded.m_bSuccess)
	{
		utils::Log(GetName() + " succeeded.");
		m_State = Done;
	}
	else
	{
		utils::Log(GetName() + " failed.");
		m_State = ServerError;
	}
}

bool CLeaderboardScoreUploadedCallback::Call()
{
	SteamAPICall_t hSteamAPICall = SteamUserStats()->UploadLeaderboardScore(m_hLeaderboard, m_eLeaderboardUploadScoreMethod, m_nScore, NULL, 0);
	if (hSteamAPICall == k_uAPICallInvalid)
	{
		return false;
	}
	m_CallResult.Set(hSteamAPICall, this, &CLeaderboardScoreUploadedCallback::OnUploadScore);
	return true;
}
