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

#include "CLeaderboardFindCallback.h"
#include "..\AGKLibraryCommands.h"
#include "AGKUtils.h"
#include <sstream>

void CLeaderboardFindCallback::OnFindLeaderboard(LeaderboardFindResult_t *pCallback, bool bIOFailure)
{
	m_hSteamLeaderboard = pCallback->m_hSteamLeaderboard;
	bool found = pCallback->m_bLeaderboardFound && !bIOFailure;
	utils::Log("OnFindLeaderboard: '" + m_Name + "'.  Found = " + std::to_string(found) + ", Handle = " + std::to_string(m_hSteamLeaderboard));
	if (found)
	{
		m_State = Done;
	}
	else
	{
		m_State = ServerError;
	}
}

bool CLeaderboardFindCallback::Call()
{
	SteamAPICall_t hSteamAPICall = SteamUserStats()->FindLeaderboard(m_Name.c_str());
	if (hSteamAPICall == k_uAPICallInvalid)
	{
		return false;
	}
	m_CallResult.Set(hSteamAPICall, this, &CLeaderboardFindCallback::OnFindLeaderboard);
	return true;
}
