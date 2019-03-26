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

/* @page ISteamInput
See the [Steam Input](https://partner.steamgames.com/doc/features/steam_input) documentation for more information.
As of Steamworks SDK v1.43 [Steam Controller](https://partner.steamgames.com/doc/features/steam_controller) is being deprecated.

Until the Steam Input API documentation has been posted, this will still link to ISteamController.

See also [Getting Started for Developers](https://partner.steamgames.com/doc/features/steam_controller/getting_started_for_devs).
*/

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

/*
@desc Reconfigure the input to use the specified action set (ie "Menu", "Walk", or "Drive").

This is cheap, and can be safely called repeatedly. It's often easier to repeatedly call it in your state loops, instead of trying to place it in all of your state transitions.
@param hInput The handle of the input you want to activate an action set for.
@param hActionSet The handle of the action set you want to activate.
@api ISteamController#ActivateActionSet
*/
extern "C" DLL_EXPORT void ActivateActionSet(int hInput, int hActionSet)
{
	ValidateInputHandle(hInput, );
	SteamInput()->ActivateActionSet(g_InputHandles[hInput], SteamHandles()->GetSteamHandle(hActionSet));
}

/*
@desc Reconfigure the input to use the specified action set layer.
@param hInput The handle of the input you want to activate an action set layer for.
@param hActionSetLayer The handle of the action set layer you want to activate.
@api ISteamController#ActivateActionSetLayer
*/
extern "C" DLL_EXPORT void ActivateActionSetLayer(int hInput, int hActionSetLayer)
{
	ValidateInputHandle(hInput, );
	SteamInput()->ActivateActionSetLayer(g_InputHandles[hInput], SteamHandles()->GetSteamHandle(hActionSetLayer));
}

/*
@desc Reconfigure the input to stop using the specified action set layer.
@param hInput The handle of the input you want to deactivate an action set layer for.
@param hActionSetLayer The handle of the action set layer you want to deactivate.
@api ISteamController#DeactivateActionSetLayer
*/
extern "C" DLL_EXPORT void DeactivateActionSetLayer(int hInput, int hActionSetLayer)
{
	ValidateInputHandle(hInput, );
	SteamInput()->DeactivateActionSetLayer(g_InputHandles[hInput], SteamHandles()->GetSteamHandle(hActionSetLayer));
}

/*
@desc Reconfigure the input to stop using all action set layers.
@param hInput The handle of the input you want to deactivate all action set layers for.
@api ISteamController#DeactivateAllActionSetLayers
*/
extern "C" DLL_EXPORT void DeactivateAllActionSetLayers(int hInput)
{
	ValidateInputHandle(hInput, );
	SteamInput()->DeactivateAllActionSetLayers(g_InputHandles[hInput]);
}

/*
@desc Gets the active action set layers for the given input.
@param hInput The handle of the input you want to get active action set layers for.
@return The number of active action set layers.
@api ISteamController#GetActiveActionSetLayers
*/
extern "C" DLL_EXPORT int GetActiveActionSetLayerCount(int hInput)
{
	ValidateInputHandle(hInput, 0);
	InputActionSetHandle_t hActionSetLayers[STEAM_INPUT_MAX_COUNT];
	return SteamInput()->GetActiveActionSetLayers(g_InputHandles[hInput], hActionSetLayers);
}

/*
@desc Gets the handle for an active action set layer for the given input by index.
@param hInput The handle of the input you want to get active action set layers for.
@param index The index to get.
@return The active action set layer handle.
@api ISteamController#GetActiveActionSetLayers
*/
extern "C" DLL_EXPORT int GetActiveActionSetLayerHandle(int hInput, int index)
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

/*
@desc Lookup the handle for an Action Set. Best to do this once on startup, and store the handles for all future API calls.
@param actionSetName The string identifier of an action set defined in the game's VDF file.
@return The handle of the specified action set.
@api ISteamController#GetActionSetHandle
*/
extern "C" DLL_EXPORT int GetActionSetHandle(const char *actionSetName)
{
	CheckInitialized(0);
	return SteamHandles()->GetPluginHandle(SteamInput()->GetActionSetHandle(actionSetName));
}

