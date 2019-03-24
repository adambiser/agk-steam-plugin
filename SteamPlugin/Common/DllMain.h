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

#pragma region Additional Methods
/* @page Additional Methods

These commands are not part of the Steamworks SDK but are included as a convenience.
*/
/*
@desc Returns the command line arguments.
@return The command line arguments as a JSON string of an array of strings.
*/
extern "C" DLL_EXPORT char *GetCommandLineArgsJSON();

/*
@desc Returns the path of the folder containing the Steam client EXE as found in the registry
by reading the value of SteamPath in HKEY_CURRENT_USER\Software\Valve\Steam.
The path may contain slashes instead of backslashes and the trailing slash may or may not be included.
@return The path at which the Steam client is installed.
*/
extern "C" DLL_EXPORT char *GetSteamPath();

/*
@desc Attempts to detect when Steam is being emulated.
Emulation is sometimes used with pirated games, but it can also be used for valid reasons.
@return 1 when Steam emulation is detected, otherwise 0.
*/
extern "C" DLL_EXPORT int IsSteamEmulated();

#undef SetFileAttributes
/*
@desc Sets the attributes of a file.
This is only included to help with development because the AppGameKit IDE deletes Steam files in the project folder when the interpreter exits.
@param filename The name of the file whose attributes are to be set.
@param attributes The file attributes to set for the file.
@return 1 if successful; otherwise, 0.
*/
extern "C" DLL_EXPORT int SetFileAttributes(const char *filename, int attributes);
#pragma endregion

#pragma region CallResult Methods
/* @page CallResult Methods

Some Steamworks methods are asynchronous commands that return call result handles.

Use GetCallResultCode to check the status of a call result and DeleteCallResult once the call result has been processed.
*/
/*
@desc Deletes a [call result](Callbacks-and-Call-Results#call-results) and its data.
@param hCallResult The call result handle to delete.
*/
extern "C" DLL_EXPORT void DeleteCallResult(int hCallResult);

/*
@desc Returns the result code of the given [call result](Callbacks-and-Call-Results#call-results).
@param hCallResult The call result handle to check.
@return An EResult code.
@api steam_api#EResult
*/
extern "C" DLL_EXPORT int GetCallResultCode(int hCallResult);
#pragma endregion

#pragma region steam_api.h
/* @page steam_api.h */
/*
@desc
Initializes the Steam API.  This method should be called before attempting to do anything else with this plugin.
This also calls RequestCurrentStats() internally so calling code does not need to do so. The result of the RequestCurrentStats call has no effect on the return value.
@return 1 when Steam API initialization succeeds; otherwise 0.
@api steam_api#SteamAPI_Init
*/
extern "C" DLL_EXPORT int Init();

/*
@desc Returns whether the Steam API is currently initialized.
@return 1 when the Steam API is initialized; otherwise 0.
*/
extern "C" DLL_EXPORT int IsSteamInitialized();

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
Allows asynchronous Steam API calls to handle any new results.
Should be called each frame sync.
@api steam_api#SteamAPI_RunCallbacks
*/
extern "C" DLL_EXPORT void RunCallbacks();

/*
@desc Shuts down the plugin and frees memory.
@api steam_api#SteamAPI_Shutdown
*/
extern "C" DLL_EXPORT void Shutdown();
#pragma endregion

#pragma region steam_gameserver.h
// TODO
#pragma endregion

#pragma region ISteamApps
/* @page ISteamApps */
/*
@desc Checks if a specific app is installed.
This only works for base applications, not DLC. Use IsDLCInstalled for DLC instead.
@param appID The App ID of the application to check.
@return 1 if the app is installed; otherwise 0.
@api ISteamApps#BIsAppInstalled
*/
extern "C" DLL_EXPORT int IsAppInstalled(int appID);

/*
@desc Checks whether the current App ID is for Cyber Cafes.
@return 1 if a cyber cafe; otherwise 0.
@api ISteamApps#BIsCybercafe*/
extern "C" DLL_EXPORT int IsCybercafe();

/*
@desc Checks if the user owns a specific DLC and if the DLC is installed
@param appID The App ID of the DLC to check.
@return 1 when the user owns a DLC and it is installed; otherwise 0.
@api ISteamApps#BIsDlcInstalled
*/
extern "C" DLL_EXPORT int IsDLCInstalled(int appID);

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
@desc Checks if the user has a VAC ban on their account.
@return 1 when the user has a VAC ban; otherwise 0.
@api ISteamApps#BIsVACBanned
*/
extern "C" DLL_EXPORT int IsVACBanned();

/*
@desc Gets the buildid of this app, may change at any time based on backend updates to the game.
@return The App BuildID if installed; otherwise 0.
@api ISteamApps#GetAppBuildId
*/
extern "C" DLL_EXPORT int GetAppBuildID();

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
@desc Gets a comma separated list of the languages the current app supports.
@return List of languages, separated by commas.
@api ISteamApps#GetAvailableGameLanguages
*/
extern "C" DLL_EXPORT char *GetAvailableGameLanguages();

/*
@desc Checks if the user is running from a beta branch, and gets the name of the branch if they are.
@return The name of the beta branch running; otherwise an empty string if not a beta branch or the app is not installed
@api ISteamApps#GetCurrentBetaName
*/
extern "C" DLL_EXPORT char *GetCurrentBetaName();

/*
@desc Gets the current language that the user has set.
@return The current language if the app is installed; otherwise an empty string.
@api ISteamApps#GetCurrentGameLanguage
*/
extern "C" DLL_EXPORT char *GetCurrentGameLanguage();

/*
@desc Gets the number of DLC pieces for the current app.
@return The number of DLC pieces for the current app.
@api ISteamApps#GetDLCCount
*/
extern "C" DLL_EXPORT int GetDLCCount();

/*
@desc Returns the App ID of a DLC by index.
@param index The index.  Should be between 0 and GetDLCCount() - 1.
@return The AppID if the index is valid; otherwise 0.
@api ISteamApps#GetDLCCount, ISteamApps#BGetDLCDataByIndex
*/
extern "C" DLL_EXPORT int GetDLCDataByIndexAppID(int index);

/*
@desc Returns whether the DLC at the given index is available.
@param index The index.  Should be between 0 and GetDLCCount() - 1.
@return 1 if available; otherwise 0.
@api ISteamApps#GetDLCCount, ISteamApps#BGetDLCDataByIndex
*/
extern "C" DLL_EXPORT int GetDLCDataByIndexAvailable(int index);

