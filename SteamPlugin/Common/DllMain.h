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

#ifndef _DLLMAIN_H_
#define _DLLMAIN_H_
#pragma once

/*
NOTE: Cannot use bool as an exported function return type because of AGK2 limitations.  Use int instead.
*/

#include "Common.h"

#pragma region ISteamMatchmaking
/* @page ISteamMatchmaking */
/*
@desc Adds the game server to the local favorites list or updates the time played of the server if it already exists in the list.

_The plugin sets the API call's rTime32LastPlayedOnServer parameter internally to the current client system time._
@param appID The App ID of the game.
@param ip The IP address of the server.
@param connectPort The port used to connect to the server.
@param queryPort The port used to query the server, in host order.
@param flags Sets the whether the server should be added to the favorites list or history list.
@param-api flags ISteamMatchmaking#k_unFavoriteFlagNone
@return An integer.  This appears to be an index position, but is not defined by the API.
@api ISteamMatchmaking#AddFavoriteGame, ISteamMatchmaking#k_unFavoriteFlagNone, ISteamMatchmaking#k_unFavoriteFlagFavorite, ISteamMatchmaking#k_unFavoriteFlagHistory
*/
extern "C" DLL_EXPORT int AddFavoriteGame(int appID, const char *ip, int connectPort, int queryPort, int flags);//, int time32LastPlayedOnServer);

//AddRequestLobbyListCompatibleMembersFilter

/*
@desc Sets the physical distance for which we should search for lobbies, this is based on the users IP address and a IP location map on the Steam backend.
@param eLobbyDistanceFilter Specifies the maximum distance.
@param-api eLobbyDistanceFilter ISteamMatchmaking#ELobbyDistanceFilter
@api ISteamMatchmaking#AddRequestLobbyListDistanceFilter
*/
extern "C" DLL_EXPORT void AddRequestLobbyListDistanceFilter(int eLobbyDistanceFilter);

/*
@desc Filters to only return lobbies with the specified number of open slots available.
@param slotsAvailable The number of open slots that must be open.
@api ISteamMatchmaking#AddRequestLobbyListFilterSlotsAvailable
*/
extern "C" DLL_EXPORT void AddRequestLobbyListFilterSlotsAvailable(int slotsAvailable);

/*
@desc Sorts the results closest to the specified value.
@param keyToMatch The filter key name to match.
@param valueToBeCloseTo The value that lobbies will be sorted on.
@api ISteamMatchmaking#AddRequestLobbyListNearValueFilter
*/
extern "C" DLL_EXPORT void AddRequestLobbyListNearValueFilter(const char *keyToMatch, int valueToBeCloseTo);

/*
@desc Adds a numerical comparison filter to the next RequestLobbyList call.
@param keyToMatch The filter key name to match.
@param valueToMatch The number to match.
@param eComparisonType The type of comparison to make.
@param-api eComparisonType ISteamMatchmaking#ELobbyComparison
@api ISteamMatchmaking#AddRequestLobbyListNumericalFilter
*/
extern "C" DLL_EXPORT void AddRequestLobbyListNumericalFilter(const char *keyToMatch, int valueToMatch, int eComparisonType);

/*
@desc Sets the maximum number of lobbies to return.
@param maxResults The maximum number of lobbies to return.
@api ISteamMatchmaking#AddRequestLobbyListResultCountFilter
*/
extern "C" DLL_EXPORT void AddRequestLobbyListResultCountFilter(int maxResults);

/*
@desc Adds a string comparison filter to the next RequestLobbyList call.
@param keyToMatch The filter key name to match.
@param valueToMatch The string to match.
@param eComparisonType The type of comparison to make.
@param-api eComparisonType ISteamMatchmaking#ELobbyComparison
@api ISteamMatchmaking#AddRequestLobbyListStringFilter
*/
extern "C" DLL_EXPORT void AddRequestLobbyListStringFilter(const char *keyToMatch, const char *valueToMatch, int eComparisonType);

