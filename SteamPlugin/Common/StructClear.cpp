#include "StructClear.h"
#include "../AGKLibraryCommands.h"

void Clear(CSteamID &value)
{
	value = k_steamIDNil;
}

void Clear(DlcInstalled_t &value)
{
	value.m_nAppID = 0;
}

void Clear(FavoritesListChanged_t &value)
{
	value.m_bAdd = 0;
	value.m_nAppID = 0;
	value.m_nConnPort = 0;
	value.m_nFlags = 0;
	value.m_nIP = 0;
	value.m_nQueryPort = 0;
	value.m_unAccountId = 0;
}

void Clear(GameLobbyJoinRequested_t &value)
{
	value.m_steamIDLobby = k_steamIDNil;
	value.m_steamIDFriend = k_steamIDNil;
}

void Clear(plugin::GamepadTextInputDismissed_t &value)
{
	value.m_bSubmitted = 0;
	value.m_chSubmittedText[0] = 0;
}

void Clear(InputAnalogActionData_t &value)
{
	value.bActive = false;
	value.eMode = k_EInputSourceMode_None;
	value.x = 0;
	value.y = 0;
}

void Clear(InputDigitalActionData_t &value)
{
	value.bActive = false;
	value.bState = false;
}

void Clear(InputMotionData_t &value)
{
	value.posAccelX = 0;
	value.posAccelY = 0;
	value.posAccelZ = 0;
	value.rotQuatW = 0;
	value.rotQuatX = 0;
	value.rotQuatY = 0;
	value.rotQuatZ = 0;
	value.rotVelX = 0;
	value.rotVelY = 0;
	value.rotVelZ = 0;
}

//void Clear(plugin::LobbyChatMsg_t &value)
//{
//	value.m_ulSteamIDLobby = k_steamIDNil;
//	value.m_ulSteamIDUser = k_steamIDNil;
//	value.m_eChatEntryType = (EChatEntryType)0;
//	if (value.m_MemblockID && agk::GetMemblockExists(value.m_MemblockID))
//	{
//		agk::DeleteMemblock(value.m_MemblockID);
//	}
//	value.m_MemblockID = 0;
//}

void Clear(LobbyChatUpdate_t &value)
{
	value.m_rgfChatMemberStateChange = (EChatMemberStateChange)0;
	value.m_ulSteamIDLobby = 0;
	value.m_ulSteamIDMakingChange = 0;
	value.m_ulSteamIDUserChanged = 0;
}

void Clear(LobbyDataUpdate_t &value)
{
	value.m_bSuccess = 0;
	value.m_ulSteamIDLobby = 0;
	value.m_ulSteamIDMember = 0;
}

void Clear(LobbyEnter_t &value)
{
	value.m_bLocked = 0;
	value.m_EChatRoomEnterResponse = 0;
	value.m_rgfChatPermissions = 0;
	value.m_ulSteamIDLobby = 0;
}

void Clear(LobbyGameCreated_t &value)
{
	value.m_ulSteamIDGameServer = 0;
	value.m_ulSteamIDLobby = 0;
	value.m_unIP = 0;
	value.m_usPort = 0;
}

void Clear(PersonaStateChange_t &value)
{
	value.m_ulSteamID = 0;
	value.m_nChangeFlags = 0;
}

void Clear(UserAchievementStored_t &value)
{
	value.m_bGroupAchievement = 0;
	value.m_nCurProgress = 0;
	value.m_nGameID = 0;
	value.m_nMaxProgress = 0;
	value.m_rgchAchievementName[0] = 0;
}

void Clear(UserStatsReceived_t &value)
{
	value.m_eResult = (EResult)0;
	value.m_nGameID = 0;
	value.m_steamIDUser = k_steamIDNil;
}

void Clear(UserStatsStored_t &value)
{
	value.m_eResult = (EResult)0;
	value.m_nGameID = 0;
}
