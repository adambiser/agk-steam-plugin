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

#pragma region ISteamRemoteStorage
/* @page ISteamRemoteStorage */
//CommitPublishedFileUpdate
//CreatePublishedFileUpdateRequest
//DeletePublishedFile
//EnumeratePublishedFilesByUserAction
//EnumeratePublishedWorkshopFiles
//EnumerateUserPublishedFiles
//EnumerateUserSharedWorkshopFiles
//EnumerateUserSubscribedFiles

/*
@desc
Deletes a file from the local disk, and propagates that delete to the cloud.

This is meant to be used when a user actively deletes a file. Use FileForget if you want to remove a file from the Steam Cloud but retain it on the users local disk.

When a file has been deleted it can be re-written with FileWrite to reupload it to the Steam Cloud.
@param filename The name of the file that will be deleted.
@return 1 if the file exists and has been successfully deleted; otherwise, 0 if the file did not exist.
@api ISteamRemoteStorage#FileDelete
*/
extern "C" DLL_EXPORT int CloudFileDelete(const char *filename);

/*
@desc Checks whether the specified file exists.
@param filename The name of the file.
@return 1 if the file exists; otherwise, 0.
@api ISteamRemoteStorage#FileExists
*/
extern "C" DLL_EXPORT int CloudFileExists(const char *filename);

/*
@desc
Deletes the file from remote storage, but leaves it on the local disk and remains accessible from the API.

When you are out of Cloud space, this can be used to allow calls to FileWrite to keep working without needing to make the user delete files.

How you decide which files to forget are up to you. It could be a simple Least Recently Used (LRU) queue or something more complicated.

Requiring the user to manage their Cloud-ized files for a game, while is possible to do, it is never recommended.
For instance, "Which file would you like to delete so that you may store this new one?" removes a significant advantage of using the Cloud in the first place: its transparency.

Once a file has been deleted or forgotten, calling FileWrite will resynchronize it in the Cloud. Rewriting a forgotten file is the only way to make it persisted again.
@param filename The name of the file that will be forgotten.
@return	1 if the file exists and has been successfully forgotten; otherwise, 0.
@api ISteamRemoteStorage#FileForget
*/
extern "C" DLL_EXPORT int CloudFileForget(const char *filename);

/*
@desc Checks if a specific file is persisted in the steam cloud.
@param filename The name of the file.
@return
1 if the file exists and the file is persisted in the Steam Cloud.
0 if FileForget was called on it and is only available locally.
@api ISteamRemoteStorage#FilePersisted
*/
extern "C" DLL_EXPORT int CloudFilePersisted(const char *filename);

/*
@desc Opens a binary file, reads the contents of the file into a memblock, and then closes the file.

**NOTE:** This is a synchronous call and as such is a will block your calling thread on the disk IO, and will also block the SteamAPI,
which can cause other threads in your application to block. To avoid "hitching" due to a busy disk on the client machine using FileReadAsync,
the asynchronous version of this API is recommended.
@param filename The name of the file to read from.
@return
A memblock ID containing the data read from the file.
Returns 0 if the file doesn't exist or the read fails.
@api ISteamRemoteStorage#FileRead
*/
extern "C" DLL_EXPORT int CloudFileRead(const char *filename);

/*
@desc Starts an asynchronous read from a file.

The offset and amount to read should be valid for the size of the file, as indicated by GetCloudFileSize.
@param filename The name of the file to read from.
@param offset The offset in bytes into the file where the read will start from. 0 if you're reading the whole file in one chunk.
@param length The amount of bytes to read starting from the offset. -1 to read the entire file.
@callback-type callresult
@callback-getters GetCloudFileReadAsyncFileName, GetCloudFileReadAsyncMemblock
@return A [call result handle](Callbacks-and-Call-Results#call-results) on success; otherwise 0.
@api ISteamRemoteStorage#FileReadAsync, ISteamRemoteStorage#RemoteStorageFileReadAsyncComplete_t
*/
extern "C" DLL_EXPORT int CloudFileReadAsync(const char *filename, int offset, int length);

/*
@desc Returns the file name for the CloudFileReadAsync call.
@param hCallResult A CloudFileReadAsync call result handle.
@return The file name.
@api ISteamRemoteStorage#FileReadAsync, ISteamRemoteStorage#RemoteStorageFileReadAsyncComplete_t
*/
extern "C" DLL_EXPORT char *GetCloudFileReadAsyncFileName(int hCallResult);

/*
@desc Returns the memblock of the data returned by the CloudFileReadAsync call.

A call result will delete its memblock in DeleteCallResult() so calling code does not need to do so.
@param hCallResult A CloudFileReadAsync call result handle.
@return A memblock ID.
@api ISteamRemoteStorage#FileReadAsync, ISteamRemoteStorage#RemoteStorageFileReadAsyncComplete_t
*/
extern "C" DLL_EXPORT int GetCloudFileReadAsyncMemblock(int hCallResult);

/*
@desc Shares a file with users and features.
@param filename The file to share.
@callback-type callresult
@callback-getters GetCloudFileShareUGCHandle, GetCloudFileShareFileName
@return A [call result handle](Callbacks-and-Call-Results#call-results) on success; otherwise 0.
@api ISteamRemoteStorage#FileShare, ISteamRemoteStorage#RemoteStorageFileShareResult_t
*/
extern "C" DLL_EXPORT int CloudFileShare(const char *filename);

/*
@desc Returns the UGC handle for the CloudFileShare call.
@param hCallResult A CloudFileShare call result handle.
@return A UGC handle.
@api ISteamRemoteStorage#FileShare, ISteamRemoteStorage#RemoteStorageFileShareResult_t
*/
extern "C" DLL_EXPORT int GetCloudFileShareUGCHandle(int hCallResult);

/*
@desc Returns the file name for the CloudFileShare call.
@param hCallResult A CloudFileShare call result handle.
@return The file name.
@api ISteamRemoteStorage#FileShare, ISteamRemoteStorage#RemoteStorageFileShareResult_t
*/
extern "C" DLL_EXPORT char *GetCloudFileShareFileName(int hCallResult);

