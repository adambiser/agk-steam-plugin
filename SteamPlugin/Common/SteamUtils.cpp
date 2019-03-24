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

int OverlayNeedsPresent()
{
	CheckInitialized(0);
	return SteamUtils()->BOverlayNeedsPresent();
}

//CheckFileSignature - Deprecated
//GetAPICallFailureReason - Not needed.
//GetAPICallResult - Not needed.

int GetAppID()
{
	return SteamUtils()->GetAppID();
}

//GetConnectedUniverse - Valve use only.
//GetCSERIPPort - Only used in Source engine games.

int GetCurrentBatteryPower()
{
	CheckInitialized(0);
	return SteamUtils()->GetCurrentBatteryPower();
}

//GetEnteredGamepadTextInput - used in callback
//GetEnteredGamepadTextLength - used in callback

#define MEMBLOCK_IMAGE_HEADER_LENGTH	12

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
		memcpy_s(agk::GetMemblockPtr(memID) + MEMBLOCK_IMAGE_HEADER_LENGTH, imageSizeInBytes, imageBuffer, imageSizeInBytes);
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

int LoadImageFromHandle(int hImage)
{
	CheckInitialized(0);
	return LoadImageFromHandle(0, hImage);
}

void LoadImageIDFromHandle(int imageID, int hImage)
{
	CheckInitialized(NORETURN);
	LoadImageFromHandle(imageID, hImage);
}

int GetIPCCallCount()
{
	CheckInitialized(0);
	return SteamUtils()->GetIPCCallCount();
}

char *GetIPCountry()
{
	CheckInitialized(NULL_STRING);
	return utils::CreateString(SteamUtils()->GetIPCountry());
}

int GetSecondsSinceAppActive()
{
	CheckInitialized(0);
	return SteamUtils()->GetSecondsSinceAppActive();
}

int GetSecondsSinceComputerActive()
{
	CheckInitialized(0);
	return SteamUtils()->GetSecondsSinceComputerActive();
}

int GetServerRealTime()
{
	CheckInitialized(0);
	return SteamUtils()->GetServerRealTime();
}

char *GetSteamUILanguage()
{
	CheckInitialized(NULL_STRING);
	return utils::CreateString(SteamUtils()->GetSteamUILanguage());
}

//IsAPICallCompleted - Not needed.

int IsOverlayEnabled()
{
	CheckInitialized(0);
	return SteamUtils()->IsOverlayEnabled();
}

int IsSteamInBigPictureMode()
{
	CheckInitialized(0);
	return SteamUtils()->IsSteamInBigPictureMode();
}

int IsSteamRunningInVR()
{
	CheckInitialized(false);
	return SteamUtils()->IsSteamRunningInVR();
}

int IsVRHeadsetStreamingEnabled()
{
	CheckInitialized(false);
	return SteamUtils()->IsVRHeadsetStreamingEnabled();
}

void SetOverlayNotificationInset(int horizontalInset, int verticalInset)
{
	CheckInitialized(NORETURN);
	SteamUtils()->SetOverlayNotificationInset(horizontalInset, verticalInset);
}

void SetOverlayNotificationPosition(int eNotificationPosition)
{
	CheckInitialized(NORETURN);
	SteamUtils()->SetOverlayNotificationPosition((ENotificationPosition)eNotificationPosition);
}

void SetVRHeadsetStreamingEnabled(int enabled)
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

void SetWarningMessageHook()
{
	CheckInitialized(NORETURN);
	SteamUtils()->SetWarningMessageHook(&SteamAPIDebugTextHook);
}

int ShowGamepadTextInput(int eInputMode, int eLineInputMode, const char *description, int charMax, const char *existingText)
{
	CheckInitialized(false);
	if (charMax > MAX_GAMEPAD_TEXT_INPUT_LENGTH)
	{
		agk::Log("ShowGamepadTextInput: Maximum text length exceeds plugin limit.");
		charMax = MAX_GAMEPAD_TEXT_INPUT_LENGTH;
	}
	Callbacks()->RegisterGamepadTextInputDismissedCallback();
	return SteamUtils()->ShowGamepadTextInput((EGamepadTextInputMode)eInputMode, (EGamepadTextInputLineMode)eLineInputMode, description, charMax, existingText);
}

void StartVRDashboard()
{
	CheckInitialized(NORETURN);
	SteamUtils()->StartVRDashboard();
}

//Callbacks
//CheckFileSignature_t

int HasGamepadTextInputDismissedResponse()
{
	CheckInitialized(0);
	return Callbacks()->HasGamepadTextInputDismissedResponse();
}

int GetGamepadTextInputDismissedSubmitted()
{
	return Callbacks()->GetGamepadTextInputDismissed().m_bSubmitted;
}

char *GetGamepadTextInputDismissedSubmittedText()
{
	return utils::CreateString(Callbacks()->GetGamepadTextInputDismissed().m_chSubmittedText);
}

int HasIPCountryChangedResponse()
{
	CheckInitialized(false);
	return Callbacks()->HasIPCountryChangedResponse();
}

int HasLowBatteryWarningResponse()
{
	CheckInitialized(false);
	return Callbacks()->HasLowBatteryPowerResponse();
}

int GetMinutesBatteryLeft()
{
	CheckInitialized(255);
	return Callbacks()->GetMinutesBatteryLeft();
}

//SteamAPICallCompleted_t

int IsSteamShuttingDown()
{
	CheckInitialized(false);
	return Callbacks()->HasSteamShutdownResponse();
}
