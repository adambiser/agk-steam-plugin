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

#ifndef _CLOBBYCREATEDCALLRESULT_H_
#define _CLOBBYCREATEDCALLRESULT_H_
#pragma once

#include "CCallResultItem.h"
#include <vector>

class CLobbyCreatedCallResult : public CCallResultItem
{
public:
	CLobbyCreatedCallResult(ELobbyType eLobbyType, int cMaxMembers) :
		CCallResultItem(),
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
protected:
	void Call();
private:
	CCallResult<CLobbyCreatedCallResult, LobbyCreated_t> m_CallResult;
	void OnLobbyCreated(LobbyCreated_t *pParam, bool bIOFailure);
	ELobbyType m_eLobbyType;
	int m_cMaxMembers;
	LobbyCreated_t m_LobbyCreated;
};

#endif // _CLOBBYCREATEDCALLRESULT_H_
