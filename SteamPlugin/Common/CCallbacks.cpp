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
#include "StructClear.h"

// Clears the callback list and current value variable.
#define CLEAR_CALLBACK_LIST(func)			\
	m_##func##Mutex.lock();					\
	m_##func##List.clear();					\
	Clear(m_Current##func##);				\
	if (m_b##func##Enabled)					\
	{										\
		m_b##func##Enabled = false;			\
		m_Callback##func##.Unregister();	\
	}										\
	m_##func##Mutex.unlock()

#define CLEAR_CALLBACK_BOOL(func)			\
	m_b##func = false

// Adds the result to the end of the callback list.
#define STORE_CALLBACK_RESULT(func, result)	\
	m_##func##Mutex.lock();					\
	m_##func##List.push_back(result);		\
	m_##func##Mutex.unlock()

// Moves the front value of the list into the current value variable.
#define CALLBACK_LIST_METHODS(func, list_type)					\
	void CCallbacks::Enable##func##Callback()					\
	{															\
		if (!m_b##func##Enabled)								\
		{														\
			m_b##func##Enabled = true;							\
			m_Callback##func##.Register(this, &CCallbacks::##func##);\
		}														\
	}															\
	\
	bool CCallbacks::Has##func##()								\
	{															\
		m_##func##Mutex.lock();									\
		if (m_##func##List.size() > 0)							\
		{														\
			m_Current##func = m_##func##List.front();			\
			m_##func##List.pop_front();							\
			m_##func##Mutex.unlock();							\
			return true;										\
		}														\
		Clear(m_Current##func##);								\
		m_##func##Mutex.unlock();								\
		return false;											\
	}															\
	\
	list_type CCallbacks::Get##func##()							\
	{															\
		return m_Current##func##;								\
	}

#define CALLBACK_BOOL_METHODS(func, callback_type)				\
	void CCallbacks::##func##(callback_type *pParam)			\
	{															\
		agk::Log("Callback: " #func);							\
		m_b##func## = true;										\
	}															\
	\
	bool CCallbacks::Has##func##()		\
	{									\
		if (m_b##func##)				\
		{								\
			m_b##func## = false;		\
			return true;				\
		}								\
		return false;					\
	}

CCallbacks *Callbacks()
{
	static CCallbacks *callbacks = new CCallbacks();
	return callbacks;
}

CCallbacks::CCallbacks() //:
	// All STEAM_CALLBACK and CALLBACK_BOOL go here.
	//m_CallbackOnNewUrlLaunchParameters(this, &CCallbacks::OnNewUrlLaunchParameters),
	//m_CallbackGameOverlayActivated(this, &CCallbacks::OnGameOverlayActivated),
	//m_CallbackOnPlaybackStatusHasChanged(this, &CCallbacks::OnPlaybackStatusHasChanged),
	//m_CallbackOnVolumeHasChanged(this, &CCallbacks::OnVolumeHasChanged),
	//m_CallbackUserAchievementIconFetched(this, &CCallbacks::OnUserAchievementIconFetched)//,
	//m_CallbackOnIPCountryChanged(this, &CCallbacks::OnIPCountryChanged),
	//m_CallbackOnLowBatteryPower(this, &CCallbacks::OnLowBatteryPower),
	//m_CallbackOnSteamShutdown(this, &CCallbacks::OnSteamShutdown)
{
	ResetSessionVariables();
}

CCallbacks::~CCallbacks(void)
{
	this->ResetSessionVariables();
}

void CCallbacks::ResetSessionVariables()
{
	// ISteamApps
	CLEAR_CALLBACK_LIST(OnDlcInstalled);
	CLEAR_CALLBACK_BOOL(OnNewUrlLaunchParameters);

	// ISteamAppTicket
	// ISteamClient
	// ISteamController/ISteamInput

	// ISteamFriends
	CLEAR_CALLBACK_LIST(OnAvatarImageLoaded);
	CLEAR_CALLBACK_LIST(OnGameLobbyJoinRequested);
	//STEAM_CALLBACK(CCallbacks, OnGameOverlayActivated, GameOverlayActivated_t, m_CallbackGameOverlayActivated);
	m_IsGameOverlayActive = false;
	CLEAR_CALLBACK_LIST(OnPersonaStateChange);

	// ISteamGameCoordinator
	// ISteamGameServer
	// ISteamGameServerStats
	// ISteamHTMLSurface
	// ISteamHTTP
	// ISteamInventory

	// ISteamMatchmaking
	CLEAR_CALLBACK_LIST(OnLobbyChatMessage);
	CLEAR_CALLBACK_LIST(OnLobbyChatUpdate);
	CLEAR_CALLBACK_LIST(OnLobbyDataUpdate);
	CLEAR_CALLBACK_LIST(OnLobbyEnter);
	CLEAR_CALLBACK_LIST(OnLobbyGameCreated);

	// ISteamMatchmakingServers

	// ISteamMusic
	CLEAR_CALLBACK_BOOL(OnPlaybackStatusHasChanged);
	CLEAR_CALLBACK_BOOL(OnVolumeHasChanged);

	// ISteamMusicRemote
	// ISteamNetworking
	// ISteamNetworkingSockets
	// ISteamNetworkingUtils
	// ISteamParties
	// ISteamRemoteStorage
	// ISteamScreenshots
	// ISteamUGC
	// ISteamUnifiedMessages
	// ISteamUser

	// ISteamUserStats
	//STEAM_CALLBACK(CCallbacks, OnUserAchievementIconFetched, UserAchievementIconFetched_t, m_CallbackUserAchievementIconFetched);
	m_AchievementIconsMap.clear();
	CLEAR_CALLBACK_LIST(OnUserAchievementStored);
	CLEAR_CALLBACK_LIST(OnUserStatsReceived);
	m_StatsInitialized = false;
	CLEAR_CALLBACK_LIST(OnUserStatsStored);
	// ISteamUtils
	// CheckFileSignature_t - Call result for  CheckFileSignature.
	CLEAR_CALLBACK_LIST(OnGamepadTextInputDismissed);
	CLEAR_CALLBACK_BOOL(OnIPCountryChanged);
	CLEAR_CALLBACK_BOOL(OnLowBatteryPower);
	m_nMinutesBatteryLeft = 255;
	CLEAR_CALLBACK_BOOL(OnSteamShutdown);
}

#pragma region ISteamApps
void CCallbacks::OnDlcInstalled(DlcInstalled_t *pParam)
{
	utils::Log("Callback: OnDlcInstalled: AppID = " + std::to_string(pParam->m_nAppID));
	STORE_CALLBACK_RESULT(OnDlcInstalled, *pParam);
}

CALLBACK_LIST_METHODS(OnDlcInstalled, DlcInstalled_t);	// InstallDLC

CALLBACK_BOOL_METHODS(OnNewUrlLaunchParameters, NewUrlLaunchParameters_t);
#pragma endregion

#pragma region ISteamAppTicket
// This should never be needed in most cases.
#pragma endregion

#pragma region ISteamClient
// No callbacks.
#pragma endregion

#pragma region ISteamController
// No callbacks.  Replaced by ISteamInput.
#pragma endregion

#pragma region ISteamFriends
void CCallbacks::OnAvatarImageLoaded(AvatarImageLoaded_t *pParam)
{
	agk::Log("Callback: OnAvatarImageLoaded");
	STORE_CALLBACK_RESULT(OnAvatarImageLoaded, pParam->m_steamID);
}

CALLBACK_LIST_METHODS(OnAvatarImageLoaded, CSteamID); // GetFriendAvatar

// ClanOfficerListResponse_t - RequestClanOfficerList
// DownloadClanActivityCountsResult_t - DownloadClanActivityCounts, always fires
// FriendRichPresenceUpdate_t - RequestFriendRichPresence, always fires
// FriendsEnumerateFollowingList_t - EnumerateFollowingList
// FriendsGetFollowerCount_t - GetFollowerCount
// FriendsIsFollowing_t - IsFollowing
// GameConnectedChatJoin_t - JoinClanChatRoom
// GameConnectedChatLeave_t - LeaveClanChatRoom
// GameConnectedClanChatMsg_t - JoinClanChatRoom
// GameConnectedFriendChatMsg_t - SetListenForFriendsMessages

void CCallbacks::OnGameLobbyJoinRequested(GameLobbyJoinRequested_t *pParam)
{
	agk::Log("Callback: OnGameLobbyJoinRequested");
	STORE_CALLBACK_RESULT(OnGameLobbyJoinRequested, *pParam);
}

CALLBACK_LIST_METHODS(OnGameLobbyJoinRequested, GameLobbyJoinRequested_t);

void CCallbacks::OnGameOverlayActivated(GameOverlayActivated_t *pParam)
{
	agk::Log("Callback: OnGameOverlayActivated");
	m_IsGameOverlayActive = pParam->m_bActive != 0;
}

// GameRichPresenceJoinRequested_t - InviteUserToGame
// GameServerChangeRequested_t - fires when requesting to join game server from friends list.
// JoinClanChatRoomCompletionResult_t - JoinClanChatRoom

// Callback for RequestUserInformation and more.
void CCallbacks::OnPersonaStateChange(PersonaStateChange_t *pParam)
{
	agk::Log("Callback: OnPersonaStateChange");
	STORE_CALLBACK_RESULT(OnPersonaStateChange, *pParam);
	if (pParam->m_nChangeFlags & k_EPersonaChangeAvatar)
	{
		// Allow HasAvatarImageLoaded to report avatar changes from here as well.
		if (m_bOnAvatarImageLoadedEnabled)
		{
			STORE_CALLBACK_RESULT(OnAvatarImageLoaded, pParam->m_ulSteamID);
		}
	}
}

CALLBACK_LIST_METHODS(OnPersonaStateChange, PersonaStateChange_t); // RequestUserInformation
#pragma endregion

#pragma region ISteamGameCoordinator
// deprecated
#pragma endregion

#pragma region ISteamGameServer
// TODO? 
#pragma endregion

#pragma region ISteamGameServerStats
// TODO?
#pragma endregion

#pragma region ISteamHTMLSurface
// TODO?
#pragma endregion

#pragma region ISteamHTTP
// TODO?
#pragma endregion

#pragma region ISteamInventory
// TODO?
#pragma endregion

#pragma region ISteamMatchmaking
void CCallbacks::OnLobbyChatMessage(LobbyChatMsg_t *pParam)
{
	agk::Log("Callback: OnLobbyChatMessage");
	plugin::LobbyChatMsg_t info(*pParam);
	SteamMatchmaking()->GetLobbyChatEntry(info.m_ulSteamIDLobby, pParam->m_iChatID, NULL, info.m_chChatEntry, MAX_CHAT_MESSAGE_LENGTH, NULL);
	STORE_CALLBACK_RESULT(OnLobbyChatMessage, info);
}

CALLBACK_LIST_METHODS(OnLobbyChatMessage, plugin::LobbyChatMsg_t); // While in a lobby

void CCallbacks::OnLobbyChatUpdate(LobbyChatUpdate_t *pParam)
{
	agk::Log("Callback: OnLobbyChatUpdate");
	STORE_CALLBACK_RESULT(OnLobbyChatUpdate, *pParam);
}

CALLBACK_LIST_METHODS(OnLobbyChatUpdate, LobbyChatUpdate_t); // While in a lobby

void CCallbacks::OnLobbyDataUpdate(LobbyDataUpdate_t *pParam)
{
	utils::Log("Callback OnLobbyDataUpdate: " + std::string((pParam->m_bSuccess) ? "Succeeded" : "Failed"));
	STORE_CALLBACK_RESULT(OnLobbyDataUpdate, *pParam);
}

CALLBACK_LIST_METHODS(OnLobbyDataUpdate, LobbyDataUpdate_t); // While in a lobby

// Fires from CreateLobby and JoinLobby.
void CCallbacks::OnLobbyEnter(LobbyEnter_t *pParam)
{
	utils::Log("Callback: OnLobbyEnter");
	STORE_CALLBACK_RESULT(OnLobbyEnter, *pParam);
	g_JoinedLobbiesMutex.lock();
	g_JoinedLobbies.push_back(pParam->m_ulSteamIDLobby);
	g_JoinedLobbiesMutex.unlock();
}

CALLBACK_LIST_METHODS(OnLobbyEnter, LobbyEnter_t); // CreateLobby, JoinLobby, also a call result

void CCallbacks::OnLobbyGameCreated(LobbyGameCreated_t *pParam)
{
	utils::Log("Callback: OnLobbyGameCreated: " + std::to_string(pParam->m_ulSteamIDLobby));
	STORE_CALLBACK_RESULT(OnLobbyGameCreated, *pParam);
}

CALLBACK_LIST_METHODS(OnLobbyGameCreated, LobbyGameCreated_t); // While in a lobby

// LobbyInvite_t - when invited by someone.
// LobbyKicked_t - Currently unused!
// LobbyMatchList_t - Call result.
#pragma endregion

#pragma region ISteamMatchmakingServers
// No callbacks.
#pragma endregion

#pragma region ISteamMusic
CALLBACK_BOOL_METHODS(OnPlaybackStatusHasChanged, PlaybackStatusHasChanged_t);
CALLBACK_BOOL_METHODS(OnVolumeHasChanged, VolumeHasChanged_t);
#pragma endregion

#pragma region ISteamMusicRemote
// TODO?
#pragma endregion

#pragma region ISteamNetworking
// TODO?
#pragma endregion

#pragma region ISteamNetworkingSockets
// TODO?
#pragma endregion

#pragma region ISteamNetworkingUtils
// TODO?
#pragma endregion

#pragma region ISteamParties
// TODO?
#pragma endregion

#pragma region ISteamRemoteStorage
// Only call results.
#pragma endregion

#pragma region ISteamScreenshots
// TODO?
#pragma endregion

#pragma region ISteamUGC
// TODO?
// Only call results.
#pragma endregion

#pragma region ISteamUnifiedMessages
// Deprecated
#pragma endregion

#pragma region ISteamUser
//ClientGameServerDeny_t
//EncryptedAppTicketResponse_t - Call result for RequestEncryptedAppTicket.
//GameWebCallback_t
//GetAuthSessionTicketResponse_t - Call result for GetAuthSessionTicket.
//IPCFailure_t
//LicensesUpdated_t
//MicroTxnAuthorizationResponse_t
//SteamServerConnectFailure_t
//SteamServersConnected_t
//SteamServersDisconnected_t
//StoreAuthURLResponse_t - Call result for RequestStoreAuthURL
//ValidateAuthTicketResponse_t - Call result for BeginAuthSession
#pragma endregion

#pragma region ISteamUserStats
// Callback for GetAchievementIcon.
void CCallbacks::OnUserAchievementIconFetched(UserAchievementIconFetched_t *pParam)
{
	if (pParam->m_nGameID.AppID() == g_AppID)
	{
		agk::Log("Callback: OnUserAchievementIconFetched");
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
		m_AchievementIconsMap[name] = hImage;
	}
	return hImage;
}

// Callback for StoreStats.
void CCallbacks::OnUserAchievementStored(UserAchievementStored_t *pCallback)
{
	if (pCallback->m_nGameID == g_AppID)
	{
		agk::Log("Callback: OnAchievementStored");
		STORE_CALLBACK_RESULT(OnUserAchievementStored, *pCallback);
	}
}

CALLBACK_LIST_METHODS(OnUserAchievementStored, UserAchievementStored_t);

void CCallbacks::OnUserStatsReceived(UserStatsReceived_t *pCallback)
{
	if (pCallback->m_nGameID == g_AppID)
	{
		utils::Log("Callback: OnUserStatsReceived.  Result = " + std::to_string(pCallback->m_eResult));
		// If the current user's stats were received, set a flag.
		if (pCallback->m_steamIDUser == SteamUser()->GetSteamID() && pCallback->m_eResult == k_EResultOK)
		{
			m_StatsInitialized = true;
		}
		STORE_CALLBACK_RESULT(OnUserStatsReceived, *pCallback);
	}
	else
	{
		utils::Log("Callback: OnUserStatsReceived.  GameID = " + std::to_string(pCallback->m_nGameID));
	}
}

CALLBACK_LIST_METHODS(OnUserStatsReceived, UserStatsReceived_t);

// Callback for StoreStats and ResetAllStats.
void CCallbacks::OnUserStatsStored(UserStatsStored_t *pCallback)
{
	if (pCallback->m_nGameID == g_AppID)
	{
		utils::Log("Callback: OnUserStatsStored " + std::string((pCallback->m_eResult == k_EResultOK) ? "Succeeded." : "Failed.  Result = " + std::to_string(pCallback->m_eResult)));
		STORE_CALLBACK_RESULT(OnUserStatsStored, *pCallback);
	}
}

CALLBACK_LIST_METHODS(OnUserStatsStored, UserStatsStored_t);
#pragma endregion

#pragma region ISteamUtils
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
	STORE_CALLBACK_RESULT(OnGamepadTextInputDismissed, info);
}

CALLBACK_LIST_METHODS(OnGamepadTextInputDismissed, plugin::GamepadTextInputDismissed_t);

//IMPL_CALLBACK_LIST(GamepadTextInputDismissed, plugin::GamepadTextInputDismissed_t)

CALLBACK_BOOL_METHODS(OnIPCountryChanged, IPCountry_t);

void CCallbacks::OnLowBatteryPower(LowBatteryPower_t *pParam)
{
	agk::Log("Callback: Low Battery Power Warning");
	m_bOnLowBatteryPower = true;
	m_nMinutesBatteryLeft = pParam->m_nMinutesBatteryLeft;
}
bool CCallbacks::HasOnLowBatteryPower()
{
	if (m_bOnLowBatteryPower)
	{
		m_bOnLowBatteryPower = false;
		return true;
	}
	return false;
}

CALLBACK_BOOL_METHODS(OnSteamShutdown, SteamShutdown_t);
#pragma endregion

#pragma region ISteamVideo
// TODO?
#pragma endregion