/*
@desc
Creates a new file, writes the memblock to the file, and then closes the file. If the target file already exists, it is overwritten.

**NOTE:** This is a synchronous call and as such is a will block your calling thread on the disk IO, and will also block the SteamAPI,
which can cause other threads in your application to block. To avoid "hitching" due to a busy disk on the client machine using FileWriteAsync,
the asynchronous version of this API is recommended.
@param filename The name of the file to write to.
@param memblockID A memblock of data to write to the file.
@return 1 if the write was successful.

Otherwise, 0 under the following conditions:
* The file you're trying to write is larger than 100MiB as defined by k_unMaxCloudFileChunkSize.
* The memblock is empty.
* You tried to write to an invalid path or filename.
* The current user's Steam Cloud storage quota has been exceeded. They may have run out of space, or have too many files.
* Steam could not write to the disk, the location might be read-only.
@api ISteamRemoteStorage#FileWrite
*/
extern "C" DLL_EXPORT int CloudFileWrite(const char *filename, int memblockID);

/*
@desc Creates a new file and asynchronously writes the raw byte data to the Steam Cloud, and then closes the file. If the target file already exists, it is overwritten.

The data in memblock is copied and the memblock can be deleted immediately after calling this method.
@param filename The name of the file to write to.
@param memblockID The memblock containing the data to write to the file.
@callback-type callresult
@callback-getters GetCloudFileWriteAsyncFileName
@return A [call result handle](Callbacks-and-Call-Results#call-results) on success; otherwise 0.
@api ISteamRemoteStorage#FileWriteAsync, ISteamRemoteStorage#RemoteStorageFileWriteAsyncComplete_t
*/
extern "C" DLL_EXPORT int CloudFileWriteAsync(const char *filename, int memblockID);

/*
@desc Returns the file name for the CloudFileWriteAsync call.
@param hCallResult A CloudFileReadAsync [call result handle](Callbacks-and-Call-Results#call-results).
@return The file name.
*/
extern "C" DLL_EXPORT char *GetCloudFileWriteAsyncFileName(int hCallResult);

/*
@desc Cancels a file write stream that was started by FileWriteStreamOpen.

This trashes all of the data written and closes the write stream, but if there was an existing file with this name, it remains untouched.
@param writeHandle The file write stream to cancel.
@return 1 or 0.  Steamworks SDK docs don't explain.
@api ISteamRemoteStorage#FileWriteStreamCancel
*/
extern "C" DLL_EXPORT int CloudFileWriteStreamCancel(int writeHandle);

/*
@desc Closes a file write stream that was started by FileWriteStreamOpen.

This flushes the stream to the disk, overwriting the existing file if there was one.
@param writeHandle The file write stream to close.
@return 1 if the file write stream was successfully closed; otherwise 0.
@api ISteamRemoteStorage#FileWriteStreamClose
*/
extern "C" DLL_EXPORT int CloudFileWriteStreamClose(int writeHandle);

/*
@desc Creates a new file output stream allowing you to stream out data to the Steam Cloud file in chunks.
If the target file already exists, it is not overwritten until FileWriteStreamClose has been called.
@param filename The name of the file to write to.
@return A file write stream handle or -1 if there's a problem.
@api ISteamRemoteStorage#FileWriteStreamOpen
*/
extern "C" DLL_EXPORT int CloudFileWriteStreamOpen(const char *filename);

/*
@desc Writes a blob of data from a memblock to the file write stream.
Data size is restricted to 100 * 1024 * 1024 bytes.
@param writeHandle The file write stream to write to.
@param memblockID A memblock containing the data to write.
@param offset The offset within the memblock of the data to write.
@param length The length of the data to write.
@return 1 if the data was successfully written to the file write stream; otherwise 0;
@api ISteamRemoteStorage#FileWriteStreamWriteChunk
@plugin-name CloudFileWriteStreamWriteChunk
*/
extern "C" DLL_EXPORT int CloudFileWriteStreamWriteChunkEx(int writeHandle, int memblockID, int offset, int length);

/*
@desc Writes the entire contents of a memblock to the file write stream.
Data size is restricted to 100 * 1024 * 1024 bytes.
@param writeHandle The file write stream to write to.
@param memblockID A memblock containing the data to write.
@return 1 if the data was successfully written to the file write stream; otherwise 0;
@api ISteamRemoteStorage#FileWriteStreamWriteChunk
*/
extern "C" DLL_EXPORT int CloudFileWriteStreamWriteChunk(int writeHandle, int memblockID);

extern "C" DLL_EXPORT int GetCloudCachedUGCCount();
extern "C" DLL_EXPORT int GetCloudCachedUGCHandle(int index);

/*
@desc Gets the total number of local files synchronized by Steam Cloud.
@return The number of files present for the current user, including files in subfolders.
@api ISteamRemoteStorage#GetFileCount
*/
extern "C" DLL_EXPORT int GetCloudFileCount();

/*
@desc Gets the file name at the given index
@param index The index of the file, this should be between 0 and GetFileCount.
@return The name of the file at the given index.  An empty string if the file doesn't exist.
@api ISteamRemoteStorage#GetFileCount, ISteamRemoteStorage#GetFileNameAndSize
*/
extern "C" DLL_EXPORT char *GetCloudFileName(int index);

/*
@desc Gets the file size at the given index
@param index The index of the file, this should be between 0 and GetFileCount.
@return The name of the file at the given index.  An empty string if the file doesn't exist.
@api ISteamRemoteStorage#GetFileCount, ISteamRemoteStorage#GetFileNameAndSize
@plugin-name GetCloudFileSize
*/
extern "C" DLL_EXPORT int GetCloudFileSizeByIndex(int index);

/*
@desc Gets the specified file's size in bytes.
@param filename 	The name of the file.
@return The size of the file in bytes. Returns 0 if the file does not exist.
@api ISteamRemoteStorage#GetFileSize
*/
extern "C" DLL_EXPORT int GetCloudFileSize(const char *filename);

/*
@desc Gets the specified file's last modified timestamp in Unix epoch format (seconds since Jan 1st 1970).
@param filename The name of the file.
@return The last modified timestamp in Unix epoch format.
@api ISteamRemoteStorage#GetFileTimestamp
*/
extern "C" DLL_EXPORT int GetCloudFileTimestamp(const char *filename);

/*
@desc Gets the number of bytes available in the user's Steam Cloud storage.
@return An integer.
@api ISteamRemoteStorage#GetQuota
*/
extern "C" DLL_EXPORT int GetCloudQuotaAvailable();

/*
@desc Gets the total amount of bytes the user has access to in the user's Steam Cloud storage.
@return An integer.
@api ISteamRemoteStorage#GetQuota
*/
extern "C" DLL_EXPORT int GetCloudQuotaTotal();

