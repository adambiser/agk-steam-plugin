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
#include "SteamPlugin.h"
#include <string>
#include "..\AGKLibraryCommands.h"

#pragma comment(lib, "steam_api.lib")

#define CheckInitialized(steamInterface, returnValue)				\
	if (!m_SteamInitialized || (NULL == steamInterface()))			\
	{																\
		agk::PluginError("Steam API has not been initialized.");	\
		return returnValue;											\
	}

SteamPlugin::SteamPlugin() :
	m_AppID(0),
	m_SteamInitialized(false),
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
	m_CallbackLobbyChatUpdated(this, &SteamPlugin::OnLobbyChatUpdated),
	m_LobbyChatUpdateUserChanged(k_steamIDNil),
	m_LobbyChatUpdateUserState((EChatMemberStateChange)0),
	m_LobbyChatUpdateUserMakingChange(k_steamIDNil),
	m_CallbackLobbyChatMessage(this, &SteamPlugin::OnLobbyChatMessage),
	m_LobbyChatMessageUser(k_steamIDNil)
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

void SteamPlugin::OnGameOverlayActivated(GameOverlayActivated_t *pParam)
{
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
			m_RequestStatsCallbackState = Done;
			m_StatsInitialized = true;
		}
		else
		{
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
void SteamPlugin::OnAchievementStored(UserAchievementStored_t *pCallback)
{
	if (pCallback->m_nGameID == m_AppID)
	{
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

const char *SteamPlugin::GetAchievementID(int index)
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
		m_FindLeaderboardCallbackState = Done;
	}
	else
	{
		// Did not find the leaderboard.
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
		m_UploadLeaderboardScoreCallbackState = Done;
	}
	else
	{
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
		m_DownloadLeaderboardEntriesCallbackState = Done;
		m_DownloadedLeaderboardEntryCount = min(pCallback->m_cEntryCount, MAX_LEADERBOARD_ENTRIES);
		for (int index = 0; index < m_DownloadedLeaderboardEntryCount; index++)
		{
			SteamUserStats()->GetDownloadedLeaderboardEntry(pCallback->m_hSteamLeaderboardEntries, index, &m_DownloadedLeaderboardEntries[index], NULL, 0);
		}
	}
	else
	{
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
		m_LobbyMatchListCallbackState = ServerError;
	}
	else
	{
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
		//m_LobbyDataUpdateCallbackState = ServerError;
		// Report nil steam ids for failure.
		LobbyDataUpdateInfo_t info;
		info.lobby = k_steamIDNil;
		info.changedID = k_steamIDNil;
		m_LobbyDataUpdated.push_back(info);
	}
	else
	{
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

// Callback for CreateLobby.
void SteamPlugin::OnLobbyCreated(LobbyCreated_t *pParam, bool bIOFailure)
{
	if (m_LobbyCreateCallbackState == Done)
	{
		agk::PluginError("OnLobbyCreated called while in done state.");
	}
	if (bIOFailure)
	{
		m_LobbyCreateCallbackState = ServerError;
	}
	else
	{
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

void SteamPlugin::OnLobbyEnter(LobbyEnter_t *pParam)
{
	OnLobbyEnter(pParam, false);
}

// Callback for JoinLobby.
void SteamPlugin::OnLobbyEnter(LobbyEnter_t *pParam, bool bIOFailure)
{
	if (bIOFailure)
	{
		m_LobbyEnterCallbackState = ServerError;
	}
	else
	{
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
		m_LobbyEnterResponse = (EChatRoomEnterResponse) pParam->m_EChatRoomEnterResponse;
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

CSteamID SteamPlugin::GetLobbyOwner(CSteamID steamIDLobby)
{
	CheckInitialized(SteamMatchmaking, k_steamIDNil);
	return SteamMatchmaking()->GetLobbyOwner(steamIDLobby);
}

int SteamPlugin::GetLobbyMemberLimit(CSteamID steamIDLobby)
{
	CheckInitialized(SteamMatchmaking, 0);
	return SteamMatchmaking()->GetLobbyMemberLimit(steamIDLobby);
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