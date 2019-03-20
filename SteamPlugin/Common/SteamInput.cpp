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

int g_InputCount = 0;
InputHandle_t g_InputHandles[STEAM_INPUT_MAX_COUNT];
InputAnalogActionData_t g_InputAnalogActionData;
InputDigitalActionData_t g_InputDigitalActionData;
InputMotionData_t g_InputMotionData;

// Handles are 1-based!
#define ValidateInputHandle(index, returnValue)		\
	index--;										\
	if (index < 0 || index >= g_InputCount)			\
	{												\
		agk::PluginError("Invalid input handle.");	\
		return returnValue;							\
	}

// As of v1.43 - Use ISteamInput instead of ISteamController.
void ActivateActionSet(int hInput, int hActionSet)
{
	ValidateInputHandle(hInput, );
	SteamInput()->ActivateActionSet(g_InputHandles[hInput], SteamHandles()->GetSteamHandle(hActionSet));
}

void ActivateActionSetLayer(int hInput, int hActionSetLayer)
{
	ValidateInputHandle(hInput, );
	SteamInput()->ActivateActionSetLayer(g_InputHandles[hInput], SteamHandles()->GetSteamHandle(hActionSetLayer));
}

void DeactivateActionSetLayer(int hInput, int hActionSetLayer)
{
	ValidateInputHandle(hInput, );
	SteamInput()->DeactivateActionSetLayer(g_InputHandles[hInput], SteamHandles()->GetSteamHandle(hActionSetLayer));
}

void DeactivateAllActionSetLayers(int hInput)
{
	ValidateInputHandle(hInput, );
	SteamInput()->DeactivateAllActionSetLayers(g_InputHandles[hInput]);
}

int GetActiveActionSetLayerCount(int hInput)
{
	ValidateInputHandle(hInput, 0);
	InputActionSetHandle_t hActionSetLayers[STEAM_INPUT_MAX_COUNT];
	return SteamInput()->GetActiveActionSetLayers(g_InputHandles[hInput], hActionSetLayers);
}

int GetActiveActionSetLayerHandle(int hInput, int index)
{
	ValidateInputHandle(hInput, 0);
	InputActionSetHandle_t hActionSetLayers[STEAM_INPUT_MAX_COUNT];
	int count = SteamInput()->GetActiveActionSetLayers(g_InputHandles[hInput], hActionSetLayers);
	if (index >= 0 && index < count)
	{
		return SteamHandles()->GetPluginHandle(hActionSetLayers[index]);
	}
	return 0;
}

int GetActionSetHandle(const char *actionSetName)
{
	CheckInitialized(0);
	return SteamHandles()->GetPluginHandle(SteamInput()->GetActionSetHandle(actionSetName));
}

int GetAnalogActionData(int hInput, int hAnalogAction)
{
	ValidateInputHandle(hInput, 0);
	g_InputAnalogActionData = SteamInput()->GetAnalogActionData(g_InputHandles[hInput], SteamHandles()->GetSteamHandle(hAnalogAction));
	return g_InputAnalogActionData.bActive;
}

int GetAnalogActionDataActive()
{
	return g_InputAnalogActionData.bActive;
}

int GetAnalogActionDataMode()
{
	return g_InputAnalogActionData.eMode;
}

float GetAnalogActionDataX()
{
	return g_InputAnalogActionData.x;
}

float GetAnalogActionDataY()
{
	return g_InputAnalogActionData.y;
}

int GetAnalogActionHandle(const char *actionName)
{
	CheckInitialized(0);
	return SteamHandles()->GetPluginHandle(SteamInput()->GetAnalogActionHandle(actionName));
}

int GetAnalogActionOriginCount(int hInput, int hActionSet, int hAnalogAction)
{
	ValidateInputHandle(hInput, 0);
	InputActionSetHandle_t actionSetHandle = SteamHandles()->GetSteamHandle(hActionSet);
	InputAnalogActionHandle_t analogActionHandle = SteamHandles()->GetSteamHandle(hAnalogAction);
	EInputActionOrigin origins[STEAM_INPUT_MAX_ORIGINS];
	return SteamInput()->GetAnalogActionOrigins(g_InputHandles[hInput], actionSetHandle, analogActionHandle, origins);
}

