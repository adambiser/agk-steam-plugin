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

#include "CCallbacks.h"

// Adds the result to the end of the callback list.
#define StoreCallbackResult(callback, result)	\
	m_##callback##Mutex.lock();					\
	m_##callback##List.push_back(result);		\
	m_##callback##Mutex.unlock()

// Moves the front value of the list into the current value variable.
#define GetNextCallbackResult(callback)						\
	m_##callback##Mutex.lock();								\
	if (m_##callback##List.size() > 0)						\
	{														\
		m_Current##callback = m_##callback##List.front();	\
		m_##callback##List.pop_front();						\
		m_##callback##Mutex.unlock();						\
		return true;										\
	}														\
	Clear(m_Current##callback##);							\
	m_##callback##Mutex.unlock();							\
	return false

// Clears the callback list and current value variable.
#define ClearCallbackList(callback) \
	m_##callback##Mutex.lock();		\
	m_##callback##List.clear();		\
	Clear(m_Current##callback##);	\
	m_##callback##Mutex.unlock()

CCallbacks *Callbacks()
{
	static CCallbacks *callbacks = new CCallbacks();
	return callbacks;
}

CCallbacks::CCallbacks() :
	m_CallbackNewUrlLaunchParameters(this, &CCallbacks::OnNewUrlLaunchParameters),
	//m_CallbackDlcInstalled(this, &CCallbacks::OnDlcInstalled),
	m_CallbackGameOverlayActivated(this, &CCallbacks::OnGameOverlayActivated),
	m_CallbackAchievementStored(this, &CCallbacks::OnAchievementStored),
	//m_CallbackUserStatsReceived(this, &CCallbacks::OnUserStatsReceived),
	m_CallbackUserStatsStored(this, &CCallbacks::OnUserStatsStored),
	//m_CallbackAvatarImageLoaded(this, &CCallbacks::OnAvatarImageLoaded),
	//m_CallbackPersonaStateChange(this, &CCallbacks::OnPersonaStateChange),
	m_CallbackAchievementIconFetched(this, &CCallbacks::OnAchievementIconFetched),
	m_CallbackLobbyDataUpdate(this, &CCallbacks::OnLobbyDataUpdate),
	m_CallbackLobbyEnter(this, &CCallbacks::OnLobbyEnter),
	m_CallbackGameLobbyJoinRequested(this, &CCallbacks::OnGameLobbyJoinRequested),
	m_CallbackLobbyGameCreated(this, &CCallbacks::OnLobbyGameCreated),
	m_CallbackLobbyChatUpdate(this, &CCallbacks::OnLobbyChatUpdate),
	m_CallbackLobbyChatMessage(this, &CCallbacks::OnLobbyChatMessage),
	m_CallbackPlaybackStatusHasChanged(this, &CCallbacks::OnPlaybackStatusHasChanged),
	m_CallbackVolumeHasChanged(this, &CCallbacks::OnVolumeHasChanged),
	m_CallbackIPCountryChanged(this, &CCallbacks::OnIPCountryChanged),
	m_CallbackLowBatteryPower(this, &CCallbacks::OnLowBatteryPower),
	m_CallbackSteamShutdown(this, &CCallbacks::OnSteamShutdown),
	m_CallbackGamepadTextInputDismissed(this, &CCallbacks::OnGamepadTextInputDismissed)
{
	// ISteamApps
	m_bOnDlcInstalledEnabled = false;
	m_bOnUserStatsReceivedEnabled = false;
	ResetSessionVariables();
	EnableOnUserStatsReceivedCallback();
}

CCallbacks::~CCallbacks(void)
{
	this->ResetSessionVariables();
}

void CCallbacks::ResetSessionVariables()
{
	if (m_bOnUserStatsReceivedEnabled)
	{
		m_bOnUserStatsReceivedEnabled = false;
		m_CallbackUserStatsReceived.Unregister();
	}
	// Callback bools
	m_bIPCountryChanged = false;
	m_bLowBatteryWarning = false;
	m_bMusicPlaybackStatusChanged = false;
	m_bMusicVolumeChanged = false;
	//m_bNewLaunchQueryParameters = false;
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
	//ClearCallbackList(NewDlcInstalled);
	ClearCallbackList(PersonaStateChange);
	// Additional variables.
	m_AchievementIconsMap.clear();
	m_AchievementStored = false;
	//m_AvatarImageLoadedEnabled = false;
	m_nMinutesBatteryLeft = 255;
	m_RequestStatsCallbackState = Idle;
	m_StatsInitialized = false;
	m_StatsStored = false;
	m_StoreStatsCallbackState = Idle;
}

void CCallbacks::OnNewUrlLaunchParameters(NewUrlLaunchParameters_t *pParam)
{
	agk::Log("Callback: OnNewLaunchQueryParameters");
	m_bNewUrlLaunchParameters = true;
}

void CCallbacks::OnDlcInstalled(DlcInstalled_t *pParam)
{
	utils::Log("Callback: OnDlcInstalled: AppID = " + std::to_string(pParam->m_nAppID));
	StoreCallbackResult(DlcInstalled, *pParam);
}

bool CCallbacks::HasNewDlcInstalled()
{
	GetNextCallbackResult(DlcInstalled);
}

// Overlay methods
void CCallbacks::OnGameOverlayActivated(GameOverlayActivated_t *pParam)
{
	agk::Log("Callback: OnGameOverlayActivated");
	m_IsGameOverlayActive = pParam->m_bActive != 0;
}

// Callback for RequestUserInformation and more.
void CCallbacks::OnPersonaStateChange(PersonaStateChange_t *pParam)
{
	agk::Log("Callback: OnPersonaStateChange");
	StoreCallbackResult(PersonaStateChange, *pParam);
	if (pParam->m_nChangeFlags & k_EPersonaChangeAvatar)
	{
		// Allow HasAvatarImageLoaded to report avatar changes from here as well.
		if (m_bOnAvatarImageLoadedEnabled)
		{
			StoreCallbackResult(AvatarImageLoadedUser, pParam->m_ulSteamID);
		}
	}
}

bool CCallbacks::HasNewPersonaStateChange()
{
	GetNextCallbackResult(PersonaStateChange);
}

void CCallbacks::OnAvatarImageLoaded(AvatarImageLoaded_t *pParam)
{
	agk::Log("Callback: OnAvatarImageLoaded");
	StoreCallbackResult(AvatarImageLoadedUser, pParam->m_steamID);
}

bool CCallbacks::HasNewAvatarImageLoadedUser()
{
	GetNextCallbackResult(AvatarImageLoadedUser);
}

// Fires from CreateLobby and JoinLobby.
void CCallbacks::OnLobbyEnter(LobbyEnter_t *pParam)
{
	utils::Log("Callback: OnLobbyEnter");
	StoreCallbackResult(LobbyEnter, *pParam);
	g_JoinedLobbiesMutex.lock();
	g_JoinedLobbies.push_back(pParam->m_ulSteamIDLobby);
	g_JoinedLobbiesMutex.unlock();
}

bool CCallbacks::HasNewLobbyEnter()
{
	GetNextCallbackResult(LobbyEnter);
}

void CCallbacks::OnGameLobbyJoinRequested(GameLobbyJoinRequested_t *pParam)
{
	agk::Log("Callback: OnGameLobbyJoinRequested");
	StoreCallbackResult(GameLobbyJoinRequested, *pParam);
}

bool CCallbacks::HasNewGameLobbyJoinRequested()
{
	GetNextCallbackResult(GameLobbyJoinRequested);
}

void CCallbacks::OnLobbyGameCreated(LobbyGameCreated_t *pParam)
{
	utils::Log("Callback: OnLobbyGameCreated: " + std::to_string(pParam->m_ulSteamIDLobby));
	StoreCallbackResult(LobbyGameCreated, *pParam);
}

bool CCallbacks::HasNewLobbyGameCreated()
{
	GetNextCallbackResult(LobbyGameCreated);
}

void CCallbacks::OnLobbyDataUpdate(LobbyDataUpdate_t *pParam)
{
	utils::Log("Callback OnLobbyDataUpdate: " + std::string((pParam->m_bSuccess) ? "Succeeded" : "Failed"));
	StoreCallbackResult(LobbyDataUpdate, *pParam);
}

bool CCallbacks::HasNewLobbyDataUpdate()
{
	GetNextCallbackResult(LobbyDataUpdate);
}

void CCallbacks::OnLobbyChatUpdate(LobbyChatUpdate_t *pParam)
{
	agk::Log("Callback: OnLobbyChatUpdate");
	StoreCallbackResult(LobbyChatUpdate, *pParam);
}

bool CCallbacks::HasNewLobbyChatUpdate()
{
	GetNextCallbackResult(LobbyChatUpdate);
}

void CCallbacks::OnLobbyChatMessage(LobbyChatMsg_t *pParam)
{
	agk::Log("Callback: OnLobbyChatMessage");
	plugin::LobbyChatMsg_t info(*pParam);
	SteamMatchmaking()->GetLobbyChatEntry(info.m_ulSteamIDLobby, pParam->m_iChatID, NULL, info.m_chChatEntry, MAX_CHAT_MESSAGE_LENGTH, NULL);
	StoreCallbackResult(LobbyChatMessage, info);
}

bool CCallbacks::HasNewLobbyChatMessage()
{
	GetNextCallbackResult(LobbyChatMessage);
}

// Music methods
void CCallbacks::OnPlaybackStatusHasChanged(PlaybackStatusHasChanged_t *pParam)
{
	agk::Log("Callback: OnPlaybackStatusHasChanged");
	m_bMusicPlaybackStatusChanged = true;
}

void CCallbacks::OnVolumeHasChanged(VolumeHasChanged_t *pParam)
{
	agk::Log("Callback: OnVolumeHasChanged");
	m_bMusicVolumeChanged = true;
}

// Callback for RequestStats.
void CCallbacks::OnUserStatsReceived(UserStatsReceived_t *pCallback)
{
	agk::Log("OnUserStatsReceived");
	if (pCallback->m_nGameID == g_AppID)
	{
		utils::Log("Callback: OnUserStatsReceived " + std::string((pCallback->m_eResult == k_EResultOK) ? "Succeeded." : "Failed.  Result = " + std::to_string(pCallback->m_eResult)));
		if (pCallback->m_eResult == k_EResultOK)
		{
			m_RequestStatsCallbackState = Done;
			m_StatsInitialized = true;
		}
		else
		{
			m_RequestStatsCallbackState = ServerError;
		}
	}
}

// Callback for StoreStats and ResetAllStats.
void CCallbacks::OnUserStatsStored(UserStatsStored_t *pCallback)
{
	if (pCallback->m_nGameID == g_AppID)
	{
		utils::Log("Callback: OnUserStatsStored " + std::string((pCallback->m_eResult == k_EResultOK) ? "Succeeded." : "Failed.  Result = " + std::to_string(pCallback->m_eResult)));
		if (pCallback->m_eResult == k_EResultOK)
		{
			m_StoreStatsCallbackState = Done;
			m_StatsStored = true;
		}
		else
		{
			m_StoreStatsCallbackState = ServerError;
		}
	}
}

// Callback for StoreStats.
void CCallbacks::OnAchievementStored(UserAchievementStored_t *pCallback)
{
	if (pCallback->m_nGameID == g_AppID)
	{
		agk::Log("Callback: OnAchievementStored");
		m_AchievementStored = true;
	}
}

bool CCallbacks::StoreStats()
{
	CheckInitialized(SteamUserStats, false);
	// Fail when the callback is currently running.
	if (m_StoreStatsCallbackState == Running)
	{
		return false;
	}
	m_StatsStored = false;
	m_AchievementStored = false;
	if (!SteamUserStats()->StoreStats())
	{
		m_StoreStatsCallbackState = ClientError;
		return false;
	}
	m_StoreStatsCallbackState = Running;
	return true;
}

bool CCallbacks::ResetAllStats(bool bAchievementsToo)
{
	CheckInitialized(SteamUserStats, false);
	// Fail when the callback is currently running.
	if (m_StoreStatsCallbackState == Running)
	{
		return false;
	}
	m_StatsStored = false;
	m_AchievementStored = false;
	if (!SteamUserStats()->ResetAllStats(bAchievementsToo))
	{
		m_StoreStatsCallbackState = ClientError;
		return false;
	}
	m_StoreStatsCallbackState = Running;
	return true;
}

// Callback for GetAchievementIcon.
void CCallbacks::OnAchievementIconFetched(UserAchievementIconFetched_t *pParam)
{
	if (pParam->m_nGameID.AppID() == g_AppID)
	{
		agk::Log("Callback: OnAchievementIconFetched");
		// Only store the results for values that are expected.
		std::string name = pParam->m_rgchAchievementName;
		auto search = m_AchievementIconsMap.find(name);
		if (search != m_AchievementIconsMap.end())
		{
			search->second = pParam->m_nIconHandle;
		}
	}
}

/*
In the Steam API, GetAchievementIcon reports failure and "no icon" the same way.
Instead this checks some failure conditions so that it can return 0 for failure and -1 when waiting on a callback.
*/
int CCallbacks::GetAchievementIcon(const char *pchName)
{
	CheckInitialized(SteamUserStats, 0);
	// See if we're waiting for a callback result for this icon.
	std::string name = pchName;
	auto search = m_AchievementIconsMap.find(name);
	if (search != m_AchievementIconsMap.end())
	{
		// If we have a result from the callback, remove it from the wait list.
		if ((search->second) != -1)
		{
			m_AchievementIconsMap.erase(search);
		}
		return search->second;
	}
	int hImage = SteamUserStats()->GetAchievementIcon(pchName);
	if (hImage == 0)
	{
		// Check to see if this is a valid achievement name.
		bool pbAchieved;
		if (!SteamUserStats()->GetAchievement(pchName, &pbAchieved))
		{
			// Not valid, return 0 for no icon.
			return 0;
		}
		hImage = -1; // Use -1 to indicate that we're waiting on a callback to provide the handle.
		//m_AchievementIconsMap.insert_or_assign(name, hImage);
		m_AchievementIconsMap[name] = hImage;
	}
	return hImage;
}

// Utils
void CCallbacks::OnIPCountryChanged(IPCountry_t *pParam)
{
	agk::Log("Callback: IP Country Changed");
	m_bIPCountryChanged = true;
}

void CCallbacks::OnLowBatteryPower(LowBatteryPower_t *pParam)
{
	agk::Log("Callback: Low Battery Power Warning");
	m_bLowBatteryWarning = true;
	m_nMinutesBatteryLeft = pParam->m_nMinutesBatteryLeft;
}

void CCallbacks::OnSteamShutdown(SteamShutdown_t *pParam)
{
	agk::Log("Callback: Steam Shutdown");
	m_bSteamShutdownNotification = true;
}

// Big Picture Mode methods
void CCallbacks::OnGamepadTextInputDismissed(GamepadTextInputDismissed_t *pCallback)
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

bool CCallbacks::HasNewGamepadTextInputDismissed()
{
	GetNextCallbackResult(GamepadTextInputDismissed);
}

