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

// Default BOOL_CALLBACK method.
#define BOOL_CALLBACK_METHODS(var, callback_type, log_text)		\
	void CCallbacks::On##var##(callback_type *pParam)			\
	{															\
		utils::Log("Callback: On" #var log_text);				\
		var.Trigger();											\
	}

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
	// Disconnect from any clan chats.
	ClanChatManager()->Reset();
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
	// Disconnect from any lobbies.
	LobbyManager()->Reset();
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

BOOL_CALLBACK_METHODS(NewUrlLaunchParameters, NewUrlLaunchParameters_t,);
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
// DownloadClanActivityCountsResult_t - DownloadClanActivityCounts

void CCallbacks::OnFriendRichPresenceUpdate(FriendRichPresenceUpdate_t *pParam)
{
	utils::Log("Callback: OnFriendRichPresenceUpdate.  AppID = " + std::to_string(pParam->m_nAppID));
	FriendRichPresenceUpdate.StoreResponse(pParam->m_steamIDFriend);
}
// FriendsEnumerateFollowingList_t - EnumerateFollowingList
// FriendsGetFollowerCount_t - GetFollowerCount
// FriendsIsFollowing_t - IsFollowing

void CCallbacks::OnGameConnectedChatJoin(GameConnectedChatJoin_t *pParam)
{
	agk::Log("Callback: OnGameConnectedChatJoin");
	GameConnectedChatJoin.StoreResponse(*pParam);
}

void CCallbacks::OnGameConnectedChatLeave(GameConnectedChatLeave_t *pParam)
{
	agk::Log("Callback: OnGameConnectedChatLeave");
	GameConnectedChatLeave.StoreResponse(*pParam);
}

void CCallbacks::OnGameConnectedClanChatMsg(GameConnectedClanChatMsg_t *pParam)
{
	agk::Log("Callback: OnGameConnectedClanChatMsg");
	plugin::GameConnectedClanChatMsg_t msg(*pParam);
	int length = SteamFriends()->GetClanChatMessage(msg.m_steamIDClanChat, msg.m_iMessageID, msg.m_Text, MAX_CLAN_CHAT_MESSAGE_LENGTH, &msg.m_ChatEntryType, &msg.m_steamIDUser);
	msg.m_Text[length] = 0;
	if (msg.m_ChatEntryType == k_EChatEntryTypeInvalid)
	{
		agk::PluginError("OnGameConnectedClanChatMsg: GetClanChatMessage received an invalid entry type.");
		return;
	}
	GameConnectedClanChatMsg.StoreResponse(msg);
}

void CCallbacks::OnGameConnectedFriendChatMsg(GameConnectedFriendChatMsg_t *pParam)
{
	agk::Log("Callback: OnGameConnectedFriendChatMsg");
	plugin::GameConnectedFriendChatMsg_t msg(*pParam);
	int length = SteamFriends()->GetFriendMessage(msg.m_steamIDUser, msg.m_iMessageID, msg.m_Text, MAX_FRIEND_CHAT_MESSAGE_LENGTH, &msg.m_ChatEntryType);
	msg.m_Text[length] = 0;
	if (msg.m_ChatEntryType == k_EChatEntryTypeInvalid)
	{
		agk::PluginError("OnGameConnectedFriendChatMsg: GetFriendMessage received an invalid entry type.");
		return;
	}
	GameConnectedFriendChatMsg.StoreResponse(*pParam);
}

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

void CCallbacks::OnGameRichPresenceJoinRequested(GameRichPresenceJoinRequested_t *pParam)
{
	utils::Log("Callback: OnGameRichPresenceJoinRequested");
	GameRichPresenceJoinRequested.StoreResponse(*pParam);
}

void CCallbacks::OnGameServerChangeRequested(GameServerChangeRequested_t *pParam)
{
	utils::Log("Callback: OnGameServerChangeRequested");
	GameServerChangeRequested.StoreResponse(*pParam);
}

//void CCallbacks::OnJoinClanChatRoomCompletionResult(JoinClanChatRoomCompletionResult_t *pParam)
//{
//	utils::Log("Callback: OnJoinClanChatRoomCompletionResult.  SteamID = " + std::to_string(pParam->m_steamIDClanChat.ConvertToUint64()) + ", Response = " + std::to_string(pParam->m_eChatRoomEnterResponse));
//	JoinClanChatRoomCompletionResult.StoreResponse(*pParam);
//	ClanChatManager()->Add(pParam->m_steamIDClanChat);
//}

// Callback for RequestUserInformation and more.
void CCallbacks::OnPersonaStateChange(PersonaStateChange_t *pParam)
{
	utils::Log("Callback: OnPersonaStateChange.  SteamID = " + std::to_string(pParam->m_ulSteamID) + ", Flags = " + std::to_string(pParam->m_nChangeFlags));
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

void Clear(plugin::LobbyChatMsg_t &value)
{
	value.m_ulSteamIDLobby = 0;
	value.m_ulSteamIDUser = 0;
	value.m_eChatEntryType = 0;
	if (value.m_MemblockID && agk::GetMemblockExists(value.m_MemblockID))
	{
		agk::DeleteMemblock(value.m_MemblockID);
	}
	value.m_MemblockID = 0;
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
	LobbyEnter.StoreResponse(*pParam);
	LobbyManager()->Add(pParam->m_ulSteamIDLobby);
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
BOOL_CALLBACK_METHODS(PlaybackStatusHasChanged, PlaybackStatusHasChanged_t,);
//void CCallbacks::OnPlaybackStatusHasChanged(PlaybackStatusHasChanged_t *pParam)
//{
//	agk::Log("Callback: OnPlaybackStatusHasChanged");
//	PlaybackStatusHasChanged.Trigger();
//}

BOOL_CALLBACK_METHODS(VolumeHasChanged, VolumeHasChanged_t, ".  New volume: " + std::to_string(pParam->m_flNewVolume));
//void CCallbacks::OnVolumeHasChanged(VolumeHasChanged_t *pParam)
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
void CCallbacks::OnUserAchievementStored(UserAchievementStored_t *pParam)
{
	if (pParam->m_nGameID == g_AppID)
	{
		agk::Log("Callback: OnUserAchievementStored");
		UserAchievementStored.StoreResponse(*pParam);
	}
}

void CCallbacks::OnUserStatsReceived(UserStatsReceived_t *pParam)
{
	if (pParam->m_nGameID == g_AppID)
	{
		utils::Log("Callback: OnUserStatsReceived.  Result = " + std::to_string(pParam->m_eResult) + ", user = " + std::to_string(pParam->m_steamIDUser.ConvertToUint64()));
		if (pParam->m_eResult == k_EResultOK)
		{
			auto it = std::find(m_StatsInitializedUsers.begin(), m_StatsInitializedUsers.end(), pParam->m_steamIDUser);
			if (it == m_StatsInitializedUsers.end())
			{
				m_StatsInitializedUsers.push_back(pParam->m_steamIDUser);
			}
			// If the current user's stats were received, set a flag.
			if (pParam->m_steamIDUser == SteamUser()->GetSteamID())
			{
				m_StatsInitialized = true;
			}
		}
		UserStatsReceived.StoreResponse(*pParam);
	}
	else
	{
		utils::Log("Callback: OnUserStatsReceived.  GameID = " + std::to_string(pParam->m_nGameID));
	}
}

// Callback for StoreStats and ResetAllStats.
void CCallbacks::OnUserStatsStored(UserStatsStored_t *pParam)
{
	if (pParam->m_nGameID == g_AppID)
	{
		utils::Log("Callback: OnUserStatsStored.  Result = " + std::to_string(pParam->m_eResult));
		UserStatsStored.StoreResponse(*pParam);
	}
}

void CCallbacks::OnUserStatsUnloaded(UserStatsUnloaded_t *pParam)
{
	utils::Log("Callback: OnUserStatsUnloaded.  User = " + std::to_string(pParam->m_steamIDUser.ConvertToUint64()));
	UserStatsUnloaded.StoreResponse(pParam->m_steamIDUser);
	auto it = std::find(m_StatsInitializedUsers.begin(), m_StatsInitializedUsers.end(), pParam->m_steamIDUser);
	if (it != m_StatsInitializedUsers.end())
	{
		m_StatsInitializedUsers.erase(it);
	}
}
#pragma endregion

#pragma region ISteamUtils
void CCallbacks::OnGamepadTextInputDismissed(GamepadTextInputDismissed_t *pParam)
{
	utils::Log("Callback: OnGamepadTextInputDismissed.  Submitted = " + std::to_string(pParam->m_bSubmitted));
	plugin::GamepadTextInputDismissed_t info(*pParam);
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

BOOL_CALLBACK_METHODS(IPCountryChanged, IPCountry_t,);

//BOOL_CALLBACK_METHODS(LowBatteryPower, LowBatteryPower_t);
void CCallbacks::OnLowBatteryPower(LowBatteryPower_t *pParam)
{
	agk::Log("Callback: OnLowBatteryPower");
	LowBatteryPower.Trigger();
	LowBatteryPower.m_Response = *pParam;
}


BOOL_CALLBACK_METHODS(SteamShutdown, SteamShutdown_t,);
#pragma endregion

#pragma region ISteamVideo
// TODO?
#pragma endregion
