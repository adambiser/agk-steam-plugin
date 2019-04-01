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

#include "CRoomManager.h"

/*
CRoomManager
*/
void CRoomManager::Add(CSteamID steamID)
{
	if (steamID == k_steamIDNil)
	{
		return;
	}
	m_JoinedMutex.lock();
	m_Joined.push_back(steamID);
	m_JoinedMutex.unlock();
	// Register callbacks when joining.
	RegisterCallbacks();
}

void CRoomManager::Remove(CSteamID steamID)
{
	m_JoinedMutex.lock();
	Leave(steamID);
	// Remove from list of joined lobbies.
	auto it = std::find(m_Joined.begin(), m_Joined.end(), steamID);
	if (it != m_Joined.end())
	{
		m_Joined.erase(it);
	}
	m_JoinedMutex.unlock();
	if (m_Joined.size() == 0)
	{
		// Unregister the callbacks when leaving the last.
		UnregisterCallbacks();
	}
}

void CRoomManager::Reset()
{
	m_JoinedMutex.lock();
	for (size_t index = 0; index < m_Joined.size(); index++)
	{
		Leave(m_Joined[index]);
	}
	m_Joined.clear();
	m_JoinedMutex.unlock();
}

/*
CLobbyManager
*/
CLobbyManager *LobbyManager()
{
	static CLobbyManager *manager = new CLobbyManager();
	return manager;
}

void CLobbyManager::Leave(CSteamID steamID)
{
	SteamMatchmaking()->LeaveLobby(steamID);
}

void CLobbyManager::RegisterCallbacks()
{
	agk::Log("Registering lobby callbacks");
	Callbacks()->LobbyChatMessage.Register();
	Callbacks()->LobbyChatUpdate.Register();
	Callbacks()->LobbyDataUpdate.Register();
	Callbacks()->LobbyGameCreated.Register();
}

void CLobbyManager::UnregisterCallbacks()
{
	agk::Log("Unregistering lobby callbacks");
	Callbacks()->LobbyChatMessage.Unregister();
	Callbacks()->LobbyChatUpdate.Unregister();
	Callbacks()->LobbyDataUpdate.Unregister();
	Callbacks()->LobbyGameCreated.Unregister();
}

/*
CClanChatManager
*/
CClanChatManager *ClanChatManager()
{
	static CClanChatManager *manager = new CClanChatManager();
	return manager;
}

void CClanChatManager::Leave(CSteamID steamID)
{
	SteamFriends()->LeaveClanChatRoom(steamID);
}

void CClanChatManager::RegisterCallbacks()
{
	agk::Log("Registering clan chat callbacks");
	Callbacks()->GameConnectedChatJoin.Register();
	Callbacks()->GameConnectedChatLeave.Register();
	Callbacks()->GameConnectedClanChatMsg.Register();
}

void CClanChatManager::UnregisterCallbacks()
{
	agk::Log("Unregistering clan chat callbacks");
	Callbacks()->GameConnectedChatJoin.Unregister();
	Callbacks()->GameConnectedChatLeave.Unregister();
	Callbacks()->GameConnectedClanChatMsg.Unregister();
}
