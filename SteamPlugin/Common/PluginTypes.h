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

#ifndef _PLUGINTYPES_H_
#define _PLUGINTYPES_H_
#pragma once

// Extra structs used when return JSON.

#include "steam_api.h"

struct LobbyGameServer_t
{
	uint32 m_unGameServerIP;		// IP of the game server (if any)
	uint16 m_unGameServerPort;		// The port of the game server (if any)
	CSteamID m_ulSteamIDGameServer;	// The game server.
};

struct DLCData_t
{
	AppId_t m_AppID;
	bool m_bAvailable;
	char m_chName[128];

	void Clear()
	{
		m_AppID = 0;
		m_bAvailable = 0;
		m_chName[0] = '\0';
	}
};

struct DownloadProgress_t
{
	AppId_t m_AppID;	// If 0, the DLC is not currently downloading.
	uint64 m_unBytesDownloaded;
	uint64 m_unBytesTotal;

	void Clear()
	{
		m_AppID = 0;
		m_unBytesDownloaded = 0;
		m_unBytesTotal = 0;
	}
};

struct FavoriteGameInfo_t
{
	AppId_t m_AppID;
	uint32 m_nIP;
	uint16 m_nConnPort;
	uint16 m_nQueryPort;
	uint32 m_unFlags;
	uint32 m_rTime32LastPlayedOnServer;
};

#define MAX_GAMEPAD_TEXT_INPUT_LENGTH	512

struct GamepadTextInputDismissedInfo_t
{
	bool dismissed;
	bool submitted;
	char text[MAX_GAMEPAD_TEXT_INPUT_LENGTH];
};

#endif // _PLUGINTYPES_H_
