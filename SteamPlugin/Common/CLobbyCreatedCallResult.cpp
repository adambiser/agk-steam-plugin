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

#include "CLobbyCreatedCallResult.h"

void CLobbyCreatedCallResult::OnLobbyCreated(LobbyCreated_t *pParam, bool bIOFailure)
{
	if (!bIOFailure)
	{
		utils::Log(GetName() + ": Succeeded.");
		m_Lobby = *pParam;
		m_State = Done;
	}
	else
	{
		utils::Log(GetName() + ": Failed.");
		m_State = ServerError;
	}
}

void CLobbyCreatedCallResult::Call()
{
	m_hSteamAPICall = SteamMatchmaking()->CreateLobby(m_eLobbyType, m_cMaxMembers);
	if (m_hSteamAPICall == k_uAPICallInvalid)
	{
		throw std::string(GetName() + ": Call returned k_uAPICallInvalid.");
	}
	m_CallResult.Set(m_hSteamAPICall, this, &CLobbyCreatedCallResult::OnLobbyCreated);
}
