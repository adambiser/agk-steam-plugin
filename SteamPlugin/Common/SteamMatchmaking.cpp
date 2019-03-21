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

#include "Common.h"
#include "SteamMatchmaking.h"

int AddFavoriteGame(int appID, const char *ipv4, int connectPort, int queryPort, int flags) //, int time32LastPlayedOnServer)
{
	CheckInitialized(0);
	if (connectPort < 0 || connectPort > 0xffff)
	{
		agk::PluginError("AddFavoriteGame: Invalid connection port.");
		return 0;
	}
	if (queryPort < 0 || queryPort > 0xffff)
	{
		agk::PluginError("AddFavoriteGame: Invalid query port.");
		return 0;
	}
	uint32 ip;
	if (!utils::ParseIP(ipv4, &ip))
	{
		return 0;
	}
	//agk::Message(agk::Str((int)now));
	return SteamMatchmaking()->AddFavoriteGame(appID, ip, connectPort, queryPort, flags, (uint32)agk::GetUnixTime());
}

//AddRequestLobbyListCompatibleMembersFilter

void AddRequestLobbyListDistanceFilter(int eLobbyDistanceFilter)
{
	CheckInitialized(NORETURN);
	return SteamMatchmaking()->AddRequestLobbyListDistanceFilter((ELobbyDistanceFilter)eLobbyDistanceFilter);
}

void AddRequestLobbyListFilterSlotsAvailable(int slotsAvailable)
{
	CheckInitialized(NORETURN);
	return SteamMatchmaking()->AddRequestLobbyListFilterSlotsAvailable(slotsAvailable);
}

void AddRequestLobbyListNearValueFilter(const char *keyToMatch, int valueToBeCloseTo)
{
	CheckInitialized(NORETURN);
	return SteamMatchmaking()->AddRequestLobbyListNearValueFilter(keyToMatch, valueToBeCloseTo);
}

void AddRequestLobbyListNumericalFilter(const char *keyToMatch, int valueToMatch, int eComparisonType)
{
	CheckInitialized(NORETURN);
	return SteamMatchmaking()->AddRequestLobbyListNumericalFilter(keyToMatch, valueToMatch, (ELobbyComparison)eComparisonType);
}

void AddRequestLobbyListResultCountFilter(int maxResults)
{
	CheckInitialized(NORETURN);
	return SteamMatchmaking()->AddRequestLobbyListResultCountFilter(maxResults);
}

void AddRequestLobbyListStringFilter(const char *keyToMatch, const char *valueToMatch, int eComparisonType)
{
	CheckInitialized(NORETURN);
	return SteamMatchmaking()->AddRequestLobbyListStringFilter(keyToMatch, valueToMatch, (ELobbyComparison)eComparisonType);
}

//CheckForPSNGameBootInvite - deprecated

int CreateLobby(int eLobbyType, int maxMembers)
{
	CheckInitialized(0);
	Callbacks()->EnableLobbyDataUpdateCallback();
	Callbacks()->EnableLobbyEnterCallback();
	return CallResults()->Add(new CLobbyCreatedCallResult((ELobbyType)eLobbyType, maxMembers));
}

int GetCreateLobbyHandle(int hCallResult)
{
	return GetCallResultValue<CLobbyCreatedCallResult>(hCallResult, &CLobbyCreatedCallResult::GetLobbyCreatedHandle);
}

int DeleteLobbyData(int hLobbySteamID, const char *key)
{
	CheckInitialized(false);
	return SteamMatchmaking()->DeleteLobbyData(SteamHandles()->GetSteamHandle(hLobbySteamID), key);
}

//GetFavoriteGame
// Leave this as by-index since AddFavoriteGame appears to return an index.
int GetFavoriteGameAppID(int index)
{
	AppId_t nAppID;
	uint32 nIP;
	uint16 nConnPort;
	uint16 nQueryPort;
	uint32 nFlags;
	uint32 rTime32LastPlayedOnServer;
	if (SteamMatchmaking()->GetFavoriteGame(index, &nAppID, &nIP, &nConnPort, &nQueryPort, &nFlags, &rTime32LastPlayedOnServer))
	{
		return nAppID;
	}
	return 0;
}

char *GetFavoriteGameIP(int index)
{
	AppId_t nAppID;
	uint32 nIP;
	uint16 nConnPort;
	uint16 nQueryPort;
	uint32 nFlags;
	uint32 rTime32LastPlayedOnServer;
	if (SteamMatchmaking()->GetFavoriteGame(index, &nAppID, &nIP, &nConnPort, &nQueryPort, &nFlags, &rTime32LastPlayedOnServer))
	{
		return utils::CreateString(utils::ToIPString(nIP));
	}
	return NULL_STRING;
}

