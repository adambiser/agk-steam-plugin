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

void SteamPlugin::AddRequestLobbyListDistanceFilter(ELobbyDistanceFilter eLobbyDistanceFilter)
{
	CheckInitialized(SteamMatchmaking, );
	SteamMatchmaking()->AddRequestLobbyListDistanceFilter(eLobbyDistanceFilter);
}

void SteamPlugin::AddRequestLobbyListFilterSlotsAvailable(int nSlotsAvailable)
{
	CheckInitialized(SteamMatchmaking, );
	SteamMatchmaking()->AddRequestLobbyListFilterSlotsAvailable(nSlotsAvailable);
}

void SteamPlugin::AddRequestLobbyListNearValueFilter(const char *pchKeyToMatch, int nValueToBeCloseTo)
{
	CheckInitialized(SteamMatchmaking, );
	SteamMatchmaking()->AddRequestLobbyListNearValueFilter(pchKeyToMatch, nValueToBeCloseTo);
}

void SteamPlugin::AddRequestLobbyListNumericalFilter(const char *pchKeyToMatch, int nValueToMatch, ELobbyComparison eComparisonType)
{
	CheckInitialized(SteamMatchmaking, );
	SteamMatchmaking()->AddRequestLobbyListNumericalFilter(pchKeyToMatch, nValueToMatch, eComparisonType);
}

void SteamPlugin::AddRequestLobbyListResultCountFilter(int cMaxResults)
{
	CheckInitialized(SteamMatchmaking, );
	SteamMatchmaking()->AddRequestLobbyListResultCountFilter(cMaxResults);
}

void SteamPlugin::AddRequestLobbyListStringFilter(const char *pchKeyToMatch, const char *pchValueToMatch, ELobbyComparison eComparisonType)
{
	CheckInitialized(SteamMatchmaking, );
	SteamMatchmaking()->AddRequestLobbyListStringFilter(pchKeyToMatch, pchValueToMatch, eComparisonType);
}

int SteamPlugin::RequestLobbyList()
{
	CLobbyMatchListCallResult *callResult = new CLobbyMatchListCallResult();
	callResult->Run();
	return AddCallResultItem(callResult);
}

int SteamPlugin::GetLobbyMatchListCount(int hCallResult)
{
	if (CLobbyMatchListCallResult *callResult = GetCallResultItem<CLobbyMatchListCallResult>(hCallResult))
	{
		return callResult->GetLobbyMatchListCount();
	}
	return 0;
}

CSteamID SteamPlugin::GetLobbyByIndex(int hCallResult, int iLobby)
{
	if (CLobbyMatchListCallResult *callResult = GetCallResultItem<CLobbyMatchListCallResult>(hCallResult))
	{
		return callResult->GetLobbyByIndex(iLobby);
	}
	return k_steamIDNil;
}

int SteamPlugin::CreateLobby(ELobbyType eLobbyType, int cMaxMembers)
{
	CLobbyCreatedCallResult *callResult = new CLobbyCreatedCallResult(eLobbyType, cMaxMembers);
	callResult->Run();
	return AddCallResultItem(callResult);
}

CSteamID SteamPlugin::GetLobbyCreatedID(int hCallResult)
{
	if (CLobbyCreatedCallResult *callResult = GetCallResultItem<CLobbyCreatedCallResult>(hCallResult))
	{
		return callResult->GetLobbyID();
	}
	return k_steamIDNil;
}

//bool SteamPlugin::SetLinkedLobby(CSteamID steamIDLobby, CSteamID steamIDLobbyDependent)
//{
//	CheckInitialized(SteamMatchmaking, false);
//	return SteamMatchmaking()->SetLinkedLobby(steamIDLobby, steamIDLobbyDependent);
//}

bool SteamPlugin::SetLobbyJoinable(CSteamID steamIDLobby, bool bLobbyJoinable)
{
	CheckInitialized(SteamMatchmaking, false);
	return SteamMatchmaking()->SetLobbyJoinable(steamIDLobby, bLobbyJoinable);
}

bool SteamPlugin::SetLobbyType(CSteamID steamIDLobby, ELobbyType eLobbyType)
{
	CheckInitialized(SteamMatchmaking, false);
	return SteamMatchmaking()->SetLobbyType(steamIDLobby, eLobbyType);
}

// Fires from CreateLobby and JoinLobby.
void SteamPlugin::OnLobbyEnter(LobbyEnter_t *pParam)
{
	utils::Log("Callback: OnLobbyEnter");
	m_LobbyEnterMutex.lock();
	m_LobbyEnterList.push_back(*pParam);
	m_LobbyEnterMutex.unlock();
	m_JoinedLobbies.push_back(pParam->m_ulSteamIDLobby);
}