/*
@desc Creates a new matchmaking lobby.  The lobby is joined once it is created.
@param eLobbyType The type and visibility of this lobby. This can be changed later via SetLobbyType.
@param-api eLobbyType ISteamMatchmaking#ELobbyType
@param maxMembers The maximum number of players that can join this lobby. This can not be above 250.
@callback-type callresult
@callback-getters GetCreateLobbyHandle
@return A [call result handle](Callbacks-and-Call-Results#call-results) on success; otherwise 0.
@api ISteamMatchmaking#CreateLobby
*/
extern "C" DLL_EXPORT int CreateLobby(int eLobbyType, int maxMembers);

/*
@desc Returns the lobby handle for the CreateLobby call.
@param hCallResult A CreateLobby call result handle.
@return A lobby SteamID handle or 0 if the call failed.
@api ISteamMatchmaking#CreateLobby, ISteamMatchmaking#LobbyCreated_t
*/
extern "C" DLL_EXPORT int GetCreateLobbyHandle(int hCallResult);

/*
@desc Removes a metadata key from the lobby.

_This can only be set by the owner of the lobby._
@param hLobbySteamID The Steam ID of the lobby to delete the metadata for.
@param key The key to delete the data for.
@return 1 when the request succeeds; otherwise 0.
@api ISteamMatchmaking#DeleteLobbyData
*/
extern "C" DLL_EXPORT int DeleteLobbyData(int hLobbySteamID, const char *key);

/*
@desc Returns the App ID of the favorite game server by index.

_Note: You must call GetFavoriteGameCount before calling this._
@param index The index of the favorite game server to get the details of. This must be between 0 and GetFavoriteGameCount() - 1
@return An App ID.
@api ISteamMatchmaking#GetFavoriteGame
*/
extern "C" DLL_EXPORT int GetFavoriteGameAppID(int index);

/*
@desc Returns the IP address of the favorite game server by index.

_Note: You must call GetFavoriteGameCount before calling this._
@param index The index of the favorite game server to get the details of. This must be between 0 and GetFavoriteGameCount() - 1
@return An IP address.
@api ISteamMatchmaking#GetFavoriteGame
*/
extern "C" DLL_EXPORT char *GetFavoriteGameIP(int index);

/*
@desc Returns the port used to connect to the favorite game server by index.

_Note: You must call GetFavoriteGameCount before calling this._
@param index The index of the favorite game server to get the details of. This must be between 0 and GetFavoriteGameCount() - 1
@return A port number.
@api ISteamMatchmaking#GetFavoriteGame
*/
extern "C" DLL_EXPORT int GetFavoriteGameConnectionPort(int index);

/*
@desc Returns the port used to query the favorite game server by index.

_Note: You must call GetFavoriteGameCount before calling this._
@param index The index of the favorite game server to get the details of. This must be between 0 and GetFavoriteGameCount() - 1
@return A port number.
@api ISteamMatchmaking#GetFavoriteGame
*/
extern "C" DLL_EXPORT int GetFavoriteGameQueryPort(int index);

/*
@desc Returns the list the favorite game server is on by index.

_Note: You must call GetFavoriteGameCount before calling this._
@param index The index of the favorite game server to get the details of. This must be between 0 and GetFavoriteGameCount() - 1
@return 0 = FavoriteFlagNone, 1 = FavoriteFlagFavorite, or 2 = FavoriteFlagHistory
@api ISteamMatchmaking#GetFavoriteGame
*/
extern "C" DLL_EXPORT int GetFavoriteGameFlags(int index);

/*
@desc Returns the time the server was last added to the favorites list by index.

_Note: You must call GetFavoriteGameCount before calling this._
@param index The index of the favorite game server to get the details of. This must be between 0 and GetFavoriteGameCount() - 1
@return Time in Unix epoch format.
@api ISteamMatchmaking#GetFavoriteGame
*/
extern "C" DLL_EXPORT int GetFavoriteGameUnixTimeLastPlayedOnServer(int index);

/*
@desc Gets the number of favorite and recent game servers the user has stored locally.
@return An integer.
@api ISteamMatchmaking#GetFavoriteGameCount
*/
extern "C" DLL_EXPORT int GetFavoriteGameCount();