/*
@desc Obtains the platforms that the specified file will syncronize to.
@param filename The name of the file.
@return Bitfield containing the platforms that the file was set to with SetSyncPlatforms.
@api ISteamRemoteStorage#GetSyncPlatforms, ISteamRemoteStorage#ERemoteStoragePlatform
*/
extern "C" DLL_EXPORT int GetCloudFileSyncPlatforms(const char *filename);

extern "C" DLL_EXPORT int GetCloudUGCDetailsAppID(int hUGC);
extern "C" DLL_EXPORT char *GetCloudUGCDetailsFileName(int hUGC);
extern "C" DLL_EXPORT int GetCloudUGCDetailsFileSize(int hUGC);
extern "C" DLL_EXPORT int GetCloudUGCDetailsOwner(int hUGC);
extern "C" DLL_EXPORT int GetCloudUGCDownloadProgressBytesDownloaded(int hUGC);
extern "C" DLL_EXPORT int GetCloudUGCDownloadProgressBytesExpected(int hUGC);

/*
@desc
Checks if the account wide Steam Cloud setting is enabled for this user; or if they disabled it in the Settings->Cloud dialog.

Ensure that you are also checking IsCloudEnabledForApp, as these two options are mutually exclusive.
@return
1 if Steam Cloud is enabled for this account; otherwise, 0.
@api
ISteamRemoteStorage#IsCloudEnabledForAccount

https://partner.steamgames.com/doc/api/ISteamRemoteStorage#IsCloudEnabledForAccount
*/
extern "C" DLL_EXPORT int IsCloudEnabledForAccount();

/*
@desc
Checks if the per game Steam Cloud setting is enabled for this user; or if they disabled it in the Game Properties->Update dialog.

Ensure that you are also checking IsCloudEnabledForAccount, as these two options are mutually exclusive.

It's generally recommended that you allow the user to toggle this setting within your in-game options, you can toggle it with SetCloudEnabledForApp.
@return
1 if Steam Cloud is enabled for this app; otherwise, 0.
@api
ISteamRemoteStorage#IsCloudEnabledForApp
*/
extern "C" DLL_EXPORT int IsCloudEnabledForApp();

/*
@desc
Toggles whether the Steam Cloud is enabled for your application.

This setting can be queried with IsCloudEnabledForApp.

**NOTE:** This must only ever be called as the direct result of the user explicitly requesting that it's enabled or not.
This is typically accomplished with a checkbox within your in-game options.
@param enabled 1 to enabled or 0 to disable the Steam Cloud for this application.
@api
ISteamRemoteStorage#SetCloudEnabledForApp
*/
extern "C" DLL_EXPORT void SetCloudEnabledForApp(int enabled);

/*
@desc
Allows you to specify which operating systems a file will be synchronized to.

Use this if you have a multiplatform game but have data which is incompatible between platforms.

Files default to k_ERemoteStoragePlatformAll when they are first created. You can use the bitwise OR operator to specify multiple platforms.
@param filename The name of the file.
@param eRemoteStoragePlatform The platforms that the file will be syncronized to.
@param-api eRemoteStoragePlatform ISteamRemoteStorage#ERemoteStoragePlatform
@return 1 if the file exists, otherwise 0.
@api ISteamRemoteStorage#SetSyncPlatforms
*/
extern "C" DLL_EXPORT int SetCloudFileSyncPlatforms(const char *filename, int eRemoteStoragePlatform);

extern "C" DLL_EXPORT int CloudUGCDownload(int hUGC, int priority);
extern "C" DLL_EXPORT int CloudUGCDownloadToLocation(int hUGC, const char *location, int priority);
/*
@plugin-name CloudUGCRead
*/
extern "C" DLL_EXPORT int CloudUGCReadEx(int hUGC, int memblockID, int dstOffset, int length, int srcOffset, int eAction);
extern "C" DLL_EXPORT int CloudUGCRead(int hUGC, int memblockID, int srcOffset, int eAction);
#pragma endregion

#pragma region ISteamUserStats
/* @page ISteamUserStats */
/*
@desc Attaches a piece of user generated content the current user's entry on a leaderboard.

Use GetCallResultCode to determine whether this call was successful.
@param hLeaderboard A leaderboard handle.
@param hUGC Handle to a piece of user generated content that was shared using CloudFileShare or CreateUGCItem.
@callback-type callresult
@return A [call result handle](Callbacks-and-Call-Results#call-results) on success; otherwise 0.
@api ISteamUserStats#AttachLeaderboardUGC
*/
extern "C" DLL_EXPORT int AttachLeaderboardUGC(int hLeaderboard, int hUGC);

/*
@desc Removes an achievement from the user.
Call StoreStats afterward to upload the stats to the server.

_This method is generally just for testing purposes._
@param name The 'API Name' of the achievement.
@return int: 1 when the call succeeds; otherwise 0.
@api ISteamUserStats#ClearAchievement
*/
extern "C" DLL_EXPORT int ClearAchievement(const char *name);

/*
@desc Downloads entries from a leaderboard.
@param hLeaderboard A leaderboard handle.
@param eLeaderboardDataRequest The type of data request to make.
@param-api eLeaderboardDataRequest ISteamUserStats#ELeaderboardDataRequest
@param rangeStart The index to start downloading entries relative to eLeaderboardDataRequest.
@param rangeEnd The last index to retrieve entries for relative to eLeaderboardDataRequest.
@callback-type callresult
@callback-getters GetDownloadLeaderboardHandle, GetDownloadLeaderboardEntryCount, GetDownloadLeaderboardEntryUser, GetDownloadLeaderboardEntryGlobalRank, GetDownloadLeaderboardEntryScore
@return A [call result handle](Callbacks-and-Call-Results#call-results) on success; otherwise 0.
@api ISteamUserStats#DownloadLeaderboardEntries
*/
extern "C" DLL_EXPORT int DownloadLeaderboardEntries(int hLeaderboard, int eLeaderboardDataRequest, int rangeStart, int rangeEnd);

//DownloadLeaderboardEntriesForUsers

/*
@desc Returns the handle to the leaderboard for the DownloadLeaderboardEntries call.
@param hCallResult A DownloadLeaderboardEntries call result handle.
@return A Steam ID handle.
@api ISteamUserStats#LeaderboardScoresDownloaded_t
*/
extern "C" DLL_EXPORT int GetDownloadLeaderboardHandle(int hCallResult);