/*
@desc Returns the name of the DLC at the given index.
@param index The index.  Should be between 0 and GetDLCCount() - 1.
@return The DLC name if the index is valid; otherwise and empty string.
@api ISteamApps#GetDLCCount, ISteamApps#BGetDLCDataByIndex
*/
extern "C" DLL_EXPORT char *GetDLCDataByIndexName(int index);

/*
@desc Gets the bytes downloaded of the download progress for DLC.
@param appID The App ID of the DLC to monitor.
@return A JSON string of the download progress.
@api ISteamApps#GetDlcDownloadProgress
*/
extern "C" DLL_EXPORT int GetDLCDownloadProgressBytesDownloaded(int appID);
/*
@desc Gets the bytes total of the download progress for DLC.
@param appID The App ID of the DLC to monitor.
@return A JSON string of the download progress.
@api ISteamApps#GetDlcDownloadProgress
*/
extern "C" DLL_EXPORT int GetDLCDownloadProgressBytesTotal(int appID);

/*
@desc Gets the time of purchase of the specified app in Unix epoch format (time since Jan 1st, 1970).
@param appID The App ID to get the purchase time for.
@return A Unix epoch time.
@api ISteamApps#GetEarliestPurchaseUnixTime
*/
extern "C" DLL_EXPORT int GetEarliestPurchaseUnixTime(int appID);

/*
@desc Asynchronously retrieves metadata details about a specific file in the depot manifest.
@param filename The filename in the current depot.
@callback-type callresult
@callback-getters GetFileDetailsSHA1, GetFileDetailsSize
@return A[call result handle](Callbacks - and-Call - Results#call - results) on success; otherwise 0.
@api ISteamApps#GetFileDetails
*/
extern "C" DLL_EXPORT int GetFileDetails(const char *filename);

/*
@desc Returns the SHA1 hash for the file.
@param hCallResult A GetFileDetails call result handle.
@return A 40 character hexidecimal string or an empty string if there was an error.
@api ISteamApps#GetFileDetails, ISteamApps#FileDetailsResult_t
*/
extern "C" DLL_EXPORT char *GetFileDetailsSHA1(int hCallResult);

/*
@desc Returns the size of the file.
@param hCallResult A GetFileDetails call result handle.
@return The file size.
@api ISteamApps#GetFileDetails, ISteamApps#FileDetailsResult_t
*/
extern "C" DLL_EXPORT int GetFileDetailsSize(int hCallResult);

///*
//@desc Returns a flag value that is not explained in the Steamworks SDK documentation.
//@param hCallResult A GetFileDetails call result handle.
//@return A flag value.
//@api ISteamApps#GetFileDetails, ISteamApps#FileDetailsResult_t
//*/
//extern "C" DLL_EXPORT int GetFileDetailsFlags(int hCallResult);

/*
@desc Gets a list of all installed depots for a given App ID in mount order.
@param appID The App to list the depots for.
@param maxDepots The maximum number of depots to obtain.
@return A JSON string of an integer array.
*/
extern "C" DLL_EXPORT char *GetInstalledDepotsJSON(int appID, int maxDepots);

/*
@desc Gets the command line if game was launched via Steam URL, e.g. steam://run/<appid>//<command line>/.
@return The command line if launched via Steam URL.
@api ISteamApps#GetLaunchCommandLine
*/
extern "C" DLL_EXPORT char *GetLaunchCommandLine();

/*
@desc Gets the associated launch parameter if the game is run via steam://run/<appid>/?param1=value1;param2=value2;param3=value3 etc.
@param key The launch key to test for. Ex: param1
@return The key value or an empty string if the key doesn't exist.
@api ISteamApps#GetLaunchQueryParam
*/
extern "C" DLL_EXPORT char *GetLaunchQueryParam(const char *key);

/*
@desc Install a DLC.  HasDLCInstalledResponse will report true when a DLC has finished installing.
@param appID The App ID of the DLC you want to install.
@api ISteamApps#InstallDLC
*/
extern "C" DLL_EXPORT void InstallDLC(int appID);

/*
@desc Triggered after the current user gains ownership of DLC and that DLC is installed.
@callback-type list
@callback-getters	GetDLCInstalledAppID
@return 1 when the callback has more responses to process; otherwise 0.
@api ISteamApps#DlcInstalled_t
*/
extern "C" DLL_EXPORT int HasDLCInstalledResponse();

/*
@desc Returns the App ID for the current LobbyGameCreated_t callback response.
@return An App ID.
@api ISteamApps#DlcInstalled_t
*/
extern "C" DLL_EXPORT int GetDLCInstalledAppID();

/*
@desc Allows you to force verify game content on next launch.

If you detect the game is out-of-date (for example, by having the client detect a version mismatch with a server),
you can call use MarkContentCorrupt to force a verify, show a message to the user, and then quit.
@param missingFilesOnly Only scan for missing files, don't verify the checksum of each file.
@return A boolean as 0 or 1.  Steam API doesn't indicate what this means.
@api ISteamApps#MarkContentCorrupt
*/
extern "C" DLL_EXPORT int MarkContentCorrupt(int missingFilesOnly);

/*
@desc Uninstall a DLC.

_Note: This appears to take place after exiting the app._
@param appID The App ID of the DLC you want to uninstall.
@api ISteamApps#UninstallDLC
*/
extern "C" DLL_EXPORT void UninstallDLC(int appID);

/*
@desc Posted after the user executes a steam url with query parameters while running.
Once a post has been reported, this method returns 0 until another post arrives.
@callback-type bool
@return 1 when the user executes a steam url with query parameters while running; otherwise 0.
@api ISteamApps#NewLaunchQueryParameters_t
*/
extern "C" DLL_EXPORT int HasNewUrlLaunchParametersResponse();
#pragma endregion

#pragma region ISteamAppsList
/* @page ISteamAppsList
This is a restricted interface that can only be used by approved apps.
*/
/*
@desc Returns the app name for the given appid.
@param appID The AppID to get the name for.
@return The app name or an empty string if the app has not been approved for this method.
@api ISteamAppList#GetAppName
*/
extern "C" DLL_EXPORT char *GetAppName(int appID);
#pragma endregion

#pragma region ISteamAppTicket
// TODO?
#pragma endregion

#pragma region ISteamClient
// TODO?
#pragma endregion

