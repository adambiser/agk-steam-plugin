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

#include "SteamUtils.h"
#include "DllMain.h"

CGamepadTextInputDismissedCallback GamepadTextInputDismissedCallback;
CIPCountryChangedCallback IPCountryChangedCallback;
CLowBatteryPowerCallback LowBatteryPowerCallback;
CSteamShutdownCallback SteamShutdownCallback;

/* @page ISteamUtils */

/*
@desc Checks if the Overlay needs a present. Only required if using event driven render updates.

Typically this call is unneeded if your game has a constantly running frame loop that calls Sync() or Swap() every frame.
@return 1 if the overlay needs you to refresh the screen, otherwise 0.
@url https://partner.steamgames.com/doc/api/ISteamUtils#BOverlayNeedsPresent
*/
extern "C" DLL_EXPORT int OverlayNeedsPresent()
{
	CheckInitialized(0);
	return SteamUtils()->BOverlayNeedsPresent();
}

//CheckFileSignature - Deprecated
//GetAPICallFailureReason - Not needed.
//GetAPICallResult - Not needed.

/*
@desc
_This method is just for testing purposes since the app should already know its AppID._

Returns the AppID or 0 if the Steam API has not been not initialized or the AppID cannot be found.
@return The AppID or 0.
@url https://partner.steamgames.com/doc/api/ISteamUtils#GetAppID
*/
extern "C" DLL_EXPORT int GetAppID()
{
	CheckInitialized(0);
	return SteamUtils()->GetAppID();
}

//GetConnectedUniverse - Valve use only.
//GetCSERIPPort - Only used in Source engine games.

/*
@desc Gets the current amount of battery power on the computer.
@return The current batter power as a percentage, from 0 to 100, or 255 when on AC power.
@url https://partner.steamgames.com/doc/api/ISteamUtils#GetCurrentBatteryPower
*/
extern "C" DLL_EXPORT int GetCurrentBatteryPower()
{
	CheckInitialized(0);
	return SteamUtils()->GetCurrentBatteryPower();
}

//GetEnteredGamepadTextInput - used in callback
//GetEnteredGamepadTextLength - used in callback

int LoadImageFromHandle(int imageID, int hImage)
{
	if (hImage == -1 || hImage == 0)
	{
		return 0;
	}
	uint32 width;
	uint32 height;
	if (!SteamUtils()->GetImageSize(hImage, &width, &height))
	{
		agk::PluginError("GetImageSize failed.");
		return 0;
	}
	// Get the actual raw RGBA data from Steam and turn it into a texture in our game engine
	const int imageSizeInBytes = width * height * 4;
	uint8 *imageBuffer = new uint8[imageSizeInBytes];
	if (SteamUtils()->GetImageRGBA(hImage, imageBuffer, imageSizeInBytes))
	{
		unsigned int memID = agk::CreateMemblock(imageSizeInBytes + MEMBLOCK_IMAGE_HEADER_LENGTH);
		agk::SetMemblockInt(memID, 0, width);
		agk::SetMemblockInt(memID, 4, height);
		agk::SetMemblockInt(memID, 8, 32); // bitdepth always 32
		//memcpy_s(agk::GetMemblockPtr(memID) + MEMBLOCK_IMAGE_HEADER_LENGTH, imageSizeInBytes, imageBuffer, imageSizeInBytes);
		memcpy(agk::GetMemblockPtr(memID) + MEMBLOCK_IMAGE_HEADER_LENGTH, imageBuffer, imageSizeInBytes);
		if (imageID)
		{
			agk::CreateImageFromMemblock(imageID, memID);
		}
		else
		{
			imageID = agk::CreateImageFromMemblock(memID);
		}
		agk::DeleteMemblock(memID);
	}
	else
	{
		imageID = 0;
		agk::PluginError("GetImageRGBA failed.");
	}
	// Free memory.
	delete[] imageBuffer;
	return imageID;
}

/*
@desc Loads an image from an image handle into a new image ID.
@param hImage The handle to the image that will be obtained.
@return An image ID containing the loaded image.
@url https://partner.steamgames.com/doc/api/SteamUtils#GetImageSize
@url https://partner.steamgames.com/doc/api/SteamUtils#GetImageRGBA
*/
extern "C" DLL_EXPORT int LoadImageFromHandle(int hImage)
{
	CheckInitialized(0);
	return LoadImageFromHandle(0, hImage);
}

/*
@desc Loads an image from an image handle into a new image ID.
@param imageID The image ID into which to load the image.
@param hImage The handle to the image that will be obtained.
@url https://partner.steamgames.com/doc/api/SteamUtils#GetImageSize
@url https://partner.steamgames.com/doc/api/SteamUtils#GetImageRGBA
@plugin-name LoadImageFromHandle
*/
extern "C" DLL_EXPORT void LoadImageIDFromHandle(int imageID, int hImage)
{
	CheckInitialized(NORETURN);
	LoadImageFromHandle(imageID, hImage);
}