/*
@desc Queries the analog action data for an input.
Also returns whether or not this action is currently available to be bound in the active action set (the same as GetAnalogActionDataActive).
@param hInput The handle of the input you want to query.
@param hAnalogAction The handle of the analog action you want to query.
@return 1 if available; otherwise 0.
@api ISteamController#GetAnalogActionData, ISteamController#ControllerAnalogActionData_t
*/
extern "C" DLL_EXPORT int GetAnalogActionData(int hInput, int hAnalogAction)
{
	ValidateInputHandle(hInput, 0);
	g_InputAnalogActionData = SteamInput()->GetAnalogActionData(g_InputHandles[hInput], SteamHandles()->GetSteamHandle(hAnalogAction));
	return g_InputAnalogActionData.bActive;
}

/*
@desc Returns whether or not this action is currently available to be bound in the active action set as of the last GetAnalogActionData call.
@return 1 if available; otherwise 0.
@api ISteamController#ControllerAnalogActionData_t
*/
extern "C" DLL_EXPORT int GetAnalogActionDataActive()
{
	return g_InputAnalogActionData.bActive;
}

/*
@desc Returns the type of data coming from this action as of the last GetAnalogActionData call
@return The type of data coming from this action, this will match what was specified in the action set's VDF definition.
@api ISteamController#ControllerAnalogActionData_t, ISteamController#EControllerSourceMode
*/
extern "C" DLL_EXPORT int GetAnalogActionDataMode()
{
	return g_InputAnalogActionData.eMode;
}

/*
@desc Returns the state of this action on the horizontal axis as of the last GetAnalogActionData call
@return The current state of this action on the horizontal axis.
@api ISteamController#ControllerAnalogActionData_t
*/
extern "C" DLL_EXPORT float GetAnalogActionDataX()
{
	return g_InputAnalogActionData.x;
}

/*
@desc Returns the state of this action on the vertical axis as of the last GetAnalogActionData call
@return The current state of this action on the vertical axis.
@api ISteamController#ControllerAnalogActionData_t
*/
extern "C" DLL_EXPORT float GetAnalogActionDataY()
{
	return g_InputAnalogActionData.y;
}

/*
@desc Get the handle of the specified Analog action.

**NOTE:** This method does not take an action set handle parameter. That means that each action in your VDF file must have a unique string identifier.
In other words, if you use an action called "up" in two different action sets, this method will only ever return one of them and the other will be ignored.
@param actionName The string identifier of the analog action defined in the game's VDF file.
@return The analog action handle.
@api ISteamController#GetAnalogActionHandle
*/
extern "C" DLL_EXPORT int GetAnalogActionHandle(const char *actionName)
{
	CheckInitialized(0);
	return SteamHandles()->GetPluginHandle(SteamInput()->GetAnalogActionHandle(actionName));
}

/*
@desc Get the number of origins for an analog action within an action set.
@param hInput The handle of the input you want to query.
@param hActionSet The handle of the action set you want to query.
@param hAnalogAction The handle of the analog action you want to query.
@return The number of origins.
@api ISteamController#GetAnalogActionOrigins
*/
extern "C" DLL_EXPORT int GetAnalogActionOriginCount(int hInput, int hActionSet, int hAnalogAction)
{
	ValidateInputHandle(hInput, 0);
	InputActionSetHandle_t actionSetHandle = SteamHandles()->GetSteamHandle(hActionSet);
	InputAnalogActionHandle_t analogActionHandle = SteamHandles()->GetSteamHandle(hAnalogAction);
	EInputActionOrigin origins[STEAM_INPUT_MAX_ORIGINS];
	return SteamInput()->GetAnalogActionOrigins(g_InputHandles[hInput], actionSetHandle, analogActionHandle, origins);
}

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
extern "C" DLL_EXPORT int GetAnalogActionOriginValue(int hInput, int hActionSet, int hAnalogAction, int index)
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

/*
@desc Enumerates currently connected controllers.

Must be called before controllers can be used because it loads the internal input handles.
@return The number of inputs found.
@api ISteamController#GetConnectedControllers
*/
extern "C" DLL_EXPORT int GetConnectedControllers()
{
	CheckInitialized(0);
	g_InputCount = SteamInput()->GetConnectedControllers(g_InputHandles);
	return g_InputCount;
}

/*
@desc Returns the associated input handle for the specified emulated gamepad.
@param index The index of the emulated gamepad you want to get an input handle for.
@return The associated input handle for the specified emulated gamepad.
@api ISteamController#GetControllerForGamepadIndex
*/
extern "C" DLL_EXPORT int GetControllerForGamepadIndex(int index)
{
	ControllerHandle_t handle = SteamInput()->GetControllerForGamepadIndex(index);
	for (int x = 0; x < g_InputCount; x++)
	{
		if (g_InputHandles[x] == handle)
		{
			return x + 1; // Input handles are 1-based!
		}
	}
	return 0;
}

