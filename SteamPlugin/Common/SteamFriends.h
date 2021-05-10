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

#ifndef _STEAMFRIENDS_H_
#define _STEAMFRIENDS_H_
#pragma once

#include "CCallbacks.h"

// GetFriendAvatar
class CAvatarImageLoadedCallback : public ListCallbackBase<CAvatarImageLoadedCallback, AvatarImageLoaded_t, CSteamID>
{
public:
	void OnResponse(AvatarImageLoaded_t *pParam)
	{
		agk::Log("Callback: OnAvatarImageLoaded");
		StoreResponse(pParam->m_steamID);
	}
	void Clear(CSteamID &value)
	{
		value = k_steamIDNil;
	}
};

extern CAvatarImageLoadedCallback AvatarImageLoadedCallback;

// ClanOfficerListResponse_t - call result for RequestClanOfficerList
// DownloadClanActivityCountsResult_t - call result for DownloadClanActivityCounts

class CFriendRichPresenceUpdateCallback : public ListCallbackBase<CFriendRichPresenceUpdateCallback, FriendRichPresenceUpdate_t, CSteamID>
{
public:
	void OnResponse(FriendRichPresenceUpdate_t *pParam)
	{
		utils::Log("Callback: OnFriendRichPresenceUpdate.  AppID = " + std::to_string(pParam->m_nAppID));
		StoreResponse(pParam->m_steamIDFriend);
	}
	void Clear(CSteamID &value)
	{
		value = k_steamIDNil;
	}
};

// FriendsEnumerateFollowingList_t - call result for EnumerateFollowingList
// FriendsGetFollowerCount_t - call result for GetFollowerCount
// FriendsIsFollowing_t - call result for IsFollowing

class CGameConnectedChatJoinCallback : public ListCallbackBase<CGameConnectedChatJoinCallback, GameConnectedChatJoin_t>
{
public:
	void OnResponse(GameConnectedChatJoin_t *pParam)
	{
		agk::Log("Callback: OnGameConnectedChatJoin");
		StoreResponse(*pParam);
	}
	void Clear(GameConnectedChatJoin_t &value)
	{
		value.m_steamIDClanChat = k_steamIDNil;
		value.m_steamIDUser = k_steamIDNil;
	}
};

class CGameConnectedChatLeaveCallback : public ListCallbackBase<CGameConnectedChatLeaveCallback, GameConnectedChatLeave_t>
{
public:
	void OnResponse(GameConnectedChatLeave_t *pParam)
	{
		agk::Log("Callback: OnGameConnectedChatLeave");
		StoreResponse(*pParam);
	}
	void Clear(GameConnectedChatLeave_t &value)
	{
		value.m_bDropped = false;
		value.m_bKicked = false;
		value.m_steamIDClanChat = k_steamIDNil;
		value.m_steamIDUser = k_steamIDNil;
	}
};

#define MAX_CLAN_CHAT_MESSAGE_LENGTH	8193

namespace wrapper
{
	struct GameConnectedClanChatMsg_t : ::GameConnectedClanChatMsg_t
	{
		// Should be big enough to hold 2048 UTF-8 characters. So 8192 bytes + 1 for '\0'.
		char m_Text[MAX_CLAN_CHAT_MESSAGE_LENGTH];
		EChatEntryType m_ChatEntryType;
		GameConnectedClanChatMsg_t() : m_Text{} {}
		GameConnectedClanChatMsg_t(::GameConnectedClanChatMsg_t from) : ::GameConnectedClanChatMsg_t(from), m_Text{} {}
	};
}
class CGameConnectedClanChatMsgCallback : public ListCallbackBase<CGameConnectedClanChatMsgCallback, GameConnectedClanChatMsg_t, wrapper::GameConnectedClanChatMsg_t>
{
public:
	void OnResponse(GameConnectedClanChatMsg_t *pParam)
	{
		agk::Log("Callback: OnGameConnectedClanChatMsg");
		wrapper::GameConnectedClanChatMsg_t msg(*pParam);
		int length = SteamFriends()->GetClanChatMessage(msg.m_steamIDClanChat, msg.m_iMessageID, msg.m_Text, MAX_CLAN_CHAT_MESSAGE_LENGTH, &msg.m_ChatEntryType, &msg.m_steamIDUser);
		msg.m_Text[length] = 0;
		if (msg.m_ChatEntryType == k_EChatEntryTypeInvalid)
		{
			agk::PluginError("OnGameConnectedClanChatMsg: GetClanChatMessage received an invalid entry type.");
			return;
		}
		StoreResponse(msg);
	}
	void Clear(wrapper::GameConnectedClanChatMsg_t &value)
	{
		value.m_ChatEntryType = (EChatEntryType)0;
		value.m_iMessageID = 0;
		value.m_steamIDClanChat = k_steamIDNil;
		value.m_steamIDUser = k_steamIDNil;
		value.m_Text[0] = 0;
	}
};

#define MAX_FRIEND_CHAT_MESSAGE_LENGTH	4096 // This is an arbitrary amount.  SDK does not specify a maximum.

namespace wrapper
{
	struct GameConnectedFriendChatMsg_t : ::GameConnectedFriendChatMsg_t
	{
		char m_Text[MAX_FRIEND_CHAT_MESSAGE_LENGTH];
		EChatEntryType m_ChatEntryType;
		GameConnectedFriendChatMsg_t() {}
		GameConnectedFriendChatMsg_t(::GameConnectedFriendChatMsg_t from) : ::GameConnectedFriendChatMsg_t(from) {}
	};
}

