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

#define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <stdio.h>
#include <steam_api.h>
//#include <steam_gameserver.h>
#include "SteamPlugin.h"
#include <string>
#include <sstream>
#include "..\AGKLibraryCommands.h"

#pragma comment(lib, "steam_api.lib")

#define CheckInitialized(steamInterface, returnValue)				\
	if (!m_SteamInitialized || (NULL == steamInterface()))			\
	{																\
		agk::PluginError("Steam API has not been initialized.");	\
		return returnValue;											\
	}

extern "C" void __cdecl SteamAPIDebugTextHook(int nSeverity, const char *pchDebugText)
{
	agk::Log(pchDebugText);
}

template <typename M> void FreeClearMap(M & map)
{
	for (typename M::iterator iter = map.begin(); iter != map.end(); iter++) {
		delete[] iter->second;
	}
	map.clear();
}

//CFileWriteAsyncItem::CFileWriteAsyncItem() :
//	m_CallbackState(Idle),
//	m_Result((EResult)0)
//{
//}
//
//bool CFileWriteAsyncItem::Call(const char *pchFile, const void *pvData, uint32 cubData)
//{
//	m_Filename = pchFile;
//	std::ostringstream msg;
//	msg << "FileWriteAsync: " << m_Filename << ".";
//	agk::Log(msg.str().c_str());
//	try
//	{
//		SteamAPICall_t hSteamAPICall = SteamRemoteStorage()->FileWriteAsync(pchFile, pvData, cubData);
//		m_CallResult.Set(hSteamAPICall, this, &CFileWriteAsyncItem::OnRemoteStorageFileWriteAsyncComplete);
//		m_CallbackState = Running;
//		return true;
//	}
//	catch (...)
//	{
//		m_CallbackState = ClientError;
//		return false;
//	}
//}
//
//void CFileWriteAsyncItem::OnRemoteStorageFileWriteAsyncComplete(RemoteStorageFileWriteAsyncComplete_t *pResult, bool bFailure)
//{
//	m_Result = pResult->m_eResult;
//	std::ostringstream msg;
//	msg << "RemoteStorageFileWriteAsyncComplete: " << m_Filename << " = ";
//	if (pResult->m_eResult == k_EResultOK && !bFailure)
//	{
//		msg << "success";
//		m_CallbackState = Done;
//	}
//	else
//	{
//		msg << "error: " << pResult->m_eResult;
//		m_CallbackState = ServerError;
//	}
//	agk::Log(msg.str().c_str());
//}

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
	m_DownloadedLeaderboardEntryCount(0),
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
	// Nothing for now
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
		m_DownloadedLeaderboardEntryCount = 0;
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
		//m_FileWriteAsyncCallbackState = Idle;
		//m_FileWriteAsyncComplete = (EResult)0;
	}
}

bool SteamPlugin::RestartAppIfNecessary(uint32 unOwnAppID)
{
	return SteamAPI_RestartAppIfNecessary(unOwnAppID);
}

int SteamPlugin::GetAppID()
{
	CheckInitialized(SteamUtils, 0);
	return SteamUtils()->GetAppID();
}

int SteamPlugin::GetAppName(AppId_t nAppID, char *pchName, int cchNameMax)
{
	CheckInitialized(SteamAppList, 0);
	return SteamAppList()->GetAppName(nAppID, pchName, cchNameMax);
}

bool SteamPlugin::LoggedOn()
{
	CheckInitialized(SteamUser, 0);
	return SteamUser()->BLoggedOn();
}

void SteamPlugin::RunCallbacks()
{
	if (!m_SteamInitialized)
	{
		return;
	}
	SteamAPI_RunCallbacks();
}

// App/DLC methods
bool SteamPlugin::GetDLCDataByIndex(int iDLC, AppId_t *pAppID, bool *pbAvailable, char *pchName, int cchNameBufferSize)
{
	CheckInitialized(SteamApps, false);
	return SteamApps()->BGetDLCDataByIndex(iDLC, pAppID, pbAvailable, pchName, cchNameBufferSize);
}

bool SteamPlugin::IsAppInstalled(AppId_t appID)
{
	CheckInitialized(SteamApps, false);
	return SteamApps()->BIsAppInstalled(appID);
}

bool SteamPlugin::IsCybercafe()
{
	CheckInitialized(SteamApps, false);
	return SteamApps()->BIsCybercafe();
}

bool SteamPlugin::IsDlcInstalled(AppId_t appID)
{
	CheckInitialized(SteamApps, false);
	return SteamApps()->BIsDlcInstalled(appID);
}

bool SteamPlugin::IsLowViolence()
{
	CheckInitialized(SteamApps, false);
	return SteamApps()->BIsLowViolence();
}

bool SteamPlugin::IsSubscribed()
{
	CheckInitialized(SteamApps, false);
	return SteamApps()->BIsSubscribed();
}

bool SteamPlugin::IsSubscribedApp(AppId_t appID)
{
	CheckInitialized(SteamApps, false);
	return SteamApps()->BIsSubscribedApp(appID);
}

bool SteamPlugin::IsSubscribedFromFreeWeekend()
{
	CheckInitialized(SteamApps, false);
	return SteamApps()->BIsSubscribedFromFreeWeekend();
}

bool SteamPlugin::IsVACBanned()
{
	CheckInitialized(SteamApps, false);
	return SteamApps()->BIsVACBanned();
}

int SteamPlugin::GetAppBuildId()
{
	CheckInitialized(SteamApps, 0);
	return SteamApps()->GetAppBuildId();
}

uint32 SteamPlugin::GetAppInstallDir(AppId_t appID, char *pchFolder, uint32 cchFolderBufferSize)
{
	CheckInitialized(SteamApps, 0);
	return SteamApps()->GetAppInstallDir(appID, pchFolder, cchFolderBufferSize);
}

CSteamID SteamPlugin::GetAppOwner()
{
	CheckInitialized(SteamApps, k_steamIDNil);
	return SteamApps()->GetAppOwner();
}

const char * SteamPlugin::GetAvailableGameLanguages()
{
	CheckInitialized(SteamApps, NULL);
	return SteamApps()->GetAvailableGameLanguages();
}

bool SteamPlugin::GetCurrentBetaName(char *pchName, int cchNameBufferSize)
{
	CheckInitialized(SteamApps, false);
	return SteamApps()->GetCurrentBetaName(pchName, cchNameBufferSize);
}

const char * SteamPlugin::GetCurrentGameLanguage()
{
	CheckInitialized(SteamApps, NULL);
	return SteamApps()->GetCurrentGameLanguage();
}

int SteamPlugin::GetDLCCount()
{
	CheckInitialized(SteamApps, 0);
	return SteamApps()->GetDLCCount();
}

bool SteamPlugin::GetDlcDownloadProgress(AppId_t nAppID, uint64 *punBytesDownloaded, uint64 *punBytesTotal)
{
	CheckInitialized(SteamApps, false);
	return SteamApps()->GetDlcDownloadProgress(nAppID, punBytesDownloaded, punBytesTotal);
}

uint32 SteamPlugin::GetEarliestPurchaseUnixTime(AppId_t nAppID)
{
	CheckInitialized(SteamApps, 0);
	return SteamApps()->GetEarliestPurchaseUnixTime(nAppID);
}

//SteamAPICall_t SteamPlugin::GetFileDetails(const char*pszFileName); // FileDetailsResult_t call result.

uint32 SteamPlugin::GetInstalledDepots(AppId_t appID, DepotId_t *pvecDepots, uint32 cMaxDepots)
{
	CheckInitialized(SteamApps, 0);
	return SteamApps()->GetInstalledDepots(appID, pvecDepots, cMaxDepots);
}

const char * SteamPlugin::GetLaunchQueryParam(const char *pchKey)
{
	CheckInitialized(SteamApps, NULL);
	return SteamApps()->GetLaunchQueryParam(pchKey);
}

void SteamPlugin::OnNewLaunchQueryParameters(NewLaunchQueryParameters_t *pParam)
{
	agk::Log("Callback: New Launch Query Parameters");
	m_HasNewLaunchQueryParameters = true;
}

bool SteamPlugin::HasNewLaunchQueryParameters()
{
	// Reports true only once per change.
	if (m_HasNewLaunchQueryParameters)
	{
		m_HasNewLaunchQueryParameters = false;
		return true;
	}
	return false;
}

void SteamPlugin::OnDlcInstalled(DlcInstalled_t *pParam)
{
	agk::Log("Callback: DLC Installed");
	// Don't store unless the main program is using this functionality.
	if (m_OnDlcInstalledEnabled)
	{
		m_DlcInstalledList.push_back(pParam->m_nAppID);
	}
}

bool SteamPlugin::HasNewDlcInstalled()
{
	m_OnDlcInstalledEnabled = true;
	if (m_DlcInstalledList.size() > 0)
	{
		m_NewDlcInstalled = m_DlcInstalledList.front();
		m_DlcInstalledList.pop_front();
		return true;
	}
	m_NewDlcInstalled = 0;
	return false;
}