/*
@desc Get the currently active action set for the specified input.
@param hInput The handle of the input you want to query.
@return The handle of the action set activated for the specified input.
@api ISteamController#GetCurrentActionSet
*/
extern "C" DLL_EXPORT int GetCurrentActionSet(int hInput)
{
	ValidateInputHandle(hInput, 0);
	return SteamHandles()->GetPluginHandle(SteamInput()->GetCurrentActionSet(g_InputHandles[hInput]));
}

/*
@desc Queries the digital action data for an input.
Also returns whether or not this action is currently available to be bound in the active action set (the same as GetDigitalActionDataActive).
@param hInput The handle of the input you want to query.
@param hDigitalAction The handle of the digital action you want to query.
@return 1 if this action is currently available; otherwise 0.
@api ISteamController#GetDigitalActionData, ISteamController#ControllerDigitalActionData_t
*/
extern "C" DLL_EXPORT int GetDigitalActionData(int hInput, int hDigitalAction)
{
	ValidateInputHandle(hInput, 0);
	g_InputDigitalActionData = SteamInput()->GetDigitalActionData(g_InputHandles[hInput], SteamHandles()->GetSteamHandle(hDigitalAction));
	return g_InputDigitalActionData.bActive;
}

/*
@desc Returns whether or not this action is currently available to be bound in the active action set.
@return 1 if this action is currently available; otherwise 0.
@api ISteamController#GetDigitalActionData, ISteamController#ControllerDigitalActionData_t
*/
extern "C" DLL_EXPORT int GetDigitalActionDataActive()
{
	return g_InputDigitalActionData.bActive;
}

/*
@desc Returns the current state of the digital game action read by the last GetDigitalActionData call.
@return The current state of this action; 1 if the action is currently pressed, otherwise 0.
@api ISteamController#GetDigitalActionData, ISteamController#ControllerDigitalActionData_t
*/
extern "C" DLL_EXPORT int GetDigitalActionDataState()
{
	return g_InputDigitalActionData.bState;
}

/*
@desc Get the handle of the specified digital action.

**NOTE:** This method does not take an action set handle parameter. That means that each action in your VDF file must have a unique string identifier.
In other words, if you use an action called "up" in two different action sets, this method will only ever return one of them and the other will be ignored.
@param actionName The string identifier of the digital action defined in the game's VDF file.
@return The handle of the specified digital action.
@api ISteamController#GetDigitalActionHandle
*/
extern "C" DLL_EXPORT int GetDigitalActionHandle(const char *actionName)
{
	CheckInitialized(0);
	return SteamHandles()->GetPluginHandle(SteamInput()->GetDigitalActionHandle(actionName));
}

/*
@desc Get the number of origins for a digital action within an action set.
@param hInput The handle of the input you want to query.
@param hActionSet The handle of the action set you want to query.
@param hDigitalAction The handle of the digital aciton you want to query.
@return A JSON integer array of the action origins.
@api ISteamController#GetDigitalActionOrigins
ISteamController#EControllerActionOrigin
*/
extern "C" DLL_EXPORT int GetDigitalActionOriginCount(int hInput, int hActionSet, int hDigitalAction)
{
	ValidateInputHandle(hInput, 0);
	InputActionSetHandle_t actionSetHandle = SteamHandles()->GetSteamHandle(hActionSet);
	InputDigitalActionHandle_t digitalActionHandle = SteamHandles()->GetSteamHandle(hDigitalAction);
	EInputActionOrigin origins[STEAM_INPUT_MAX_ORIGINS];
	return SteamInput()->GetDigitalActionOrigins(g_InputHandles[hInput], actionSetHandle, digitalActionHandle, origins);
}

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
extern "C" DLL_EXPORT int GetDigitalActionOriginValue(int hInput, int hActionSet, int hDigitalAction, int index)
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

/*
@desc Returns the associated gamepad index for the specified input, if emulating a gamepad.
@param hInput The handle of the input you want to get a gamepad index for.
@return An integer.
@api ISteamController#GetGamepadIndexForController
*/
extern "C" DLL_EXPORT int GetGamepadIndexForController(int hInput)
{
	ValidateInputHandle(hInput, 0);
	return SteamInput()->GetGamepadIndexForController(g_InputHandles[hInput]);
}

