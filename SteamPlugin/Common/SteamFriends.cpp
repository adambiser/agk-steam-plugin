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

#include "Common.h"

void ActivateGameOverlay(const char *dialogName)
{
	CheckInitialized(NORETURN);
	SteamFriends()->ActivateGameOverlay(dialogName);
}

void ActivateGameOverlayInviteDialog(int hLobbySteamID)
{
	CheckInitialized(NORETURN);
	SteamFriends()->ActivateGameOverlayInviteDialog(SteamHandles()->GetSteamHandle(hLobbySteamID));
}

void ActivateGameOverlayToStore(int appID, int flag)
{
	CheckInitialized(NORETURN);
	SteamFriends()->ActivateGameOverlayToStore(appID, (EOverlayToStoreFlag)flag);
}

void ActivateGameOverlayToUser(const char *dialogName, int hSteamID)
{
	CheckInitialized(NORETURN);
	SteamFriends()->ActivateGameOverlayToUser(dialogName, SteamHandles()->GetSteamHandle(hSteamID));
}

void ActivateGameOverlayToWebPage(const char *url)
{
	CheckInitialized(NORETURN);
	SteamFriends()->ActivateGameOverlayToWebPage(url, k_EActivateGameOverlayToWebPageMode_Default);
}

void ActivateGameOverlayToWebPageModal(const char *url)
{
	CheckInitialized(NORETURN);
	SteamFriends()->ActivateGameOverlayToWebPage(url, k_EActivateGameOverlayToWebPageMode_Modal);
}

void ClearRichPresence()
{
	SteamFriends()->ClearRichPresence();
}

int CloseClanChatWindowInSteam(int hSteamIDClanChat)
{
	return SteamFriends()->CloseClanChatWindowInSteam(SteamHandles()->GetSteamHandle(hSteamIDClanChat));
}

#pragma region CDownloadClanActivityCountsResultCallResult
struct PluginDownloadClanActivityCountsResult_t : DownloadClanActivityCountsResult_t
{
	EResult m_eResult;

	PluginDownloadClanActivityCountsResult_t& operator=(const DownloadClanActivityCountsResult_t &from)
	{
		m_bSuccess = from.m_bSuccess;
		m_eResult = from.m_bSuccess ? k_EResultOK : k_EResultFail;
		return *this;
	}
};

class CDownloadClanActivityCountsResultCallResult : public CCallResultItem<DownloadClanActivityCountsResult_t, PluginDownloadClanActivityCountsResult_t>
{
public:
	CDownloadClanActivityCountsResultCallResult()
	{
		m_CallResultName = "DownloadClanActivityCounts(" + std::to_string(s_Clans.size()) + " clans)";
		if (s_Clans.size() == 0)
		{
			m_hSteamAPICall = k_uAPICallInvalid;
			agk::PluginError("DownloadClanActivityCounts: No clans to refresh.");
			return;
		}
		m_hSteamAPICall = SteamFriends()->DownloadClanActivityCounts(s_Clans.data(), (int)s_Clans.size());
		if (m_hSteamAPICall != k_uAPICallInvalid)
		{
			s_Clans.clear();
		}
	}
	static void AddClan(CSteamID steamIDClan)
	{
		s_Clans.push_back(steamIDClan);
	}
protected:
	static std::vector<CSteamID> s_Clans;
};
std::vector<CSteamID> CDownloadClanActivityCountsResultCallResult::s_Clans;
#pragma endregion

void AddDownloadClanActivityClan(int hSteamIDClan)
{
	CDownloadClanActivityCountsResultCallResult::AddClan(SteamHandles()->GetSteamHandle(hSteamIDClan));
}

int DownloadClanActivityCounts()
{
	CheckInitialized(0);
	return CallResults()->Add(new CDownloadClanActivityCountsResultCallResult());
}