void SteamPlugin::InstallDLC(AppId_t nAppID)
{
	// Triggers a DlcInstalled_t callback.
	CheckInitialized(SteamApps, );
	m_OnDlcInstalledEnabled = true;
	return SteamApps()->InstallDLC(nAppID);
}

bool SteamPlugin::MarkContentCorrupt(bool bMissingFilesOnly)
{
	CheckInitialized(SteamApps, false);
	return SteamApps()->MarkContentCorrupt(bMissingFilesOnly);
}

void SteamPlugin::UninstallDLC(AppId_t nAppID)
{
	CheckInitialized(SteamApps, );
	return SteamApps()->UninstallDLC(nAppID);
}

// Overlay methods
void SteamPlugin::OnGameOverlayActivated(GameOverlayActivated_t *pParam)
{
	agk::Log("Callback: Game Overlay Activated");
	m_IsGameOverlayActive = pParam->m_bActive != 0;
}

void SteamPlugin::ActivateGameOverlay(const char *pchDialog)
{
	CheckInitialized(SteamFriends, );
	SteamFriends()->ActivateGameOverlay(pchDialog);
}

void SteamPlugin::ActivateGameOverlayInviteDialog(CSteamID steamIDLobby)
{
	CheckInitialized(SteamFriends, );
	SteamFriends()->ActivateGameOverlayInviteDialog(steamIDLobby);
}

void SteamPlugin::ActivateGameOverlayToStore(AppId_t nAppID, EOverlayToStoreFlag eFlag)
{
	CheckInitialized(SteamFriends, );
	SteamFriends()->ActivateGameOverlayToStore(nAppID, eFlag);
}

void SteamPlugin::ActivateGameOverlayToUser(const char *pchDialog, CSteamID steamID)
{
	CheckInitialized(SteamFriends, );
	SteamFriends()->ActivateGameOverlayToUser(pchDialog, steamID);
}

void SteamPlugin::ActivateGameOverlayToWebPage(const char *pchURL)
{
	CheckInitialized(SteamFriends, );
	SteamFriends()->ActivateGameOverlayToWebPage(pchURL);
}

const char *SteamPlugin::GetPersonaName()
{
	CheckInitialized(SteamFriends, NULL);
	return SteamFriends()->GetPersonaName();
}

CSteamID SteamPlugin::GetSteamID()
{
	CheckInitialized(SteamUser, k_steamIDNil);
	return SteamUser()->GetSteamID();
}

// Callback for RequestUserInformation and more.
void SteamPlugin::OnPersonaStateChanged(PersonaStateChange_t *pParam)
{
	agk::Log("Callback: Persona State Changed");
	if (m_PersonaStateChangedEnabled)
	{
		m_PersonaStateChangeList.push_back(*pParam);
	}
	if (m_AvatarImageLoadedEnabled)
	{
		if (pParam->m_nChangeFlags & k_EPersonaChangeAvatar)
		{
			// Allow HasAvatarImageLoaded to report avatar changes from here as well.
			m_AvatarImageLoadedUsers.push_back(pParam->m_ulSteamID);
		}
	}
}

bool SteamPlugin::HasPersonaStateChanged()
{
	if (m_PersonaStateChangeList.size() > 0)
	{
		m_PersonaStateChange = m_PersonaStateChangeList.front();
		m_PersonaStateChangeList.pop_front();
		return true;
	}
	m_PersonaStateChange.m_ulSteamID = NULL;
	m_PersonaStateChange.m_nChangeFlags = 0;
	return false;
}

bool SteamPlugin::RequestUserInformation(CSteamID steamIDUser, bool bRequireNameOnly)
{
	CheckInitialized(SteamFriends, false);
	m_PersonaStateChangedEnabled = true;
	return SteamFriends()->RequestUserInformation(steamIDUser, bRequireNameOnly);
}

void SteamPlugin::OnAvatarImageLoaded(AvatarImageLoaded_t *pParam)
{
	agk::Log("Callback: Avatar Image Loaded");
	if (!m_AvatarImageLoadedEnabled)
	{
		return;
	}
	m_AvatarImageLoadedUsers.push_back(pParam->m_steamID);
}

bool SteamPlugin::HasAvatarImageLoaded()
{
	if (m_AvatarImageLoadedUsers.size() > 0)
	{
		m_AvatarUser = m_AvatarImageLoadedUsers.front();
		m_AvatarImageLoadedUsers.pop_front();
		return true;
	}
	m_AvatarUser = k_steamIDNil;
	return false;
}

// NOTE: The Steam API appears to implicitly call RequestUserInformation when needed.
int SteamPlugin::GetFriendAvatar(CSteamID steamID, EAvatarSize size)
{
	CheckInitialized(SteamFriends, 0);
	m_AvatarImageLoadedEnabled = true;
	//SteamFriends()->RequestUserInformation(steamID, false);
	int hImage = 0;
	switch (size)
	{
	case Small:
		hImage = SteamFriends()->GetSmallFriendAvatar(steamID);
		break;
	case Medium:
		hImage = SteamFriends()->GetMediumFriendAvatar(steamID);
		break;
	case Large:
		hImage = SteamFriends()->GetLargeFriendAvatar(steamID);
		break;
	default:
		agk::PluginError("Requested invalid avatar size.");
		return 0;
	}
	return hImage;
}

int SteamPlugin::GetFriendCount(EFriendFlags iFriendFlags)
{
	CheckInitialized(SteamFriends, 0);
	m_PersonaStateChangedEnabled = true;
	return SteamFriends()->GetFriendCount(iFriendFlags);
}
CSteamID SteamPlugin::GetFriendByIndex(int iFriend, EFriendFlags iFriendFlags)
{
	CheckInitialized(SteamFriends, k_steamIDNil);
	return SteamFriends()->GetFriendByIndex(iFriend, iFriendFlags);
}

bool SteamPlugin::GetFriendGamePlayed(CSteamID steamIDFriend, FriendGameInfo_t *pFriendGameInfo)
{
	CheckInitialized(SteamFriends, false);
	m_PersonaStateChangedEnabled = true;
	return SteamFriends()->GetFriendGamePlayed(steamIDFriend, pFriendGameInfo);
}

const char *SteamPlugin::GetFriendPersonaName(CSteamID steamIDFriend)
{
	CheckInitialized(SteamFriends, NULL);
	m_PersonaStateChangedEnabled = true;
	return SteamFriends()->GetFriendPersonaName(steamIDFriend);
}

EPersonaState SteamPlugin::GetFriendPersonaState(CSteamID steamIDFriend)
{
	CheckInitialized(SteamFriends, k_EPersonaStateOffline);
	m_PersonaStateChangedEnabled = true;
	return SteamFriends()->GetFriendPersonaState(steamIDFriend);
}

EFriendRelationship SteamPlugin::GetFriendRelationship(CSteamID steamIDFriend)
{
	CheckInitialized(SteamFriends, k_EFriendRelationshipNone);
	return SteamFriends()->GetFriendRelationship(steamIDFriend);
}

/*
NOTE:
Steam docs say
"If the Steam level is not immediately available for the specified user then this returns 0 and queues it to be downloaded from the Steam servers. 
When it gets downloaded a PersonaStateChange_t callback will be posted with m_nChangeFlags including k_EPersonaChangeSteamLevel."

HOWEVER, this doesn't actually appear to be the case.  GetFriendSteamLevel returns 0, but the callback never seems to fire.
Possible solution is to just keep requesting the level when 0 is returned.
*/
int SteamPlugin::GetFriendSteamLevel(CSteamID steamIDFriend)
{
	CheckInitialized(SteamFriends, 0);
	m_PersonaStateChangedEnabled = true;
	return SteamFriends()->GetFriendSteamLevel(steamIDFriend);
}

const char *SteamPlugin::GetPlayerNickname(CSteamID steamIDPlayer)
{
	CheckInitialized(SteamFriends, NULL);
	return SteamFriends()->GetPlayerNickname(steamIDPlayer);
}

bool SteamPlugin::HasFriend(CSteamID steamIDFriend, EFriendFlags iFriendFlags)
{
	CheckInitialized(SteamFriends, k_EPersonaStateOffline);
	return SteamFriends()->HasFriend(steamIDFriend, iFriendFlags);
}

// Friend group methods
int SteamPlugin::GetFriendsGroupCount()
{
	CheckInitialized(SteamFriends, 0);
	return SteamFriends()->GetFriendsGroupCount();
}

FriendsGroupID_t SteamPlugin::GetFriendsGroupIDByIndex(int iFriendGroup)
{
	CheckInitialized(SteamFriends, k_FriendsGroupID_Invalid);
	return SteamFriends()->GetFriendsGroupIDByIndex(iFriendGroup);
}

int SteamPlugin::GetFriendsGroupMembersCount(FriendsGroupID_t friendsGroupID)
{
	CheckInitialized(SteamFriends, 0);
	return SteamFriends()->GetFriendsGroupMembersCount(friendsGroupID);
}

