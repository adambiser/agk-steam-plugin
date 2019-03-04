#include "SteamPlugin.h"

int SteamPlugin::GetAppID()
{
	CheckInitialized(SteamUtils, 0);
	return SteamUtils()->GetAppID();
}

int SteamPlugin::LoadImageFromHandle(int hImage)
{
	return SteamPlugin::LoadImageFromHandle(0, hImage);
}

int SteamPlugin::LoadImageFromHandle(int imageID, int hImage)
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
		unsigned int memID = agk::CreateMemblock(imageSizeInBytes + 4 * 3);
		agk::SetMemblockInt(memID, 0, width);
		agk::SetMemblockInt(memID, 4, height);
		agk::SetMemblockInt(memID, 8, 32); // bitdepth always 32
		for (int index = 0, offset = 12; index < imageSizeInBytes; index++, offset++)
		{
			agk::SetMemblockByte(memID, offset, imageBuffer[index]);
		}
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

// Utils
uint8 SteamPlugin::GetCurrentBatteryPower()
{
	CheckInitialized(SteamUtils, 0);
	return SteamUtils()->GetCurrentBatteryPower();
}

uint32 SteamPlugin::GetIPCCallCount()
{
	CheckInitialized(SteamUtils, 0);
	return SteamUtils()->GetIPCCallCount();
}

const char *SteamPlugin::GetIPCountry()
{
	CheckInitialized(SteamUtils, NULL);
	return SteamUtils()->GetIPCountry();
}

uint32 SteamPlugin::GetSecondsSinceAppActive()
{
	CheckInitialized(SteamUtils, 0);
	return SteamUtils()->GetSecondsSinceAppActive();
}

uint32 SteamPlugin::GetSecondsSinceComputerActive()
{
	CheckInitialized(SteamUtils, 0);
	return SteamUtils()->GetSecondsSinceComputerActive();
}

uint32 SteamPlugin::GetServerRealTime()
{
	CheckInitialized(SteamUtils, 0);
	return SteamUtils()->GetServerRealTime();
}

const char *SteamPlugin::GetSteamUILanguage()
{
	CheckInitialized(SteamUtils, NULL);
	return SteamUtils()->GetSteamUILanguage();
}

bool SteamPlugin::IsOverlayEnabled()
{
	CheckInitialized(SteamUtils, 0);
	return SteamUtils()->IsOverlayEnabled();
}

void SteamPlugin::SetOverlayNotificationInset(int nHorizontalInset, int nVerticalInset)
{
	CheckInitialized(SteamUtils, );
	SteamUtils()->SetOverlayNotificationInset(nHorizontalInset, nVerticalInset);
}

void SteamPlugin::SetOverlayNotificationPosition(ENotificationPosition eNotificationPosition)
{
	CheckInitialized(SteamUtils, );
	SteamUtils()->SetOverlayNotificationPosition(eNotificationPosition);
}

void SteamPlugin::OnIPCountryChanged(IPCountry_t *pParam)
{
	agk::Log("Callback: IP Country Changed");
	m_HasIPCountryChanged = true;
}

bool SteamPlugin::HasIPCountryChanged()
{
	if (m_HasIPCountryChanged)
	{
		m_HasIPCountryChanged = false;
		return true;
	}
	return false;
}

void SteamPlugin::OnLowBatteryPower(LowBatteryPower_t *pParam)
{
	agk::Log("Callback: Low Battery Power Warning");
	m_HasLowBatteryWarning = true;
	m_nMinutesBatteryLeft = pParam->m_nMinutesBatteryLeft;
}

bool SteamPlugin::HasLowBatteryWarning()
{
	if (m_HasLowBatteryWarning)
	{
		m_HasLowBatteryWarning = false;
		return true;
	}
	return false;
}

void SteamPlugin::OnSteamShutdown(SteamShutdown_t *pParam)
{
	agk::Log("Callback: Steam Shutdown");
	m_IsSteamShuttingDown = true;
}