#pragma region ISteamController/ISteamInput
/* @page ISteamInput
See the [Steam Input](https://partner.steamgames.com/doc/features/steam_input) documentation for more information.
[Steam Controller](https://partner.steamgames.com/doc/features/steam_controller) has been deprecated.

Until the Steam Input API documentation has been posted, this will still link to ISteamController.

See also [Getting Started for Developers](https://partner.steamgames.com/doc/features/steam_controller/getting_started_for_devs).
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
@desc Reconfigure the input to use the specified action set layer.
@param hInput The handle of the input you want to activate an action set layer for.
@param hActionSetLayer The handle of the action set layer you want to activate.
@api ISteamController#ActivateActionSetLayer
*/
extern "C" DLL_EXPORT void ActivateActionSetLayer(int hInput, int hActionSetLayer);

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
@return The number of active action set layers.
@api ISteamController#GetActiveActionSetLayers
*/
extern "C" DLL_EXPORT int GetActiveActionSetLayerCount(int hInput);

/*
@desc Gets the handle for an active action set layer for the given input by index.
@param hInput The handle of the input you want to get active action set layers for.
@param index The index to get.
@return The active action set layer handle.
@api ISteamController#GetActiveActionSetLayers
*/
extern "C" DLL_EXPORT int GetActiveActionSetLayerHandle(int hInput, int index);

/*
@desc Lookup the handle for an Action Set. Best to do this once on startup, and store the handles for all future API calls.
@param actionSetName The string identifier of an action set defined in the game's VDF file.
@return The handle of the specified action set.
@api ISteamController#GetActionSetHandle
*/
extern "C" DLL_EXPORT int GetActionSetHandle(const char *actionSetName);

/*
@desc Queries the analog action data for an input.
Also returns whether or not this action is currently available to be bound in the active action set (the same as GetAnalogActionDataActive).
@param hInput The handle of the input you want to query.
@param hAnalogAction The handle of the analog action you want to query.
@return 1 if available; otherwise 0.
@api ISteamController#GetAnalogActionData, ISteamController#ControllerAnalogActionData_t
*/
extern "C" DLL_EXPORT int GetAnalogActionData(int hInput, int hAnalogAction);

/*
@desc Returns whether or not this action is currently available to be bound in the active action set as of the last GetAnalogActionData call.
@return 1 if available; otherwise 0.
@api ISteamController#ControllerAnalogActionData_t
*/
extern "C" DLL_EXPORT int GetAnalogActionDataActive();

/*
@desc Returns the type of data coming from this action as of the last GetAnalogActionData call
@return The type of data coming from this action, this will match what was specified in the action set's VDF definition.
@api ISteamController#ControllerAnalogActionData_t, ISteamController#EControllerSourceMode
*/
extern "C" DLL_EXPORT int GetAnalogActionDataMode();

/*
@desc Returns the state of this action on the horizontal axis as of the last GetAnalogActionData call
@return The current state of this action on the horizontal axis.
@api ISteamController#ControllerAnalogActionData_t
*/
extern "C" DLL_EXPORT float GetAnalogActionDataX();

/*
@desc Returns the state of this action on the vertical axis as of the last GetAnalogActionData call
@return The current state of this action on the vertical axis.
@api ISteamController#ControllerAnalogActionData_t
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
@desc Get the number of origins for an analog action within an action set.
@param hInput The handle of the input you want to query.
@param hActionSet The handle of the action set you want to query.
@param hAnalogAction The handle of the analog action you want to query.
@return The number of origins.
@api ISteamController#GetAnalogActionOrigins
*/
extern "C" DLL_EXPORT int GetAnalogActionOriginCount(int hInput, int hActionSet, int hAnalogAction);
/*
@desc Get the origin for an analog action within an action set by index.
Use this to display the appropriate on-screen prompt for the action.
@param hInput The handle of the input you want to query.
@param hActionSet The handle of the action set you want to query.
@param hAnalogAction The handle of the analog action you want to query.
@param index The index of the origin.
@return The origin.
@api ISteamController#GetAnalogActionOrigins, ISteamController#EControllerActionOrigin
*/
extern "C" DLL_EXPORT int GetAnalogActionOriginValue(int hInput, int hActionSet, int hAnalogAction, int index);

/*
@desc Enumerates currently connected controllers.

Must be called before controllers can be used because it loads the internal input handles.
@return The number of inputs found.
@api ISteamController#GetConnectedControllers
*/
extern "C" DLL_EXPORT int GetConnectedControllers();

/*
@desc Returns the associated input handle for the specified emulated gamepad.
@param hInput The index of the emulated gamepad you want to get an input handle for.
@return The associated input handle for the specified emulated gamepad.
@api ISteamController#GetControllerForGamepadIndex
*/
//extern "C" DLL_EXPORT ControllerHandle_t GetControllerForGamepadIndex(int nIndex);

/*
@desc Get the currently active action set for the specified input.
@param hInput The handle of the input you want to query.
@return The handle of the action set activated for the specified input.
@api ISteamController#GetCurrentActionSet
*/
extern "C" DLL_EXPORT int GetCurrentActionSet(int hInput);

/*
@desc Queries the digital action data for an input.
Also returns whether or not this action is currently available to be bound in the active action set (the same as GetDigitalActionDataActive).
@param hInput The handle of the input you want to query.
@param hDigitalAction The handle of the digital action you want to query.
@return 1 if this action is currently available; otherwise 0.
@api ISteamController#GetDigitalActionData, ISteamController#ControllerDigitalActionData_t
*/
extern "C" DLL_EXPORT int GetDigitalActionData(int hInput, int hDigitalAction);
/*
@desc Returns whether or not this action is currently available to be bound in the active action set.
@return 1 if this action is currently available; otherwise 0.
@api ISteamController#GetDigitalActionData, ISteamController#ControllerDigitalActionData_t
*/
extern "C" DLL_EXPORT int GetDigitalActionDataActive();

/*
@desc Returns the current state of the digital game action read by the last GetDigitalActionData call.
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
@desc Get the number of origins for a digital action within an action set.
@param hInput The handle of the input you want to query.
@param hActionSet The handle of the action set you want to query.
@param hDigitalAction The handle of the digital aciton you want to query.
@return A JSON integer array of the action origins.
@api ISteamController#GetDigitalActionOrigins
ISteamController#EControllerActionOrigin
*/
extern "C" DLL_EXPORT int GetDigitalActionOriginCount(int hInput, int hActionSet, int hDigitalAction);