void SteamPlugin::GetFriendsGroupMembersList(FriendsGroupID_t friendsGroupID, CSteamID *pOutSteamIDMembers, int nMembersCount)
{
	if (!m_SteamInitialized || NULL == SteamFriends())
	{
		pOutSteamIDMembers = NULL;
		return;
	}
	SteamFriends()->GetFriendsGroupMembersList(friendsGroupID, pOutSteamIDMembers, nMembersCount);
}

const char *SteamPlugin::GetFriendsGroupName(FriendsGroupID_t friendsGroupID)
{
	CheckInitialized(SteamFriends, NULL);
	return SteamFriends()->GetFriendsGroupName(friendsGroupID);
}

int SteamPlugin::LoadImageFromHandle(int hImage)
{
	return SteamPlugin::LoadImageFromHandle(0, hImage);
}

int SteamPlugin::LoadImageFromHandle(int imageID, int hImage)
{
	if (hImage == -1 || hImage == 0)
	{
		return 0;
	}
	uint32 width;
	uint32 height;
	if (!SteamUtils()->GetImageSize(hImage, &width, &height))
	{
		agk::PluginError("GetImageSize failed.");
		return 0;
	}
	// Get the actual raw RGBA data from Steam and turn it into a texture in our game engine
	const int imageSizeInBytes = width * height * 4;
	uint8 *imageBuffer = new uint8[imageSizeInBytes];
	if (SteamUtils()->GetImageRGBA(hImage, imageBuffer, imageSizeInBytes))
	{
		unsigned int memID = agk::CreateMemblock(imageSizeInBytes + 4 * 3);
		agk::SetMemblockInt(memID, 0, width);
		agk::SetMemblockInt(memID, 4, height);
		agk::SetMemblockInt(memID, 8, 32); // bitdepth always 32
		for (int index = 0, offset = 12; index < imageSizeInBytes; index++, offset++)
		{
			agk::SetMemblockByte(memID, offset, imageBuffer[index]);
		}
		if (imageID)
		{
			agk::CreateImageFromMemblock(imageID, memID);
		}
		else
		{
			imageID = agk::CreateImageFromMemblock(memID);
		}
		agk::DeleteMemblock(memID);
	}
	else
	{
		imageID = 0;
		agk::PluginError("GetImageRGBA failed.");
	}
	// Free memory.
	delete[] imageBuffer;
	return imageID;
}

// Callback for RequestStats.
void SteamPlugin::OnUserStatsReceived(UserStatsReceived_t *pCallback)
{
	if (pCallback->m_nGameID == m_AppID)
	{
		if (pCallback->m_eResult == k_EResultOK)
		{
			agk::Log("Callback: User Stats Received Succeeded");
			m_RequestStatsCallbackState = Done;
			m_StatsInitialized = true;
		}
		else
		{
			agk::Log("Callback: User Stats Received Failed");
			m_RequestStatsCallbackState = ServerError;
		}
	}
}

bool SteamPlugin::RequestStats()
{
	// Set to false so that we can detect when the callback finishes for this request.
	m_StatsInitialized = false;
	CheckInitialized(SteamUser, false);
	if (!SteamUser()->BLoggedOn())
	{
		return false;
	}
	CheckInitialized(SteamUserStats, false);
	// Fail when the callback is currently running.
	if (m_RequestStatsCallbackState == Running)
	{
		return false;
	}
	if (!SteamUserStats()->RequestCurrentStats())
	{
		m_RequestStatsCallbackState = ClientError;
		return false;
	}
	m_RequestStatsCallbackState = Running;
	return true;
}

// Callback for StoreStats and ResetAllStats.
void SteamPlugin::OnUserStatsStored(UserStatsStored_t *pCallback)
{
	if (pCallback->m_nGameID == m_AppID)
	{
		if (pCallback->m_eResult == k_EResultOK)
		{
			agk::Log("Callback: User Stats Stored Succeeded");
			m_StoreStatsCallbackState = Done;
			m_StatsStored = true;
		}
		else
		{
			agk::Log("Callback: User Stats Stored Failed");
			m_StoreStatsCallbackState = ServerError;
		}
	}
}

// Callback for StoreStats.
void SteamPlugin::OnAchievementStored(UserAchievementStored_t *pCallback)
{
	if (pCallback->m_nGameID == m_AppID)
	{
		agk::Log("Callback: Achievement Stored");
		m_AchievementStored = true;
	}
}

bool SteamPlugin::StoreStats()
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

bool SteamPlugin::ResetAllStats(bool bAchievementsToo)
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

int SteamPlugin::GetNumAchievements()
{
	CheckInitialized(SteamUserStats, 0);
	return SteamUserStats()->GetNumAchievements();
}

const char *SteamPlugin::GetAchievementAPIName(int index)
{
	CheckInitialized(SteamUserStats, NULL);
	return SteamUserStats()->GetAchievementName(index);
}

const char *SteamPlugin::GetAchievementDisplayAttribute(const char *pchName, const char *pchKey)
{
	CheckInitialized(SteamUserStats, NULL);
	return SteamUserStats()->GetAchievementDisplayAttribute(pchName, pchKey);
}

// Callback for GetAchievementIcon.
void SteamPlugin::OnAchievementIconFetched(UserAchievementIconFetched_t *pParam)
{
	if (pParam->m_nGameID.AppID() == m_AppID)
	{
		agk::Log("Callback: Achievement Icon Fetched");
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
int SteamPlugin::GetAchievementIcon(const char *pchName)
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
		m_AchievementIconsMap.insert_or_assign(name, hImage);
	}
	return hImage;
}

bool SteamPlugin::GetAchievement(const char *pchName, bool *pbAchieved)
{
	CheckInitialized(SteamUserStats, false);
	return SteamUserStats()->GetAchievement(pchName, pbAchieved);
}

bool SteamPlugin::SetAchievement(const char *pchName)
{
	CheckInitialized(SteamUserStats, false);
	return SteamUserStats()->SetAchievement(pchName);
}

bool SteamPlugin::GetAchievementAndUnlockTime(const char *pchName, bool *pbAchieved, uint32 *punUnlockTime)
{
	CheckInitialized(SteamUserStats, false);
	return SteamUserStats()->GetAchievementAndUnlockTime(pchName, pbAchieved, punUnlockTime);
}

bool SteamPlugin::IndicateAchievementProgress(const char *pchName, uint32 nCurProgress, uint32 nMaxProgress)
{
	CheckInitialized(SteamUserStats, false);
	return SteamUserStats()->IndicateAchievementProgress(pchName, nCurProgress, nMaxProgress);
}

bool SteamPlugin::ClearAchievement(const char *pchName)
{
	CheckInitialized(SteamUserStats, false);
	return SteamUserStats()->ClearAchievement(pchName);
}

bool SteamPlugin::GetStat(const char *pchName, int32 *pData)
{
	CheckInitialized(SteamUserStats, false);
	return SteamUserStats()->GetStat(pchName, pData);
}

bool SteamPlugin::GetStat(const char *pchName, float *pData)
{
	CheckInitialized(SteamUserStats, false);
	return SteamUserStats()->GetStat(pchName, pData);
}

bool SteamPlugin::SetStat(const char *pchName, int32 nData)
{
	CheckInitialized(SteamUserStats, false);
	return SteamUserStats()->SetStat(pchName, nData);
}

bool SteamPlugin::SetStat(const char *pchName, float fData)
{
	CheckInitialized(SteamUserStats, false);
	return SteamUserStats()->SetStat(pchName, fData);
}

bool SteamPlugin::UpdateAvgRateStat(const char *pchName, float flCountThisSession, double dSessionLength)
{
	CheckInitialized(SteamUserStats, false);
	return SteamUserStats()->UpdateAvgRateStat(pchName, flCountThisSession, dSessionLength);
}

// Callback for FindLeaderboard.
void SteamPlugin::OnFindLeaderboard(LeaderboardFindResult_t *pCallback, bool bIOFailure)
{
	if (pCallback->m_bLeaderboardFound && !bIOFailure)
	{
		agk::Log("Callback: Find Leaderboard Succeeded");
		m_FindLeaderboardCallbackState = Done;
	}
	else
	{
		// Did not find the leaderboard.
		agk::Log("Callback: Find Leaderboard Failed");
		m_FindLeaderboardCallbackState = ServerError;
	}
	m_LeaderboardFindResult = *pCallback;
}

bool SteamPlugin::FindLeaderboard(const char *pchLeaderboardName)
{
	CheckInitialized(SteamUserStats, false);
	// Fail when the callback is currently running.
	if (m_FindLeaderboardCallbackState == Running)
	{
		return false;
	}
	try
	{
		// Clear result structure.
		m_LeaderboardFindResult.m_bLeaderboardFound = 0;
		m_LeaderboardFindResult.m_hSteamLeaderboard = 0;
		SteamAPICall_t hSteamAPICall = SteamUserStats()->FindLeaderboard(pchLeaderboardName);
		m_CallResultFindLeaderboard.Set(hSteamAPICall, this, &SteamPlugin::OnFindLeaderboard);
		m_FindLeaderboardCallbackState = Running;
		return true;
	}
	catch (...)
	{
		m_FindLeaderboardCallbackState = ClientError;
		return false;
	}
}

