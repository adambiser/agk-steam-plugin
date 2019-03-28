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

#include "DllMain.h"
#include "PluginTypes.h"
#include "StructClear.h"
#include <steam_api.h>
#include <list>
#include <map>
#include <mutex>
#include <vector>
//member_type T::*member_name
class CCallbacks;

template <class callback_type, void(CCallbacks::*callback_function)(callback_type*), class list_type = callback_type>
struct ListCallback
{
	bool m_bEnabled;
	std::list<list_type> m_List;
	std::mutex m_Mutex;
	list_type m_CurrentValue;
	CCallbackManual<CCallbacks, callback_type> m_Callback;

	void Register()
	{
		if (!g_SteamInitialized)
		{
			return;
		}
		if (!m_bEnabled)
		{
			m_bEnabled = true;
			m_Callback.Register(Callbacks(), callback_function);
		}
	}
	void Unregister()
	{
		if (!g_SteamInitialized)
		{
			return;
		}
		if (m_bEnabled)
		{
			m_bEnabled = false;
			m_Callback.Unregister();
		}
	}
	void StoreResponse(list_type result)
	{
		m_Mutex.lock();
		m_List.push_back(result);
		m_Mutex.unlock();
	}
	bool HasResponse()
	{
		m_Mutex.lock();
		if (m_List.size() > 0)
		{
			m_CurrentValue = m_List.front();
			m_List.pop_front();
			m_Mutex.unlock();
			return true;
		}
		Clear(m_CurrentValue);
		m_Mutex.unlock();
		return false;
	}
	list_type GetCurrent()
	{
		return m_CurrentValue;
	}
	/* Clears the callback list and current value variable. */
	void Reset()
	{
		m_Mutex.lock();
		Clear(m_CurrentValue);
		m_List.clear();
		if (m_bEnabled)
		{
			m_bEnabled = false;
			m_Callback.Unregister();
		}
		m_Mutex.unlock();
	}
};