/*
@desc Get the origin for a digital action within an action set by index.
Use this to display the appropriate on-screen prompt for the action.
@param hInput The handle of the input you want to query.
@param hActionSet The handle of the action set you want to query.
@param hDigitalAction The handle of the digital aciton you want to query.
@param index The index of the origin.
@return A JSON integer array of the action origins.
@api ISteamController#GetDigitalActionOrigins, ISteamController#EControllerActionOrigin
*/
extern "C" DLL_EXPORT int GetDigitalActionOriginValue(int hInput, int hActionSet, int hDigitalAction, int index);

/*
@desc Returns the associated gamepad index for the specified input, if emulating a gamepad.
@param hInput The handle of the input you want to get a gamepad index for.
@return An integer.
@api ISteamController#GetGamepadIndexForController
*/
//extern "C" DLL_EXPORT int GetGamepadIndexForController(ControllerHandle_t ulController);

/*
@desc Get a local path to art for on-screen glyph for a particular origin.
@param eOrigin The origin you want to get the glyph for.
@param-api eOrigin ISteamController#EControllerActionOrigin
@return The path to the png file for the glyph.
@api ISteamController#GetGlyphForActionOrigin
*/
extern "C" DLL_EXPORT char * GetGlyphForActionOrigin(int eOrigin);

/*
@desc Returns the input type (device model) for the specified input. This tells you if a given input is a Steam controller, XBox 360 controller, PS4 controller, etc.
@param hInput The handle of the input.
@return Returns the input type (device model) for the specified input.
@api ISteamController#GetInputTypeForHandle
*/
extern "C" DLL_EXPORT int GetInputTypeForHandle(int hInput);

/*
@desc Reads the raw motion data for the specified input;
@param hInput The handle of the input you want to get motion data for.
@api ISteamController#GetDigitalActionData, ISteamController#ControllerDigitalActionData_t
*/
extern "C" DLL_EXPORT void GetMotionData(int hInput);
/*
@desc Returns the positional acceleration, x axis for the last GetMotionData call.
@return Positional acceleration, x axis.
@api ISteamController#ControllerDigitalActionData_t
*/
extern "C" DLL_EXPORT float GetMotionDataPosAccelX();

/*
@desc Returns the positional acceleration, y axis for the last GetMotionData call
@return Positional acceleration, y axis.
@api ISteamController#ControllerDigitalActionData_t
*/
extern "C" DLL_EXPORT float GetMotionDataPosAccelY();

/*
@desc Returns the positional acceleration, z axis for the last GetMotionData call
@return Positional acceleration, z axis.
@api ISteamController#ControllerDigitalActionData_t
*/
extern "C" DLL_EXPORT float GetMotionDataPosAccelZ();

/*
@desc Returns the sensor-fused absolute rotation (will drift in heading), w axis for the last GetMotionData call
GetMotionData MUST be called in order to populate the value returned by this method.
@return Sensor-fused absolute rotation (will drift in heading), w axis.
@api ISteamController#ControllerDigitalActionData_t
*/
extern "C" DLL_EXPORT float GetMotionDataRotQuatW();

/*
@desc Returns the sensor-fused absolute rotation (will drift in heading), x axis for the last GetMotionData call
@return Sensor-fused absolute rotation (will drift in heading), x axis.
@api ISteamController#ControllerDigitalActionData_t
*/
extern "C" DLL_EXPORT float GetMotionDataRotQuatX();

/*
@desc Returns the sensor-fused absolute rotation (will drift in heading), y axis for the last GetMotionData call
@return Sensor-fused absolute rotation (will drift in heading), y axis.
@api ISteamController#ControllerDigitalActionData_t
*/
extern "C" DLL_EXPORT float GetMotionDataRotQuatY();

/*
@desc Returns the sensor-fused absolute rotation (will drift in heading), z axis for the last GetMotionData call
@return Sensor-fused absolute rotation (will drift in heading), z axis.
@api ISteamController#ControllerDigitalActionData_t
*/
extern "C" DLL_EXPORT float GetMotionDataRotQuatZ();

/*
@desc Returns the angular velocity, x axis for the last GetMotionData call
@return Angular velocity, x axis.
@api ISteamController#ControllerDigitalActionData_t
*/
extern "C" DLL_EXPORT float GetMotionDataRotVelX();

/*
@desc Returns the angular velocity, y axis for the last GetMotionData call
@return Angular velocity, y axis.
@api ISteamController#ControllerDigitalActionData_t
*/
extern "C" DLL_EXPORT float GetMotionDataRotVelY();

/*
@desc Returns the angular velocity, z axis for the last GetMotionData call
@return Angular velocity, z axis.
@api ISteamController#ControllerDigitalActionData_t
*/
extern "C" DLL_EXPORT float GetMotionDataRotVelZ();

/*
@desc Returns a localized string (from Steam's language setting) for the specified origin.
@param eOrigin The origin you want to get the string for.
@return The localized string for the specified origin.
@api ISteamController#GetStringForActionOrigin, ISteamController#EControllerActionOrigin
*/
extern "C" DLL_EXPORT char *GetStringForActionOrigin(int eOrigin);

/*
@desc Must be called when starting use of the ISteamInput interface.
@return Always 1 if the Steam plugin has been initialized; otherwise 0.
@api ISteamController#Init
*/
extern "C" DLL_EXPORT int InitSteamInput();

/*
@desc Synchronize API state with the latest Steam Input inputs available.
This is performed automatically by RunCallbacks, but for the absolute lowest possible latency, you can call this directly before reading input state.
@api ISteamController#RunFrame
*/
extern "C" DLL_EXPORT void RunFrame();

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
@desc Invokes the Steam overlay and brings up the binding screen.
@param hInput The handle of the input you want to bring up the binding screen for.
@return 1 for success; 0 if overlay is disabled/unavailable, or the user is not in Big Picture Mode.
@api ISteamController#ShowBindingPanel
*/
extern "C" DLL_EXPORT int ShowBindingPanel(int hInput);

/*
@desc Must be called when ending use of the Steam Input interface.

This is called within Shutdown and also while unloading the plugin, so calling it explictly should not be necessary.
@return Always 1 if the Steam plugin has been initialized; otherwise 0.
@api ISteamController#Shutdown
*/
extern "C" DLL_EXPORT int ShutdownSteamInput();