const char * SteamPlugin::GetLeaderboardName(SteamLeaderboard_t hLeaderboard)
{
	if (!m_StatsInitialized || !hLeaderboard)
	{
		return NULL;
	}
	return SteamUserStats()->GetLeaderboardName(hLeaderboard);
}

int SteamPlugin::GetLeaderboardEntryCount(SteamLeaderboard_t hLeaderboard)
{
	if (!m_StatsInitialized || !hLeaderboard)
	{
		return 0;
	}
	return SteamUserStats()->GetLeaderboardEntryCount(hLeaderboard);
}

ELeaderboardDisplayType SteamPlugin::GetLeaderboardDisplayType(SteamLeaderboard_t hLeaderboard)
{
	if (!m_StatsInitialized || !hLeaderboard)
	{
		return k_ELeaderboardDisplayTypeNone;
	}
	return SteamUserStats()->GetLeaderboardDisplayType(hLeaderboard);
}

ELeaderboardSortMethod SteamPlugin::GetLeaderboardSortMethod(SteamLeaderboard_t hLeaderboard)
{
	if (!m_StatsInitialized || !hLeaderboard)
	{
		return k_ELeaderboardSortMethodNone;
	}
	return SteamUserStats()->GetLeaderboardSortMethod(hLeaderboard);
}

// Callback for UploadLeaderboardScore.
void SteamPlugin::OnUploadScore(LeaderboardScoreUploaded_t *pCallback, bool bIOFailure)
{
	if (pCallback->m_bSuccess && !bIOFailure)
	{
		agk::Log("Callback: Upload Leaderboard Score Succeeded");
		m_UploadLeaderboardScoreCallbackState = Done;
	}
	else
	{
		agk::Log("Callback: Upload Leaderboard Score Failed");
		m_UploadLeaderboardScoreCallbackState = ServerError;
	}
	m_LeaderboardScoreUploaded = *pCallback;
	// Factor in bIOFailure.
	m_LeaderboardScoreUploaded.m_bSuccess = m_LeaderboardScoreUploaded.m_bSuccess && !bIOFailure;
}

bool SteamPlugin::UploadLeaderboardScore(SteamLeaderboard_t hLeaderboard, ELeaderboardUploadScoreMethod eLeaderboardUploadScoreMethod, int score)
{
	CheckInitialized(SteamUserStats, false);
	if (!hLeaderboard)
	{
		m_UploadLeaderboardScoreCallbackState = ClientError;
		return false;
	}
	// Fail when the callback is currently running.
	if (m_UploadLeaderboardScoreCallbackState == Running)
	{
		return false;
	}
	try
	{
		// Clear result structure.
		m_LeaderboardScoreUploaded.m_bScoreChanged = false;
		m_LeaderboardScoreUploaded.m_bSuccess = false;
		m_LeaderboardScoreUploaded.m_hSteamLeaderboard = 0;
		m_LeaderboardScoreUploaded.m_nGlobalRankNew = 0;
		m_LeaderboardScoreUploaded.m_nGlobalRankPrevious = 0;
		m_LeaderboardScoreUploaded.m_nScore = 0;
		// Call it.
		SteamAPICall_t hSteamAPICall = SteamUserStats()->UploadLeaderboardScore(hLeaderboard, eLeaderboardUploadScoreMethod, score, NULL, 0);
		m_CallResultUploadScore.Set(hSteamAPICall, this, &SteamPlugin::OnUploadScore);
		m_UploadLeaderboardScoreCallbackState = Running;
		return true;
	}
	catch (...)
	{
		m_UploadLeaderboardScoreCallbackState = ClientError;
		return false;
	}
}

// Callback for DownloadLeaderboardEntries.
void SteamPlugin::OnDownloadScore(LeaderboardScoresDownloaded_t *pCallback, bool bIOFailure)
{
	if (!bIOFailure)
	{
		agk::Log("Callback: Download Leaderboard Entries Succeeded");
		m_DownloadLeaderboardEntriesCallbackState = Done;
		m_DownloadedLeaderboardEntryCount = min(pCallback->m_cEntryCount, MAX_LEADERBOARD_ENTRIES);
		for (int index = 0; index < m_DownloadedLeaderboardEntryCount; index++)
		{
			SteamUserStats()->GetDownloadedLeaderboardEntry(pCallback->m_hSteamLeaderboardEntries, index, &m_DownloadedLeaderboardEntries[index], NULL, 0);
		}
	}
	else
	{
		agk::Log("Callback: Download Leaderboard Entries Failed");
		m_DownloadLeaderboardEntriesCallbackState = ServerError;
	}
}

bool SteamPlugin::DownloadLeaderboardEntries(SteamLeaderboard_t hLeaderboard, ELeaderboardDataRequest eLeaderboardDataRequest, int nRangeStart, int nRangeEnd)
{
	CheckInitialized(SteamUserStats, false);
	if (!hLeaderboard)
	{
		m_DownloadLeaderboardEntriesCallbackState = ClientError;
		return false;
	}
	// Fail when the callback is currently running.
	if (m_DownloadLeaderboardEntriesCallbackState == Running)
	{
		return false;
	}
	try
	{
		m_DownloadedLeaderboardEntryCount = 0;
		SteamAPICall_t hSteamAPICall = SteamUserStats()->DownloadLeaderboardEntries(hLeaderboard, eLeaderboardDataRequest, nRangeStart, nRangeEnd);
		m_CallResultDownloadScore.Set(hSteamAPICall, this, &SteamPlugin::OnDownloadScore);
		m_DownloadLeaderboardEntriesCallbackState = Running;
		return true;
	}
	catch (...)
	{
		m_DownloadLeaderboardEntriesCallbackState = ClientError;
		return false;
	}
}

int SteamPlugin::GetDownloadedLeaderboardEntryGlobalRank(int index)
{
	if (index < 0 || index >= m_DownloadedLeaderboardEntryCount)
	{
		agk::PluginError("GetDownloadedLeaderboardEntryGlobalRank index out of bounds.");
		return 0;
	}
	return m_DownloadedLeaderboardEntries[index].m_nGlobalRank;
}

int SteamPlugin::GetDownloadedLeaderboardEntryScore(int index)
{
	if (index < 0 || index >= m_DownloadedLeaderboardEntryCount)
	{
		agk::PluginError("GetDownloadedLeaderboardEntryScore index out of bounds.");
		return 0;
	}
	return m_DownloadedLeaderboardEntries[index].m_nScore;
}

CSteamID SteamPlugin::GetDownloadedLeaderboardEntryUser(int index)
{
	if (index < 0 || index >= m_DownloadedLeaderboardEntryCount)
	{
		agk::PluginError("GetDownloadedLeaderboardEntryAvatar index out of bounds.");
		return k_steamIDNil;
	}
	return m_DownloadedLeaderboardEntries[index].m_steamIDUser;
}

void SteamPlugin::AddRequestLobbyListDistanceFilter(ELobbyDistanceFilter eLobbyDistanceFilter)
{
	CheckInitialized(SteamMatchmaking, );
	SteamMatchmaking()->AddRequestLobbyListDistanceFilter(eLobbyDistanceFilter);
}

void SteamPlugin::AddRequestLobbyListFilterSlotsAvailable(int nSlotsAvailable)
{
	CheckInitialized(SteamMatchmaking, );
	SteamMatchmaking()->AddRequestLobbyListFilterSlotsAvailable(nSlotsAvailable);
}

void SteamPlugin::AddRequestLobbyListNearValueFilter(const char *pchKeyToMatch, int nValueToBeCloseTo)
{
	CheckInitialized(SteamMatchmaking, );
	SteamMatchmaking()->AddRequestLobbyListNearValueFilter(pchKeyToMatch, nValueToBeCloseTo);
}

void SteamPlugin::AddRequestLobbyListNumericalFilter(const char *pchKeyToMatch, int nValueToMatch, ELobbyComparison eComparisonType)
{
	CheckInitialized(SteamMatchmaking, );
	SteamMatchmaking()->AddRequestLobbyListNumericalFilter(pchKeyToMatch, nValueToMatch, eComparisonType);
}

void SteamPlugin::AddRequestLobbyListResultCountFilter(int cMaxResults)
{
	CheckInitialized(SteamMatchmaking, );
	SteamMatchmaking()->AddRequestLobbyListResultCountFilter(cMaxResults);
}

void SteamPlugin::AddRequestLobbyListStringFilter(const char *pchKeyToMatch, const char *pchValueToMatch, ELobbyComparison eComparisonType)
{
	CheckInitialized(SteamMatchmaking, );
	SteamMatchmaking()->AddRequestLobbyListStringFilter(pchKeyToMatch, pchValueToMatch, eComparisonType);
}

// Callback for RequestLobbyList.
void SteamPlugin::OnLobbyMatchList(LobbyMatchList_t *pLobbyMatchList, bool bIOFailure)
{
	if (bIOFailure)
	{
		agk::Log("Callback: Lobby List Failed");
		m_LobbyMatchListCallbackState = ServerError;
	}
	else
	{
		agk::Log("Callback: Lobby List Succeeded");
		m_LobbyMatchListCallbackState = Done;
		m_LobbyMatchList = *pLobbyMatchList;
	}
}