/*
@desc Gets the metadata associated with the specified key from the specified lobby.
@param hLobbySteamID The Steam ID of the lobby to get the metadata from.
@param key The key to get the value of.
@return The value for the given key or an empty string if the key or lobby doesn't exist.
@api ISteamMatchmaking#GetLobbyData
*/
extern "C" DLL_EXPORT char *GetLobbyData(int hLobbySteamID, const char *key);

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
extern "C" DLL_EXPORT char *GetLobbyDataJSON(int hLobbySteamID);

/*
@desc Gets the IP address of a game server set in a lobby.
@param hLobbySteamID The Steam ID of the lobby to get the game server information from.
@return The IP address for the lobby game server.
@api ISteamMatchmaking#GetLobbyGameServer
*/
extern "C" DLL_EXPORT char *GetLobbyGameServerIP(int hLobbySteamID);

/*
@desc Gets the connection port of a game server set in a lobby.
@param hLobbySteamID The Steam ID of the lobby to get the game server information from.
@return The connection port for the lobby game server.
@api ISteamMatchmaking#GetLobbyGameServer
*/
extern "C" DLL_EXPORT int GetLobbyGameServerPort(int hLobbySteamID);

/*
@desc Gets the Steam ID handle of a game server set in a lobby.
@param hLobbySteamID The Steam ID of the lobby to get the game server information from.
@return The Steam ID handle for the lobby game server.
@api ISteamMatchmaking#GetLobbyGameServer
*/
extern "C" DLL_EXPORT int GetLobbyGameServerSteamID(int hLobbySteamID);

/*
@desc Gets the Steam ID handle of the lobby member at the given index.
@param hLobbySteamID This MUST be the same lobby used in the previous call to GetNumLobbyMembers!
@param index An index between 0 and GetNumLobbyMembers.
@return A Steam ID handle.
@api ISteamMatchmaking#GetLobbyMemberByIndex
*/
extern "C" DLL_EXPORT int GetLobbyMemberByIndex(int hLobbySteamID, int index);

/*
@desc Gets per-user metadata from another player in the specified lobby.
@param hLobbySteamID The Steam ID handle of the lobby that the other player is in.
@param hSteamIDUser The Steam ID handle of the player to get the metadata from.
@param key The key to get the value of.
@return The value for the given key or an empty string if the key, member, or lobby doesn't exist.
@api ISteamMatchmaking#GetLobbyMemberData
*/
extern "C" DLL_EXPORT char *GetLobbyMemberData(int hLobbySteamID, int hSteamIDUser, const char *key);

/*
@desc The current limit on the number of users who can join the lobby.
@param hLobbySteamID The Steam ID handle of the lobby to get the member limit of.
@return A positive integer or 0 if no limit.
@api ISteamMatchmaking#GetLobbyMemberLimit
*/
extern "C" DLL_EXPORT int GetLobbyMemberLimit(int hLobbySteamID);

/*
@desc Returns the current lobby owner.
@param hLobbySteamID The Steam ID handle of the lobby to get the owner of.
@return A steam ID handle or 0.
@api ISteamMatchmaking#GetLobbyOwner
*/
extern "C" DLL_EXPORT int GetLobbyOwner(int hLobbySteamID);

/*
@desc Gets the number of users in a lobby.
@param hLobbySteamID The Steam ID handle of the lobby to get the number of members of.
@return The number of members in the lobby, 0 if the current user has no data from the lobby.
@api ISteamMatchmaking#GetNumLobbyMembers
*/
extern "C" DLL_EXPORT int GetNumLobbyMembers(int hLobbySteamID);

/*
@desc Invite another user to the lobby.
@param hLobbySteamID The Steam ID handle of the lobby to invite the user to.
@param hInviteeSteamID The Steam ID handle of the person who will be invited.
@return 1 when the invitation was sent successfully; otherwise 0.
@api ISteamMatchmaking#InviteUserToLobby
*/
extern "C" DLL_EXPORT int InviteUserToLobby(int hLobbySteamID, int hInviteeSteamID);