int GetFavoriteGameConnectionPort(int index)
{
	AppId_t nAppID;
	uint32 nIP;
	uint16 nConnPort;
	uint16 nQueryPort;
	uint32 nFlags;
	uint32 rTime32LastPlayedOnServer;
	if (SteamMatchmaking()->GetFavoriteGame(index, &nAppID, &nIP, &nConnPort, &nQueryPort, &nFlags, &rTime32LastPlayedOnServer))
	{
		return nConnPort;
	}
	return 0;
}

int GetFavoriteGameQueryPort(int index)
{
	AppId_t nAppID;
	uint32 nIP;
	uint16 nConnPort;
	uint16 nQueryPort;
	uint32 nFlags;
	uint32 rTime32LastPlayedOnServer;
	if (SteamMatchmaking()->GetFavoriteGame(index, &nAppID, &nIP, &nConnPort, &nQueryPort, &nFlags, &rTime32LastPlayedOnServer))
	{
		return nQueryPort;
	}
	return 0;
}

int GetFavoriteGameFlags(int index)
{
	AppId_t nAppID;
	uint32 nIP;
	uint16 nConnPort;
	uint16 nQueryPort;
	uint32 nFlags;
	uint32 rTime32LastPlayedOnServer;
	if (SteamMatchmaking()->GetFavoriteGame(index, &nAppID, &nIP, &nConnPort, &nQueryPort, &nFlags, &rTime32LastPlayedOnServer))
	{
		return nFlags;
	}
	return 0;
}

int GetFavoriteGameUnixTimeLastPlayedOnServer(int index)
{
	AppId_t nAppID;
	uint32 nIP;
	uint16 nConnPort;
	uint16 nQueryPort;
	uint32 nFlags;
	uint32 rTime32LastPlayedOnServer;
	if (SteamMatchmaking()->GetFavoriteGame(index, &nAppID, &nIP, &nConnPort, &nQueryPort, &nFlags, &rTime32LastPlayedOnServer))
	{
		return nFlags;
	}
	return 0;
}

int GetFavoriteGameCount()
{
	CheckInitialized(0);
	return SteamMatchmaking()->GetFavoriteGameCount();
}

//GetLobbyByIndex - used in callback
//GetLobbyChatEntry - used in callback

char *GetLobbyData(int hLobbySteamID, const char *key)
{
	CheckInitialized(NULL_STRING);
	return utils::CreateString(SteamMatchmaking()->GetLobbyData(SteamHandles()->GetSteamHandle(hLobbySteamID), key));
}

//GetLobbyDataByIndex - see GetLobbyDataJSON
//GetLobbyDataCount - see GetLobbyDataJSON

// The only time GetLobbyDataCount and GetLobbyDataByIndex seem useful is to iterate all of the lobby data.
// Returning the Key/Value list as JSON seems nicer than having to use GetLobbyDataCount, GetLobbyDataByIndexKey, and GetLobbyDataByIndexValue.
char *GetLobbyDataJSON(int hLobbySteamID)
{
	CheckInitialized(NULL_STRING);
	CSteamID steamIDLobby = SteamHandles()->GetSteamHandle(hLobbySteamID);
	std::string json("[");
	for (int index = 0; index < SteamMatchmaking()->GetLobbyDataCount(steamIDLobby); index++)
	{
		char key[k_nMaxLobbyKeyLength];
		char value[k_cubChatMetadataMax];
		if (SteamMatchmaking()->GetLobbyDataByIndex(steamIDLobby, index, key, k_nMaxLobbyKeyLength, value, k_cubChatMetadataMax))
		{
			if (index > 0)
			{
				json += ", ";
			}
			json += "{"
				"\"Key\": \"" + utils::EscapeJSON(key) + "\", "
				"\"Value\": \"" + utils::EscapeJSON(value) + "\""
				"}";
		}
		else
		{
			utils::PluginError("GetLobbyDataByIndex: Error reading index " + std::to_string(index) + " of lobby " + std::to_string(hLobbySteamID));
			return NULL_STRING;
		}
	}
	json += "]";
	return utils::CreateString(json);
}