bool SteamPlugin::RequestLobbyList()
{
	// Clear result structure.
	m_LobbyMatchList.m_nLobbiesMatching = 0;
	CheckInitialized(SteamMatchmaking, false);
	try
	{
		SteamAPICall_t hSteamAPICall = SteamMatchmaking()->RequestLobbyList();
		m_CallResultLobbyMatchList.Set(hSteamAPICall, this, &SteamPlugin::OnLobbyMatchList);
		m_LobbyMatchListCallbackState = Running;
		return true;
	}
	catch (...)
	{
		m_LobbyMatchListCallbackState = ClientError;
		return false;
	}
}

CSteamID SteamPlugin::GetLobbyByIndex(int iLobby)
{
	CheckInitialized(SteamMatchmaking, k_steamIDNil);
	if (m_LobbyMatchList.m_nLobbiesMatching == 0 || iLobby >= (int)m_LobbyMatchList.m_nLobbiesMatching)
	{
		return k_steamIDNil;
	}
	return SteamMatchmaking()->GetLobbyByIndex(iLobby);
}

// Callback for CreateLobby.
void SteamPlugin::OnLobbyCreated(LobbyCreated_t *pParam, bool bIOFailure)
{
	if (m_LobbyCreateCallbackState == Done)
	{
		agk::PluginError("OnLobbyCreated called while in done state.");
	}
	if (bIOFailure)
	{
		agk::Log("Callback: Lobby Created Failed");
		m_LobbyCreateCallbackState = ServerError;
	}
	else
	{
		agk::Log("Callback: Lobby Created Succeeded");
		m_LobbyCreateCallbackState = Done;
		m_LobbyCreatedID = pParam->m_ulSteamIDLobby;
		m_LobbyCreatedResult = pParam->m_eResult;
	}
}

bool SteamPlugin::CreateLobby(ELobbyType eLobbyType, int cMaxMembers)
{
	m_LobbyEnterBlocked = false;
	m_LobbyEnterResponse = (EChatRoomEnterResponse)0;
	m_LobbyCreatedID = k_steamIDNil;
	m_LobbyCreatedResult = (EResult)0;
	CheckInitialized(SteamMatchmaking, false);
	try
	{
		SteamAPICall_t hSteamAPICall = SteamMatchmaking()->CreateLobby(eLobbyType, cMaxMembers);
		m_CallResultLobbyCreate.Set(hSteamAPICall, this, &SteamPlugin::OnLobbyCreated);
		m_LobbyCreateCallbackState = Running;
		return true;
	}
	catch (...)
	{
		m_LobbyCreateCallbackState = ClientError;
		return false;
	}
}

//bool SteamPlugin::SetLinkedLobby(CSteamID steamIDLobby, CSteamID steamIDLobbyDependent)
//{
//	CheckInitialized(SteamMatchmaking, false);
//	return SteamMatchmaking()->SetLinkedLobby(steamIDLobby, steamIDLobbyDependent);
//}

bool SteamPlugin::SetLobbyJoinable(CSteamID steamIDLobby, bool bLobbyJoinable)
{
	CheckInitialized(SteamMatchmaking, false);
	return SteamMatchmaking()->SetLobbyJoinable(steamIDLobby, bLobbyJoinable);
}

bool SteamPlugin::SetLobbyType(CSteamID steamIDLobby, ELobbyType eLobbyType)
{
	CheckInitialized(SteamMatchmaking, false);
	return SteamMatchmaking()->SetLobbyType(steamIDLobby, eLobbyType);
}

void SteamPlugin::OnLobbyEnter(LobbyEnter_t *pParam)
{
	OnLobbyEnter(pParam, false);
}

// Callback for JoinLobby.
void SteamPlugin::OnLobbyEnter(LobbyEnter_t *pParam, bool bIOFailure)
{
	if (bIOFailure)
	{
		agk::Log("Callback: Lobby Enter Failed");
		m_LobbyEnterCallbackState = ServerError;
	}
	else
	{
		agk::Log("Callback: Lobby Enter Succeeded");
		if (m_LobbyEnterCallbackState == Done)
		{
			if (m_LobbyEnterID == pParam->m_ulSteamIDLobby)
			{
				// Already notified of entering this lobby. Ignore.
				return;
			}
			agk::PluginError("OnLobbyEnter called while in done state.");
		}
		m_LobbyEnterCallbackState = Done;
		m_LobbyEnterID = pParam->m_ulSteamIDLobby;
		m_LobbyEnterBlocked = pParam->m_bLocked;
		m_LobbyEnterResponse = (EChatRoomEnterResponse)pParam->m_EChatRoomEnterResponse;
		if (!pParam->m_bLocked && pParam->m_EChatRoomEnterResponse == k_EChatRoomEnterResponseSuccess)
		{
			// Add to list of joined lobbies.
			auto it = std::find(m_JoinedLobbies.begin(), m_JoinedLobbies.end(), pParam->m_ulSteamIDLobby);
			if (it != m_JoinedLobbies.end())
			{
				m_JoinedLobbies.push_back(pParam->m_ulSteamIDLobby);
			}
		}
	}
}

bool SteamPlugin::JoinLobby(CSteamID steamIDLobby)
{
	m_LobbyEnterBlocked = false;
	m_LobbyEnterResponse = (EChatRoomEnterResponse)0;
	CheckInitialized(SteamMatchmaking, false);
	try
	{
		SteamAPICall_t hSteamAPICall = SteamMatchmaking()->JoinLobby(steamIDLobby);
		m_CallResultLobbyEnter.Set(hSteamAPICall, this, &SteamPlugin::OnLobbyEnter);
		m_LobbyEnterCallbackState = Running;
		return true;
	}
	catch (...)
	{
		m_LobbyEnterCallbackState = ClientError;
		return false;
	}
}

bool SteamPlugin::InviteUserToLobby(CSteamID steamIDLobby, CSteamID steamIDInvitee)
{
	CheckInitialized(SteamMatchmaking, false);
	return SteamMatchmaking()->InviteUserToLobby(steamIDLobby, steamIDInvitee);
}

void SteamPlugin::OnGameLobbyJoinRequested(GameLobbyJoinRequested_t *pParam)
{
	agk::Log("Callback: Lobby Join Requested");
	m_GameLobbyJoinRequestedInfo = *pParam;
}

CSteamID SteamPlugin::GetGameLobbyJoinRequestedLobby()
{
	CSteamID lobby = m_GameLobbyJoinRequestedInfo.m_steamIDLobby;
	m_GameLobbyJoinRequestedInfo.m_steamIDLobby = k_steamIDNil;
	m_GameLobbyJoinRequestedInfo.m_steamIDFriend = k_steamIDNil;
	return lobby;
}

void SteamPlugin::LeaveLobby(CSteamID steamIDLobby)
{
	CheckInitialized(SteamMatchmaking, );
	SteamMatchmaking()->LeaveLobby(steamIDLobby);
	// Remove from list of joined lobbies.
	auto it = std::find(m_JoinedLobbies.begin(), m_JoinedLobbies.end(), steamIDLobby);
	if (it != m_JoinedLobbies.end())
	{
		m_JoinedLobbies.erase(it);
	}
}

bool SteamPlugin::GetLobbyGameServer(CSteamID steamIDLobby, uint32 *punGameServerIP, uint16 *punGameServerPort, CSteamID *psteamIDGameServer)
{
	CheckInitialized(SteamMatchmaking, false);
	return SteamMatchmaking()->GetLobbyGameServer(steamIDLobby, punGameServerIP, punGameServerPort, psteamIDGameServer);
}

void SteamPlugin::SetLobbyGameServer(CSteamID steamIDLobby, uint32 unGameServerIP, uint16 unGameServerPort, CSteamID steamIDGameServer) // Triggers a LobbyGameCreated_t callback.
{
	CheckInitialized(SteamMatchmaking, );
	SteamMatchmaking()->SetLobbyGameServer(steamIDLobby, unGameServerIP, unGameServerPort, steamIDGameServer);
}

LobbyGameCreated_t SteamPlugin::GetLobbyGameCreated()
{
	LobbyGameCreated_t temp = m_LobbyGameCreatedInfo;
	// Clear the stored value.
	m_LobbyGameCreatedInfo.m_ulSteamIDLobby = 0;
	m_LobbyGameCreatedInfo.m_ulSteamIDGameServer = 0;
	m_LobbyGameCreatedInfo.m_unIP = 0;
	m_LobbyGameCreatedInfo.m_usPort = 0;
	return temp;
}

void SteamPlugin::OnLobbyGameCreated(LobbyGameCreated_t *pParam)
{
	agk::Log("Callback: Lobby Game Created");
	m_LobbyGameCreatedInfo = *pParam;
}

const char *SteamPlugin::GetLobbyData(CSteamID steamIDLobby, const char *pchKey)
{
	CheckInitialized(SteamMatchmaking, false);
	return SteamMatchmaking()->GetLobbyData(steamIDLobby, pchKey);
}

