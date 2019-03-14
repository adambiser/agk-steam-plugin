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

#ifndef _CALLBACKS_H
#define _CALLBACKS_H
#pragma once

#include <steam_api.h>
#include "DllMain.h"
#include "StructClear.h"
#include "Utils.h"
#include <list>
#include <map>
#include <mutex>
#include <vector>

#if defined(_WINDOWS)
#if defined(_WIN64)
#pragma comment(lib, "steam_api64.lib")
#else
#pragma comment(lib, "steam_api.lib")
#endif
#endif

enum EAvatarSize
{
	Small = 0,
	Medium = 1,
	Large = 2
};

#define CheckInitialized(steamInterface, returnValue)				\
	if (!g_SteamInitialized || (NULL == steamInterface()))			\
	{																\
		agk::PluginError("Steam API has not been initialized.");	\
		return returnValue;											\
	}

#define SETUP_CALLBACK_LIST(callback, type)	\
public:										\
	bool HasNew##callback##();				\
	type GetNew##callback##() { return m_Current##callback##; } \
private:									\
	std::list<##type##> m_##callback##List;	\
	std::mutex m_##callback##Mutex;			\
	type m_Current##callback

// Reports true only once per change.
#define SETUP_CALLBACK_BOOL(callback)		\
private:									\
	bool m_b##callback##;					\
