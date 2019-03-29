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

#include "DllMain.h"

/* @page ISteamMatchmaking */

/*
@desc Adds the game server to the local favorites list or updates the time played of the server if it already exists in the list.

_The plugin sets the API call's rTime32LastPlayedOnServer parameter internally to the current client system time._
@param appID The App ID of the game.
@param ipv4 The IP address of the server.
@param connectPort The port used to connect to the server.
@param queryPort The port used to query the server, in host order.
@param flags Sets the whether the server should be added to the favorites list or history list.
@param-url flags https://partner.steamgames.com/doc/api/ISteamMatchmaking#k_unFavoriteFlagNone
@return An integer.  This appears to be an index position, but is not defined by the API.
@url https://partner.steamgames.com/doc/api/ISteamMatchmaking#AddFavoriteGame
@url https://partner.steamgames.com/doc/api/ISteamMatchmaking#k_unFavoriteFlagNone
@url https://partner.steamgames.com/doc/api/ISteamMatchmaking#k_unFavoriteFlagFavorite
@url https://partner.steamgames.com/doc/api/ISteamMatchmaking#k_unFavoriteFlagHistory
*/
extern "C" DLL_EXPORT int AddFavoriteGame(int appID, const char *ipv4, int connectPort, int queryPort, int flags) //, int time32LastPlayedOnServer)
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
	return SteamMatchmaking()->AddFavoriteGame(appID, ip, connectPort, queryPort, flags, (uint32)agk::GetUnixTime());
}

// AddRequestLobbyListCompatibleMembersFilter - unused

/*
@desc Sets the physical distance for which we should search for lobbies, this is based on the users IP address and a IP location map on the Steam backend.
@param eLobbyDistanceFilter Specifies the maximum distance.
@param-url eLobbyDistanceFilter https://partner.steamgames.com/doc/api/ISteamMatchmaking#ELobbyDistanceFilter
@url https://partner.steamgames.com/doc/api/ISteamMatchmaking#AddRequestLobbyListDistanceFilter
*/
extern "C" DLL_EXPORT void AddRequestLobbyListDistanceFilter(int eLobbyDistanceFilter)
{
	CheckInitialized(NORETURN);
	return SteamMatchmaking()->AddRequestLobbyListDistanceFilter((ELobbyDistanceFilter)eLobbyDistanceFilter);
}

/*
@desc Filters to only return lobbies with the specified number of open slots available.
@param slotsAvailable The number of open slots that must be open.
@url https://partner.steamgames.com/doc/api/ISteamMatchmaking#AddRequestLobbyListFilterSlotsAvailable
*/
extern "C" DLL_EXPORT void AddRequestLobbyListFilterSlotsAvailable(int slotsAvailable)
{
	CheckInitialized(NORETURN);
	return SteamMatchmaking()->AddRequestLobbyListFilterSlotsAvailable(slotsAvailable);
}

/*
@desc Sorts the results closest to the specified value.
@param keyToMatch The filter key name to match.
@param valueToBeCloseTo The value that lobbies will be sorted on.
@url https://partner.steamgames.com/doc/api/ISteamMatchmaking#AddRequestLobbyListNearValueFilter
*/
extern "C" DLL_EXPORT void AddRequestLobbyListNearValueFilter(const char *keyToMatch, int valueToBeCloseTo)
{
	CheckInitialized(NORETURN);
	return SteamMatchmaking()->AddRequestLobbyListNearValueFilter(keyToMatch, valueToBeCloseTo);
}

/*
@desc Adds a numerical comparison filter to the next RequestLobbyList call.
@param keyToMatch The filter key name to match.
@param valueToMatch The number to match.
@param eComparisonType The type of comparison to make.
@param-url eComparisonType https://partner.steamgames.com/doc/api/ISteamMatchmaking#ELobbyComparison
@url https://partner.steamgames.com/doc/api/ISteamMatchmaking#AddRequestLobbyListNumericalFilter
*/
extern "C" DLL_EXPORT void AddRequestLobbyListNumericalFilter(const char *keyToMatch, int valueToMatch, int eComparisonType)
{
	CheckInitialized(NORETURN);
	return SteamMatchmaking()->AddRequestLobbyListNumericalFilter(keyToMatch, valueToMatch, (ELobbyComparison)eComparisonType);
}

/*
@desc Sets the maximum number of lobbies to return.
@param maxResults The maximum number of lobbies to return.
@url https://partner.steamgames.com/doc/api/ISteamMatchmaking#AddRequestLobbyListResultCountFilter
*/
extern "C" DLL_EXPORT void AddRequestLobbyListResultCountFilter(int maxResults)
{
	CheckInitialized(NORETURN);
	return SteamMatchmaking()->AddRequestLobbyListResultCountFilter(maxResults);
}

/*
@desc Adds a string comparison filter to the next RequestLobbyList call.
@param keyToMatch The filter key name to match.
@param valueToMatch The string to match.
@param eComparisonType The type of comparison to make.
@param-url eComparisonType https://partner.steamgames.com/doc/api/ISteamMatchmaking#ELobbyComparison
@url https://partner.steamgames.com/doc/api/ISteamMatchmaking#AddRequestLobbyListStringFilter
*/
extern "C" DLL_EXPORT void AddRequestLobbyListStringFilter(const char *keyToMatch, const char *valueToMatch, int eComparisonType)
{
	CheckInitialized(NORETURN);
	return SteamMatchmaking()->AddRequestLobbyListStringFilter(keyToMatch, valueToMatch, (ELobbyComparison)eComparisonType);
}

//CheckForPSNGameBootInvite - deprecated

class CCreateLobbyCallResult : public CCallResultItem<LobbyCreated_t>
{
public:
	CCreateLobbyCallResult(ELobbyType eLobbyType, int cMaxMembers)
	{
		m_CallResultName = "CreateLobby("
			+ std::to_string(eLobbyType) + ", "
			+ std::to_string(cMaxMembers) + ")";
		m_hSteamAPICall = SteamMatchmaking()->CreateLobby(eLobbyType, cMaxMembers);
	}
	uint64 GetLobbyCreatedHandle() { return m_Response.m_ulSteamIDLobby; }
};

/*
@desc Creates a new matchmaking lobby.  The lobby is joined once it is created.
@param eLobbyType The type and visibility of this lobby. This can be changed later via SetLobbyType.
@param-url eLobbyType https://partner.steamgames.com/doc/api/ISteamMatchmaking#ELobbyType
@param maxMembers The maximum number of players that can join this lobby. This can not be above 250.
@callback-type callresult
@callback-getters GetCreateLobbyHandle
@return A [call result handle](Callbacks-and-Call-Results#call-results) on success; otherwise 0.
@url https://partner.steamgames.com/doc/api/ISteamMatchmaking#CreateLobby
*/
extern "C" DLL_EXPORT int CreateLobby(int eLobbyType, int maxMembers)
{
	CheckInitialized(0);
	//Callbacks()->RegisterLobbyEnterCallback();
	Callbacks()->LobbyEnter.Register();
	return CallResults()->Add(new CCreateLobbyCallResult((ELobbyType)eLobbyType, maxMembers));
}

