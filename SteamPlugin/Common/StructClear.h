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

#ifndef _STRUCTCLEAR_H_
#define _STRUCTCLEAR_H_
#pragma once

#include "PluginTypes.h"
#include "steam_api.h"

void Clear(CSteamID &value);
void Clear(DlcInstalled_t &value);
void Clear(FavoritesListChanged_t &value);
void Clear(GameConnectedChatJoin_t & value);
void Clear(GameConnectedChatLeave_t &value);
void Clear(plugin::GameConnectedClanChatMsg_t &value);
void Clear(plugin::GameConnectedFriendChatMsg_t &value);
void Clear(GameLobbyJoinRequested_t &value);
void Clear(plugin::GamepadTextInputDismissed_t &value);
void Clear(GameRichPresenceJoinRequested_t &value);
void Clear(GameServerChangeRequested_t &value);
void Clear(InputAnalogActionData_t &value);
void Clear(InputDigitalActionData_t &value);
void Clear(InputMotionData_t &value);
void Clear(JoinClanChatRoomCompletionResult_t &value);
//void Clear(plugin::LobbyChatMsg_t &value);
void Clear(LobbyChatUpdate_t &value);
void Clear(LobbyDataUpdate_t &value);
void Clear(LobbyEnter_t &value);
void Clear(LobbyGameCreated_t &value);
void Clear(PersonaStateChange_t &value);
void Clear(UserAchievementStored_t &value);
void Clear(UserStatsReceived_t &value);
void Clear(UserStatsStored_t &value);

#endif // _STRUCTCLEAR_H_
