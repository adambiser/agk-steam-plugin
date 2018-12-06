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

// Callback states
enum ECallbackState
{
	ServerError = -2,
	ClientError = -1,
	Idle = 0,
	Running = 1,
	Done = 2
};

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
// App/DLC methods
extern "C" DLL_EXPORT char *GetDLCDataJSON();
extern "C" DLL_EXPORT char *GetDLCDataByIndexJSON(int index);
extern "C" DLL_EXPORT int IsAppInstalled(int appID);
extern "C" DLL_EXPORT int IsCybercafe();
extern "C" DLL_EXPORT int IsDLCInstalled(int appID);
extern "C" DLL_EXPORT int IsLowViolence();
extern "C" DLL_EXPORT int IsSubscribed();
extern "C" DLL_EXPORT int IsSubscribedApp(int appID);
extern "C" DLL_EXPORT int IsSubscribedFromFreeWeekend();
extern "C" DLL_EXPORT int IsVACBanned();
extern "C" DLL_EXPORT int GetAppBuildID();
extern "C" DLL_EXPORT char *GetAppInstallDir(int appID);
extern "C" DLL_EXPORT int GetAppOwner();
extern "C" DLL_EXPORT char *GetAvailableGameLanguages();
extern "C" DLL_EXPORT char *GetCurrentBetaName();
extern "C" DLL_EXPORT char *GetCurrentGameLanguage();
extern "C" DLL_EXPORT int GetDLCCount();
extern "C" DLL_EXPORT char *GetDLCDownloadProgressJSON(int appID);
extern "C" DLL_EXPORT int GetEarliestPurchaseUnixTime(int appID);
//SteamAPICall_t GetFileDetails(const char*pszFileName); // FileDetailsResult_t call result.
extern "C" DLL_EXPORT char *GetInstalledDepotsJSON(int appID, int maxDepots);
extern "C" DLL_EXPORT char *GetLaunchQueryParam(char *key);
extern "C" DLL_EXPORT int HasNewLaunchQueryParameters();
extern "C" DLL_EXPORT int HasNewDLCInstalled();
extern "C" DLL_EXPORT int GetNewDLCInstalled();
extern "C" DLL_EXPORT void InstallDLC(int appID); // Triggers a DlcInstalled_t callback.
extern "C" DLL_EXPORT int MarkContentCorrupt(int missingFilesOnly);
extern "C" DLL_EXPORT void UninstallDLC(int appID);
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
extern "C" DLL_EXPORT char *GetSteamID3(int hUserSteamID);
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
extern "C" DLL_EXPORT int HasGameLobbyJoinRequest();
extern "C" DLL_EXPORT int GetGameLobbyJoinRequestedLobby();
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
// Util methods
extern "C" DLL_EXPORT int GetCurrentBatteryPower();
extern "C" DLL_EXPORT int GetIPCCallCount();
extern "C" DLL_EXPORT char *GetIPCountry();
extern "C" DLL_EXPORT int GetSecondsSinceAppActive();
extern "C" DLL_EXPORT int GetSecondsSinceComputerActive();
extern "C" DLL_EXPORT int GetServerRealTime();
extern "C" DLL_EXPORT char *GetSteamUILanguage();
extern "C" DLL_EXPORT int IsOverlayEnabled();
extern "C" DLL_EXPORT void SetOverlayNotificationInset(int horizontalInset, int verticalInset);
extern "C" DLL_EXPORT void SetOverlayNotificationPosition(int eNotificationPosition);
extern "C" DLL_EXPORT int HasIPCountryChanged();
extern "C" DLL_EXPORT int HasLowBatteryWarning();
extern "C" DLL_EXPORT int GetMinutesBatteryLeft();
extern "C" DLL_EXPORT int IsSteamShuttingDown();
extern "C" DLL_EXPORT void SetWarningMessageHook();
// Big Picture methods
extern "C" DLL_EXPORT int IsSteamInBigPictureMode();
extern "C" DLL_EXPORT int HasGamepadTextInputDismissedInfo();
extern "C" DLL_EXPORT char *GetGamepadTextInputDismissedInfoJSON();
extern "C" DLL_EXPORT int ShowGamepadTextInput(int eInputMode, int eLineInputMode, char *description, int charMax, char *existingText);
// VR methods
extern "C" DLL_EXPORT int IsSteamRunningInVR();
extern "C" DLL_EXPORT void StartVRDashboard();
extern "C" DLL_EXPORT void SetVRHeadsetStreamingEnabled(int enabled);
extern "C" DLL_EXPORT int IsVRHeadsetStreamingEnabled();
/***********************************************
@group	Cloud
@page	Cloud Information
************************************************/
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
@parm enabled 1 to enabled or 0 to disable the Steam Cloud for this application.
@api
ISteamRemoteStorage#SetCloudEnabledForApp
*/
extern "C" DLL_EXPORT void SetCloudEnabledForApp(int enabled);
/***********************************************
@group	Cloud
@page	Files
************************************************/
/*
@desc
Deletes a file from the local disk, and propagates that delete to the cloud.

This is meant to be used when a user actively deletes a file. Use FileForget if you want to remove a file from the Steam Cloud but retain it on the users local disk.

When a file has been deleted it can be re-written with FileWrite to reupload it to the Steam Cloud.
@parm filename The name of the file that will be deleted.
1 if the file exists and has been successfully deleted; otherwise, 0 if the file did not exist.
@api ISteamRemoteStorage#FileDelete
*/
extern "C" DLL_EXPORT int CloudFileDelete(char *filename);
/*
@desc Checks whether the specified file exists.
@parm filename The name of the file.
@return 1 if the file exists; otherwise, 0.
@api ISteamRemoteStorage#FileExists
*/
extern "C" DLL_EXPORT int CloudFileExists(char *filename);
/*
@desc
Deletes the file from remote storage, but leaves it on the local disk and remains accessible from the API.

When you are out of Cloud space, this can be used to allow calls to FileWrite to keep working without needing to make the user delete files.

How you decide which files to forget are up to you. It could be a simple Least Recently Used (LRU) queue or something more complicated.

Requiring the user to manage their Cloud-ized files for a game, while is possible to do, it is never recommended.
For instance, "Which file would you like to delete so that you may store this new one?" removes a significant advantage of using the Cloud in the first place: its transparency.

Once a file has been deleted or forgotten, calling FileWrite will resynchronize it in the Cloud. Rewriting a forgotten file is the only way to make it persisted again.
@parm filename The name of the file that will be forgotten.
@return	1 if the file exists and has been successfully forgotten; otherwise, 0.
@api ISteamRemoteStorage#FileForget
*/
extern "C" DLL_EXPORT int CloudFileForget(char *filename);
/*
@desc
Checks if a specific file is persisted in the steam cloud.
@parm filename The name of the file.
@return
1 if the file exists and the file is persisted in the Steam Cloud.
0 if FileForget was called on it and is only available locally.
@api ISteamRemoteStorage#FilePersisted
*/
extern "C" DLL_EXPORT int CloudFilePersisted(char *filename);
/*
@desc
Opens a binary file, reads the contents of the file into a memblock, and then closes the file.

**NOTE:** This is a synchronous call and as such is a will block your calling thread on the disk IO, and will also block the SteamAPI, 
which can cause other threads in your application to block. To avoid "hitching" due to a busy disk on the client machine using FileReadAsync, 
the asynchronous version of this API is recommended.
@parm filename The name of the file to read from.
@return
A memblock ID containing the data read from the file.
Returns 0 if the file doesn't exist or the read fails.
@api ISteamRemoteStorage#FileRead
*/
extern "C" DLL_EXPORT int CloudFileRead(char *filename);
extern "C" DLL_EXPORT int CloudFileReadAsync(char *filename, int offset, int length);
extern "C" DLL_EXPORT int GetCloudFileReadAsyncCallbackState(char *filename);
extern "C" DLL_EXPORT int GetCloudFileReadAsyncResult(char *filename);
extern "C" DLL_EXPORT int GetCloudFileReadAsyncMemblock(char *filename);
//extern "C" DLL_EXPORT SteamAPICall_t CloudFileShare(const char *filename);
/*
@desc
Creates a new file, writes the memblock to the file, and then closes the file. If the target file already exists, it is overwritten.

**NOTE:** This is a synchronous call and as such is a will block your calling thread on the disk IO, and will also block the SteamAPI, 
which can cause other threads in your application to block. To avoid "hitching" due to a busy disk on the client machine using FileWriteAsync, 
the asynchronous version of this API is recommended.
@parm filename The name of the file to write to.
@parm memblock A memblock of data to write to the file.
@return 1 if the write was successful.

Otherwise, 0 under the following conditions:
* The file you're trying to write is larger than 100MiB as defined by k_unMaxCloudFileChunkSize.
* The memblock is empty.
* You tried to write to an invalid path or filename.
* The current user's Steam Cloud storage quota has been exceeded. They may have run out of space, or have too many files.
* Steam could not write to the disk, the location might be read-only.
@api ISteamRemoteStorage#FileWrite
*/
extern "C" DLL_EXPORT int CloudFileWrite(char *filename, int memblockID);
extern "C" DLL_EXPORT int CloudFileWriteAsync(char *filename, int memblockID);
extern "C" DLL_EXPORT int GetCloudFileWriteAsyncCallbackState(char *filename);
extern "C" DLL_EXPORT int GetCloudFileWriteAsyncResult(char *filename);
//extern "C" DLL_EXPORT bool CloudFileWriteStreamCancel(UGCFileWriteStreamHandle_t writeHandle);
//extern "C" DLL_EXPORT bool CloudFileWriteStreamClose(UGCFileWriteStreamHandle_t writeHandle);
//extern "C" DLL_EXPORT UGCFileWriteStreamHandle_t FileWriteStreamOpen(const char *filename);
//extern "C" DLL_EXPORT bool CloudFileWriteStreamWriteChunk(UGCFileWriteStreamHandle_t writeHandle, const void *pvData, int32 cubData);
/*
@desc Gets the total number of local files synchronized by Steam Cloud.
@return The number of files present for the current user, including files in subfolders.
@api ISteamRemoteStorage#GetFileCount
*/
extern "C" DLL_EXPORT int GetCloudFileCount();
/*
@desc Gets the file name at the given index
@parm index The index of the file, this should be between 0 and GetFileCount.
@return
The name of the file at the given index.  An empty string if the file doesn't exist.
@api ISteamRemoteStorage#GetFileCount, ISteamRemoteStorage#GetFileNameAndSize
*/
extern "C" DLL_EXPORT char * GetCloudFileName(int index);
/*
@desc Gets the name and size of all local files synchronized by Steam Cloud.
@return
A JSON string with the file information. (See steam_constants.agc)
```
Type CloudFileInfo_t
	Name as string		// The file name.
	Size as integer		// The size of the file.
EndType
```
@api ISteamRemoteStorage#GetFileCount, ISteamRemoteStorage#GetFileNameAndSize
*/
extern "C" DLL_EXPORT char *GetCloudFileListJSON();
/*
@desc Gets the specified files size in bytes.
@parm filename 	The name of the file.
@return The size of the file in bytes. Returns 0 if the file does not exist.
@api ISteamRemoteStorage#GetFileSize
*/
extern "C" DLL_EXPORT int GetCloudFileSize(char *filename);
/*
@desc
Gets the specified file's last modified timestamp in Unix epoch format (seconds since Jan 1st 1970).
@parm filename The name of the file.
@return The last modified timestamp in Unix epoch format.
@api ISteamRemoteStorage#GetFileTimestamp
*/
extern "C" DLL_EXPORT int GetCloudFileTimestamp(char *filename);
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
@desc Gets the number of bytes available, and total on the user's Steam Cloud storage.
@return
A JSON string with the quote information. (See steam_constants.agc)
```
Type CloudQuota_t
	Available as integer	// The number of bytes available.
	Total as integer		// Total amount of bytes the user has access to.
	EndType
```
@api ISteamRemoteStorage#GetQuota
*/
extern "C" DLL_EXPORT char *GetCloudQuotaJSON();
/*
@desc Obtains the platforms that the specified file will syncronize to.
@parm filename The name of the file.
@return Bitfield containing the platforms that the file was set to with SetSyncPlatforms.
@api ISteamRemoteStorage#GetSyncPlatforms
ISteamRemoteStorage#ERemoteStoragePlatform
*/
extern "C" DLL_EXPORT int GetCloudFileSyncPlatforms(char *filename);
/*
@desc
Allows you to specify which operating systems a file will be synchronized to.

Use this if you have a multiplatform game but have data which is incompatible between platforms.

Files default to k_ERemoteStoragePlatformAll when they are first created. You can use the bitwise OR operator to specify multiple platforms.
@parm filename The name of the file.
@parm eRemoteStoragePlatform The platforms that the file will be syncronized to.
@parm-api eRemoteStoragePlatform ISteamRemoteStorage#ERemoteStoragePlatform
@return 1 if the file exists, otherwise 0.
@api ISteamRemoteStorage#SetSyncPlatforms
*/
extern "C" DLL_EXPORT bool SetCloudFileSyncPlatforms(char *filename, int eRemoteStoragePlatform);
// User-Generated Content
//extern "C" DLL_EXPORT int32 GetCachedUGCCount();
//extern "C" DLL_EXPORT UGCHandle_t GetCachedUGCHandle(int32 iCachedContent);
//extern "C" DLL_EXPORT bool GetUGCDetails(UGCHandle_t hContent, AppId_t *pnAppID, char **ppchName, int32 *pnFileSizeInBytes, CSteamID *pSteamIDOwner);
//extern "C" DLL_EXPORT bool GetUGCDownloadProgress(UGCHandle_t hContent, int32 *pnBytesDownloaded, int32 *pnBytesExpected);
//extern "C" DLL_EXPORT SteamAPICall_t UGCDownload(UGCHandle_t hContent, uint32 unPriority);
//extern "C" DLL_EXPORT SteamAPICall_t UGCDownloadToLocation(UGCHandle_t hContent, const char *pchLocation, uint32 unPriority);
//extern "C" DLL_EXPORT int32 UGCRead(UGCHandle_t hContent, void *pvData, int32 cubDataToRead, uint32 cOffset, EUGCReadAction eAction);

#endif // DLLMAIN_H_