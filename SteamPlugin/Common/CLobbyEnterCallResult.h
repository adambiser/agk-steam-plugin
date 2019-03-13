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

#ifndef _CLOBBYENTERCALLRESULT_H_
#define _CLOBBYENTERCALLRESULT_H_
#pragma once

#include "CCallResultItem.h"
#include <vector>

class CLobbyEnterCallResult : public CCallResultItem
{
public:
	CLobbyEnterCallResult(CSteamID steamIDLobby) :
		CCallResultItem(),
		m_steamIDLobby(steamIDLobby)
	{
		//m_LobbyEnter.m_bLocked = 0;
		//m_LobbyEnter.m_EChatRoomEnterResponse = 0;
		//m_LobbyEnter.m_rgfChatPermissions = 0; // unused - always 0.
		//m_LobbyEnter.m_ulSteamIDLobby = 0;
	}
	virtual ~CLobbyEnterCallResult(void)
	{
		m_CallResult.Cancel();
	}
	std::string GetName()
	{
		return "JoinLobby(" + std::to_string(m_steamIDLobby.ConvertToUint64()) + ")";
	}
protected:
	void Call();
private:
	CCallResult<CLobbyEnterCallResult, LobbyEnter_t> m_CallResult;
	void OnLobbyEnter(LobbyEnter_t *pParam, bool bIOFailure);
	CSteamID m_steamIDLobby;
	LobbyEnter_t m_LobbyEnter;
};

#endif // _CLOBBYENTERCALLRESULT_H_
