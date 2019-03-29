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

// Default BOOL_CALLBACK methods.
//#define BOOL_CALLBACK_METHODS(func, callback_type)			\
//	void CCallbacks::On##func##(callback_type *pParam)		\
//	{														\
//		agk::Log("Callback: On" #func);						\
//		m_b##func## = true;									\
//	}														\
//	bool CCallbacks::Has##func##Response()					\
//	{														\
//		if (m_b##func##)									\
//		{													\
//			m_b##func## = false;							\
//			return true;									\
//		}													\
//		return false;										\
//	}

//#define BOOL_CALLBACK_METHODS(var, callback_type)			\
//	void CCallbacks::On##var##(callback_type *pCallback)	\
//	{ \
//		agk::Log("Callback: On" #var); \
//		var.StoreResponse(*pCallback); \
//	}


CCallbacks *Callbacks()
{
	static CCallbacks *callbacks = new CCallbacks();
	return callbacks;
}

CCallbacks::CCallbacks()
{
	Reset();
}

CCallbacks::~CCallbacks(void)
{
	this->Reset();
}

void CCallbacks::Reset()
{
	// ISteamApps
	DlcInstalled.Reset();
	NewUrlLaunchParameters.Reset();

	// ISteamAppTicket
	// ISteamClient
	// ISteamController/ISteamInput

	// ISteamFriends
	AvatarImageLoaded.Reset();
	GameLobbyJoinRequested.Reset();
	m_IsGameOverlayActive = false;
	PersonaStateChange.Reset();

	// ISteamGameCoordinator
	// ISteamGameServer
	// ISteamGameServerStats
	// ISteamHTMLSurface
	// ISteamHTTP
	// ISteamInventory

	// ISteamMatchmaking
	FavoritesListChanged.Reset();
	LobbyChatMessage.Reset();
	LobbyChatUpdate.Reset();
	LobbyDataUpdate.Reset();
	LobbyEnter.Reset();
	LobbyGameCreated.Reset();

	// ISteamMatchmakingServers

	// ISteamMusic
	PlaybackStatusHasChanged.Reset();
	VolumeHasChanged.Reset();

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
	UserAchievementStored.Reset();
	UserStatsReceived.Reset();
	m_StatsInitialized = false;
	m_StatsInitializedUsers.clear();
	UserStatsStored.Reset();
	UserStatsUnloaded.Reset();
	// ISteamUtils
	// CheckFileSignature_t - Call result for  CheckFileSignature.
	GamepadTextInputDismissed.Reset();
	IPCountryChanged.Reset();
	LowBatteryPower.Reset();
	//m_nMinutesBatteryLeft = 0;
	SteamShutdown.Reset();
}

#pragma region ISteamApps
void CCallbacks::OnDlcInstalled(DlcInstalled_t *pParam)
{
	utils::Log("Callback: OnDlcInstalled.  AppID = " + std::to_string(pParam->m_nAppID));
	DlcInstalled.StoreResponse(*pParam);
}

//BOOL_CALLBACK_METHODS(NewUrlLaunchParameters, NewUrlLaunchParameters_t);
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
	AvatarImageLoaded.StoreResponse(pParam->m_steamID);
}

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
	GameLobbyJoinRequested.StoreResponse(*pParam);
}

void CCallbacks::OnGameOverlayActivated(GameOverlayActivated_t *pParam)
{
	utils::Log("Callback: OnGameOverlayActivated.  Active = " + std::to_string(pParam->m_bActive));
	m_IsGameOverlayActive = pParam->m_bActive != 0;
}

// GameRichPresenceJoinRequested_t - InviteUserToGame
// GameServerChangeRequested_t - fires when requesting to join game server from friends list.
// JoinClanChatRoomCompletionResult_t - JoinClanChatRoom