/*
@desc Stops the momentum of an analog action (where applicable, ie a touchpad w/ virtual trackball settings).
@param hInput The handle of the input to affect.
@param hAnalogAction The analog action handle to stop momentum for.
@api ISteamController#StopAnalogActionMomentum
*/
extern "C" DLL_EXPORT void StopAnalogActionMomentum(int hInput, int hAnalogAction);

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
@param eDestinationInputType The input type to convert from.
@param-api eDestinationInputType ISteamInput#ESteamInputType
@param eSourceOrigin The action origin to convert from.
@param-api eSourceOrigin ISteamInput#EInputActionOrigin
@return The action origin for the destination input type.
@api ISteamInput#TranslateActionOrigin, ISteamInput#EInputActionOrigin
*/
extern "C" DLL_EXPORT int TranslateActionOrigin(int eDestinationInputType, int eSourceOrigin);
#pragma endregion

#pragma region ISteamFriends
/* @page ISteamFriends */
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

//ClearRichPresence
//CloseClanChatWindowInSteam
//DownloadClanActivityCounts
//EnumerateFollowingList
//GetChatMemberByIndex
//GetClanActivityCounts
//GetClanByIndex
//GetClanChatMemberCount
//GetClanChatMessage
//GetClanCount
//GetClanName
//GetClanOfficerByIndex
//GetClanOfficerCount
//GetClanOwner
//GetClanTag
//GetCoplayFriend
//GetCoplayFriendCount
//GetFollowerCount
//GetFriendByIndex
//GetFriendCoplayGame
//GetFriendCoplayTime

/*
@desc Gets the number of users the client knows about who meet a specified criteria.
@param friendFlags A combined union (binary "or") of one or more EFriendFlags.
@param-api friendFlags ISteamFriends#EFriendFlags
@return The number of friends matching the given criteria.
@api ISteamFriends#GetFriendCount
*/
extern "C" DLL_EXPORT int GetFriendCount(int friendFlags);

/*
@desc Returns an integer array as JSON that combines the GetFriendCount and GetFriendByIndex calls.
@param friendFlags A combined union (binary "or") of EFriendFlags.
@param-api friendFlags ISteamFriends#EFriendFlags
@return An integer array of Steam ID handles as a JSON string.
@api ISteamFriends#GetFriendCount, ISteamFriends#GetFriendByIndex
*/
extern "C" DLL_EXPORT char *GetFriendListJSON(int friendFlags);

//GetFriendCountFromSource
//GetFriendFromSourceByIndex

/*
@desc Returns the game App ID that the friend is playing.
@param hUserSteamID A user Steam ID handle.
@return The game App ID if the friend is playing a game; otherwise 0.
@api ISteamFriends#GetFriendGamePlayed, ISteamFriends#FriendGameInfo_t
*/
extern "C" DLL_EXPORT int GetFriendGamePlayedGameAppID(int hUserSteamID);

/*
@desc Returns the IP of the server the friend is playing on.
@param hUserSteamID A user Steam ID handle.
@return The IP address if the friend is playing a game; otherwise an empty string.
@api ISteamFriends#GetFriendGamePlayed, ISteamFriends#FriendGameInfo_t
*/
extern "C" DLL_EXPORT char *GetFriendGamePlayedIP(int hUserSteamID);

/*
@desc Returns the game port of the server the friend is playing on.
@param hUserSteamID A user Steam ID handle.
@return The game port if the friend is playing a game; otherwise 0.
@api ISteamFriends#GetFriendGamePlayed, ISteamFriends#FriendGameInfo_t
*/
extern "C" DLL_EXPORT int GetFriendGamePlayedConnectionPort(int hUserSteamID);

/*
@desc Returns the query port of the server the friend is playing on.
@param hUserSteamID A user Steam ID handle.
@return The query port if the friend is playing a game; otherwise 0.
@api ISteamFriends#GetFriendGamePlayed, ISteamFriends#FriendGameInfo_t
*/
extern "C" DLL_EXPORT int GetFriendGamePlayedQueryPort(int hUserSteamID);

/*
@desc Returns the Steam ID handle of lobby the friend is in.
@param hUserSteamID A user Steam ID handle.
@return A Steam ID lobby handle if the friend is in a lobby; otherwise 0.
@api ISteamFriends#GetFriendGamePlayed, ISteamFriends#FriendGameInfo_t
*/
extern "C" DLL_EXPORT int GetFriendGamePlayedLobby(int hUserSteamID);

//GetFriendMessage

/*
@desc Gets the specified user's persona (display) name.
@param hUserSteamID A user Steam ID handle.
@return the user's name.
@api ISteamFriends#GetFriendPersonaName
*/
extern "C" DLL_EXPORT char *GetFriendPersonaName(int hUserSteamID);

//GetFriendPersonaNameHistory

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

//GetFriendRichPresence
//GetFriendRichPresenceKeyByIndex
//GetFriendRichPresenceKeyCount

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

/*
@desc Gets the Steam level of the specified user.

_This will return 0 unless the level has been cached by the API.  Despite how it appears that it should behave, the Steam API does not seem to fire the PersonaStateChange_t callback when Steam levels cache._
@param hUserSteamID A user Steam ID handle.
@return the user's Steam level or 0 if not loaded.
@api ISteamFriends#GetFriendSteamLevel
*/
extern "C" DLL_EXPORT int GetFriendSteamLevel(int hUserSteamID);

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
@desc Gets the current users persona (display) name.
@return the name
@api ISteamFriends#GetPersonaName
*/
extern "C" DLL_EXPORT char *GetPersonaName();

//GetPersonaState

/*
@desc Gets the nickname that the current user has set for the specified user.
@param hUserSteamID A user Steam ID handle.
@return The player's nickname.
@api ISteamFriends#GetPlayerNickname
*/
extern "C" DLL_EXPORT char *GetPlayerNickname(int hUserSteamID);

//GetPersonaState

/*
@desc Checks if the user meets the specified criteria.
@param hUserSteamID A user Steam ID handle.
@param friendFlags A combined union (binary "or") of one or more EFriendFlags.
@param-api friendFlags ISteamFriends#EFriendFlags
@return 1 when true, 0 when false.
@api ISteamFriends#HasFriend
*/
extern "C" DLL_EXPORT int HasFriend(int hUserSteamID, int friendFlags);

