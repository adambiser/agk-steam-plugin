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
#define MAX_GAME_CONNECTED_CLAN_CHAT_LENGTH	8193

	struct GameConnectedClanChatMsg_t : ::GameConnectedClanChatMsg_t
	{
		// Should be big enough to hold 2048 UTF-8 characters. So 8192 bytes + 1 for '\0'.
		char m_Text[MAX_GAME_CONNECTED_CLAN_CHAT_LENGTH];
		EChatEntryType m_ChatEntryType;
		GameConnectedClanChatMsg_t() : m_Text{} {}
		GameConnectedClanChatMsg_t(::GameConnectedClanChatMsg_t from) : ::GameConnectedClanChatMsg_t(from), m_Text{} {}
	};

	struct GameConnectedFriendChatMsg_t : ::GameConnectedFriendChatMsg_t
	{
		GameConnectedFriendChatMsg_t() {}
		GameConnectedFriendChatMsg_t(::GameConnectedFriendChatMsg_t from) : ::GameConnectedFriendChatMsg_t(from) {}
	};

#define MAX_GAMEPAD_TEXT_INPUT_LENGTH	512

	struct GamepadTextInputDismissed_t : ::GamepadTextInputDismissed_t
	{
		char m_chSubmittedText[MAX_GAMEPAD_TEXT_INPUT_LENGTH];
		GamepadTextInputDismissed_t() : m_chSubmittedText{} {}
		GamepadTextInputDismissed_t(::GamepadTextInputDismissed_t from) : ::GamepadTextInputDismissed_t(from), m_chSubmittedText{} {}
	};

#define MAX_CHAT_MESSAGE_LENGTH			4096

	struct LobbyChatMsg_t : ::LobbyChatMsg_t
	{
		int m_MemblockID; // the message
		LobbyChatMsg_t() : m_MemblockID(0) {}
		LobbyChatMsg_t(::LobbyChatMsg_t from) : ::LobbyChatMsg_t(from), m_MemblockID(0) {}
	};
}
#endif // _PLUGINTYPES_H_