/*
@desc Get a local path to art for on-screen glyph for a particular origin.
@param eOrigin The origin you want to get the glyph for.
@param-api eOrigin ISteamController#EControllerActionOrigin
@return The path to the png file for the glyph.
@api ISteamController#GetGlyphForActionOrigin
*/
extern "C" DLL_EXPORT char *GetGlyphForActionOrigin(int eOrigin)
{
	CheckInitialized(NULL_STRING);
	return utils::CreateString(SteamInput()->GetGlyphForActionOrigin((EInputActionOrigin)eOrigin));
}

/*
@desc Returns the input type (device model) for the specified input. This tells you if a given input is a Steam controller, XBox 360 controller, PS4 controller, etc.
@param hInput The handle of the input.
@return Returns the input type (device model) for the specified input.
@api ISteamController#GetInputTypeForHandle
*/
extern "C" DLL_EXPORT int GetInputTypeForHandle(int hInput)
{
	ValidateInputHandle(hInput, k_ESteamInputType_Unknown);
	return SteamInput()->GetInputTypeForHandle(g_InputHandles[hInput]);
}

/*
@desc Reads the raw motion data for the specified input;
@param hInput The handle of the input you want to get motion data for.
@api ISteamController#GetDigitalActionData, ISteamController#ControllerDigitalActionData_t
*/
extern "C" DLL_EXPORT void GetMotionData(int hInput)
{
	ValidateInputHandle(hInput, );
	g_InputMotionData = SteamInput()->GetMotionData(g_InputHandles[hInput]);
}

/*
@desc Returns the positional acceleration, x axis for the last GetMotionData call.
@return Positional acceleration, x axis.
@api ISteamController#ControllerDigitalActionData_t
*/
extern "C" DLL_EXPORT float GetMotionDataPosAccelX()
{
	return g_InputMotionData.posAccelX;
}

/*
@desc Returns the positional acceleration, y axis for the last GetMotionData call
@return Positional acceleration, y axis.
@api ISteamController#ControllerDigitalActionData_t
*/
extern "C" DLL_EXPORT float GetMotionDataPosAccelY()
{
	return g_InputMotionData.posAccelY;
}

/*
@desc Returns the positional acceleration, z axis for the last GetMotionData call
@return Positional acceleration, z axis.
@api ISteamController#ControllerDigitalActionData_t
*/
extern "C" DLL_EXPORT float GetMotionDataPosAccelZ()
{
	return g_InputMotionData.posAccelZ;
}

/*
@desc Returns the sensor-fused absolute rotation (will drift in heading), w axis for the last GetMotionData call
GetMotionData MUST be called in order to populate the value returned by this method.
@return Sensor-fused absolute rotation (will drift in heading), w axis.
@api ISteamController#ControllerDigitalActionData_t
*/
extern "C" DLL_EXPORT float GetMotionDataRotQuatW()
{
	return g_InputMotionData.rotQuatW;
}

/*
@desc Returns the sensor-fused absolute rotation (will drift in heading), x axis for the last GetMotionData call
@return Sensor-fused absolute rotation (will drift in heading), x axis.
@api ISteamController#ControllerDigitalActionData_t
*/
extern "C" DLL_EXPORT float GetMotionDataRotQuatX()
{
	return g_InputMotionData.rotQuatX;
}

/*
@desc Returns the sensor-fused absolute rotation (will drift in heading), y axis for the last GetMotionData call
@return Sensor-fused absolute rotation (will drift in heading), y axis.
@api ISteamController#ControllerDigitalActionData_t
*/
extern "C" DLL_EXPORT float GetMotionDataRotQuatY()
{
	return g_InputMotionData.rotQuatY;
}

/*
@desc Returns the sensor-fused absolute rotation (will drift in heading), z axis for the last GetMotionData call
@return Sensor-fused absolute rotation (will drift in heading), z axis.
@api ISteamController#ControllerDigitalActionData_t
*/
extern "C" DLL_EXPORT float GetMotionDataRotQuatZ()
{
	return g_InputMotionData.rotQuatZ;
}

/*
@desc Returns the angular velocity, x axis for the last GetMotionData call
@return Angular velocity, x axis.
@api ISteamController#ControllerDigitalActionData_t
*/
extern "C" DLL_EXPORT float GetMotionDataRotVelX()
{
	return g_InputMotionData.rotVelX;
}