char *GetLobbyGameServerIP(int hLobbySteamID)
{
	uint32 nIP;
	uint16 nPort;
	CSteamID ulSteamID;
	if (SteamMatchmaking()->GetLobbyGameServer(SteamHandles()->GetSteamHandle(hLobbySteamID), &nIP, &nPort, &ulSteamID))
	{
		return utils::CreateString(utils::ToIPString(nIP));
	}
	return NULL_STRING;
}

int GetLobbyGameServerPort(int hLobbySteamID)
{
	uint32 nIP;
	uint16 nPort;
	CSteamID ulSteamID;
	if (SteamMatchmaking()->GetLobbyGameServer(SteamHandles()->GetSteamHandle(hLobbySteamID), &nIP, &nPort, &ulSteamID))
	{
		return nPort;
	}
	return 0;
}

int GetLobbyGameServerSteamID(int hLobbySteamID)
{
	uint32 nIP;
	uint16 nPort;
	CSteamID ulSteamID;
	if (SteamMatchmaking()->GetLobbyGameServer(SteamHandles()->GetSteamHandle(hLobbySteamID), &nIP, &nPort, &ulSteamID))
	{
		return SteamHandles()->GetPluginHandle(ulSteamID);
	}
	return 0;
}

int GetLobbyMemberByIndex(int hLobbySteamID, int index)
{
	CheckInitialized(0);
	return SteamHandles()->GetPluginHandle(SteamMatchmaking()->GetLobbyMemberByIndex(SteamHandles()->GetSteamHandle(hLobbySteamID), index));
}

char *GetLobbyMemberData(int hLobbySteamID, int hUserSteamID, const char *key)
{
	CheckInitialized(NULL_STRING);
	return utils::CreateString(SteamMatchmaking()->GetLobbyMemberData(SteamHandles()->GetSteamHandle(hLobbySteamID), SteamHandles()->GetSteamHandle(hUserSteamID), key));
}

int GetLobbyMemberLimit(int hLobbySteamID)
{
	CheckInitialized(0);
	return SteamMatchmaking()->GetLobbyMemberLimit(SteamHandles()->GetSteamHandle(hLobbySteamID));
}

int GetLobbyOwner(int hLobbySteamID)
{
	CheckInitialized(0);
	return SteamHandles()->GetPluginHandle(SteamMatchmaking()->GetLobbyOwner(SteamHandles()->GetSteamHandle(hLobbySteamID)));
}

int GetNumLobbyMembers(int hLobbySteamID)
{
	CheckInitialized(0);
	return SteamMatchmaking()->GetNumLobbyMembers(SteamHandles()->GetSteamHandle(hLobbySteamID));
}

int InviteUserToLobby(int hLobbySteamID, int hInviteeSteamID)
{
	CheckInitialized(false);
	return SteamMatchmaking()->InviteUserToLobby(SteamHandles()->GetSteamHandle(hLobbySteamID), SteamHandles()->GetSteamHandle(hInviteeSteamID));
}

int JoinLobby(int hLobbySteamID)
{
	CheckInitialized(false);
	Callbacks()->EnableLobbyDataUpdateCallback();
	Callbacks()->EnableLobbyEnterCallback();
	return CallResults()->Add(new CLobbyEnterCallResult(SteamHandles()->GetSteamHandle(hLobbySteamID)));
}

// Unused - always 0
//int GetJoinLobbyChatPermissions(int hCallResult)
//{
//	return GetCallResultValue<CLobbyEnterCallResult>(hCallResult, &CLobbyEnterCallResult::GetLobbyEnterChatPermissions);
//}

int GetJoinLobbyChatRoomEnterResponse(int hCallResult)
{
	return GetCallResultValue<CLobbyEnterCallResult>(hCallResult, &CLobbyEnterCallResult::GetLobbyEnterChatRoomEnterResponse);
}

int GetJoinLobbyHandle(int hCallResult)
{
	return GetCallResultValue<CLobbyEnterCallResult>(hCallResult, &CLobbyEnterCallResult::GetLobbyEnterHandle);
}

int GetJoinLobbyLocked(int hCallResult)
{
	return GetCallResultValue<CLobbyEnterCallResult>(hCallResult, &CLobbyEnterCallResult::GetLobbyEnterLocked);
}

void LeaveLobby(int hLobbySteamID)
{
	CheckInitialized(NORETURN);
	CSteamID steamIDLobby = SteamHandles()->GetSteamHandle(hLobbySteamID);
	g_JoinedLobbiesMutex.lock();
	SteamMatchmaking()->LeaveLobby(steamIDLobby);
	// Remove from list of joined lobbies.
	auto it = std::find(g_JoinedLobbies.begin(), g_JoinedLobbies.end(), steamIDLobby);
	if (it != g_JoinedLobbies.end())
	{
		g_JoinedLobbies.erase(it);
	}
	g_JoinedLobbiesMutex.unlock();
}