/*
@desc Returns the number of Inter-Process Communication calls made since the last time this method was called.

Used for perf debugging so you can determine how many IPC (Inter-Process Communication) calls your game makes per frame
Every IPC call is at minimum a thread context switch if not a process one so you want to rate control how often you do them.
@return An integer.
@url https://partner.steamgames.com/doc/api/ISteamUtils#GetIPCCallCount
*/
extern "C" DLL_EXPORT int GetIPCCallCount()
{
	CheckInitialized(0);
	return SteamUtils()->GetIPCCallCount();
}

/*
@desc Returns the 2 character ISO 3166-1-alpha-2 format country code which client is running in.
@return A two character string.
@url https://partner.steamgames.com/doc/api/ISteamUtils#GetIPCountry
*/
extern "C" DLL_EXPORT char *GetIPCountry()
{
	CheckInitialized(NULL_STRING);
	return utils::CreateString(SteamUtils()->GetIPCountry());
}

/*
@desc Returns the number of seconds since the application was active.
@return An integer.
@url https://partner.steamgames.com/doc/api/ISteamUtils#GetSecondsSinceAppActive
*/
extern "C" DLL_EXPORT int GetSecondsSinceAppActive()
{
	CheckInitialized(0);
	return SteamUtils()->GetSecondsSinceAppActive();
}

/*
@desc Returns the number of seconds since the user last moved the mouse.
@return An integer
@url https://partner.steamgames.com/doc/api/ISteamUtils#GetSecondsSinceComputerActive
*/
extern "C" DLL_EXPORT int GetSecondsSinceComputerActive()
{
	CheckInitialized(0);
	return SteamUtils()->GetSecondsSinceComputerActive();
}

/*
@desc Returns the Steam server time in Unix epoch format.
@return An integer.
@url https://partner.steamgames.com/doc/api/ISteamUtils#GetServerRealTime
*/
extern "C" DLL_EXPORT int GetServerRealTime()
{
	CheckInitialized(0);
	return SteamUtils()->GetServerRealTime();
}

/*
@desc Returns the language the steam client is running in.

You probably want GetCurrentGameLanguage instead, this should only be used in very special cases.
@return A string.
@url https://partner.steamgames.com/doc/api/ISteamUtils#GetSteamUILanguage
*/
extern "C" DLL_EXPORT char *GetSteamUILanguage()
{
	CheckInitialized(NULL_STRING);
	return utils::CreateString(SteamUtils()->GetSteamUILanguage());
}

/*
@desc Initializes text filtering. Returns false if filtering is unavailable for the language the user is currently running in.
If the language is unsupported, the FilterText API will act as a passthrough.
@return 1 if text filtering is available; otherwise 0.
@url https://partner.steamgames.com/doc/api/ISteamUtils#InitFilterText
*/
extern "C" DLL_EXPORT int InitFilterText()
{
	CheckInitialized(false);
	return SteamUtils()->InitFilterText();
}

/*
@desc Filters the provided input message and places the filtered result into pchOutFilteredText.
@param inputMessage The input string that should be filtered.
@param legalOnly 0 if you want profanity and legally required filtering (where required) and 1 if you want legally required filtering only.
@return The filtered text.
@url https://partner.steamgames.com/doc/api/ISteamUtils#FilterText
*/
extern "C" DLL_EXPORT char *FilterText(const char *inputMessage, int legalOnly)
{
	CheckInitialized(false);
	//size_t bufferLength = strnlen_s(inputMessage, 4096) + 1;
	size_t bufferLength = strnlen(inputMessage, 4096) + 1;
	char *pchOutFilteredText = new char[bufferLength];
	SteamUtils()->FilterText(pchOutFilteredText, (int) bufferLength, inputMessage, legalOnly != 0);
	char *result = utils::CreateString(pchOutFilteredText);
	delete[] pchOutFilteredText;
	return result;
}

//IsAPICallCompleted - Not needed.

/*
@desc Checks if the Steam Overlay is running and the user can access it.

The overlay process could take a few seconds to start and hook the game process, so this method will initially return 0 while the overlay is loading.
@return 1 when the overlay is enabled; otherwise 0.
@url https://partner.steamgames.com/doc/api/ISteamUtils#IsOverlayEnabled
*/
extern "C" DLL_EXPORT int IsOverlayEnabled()
{
	CheckInitialized(0);
	return SteamUtils()->IsOverlayEnabled();
}