#define _LIST_CALLBACK_MAIN(func, callback_type, list_type)						\
	STEAM_CALLBACK_MANUAL(CCallbacks, On##func, callback_type, m_Callback##func);\
public:																			\
	void Register##func##Callback()												\
	{																			\
		if (!g_SteamInitialized)												\
		{																		\
			return;																\
		}																		\
		if (!m_b##func##Enabled)												\
		{																		\
			m_b##func##Enabled = true;											\
			m_Callback##func##.Register(this, &CCallbacks::On##func##);			\
		}																		\
	}																			\
	void Unregister##func##Callback()											\
	{																			\
		if (!g_SteamInitialized)												\
		{																		\
			return;																\
		}																		\
		if (m_b##func##Enabled)													\
		{																		\
			m_b##func##Enabled = false;											\
			m_Callback##func##.Unregister();									\
		}																		\
	}																			\
	/* Moves the front value of the list into the current value variable. */	\
	bool Has##func##Response()													\
	{																			\
		m_##func##Mutex.lock();													\
		if (m_##func##List.size() > 0)											\
		{																		\
			m_Current##func = m_##func##List.front();							\
			m_##func##List.pop_front();											\
			m_##func##Mutex.unlock();											\
			return true;														\
		}																		\
		Clear(m_Current##func##);												\
		m_##func##Mutex.unlock();												\
		return false;															\
	}																			\
	list_type Get##func##()														\
	{																			\
		return m_Current##func##;												\
	}																			\
private:																		\
	/* Clears the callback list and current value variable. */					\
	void Clear##func##() 														\
	{																			\
		m_##func##Mutex.lock();													\
		Clear(m_Current##func##);												\
		m_##func##List.clear();													\
/*		while (m_##func##List.size() > 0)										\
		{																		\
			m_Current##func = m_##func##List.front();							\
			m_##func##List.pop_front();											\
			Clear(m_Current##func##);											\
		}																		\
*/		if (m_b##func##Enabled)													\
		{																		\
			m_b##func##Enabled = false;											\
			m_Callback##func##.Unregister();									\
		}																		\
		m_##func##Mutex.unlock();												\
	} 																			\
	bool m_b##func##Enabled;													\
	std::list<##list_type##> m_##func##List;									\
	std::mutex m_##func##Mutex;													\
	list_type m_Current##func

// Allow the list_type parameter to be 'optional'.  When omitted, use callback_type.
#define _LIST_CALLBACK_2(func, callback_type)				_LIST_CALLBACK_MAIN(func, callback_type, callback_type)
#define _LIST_CALLBACK_3(func, callback_type, list_type) 	_LIST_CALLBACK_MAIN(func, callback_type, list_type)
#define _LIST_CALLBACK_HELPER( _1, _2, SELECTED, ... )		_LIST_CALLBACK_##SELECTED
#define _LIST_CALLBACK_SELECT( X, Y )		_LIST_CALLBACK_HELPER X Y
#define LIST_CALLBACK(func, ...)			_LIST_CALLBACK_SELECT( ( __VA_ARGS__, 3, 2 ), ( func, __VA_ARGS__ ) )

#define BOOL_CALLBACK(func, callback_type)		\
	STEAM_CALLBACK(CCallbacks, On##func, callback_type);\
public:											\
	bool Has##func##Response();					\
private:										\
	void Clear##func##() 						\
	{											\
		m_b##func = false;						\
	} 											\
	bool m_b##func

class CCallbacks
{
public:
	CCallbacks();
	virtual ~CCallbacks(void);

	void Reset();
#pragma region ISteamApps
private:
	// AppProofOfPurchaseKeyResponse_t - Only used internally in Steam.
	LIST_CALLBACK(DlcInstalled, DlcInstalled_t);	// InstallDLC
	// FileDetailsResult_t - call result for GetFileDetails
	BOOL_CALLBACK(NewUrlLaunchParameters, NewUrlLaunchParameters_t);
	//BOOL_CALLBACK(NewLaunchQueryParameters, NewLaunchQueryParameters_t); - Removed SDK v1.43
	// RegisterActivationCodeResponse_t - Only used internally in Steam.
#pragma endregion

#pragma region ISteamAppTicket
	// This should never be needed in most cases.
#pragma endregion

#pragma region ISteamClient
	// No callbacks.
#pragma endregion

#pragma region ISteamController/ISteamInput
	// No callbacks.  Replaced by ISteamInput.
#pragma endregion

#pragma region ISteamFriends
private:
	LIST_CALLBACK(AvatarImageLoaded, AvatarImageLoaded_t, CSteamID); // GetFriendAvatar
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
	LIST_CALLBACK(GameLobbyJoinRequested, GameLobbyJoinRequested_t);
	STEAM_CALLBACK(CCallbacks, OnGameOverlayActivated, GameOverlayActivated_t); // always enabled.
	bool m_IsGameOverlayActive;
	// GameRichPresenceJoinRequested_t - InviteUserToGame
	// GameServerChangeRequested_t - fires when requesting to join game server from friends list.
	// JoinClanChatRoomCompletionResult_t - JoinClanChatRoom
	LIST_CALLBACK(PersonaStateChange, PersonaStateChange_t); // RequestUserInformation
	// SetPersonaNameResponse_t - SetPersonaName
public:
	bool IsGameOverlayActive() { return m_IsGameOverlayActive; }
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
private:
	// FavoritesListAccountsUpdated_t - no information given about this.
	LIST_CALLBACK(FavoritesListChanged, FavoritesListChanged_t);
	//LIST_CALLBACK(LobbyChatMessage, LobbyChatMsg_t, plugin::LobbyChatMsg_t);
	void OnLobbyChatMessage(LobbyChatMsg_t *pParam); // While in a lobby
	LIST_CALLBACK(LobbyChatUpdate, LobbyChatUpdate_t); // While in a lobby
	// LobbyCreated_t - Call result
	void OnLobbyDataUpdate(LobbyDataUpdate_t *pParam); // While in a lobby
	void OnLobbyEnter(LobbyEnter_t *pParam); // While in a lobby
	//LIST_CALLBACK(LobbyEnter, LobbyEnter_t); // CreateLobby, JoinLobby, also a call result
	LIST_CALLBACK(LobbyGameCreated, LobbyGameCreated_t); // While in a lobby
	//TODO LobbyInvite_t - Someone has invited you to join a Lobby. Normally you don't need to do anything with this...
	// LobbyKicked_t - Currently unused!
	// LobbyMatchList_t - Call result.
	// PSNGameBootInviteResult_t - deprecated
public:
	ListCallback<LobbyChatMsg_t, &CCallbacks::OnLobbyChatMessage, plugin::LobbyChatMsg_t> LobbyChatMessage;
	ListCallback<LobbyDataUpdate_t, &CCallbacks::OnLobbyDataUpdate> LobbyDataUpdate;
	ListCallback<LobbyEnter_t, &CCallbacks::OnLobbyEnter> LobbyEnter;
#pragma endregion

#pragma region ISteamMatchmakingServers
	// No callbacks.
#pragma endregion

#pragma region ISteamMusic
private:
	BOOL_CALLBACK(PlaybackStatusHasChanged, PlaybackStatusHasChanged_t);
	BOOL_CALLBACK(VolumeHasChanged, VolumeHasChanged_t); // TODO Return VolumeHasChanged_t.m_flNewVolume ?
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
private:
	// RemoteStorageAppSyncedClient_t - deprecated
	// RemoteStorageAppSyncedServer_t - deprecated
	// RemoteStorageAppSyncProgress_t - deprecated
	// RemoteStorageAppSyncStatusCheck_t - deprecated
	// RemoteStorageDeletePublishedFileResult_t - deprecated

	// RemoteStorageDownloadUGCResult_t - Call result for UGCDownload, UGCDownloadToLocation.
	
	// RemoteStorageEnumeratePublishedFilesByUserActionResult_t - deprecated
	// RemoteStorageEnumerateUserPublishedFilesResult_t - deprecated
	// RemoteStorageEnumerateUserSharedWorkshopFilesResult_t - deprecated
	// RemoteStorageEnumerateUserSubscribedFilesResult_t - deprecated
	// RemoteStorageEnumerateWorkshopFilesResult_t - deprecated

	// RemoteStorageFileReadAsyncComplete_t - Call result for FileReadAsync.
	// RemoteStorageFileShareResult_t - Call result for FileShare.
	// RemoteStorageFileWriteAsyncComplete_t - Call result for FileWriteAsync.

	// RemoteStorageGetPublishedFileDetailsResult_t - deprecated
	// RemoteStorageGetPublishedItemVoteDetailsResult_t - deprecated
	// RemoteStoragePublishedFileDeleted_t - deprecated
	// RemoteStoragePublishedFileSubscribed_t - deprecated
	// RemoteStoragePublishedFileUnsubscribed_t - deprecated
	// RemoteStoragePublishedFileUpdated_t - deprecated
	// RemoteStoragePublishFileProgress_t - deprecated
	// RemoteStoragePublishFileResult_t - deprecated
	// RemoteStorageSetUserPublishedFileActionResult_t - deprecated

	// RemoteStorageSubscribePublishedFileResult_t - Call result for SubscribePublishedFile, ISteamUGC::SubscribeItem.
	// RemoteStorageUnsubscribePublishedFileResult_t - Call result for UnsubscribePublishedFile, ISteamUGC::UnsubscribeItem.

	// RemoteStorageUpdatePublishedFileResult_t - deprecated
	// RemoteStorageUpdateUserPublishedItemVoteResult_t - deprecated
	// RemoteStorageUserVoteDetails_t - deprecated
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
	//ClientGameServerDeny_t - game server stuff
	//EncryptedAppTicketResponse_t - Call result for RequestEncryptedAppTicket.
	//GameWebCallback_t
	//GetAuthSessionTicketResponse_t - Call result for GetAuthSessionTicket.

	//IPCFailure_t - Steam client had a fatal error.

	//LicensesUpdated_t

	//MicroTxnAuthorizationResponse_t - micro transaction stuff.

	//SteamServerConnectFailure_t
	//SteamServersConnected_t
	//SteamServersDisconnected_t

	//StoreAuthURLResponse_t - Call result for RequestStoreAuthURL
	//ValidateAuthTicketResponse_t - Call result for BeginAuthSession
#pragma endregion

#pragma region ISteamUserStats
private:
	//GlobalAchievementPercentagesReady_t - Call result for RequestGlobalAchievementPercentages
	//GlobalStatsReceived_t - Call result for RequestGlobalStats
	//LeaderboardFindResult_t - Call result for FindOrCreateLeaderboard, FindLeaderboard
	//LeaderboardScoresDownloaded_t - Call result for DownloadLeaderboardEntries, DownloadLeaderboardEntriesForUsers
	//LeaderboardScoreUploaded_t - Call result for UploadLeaderboardScore
	//LeaderboardUGCSet_t - Call result for AttachLeaderboardUGC
	//NumberOfCurrentPlayers_t - Call result for GetNumberOfCurrentPlayers
	//PS3TrophiesInstalled_t - ignore
	STEAM_CALLBACK(CCallbacks, OnUserAchievementIconFetched, UserAchievementIconFetched_t);
	std::map<std::string, int> m_AchievementIconsMap;
	LIST_CALLBACK(UserAchievementStored, UserAchievementStored_t);
	LIST_CALLBACK(UserStatsReceived, UserStatsReceived_t);
	bool m_StatsInitialized;
	std::vector<CSteamID> m_StatsInitializedUsers;
	LIST_CALLBACK(UserStatsStored, UserStatsStored_t);
	LIST_CALLBACK(UserStatsUnloaded, UserStatsUnloaded_t, CSteamID); // -RequestUserStats again
public:
	// While GetAchievementIcon has an internal callback, there's no need to make them external.
	int GetAchievementIcon(const char *pchName);
	bool StatsInitialized() { return m_StatsInitialized; }
	bool StatsInitializedForUser(CSteamID user);
#pragma endregion

#pragma region ISteamUtils
private:
	// CheckFileSignature_t - Call result for  CheckFileSignature.
	LIST_CALLBACK(GamepadTextInputDismissed, GamepadTextInputDismissed_t, plugin::GamepadTextInputDismissed_t);
	BOOL_CALLBACK(IPCountryChanged, IPCountry_t);
	BOOL_CALLBACK(LowBatteryPower, LowBatteryPower_t);
	uint8 m_nMinutesBatteryLeft;
	//SteamAPICallCompleted_t - not needed
	BOOL_CALLBACK(SteamShutdown, SteamShutdown_t);
public:
	uint8 GetMinutesBatteryLeft() { return m_nMinutesBatteryLeft; }
#pragma endregion

#pragma region ISteamVideo
	// TODO?
#pragma endregion
};

CCallbacks *Callbacks();

#endif // _CALLBACKS_H