/*
@desc Returns the angular velocity, y axis for the last GetMotionData call
@return Angular velocity, y axis.
@api ISteamController#ControllerDigitalActionData_t
*/
extern "C" DLL_EXPORT float GetMotionDataRotVelY()
{
	return g_InputMotionData.rotVelY;

}
/*
@desc Returns the angular velocity, z axis for the last GetMotionData call
@return Angular velocity, z axis.
@api ISteamController#ControllerDigitalActionData_t
*/
extern "C" DLL_EXPORT float GetMotionDataRotVelZ()
{
	return g_InputMotionData.rotVelZ;
}

/*
@desc Returns a localized string (from Steam's language setting) for the specified origin.
@param eOrigin The origin you want to get the string for.
@return The localized string for the specified origin.
@api ISteamController#GetStringForActionOrigin, ISteamController#EControllerActionOrigin
*/
extern "C" DLL_EXPORT char *GetStringForActionOrigin(int eOrigin)
{
	CheckInitialized(NULL_STRING);
	return utils::CreateString(SteamInput()->GetStringForActionOrigin((EInputActionOrigin)eOrigin));
}

/*
@desc Must be called when starting use of the ISteamInput interface.
@return Always 1 if the Steam plugin has been initialized; otherwise 0.
@api ISteamController#Init
*/
extern "C" DLL_EXPORT int InitSteamInput()
{
	CheckInitialized(false);
	if (SteamInput()->Init())
	{
		// Give the API some time to refresh the inputs.
		SteamInput()->RunFrame();
		return true;
	}
	return false;
}

/*
@desc Synchronize API state with the latest Steam Input inputs available.
This is performed automatically by RunCallbacks, but for the absolute lowest possible latency, you can call this directly before reading input state.
@api ISteamController#RunFrame
*/
extern "C" DLL_EXPORT void RunFrame()
{
	SteamInput()->RunFrame();
}

/*
@desc Set the input LED color on supported inputs.
@param hInput The handle of the input to affect.
@param red The red component of the color to set (0-255).
@param green The green component of the color to set (0-255).
@param blue The blue component of the color to set (0-255).
@api ISteamController#SetLEDColor, ISteamController#ESteamControllerLEDFlag
*/
extern "C" DLL_EXPORT void SetInputLEDColor(int hInput, int red, int green, int blue)
{
	ValidateInputHandle(hInput, );
	Clamp(red, 0, UINT8_MAX);
	Clamp(green, 0, UINT8_MAX);
	Clamp(blue, 0, UINT8_MAX);
	SteamInput()->SetLEDColor(g_InputHandles[hInput], red, green, blue, k_ESteamInputLEDFlag_SetColor);
}

/*
@desc Set the input LED color back to the default (out-of-game) settings.
@param hInput The handle of the input to affect.
@api ISteamController#SetLEDColor, ISteamController#ESteamControllerLEDFlag
*/
extern "C" DLL_EXPORT void ResetInputLEDColor(int hInput)
{
	ValidateInputHandle(hInput, );
	SteamInput()->SetLEDColor(g_InputHandles[hInput], 0, 0, 0, k_ESteamInputLEDFlag_RestoreUserDefault);
}

//ShowAnalogActionOrigins - deprecated

/*
@desc Invokes the Steam overlay and brings up the binding screen.
@param hInput The handle of the input you want to bring up the binding screen for.
@return 1 for success; 0 if overlay is disabled/unavailable, or the user is not in Big Picture Mode.
@api ISteamController#ShowBindingPanel
*/
extern "C" DLL_EXPORT int ShowBindingPanel(int hInput)
{
	ValidateInputHandle(hInput, false);
	return SteamInput()->ShowBindingPanel(g_InputHandles[hInput]);
}

//ShowDigitalActionOrigins - deprecated

/*
@desc Must be called when ending use of the Steam Input interface.

This is called within Shutdown and also while unloading the plugin, so calling it explictly should not be necessary.
@return Always 1 if the Steam plugin has been initialized; otherwise 0.
@api ISteamController#Shutdown
*/
extern "C" DLL_EXPORT int ShutdownSteamInput()
{
	CheckInitialized(0);
	return SteamInput()->Shutdown();
}