/*
@desc Returns whether the current launcher is a Steam China launcher.
You can cause the client to behave as the Steam China launcher by adding "-dev -steamchina" to the command line when running Steam.
@return 1 when the current launcher is a Steam China launcher; otherwise 0.
@url https://partner.steamgames.com/doc/api/ISteamUtils#IsSteamChinaLauncher
*/
extern "C" DLL_EXPORT int IsSteamChinaLauncher()
{
	CheckInitialized(0);
	return SteamUtils()->IsSteamChinaLauncher();
}

/*
@desc Checks if Steam and the Steam Overlay are running in Big Picture mode.
@return 1 if in Big Picture Mode; otherwise 0.
@url https://partner.steamgames.com/doc/api/ISteamUtils#IsSteamInBigPictureMode
*/
extern "C" DLL_EXPORT int IsSteamInBigPictureMode()
{
	CheckInitialized(0);
	return SteamUtils()->IsSteamInBigPictureMode();
}

/*
@desc Checks if Steam is running in VR mode.
@return 1 when Steam is running in VR mode; otherwise 0.
@url https://partner.steamgames.com/doc/api/ISteamUtils#IsSteamRunningInVR
*/
extern "C" DLL_EXPORT int IsSteamRunningInVR()
{
	CheckInitialized(false);
	return SteamUtils()->IsSteamRunningInVR();
}

/*
@desc Checks if the HMD view will be streamed via Steam In-Home Streaming.

See API reference for further information.
@return 1 if streaming is enabled; otherwise 0.
@url https://partner.steamgames.com/doc/api/ISteamUtils#IsVRHeadsetStreamingEnabled
*/
extern "C" DLL_EXPORT int IsVRHeadsetStreamingEnabled()
{
	CheckInitialized(false);
	return SteamUtils()->IsVRHeadsetStreamingEnabled();
}

/*
@desc Sets the inset of the overlay notification from the corner specified by SetOverlayNotificationPosition.
@param horizontalInset The horizontal (left-right) distance in pixels from the corner.
@param verticalInset The vertical (up-down) distance in pixels from the corner.
@url https://partner.steamgames.com/doc/api/ISteamUtils#SetOverlayNotificationInset
*/
extern "C" DLL_EXPORT void SetOverlayNotificationInset(int horizontalInset, int verticalInset)
{
	CheckInitialized(NORETURN);
	SteamUtils()->SetOverlayNotificationInset(horizontalInset, verticalInset);
}

/*
@desc Sets which corner the Steam overlay notification popup should display itself in.

This position is per-game and is reset each launch.
@param eNotificationPosition The notification popup position.
@param-url eNotificationPosition https://partner.steamgames.com/doc/api/steam_api#ENotificationPosition
@url https://partner.steamgames.com/doc/api/ISteamUtils#SetOverlayNotificationPosition
*/
extern "C" DLL_EXPORT void SetOverlayNotificationPosition(int eNotificationPosition)
{
	CheckInitialized(NORETURN);
	SteamUtils()->SetOverlayNotificationPosition((ENotificationPosition)eNotificationPosition);
}

/*
@desc Set whether the HMD content will be streamed via Steam In-Home Streaming.

See API reference for further information.
@param enabled Turns VR HMD Streaming on (1) or off (0).
@url https://partner.steamgames.com/doc/api/ISteamUtils#SetVRHeadsetStreamingEnabled
*/
extern "C" DLL_EXPORT void SetVRHeadsetStreamingEnabled(int enabled)
{
	CheckInitialized(NORETURN);
	SteamUtils()->SetVRHeadsetStreamingEnabled(enabled != 0);
}

extern "C" void __cdecl SteamAPIDebugTextHook(int nSeverity, const char *pchDebugText)
{
	switch (nSeverity)
	{
	case 1: // warning
		utils::Log(std::string("Warning: ") + pchDebugText);
		break;
	case 0: // message
	default: // undefined severity
		agk::Log(pchDebugText);
		break;
	}
}

/*
@desc Sets a warning message hook within the plugin to receive Steam API warnings and info messages and output them to AGK's Debug Log.

_Note: I have never seen a warning show up and don't know how to force one to fire, so I can only assume the plugin code is correct._
@url https://partner.steamgames.com/doc/api/ISteamUtils#SetWarningMessageHook
*/
extern "C" DLL_EXPORT void SetWarningMessageHook()
{
	CheckInitialized(NORETURN);
	SteamUtils()->SetWarningMessageHook(&SteamAPIDebugTextHook);
}