// Callback for RequestUserInformation and more.
void CCallbacks::OnPersonaStateChange(PersonaStateChange_t *pParam)
{
	agk::Log("Callback: OnPersonaStateChange"); //.SteamID = " + std::to_string(pParam->m_ulSteamID) + ", Flags = " + std::to_string(pParam->m_nChangeFlags));
	PersonaStateChange.StoreResponse(*pParam);
	if (pParam->m_nChangeFlags & k_EPersonaChangeAvatar)
	{
		// Allow HasAvatarImageLoaded to report avatar changes from here as well.
		if (AvatarImageLoaded.Enabled())
		{
			AvatarImageLoaded.StoreResponse(pParam->m_ulSteamID);
		}
	}
}
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
void CCallbacks::OnFavoritesListChanged(FavoritesListChanged_t *pParam)
{
	agk::Log("Callback: OnFavoritesListChanged");
	FavoritesListChanged.StoreResponse(*pParam);
}

void CCallbacks::OnLobbyChatMessage(LobbyChatMsg_t *pParam)
{
	agk::Log("Callback: OnLobbyChatMessage");
	plugin::LobbyChatMsg_t info(*pParam);
	char data[MAX_CHAT_MESSAGE_LENGTH];
	int length = SteamMatchmaking()->GetLobbyChatEntry(info.m_ulSteamIDLobby, pParam->m_iChatID, NULL, data, MAX_CHAT_MESSAGE_LENGTH, NULL);
	data[length] = 0;
	if (length > 0)
	{
		info.m_MemblockID = agk::CreateMemblock(length);
		memcpy_s(agk::GetMemblockPtr(info.m_MemblockID), length, data, length);
	}
	//STORE_CALLBACK_RESULT(LobbyChatMessage, info);
	LobbyChatMessage.StoreResponse(info);
}

void CCallbacks::OnLobbyChatUpdate(LobbyChatUpdate_t *pParam)
{
	agk::Log("Callback: OnLobbyChatUpdate");
	LobbyChatUpdate.StoreResponse(*pParam);
}

void CCallbacks::OnLobbyDataUpdate(LobbyDataUpdate_t *pParam)
{
	utils::Log("Callback OnLobbyDataUpdate.  Success: " + std::to_string(pParam->m_bSuccess));
	LobbyDataUpdate.StoreResponse(*pParam);
}

// Fires from CreateLobby and JoinLobby.
void CCallbacks::OnLobbyEnter(LobbyEnter_t *pParam)
{
	agk::Log("Callback: OnLobbyEnter");
	//STORE_CALLBACK_RESULT(LobbyEnter, *pParam);
	LobbyEnter.StoreResponse(*pParam);
	if (pParam->m_ulSteamIDLobby != 0)
	{
		g_JoinedLobbiesMutex.lock();
		g_JoinedLobbies.push_back(pParam->m_ulSteamIDLobby);
		g_JoinedLobbiesMutex.unlock();
		// Register the in-lobby callbacks.
		agk::Log("Registering in-lobby callbacks");
		//RegisterLobbyChatMessageCallback();
		LobbyChatMessage.Register();
		LobbyChatUpdate.Register();
		//RegisterLobbyDataUpdateCallback();
		LobbyDataUpdate.Register();
		LobbyGameCreated.Register();
	}
}

void CCallbacks::OnLobbyGameCreated(LobbyGameCreated_t *pParam)
{
	agk::Log("Callback: OnLobbyGameCreated");
	LobbyGameCreated.StoreResponse(*pParam);
}

// LobbyInvite_t - when invited by someone.
// LobbyKicked_t - Currently unused!
// LobbyMatchList_t - Call result.
#pragma endregion

#pragma region ISteamMatchmakingServers
// No callbacks.
#pragma endregion

#pragma region ISteamMusic
//BOOL_CALLBACK_METHODS(PlaybackStatusHasChanged, PlaybackStatusHasChanged_t);
//void CCallbacks::OnPlaybackStatusHasChanged(PlaybackStatusHasChanged_t *pCallback)
//{
//	agk::Log("Callback: OnPlaybackStatusHasChanged");
//	PlaybackStatusHasChanged.Trigger();
//}

//BOOL_CALLBACK_METHODS(VolumeHasChanged, VolumeHasChanged_t);
//void CCallbacks::OnVolumeHasChanged(VolumeHasChanged_t *pCallback)
//{
//	agk::Log("Callback: OnVolumeHasChanged");
//	VolumeHasChanged.Trigger();
//}
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