//InviteUserToGame
//IsClanChatAdmin
//IsClanPublic
//IsClanOfficialGameGroup
//IsClanChatWindowOpenInSteam
//IsFollowing
//IsUserInSource
//JoinClanChatRoom
//LeaveClanChatRoom
//OpenClanChatWindowInSteam
//ReplyToFriendMessage
//RequestClanOfficerList
//RequestFriendRichPresence

/*
@desc Requests the persona name and optionally the avatar of a specified user (when requireNameOnly is 0).
@param hUserSteamID A user Steam ID handle.
@param requireNameOnly When 0 the avatar for the user is also loaded; otherwise only the name is loaded.
@return 1 when the user data needs to be loaded and will be reported by the PersonaStateChange_t callback.  0 when the user data is already loaded.
@api ISteamFriends#RequestUserInformation
*/
extern "C" DLL_EXPORT int RequestUserInformation(int hUserSteamID, int requireNameOnly);

//SendClanChatMessage
//SetInGameVoiceSpeaking
//SetListenForFriendsMessages
//SetPersonaName
//SetPlayedWith
//SetRichPresence

//Callbacks
/*
@desc Triggered when an avatar is loaded if you have tried requesting it when it was unavailable.
@callback-type list
@callback-getters	GetAvatarImageLoadedUser
@return 1 when the callback has more responses to process; otherwise 0.
@api ISteamFriends#AvatarImageLoaded_t
*/
extern "C" DLL_EXPORT int HasAvatarImageLoadedResponse();

/*
@desc Returns the Steam ID handle for current AvatarImageLoaded_t callback response.
Call GetFriendAvatar with the returned user handle to get the image handle
@return The SteamID handle of the user whose avatar loaded.
@api ISteamFriends#AvatarImageLoaded_t
*/
extern "C" DLL_EXPORT int GetAvatarImageLoadedUser();

//ClanOfficerListResponse_t
//DownloadClanActivityCountsResult_t
//FriendRichPresenceUpdate_t
//FriendsEnumerateFollowingList_t
//FriendsGetFollowerCount_t
//FriendsIsFollowing_t
//GameConnectedChatJoin_t
//GameConnectedChatLeave_t
//GameConnectedClanChatMsg_t
//GameConnectedFriendChatMsg_t

/*
@desc Triggered when the user tries to join a lobby from their friends list or from an invite.
The game client should attempt to connect to specified lobby when this is received.
If the game isn't running yet then the game will be automatically launched with the command line parameter +connect_lobby <64-bit lobby Steam ID> instead.
@callback-type list
@callback-getters GetGameLobbyJoinRequestedLobby
@return 1 when the callback has more responses to process; otherwise 0.
@api ISteamFriends#GameLobbyJoinRequested_t
*/
extern "C" DLL_EXPORT int HasGameLobbyJoinRequestedResponse();

/*
@desc Gets the lobby to which the user was invited for the current GameLobbyJoinRequested_t callback response.
@return A lobby Steam ID handle or 0.
*/
extern "C" DLL_EXPORT int GetGameLobbyJoinRequestedLobby();

/*
@desc Gets whether the Steam Overlay has been activated or deactivated.
@return 1 when the Steam overlay is active and 0 when it is not.
@api ISteamFriends#GameOverlayActivated_t
*/
extern "C" DLL_EXPORT int IsGameOverlayActive();

//GameRichPresenceJoinRequested_t
//GameServerChangeRequested_t
//JoinClanChatRoomCompletionResult_t

/*
@desc Triggered whenever a friend's status changes.
@callback-type list
@callback-getters	GetPersonaStateChangeFlags, GetPersonaStateChangeSteamID
@return 1 when the callback has more responses to process; otherwise 0.
@api ISteamFriends#PersonaStateChange_t
*/
extern "C" DLL_EXPORT int HasPersonaStateChangeResponse();

/*
@desc Returns the bit-wise union of EPersonaChange values for the current PersonaStateChange_t callback response.
@return The EPersonaChange values of the user whose persona state changed.
@api ISteamFriends#PersonaStateChange_t, ISteamFriends#EPersonaChange
*/
extern "C" DLL_EXPORT int GetPersonaStateChangeFlags();

/*
@desc Returns the SteamID handle of the user whose persona state changed for the current PersonaStateChange_t callback response.
@return The SteamID handle of the user whose persona state changed.
@api ISteamFriends#PersonaStateChange_t
*/
extern "C" DLL_EXPORT int GetPersonaStateChangeSteamID();

//SetPersonaNameResponse_t
#pragma endregion

#pragma region ISteamGameCoordinator
// deprecated
#pragma endregion

#pragma region ISteamGameServer
// TODO? 
#pragma endregion

#pragma region ISteamGameServerStats
// TODO?
#pragma endregion

#pragma region ISteamHTMLSurface
// TODO?
#pragma endregion

#pragma region ISteamHTTP
// TODO?
#pragma endregion

#pragma region ISteamInventory
// TODO?
#pragma endregion

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
@param hUserSteamID The Steam ID handle of the player to get the metadata from.
@param key The key to get the value of.
@return The value for the given key or an empty string if the key, member, or lobby doesn't exist.
@api ISteamMatchmaking#GetLobbyMemberData
*/
extern "C" DLL_EXPORT char *GetLobbyMemberData(int hLobbySteamID, int hUserSteamID, const char *key);

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

#pragma region ISteamMatchmakingServers
// TODO
#pragma endregion

#pragma region ISteamMusic
/* @page ISteamMusic */
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
@desc Sets the volume of the Steam Music player.
@param volume The volume percentage to set from 0.0 to 1.0.
@api ISteamMusic#SetVolume
*/
extern "C" DLL_EXPORT void SetMusicVolume(float volume);

// Callbacks
/*
@desc Notifies the caller that the music playback status has changed since the last call.
@callback-type bool
@return 1 when a change has occurred; otherwise 0.
@api ISteamMusic#PlaybackStatusHasChanged_t
*/
extern "C" DLL_EXPORT int HasMusicPlaybackStatusChangedResponse();

/*
@desc Notifies the caller that the music volume has changed since the last call.
@callback-type bool
@return 1 when a change has occurred; otherwise 0.
@api ISteamMusic#VolumeHasChanged_t
*/
extern "C" DLL_EXPORT int HasMusicVolumeChangedResponse();
#pragma endregion

#pragma region ISteamMusicRemote
// TODO?
#pragma endregion

#pragma region ISteamNetworking
// TODO?
#pragma endregion

#pragma region ISteamNetworkingSockets
// TODO?
#pragma endregion