public:										\
	bool Has##callback##()					\
	{										\
		if (m_b##callback##)				\
		{									\
			m_b##callback## = false;		\
			return true;					\
		}									\
		return false;						\
	}

#define ADD_CALLBACK(thisclass, func, callback_type, var)			\
	STEAM_CALLBACK_MANUAL(thisclass, func, callback_type, var);		\
public:																\
	void Enable##func##Callback()									\
	{																\
		if (!m_b##func##Enabled)									\
		{															\
			m_b##func##Enabled = true;								\
			var##.Register(this, &##thisclass##::##func##);			\
		}															\
	}																\
private:															\
	bool m_b##func##Enabled


class CCallbacks
{
public:
	CCallbacks();
	virtual ~CCallbacks(void);

	// General methods.
public:
	void ResetSessionVariables();

	// TODO Remove this.
	// Return to Idle after reporting Done.
	ECallbackState getCallbackState(ECallbackState *callbackState)
	{
		if (*callbackState == Done)
		{
			*callbackState = Idle;
			return Done;
		}
		return *callbackState;
	}
private:
#pragma region ISteamApps
	ADD_CALLBACK(CCallbacks, OnDlcInstalled, DlcInstalled_t, m_CallbackDlcInstalled);	// InstallDLC
	SETUP_CALLBACK_LIST(DlcInstalled, DlcInstalled_t);
	//ADD_CALLBACK(CCallbacks, OnFileDetailsResult, FileDetailsResult_t, m_CallbackFileDetailsResult);	// GetFileDetails
	//SETUP_CALLBACK_LIST(FileDetailsResult, FileDetailsResult_t);
	STEAM_CALLBACK(CCallbacks, OnNewUrlLaunchParameters, NewUrlLaunchParameters_t, m_CallbackNewUrlLaunchParameters);
	SETUP_CALLBACK_BOOL(NewUrlLaunchParameters);
	// Removed SDK v1.43
	//STEAM_CALLBACK(CCallbacks, OnNewLaunchQueryParameters, NewLaunchQueryParameters_t, m_CallbackNewLaunchQueryParameters);
	//SETUP_CALLBACK_BOOL(NewLaunchQueryParameters);
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
	ADD_CALLBACK(CCallbacks, OnAvatarImageLoaded, AvatarImageLoaded_t, m_CallbackAvatarImageLoaded); // GetFriendAvatar
	SETUP_CALLBACK_LIST(AvatarImageLoadedUser, CSteamID);
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
	STEAM_CALLBACK(CCallbacks, OnGameLobbyJoinRequested, GameLobbyJoinRequested_t, m_CallbackGameLobbyJoinRequested);
	SETUP_CALLBACK_LIST(GameLobbyJoinRequested, GameLobbyJoinRequested_t);
	STEAM_CALLBACK(CCallbacks, OnGameOverlayActivated, GameOverlayActivated_t, m_CallbackGameOverlayActivated);
	bool m_IsGameOverlayActive;
public:
	bool IsGameOverlayActive() { return m_IsGameOverlayActive; }
private:
	// GameRichPresenceJoinRequested_t - InviteUserToGame
	// GameServerChangeRequested_t - fires when requesting to join game server from friends list.
	// JoinClanChatRoomCompletionResult_t - JoinClanChatRoom
	ADD_CALLBACK(CCallbacks, OnPersonaStateChange, PersonaStateChange_t, m_CallbackPersonaStateChange); // RequestUserInformation
	SETUP_CALLBACK_LIST(PersonaStateChange, PersonaStateChange_t);
	// SetPersonaNameResponse_t - SetPersonaName
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
	// FavoritesListChanged_t - fires when server favorites list changes.
	STEAM_CALLBACK(CCallbacks, OnLobbyChatMessage, LobbyChatMsg_t, m_CallbackLobbyChatMessage); // While in a lobby
	SETUP_CALLBACK_LIST(LobbyChatMessage, plugin::LobbyChatMsg_t);
	STEAM_CALLBACK(CCallbacks, OnLobbyChatUpdate, LobbyChatUpdate_t, m_CallbackLobbyChatUpdate); // While in a lobby
	SETUP_CALLBACK_LIST(LobbyChatUpdate, LobbyChatUpdate_t);
	// LobbyCreated_t - Call result
	STEAM_CALLBACK(CCallbacks, OnLobbyDataUpdate, LobbyDataUpdate_t, m_CallbackLobbyDataUpdate); // While in a lobby
	SETUP_CALLBACK_LIST(LobbyDataUpdate, LobbyDataUpdate_t);
	STEAM_CALLBACK(CCallbacks, OnLobbyEnter, LobbyEnter_t, m_CallbackLobbyEnter); // CreateLobby, JoinLobby, also a call result
	SETUP_CALLBACK_LIST(LobbyEnter, LobbyEnter_t);
	STEAM_CALLBACK(CCallbacks, OnLobbyGameCreated, LobbyGameCreated_t, m_CallbackLobbyGameCreated); // While in a lobby
	SETUP_CALLBACK_LIST(LobbyGameCreated, LobbyGameCreated_t);
	// LobbyInvite_t - when invited by someone.
	// LobbyKicked_t - Currently unused!
	// LobbyMatchList_t - Call result.
#pragma endregion

#pragma region ISteamMatchmakingServers
	// No callbacks.
#pragma endregion

#pragma region ISteamMusic
	STEAM_CALLBACK(CCallbacks, OnPlaybackStatusHasChanged, PlaybackStatusHasChanged_t, m_CallbackPlaybackStatusHasChanged);
	SETUP_CALLBACK_BOOL(MusicPlaybackStatusChanged);
	STEAM_CALLBACK(CCallbacks, OnVolumeHasChanged, VolumeHasChanged_t, m_CallbackVolumeHasChanged); // TODO Return VolumeHasChanged_t.m_flNewVolume ?
	SETUP_CALLBACK_BOOL(MusicVolumeChanged);
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
	// RemoteStorageDownloadUGCResult_t - Call result for UGCDownload, UGCDownloadToLocation.
	// RemoteStorageFileReadAsyncComplete_t - Call result for FileReadAsync.
	// RemoteStorageFileShareResult_t - Call result for FileShare.
	// RemoteStorageFileWriteAsyncComplete_t - Call result for FileWriteAsync.
	// RemoteStoragePublishedFileSubscribed_t - deprecated?
	// RemoteStoragePublishedFileUnsubscribed_t - deprecated?
	// RemoteStorageSubscribePublishedFileResult_t - Call result for SubscribePublishedFile, ISteamUGC::SubscribeItem.
	// RemoteStorageUnsubscribePublishedFileResult_t - Call result for UnsubscribePublishedFile, ISteamUGC::UnsubscribeItem.
#pragma endregion

#pragma region ISteamScreenshots
	// TODO?
#pragma endregion

#pragma region ISteamUGC
	// TODO?
	//AddAppDependencyResult_t - Call result for AddAppDependency
	//AddUGCDependencyResult_t - Call result for AddDependency
	//CreateItemResult_t - Call result for CreateItem
	//DownloadItemResult_t - Call result for DownloadItem
	//GetAppDependenciesResult_t - Call result for GetAppDependencies
	//DeleteItemResult_t - Call result for DeleteItem
	//GetUserItemVoteResult_t - Call result for GetUserItemVote
	//ItemInstalled_t - Callback.  Check m_unAppID for this game's appid!
	//RemoveAppDependencyResult_t - Call result for RemoveAppDependency.
	//RemoveUGCDependencyResult_t - Call result for RemoveDependency
	//SetUserItemVoteResult_t - Call result for SetUserItemVote
	//StartPlaytimeTrackingResult_t - Call result for StartPlaytimeTracking
	//SteamUGCQueryCompleted_t - Call result for SendQueryUGCRequest
	//StopPlaytimeTrackingResult_t - Call result for StopPlaytimeTracking, StopPlaytimeTrackingForAllItems
	//SubmitItemUpdateResult_t - Call result for SubmitItemUpdate
	//UserFavoriteItemsListChanged_t - Call result for AddItemToFavorites, RemoveItemFromFavorites
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
	//GlobalAchievementPercentagesReady_t - RequestGlobalAchievementPercentages
	//GlobalStatsReceived_t - RequestGlobalStats
	//LeaderboardFindResult_t - Call result for FindOrCreateLeaderboard, FindLeaderboard
	//LeaderboardScoresDownloaded_t - Call result for DownloadLeaderboardEntries, DownloadLeaderboardEntriesForUsers
	//LeaderboardScoreUploaded_t - Call result for UploadLeaderboardScore
	//LeaderboardUGCSet_t - Call result for AttachLeaderboardUGC
	//NumberOfCurrentPlayers_t - GetNumberOfCurrentPlayers
	//PS3TrophiesInstalled_t - ignore
	STEAM_CALLBACK(CCallbacks, OnAchievementIconFetched, UserAchievementIconFetched_t, m_CallbackAchievementIconFetched);
	std::map<std::string, int> m_AchievementIconsMap;
public:
	// While GetAchievementIcon has an internal callback, there's no need to make them external.
	int GetAchievementIcon(const char *pchName);
private:
	STEAM_CALLBACK(CCallbacks, OnAchievementStored, UserAchievementStored_t, m_CallbackAchievementStored);
	bool m_AchievementStored;
public:
	bool AchievementStored() { return m_AchievementStored; }
private:
	ADD_CALLBACK(CCallbacks, OnUserStatsReceived, UserStatsReceived_t, m_CallbackUserStatsReceived);
	ECallbackState m_RequestStatsCallbackState;
	bool m_StatsInitialized;
public:
	ECallbackState GetRequestStatsCallbackState() { return getCallbackState(&m_RequestStatsCallbackState); }
	bool StatsInitialized() { return m_StatsInitialized; }
private:
	STEAM_CALLBACK(CCallbacks, OnUserStatsStored, UserStatsStored_t, m_CallbackUserStatsStored);
	ECallbackState m_StoreStatsCallbackState;
	bool m_StatsStored;
public:
	ECallbackState GetStoreStatsCallbackState() { return getCallbackState(&m_StoreStatsCallbackState); }
	bool StatsStored() { return m_StatsStored; }
	//UserStatsUnloaded_t - RequestUserStats again

	// TODO:
	bool StoreStats();
	bool ResetAllStats(bool bAchievementsToo);
private:
#pragma endregion

#pragma region ISteamUtils
	// CheckFileSignature_t - Call result for  CheckFileSignature.
	STEAM_CALLBACK(CCallbacks, OnGamepadTextInputDismissed, GamepadTextInputDismissed_t, m_CallbackGamepadTextInputDismissed);
	SETUP_CALLBACK_LIST(GamepadTextInputDismissed, plugin::GamepadTextInputDismissed_t);
	STEAM_CALLBACK(CCallbacks, OnIPCountryChanged, IPCountry_t, m_CallbackIPCountryChanged);
	SETUP_CALLBACK_BOOL(IPCountryChanged);
	STEAM_CALLBACK(CCallbacks, OnLowBatteryPower, LowBatteryPower_t, m_CallbackLowBatteryPower);
	SETUP_CALLBACK_BOOL(LowBatteryWarning);
	uint8 m_nMinutesBatteryLeft;
public:
	uint8 GetMinutesBatteryLeft() { return m_nMinutesBatteryLeft; }
private:
	//SteamAPICallCompleted_t - not needed
	STEAM_CALLBACK(CCallbacks, OnSteamShutdown, SteamShutdown_t, m_CallbackSteamShutdown);
	SETUP_CALLBACK_BOOL(SteamShutdownNotification);
#pragma endregion

#pragma region ISteamVideo
	// TODO?
#pragma endregion
};

CCallbacks *Callbacks();

#endif // _CALLBACKS_H
