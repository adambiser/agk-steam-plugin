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

class CLobbyCreatedCallResult : public CCallResultItemBase
{
public:
	CLobbyCreatedCallResult(ELobbyType eLobbyType, int cMaxMembers) :
		CCallResultItemBase(),
		m_eLobbyType(eLobbyType),
		m_cMaxMembers(cMaxMembers)
	{
		m_CallResultName = "CreateLobby("
			+ std::to_string(m_eLobbyType) + ", "
			+ std::to_string(m_cMaxMembers) + ")";
		m_LobbyCreated.m_eResult = (EResult)0;
		m_LobbyCreated.m_ulSteamIDLobby = 0;
	}
	virtual ~CLobbyCreatedCallResult(void)
	{
		m_CallResult.Cancel();
	}
	uint64 GetLobbyCreatedHandle()
	{
		return m_LobbyCreated.m_ulSteamIDLobby;
	}
protected:
	void Call();
private:
	CCallResult<CLobbyCreatedCallResult, LobbyCreated_t> m_CallResult;
	void OnLobbyCreated(LobbyCreated_t *pParam, bool bIOFailure);
	ELobbyType m_eLobbyType;
	int m_cMaxMembers;
	LobbyCreated_t m_LobbyCreated;
};

class CLobbyEnterCallResult : public CCallResultItemBase
{
public:
	CLobbyEnterCallResult(CSteamID steamIDLobby) :
		CCallResultItemBase(),
		m_steamIDLobby(steamIDLobby)
	{
		m_CallResultName = "JoinLobby(" + std::to_string(m_steamIDLobby.ConvertToUint64()) + ")";
		m_LobbyEnter.m_bLocked = 0;
		m_LobbyEnter.m_EChatRoomEnterResponse = 0;
		m_LobbyEnter.m_rgfChatPermissions = 0; // unused - always 0.
		m_LobbyEnter.m_ulSteamIDLobby = 0;
	}
	virtual ~CLobbyEnterCallResult(void)
	{
		m_CallResult.Cancel();
	}
	uint64 GetLobbyEnterHandle()
	{
		return m_LobbyEnter.m_ulSteamIDLobby;
	}
	int GetLobbyEnterChatPermissions()
	{
		return (int)m_LobbyEnter.m_rgfChatPermissions;
	}
	int GetLobbyEnterLocked()
	{
		return (int)m_LobbyEnter.m_bLocked;
	}
	int GetLobbyEnterChatRoomEnterResponse()
	{
		return (int)m_LobbyEnter.m_EChatRoomEnterResponse;
	}
protected:
	void Call();
private:
	CCallResult<CLobbyEnterCallResult, LobbyEnter_t> m_CallResult;
	void OnLobbyEnter(LobbyEnter_t *pParam, bool bIOFailure);
	CSteamID m_steamIDLobby;
	LobbyEnter_t m_LobbyEnter;
};

class CLobbyMatchListCallResult : public CCallResultItemBase
{
public:
	CLobbyMatchListCallResult() : CCallResultItemBase()
	{
		m_CallResultName = "RequestLobbyList()";
	}
	virtual ~CLobbyMatchListCallResult(void)
	{
		m_CallResult.Cancel();
	}
	int GetLobbyCount()
	{
		return (int)m_Lobbies.size();
	}
	bool IsValidIndex(int index)
	{
		return index >= 0 && index < (int)m_Lobbies.size();
	}
	uint64 GetLobby(int index)
	{
		return m_Lobbies[index].ConvertToUint64();
	}
protected:
	void Call();
private:
	CCallResult<CLobbyMatchListCallResult, LobbyMatchList_t> m_CallResult;
	void OnLobbyMatchList(LobbyMatchList_t *pLobbyMatchList, bool bIOFailure);
	std::vector<CSteamID> m_Lobbies;
};

#endif // _STEAMMATCHMAKING_H_