/*
@desc Stops the momentum of an analog action (where applicable, ie a touchpad w/ virtual trackball settings).
@param hInput The handle of the input to affect.
@param hAnalogAction The analog action handle to stop momentum for.
@api ISteamController#StopAnalogActionMomentum
*/
extern "C" DLL_EXPORT void StopAnalogActionMomentum(int hInput, int hAnalogAction)
{
	ValidateInputHandle(hInput, );
	InputAnalogActionHandle_t analogActionHandle = SteamHandles()->GetSteamHandle(hAnalogAction);
	//ValidateActionHandle(InputAnalogActionHandle_t, analogActionHandle, hAnalogAction, m_AnalogActionHandles, );
	SteamInput()->StopAnalogActionMomentum(g_InputHandles[hInput], analogActionHandle);
}

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
extern "C" DLL_EXPORT void TriggerInputHapticPulse(int hInput, int eTargetPad, int duration)
{
	ValidateInputHandle(hInput, );
	Clamp(duration, 0, USHRT_MAX);
	SteamInput()->TriggerHapticPulse(g_InputHandles[hInput], (ESteamControllerPad)eTargetPad, (unsigned short)duration);
}

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
extern "C" DLL_EXPORT void TriggerInputRepeatedHapticPulse(int hInput, int eTargetPad, int onDuration, int offDuration, int repeat)
{
	ValidateInputHandle(hInput, );
	Clamp(onDuration, 0, USHRT_MAX);
	Clamp(offDuration, 0, USHRT_MAX);
	Clamp(repeat, 0, USHRT_MAX);
	SteamInput()->TriggerRepeatedHapticPulse(g_InputHandles[hInput], (ESteamControllerPad)eTargetPad, (unsigned short)onDuration, (unsigned short)offDuration, (unsigned short)repeat, 0);
}

/*
@desc Trigger a vibration event on supported inputs.

This API call will be ignored for incompatible input models.
@param hInput The handle of the input to affect.
@param leftSpeed The period of the left rumble motor's vibration, in microseconds.
@param rightSpeed The period of the right rumble motor's vibration, in microseconds.
@api ISteamController#TriggerVibration
*/
extern "C" DLL_EXPORT void TriggerInputVibration(int hInput, int leftSpeed, int rightSpeed)
{
	ValidateInputHandle(hInput, );
	Clamp(leftSpeed, 0, USHRT_MAX);
	Clamp(rightSpeed, 0, USHRT_MAX);
	SteamInput()->TriggerVibration(g_InputHandles[hInput], (unsigned short)leftSpeed, (unsigned short)rightSpeed);
}

// New for v1.43
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
extern "C" DLL_EXPORT int GetActionOriginFromXboxOrigin(int hInput, int eOrigin)
{
	ValidateInputHandle(hInput, k_EInputActionOrigin_None);
	return SteamInput()->GetActionOriginFromXboxOrigin(g_InputHandles[hInput], (EXboxOrigin)eOrigin);
}

/*
@desc Returns a localized string (from Steam's language setting) for the specified Xbox controller origin.
@param eOrigin The Xbox controller origin.
@param-api eOrigin ISteamInput#EXboxOrigin
@return The localized string for the specified Xbox controller origin.
@api ISteamInput#GetStringForXboxOrigin
*/
extern "C" DLL_EXPORT char *GetStringForXboxOrigin(int eOrigin)
{
	CheckInitialized(NULL_STRING);
	return utils::CreateString(SteamInput()->GetStringForXboxOrigin((EXboxOrigin)eOrigin));
}

/*
@desc Returns the local path to art for the on-screen glyph for a particular Xbox controller origin.
@param eOrigin The Xbox controller origin.
@param-api eOrigin ISteamInput#EXboxOrigin
@return The path to art for the on-screen glyph for a particular Xbox controller origin.
@api ISteamInput#GetGlyphForXboxOrigin
*/
extern "C" DLL_EXPORT char *GetGlyphForXboxOrigin(int eOrigin)
{
	CheckInitialized(NULL_STRING);
	return utils::CreateString(SteamInput()->GetGlyphForXboxOrigin((EXboxOrigin)eOrigin));
}

/*
@desc Convert an origin to another controller type.
@param eDestinationInputType The input type to convert from.
@param-api eDestinationInputType ISteamInput#ESteamInputType
@param eSourceOrigin The action origin to convert from.
@param-api eSourceOrigin ISteamInput#EInputActionOrigin
@return The action origin for the destination input type.
@api ISteamInput#TranslateActionOrigin, ISteamInput#EInputActionOrigin
*/
extern "C" DLL_EXPORT int TranslateActionOrigin(int eDestinationInputType, int eSourceOrigin)
{
	CheckInitialized(k_EInputActionOrigin_None);
	return SteamInput()->TranslateActionOrigin((ESteamInputType)eDestinationInputType, (EInputActionOrigin)eSourceOrigin);
}