#pragma region CFriendsEnumerateFollowingListCallResult
class CFriendsEnumerateFollowingListCallResult : public CCallResultItem<FriendsEnumerateFollowingList_t>
{
public:
	CFriendsEnumerateFollowingListCallResult(uint32 unStartIndex)
	{
		m_CallResultName = "EnumerateFollowingList(" + std::to_string(unStartIndex) + ")";
		m_hSteamAPICall = SteamFriends()->EnumerateFollowingList(unStartIndex);
	}
	bool IsValidIndex(int index) { return index >= 0 && index < m_Response.m_nResultsReturned; }
	CSteamID GetSteamID(int index) { return m_Response.m_rgSteamID[index]; }
	int GetResultsReturned() { return m_Response.m_nResultsReturned; }
	int GetTotalResultCount() { return m_Response.m_nTotalResultCount; }
};
#pragma endregion

int EnumerateFollowingList(int startIndex)
{
	CheckInitialized(0);
	return CallResults()->Add(new CFriendsEnumerateFollowingListCallResult(startIndex));
}

int GetEnumerateFollowingListResultsReturned(int hCallResult)
{
	return GetCallResultValue<CFriendsEnumerateFollowingListCallResult>(hCallResult, &CFriendsEnumerateFollowingListCallResult::GetResultsReturned);
}

int GetEnumerateFollowingListTotalResultCount(int hCallResult)
{
	return GetCallResultValue<CFriendsEnumerateFollowingListCallResult>(hCallResult, &CFriendsEnumerateFollowingListCallResult::GetTotalResultCount);
}

int GetEnumerateFollowingListSteamID(int hCallResult, int index)
{
	return GetCallResultValue<CFriendsEnumerateFollowingListCallResult>(hCallResult, index, &CFriendsEnumerateFollowingListCallResult::GetSteamID, __FUNCTION__);
}

//GetChatMemberByIndex - see GetChatMemberListJSON

int GetClanActivityCountsOnline(int hSteamIDClan)
{
	int nOnline;
	int nInGame;
	int nChatting;
	if (SteamFriends()->GetClanActivityCounts(SteamHandles()->GetSteamHandle(hSteamIDClan), &nOnline, &nInGame, &nChatting))
	{
		return nOnline;
	}
	return -1;
}

int GetClanActivityCountsInGame(int hSteamIDClan)
{
	int nOnline;
	int nInGame;
	int nChatting;
	if (SteamFriends()->GetClanActivityCounts(SteamHandles()->GetSteamHandle(hSteamIDClan), &nOnline, &nInGame, &nChatting))
	{
		return nInGame;
	}
	return -1;
}

int GetClanActivityCountsChatting(int hSteamIDClan)
{
	int nOnline;
	int nInGame;
	int nChatting;
	if (SteamFriends()->GetClanActivityCounts(SteamHandles()->GetSteamHandle(hSteamIDClan), &nOnline, &nInGame, &nChatting))
	{
		return nChatting;
	}
	return -1;
}

//GetClanByIndex - see GetClanListJSON

int GetClanChatMemberCount(int hSteamIDClan)
{
	CSteamID steamIDClan = SteamHandles()->GetSteamHandle(hSteamIDClan);
	return SteamFriends()->GetClanChatMemberCount(steamIDClan);
}

// This is an array of integers.  Speed is not important.  Returning JSON seems easiest.
char *GetClanChatMemberListJSON(int hSteamIDClan)
{
	std::string json("[");
	CSteamID steamIDClan = SteamHandles()->GetSteamHandle(hSteamIDClan);
	for (int index = 0; index < SteamFriends()->GetClanChatMemberCount(steamIDClan); index++)
	{
		if (index > 0)
		{
			json += ", ";
		}
		json += std::to_string(SteamHandles()->GetPluginHandle(SteamFriends()->GetChatMemberByIndex(steamIDClan, index)));
	}
	json += "]";
	return utils::CreateString(json);
}

//GetClanChatMessage - used in GameConnectedClanChatMsg_t 

int GetClanCount()
{
	return SteamFriends()->GetClanCount();
}

char *GetClanListJSON()
{
	std::string json("[");
	for (int index = 0; index < SteamFriends()->GetClanCount(); index++)
	{
		if (index > 0)
		{
			json += ", ";
		}
		json += std::to_string(SteamHandles()->GetPluginHandle(SteamFriends()->GetClanByIndex(index)));
	}
	json += "]";
	return utils::CreateString(json);
}

char *GetClanName(int hSteamIDClan)
{
	CheckInitialized(NULL_STRING);
	return utils::CreateString(SteamFriends()->GetClanName(SteamHandles()->GetSteamHandle(hSteamIDClan)));
}