int SteamPlugin::GetLobbyDataCount(CSteamID steamIDLobby)
{
	CheckInitialized(SteamMatchmaking, 0);
	return SteamMatchmaking()->GetLobbyDataCount(steamIDLobby);
}

bool SteamPlugin::GetLobbyDataByIndex(CSteamID steamIDLobby, int iLobbyData, char *pchKey, int cchKeyBufferSize, char *pchValue, int cchValueBufferSize)
{
	CheckInitialized(SteamMatchmaking, false);
	return SteamMatchmaking()->GetLobbyDataByIndex(steamIDLobby, iLobbyData, pchKey, cchKeyBufferSize, pchValue, cchValueBufferSize);
}

void SteamPlugin::SetLobbyData(CSteamID steamIDLobby, const char *pchKey, const char *pchValue)
{
	CheckInitialized(SteamMatchmaking, );
	SteamMatchmaking()->SetLobbyData(steamIDLobby, pchKey, pchValue);
}

bool SteamPlugin::DeleteLobbyData(CSteamID steamIDLobby, const char *pchKey)
{
	CheckInitialized(SteamMatchmaking, false);
	return SteamMatchmaking()->DeleteLobbyData(steamIDLobby, pchKey);
}

bool SteamPlugin::RequestLobbyData(CSteamID steamIDLobby)
{
	CheckInitialized(SteamMatchmaking, false);
	return SteamMatchmaking()->RequestLobbyData(steamIDLobby);
}

void SteamPlugin::OnLobbyDataUpdated(LobbyDataUpdate_t *pParam)
{
	lobbyDataUpdateMutex.lock();
	if (!pParam->m_bSuccess)
	{
		agk::Log("Callback: Lobby Data Update Failed");
		//m_LobbyDataUpdateCallbackState = ServerError;
		// Report nil steam ids for failure.
		LobbyDataUpdateInfo_t info;
		info.lobby = k_steamIDNil;
		info.changedID = k_steamIDNil;
		m_LobbyDataUpdated.push_back(info);
	}
	else
	{
		agk::Log("Callback: Lobby Data Update Succeeded");
		//m_LobbyDataUpdateCallbackState = Done;
		LobbyDataUpdateInfo_t info;
		info.lobby = pParam->m_ulSteamIDLobby;
		info.changedID = pParam->m_ulSteamIDMember;
		m_LobbyDataUpdated.push_back(info);
	}
	lobbyDataUpdateMutex.unlock();
}

bool SteamPlugin::HasLobbyDataUpdated()
{
	lobbyDataUpdateMutex.lock();
	if (m_LobbyDataUpdated.size() > 0)
	{
		LobbyDataUpdateInfo_t info = m_LobbyDataUpdated.front();
		m_LobbyDataUpdatedLobby = info.lobby;
		m_LobbyDataUpdatedID = info.changedID;
		m_LobbyDataUpdated.pop_front();
		lobbyDataUpdateMutex.unlock();
		return true;
	}
	m_LobbyDataUpdatedLobby = k_steamIDNil;
	m_LobbyDataUpdatedID = k_steamIDNil;
	lobbyDataUpdateMutex.unlock();
	return false;
}

const char *SteamPlugin::GetLobbyMemberData(CSteamID steamIDLobby, CSteamID steamIDUser, const char *pchKey)
{
	CheckInitialized(SteamMatchmaking, NULL);
	return SteamMatchmaking()->GetLobbyMemberData(steamIDLobby, steamIDUser, pchKey);
}

void SteamPlugin::SetLobbyMemberData(CSteamID steamIDLobby, const char *pchKey, const char *pchValue)
{
	CheckInitialized(SteamMatchmaking, );
	SteamMatchmaking()->SetLobbyMemberData(steamIDLobby, pchKey, pchValue);
}

CSteamID SteamPlugin::GetLobbyOwner(CSteamID steamIDLobby)
{
	CheckInitialized(SteamMatchmaking, k_steamIDNil);
	return SteamMatchmaking()->GetLobbyOwner(steamIDLobby);
}

bool SteamPlugin::SetLobbyOwner(CSteamID steamIDLobby, CSteamID steamIDNewOwner)
{
	CheckInitialized(SteamMatchmaking, false);
	return SteamMatchmaking()->SetLobbyOwner(steamIDLobby, steamIDNewOwner);
}

int SteamPlugin::GetLobbyMemberLimit(CSteamID steamIDLobby)
{
	CheckInitialized(SteamMatchmaking, 0);
	return SteamMatchmaking()->GetLobbyMemberLimit(steamIDLobby);
}

bool SteamPlugin::SetLobbyMemberLimit(CSteamID steamIDLobby, int cMaxMembers)
{
	CheckInitialized(SteamMatchmaking, false);
	return SteamMatchmaking()->SetLobbyMemberLimit(steamIDLobby, cMaxMembers);
}

int SteamPlugin::GetNumLobbyMembers(CSteamID steamIDLobby)
{
	CheckInitialized(SteamMatchmaking, 0);
	return SteamMatchmaking()->GetNumLobbyMembers(steamIDLobby);
}

CSteamID SteamPlugin::GetLobbyMemberByIndex(CSteamID steamIDLobby, int iMember)
{
	CheckInitialized(SteamMatchmaking, k_steamIDNil);
	return SteamMatchmaking()->GetLobbyMemberByIndex(steamIDLobby, iMember);
}

void SteamPlugin::OnLobbyChatUpdated(LobbyChatUpdate_t *pParam)
{
	agk::Log("Callback: Lobby Chat updated");
	ChatUpdateInfo_t info;
	info.userChanged = pParam->m_ulSteamIDUserChanged;
	info.userState = (EChatMemberStateChange) pParam->m_rgfChatMemberStateChange;
	info.userMakingChange = pParam->m_ulSteamIDMakingChange;
	m_ChatUpdates.push_back(info);
}

bool SteamPlugin::HasLobbyChatUpdate()
{
	if (m_ChatUpdates.size() > 0)
	{
		ChatUpdateInfo_t info = m_ChatUpdates.front();
		m_LobbyChatUpdateUserChanged = info.userChanged;
		m_LobbyChatUpdateUserState = info.userState;
		m_LobbyChatUpdateUserMakingChange = info.userMakingChange;
		m_ChatUpdates.pop_front();
		return true;
	}
	m_LobbyChatUpdateUserChanged = k_steamIDNil;
	m_LobbyChatUpdateUserState = (EChatMemberStateChange) 0;
	m_LobbyChatUpdateUserMakingChange = k_steamIDNil;
	return false;
}

void SteamPlugin::OnLobbyChatMessage(LobbyChatMsg_t *pParam)
{
	agk::Log("Callback: Lobby Chat Message");
	//m_LobbyChatMessageCallbackState = Done;
	ChatMessageInfo_t info;
	info.user = pParam->m_ulSteamIDUser;
	SteamMatchmaking()->GetLobbyChatEntry(pParam->m_ulSteamIDLobby, pParam->m_iChatID, NULL, info.text, 4096, NULL);
	m_ChatMessages.push_back(info);
}

bool SteamPlugin::HasLobbyChatMessage()
{
	if (m_ChatMessages.size() > 0)
	{
		ChatMessageInfo_t info = m_ChatMessages.front();
		m_LobbyChatMessageUser = info.user;
		strcpy_s(m_LobbyChatMessageText, strlen(info.text) + 1, info.text);
		m_ChatMessages.pop_front();
		return true;
	}
	m_LobbyChatMessageUser = k_steamIDNil;
	*m_LobbyChatMessageText = NULL;
	return false;
}

bool SteamPlugin::SendLobbyChatMessage(CSteamID steamIDLobby, const char *pvMsgBody)
{
	CheckInitialized(SteamMatchmaking, false);
	return SteamMatchmaking()->SendLobbyChatMsg(steamIDLobby, pvMsgBody, strlen(pvMsgBody) + 1);
}

// Lobby methods: Favorite games
int SteamPlugin::AddFavoriteGame(AppId_t nAppID, uint32 nIP, uint16 nConnPort, uint16 nQueryPort, uint32 unFlags, uint32 rTime32LastPlayedOnServer)
{
	CheckInitialized(SteamMatchmaking, 0);
	return SteamMatchmaking()->AddFavoriteGame(nAppID, nIP, nConnPort, nQueryPort, unFlags, rTime32LastPlayedOnServer);
}

int SteamPlugin::GetFavoriteGameCount()
{
	CheckInitialized(SteamMatchmaking, 0);
	return SteamMatchmaking()->GetFavoriteGameCount();
}

bool SteamPlugin::GetFavoriteGame(int iGame, AppId_t *pnAppID, uint32 *pnIP, uint16 *pnConnPort, uint16 *pnQueryPort, uint32 *punFlags, uint32 *pRTime32LastPlayedOnServer)
{
	CheckInitialized(SteamMatchmaking, false);
	return SteamMatchmaking()->GetFavoriteGame(iGame, pnAppID, pnIP, pnConnPort, pnQueryPort, punFlags, pRTime32LastPlayedOnServer);
}