/*
@desc Returns the entry count for the DownloadLeaderboardEntries call.
@param hCallResult A DownloadLeaderboardEntries call result handle.
@return The number of entries.
@api ISteamUserStats#LeaderboardScoresDownloaded_t
*/
extern "C" DLL_EXPORT int GetDownloadLeaderboardEntryCount(int hCallResult);

/*
@desc Returns the handle of the user of an entry for the DownloadLeaderboardEntries call.
@param hCallResult A DownloadLeaderboardEntries call result handle.
@param index The entry index.
@return A Steam ID handle.
@api ISteamUserStats#LeaderboardScoresDownloaded_t
*/
extern "C" DLL_EXPORT int GetDownloadLeaderboardEntryUser(int hCallResult, int index);

/*
@desc Returns the global rank of an entry for the DownloadLeaderboardEntries call.
@param hCallResult A DownloadLeaderboardEntries call result handle.
@param index The entry index.
@return A Steam ID handle.
@api ISteamUserStats#LeaderboardScoresDownloaded_t
*/
extern "C" DLL_EXPORT int GetDownloadLeaderboardEntryGlobalRank(int hCallResult, int index);

/*
@desc Returns the score of an entry for the DownloadLeaderboardEntries call.
@param hCallResult A DownloadLeaderboardEntries call result handle.
@param index The entry index.
@return An integer
@api ISteamUserStats#LeaderboardScoresDownloaded_t
*/
extern "C" DLL_EXPORT int GetDownloadLeaderboardEntryScore(int hCallResult, int index);

/*
@desc Returns a JSON array of integers for the details of an entry for the DownloadLeaderboardEntries call.
@param hCallResult A DownloadLeaderboardEntries call result handle.
@param index The entry index.
@return A JSON array of integers.
@api ISteamUserStats#LeaderboardScoresDownloaded_t
*/
extern "C" DLL_EXPORT char *GetDownloadLeaderboardEntryDetails(int hCallResult, int index);

/*
@desc Returns the UGC handle for the details of an entry for the DownloadLeaderboardEntries call.
@param hCallResult A DownloadLeaderboardEntries call result handle.
@param index The entry index.
@return A UGC handle or 0.
@api ISteamUserStats#LeaderboardScoresDownloaded_t
*/
extern "C" DLL_EXPORT int GetDownloadLeaderboardEntryUGC(int hCallResult, int index);
/*
@desc Sends a request to find the handle for a leaderboard.
@param leaderboardName The name of the leaderboard to find.
@callback-type callresult
@callback-getters GetFindLeaderboardFound, GetFindLeaderboardHandle
@return A [call result handle](Callbacks-and-Call-Results#call-results) on success; otherwise 0.
@api ISteamUserStats#FindLeaderboard
*/
extern "C" DLL_EXPORT int FindLeaderboard(const char *leaderboardName);

/*
@desc Gets a leaderboard by name, it will create it if it's not yet created.
Leaderboards created with this function will not automatically show up in the Steam Community.
You must manually set the Community Name field in the App Admin panel of the Steamworks website.

_Note: If either eLeaderboardSortMethod OR eLeaderboardDisplayType are 0 (none), an error is raised.
@param leaderboardName The name of the leaderboard to find.
@param eLeaderboardSortMethod The sort order of the new leaderboard if it's created.
@param-api eLeaderboardSortMethod ISteamUserStats#ELeaderboardSortMethod
@param eLeaderboardDisplayType The display type (used by the Steam Community web site) of the new leaderboard if it's created.
@param-api eLeaderboardDisplayType ISteamUserStats#ELeaderboardDisplayType
@callback-type callresult
@callback-getters GetFindLeaderboardFound, GetFindLeaderboardHandle
@return A [call result handle](Callbacks-and-Call-Results#call-results) on success; otherwise 0.
@api ISteamUserStats#FindOrCreateLeaderboard
*/
extern "C" DLL_EXPORT int FindOrCreateLeaderboard(const char *leaderboardName, int eLeaderboardSortMethod, int eLeaderboardDisplayType);

/*
@desc Returns whether the leaderboard was found for the FindLeaderboard call.
@param hCallResult A FindLeaderboard call result handle.
@return 1 if found; otherwise 0.
@api ISteamUserStats#LeaderboardFindResult_t
*/
extern "C" DLL_EXPORT int GetFindLeaderboardFound(int hCallResult);

/*
@desc Returns the found leaderboard for the FindLeaderboard call.
@param hCallResult A FindLeaderboard call result handle.
@return A Steam ID handle.
@api ISteamUserStats#LeaderboardFindResult_t
*/
extern "C" DLL_EXPORT int GetFindLeaderboardHandle(int hCallResult);

/*
@desc Gets whether the user has achieved this achievement.
@param name The 'API Name' of the achievement.
@return 1 when the user has achieved this achievement; otherwise 0.
@api ISteamUserStats#GetAchievement
*/
extern "C" DLL_EXPORT int GetAchievement(const char *name);

/*
@desc Returns the percentage of users who have unlocked the specified achievement.

You must have called RequestGlobalAchievementPercentages and it needs to return successfully via its callback prior to calling this.

_If RequestGlobalAchievementPercentages has not been called or if the specified 'API Name' does not exist in the global achievement percentages, an error will be raised._
@param name The 'API Name' of the achievement.
@return The percentage of people that have unlocked this achievement from 0 to 100 or -1 if there is an error.
@api ISteamUserStats#GetAchievementAchievedPercent
*/
extern "C" DLL_EXPORT float GetAchievementAchievedPercent(const char *name);

/*
@desc Gets the time at which an achievement was unlocked, if ever.
@param name The 'API Name' of the achievement.
@return The unload time in Unix time.
@api ISteamUserStats#GetAchievementAndUnlockTime
*/
extern "C" DLL_EXPORT int GetAchievementUnlockTime(const char *name);

/*
@desc Gets the localized achievement name.
@param name The 'API Name' of the achievement.
@return The localized achievement name.
@api ISteamUserStats#GetAchievementDisplayAttribute
*/
extern "C" DLL_EXPORT char *GetAchievementDisplayName(const char *name);

/*
@desc Gets the localized achievement description.
@param name The 'API Name' of the achievement.
@return The localized achievement description.
@api ISteamUserStats#GetAchievementDisplayAttribute
*/
extern "C" DLL_EXPORT char *GetAchievementDisplayDesc(const char *name);

