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

#ifndef _DLLMAIN_H_
#define _DLLMAIN_H_
#pragma once

#include "..\AGKLibraryCommands.h"
#include "steam_api.h"
#include "PluginTypes.h"

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

int GetPluginHandle(uint64 handle);
int GetPluginHandle(CSteamID steamID);

extern uint64 g_AppID;
extern bool g_SteamInitialized;
extern bool g_IsGameOverlayActive;

/* @page General Commands */
/*
@desc
Initializes the Steam API.  This method should be called before attempting to do anything else with this plugin.
This also calls RequestStats() internally so calling code does not need to do so. The result of the RequestStats call has no effect on the return value.
@return 1 when Steam API initialization succeeds; otherwise 0.
@api steam_api#SteamAPI_Init
*/
extern "C" DLL_EXPORT int Init();
/*
@desc Shuts down the plugin and frees memory.
@api steam_api#SteamAPI_Shutdown
*/
extern "C" DLL_EXPORT void Shutdown();
/*
@desc Gets whether the Steam API is currently initialized.
@return 1 when the Steam API is initialized; otherwise 0.
*/
extern "C" DLL_EXPORT int SteamInitialized();
/*
@desc 
_When used, this command should be called near the beginning of the starting process and should be called before Init._

Tests to see if the game has been started through the Steam Client.  Effectively serves as a basic DRM by forcing the game to be started by the Steam Client.
@param ownAppID The Steam AppID of your game.
@return
1 when a new instance of the game has been started by Steam Client and this instance should close as soon as possible; 
otherwise 0 when the game is already running from the Steam Client.
0 will also be reported if the steam_appid.txt file exists.
@api steam_api#SteamAPI_RestartAppIfNecessary
*/
extern "C" DLL_EXPORT int RestartAppIfNecessary(int ownAppID);
/*
@desc
_This method is just for testing purposes since the app should already know its AppID._

Returns the AppID or 0 if the Steam API has not been not initialized or the AppID cannot be found.
@return The AppID or 0.
@api ISteamUtils#GetAppID
*/
extern "C" DLL_EXPORT int GetAppID();
/*
@desc
_This method is part of a restricted interface that can only be used by approved apps._

Returns the app name for the given appid.
@param appID The AppID to get the name for.
@return The app name or an empty string if the app has not been approved for this method.
@api ISteamAppList#GetAppName
*/
extern "C" DLL_EXPORT char *GetAppName(int appID);
/*
@desc Checks to see if a Steam ID handle is valid.
@param hSteamID The Steam ID handle to check.
@return 1 when the Steam ID handle is valid; otherwise 0.
*/
extern "C" DLL_EXPORT int IsSteamIDValid(int hSteamID);
/*
@desc Returns the handle for the given SteamID64 string.
@param steamID64 A SteamID64 string.
@return A Steam ID handle or 0.
*/
extern "C" DLL_EXPORT int GetHandleFromSteamID64(const char *steamID64);
/*
@desc
Allows asynchronous Steam API calls to handle any new results.
Should be called each frame sync.
@api steam_api#SteamAPI_RunCallbacks
*/
extern "C" DLL_EXPORT void RunCallbacks();
/*
@desc Returns the command line arguments.

_Note: This command is not part of the Steam API, but is included as a convenience._
@return The command line arguments as a JSON string of an array of strings.
*/
extern "C" DLL_EXPORT char *GetCommandLineArgsJSON();
/*
@desc Returns the path of the folder containing the Steam client EXE as found in the registry
by reading the value of SteamPath in HKEY_CURRENT_USER\Software\Valve\Steam.
The path may contain slashes instead of backslashes and the trailing slash may or may not be included.

_Note: This command is not part of the Steam API, but is included as a convenience._
@return The Steam path.
*/
extern "C" DLL_EXPORT char *GetSteamPath();
/*
@desc Attempts to detect when Steam is being emulated.  Emulation is sometimes used with pirated games, but it can also be used for valid reasons.

_Note: This command is not part of the Steam API, but is included as a convenience._
@return 1 when Steam emulation is detected, otherwise 0.
*/
extern "C" DLL_EXPORT int IsSteamEmulated();
#undef SetFileAttributes
/*
@desc Sets the attributes of a file.
This is only included to help with development because the AppGameKit IDE deletes Steam files 
in the project folder when the interpreter exits.
@param filename The name of the file whose attributes are to be set.
@param attributes The file attributes to set for the file.
@return 1 if successful; otherwise, 0.
*/
extern "C" DLL_EXPORT int SetFileAttributes(const char *filename, int attributes);
/* @page Callbacks */
/*
@desc Deletes a [call result](Callbacks-and-Call-Results#call-results) and frees any associated memory for the call result.
@param hCallResult The call result handle to delete.
*/
extern "C" DLL_EXPORT void DeleteCallResult(int hCallResult);
/*
@desc Returns the state of the given [call result](Callbacks-and-Call-Results#call-results).
@param hCallResult The call result handle to check.
@return [A callback state](Callbacks-and-Call-Results#states)
*/
extern "C" DLL_EXPORT int GetCallResultState(int hCallResult);
/*
@desc Returns the result code of the given [call result](Callbacks-and-Call-Results#call-results).
@param hCallResult The call result handle to check.
@return An EResult code.
@api link to EResult
*/
extern "C" DLL_EXPORT int GetCallResultCode(int hCallResult);
/*
@desc A JSON string representing the data returned by the given [call result](Callbacks-and-Call-Results#call-results).
The format of the JSON string is determined by the call the call result is for.
@param hCallResult The call result handle to check.
@return A JSON string.
*/
extern "C" DLL_EXPORT char *GetCallResultJSON(int hCallResult);
/* @page App Information */
/*
@desc Gets the buildid of this app, may change at any time based on backend updates to the game.
@return The App BuildID if installed; otherwise 0.
@api ISteamApps#GetAppBuildId
*/
extern "C" DLL_EXPORT int GetAppBuildID();
/*
@desc Checks if the user is running from a beta branch, and gets the name of the branch if they are.
@return The name of the beta branch running; otherwise an empty string if not a beta branch or the app is not installed
@api ISteamApps#GetCurrentBetaName
*/
extern "C" DLL_EXPORT char *GetCurrentBetaName();
/*
@desc Gets a comma separated list of the languages the current app supports.
@return List of languages, separated by commas.
@api ISteamApps#GetAvailableGameLanguages
*/
extern "C" DLL_EXPORT char *GetAvailableGameLanguages();
/*
@desc Gets the current language that the user has set.
@return The current language if the app is installed; otherwise an empty string.
@api ISteamApps#GetCurrentGameLanguage
*/
extern "C" DLL_EXPORT char *GetCurrentGameLanguage();
/*
@desc Checks if a specific app is installed.
This only works for base applications, not DLC. Use IsDLCInstalled for DLC instead.
@param appID The App ID of the application to check.
@return 1 if the app is installed; otherwise 0.
@api ISteamApps#BIsAppInstalled
*/
extern "C" DLL_EXPORT int IsAppInstalled(int appID);
/*
@desc Gets the install folder for a specific AppID.
This works even if the application is not installed, based on where the game would be installed with the default Steam library location.
@param appID The App ID to get the install dir for.
@return The path the app is installed, or would be installed.
@api ISteamApps#GetAppInstallDir
*/
extern "C" DLL_EXPORT char *GetAppInstallDir(int appID);
/*
@desc Gets the Steam ID handle of the original owner of the current app. If it's different from the current user then it is borrowed.
@return A Steam ID handle.
@api ISteamApps#GetAppOwner
*/
extern "C" DLL_EXPORT int GetAppOwner();
/*
@desc Gets the time of purchase of the specified app in Unix epoch format (time since Jan 1st, 1970).
@param appID The App ID to get the purchase time for.
@return A Unix epoch time.
@api ISteamApps#GetEarliestPurchaseUnixTime
*/
extern "C" DLL_EXPORT int GetEarliestPurchaseUnixTime(int appID);
/*
@desc Gets a list of all installed depots for a given App ID in mount order.
@param appID The App to list the depots for.
@param maxDepots The maximum number of depots to obtain.
@return A JSON string of an integer array.
*/
extern "C" DLL_EXPORT char *GetInstalledDepotsJSON(int appID, int maxDepots);
/*
@desc Gets the associated launch parameter if the game is run via steam://run/<appid>/?param1=value1;param2=value2;param3=value3 etc.
@param key The launch key to test for. Ex: param1
@return The key value or an empty string if the key doesn't exist.
@api ISteamApps#GetLaunchQueryParam
*/
extern "C" DLL_EXPORT char *GetLaunchQueryParam(const char *key);
/*
@desc Posted after the user executes a steam url with query parameters while running.
Once a post has been reported, this method returns 0 until another post arrives.
@return 1 when the user executes a steam url with query parameters while running; otherwise 0.
@api ISteamApps#NewLaunchQueryParameters_t
*/
extern "C" DLL_EXPORT int HasNewLaunchQueryParameters();
/*
@desc Gets the command line if game was launched via Steam URL, e.g. steam://run/<appid>//<command line>/.
@return The command line if launched via Steam URL.
@api ISteamApps#GetLaunchCommandLine
*/
extern "C" DLL_EXPORT char *GetLaunchCommandLine();
/*
@desc Checks whether the current App ID is for Cyber Cafes.
@return 1 if a cyber cafe; otherwise 0.
@api ISteamApps#BIsCybercafe*/
extern "C" DLL_EXPORT int IsCybercafe();
/*
@desc Checks if the license owned by the user provides low violence depots.
@return 1 if the license owned by the user provides low violence depots; otherwise 0.
@api ISteamApps#BIsLowViolence
*/
extern "C" DLL_EXPORT int IsLowViolence();
/*
@desc Checks if the active user is subscribed to the current App ID.
@return 1 if the active user owns the current AppId; otherwise 0.
@api ISteamApps#BIsSubscribed
*/
extern "C" DLL_EXPORT int IsSubscribed();
/*
@desc Checks if the active user is subscribed to a specified AppId.
Only use this if you need to check ownership of another game related to yours, a demo for example.
@param appID The App ID to check.
@return 1 if the active user is subscribed to the specified App ID; otherwise 0.
@api ISteamApps#BIsSubscribedApp
*/
extern "C" DLL_EXPORT int IsSubscribedApp(int appID);
/*
@desc Check if user borrowed this game via Family Sharing, If true, call GetAppOwner() to get the lender SteamID
@return 1 if the active user borrowed this game via Family Sharing; otherwise 0.
@api ISteamApps#BIsSubscribedFromFamilySharing
*/
extern "C" DLL_EXPORT int IsSubscribedFromFamilySharing();
/*
@desc Checks if the user is subscribed to the current App ID through a free weekend.
@return 1 if the active user is subscribed to the current App Id via a free weekend; otherwise 0 for any other kind of license.
@api ISteamApps#BIsSubscribedFromFreeWeekend
*/
extern "C" DLL_EXPORT int IsSubscribedFromFreeWeekend();
/*
@desc Allows you to force verify game content on next launch.

If you detect the game is out-of-date (for example, by having the client detect a version mismatch with a server),
you can call use MarkContentCorrupt to force a verify, show a message to the user, and then quit.
@return A boolean as 0 or 1.  Steam API doesn't indicate what this means.
@api ISteamApps#MarkContentCorrupt
*/
extern "C" DLL_EXPORT int MarkContentCorrupt(int missingFilesOnly);
/* @page DLC Information */
/*
@desc Checks if the user owns a specific DLC and if the DLC is installed
@param appID The App ID of the DLC to check.
@return 1 when the user owns a DLC and it is installed; otherwise 0.
@api ISteamApps#BIsDlcInstalled
*/
extern "C" DLL_EXPORT int IsDLCInstalled(int appID);
/*
@desc Gets the number of DLC pieces for the current app.
@return The number of DLC pieces for the current app.
@api ISteamApps#GetDLCCount
*/
extern "C" DLL_EXPORT int GetDLCCount();
/*
@desc Gets DLC metadata of all DLCs for this app.
@return A JSON string of an array of DLC information. (See steam_constants.agc)
```
Type DLCData_t
	AppID as integer	// If 0, the DLC is hidden.
	Available as integer	// Boolean: If 1, the user owns the DLC.
	Name as string
EndType
```
@api ISteamApps#GetDLCCount, ISteamApps#BGetDLCDataByIndex
*/
extern "C" DLL_EXPORT char *GetDLCDataJSON();
/*
@desc HasNewDLCInstalled will report true when a DLC has finished installing.
@param appID The App ID of the DLC you want to install.
@api ISteamApps#InstallDLC
*/
extern "C" DLL_EXPORT void InstallDLC(int appID); // Triggers a DlcInstalled_t callback.
/*
@desc Uninstall a DLC.

_Note: This appears to take place after exiting the app._
@param appID The App ID of the DLC you want to uninstall.
@api ISteamApps#UninstallDLC
*/
extern "C" DLL_EXPORT void UninstallDLC(int appID);
/*
@desc Gets the download progress for DLC.
@param appID The App ID of the DLC to monitor.
@return A JSON string of the download progress.
```
Type DownloadProgress_t
	AppID as integer			// If 0, the DLC is not currently downloading.
	BytesDownloaded as integer
	BytesTotal as integer
EndType
```
@api ISteamApps#GetDlcDownloadProgress
*/
extern "C" DLL_EXPORT char *GetDLCDownloadProgressJSON(int appID);
/*
@desc Triggered after the current user gains ownership of DLC and that DLC is installed.
@return 1 when a DLC has finished installing; otherwise 0.
@api ISteamApps#DlcInstalled_t
*/
extern "C" DLL_EXPORT int HasNewDLCInstalled();
/*
@desc Gets the App ID of the newly installed DLC.

_HasNewDLCInstalled must be called prior to this method._
@return An App ID.
@api ISteamApps#DlcInstalled_t
*/
extern "C" DLL_EXPORT int GetNewDLCInstalled();
/* @page Overlay */
/*
@desc Checks if the Steam Overlay is running and the user can access it.

The overlay process could take a few seconds to start and hook the game process, so this method will initially return 0 while the overlay is loading.
@return 1 when the overlay is enabled; otherwise 0.
@api ISteamUtils#IsOverlayEnabled
*/
extern "C" DLL_EXPORT int IsOverlayEnabled();
/* 
@desc Gets whether the Steam Overlay has been activated or deactivated. 
@return 1 when the Steam overlay is active and 0 when it is not.
@api ISteamFriends#GameOverlayActivated_t
*/
extern "C" DLL_EXPORT int IsGameOverlayActive();
/*
@desc Activates the Steam Overlay to a specific dialog.
@param dialogName The dialog to open. Valid options are: "friends", "community", "players", "settings", "officialgamegroup", "stats", "achievements".
@api ISteamFriends#ActivateGameOverlay
*/
extern "C" DLL_EXPORT void ActivateGameOverlay(const char *dialogName);
/*
@desc Activates the Steam Overlay to open the invite dialog. Invitations sent from this dialog will be for the provided lobby.
@param hLobbySteamID The Steam ID handle of the lobby that selected users will be invited to.
@api ISteamFriends#ActivateGameOverlayInviteDialog
*/
extern "C" DLL_EXPORT void ActivateGameOverlayInviteDialog(int hLobbySteamID);
/*
@desc Activates the Steam Overlay to the Steam store page for the provided app.
@param appID The app ID to show the store page of or 0 for the front page of the Steam store.
@param flag Flags to modify the behavior when the page opens.
@param-api flag ISteamFriends#EOverlayToStoreFlag
@api 
*/
extern "C" DLL_EXPORT void ActivateGameOverlayToStore(int appID, int flag);
/*
@desc Activates Steam Overlay to a specific dialog.
@param dialogName The dialog to open. Valid options are: "steamid", "chat", "jointrade", "stats", "achievements", "friendadd", "friendremove", "friendrequestaccept", "friendrequestignore".
@param hSteamID The Steam ID handle of the context to open this dialog to.
@api ISteamFriends#ActivateGameOverlayToUser
*/
extern "C" DLL_EXPORT void ActivateGameOverlayToUser(const char *dialogName, int hSteamID);
/*
@desc Activates Steam Overlay web browser directly to the specified URL.
@param url The webpage to open. (A fully qualified address with the protocol is required.)
@api ISteamFriends#ActivateGameOverlayToWebPage
*/
extern "C" DLL_EXPORT void ActivateGameOverlayToWebPage(const char *url);
/*
@desc Activates Steam Overlay web browser directly to the specified URL.
The browser will be opened in a special overlay configuration which hides all other windows that the user has open in the overlay.
@param url The webpage to open. (A fully qualified address with the protocol is required.)
@api ISteamFriends#ActivateGameOverlayToWebPage
*/
extern "C" DLL_EXPORT void ActivateGameOverlayToWebPageModal(const char *url);
/*
@desc Sets the inset of the overlay notification from the corner specified by SetOverlayNotificationPosition.
@param horizontalInset The horizontal (left-right) distance in pixels from the corner.
@param verticalInset The vertical (up-down) distance in pixels from the corner.
@api ISteamUtils#SetOverlayNotificationInset
*/
extern "C" DLL_EXPORT void SetOverlayNotificationInset(int horizontalInset, int verticalInset);
/*
@desc Sets which corner the Steam overlay notification popup should display itself in.

This position is per-game and is reset each launch.
@param eNotificationPosition The notification popup position.
@param-api eNotificationPosition steam_api#ENotificationPosition
@api ISteamUtils#SetOverlayNotificationPosition
*/
extern "C" DLL_EXPORT void SetOverlayNotificationPosition(int eNotificationPosition);
/* @page Current User */
/*
@desc Checks to see whether the user is logged on to Steam.
@return 1 when the user is logged on; otherwise 0.
@api ISteamUser#BLoggedOn
*/
extern "C" DLL_EXPORT int LoggedOn();
/*
@desc Gets the current users persona (display) name.
@return the name
@api ISteamFriends#GetPersonaName
*/
extern "C" DLL_EXPORT char *GetPersonaName();
/*
@desc Gets a handle to the Steam ID of the account currently logged into the Steam client.
@return A SteamID handle
@api ISteamUser#GetSteamID
*/
extern "C" DLL_EXPORT int GetSteamID();
/*
@desc Checks if the user has a VAC ban on their account.
@return 1 when the user has a VAC ban; otherwise 0.
@api ISteamApps#BIsVACBanned
*/
extern "C" DLL_EXPORT int IsVACBanned();
/* @page Friend Information */
/*
@desc Gets the the account ID (profile number) for the given Steam ID handle.
@param hUserSteamID A user Steam ID handle.
@return The account ID.
*/
extern "C" DLL_EXPORT int GetAccountID(int hUserSteamID);
/*
@desc Gets the the SteamID3 for the given Steam ID handle.
@param hUserSteamID A user Steam ID handle.
@return A string containing the SteamID3.
*/
extern "C" DLL_EXPORT char *GetSteamID3(int hUserSteamID);
/*
@desc Gets the the SteamID64 (profile number) for the given Steam ID handle.
@param hUserSteamID A user Steam ID handle.
@return A string containing the 64-bit SteamID64 in base 10.
*/
extern "C" DLL_EXPORT char *GetSteamID64(int hUserSteamID);
/*
@desc Indicates whether the PersonaStateChange_t [[callback|Callbacks-and-Call-Results#callbacks]] has stored data.
@return 1 when there is data stored; otherwise 0.
@api ISteamFriends#PersonaStateChange_t
*/
extern "C" DLL_EXPORT int HasPersonaStateChange();
/*
@desc Returns a JSON object for the PersonaStateChange_t callback data.

_HasPersonaStateChanged must be called prior to this method._
@return The SteamID handle of the user whose persona state changed
@api ISteamFriends#PersonaStateChange_t
*/
extern "C" DLL_EXPORT char *GetPersonaStateChangeJSON();
/*
@desc Requests the persona name and optionally the avatar of a specified user (when requireNameOnly is 0).
@param hUserSteamID A user Steam ID handle.
@param requireNameOnly When 0 the avatar for the user is also loaded; otherwise only the name is loaded.
@return 1 when the user data needs to be loaded and will be reported by the PersonaStateChange_t callback.  0 when the user data is already loaded.
@api ISteamFriends#RequestUserInformation
*/
extern "C" DLL_EXPORT int RequestUserInformation(int hUserSteamID, int requireNameOnly);
/*
@desc Checks if the specified friend is in a game, and gets info about the game if they are.
@param hUserSteamID A user Steam ID handle.
@return A string representation of the FriendGameInfo_t data as a type JSON.
```
Type FriendGameInfo_t
	InGame as integer // boolean
	GameAppID as integer
	GameIP as integer
	GamePort as integer
	QueryPort as integer
	SteamIDLobby as integer // CSteamID handle
EndType
```
@api ISteamFriends#GetFriendGamePlayed
*/
extern "C" DLL_EXPORT char *GetFriendGamePlayedJSON(int hUserSteamID);
/*
@desc Gets the specified user's persona (display) name.
@param hUserSteamID A user Steam ID handle.
@return the user's name.
@api ISteamFriends#GetFriendPersonaName
*/
extern "C" DLL_EXPORT char *GetFriendPersonaName(int hUserSteamID);
/*
@desc Gets the current status of the specified user.
@param hUserSteamID A user Steam ID handle.
@return An [EPersonaState](https://partner.steamgames.com/doc/api/ISteamFriends#EPersonaState) value.
@api ISteamFriends#EPersonaState, ISteamFriends#GetFriendPersonaState
*/
extern "C" DLL_EXPORT int GetFriendPersonaState(int hUserSteamID);
/*
@desc Gets a relationship to a specified user.
@param hUserSteamID A user Steam ID handle.
@return An [EFriendRelationship](https://partner.steamgames.com/doc/api/ISteamFriends#EFriendRelationship) value.
@api ISteamFriends#GetFriendRelationship
*/
extern "C" DLL_EXPORT int GetFriendRelationship(int hUserSteamID);
/*
@desc Gets the Steam level of the specified user.

_This will return 0 unless the level has been cached by the API.  Despite how it appears that it should behave, the Steam API does not seem to fire the PersonaStateChange_t callback when Steam levels cache._
@param hUserSteamID A user Steam ID handle.
@return the user's Steam level or 0 if not loaded.
@api ISteamFriends#GetFriendSteamLevel
*/
extern "C" DLL_EXPORT int GetFriendSteamLevel(int hUserSteamID);
/*
@desc Gets the nickname that the current user has set for the specified user.
@param hUserSteamID A user Steam ID handle.
@return The player's nickname.
@api ISteamFriends#GetPlayerNickname
*/
extern "C" DLL_EXPORT char *GetPlayerNickname(int hUserSteamID);
/*
@desc Checks if the user meets the specified criteria.
@param hUserSteamID A user Steam ID handle.
@param friendFlags A combined union (binary "or") of one or more EFriendFlags.
@param-api friendFlags ISteamFriends#EFriendFlags
@return 1 when true, 0 when false.
@api ISteamFriends#HasFriend
*/
extern "C" DLL_EXPORT int HasFriend(int hUserSteamID, int friendFlags);
/* @page Avatars */
/*
@desc Gets the avatar image handle for the given steam id and avatar size.  Avatars come in three sizes.  The Steam API has a command for each size, but this plugin combines them into a single command for simplicity.

If the avatar has not been cached, the returned image handle will be -1.  HasAvatarImageLoaded will report when a user's avatar has loaded and GetFriendAvatar can be called again to retrieve it.

According to the Steamworks documentation:
> This only works for users that the local user knows about. They will automatically know about their friends, people on leaderboards they've requested, or people in the same source as them (Steam group, chat room, lobby, or game server). If they don't know about them then you must call RequestUserInformation to cache the avatar locally.

Values for the size parameter:
```
#constant AVATAR_SMALL	0 // 32x32
#constant AVATAR_MEDIUM	1 // 64x64
#constant AVATAR_LARGE	2 // 128x128
```
@param hUserSteamID A user Steam ID handle.
@param size The size of the avatar to load.
@return 0 when no avatar is set, -1 when the image needs to be loaded, and a positive number is an image handle.  Use [LoadImageFromHandle](Image-Loading#loadimagefromhandle) to load the image from the handle.
@api ISteamFriends#GetSmallFriendAvatar, ISteamFriends#GetMediumFriendAvatar, ISteamFriends#GetLargeFriendAvatar
*/
extern "C" DLL_EXPORT int GetFriendAvatar(int hUserSteamID, int size);
/*
@desc Indicates whether the AvatarImageLoaded_t [callback](Callbacks-and-Call-Results#callbacks) has stored data.
@return 1 when there is data stored; otherwise 0.
@api ISteamFriends#AvatarImageLoaded_t
*/
extern "C" DLL_EXPORT int HasAvatarImageLoaded();
/*
@desc Returns a handle for m_steamID stored from the AvatarImageLoaded_t callback.
Call GetFriendAvatar with the returned user handle to get the image handle

_HasAvatarImageLoaded must be called prior to this method._
@return The SteamID handle of the user whose avatar loaded.
@api ISteamFriends#AvatarImageLoaded_t
*/
extern "C" DLL_EXPORT int GetAvatarImageLoadedUser();
/* @page Friend List */
/*
@desc Returns an integer array as JSON that combines the GetFriendCount and GetFriendByIndex calls.
@param friendFlags A combined union (binary "or") of EFriendFlags.
@param-api friendFlags ISteamFriends#EFriendFlags
@return An integer array of Steam ID handles as a JSON string.
@api ISteamFriends#GetFriendByIndex
*/
extern "C" DLL_EXPORT char *GetFriendListJSON(int friendFlags);
/* @page Friends Groups */
/*
@desc Gets the number of friends groups (tags) the user has created.
@return The number of friends groups for the current user.
@api ISteamFriends#GetFriendsGroupCount
*/
extern "C" DLL_EXPORT int GetFriendsGroupCount();
/*
@desc Gets the friends group ID for the given index.
@param index An index between 0 and GetFriendsGroupCount.
@return The friends group Steam ID handle.
@api ISteamFriends#GetFriendsGroupIDByIndex
*/
extern "C" DLL_EXPORT int GetFriendsGroupIDByIndex(int index);
/*
@desc Gets the number of friends in a given friends group.
@param hFriendsGroupID The friends group ID handle to get the number of friends in.
@return The number of friends in a friends group.
@api ISteamFriends#GetFriendsGroupMembersCount
*/
extern "C" DLL_EXPORT int GetFriendsGroupMembersCount(int hFriendsGroupID);
/*
@desc Gets a JSON integer array of SteamID handles of the list of members in a group.
@param hFriendsGroupID The friends group ID handle to get the members list of.
@return An integer array of Steam ID handles as a JSON string.
*/
extern "C" DLL_EXPORT char *GetFriendsGroupMembersListJSON(int hFriendsGroupID);
/*
@desc Gets the name for the given friends group.
@param hFriendsGroupID The friends group ID handle to get the name of.
@return The name of the friends group.
@api ISteamFriends#GetFriendsGroupName
*/
extern "C" DLL_EXPORT char *GetFriendsGroupName(int hFriendsGroupID);
/* @page Image Loading */
/*
@desc Loads an image from an image handle into a new image ID. 
@param hImage The handle to the image that will be obtained.
@return An image ID containing the loaded image.
@api SteamUtils#GetImageSize, SteamUtils#GetImageRGBA
*/
extern "C" DLL_EXPORT int LoadImageFromHandle(int hImage);
/*
@desc Loads an image from an image handle into a new image ID.
@param imageID The image ID into which to load the image.
@param hImage The handle to the image that will be obtained.
@api SteamUtils#GetImageSize, SteamUtils#GetImageRGBA
*/
extern "C" DLL_EXPORT void LoadImageIDFromHandle(int imageID, int hImage);
/* @page Request and Store */
/*
@desc _[This command is initiates a callback.](Callbacks-and-Call-Results#callbacks)_

Sends a request for user stats to Steam.

_This command is called within Init so AGK code will likely never need to call this command explicitly._
@return 1 when sending the request succeeds; otherwise 0.  This is not an indication of whether user stats are initialized.  See StatsInitialized.
@api ISteamUserStats#RequestUserStats
*/
extern "C" DLL_EXPORT int RequestStats();
/*
@desc Returns the state of the RequestStats callback.
@return [A callback state](Callbacks-and-Call-Results#states)
*/
extern "C" DLL_EXPORT int GetRequestStatsCallbackState();
/*
@desc Checks to see whether user stats have been initialized after a RequestStats call.
@return 1 when users stats are initialized; otherwise 0.
*/
extern "C" DLL_EXPORT int StatsInitialized();
/*
@desc _[This command is initiates a callback.](Callbacks-and-Call-Results#callbacks)_

Stores user stats online.
@return 1 when sending the request to store user stats succeeds; otherwise 0.
@api ISteamUserStats#StoreStats
*/
extern "C" DLL_EXPORT int StoreStats();
/*
@desc _[This command is initiates a callback.](Callbacks-and-Call-Results#callbacks)_

Resets user stats and optionally all achievements (when bAchievementsToo is 1).  This command also triggers the StoreStats callback.
@param achievementsToo When 1 then achievements are also cleared.
@return 1 when sending the request to clear user stats succeeds; otherwise 0.
@api ISteamUserStats#ResetAllStats
*/
extern "C" DLL_EXPORT int ResetAllStats(int achievementsToo);
/*
@desc Returns the state of the StoreStats callback.
@return [A callback state.](Callbacks-and-Call-Results#states)
*/
extern "C" DLL_EXPORT int GetStoreStatsCallbackState();
/*
@desc Checks to see whether user stats have been stored online.
@return 1 when users stats have been stored online; otherwise 0.
*/
extern "C" DLL_EXPORT int StatsStored();
/*
@desc Checks to see whether achievements have been stored online.
StatsStored will also indicate success when this happens.
@return 1 when achievements have been stored online; otherwise 0.
*/
extern "C" DLL_EXPORT int AchievementStored();
/* @page Achievements
**User stats must first be requested and finish initializing before these commands can be used.**
*/
/*
@desc Gets the number of achievements for the app.

_This method is generally just for testing purposes since the app should already know what the achievements are._
@return The number of achievements.
@api ISteamUserStats#GetNumAchievements
*/
extern "C" DLL_EXPORT int GetNumAchievements();
/*
@desc Gets the achievement ID for the achievement index.

_This method is generally just for testing purposes since the app should already know the achievement IDs._
@param index Index of the achievement.
@return Gets the 'API name' for an achievement index between 0 and GetNumAchievements.
@api ISteamUserStats#GetAchievementName
*/
extern "C" DLL_EXPORT char *GetAchievementAPIName(int index);
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
@desc Gets whether the user has achieved this achievement.
@param name The 'API Name' of the achievement.
@return 1 when the user has achieved this achievement; otherwise 0.
@api ISteamUserStats#GetAchievement
*/
extern "C" DLL_EXPORT int GetAchievement(const char *name);
/*
@desc Gets the time at which an achievement was unlocked, if ever.
@param name The 'API Name' of the achievement.
@return The unload time in Unix time.
@api ISteamUserStats#GetAchievementAndUnlockTime
*/
extern "C" DLL_EXPORT int GetAchievementUnlockTime(const char *name);
/*
@desc Gives an achievement to the user.

Call StoreStats afterward to notify the user of the achievement.  Otherwise, they will be notified after the game exits.
@param name The 'API Name' of the achievement.
@return 1 when the call succeeds; otherwise 0.
@api ISteamUserStats#SetAchievement
*/
extern "C" DLL_EXPORT int SetAchievement(const char *name);
/*
@desc Raises a notification about achievemnt progress for progress stat achievements.
The notification only shows when current progress is less than the max.

SetStat still needs to be used to set the progress stat value.

Steamworks documentation suggests only using this at intervals in the progression rather than every step of the way.
ie: Every 25 wins out of 100.
@param name The 'API Name' of the achievement.
@param curProgress The current progress.
@param maxProgress The progress required to unlock the achievement.
@return 1 when the call succeeds; otherwise 0.
@api ISteamUserStats#IndicateAchievementProgress
*/
extern "C" DLL_EXPORT int IndicateAchievementProgress(const char *name, int curProgress, int maxProgress);
/*
@desc Removes an achievement from the user.
Call StoreStats afterward to upload the stats to the server.

_This method is generally just for testing purposes._
@param name The 'API Name' of the achievement.
@return int: 1 when the call succeeds; otherwise 0.
@api ISteamUserStats#ClearAchievement
*/
extern "C" DLL_EXPORT int ClearAchievement(const char *name);
/* @page Setters and Getters
**User stats must first be requested and finish initializing before these commands can be used.**
*/
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
/*
@desc Updates an AVGRATE stat with new values.  The value of these fields can be read with GetStatFloat.
@param name The 'API Name' of the stat.
@param countThisSession The value accumulation since the last call to this method.
@param sessionLength The amount of time in seconds since the last call to this method.
@return 1 if setting the stat succeeds; otherwise 0.
@api ISteamUserStats#UpdateAvgRateStat
*/
extern "C" DLL_EXPORT int UpdateAvgRateStat(const char *name, float countThisSession, float sessionLength);
/* @page Find Leaderboard
**User stats must first be requested and finish initializing before these commands can be used.**
*/
/*
FindLeaderboard
@desc Sends a request to find the handle for a leaderboard.
@param leaderboardName The name of the leaderboard to find.
@return A [call result handle](Callbacks-and-Call-Results#call-results) on success; otherwise 0.
@api ISteamUserStats#FindLeaderboard
*/
extern "C" DLL_EXPORT int FindLeaderboard(const char *leaderboardName);
/* @page Leaderboard Information
**In order to use a leaderboard, its handle must first be retrieved from the server.**
*/
/*
@desc Returns the leaderboard name.
@param hLeaderboard A leaderboard handle.
@return The leaderboard name.
@api ISteamUserStats#GetLeaderboardName
*/
extern "C" DLL_EXPORT char *GetLeaderboardName(int hLeaderboard);
/*
@desc Returns the total number of entries in a leaderboard.
@param hLeaderboard A leaderboard handle.
@return The number of entries in the leaderboard.
@api ISteamUserStats#GetLeaderboardEntryCount
*/
extern "C" DLL_EXPORT int GetLeaderboardEntryCount(int hLeaderboard);
/*
@desc Returns the display type of a leaderboard.
@param hLeaderboard A leaderboard handle.
@return An [ELeaderboardDisplayType](https://partner.steamgames.com/doc/api/ISteamUserStats#ELeaderboardDisplayType) value.
@api ISteamUserStats#GetLeaderboardDisplayType
*/
extern "C" DLL_EXPORT int GetLeaderboardDisplayType(int hLeaderboard);
/*
@desc Returns the sort order of a leaderboard.
@param hLeaderboard A leaderboard handle.
@return An [ELeaderboardSortMethod](https://partner.steamgames.com/doc/api/ISteamUserStats#ELeaderboardSortMethod) value.
@api ISteamUserStats#GetLeaderboardSortMethod
*/
extern "C" DLL_EXPORT int GetLeaderboardSortMethod(int hLeaderboard);
/* @page Uploading Scores
**In order to use a leaderboard, its handle must first be retrieved from the server.**
*/
/*
@desc Uploads a score to a leaderboard.  The leaderboard will keep the user's best score.
@param hLeaderboard A leaderboard handle.
@param score The score to upload.
@return A [call result handle](Callbacks-and-Call-Results#call-results) on success; otherwise 0.
@api ISteamUserStats#UploadLeaderboardScore, ISteamUserStats#ELeaderboardUploadScoreMethod
*/
extern "C" DLL_EXPORT int UploadLeaderboardScore(int hLeaderboard, int score);
/*
@desc Uploads a score to a leaderboard.  Forces the server to accept the uploaded score and replace any existing score.
@param hLeaderboard A leaderboard handle.
@param score The score to upload.
@return A [call result handle](Callbacks-and-Call-Results#call-results) on success; otherwise 0.
@api ISteamUserStats#UploadLeaderboardScore, ISteamUserStats#ELeaderboardUploadScoreMethod
*/
extern "C" DLL_EXPORT int UploadLeaderboardScoreForceUpdate(int hLeaderboard, int score);
/* @page Downloading Entries
**In order to use a leaderboard, its handle must first be retrieved from the server using FindLeaderboard.**
*/
/*
@desc Downloads entries from a leaderboard.
@param hLeaderboard A leaderboard handle.
@param eLeaderboardDataRequest The type of data request to make.
@param-api eLeaderboardDataRequest ISteamUserStats#ELeaderboardDataRequest
@param rangeStart The index to start downloading entries relative to eLeaderboardDataRequest.
@param rangeEnd The last index to retrieve entries for relative to eLeaderboardDataRequest.
@return A [call result handle](Callbacks-and-Call-Results#call-results) on success; otherwise 0.
@api ISteamUserStats#DownloadLeaderboardEntries
*/
extern "C" DLL_EXPORT int DownloadLeaderboardEntries(int hLeaderboard, int eLeaderboardDataRequest, int rangeStart, int rangeEnd);
/* @page Lobby List */
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
@desc Get a filtered list of relevant lobbies.
@return A [call result handle](Callbacks-and-Call-Results#call-results) on success; otherwise 0.
@api ISteamMatchmaking#RequestLobbyList, ISteamMatchmaking#LobbyMatchList_t
*/
extern "C" DLL_EXPORT int RequestLobbyList();
/* @page Creating, Joining, and Leaving Lobbies */
/*
@desc Creates a new matchmaking lobby.  The lobby is joined once it is created.
@param eLobbyType The type and visibility of this lobby. This can be changed later via SetLobbyType.
@param-api eLobbyType ISteamMatchmaking#ELobbyType
@param maxMembers The maximum number of players that can join this lobby. This can not be above 250.
@return A [call result handle](Callbacks-and-Call-Results#call-results) on success; otherwise 0.
@api ISteamMatchmaking#CreateLobby
*/
extern "C" DLL_EXPORT int CreateLobby(int eLobbyType, int maxMembers);
//extern "C" DLL_EXPORT int SetLinkedLobby(int hLobbySteamID, int hLobbyDependentSteamID);
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
@desc Updates what type of lobby this is.

_This can only be set by the owner of the lobby._
@param hLobbySteamID The Steam ID handle of the lobby
@param eLobbyType The new lobby type to that will be set.
@param-api eLobbyType ISteamMatchmaking#ELobbyType
@return 1 when the call succeeds; otherwise 0.
@api ISteamMatchmaking#SetLobbyType
*/
extern "C" DLL_EXPORT int SetLobbyType(int hLobbySteamID, int eLobbyType);
/*
@desc Joins an existing lobby.
@param hLobbySteamID The Steam ID handle of the lobby
@return A [call result handle](Callbacks-and-Call-Results#call-results) on success; otherwise 0.
@api ISteamMatchmaking#JoinLobby
*/
extern "C" DLL_EXPORT int JoinLobby(int hLobbySteamID);
/*
@desc Indicates whether the OnLobbyEnter callback has accumulated data to process.
@return 1 when the callback has data; otherwise 0.
@api ISteamMatchmaking#LobbyEnter_t
*/
extern "C" DLL_EXPORT int HasLobbyEnterResponse();
/*
@desc Gets current LobbyEnter_t information returned by the OnLobbyEnter callback as JSON data.

_HasLobbyEnterResponse must be called prior to this method._

JSON:
A result code as outlined at [LobbyEnter_t](https://partner.steamgames.com/doc/api/ISteamMatchmaking#LobbyEnter_t).
@return A lobby Steam ID handle or 0 if the LobbyEnter_t call result failed.
@api ISteamMatchmaking#LobbyEnter_t
*/
extern "C" DLL_EXPORT char *GetLobbyEnterResponseJSON();
/*
@desc Invite another user to the lobby.
@param hLobbySteamID The Steam ID handle of the lobby to invite the user to.
@param hInviteeSteamID The Steam ID handle of the person who will be invited.
@return 1 when the invitation was sent successfully; otherwise 0.
@api ISteamMatchmaking#InviteUserToLobby
*/
extern "C" DLL_EXPORT int InviteUserToLobby(int hLobbySteamID, int hInviteeSteamID);
/*
@desc Gets whether the user has accepted a request to join a game lobby.
@return 1 when the user has accepted a request to join a game lobby; otherwise 0.
@api ISteamFriends#GameLobbyJoinRequested_t
*/
extern "C" DLL_EXPORT int HasGameLobbyJoinRequest();
/*
@desc Gets the lobby Steam ID handle to which the user was invited.
@return A lobby Steam ID handle or 0.
*/
extern "C" DLL_EXPORT int GetGameLobbyJoinRequestedLobby();
/*
@desc Leave a lobby that the user is currently in; this will take effect immediately on the client side, other users in the lobby will be notified by a LobbyChatUpdate_t callback.
@param hLobbySteamID The Steam ID handle of the lobby to leave.
@api ISteamMatchmaking#LeaveLobby
*/
extern "C" DLL_EXPORT void LeaveLobby(int hLobbySteamID);
/* @page Lobby Game Server */
/*
@desc Gets the details of a game server set in a lobby.
@param hLobbySteamID The Steam ID of the lobby to get the game server information from.
@return A JSON string that parses to the following type.
```
Type GameServerInfo_t // aka LobbyGameCreated_t
	hLobby as integer
	IP as string
	Port as integer
	hGameServer as integer
EndType
```
@api ISteamMatchmaking#GetLobbyGameServer
*/
extern "C" DLL_EXPORT char *GetLobbyGameServerJSON(int hLobbySteamID);
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
extern "C" DLL_EXPORT int SetLobbyGameServer(int hLobbySteamID, const char *gameServerIP, int gameServerPort, int hGameServerSteamID); // Triggers a LobbyGameCreated_t callback.
/*
@desc Indicates that a lobby game was created since the last call.
@return 1 when a lobby game was created; otherwise 0.
@api ISteamMatchmaking#LobbyGameCreated_t
*/
extern "C" DLL_EXPORT int HasLobbyGameCreated();
/*
@desc Returns a JSON description of the newly-created lobby game.
_This can only be read once per lobby game creation!_
@return A JSON string that parses to the following type.
```
Type LobbyGameCreated_t
	hLobby as integer
	IP as string
	Port as integer
	hGameServer as integer
EndType
```
@api ISteamMatchmaking#GetLobbyGameCreated
*/
extern "C" DLL_EXPORT char *GetLobbyGameCreatedJSON();
/* @page Lobby Data */
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
@desc Sets a key/value pair in the lobby metadata.

_This can only be set by the owner of the lobby._
@param hLobbySteamID The Steam ID of the lobby to set the metadata for.
@param key The key to set the data for.
@param value The value to set.
@api ISteamMatchmaking#SetLobbyData
*/
extern "C" DLL_EXPORT void SetLobbyData(int hLobbySteamID, const char *key, const char *value);
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
@desc Refreshes all of the metadata for a lobby that you're not in right now.
@param hLobbySteamID The Steam ID of the lobby to refresh the metadata of.
@return 1 when the request for lobby data succeeds will be reported by the LobbyDataUpdate_t callback. 0 when the request fails.
@api ISteamMatchmaking#RequestLobbyData, ISteamMatchmaking#LobbyDataUpdate_t
*/
extern "C" DLL_EXPORT int RequestLobbyData(int hLobbySteamID);
/*
@desc Indicates whether the LobbyDataUpdate_t callback has stored lobby data update notifications.
@return 1 when lobby data has updated; otherwise 0.
@api ISteamMatchmaking#LobbyDataUpdate_t
*/
extern "C" DLL_EXPORT int HasLobbyDataUpdateResponse();
extern "C" DLL_EXPORT char *GetLobbyDataUpdateResponseJSON();
/*
@desc Gets per-user metadata from another player in the specified lobby.
@param hLobbySteamID The Steam ID handle of the lobby that the other player is in.
@param hUserSteamID The Steam ID handle of the player to get the metadata from.
@param key The key to get the value of.
@return The value for the given key or an empty string if the key, member, or lobby doesn't exist.
@api ISteamMatchmaking#GetLobbyMemberData
*/
extern "C" DLL_EXPORT char *GetLobbyMemberData(int hLobbySteamID, int hUserSteamID, const char *key);
/*
@desc Sets per-user metadata for the local user.

Each user in the lobby will be receive notification of the lobby data change via HasLobbyDataUpdated, and any new users joining will receive any existing data.
@param hLobbySteamID The Steam ID of the lobby to set our metadata in.
@param key The key to set the data for.
@param value The value to set.
@api ISteamMatchmaking#SetLobbyMemberData
*/
extern "C" DLL_EXPORT void SetLobbyMemberData(int hLobbySteamID, const char *key, const char *value);
/* @page Lobby Members and Status */
/*
@desc Returns the current lobby owner.
@param hLobbySteamID The Steam ID handle of the lobby to get the owner of.
@return A steam ID handle or 0.
@api ISteamMatchmaking#GetLobbyOwner
*/
extern "C" DLL_EXPORT int GetLobbyOwner(int hLobbySteamID);
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
@desc The current limit on the number of users who can join the lobby.
@param hLobbySteamID The Steam ID handle of the lobby to get the member limit of.
@return A positive integer or 0 if no limit.
@api ISteamMatchmaking#GetLobbyMemberLimit
*/
extern "C" DLL_EXPORT int GetLobbyMemberLimit(int hLobbySteamID);
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
@desc Gets the number of users in a lobby.
@param hLobbySteamID The Steam ID handle of the lobby to get the number of members of.
@return The number of members in the lobby, 0 if the current user has no data from the lobby.
@api ISteamMatchmaking#GetNumLobbyMembers
*/
extern "C" DLL_EXPORT int GetNumLobbyMembers(int hLobbySteamID);
/*
@desc Gets the Steam ID handle of the lobby member at the given index.
@param hLobbySteamID This MUST be the same lobby used in the previous call to GetNumLobbyMembers!
@param index An index between 0 and GetNumLobbyMembers.
@return A Steam ID handle.
@api ISteamMatchmaking#GetLobbyMemberByIndex
*/
extern "C" DLL_EXPORT int GetLobbyMemberByIndex(int hLobbySteamID, int index);
/*
@desc Indicates whether the LobbyChatUpdate_t callback has stored data.
A lobby chat room state has changed, this is usually sent when a user has joined or left the lobby.
@return 1 when there is data stored; otherwise 0.
@api ISteamMatchmaking#LobbyChatUpdate_t
*/
extern "C" DLL_EXPORT int HasLobbyChatUpdate();
/*
@desc The lobby in which the chat update occured.

_HasLobbyChatUpdate must be called prior to this method._
@return A Steam ID handle.
@api ISteamMatchmaking#LobbyChatUpdate_t
*/
extern "C" DLL_EXPORT int GetLobbyChatUpdateLobby();
/*
@desc The user whose status in the lobby has changed.

_HasLobbyChatUpdate must be called prior to this method._
@return A Steam ID handle.
@api ISteamMatchmaking#LobbyChatUpdate_t
*/
extern "C" DLL_EXPORT int GetLobbyChatUpdateUserChanged();
/*
@desc The new user state for the user whose status changed.

_HasLobbyChatUpdate must be called prior to this method._
@return [EChatMemberStateChange bit data](https://partner.steamgames.com/doc/api/ISteamMatchmaking#EChatMemberStateChange)
@api ISteamMatchmaking#LobbyChatUpdate_t
*/
extern "C" DLL_EXPORT int GetLobbyChatUpdateUserState();
/*
@desc Chat member who made the change.

_HasLobbyChatUpdate must be called prior to this method._
@return A Steam ID handle.
@api ISteamMatchmaking#LobbyChatUpdate_t
*/
extern "C" DLL_EXPORT int GetLobbyChatUpdateUserMakingChange();
/* @page Lobby Chat Messages */
/*
@desc A chat message for this lobby has been received.
Indicates whether the LobbyChatMsg_t callback has stored data.
@return 1 when there is data stored; otherwise 0.
@api ISteamMatchmaking#LobbyChatMsg_t
*/
extern "C" DLL_EXPORT int HasLobbyChatMessage();
/*
@desc Gets the Steam ID handle of the user who sent this message.

_HasLobbyChatMessage must be called prior to this method._
@return A Steam ID handle.
@api ISteamMatchmaking#GetLobbyChatEntry, ISteamMatchmaking#LobbyChatMsg_t
*/
extern "C" DLL_EXPORT int GetLobbyChatMessageUser();
/*
@desc Gets the chat message that was sent.

_HasLobbyChatMessage must be called prior to this method._
@return The contents of the chat message.
@api ISteamMatchmaking#GetLobbyChatEntry, ISteamMatchmaking#LobbyChatMsg_t
*/
extern "C" DLL_EXPORT char *GetLobbyChatMessageText();
/*
@desc Broadcasts a chat message to the all of the users in the lobby.
@param hLobbySteamID The Steam ID handle of the lobby to send the chat message to.
@param message The message to send.
@return 1 when the send succeeds; otherwise 0.
@api ISteamMatchmaking#SendLobbyChatMessage
*/
extern "C" DLL_EXPORT int SendLobbyChatMessage(int hLobbySteamID, const char *message);
/* @page Favorite Games */
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
/*
@desc Gets the number of favorite and recent game servers the user has stored locally.
@return An integer.
@api ISteamMatchmaking#GetFavoriteGameCount
*/
extern "C" DLL_EXPORT int GetFavoriteGameCount();
/*
@desc Gets the details of the favorite game server by index.
@param index The index of the favorite game server to get the details of. This must be between 0 and GetFavoriteGameCount
@return Returns a JSON string of the game server information that parses to the following type.
```
Type FavoriteGameInfo_t
	AppID as integer
	IPv4 as string
	ConnectPort as integer
	QueryPort as integer
	Flags as integer
	TimeLastPlayedOnServer as integer
EndType
```
@api ISteamMatchmaking#GetFavoriteGame
*/
extern "C" DLL_EXPORT char *GetFavoriteGameJSON(int index);
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
// Game server
//extern "C" DLL_EXPORT char *GetPublicIP();
/* @page Music */
/*
@desc Checks if Steam Music is enabled.
@return 1 if Steam Music is enabled; otherwise 0.
@api ISteamMusic#BIsEnabled
*/
extern "C" DLL_EXPORT int IsMusicEnabled();
/*
@desc Checks if Steam Music is active. This does not necessarily mean that a song is currently playing, it may be paused.
@return 1 if Steam Music is active; otherwise 0.
@api ISteamMusic#BIsPlaying
*/
extern "C" DLL_EXPORT int IsMusicPlaying();
/*
@desc Gets the current status of the Steam Music player.
@return [An AudioPlayback_Status value](https://partner.steamgames.com/doc/api/ISteamMusic#AudioPlayback_Status)
@api ISteamMusic#GetPlaybackStatus
*/
extern "C" DLL_EXPORT int GetMusicPlaybackStatus();
/*
@desc Gets the current volume of the Steam Music player.
@return The volume is returned as a percentage between 0.0 and 1.0.
@api ISteamMusic#GetVolume
*/
extern "C" DLL_EXPORT float GetMusicVolume();
/*
@desc Sets the volume of the Steam Music player.
@param volume The volume percentage to set from 0.0 to 1.0.
@api ISteamMusic#SetVolume
*/
extern "C" DLL_EXPORT void SetMusicVolume(float volume);
/*
@desc Pause the Steam Music player.
@api ISteamMusic#Pause
*/
extern "C" DLL_EXPORT void PauseMusic();
/*
@desc Have the Steam Music player resume playing.
@api ISteamMusic#Play
*/
extern "C" DLL_EXPORT void PlayMusic();
/*
@desc Have the Steam Music player skip to the next song.
@api ISteamMusic#PlayNext
*/
extern "C" DLL_EXPORT void PlayNextSong();
/*
@desc Have the Steam Music player play the previous song.
@api ISteamMusic#PlayPrevious
*/
extern "C" DLL_EXPORT void PlayPreviousSong();
/*
@desc Notifies the caller that the music playback status has changed since the last call.
@return 1 when a change has occurred; otherwise 0.
@api ISteamMusic#PlaybackStatusHasChanged_t
*/
extern "C" DLL_EXPORT int HasMusicPlaybackStatusChanged();
/*
@desc Notifies the caller that the music volume has changed since the last call.
@return 1 when a change has occurred; otherwise 0.
@api ISteamMusic#VolumeHasChanged_t
*/
extern "C" DLL_EXPORT int HasMusicVolumeChanged();
/* @page Utility Methods */
/*
@desc Gets the current amount of battery power on the computer.
@return The current batter power as a percentage, from 0 to 100, or 255 when on AC power.
@api ISteamUtils#GetCurrentBatteryPower
*/
extern "C" DLL_EXPORT int GetCurrentBatteryPower();
/*
@desc Tests when running on a laptop and there is less than 10 minutes of battery.  Fires every minute afterwards.
This method returns 1 once per warning.  It is not reported as an on going effect.
@return 1 when there is a low battery warning; otherwise 0.
@api ISteamUtils#LowBatteryPower_t
*/
extern "C" DLL_EXPORT int HasLowBatteryWarning();
/*
@desc HasLowBatteryWarning should be checked before calling this method.

Reports the estimate battery life in minutes when a low battery warning occurs.
@return Battery life in minutes.
@api ISteamUtils#LowBatteryPower_t
*/
extern "C" DLL_EXPORT int GetMinutesBatteryLeft();
/*
@desc Returns the number of Inter-Process Communication calls made since the last time this method was called.

Used for perf debugging so you can determine how many IPC (Inter-Process Communication) calls your game makes per frame
Every IPC call is at minimum a thread context switch if not a process one so you want to rate control how often you do them.
@return An integer.
@api ISteamUtils#GetIPCCallCount
*/
extern "C" DLL_EXPORT int GetIPCCallCount();
/*
@desc Returns the 2 character ISO 3166-1-alpha-2 format country code which client is running in.
@return A two character string.
@api ISteamUtils#GetIPCountry
*/
extern "C" DLL_EXPORT char *GetIPCountry();
/*
@desc Reports when the country of the user has changed.  Use GetIPCountry to get the new value.
@return 1 when the country has changed; otherwise 0.
@api ISteamUtils#IPCountry_t
*/
extern "C" DLL_EXPORT int HasIPCountryChanged();
/*
@desc Returns the number of seconds since the application was active.
@return An integer.
@api ISteamUtils#GetSecondsSinceAppActive
*/
extern "C" DLL_EXPORT int GetSecondsSinceAppActive();
/*
@desc Returns the number of seconds since the user last moved the mouse.
@return An integer
@api ISteamUtils#GetSecondsSinceComputerActive
*/
extern "C" DLL_EXPORT int GetSecondsSinceComputerActive();
/*
@desc Returns the Steam server time in Unix epoch format.
@return An integer.
@api ISteamUtils#GetServerRealTime
*/
extern "C" DLL_EXPORT int GetServerRealTime();
/*
@desc Returns the language the steam client is running in.

You probably want GetCurrentGameLanguage instead, this should only be used in very special cases.
@return A string.
@api ISteamUtils#GetSteamUILanguage
*/
extern "C" DLL_EXPORT char *GetSteamUILanguage();
/*
@desc Called when Steam wants to shutdown.
@return 1 when Steam is shutting down; otherwise 0.
@api ISteamUtils#SteamShutdown_t
*/
extern "C" DLL_EXPORT int IsSteamShuttingDown();
/*
@desc Sets a warning message hook within the plugin to receive Steam API warnings and info messages and output them to AGK's Debug Log.

_Note: I have never seen a warning show up and don't know how to force one to fire, so I can only assume the plugin code is correct._
@api ISteamUtils#SetWarningMessageHook
*/
extern "C" DLL_EXPORT void SetWarningMessageHook();
/* @page Big Picture Mode */
/*
@desc Checks if Steam and the Steam Overlay are running in Big Picture mode.
@return 1 if in Big Picture Mode; otherwise 0.
@api ISteamUtils#IsSteamInBigPictureMode
*/
extern "C" DLL_EXPORT int IsSteamInBigPictureMode();
/*
@desc Activates the Big Picture text input dialog which only supports gamepad input.

Note: charMax is limited to 512 characters.
@param eInputMode Selects the input mode to use, either Normal or Password (hidden text).
@param-api eInputMode ISteamUtils#EGamepadTextInputMode
@param eLineInputMode Controls whether to use single or multi line input.
@param-api eLineInputMode ISteamUtils#EGamepadTextInputLineMode
@param description Sets the description that should inform the user what the input dialog is for.
@param charMax The maximum number of characters that the user can input.
@param existingText Sets the preexisting text which the user can edit.
@return 1 if the input overlay opens; otherwise 0.
@api ISteamUtils#ShowGamepadTextInput
*/
extern "C" DLL_EXPORT int ShowGamepadTextInput(int eInputMode, int eLineInputMode, const char *description, int charMax, const char *existingText);
/*
@desc Returns 1 when the big picture gamepad text input has been closed.
@return 1 when the big picture gamepad text input has closed; otherwise 0.
@api ISteamUtils#GamepadTextInputDismissed_t
*/
extern "C" DLL_EXPORT int HasGamepadTextInputDismissedInfo();
/*
@desc Gets the result of big picture gamepad text input.

_HasGamepadTextInputDismissedInfo should be checked prior to calling this method._
@return A JSON string with this structure.  (See steam_constants.agc)
```
Type GamepadTextInputDismissedInfo_t
	Submitted as integer	// 1 when the user submitted text. 0 when the user cancels.
	Text as string
EndType
```
@api ISteamUtils#GamepadTextInputDismissed_t
*/
extern "C" DLL_EXPORT char *GetGamepadTextInputDismissedInfoJSON();
/* @page VR
**Note:**  
I don't own a VR device and don't have the SteamVR Tool installed.  I don't know if these commands are useful for those who do, but they are here for completeness.
*/
/*
@desc Checks if Steam is running in VR mode.
@return 1 when Steam is running in VR mode; otherwise 0.
@api ISteamUtils#IsSteamRunningInVR
*/
extern "C" DLL_EXPORT int IsSteamRunningInVR();
/*
@desc Asks Steam to create and render the OpenVR dashboard.
@api ISteamUtils#StartVRDashboard
*/
extern "C" DLL_EXPORT void StartVRDashboard();
/*
@desc Set whether the HMD content will be streamed via Steam In-Home Streaming.

See API reference for further information.
@param enabled Turns VR HMD Streaming on (1) or off (0).
@api ISteamUtils#SetVRHeadsetStreamingEnabled
*/
extern "C" DLL_EXPORT void SetVRHeadsetStreamingEnabled(int enabled);
/*
@desc Checks if the HMD view will be streamed via Steam In-Home Streaming.

See API reference for further information.
@return 1 if streaming is enabled; otherwise 0.
@api ISteamUtils#IsVRHeadsetStreamingEnabled
*/
extern "C" DLL_EXPORT int IsVRHeadsetStreamingEnabled();
/* @page Cloud Information */
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
/* @page Cloud File List */
/*
@desc Gets the total number of local files synchronized by Steam Cloud.
@return The number of files present for the current user, including files in subfolders.
@api ISteamRemoteStorage#GetFileCount
*/
extern "C" DLL_EXPORT int GetCloudFileCount();
/*
@desc Gets the file name at the given index
@param index The index of the file, this should be between 0 and GetFileCount.
@return
The name of the file at the given index.  An empty string if the file doesn't exist.
@api ISteamRemoteStorage#GetFileCount, ISteamRemoteStorage#GetFileNameAndSize
*/
extern "C" DLL_EXPORT char *GetCloudFileName(int index);
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
/* @page Cloud File Methods */
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
@desc
Checks if a specific file is persisted in the steam cloud.
@param filename The name of the file.
@return
1 if the file exists and the file is persisted in the Steam Cloud.
0 if FileForget was called on it and is only available locally.
@api ISteamRemoteStorage#FilePersisted
*/
extern "C" DLL_EXPORT int CloudFilePersisted(const char *filename);
/*
@desc
Opens a binary file, reads the contents of the file into a memblock, and then closes the file.

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

Check GetCloudFileReadAsyncCallbackState to see when the callback completes.
@param filename The name of the file to read from.
@param offset The offset in bytes into the file where the read will start from. 0 if you're reading the whole file in one chunk.
@param length The amount of bytes to read starting from the offset. -1 to read the entire file.
@return A [call result handle](Callbacks-and-Call-Results#call-results) on success; otherwise 0.
@api ISteamRemoteStorage#FileReadAsync, ISteamRemoteStorage#RemoteStorageFileReadAsyncComplete_t
*/
extern "C" DLL_EXPORT int CloudFileReadAsync(const char *filename, int offset, int length);
/*
@desc Returns the file name of the CloudFileReadAsync call result operation.
@param hCallResult A CloudFileReadAsync [call result handle](Callbacks-and-Call-Results#call-results).
@return The file name.
*/
extern "C" DLL_EXPORT char *GetCloudFileReadAsyncFileName(int hCallResult);
/*
@desc Returns the memblock of the data returned by the CloudFileReadAsync callback operation.

A call result will delete its memblock in DeleteCallResult() so calling code does not need to do so.
@param hCallResult A CloudFileReadAsync [call result handle](Callbacks-and-Call-Results#call-results).
@return A memblock ID.
*/
extern "C" DLL_EXPORT int GetCloudFileReadAsyncMemblock(int hCallResult);
//extern "C" DLL_EXPORT SteamAPICall_t CloudFileShare(const char *filename);
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

Check GetCloudFileWriteAsyncCallbackState to see when the callback completes.
@param filename The name of the file to write to.
@param memblockID The memblock containing the data to write to the file.
@return A [call result handle](Callbacks-and-Call-Results#call-results) on success; otherwise 0.
@api ISteamRemoteStorage#FileWriteAsync, ISteamRemoteStorage#RemoteStorageFileWriteAsyncComplete_t
*/
extern "C" DLL_EXPORT int CloudFileWriteAsync(const char *filename, int memblockID);
/*
@desc Returns the file name of the CloudFileWriteAsync call result operation.
@param hCallResult A CloudFileReadAsync [call result handle](Callbacks-and-Call-Results#call-results).
@return The file name.
*/
extern "C" DLL_EXPORT char *GetCloudFileWriteAsyncFileName(int hCallResult);

//extern "C" DLL_EXPORT bool CloudFileWriteStreamCancel(UGCFileWriteStreamHandle_t writeHandle);
//extern "C" DLL_EXPORT bool CloudFileWriteStreamClose(UGCFileWriteStreamHandle_t writeHandle);
//extern "C" DLL_EXPORT UGCFileWriteStreamHandle_t FileWriteStreamOpen(const char *filename);
//extern "C" DLL_EXPORT bool CloudFileWriteStreamWriteChunk(UGCFileWriteStreamHandle_t writeHandle, const void *pvData, int32 cubData);
/*
@desc Gets the specified files size in bytes.
@param filename 	The name of the file.
@return The size of the file in bytes. Returns 0 if the file does not exist.
@api ISteamRemoteStorage#GetFileSize
*/
extern "C" DLL_EXPORT int GetCloudFileSize(const char *filename);
/*
@desc
Gets the specified file's last modified timestamp in Unix epoch format (seconds since Jan 1st 1970).
@param filename The name of the file.
@return The last modified timestamp in Unix epoch format.
@api ISteamRemoteStorage#GetFileTimestamp
*/
extern "C" DLL_EXPORT int GetCloudFileTimestamp(const char *filename);
/*
@desc Obtains the platforms that the specified file will syncronize to.
@param filename The name of the file.
@return Bitfield containing the platforms that the file was set to with SetSyncPlatforms.
@api ISteamRemoteStorage#GetSyncPlatforms, ISteamRemoteStorage#ERemoteStoragePlatform
*/
extern "C" DLL_EXPORT int GetCloudFileSyncPlatforms(const char *filename);
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
/* @page User-Generated Content */
//extern "C" DLL_EXPORT int GetCachedUGCCount();
//extern "C" DLL_EXPORT UGCHandle_t GetCachedUGCHandle(int32 iCachedContent);
//extern "C" DLL_EXPORT bool GetUGCDetails(UGCHandle_t hContent, AppId_t *pnAppID, const char **pname, int32 *pnFileSizeInBytes, CSteamID *pSteamIDOwner);
//extern "C" DLL_EXPORT bool GetUGCDownloadProgress(UGCHandle_t hContent, int32 *pnBytesDownloaded, int32 *pnBytesExpected);
//extern "C" DLL_EXPORT SteamAPICall_t UGCDownload(UGCHandle_t hContent, uint32 unPriority);
//extern "C" DLL_EXPORT SteamAPICall_t UGCDownloadToLocation(UGCHandle_t hContent, const char *location, uint32 unPriority);
//extern "C" DLL_EXPORT int32 UGCRead(UGCHandle_t hContent, void *pvData, int32 cubDataToRead, uint32 cOffset, EUGCReadAction eAction);

//SteamAPICall_t GetFileDetails(const char*pszFileName); // FileDetailsResult_t call result.

/*
	@page Input Information
See the [Steam Input](https://partner.steamgames.com/doc/features/steam_input) documentation for more information.
[Steam Controller](https://partner.steamgames.com/doc/features/steam_controller) has been deprecated.

Until the Steam Input API documentation has been posted, this will still link to ISteamController.

See also [Getting Started for Developers](https://partner.steamgames.com/doc/features/steam_controller/getting_started_for_devs).
*/
/*
@desc Must be called when starting use of the ISteamInput interface.
@return Always 1 if the Steam plugin has been initialized; otherwise 0.
@api ISteamController#Init
*/
extern "C" DLL_EXPORT int InitSteamInput();
/*
@desc Must be called when ending use of the Steam Input interface.

This is called within Shutdown and also while unloading the plugin, so calling it explictly should not be necessary.
@return Always 1 if the Steam plugin has been initialized; otherwise 0.
@api ISteamController#Shutdown
*/
extern "C" DLL_EXPORT int ShutdownSteamInput();
/*
@desc Enumerates currently connected controllers.

Must be called before controllers can be used because it loads the internal input handles.
@return The number of inputs found.
@api ISteamController#GetConnectedControllers
*/
extern "C" DLL_EXPORT int GetConnectedControllers();
/*
@desc Returns the input type (device model) for the specified input. This tells you if a given input is a Steam controller, XBox 360 controller, PS4 controller, etc.
@param hInput The handle of the input.
@return Returns the input type (device model) for the specified input.
@api ISteamController#GetInputTypeForHandle
*/
extern "C" DLL_EXPORT int GetInputTypeForHandle(int hInput);
/*
@desc Returns the associated input handle for the specified emulated gamepad.
@param hInput The index of the emulated gamepad you want to get an input handle for.
@return The associated input handle for the specified emulated gamepad.
@api ISteamController#GetControllerForGamepadIndex
*/
//extern "C" DLL_EXPORT ControllerHandle_t GetControllerForGamepadIndex(int nIndex);
/*
@desc Returns the associated gamepad index for the specified input, if emulating a gamepad.
@param hInput The handle of the input you want to get a gamepad index for.
@return An integer.
@api ISteamController#GetGamepadIndexForController
*/
//extern "C" DLL_EXPORT int GetGamepadIndexForController(ControllerHandle_t ulController);
/*
@desc Synchronize API state with the latest Steam Input inputs available.
This is performed automatically by RunCallbacks, but for the absolute lowest possible latency, you can call this directly before reading input state.
@api ISteamController#RunFrame
*/
extern "C" DLL_EXPORT void RunFrame();
/*
@desc Invokes the Steam overlay and brings up the binding screen.
@param hInput The handle of the input you want to bring up the binding screen for.
@return 1 for success; 0 if overlay is disabled/unavailable, or the user is not in Big Picture Mode.
@api ISteamController#ShowBindingPanel
*/
extern "C" DLL_EXPORT int ShowBindingPanel(int hInput);
/*
	@page Input Action Sets and Layers
*/
/*
@desc Reconfigure the input to use the specified action set (ie "Menu", "Walk", or "Drive").

This is cheap, and can be safely called repeatedly. It's often easier to repeatedly call it in your state loops, instead of trying to place it in all of your state transitions.
@param hInput The handle of the input you want to activate an action set for.
@param hActionSet The handle of the action set you want to activate.
@api ISteamController#ActivateActionSet
*/
extern "C" DLL_EXPORT void ActivateActionSet(int hInput, int hActionSet);
/*
@desc Lookup the handle for an Action Set. Best to do this once on startup, and store the handles for all future API calls.
@param actionSetName The string identifier of an action set defined in the game's VDF file.
@return The handle of the specified action set.
@api ISteamController#GetActionSetHandle
*/
extern "C" DLL_EXPORT int GetActionSetHandle(const char *actionSetName);
/*
@desc Get the currently active action set for the specified input.
@param hInput The handle of the input you want to query.
@return The handle of the action set activated for the specified input.
@api ISteamController#GetCurrentActionSet
*/
extern "C" DLL_EXPORT int GetCurrentActionSet(int hInput);
/*
@desc Reconfigure the input to use the specified action set layer.
@param hInput The handle of the input you want to activate an action set layer for.
@param hActionSetLayer The handle of the action set layer you want to activate.
@api ISteamController#ActivateActionSetLayer
*/
extern "C" DLL_EXPORT void ActivateActionSetLayer(int hInput,  int hActionSetLayer);
/*
@desc Reconfigure the input to stop using the specified action set layer.
@param hInput The handle of the input you want to deactivate an action set layer for.
@param hActionSetLayer The handle of the action set layer you want to deactivate.
@api ISteamController#DeactivateActionSetLayer
*/
extern "C" DLL_EXPORT void DeactivateActionSetLayer(int hInput, int hActionSetLayer);
/*
@desc Reconfigure the input to stop using all action set layers.
@param hInput The handle of the input you want to deactivate all action set layers for.
@api ISteamController#DeactivateAllActionSetLayers
*/
extern "C" DLL_EXPORT void DeactivateAllActionSetLayers(int hInput);
/*
@desc Gets the active action set layers for the given input.
@param hInput The handle of the input you want to get active action set layers for.
@return A JSON integer array of active action set layers.
@api ISteamController#GetActiveActionSetLayers
*/
extern "C" DLL_EXPORT char *GetActiveActionSetLayersJSON(int hInput);
/*
	@page Input Actions and Motion
*/
/*
@desc Reads and stores the current state of the supplied analog game action so that it can be returned by GetAnalogActionDataActive, GetAnalogActionDataMode, GetAnalogActionDataX, and GetAnalogActionDataY.
@param hInput The handle of the input you want to query.
@param hAnalogAction The handle of the analog action you want to query.
@api ISteamController#GetAnalogActionData, ISteamController#ControllerAnalogActionData_t
*/
extern "C" DLL_EXPORT void GetAnalogActionData(int hInput, int hAnalogAction);
/*
@desc Returns the current availability to be bound in the active action set of the analog game action read by the last GetAnalogActionData call.

**NOTE:**
GetAnalogActionData MUST be called in order to populate the value returned by this method.
@return Whether or not this action is currently available to be bound in the active action set.
@api ISteamController#GetAnalogActionData, ISteamController#ControllerAnalogActionData_t

*/
extern "C" DLL_EXPORT int GetAnalogActionDataActive();
/*
@desc Returns the type of data coming from this action as read by the last GetAnalogActionData call

**NOTE:**
GetAnalogActionData MUST be called in order to populate the value returned by this method.
@return The type of data coming from this action, this will match what was specified in the action set's VDF definition.
@api ISteamController#GetAnalogActionData, ISteamController#ControllerAnalogActionData_t
*/
extern "C" DLL_EXPORT int GetAnalogActionDataMode();
/*
@desc Returns the current state of this action on the horizontal axis read by the last GetAnalogActionData call

**NOTE:**
GetAnalogActionData MUST be called in order to populate the value returned by this method.
@return The current state of this action on the horizontal axis.
@api ISteamController#GetAnalogActionData, ISteamController#ControllerAnalogActionData_t
*/
extern "C" DLL_EXPORT float GetAnalogActionDataX();
/*
@desc Returns the current state of this action on the vertical axis read by the last GetAnalogActionData call

**NOTE:**
GetAnalogActionData MUST be called in order to populate the value returned by this method.
@return The current state of this action on the vertical axis.
@api ISteamController#GetAnalogActionData, ISteamController#ControllerAnalogActionData_t
*/
extern "C" DLL_EXPORT float GetAnalogActionDataY();

/*
@desc Get the handle of the specified Analog action.

**NOTE:** This method does not take an action set handle parameter. That means that each action in your VDF file must have a unique string identifier.
In other words, if you use an action called "up" in two different action sets, this method will only ever return one of them and the other will be ignored.
@param actionName The string identifier of the analog action defined in the game's VDF file.
@return The analog action handle.
@api ISteamController#GetAnalogActionHandle
*/
extern "C" DLL_EXPORT int GetAnalogActionHandle(const char *actionName);
/*
@desc Stops the momentum of an analog action (where applicable, ie a touchpad w/ virtual trackball settings).
@param hInput The handle of the input to affect.
@param hAnalogAction The analog action handle to stop momentum for.
@api ISteamController#StopAnalogActionMomentum
*/
extern "C" DLL_EXPORT void StopAnalogActionMomentum(int hInput, int hAnalogAction);
/*
@desc Reads and stores the current state of the supplied digital game action so that it can be returned by GetDigitalActionDataActive and GetDigitalActionDataState.
@param hInput The handle of the input you want to query.
@param hDigitalAction The handle of the digital action you want to query.
@api ISteamController#GetDigitalActionData, ISteamController#ControllerDigitalActionData_t
*/
extern "C" DLL_EXPORT void GetDigitalActionData(int hInput, int hDigitalAction);
/*
@desc Returns the current availability to be bound in the active action set of the digital game action read by the last GetDigitalActionData call.

**NOTE:**  
GetDigitalActionData MUST be called in order to populate the value returned by this method.
@return Whether or not this action is currently available to be bound in the active action set.
@api ISteamController#GetDigitalActionData, ISteamController#ControllerDigitalActionData_t
*/
extern "C" DLL_EXPORT int GetDigitalActionDataActive();
/*
@desc Returns the current state of the digital game action read by the last GetDigitalActionData call.

**NOTE:**
GetDigitalActionData MUST be called in order to populate the value returned by this method.
@return The current state of this action; 1 if the action is currently pressed, otherwise 0.
@api ISteamController#GetDigitalActionData, ISteamController#ControllerDigitalActionData_t
*/
extern "C" DLL_EXPORT int GetDigitalActionDataState();
/*
@desc Get the handle of the specified digital action.

**NOTE:** This method does not take an action set handle parameter. That means that each action in your VDF file must have a unique string identifier.
In other words, if you use an action called "up" in two different action sets, this method will only ever return one of them and the other will be ignored.
@param actionName The string identifier of the digital action defined in the game's VDF file.
@return The handle of the specified digital action.
@api ISteamController#GetDigitalActionHandle
*/
extern "C" DLL_EXPORT int GetDigitalActionHandle(const char *actionName);
/*
@desc Reads and stores the raw motion data for the specified input so that it can be returned by 
GetMotionDataPosAccelX, GetMotionDataPosAccelY, GetMotionDataPosAccelZ,
GetMotionDataPosRotQuatW, GetMotionDataPosRotQuatX, GetMotionDataPosRotQuatY, GetMotionDataPosRotQuatZ,
GetMotionDataPosRotVelX, GetMotionDataPosRotVelY, and GetMotionDataPosRotVelZ.
@param hInput The handle of the input you want to get motion data for.
@api ISteamController#GetMotionData, ISteamController#ControllerMotionData_t
*/
extern "C" DLL_EXPORT void GetMotionData(int hInput);
/*
@desc Returns the positional acceleration, x axis of the controller motion data read by the last GetMotionData call.

**NOTE:**
GetMotionData MUST be called in order to populate the value returned by this method.
@return Positional acceleration, x axis.
@api ISteamController#GetDigitalActionData, ISteamController#ControllerDigitalActionData_t
*/
extern "C" DLL_EXPORT float GetMotionDataPosAccelX();
/*
@desc Returns the positional acceleration, y axis of the input motion data read by the last GetMotionData call.

**NOTE:**
GetMotionData MUST be called in order to populate the value returned by this method.
@return Positional acceleration, y axis.
@api ISteamController#GetDigitalActionData, ISteamController#ControllerDigitalActionData_t
*/
extern "C" DLL_EXPORT float GetMotionDataPosAccelY();
/*
@desc Returns the positional acceleration, z axis of the input motion data read by the last GetMotionData call.

**NOTE:**
GetMotionData MUST be called in order to populate the value returned by this method.
@return Positional acceleration, z axis.
@api ISteamController#GetDigitalActionData, ISteamController#ControllerDigitalActionData_t
*/
extern "C" DLL_EXPORT float GetMotionDataPosAccelZ();
/*
@desc Returns the sensor-fused absolute rotation (will drift in heading), w axis of the input motion data read by the last GetMotionData call.

**NOTE:**
GetMotionData MUST be called in order to populate the value returned by this method.
@return Sensor-fused absolute rotation (will drift in heading), w axis.
@api ISteamController#GetDigitalActionData, ISteamController#ControllerDigitalActionData_t
*/
extern "C" DLL_EXPORT float GetMotionDataRotQuatW();
/*
@desc Returns the sensor-fused absolute rotation (will drift in heading), x axis of the input motion data read by the last GetMotionData call.

**NOTE:**
GetMotionData MUST be called in order to populate the value returned by this method.
@return Sensor-fused absolute rotation (will drift in heading), x axis.
@api ISteamController#GetDigitalActionData, ISteamController#ControllerDigitalActionData_t
*/
extern "C" DLL_EXPORT float GetMotionDataRotQuatX();
/*
@desc Returns the sensor-fused absolute rotation (will drift in heading), y axis of the input motion data read by the last GetMotionData call.

**NOTE:**
GetMotionData MUST be called in order to populate the value returned by this method.
@return Sensor-fused absolute rotation (will drift in heading), y axis.
@api ISteamController#GetDigitalActionData, ISteamController#ControllerDigitalActionData_t
*/
extern "C" DLL_EXPORT float GetMotionDataRotQuatY();
/*
@desc Returns the sensor-fused absolute rotation (will drift in heading), z axis of the input motion data read by the last GetMotionData call.

**NOTE:**
GetMotionData MUST be called in order to populate the value returned by this method.
@return Sensor-fused absolute rotation (will drift in heading), z axis.
@api ISteamController#GetDigitalActionData, ISteamController#ControllerDigitalActionData_t
*/
extern "C" DLL_EXPORT float GetMotionDataRotQuatZ();
/*
@desc Returns the angular velocity, x axis of the input motion data read by the last GetMotionData call.

**NOTE:**
GetMotionData MUST be called in order to populate the value returned by this method.
@return Angular velocity, x axis.
@api ISteamController#GetDigitalActionData, ISteamController#ControllerDigitalActionData_t
*/
extern "C" DLL_EXPORT float GetMotionDataRotVelX();
/*
@desc Returns the angular velocity, y axis of the input motion data read by the last GetMotionData call.

**NOTE:**
GetMotionData MUST be called in order to populate the value returned by this method.
@return Angular velocity, y axis.
@api ISteamController#GetDigitalActionData, ISteamController#ControllerDigitalActionData_t
*/
extern "C" DLL_EXPORT float GetMotionDataRotVelY();
/*
@desc Returns the angular velocity, z axis of the input motion data read by the last GetMotionData call.

**NOTE:**
GetMotionData MUST be called in order to populate the value returned by this method.
@return Angular velocity, z axis.
@api ISteamController#GetDigitalActionData, ISteamController#ControllerDigitalActionData_t
*/
extern "C" DLL_EXPORT float GetMotionDataRotVelZ();
/*
	@page Input Action Origins
*/
/*
@desc Get the origin(s) for an analog action within an action set by filling originsOut with EInputActionOrigin handles.
Use this to display the appropriate on-screen prompt for the action.
@param hInput The handle of the input you want to query.
@param hActionSet The handle of the action set you want to query.
@param hAnalogAction The handle of the analog action you want to query.
@return The number of origins supplied in originsOut.
@api ISteamController#GetAnalogActionOrigins
*/
extern "C" DLL_EXPORT char *GetAnalogActionOriginsJSON(int hInput, int hActionSet, int hAnalogAction);
/*
@desc Get a JSON integer array the origin(s) for a digital action within an action set.
Use this to display the appropriate on-screen prompt for the action.
@param hInput The handle of the input you want to query.
@param hActionSet The handle of the action set you want to query.
@param hDigitalAction The handle of the digital aciton you want to query.
@return A JSON integer array of the action origins.
@api ISteamController#GetDigitalActionOrigins
*/
extern "C" DLL_EXPORT char *GetDigitalActionOriginsJSON(int hInput, int hActionSet, int hDigitalAction);
/*
@desc Get a local path to art for on-screen glyph for a particular origin.
@param eOrigin The origin you want to get the glyph for.
@return The path to the png file for the glyph.
@api ISteamController#GetGlyphForActionOrigin, ISteamController#EControllerActionOrigin
*/
extern "C" DLL_EXPORT char * GetGlyphForActionOrigin(int eOrigin);
/*
@desc Returns a localized string (from Steam's language setting) for the specified origin.
@param eOrigin The origin you want to get the string for.
@return The localized string for the specified origin.
@api ISteamController#GetStringForActionOrigin, ISteamController#EControllerActionOrigin
*/
extern "C" DLL_EXPORT char * GetStringForActionOrigin(int eOrigin);
/*
@desc Get the equivalent ActionOrigin for a given Xbox controller origin.
This can be chained with GetGlyphForActionOrigin to provide future proof glyphs for non-Steam Input API action games.

Note - this only translates the buttons directly and doesn't take into account any remapping a user has made in their configuration
@param hInput The handle of the input you want to query.
@param eOrigin The Xbox controller origin.
@param-api eOrigin ISteamInput#EXboxOrigin
@return The ActionOrigin for a given Xbox controller origin.
@api ISteamInput#GetActionOriginFromXboxOrigin, ISteamInput#EInputActionOrigin
*/
extern "C" DLL_EXPORT int GetActionOriginFromXboxOrigin(int hInput, int eOrigin);
/*
@desc Returns a localized string (from Steam's language setting) for the specified Xbox controller origin.
@param eOrigin The Xbox controller origin.
@param-api eOrigin ISteamInput#EXboxOrigin
@return The localized string for the specified Xbox controller origin.
@api ISteamInput#GetStringForXboxOrigin
*/
extern "C" DLL_EXPORT char *GetStringForXboxOrigin(int eOrigin);
/*
@desc Returns the local path to art for the on-screen glyph for a particular Xbox controller origin.
@param eOrigin The Xbox controller origin.
@param-api eOrigin ISteamInput#EXboxOrigin
@return The path to art for the on-screen glyph for a particular Xbox controller origin.
@api ISteamInput#GetGlyphForXboxOrigin
*/
extern "C" DLL_EXPORT char *GetGlyphForXboxOrigin(int eOrigin);
/*
@desc Convert an origin to another controller type.
For inputs not present on the other controller type this will return k_EInputActionOrigin_None.

When a new input type is added you will be able to pass in k_ESteamInputType_Unknown amd the closest 
origin that your version of the SDK regonized will be returned
ex: if a Playstation 5 controller was released, this method would return Playstation 4 origins.
@param eDestinationInputType The input type to convert from.
@param-api eDestinationInputType ISteamInput#ESteamInputType
@param eSourceOrigin The action origin to convert from.
@param-api eSourceOrigin ISteamInput#EInputActionOrigin
@return The action origin for the destination input type.
@api ISteamInput#TranslateActionOrigin, ISteamInput#EInputActionOrigin
*/
extern "C" DLL_EXPORT int TranslateActionOrigin(int eDestinationInputType, int eSourceOrigin);
/*
	@page Input Effects
*/
/*
@desc Set the input LED color on supported inputs.
@param hInput The handle of the input to affect.
@param red The red component of the color to set (0-255).
@param green The green component of the color to set (0-255).
@param blue The blue component of the color to set (0-255).
@api ISteamController#SetLEDColor, ISteamController#ESteamControllerLEDFlag
*/
extern "C" DLL_EXPORT void SetInputLEDColor(int hInput, int red, int green, int blue);
/*
@desc Set the input LED color back to the default (out-of-game) settings.
@param hInput The handle of the input to affect.
@api ISteamController#SetLEDColor, ISteamController#ESteamControllerLEDFlag
*/
extern "C" DLL_EXPORT void ResetInputLEDColor(int hInput);
/*
@desc Triggers a (low-level) haptic pulse on supported inputs.

**NOTES**  
Currently only the VSC supports haptic pulses.  This API call will be ignored for all other input models.
@param hInput The handle of the input to affect.
@param eTargetPad Which haptic touch pad to affect.
@param-api eTargetPad ISteamController#ESteamControllerPad
@param duration Duration of the pulse, in microseconds (1/1,000,000th of a second)
@api ISteamController#TriggerHapticPulse
*/
extern "C" DLL_EXPORT void TriggerInputHapticPulse(int hInput, int eTargetPad, int duration);
/*
@desc Triggers a repeated haptic pulse on supported inputs.

**NOTES**  
Currently only the VSC supports haptic pulses.  
This API call will be ignored for incompatible input models.  
This is a more user-friendly function to call than TriggerHapticPulse as it can generate pulse patterns long enough to be actually noticed by the user.  
@param hInput The handle of the input to affect.
@param eTargetPad Which haptic touch pad to affect.
@param-api eTargetPad ISteamController#ESteamControllerPad
@param onDuration Duration of the pulse, in microseconds (1/1,000,000th of a second).
@param offDuration Duration of the pause between pulses, in microseconds.
@param repeat Number of times to repeat the onDuration / offDuration duty cycle.
@api ISteamController#TriggerRepeatedHapticPulse
*/
extern "C" DLL_EXPORT void TriggerInputRepeatedHapticPulse(int hInput, int eTargetPad, int onDuration, int offDuration, int repeat);
/*
@desc Trigger a vibration event on supported inputs.

This API call will be ignored for incompatible input models.
@param hInput The handle of the input to affect.
@param leftSpeed The period of the left rumble motor's vibration, in microseconds.
@param rightSpeed The period of the right rumble motor's vibration, in microseconds.
@api ISteamController#TriggerVibration
*/
extern "C" DLL_EXPORT void TriggerInputVibration(int hInput, int leftSpeed, int rightSpeed);

#endif // _DLLMAIN_H_