bool SteamPlugin::RemoveFavoriteGame(AppId_t nAppID, uint32 nIP, uint16 nConnPort, uint16 nQueryPort, uint32 unFlags)
{
	CheckInitialized(SteamMatchmaking, false);
	return SteamMatchmaking()->RemoveFavoriteGame(nAppID, nIP, nConnPort, nQueryPort, unFlags);
}

//uint32 SteamPlugin::GetPublicIP()
//{
//	SteamGameServer_Init(0xc0a80068, 27015, 11000, 11001, eServerModeNoAuthentication, "1.0.0.0");
//	CheckInitialized(SteamGameServer, 0);
//	uint32 ip = SteamGameServer()->GetPublicIP();
//	SteamGameServer_Shutdown();
//	return ip;
//}

// Music methods
bool SteamPlugin::IsMusicEnabled()
{
	CheckInitialized(SteamMusic, false);
	return SteamMusic()->BIsEnabled();
}

bool SteamPlugin::IsMusicPlaying()
{
	CheckInitialized(SteamMusic, false);
	return SteamMusic()->BIsPlaying();
}

AudioPlayback_Status SteamPlugin::GetMusicPlaybackStatus()
{
	CheckInitialized(SteamMusic, AudioPlayback_Undefined);
	return SteamMusic()->GetPlaybackStatus();
}

float SteamPlugin::GetMusicVolume()
{
	CheckInitialized(SteamMusic, 0.0);
	return SteamMusic()->GetVolume();
}

void SteamPlugin::PauseMusic()
{
	CheckInitialized(SteamMusic, );
	SteamMusic()->Pause();
}

void SteamPlugin::PlayMusic()
{
	CheckInitialized(SteamMusic, );
	SteamMusic()->Play();
}

void SteamPlugin::PlayNextSong()
{
	CheckInitialized(SteamMusic, );
	SteamMusic()->PlayNext();
}

void SteamPlugin::PlayPreviousSong()
{
	CheckInitialized(SteamMusic, );
	SteamMusic()->PlayPrevious();
}

void SteamPlugin::SetMusicVolume(float flVolume)
{
	CheckInitialized(SteamMusic, );
	SteamMusic()->SetVolume(flVolume);
}

void SteamPlugin::OnPlaybackStatusHasChanged(PlaybackStatusHasChanged_t *pParam)
{
	agk::Log("Callback: Music Playback Status Changed");
	m_PlaybackStatusHasChanged = true;
}

bool SteamPlugin::HasMusicPlaybackStatusChanged()
{
	// Reports true only once per change.
	if (m_PlaybackStatusHasChanged)
	{
		m_PlaybackStatusHasChanged = false;
		return true;
	}
	return false;
}

void SteamPlugin::OnVolumeHasChanged(VolumeHasChanged_t *pParam)
{
	agk::Log("Callback: Music Volume Changed");
	m_VolumeHasChanged = true;
}

bool SteamPlugin::HasMusicVolumeChanged()
{
	// Reports true only once per change.
	bool temp = m_VolumeHasChanged;
	m_VolumeHasChanged = false;
	return temp;
}

// Utils
uint8 SteamPlugin::GetCurrentBatteryPower()
{
	CheckInitialized(SteamUtils, 0);
	return SteamUtils()->GetCurrentBatteryPower();
}

uint32 SteamPlugin::GetIPCCallCount()
{
	CheckInitialized(SteamUtils, 0);
	return SteamUtils()->GetIPCCallCount();
}

const char *SteamPlugin::GetIPCountry()
{
	CheckInitialized(SteamUtils, NULL);
	return SteamUtils()->GetIPCountry();
}

uint32 SteamPlugin::GetSecondsSinceAppActive()
{
	CheckInitialized(SteamUtils, 0);
	return SteamUtils()->GetSecondsSinceAppActive();
}

uint32 SteamPlugin::GetSecondsSinceComputerActive()
{
	CheckInitialized(SteamUtils, 0);
	return SteamUtils()->GetSecondsSinceComputerActive();
}

uint32 SteamPlugin::GetServerRealTime()
{
	CheckInitialized(SteamUtils, 0);
	return SteamUtils()->GetServerRealTime();
}

const char *SteamPlugin::GetSteamUILanguage()
{
	CheckInitialized(SteamUtils, NULL);
	return SteamUtils()->GetSteamUILanguage();
}

bool SteamPlugin::IsOverlayEnabled()
{
	CheckInitialized(SteamUtils, 0);
	return SteamUtils()->IsOverlayEnabled();
}

void SteamPlugin::SetOverlayNotificationInset(int nHorizontalInset, int nVerticalInset)
{
	CheckInitialized(SteamUtils, );
	SteamUtils()->SetOverlayNotificationInset(nHorizontalInset, nVerticalInset);
}

void SteamPlugin::SetOverlayNotificationPosition(ENotificationPosition eNotificationPosition)
{
	CheckInitialized(SteamUtils, );
	SteamUtils()->SetOverlayNotificationPosition(eNotificationPosition);
}

void SteamPlugin::OnIPCountryChanged(IPCountry_t *pParam)
{
	agk::Log("Callback: IP Country Changed");
	m_HasIPCountryChanged = true;
}

bool SteamPlugin::HasIPCountryChanged()
{
	if (m_HasIPCountryChanged)
	{
		m_HasIPCountryChanged = false;
		return true;
	}
	return false;
}

void SteamPlugin::OnLowBatteryPower(LowBatteryPower_t *pParam)
{
	agk::Log("Callback: Low Battery Power Warning");
	m_HasLowBatteryWarning = true;
	m_nMinutesBatteryLeft = pParam->m_nMinutesBatteryLeft;
}

bool SteamPlugin::HasLowBatteryWarning()
{
	if (m_HasLowBatteryWarning)
	{
		m_HasLowBatteryWarning = false;
		return true;
	}
	return false;
}

void SteamPlugin::OnSteamShutdown(SteamShutdown_t *pParam)
{
	agk::Log("Callback: Steam Shutdown");
	m_IsSteamShuttingDown = true;
}

bool SteamPlugin::IsSteamShuttingDown()
{
	if (m_IsSteamShuttingDown)
	{
		m_IsSteamShuttingDown = false;
		return true;
	}
	return m_IsSteamShuttingDown;
}

void SteamPlugin::SetWarningMessageHook()
{
	CheckInitialized(SteamUtils, );
	SteamUtils()->SetWarningMessageHook(&SteamAPIDebugTextHook);
}

// Big Picture Mode methods
bool SteamPlugin::IsSteamInBigPictureMode()
{
	CheckInitialized(SteamUtils, false);
	return SteamUtils()->IsSteamInBigPictureMode();

}
bool SteamPlugin::ShowGamepadTextInput(EGamepadTextInputMode eInputMode, EGamepadTextInputLineMode eLineInputMode, const char *pchDescription, uint32 unCharMax, const char *pchExistingText)
{
	CheckInitialized(SteamUtils, false);
	if (unCharMax >= sizeof(m_GamepadTextInputDismissedInfo.text))
	{
		agk::Log("ShowGamepadTextInput: Maximum text length exceeds plugin limit.");
		unCharMax = sizeof(m_GamepadTextInputDismissedInfo.text) - 1;
	}
	return SteamUtils()->ShowGamepadTextInput(eInputMode, eLineInputMode, pchDescription, unCharMax, pchExistingText);
}

bool SteamPlugin::HasGamepadTextInputDismissedInfo()
{
	return m_GamepadTextInputDismissedInfo.dismissed;
}

void SteamPlugin::GetGamepadTextInputDismissedInfo(bool *pbSubmitted, char *pchText)
{
	//*info = m_GamepadTextInputDismissedInfo;
	*pbSubmitted = m_GamepadTextInputDismissedInfo.submitted;
	strcpy_s(pchText, strnlen_s(m_GamepadTextInputDismissedInfo.text, MAX_GAMEPAD_TEXT_INPUT_LENGTH) + 1, m_GamepadTextInputDismissedInfo.text);
	m_GamepadTextInputDismissedInfo.dismissed = false;
}

void SteamPlugin::OnGamepadTextInputDismissed(GamepadTextInputDismissed_t *pCallback)
{
	agk::Log("Callback: Gamepad Text Input Dismissed");
	m_GamepadTextInputDismissedInfo.dismissed = true;
	m_GamepadTextInputDismissedInfo.submitted = pCallback->m_bSubmitted;
	*m_GamepadTextInputDismissedInfo.text = NULL;
	if (!pCallback->m_bSubmitted)
	{
		// User canceled.
		return;
	}
	uint32 length = SteamUtils()->GetEnteredGamepadTextLength(); 
	bool success = SteamUtils()->GetEnteredGamepadTextInput(m_GamepadTextInputDismissedInfo.text, length);
	if (!success) {
		// This should only ever happen if there's a length mismatch.
		agk::Log("GetEnteredGamepadTextInput failed.");
	} 
}

// VR methods
bool SteamPlugin::IsSteamRunningInVR()
{
	CheckInitialized(SteamUtils, false);
	return SteamUtils()->IsSteamRunningInVR();
}