/*
@desc Gets whether an achievement is hidden.
@param name The 'API Name' of the achievement.
@return 1 when the achievement is hidden, 0 when it is not hidden.
@api ISteamUserStats#GetAchievementDisplayAttribute
*/
extern "C" DLL_EXPORT int GetAchievementDisplayHidden(const char *name);

/*
@desc Gets the achievement icon for the current user's current achievement state.
@param name The 'API Name' of the achievement.
@return 0 when no icon is set, -1 when the icon needs to download via callback, or an image handle. [Similar to how avatars work.](Avatars)
@api ISteamUserStats#GetAchievementIcon
*/
extern "C" DLL_EXPORT int GetAchievementIcon(const char *name);
/*
@desc Gets the achievement ID for the achievement index.

_This method is generally just for testing purposes since the app should already know the achievement IDs._
@param index Index of the achievement.
@return Gets the 'API name' for an achievement index between 0 and GetNumAchievements.
@api ISteamUserStats#GetAchievementName
*/
extern "C" DLL_EXPORT char *GetAchievementAPIName(int index);

/*
@desc Gets the lifetime totals for an aggregated integer stat.

You must have called RequestGlobalStats and it needs to return successfully via its callback prior to calling this.

_Note: Since AppGameKit is limited to 32-bit integers, this value is returned as a string._

_If there is a problem getting the value, an error is raised._
@param name The 'API Name' of the stat.
@return The lifetime totals for a stat.
@api ISteamUserStats#GetGlobalStat
*/
extern "C" DLL_EXPORT char *GetGlobalStatInt64AsString(const char *name);

/*
@desc Gets the lifetime totals for an aggregated float stat.

You must have called RequestGlobalStats and it needs to return successfully via its callback prior to calling this.

_Note: Since AppGameKit is limited to floats, this value is returned as a string._

_If there is a problem getting the value, an error is raised._
@param name The 'API Name' of the stat.
@param precision The precision of the returned string.
@return The lifetime totals for a stat.
@api ISteamUserStats#GetGlobalStat
*/
extern "C" DLL_EXPORT char *GetGlobalStatDoubleAsString(const char *name, int precision);

/*
@desc Gets the daily history for an aggregated int stat.

Returned as a JSON array of strings, starting with today's value.
@param name The 'API Name' of the stat.
@return A JSON array of int64 values as strings.
@api ISteamUserStats#GetGlobalStatHistory
*/
extern "C" DLL_EXPORT char *GetGlobalStatHistoryInt64JSON(const char *name);

/*
@desc Gets the daily history for an aggregated floar stat.

Returned as a JSON array of strings, starting with today's value.
@param name The 'API Name' of the stat.
@param precision The precision of the returned string.
@return A JSON array of double values as strings.
@api ISteamUserStats#GetGlobalStatHistory
*/
extern "C" DLL_EXPORT char *GetGlobalStatHistoryDoubleJSON(const char *name, int precision);

/*
@desc Returns the display type of a leaderboard.
@param hLeaderboard A leaderboard handle.
@return An ELeaderboardDisplayType value.
@api ISteamUserStats#GetLeaderboardDisplayType, ISteamUserStats#ELeaderboardDisplayType
*/
extern "C" DLL_EXPORT int GetLeaderboardDisplayType(int hLeaderboard);

/*
@desc Returns the total number of entries in a leaderboard.
@param hLeaderboard A leaderboard handle.
@return The number of entries in the leaderboard.
@api ISteamUserStats#GetLeaderboardEntryCount
*/
extern "C" DLL_EXPORT int GetLeaderboardEntryCount(int hLeaderboard);

/*
@desc Returns the leaderboard name.
@param hLeaderboard A leaderboard handle.
@return The leaderboard name.
@api ISteamUserStats#GetLeaderboardName
*/
extern "C" DLL_EXPORT char *GetLeaderboardName(int hLeaderboard);

/*
@desc Returns the sort order of a leaderboard.
@param hLeaderboard A leaderboard handle.
@return ELeaderboardSortMethod value.
@api ISteamUserStats#GetLeaderboardSortMethod, ISteamUserStats#ELeaderboardSortMethod
*/
extern "C" DLL_EXPORT int GetLeaderboardSortMethod(int hLeaderboard);

/*
@desc Gets the info on the most achieved achievement for the game.
Use GetMostAchievedAchievementInfoName, GetMostAchievedAchievementInfoPercent, and GetMostAchievedAchievementInfoAchieved to get this info.

You must have called RequestGlobalAchievementPercentages and it needs to return successfully via its callback prior to calling this.

_Note: The plugin keeps track of the iterator mentioned in the Steamworks documentation.__

Usage:
```
if Steam.GetMostAchievedAchievementInfo()
	repeat
		Log("Name: " + Steam.GetMostAchievedAchievementInfoName() + ", percent: " + str(Steam.GetMostAchievedAchievementInfoPercent()) + ", unlocked: " + str(Steam.GetMostAchievedAchievementInfoUnlocked()))
	until not Steam.GetNextMostAchievedAchievementInfo()
endif
```
@return 1 If achievement info was loaded; otherwise 0.
@api ISteamUserStats#GetMostAchievedAchievementInfo
*/
extern "C" DLL_EXPORT int GetMostAchievedAchievementInfo();

/*
@desc Gets the info on the next most achieved achievement for the game.
Use GetMostAchievedAchievementInfoName, GetMostAchievedAchievementInfoPercent, and GetMostAchievedAchievementInfoAchieved to get this info.

You must have called RequestGlobalAchievementPercentages and it needs to return successfully via its callback prior to calling this.

_Note: The plugin keeps track of the iterator mentioned in the Steamworks documentation.__

Usage:
```
if Steam.GetMostAchievedAchievementInfo()
repeat
Log("Name: " + Steam.GetMostAchievedAchievementInfoName() + ", percent: " + str(Steam.GetMostAchievedAchievementInfoPercent()) + ", unlocked: " + str(Steam.GetMostAchievedAchievementInfoUnlocked()))
until not Steam.GetNextMostAchievedAchievementInfo()
endif
```
@return 1 If achievement info was loaded; otherwise 0.
@api ISteamUserStats#GetNextMostAchievedAchievementInfo
*/
extern "C" DLL_EXPORT int GetNextMostAchievedAchievementInfo();