#pragma region ISteamNetworkingUtils
// TODO?
#pragma endregion

#pragma region ISteamParties
// TODO?
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
*/
extern "C" DLL_EXPORT char *GetCloudFileReadAsyncFileName(int hCallResult);

/*
@desc Returns the memblock of the data returned by the CloudFileReadAsync call.

A call result will delete its memblock in DeleteCallResult() so calling code does not need to do so.
@param hCallResult A CloudFileReadAsync call result handle.
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

//extern "C" DLL_EXPORT int GetCachedUGCCount();

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

//GetPublishedFileDetails
//GetPublishedItemVoteDetails

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

//GetUGCDetails
//GetUGCDownloadProgress
//GetUserPublishedItemVoteDetails

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

//PublishVideo
//PublishWorkshopFile
//ResetFileRequestState

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

//SetUserPublishedFileAction
//SubscribePublishedFile
//SynchronizeToClient
//SynchronizeToServer
//UGCDownload
//UGCDownloadToLocation
//UGCRead
//UnsubscribePublishedFile
//UpdatePublishedFileDescription
//UpdatePublishedFileFile
//UpdatePublishedFilePreviewFile
//UpdatePublishedFileSetChangeDescription
//UpdatePublishedFileTags
//UpdatePublishedFileTitle
//UpdatePublishedFileVisibility
//UpdateUserPublishedItemVote
#pragma endregion

#pragma region ISteamScreenshots
// TODO?
#pragma endregion

#pragma region ISteamUGC
// TODO?
//AddAppDependencyResult_t - Call result for AddAppDependency
//AddUGCDependencyResult_t - Call result for AddDependency
//CreateItemResult_t - Call result for CreateItem
//DownloadItemResult_t - Call result for DownloadItem
//GetAppDependenciesResult_t - Call result for GetAppDependencies
//DeleteItemResult_t - Call result for DeleteItem
//GetUserItemVoteResult_t - Call result for GetUserItemVote
//ItemInstalled_t - Callback.  Check m_unAppID for this game's appid!
//RemoveAppDependencyResult_t - Call result for RemoveAppDependency.
//RemoveUGCDependencyResult_t - Call result for RemoveDependency
//SetUserItemVoteResult_t - Call result for SetUserItemVote
//StartPlaytimeTrackingResult_t - Call result for StartPlaytimeTracking
//SteamUGCQueryCompleted_t - Call result for SendQueryUGCRequest
//StopPlaytimeTrackingResult_t - Call result for StopPlaytimeTracking, StopPlaytimeTrackingForAllItems
//SubmitItemUpdateResult_t - Call result for SubmitItemUpdate
//UserFavoriteItemsListChanged_t - Call result for AddItemToFavorites, RemoveItemFromFavorites
#pragma endregion

#pragma region ISteamUnifiedMessages
// Deprecated
#pragma endregion

#pragma region ISteamUser
/* @page ISteamUser */
//AdvertiseGame
//BeginAuthSession

/*
@desc Checks if the current users looks like they are behind a NAT device.
@return 1 if the current user is behind a NAT, otherwise 0.
@api ISteamUser#BIsBehindNAT
*/
extern "C" DLL_EXPORT int IsBehindNAT();

/*
@desc Checks whether the user's phone number is used to uniquely identify them.
@return 1 f the current user's phone uniquely verifies their identity; otherwise, 0.
@api ISteamUser#BIsPhoneIdentifying
*/
extern "C" DLL_EXPORT int IsPhoneIdentifying();

/*
@desc Checks whether the current user's phone number is awaiting (re)verification.
@return 1 if the it is requiring verification; otherwise, 0.
@api ISteamUser#BIsPhoneRequiringVerification
*/
extern "C" DLL_EXPORT int IsPhoneRequiringVerification();

/*
@desc Checks whether the current user has verified their phone number.
@return 1 if the current user has phone verification enabled; otherwise, 0.
@api ISteamUser#BIsTwoFactorEnabled
*/
extern "C" DLL_EXPORT int IsPhoneVerified();

/*
@desc Checks whether the current user has Steam Guard two factor authentication enabled on their account.
@return 1 if the current user has two factor authentication enabled; otherwise, 0.
@api ISteamUser#IsPhoneVerified
*/
extern "C" DLL_EXPORT int IsTwoFactorEnabled();

/*
@desc Checks to see whether the user is logged on to Steam.
@return 1 when the user is logged on; otherwise 0.
@api ISteamUser#BLoggedOn
*/
extern "C" DLL_EXPORT int LoggedOn();

//CancelAuthTicket
//DecompressVoice
//EndAuthSession
//GetAuthSessionTicket
//GetAvailableVoice
//GetEncryptedAppTicket

/*
@desc Gets the level of the users Steam badge for your game.
The user can have two different badges for a series; the regular badge (max level 5) and the foil badge (max level 1).
@param series If you only have one set of cards, the series will be 1.
@param foil Check if they have received the foil badge.
@return The user's badge level.
@api ISteamUser#GetGameBadgeLevel
*/
extern "C" DLL_EXPORT int GetGameBadgeLevel(int series, int foil);

//GetHSteamUser

/*
@desc Gets the Steam level of the user, as shown on their Steam community profile.
@return The level of the current user.
*/
extern "C" DLL_EXPORT int GetPlayerSteamLevel();

/*
@desc Gets a handle to the Steam ID of the account currently logged into the Steam client.
@return A SteamID handle
@api ISteamUser#GetSteamID
*/
extern "C" DLL_EXPORT int GetSteamID();

/*
@desc Checks to see if a Steam ID handle is valid.
@param hSteamID The Steam ID handle to check.
@return 1 when the Steam ID handle is valid; otherwise 0.
*/
extern "C" DLL_EXPORT int IsSteamIDValid(int hSteamID);

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
@desc Returns the handle for the given SteamID64 string.
@param steamID64 A SteamID64 string.
@return A Steam ID handle or 0.
*/
extern "C" DLL_EXPORT int GetHandleFromSteamID64(const char *steamID64);

//GetUserDataFolder
//GetVoice
//GetVoiceOptimalSampleRate
//InitiateGameConnection
//RequestEncryptedAppTicket
//RequestStoreAuthURL
//StartVoiceRecording
//StopVoiceRecording
//TerminateGameConnection
//TrackAppUsageEvent
//UserHasLicenseForApp