bool CCallbacks::StatsInitializedForUser(CSteamID steamIDUser)
{
	auto it = std::find(m_StatsInitializedUsers.begin(), m_StatsInitializedUsers.end(), steamIDUser);
	return it != m_StatsInitializedUsers.end();
}

// Callback for StoreStats.
void CCallbacks::OnUserAchievementStored(UserAchievementStored_t *pCallback)
{
	if (pCallback->m_nGameID == g_AppID)
	{
		agk::Log("Callback: OnUserAchievementStored");
		UserAchievementStored.StoreResponse(*pCallback);
	}
}

void CCallbacks::OnUserStatsReceived(UserStatsReceived_t *pCallback)
{
	if (pCallback->m_nGameID == g_AppID)
	{
		agk::Log(__FUNCTION__);
		utils::Log("Callback: OnUserStatsReceived.  Result = " + std::to_string(pCallback->m_eResult) + ", user = " + std::to_string(pCallback->m_steamIDUser.ConvertToUint64()));
		if (pCallback->m_eResult == k_EResultOK)
		{
			auto it = std::find(m_StatsInitializedUsers.begin(), m_StatsInitializedUsers.end(), pCallback->m_steamIDUser);
			if (it == m_StatsInitializedUsers.end())
			{
				m_StatsInitializedUsers.push_back(pCallback->m_steamIDUser);
			}
			// If the current user's stats were received, set a flag.
			if (pCallback->m_steamIDUser == SteamUser()->GetSteamID())
			{
				m_StatsInitialized = true;
			}
		}
		UserStatsReceived.StoreResponse(*pCallback);
	}
	else
	{
		utils::Log("Callback: OnUserStatsReceived.  GameID = " + std::to_string(pCallback->m_nGameID));
	}
}

// Callback for StoreStats and ResetAllStats.
void CCallbacks::OnUserStatsStored(UserStatsStored_t *pCallback)
{
	if (pCallback->m_nGameID == g_AppID)
	{
		utils::Log("Callback: OnUserStatsStored.  Result = " + std::to_string(pCallback->m_eResult));
		UserStatsStored.StoreResponse(*pCallback);
	}
}

void CCallbacks::OnUserStatsUnloaded(UserStatsUnloaded_t *pCallback)
{
	utils::Log("Callback: OnUserStatsUnloaded.  User = " + std::to_string(pCallback->m_steamIDUser.ConvertToUint64()));
	auto it = std::find(m_StatsInitializedUsers.begin(), m_StatsInitializedUsers.end(), pCallback->m_steamIDUser);
	if (it != m_StatsInitializedUsers.end())
	{
		m_StatsInitializedUsers.erase(it);
	}
	UserStatsUnloaded.StoreResponse(pCallback->m_steamIDUser);
}
#pragma endregion

#pragma region ISteamUtils
void CCallbacks::OnGamepadTextInputDismissed(GamepadTextInputDismissed_t *pCallback)
{
	utils::Log("Callback: OnGamepadTextInputDismissed.  Submitted = " + std::to_string(pCallback->m_bSubmitted));
	plugin::GamepadTextInputDismissed_t info(*pCallback);
	if (info.m_bSubmitted)
	{
		uint32 length = SteamUtils()->GetEnteredGamepadTextLength();
		if (!SteamUtils()->GetEnteredGamepadTextInput(info.m_chSubmittedText, length))
		{
			// This should only ever happen if there's a length mismatch.
			agk::PluginError("OnGamepadTextInputDismissed: GetEnteredGamepadTextInput failed.");
		}
	}
	GamepadTextInputDismissed.StoreResponse(info);
}

//BOOL_CALLBACK_METHODS(IPCountryChanged, IPCountry_t);

//BOOL_CALLBACK_METHODS(LowBatteryPower, LowBatteryPower_t);

//BOOL_CALLBACK_METHODS(SteamShutdown, SteamShutdown_t);
#pragma endregion

#pragma region ISteamVideo
// TODO?
#pragma endregion
