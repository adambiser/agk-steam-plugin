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

#include "SteamPlugin.h"

#define MEMBLOCK_IMAGE_HEADER_LENGTH	12

//int SteamPlugin::LoadImageFromHandle(int hImage)
//{
//	return SteamPlugin::LoadImageFromHandle(0, hImage);
//}
//
//int SteamPlugin::LoadImageFromHandle(int imageID, int hImage)
//{
	//if (hImage == -1 || hImage == 0)
	//{
	//	return 0;
	//}
	//uint32 width;
	//uint32 height;
	//if (!SteamUtils()->GetImageSize(hImage, &width, &height))
	//{
	//	agk::PluginError("GetImageSize failed.");
	//	return 0;
	//}
	//// Get the actual raw RGBA data from Steam and turn it into a texture in our game engine
	//const int imageSizeInBytes = width * height * 4;
	//uint8 *imageBuffer = new uint8[imageSizeInBytes];
	//if (SteamUtils()->GetImageRGBA(hImage, imageBuffer, imageSizeInBytes))
	//{
	//	unsigned int memID = agk::CreateMemblock(imageSizeInBytes + MEMBLOCK_IMAGE_HEADER_LENGTH);
	//	agk::SetMemblockInt(memID, 0, width);
	//	agk::SetMemblockInt(memID, 4, height);
	//	agk::SetMemblockInt(memID, 8, 32); // bitdepth always 32
	//	memcpy_s(agk::GetMemblockPtr(memID) + MEMBLOCK_IMAGE_HEADER_LENGTH, imageSizeInBytes, imageBuffer, imageSizeInBytes);
	//	if (imageID)
	//	{
	//		agk::CreateImageFromMemblock(imageID, memID);
	//	}
	//	else
	//	{
	//		imageID = agk::CreateImageFromMemblock(memID);
	//	}
	//	agk::DeleteMemblock(memID);
	//}
	//else
	//{
	//	imageID = 0;
	//	agk::PluginError("GetImageRGBA failed.");
	//}
	//// Free memory.
	//delete[] imageBuffer;
	//return imageID;
//}

// Utils
void SteamPlugin::OnIPCountryChanged(IPCountry_t *pParam)
{
	agk::Log("Callback: IP Country Changed");
	m_bIPCountryChanged = true;
}

void SteamPlugin::OnLowBatteryPower(LowBatteryPower_t *pParam)
{
	agk::Log("Callback: Low Battery Power Warning");
	m_bLowBatteryWarning = true;
	m_nMinutesBatteryLeft = pParam->m_nMinutesBatteryLeft;
}

void SteamPlugin::OnSteamShutdown(SteamShutdown_t *pParam)
{
	agk::Log("Callback: Steam Shutdown");
	m_bSteamShutdownNotification = true;
}

// Big Picture Mode methods
void SteamPlugin::OnGamepadTextInputDismissed(GamepadTextInputDismissed_t *pCallback)
{
	agk::Log("Callback: Gamepad Text Input Dismissed");
	plugin::GamepadTextInputDismissed_t info(*pCallback);
	if (info.m_bSubmitted)
	{
		uint32 length = SteamUtils()->GetEnteredGamepadTextLength();
		if (!SteamUtils()->GetEnteredGamepadTextInput(info.m_chSubmittedText, length))
		{
			// This should only ever happen if there's a length mismatch.
			agk::PluginError("GetEnteredGamepadTextInput failed.");
		}
	}
	StoreCallbackResult(GamepadTextInputDismissed, info);
}

bool SteamPlugin::HasGamepadTextInputDismissed()
{
	GetNextCallbackResult(GamepadTextInputDismissed);
}