/*
@desc Joins an existing lobby.
@param hLobbySteamID The Steam ID handle of the lobby
@callback-type callresult
@callback-getters GetJoinLobbyChatRoomEnterResponse, GetJoinLobbyHandle, GetJoinLobbyLocked
@return A [call result handle](Callbacks-and-Call-Results#call-results) on success; otherwise 0.
@api ISteamMatchmaking#JoinLobby
*/
extern "C" DLL_EXPORT int JoinLobby(int hLobbySteamID);

// Unused - always 0.
//extern "C" DLL_EXPORT int GetJoinLobbyChatPermissions(int hCallResult);

/*
@desc Returns the EChatRoomEnterResponse for the JoinLobby call.
@param hCallResult A JoinLobby call result handle.
@return An EChatRoomEnterResponse value.
@return-api steam_api#EChatRoomEnterResponse
@api ISteamMatchmaking#LobbyEnter_t
*/
extern "C" DLL_EXPORT int GetJoinLobbyChatRoomEnterResponse(int hCallResult);

/*
@desc Returns the lobby Steam ID handle for the JoinLobby call.
@param hCallResult A JoinLobby call result handle.
@return The lobby Steam ID handle.
@api ISteamMatchmaking#LobbyEnter_t
*/
extern "C" DLL_EXPORT int GetJoinLobbyHandle(int hCallResult);

/*
@desc Returns whether the lobby is locked for the JoinLobby call.
@param hCallResult A JoinLobby call result handle.
@return 1 if only invited users can join the lobby; otherwise 0.
@api ISteamMatchmaking#LobbyEnter_t
*/
extern "C" DLL_EXPORT int GetJoinLobbyLocked(int hCallResult);

/*
@desc Leave a lobby that the user is currently in; this will take effect immediately on the client side, other users in the lobby will be notified by a LobbyChatUpdate_t callback.
@param hLobbySteamID The Steam ID handle of the lobby to leave.
@api ISteamMatchmaking#LeaveLobby
*/
extern "C" DLL_EXPORT void LeaveLobby(int hLobbySteamID);

/*
@desc Removes the game server from the local favorites list.
@param appID The App ID of the game.
@param ip The IP address of the server.
@param connectPort The port used to connect to the server, in host order.
@param queryPort The port used to query the server, in host order.
@param flags Whether the server is on the favorites list or history list. See k_unFavoriteFlagNone for more information.
@param-api flags ISteamMatchmaking#k_unFavoriteFlagNone
@return 1 if the server was removed; otherwise, 0 if the specified server was not on the users local favorites list.
@api ISteamMatchmaking#RemoveFavoriteGame, ISteamMatchmaking#k_unFavoriteFlagNone, ISteamMatchmaking#k_unFavoriteFlagFavorite, ISteamMatchmaking#k_unFavoriteFlagHistory
*/
extern "C" DLL_EXPORT int RemoveFavoriteGame(int appID, const char *ip, int connectPort, int queryPort, int flags);

/*
@desc Refreshes all of the metadata for a lobby that you're not in right now.
@param hLobbySteamID The Steam ID of the lobby to refresh the metadata of.
@return 1 when the request for lobby data succeeds will be reported by the LobbyDataUpdate_t callback. 0 when the request fails.
@api ISteamMatchmaking#RequestLobbyData, ISteamMatchmaking#LobbyDataUpdate_t
*/
extern "C" DLL_EXPORT int RequestLobbyData(int hLobbySteamID);

/*
@desc Request a filtered list of relevant lobbies.
@callback-type callresult
@callback-getters GetRequestLobbyListCount, GetRequestLobbyListHandle
@return A [call result handle](Callbacks-and-Call-Results#call-results) on success; otherwise 0.
@api ISteamMatchmaking#RequestLobbyList
*/
extern "C" DLL_EXPORT int RequestLobbyList();

/*
@desc Gets the number of lobbies returned by the RequestLobbyList call.
@param hCallResult A RequestLobbyList call result handle.
@return The list count.
@api ISteamMatchmaking#LobbyMatchList_t
*/
extern "C" DLL_EXPORT int GetRequestLobbyListCount(int hCallResult);

