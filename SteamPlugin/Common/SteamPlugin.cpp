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

#include "SteamPlugin.h"

SteamPlugin::SteamPlugin() :
	m_CallbackNewLaunchQueryParameters(this, &SteamPlugin::OnNewLaunchQueryParameters),
	m_CallbackDlcInstalled(this, &SteamPlugin::OnDlcInstalled),
	m_CallbackGameOverlayActivated(this, &SteamPlugin::OnGameOverlayActivated),
	m_CallbackAchievementStored(this, &SteamPlugin::OnAchievementStored),
	m_CallbackUserStatsReceived(this, &SteamPlugin::OnUserStatsReceived),
	m_CallbackUserStatsStored(this, &SteamPlugin::OnUserStatsStored),
	m_CallbackAvatarImageLoaded(this, &SteamPlugin::OnAvatarImageLoaded),
	m_CallbackPersonaStateChange(this, &SteamPlugin::OnPersonaStateChange),
	m_CallbackAchievementIconFetched(this, &SteamPlugin::OnAchievementIconFetched),
	m_CallResultLobbyDataUpdate(this, &SteamPlugin::OnLobbyDataUpdate),
	m_MainCallResultLobbyEnter(this, &SteamPlugin::OnLobbyEnter),
	m_CallbackGameLobbyJoinRequested(this, &SteamPlugin::OnGameLobbyJoinRequested),
	m_CallbackLobbyGameCreated(this, &SteamPlugin::OnLobbyGameCreated),
	m_CallbackLobbyChatUpdate(this, &SteamPlugin::OnLobbyChatUpdate),
	m_CallbackLobbyChatMessage(this, &SteamPlugin::OnLobbyChatMessage),
	m_CallbackPlaybackStatusHasChanged(this, &SteamPlugin::OnPlaybackStatusHasChanged),
	m_CallbackVolumeHasChanged(this, &SteamPlugin::OnVolumeHasChanged),
	m_CallbackIPCountryChanged(this, &SteamPlugin::OnIPCountryChanged),
	m_CallbackLowBatteryPower(this, &SteamPlugin::OnLowBatteryPower),
	m_CallbackSteamShutdown(this, &SteamPlugin::OnSteamShutdown),
	m_CallbackGamepadTextInputDismissed(this, &SteamPlugin::OnGamepadTextInputDismissed)
{
	ResetSessionVariables();
	// TODO REMOVE!
	m_AppID = 480;
}

SteamPlugin::~SteamPlugin(void)
{
	this->Shutdown();
}

void SteamPlugin::Shutdown()
{
	if (m_SteamInitialized)
	{
		// Disconnect from any lobbies.
		while (m_JoinedLobbies.size() > 0)
		{
			LeaveLobby(m_JoinedLobbies.back());
		}
		// Clear any remaining callback items.
		for (auto iter = m_CallResultItems.begin(); iter != m_CallResultItems.end(); iter++) {
			delete iter->second;
		}
		ResetSessionVariables();
	}
}

void SteamPlugin::ResetSessionVariables()
{
	// CallResults
	m_CallResultItems.clear();
	m_CurrentCallResultHandle = 0;
	// Callback bools
	m_bIPCountryChanged = false;
	m_bLowBatteryWarning = false;
	m_bMusicPlaybackStatusChanged = false;
	m_bMusicVolumeChanged = false;
	m_bNewLaunchQueryParameters = false;
	m_bSteamShutdownNotification = false;
	// Callback lists.
	ClearCallbackList(AvatarImageLoadedUser);
	ClearCallbackList(GameLobbyJoinRequested);
	ClearCallbackList(GamepadTextInputDismissed);
	ClearCallbackList(LobbyChatMessage);
	ClearCallbackList(LobbyChatUpdate);
	ClearCallbackList(LobbyDataUpdate);
	ClearCallbackList(LobbyEnter);
	ClearCallbackList(LobbyGameCreated);
	ClearCallbackList(NewDlcInstalled);
	ClearCallbackList(PersonaStateChange);
	// Input 
	Clear(m_AnalogActionData);
	Clear(m_DigitalActionData);
	Clear(m_MotionData);
	// Additional variables.
	m_AchievementIconsMap.clear();
	m_AchievementStored = false;
	m_AppID = 0;
	m_AvatarImageLoadedEnabled = false;
	m_IsGameOverlayActive = false;
	m_JoinedLobbies.clear();
	m_nMinutesBatteryLeft = 255;
	m_OnDlcInstalledEnabled = false;
	m_PersonaStateChangedEnabled = false;
	m_RequestStatsCallbackState = Idle;
	m_StatsInitialized = false;
	m_StatsStored = false;
	m_SteamInitialized = false;
	m_StoreStatsCallbackState = Idle;
}

int SteamPlugin::AddCallResultItem(CCallResultItem *callResult)
{
	if (callResult == NULL)
	{
		return 0;
	}
	while (m_CallResultItems.find(++m_CurrentCallResultHandle) != m_CallResultItems.end());
	m_CallResultItems[m_CurrentCallResultHandle] = callResult;
	return m_CurrentCallResultHandle;
}

void SteamPlugin::DeleteCallResultItem(int hCallResult)
{
	auto search = m_CallResultItems.find(hCallResult);
	if (search != m_CallResultItems.end())
	{
		delete search->second;
		m_CallResultItems.erase(search);
	}
}

ECallbackState SteamPlugin::GetCallResultItemState(int hCallResult)
{
	if (CCallResultItem *callResult = GetCallResultItem<CCallResultItem>(hCallResult))
	{
		return callResult->GetState();
	}
	return ClientError;
}
