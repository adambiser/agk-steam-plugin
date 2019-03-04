#include "SteamPlugin.h"

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

void SteamPlugin::ActivateGameOverlayToWebPage(const char *pchURL, EActivateGameOverlayToWebPageMode eMode)
{
	CheckInitialized(SteamFriends, );
	SteamFriends()->ActivateGameOverlayToWebPage(pchURL, eMode);
}

const char *SteamPlugin::GetPersonaName()
{
	CheckInitialized(SteamFriends, NULL);
	return SteamFriends()->GetPersonaName();
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