/*
@desc Gets the lobby Steam ID handle returned by the RequestLobbyList call by index.
@param hCallResult A RequestLobbyList call result handle.
@param index The lobby index.
@return A Steam ID handle.
@api ISteamMatchmaking#LobbyMatchList_t
*/
extern "C" DLL_EXPORT int GetRequestLobbyListHandle(int hCallResult, int index);

/*
@desc Broadcasts a chat message to the all of the users in the lobby.
@param hLobbySteamID The Steam ID handle of the lobby to send the chat message to.
@param memblockID A memblock containing the message to send.
@return 1 when the send succeeds; otherwise 0.
@api ISteamMatchmaking#SendLobbyChatMessage
@plugin-name SendLobbyChatMessage
*/
extern "C" DLL_EXPORT int SendLobbyChatMessageMemblock(int hLobbySteamID, int memblockID);

/*
@desc Broadcasts a chat message to the all of the users in the lobby.
@param hLobbySteamID The Steam ID handle of the lobby to send the chat message to.
@param message The message to send.
@return 1 when the send succeeds; otherwise 0.
@api ISteamMatchmaking#SendLobbyChatMessage
*/
extern "C" DLL_EXPORT int SendLobbyChatMessage(int hLobbySteamID, const char *message);

//extern "C" DLL_EXPORT int SetLinkedLobby(int hLobbySteamID, int hLobbyDependentSteamID);

/*
@desc Sets a key/value pair in the lobby metadata.

_This can only be set by the owner of the lobby._
@param hLobbySteamID The Steam ID of the lobby to set the metadata for.
@param key The key to set the data for.
@param value The value to set.
@api ISteamMatchmaking#SetLobbyData
*/
extern "C" DLL_EXPORT void SetLobbyData(int hLobbySteamID, const char *key, const char *value);

/*
@desc Sets the game server associated with the lobby.  This method only accepts IPv4 addresses.
Either an IP/port or a Game Server SteamID handle must be given.

_This can only be set by the owner of the lobby._
@param hLobbySteamID The Steam ID handle of the lobby to set the game server information for.
@param gameServerIP Sets the IP address of the game server.
@param gameServerPort Sets the connection port of the game server.
@param hGameServerSteamID Sets the Steam ID handle of the game server.
@return 1 when the call succeeds; otherwise 0.
@api ISteamMatchmaking#SetLobbyGameServer
*/
extern "C" DLL_EXPORT int SetLobbyGameServer(int hLobbySteamID, const char *gameServerIP, int gameServerPort, int hGameServerSteamID);

/*
@desc Sets whether or not a lobby is joinable by other players. This always defaults to enabled for a new lobby.

_This can only be set by the owner of the lobby._
@param hLobbySteamID The Steam ID handle of the lobby
@param lobbyJoinable 1 to allow or 0 to disallow users to join this lobby.
@return 1 when the call succeeds; otherwise 0.
@api ISteamMatchmaking#SetLobbyJoinable
*/
extern "C" DLL_EXPORT int SetLobbyJoinable(int hLobbySteamID, int lobbyJoinable);

/*
@desc Sets per-user metadata for the local user.

Each user in the lobby will be receive notification of the lobby data change via HasLobbyDataUpdated, and any new users joining will receive any existing data.
@param hLobbySteamID The Steam ID of the lobby to set our metadata in.
@param key The key to set the data for.
@param value The value to set.
@api ISteamMatchmaking#SetLobbyMemberData
*/
extern "C" DLL_EXPORT void SetLobbyMemberData(int hLobbySteamID, const char *key, const char *value);

/*
@desc Set the maximum number of players that can join the lobby.

_This can only be set by the owner of the lobby._
@param hLobbySteamID The Steam ID handle of the lobby to set the member limit for.
@param maxMembers The maximum number of players allowed in this lobby. This can not be above 250.
@return 1 if the limit was successfully set; otherwise 0.
@api ISteamMatchmaking#SetLobbyMemberLimit
*/
extern "C" DLL_EXPORT int SetLobbyMemberLimit(int hLobbySteamID, int maxMembers);