int GetAnalogActionOriginValue(int hInput, int hActionSet, int hAnalogAction, int index)
{
	ValidateInputHandle(hInput, 0);
	InputActionSetHandle_t actionSetHandle = SteamHandles()->GetSteamHandle(hActionSet);
	InputAnalogActionHandle_t analogActionHandle = SteamHandles()->GetSteamHandle(hAnalogAction);
	EInputActionOrigin origins[STEAM_INPUT_MAX_ORIGINS];
	int count = SteamInput()->GetAnalogActionOrigins(g_InputHandles[hInput], actionSetHandle, analogActionHandle, origins);
	if (index >= 0 && index < count)
	{
		return origins[index];
	}
	return 0;
}

int GetConnectedControllers()
{
	CheckInitialized(0);
	g_InputCount = SteamInput()->GetConnectedControllers(g_InputHandles);
	return g_InputCount;
}

//GetControllerForGamepadIndex

int GetCurrentActionSet(int hInput)
{
	ValidateInputHandle(hInput, 0);
	return SteamHandles()->GetPluginHandle(SteamInput()->GetCurrentActionSet(g_InputHandles[hInput]));
}

int GetDigitalActionData(int hInput, int hDigitalAction)
{
	ValidateInputHandle(hInput, 0);
	g_InputDigitalActionData = SteamInput()->GetDigitalActionData(g_InputHandles[hInput], SteamHandles()->GetSteamHandle(hDigitalAction));
	return g_InputDigitalActionData.bActive;
}

int GetDigitalActionDataActive()
{
	return g_InputDigitalActionData.bActive;
}

int GetDigitalActionDataState()
{
	return g_InputDigitalActionData.bState;
}

int GetDigitalActionHandle(const char *actionName)
{
	CheckInitialized(0);
	return SteamHandles()->GetPluginHandle(SteamInput()->GetDigitalActionHandle(actionName));
}

int GetDigitalActionOriginCount(int hInput, int hActionSet, int hDigitalAction)
{
	ValidateInputHandle(hInput, 0);
	InputActionSetHandle_t actionSetHandle = SteamHandles()->GetSteamHandle(hActionSet);
	InputDigitalActionHandle_t digitalActionHandle = SteamHandles()->GetSteamHandle(hDigitalAction);
	EInputActionOrigin origins[STEAM_INPUT_MAX_ORIGINS];
	return SteamInput()->GetDigitalActionOrigins(g_InputHandles[hInput], actionSetHandle, digitalActionHandle, origins);
}

int GetDigitalActionOriginValue(int hInput, int hActionSet, int hDigitalAction, int index)
{
	ValidateInputHandle(hInput, 0);
	InputActionSetHandle_t actionSetHandle = SteamHandles()->GetSteamHandle(hActionSet);
	InputDigitalActionHandle_t digitalActionHandle = SteamHandles()->GetSteamHandle(hDigitalAction);
	EInputActionOrigin origins[STEAM_INPUT_MAX_ORIGINS];
	int count = SteamInput()->GetDigitalActionOrigins(g_InputHandles[hInput], actionSetHandle, digitalActionHandle, origins);
	if (index >= 0 && index < count)
	{
		return origins[index];
	}
	return 0;
}

//GetGamepadIndexForController

char *GetGlyphForActionOrigin(int eOrigin)
{
	CheckInitialized(NULL_STRING);
	return utils::CreateString(SteamInput()->GetGlyphForActionOrigin((EInputActionOrigin)eOrigin));
}

int GetInputTypeForHandle(int hInput)
{
	ValidateInputHandle(hInput, k_ESteamInputType_Unknown);
	return SteamInput()->GetInputTypeForHandle(g_InputHandles[hInput]);
}

void GetMotionData(int hInput)
{
	ValidateInputHandle(hInput, );
	g_InputMotionData = SteamInput()->GetMotionData(g_InputHandles[hInput]);
}

float GetMotionDataPosAccelX()
{
	return g_InputMotionData.posAccelX;
}

float GetMotionDataPosAccelY()
{
	return g_InputMotionData.posAccelY;
}

float GetMotionDataPosAccelZ()
{
	return g_InputMotionData.posAccelZ;
}

float GetMotionDataRotQuatW()
{
	return g_InputMotionData.rotQuatW;
}

float GetMotionDataRotQuatX()
{
	return g_InputMotionData.rotQuatX;
}

float GetMotionDataRotQuatY()
{
	return g_InputMotionData.rotQuatY;
}

float GetMotionDataRotQuatZ()
{
	return g_InputMotionData.rotQuatZ;
}

float GetMotionDataRotVelX()
{
	return g_InputMotionData.rotVelX;
}

float GetMotionDataRotVelY()
{
	return g_InputMotionData.rotVelY;

}
float GetMotionDataRotVelZ()
{
	return g_InputMotionData.rotVelZ;
}

