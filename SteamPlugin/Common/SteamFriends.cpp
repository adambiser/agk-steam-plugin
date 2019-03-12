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

#include "SteamPlugin.h"

// Overlay methods
void SteamPlugin::OnGameOverlayActivated(GameOverlayActivated_t *pParam)
{
	agk::Log("Callback: OnGameOverlayActivated");
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
void SteamPlugin::OnPersonaStateChange(PersonaStateChange_t *pParam)
{
	agk::Log("Callback: OnPersonaStateChange");
	if (m_PersonaStateChangedEnabled)
	{
		m_PersonaStateChangeMutex.lock();
		m_PersonaStateChangeList.push_back(*pParam);
		m_PersonaStateChangeMutex.unlock();
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

bool SteamPlugin::HasPersonaStateChange()
{
	m_PersonaStateChangeMutex.lock();
	if (m_PersonaStateChangeList.size() > 0)
	{
		m_CurrentPersonaStateChange = m_PersonaStateChangeList.front();
		m_PersonaStateChangeList.pop_front();
		m_PersonaStateChangeMutex.unlock();
		return true;
	}
	m_PersonaStateChangeMutex.unlock();
	ClearCurrentPersonaStateChange();
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
	agk::Log("Callback: OnAvatarImageLoaded");
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