/*
@desc Changes who the lobby owner is.
Triggers HasLobbyChatUpdate for all lobby members.

_This can only be set by the owner of the lobby._
@param hLobbySteamID The Steam ID handle of the lobby where the owner change will take place.
@param hNewOwnerSteamID The Steam ID handle of the user that will be the new owner of the lobby, they must be in the lobby.
@return 1 if the owner is successfully changed; otherwise 0.
@api ISteamMatchmaking#SetLobbyOwner
*/
extern "C" DLL_EXPORT int SetLobbyOwner(int hLobbySteamID, int hNewOwnerSteamID);

/*
@desc Updates what type of lobby this is.

_This can only be set by the owner of the lobby._
@param hLobbySteamID The Steam ID handle of the lobby
@param eLobbyType The new lobby type to that will be set.
@param-api eLobbyType ISteamMatchmaking#ELobbyType
@return 1 when the call succeeds; otherwise 0.
@api ISteamMatchmaking#SetLobbyType
*/
extern "C" DLL_EXPORT int SetLobbyType(int hLobbySteamID, int eLobbyType);

// Callbacks
/*
@desc Triggered when a favorites list change message has been received.
@callback-type list
@callback-getters	GetFavoritesListChangedIP, GetFavoritesListChangedQueryPort, GetFavoritesListChangedConnectionPort, GetFavoritesListChangedAppID,
	GetFavoritesListChangedFlags, GetFavoritesListChangedIsAdd, GetFavoritesListChangedAccountID
@return 1 when the callback has more responses to process; otherwise 0.
@api ISteamMatchmaking#FavoritesListAccountsUpdated_t
*/
extern "C" DLL_EXPORT int HasFavoritesListChangedResponse();

/*
@desc Gets the IP of the current FavoritesListAccountsUpdated_t callback reponse.
When an empty string, reload the entire list; otherwise it means just one server.
@return An IP address or empty string.
@api ISteamMatchmaking#FavoritesListAccountsUpdated_t
*/
extern "C" DLL_EXPORT char *GetFavoritesListChangedIP();

/*
@desc Gets the query port of the current FavoritesListAccountsUpdated_t callback reponse.
@return An integer.
@api ISteamMatchmaking#FavoritesListAccountsUpdated_t
*/
extern "C" DLL_EXPORT int GetFavoritesListChangedQueryPort();

/*
@desc Gets the connection port of the current FavoritesListAccountsUpdated_t callback reponse.
@return An integer.
@api ISteamMatchmaking#FavoritesListAccountsUpdated_t
*/
extern "C" DLL_EXPORT int GetFavoritesListChangedConnectionPort();

/*
@desc Gets the App ID of the game server for the current FavoritesListAccountsUpdated_t callback reponse.
@return An integer.
@api ISteamMatchmaking#FavoritesListAccountsUpdated_t
*/
extern "C" DLL_EXPORT int GetFavoritesListChangedAppID();

/*
@desc Gets the query port of the current FavoritesListAccountsUpdated_t callback reponse.
@return Whether the server is on the favorites list or history list. See k_unFavoriteFlagNone for more information.
@api ISteamMatchmaking#FavoritesListAccountsUpdated_t, ISteamMatchmaking#k_unFavoriteFlagNone, ISteamMatchmaking#k_unFavoriteFlagFavorite, ISteamMatchmaking#k_unFavoriteFlagHistory
*/
extern "C" DLL_EXPORT int GetFavoritesListChangedFlags();

/*
@desc Gets whether the game server was added (1) or removed (0) from the list for the current FavoritesListAccountsUpdated_t callback reponse.
@return 1 if added, 0 if removed.
@api ISteamMatchmaking#FavoritesListAccountsUpdated_t
*/
extern "C" DLL_EXPORT int GetFavoritesListChangedIsAdd();

/*
@desc Gets the account ID of the current FavoritesListAccountsUpdated_t callback reponse.
@return An integer.
@api ISteamMatchmaking#FavoritesListAccountsUpdated_t, steam_api#AccountID_t
*/
extern "C" DLL_EXPORT int GetFavoritesListChangedAccountID();