/*
@desc Returns the 'API Name' of the achievement as loaded by GetMostAchievedAchievementInfo or GetNextMostAchievedAchievementInfo.
@return 'API Name' of an achievement or an empty string.
@api ISteamUserStats#GetMostAchievedAchievementInfo, ISteamUserStats#GetNextMostAchievedAchievementInfo
*/
extern "C" DLL_EXPORT char *GetMostAchievedAchievementInfoName();

/*
@desc Returns the percentage of people that have unlocked this achievement from 0 to 100 as loaded by GetMostAchievedAchievementInfo or GetNextMostAchievedAchievementInfo.
@return A float.
@api ISteamUserStats#GetMostAchievedAchievementInfo, ISteamUserStats#GetNextMostAchievedAchievementInfo
*/
extern "C" DLL_EXPORT float GetMostAchievedAchievementInfoPercent();

/*
@desc Returns an integer indicating whether the current user has unlocked this achievement as loaded by GetMostAchievedAchievementInfo or GetNextMostAchievedAchievementInfo.
@return 1 if the current user has unlocked this achievement; otherwise 0;
@api ISteamUserStats#GetMostAchievedAchievementInfo, ISteamUserStats#GetNextMostAchievedAchievementInfo
*/
extern "C" DLL_EXPORT int GetMostAchievedAchievementInfoUnlocked();

/*
@desc Gets the number of achievements for the app.

_This method is generally just for testing purposes since the app should already know what the achievements are._
@return The number of achievements.
@api ISteamUserStats#GetNumAchievements
*/
extern "C" DLL_EXPORT int GetNumAchievements();

/*
@desc Asynchronously retrieves the total number of players currently playing the current game. Both online and in offline mode.
@callback-type callresult
@callback-getters GetNumberOfCurrentPlayersResult
@return A [call result handle](Callbacks-and-Call-Results#call-results) on success; otherwise 0.
@api ISteamUserStats#GetNumberOfCurrentPlayers, ISteamUserStats#NumberOfCurrentPlayers_t
*/
extern "C" DLL_EXPORT int GetNumberOfCurrentPlayers();

/*
@desc Returns the number of current players returned by the GetNumberOfCurrentPlayers call.
@param hCallResult A GetNumberOfCurrentPlayers call result.
@return An integer.
@api ISteamUserStats#GetNumberOfCurrentPlayers, ISteamUserStats#NumberOfCurrentPlayers_t
*/
extern "C" DLL_EXPORT int GetNumberOfCurrentPlayersResult(int hCallResult);

/*
@desc Gets the current value of an integer stat for the current user.
If the stat is not defined as an integer, an error will be raised.
@param name The 'API Name' of the stat.
@return The value of the stat.
@api ISteamUserStats#GetStat
*/
extern "C" DLL_EXPORT int GetStatInt(const char *name);

/*
@desc Gets the current value of a float stat for the current user.
If the stat is not defined as a float, an error will be raised.
@param name The 'API Name' of the stat.
@return The value of the stat.
@api ISteamUserStats#GetStat
*/
extern "C" DLL_EXPORT float GetStatFloat(const char *name);

//GetTrophySpaceRequiredBeforeInstall - deprecated

/*
@desc Gets whether the specified user has achieved this achievement.
@param hSteamID The Steam ID handle of the user.
@param name The 'API Name' of the achievement.
@return 1 when the user has achieved this achievement; otherwise 0.
@api ISteamUserStats#GetUserAchievement
*/
extern "C" DLL_EXPORT int GetUserAchievement(int hSteamID, const char *name);

/*
@desc Gets the time at which an achievement was unlocked, if ever.
@param hSteamID The Steam ID handle of the user.
@param name The 'API Name' of the achievement.
@return The unload time in Unix time.
@api ISteamUserStats#GetUserAchievementAndUnlockTime
*/
extern "C" DLL_EXPORT int GetUserAchievementUnlockTime(int hSteamID, const char *name);

/*
@desc Gets the current value of an integer stat for the specified user.
If the stat is not defined as an integer, an error will be raised.
@param hSteamID The Steam ID handle of the user.
@param name The 'API Name' of the stat.
@return The value of the stat.
@api ISteamUserStats#GetUserStat
*/
extern "C" DLL_EXPORT int GetUserStatInt(int hSteamID, const char *name);

/*
@desc Gets the current value of a float stat for the specified user.
If the stat is not defined as a float, an error will be raised.
@param hSteamID The Steam ID handle of the user.
@param name The 'API Name' of the stat.
@return The value of the stat.
@api ISteamUserStats#GetUserStat
*/
extern "C" DLL_EXPORT float GetUserStatFloat(int hSteamID, const char *name);

//GetUserStatsData - deprecated

/*
@desc Raises a notification about achievemnt progress for progress stat achievements.
The notification only shows when current progress is less than the max.

SetStat still needs to be used to set the progress stat value.

Steamworks documentation suggests only using this at intervals in the progression rather than every step of the way.
ie: Every 25 wins out of 100.
@param name The 'API Name' of the achievement.
@param curProgress The current progress.
@param maxProgress The progress required to unlock the achievement.
@callbacks HasUserAchievementStoredResponse
@return 1 when the call succeeds; otherwise 0.
@api ISteamUserStats#IndicateAchievementProgress
*/
extern "C" DLL_EXPORT int IndicateAchievementProgress(const char *name, int curProgress, int maxProgress);

//InstallPS3Trophies

/*
@desc _[This command is initiates a callback.](Callbacks-and-Call-Results#callbacks)_

Sends a request for current user's stats to Steam.

_This command is called within Init so AppGameKit code will likely never need to call this command explicitly._
@return 1 when sending the request succeeds; otherwise 0.  This is not an indication of whether user stats are initialized.  See StatsInitialized.
@api ISteamUserStats#RequestCurrentStats
*/
extern "C" DLL_EXPORT int RequestCurrentStats();

/*
@desc Asynchronously fetch the data for the percentage of players who have received each achievement for the current game globally.
@callback-type callresult
@callback-getters GetCallResultCode
@return A [call result handle](Callbacks-and-Call-Results#call-results) on success; otherwise 0.
@api ISteamUserStats#RequestGlobalAchievementPercentages, ISteamUserStats#GlobalAchievementPercentagesReady_t
*/
extern "C" DLL_EXPORT int RequestGlobalAchievementPercentages();

