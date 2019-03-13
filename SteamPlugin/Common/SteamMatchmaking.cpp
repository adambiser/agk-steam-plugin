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

#include "SteamPlugin.h"
#include "CLobbyCreatedCallResult.h"
#include "CLobbyEnterCallResult.h"
#include "CLobbyMatchListCallResult.h"

int SteamPlugin::RequestLobbyList()
{
	CLobbyMatchListCallResult *callResult = new CLobbyMatchListCallResult();
	callResult->Run();
	return AddCallResultItem(callResult);
}

int SteamPlugin::CreateLobby(ELobbyType eLobbyType, int cMaxMembers)
{
	CLobbyCreatedCallResult *callResult = new CLobbyCreatedCallResult(eLobbyType, cMaxMembers);
	callResult->Run();
	return AddCallResultItem(callResult);
}

//bool SteamPlugin::SetLinkedLobby(CSteamID steamIDLobby, CSteamID steamIDLobbyDependent)
//{
//	CheckInitialized(SteamMatchmaking, false);
//	return SteamMatchmaking()->SetLinkedLobby(steamIDLobby, steamIDLobbyDependent);
//}

// Fires from CreateLobby and JoinLobby.
void SteamPlugin::OnLobbyEnter(LobbyEnter_t *pParam)
{
	utils::Log("Callback: OnLobbyEnter");
	StoreCallbackResult(LobbyEnter, *pParam);
	m_JoinedLobbies.push_back(pParam->m_ulSteamIDLobby);
}

bool SteamPlugin::HasLobbyEnter()
{
	GetNextCallbackResult(LobbyEnter);
}

int SteamPlugin::JoinLobby(CSteamID steamIDLobby)
{
	CheckInitialized(SteamMatchmaking, 0);
	CLobbyEnterCallResult *callResult = new CLobbyEnterCallResult(steamIDLobby);
	callResult->Run();
	return AddCallResultItem(callResult);
}

void SteamPlugin::OnGameLobbyJoinRequested(GameLobbyJoinRequested_t *pParam)
{
	agk::Log("Callback: OnGameLobbyJoinRequested");
	StoreCallbackResult(GameLobbyJoinRequested, *pParam);
}

bool SteamPlugin::HasGameLobbyJoinRequested()
{
	GetNextCallbackResult(GameLobbyJoinRequested);
}

void SteamPlugin::LeaveLobby(CSteamID steamIDLobby)
{
	CheckInitialized(SteamMatchmaking, );
	SteamMatchmaking()->LeaveLobby(steamIDLobby);
	// Remove from list of joined lobbies.
	auto it = std::find(m_JoinedLobbies.begin(), m_JoinedLobbies.end(), steamIDLobby);
	if (it != m_JoinedLobbies.end())
	{
		m_JoinedLobbies.erase(it);
	}
}

void SteamPlugin::OnLobbyGameCreated(LobbyGameCreated_t *pParam)
{
	utils::Log("Callback: OnLobbyGameCreated: " + std::to_string(pParam->m_ulSteamIDLobby));
	StoreCallbackResult(LobbyGameCreated, *pParam);
}

bool SteamPlugin::HasLobbyGameCreated()
{
	GetNextCallbackResult(LobbyGameCreated);
}

void SteamPlugin::OnLobbyDataUpdate(LobbyDataUpdate_t *pParam)
{
	utils::Log("Callback OnLobbyDataUpdate: " + std::string((pParam->m_bSuccess) ? "Succeeded" : "Failed"));
	StoreCallbackResult(LobbyDataUpdate, *pParam);
}

bool SteamPlugin::HasLobbyDataUpdate()
{
	GetNextCallbackResult(LobbyDataUpdate);
}

void SteamPlugin::OnLobbyChatUpdate(LobbyChatUpdate_t *pParam)
{
	agk::Log("Callback: OnLobbyChatUpdate");
	StoreCallbackResult(LobbyChatUpdate, *pParam);
}

bool SteamPlugin::HasLobbyChatUpdate()
{
	GetNextCallbackResult(LobbyChatUpdate);
}

void SteamPlugin::OnLobbyChatMessage(LobbyChatMsg_t *pParam)
{
	agk::Log("Callback: OnLobbyChatMessage");
	plugin::LobbyChatMsg_t info(*pParam);
	SteamMatchmaking()->GetLobbyChatEntry(info.m_ulSteamIDLobby, pParam->m_iChatID, NULL, info.m_chChatEntry, MAX_CHAT_MESSAGE_LENGTH, NULL);
	StoreCallbackResult(LobbyChatMessage, info);
}

bool SteamPlugin::HasLobbyChatMessage()
{
	GetNextCallbackResult(LobbyChatMessage);
}
