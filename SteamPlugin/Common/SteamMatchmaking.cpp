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

// Callback for CreateLobby.
void SteamPlugin::OnLobbyCreated(LobbyCreated_t *pParam, bool bIOFailure)
{
	if (m_LobbyCreateCallbackState == Done)
	{
		agk::PluginError("OnLobbyCreated called while in done state.");
	}
	if (bIOFailure)
	{
		agk::Log("Callback: Lobby Created Failed");
		m_LobbyCreateCallbackState = ServerError;
	}
	else
	{
		agk::Log("Callback: Lobby Created Succeeded");
		m_LobbyCreateCallbackState = Done;
		m_LobbyCreatedID = pParam->m_ulSteamIDLobby;
		m_LobbyCreatedResult = pParam->m_eResult;
	}
}

bool SteamPlugin::CreateLobby(ELobbyType eLobbyType, int cMaxMembers)
{
	m_LobbyEnterBlocked = false;
	m_LobbyEnterResponse = (EChatRoomEnterResponse)0;
	m_LobbyCreatedID = k_steamIDNil;
	m_LobbyCreatedResult = (EResult)0;
	CheckInitialized(SteamMatchmaking, false);
	try
	{
		SteamAPICall_t hSteamAPICall = SteamMatchmaking()->CreateLobby(eLobbyType, cMaxMembers);
		m_CallResultLobbyCreate.Set(hSteamAPICall, this, &SteamPlugin::OnLobbyCreated);
		m_LobbyCreateCallbackState = Running;
		return true;
	}
	catch (...)
	{
		m_LobbyCreateCallbackState = ClientError;
		return false;
	}
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

void SteamPlugin::OnLobbyEnter(LobbyEnter_t *pParam)
{
	OnLobbyEnter(pParam, false);
}

// Callback for JoinLobby.
void SteamPlugin::OnLobbyEnter(LobbyEnter_t *pParam, bool bIOFailure)
{
	if (bIOFailure)
	{
		agk::Log("Callback: Lobby Enter Failed");
		m_LobbyEnterCallbackState = ServerError;
	}
	else
	{
		agk::Log("Callback: Lobby Enter Succeeded");
		if (m_LobbyEnterCallbackState == Done)
		{
			if (m_LobbyEnterID == pParam->m_ulSteamIDLobby)
			{
				// Already notified of entering this lobby. Ignore.
				return;
			}
			agk::PluginError("OnLobbyEnter called while in done state.");
		}
		m_LobbyEnterCallbackState = Done;
		m_LobbyEnterID = pParam->m_ulSteamIDLobby;
		m_LobbyEnterBlocked = pParam->m_bLocked;
		m_LobbyEnterResponse = (EChatRoomEnterResponse)pParam->m_EChatRoomEnterResponse;
		if (!pParam->m_bLocked && pParam->m_EChatRoomEnterResponse == k_EChatRoomEnterResponseSuccess)
		{
			// Add to list of joined lobbies.
			auto it = std::find(m_JoinedLobbies.begin(), m_JoinedLobbies.end(), pParam->m_ulSteamIDLobby);
			if (it != m_JoinedLobbies.end())
			{
				m_JoinedLobbies.push_back(pParam->m_ulSteamIDLobby);
			}
		}
	}
}

bool SteamPlugin::JoinLobby(CSteamID steamIDLobby)
{
	m_LobbyEnterBlocked = false;
	m_LobbyEnterResponse = (EChatRoomEnterResponse)0;
	CheckInitialized(SteamMatchmaking, false);
	try
	{
		SteamAPICall_t hSteamAPICall = SteamMatchmaking()->JoinLobby(steamIDLobby);
		m_CallResultLobbyEnter.Set(hSteamAPICall, this, &SteamPlugin::OnLobbyEnter);
		m_LobbyEnterCallbackState = Running;
		return true;
	}
	catch (...)
	{
		m_LobbyEnterCallbackState = ClientError;
		return false;
	}
}

bool SteamPlugin::InviteUserToLobby(CSteamID steamIDLobby, CSteamID steamIDInvitee)
{
	CheckInitialized(SteamMatchmaking, false);
	return SteamMatchmaking()->InviteUserToLobby(steamIDLobby, steamIDInvitee);
}

void SteamPlugin::OnGameLobbyJoinRequested(GameLobbyJoinRequested_t *pParam)
{
	agk::Log("Callback: Lobby Join Requested");
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
	agk::Log("Callback: Lobby Game Created");
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
	lobbyDataUpdateMutex.lock();
	if (!pParam->m_bSuccess)
	{
		agk::Log("Callback: Lobby Data Update Failed");
		//m_LobbyDataUpdateCallbackState = ServerError;
		// Report nil steam ids for failure.
		LobbyDataUpdateInfo_t info;
		info.lobby = k_steamIDNil;
		info.changedID = k_steamIDNil;
		m_LobbyDataUpdated.push_back(info);
	}
	else
	{
		agk::Log("Callback: Lobby Data Update Succeeded");
		//m_LobbyDataUpdateCallbackState = Done;
		LobbyDataUpdateInfo_t info;
		info.lobby = pParam->m_ulSteamIDLobby;
		info.changedID = pParam->m_ulSteamIDMember;
		m_LobbyDataUpdated.push_back(info);
	}
	lobbyDataUpdateMutex.unlock();
}

bool SteamPlugin::HasLobbyDataUpdated()
{
	lobbyDataUpdateMutex.lock();
	if (m_LobbyDataUpdated.size() > 0)
	{
		LobbyDataUpdateInfo_t info = m_LobbyDataUpdated.front();
		m_LobbyDataUpdatedLobby = info.lobby;
		m_LobbyDataUpdatedID = info.changedID;
		m_LobbyDataUpdated.pop_front();
		lobbyDataUpdateMutex.unlock();
		return true;
	}
	m_LobbyDataUpdatedLobby = k_steamIDNil;
	m_LobbyDataUpdatedID = k_steamIDNil;
	lobbyDataUpdateMutex.unlock();
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
	agk::Log("Callback: Lobby Chat updated");
	ChatUpdateInfo_t info;
	info.userChanged = pParam->m_ulSteamIDUserChanged;
	info.userState = (EChatMemberStateChange)pParam->m_rgfChatMemberStateChange;
	info.userMakingChange = pParam->m_ulSteamIDMakingChange;
	m_ChatUpdates.push_back(info);
}

bool SteamPlugin::HasLobbyChatUpdate()
{
	if (m_ChatUpdates.size() > 0)
	{
		ChatUpdateInfo_t info = m_ChatUpdates.front();
		m_LobbyChatUpdateUserChanged = info.userChanged;
		m_LobbyChatUpdateUserState = info.userState;
		m_LobbyChatUpdateUserMakingChange = info.userMakingChange;
		m_ChatUpdates.pop_front();
		return true;
	}
	m_LobbyChatUpdateUserChanged = k_steamIDNil;
	m_LobbyChatUpdateUserState = (EChatMemberStateChange)0;
	m_LobbyChatUpdateUserMakingChange = k_steamIDNil;
	return false;
}

void SteamPlugin::OnLobbyChatMessage(LobbyChatMsg_t *pParam)
{
	agk::Log("Callback: Lobby Chat Message");
	//m_LobbyChatMessageCallbackState = Done;
	ChatMessageInfo_t info;
	info.user = pParam->m_ulSteamIDUser;
	SteamMatchmaking()->GetLobbyChatEntry(pParam->m_ulSteamIDLobby, pParam->m_iChatID, NULL, info.text, 4096, NULL);
	m_ChatMessages.push_back(info);
}

bool SteamPlugin::HasLobbyChatMessage()
{
	if (m_ChatMessages.size() > 0)
	{
		ChatMessageInfo_t info = m_ChatMessages.front();
		m_LobbyChatMessageUser = info.user;
		strcpy_s(m_LobbyChatMessageText, strlen(info.text) + 1, info.text);
		m_ChatMessages.pop_front();
		return true;
	}
	m_LobbyChatMessageUser = k_steamIDNil;
	*m_LobbyChatMessageText = NULL;
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
