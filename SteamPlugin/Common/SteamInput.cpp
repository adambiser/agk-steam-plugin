#include "SteamPlugin.h"

/*
Input Information
*/
bool SteamPlugin::InitSteamInput()
{
	CheckInitialized(SteamInput, 0);
	if (SteamInput()->Init())
	{
		// Give the API some time to refresh the inputs.
		SteamInput()->RunFrame();
		return true;
	}
	return false;
}

bool SteamPlugin::ShutdownSteamInput()
{
	CheckInitialized(SteamInput, 0);
	return SteamInput()->Shutdown();
}

int SteamPlugin::GetConnectedControllers(InputHandle_t *handlesOut)
{
	CheckInitialized(SteamInput, 0);
	return SteamInput()->GetConnectedControllers(handlesOut);
}

ESteamInputType SteamPlugin::GetInputTypeForHandle(InputHandle_t inputHandle)
{
	CheckInitialized(SteamInput, k_ESteamInputType_Unknown);
	return SteamInput()->GetInputTypeForHandle(inputHandle);
}

void SteamPlugin::RunFrame()
{
	SteamInput()->RunFrame();
}

bool SteamPlugin::ShowBindingPanel(InputHandle_t inputHandle)
{
	CheckInitialized(SteamInput, false);
	return SteamInput()->ShowBindingPanel(inputHandle);
}

/*
Input Action Sets and Layers
*/
void SteamPlugin::ActivateActionSet(InputHandle_t inputHandle, InputActionSetHandle_t actionSetHandle)
{
	CheckInitialized(SteamInput, );
	SteamInput()->ActivateActionSet(inputHandle, actionSetHandle);
}

InputActionSetHandle_t SteamPlugin::GetActionSetHandle(const char *pszActionSetName)
{
	CheckInitialized(SteamInput, 0);
	return SteamInput()->GetActionSetHandle(pszActionSetName);
}

InputActionSetHandle_t SteamPlugin::GetCurrentActionSet(InputHandle_t inputHandle)
{
	CheckInitialized(SteamInput, 0);
	return SteamInput()->GetCurrentActionSet(inputHandle);
}

void SteamPlugin::ActivateActionSetLayer(InputHandle_t inputHandle, InputActionSetHandle_t actionSetLayerHandle)
{
	CheckInitialized(SteamInput, );
	SteamInput()->ActivateActionSetLayer(inputHandle, actionSetLayerHandle);
}

void SteamPlugin::DeactivateActionSetLayer(InputHandle_t inputHandle, InputActionSetHandle_t actionSetLayerHandle)
{
	CheckInitialized(SteamInput, );
	SteamInput()->DeactivateActionSetLayer(inputHandle, actionSetLayerHandle);
}

void SteamPlugin::DeactivateAllActionSetLayers(InputHandle_t inputHandle)
{
	CheckInitialized(SteamInput, );
	SteamInput()->DeactivateAllActionSetLayers(inputHandle);
}

int SteamPlugin::GetActiveActionSetLayers(InputHandle_t inputHandle, InputActionSetHandle_t *handlesOut)
{
	CheckInitialized(SteamInput, 0);
	return SteamInput()->GetActiveActionSetLayers(inputHandle, handlesOut);
}

/*
Input Actions and Motion
*/
void SteamPlugin::GetAnalogActionData(InputHandle_t inputHandle, InputAnalogActionHandle_t analogActionHandle)
{
	CheckInitialized(SteamInput, );
	m_AnalogActionData = SteamInput()->GetAnalogActionData(inputHandle, analogActionHandle);
}

InputAnalogActionHandle_t SteamPlugin::GetAnalogActionHandle(const char *pszActionName)
{
	CheckInitialized(SteamInput, 0);
	return SteamInput()->GetAnalogActionHandle(pszActionName);
}

void SteamPlugin::StopAnalogActionMomentum(InputHandle_t inputHandle, InputAnalogActionHandle_t analogActionHandle)
{
	CheckInitialized(SteamInput, );
	return SteamInput()->StopAnalogActionMomentum(inputHandle, analogActionHandle);
}

void SteamPlugin::GetDigitalActionData(InputHandle_t inputHandle, InputDigitalActionHandle_t digitalActionHandle)
{
	CheckInitialized(SteamInput, );
	m_DigitalActionData = SteamInput()->GetDigitalActionData(inputHandle, digitalActionHandle);
}