bool SteamPlugin::IsSteamShuttingDown()
{
	if (m_IsSteamShuttingDown)
	{
		m_IsSteamShuttingDown = false;
		return true;
	}
	return m_IsSteamShuttingDown;
}

extern "C" void __cdecl SteamAPIDebugTextHook(int nSeverity, const char *pchDebugText)
{
	agk::Log(pchDebugText);
}

void SteamPlugin::SetWarningMessageHook()
{
	CheckInitialized(SteamUtils, );
	SteamUtils()->SetWarningMessageHook(&SteamAPIDebugTextHook);
}

// Big Picture Mode methods
bool SteamPlugin::IsSteamInBigPictureMode()
{
	CheckInitialized(SteamUtils, false);
	return SteamUtils()->IsSteamInBigPictureMode();

}
bool SteamPlugin::ShowGamepadTextInput(EGamepadTextInputMode eInputMode, EGamepadTextInputLineMode eLineInputMode, const char *pchDescription, uint32 unCharMax, const char *pchExistingText)
{
	CheckInitialized(SteamUtils, false);
	if (unCharMax >= sizeof(m_GamepadTextInputDismissedInfo.text))
	{
		agk::Log("ShowGamepadTextInput: Maximum text length exceeds plugin limit.");
		unCharMax = sizeof(m_GamepadTextInputDismissedInfo.text) - 1;
	}
	return SteamUtils()->ShowGamepadTextInput(eInputMode, eLineInputMode, pchDescription, unCharMax, pchExistingText);
}

bool SteamPlugin::HasGamepadTextInputDismissedInfo()
{
	return m_GamepadTextInputDismissedInfo.dismissed;
}

void SteamPlugin::GetGamepadTextInputDismissedInfo(bool *pbSubmitted, char *pchText)
{
	//*info = m_GamepadTextInputDismissedInfo;
	*pbSubmitted = m_GamepadTextInputDismissedInfo.submitted;
	strcpy_s(pchText, strnlen_s(m_GamepadTextInputDismissedInfo.text, MAX_GAMEPAD_TEXT_INPUT_LENGTH) + 1, m_GamepadTextInputDismissedInfo.text);
	m_GamepadTextInputDismissedInfo.dismissed = false;
}

void SteamPlugin::OnGamepadTextInputDismissed(GamepadTextInputDismissed_t *pCallback)
{
	agk::Log("Callback: Gamepad Text Input Dismissed");
	m_GamepadTextInputDismissedInfo.dismissed = true;
	m_GamepadTextInputDismissedInfo.submitted = pCallback->m_bSubmitted;
	*m_GamepadTextInputDismissedInfo.text = NULL;
	if (!pCallback->m_bSubmitted)
	{
		// User canceled.
		return;
	}
	uint32 length = SteamUtils()->GetEnteredGamepadTextLength();
	bool success = SteamUtils()->GetEnteredGamepadTextInput(m_GamepadTextInputDismissedInfo.text, length);
	if (!success) {
		// This should only ever happen if there's a length mismatch.
		agk::Log("GetEnteredGamepadTextInput failed.");
	}
}

// VR methods
bool SteamPlugin::IsSteamRunningInVR()
{
	CheckInitialized(SteamUtils, false);
	return SteamUtils()->IsSteamRunningInVR();
}

void SteamPlugin::StartVRDashboard()
{
	CheckInitialized(SteamUtils, );
	SteamUtils()->StartVRDashboard();
}

void SteamPlugin::SetVRHeadsetStreamingEnabled(bool bEnabled)
{
	CheckInitialized(SteamUtils, );
	SteamUtils()->SetVRHeadsetStreamingEnabled(bEnabled);
}

bool SteamPlugin::IsVRHeadsetStreamingEnabled()
{
	CheckInitialized(SteamUtils, false);
	return SteamUtils()->IsVRHeadsetStreamingEnabled();
}