//GetClanOfficerByIndex - see GetClanOfficerListJSON

int GetClanOfficerCount(int hSteamIDClan)
{
	return SteamFriends()->GetClanOfficerCount(SteamHandles()->GetSteamHandle(hSteamIDClan));
}

char *GetClanOfficerListJSON(int hSteamIDClan)
{
	std::string json("[");
	CSteamID steamIDClan = SteamHandles()->GetSteamHandle(hSteamIDClan);
	for (int index = 0; index < SteamFriends()->GetClanOfficerCount(steamIDClan); index++)
	{
		if (index > 0)
		{
			json += ", ";
		}
		json += std::to_string(SteamHandles()->GetPluginHandle(SteamFriends()->GetClanOfficerByIndex(steamIDClan, index)));
	}
	json += "]";
	return utils::CreateString(json);
}

int GetClanOwner(int hSteamIDClan)
{
	return SteamHandles()->GetPluginHandle(SteamFriends()->GetClanOwner(SteamHandles()->GetSteamHandle(hSteamIDClan)));
}

char *GetClanTag(int hSteamIDClan)
{
	return utils::CreateString(SteamFriends()->GetClanTag(SteamHandles()->GetSteamHandle(hSteamIDClan)));
}

int GetCoplayFriend(int index)
{
	return SteamHandles()->GetPluginHandle(SteamFriends()->GetCoplayFriend(index));
}

int GetCoplayFriendCount()
{
	return SteamFriends()->GetCoplayFriendCount();
}
//GetFollowerCount - FriendsGetFollowerCount_t
//GetFriendByIndex - see GetFriendListJSON
//GetFriendCoplayGame
//GetFriendCoplayTime

int GetFriendCount(int friendFlags)
{
	return SteamFriends()->GetFriendCount((EFriendFlags)friendFlags);
}

// This is an array of integers.  Speed is not important.  Returning JSON seems easiest.
char *GetFriendListJSON(int friendFlags)
{
	std::string json("[");
	for (int index = 0; index < SteamFriends()->GetFriendCount((EFriendFlags)friendFlags); index++)
	{
		if (index > 0)
		{
			json += ", ";
		}
		json += std::to_string(SteamHandles()->GetPluginHandle(SteamFriends()->GetFriendByIndex(index, (EFriendFlags)friendFlags)));
	}
	json += "]";
	return utils::CreateString(json);
}

//GetFriendCountFromSource
//GetFriendFromSourceByIndex
//GetFriendGamePlayed

int GetFriendGamePlayedGameAppID(int hUserSteamID)
{
	FriendGameInfo_t pFriendGameInfo;
	if (SteamFriends()->GetFriendGamePlayed(SteamHandles()->GetSteamHandle(hUserSteamID), &pFriendGameInfo))
	{
		return pFriendGameInfo.m_gameID.AppID();
	}
	return 0;
}

char *GetFriendGamePlayedIP(int hUserSteamID)
{
	FriendGameInfo_t pFriendGameInfo;
	if (SteamFriends()->GetFriendGamePlayed(SteamHandles()->GetSteamHandle(hUserSteamID), &pFriendGameInfo))
	{
		return utils::CreateString(utils::ToIPString(pFriendGameInfo.m_unGameIP));
	}
	return NULL_STRING;
}

int GetFriendGamePlayedConnectionPort(int hUserSteamID)
{
	FriendGameInfo_t pFriendGameInfo;
	if (SteamFriends()->GetFriendGamePlayed(SteamHandles()->GetSteamHandle(hUserSteamID), &pFriendGameInfo))
	{
		return pFriendGameInfo.m_usGamePort;
	}
	return 0;
}

int GetFriendGamePlayedQueryPort(int hUserSteamID)
{
	FriendGameInfo_t pFriendGameInfo;
	if (SteamFriends()->GetFriendGamePlayed(SteamHandles()->GetSteamHandle(hUserSteamID), &pFriendGameInfo))
	{
		return pFriendGameInfo.m_usQueryPort;
	}
	return 0;
}