class CGameConnectedFriendChatMsgCallback : public ListCallbackBase<CGameConnectedFriendChatMsgCallback, GameConnectedFriendChatMsg_t, wrapper::GameConnectedFriendChatMsg_t>
{
public:
	void OnResponse(GameConnectedFriendChatMsg_t *pParam)
	{
		agk::Log("Callback: OnGameConnectedFriendChatMsg");
		wrapper::GameConnectedFriendChatMsg_t msg(*pParam);
		int length = SteamFriends()->GetFriendMessage(msg.m_steamIDUser, msg.m_iMessageID, msg.m_Text, MAX_FRIEND_CHAT_MESSAGE_LENGTH, &msg.m_ChatEntryType);
		msg.m_Text[length] = 0;
		if (msg.m_ChatEntryType == k_EChatEntryTypeInvalid)
		{
			agk::PluginError("OnGameConnectedFriendChatMsg: GetFriendMessage received an invalid entry type.");
			return;
		}
		StoreResponse(*pParam);
	}
	void Clear(wrapper::GameConnectedFriendChatMsg_t &value)
	{
		value.m_ChatEntryType = (EChatEntryType)0;
		value.m_iMessageID = 0;
		value.m_steamIDUser = k_steamIDNil;
		value.m_Text[0] = 0;
	}
};

class CGameLobbyJoinRequestedCallback : public ListCallbackBase<CGameLobbyJoinRequestedCallback, GameLobbyJoinRequested_t>
{
public:
	void OnResponse(GameLobbyJoinRequested_t *pParam)
	{
		agk::Log("Callback: OnGameLobbyJoinRequested");
		StoreResponse(*pParam);
	}
	void Clear(GameLobbyJoinRequested_t &value)
	{
		value.m_steamIDFriend = k_steamIDNil;
		value.m_steamIDLobby = k_steamIDNil;
	}
};

class CGameOverlayActivatedCallback : public BoolCallbackBase<CGameOverlayActivatedCallback, GameOverlayActivated_t>
{
public:
	void OnResponse(GameOverlayActivated_t *pParam)
	{
		utils::Log("Callback: OnGameOverlayActivated.  Active = " + std::to_string(pParam->m_bActive));
		//BoolCallbackBase::OnResponse(pParam);
		m_IsGameOverlayActive = pParam->m_bActive != 0;
	}
	// Clear should not change the value of m_IsGameOverlayActive.  That's given by the Steam client.
	bool IsGameOverlayActive() { return m_IsGameOverlayActive; }
private:
	bool m_IsGameOverlayActive;
};

class CGameRichPresenceJoinRequestedCallback : public ListCallbackBase<CGameRichPresenceJoinRequestedCallback, GameRichPresenceJoinRequested_t>
{
public:
	void OnResponse(GameRichPresenceJoinRequested_t *pParam)
	{
		agk::Log("Callback: OnGameRichPresenceJoinRequested");
		StoreResponse(*pParam);
	}
	void Clear(GameRichPresenceJoinRequested_t &value)
	{
		value.m_rgchConnect[0] = 0;
		value.m_steamIDFriend = k_steamIDNil;
	}
};

class CGameServerChangeRequestedCallback : public ListCallbackBase<CGameServerChangeRequestedCallback, GameServerChangeRequested_t>
{
public:
	void OnResponse(GameServerChangeRequested_t *pParam)
	{
		agk::Log("Callback: OnGameServerChangeRequested");
		StoreResponse(*pParam);
	}
	void Clear(GameServerChangeRequested_t &value)
	{
		value.m_rgchPassword[0] = 0;
		value.m_rgchServer[0] = 0;
	}
};

//JoinClanChatRoomCompletionResult_t - call result for JoinClanChatRoom

class COverlayBrowserProtocolNavigationCallBack : public ListCallbackBase<COverlayBrowserProtocolNavigationCallBack, OverlayBrowserProtocolNavigation_t>
{
public:
	void OnResponse(OverlayBrowserProtocolNavigation_t *pParam)
	{
		agk::Log("Callback: OverlayBrowserProtocolNavigation.");
		StoreResponse(*pParam);
	}
	void Clear(OverlayBrowserProtocolNavigation_t& value)
	{
		value.rgchURI[0] = 0;
	}
};
	

class CPersonaStateChangeCallback : public ListCallbackBase<CPersonaStateChangeCallback, PersonaStateChange_t>
{
public:
	void OnResponse(PersonaStateChange_t *pParam)
	{
		utils::Log("Callback: OnPersonaStateChange.  SteamID = " + std::to_string(pParam->m_ulSteamID) + ", Flags = " + std::to_string(pParam->m_nChangeFlags));
		StoreResponse(*pParam);
		if (pParam->m_nChangeFlags & k_EPersonaChangeAvatar)
		{
			// Allow HasAvatarImageLoaded to report avatar changes from here as well.
			if (AvatarImageLoadedCallback.IsRegistered())
			{
				AvatarImageLoadedCallback.StoreResponse(pParam->m_ulSteamID);
			}
		}
	}
	void Clear(PersonaStateChange_t &value)
	{
		value.m_nChangeFlags = 0;
		value.m_ulSteamID = 0;
	}
};

// SetPersonaNameResponse_t - call result for SetPersonaName

#endif // _STEAMFRIENDS_H_
