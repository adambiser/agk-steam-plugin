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

class CCallbacks;

template <class callback_type, void(CCallbacks::*callback_function)(callback_type*), class list_type = callback_type>
class ListCallback
{
public:
	ListCallback() : m_bEnabled(false) {};
	virtual ~ListCallback() {}
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
	// Clears the callback response list and current value.
	void Reset()
	{
		m_Mutex.lock();
		ClearCurrent();// Clear(m_CurrentValue);
		m_List.clear();
		Unregister();
		m_Mutex.unlock();
	}
	// Only for use in the callback function.
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
		ClearCurrent();//Clear(m_CurrentValue);
		m_Mutex.unlock();
		return false;
	}
	list_type GetCurrent() const
	{
		return m_CurrentValue;
	}
	bool Enabled() const
	{
		return m_bEnabled;
	}
protected:
	virtual void ClearCurrent() = 0;
	list_type m_CurrentValue;
private:
	bool m_bEnabled;
	std::list<list_type> m_List;
	std::mutex m_Mutex;
	CCallbackManual<CCallbacks, callback_type> m_Callback;
};

class BoolCallback
{
public:
	BoolCallback() : m_bHadResponse(false) {};
	// Clears the callback response list and current value.
	virtual void Reset()
	{
		m_bHadResponse = false;
	}
	// Only for use in the callback function.
	void Trigger()
	{
		m_bHadResponse = true;
	}
	bool HasResponse()
	{
		if (m_bHadResponse)
		{
			m_bHadResponse = false;
			return true;
		}
		return false;
	}
protected:
	bool m_bHadResponse;
};

class CCallbacks
{
public:
	CCallbacks();
	virtual ~CCallbacks(void);

	void Reset();
#pragma region ISteamApps
private:
	// AppProofOfPurchaseKeyResponse_t - Only used internally in Steam.
	void OnDlcInstalled(DlcInstalled_t*);	// InstallDLC
	// FileDetailsResult_t - call result for GetFileDetails
	void OnNewUrlLaunchParameters(NewUrlLaunchParameters_t*);
	//NewLaunchQueryParameters_t - Removed SDK v1.43
	// RegisterActivationCodeResponse_t - Only used internally in Steam.
public:
	void Clear(DlcInstalled_t &value)
	{
		value.m_nAppID = 0;
	}
	class : public ListCallback<DlcInstalled_t, &OnDlcInstalled>
	{
	protected:
		void ClearCurrent()
		{
			m_CurrentValue.m_nAppID = 0;
		}
	} DlcInstalled;	// InstallDLC
	BoolCallback NewUrlLaunchParameters;
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
	void OnAvatarImageLoaded(AvatarImageLoaded_t*); // GetFriendAvatar
	// ClanOfficerListResponse_t - call result for RequestClanOfficerList
	// DownloadClanActivityCountsResult_t - call result for DownloadClanActivityCounts
	void OnFriendRichPresenceUpdate(FriendRichPresenceUpdate_t*); // RequestFriendRichPresence, always fires
	// FriendsEnumerateFollowingList_t - call result for EnumerateFollowingList
	// FriendsGetFollowerCount_t - call result for GetFollowerCount
	// FriendsIsFollowing_t - call result for IsFollowing
	void OnGameConnectedChatJoin(GameConnectedChatJoin_t*); // JoinClanChatRoom
	void OnGameConnectedChatLeave(GameConnectedChatLeave_t*); // LeaveClanChatRoom
	void OnGameConnectedClanChatMsg(GameConnectedClanChatMsg_t*); // JoinClanChatRoom
	void OnGameConnectedFriendChatMsg(GameConnectedFriendChatMsg_t*); // SetListenForFriendsMessages
	void OnGameLobbyJoinRequested(GameLobbyJoinRequested_t*);
	STEAM_CALLBACK(CCallbacks, OnGameOverlayActivated, GameOverlayActivated_t); // always enabled.
	bool m_IsGameOverlayActive;
	// GameRichPresenceJoinRequested_t - InviteUserToGame
	// GameServerChangeRequested_t - fires when requesting to join game server from friends list.
	// JoinClanChatRoomCompletionResult_t - JoinClanChatRoom
	void OnPersonaStateChange(PersonaStateChange_t*); // RequestUserInformation
	// SetPersonaNameResponse_t - SetPersonaName
public:
	class : public ListCallback<AvatarImageLoaded_t, &OnAvatarImageLoaded, CSteamID>
	{
	protected:
		void ClearCurrent()
		{
			m_CurrentValue = k_steamIDNil;
		}
	} AvatarImageLoaded; // GetFriendAvatar

	class : public ListCallback<FriendRichPresenceUpdate_t, &OnFriendRichPresenceUpdate, CSteamID>
	{
	protected:
		void ClearCurrent()
		{
			m_CurrentValue = k_steamIDNil;
		}
	} FriendRichPresenceUpdate;