int RemoveFavoriteGame(int appID, const char *ipv4, int connectPort, int queryPort, int flags)
{
	CheckInitialized(0);
	if (connectPort < 0 || connectPort > 0xffff)
	{
		agk::PluginError("RemoveFavoriteGame: Invalid connection port.");
		return 0;
	}
	if (queryPort < 0 || queryPort > 0xffff)
	{
		agk::PluginError("RemoveFavoriteGame: Invalid query port.");
		return 0;
	}
	uint32 ip;
	if (!utils::ParseIP(ipv4, &ip))
	{
		return 0;
	}
	return SteamMatchmaking()->RemoveFavoriteGame(appID, ip, connectPort, queryPort, flags);
}

int RequestLobbyData(int hLobbySteamID)
{
	CheckInitialized(false);
	return SteamMatchmaking()->RequestLobbyData(SteamHandles()->GetSteamHandle(hLobbySteamID));
}

int RequestLobbyList()
{
	CheckInitialized(0);
	return CallResults()->Add(new CLobbyMatchListCallResult());
}

int GetRequestLobbyListCount(int hCallResult)
{
	return GetCallResultValue<CLobbyMatchListCallResult>(hCallResult, &CLobbyMatchListCallResult::GetLobbyCount);
}

int GetRequestLobbyListHandle(int hCallResult, int index)
{
	return GetCallResultValue<CLobbyMatchListCallResult>(hCallResult, index, &CLobbyMatchListCallResult::GetLobby, __FUNCTION__);
}

int SendLobbyChatMessage(int hLobbySteamID, const char *msg)
{
	CheckInitialized(0);
	return SteamMatchmaking()->SendLobbyChatMsg(SteamHandles()->GetSteamHandle(hLobbySteamID), msg, (int)strlen(msg) + 1);
}

//int SetLinkedLobby(int hLobbySteamID, int hLobbyDependentSteamID)
//{
//	CheckInitialized(false);
//	return Callbacks()->SetLinkedLobby(SteamHandles()->GetSteamHandle(hLobbySteamID), SteamHandles()->GetSteamHandle(hLobbyDependentSteamID));
//}

void SetLobbyData(int hLobbySteamID, const char *key, const char *value)
{
	CheckInitialized(NORETURN);
	SteamMatchmaking()->SetLobbyData(SteamHandles()->GetSteamHandle(hLobbySteamID), key, value);
}

int SetLobbyGameServer(int hLobbySteamID, const char *gameServerIP, int gameServerPort, int hGameServerSteamID)
{
	CheckInitialized(false);
	Callbacks()->EnableLobbyGameCreatedCallback();
	if (gameServerPort < 0 || gameServerPort > 0xffff)
	{
		agk::PluginError("SetLobbyGameServer: Invalid game server port.");
		return false;
	}
	uint32 ip;
	if (!utils::ParseIP(gameServerIP, &ip))
	{
		return false;
	}
	SteamMatchmaking()->SetLobbyGameServer(SteamHandles()->GetSteamHandle(hLobbySteamID), ip, gameServerPort, SteamHandles()->GetSteamHandle(hGameServerSteamID));
	return true;
}

int SetLobbyJoinable(int hLobbySteamID, int lobbyJoinable)
{
	CheckInitialized(false);
	return SteamMatchmaking()->SetLobbyJoinable(SteamHandles()->GetSteamHandle(hLobbySteamID), lobbyJoinable != 0);
}

void SetLobbyMemberData(int hLobbySteamID, const char *key, const char *value)
{
	CheckInitialized(NORETURN);
	return SteamMatchmaking()->SetLobbyMemberData(SteamHandles()->GetSteamHandle(hLobbySteamID), key, value);
}

int SetLobbyMemberLimit(int hLobbySteamID, int maxMembers)
{
	CheckInitialized(0);
	return SteamMatchmaking()->SetLobbyMemberLimit(SteamHandles()->GetSteamHandle(hLobbySteamID), maxMembers);
}

int SetLobbyOwner(int hLobbySteamID, int hNewOwnerSteamID)
{
	CheckInitialized(0);
	return SteamMatchmaking()->SetLobbyOwner(SteamHandles()->GetSteamHandle(hLobbySteamID), SteamHandles()->GetSteamHandle(hNewOwnerSteamID));
}