int GetFriendGamePlayedLobby(int hUserSteamID)
{
	FriendGameInfo_t pFriendGameInfo;
	if (SteamFriends()->GetFriendGamePlayed(SteamHandles()->GetSteamHandle(hUserSteamID), &pFriendGameInfo))
	{
		return SteamHandles()->GetPluginHandle(pFriendGameInfo.m_steamIDLobby);
	}
	return 0;
}

//GetFriendMessage

char *GetFriendPersonaName(int hUserSteamID)
{
	CheckInitialized(NULL_STRING);
	Callbacks()->RegisterPersonaStateChangeCallback();
	return utils::CreateString(SteamFriends()->GetFriendPersonaName(SteamHandles()->GetSteamHandle(hUserSteamID)));
}

//GetFriendPersonaNameHistory

int GetFriendPersonaState(int hUserSteamID)
{
	CheckInitialized(-1);
	Callbacks()->RegisterPersonaStateChangeCallback();
	return SteamFriends()->GetFriendPersonaState(SteamHandles()->GetSteamHandle(hUserSteamID));
}

int GetFriendRelationship(int hUserSteamID)
{
	CheckInitialized(-1);
	return SteamFriends()->GetFriendRelationship(SteamHandles()->GetSteamHandle(hUserSteamID));
}

//GetFriendRichPresence
//GetFriendRichPresenceKeyByIndex
//GetFriendRichPresenceKeyCount

int GetFriendsGroupCount()
{
	CheckInitialized(0);
	return SteamFriends()->GetFriendsGroupCount();
}

int GetFriendsGroupIDByIndex(int index)
{
	CheckInitialized(k_FriendsGroupID_Invalid);
	return SteamFriends()->GetFriendsGroupIDByIndex(index);
}

int GetFriendsGroupMembersCount(int hFriendsGroupID)
{
	CheckInitialized(0);
	return SteamFriends()->GetFriendsGroupMembersCount(hFriendsGroupID);
}

// This is an array of integers.  Speed is not important.  Returning JSON seems easiest.
char *GetFriendsGroupMembersListJSON(int hFriendsGroupID)
{
	std::string json("[");
	if (IsSteamInitialized())
	{
		int count = SteamFriends()->GetFriendsGroupMembersCount(hFriendsGroupID);
		if (count > 0)
		{
			CSteamID *pSteamIDMembers = new CSteamID[count];
			SteamFriends()->GetFriendsGroupMembersList(hFriendsGroupID, pSteamIDMembers, count);
			for (int index = 0; index < count; index++)
			{
				if (index > 0)
				{
					json += ", ";
				}
				json += std::to_string(SteamHandles()->GetPluginHandle(pSteamIDMembers[index]));
			}
			delete[] pSteamIDMembers;
		}
	}
	json += "]";
	return utils::CreateString(json);
}

char *GetFriendsGroupName(int hFriendsGroupID)
{
	CheckInitialized(NULL_STRING);
	return utils::CreateString(SteamFriends()->GetFriendsGroupName(hFriendsGroupID));
}

/*
NOTE: Steam docs say
"If the Steam level is not immediately available for the specified user then this returns 0 and queues it to be downloaded from the Steam servers.
When it gets downloaded a PersonaStateChange_t callback will be posted with m_nChangeFlags including k_EPersonaChangeSteamLevel."

HOWEVER, this doesn't actually appear to be the case.  GetFriendSteamLevel returns 0, but the callback never seems to fire.
Possible solution is to just keep requesting the level when 0 is returned.
*/
int GetFriendSteamLevel(int hUserSteamID)
{
	CheckInitialized(0);
	Callbacks()->RegisterPersonaStateChangeCallback();
	return SteamFriends()->GetFriendSteamLevel(SteamHandles()->GetSteamHandle(hUserSteamID));
}

// NOTE: The Steam API appears to implicitly call RequestUserInformation when needed.
int GetFriendAvatar(int hUserSteamID, int size)
{
	CheckInitialized(0);
	Callbacks()->RegisterAvatarImageLoadedCallback();
	Callbacks()->RegisterPersonaStateChangeCallback();
	CSteamID steamID = SteamHandles()->GetSteamHandle(hUserSteamID);
	//SteamFriends()->RequestUserInformation(steamID, false);
	int hImage = 0;
	switch (size)
	{
	case AVATAR_SMALL:
		hImage = SteamFriends()->GetSmallFriendAvatar(steamID);
		break;
	case AVATAR_MEDIUM:
		hImage = SteamFriends()->GetMediumFriendAvatar(steamID);
		break;
	case AVATAR_LARGE:
		hImage = SteamFriends()->GetLargeFriendAvatar(steamID);
		break;
	default:
		agk::PluginError("Invalid avatar size.");
		return 0;
	}
	return hImage;
}