	class : public ListCallback<GameConnectedChatJoin_t, &OnGameConnectedChatJoin>
	{
	protected:
		void ClearCurrent()
		{
			m_CurrentValue.m_steamIDClanChat = k_steamIDNil;
			m_CurrentValue.m_steamIDUser = k_steamIDNil;
		}
	} GameConnectedChatJoin;
	class : public ListCallback<GameConnectedChatLeave_t, &OnGameConnectedChatLeave>
	{
	protected:
		void ClearCurrent()
		{
			m_CurrentValue.m_steamIDClanChat = k_steamIDNil;
			m_CurrentValue.m_steamIDUser = k_steamIDNil;
			m_CurrentValue.m_bDropped = false;
			m_CurrentValue.m_bKicked = false;
		}
	} GameConnectedChatLeave;
	//ListCallback<GameConnectedClanChatMsg_t, &OnGameConnectedClanChatMsg, plugin::GameConnectedClanChatMsg_t> GameConnectedClanChatMsg;
	//ListCallback<GameConnectedFriendChatMsg_t, &OnGameConnectedFriendChatMsg, plugin::GameConnectedFriendChatMsg_t> GameConnectedFriendChatMsg_;
	class : public ListCallback<GameLobbyJoinRequested_t, &OnGameLobbyJoinRequested>
	{
	protected:
		void ClearCurrent()
		{
			m_CurrentValue.m_steamIDLobby = k_steamIDNil;
			m_CurrentValue.m_steamIDFriend = k_steamIDNil;
		}
	} GameLobbyJoinRequested;
	bool IsGameOverlayActive() { return m_IsGameOverlayActive; }
	class : public ListCallback<PersonaStateChange_t, &OnPersonaStateChange>
	{
	protected:
		void ClearCurrent()
		{
			m_CurrentValue.m_ulSteamID = 0;
			m_CurrentValue.m_nChangeFlags = 0;
		}
	}PersonaStateChange; // RequestUserInformation
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
	void OnFavoritesListChanged(FavoritesListChanged_t*);
	void OnLobbyChatMessage(LobbyChatMsg_t*); // While in a lobby
	void OnLobbyChatUpdate(LobbyChatUpdate_t*); // While in a lobby
	// LobbyCreated_t - Call result
	void OnLobbyDataUpdate(LobbyDataUpdate_t*); // While in a lobby
	void OnLobbyEnter(LobbyEnter_t*); // CreateLobby, JoinLobby, also a call result
	void OnLobbyGameCreated(LobbyGameCreated_t*); // While in a lobby
	//TODO LobbyInvite_t - Someone has invited you to join a Lobby. Normally you don't need to do anything with this...
	// LobbyKicked_t - Currently unused!
	// LobbyMatchList_t - Call result.
	// PSNGameBootInviteResult_t - deprecated
public:
	class : public ListCallback<FavoritesListChanged_t, &OnFavoritesListChanged>
	{
	protected:
		void ClearCurrent()
		{
			m_CurrentValue.m_nIP = 0;
			m_CurrentValue.m_nQueryPort = 0;
			m_CurrentValue.m_nConnPort = 0;
			m_CurrentValue.m_nAppID = 0;
			m_CurrentValue.m_nFlags = 0;
			m_CurrentValue.m_bAdd = false;
			m_CurrentValue.m_unAccountId = 0;
		}
	} FavoritesListChanged;
	class : public ListCallback<LobbyChatUpdate_t, &OnLobbyChatUpdate>
	{
	protected:
		void ClearCurrent()
		{
			m_CurrentValue.m_ulSteamIDLobby = 0;
			m_CurrentValue.m_ulSteamIDUserChanged = 0;
			m_CurrentValue.m_ulSteamIDMakingChange = 0;
			m_CurrentValue.m_rgfChatMemberStateChange = 0;
		}
	} LobbyChatUpdate;
	class : public ListCallback<LobbyChatMsg_t, &OnLobbyChatMessage, plugin::LobbyChatMsg_t>
	{
	protected:
		void ClearCurrent()
		{
			m_CurrentValue.m_ulSteamIDLobby = k_steamIDNil;
			m_CurrentValue.m_ulSteamIDUser = k_steamIDNil;
			m_CurrentValue.m_eChatEntryType = (EChatEntryType)0;
			m_CurrentValue.m_MemblockID = 0; // TODO ditch the special struct, move this into the call back class.
		}
	} LobbyChatMessage;
	class : public ListCallback<LobbyDataUpdate_t, &OnLobbyDataUpdate>
	{
	protected:
		void ClearCurrent()
		{
			m_CurrentValue.m_ulSteamIDLobby = 0;
			m_CurrentValue.m_ulSteamIDMember = 0;
			m_CurrentValue.m_bSuccess = 0;
		}
	} LobbyDataUpdate;
	class : public ListCallback<LobbyEnter_t, &OnLobbyEnter>
	{
	protected:
		void ClearCurrent()
		{
			m_CurrentValue.m_ulSteamIDLobby = 0;
			m_CurrentValue.m_rgfChatPermissions = 0;
			m_CurrentValue.m_bLocked = false;
			m_CurrentValue.m_EChatRoomEnterResponse = 0;
		}
	} LobbyEnter;
	class : public ListCallback<LobbyGameCreated_t, &OnLobbyGameCreated>
	{
	protected:
		void ClearCurrent()
		{
			m_CurrentValue.m_ulSteamIDLobby = 0;
			m_CurrentValue.m_ulSteamIDGameServer = 0;
			m_CurrentValue.m_unIP = 0;
			m_CurrentValue.m_usPort = 0;
		}
	} LobbyGameCreated;
#pragma endregion

#pragma region ISteamMatchmakingServers
	// No callbacks.
#pragma endregion

#pragma region ISteamMusic
private:
	STEAM_CALLBACK(CCallbacks, OnPlaybackStatusHasChanged, PlaybackStatusHasChanged_t);
	STEAM_CALLBACK(CCallbacks, OnVolumeHasChanged, VolumeHasChanged_t);
public:
	BoolCallback PlaybackStatusHasChanged;
	BoolCallback VolumeHasChanged;
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
	void OnUserAchievementStored(UserAchievementStored_t*);
	void OnUserStatsReceived(UserStatsReceived_t*);
	bool m_StatsInitialized;
	std::vector<CSteamID> m_StatsInitializedUsers;
	void OnUserStatsStored(UserStatsStored_t*);
	void OnUserStatsUnloaded(UserStatsUnloaded_t*); // -RequestUserStats again
public:
	// While GetAchievementIcon has an internal callback, there's no need to make them external.
	int GetAchievementIcon(const char *pchName);
	class : public ListCallback<UserAchievementStored_t, &OnUserAchievementStored>
	{
	protected:
		void ClearCurrent()
		{
			m_CurrentValue.m_nGameID = 0;
			m_CurrentValue.m_bGroupAchievement = false;
			m_CurrentValue.m_rgchAchievementName[0] = 0;
			m_CurrentValue.m_nCurProgress = 0;
			m_CurrentValue.m_nMaxProgress = 0;
		}
	} UserAchievementStored;
	class : public ListCallback<UserStatsReceived_t, &OnUserStatsReceived>
	{
	protected:
		void ClearCurrent()
		{
			m_CurrentValue.m_nGameID = 0;
			m_CurrentValue.m_eResult = (EResult)0;
			m_CurrentValue.m_steamIDUser = k_steamIDNil;
		}
	} UserStatsReceived;
	bool StatsInitialized() { return m_StatsInitialized; }
	bool StatsInitializedForUser(CSteamID user);
	class : public ListCallback<UserStatsStored_t, &OnUserStatsStored>
	{
	protected:
		void ClearCurrent()
		{
			m_CurrentValue.m_nGameID = 0;
			m_CurrentValue.m_eResult = (EResult)0;
		}
	} UserStatsStored;
	class : public ListCallback<UserStatsUnloaded_t, &OnUserStatsUnloaded, CSteamID>
	{
	protected:
		void ClearCurrent()
		{
			m_CurrentValue = k_steamIDNil;
		}
	} UserStatsUnloaded; // Call RequestUserStats again
#pragma endregion

#pragma region ISteamUtils
private:
	// CheckFileSignature_t - Call result for  CheckFileSignature.
	void OnGamepadTextInputDismissed(GamepadTextInputDismissed_t*);
	STEAM_CALLBACK(CCallbacks, OnIPCountryChanged, IPCountry_t);
	STEAM_CALLBACK(CCallbacks, OnLowBatteryPower, LowBatteryPower_t);
	//SteamAPICallCompleted_t - not needed
	STEAM_CALLBACK(CCallbacks, OnSteamShutdown, SteamShutdown_t);
public:
	class : public ListCallback<GamepadTextInputDismissed_t, &OnGamepadTextInputDismissed, plugin::GamepadTextInputDismissed_t>
	{
	protected:
		void ClearCurrent()
		{
			m_CurrentValue.m_bSubmitted = 0;
			m_CurrentValue.m_chSubmittedText[0] = 0; // TODO ditch special struct. move into class
		}
	} GamepadTextInputDismissed;
	BoolCallback IPCountryChanged;
	class : public BoolCallback
	{
	public:
		void Reset()
		{
			BoolCallback::Reset();
			m_Response.m_nMinutesBatteryLeft = 0;
		}
		uint8 GetMinutesBatteryLeft() { return m_Response.m_nMinutesBatteryLeft; }
		LowBatteryPower_t m_Response;
	} LowBatteryPower;
	BoolCallback SteamShutdown;
#pragma endregion

#pragma region ISteamVideo
	// TODO?
#pragma endregion
};

CCallbacks *Callbacks();

#endif // _CALLBACKS_H