/*
@desc Triggered when a lobby chat message has been received.
@callback-type list
@callback-getters	GetLobbyChatMessageLobby, GetLobbyChatMessageUser, GetLobbyChatMessageText
@return 1 when the callback has more responses to process; otherwise 0.
@api ISteamMatchmaking#LobbyChatMsg_t
*/
extern "C" DLL_EXPORT int HasLobbyChatMessageResponse();

/*
@desc Gets the lobby for the current LobbyChatMsg_t callback response.
@return A lobby Steam ID handle.
@api ISteamMatchmaking#LobbyChatMsg_t
*/
extern "C" DLL_EXPORT int GetLobbyChatMessageLobby();

/*
@desc Gets the Steam ID handle of the user for the current LobbyChatMsg_t callback response.
@return A Steam ID handle.
@api ISteamMatchmaking#GetLobbyChatEntry, ISteamMatchmaking#LobbyChatMsg_t
*/
extern "C" DLL_EXPORT int GetLobbyChatMessageUser();

///*
//@desc Gets the chat entry type for the current LobbyChatMsg_t callback response.
//@return An EChatEntryType value.
//@api ISteamMatchmaking#LobbyChatMsg_t, steam_api#EChatEntryType
//*/
//extern "C" DLL_EXPORT int GetLobbyChatMessageEntryType();

/*
@desc Gets the chat message in a memblock for the current LobbyChatMsg_t callback response.
The memblock will be deleted the next time HasLobbyChatMessageResponse is called.
@return A memblock containing the contents of the chat message.
@api ISteamMatchmaking#GetLobbyChatEntry, ISteamMatchmaking#LobbyChatMsg_t
*/
extern "C" DLL_EXPORT int GetLobbyChatMessageMemblock();

/*
@desc Gets the chat message as text for the current LobbyChatMsg_t callback response.
@return The contents of the chat message.
@api ISteamMatchmaking#GetLobbyChatEntry, ISteamMatchmaking#LobbyChatMsg_t
*/
extern "C" DLL_EXPORT char *GetLobbyChatMessageText();

/*
@desc Triggered when a lobby chat room state has changed, this is usually sent when a user has joined or left the lobby.
@callback-type list
@callback-getters GetLobbyChatUpdateLobby, GetLobbyChatUpdateUserChanged, GetLobbyChatUpdateUserState, GetLobbyChatUpdateUserMakingChange
@return 1 when the callback has more responses to process; otherwise 0.
@api ISteamMatchmaking#LobbyChatUpdate_t
*/
extern "C" DLL_EXPORT int HasLobbyChatUpdateResponse();

/*
@desc The lobby for the current LobbyChatUpdate_t callback response.
@return A Steam ID handle.
@api ISteamMatchmaking#LobbyChatUpdate_t
*/
extern "C" DLL_EXPORT int GetLobbyChatUpdateLobby();

/*
@desc The user whose status in the lobby has changed for the current LobbyChatUpdate_t callback response.
@return A Steam ID handle.
@api ISteamMatchmaking#LobbyChatUpdate_t
*/
extern "C" DLL_EXPORT int GetLobbyChatUpdateUserChanged();

/*
@desc The new user state for the user whose status changed for the current LobbyChatUpdate_t callback response.
@return EChatMemberStateChange bit data
@return-api ISteamMatchmaking#EChatMemberStateChange
@api ISteamMatchmaking#LobbyChatUpdate_t
*/
extern "C" DLL_EXPORT int GetLobbyChatUpdateUserState();

/*
@desc Chat member who made the change for the current LobbyChatUpdate_t callback response.
@return A Steam ID handle.
@api ISteamMatchmaking#LobbyChatUpdate_t
*/
extern "C" DLL_EXPORT int GetLobbyChatUpdateUserMakingChange();

/*
@desc Triggered when the lobby metadata has changed.
@callback-type list
@callback-getters GetLobbyDataUpdateLobby, GetLobbyDataUpdateMember, GetLobbyDataUpdateSuccess
@return 1 when the callback has more responses to process; otherwise 0.
@api ISteamMatchmaking#LobbyDataUpdate_t
*/
extern "C" DLL_EXPORT int HasLobbyDataUpdateResponse();

