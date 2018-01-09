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
extern "C" DLL_EXPORT int IsSteamIDValid(int hSteamID);
extern "C" DLL_EXPORT int GetHandleFromSteamID64(char *steamID64);
extern "C" DLL_EXPORT void RunCallbacks();
extern "C" DLL_EXPORT char *GetCommandLineArgsJSON();
// Overlay methods
extern "C" DLL_EXPORT int IsGameOverlayActive();
extern "C" DLL_EXPORT void ActivateGameOverlay(const char *pchDialog);
extern "C" DLL_EXPORT void ActivateGameOverlayInviteDialog(int hLobbySteamID);
extern "C" DLL_EXPORT void ActivateGameOverlayToStore(int appID, int flag);
extern "C" DLL_EXPORT void ActivateGameOverlayToUser(const char *pchDialog, int hSteamID);
extern "C" DLL_EXPORT void ActivateGameOverlayToWebPage(const char *url);
// User/Friend methods
extern "C" DLL_EXPORT int LoggedOn();
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
extern "C" DLL_EXPORT void AddRequestLobbyListDistanceFilter(int eLobbyDistanceFilter);
extern "C" DLL_EXPORT void AddRequestLobbyListFilterSlotsAvailable(int slotsAvailable);
extern "C" DLL_EXPORT void AddRequestLobbyListNearValueFilter(char *pchKeyToMatch, int valueToBeCloseTo);
extern "C" DLL_EXPORT void AddRequestLobbyListNumericalFilter(char *pchKeyToMatch, int valueToMatch, int eComparisonType);
extern "C" DLL_EXPORT void AddRequestLobbyListResultCountFilter(int maxResults);
extern "C" DLL_EXPORT void AddRequestLobbyListStringFilter(char *pchKeyToMatch, char *pchValueToMatch, int eComparisonType);
extern "C" DLL_EXPORT int RequestLobbyList(); //  LobbyMatchList_t call result.
extern "C" DLL_EXPORT int GetLobbyMatchListCallbackState();
extern "C" DLL_EXPORT int GetLobbyMatchListCount();
extern "C" DLL_EXPORT int GetLobbyByIndex(int index);
// Lobby methods: Create, Join, Leave
extern "C" DLL_EXPORT int CreateLobby(int eLobbyType, int maxMembers);
extern "C" DLL_EXPORT int GetLobbyCreateCallbackState();
extern "C" DLL_EXPORT int GetLobbyCreatedID();
extern "C" DLL_EXPORT int GetLobbyCreatedResult();
//extern "C" DLL_EXPORT int SetLinkedLobby(int hLobbySteamID, int hLobbyDependentSteamID);
extern "C" DLL_EXPORT int SetLobbyJoinable(int hLobbySteamID, bool lobbyJoinable);
extern "C" DLL_EXPORT int SetLobbyType(int hLobbySteamID, int eLobbyType);
extern "C" DLL_EXPORT int JoinLobby(int hLobbySteamID);
extern "C" DLL_EXPORT int GetLobbyEnterCallbackState();
extern "C" DLL_EXPORT int GetLobbyEnterID();
extern "C" DLL_EXPORT int GetLobbyEnterBlocked();
extern "C" DLL_EXPORT int GetLobbyEnterResponse();
extern "C" DLL_EXPORT int InviteUserToLobby(int hLobbySteamID, int hInviteeSteamID);
extern "C" DLL_EXPORT void LeaveLobby(int hLobbySteamID);
// Lobby methods: Game server
extern "C" DLL_EXPORT char *GetLobbyGameServerJSON(int hLobbySteamID);
extern "C" DLL_EXPORT int SetLobbyGameServer(int hLobbySteamID, char *gameServerIP, int gameServerPort, int hGameServerSteamID); // Triggers a LobbyGameCreated_t callback.
extern "C" DLL_EXPORT int HasLobbyGameCreated();
extern "C" DLL_EXPORT char *GetLobbyGameCreatedJSON();
// Lobby methods: Data
extern "C" DLL_EXPORT char *GetLobbyData(int hLobbySteamID, char *key);
extern "C" DLL_EXPORT int GetLobbyDataCount(int hLobbySteamID);
extern "C" DLL_EXPORT char *GetLobbyDataByIndexJSON(int hLobbySteamID, int index); // Returns json of key/value data.
extern "C" DLL_EXPORT char *GetLobbyDataJSON(int hLobbySteamID);
extern "C" DLL_EXPORT void SetLobbyData(int hLobbySteamID, char *key, char *value);
extern "C" DLL_EXPORT int DeleteLobbyData(int hLobbySteamID, char *key);
extern "C" DLL_EXPORT int RequestLobbyData(int hLobbySteamID);
extern "C" DLL_EXPORT int HasLobbyDataUpdated();
extern "C" DLL_EXPORT int GetLobbyDataUpdatedLobby();
extern "C" DLL_EXPORT int GetLobbyDataUpdatedID();
extern "C" DLL_EXPORT char *GetLobbyMemberData(int hLobbySteamID, int hUserSteamID, char *key);
extern "C" DLL_EXPORT void SetLobbyMemberData(int hLobbySteamID, char *key, char *value);
// Lobby methods: Members and status
extern "C" DLL_EXPORT int GetLobbyOwner(int hLobbySteamID);
extern "C" DLL_EXPORT int SetLobbyOwner(int hLobbySteamID, int hNewOwnerSteamID);
extern "C" DLL_EXPORT int GetLobbyMemberLimit(int hLobbySteamID);
extern "C" DLL_EXPORT int SetLobbyMemberLimit(int hLobbySteamID, int maxMembers);
extern "C" DLL_EXPORT int GetNumLobbyMembers(int hLobbySteamID);
extern "C" DLL_EXPORT int GetLobbyMemberByIndex(int hLobbySteamID, int index);
extern "C" DLL_EXPORT int HasLobbyChatUpdate();
extern "C" DLL_EXPORT int GetLobbyChatUpdateUserChanged();
extern "C" DLL_EXPORT int GetLobbyChatUpdateUserState();
extern "C" DLL_EXPORT int GetLobbyChatUpdateUserMakingChange();
// Lobby methods: Chat messages
extern "C" DLL_EXPORT int HasLobbyChatMessage();
extern "C" DLL_EXPORT int GetLobbyChatMessageUser();
extern "C" DLL_EXPORT char *GetLobbyChatMessageText();
extern "C" DLL_EXPORT int SendLobbyChatMessage(int hLobbySteamID, char *msg);
// Lobby methods: Favorite games
extern "C" DLL_EXPORT int AddFavoriteGame(int appID, char *ip, int connectPort, int queryPort, int flags);//, int time32LastPlayedOnServer);
extern "C" DLL_EXPORT int GetFavoriteGameCount();
extern "C" DLL_EXPORT char *GetFavoriteGameJSON(int index);
extern "C" DLL_EXPORT int RemoveFavoriteGame(int appID, char *ip, int connectPort, int queryPort, int flags);
// Game server
//extern "C" DLL_EXPORT char *GetPublicIP();
// Music methods
extern "C" DLL_EXPORT int IsMusicEnabled();
extern "C" DLL_EXPORT int IsMusicPlaying();
extern "C" DLL_EXPORT int GetMusicPlaybackStatus();
extern "C" DLL_EXPORT float GetMusicVolume();
extern "C" DLL_EXPORT void PauseMusic();
extern "C" DLL_EXPORT void PlayMusic();
extern "C" DLL_EXPORT void PlayNextSong();
extern "C" DLL_EXPORT void PlayPreviousSong();
extern "C" DLL_EXPORT void SetMusicVolume(float volume);
extern "C" DLL_EXPORT int HasMusicPlaybackStatusChanged();
extern "C" DLL_EXPORT int HasMusicVolumeChanged();

#endif // DLLMAIN_H_