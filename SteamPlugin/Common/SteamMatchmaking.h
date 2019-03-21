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

namespace wrapper
{
	struct LobbyEnter_t : ::LobbyEnter_t
	{
		EResult m_eResult;

		LobbyEnter_t() : m_eResult((EResult)0) {}

		LobbyEnter_t(::LobbyEnter_t param) : LobbyEnter_t()
		{
			m_ulSteamIDLobby = param.m_ulSteamIDLobby;
			m_rgfChatPermissions = param.m_rgfChatPermissions;
			m_bLocked = param.m_bLocked;
			m_EChatRoomEnterResponse = param.m_EChatRoomEnterResponse;
			m_eResult = m_ulSteamIDLobby != 0 ? k_EResultOK : k_EResultFail;
		}
	};

	struct LobbyMatchList_t
	{
		EResult m_eResult;
		std::vector<CSteamID> m_Lobbies;

		LobbyMatchList_t() : m_eResult((EResult)0) {}

		LobbyMatchList_t(::LobbyMatchList_t param) : LobbyMatchList_t()
		{
			for (int index = 0; index < (int)param.m_nLobbiesMatching; index++)
			{
				m_Lobbies.push_back(SteamMatchmaking()->GetLobbyByIndex(index));
			}
			m_eResult = k_EResultOK;
		}
	};
}

class CLobbyCreatedCallResult : public CCallResultItem<LobbyCreated_t>
{
public:
	CLobbyCreatedCallResult(ELobbyType eLobbyType, int cMaxMembers) :
		m_eLobbyType(eLobbyType),
		m_cMaxMembers(cMaxMembers)
	{
		m_CallResultName = "CreateLobby("
			+ std::to_string(m_eLobbyType) + ", "
			+ std::to_string(m_cMaxMembers) + ")";
		m_Response.m_eResult = (EResult)0;
		m_Response.m_ulSteamIDLobby = 0;
	}
	uint64 GetLobbyCreatedHandle() { return m_Response.m_ulSteamIDLobby; }
protected:
	SteamAPICall_t Call()
	{
		return SteamMatchmaking()->CreateLobby(m_eLobbyType, m_cMaxMembers);
	}
private:
	ELobbyType m_eLobbyType;
	int m_cMaxMembers;
};

class CLobbyEnterCallResult : public CCallResultItem<LobbyEnter_t, wrapper::LobbyEnter_t>
{
public:
	CLobbyEnterCallResult(CSteamID steamIDLobby) :
		m_steamIDLobby(steamIDLobby)
	{
		m_CallResultName = "JoinLobby(" + std::to_string(m_steamIDLobby.ConvertToUint64()) + ")";
		m_Response.m_bLocked = 0;
		m_Response.m_EChatRoomEnterResponse = 0;
		m_Response.m_rgfChatPermissions = 0; // unused - always 0.
		m_Response.m_ulSteamIDLobby = 0;
	}
	uint64 GetLobbyEnterHandle() { return m_Response.m_ulSteamIDLobby; }
	int GetLobbyEnterChatPermissions() { return (int)m_Response.m_rgfChatPermissions; }
	int GetLobbyEnterLocked() { return (int)m_Response.m_bLocked; }
	int GetLobbyEnterChatRoomEnterResponse() { return (int)m_Response.m_EChatRoomEnterResponse; }
protected:
	SteamAPICall_t Call()
	{
		return SteamMatchmaking()->JoinLobby(m_steamIDLobby);
	}
private:
	CSteamID m_steamIDLobby;
};

class CLobbyMatchListCallResult : public CCallResultItem<LobbyMatchList_t, wrapper::LobbyMatchList_t>
{
public:
	CLobbyMatchListCallResult()
	{
		m_CallResultName = "RequestLobbyList()";
	}
	int GetLobbyCount() { return (int)m_Response.m_Lobbies.size(); }
	bool IsValidIndex(int index) { return index >= 0 && index < (int)m_Response.m_Lobbies.size(); }
	uint64 GetLobby(int index) { return m_Response.m_Lobbies[index].ConvertToUint64(); }
protected:
	SteamAPICall_t Call()
	{
		return SteamMatchmaking()->RequestLobbyList();
	}
};

#endif // _STEAMMATCHMAKING_H_
