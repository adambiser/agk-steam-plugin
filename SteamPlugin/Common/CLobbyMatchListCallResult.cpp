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

#include "CLobbyMatchListCallResult.h"

void CLobbyMatchListCallResult::OnLobbyMatchList(LobbyMatchList_t *pLobbyMatchList, bool bIOFailure)
{
	if (!bIOFailure)
	{
		utils::Log(GetName() + ": Succeeded.");
		for (int index = 0; index < (int)pLobbyMatchList->m_nLobbiesMatching; index++)
		{
			m_Lobbies.push_back(SteamMatchmaking()->GetLobbyByIndex(index));
		}
		m_State = Done;
	}
	else
	{
		utils::Log(GetName() + ": Failed.");
		m_State = ServerError;
	}
}

void CLobbyMatchListCallResult::Call()
{
	m_hSteamAPICall = SteamMatchmaking()->RequestLobbyList();
	if (m_hSteamAPICall == k_uAPICallInvalid)
	{
		throw std::string("Callback \"" + GetName() + "\" returned k_uAPICallInvalid.");
	}
	m_CallResult.Set(m_hSteamAPICall, this, &CLobbyMatchListCallResult::OnLobbyMatchList);
}

CSteamID CLobbyMatchListCallResult::GetLobbyByIndex(int index)
{
	if (index < 0 || index >= (int)m_Lobbies.size())
	{
		utils::PluginError(GetName() + ": Index out of range: " + std::to_string(index));
		return k_steamIDNil;
	}
	return m_Lobbies[index];
}