/*
@desc Returns the lobby handle for the CreateLobby call.
@param hCallResult A CreateLobby call result handle.
@return A lobby SteamID handle or 0 if the call failed.
@url https://partner.steamgames.com/doc/api/ISteamMatchmaking#CreateLobby
@url https://partner.steamgames.com/doc/api/ISteamMatchmaking#LobbyCreated_t
*/
extern "C" DLL_EXPORT int GetCreateLobbyHandle(int hCallResult)
{
	return GetCallResultValue(hCallResult, &CCreateLobbyCallResult::GetLobbyCreatedHandle);
}

/*
@desc Removes a metadata key from the lobby.

_This can only be set by the owner of the lobby._
@param hLobbySteamID The Steam ID of the lobby to delete the metadata for.
@param key The key to delete the data for.
@return 1 when the request succeeds; otherwise 0.
@url https://partner.steamgames.com/doc/api/ISteamMatchmaking#DeleteLobbyData
*/
extern "C" DLL_EXPORT int DeleteLobbyData(int hLobbySteamID, const char *key)
{
	CheckInitialized(false);
	return SteamMatchmaking()->DeleteLobbyData(SteamHandles()->GetSteamHandle(hLobbySteamID), key);
}

/*
@desc Returns the App ID of the favorite game server by index.

_Note: You must call GetFavoriteGameCount before calling this._
@param index The index of the favorite game server to get the details of. This must be between 0 and GetFavoriteGameCount() - 1
@return An App ID.
@url https://partner.steamgames.com/doc/api/ISteamMatchmaking#GetFavoriteGame
*/
extern "C" DLL_EXPORT int GetFavoriteGameAppID(int index)
{
	CheckInitialized(0);
	//TODO? Will this need a GetFavoriteGame call to load the data and then these methods to get each piece?  Would be faster...
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

/*
@desc Returns the IP address of the favorite game server by index.

_Note: You must call GetFavoriteGameCount before calling this._
@param index The index of the favorite game server to get the details of. This must be between 0 and GetFavoriteGameCount() - 1
@return An IP address.
@url https://partner.steamgames.com/doc/api/ISteamMatchmaking#GetFavoriteGame
*/
extern "C" DLL_EXPORT char *GetFavoriteGameIP(int index)
{
	CheckInitialized(NULL_STRING);
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

/*
@desc Returns the port used to connect to the favorite game server by index.

_Note: You must call GetFavoriteGameCount before calling this._
@param index The index of the favorite game server to get the details of. This must be between 0 and GetFavoriteGameCount() - 1
@return A port number.
@url https://partner.steamgames.com/doc/api/ISteamMatchmaking#GetFavoriteGame
*/
extern "C" DLL_EXPORT int GetFavoriteGameConnectionPort(int index)
{
	CheckInitialized(0);
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

/*
@desc Returns the port used to query the favorite game server by index.

_Note: You must call GetFavoriteGameCount before calling this._
@param index The index of the favorite game server to get the details of. This must be between 0 and GetFavoriteGameCount() - 1
@return A port number.
@url https://partner.steamgames.com/doc/api/ISteamMatchmaking#GetFavoriteGame
*/
extern "C" DLL_EXPORT int GetFavoriteGameQueryPort(int index)
{
	CheckInitialized(0);
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

/*
@desc Returns the list the favorite game server is on by index.

_Note: You must call GetFavoriteGameCount before calling this._
@param index The index of the favorite game server to get the details of. This must be between 0 and GetFavoriteGameCount() - 1
@return 0 = FavoriteFlagNone, 1 = FavoriteFlagFavorite, or 2 = FavoriteFlagHistory
@url https://partner.steamgames.com/doc/api/ISteamMatchmaking#GetFavoriteGame
*/
extern "C" DLL_EXPORT int GetFavoriteGameFlags(int index)
{
	CheckInitialized(0);
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

/*
@desc Returns the time the server was last added to the favorites list by index.

_Note: You must call GetFavoriteGameCount before calling this._
@param index The index of the favorite game server to get the details of. This must be between 0 and GetFavoriteGameCount() - 1
@return Time in Unix epoch format.
@url https://partner.steamgames.com/doc/api/ISteamMatchmaking#GetFavoriteGame
*/
extern "C" DLL_EXPORT int GetFavoriteGameUnixTimeLastPlayedOnServer(int index)
{
	CheckInitialized(0);
	AppId_t nAppID;
	uint32 nIP;
	uint16 nConnPort;
	uint16 nQueryPort;
	uint32 nFlags;
	uint32 rTime32LastPlayedOnServer;
	if (SteamMatchmaking()->GetFavoriteGame(index, &nAppID, &nIP, &nConnPort, &nQueryPort, &nFlags, &rTime32LastPlayedOnServer))
	{
		return rTime32LastPlayedOnServer;
	}
	return 0;
}

/*
@desc Gets the number of favorite and recent game servers the user has stored locally.
@return An integer.
@url https://partner.steamgames.com/doc/api/ISteamMatchmaking#GetFavoriteGameCount
*/
extern "C" DLL_EXPORT int GetFavoriteGameCount()
{
	CheckInitialized(0);
	return SteamMatchmaking()->GetFavoriteGameCount();
}

//GetLobbyByIndex - used in LobbyMatchList_t call result.
//GetLobbyChatEntry - used in LobbyChatMsg_t callback.

/*
@desc Gets the metadata associated with the specified key from the specified lobby.
@param hLobbySteamID The Steam ID of the lobby to get the metadata from.
@param key The key to get the value of.
@return The value for the given key or an empty string if the key or lobby doesn't exist.
@url https://partner.steamgames.com/doc/api/ISteamMatchmaking#GetLobbyData
*/
extern "C" DLL_EXPORT char *GetLobbyData(int hLobbySteamID, const char *key)
{
	CheckInitialized(NULL_STRING);
	return utils::CreateString(SteamMatchmaking()->GetLobbyData(SteamHandles()->GetSteamHandle(hLobbySteamID), key));
}

/*
@desc Gets all metadata for a given lobby.
@param hLobbySteamID The Steam ID of the lobby to get the metadata from.
@return A JSON string of key/value pairs for all data for a lobby.
An array to type KeyValuePair.
```
Type KeyValuePair
    Key as string
    Value as string
EndType
```
*/
extern "C" DLL_EXPORT char *GetLobbyDataJSON(int hLobbySteamID)
{
	// The only time GetLobbyDataCount and GetLobbyDataByIndex seem useful is to iterate all of the lobby data.
	// Returning the Key/Value list as JSON seems nicer than having to use GetLobbyDataCount, GetLobbyDataByIndexKey, and GetLobbyDataByIndexValue.
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

//TODO? Will this need a GetLobbyGameServer call to load the data and then these methods to get each piece?  Would be faster...
/*
@desc Gets the IP address of a game server set in a lobby.
@param hLobbySteamID The Steam ID of the lobby to get the game server information from.
@return The IP address for the lobby game server.
@url https://partner.steamgames.com/doc/api/ISteamMatchmaking#GetLobbyGameServer
*/
extern "C" DLL_EXPORT char *GetLobbyGameServerIP(int hLobbySteamID)
{
	CheckInitialized(NULL_STRING);
	uint32 nIP;
	uint16 nPort;
	CSteamID ulSteamID;
	if (SteamMatchmaking()->GetLobbyGameServer(SteamHandles()->GetSteamHandle(hLobbySteamID), &nIP, &nPort, &ulSteamID))
	{
		return utils::CreateString(utils::ToIPString(nIP));
	}
	return NULL_STRING;
}

/*
@desc Gets the connection port of a game server set in a lobby.
@param hLobbySteamID The Steam ID of the lobby to get the game server information from.
@return The connection port for the lobby game server.
@url https://partner.steamgames.com/doc/api/ISteamMatchmaking#GetLobbyGameServer
*/
extern "C" DLL_EXPORT int GetLobbyGameServerPort(int hLobbySteamID)
{
	CheckInitialized(0);
	uint32 nIP;
	uint16 nPort;
	CSteamID ulSteamID;
	if (SteamMatchmaking()->GetLobbyGameServer(SteamHandles()->GetSteamHandle(hLobbySteamID), &nIP, &nPort, &ulSteamID))
	{
		return nPort;
	}
	return 0;
}

/*
@desc Gets the Steam ID handle of a game server set in a lobby.
@param hLobbySteamID The Steam ID of the lobby to get the game server information from.
@return The Steam ID handle for the lobby game server.
@url https://partner.steamgames.com/doc/api/ISteamMatchmaking#GetLobbyGameServer
*/
extern "C" DLL_EXPORT int GetLobbyGameServerSteamID(int hLobbySteamID)
{
	CheckInitialized(0);
	uint32 nIP;
	uint16 nPort;
	CSteamID ulSteamID;
	if (SteamMatchmaking()->GetLobbyGameServer(SteamHandles()->GetSteamHandle(hLobbySteamID), &nIP, &nPort, &ulSteamID))
	{
		return SteamHandles()->GetPluginHandle(ulSteamID);
	}
	return 0;
}

/*
@desc Gets the Steam ID handle of the lobby member at the given index.
@param hLobbySteamID This MUST be the same lobby used in the previous call to GetNumLobbyMembers!
@param index An index between 0 and GetNumLobbyMembers.
@return A Steam ID handle.
@url https://partner.steamgames.com/doc/api/ISteamMatchmaking#GetLobbyMemberByIndex
*/
extern "C" DLL_EXPORT int GetLobbyMemberByIndex(int hLobbySteamID, int index)
{
	CheckInitialized(0);
	return SteamHandles()->GetPluginHandle(SteamMatchmaking()->GetLobbyMemberByIndex(SteamHandles()->GetSteamHandle(hLobbySteamID), index));
}

/*
@desc Gets per-user metadata from another player in the specified lobby.
@param hLobbySteamID The Steam ID handle of the lobby that the other player is in.
@param hSteamIDUser The Steam ID handle of the player to get the metadata from.
@param key The key to get the value of.
@return The value for the given key or an empty string if the key, member, or lobby doesn't exist.
@url https://partner.steamgames.com/doc/api/ISteamMatchmaking#GetLobbyMemberData
*/
extern "C" DLL_EXPORT char *GetLobbyMemberData(int hLobbySteamID, int hSteamIDUser, const char *key)
{
	CheckInitialized(NULL_STRING);
	return utils::CreateString(SteamMatchmaking()->GetLobbyMemberData(SteamHandles()->GetSteamHandle(hLobbySteamID), SteamHandles()->GetSteamHandle(hSteamIDUser), key));
}

/*
@desc The current limit on the number of users who can join the lobby.
@param hLobbySteamID The Steam ID handle of the lobby to get the member limit of.
@return A positive integer or 0 if no limit.
@url https://partner.steamgames.com/doc/api/ISteamMatchmaking#GetLobbyMemberLimit
*/
extern "C" DLL_EXPORT int GetLobbyMemberLimit(int hLobbySteamID)
{
	CheckInitialized(0);
	return SteamMatchmaking()->GetLobbyMemberLimit(SteamHandles()->GetSteamHandle(hLobbySteamID));
}

/*
@desc Returns the current lobby owner.
@param hLobbySteamID The Steam ID handle of the lobby to get the owner of.
@return A steam ID handle or 0.
@url https://partner.steamgames.com/doc/api/ISteamMatchmaking#GetLobbyOwner
*/
extern "C" DLL_EXPORT int GetLobbyOwner(int hLobbySteamID)
{
	CheckInitialized(0);
	return SteamHandles()->GetPluginHandle(SteamMatchmaking()->GetLobbyOwner(SteamHandles()->GetSteamHandle(hLobbySteamID)));
}

/*
@desc Gets the number of users in a lobby.
@param hLobbySteamID The Steam ID handle of the lobby to get the number of members of.
@return The number of members in the lobby, 0 if the current user has no data from the lobby.
@url https://partner.steamgames.com/doc/api/ISteamMatchmaking#GetNumLobbyMembers
*/
extern "C" DLL_EXPORT int GetNumLobbyMembers(int hLobbySteamID)
{
	CheckInitialized(0);
	return SteamMatchmaking()->GetNumLobbyMembers(SteamHandles()->GetSteamHandle(hLobbySteamID));
}

/*
@desc Invite another user to the lobby.
@param hLobbySteamID The Steam ID handle of the lobby to invite the user to.
@param hInviteeSteamID The Steam ID handle of the person who will be invited.
@return 1 when the invitation was sent successfully; otherwise 0.
@url https://partner.steamgames.com/doc/api/ISteamMatchmaking#InviteUserToLobby
*/
extern "C" DLL_EXPORT int InviteUserToLobby(int hLobbySteamID, int hInviteeSteamID)
{
	CheckInitialized(false);
	return SteamMatchmaking()->InviteUserToLobby(SteamHandles()->GetSteamHandle(hLobbySteamID), SteamHandles()->GetSteamHandle(hInviteeSteamID));
}

class CJoinLobbyCallResult : public CCallResultItem<LobbyEnter_t, WrappedResponse <LobbyEnter_t, uint64, &LobbyEnter_t::m_ulSteamIDLobby>>
{
public:
	CJoinLobbyCallResult(CSteamID steamIDLobby)
	{
		m_CallResultName = "JoinLobby(" + std::to_string(steamIDLobby.ConvertToUint64()) + ")";
		m_hSteamAPICall = SteamMatchmaking()->JoinLobby(steamIDLobby);
	}
	uint64 GetLobbyEnterHandle() { return m_Response.m_ulSteamIDLobby; }
	int GetLobbyEnterChatPermissions() { return (int)m_Response.m_rgfChatPermissions; }
	int GetLobbyEnterLocked() { return (int)m_Response.m_bLocked; }
	int GetLobbyEnterChatRoomEnterResponse() { return (int)m_Response.m_EChatRoomEnterResponse; }
};

/*
@desc Joins an existing lobby.
@param hLobbySteamID The Steam ID handle of the lobby
@callback-type callresult
@callback-getters GetJoinLobbyChatRoomEnterResponse, GetJoinLobbyHandle, GetJoinLobbyLocked
@return A [call result handle](Callbacks-and-Call-Results#call-results) on success; otherwise 0.
@url https://partner.steamgames.com/doc/api/ISteamMatchmaking#JoinLobby
*/
extern "C" DLL_EXPORT int JoinLobby(int hLobbySteamID)
{
	CheckInitialized(false);
	//Callbacks()->RegisterLobbyEnterCallback();
	Callbacks()->LobbyEnter.Register();
	return CallResults()->Add(new CJoinLobbyCallResult(SteamHandles()->GetSteamHandle(hLobbySteamID)));
}

// Unused - always 0
//int GetJoinLobbyChatPermissions(int hCallResult)
//{
//	return GetCallResultValue(hCallResult, &CJoinLobbyCallResult::GetLobbyEnterChatPermissions);
//}

/*
@desc Returns the EChatRoomEnterResponse for the JoinLobby call.
@param hCallResult A JoinLobby call result handle.
@return An EChatRoomEnterResponse value.
@return-url https://partner.steamgames.com/doc/api/steam_api#EChatRoomEnterResponse
@url https://partner.steamgames.com/doc/api/ISteamMatchmaking#LobbyEnter_t
*/
extern "C" DLL_EXPORT int GetJoinLobbyChatRoomEnterResponse(int hCallResult)
{
	return GetCallResultValue(hCallResult, &CJoinLobbyCallResult::GetLobbyEnterChatRoomEnterResponse);
}

/*
@desc Returns the lobby Steam ID handle for the JoinLobby call.
@param hCallResult A JoinLobby call result handle.
@return The lobby Steam ID handle.
@url https://partner.steamgames.com/doc/api/ISteamMatchmaking#LobbyEnter_t
*/
extern "C" DLL_EXPORT int GetJoinLobbyHandle(int hCallResult)
{
	return GetCallResultValue(hCallResult, &CJoinLobbyCallResult::GetLobbyEnterHandle);
}

/*
@desc Returns whether the lobby is locked for the JoinLobby call.
@param hCallResult A JoinLobby call result handle.
@return 1 if only invited users can join the lobby; otherwise 0.
@url https://partner.steamgames.com/doc/api/ISteamMatchmaking#LobbyEnter_t
*/
extern "C" DLL_EXPORT int GetJoinLobbyLocked(int hCallResult)
{
	return GetCallResultValue(hCallResult, &CJoinLobbyCallResult::GetLobbyEnterLocked);
}

/*
@desc Leave a lobby that the user is currently in; this will take effect immediately on the client side, other users in the lobby will be notified by a LobbyChatUpdate_t callback.
@param hLobbySteamID The Steam ID handle of the lobby to leave.
@url https://partner.steamgames.com/doc/api/ISteamMatchmaking#LeaveLobby
*/
extern "C" DLL_EXPORT void LeaveLobby(int hLobbySteamID)
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
	if (g_JoinedLobbies.size() == 0)
	{
		// Unregister the in-lobby callbacks when leaving the last lobby.
		// Should be the same callbacks that get registered in CCallbacks::OnLobbyEnter.
		agk::Log("Unregistering in-lobby callbacks");
		Callbacks()->LobbyChatMessage.Unregister();
		Callbacks()->LobbyChatUpdate.Unregister();
		Callbacks()->LobbyDataUpdate.Unregister();
		Callbacks()->LobbyGameCreated.Unregister();
	}
}

/*
@desc Removes the game server from the local favorites list.
@param appID The App ID of the game.
@param ipv4 The IP address of the server.
@param connectPort The port used to connect to the server, in host order.
@param queryPort The port used to query the server, in host order.
@param flags Whether the server is on the favorites list or history list. See k_unFavoriteFlagNone for more information.
@param-url flags https://partner.steamgames.com/doc/api/ISteamMatchmaking#k_unFavoriteFlagNone
@return 1 if the server was removed; otherwise, 0 if the specified server was not on the users local favorites list.
@url https://partner.steamgames.com/doc/api/ISteamMatchmaking#RemoveFavoriteGame
@url https://partner.steamgames.com/doc/api/ISteamMatchmaking#k_unFavoriteFlagNone
@url https://partner.steamgames.com/doc/api/ISteamMatchmaking#k_unFavoriteFlagFavorite
@url https://partner.steamgames.com/doc/api/ISteamMatchmaking#k_unFavoriteFlagHistory
*/
extern "C" DLL_EXPORT int RemoveFavoriteGame(int appID, const char *ipv4, int connectPort, int queryPort, int flags)
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

/*
@desc Refreshes all of the metadata for a lobby that you're not in right now.
@param hLobbySteamID The Steam ID of the lobby to refresh the metadata of.
@return 1 when the request for lobby data succeeds will be reported by the LobbyDataUpdate_t callback. 0 when the request fails.
@url https://partner.steamgames.com/doc/api/ISteamMatchmaking#RequestLobbyData
@url https://partner.steamgames.com/doc/api/ISteamMatchmaking#LobbyDataUpdate_t
*/
extern "C" DLL_EXPORT int RequestLobbyData(int hLobbySteamID)
{
	CheckInitialized(false);
	return SteamMatchmaking()->RequestLobbyData(SteamHandles()->GetSteamHandle(hLobbySteamID));
}

class CRequestLobbyListCallResult : public CCallResultItem<LobbyMatchList_t, AlwaysOKResponse<LobbyMatchList_t>>
{
public:
	CRequestLobbyListCallResult()
	{
		m_CallResultName = "RequestLobbyList()";
		m_hSteamAPICall = SteamMatchmaking()->RequestLobbyList();
	}
	int GetLobbyCount() { return (int)m_Lobbies.size(); }
	bool IsValidIndex(int index) { return index >= 0 && index < (int)m_Lobbies.size(); }
	uint64 GetLobby(int index) { return m_Lobbies[index].ConvertToUint64(); }
protected:
	std::vector<CSteamID> m_Lobbies;
	void OnResponse(LobbyMatchList_t *pCallResult, bool bFailure)
	{
		CCallResultItem::OnResponse(pCallResult, bFailure);
		for (int index = 0; index < (int)m_Response.m_nLobbiesMatching; index++)
		{
			m_Lobbies.push_back(SteamMatchmaking()->GetLobbyByIndex(index));
		}
	}
};

/*
@desc Request a filtered list of relevant lobbies.
@callback-type callresult
@callback-getters GetRequestLobbyListCount, GetRequestLobbyListHandle
@return A [call result handle](Callbacks-and-Call-Results#call-results) on success; otherwise 0.
@url https://partner.steamgames.com/doc/api/ISteamMatchmaking#RequestLobbyList
*/
extern "C" DLL_EXPORT int RequestLobbyList()
{
	CheckInitialized(0);
	return CallResults()->Add(new CRequestLobbyListCallResult());
}

/*
@desc Gets the number of lobbies returned by the RequestLobbyList call.
@param hCallResult A RequestLobbyList call result handle.
@return The list count.
@url https://partner.steamgames.com/doc/api/ISteamMatchmaking#LobbyMatchList_t
*/
extern "C" DLL_EXPORT int GetRequestLobbyListCount(int hCallResult)
{
	return GetCallResultValue(hCallResult, &CRequestLobbyListCallResult::GetLobbyCount);
}

/*
@desc Gets the lobby Steam ID handle returned by the RequestLobbyList call by index.
@param hCallResult A RequestLobbyList call result handle.
@param index The lobby index.
@return A Steam ID handle.
@url https://partner.steamgames.com/doc/api/ISteamMatchmaking#LobbyMatchList_t
*/
extern "C" DLL_EXPORT int GetRequestLobbyListHandle(int hCallResult, int index)
{
	return GetCallResultValue(hCallResult, index, &CRequestLobbyListCallResult::GetLobby, __FUNCTION__);
}

/*
@desc Broadcasts a chat message to the all of the users in the lobby.
@param hLobbySteamID The Steam ID handle of the lobby to send the chat message to.
@param memblockID A memblock containing the message to send.
@return 1 when the send succeeds; otherwise 0.
@url https://partner.steamgames.com/doc/api/ISteamMatchmaking#SendLobbyChatMessage
@plugin-name SendLobbyChatMessage
*/
extern "C" DLL_EXPORT int SendLobbyChatMessageMemblock(int hLobbySteamID, int memblockID)
{
	CheckInitialized(0);
	return SteamMatchmaking()->SendLobbyChatMsg(SteamHandles()->GetSteamHandle(hLobbySteamID), agk::GetMemblockPtr(memblockID), agk::GetMemblockSize(memblockID));
}

/*
@desc Broadcasts a chat message to the all of the users in the lobby.
@param hLobbySteamID The Steam ID handle of the lobby to send the chat message to.
@param message The message to send.
@return 1 when the send succeeds; otherwise 0.
@url https://partner.steamgames.com/doc/api/ISteamMatchmaking#SendLobbyChatMessage
*/
extern "C" DLL_EXPORT int SendLobbyChatMessage(int hLobbySteamID, const char *message)
{
	CheckInitialized(0);
	return SteamMatchmaking()->SendLobbyChatMsg(SteamHandles()->GetSteamHandle(hLobbySteamID), message, (int)strnlen_s(message, MAX_CHAT_MESSAGE_LENGTH) + 1);
}

// SetLinkedLobby - unused

/*
@desc Sets a key/value pair in the lobby metadata.

_This can only be set by the owner of the lobby._
@param hLobbySteamID The Steam ID of the lobby to set the metadata for.
@param key The key to set the data for.
@param value The value to set.
@url https://partner.steamgames.com/doc/api/ISteamMatchmaking#SetLobbyData
*/
extern "C" DLL_EXPORT void SetLobbyData(int hLobbySteamID, const char *key, const char *value)
{
	CheckInitialized(NORETURN);
	SteamMatchmaking()->SetLobbyData(SteamHandles()->GetSteamHandle(hLobbySteamID), key, value);
}

/*
@desc Sets the game server associated with the lobby.  This method only accepts IPv4 addresses.
Either an IP/port or a Game Server SteamID handle must be given.

_This can only be set by the owner of the lobby._
@param hLobbySteamID The Steam ID handle of the lobby to set the game server information for.
@param gameServerIP Sets the IP address of the game server.
@param gameServerPort Sets the connection port of the game server.
@param hGameServerSteamID Sets the Steam ID handle of the game server.
@return 1 when the call succeeds; otherwise 0.
@url https://partner.steamgames.com/doc/api/ISteamMatchmaking#SetLobbyGameServer
*/
extern "C" DLL_EXPORT int SetLobbyGameServer(int hLobbySteamID, const char *gameServerIP, int gameServerPort, int hGameServerSteamID)
{
	CheckInitialized(false);
	Callbacks()->LobbyGameCreated.Register();
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

/*
@desc Sets whether or not a lobby is joinable by other players. This always defaults to enabled for a new lobby.

_This can only be set by the owner of the lobby._
@param hLobbySteamID The Steam ID handle of the lobby
@param lobbyJoinable 1 to allow or 0 to disallow users to join this lobby.
@return 1 when the call succeeds; otherwise 0.
@url https://partner.steamgames.com/doc/api/ISteamMatchmaking#SetLobbyJoinable
*/
extern "C" DLL_EXPORT int SetLobbyJoinable(int hLobbySteamID, int lobbyJoinable)
{
	CheckInitialized(false);
	return SteamMatchmaking()->SetLobbyJoinable(SteamHandles()->GetSteamHandle(hLobbySteamID), lobbyJoinable != 0);
}

/*
@desc Sets per-user metadata for the local user.

Each user in the lobby will be receive notification of the lobby data change via HasLobbyDataUpdated, and any new users joining will receive any existing data.
@param hLobbySteamID The Steam ID of the lobby to set our metadata in.
@param key The key to set the data for.
@param value The value to set.
@url https://partner.steamgames.com/doc/api/ISteamMatchmaking#SetLobbyMemberData
*/
extern "C" DLL_EXPORT void SetLobbyMemberData(int hLobbySteamID, const char *key, const char *value)
{
	CheckInitialized(NORETURN);
	return SteamMatchmaking()->SetLobbyMemberData(SteamHandles()->GetSteamHandle(hLobbySteamID), key, value);
}

/*
@desc Set the maximum number of players that can join the lobby.

_This can only be set by the owner of the lobby._
@param hLobbySteamID The Steam ID handle of the lobby to set the member limit for.
@param maxMembers The maximum number of players allowed in this lobby. This can not be above 250.
@return 1 if the limit was successfully set; otherwise 0.
@url https://partner.steamgames.com/doc/api/ISteamMatchmaking#SetLobbyMemberLimit
*/
extern "C" DLL_EXPORT int SetLobbyMemberLimit(int hLobbySteamID, int maxMembers)
{
	CheckInitialized(0);
	return SteamMatchmaking()->SetLobbyMemberLimit(SteamHandles()->GetSteamHandle(hLobbySteamID), maxMembers);
}

/*
@desc Changes who the lobby owner is.
Triggers HasLobbyChatUpdate for all lobby members.

_This can only be set by the owner of the lobby._
@param hLobbySteamID The Steam ID handle of the lobby where the owner change will take place.
@param hNewOwnerSteamID The Steam ID handle of the user that will be the new owner of the lobby, they must be in the lobby.
@return 1 if the owner is successfully changed; otherwise 0.
@url https://partner.steamgames.com/doc/api/ISteamMatchmaking#SetLobbyOwner
*/
extern "C" DLL_EXPORT int SetLobbyOwner(int hLobbySteamID, int hNewOwnerSteamID)
{
	CheckInitialized(0);
	return SteamMatchmaking()->SetLobbyOwner(SteamHandles()->GetSteamHandle(hLobbySteamID), SteamHandles()->GetSteamHandle(hNewOwnerSteamID));
}

/*
@desc Updates what type of lobby this is.

_This can only be set by the owner of the lobby._
@param hLobbySteamID The Steam ID handle of the lobby
@param eLobbyType The new lobby type to that will be set.
@param-url eLobbyType https://partner.steamgames.com/doc/api/ISteamMatchmaking#ELobbyType
@return 1 when the call succeeds; otherwise 0.
@url https://partner.steamgames.com/doc/api/ISteamMatchmaking#SetLobbyType
*/
extern "C" DLL_EXPORT int SetLobbyType(int hLobbySteamID, int eLobbyType)
{
	CheckInitialized(false);
	return SteamMatchmaking()->SetLobbyType(SteamHandles()->GetSteamHandle(hLobbySteamID), (ELobbyType)eLobbyType);
}

// Callbacks
/*
@desc Triggered when a favorites list change message has been received.
@callback-type list
@callback-getters	GetFavoritesListChangedIP, GetFavoritesListChangedQueryPort, GetFavoritesListChangedConnectionPort, GetFavoritesListChangedAppID,
GetFavoritesListChangedFlags, GetFavoritesListChangedIsAdd, GetFavoritesListChangedAccountID
@return 1 when the callback has more responses to process; otherwise 0.
@url https://partner.steamgames.com/doc/api/ISteamMatchmaking#FavoritesListAccountsUpdated_t
*/
extern "C" DLL_EXPORT int HasFavoritesListChangedResponse()
{
	Callbacks()->FavoritesListChanged.Register();
	return Callbacks()->FavoritesListChanged.HasResponse();
}

/*
@desc Gets the IP of the current FavoritesListAccountsUpdated_t callback reponse.
When an empty string, reload the entire list; otherwise it means just one server.
@return An IP address or empty string.
@url https://partner.steamgames.com/doc/api/ISteamMatchmaking#FavoritesListAccountsUpdated_t
*/
extern "C" DLL_EXPORT char *GetFavoritesListChangedIP()
{
	return utils::CreateString(utils::ToIPString(Callbacks()->FavoritesListChanged.GetCurrent().m_nIP));
}

/*
@desc Gets the query port of the current FavoritesListAccountsUpdated_t callback reponse.
@return An integer.
@url https://partner.steamgames.com/doc/api/ISteamMatchmaking#FavoritesListAccountsUpdated_t
*/
extern "C" DLL_EXPORT int GetFavoritesListChangedQueryPort()
{
	return Callbacks()->FavoritesListChanged.GetCurrent().m_nQueryPort;
}

/*
@desc Gets the connection port of the current FavoritesListAccountsUpdated_t callback reponse.
@return An integer.
@url https://partner.steamgames.com/doc/api/ISteamMatchmaking#FavoritesListAccountsUpdated_t
*/
extern "C" DLL_EXPORT int GetFavoritesListChangedConnectionPort()
{
	return Callbacks()->FavoritesListChanged.GetCurrent().m_nConnPort;
}

/*
@desc Gets the App ID of the game server for the current FavoritesListAccountsUpdated_t callback reponse.
@return An integer.
@url https://partner.steamgames.com/doc/api/ISteamMatchmaking#FavoritesListAccountsUpdated_t
*/
extern "C" DLL_EXPORT int GetFavoritesListChangedAppID()
{
	return Callbacks()->FavoritesListChanged.GetCurrent().m_nAppID;
}

/*
@desc Gets the query port of the current FavoritesListAccountsUpdated_t callback reponse.
@return Whether the server is on the favorites list or history list. See k_unFavoriteFlagNone for more information.
@url https://partner.steamgames.com/doc/api/ISteamMatchmaking#FavoritesListAccountsUpdated_t
@url https://partner.steamgames.com/doc/api/ISteamMatchmaking#k_unFavoriteFlagNone
@url https://partner.steamgames.com/doc/api/ISteamMatchmaking#k_unFavoriteFlagFavorite
@url https://partner.steamgames.com/doc/api/ISteamMatchmaking#k_unFavoriteFlagHistory
*/
extern "C" DLL_EXPORT int GetFavoritesListChangedFlags()
{
	return Callbacks()->FavoritesListChanged.GetCurrent().m_nFlags;
}

/*
@desc Gets whether the game server was added (1) or removed (0) from the list for the current FavoritesListAccountsUpdated_t callback reponse.
@return 1 if added, 0 if removed.
@url https://partner.steamgames.com/doc/api/ISteamMatchmaking#FavoritesListAccountsUpdated_t
*/
extern "C" DLL_EXPORT int GetFavoritesListChangedIsAdd()
{
	return Callbacks()->FavoritesListChanged.GetCurrent().m_bAdd;
}

/*
@desc Gets the account ID of the current FavoritesListAccountsUpdated_t callback reponse.
@return An integer.
@url https://partner.steamgames.com/doc/api/ISteamMatchmaking#FavoritesListAccountsUpdated_t
*/
extern "C" DLL_EXPORT int GetFavoritesListChangedAccountID()
{
	return Callbacks()->FavoritesListChanged.GetCurrent().m_unAccountId;
}

/*
@desc Triggered when a lobby chat message has been received.
@callback-type list
@callback-getters	GetLobbyChatMessageLobby, GetLobbyChatMessageUser, GetLobbyChatMessageText
@return 1 when the callback has more responses to process; otherwise 0.
@url https://partner.steamgames.com/doc/api/ISteamMatchmaking#LobbyChatMsg_t
*/
extern "C" DLL_EXPORT int HasLobbyChatMessageResponse()
{
	CheckInitialized(false);
	return Callbacks()->LobbyChatMessage.HasResponse();
}

/*
@desc Gets the lobby for the current LobbyChatMsg_t callback response.
@return A lobby Steam ID handle.
@url https://partner.steamgames.com/doc/api/ISteamMatchmaking#LobbyChatMsg_t
*/
extern "C" DLL_EXPORT int GetLobbyChatMessageLobby()
{
	CheckInitialized(0);
	return SteamHandles()->GetPluginHandle(Callbacks()->LobbyChatMessage.GetCurrent().m_ulSteamIDLobby);
}

/*
@desc Gets the Steam ID handle of the user for the current LobbyChatMsg_t callback response.
@return A Steam ID handle.
@url https://partner.steamgames.com/doc/api/ISteamMatchmaking#GetLobbyChatEntry
@url https://partner.steamgames.com/doc/api/ISteamMatchmaking#LobbyChatMsg_t
*/
extern "C" DLL_EXPORT int GetLobbyChatMessageUser()
{
	CheckInitialized(0);
	return SteamHandles()->GetPluginHandle(Callbacks()->LobbyChatMessage.GetCurrent().m_ulSteamIDUser);
}

// Always k_EChatEntryTypeChatMsg
///*
//@desc Gets the chat entry type for the current LobbyChatMsg_t callback response.
//@return An EChatEntryType value.
//@url https://partner.steamgames.com/doc/api/ISteamMatchmaking#LobbyChatMsg_t
//@url https://partner.steamgames.com/doc/api/steam_api#EChatEntryType
//*/
//extern "C" DLL_EXPORT int GetLobbyChatMessageEntryType()
//{
//	CheckInitialized(0);
//	return SteamHandles()->GetPluginHandle(Callbacks()->GetLobbyChatMessage().m_eChatEntryType);
//}

/*
@desc Gets the chat message in a memblock for the current LobbyChatMsg_t callback response.
The memblock will be deleted the next time HasLobbyChatMessageResponse is called.
@return A memblock containing the contents of the chat message.
@url https://partner.steamgames.com/doc/api/ISteamMatchmaking#GetLobbyChatEntry
@url https://partner.steamgames.com/doc/api/ISteamMatchmaking#LobbyChatMsg_t
*/
extern "C" DLL_EXPORT int GetLobbyChatMessageMemblock()
{
	return Callbacks()->LobbyChatMessage.GetCurrent().m_MemblockID;
}

/*
@desc Gets the chat message as text for the current LobbyChatMsg_t callback response.
@return The contents of the chat message.
@url https://partner.steamgames.com/doc/api/ISteamMatchmaking#GetLobbyChatEntry
@url https://partner.steamgames.com/doc/api/ISteamMatchmaking#LobbyChatMsg_t
*/
extern "C" DLL_EXPORT char *GetLobbyChatMessageText()
{
	CheckInitialized(NULL_STRING);
	int memblock = Callbacks()->LobbyChatMessage.GetCurrent().m_MemblockID;
	if (memblock)
	{
		return agk::GetMemblockString(memblock, 0, agk::GetMemblockSize(memblock));
	}
	return NULL_STRING;
}

/*
@desc Triggered when a lobby chat room state has changed, this is usually sent when a user has joined or left the lobby.
@callback-type list
@callback-getters GetLobbyChatUpdateLobby, GetLobbyChatUpdateUserChanged, GetLobbyChatUpdateUserState, GetLobbyChatUpdateUserMakingChange
@return 1 when the callback has more responses to process; otherwise 0.
@url https://partner.steamgames.com/doc/api/ISteamMatchmaking#LobbyChatUpdate_t
*/
extern "C" DLL_EXPORT int HasLobbyChatUpdateResponse()
{
	CheckInitialized(false);
	return Callbacks()->LobbyChatUpdate.HasResponse();
}

/*
@desc The lobby for the current LobbyChatUpdate_t callback response.
@return A Steam ID handle.
@url https://partner.steamgames.com/doc/api/ISteamMatchmaking#LobbyChatUpdate_t
*/
extern "C" DLL_EXPORT int GetLobbyChatUpdateLobby()
{
	CheckInitialized(0);
	return SteamHandles()->GetPluginHandle(Callbacks()->LobbyChatUpdate.GetCurrent().m_ulSteamIDLobby);
}

/*
@desc The user whose status in the lobby has changed for the current LobbyChatUpdate_t callback response.
@return A Steam ID handle.
@url https://partner.steamgames.com/doc/api/ISteamMatchmaking#LobbyChatUpdate_t
*/
extern "C" DLL_EXPORT int GetLobbyChatUpdateUserChanged()
{
	CheckInitialized(0);
	return SteamHandles()->GetPluginHandle(Callbacks()->LobbyChatUpdate.GetCurrent().m_ulSteamIDUserChanged);
}

/*
@desc The new user state for the user whose status changed for the current LobbyChatUpdate_t callback response.
@return EChatMemberStateChange bit data
@return-url https://partner.steamgames.com/doc/api/ISteamMatchmaking#EChatMemberStateChange
@url https://partner.steamgames.com/doc/api/ISteamMatchmaking#LobbyChatUpdate_t
*/
extern "C" DLL_EXPORT int GetLobbyChatUpdateUserState()
{
	CheckInitialized(0);
	return Callbacks()->LobbyChatUpdate.GetCurrent().m_rgfChatMemberStateChange;
}

/*
@desc Chat member who made the change for the current LobbyChatUpdate_t callback response.
@return A Steam ID handle.
@url https://partner.steamgames.com/doc/api/ISteamMatchmaking#LobbyChatUpdate_t
*/
extern "C" DLL_EXPORT int GetLobbyChatUpdateUserMakingChange()
{
	CheckInitialized(0);
	return SteamHandles()->GetPluginHandle(Callbacks()->LobbyChatUpdate.GetCurrent().m_ulSteamIDMakingChange);
}

/*
@desc Triggered when the lobby metadata has changed.
@callback-type list
@callback-getters GetLobbyDataUpdateLobby, GetLobbyDataUpdateMember, GetLobbyDataUpdateSuccess
@return 1 when the callback has more responses to process; otherwise 0.
@url https://partner.steamgames.com/doc/api/ISteamMatchmaking#LobbyDataUpdate_t
*/
extern "C" DLL_EXPORT int HasLobbyDataUpdateResponse()
{
	CheckInitialized(false);
	return Callbacks()->LobbyDataUpdate.HasResponse();
}

/*
@desc Returns the lobby whose metadata has changed for the current LobbyDataUpdate_t callback response.
@return A Steam ID handle.
@url https://partner.steamgames.com/doc/api/ISteamMatchmaking#LobbyDataUpdate_t
*/
extern "C" DLL_EXPORT int GetLobbyDataUpdateLobby()
{
	return SteamHandles()->GetPluginHandle(Callbacks()->LobbyDataUpdate.GetCurrent().m_ulSteamIDLobby);
}

/*
@desc Returns the handle of the member whose metadata has changed for the current LobbyDataUpdate_t callback response.
If this value is a user in the lobby, then use GetLobbyMemberData to access per-user details;
otherwise, if GetLobbyDataUpdateMember == GetLobbyDataUpdateLobby, use GetLobbyData to access the lobby metadata.
@return A Steam ID handle.
@url https://partner.steamgames.com/doc/api/ISteamMatchmaking#LobbyDataUpdate_t
*/
extern "C" DLL_EXPORT int GetLobbyDataUpdateMember()
{
	return SteamHandles()->GetPluginHandle(Callbacks()->LobbyDataUpdate.GetCurrent().m_ulSteamIDMember);
}

/*
@desc Returns the success of the lobby data update for the current LobbyDataUpdate_t callback response.
@return 1 if the lobby data was successfully changed, otherwise 0.
@url https://partner.steamgames.com/doc/api/ISteamMatchmaking#LobbyDataUpdate_t
*/
extern "C" DLL_EXPORT int GetLobbyDataUpdateSuccess()
{
	return SteamHandles()->GetPluginHandle(Callbacks()->LobbyDataUpdate.GetCurrent().m_bSuccess);
}

/*
@desc Recieved upon attempting to enter a lobby. Lobby metadata is available to use immediately after receiving this.
@callback-type list
@callback-getters GetLobbyEnterChatRoomEnterResponse, GetLobbyEnterLobby, GetLobbyEnterLocked
@return 1 when the callback has more responses to process; otherwise 0.
@url https://partner.steamgames.com/doc/api/ISteamMatchmaking#LobbyEnter_t
*/
extern "C" DLL_EXPORT int HasLobbyEnterResponse()
{
	CheckInitialized(false);
	return Callbacks()->LobbyEnter.HasResponse();
}

// Unused - always 0
//int GetLobbyEnterChatPermissions()
//{
//	return Callbacks()->GetLobbyEnter().m_rgfChatPermissions;
//}

/*
@desc Gets EChatRoomEnterResponse for the current LobbyEnter_t callback response.
@return An EChatRoomEnterResponse value.
@return-url https://partner.steamgames.com/doc/api/steam_api#EChatRoomEnterResponse
@url https://partner.steamgames.com/doc/api/ISteamMatchmaking#LobbyEnter_t
*/
extern "C" DLL_EXPORT int GetLobbyEnterChatRoomEnterResponse()
{
	return Callbacks()->LobbyEnter.GetCurrent().m_EChatRoomEnterResponse;
}

/*
@desc Returns the lobby for the current LobbyEnter_t callback response.
@return The lobby Steam ID handle.
@url https://partner.steamgames.com/doc/api/ISteamMatchmaking#LobbyEnter_t
*/
extern "C" DLL_EXPORT int GetLobbyEnterLobby()
{
	return SteamHandles()->GetPluginHandle(Callbacks()->LobbyEnter.GetCurrent().m_ulSteamIDLobby);
}

/*
@desc Returns the locked value for the current LobbyEnter_t callback response.
@return 1 if only invited users can join the lobby; otherwise 0.
@url https://partner.steamgames.com/doc/api/ISteamMatchmaking#LobbyEnter_t
*/
extern "C" DLL_EXPORT int GetLobbyEnterLocked()
{
	return Callbacks()->LobbyEnter.GetCurrent().m_bLocked;
}

/*
@desc Indicates that a lobby game was created since the last call.
_This can only be read once per lobby game creation!_
@callback-type list
@callback-getters GetLobbyGameCreatedGameServer, GetLobbyGameCreatedLobby, GetLobbyGameCreatedIP, GetLobbyGameCreatedPort
@return 1 when a lobby game was created; otherwise 0.
@url https://partner.steamgames.com/doc/api/ISteamMatchmaking#LobbyGameCreated_t
*/
extern "C" DLL_EXPORT int HasLobbyGameCreatedResponse()
{
	return Callbacks()->LobbyGameCreated.HasResponse();
}

/*
@desc Returns the current game server Steam ID handle for the LobbyGameCreated_t callback.
@return A Steam ID handle.
@url https://partner.steamgames.com/doc/api/ISteamMatchmaking#LobbyGameCreated_t
*/
extern "C" DLL_EXPORT int GetLobbyGameCreatedGameServer()
{
	return SteamHandles()->GetPluginHandle(Callbacks()->LobbyGameCreated.GetCurrent().m_ulSteamIDGameServer);
}

/*
@desc Returns the Steam ID handle of the lobby that set the game server for the current LobbyGameCreated_t callback response.
@return A Steam ID handle.
@url https://partner.steamgames.com/doc/api/ISteamMatchmaking#LobbyGameCreated_t
*/
extern "C" DLL_EXPORT int GetLobbyGameCreatedLobby()
{
	return SteamHandles()->GetPluginHandle(Callbacks()->LobbyGameCreated.GetCurrent().m_ulSteamIDLobby);
}

/*
@desc Returns the IP address of the game server for the current LobbyGameCreated_t callback response.
@return A Steam ID handle.
@url https://partner.steamgames.com/doc/api/ISteamMatchmaking#LobbyGameCreated_t
*/
extern "C" DLL_EXPORT char *GetLobbyGameCreatedIP()
{
	return utils::CreateString(utils::ToIPString(Callbacks()->LobbyGameCreated.GetCurrent().m_unIP));
}

/*
@desc Returns the connection port of the game server for the current LobbyGameCreated_t callback response.
@return A Steam ID handle.
@url https://partner.steamgames.com/doc/api/ISteamMatchmaking#LobbyGameCreated_t
*/
extern "C" DLL_EXPORT int GetLobbyGameCreatedPort()
{
	return Callbacks()->LobbyGameCreated.GetCurrent().m_usPort;
}