/*
@desc Returns the lobby whose metadata has changed for the current LobbyDataUpdate_t callback response.
@return A Steam ID handle.
@api ISteamMatchmaking#LobbyDataUpdate_t
*/
extern "C" DLL_EXPORT int GetLobbyDataUpdateLobby();

/*
@desc Returns the handle of the member whose metadata has changed for the current LobbyDataUpdate_t callback response.
If this value is a user in the lobby, then use GetLobbyMemberData to access per-user details; 
otherwise, if GetLobbyDataUpdateMember == GetLobbyDataUpdateLobby, use GetLobbyData to access the lobby metadata.
@return A Steam ID handle.
@api ISteamMatchmaking#LobbyDataUpdate_t
*/
extern "C" DLL_EXPORT int GetLobbyDataUpdateMember();
/*
@desc Returns the success of the lobby data update for the current LobbyDataUpdate_t callback response.
@return 1 if the lobby data was successfully changed, otherwise 0.
@api ISteamMatchmaking#LobbyDataUpdate_t
*/
extern "C" DLL_EXPORT int GetLobbyDataUpdateSuccess();

/*
@desc Recieved upon attempting to enter a lobby. Lobby metadata is available to use immediately after receiving this.
@callback-type list
@callback-getters GetLobbyEnterChatRoomEnterResponse, GetLobbyEnterLobby, GetLobbyEnterLocked
@return 1 when the callback has more responses to process; otherwise 0.
@api ISteamMatchmaking#LobbyEnter_t
*/
extern "C" DLL_EXPORT int HasLobbyEnterResponse();

// Unused - always 0.
//extern "C" DLL_EXPORT int GetLobbyEnterChatPermissions();

/*
@desc Gets EChatRoomEnterResponse for the current LobbyEnter_t callback response.
@return An EChatRoomEnterResponse value.
@return-api steam_api#EChatRoomEnterResponse
@api ISteamMatchmaking#LobbyEnter_t
*/
extern "C" DLL_EXPORT int GetLobbyEnterChatRoomEnterResponse();

/*
@desc Returns the lobby for the current LobbyEnter_t callback response.
@return The lobby Steam ID handle.
@api ISteamMatchmaking#LobbyEnter_t
*/
extern "C" DLL_EXPORT int GetLobbyEnterLobby();

/*
@desc Returns the locked value for the current LobbyEnter_t callback response.
@return 1 if only invited users can join the lobby; otherwise 0.
@api ISteamMatchmaking#LobbyEnter_t
*/
extern "C" DLL_EXPORT int GetLobbyEnterLocked();

/*
@desc Indicates that a lobby game was created since the last call.
_This can only be read once per lobby game creation!_
@callback-type list
@callback-getters GetLobbyGameCreatedGameServer, GetLobbyGameCreatedLobby, GetLobbyGameCreatedIP, GetLobbyGameCreatedPort
@return 1 when a lobby game was created; otherwise 0.
@api ISteamMatchmaking#LobbyGameCreated_t
*/
extern "C" DLL_EXPORT int HasLobbyGameCreatedResponse();

/*
@desc Returns the current game server Steam ID handle for the LobbyGameCreated_t callback.
@return A Steam ID handle.
@api ISteamMatchmaking#LobbyGameCreated_t
*/
extern "C" DLL_EXPORT int GetLobbyGameCreatedGameServer();

/*
@desc Returns the Steam ID handle of the lobby that set the game server for the current LobbyGameCreated_t callback response.
@return A Steam ID handle.
@api ISteamMatchmaking#LobbyGameCreated_t
*/
extern "C" DLL_EXPORT int GetLobbyGameCreatedLobby();

/*
@desc Returns the IP address of the game server for the current LobbyGameCreated_t callback response.
@return A Steam ID handle.
@api ISteamMatchmaking#LobbyGameCreated_t
*/
extern "C" DLL_EXPORT char *GetLobbyGameCreatedIP();

/*
@desc Returns the connection port of the game server for the current LobbyGameCreated_t callback response.
@return A Steam ID handle.
@api ISteamMatchmaking#LobbyGameCreated_t
*/
extern "C" DLL_EXPORT int GetLobbyGameCreatedPort();
#pragma endregion

#endif // _DLLMAIN_H_