InputDigitalActionHandle_t SteamPlugin::GetDigitalActionHandle(const char *pszActionName)
{
	CheckInitialized(SteamInput, 0);
	return SteamInput()->GetDigitalActionHandle(pszActionName);
}

void SteamPlugin::GetMotionData(InputHandle_t inputHandle)
{
	CheckInitialized(SteamInput, );
	m_MotionData = SteamInput()->GetMotionData(inputHandle);
}

/*
Input Action Origins.
*/
int SteamPlugin::GetAnalogActionOrigins(InputHandle_t inputHandle, InputActionSetHandle_t actionSetHandle, InputAnalogActionHandle_t analogActionHandle, EInputActionOrigin *originsOut)
{
	CheckInitialized(SteamInput, 0);
	return SteamInput()->GetAnalogActionOrigins(inputHandle, actionSetHandle, analogActionHandle, originsOut);
}

int SteamPlugin::GetDigitalActionOrigins(InputHandle_t inputHandle, InputActionSetHandle_t actionSetHandle, InputDigitalActionHandle_t digitalActionHandle, EInputActionOrigin *originsOut)
{
	CheckInitialized(SteamInput, 0);
	return SteamInput()->GetDigitalActionOrigins(inputHandle, actionSetHandle, digitalActionHandle, originsOut);
}

const char *SteamPlugin::GetGlyphForActionOrigin(EInputActionOrigin eOrigin)
{
	CheckInitialized(SteamInput, NULL);
	return SteamInput()->GetGlyphForActionOrigin(eOrigin);
}

const char *SteamPlugin::GetStringForActionOrigin(EInputActionOrigin eOrigin)
{
	CheckInitialized(SteamInput, NULL);
	return SteamInput()->GetStringForActionOrigin(eOrigin);
}

EInputActionOrigin SteamPlugin::GetActionOriginFromXboxOrigin(InputHandle_t inputHandle, EXboxOrigin eOrigin)
{
	CheckInitialized(SteamInput, k_EInputActionOrigin_None);
	return SteamInput()->GetActionOriginFromXboxOrigin(inputHandle, eOrigin);
}

const char *SteamPlugin::GetStringForXboxOrigin(EXboxOrigin eOrigin)
{
	CheckInitialized(SteamInput, NULL);
	return SteamInput()->GetStringForXboxOrigin(eOrigin);
}

const char *SteamPlugin::GetGlyphForXboxOrigin(EXboxOrigin eOrigin)
{
	CheckInitialized(SteamInput, NULL);
	return SteamInput()->GetGlyphForXboxOrigin(eOrigin);
}

EInputActionOrigin SteamPlugin::TranslateActionOrigin(ESteamInputType eDestinationInputType, EInputActionOrigin eSourceOrigin)
{
	CheckInitialized(SteamInput, k_EInputActionOrigin_None);
	return SteamInput()->TranslateActionOrigin(eDestinationInputType, eSourceOrigin);
}

/*
Input Effects
*/
void SteamPlugin::SetLEDColor(InputHandle_t inputHandle, uint8 nColorR, uint8 nColorG, uint8 nColorB, unsigned int nFlags)
{
	CheckInitialized(SteamInput, );
	SteamInput()->SetLEDColor(inputHandle, nColorR, nColorG, nColorB, nFlags);
}

void SteamPlugin::TriggerHapticPulse(InputHandle_t inputHandle, ESteamControllerPad eTargetPad, unsigned short duration)
{
	CheckInitialized(SteamInput, );
	SteamInput()->TriggerHapticPulse(inputHandle, eTargetPad, duration);
}

void SteamPlugin::TriggerRepeatedHapticPulse(InputHandle_t inputHandle, ESteamControllerPad eTargetPad, unsigned short usDurationMicroSec, unsigned short usOffMicroSec, unsigned short unRepeat, unsigned int nFlags)
{
	CheckInitialized(SteamInput, );
	SteamInput()->TriggerRepeatedHapticPulse(inputHandle, eTargetPad, usDurationMicroSec, usOffMicroSec, unRepeat, nFlags);
}

void SteamPlugin::TriggerVibration(InputHandle_t inputHandle, unsigned short usLeftSpeed, unsigned short usRightSpeed)
{
	CheckInitialized(SteamInput, );
	SteamInput()->TriggerVibration(inputHandle, usLeftSpeed, usRightSpeed);
}