bool SteamPlugin::HasLobbyEnterResponse()
{
	m_LobbyEnterMutex.lock();
	if (m_LobbyEnterList.size() > 0)
	{
		m_CurrentLobbyEnter = m_LobbyEnterList.front();
		m_LobbyEnterList.pop_front();
		m_LobbyEnterMutex.unlock();
		return true;
	}
	m_LobbyEnterMutex.unlock();
	ClearCurrentLobbyEnter();
	return false;
}

int SteamPlugin::JoinLobby(CSteamID steamIDLobby)
{
	CheckInitialized(SteamMatchmaking, 0);
	CLobbyEnterCallResult *callResult = new CLobbyEnterCallResult(steamIDLobby);
	callResult->Run();
	return AddCallResultItem(callResult);
}

CSteamID SteamPlugin::GetLobbyEnterID(int hCallResult)
{
	if (CLobbyEnterCallResult *callResult = GetCallResultItem<CLobbyEnterCallResult>(hCallResult))
	{
		return callResult->GetLobbyID();
	}
	return k_steamIDNil;
}

bool SteamPlugin::GetLobbyEnterLocked(int hCallResult)
{
	if (CLobbyEnterCallResult *callResult = GetCallResultItem<CLobbyEnterCallResult>(hCallResult))
	{
		return callResult->GetLobbyLocked();
	}
	return true;
}

EChatRoomEnterResponse SteamPlugin::GetLobbyEnterResponse(int hCallResult)
{
	if (CLobbyEnterCallResult *callResult = GetCallResultItem<CLobbyEnterCallResult>(hCallResult))
	{
		return callResult->GetEnterResponse();
	}
	return k_EChatRoomEnterResponseDoesntExist;
}

bool SteamPlugin::InviteUserToLobby(CSteamID steamIDLobby, CSteamID steamIDInvitee)
{
	CheckInitialized(SteamMatchmaking, false);
	return SteamMatchmaking()->InviteUserToLobby(steamIDLobby, steamIDInvitee);
}

void SteamPlugin::OnGameLobbyJoinRequested(GameLobbyJoinRequested_t *pParam)
{
	agk::Log("Callback: OnGameLobbyJoinRequested");
	m_GameLobbyJoinRequestedInfo = *pParam;
}