void SteamPlugin::StartVRDashboard()
{
	CheckInitialized(SteamUtils, );
	SteamUtils()->StartVRDashboard();
}

void SteamPlugin::SetVRHeadsetStreamingEnabled(bool bEnabled)
{
	CheckInitialized(SteamUtils, );
	SteamUtils()->SetVRHeadsetStreamingEnabled(bEnabled);
}

bool SteamPlugin::IsVRHeadsetStreamingEnabled()
{
	CheckInitialized(SteamUtils, false);
	return SteamUtils()->IsVRHeadsetStreamingEnabled();
}

// Cloud methods: Information
bool SteamPlugin::IsCloudEnabledForAccount()
{
	CheckInitialized(SteamRemoteStorage, false);
	return SteamRemoteStorage()->IsCloudEnabledForAccount();
}

bool SteamPlugin::IsCloudEnabledForApp()
{
	CheckInitialized(SteamRemoteStorage, false);
	return SteamRemoteStorage()->IsCloudEnabledForApp();
}

void SteamPlugin::SetCloudEnabledForApp(bool bEnabled)
{
	CheckInitialized(SteamRemoteStorage, );
	SteamRemoteStorage()->SetCloudEnabledForApp(bEnabled);
}

// Cloud Methods: Files
bool SteamPlugin::FileDelete(const char *pchFile)
{
	CheckInitialized(SteamRemoteStorage, false);
	return SteamRemoteStorage()->FileDelete(pchFile);
}

bool SteamPlugin::FileExists(const char *pchFile)
{
	CheckInitialized(SteamRemoteStorage, false);
	return SteamRemoteStorage()->FileExists(pchFile);
}

bool SteamPlugin::FileForget(const char *pchFile)
{
	CheckInitialized(SteamRemoteStorage, false);
	return SteamRemoteStorage()->FileForget(pchFile);
}

bool SteamPlugin::FilePersisted(const char *pchFile)
{
	CheckInitialized(SteamRemoteStorage, false);
	return SteamRemoteStorage()->FilePersisted(pchFile);
}

int32 SteamPlugin::FileRead(const char *pchFile, void *pvData, int32 cubDataToRead)
{
	CheckInitialized(SteamRemoteStorage, 0);
	return SteamRemoteStorage()->FileRead(pchFile, pvData, cubDataToRead);
}

CFileReadAsyncItem *SteamPlugin::GetFileReadAsyncItem(std::string filename)
{
	auto search = m_FileReadAsyncItemMap.find(filename);
	if (search != m_FileReadAsyncItemMap.end())
	{
		return search->second;
	}
	return NULL;
}

bool SteamPlugin::FileReadAsync(const char *pchFile, uint32 nOffset, uint32 cubToRead)
{
	CheckInitialized(SteamRemoteStorage, false);
	std::string filename = pchFile;
	CFileReadAsyncItem *item = GetFileReadAsyncItem(filename);
	if (!item)
	{
		item = new CFileReadAsyncItem();
		m_FileReadAsyncItemMap.insert_or_assign(filename, item);
	}
	return item->Call(pchFile, nOffset, cubToRead);
}

ECallbackState SteamPlugin::GetFileReadAsyncCallbackState(const char *pchFile)
{
	std::string filename = pchFile;
	CFileReadAsyncItem *item = GetFileReadAsyncItem(filename);
	if (item)
	{
		return getCallbackState(&item->m_CallbackState);
	}
	return Idle;
}

EResult SteamPlugin::GetFileReadAsyncResult(const char *pchFile)
{
	std::string filename = pchFile;
	CFileReadAsyncItem *item = GetFileReadAsyncItem(filename);
	if (item)
	{
		return item->m_Result;
	}
	return (EResult)0;
}

int SteamPlugin::GetFileReadAsyncMemblock(const char *pchFile)
{
	std::string filename = pchFile;
	CFileReadAsyncItem *item = GetFileReadAsyncItem(filename);
	if (item)
	{
		return item->m_MemblockID;
	}
	return 0;
}

//SteamAPICall_t SteamPlugin::FileShare(const char *pchFile);

bool SteamPlugin::FileWrite(const char *pchFile, const void *pvData, int32 cubData)
{
	CheckInitialized(SteamRemoteStorage, false);
	return SteamRemoteStorage()->FileWrite(pchFile, pvData, cubData);
}

CFileWriteAsyncItem *SteamPlugin::GetFileWriteAsyncItem(std::string filename)
{
	auto search = m_FileWriteAsyncItemMap.find(filename);
	if (search != m_FileWriteAsyncItemMap.end())
	{
		return search->second;
	}
	return NULL;
}

bool SteamPlugin::FileWriteAsync(const char *pchFile, const void *pvData, uint32 cubData)
{
	CheckInitialized(SteamRemoteStorage, false);
	std::string filename = pchFile;
	CFileWriteAsyncItem *item = GetFileWriteAsyncItem(filename);
	if (!item)
	{
		item = new CFileWriteAsyncItem();
		m_FileWriteAsyncItemMap.insert_or_assign(filename, item);
	}
	return item->Call(pchFile, pvData, cubData);
}

ECallbackState SteamPlugin::GetFileWriteAsyncCallbackState(const char *pchFile)
{
	std::string filename = pchFile;
	CFileWriteAsyncItem *item = GetFileWriteAsyncItem(filename);
	if (item)
	{
		return getCallbackState(&item->m_CallbackState);
	}
	return Idle;
}

EResult SteamPlugin::GetFileWriteAsyncResult(const char *pchFile)
{
	std::string filename = pchFile;
	CFileWriteAsyncItem *item = GetFileWriteAsyncItem(filename);
	if (item)
	{
		return item->m_Result;
	}
	return (EResult)0;
}

//bool SteamPlugin::FileWriteStreamCancel(UGCFileWriteStreamHandle_t writeHandle);
//bool SteamPlugin::FileWriteStreamClose(UGCFileWriteStreamHandle_t writeHandle);
//UGCFileWriteStreamHandle_t SteamPlugin::FileWriteStreamOpen(const char *pchFile);
//bool SteamPlugin::FileWriteStreamWriteChunk(UGCFileWriteStreamHandle_t writeHandle, const void *pvData, int32 cubData);

int32 SteamPlugin::GetFileCount()
{
	CheckInitialized(SteamRemoteStorage, 0);
	return SteamRemoteStorage()->GetFileCount();
}

const char * SteamPlugin::GetFileNameAndSize(int iFile, int32 *pnFileSizeInBytes)
{
	CheckInitialized(SteamRemoteStorage, NULL);
	return SteamRemoteStorage()->GetFileNameAndSize(iFile, pnFileSizeInBytes);
}

int32 SteamPlugin::GetFileSize(const char *pchFile)
{
	CheckInitialized(SteamRemoteStorage, 0);
	return SteamRemoteStorage()->GetFileSize(pchFile);
}

int64 SteamPlugin::GetFileTimestamp(const char *pchFile)
{
	CheckInitialized(SteamRemoteStorage, 0);
	return SteamRemoteStorage()->GetFileTimestamp(pchFile);
}

bool SteamPlugin::GetQuota(uint64 *pnTotalBytes, uint64 *puAvailableBytes)
{
	CheckInitialized(SteamRemoteStorage, false);
	return SteamRemoteStorage()->GetQuota(pnTotalBytes, puAvailableBytes);
}

ERemoteStoragePlatform SteamPlugin::GetSyncPlatforms(const char *pchFile)
{
	CheckInitialized(SteamRemoteStorage, k_ERemoteStoragePlatformNone);
	return SteamRemoteStorage()->GetSyncPlatforms(pchFile);
}

bool SteamPlugin::SetSyncPlatforms(const char *pchFile, ERemoteStoragePlatform eRemoteStoragePlatform)
{
	CheckInitialized(SteamRemoteStorage, false);
	return SteamRemoteStorage()->SetSyncPlatforms(pchFile, eRemoteStoragePlatform);
}

// User-Generated Content
int32 SteamPlugin::GetCachedUGCCount()
{
	CheckInitialized(SteamRemoteStorage, 0);
	return SteamRemoteStorage()->GetCachedUGCCount();
}
//UGCHandle_t SteamPlugin::GetCachedUGCHandle(int32 iCachedContent);
//bool SteamPlugin::GetUGCDetails(UGCHandle_t hContent, AppId_t *pnAppID, char **ppchName, int32 *pnFileSizeInBytes, CSteamID *pSteamIDOwner);
//bool SteamPlugin::GetUGCDownloadProgress(UGCHandle_t hContent, int32 *pnBytesDownloaded, int32 *pnBytesExpected);
//SteamAPICall_t SteamPlugin::UGCDownload(UGCHandle_t hContent, uint32 unPriority);
//SteamAPICall_t SteamPlugin::UGCDownloadToLocation(UGCHandle_t hContent, const char *pchLocation, uint32 unPriority);
//int32 SteamPlugin::UGCRead(UGCHandle_t hContent, void *pvData, int32 cubDataToRead, uint32 cOffset, EUGCReadAction eAction);

