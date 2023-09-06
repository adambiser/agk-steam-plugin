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

#ifndef _STEAMUTILS_H_
#define _STEAMUTILS_H_
#pragma once

#include "CCallbacks.h"

// CheckFileSignature_t - Call result for  CheckFileSignature.

#define MAX_GAMEPAD_TEXT_INPUT_LENGTH	512

namespace wrapper
{
	struct GamepadTextInputDismissed_t : ::GamepadTextInputDismissed_t
	{
		char m_chSubmittedText[MAX_GAMEPAD_TEXT_INPUT_LENGTH + 1];
		GamepadTextInputDismissed_t() : m_chSubmittedText{} {}
		GamepadTextInputDismissed_t(::GamepadTextInputDismissed_t from) : ::GamepadTextInputDismissed_t(from), m_chSubmittedText{} {}
	};
}

class CGamepadTextInputDismissedCallback : public ListCallbackBase<CGamepadTextInputDismissedCallback, GamepadTextInputDismissed_t, wrapper::GamepadTextInputDismissed_t>
{
public:
	void OnResponse(GamepadTextInputDismissed_t *pParam)
	{
		agk::Log("Callback: OnGamepadTextInputDismissed");
		// Convert to the wrapper version here so that m_chSubmittedText doesn't clear later.
		wrapper::GamepadTextInputDismissed_t response(*pParam);
		if (response.m_bSubmitted)
		{
			uint32 length = SteamUtils()->GetEnteredGamepadTextLength();
			//utils::Log("GetEnteredGamepadTextLength: " + std::to_string(length));
			bool success = SteamUtils()->GetEnteredGamepadTextInput(response.m_chSubmittedText, length);
			if (!success)
			{
				agk::Log("GamepadTextInputDismissedCallback - GetEnteredGamepadTextInput failed.");
				strcpy(response.m_chSubmittedText, "ERROR");
			}
		}
		else
		{
			Clear(response);
		}
		StoreResponse(response); // Store the wrapper version!
	}
	void Clear(wrapper::GamepadTextInputDismissed_t &value)
	{
		value.m_bSubmitted = false;
		value.m_chSubmittedText[0] = 0;
		value.m_unSubmittedText = 0;
	}
};

class CIPCountryChangedCallback : public BoolCallbackBase<CIPCountryChangedCallback, IPCountry_t>
{
public:
	void OnResponse(IPCountry_t *pParam)
	{
		agk::Log("Callback: OnIPCountryChanged");
		BoolCallbackBase::OnResponse(pParam);
	}
};

class CLowBatteryPowerCallback : public BoolCallbackBase<CLowBatteryPowerCallback, LowBatteryPower_t>
{
public:
	void OnResponse(LowBatteryPower_t *pParam)
	{
		agk::Log("Callback: OnLowBatteryPower");
		BoolCallbackBase::OnResponse(pParam);
		m_nMinutesBatteryLeft = pParam->m_nMinutesBatteryLeft;
	}
	void Reset()
	{
		BoolCallbackBase::Reset();
		m_nMinutesBatteryLeft = 0;
	}
	uint8 GetMinutesBatteryLeft() { return m_nMinutesBatteryLeft; }
private:
	uint8 m_nMinutesBatteryLeft;

};

//SteamAPICallCompleted_t - not needed

class CSteamShutdownCallback : public BoolCallbackBase<CSteamShutdownCallback, SteamShutdown_t>
{
public:
	void OnResponse(SteamShutdown_t *pParam)
	{
		agk::Log("Callback: OnSteamShutdown");
		BoolCallbackBase::OnResponse(pParam);
	}
};

#endif // _STEAMUTILS_H_
