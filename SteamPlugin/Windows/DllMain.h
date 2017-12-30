/*
Copyright (c) 2017 Adam Biser <adambiser@gmail.com>

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

#ifndef DLLMAIN_H_
#define DLLMAIN_H_

#include "..\AGKLibraryCommands.h"

/*
NOTE: Cannot use bool as an exported function return type because of AGK2 limitations.  Use int instead.
*/

// These match ECallbackState in SteamPlugin.h
#define STATE_SERVER_ERROR	-2
#define STATE_CLIENT_ERROR	-1
#define STATE_IDLE			0
#define STATE_RUNNING		1
#define STATE_DONE			2

// General methods.
extern "C" DLL_EXPORT int Init();
extern "C" DLL_EXPORT void Shutdown();
extern "C" DLL_EXPORT int SteamInitialized();
extern "C" DLL_EXPORT int RestartAppIfNecessary(int ownAppID);
extern "C" DLL_EXPORT int GetAppID();
extern "C" DLL_EXPORT char *GetAppName(int appID);
extern "C" DLL_EXPORT int LoggedOn();
extern "C" DLL_EXPORT void RunCallbacks();
// Overlay methods
extern "C" DLL_EXPORT int IsGameOverlayActive();
extern "C" DLL_EXPORT void ActivateGameOverlay(const char *pchDialog);
extern "C" DLL_EXPORT void ActivateGameOverlayInviteDialog(int hLobbySteamID);
extern "C" DLL_EXPORT void ActivateGameOverlayToStore(int appID, int flag);
extern "C" DLL_EXPORT void ActivateGameOverlayToUser(const char *pchDialog, int hSteamID);
extern "C" DLL_EXPORT void ActivateGameOverlayToWebPage(const char *url);
// User/Friend methods
extern "C" DLL_EXPORT char *GetPersonaName();
extern "C" DLL_EXPORT int GetSteamID();
extern "C" DLL_EXPORT char *GetSteamID64(int hUserSteamID);
extern "C" DLL_EXPORT int HasPersonaStateChanged();
extern "C" DLL_EXPORT int GetPersonaStateChangedUser();
extern "C" DLL_EXPORT int GetPersonaStateChangedFlags();
extern "C" DLL_EXPORT int RequestUserInformation(int hUserSteamID, int requireNameOnly);
extern "C" DLL_EXPORT int HasAvatarImageLoaded();
extern "C" DLL_EXPORT int GetAvatarImageLoadedUser();
extern "C" DLL_EXPORT int GetFriendAvatar(int hUserSteamID, int size);
extern "C" DLL_EXPORT int GetFriendCount(int friendFlags);
extern "C" DLL_EXPORT int GetFriendByIndex(int index, int friendFlags);
extern "C" DLL_EXPORT char *GetFriendListJSON(int friendFlags);
extern "C" DLL_EXPORT char *GetFriendGamePlayedJSON(int hUserSteamID);
extern "C" DLL_EXPORT char *GetFriendPersonaName(int hUserSteamID);
extern "C" DLL_EXPORT int GetFriendPersonaState(int hUserSteamID);
extern "C" DLL_EXPORT int GetFriendRelationship(int hUserSteamID);
extern "C" DLL_EXPORT int GetFriendSteamLevel(int hUserSteamID);
extern "C" DLL_EXPORT char *GetPlayerNickname(int hUserSteamID);
extern "C" DLL_EXPORT int HasFriend(int hUserSteamID, int iFriendFlags);
// Friends group methods
extern "C" DLL_EXPORT int GetFriendsGroupCount();
extern "C" DLL_EXPORT int GetFriendsGroupIDByIndex(int index);
extern "C" DLL_EXPORT int GetFriendsGroupMembersCount(int friendsGroupID);
extern "C" DLL_EXPORT char *GetFriendsGroupMembersListJSON(int friendsGroupID); // return a json array of SteamID handles
extern "C" DLL_EXPORT char *GetFriendsGroupName(int friendsGroupID);
// Image methods
extern "C" DLL_EXPORT int LoadImageFromHandle(int hImage);
extern "C" DLL_EXPORT void LoadImageIDFromHandle(int imageID, int hImage);
// General user stats methods.
extern "C" DLL_EXPORT int RequestStats();
extern "C" DLL_EXPORT int GetRequestStatsCallbackState();
extern "C" DLL_EXPORT int StatsInitialized();
extern "C" DLL_EXPORT int StoreStats();
extern "C" DLL_EXPORT int ResetAllStats(int bAchievementsToo);
extern "C" DLL_EXPORT int GetStoreStatsCallbackState();
extern "C" DLL_EXPORT int StatsStored();
extern "C" DLL_EXPORT int AchievementStored();
// Achievements methods.
extern "C" DLL_EXPORT int GetNumAchievements();
extern "C" DLL_EXPORT char *GetAchievementID(int index);
extern "C" DLL_EXPORT char *GetAchievementDisplayName(const char *pchName);
extern "C" DLL_EXPORT char *GetAchievementDisplayDesc(const char *pchName);
extern "C" DLL_EXPORT int GetAchievementDisplayHidden(const char *pchName);
extern "C" DLL_EXPORT int GetAchievementIcon(const char *pchName);
extern "C" DLL_EXPORT int GetAchievement(const char *pchName);
extern "C" DLL_EXPORT int GetAchievementUnlockTime(const char *pchName);
extern "C" DLL_EXPORT int SetAchievement(const char *pchName);
extern "C" DLL_EXPORT int IndicateAchievementProgress(const char *pchName, int nCurProgress, int nMaxProgress);
extern "C" DLL_EXPORT int ClearAchievement(const char *pchName);
// User stats methods.
extern "C" DLL_EXPORT int GetStatInt(const char *pchName);
extern "C" DLL_EXPORT float GetStatFloat(const char *pchName);
extern "C" DLL_EXPORT int SetStatInt(const char *pchName, int nData);
extern "C" DLL_EXPORT int SetStatFloat(const char *pchName, float fData);
extern "C" DLL_EXPORT int UpdateAvgRateStat(const char *pchName, float flCountThisSession, float dSessionLength);
// Leaderboards
extern "C" DLL_EXPORT int FindLeaderboard(const char *pchLeaderboardName);
extern "C" DLL_EXPORT int GetFindLeaderboardCallbackState();
extern "C" DLL_EXPORT int GetLeaderboardHandle();
extern "C" DLL_EXPORT char *GetLeaderboardName(int hLeaderboard);
extern "C" DLL_EXPORT int GetLeaderboardEntryCount(int hLeaderboard);
extern "C" DLL_EXPORT int GetLeaderboardDisplayType(int hLeaderboard);
extern "C" DLL_EXPORT int GetLeaderboardSortMethod(int hLeaderboard);
extern "C" DLL_EXPORT int UploadLeaderboardScore(int hLeaderboard, int score);
extern "C" DLL_EXPORT int UploadLeaderboardScoreForceUpdate(int hLeaderboard, int score);
extern "C" DLL_EXPORT int GetUploadLeaderboardScoreCallbackState();
extern "C" DLL_EXPORT int LeaderboardScoreStored();
extern "C" DLL_EXPORT int LeaderboardScoreChanged();
extern "C" DLL_EXPORT int GetLeaderboardUploadedScore();
extern "C" DLL_EXPORT int GetLeaderboardGlobalRankNew();
extern "C" DLL_EXPORT int GetLeaderboardGlobalRankPrevious();
// https://partner.steamgames.com/doc/api/ISteamUserStats#ELeaderboardDataRequest
extern "C" DLL_EXPORT int DownloadLeaderboardEntries(int hLeaderboard, int eLeaderboardDataRequest, int nRangeStart, int nRangeEnd);
extern "C" DLL_EXPORT int GetDownloadLeaderboardEntriesCallbackState();
extern "C" DLL_EXPORT int GetDownloadedLeaderboardEntryCount();
extern "C" DLL_EXPORT int GetDownloadedLeaderboardEntryGlobalRank(int index);
extern "C" DLL_EXPORT int GetDownloadedLeaderboardEntryScore(int index);
extern "C" DLL_EXPORT int GetDownloadedLeaderboardEntryUser(int index);
// Lobby methods: List
extern "C" DLL_EXPORT int RequestLobbyList(); //  LobbyMatchList_t call result.
extern "C" DLL_EXPORT int GetLobbyMatchListCallbackState();
extern "C" DLL_EXPORT int GetLobbyMatchListCount();
extern "C" DLL_EXPORT int GetLobbyByIndex(int index);
// Lobby methods: Data
extern "C" DLL_EXPORT int GetLobbyDataCount(int hLobbySteamID);
extern "C" DLL_EXPORT char *GetLobbyDataByIndex(int hLobbySteamID, int index); // Returns json of key/value data.
extern "C" DLL_EXPORT char *GetLobbyDataJSON(int hLobbySteamID);
extern "C" DLL_EXPORT char *GetLobbyData(int hLobbySteamID, char *key);
extern "C" DLL_EXPORT int RequestLobbyData(int hLobbySteamID);
extern "C" DLL_EXPORT int HasLobbyDataUpdated();
extern "C" DLL_EXPORT int GetLobbyDataUpdatedLobby();
extern "C" DLL_EXPORT int GetLobbyDataUpdatedID();
extern "C" DLL_EXPORT char *GetLobbyMemberData(int hLobbySteamID, int hUserSteamID, char *key);
extern "C" DLL_EXPORT void SetLobbyMemberData(int hLobbySteamID, char *key, char *value);
// Lobby methods: Owner methods
extern "C" DLL_EXPORT int DeleteLobbyData(int hLobbySteamID, char *key);
extern "C" DLL_EXPORT void SetLobbyData(int hLobbySteamID, char *key, char *value);
//bool SetLinkedLobby(CSteamID steamIDLobby, CSteamID steamIDLobbyDependent);
//void SetLobbyGameServer(CSteamID steamIDLobby, uint32 unGameServerIP, uint16 unGameServerPort, CSteamID steamIDGameServer); // Triggers a LobbyGameCreated_t callback.
//bool SetLobbyJoinable(CSteamID steamIDLobby, bool bLobbyJoinable);
//bool SetLobbyMemberLimit(CSteamID steamIDLobby, int cMaxMembers);
//bool SetLobbyOwner(CSteamID steamIDLobby, CSteamID steamIDNewOwner);
//bool SetLobbyType(CSteamID steamIDLobby, ELobbyType eLobbyType);
// Lobby methods: Create
extern "C" DLL_EXPORT int CreateLobby(int eLobbyType, int maxMembers);
extern "C" DLL_EXPORT int GetLobbyCreateCallbackState();
extern "C" DLL_EXPORT int GetLobbyCreatedID();
extern "C" DLL_EXPORT int GetLobbyCreatedResult();
/*
k_EResultOK - The lobby was successfully created.
k_EResultFail - The server responded, but with an unknown internal error.
k_EResultTimeout - The message was sent to the Steam servers, but it didn't respond.
k_EResultNoConnection - Your game client has created too many lobbies and is being rate limited.
k_EResultAccessDenied - Your game isn't set to allow lobbies, or your client does haven't rights to play the game
k_EResultNoConnection - Your Steam client doesn't have a connection to the back-end.
*/
// Lobby methods: Join
extern "C" DLL_EXPORT int JoinLobby(int hLobbySteamID);
extern "C" DLL_EXPORT int GetLobbyEnterCallbackState();
extern "C" DLL_EXPORT int GetLobbyEnterID();
extern "C" DLL_EXPORT int GetLobbyEnterBlocked();
extern "C" DLL_EXPORT int GetLobbyEnterResponse();
// Lobby methods: Leave
extern "C" DLL_EXPORT void LeaveLobby(int hLobbySteamID);
// Lobby methods: Members
extern "C" DLL_EXPORT int GetLobbyOwner(int hLobbySteamID);
extern "C" DLL_EXPORT int GetLobbyMemberLimit(int hLobbySteamID);
extern "C" DLL_EXPORT int GetNumLobbyMembers(int hLobbySteamID);
extern "C" DLL_EXPORT int GetLobbyMemberByIndex(int hLobbySteamID, int index);
// Lobby methods: Member status
extern "C" DLL_EXPORT int HasLobbyChatUpdate();
extern "C" DLL_EXPORT int GetLobbyChatUpdateUserChanged();
extern "C" DLL_EXPORT int GetLobbyChatUpdateUserState();
extern "C" DLL_EXPORT int GetLobbyChatUpdateUserMakingChange();
// Lobby methods: Chat messages
extern "C" DLL_EXPORT int HasLobbyChatMessage();
extern "C" DLL_EXPORT int GetLobbyChatMessageUser();
extern "C" DLL_EXPORT char *GetLobbyChatMessageText();
extern "C" DLL_EXPORT int SendLobbyChatMessage(int hLobbySteamID, char *msg);