int SetLobbyType(int hLobbySteamID, int eLobbyType)
{
	CheckInitialized(false);
	return SteamMatchmaking()->SetLobbyType(SteamHandles()->GetSteamHandle(hLobbySteamID), (ELobbyType)eLobbyType);
}

// Callbacks
int HasLobbyChatMessageResponse()
{
	CheckInitialized(false);
	return Callbacks()->HasLobbyChatMessageResponse();
}

int GetLobbyChatMessageUser()
{
	CheckInitialized(0);
	return SteamHandles()->GetPluginHandle(Callbacks()->GetLobbyChatMessage().m_ulSteamIDUser);
}

char *GetLobbyChatMessageText()
{
	CheckInitialized(NULL_STRING);
	return utils::CreateString(Callbacks()->GetLobbyChatMessage().m_chChatEntry);
}

int HasLobbyChatUpdateResponse()
{
	CheckInitialized(false);
	return Callbacks()->HasLobbyChatUpdateResponse();
}

int GetLobbyChatUpdateLobby()
{
	CheckInitialized(0);
	return SteamHandles()->GetPluginHandle(Callbacks()->GetLobbyChatUpdate().m_ulSteamIDLobby);
}

int GetLobbyChatUpdateUserChanged()
{
	CheckInitialized(0);
	return SteamHandles()->GetPluginHandle(Callbacks()->GetLobbyChatUpdate().m_ulSteamIDUserChanged);
}

int GetLobbyChatUpdateUserState()
{
	CheckInitialized(0);
	return Callbacks()->GetLobbyChatUpdate().m_rgfChatMemberStateChange;
}

int GetLobbyChatUpdateUserMakingChange()
{
	CheckInitialized(0);
	return SteamHandles()->GetPluginHandle(Callbacks()->GetLobbyChatUpdate().m_ulSteamIDMakingChange);
}

int HasLobbyDataUpdateResponse()
{
	CheckInitialized(false);
	return Callbacks()->HasLobbyDataUpdateResponse();
}

int GetLobbyDataUpdateLobby()
{
	return SteamHandles()->GetPluginHandle(Callbacks()->GetLobbyDataUpdate().m_ulSteamIDLobby);
}

int GetLobbyDataUpdateMember()
{
	return SteamHandles()->GetPluginHandle(Callbacks()->GetLobbyDataUpdate().m_ulSteamIDMember);
}

int GetLobbyDataUpdateSuccess()
{
	return SteamHandles()->GetPluginHandle(Callbacks()->GetLobbyDataUpdate().m_bSuccess);
}

int HasLobbyEnterResponse()
{
	CheckInitialized(false);
	return Callbacks()->HasLobbyEnterResponse();
}

// Unused - always 0
//int GetLobbyEnterChatPermissions()
//{
//	return Callbacks()->GetLobbyEnter().m_rgfChatPermissions;
//}

int GetLobbyEnterChatRoomEnterResponse()
{
	return Callbacks()->GetLobbyEnter().m_EChatRoomEnterResponse;
}

int GetLobbyEnterLobby()
{
	return SteamHandles()->GetPluginHandle(Callbacks()->GetLobbyEnter().m_ulSteamIDLobby);
}

int GetLobbyEnterLocked()
{
	return Callbacks()->GetLobbyEnter().m_bLocked;
}

int HasGameLobbyJoinRequestedResponse()
{
	return Callbacks()->HasGameLobbyJoinRequestedResponse();
}

int GetGameLobbyJoinRequestedLobby()
{
	return SteamHandles()->GetPluginHandle(Callbacks()->GetGameLobbyJoinRequested().m_steamIDLobby);
}

int HasLobbyGameCreatedResponse()
{
	return Callbacks()->HasLobbyGameCreatedResponse();
}

int GetLobbyGameCreatedGameServer()
{
	return SteamHandles()->GetPluginHandle(Callbacks()->GetLobbyGameCreated().m_ulSteamIDGameServer);
}

int GetLobbyGameCreatedLobby()
{
	return SteamHandles()->GetPluginHandle(Callbacks()->GetLobbyGameCreated().m_ulSteamIDLobby);
}

char *GetLobbyGameCreatedIP()
{
	return utils::CreateString(utils::ToIPString(Callbacks()->GetLobbyGameCreated().m_unIP));
}

int GetLobbyGameCreatedPort()
{
	return Callbacks()->GetLobbyGameCreated().m_usPort;
}