char *GetPersonaName()
{
	CheckInitialized(NULL_STRING);
	return utils::CreateString(SteamFriends()->GetPersonaName());
}

//GetPersonaState

char *GetPlayerNickname(int hUserSteamID)
{
	CheckInitialized(NULL_STRING);
	return utils::CreateString(SteamFriends()->GetPlayerNickname(SteamHandles()->GetSteamHandle(hUserSteamID)));
}

//GetUserRestrictions

int HasFriend(int hUserSteamID, int friendFlags)
{
	CheckInitialized(false);
	return SteamFriends()->HasFriend(SteamHandles()->GetSteamHandle(hUserSteamID), (EFriendFlags)friendFlags);
}

//InviteUserToGame - GameRichPresenceJoinRequested_t
//IsClanChatAdmin
//IsClanPublic
//IsClanOfficialGameGroup
//IsClanChatWindowOpenInSteam
//IsFollowing - FriendsIsFollowing_t
//IsUserInSource
//JoinClanChatRoom - JoinClanChatRoomCompletionResult_t (cr) GameConnectedChatJoin_t, GameConnectedClanChatMsg_t, GameConnectedChatLeave_t
//LeaveClanChatRoom - GameConnectedChatLeave_t
//OpenClanChatWindowInSteam
//ReplyToFriendMessage
//RequestClanOfficerList - ClanOfficerListResponse_t
//RequestFriendRichPresence - FriendRichPresenceUpdate_t

int RequestUserInformation(int hUserSteamID, int requireNameOnly)
{
	CheckInitialized(false);
	Callbacks()->RegisterPersonaStateChangeCallback();
	Callbacks()->RegisterAvatarImageLoadedCallback();
	return SteamFriends()->RequestUserInformation(SteamHandles()->GetSteamHandle(hUserSteamID), requireNameOnly != 0);
}

//SendClanChatMessage
//SetInGameVoiceSpeaking
//SetListenForFriendsMessages - GameConnectedFriendChatMsg_t
//SetPersonaName - SetPersonaNameResponse_t
//SetPlayedWith
//SetRichPresence

//Callbacks
int HasAvatarImageLoadedResponse()
{
	CheckInitialized(false);
	return Callbacks()->HasAvatarImageLoadedResponse();
}

int GetAvatarImageLoadedUser()
{
	CheckInitialized(0);
	return SteamHandles()->GetPluginHandle(Callbacks()->GetAvatarImageLoaded());
}

//ClanOfficerListResponse_t
//DownloadClanActivityCountsResult_t
//FriendRichPresenceUpdate_t
//FriendsEnumerateFollowingList_t
//FriendsGetFollowerCount_t
//FriendsIsFollowing_t
//GameConnectedChatJoin_t
//GameConnectedChatLeave_t
//GameConnectedClanChatMsg_t
//GameConnectedFriendChatMsg_t
//GameLobbyJoinRequested_t - always fires

int IsGameOverlayActive()
{
	CheckInitialized(false);
	return Callbacks()->IsGameOverlayActive();
}

//GameRichPresenceJoinRequested_t
//GameServerChangeRequested_t - always fires
//JoinClanChatRoomCompletionResult_t

int HasPersonaStateChangeResponse()
{
	CheckInitialized(false);
	return Callbacks()->HasPersonaStateChangeResponse();
}

int GetPersonaStateChangeFlags()
{
	CheckInitialized(0);
	return Callbacks()->GetPersonaStateChange().m_nChangeFlags;
}

int GetPersonaStateChangeSteamID()
{
	CheckInitialized(0);
	return SteamHandles()->GetPluginHandle(Callbacks()->GetPersonaStateChange().m_ulSteamID);
}

//SetPersonaNameResponse_t
