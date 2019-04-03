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

#ifndef _STEAMMATCHMAKING_H_
#define _STEAMMATCHMAKING_H_
#pragma once

#include "CCallbacks.h"

#define MAX_CHAT_MESSAGE_LENGTH			4096

// FavoritesListAccountsUpdated_t - no information given about this.

class CFavoritesListChangedCallback : public ListCallbackBase<CFavoritesListChangedCallback, FavoritesListChanged_t>
{
public:
	void OnResponse(FavoritesListChanged_t *pParam)
	{
		agk::Log("Callback: OnFavoritesListChanged");
		StoreResponse(*pParam);
	}
	void Clear(FavoritesListChanged_t &value)
	{
		value.m_bAdd = false;
		value.m_nAppID = 0;
		value.m_nConnPort = 0;
		value.m_nFlags = 0;
		value.m_nIP = 0;
		value.m_nQueryPort = 0;
		value.m_unAccountId = 0;
	}
};

namespace wrapper
{
	struct LobbyChatMsg_t : ::LobbyChatMsg_t
	{
		int m_MemblockID; // the message
		LobbyChatMsg_t() : m_MemblockID(0) {}
		LobbyChatMsg_t(::LobbyChatMsg_t from) : ::LobbyChatMsg_t(from), m_MemblockID(0) {}
	};
}

// While in a lobby
class CLobbyChatMessageCallback : public ListCallbackBase<CLobbyChatMessageCallback, ::LobbyChatMsg_t, wrapper::LobbyChatMsg_t>
{
public:
	void OnResponse(::LobbyChatMsg_t *pParam)
	{
		agk::Log("Callback: OnLobbyChatMessage");
		wrapper::LobbyChatMsg_t info(*pParam);
		char data[MAX_CHAT_MESSAGE_LENGTH];
		int length = SteamMatchmaking()->GetLobbyChatEntry(info.m_ulSteamIDLobby, pParam->m_iChatID, NULL, data, MAX_CHAT_MESSAGE_LENGTH, NULL);
		data[length] = 0;
		if (length > 0)
		{
			info.m_MemblockID = agk::CreateMemblock(length);
			memcpy_s(agk::GetMemblockPtr(info.m_MemblockID), length, data, length);
		}
		StoreResponse(info);
	}
	void Clear(wrapper::LobbyChatMsg_t &value)
	{
		value.m_eChatEntryType = 0;
		value.m_iChatID = 0;
		value.m_ulSteamIDLobby = 0;
		value.m_ulSteamIDUser = 0;
		if (value.m_MemblockID && agk::GetMemblockExists(value.m_MemblockID))
		{
			agk::DeleteMemblock(value.m_MemblockID);
		}
		value.m_MemblockID = 0;
	}
};

// While in a lobby
class CLobbyChatUpdateCallback : public ListCallbackBase<CLobbyChatUpdateCallback, LobbyChatUpdate_t>
{
public:
	void OnResponse(LobbyChatUpdate_t *pParam)
	{
		agk::Log("Callback: OnLobbyChatUpdate");
		StoreResponse(*pParam);
	}
	void Clear(LobbyChatUpdate_t &value)
	{
		value.m_rgfChatMemberStateChange = 0;
		value.m_ulSteamIDLobby = 0;
		value.m_ulSteamIDMakingChange = 0;
		value.m_ulSteamIDUserChanged = 0;
	}
};

// LobbyCreated_t - Call result

// While in a lobby
class CLobbyDataUpdateCallback : public ListCallbackBase<CLobbyDataUpdateCallback, LobbyDataUpdate_t>
{
public:
	void OnResponse(LobbyDataUpdate_t *pParam)
	{
		utils::Log("Callback OnLobbyDataUpdate.  Success: " + std::to_string(pParam->m_bSuccess));
		StoreResponse(*pParam);
	}
	void Clear(LobbyDataUpdate_t &value)
	{
		value.m_bSuccess = 0;
		value.m_ulSteamIDLobby = 0;
		value.m_ulSteamIDMember = 0;
	}
};

// CreateLobby, JoinLobby, also a call result
class CLobbyEnterCallback : public ListCallbackBase<CLobbyEnterCallback, LobbyEnter_t>
{
public:
	void OnResponse(LobbyEnter_t *pParam)
	{
		agk::Log("LobbyEnterCallback Response");
		this->StoreResponse(*pParam);
		LobbyManager()->Add(pParam->m_ulSteamIDLobby);
	}
	void Clear(LobbyEnter_t &value)
	{
		value.m_bLocked = false;
		value.m_EChatRoomEnterResponse = (EChatRoomEnterResponse)0;
		value.m_rgfChatPermissions = 0;
		value.m_ulSteamIDLobby = 0;
	}
};

// While in a lobby
class CLobbyGameCreatedCallback : public ListCallbackBase<CLobbyGameCreatedCallback, LobbyGameCreated_t>
{
public:
	void OnResponse(LobbyGameCreated_t *pParam)
	{
		agk::Log("Callback: OnLobbyGameCreated");
		StoreResponse(*pParam);
	}
	void Clear(LobbyGameCreated_t &value)
	{
		value.m_ulSteamIDGameServer = 0;
		value.m_ulSteamIDLobby = 0;
		value.m_unIP = 0;
		value.m_usPort = 0;
	}
};

//TODO? LobbyInvite_t - Someone has invited you to join a Lobby. Normally you don't need to do anything with this...

// LobbyKicked_t - Currently unused!
// LobbyMatchList_t - Call result.
// PSNGameBootInviteResult_t - deprecated

#endif // _STEAMMATCHMAKING_H_
