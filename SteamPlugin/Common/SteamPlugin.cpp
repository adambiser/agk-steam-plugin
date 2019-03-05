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

template <typename M> void FreeClearMap(M & map)
{
	for (typename M::iterator iter = map.begin(); iter != map.end(); iter++) {
		delete iter->second;
	}
	map.clear();
}

SteamPlugin::SteamPlugin() :
	m_AppID(0),
	m_SteamInitialized(false),
	m_HasNewLaunchQueryParameters(false),
	m_CallbackNewLaunchQueryParameters(this, &SteamPlugin::OnNewLaunchQueryParameters),
	m_OnDlcInstalledEnabled(false),
	m_NewDlcInstalled(0),
	m_CallbackDlcInstalled(this, &SteamPlugin::OnDlcInstalled),
	m_IsGameOverlayActive(false),
	m_CallbackGameOverlayActivated(this, &SteamPlugin::OnGameOverlayActivated),
	m_AvatarImageLoadedEnabled(false),
	m_AvatarUser(k_steamIDNil),
	m_PersonaStateChangedEnabled(false),
	m_RequestStatsCallbackState(Idle),
	m_StatsInitialized(false),
	m_StoreStatsCallbackState(Idle),
	m_StatsStored(false),
	m_AchievementStored(false),
	m_FindLeaderboardCallbackState(Idle),
	m_UploadLeaderboardScoreCallbackState(Idle),
	m_DownloadLeaderboardEntriesCallbackState(Idle),
	m_CallbackAchievementStored(this, &SteamPlugin::OnAchievementStored),
	m_CallbackUserStatsReceived(this, &SteamPlugin::OnUserStatsReceived),
	m_CallbackUserStatsStored(this, &SteamPlugin::OnUserStatsStored),
	m_CallbackAvatarImageLoaded(this, &SteamPlugin::OnAvatarImageLoaded),
	m_CallbackPersonaStateChanged(this, &SteamPlugin::OnPersonaStateChanged),
	m_CallbackAchievementIconFetched(this, &SteamPlugin::OnAchievementIconFetched),
	m_LobbyMatchListCallbackState(Idle),
	m_CallResultLobbyDataUpdate(this, &SteamPlugin::OnLobbyDataUpdated),
	m_LobbyDataUpdatedLobby(k_steamIDNil),
	m_LobbyDataUpdatedID(k_steamIDNil),
	m_LobbyCreateCallbackState(Idle),
	m_LobbyCreatedID(k_steamIDNil),
	m_LobbyCreatedResult((EResult)0),
	m_LobbyEnterCallbackState(Idle),
	m_MainCallResultLobbyEnter(this, &SteamPlugin::OnLobbyEnter),
	m_LobbyEnterID(k_steamIDNil),
	m_LobbyEnterBlocked(false),
	m_LobbyEnterResponse((EChatRoomEnterResponse)0),
	m_CallbackGameLobbyJoinRequested(this, &SteamPlugin::OnGameLobbyJoinRequested),
	m_CallbackLobbyGameCreated(this, &SteamPlugin::OnLobbyGameCreated),
	m_CallbackLobbyChatUpdated(this, &SteamPlugin::OnLobbyChatUpdated),
	m_LobbyChatUpdateUserChanged(k_steamIDNil),
	m_LobbyChatUpdateUserState((EChatMemberStateChange)0),
	m_LobbyChatUpdateUserMakingChange(k_steamIDNil),
	m_CallbackLobbyChatMessage(this, &SteamPlugin::OnLobbyChatMessage),
	m_LobbyChatMessageUser(k_steamIDNil),
	m_CallbackPlaybackStatusHasChanged(this, &SteamPlugin::OnPlaybackStatusHasChanged),
	m_PlaybackStatusHasChanged(false),
	m_CallbackVolumeHasChanged(this, &SteamPlugin::OnVolumeHasChanged),
	m_VolumeHasChanged(false),
	m_HasIPCountryChanged(false),
	m_CallbackIPCountryChanged(this, &SteamPlugin::OnIPCountryChanged),
	m_HasLowBatteryWarning(false),
	m_nMinutesBatteryLeft(255),
	m_CallbackLowBatteryPower(this, &SteamPlugin::OnLowBatteryPower),
	m_IsSteamShuttingDown(false),
	m_CallbackSteamShutdown(this, &SteamPlugin::OnSteamShutdown),
	m_CallbackGamepadTextInputDismissed(this, &SteamPlugin::OnGamepadTextInputDismissed)
{
	m_DigitalActionData.bActive = false;
	m_DigitalActionData.bState = false;
	m_AnalogActionData.bActive = false;
	m_AnalogActionData.eMode = k_EInputSourceMode_None;
	m_AnalogActionData.x = 0;
	m_AnalogActionData.y = 0;
	m_MotionData.posAccelX = 0;
	m_MotionData.posAccelY = 0;
	m_MotionData.posAccelZ = 0;
	m_MotionData.rotQuatW = 0;
	m_MotionData.rotQuatX = 0;
	m_MotionData.rotQuatY = 0;
	m_MotionData.rotQuatZ = 0;
	m_MotionData.rotVelX = 0;
	m_MotionData.rotVelY = 0;
	m_MotionData.rotVelZ = 0;
}