char *GetStringForActionOrigin(int eOrigin)
{
	CheckInitialized(NULL_STRING);
	return utils::CreateString(SteamInput()->GetStringForActionOrigin((EInputActionOrigin)eOrigin));
}

int InitSteamInput()
{
	CheckInitialized(false);
	if (SteamInput()->Init())
	{
		// Give the API some time to refresh the inputs.
		RunFrame();
		return true;
	}
	return false;
}

void RunFrame()
{
	SteamInput()->RunFrame();
}

void SetInputLEDColor(int hInput, int red, int green, int blue)
{
	ValidateInputHandle(hInput, );
	Clamp(red, 0, UINT8_MAX);
	Clamp(green, 0, UINT8_MAX);
	Clamp(blue, 0, UINT8_MAX);
	SteamInput()->SetLEDColor(g_InputHandles[hInput], red, green, blue, k_ESteamInputLEDFlag_SetColor);
}

void ResetInputLEDColor(int hInput)
{
	ValidateInputHandle(hInput, );
	SteamInput()->SetLEDColor(g_InputHandles[hInput], 0, 0, 0, k_ESteamInputLEDFlag_RestoreUserDefault);
}

//ShowAnalogActionOrigins - deprecated

int ShowBindingPanel(int hInput)
{
	ValidateInputHandle(hInput, false);
	return SteamInput()->ShowBindingPanel(g_InputHandles[hInput]);
}

//ShowDigitalActionOrigins - deprecated

int ShutdownSteamInput()
{
	CheckInitialized(0);
	return SteamInput()->Shutdown();
}

void StopAnalogActionMomentum(int hInput, int hAnalogAction)
{
	ValidateInputHandle(hInput, );
	InputAnalogActionHandle_t analogActionHandle = SteamHandles()->GetSteamHandle(hAnalogAction);
	//ValidateActionHandle(InputAnalogActionHandle_t, analogActionHandle, hAnalogAction, m_AnalogActionHandles, );
	SteamInput()->StopAnalogActionMomentum(g_InputHandles[hInput], analogActionHandle);
}

void TriggerInputHapticPulse(int hInput, int eTargetPad, int duration)
{
	ValidateInputHandle(hInput, );
	Clamp(duration, 0, USHRT_MAX);
	SteamInput()->TriggerHapticPulse(g_InputHandles[hInput], (ESteamControllerPad)eTargetPad, (unsigned short)duration);
}

void TriggerInputRepeatedHapticPulse(int hInput, int eTargetPad, int onDuration, int offDuration, int repeat)
{
	ValidateInputHandle(hInput, );
	Clamp(onDuration, 0, USHRT_MAX);
	Clamp(offDuration, 0, USHRT_MAX);
	Clamp(repeat, 0, USHRT_MAX);
	SteamInput()->TriggerRepeatedHapticPulse(g_InputHandles[hInput], (ESteamControllerPad)eTargetPad, (unsigned short)onDuration, (unsigned short)offDuration, (unsigned short)repeat, 0);
}

void TriggerInputVibration(int hInput, int leftSpeed, int rightSpeed)
{
	ValidateInputHandle(hInput, );
	Clamp(leftSpeed, 0, USHRT_MAX);
	Clamp(rightSpeed, 0, USHRT_MAX);
	SteamInput()->TriggerVibration(g_InputHandles[hInput], (unsigned short)leftSpeed, (unsigned short)rightSpeed);
}

// New for v1.43
int GetActionOriginFromXboxOrigin(int hInput, int eOrigin)
{
	ValidateInputHandle(hInput, k_EInputActionOrigin_None);
	return SteamInput()->GetActionOriginFromXboxOrigin(g_InputHandles[hInput], (EXboxOrigin)eOrigin);
}

char *GetStringForXboxOrigin(int eOrigin)
{
	CheckInitialized(NULL_STRING);
	return utils::CreateString(SteamInput()->GetStringForXboxOrigin((EXboxOrigin)eOrigin));
}

char *GetGlyphForXboxOrigin(int eOrigin)
{
	CheckInitialized(NULL_STRING);
	return utils::CreateString(SteamInput()->GetGlyphForXboxOrigin((EXboxOrigin)eOrigin));
}

int TranslateActionOrigin(int eDestinationInputType, int eSourceOrigin)
{
	CheckInitialized(k_EInputActionOrigin_None);
	return SteamInput()->TranslateActionOrigin((ESteamInputType)eDestinationInputType, (EInputActionOrigin)eSourceOrigin);
}