/*
@desc Asynchronously fetches global stats data, which is available for stats marked as "aggregated" in the App Admin panel of the Steamworks website.
@param historyDays How many days of day-by-day history to retrieve in addition to the overall totals. The limit is 60.
@callback-type callresult
@callback-getters GetCallResultCode
@return A [call result handle](Callbacks-and-Call-Results#call-results) on success; otherwise 0.
@api ISteamUserStats#RequestGlobalStats, ISteamUserStats#GlobalStatsReceived_t
*/
extern "C" DLL_EXPORT int RequestGlobalStats(int historyDays);

/*
@desc _[This command is initiates a callback.](Callbacks-and-Call-Results#callbacks)_

Sends a request for another user's stats to Steam.
@param hSteamID The Steam ID handle of the user to load stats for.
@return 1 when sending the request succeeds; otherwise 0.  This is not an indication of whether user stats are initialized.  See StatsInitialized.
@api ISteamUserStats#RequestCurrentStats
*/
extern "C" DLL_EXPORT int RequestUserStats(int hSteamID);

/*
@desc _[This command is initiates a callback.](Callbacks-and-Call-Results#callbacks)_

Resets user stats and optionally all achievements (when bAchievementsToo is 1).  This command also triggers the StoreStats callback.
@param achievementsToo When 1 then achievements are also cleared.
@return 1 when sending the request to clear user stats succeeds; otherwise 0.
@api ISteamUserStats#ResetAllStats
*/
extern "C" DLL_EXPORT int ResetAllStats(int achievementsToo);

/*
@desc Gives an achievement to the user.

Call StoreStats afterward to notify the user of the achievement.  Otherwise, they will be notified after the game exits.
@param name The 'API Name' of the achievement.
@return 1 when the call succeeds; otherwise 0.
@api ISteamUserStats#SetAchievement
*/
extern "C" DLL_EXPORT int SetAchievement(const char *name);

/*
@desc Sets the value of an integer stat for the current user.
@param name The 'API Name' of the stat.
@param value The new value of the stat. This must be an absolute value, it will not increment or decrement for you.
@return 1 if setting the value succeeds; otherwise 0.
@api ISteamUserStats#SetStat
*/
extern "C" DLL_EXPORT int SetStatInt(const char *name, int value);

/*
@desc Sets the value of a float stat for the current user.
@param name The 'API Name' of the stat.
@param value The new value of the stat. This must be an absolute value, it will not increment or decrement for you.
@return 1 if setting the value succeeds; otherwise 0.
@api ISteamUserStats#SetStat
*/
extern "C" DLL_EXPORT int SetStatFloat(const char *name, float value);

//SetUserStatsData

/*
@desc Stores user stats online.
@callbacks HasUserAchievementStoredResponse, HasUserStatsReceivedResponse
@return 1 when sending the request to store user stats succeeds; otherwise 0.
@api ISteamUserStats#StoreStats
*/
extern "C" DLL_EXPORT int StoreStats();
/*
@desc Returns whether stats are currently being stored.
@return 1 while stats are being stored; otherwise 0.
@api ISteamUserStats#StoreStats
*/
extern "C" DLL_EXPORT int IsStoringStats();

/*
@desc Updates an AVGRATE stat with new values.  The value of these fields can be read with GetStatFloat.
@param name The 'API Name' of the stat.
@param countThisSession The value accumulation since the last call to this method.
@param sessionLength The amount of time in seconds since the last call to this method.
@return 1 if setting the stat succeeds; otherwise 0.
@api ISteamUserStats#UpdateAvgRateStat
*/
extern "C" DLL_EXPORT int UpdateAvgRateStat(const char *name, float countThisSession, float sessionLength);

/*
@desc Adds a detail to be sent in the next UploadLeaderboardScore call.  A maximum of 64 details can be added.
Details are optional.
@param detail The detail element to add.
@return 1 if the detail was successfully added; otherwise 0 meaning that the maximum number of details has been reached.
@api ISteamUserStats#UploadLeaderboardScore
*/
extern "C" DLL_EXPORT int AddUploadLeaderboardScoreDetail(int detail);

/*
@desc Uploads a score to a leaderboard.  The leaderboard will keep the user's best score.
@param hLeaderboard A leaderboard handle.
@param score The score to upload.
@callback-type callresult
@callback-getters GetUploadLeaderboardScoreSuccess, GetUploadLeaderboardScoreHandle, GetUploadLeaderboardScoreValue,
GetUploadLeaderboardScoreChanged, GetUploadLeaderboardScoreRankNew, GetUploadLeaderboardScoreRankPrevious
@return A [call result handle](Callbacks-and-Call-Results#call-results) on success; otherwise 0.
@api ISteamUserStats#UploadLeaderboardScore, ISteamUserStats#ELeaderboardUploadScoreMethod
*/
extern "C" DLL_EXPORT int UploadLeaderboardScore(int hLeaderboard, int score);

/*
@desc Uploads a score to a leaderboard.  Forces the server to accept the uploaded score and replace any existing score.
@param hLeaderboard A leaderboard handle.
@param score The score to upload.
@callback-type callresult
@callback-getters GetUploadLeaderboardScoreSuccess, GetUploadLeaderboardScoreHandle, GetUploadLeaderboardScoreValue,
GetUploadLeaderboardScoreChanged, GetUploadLeaderboardScoreRankNew, GetUploadLeaderboardScoreRankPrevious
@return A [call result handle](Callbacks-and-Call-Results#call-results) on success; otherwise 0.
@api ISteamUserStats#UploadLeaderboardScore, ISteamUserStats#ELeaderboardUploadScoreMethod
*/
extern "C" DLL_EXPORT int UploadLeaderboardScoreForceUpdate(int hLeaderboard, int score);

/*
@desc Returns the success of the UploadLeaderboardScore call.
@param hCallResult A UploadLeaderboardScore call result.
@return 1 on success; otherwise 0.
@api ISteamUserStats#LeaderboardScoreUploaded_t
*/
extern "C" DLL_EXPORT int GetUploadLeaderboardScoreSuccess(int hCallResult);

/*
@desc Returns the leaderboard handle of the UploadLeaderboardScore call.
@param hCallResult A UploadLeaderboardScore call result.
@return A Steam ID handle.
@api ISteamUserStats#LeaderboardScoreUploaded_t
*/
extern "C" DLL_EXPORT int GetUploadLeaderboardScoreHandle(int hCallResult);