/*
@desc Activates the Big Picture text input dialog which only supports gamepad input.

Note: charMax is limited to 512 characters.
@param eInputMode Selects the input mode to use, either Normal or Password (hidden text).
@param-url eInputMode https://partner.steamgames.com/doc/api/ISteamUtils#EGamepadTextInputMode
@param eLineInputMode Controls whether to use single or multi line input.
@param-url eLineInputMode https://partner.steamgames.com/doc/api/ISteamUtils#EGamepadTextInputLineMode
@param description Sets the description that should inform the user what the input dialog is for.
@param charMax The maximum number of characters that the user can input.
@param existingText Sets the preexisting text which the user can edit.
@return 1 if the input overlay opens; otherwise 0.
@url https://partner.steamgames.com/doc/api/ISteamUtils#ShowGamepadTextInput
*/
extern "C" DLL_EXPORT int ShowGamepadTextInput(int eInputMode, int eLineInputMode, const char *description, int charMax, const char *existingText)
{
	CheckInitialized(false);
	if (charMax > MAX_GAMEPAD_TEXT_INPUT_LENGTH)
	{
		agk::Log("ShowGamepadTextInput: Maximum text length exceeds plugin limit.");
		charMax = MAX_GAMEPAD_TEXT_INPUT_LENGTH;
	}
	GamepadTextInputDismissedCallback.Register();
	return SteamUtils()->ShowGamepadTextInput((EGamepadTextInputMode)eInputMode, (EGamepadTextInputLineMode)eLineInputMode, description, charMax, existingText);
}

/*
@desc Asks Steam to create and render the OpenVR dashboard.
@url https://partner.steamgames.com/doc/api/ISteamUtils#StartVRDashboard
*/
extern "C" DLL_EXPORT void StartVRDashboard()
{
	CheckInitialized(NORETURN);
	SteamUtils()->StartVRDashboard();
}

//Callbacks
//CheckFileSignature_t

/*
@desc Triggered when the big picture gamepad text input has been closed.
@callback-type list
@callback-getters GetGamepadTextInputDismissedSubmitted, GetGamepadTextInputDismissedSubmittedText
@return 1 when the big picture gamepad text input has closed; otherwise 0.
@url https://partner.steamgames.com/doc/api/ISteamUtils#GamepadTextInputDismissed_t
*/
extern "C" DLL_EXPORT int HasGamepadTextInputDismissedResponse()
{
	CheckInitialized(0);
	return GamepadTextInputDismissedCallback.HasResponse();
}

/*
@desc Gets whether the user has entered and accepted text for the current GamepadTextInputDismissed_t call.
@return 1 if user entered and accepted text; otherwise 0.
@url https://partner.steamgames.com/doc/api/ISteamUtils#GamepadTextInputDismissed_t
*/
extern "C" DLL_EXPORT int GetGamepadTextInputDismissedSubmitted()
{
	return GamepadTextInputDismissedCallback.GetCurrent().m_bSubmitted;
}

/*
@desc Gets the text for the current GamepadTextInputDismissed_t call.
@return A string.
@url https://partner.steamgames.com/doc/api/ISteamUtils#GamepadTextInputDismissed_t
*/
extern "C" DLL_EXPORT char *GetGamepadTextInputDismissedSubmittedText()
{
	return utils::CreateString(GamepadTextInputDismissedCallback.GetCurrent().m_chSubmittedText);
}

/*
@desc Reports when the country of the user has changed.  Use GetIPCountry to get the new value.
@callback-type bool
@return 1 when the country has changed; otherwise 0.
@url https://partner.steamgames.com/doc/api/ISteamUtils#IPCountry_t
*/
extern "C" DLL_EXPORT int HasIPCountryChangedResponse()
{
	CheckInitialized(false);
	return IPCountryChangedCallback.HasResponse();
}

/*
@desc Tests when running on a laptop and there is less than 10 minutes of battery.  Fires every minute afterwards.
This method returns 1 once per warning.  It is not reported as an on going effect.
@callback-type bool
@callback-getters GetMinutesBatteryLeft
@return 1 when there is a low battery warning; otherwise 0.
@url https://partner.steamgames.com/doc/api/ISteamUtils#LowBatteryPower_t
*/
extern "C" DLL_EXPORT int HasLowBatteryWarningResponse()
{
	CheckInitialized(false);
	return LowBatteryPowerCallback.HasResponse();
}

/*
@desc HasLowBatteryWarning should be checked before calling this method.

Reports the estimate battery life in minutes when a low battery warning occurs.
@return Battery life in minutes.
@url https://partner.steamgames.com/doc/api/ISteamUtils#LowBatteryPower_t
*/
extern "C" DLL_EXPORT int GetMinutesBatteryLeft()
{
	CheckInitialized(255);
	return LowBatteryPowerCallback.GetMinutesBatteryLeft();
}

//SteamAPICallCompleted_t

/*
@desc Called when Steam wants to shutdown.
@callback-type bool
@return 1 when Steam is shutting down; otherwise 0.
@url https://partner.steamgames.com/doc/api/ISteamUtils#SteamShutdown_t
*/
extern "C" DLL_EXPORT int IsSteamShuttingDown()
{
	CheckInitialized(false);
	return SteamShutdownCallback.HasResponse();
}