/*
int AddFavoriteGame(AppId_t nAppID, uint32 nIP, uint16 nConnPort, uint16 nQueryPort, uint32 unFlags, uint32 rTime32LastPlayedOnServer);
bool GetFavoriteGame(int iGame, AppId_t *pnAppID, uint32 *pnIP, uint16 *pnConnPort, uint16 *pnQueryPort, uint32 *punFlags, uint32 *pRTime32LastPlayedOnServer);
bool RemoveFavoriteGame(AppId_t nAppID, uint32 nIP, uint16 nConnPort, uint16 nQueryPort, uint32 unFlags);
int GetFavoriteGameCount();

void AddRequestLobbyListDistanceFilter(ELobbyDistanceFilter eLobbyDistanceFilter);
void AddRequestLobbyListFilterSlotsAvailable(int nSlotsAvailable);
void AddRequestLobbyListNearValueFilter(const char *pchKeyToMatch, int nValueToBeCloseTo);
void AddRequestLobbyListNumericalFilter(const char *pchKeyToMatch, int nValueToMatch, ELobbyComparison eComparisonType);
void AddRequestLobbyListStringFilter(const char *pchKeyToMatch, const char *pchValueToMatch, ELobbyComparison eComparisonType);
void AddRequestLobbyListResultCountFilter(int cMaxResults);

bool GetLobbyGameServer(CSteamID steamIDLobby, uint32 *punGameServerIP, uint16 *punGameServerPort, CSteamID *psteamIDGameServer);
bool InviteUserToLobby(CSteamID steamIDLobby, CSteamID steamIDInvitee); // Can this be done?
*/

#endif // DLLMAIN_H_