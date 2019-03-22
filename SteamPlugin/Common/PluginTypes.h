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

// Extra structs used when returning JSON.
// Also includes modified versions of Steam structs.  the same name is used for clarity (hopefully).

#include "steam_api.h"

namespace plugin
{

	struct DLCData_t
	{
		AppId_t m_AppID;
		bool m_bAvailable;
		char m_chName[128];

		void Clear()
		{
			m_AppID = 0;
			m_bAvailable = 0;
			m_chName[0] = 0;
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

	//struct FavoriteGame_t
	//{
	//	AppId_t m_AppID;
	//	uint32 m_nIP;
	//	uint16 m_nConnPort;
	//	uint16 m_nQueryPort;
	//	uint32 m_unFlags;
	//	uint32 m_rTime32LastPlayedOnServer;
	//};

#define MAX_GAMEPAD_TEXT_INPUT_LENGTH	512

	struct GamepadTextInputDismissed_t
	{
		bool m_bSubmitted;
		char m_chSubmittedText[MAX_GAMEPAD_TEXT_INPUT_LENGTH];

		GamepadTextInputDismissed_t()
		{
			m_bSubmitted = 0;
			m_chSubmittedText[0] = 0;
		};
		GamepadTextInputDismissed_t(::GamepadTextInputDismissed_t value)
		{
			m_bSubmitted = value.m_bSubmitted;
			m_chSubmittedText[0] = 0;
		}
	};

#define MAX_CHAT_MESSAGE_LENGTH			4096

	struct LobbyChatMsg_t
	{
		CSteamID m_ulSteamIDLobby;		// the lobby id this is in
		CSteamID m_ulSteamIDUser;		// steamID of the user who has sent this message
		EChatEntryType m_eChatEntryType;			// type of message
		int m_MemblockID;// the message

		LobbyChatMsg_t()
		{
			m_ulSteamIDLobby = k_steamIDNil;
			m_ulSteamIDUser = k_steamIDNil;
			m_eChatEntryType = (EChatEntryType)0;
			m_MemblockID = 0;
		};
		LobbyChatMsg_t(::LobbyChatMsg_t value)
		{
			m_ulSteamIDLobby = value.m_ulSteamIDLobby;
			m_ulSteamIDUser = value.m_ulSteamIDUser;
			m_eChatEntryType = (EChatEntryType)value.m_eChatEntryType;
			m_MemblockID = 0;
		}
	};
}
#endif // _PLUGINTYPES_H_