CSteamID SteamPlugin::GetGameLobbyJoinRequestedLobby()
{
	CSteamID lobby = m_GameLobbyJoinRequestedInfo.m_steamIDLobby;
	m_GameLobbyJoinRequestedInfo.m_steamIDLobby = k_steamIDNil;
	m_GameLobbyJoinRequestedInfo.m_steamIDFriend = k_steamIDNil;
	return lobby;
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

bool SteamPlugin::GetLobbyGameServer(CSteamID steamIDLobby, uint32 *punGameServerIP, uint16 *punGameServerPort, CSteamID *psteamIDGameServer)
{
	CheckInitialized(SteamMatchmaking, false);
	return SteamMatchmaking()->GetLobbyGameServer(steamIDLobby, punGameServerIP, punGameServerPort, psteamIDGameServer);
}

void SteamPlugin::SetLobbyGameServer(CSteamID steamIDLobby, uint32 unGameServerIP, uint16 unGameServerPort, CSteamID steamIDGameServer) // Triggers a LobbyGameCreated_t callback.
{
	CheckInitialized(SteamMatchmaking, );
	SteamMatchmaking()->SetLobbyGameServer(steamIDLobby, unGameServerIP, unGameServerPort, steamIDGameServer);
}

LobbyGameCreated_t SteamPlugin::GetLobbyGameCreated()
{
	LobbyGameCreated_t temp = m_LobbyGameCreatedInfo;
	// Clear the stored value.
	m_LobbyGameCreatedInfo.m_ulSteamIDLobby = 0;
	m_LobbyGameCreatedInfo.m_ulSteamIDGameServer = 0;
	m_LobbyGameCreatedInfo.m_unIP = 0;
	m_LobbyGameCreatedInfo.m_usPort = 0;
	return temp;
}

void SteamPlugin::OnLobbyGameCreated(LobbyGameCreated_t *pParam)
{
	agk::Log("Callback: OnLobbyGameCreated");
	m_LobbyGameCreatedInfo = *pParam;
}

const char *SteamPlugin::GetLobbyData(CSteamID steamIDLobby, const char *pchKey)
{
	CheckInitialized(SteamMatchmaking, false);
	return SteamMatchmaking()->GetLobbyData(steamIDLobby, pchKey);
}

int SteamPlugin::GetLobbyDataCount(CSteamID steamIDLobby)
{
	CheckInitialized(SteamMatchmaking, 0);
	return SteamMatchmaking()->GetLobbyDataCount(steamIDLobby);
}

bool SteamPlugin::GetLobbyDataByIndex(CSteamID steamIDLobby, int iLobbyData, char *pchKey, int cchKeyBufferSize, char *pchValue, int cchValueBufferSize)
{
	CheckInitialized(SteamMatchmaking, false);
	return SteamMatchmaking()->GetLobbyDataByIndex(steamIDLobby, iLobbyData, pchKey, cchKeyBufferSize, pchValue, cchValueBufferSize);
}

void SteamPlugin::SetLobbyData(CSteamID steamIDLobby, const char *pchKey, const char *pchValue)
{
	CheckInitialized(SteamMatchmaking, );
	SteamMatchmaking()->SetLobbyData(steamIDLobby, pchKey, pchValue);
}

bool SteamPlugin::DeleteLobbyData(CSteamID steamIDLobby, const char *pchKey)
{
	CheckInitialized(SteamMatchmaking, false);
	return SteamMatchmaking()->DeleteLobbyData(steamIDLobby, pchKey);
}

bool SteamPlugin::RequestLobbyData(CSteamID steamIDLobby)
{
	CheckInitialized(SteamMatchmaking, false);
	return SteamMatchmaking()->RequestLobbyData(steamIDLobby);
}

void SteamPlugin::OnLobbyDataUpdated(LobbyDataUpdate_t *pParam)
{
	utils::Log("Callback OnLobbyDataUpdated: " + std::string((pParam->m_bSuccess) ? "Succeeded" : "Failed"));
	m_LobbyDataUpdatedMutex.lock();
	m_LobbyDataUpdatedList.push_back(*pParam);
	m_LobbyDataUpdatedMutex.unlock();
}

bool SteamPlugin::HasLobbyDataUpdated()
{
	m_LobbyDataUpdatedMutex.lock();
	if (m_LobbyDataUpdatedList.size() > 0)
	{
		m_CurrentLobbyDataUpdate = m_LobbyDataUpdatedList.front();
		m_LobbyDataUpdatedList.pop_front();
		m_LobbyDataUpdatedMutex.unlock();
		return true;
	}
	ClearCurrentLobbyDataUpdate();
	m_LobbyDataUpdatedMutex.unlock();
	return false;
}

const char *SteamPlugin::GetLobbyMemberData(CSteamID steamIDLobby, CSteamID steamIDUser, const char *pchKey)
{
	CheckInitialized(SteamMatchmaking, NULL);
	return SteamMatchmaking()->GetLobbyMemberData(steamIDLobby, steamIDUser, pchKey);
}

void SteamPlugin::SetLobbyMemberData(CSteamID steamIDLobby, const char *pchKey, const char *pchValue)
{
	CheckInitialized(SteamMatchmaking, );
	SteamMatchmaking()->SetLobbyMemberData(steamIDLobby, pchKey, pchValue);
}

CSteamID SteamPlugin::GetLobbyOwner(CSteamID steamIDLobby)
{
	CheckInitialized(SteamMatchmaking, k_steamIDNil);
	return SteamMatchmaking()->GetLobbyOwner(steamIDLobby);
}

bool SteamPlugin::SetLobbyOwner(CSteamID steamIDLobby, CSteamID steamIDNewOwner)
{
	CheckInitialized(SteamMatchmaking, false);
	return SteamMatchmaking()->SetLobbyOwner(steamIDLobby, steamIDNewOwner);
}

int SteamPlugin::GetLobbyMemberLimit(CSteamID steamIDLobby)
{
	CheckInitialized(SteamMatchmaking, 0);
	return SteamMatchmaking()->GetLobbyMemberLimit(steamIDLobby);
}

bool SteamPlugin::SetLobbyMemberLimit(CSteamID steamIDLobby, int cMaxMembers)
{
	CheckInitialized(SteamMatchmaking, false);
	return SteamMatchmaking()->SetLobbyMemberLimit(steamIDLobby, cMaxMembers);
}

int SteamPlugin::GetNumLobbyMembers(CSteamID steamIDLobby)
{
	CheckInitialized(SteamMatchmaking, 0);
	return SteamMatchmaking()->GetNumLobbyMembers(steamIDLobby);
}

CSteamID SteamPlugin::GetLobbyMemberByIndex(CSteamID steamIDLobby, int iMember)
{
	CheckInitialized(SteamMatchmaking, k_steamIDNil);
	return SteamMatchmaking()->GetLobbyMemberByIndex(steamIDLobby, iMember);
}

void SteamPlugin::OnLobbyChatUpdated(LobbyChatUpdate_t *pParam)
{
	agk::Log("Callback: OnLobbyChatUpdated");
	m_LobbyChatUpdatedMutex.lock();
	m_LobbyChatUpdatedList.push_back(*pParam);
	m_LobbyChatUpdatedMutex.unlock();
}

bool SteamPlugin::HasLobbyChatUpdate()
{
	m_LobbyChatUpdatedMutex.lock();
	if (m_LobbyChatUpdatedList.size() > 0)
	{
		m_CurrentLobbyChatUpdate = m_LobbyChatUpdatedList.front();
		m_LobbyChatUpdatedList.pop_front();
		m_LobbyChatUpdatedMutex.unlock();
		return true;
	}
	ClearCurrentLobbyChatUpdate();
	m_LobbyChatUpdatedMutex.unlock();
	return false;
}

void SteamPlugin::OnLobbyChatMessage(LobbyChatMsg_t *pParam)
{
	agk::Log("Callback: OnLobbyChatMessage");
	ChatMessageInfo_t info;
	info.user = pParam->m_ulSteamIDUser;
	SteamMatchmaking()->GetLobbyChatEntry(pParam->m_ulSteamIDLobby, pParam->m_iChatID, NULL, info.text, MAX_CHAT_MESSAGE_LENGTH, NULL);
	m_LobbyChatMessageMutex.lock();
	m_LobbyChatMessageList.push_back(info);
	m_LobbyChatMessageMutex.unlock();
}

bool SteamPlugin::HasLobbyChatMessage()
{
	m_LobbyChatMessageMutex.lock();
	if (m_LobbyChatMessageList.size() > 0)
	{
		m_CurrentLobbyChatMessage = m_LobbyChatMessageList.front();
		m_LobbyChatMessageList.pop_front();
		m_LobbyChatMessageMutex.unlock();
		return true;
	}
	m_LobbyChatMessageMutex.unlock();
	ClearCurrentLobbyChatMessage();
	return false;
}

bool SteamPlugin::SendLobbyChatMessage(CSteamID steamIDLobby, const char *pvMsgBody)
{
	CheckInitialized(SteamMatchmaking, false);
	return SteamMatchmaking()->SendLobbyChatMsg(steamIDLobby, pvMsgBody, (int)strlen(pvMsgBody) + 1);
}

// Lobby methods: Favorite games
int SteamPlugin::AddFavoriteGame(AppId_t nAppID, uint32 nIP, uint16 nConnPort, uint16 nQueryPort, uint32 unFlags, uint32 rTime32LastPlayedOnServer)
{
	CheckInitialized(SteamMatchmaking, 0);
	return SteamMatchmaking()->AddFavoriteGame(nAppID, nIP, nConnPort, nQueryPort, unFlags, rTime32LastPlayedOnServer);
}

int SteamPlugin::GetFavoriteGameCount()
{
	CheckInitialized(SteamMatchmaking, 0);
	return SteamMatchmaking()->GetFavoriteGameCount();
}

bool SteamPlugin::GetFavoriteGame(int iGame, AppId_t *pnAppID, uint32 *pnIP, uint16 *pnConnPort, uint16 *pnQueryPort, uint32 *punFlags, uint32 *pRTime32LastPlayedOnServer)
{
	CheckInitialized(SteamMatchmaking, false);
	return SteamMatchmaking()->GetFavoriteGame(iGame, pnAppID, pnIP, pnConnPort, pnQueryPort, punFlags, pRTime32LastPlayedOnServer);
}

bool SteamPlugin::RemoveFavoriteGame(AppId_t nAppID, uint32 nIP, uint16 nConnPort, uint16 nQueryPort, uint32 unFlags)
{
	CheckInitialized(SteamMatchmaking, false);
	return SteamMatchmaking()->RemoveFavoriteGame(nAppID, nIP, nConnPort, nQueryPort, unFlags);
}

//uint32 SteamPlugin::GetPublicIP()
//{
//	SteamGameServer_Init(0xc0a80068, 27015, 11000, 11001, eServerModeNoAuthentication, "1.0.0.0");
//	CheckInitialized(SteamGameServer, 0);
//	uint32 ip = SteamGameServer()->GetPublicIP();
//	SteamGameServer_Shutdown();
//	return ip;
//}
