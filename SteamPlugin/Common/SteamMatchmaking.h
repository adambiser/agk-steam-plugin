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

#include "CCallResultItem.h"

class CLobbyCreatedCallResult : public CCallResultItem<LobbyCreated_t>
{
public:
	CLobbyCreatedCallResult(ELobbyType eLobbyType, int cMaxMembers)
	{
		m_CallResultName = "CreateLobby(" 
			+ std::to_string(eLobbyType) + ", " 
			+ std::to_string(cMaxMembers) + ")";
		m_hSteamAPICall = SteamMatchmaking()->CreateLobby(eLobbyType, cMaxMembers);
	}
	uint64 GetLobbyCreatedHandle() { return m_Response.m_ulSteamIDLobby; }
};

struct PluginLobbyEnter_t : LobbyEnter_t
{
	EResult m_eResult;

	PluginLobbyEnter_t() : m_eResult((EResult)0) {}

	PluginLobbyEnter_t(LobbyEnter_t param) : PluginLobbyEnter_t()
	{
		m_ulSteamIDLobby = param.m_ulSteamIDLobby;
		m_rgfChatPermissions = param.m_rgfChatPermissions;
		m_bLocked = param.m_bLocked;
		m_EChatRoomEnterResponse = param.m_EChatRoomEnterResponse;
		m_eResult = m_ulSteamIDLobby != 0 ? k_EResultOK : k_EResultFail;
	}
};

class CLobbyEnterCallResult : public CCallResultItem<LobbyEnter_t, PluginLobbyEnter_t>
{
public:
	CLobbyEnterCallResult(CSteamID steamIDLobby)
	{
		m_CallResultName = "JoinLobby(" + std::to_string(steamIDLobby.ConvertToUint64()) + ")";
		m_hSteamAPICall = SteamMatchmaking()->JoinLobby(steamIDLobby);
	}
	uint64 GetLobbyEnterHandle() { return m_Response.m_ulSteamIDLobby; }
	int GetLobbyEnterChatPermissions() { return (int)m_Response.m_rgfChatPermissions; }
	int GetLobbyEnterLocked() { return (int)m_Response.m_bLocked; }
	int GetLobbyEnterChatRoomEnterResponse() { return (int)m_Response.m_EChatRoomEnterResponse; }
};

struct PluginLobbyMatchList_t
{
	EResult m_eResult;
	std::vector<CSteamID> m_Lobbies;

	PluginLobbyMatchList_t() : m_eResult((EResult)0) {}

	PluginLobbyMatchList_t(LobbyMatchList_t param) : PluginLobbyMatchList_t()
	{
		for (int index = 0; index < (int)param.m_nLobbiesMatching; index++)
		{
			m_Lobbies.push_back(SteamMatchmaking()->GetLobbyByIndex(index));
		}
		m_eResult = k_EResultOK;
	}
};

class CLobbyMatchListCallResult : public CCallResultItem<LobbyMatchList_t, PluginLobbyMatchList_t>
{
public:
	CLobbyMatchListCallResult()
	{
		m_CallResultName = "RequestLobbyList()";
		m_hSteamAPICall = SteamMatchmaking()->RequestLobbyList();
	}
	int GetLobbyCount() { return (int)m_Response.m_Lobbies.size(); }
	bool IsValidIndex(int index) { return index >= 0 && index < (int)m_Response.m_Lobbies.size(); }
	uint64 GetLobby(int index) { return m_Response.m_Lobbies[index].ConvertToUint64(); }
};

#endif // _STEAMMATCHMAKING_H_