/*
@desc Returns the score of the UploadLeaderboardScore call.
@param hCallResult A UploadLeaderboardScore call result.
@return An integer.
@api ISteamUserStats#LeaderboardScoreUploaded_t
*/
extern "C" DLL_EXPORT int GetUploadLeaderboardScoreValue(int hCallResult);

/*
@desc Returns whether the score on the leaderboard changed for the UploadLeaderboardScore call.
@param hCallResult A UploadLeaderboardScore call result.
@return 1 if the score on the leaderboard changed; otherwise 0.
@api ISteamUserStats#LeaderboardScoreUploaded_t
*/
extern "C" DLL_EXPORT int GetUploadLeaderboardScoreChanged(int hCallResult);

/*
@desc Returns the new rank of the user on the leaderboard for the UploadLeaderboardScore call.
@param hCallResult A UploadLeaderboardScore call result.
@return The user's new rank.
@api ISteamUserStats#LeaderboardScoreUploaded_t
*/
extern "C" DLL_EXPORT int GetUploadLeaderboardScoreRankNew(int hCallResult);

/*
@desc Returns the previous rank of the user on the leaderboard of the UploadLeaderboardScore call.
@param hCallResult A UploadLeaderboardScore call result.
@return The user's previous rank or 0 if the user had no previous rank.
@api ISteamUserStats#LeaderboardScoreUploaded_t
*/
extern "C" DLL_EXPORT int GetUploadLeaderboardScoreRankPrevious(int hCallResult);

//Callbacks

//GlobalAchievementPercentagesReady_t - RequestGlobalAchievementPercentages
//GlobalStatsReceived_t - RequestGlobalStats
//LeaderboardFindResult_t - Call result for FindOrCreateLeaderboard, FindLeaderboard
//LeaderboardScoresDownloaded_t - Call result for DownloadLeaderboardEntries, DownloadLeaderboardEntriesForUsers
//LeaderboardScoreUploaded_t - Call result for UploadLeaderboardScore
//LeaderboardUGCSet_t - Call result for AttachLeaderboardUGC
//NumberOfCurrentPlayers_t - GetNumberOfCurrentPlayers
//PS3TrophiesInstalled_t - ignore

/*
@desc Triggered by request to store the achievements on the server, or an "indicate progress" call.
@callback-type list
@callback-getters GetUserAchievementStoredName, GetUserAchievementStoredCurrentProgress, GetUserAchievementStoredMaxProgress
@return 1 when achievements have been stored online; otherwise 0.
@api ISteamUserStats#UserAchievementStored_t
*/
extern "C" DLL_EXPORT int HasUserAchievementStoredResponse();

// Unused
//extern "C" DLL_EXPORT int GetUserAchievementStoredIsGroup();

/*
@desc Returns the name of the achievement for the current UserAchievementStored_t callback response.
@return The achievement name.
@api ISteamUserStats#UserAchievementStored_t
*/
extern "C" DLL_EXPORT char *GetUserAchievementStoredName();

/*
@desc Returns the current progress of the achievement for the current UserAchievementStored_t callback response.
@return An integer.
@api ISteamUserStats#UserAchievementStored_t
*/
extern "C" DLL_EXPORT int GetUserAchievementStoredCurrentProgress();

/*
@desc Returns the maximum progress of the achievement for the current UserAchievementStored_t callback response.
@return An integer.
@api ISteamUserStats#UserAchievementStored_t
*/
extern "C" DLL_EXPORT int GetUserAchievementStoredMaxProgress();

/*
@desc Triggered when the latest stats and achievements for a specific user (including the local user) have been received from the server.
@return 1 when the callback has more responses to process; otherwise 0.
@callback-type list
@callback-getters GetUserStatsReceivedResult, GetUserStatsReceivedUser
@api ISteamUserStats#UserStatsReceived_t
*/
extern "C" DLL_EXPORT int HasUserStatsReceivedResponse();

// This plugin only reports for the current app id.
//extern "C" DLL_EXPORT int GetUserStatsReceivedGameAppID();

/*
@desc Returns whether the call was successful for the current UserStatsReceived_t callback response.
@return 1 when the call is successful; otherwise 0.
@api ISteamUserStats#UserStatsReceived_t
*/
extern "C" DLL_EXPORT int GetUserStatsReceivedResult();

/*
@desc Returns the user whose stats were retrieved for the current UserStatsReceived_t callback response.
@return A Steam ID handle.
@api ISteamUserStats#UserStatsReceived_t
*/
extern "C" DLL_EXPORT int GetUserStatsReceivedUser();

/*
@desc Checks to see whether user stats have been initialized after a RequestCurrentStats call.
@return 1 when users stats are initialized; otherwise 0.
*/
extern "C" DLL_EXPORT int StatsInitialized();

/*
@desc Checks to see whether a user's stats have been initialized after a RequestUserStats call.
@param hSteamID The Steam ID handle of the user to check.
@return 1 when the user's stats are initialized; otherwise 0.
@plugin-name StatsInitialized
*/
extern "C" DLL_EXPORT int StatsInitializedForUser(int hSteamID);

/*
@desc Triggered when the stats and achievements for a user have been unloaded.
@return 1 when the callback has more responses to process; otherwise 0.
@callback-type list
@callback-getters GetUserStatsUnloadedUser
@api ISteamUserStats#UserStatsUnloaded_t
*/
extern "C" DLL_EXPORT int HasUserStatsUnloadedResponse();

/*
@desc Returns the user whose stats were unloaded for the current UserStatsUnloaded_t callback response.
@return A Steam ID handle.
@api ISteamUserStats#UserStatsUnloaded_t
*/
extern "C" DLL_EXPORT int GetUserStatsUnloadedUser();

/*
@desc Triggered by a request to store the user stats.
@callback-type list
@callback-getters GetUserStatsStoredResult
@return 1 when users stats have been stored online; otherwise 0.
@api ISteamUserStats#UserStatsStored_t
*/
extern "C" DLL_EXPORT int HasUserStatsStoredResponse();

/*
@desc Returns whether the call was successful for the current UserStatsStored_t callback response.
@return 1 when the call is successful; otherwise 0.
@api ISteamUserStats#UserStatsStored_t
*/
extern "C" DLL_EXPORT int GetUserStatsStoredResult();
#pragma endregion

#endif // _DLLMAIN_H_