//Callbacks
//ClientGameServerDeny_t
//EncryptedAppTicketResponse_t - Call result for RequestEncryptedAppTicket.
//GameWebCallback_t
//GetAuthSessionTicketResponse_t - Call result for GetAuthSessionTicket.
//IPCFailure_t
//LicensesUpdated_t
//MicroTxnAuthorizationResponse_t
//SteamServerConnectFailure_t
//SteamServersConnected_t
//SteamServersDisconnected_t
//StoreAuthURLResponse_t - Call result for RequestStoreAuthURL
//ValidateAuthTicketResponse_t - Call result for BeginAuthSession
#pragma endregion

#pragma region ISteamUserStats
/* @page ISteamUserStats */
//AttachLeaderboardUGC

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

#pragma region ISteamUtils
/* @page ISteamUtils */
/*
@desc Checks if the Overlay needs a present. Only required if using event driven render updates.

Typically this call is unneeded if your game has a constantly running frame loop that calls Sync() or Swap() every frame.
@return 1 if the overlay needs you to refresh the screen, otherwise 0.
@api ISteamUtils#BOverlayNeedsPresent
*/
extern "C" DLL_EXPORT int OverlayNeedsPresent();

/*
@desc
_This method is just for testing purposes since the app should already know its AppID._

Returns the AppID or 0 if the Steam API has not been not initialized or the AppID cannot be found.
@return The AppID or 0.
@api ISteamUtils#GetAppID
*/
extern "C" DLL_EXPORT int GetAppID();

/*
@desc Gets the current amount of battery power on the computer.
@return The current batter power as a percentage, from 0 to 100, or 255 when on AC power.
@api ISteamUtils#GetCurrentBatteryPower
*/
extern "C" DLL_EXPORT int GetCurrentBatteryPower();

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
@plugin-name LoadImageFromHandle
*/
extern "C" DLL_EXPORT void LoadImageIDFromHandle(int imageID, int hImage);

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
@desc Checks if the Steam Overlay is running and the user can access it.

The overlay process could take a few seconds to start and hook the game process, so this method will initially return 0 while the overlay is loading.
@return 1 when the overlay is enabled; otherwise 0.
@api ISteamUtils#IsOverlayEnabled
*/
extern "C" DLL_EXPORT int IsOverlayEnabled();

/*
@desc Checks if Steam and the Steam Overlay are running in Big Picture mode.
@return 1 if in Big Picture Mode; otherwise 0.
@api ISteamUtils#IsSteamInBigPictureMode
*/
extern "C" DLL_EXPORT int IsSteamInBigPictureMode();

/*
@desc Checks if Steam is running in VR mode.
@return 1 when Steam is running in VR mode; otherwise 0.
@api ISteamUtils#IsSteamRunningInVR
*/
extern "C" DLL_EXPORT int IsSteamRunningInVR();

/*
@desc Checks if the HMD view will be streamed via Steam In-Home Streaming.

See API reference for further information.
@return 1 if streaming is enabled; otherwise 0.
@api ISteamUtils#IsVRHeadsetStreamingEnabled
*/
extern "C" DLL_EXPORT int IsVRHeadsetStreamingEnabled();

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

/*
@desc Set whether the HMD content will be streamed via Steam In-Home Streaming.

See API reference for further information.
@param enabled Turns VR HMD Streaming on (1) or off (0).
@api ISteamUtils#SetVRHeadsetStreamingEnabled
*/
extern "C" DLL_EXPORT void SetVRHeadsetStreamingEnabled(int enabled);

/*
@desc Sets a warning message hook within the plugin to receive Steam API warnings and info messages and output them to AGK's Debug Log.

_Note: I have never seen a warning show up and don't know how to force one to fire, so I can only assume the plugin code is correct._
@api ISteamUtils#SetWarningMessageHook
*/
extern "C" DLL_EXPORT void SetWarningMessageHook();

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
@desc Asks Steam to create and render the OpenVR dashboard.
@api ISteamUtils#StartVRDashboard
*/
extern "C" DLL_EXPORT void StartVRDashboard();

//Callbacks
/*
@desc Triggered when the big picture gamepad text input has been closed.
@callback-type list
@callback-getters GetGamepadTextInputDismissedSubmitted, GetGamepadTextInputDismissedSubmittedText
@return 1 when the big picture gamepad text input has closed; otherwise 0.
@api ISteamUtils#GamepadTextInputDismissed_t
*/
extern "C" DLL_EXPORT int HasGamepadTextInputDismissedResponse();

/*
@desc Gets whether the user has entered and accepted text for the current GamepadTextInputDismissed_t call.
@return 1 if user entered and accepted text; otherwise 0.
@api ISteamUtils#GamepadTextInputDismissed_t
*/
extern "C" DLL_EXPORT int GetGamepadTextInputDismissedSubmitted();

/*
@desc Gets the text for the current GamepadTextInputDismissed_t call.
@return A string.
@api ISteamUtils#GamepadTextInputDismissed_t
*/
extern "C" DLL_EXPORT char *GetGamepadTextInputDismissedSubmittedText();

/*
@desc Reports when the country of the user has changed.  Use GetIPCountry to get the new value.
@callback-type bool
@return 1 when the country has changed; otherwise 0.
@api ISteamUtils#IPCountry_t
*/
extern "C" DLL_EXPORT int HasIPCountryChangedResponse();

/*
@desc Tests when running on a laptop and there is less than 10 minutes of battery.  Fires every minute afterwards.
This method returns 1 once per warning.  It is not reported as an on going effect.
@callback-type bool
@callback-getters GetMinutesBatteryLeft
@return 1 when there is a low battery warning; otherwise 0.
@api ISteamUtils#LowBatteryPower_t
*/
extern "C" DLL_EXPORT int HasLowBatteryWarningResponse();

/*
@desc HasLowBatteryWarning should be checked before calling this method.

Reports the estimate battery life in minutes when a low battery warning occurs.
@return Battery life in minutes.
@api ISteamUtils#LowBatteryPower_t
*/
extern "C" DLL_EXPORT int GetMinutesBatteryLeft();

//SteamAPICallCompleted_t

/*
@desc Called when Steam wants to shutdown.
@callback-type bool
@return 1 when Steam is shutting down; otherwise 0.
@api ISteamUtils#SteamShutdown_t
*/
extern "C" DLL_EXPORT int IsSteamShuttingDown();
#pragma endregion

#pragma region ISteamVideo
// TODO?
#pragma endregion

#endif // _DLLMAIN_H_