SteamPlugin::~SteamPlugin(void)
{
	this->Shutdown();
}

bool SteamPlugin::Init()
{
	if (!m_SteamInitialized)
	{
		m_SteamInitialized = SteamAPI_Init();
		if (m_SteamInitialized)
		{
			m_AppID = SteamUtils()->GetAppID();
		}
	}
	return m_SteamInitialized;
}

void SteamPlugin::Shutdown()
{
	if (m_SteamInitialized)
	{
		ShutdownSteamInput();
		// Disconnect from any lobbies.
		while (m_JoinedLobbies.size() > 0)
		{
			LeaveLobby(m_JoinedLobbies.back());
			m_JoinedLobbies.pop_back();
		}
		SteamAPI_Shutdown();
		// Reset member variables.
		m_AppID = 0;
		m_SteamInitialized = false;
		m_HasNewLaunchQueryParameters = false;
		m_OnDlcInstalledEnabled = false;
		m_NewDlcInstalled = 0;
		m_DlcInstalledList.clear();
		m_IsGameOverlayActive = false;
		m_AvatarImageLoadedEnabled = false;
		m_AvatarImageLoadedUsers.clear();
		m_AvatarUser = k_steamIDNil;
		m_PersonaStateChangedEnabled = false;
		m_PersonaStateChangeList.clear();
		//m_PersonaStateChange
		m_RequestStatsCallbackState = Idle;
		m_StatsInitialized = false;
		m_StoreStatsCallbackState = Idle;
		m_StatsStored = false;
		m_AchievementStored = false;
		m_AchievementIconsMap.clear();
		m_FindLeaderboardCallbackState = Idle;
		//m_LeaderboardFindResult
		m_UploadLeaderboardScoreCallbackState = Idle;
		//m_LeaderboardScoreUploaded
		m_DownloadLeaderboardEntriesCallbackState = Idle;
		m_DownloadedLeaderboardEntries.clear();
		m_LobbyMatchListCallbackState = Idle;
		//m_LobbyMatchList
		m_LobbyDataUpdatedLobby = k_steamIDNil;
		m_LobbyDataUpdatedID = k_steamIDNil;
		m_LobbyDataUpdated.clear();
		m_LobbyCreateCallbackState = Idle;
		m_LobbyCreatedID = k_steamIDNil;
		m_LobbyCreatedResult = (EResult)0;
		m_JoinedLobbies.clear();
		m_LobbyEnterCallbackState = Idle;
		m_LobbyEnterID = k_steamIDNil;
		m_LobbyEnterBlocked = false;
		m_LobbyEnterResponse = (EChatRoomEnterResponse)0;
		m_LobbyChatUpdateUserChanged = k_steamIDNil;
		m_LobbyChatUpdateUserState = (EChatMemberStateChange)0;
		m_LobbyChatUpdateUserMakingChange = k_steamIDNil;
		m_ChatUpdates.clear();
		m_LobbyChatMessageUser = k_steamIDNil;
		m_PlaybackStatusHasChanged = false;
		m_VolumeHasChanged = false;
		m_HasIPCountryChanged = false;
		m_HasLowBatteryWarning = false;
		m_nMinutesBatteryLeft = 255;
		m_IsSteamShuttingDown = false;
		FreeClearMap(m_FileWriteAsyncItemMap);
		FreeClearMap(m_FileReadAsyncItemMap);
		m_DigitalActionData.bActive = false;
		m_DigitalActionData.bState = false;
		m_AnalogActionData.bActive = false;
		m_AnalogActionData.eMode = k_EInputSourceMode_None;
		m_AnalogActionData.x = 0;
		m_AnalogActionData.y = 0;
		m_MotionData.posAccelX = 0;
		m_MotionData.posAccelY = 0;
		m_MotionData.posAccelZ = 0;
		m_MotionData.rotQuatW = 0;
		m_MotionData.rotQuatX = 0;
		m_MotionData.rotQuatY = 0;
		m_MotionData.rotQuatZ = 0;
		m_MotionData.rotVelX = 0;
		m_MotionData.rotVelY = 0;
		m_MotionData.rotVelZ = 0;
	}
}