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

#ifndef _CROOMMANAGER_H_
#define _CROOMMANAGER_H_
#pragma once

#include "DllMain.h"
#include <mutex>
#include <vector>

// Registers callbacks when joining a lobby or clan chat.  Unregisters when the last is left.
class CRoomManager
{
public:
	CRoomManager() {}
	virtual ~CRoomManager() {}
	void Add(CSteamID steamID);
	void Remove(CSteamID steamID);
	void Reset();
protected:
	virtual void Leave(CSteamID steamID) = 0;
	virtual void RegisterCallbacks() = 0;
	virtual void UnregisterCallbacks() = 0;
	std::mutex m_JoinedMutex;
	std::vector<CSteamID> m_Joined;
};

class CLobbyManager : public CRoomManager
{
protected:
	void Leave(CSteamID steamID);
	void RegisterCallbacks();
	void UnregisterCallbacks();
};

CLobbyManager *LobbyManager();

class CClanChatManager : public CRoomManager
{
protected:
	void Leave(CSteamID steamID);
	void RegisterCallbacks();
	void UnregisterCallbacks();
};

CClanChatManager *ClanChatManager();

#endif // _CROOMMANAGER_H_
