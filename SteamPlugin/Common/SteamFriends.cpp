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

#include "SteamFriends.h"
#include "DllMain.h"

CAvatarImageLoadedCallback AvatarImageLoadedCallback;
CFriendRichPresenceUpdateCallback FriendRichPresenceUpdateCallback;
CGameConnectedChatJoinCallback GameConnectedChatJoinCallback;
CGameConnectedChatLeaveCallback GameConnectedChatLeaveCallback;
CGameConnectedClanChatMsgCallback GameConnectedClanChatMsgCallback;
CGameConnectedFriendChatMsgCallback GameConnectedFriendChatMsgCallback;
CGameLobbyJoinRequestedCallback GameLobbyJoinRequestedCallback;
CGameOverlayActivatedCallback GameOverlayActivatedCallback;
CGameRichPresenceJoinRequestedCallback GameRichPresenceJoinRequestedCallback;
CGameServerChangeRequestedCallback GameServerChangeRequestedCallback;
CPersonaStateChangeCallback PersonaStateChangeCallback;

/* @page ISteamFriends */

/*
@desc Activates the Steam Overlay to a specific dialog.
@param dialogName The dialog to open.
Valid options are: "friends", "community", "players", "settings", "officialgamegroup", "stats", "achievements".
@url https://partner.steamgames.com/doc/api/ISteamFriends#ActivateGameOverlay
*/
extern "C" DLL_EXPORT void ActivateGameOverlay(const char *dialogName)
{
	CheckInitialized(NORETURN);
	SteamFriends()->ActivateGameOverlay(dialogName);
}

/*
@desc Activates the Steam Overlay to open the invite dialog. Invitations sent from this dialog will be for the provided lobby.
@param hLobbySteamID The Steam ID handle of the lobby that selected users will be invited to.
@url https://partner.steamgames.com/doc/api/ISteamFriends#ActivateGameOverlayInviteDialog
*/
extern "C" DLL_EXPORT void ActivateGameOverlayInviteDialog(int hLobbySteamID)
{
	CheckInitialized(NORETURN);
	SteamFriends()->ActivateGameOverlayInviteDialog(SteamHandles()->GetSteamHandle(hLobbySteamID));
}

/*
@desc Activates the Steam Overlay to the Steam store page for the provided app.
@param appID The app ID to show the store page of or 0 for the front page of the Steam store.
@param flag Flags to modify the behavior when the page opens.
@param-url flag https://partner.steamgames.com/doc/api/ISteamFriends#EOverlayToStoreFlag
@url https://partner.steamgames.com/doc/api/ISteamFriends#ActivateGameOverlayToStore
*/
extern "C" DLL_EXPORT void ActivateGameOverlayToStore(int appID, int flag)
{
	CheckInitialized(NORETURN);
	SteamFriends()->ActivateGameOverlayToStore(appID, (EOverlayToStoreFlag)flag);
}

/*
@desc Activates Steam Overlay to a specific dialog.
@param dialogName The dialog to open. Valid options are: "steamid", "chat", "jointrade", "stats", "achievements", "friendadd", "friendremove", "friendrequestaccept", "friendrequestignore".
@param hSteamID The Steam ID handle of the context to open this dialog to.
@url https://partner.steamgames.com/doc/api/ISteamFriends#ActivateGameOverlayToUser
*/
extern "C" DLL_EXPORT void ActivateGameOverlayToUser(const char *dialogName, int hSteamID)
{
	CheckInitialized(NORETURN);
	SteamFriends()->ActivateGameOverlayToUser(dialogName, SteamHandles()->GetSteamHandle(hSteamID));
}

/*
@desc Activates Steam Overlay web browser directly to the specified URL.
@param url The webpage to open. (A fully qualified address with the protocol is required.)
@url https://partner.steamgames.com/doc/api/ISteamFriends#ActivateGameOverlayToWebPage
*/
extern "C" DLL_EXPORT void ActivateGameOverlayToWebPage(const char *url)
{
	CheckInitialized(NORETURN);
	SteamFriends()->ActivateGameOverlayToWebPage(url, k_EActivateGameOverlayToWebPageMode_Default);
}

/*
@desc Activates Steam Overlay web browser directly to the specified URL.
The browser will be opened in a special overlay configuration which hides all other windows that the user has open in the overlay.
@param url The webpage to open. (A fully qualified address with the protocol is required.)
@url https://partner.steamgames.com/doc/api/ISteamFriends#ActivateGameOverlayToWebPage
*/
extern "C" DLL_EXPORT void ActivateGameOverlayToWebPageModal(const char *url)
{
	CheckInitialized(NORETURN);
	SteamFriends()->ActivateGameOverlayToWebPage(url, k_EActivateGameOverlayToWebPageMode_Modal);
}

/*
@desc Clears all of the current user's Rich Presence key/values.
@url https://partner.steamgames.com/doc/api/ISteamFriends#ClearRichPresence
*/
extern "C" DLL_EXPORT void ClearRichPresence()
{
	CheckInitialized(NORETURN);
	SteamFriends()->ClearRichPresence();
}

/*
@desc Closes the specified Steam group chat room in the Steam UI.
@param hSteamIDClanChat The Steam ID handlle of the Steam group chat room to close.
@return 1 if the user successfully left the Steam group chat room; 0 if the user is not in the provided Steam group chat room.
@url https://partner.steamgames.com/doc/api/ISteamFriends#CloseClanChatWindowInSteam
*/
extern "C" DLL_EXPORT int CloseClanChatWindowInSteam(int hSteamIDClanChat)
{
	CheckInitialized(0);
	return SteamFriends()->CloseClanChatWindowInSteam(SteamHandles()->GetSteamHandle(hSteamIDClanChat));
}

class CDownloadClanActivityCountsCallResult : public CCallResultItem<DownloadClanActivityCountsResult_t, ResponseWrapper<DownloadClanActivityCountsResult_t>>
{
public:
	CDownloadClanActivityCountsCallResult()
	{
		m_CallResultName = "DownloadClanActivityCounts(" + std::to_string(s_Clans.size()) + " clans)";
		if (s_Clans.size() == 0)
		{
			m_hSteamAPICall = k_uAPICallInvalid;
			agk::PluginError("DownloadClanActivityCounts: No clans to refresh.");
			return;
		}
		m_hSteamAPICall = SteamFriends()->DownloadClanActivityCounts(s_Clans.data(), (int)s_Clans.size());
		if (m_hSteamAPICall != k_uAPICallInvalid)
		{
			s_Clans.clear();
		}
	}
	static void AddClan(CSteamID steamIDClan)
	{
		s_Clans.push_back(steamIDClan);
	}
protected:
	static std::vector<CSteamID> s_Clans;
};
std::vector<CSteamID> CDownloadClanActivityCountsCallResult::s_Clans;

/*
@desc Adds a clan to be refreshed in the next DownloadClanActivityCounts call.
@param hSteamIDClan The clan's Steam ID handle.
@url https://partner.steamgames.com/doc/api/ISteamFriends#DownloadClanActivityCounts
*/
extern "C" DLL_EXPORT void AddDownloadClanActivityClan(int hSteamIDClan)
{
	CDownloadClanActivityCountsCallResult::AddClan(SteamHandles()->GetSteamHandle(hSteamIDClan));
}

/*
@desc Refresh the Steam Group activity data or get the data from groups other than one that the current user is a member.

Add clans to refresh using AddDownloadClanActivityClan.

Use GetCallResultCode to determine when this call succeeds and whether it was successful.
@callback-type callresult
@return A [call result handle](Callbacks-and-Call-Results#call-results) on success; otherwise 0.
@url https://partner.steamgames.com/doc/api/ISteamFriends#DownloadClanActivityCounts
@url https://partner.steamgames.com/doc/api/ISteamFriends#DownloadClanActivityCountsResult_t
*/
extern "C" DLL_EXPORT int DownloadClanActivityCounts()
{
	CheckInitialized(0);
	return CallResults()->Add(new CDownloadClanActivityCountsCallResult());
}

class CEnumerateFollowingListCallResult : public CCallResultItem<FriendsEnumerateFollowingList_t>
{
public:
	CEnumerateFollowingListCallResult(uint32 unStartIndex)
	{
		m_CallResultName = "EnumerateFollowingList(" + std::to_string(unStartIndex) + ")";
		m_hSteamAPICall = SteamFriends()->EnumerateFollowingList(unStartIndex);
	}
	bool IsValidIndex(int index) { return index >= 0 && index < m_Response.m_nResultsReturned; }
	CSteamID GetSteamID(int index) { return m_Response.m_rgSteamID[index]; }
	int GetResultsReturned() { return m_Response.m_nResultsReturned; }
	int GetTotalResultCount() { return m_Response.m_nTotalResultCount; }
};

/*
@desc Gets the list of users that the current user is following.
@param startIndex The index to start receiving followers from. This should be 0 on the initial call.
@callback-type callresult
@callback-getters GetEnumerateFollowingListResultsReturned, GetEnumerateFollowingListTotalResultCount, GetEnumerateFollowingListSteamID
@return A [call result handle](Callbacks-and-Call-Results#call-results) on success; otherwise 0.
@url https://partner.steamgames.com/doc/api/ISteamFriends#DownloadClanActivityCounts
@url https://partner.steamgames.com/doc/api/ISteamFriends#DownloadClanActivityCountsResult_t
*/
extern "C" DLL_EXPORT int EnumerateFollowingList(int startIndex)
{
	CheckInitialized(0);
	return CallResults()->Add(new CEnumerateFollowingListCallResult(startIndex));
}

/*
@desc Gets the number of results returned by the EnumerateFollowingList call result.
@param hCallResult An EnumerateFollowingList call result handle.
@return The number of results returned.
*/
extern "C" DLL_EXPORT int GetEnumerateFollowingListResultsReturned(int hCallResult)
{
	//return GetCallResultValue(hCallResult, &CEnumerateFollowingListCallResult::GetResultsReturned);
	return GetCallResultResponse<CEnumerateFollowingListCallResult>(hCallResult, &FriendsEnumerateFollowingList_t::m_nResultsReturned);
}

/*
@desc Gets the total number of people the user is following for the EnumerateFollowingList call result.

If this value is greater than the start index for the call + GetEnumerateFollowingListResultsReturned,
call EnumerateFollowingList with start index + GetEnumerateFollowingListResultsReturned as the new start index.
@param hCallResult An EnumerateFollowingList call result handle.
@return The total number of people we are following.
*/
extern "C" DLL_EXPORT int GetEnumerateFollowingListTotalResultCount(int hCallResult)
{
	return GetCallResultValue(hCallResult, &CEnumerateFollowingListCallResult::GetTotalResultCount);
}

/*
@desc Gets a user from the EnumerateFollowingList call result.
@param hCallResult An EnumerateFollowingList call result handle.
@param index The index of the followed user to retrieve.
@return A Steam ID handle.
*/
extern "C" DLL_EXPORT int GetEnumerateFollowingListSteamID(int hCallResult, int index)
{
	return GetCallResultValue(hCallResult, index, &CEnumerateFollowingListCallResult::GetSteamID, __FUNCTION__);
}

/*
@desc Gets the Steam ID at the given index in a Steam group chat.

_NOTE: You must call GetClanChatMemberCount before calling this._
@param hSteamIDClan This MUST be the same source used in the previous call to GetClanChatMemberCount!
@param index The index.
@return A Steam ID handle.  Invalid indices return 0.
@url https://partner.steamgames.com/doc/api/ISteamFriends#GetChatMemberByIndex
*/
extern "C" DLL_EXPORT int GetChatMemberByIndex(int hSteamIDClan, int index)
{
	CheckInitialized(0);
	return SteamHandles()->GetPluginHandle(SteamFriends()->GetChatMemberByIndex(SteamHandles()->GetSteamHandle(hSteamIDClan), index));
}

/*
@desc Returns the number of members that are online in a Steam group.
@param hSteamIDClan The Steam ID handle of the group to get the count for.
@return The number of members or -1 if there's a problem.
@url https://partner.steamgames.com/doc/api/ISteamFriends#GetClanActivityCounts
*/
extern "C" DLL_EXPORT int GetClanActivityCountsOnline(int hSteamIDClan)
{
	CheckInitialized(0);
	int nOnline;
	int nInGame;
	int nChatting;
	if (SteamFriends()->GetClanActivityCounts(SteamHandles()->GetSteamHandle(hSteamIDClan), &nOnline, &nInGame, &nChatting))
	{
		return nOnline;
	}
	return -1;
}

/*
@desc Returns the number of members that are in game in a Steam group.
@param hSteamIDClan The Steam ID handle of the group to get the count for.
@return The number of members or -1 if there's a problem.
@url https://partner.steamgames.com/doc/api/ISteamFriends#GetClanActivityCounts
*/
extern "C" DLL_EXPORT int GetClanActivityCountsInGame(int hSteamIDClan)
{
	CheckInitialized(0);
	int nOnline;
	int nInGame;
	int nChatting;
	if (SteamFriends()->GetClanActivityCounts(SteamHandles()->GetSteamHandle(hSteamIDClan), &nOnline, &nInGame, &nChatting))
	{
		return nInGame;
	}
	return -1;
}

/*
@desc Returns the number of members that are chatting in a Steam group.
@param hSteamIDClan The Steam ID handle of the group to get the count for.
@return The number of members or -1 if there's a problem.
@url https://partner.steamgames.com/doc/api/ISteamFriends#GetClanActivityCounts
*/
extern "C" DLL_EXPORT int GetClanActivityCountsChatting(int hSteamIDClan)
{
	CheckInitialized(0);
	int nOnline;
	int nInGame;
	int nChatting;
	if (SteamFriends()->GetClanActivityCounts(SteamHandles()->GetSteamHandle(hSteamIDClan), &nOnline, &nInGame, &nChatting))
	{
		return nChatting;
	}
	return -1;
}

/*
@desc Gets the Steam group's Steam ID at the given index.

_NOTE: You must call GetClanCount before calling this._
@param index The index.
@return A Steam ID handle.  Invalid indices return 0.
@url https://partner.steamgames.com/doc/api/ISteamFriends#GetClanByIndex
*/
extern "C" DLL_EXPORT int GetClanByIndex(int index)
{
	CheckInitialized(0);
	return SteamHandles()->GetPluginHandle(SteamFriends()->GetClanByIndex(index));
}

/*
@desc Gets the number of users in a Steam group chat.

NOTE: Large steam groups cannot be iterated by the local user.
@param hSteamIDClan The Steam ID handle of the group to get the user count of.
@return An integer.
@url https://partner.steamgames.com/doc/api/ISteamFriends#GetClanChatMemberCount
*/
extern "C" DLL_EXPORT int GetClanChatMemberCount(int hSteamIDClan)
{
	CheckInitialized(0);
	CSteamID steamIDClan = SteamHandles()->GetSteamHandle(hSteamIDClan);
	return SteamFriends()->GetClanChatMemberCount(steamIDClan);
}

/*
@desc Gets the users in a Steam group chat.

NOTE: Large steam groups cannot be iterated by the local user.
@param hSteamIDClan The Steam ID handle of the group to get the users of.
@return A JSON array of Steam ID handles.
@url https://partner.steamgames.com/doc/api/ISteamFriends#GetChatMemberByIndex
@url https://partner.steamgames.com/doc/api/ISteamFriends#GetClanChatMemberCount
*/
extern "C" DLL_EXPORT char *GetClanChatMemberListJSON(int hSteamIDClan)
{
	std::string json("[");
	if (g_SteamInitialized)
	{
		CSteamID steamIDClan = SteamHandles()->GetSteamHandle(hSteamIDClan);
		for (int index = 0; index < SteamFriends()->GetClanChatMemberCount(steamIDClan); index++)
		{
			if (index > 0)
			{
				json += ", ";
			}
			json += std::to_string(SteamHandles()->GetPluginHandle(SteamFriends()->GetChatMemberByIndex(steamIDClan, index)));
		}
	}
	json += "]";
	return utils::CreateString(json);
}

//GetClanChatMessage - used in GameConnectedClanChatMsg_t 

/*
@desc Gets the number of Steam groups that the current user is a member of.
@return An integer.
@url https://partner.steamgames.com/doc/api/ISteamFriends#GetClanCount
*/
extern "C" DLL_EXPORT int GetClanCount()
{
	CheckInitialized(0);
	return SteamFriends()->GetClanCount();
}

/*
@desc Gets A JSON array of Steam ID group handles that the current user is a member of.
@return A JSON array of Steam ID handles.
@url https://partner.steamgames.com/doc/api/ISteamFriends#GetClanCount
@url https://partner.steamgames.com/doc/api/ISteamFriends#GetClanByIndex
*/
extern "C" DLL_EXPORT char *GetClanListJSON()
{
	std::string json("[");
	if (g_SteamInitialized)
	{
		for (int index = 0; index < SteamFriends()->GetClanCount(); index++)
		{
			if (index > 0)
			{
				json += ", ";
			}
			json += std::to_string(SteamHandles()->GetPluginHandle(SteamFriends()->GetClanByIndex(index)));
		}
	}
	json += "]";
	return utils::CreateString(json);
}

/*
@desc Gets the display name for the specified Steam group; if the local client knows about it.
@param hSteamIDClan The Steam ID handle of the group to get the name for.
@return The display name for the group or an empty string.
@url https://partner.steamgames.com/doc/api/ISteamFriends#GetClanName
*/
extern "C" DLL_EXPORT char *GetClanName(int hSteamIDClan)
{
	CheckInitialized(NULL_STRING);
	return utils::CreateString(SteamFriends()->GetClanName(SteamHandles()->GetSteamHandle(hSteamIDClan)));
}

/*
@desc Gets the Steam ID handle of the officer at the given index in a Steam group.

_NOTE: You must call GetClanOfficerCount before calling this._
@param hSteamIDClan This must be the same steam group used in the previous call to GetClanOfficerCount!
@param index The index.
@return A Steam ID handle.  0 if the call is invalid.
@url https://partner.steamgames.com/doc/api/ISteamFriends#GetClanOfficerByIndex
@url https://partner.steamgames.com/doc/api/ISteamFriends#GetClanOfficerCount
*/
extern "C" DLL_EXPORT int GetClanOfficerByIndex(int hSteamIDClan, int index)
{
	CheckInitialized(0);
	return SteamHandles()->GetPluginHandle(SteamFriends()->GetClanOfficerByIndex(SteamHandles()->GetSteamHandle(hSteamIDClan), index));
}

/*
@desc Gets the number of officers (administrators and moderators) in a specified Steam group.

_NOTE: You must call RequestClanOfficerList before this to get the required data!_
@param hSteamIDClan The Steam ID handle of the group to get the officer count for.
@return An integer.
@url https://partner.steamgames.com/doc/api/ISteamFriends#GetClanOfficerCount
@url https://partner.steamgames.com/doc/api/ISteamFriends#RequestClanOfficerList
*/
extern "C" DLL_EXPORT int GetClanOfficerCount(int hSteamIDClan)
{
	CheckInitialized(0);
	return SteamFriends()->GetClanOfficerCount(SteamHandles()->GetSteamHandle(hSteamIDClan));
}

/*
@desc Gets the Steam ID handles of the officers (administrators and moderators) in a specified Steam group.

_NOTE: You must call RequestClanOfficerList before this to get the required data!_
@param hSteamIDClan The Steam ID handle of the group to get the officers for.
@return A JSON array of Steam ID handles.
@url https://partner.steamgames.com/doc/api/ISteamFriends#GetClanOfficerCount
@url https://partner.steamgames.com/doc/api/ISteamFriends#GetClanOfficerByIndex
@url https://partner.steamgames.com/doc/api/ISteamFriends#RequestClanOfficerList
*/
extern "C" DLL_EXPORT char *GetClanOfficerListJSON(int hSteamIDClan)
{
	std::string json("[");
	if (g_SteamInitialized)
	{
		CSteamID steamIDClan = SteamHandles()->GetSteamHandle(hSteamIDClan);
		for (int index = 0; index < SteamFriends()->GetClanOfficerCount(steamIDClan); index++)
		{
			if (index > 0)
			{
				json += ", ";
			}
			json += std::to_string(SteamHandles()->GetPluginHandle(SteamFriends()->GetClanOfficerByIndex(steamIDClan, index)));
		}
	}
	json += "]";
	return utils::CreateString(json);
}

/*
@desc Gets the owner of a Steam Group.

_NOTE: You must call RequestClanOfficerList before this to get the required data!_
@param hSteamIDClan The Steam ID handle of the group to get the owner for.
@return A Steam ID handle.
@url https://partner.steamgames.com/doc/api/ISteamFriends#GetClanOwner
@url https://partner.steamgames.com/doc/api/ISteamFriends#RequestClanOfficerList
*/
extern "C" DLL_EXPORT int GetClanOwner(int hSteamIDClan)
{
	CheckInitialized(0);
	return SteamHandles()->GetPluginHandle(SteamFriends()->GetClanOwner(SteamHandles()->GetSteamHandle(hSteamIDClan)));
}

/*
@desc Gets the unique tag (abbreviation) for the specified Steam group; If the local client knows about it.
@param hSteamIDClan The Steam ID handle of the group to get the tag for.
@return The tag or an empty string if the user doesn't know about the group.
@url https://partner.steamgames.com/doc/api/ISteamFriends#GetClanTag
*/
extern "C" DLL_EXPORT char *GetClanTag(int hSteamIDClan)
{
	CheckInitialized(NULL_STRING);
	return utils::CreateString(SteamFriends()->GetClanTag(SteamHandles()->GetSteamHandle(hSteamIDClan)));
}

/*
@desc Gets the Steam ID of the recently played with user at the given index.
@param index The index.
@return A Steam ID Handle.  Invalid indices return 0.
@url https://partner.steamgames.com/doc/api/ISteamFriends#GetCoplayFriend
*/
extern "C" DLL_EXPORT int GetCoplayFriend(int index)
{
	CheckInitialized(0);
	return SteamHandles()->GetPluginHandle(SteamFriends()->GetCoplayFriend(index));
}

/*
@desc Gets the number of players that the current users has recently played with, across all games.
@return The number of users that the current user has recently played with.
@url https://partner.steamgames.com/doc/api/ISteamFriends#GetCoplayFriendCount
*/
extern "C" DLL_EXPORT int GetCoplayFriendCount()
{
	CheckInitialized(0);
	return SteamFriends()->GetCoplayFriendCount();
}

/*
@desc Returns a JSON array of Steam ID handles of the players that the current users has recently played with, across all games.
@return A JSON array of Steam ID handles.
@url https://partner.steamgames.com/doc/api/ISteamFriends#GetCoplayFriendCount
*/
extern "C" DLL_EXPORT char *GetCoplayFriendListJSON()
{
	std::string json("[");
	if (g_SteamInitialized)
	{
		for (int index = 0; index < SteamFriends()->GetCoplayFriendCount(); index++)
		{
			if (index > 0)
			{
				json += ", ";
			}
			json += std::to_string(SteamHandles()->GetPluginHandle(SteamFriends()->GetCoplayFriend(index)));
		}
	}
	json += "]";
	return utils::CreateString(json);
}

class CGetFollowerCountCallResult : public CCallResultItem<FriendsGetFollowerCount_t>
{
public:
	CGetFollowerCountCallResult(CSteamID steamID)
	{
		m_CallResultName = "GetFollowerCount(" + std::to_string(steamID.ConvertToUint64()) + ")";
		m_hSteamAPICall = SteamFriends()->GetFollowerCount(steamID);
	}
	int GetCount() { return m_Response.m_nCount; }
	CSteamID GetSteamID() { return m_Response.m_steamID; }
};

/*
@desc Gets the number of users following the specified user.
Use GetFollowerCountValue to get the value once the call result completes.
@param hSteamID The user's Steam ID handle.
@callback-type callresult
@callback-getters GetFollowerCountValue, GetFollowerCountSteamID
@return A [call result handle](Callbacks-and-Call-Results#call-results) on success; otherwise 0.
@url https://partner.steamgames.com/doc/api/ISteamFriends#GetFollowerCount
@url https://partner.steamgames.com/doc/api/ISteamFriends#FriendsGetFollowerCount_t
*/
extern "C" DLL_EXPORT int GetFollowerCount(int hSteamID)
{
	CheckInitialized(0);
	return CallResults()->Add(new CGetFollowerCountCallResult(SteamHandles()->GetSteamHandle(hSteamID)));
}

/*
@desc Gets the number of users following the user for the GetFollowerCount call result.
@param hCallResult A GetFollowerCount call result handle.
@return An integer.
*/
extern "C" DLL_EXPORT int GetFollowerCountValue(int hCallResult)
{
	return GetCallResultValue(hCallResult, &CGetFollowerCountCallResult::GetCount);
}

/*
@desc Gets the user's Steam ID handle for the GetFollowerCount call result.
@param hCallResult A GetFollowerCount call result handle.
@return A Steam ID handle.
*/
extern "C" DLL_EXPORT int GetFollowerCountSteamID(int hCallResult)
{
	return GetCallResultValue(hCallResult, &CGetFollowerCountCallResult::GetSteamID);
}

/*
@desc Gets the Steam ID of the user at the given index.

_NOTE: You must call GetFriendCount before calling this._
@param index The index.
@param friendFlags A combined union (binary "or") of EFriendFlags. This must be the same value as used in the previous call to GetFriendCount.
@return A Steam ID handle.  Invalid indices return 0.
@url https://partner.steamgames.com/doc/api/ISteamFriends#GetFriendByIndex
*/
extern "C" DLL_EXPORT int GetFriendByIndex(int index, int friendFlags)
{
	CheckInitialized(0);
	return SteamHandles()->GetPluginHandle(SteamFriends()->GetFriendByIndex(index, (EFriendFlags)friendFlags));
}

/*
@desc Gets the app ID of the game that user played with someone on their recently-played-with list.
@param hSteamID The Steam ID handle of the user on the recently-played-with list to get the game played.
@return A game App ID.  0 if not on the recently-played-with-list.
@url https://partner.steamgames.com/doc/api/ISteamFriends#GetFriendCoplayGame
*/
extern "C" DLL_EXPORT int GetFriendCoplayGame(int hSteamID)
{
	CheckInitialized(0);
	return SteamFriends()->GetFriendCoplayGame(SteamHandles()->GetSteamHandle(hSteamID));
}

/*
@desc Gets the timestamp of when the user played with someone on their recently-played-with list.
@param hSteamID The Steam ID handle of the user on the recently-played-with list to get the timestamp for.
@return The time in Unix epoch format.  0 if not on the recently-played-with-list.
@url https://partner.steamgames.com/doc/api/ISteamFriends#GetFriendCoplayTime
*/
extern "C" DLL_EXPORT int GetFriendCoplayTime(int hSteamID)
{
	CheckInitialized(0);
	return SteamFriends()->GetFriendCoplayTime(SteamHandles()->GetSteamHandle(hSteamID));
}

/*
@desc Gets the number of users the client knows about who meet a specified criteria.
@param friendFlags A combined union (binary "or") of one or more EFriendFlags.
@param-url friendFlags https://partner.steamgames.com/doc/api/ISteamFriends#EFriendFlags
@return The number of friends matching the given criteria.
@url https://partner.steamgames.com/doc/api/ISteamFriends#GetFriendCount
*/
extern "C" DLL_EXPORT int GetFriendCount(int friendFlags)
{
	CheckInitialized(0);
	return SteamFriends()->GetFriendCount((EFriendFlags)friendFlags);
}

/*
@desc Returns an integer array as JSON that combines the GetFriendCount and GetFriendByIndex calls.
@param friendFlags A combined union (binary "or") of EFriendFlags.
@param-url friendFlags https://partner.steamgames.com/doc/api/ISteamFriends#EFriendFlags
@return An integer array of Steam ID handles as a JSON string.
@url https://partner.steamgames.com/doc/api/ISteamFriends#GetFriendCount
@url https://partner.steamgames.com/doc/api/ISteamFriends#GetFriendByIndex
*/
extern "C" DLL_EXPORT char *GetFriendListJSON(int friendFlags)
{
	std::string json("[");
	if (g_SteamInitialized)
	{
		for (int index = 0; index < SteamFriends()->GetFriendCount((EFriendFlags)friendFlags); index++)
		{
			if (index > 0)
			{
				json += ", ";
			}
			json += std::to_string(SteamHandles()->GetPluginHandle(SteamFriends()->GetFriendByIndex(index, (EFriendFlags)friendFlags)));
		}
	}
	json += "]";
	return utils::CreateString(json);
}

/*
@desc Get the number of users in a source (Steam group, chat room, lobby, or game server).

_NOTE: Large Steam groups cannot be iterated by the local user._
@param hSteamIDSource The Steam group, chat room, lobby or game server to get the user count of.
@return An integer.
@url https://partner.steamgames.com/doc/api/ISteamFriends#GetFriendCountFromSource
*/
extern "C" DLL_EXPORT int GetFriendCountFromSource(int hSteamIDSource)
{
	CheckInitialized(0);
	return SteamFriends()->GetFriendCountFromSource(SteamHandles()->GetSteamHandle(hSteamIDSource));
}

/*
@desc Gets the Steam ID handle at the given index from a source (Steam group, chat room, lobby, or game server).

_NOTE: You must call GetFriendCountFromSource before calling this._
@param hSteamIDSource The Steam group, chat room, lobby or game server to get the user count of.
@param index The index.
@return A Steam ID handle.  Invalid indices return 0.
@url https://partner.steamgames.com/doc/api/ISteamFriends#GetFriendFromSourceByIndex
*/
extern "C" DLL_EXPORT int GetFriendFromSourceByIndex(int hSteamIDSource, int index)
{
	CheckInitialized(0);
	return SteamHandles()->GetPluginHandle(SteamFriends()->GetFriendFromSourceByIndex(SteamHandles()->GetSteamHandle(hSteamIDSource), index));
}

/*
@desc Get a JSON array of Steam ID handles of the users in a source (Steam group, chat room, lobby, or game server).

_NOTE: Large Steam groups cannot be iterated by the local user._
@param hSteamIDSource The Steam group, chat room, lobby or game server to get the user count of.
@return A JSON array of Steam ID handles.
@url https://partner.steamgames.com/doc/api/ISteamFriends#GetFriendCountFromSource
@url https://partner.steamgames.com/doc/api/ISteamFriends#GetFriendFromSourceByIndex
*/
extern "C" DLL_EXPORT char *GetFriendFromSourceListJSON(int hSteamIDSource)
{
	std::string json("[");
	if (g_SteamInitialized)
	{
		CSteamID steamIDSource = SteamHandles()->GetSteamHandle(hSteamIDSource);
		for (int index = 0; index < SteamFriends()->GetFriendCountFromSource(steamIDSource); index++)
		{
			if (index > 0)
			{
				json += ", ";
			}
			json += std::to_string(SteamHandles()->GetPluginHandle(SteamFriends()->GetFriendFromSourceByIndex(steamIDSource, index)));
		}
	}
	json += "]";
	return utils::CreateString(json);
}

/*
@desc Returns the game App ID that the friend is playing.
@param hSteamIDUser A user Steam ID handle.
@return The game App ID if the friend is playing a game; otherwise 0.
@url https://partner.steamgames.com/doc/api/ISteamFriends#GetFriendGamePlayed
@url https://partner.steamgames.com/doc/api/ISteamFriends#FriendGameInfo_t
*/
extern "C" DLL_EXPORT int GetFriendGamePlayedGameAppID(int hSteamIDUser)
{
	CheckInitialized(0);
	FriendGameInfo_t pFriendGameInfo;
	if (SteamFriends()->GetFriendGamePlayed(SteamHandles()->GetSteamHandle(hSteamIDUser), &pFriendGameInfo))
	{
		return pFriendGameInfo.m_gameID.AppID();
	}
	return 0;
}

/*
@desc Returns the IP of the server the friend is playing on.
@param hSteamIDUser A user Steam ID handle.
@return The IP address if the friend is playing a game; otherwise an empty string.
@url https://partner.steamgames.com/doc/api/ISteamFriends#GetFriendGamePlayed
@url https://partner.steamgames.com/doc/api/ISteamFriends#FriendGameInfo_t
*/
extern "C" DLL_EXPORT char *GetFriendGamePlayedIP(int hSteamIDUser)
{
	CheckInitialized(NULL_STRING);
	FriendGameInfo_t pFriendGameInfo;
	if (SteamFriends()->GetFriendGamePlayed(SteamHandles()->GetSteamHandle(hSteamIDUser), &pFriendGameInfo))
	{
		return utils::CreateString(utils::ToIPString(pFriendGameInfo.m_unGameIP));
	}
	return NULL_STRING;
}

/*
@desc Returns the game port of the server the friend is playing on.
@param hSteamIDUser A user Steam ID handle.
@return The game port if the friend is playing a game; otherwise 0.
@url https://partner.steamgames.com/doc/api/ISteamFriends#GetFriendGamePlayed
@url https://partner.steamgames.com/doc/api/ISteamFriends#FriendGameInfo_t
*/
extern "C" DLL_EXPORT int GetFriendGamePlayedConnectionPort(int hSteamIDUser)
{
	CheckInitialized(0);
	FriendGameInfo_t pFriendGameInfo;
	if (SteamFriends()->GetFriendGamePlayed(SteamHandles()->GetSteamHandle(hSteamIDUser), &pFriendGameInfo))
	{
		return pFriendGameInfo.m_usGamePort;
	}
	return 0;
}

/*
@desc Returns the query port of the server the friend is playing on.
@param hSteamIDUser A user Steam ID handle.
@return The query port if the friend is playing a game; otherwise 0.
@url https://partner.steamgames.com/doc/api/ISteamFriends#GetFriendGamePlayed
@url https://partner.steamgames.com/doc/api/ISteamFriends#FriendGameInfo_t
*/
extern "C" DLL_EXPORT int GetFriendGamePlayedQueryPort(int hSteamIDUser)
{
	CheckInitialized(0);
	FriendGameInfo_t pFriendGameInfo;
	if (SteamFriends()->GetFriendGamePlayed(SteamHandles()->GetSteamHandle(hSteamIDUser), &pFriendGameInfo))
	{
		return pFriendGameInfo.m_usQueryPort;
	}
	return 0;
}

/*
@desc Returns the Steam ID handle of lobby the friend is in.
@param hSteamIDUser A user Steam ID handle.
@return A Steam ID lobby handle if the friend is in a lobby; otherwise 0.
@url https://partner.steamgames.com/doc/api/ISteamFriends#GetFriendGamePlayed
@url https://partner.steamgames.com/doc/api/ISteamFriends#FriendGameInfo_t
*/
extern "C" DLL_EXPORT int GetFriendGamePlayedLobby(int hSteamIDUser)
{
	CheckInitialized(0);
	FriendGameInfo_t pFriendGameInfo;
	if (SteamFriends()->GetFriendGamePlayed(SteamHandles()->GetSteamHandle(hSteamIDUser), &pFriendGameInfo))
	{
		return SteamHandles()->GetPluginHandle(pFriendGameInfo.m_steamIDLobby);
	}
	return 0;
}

//GetFriendMessage - used in GameConnectedFriendChatMsg_t callback.

/*
@desc Gets the specified user's persona (display) name.
@param hSteamIDUser A user Steam ID handle.
@return the user's name.
@url https://partner.steamgames.com/doc/api/ISteamFriends#GetFriendPersonaName
*/
extern "C" DLL_EXPORT char *GetFriendPersonaName(int hSteamIDUser)
{
	CheckInitialized(NULL_STRING);
	PersonaStateChangeCallback.Register();
	return utils::CreateString(SteamFriends()->GetFriendPersonaName(SteamHandles()->GetSteamHandle(hSteamIDUser)));
}

/*
@desc Gets one of the previous display names for the specified user.

This only works for display names that the current user has seen on the local computer.
@param hSteamIDFriend The Steam ID handle of the other user.
@param index The index of the history to receive. 0 is their current persona name, 1 is their most recent before they changed it, etc.
@return The players old persona name at the given index. Returns an empty string when there are no further items in the history.
@url https://partner.steamgames.com/doc/api/ISteamFriends#GetFriendPersonaNameHistory
*/
extern "C" DLL_EXPORT char *GetFriendPersonaNameHistory(int hSteamIDFriend, int index)
{
	CheckInitialized(NULL_STRING);
	return utils::CreateString(SteamFriends()->GetFriendPersonaNameHistory(SteamHandles()->GetSteamHandle(hSteamIDFriend), index));
}

/*
@desc Gets a JSON array of the previous display names for the specified user.

This only works for display names that the current user has seen on the local computer.
@param hSteamIDFriend The Steam ID handle of the other user.
@return A JSON array of strings.
@url https://partner.steamgames.com/doc/api/ISteamFriends#GetFriendPersonaNameHistory
*/
extern "C" DLL_EXPORT char *GetFriendPersonaNameHistoryJSON(int hSteamIDFriend)
{
	std::string json("[");
	if (g_SteamInitialized)
	{
		CSteamID steamIDFriend = SteamHandles()->GetSteamHandle(hSteamIDFriend);
		int index = 0;
		const char *name = SteamFriends()->GetFriendPersonaNameHistory(steamIDFriend, index);
		while ((strnlen_s(name, k_cchPersonaNameMax) > 0))
		{
			if (index > 0)
			{
				json += ", ";
			}
			json += "\"" + utils::EscapeJSON(name) + "\"";
			index++;
			name = SteamFriends()->GetFriendPersonaNameHistory(steamIDFriend, index);
		}
	}
	json += "]";
	return utils::CreateString(json);
}

/*
@desc Gets the current status of the specified user.
@param hSteamIDUser A user Steam ID handle.
@return An EPersonaState value.
@return-url https://partner.steamgames.com/doc/api/ISteamFriends#EPersonaState
@url https://partner.steamgames.com/doc/api/ISteamFriends#GetFriendPersonaState
*/
extern "C" DLL_EXPORT int GetFriendPersonaState(int hSteamIDUser)
{
	CheckInitialized(-1);
	PersonaStateChangeCallback.Register();
	return SteamFriends()->GetFriendPersonaState(SteamHandles()->GetSteamHandle(hSteamIDUser));
}

/*
@desc Gets a relationship to a specified user.
@param hSteamIDUser A user Steam ID handle.
@return An EFriendRelationship value.
@return-url https://partner.steamgames.com/doc/api/ISteamFriends#EFriendRelationship
@url https://partner.steamgames.com/doc/api/ISteamFriends#GetFriendRelationship
*/
extern "C" DLL_EXPORT int GetFriendRelationship(int hSteamIDUser)
{
	CheckInitialized(-1);
	return SteamFriends()->GetFriendRelationship(SteamHandles()->GetSteamHandle(hSteamIDUser));
}

/*
@desc Get a Rich Presence value from a specified friend.
@param hSteamIDFriend The friend to get the Rich Presence value for.
@param key The Rich Presence key to request.
@return The value.  An empty string if the specified key is not set.
@url https://partner.steamgames.com/doc/api/ISteamFriends#GetFriendRichPresence
*/
extern "C" DLL_EXPORT char *GetFriendRichPresence(int hSteamIDFriend, const char *key)
{
	CheckInitialized(NULL_STRING);
	return utils::CreateString(SteamFriends()->GetFriendRichPresence(SteamHandles()->GetSteamHandle(hSteamIDFriend), key));
}

/*
@desc Gets the Rich Presence key at the given index from a specified friend.

_NOTE: GetFriendRichPresenceKeyCount must ba called before this can be used._

_This is typically only ever used for debugging purposes._
@param hSteamIDFriend This should be the same user provided to the previous call to GetFriendRichPresenceKeyCount!
@param index The index
@return A key name.  An empty string if the index is invalid.
@url https://partner.steamgames.com/doc/api/ISteamFriends#GetFriendRichPresenceKeyByIndex
*/
extern "C" DLL_EXPORT char *GetFriendRichPresenceKeyByIndex(int hSteamIDFriend, int index)
{
	CheckInitialized(NULL_STRING);
	return utils::CreateString(SteamFriends()->GetFriendRichPresenceKeyByIndex(SteamHandles()->GetSteamHandle(hSteamIDFriend), index));
}

/*
@desc Gets the number of Rich Presence keys that are set on the specified user.

_This is typically only ever used for debugging purposes._
@param hSteamIDFriend The Steam ID of the user to get the Rich Presence Key Count of.
@return An integer.
@url https://partner.steamgames.com/doc/api/ISteamFriends#GetFriendRichPresenceKeyCount
*/
extern "C" DLL_EXPORT int GetFriendRichPresenceKeyCount(int hSteamIDFriend)
{
	CheckInitialized(0);
	return SteamFriends()->GetFriendRichPresenceKeyCount(SteamHandles()->GetSteamHandle(hSteamIDFriend));
}

/*
@desc Gets a JSON array of the Rich Presence keys that are set on the specified user.

_This is typically only ever used for debugging purposes._
@param hSteamIDFriend The Steam ID of the user to get the Rich Presence Keys for.
@return A JSON array of strings.
@url https://partner.steamgames.com/doc/api/ISteamFriends#GetFriendRichPresenceKeyCount
@url https://partner.steamgames.com/doc/api/ISteamFriends#GetFriendRichPresenceKeyByIndex
*/
extern "C" DLL_EXPORT char *GetFriendRichPresenceKeyListJSON(int hSteamIDFriend)
{
	std::string json("[");
	if (g_SteamInitialized)
	{
		CSteamID steamIDFriend = SteamHandles()->GetSteamHandle(hSteamIDFriend);
		for (int index = 0; index < SteamFriends()->GetFriendRichPresenceKeyCount(steamIDFriend); index++)
		{

			if (index > 0)
			{
				json += ", ";
			}
			json += "\"" + utils::EscapeJSON(SteamFriends()->GetFriendRichPresenceKeyByIndex(steamIDFriend, index)) + "\"";
		}
	}
	json += "]";
	return utils::CreateString(json);
}

/*
@desc Gets the number of friends groups (tags) the user has created.
@return The number of friends groups for the current user.
@url https://partner.steamgames.com/doc/api/ISteamFriends#GetFriendsGroupCount
*/
extern "C" DLL_EXPORT int GetFriendsGroupCount()
{
	CheckInitialized(0);
	return SteamFriends()->GetFriendsGroupCount();
}

/*
@desc Gets the friends group ID for the given index.
@param index An index between 0 and GetFriendsGroupCount.
@return The friends group Steam ID handle.
@url https://partner.steamgames.com/doc/api/ISteamFriends#GetFriendsGroupIDByIndex
*/
extern "C" DLL_EXPORT int GetFriendsGroupIDByIndex(int index)
{
	CheckInitialized(k_FriendsGroupID_Invalid);
	return SteamFriends()->GetFriendsGroupIDByIndex(index);
}

/*
@desc Gets the number of friends in a given friends group.
@param hFriendsGroupID The friends group ID handle to get the number of friends in.
@return The number of friends in a friends group.
@url https://partner.steamgames.com/doc/api/ISteamFriends#GetFriendsGroupMembersCount
*/
extern "C" DLL_EXPORT int GetFriendsGroupMembersCount(int hFriendsGroupID)
{
	CheckInitialized(0);
	return SteamFriends()->GetFriendsGroupMembersCount(hFriendsGroupID);
}

/*
@desc Gets a JSON integer array of SteamID handles of the list of members in a group.
@param hFriendsGroupID The friends group ID handle to get the members list of.
@return An integer array of Steam ID handles as a JSON string.
@url https://partner.steamgames.com/doc/api/ISteamFriends#GetFriendsGroupMembersCount
@url https://partner.steamgames.com/doc/api/ISteamFriends#GetFriendsGroupMembersList
*/
extern "C" DLL_EXPORT char *GetFriendsGroupMembersListJSON(int hFriendsGroupID)
{
	std::string json("[");
	if (g_SteamInitialized)
	{
		int count = SteamFriends()->GetFriendsGroupMembersCount(hFriendsGroupID);
		if (count > 0)
		{
			CSteamID *pSteamIDMembers = new CSteamID[count];
			SteamFriends()->GetFriendsGroupMembersList(hFriendsGroupID, pSteamIDMembers, count);
			for (int index = 0; index < count; index++)
			{
				if (index > 0)
				{
					json += ", ";
				}
				json += std::to_string(SteamHandles()->GetPluginHandle(pSteamIDMembers[index]));
			}
			delete[] pSteamIDMembers;
		}
	}
	json += "]";
	return utils::CreateString(json);
}

/*
@desc Gets the name for the given friends group.
@param hFriendsGroupID The friends group ID handle to get the name of.
@return The name of the friends group.
@url https://partner.steamgames.com/doc/api/ISteamFriends#GetFriendsGroupName
*/
extern "C" DLL_EXPORT char *GetFriendsGroupName(int hFriendsGroupID)
{
	CheckInitialized(NULL_STRING);
	return utils::CreateString(SteamFriends()->GetFriendsGroupName(hFriendsGroupID));
}

/*
@desc Gets the Steam level of the specified user.

_This will return 0 unless the level has been cached by the API.  Despite how it appears that it should behave, the Steam API does not seem to fire the PersonaStateChange_t callback when Steam levels cache._
@param hSteamIDUser A user Steam ID handle.
@return the user's Steam level or 0 if not loaded.
@url https://partner.steamgames.com/doc/api/ISteamFriends#GetFriendSteamLevel
*/
extern "C" DLL_EXPORT int GetFriendSteamLevel(int hSteamIDUser)
{
	/*
	NOTE: Steam docs say
	"If the Steam level is not immediately available for the specified user then this returns 0 and queues it to be downloaded from the Steam servers.
	When it gets downloaded a PersonaStateChange_t callback will be posted with m_nChangeFlags including k_EPersonaChangeSteamLevel."

	HOWEVER, this doesn't actually appear to be the case.  GetFriendSteamLevel returns 0, but the callback never seems to fire.
	Possible solution is to just keep requesting the level when 0 is returned.
	*/
	CheckInitialized(0);
	PersonaStateChangeCallback.Register();
	return SteamFriends()->GetFriendSteamLevel(SteamHandles()->GetSteamHandle(hSteamIDUser));
}

/*
@desc Gets the avatar image handle for the given steam id and avatar size.  Avatars come in three sizes.  The Steam API has a command for each size, but this plugin combines them into a single command for simplicity.

If the avatar has not been cached, the returned image handle will be -1.  HasAvatarImageLoaded will report when a user's avatar has loaded and GetFriendAvatar can be called again to retrieve it.

According to the Steamworks documentation:
> This only works for users that the local user knows about. They will automatically know about their friends, people on leaderboards they've requested, or people in the same source as them (Steam group, chat room, lobby, or game server). If they don't know about them then you must call RequestUserInformation to cache the avatar locally.

Values for the size parameter:
```
#constant AVATAR_SMALL	0 // 32x32
#constant AVATAR_MEDIUM	1 // 64x64
#constant AVATAR_LARGE	2 // 128x128
```
@param hSteamIDUser A user Steam ID handle.
@param size The size of the avatar to load.
@return 0 when no avatar is set, -1 when the image needs to be loaded, and a positive number is an image handle.
Use [LoadImageFromHandle](Image-Loading#loadimagefromhandle) to load the image from the handle.
@url https://partner.steamgames.com/doc/api/ISteamFriends#GetSmallFriendAvatar
@url https://partner.steamgames.com/doc/api/ISteamFriends#GetMediumFriendAvatar
@url https://partner.steamgames.com/doc/api/ISteamFriends#GetLargeFriendAvatar
*/
extern "C" DLL_EXPORT int GetFriendAvatar(int hSteamIDUser, int size)
{
	CheckInitialized(0);
	AvatarImageLoadedCallback.Register();
	PersonaStateChangeCallback.Register();
	CSteamID steamID = SteamHandles()->GetSteamHandle(hSteamIDUser);
	// NOTE: The Steam API appears to implicitly call RequestUserInformation when needed.
	//SteamFriends()->RequestUserInformation(steamID, false);
	int hImage = 0;
	switch (size)
	{
	case AVATAR_SMALL:
		hImage = SteamFriends()->GetSmallFriendAvatar(steamID);
		break;
	case AVATAR_MEDIUM:
		hImage = SteamFriends()->GetMediumFriendAvatar(steamID);
		break;
	case AVATAR_LARGE:
		hImage = SteamFriends()->GetLargeFriendAvatar(steamID);
		break;
	default:
		agk::PluginError("Invalid avatar size.");
		return 0;
	}
	return hImage;
}

/*
@desc Gets a handle to the large (128*128px) avatar for the specified user.
@param hSteamIDUser A user Steam ID handle.
@return 0 when no avatar is set, -1 when the image needs to be loaded, and a positive number is an image handle.
Use [LoadImageFromHandle](Image-Loading#loadimagefromhandle) to load the image from the handle.
@url https://partner.steamgames.com/doc/api/ISteamFriends#GetLargeFriendAvatar
*/
extern "C" DLL_EXPORT int GetLargeFriendAvatar(int hSteamIDUser)
{
	return GetFriendAvatar(hSteamIDUser, AVATAR_LARGE);
}

/*
@desc Gets a handle to the medium (64*64px) avatar for the specified user.
@param hSteamIDUser A user Steam ID handle.
@return 0 when no avatar is set, -1 when the image needs to be loaded, and a positive number is an image handle.
Use [LoadImageFromHandle](Image-Loading#loadimagefromhandle) to load the image from the handle.
@url https://partner.steamgames.com/doc/api/ISteamFriends#GetMediumFriendAvatar
*/
extern "C" DLL_EXPORT int GetMediumFriendAvatar(int hSteamIDUser)
{
	return GetFriendAvatar(hSteamIDUser, AVATAR_MEDIUM);
}

/*
@desc Gets the current users persona (display) name.
@return the name
@url https://partner.steamgames.com/doc/api/ISteamFriends#GetPersonaName
*/
extern "C" DLL_EXPORT char *GetPersonaName()
{
	CheckInitialized(NULL_STRING);
	return utils::CreateString(SteamFriends()->GetPersonaName());
}

/*
@desc Gets the friend status of the current user.

To get the state of other users use GetFriendPersonaState.
@return The status of the current user.
@return-url https://partner.steamgames.com/doc/api/ISteamFriends#EPersonaState
@url https://partner.steamgames.com/doc/api/ISteamFriends#GetPersonaState
*/
extern "C" DLL_EXPORT int GetPersonaState()
{
	CheckInitialized(0);
	return SteamFriends()->GetPersonaState();
}

/*
@desc Gets the nickname that the current user has set for the specified user.
@param hSteamIDUser A user Steam ID handle.
@return The player's nickname.
@url https://partner.steamgames.com/doc/api/ISteamFriends#GetPlayerNickname
*/
extern "C" DLL_EXPORT char *GetPlayerNickname(int hSteamIDUser)
{
	CheckInitialized(NULL_STRING);
	return utils::CreateString(SteamFriends()->GetPlayerNickname(SteamHandles()->GetSteamHandle(hSteamIDUser)));
}

/*
@desc Gets a handle to the small (32*32px) avatar for the specified user.
@param hSteamIDUser A user Steam ID handle.
@return 0 when no avatar is set, -1 when the image needs to be loaded, and a positive number is an image handle.
Use [LoadImageFromHandle](Image-Loading#loadimagefromhandle) to load the image from the handle.
@url https://partner.steamgames.com/doc/api/ISteamFriends#GetMediumFriendAvatar
*/
extern "C" DLL_EXPORT int GetSmallFriendAvatar(int hSteamIDUser)
{
	return GetFriendAvatar(hSteamIDUser, AVATAR_SMALL);
}

/*
@desc Checks if current user is chat restricted.
@return Restriction flags.
@return-url https://partner.steamgames.com/doc/api/ISteamFriends#EUserRestriction
@url https://partner.steamgames.com/doc/api/ISteamFriends#GetUserRestrictions
*/
extern "C" DLL_EXPORT int GetUserRestrictions()
{
	CheckInitialized(0);
	return SteamFriends()->GetUserRestrictions();
}

/*
@desc Checks if the user meets the specified criteria.
@param hSteamIDUser A user Steam ID handle.
@param friendFlags A combined union (binary "or") of one or more EFriendFlags.
@param-url friendFlags https://partner.steamgames.com/doc/api/ISteamFriends#EFriendFlags
@return 1 when true, 0 when false.
@url https://partner.steamgames.com/doc/api/ISteamFriends#HasFriend
*/
extern "C" DLL_EXPORT int HasFriend(int hSteamIDUser, int friendFlags)
{
	CheckInitialized(false);
	return SteamFriends()->HasFriend(SteamHandles()->GetSteamHandle(hSteamIDUser), (EFriendFlags)friendFlags);
}

/*
@desc Invites a friend or clan member to the current game using a special invite string.
@param hSteamIDFriend The Steam ID of the friend to invite.
@param connectString A string that lets the friend know how to join the game (I.E. the game server IP). This can not be longer than specified in k_cchMaxRichPresenceValueLength.
@return 1 if the invite was successfully sent; otherwise 0.
@url https://partner.steamgames.com/doc/api/ISteamFriends#InviteUserToGame
*/
extern "C" DLL_EXPORT int InviteUserToGame(int hSteamIDFriend, const char *connectString) //Fires GameRichPresenceJoinRequested_t for other user
{
	CheckInitialized(0);
	return SteamFriends()->InviteUserToGame(SteamHandles()->GetSteamHandle(hSteamIDFriend), connectString);
}

/*
@desc Checks if a user in the Steam group chat room is an admin.
@param hSteamIDClanChat The Steam ID of the Steam group chat room.
@param hSteamIDUser The Steam ID of the user to check the admin status of.
@return 1 if the specified user is an admin; otherwise 0.
@url https://partner.steamgames.com/doc/api/ISteamFriends#IsClanChatAdmin
*/
extern "C" DLL_EXPORT int IsClanChatAdmin(int hSteamIDClanChat, int hSteamIDUser)
{
	CheckInitialized(0);
	return SteamFriends()->IsClanChatAdmin(SteamHandles()->GetSteamHandle(hSteamIDClanChat), SteamHandles()->GetSteamHandle(hSteamIDUser));
}

/*
@desc Checks if the Steam group is public.
@param hSteamIDClan The Steam ID of the Steam group chat room.
@return 1 if the specified group is public; otherwise 0.
@url https://partner.steamgames.com/doc/api/ISteamFriends#IsClanPublic
*/
extern "C" DLL_EXPORT int IsClanPublic(int hSteamIDClan)
{
	CheckInitialized(0);
	return SteamFriends()->IsClanPublic(SteamHandles()->GetSteamHandle(hSteamIDClan));
}

/*
@desc Checks if the Steam group is an official game group/community hub.
@param hSteamIDClan The Steam ID of the Steam group.
@return 1 if the specified group is an official game group/community hub; otherwise 0.
@url https://partner.steamgames.com/doc/api/ISteamFriends#IsClanOfficialGameGroup
*/
extern "C" DLL_EXPORT int IsClanOfficialGameGroup(int hSteamIDClan)
{
	CheckInitialized(0);
	return SteamFriends()->IsClanOfficialGameGroup(SteamHandles()->GetSteamHandle(hSteamIDClan));
}

/*
@desc Checks if the Steam Group chat room is open in the Steam UI.
@param hSteamIDClan The Steam ID of the Steam group chat room to check.
@return 1 if the specified Steam group chat room is opened; otherwise 0.
@url https://partner.steamgames.com/doc/api/ISteamFriends#IsClanChatWindowOpenInSteam
*/
extern "C" DLL_EXPORT int IsClanChatWindowOpenInSteam(int hSteamIDClan)
{
	CheckInitialized(0);
	return SteamFriends()->IsClanChatWindowOpenInSteam(SteamHandles()->GetSteamHandle(hSteamIDClan));
}

class CIsFollowingCallResult : public CCallResultItem<FriendsIsFollowing_t>
{
public:
	CIsFollowingCallResult(CSteamID steamID)
	{
		m_CallResultName = "IsFollowing(" + std::to_string(steamID.ConvertToUint64()) + ")";
		m_hSteamAPICall = SteamFriends()->IsFollowing(steamID);
	}
	CSteamID GetSteamID() { return m_Response.m_steamID; }
	bool IsFollowing() { return m_Response.m_bIsFollowing; }
};

/*
@desc Checks if the current user is following the specified user.
@param hSteamID The Steam ID of the check if we are following.
@callback-type callresult
@callback-getters GetIsFollowingSteamID, GetIsFollowingResult
@return A [call result handle](Callbacks-and-Call-Results#call-results) on success; otherwise 0.
@url https://partner.steamgames.com/doc/api/ISteamFriends#IsFollowing
@url https://partner.steamgames.com/doc/api/ISteamFriends#FriendsIsFollowing_t
*/
extern "C" DLL_EXPORT int IsFollowing(int hSteamID)
{
	CheckInitialized(0);
	return CallResults()->Add(new CIsFollowingCallResult(SteamHandles()->GetSteamHandle(hSteamID)));
}

/*
@desc Gets the Steam ID that was checked for the IsFollowing call result.
@param hCallResult An IsFollowing call result handle.
@return A Steam ID handle.
*/
extern "C" DLL_EXPORT int GetIsFollowingSteamID(int hCallResult)
{
	return GetCallResultValue(hCallResult, &CIsFollowingCallResult::GetSteamID);
}

/*
@desc Gets the 'is following' result for the IsFollowing call result.
@param hCallResult An IsFollowing call result handle.
@return 1 if following; otherwise 0.
*/
extern "C" DLL_EXPORT int GetIsFollowingResult(int hCallResult)
{
	return GetCallResultValue(hCallResult, &CIsFollowingCallResult::IsFollowing);
}

/*
@desc Checks if a specified user is in a source (Steam group, chat room, lobby, or game serve
@param hSteamIDUser The user to check if they are in the source.
@param hSteamIDSource The source to check for the user.
@return 1 if the local user can see that steamIDUser is a member or in steamIDSource; otherwise, 0.
@url https://partner.steamgames.com/doc/api/ISteamFriends#IsUserInSource
*/
extern "C" DLL_EXPORT int IsUserInSource(int hSteamIDUser, int hSteamIDSource)
{
	CheckInitialized(0);
	return SteamFriends()->IsUserInSource(SteamHandles()->GetSteamHandle(hSteamIDUser), SteamHandles()->GetSteamHandle(hSteamIDSource));
}

void ResponseWrapper<JoinClanChatRoomCompletionResult_t>::SetResult()
{
	m_eResult = (m_eChatRoomEnterResponse == k_EChatRoomEnterResponseSuccess) ? k_EResultOK : k_EResultFail;
}

class CJoinClanChatRoomCallResult : public CCallResultItem<JoinClanChatRoomCompletionResult_t, ResponseWrapper<JoinClanChatRoomCompletionResult_t>>
{
public:
	CJoinClanChatRoomCallResult(CSteamID steamIDClan)
	{
		m_CallResultName = "JoinClanChatRoom(" + std::to_string(steamIDClan.ConvertToUint64()) + ")";
		m_hSteamAPICall = SteamFriends()->JoinClanChatRoom(steamIDClan);
	}
	int GetChatRoomEnterResponse() { return (int)m_Response.m_eChatRoomEnterResponse; }
	CSteamID GetSteamIDClanChat() { return m_Response.m_steamIDClanChat; }
protected:
	void OnResponse(JoinClanChatRoomCompletionResult_t *pCallResult, bool bFailure)
	{
		CCallResultItem::OnResponse(pCallResult, bFailure);
		ClanChatManager()->Add(pCallResult->m_steamIDClanChat);
	}
};

/*
@desc Allows the user to join Steam group (clan) chats right within the game.
@param hSteamIDClan The Steam ID of the Steam group to join.
@return A [call result handle](Callbacks-and-Call-Results#call-results) on success; otherwise 0.
@callback-type callresult
@callback-getters GetJoinClanChatRoomEnterResponse, GetJoinClanChatRoomSteamID
@url https://partner.steamgames.com/doc/api/ISteamFriends#JoinClanChatRoom
@url https://partner.steamgames.com/doc/api/ISteamFriends#JoinClanChatRoomCompletionResult_t
*/
extern "C" DLL_EXPORT int JoinClanChatRoom(int hSteamIDClan)
{
	CheckInitialized(0);
	//GameConnectedChatJoinCallback.Register();
	return CallResults()->Add(new CJoinClanChatRoomCallResult(SteamHandles()->GetSteamHandle(hSteamIDClan)));
}

/*
@desc Returns the result of the JoinClanChatRoom call result.
Provides more detail that GetCallResultCode.
@param hCallResult A JoinClanChatRoom call result handle.
@return An EChatRoomEnterResponse value.
@return-url https://partner.steamgames.com/doc/api/steam_api#EChatRoomEnterResponse
*/
extern "C" DLL_EXPORT int GetJoinClanChatRoomEnterResponse(int hCallResult)
{
	return GetCallResultValue(hCallResult, &CJoinClanChatRoomCallResult::GetChatRoomEnterResponse);
}

/*
@desc The Steam ID of the chat that the user has joined for the given JoinClanChatRoom
@param hCallResult A JoinClanChatRoom call result handle.
@return A Steam ID handle.
*/
extern "C" DLL_EXPORT int GetJoinClanChatRoomSteamID(int hCallResult)
{
	return GetCallResultValue(hCallResult, &CJoinClanChatRoomCallResult::GetSteamIDClanChat);
}

/*
@desc Leaves a Steam group chat that the user has previously entered with JoinClanChatRoom.

Triggers a GameConnectedChatLeave_t callback for the other members in the group chat.
@param hSteamIDClan The Steam ID of the Steam group chat to leave.
@return 1 if user is in the specified chat room, otherwise 0.
@url https://partner.steamgames.com/doc/api/ISteamFriends#LeaveClanChatRoom
*/
extern "C" DLL_EXPORT int LeaveClanChatRoom(int hSteamIDClan)
{
	CheckInitialized(0);
	return SteamFriends()->LeaveClanChatRoom(SteamHandles()->GetSteamHandle(hSteamIDClan));
}

/*
@desc Opens the specified Steam group chat room in the Steam UI.
@param hSteamIDClanChat The Steam ID of the Steam group chat room to open.
@return 1 if the user successfully entered the Steam group chat room; otherwise, 0.
@url https://partner.steamgames.com/doc/api/ISteamFriends#OpenClanChatWindowInSteam
*/
extern "C" DLL_EXPORT int OpenClanChatWindowInSteam(int hSteamIDClanChat)
{
	CheckInitialized(0);
	return SteamFriends()->OpenClanChatWindowInSteam(SteamHandles()->GetSteamHandle(hSteamIDClanChat));
}

/*
@desc Sends a message to a Steam friend.
@param hSteamIDFriend The Steam ID of the friend to send the message to.
@param message The UTF-8 formatted message to send.
@return 1 if the message was successfully sent.
0 if the current user is rate limited or chat restricted.
@url https://partner.steamgames.com/doc/api/ISteamFriends#ReplyToFriendMessage
*/
extern "C" DLL_EXPORT int ReplyToFriendMessage(int hSteamIDFriend, const char *message)
{
	CheckInitialized(0);
	return SteamFriends()->ReplyToFriendMessage(SteamHandles()->GetSteamHandle(hSteamIDFriend), message);
}

class CClanOfficerListResponseCallResult : public CCallResultItem<ClanOfficerListResponse_t, ResponseWrapper<ClanOfficerListResponse_t>>
{
public:
	CClanOfficerListResponseCallResult(CSteamID steamIDClan)
	{
		m_CallResultName = "RequestClanOfficerList(" + std::to_string(steamIDClan.ConvertToUint64()) + ")";
		m_hSteamAPICall = SteamFriends()->RequestClanOfficerList(steamIDClan);
	}
	CSteamID GetSteamIDClan() { return m_Response.m_steamIDClan; }
	int GetOfficerCount() { return m_Response.m_cOfficers; }
};

/*
@desc Requests information about a Steam group officers (administrators and moderators).
@param hSteamIDClan The Steam group to get the officers list for.
@callback-type callresult
@callback-getters GetRequestClanOfficerListOfficerCount, GetRequestClanOfficerListClan
@return A [call result handle](Callbacks-and-Call-Results#call-results) on success; otherwise 0.
@url https://partner.steamgames.com/doc/api/ISteamFriends#RequestClanOfficerList
@url https://partner.steamgames.com/doc/api/ISteamFriends#ClanOfficerListResponse_t
*/
extern "C" DLL_EXPORT int RequestClanOfficerList(int hSteamIDClan)
{
	CheckInitialized(0);
	return CallResults()->Add(new CClanOfficerListResponseCallResult(SteamHandles()->GetSteamHandle(hSteamIDClan)));
}

/*
@desc Gets the number of officers in the group from the call result response. This is the same as GetClanOfficerCount.
@param hCallResult An RequestClanOfficerList call result handle.
@return An integer.
*/
extern "C" DLL_EXPORT int GetRequestClanOfficerListOfficerCount(int hCallResult)
{
	return GetCallResultValue(hCallResult, &CClanOfficerListResponseCallResult::GetOfficerCount);
}

/*
@desc Gets the Steam group that the call result just got the officer list for.
@param hCallResult An RequestClanOfficerList call result handle.
@return A Steam ID handle.  0 if there was a problem.
*/
extern "C" DLL_EXPORT int GetRequestClanOfficerListClan(int hCallResult)
{
	return GetCallResultValue(hCallResult, &CClanOfficerListResponseCallResult::GetSteamIDClan);
}

/*
@desc Requests Rich Presence data from a specific user.

Triggers a FriendRichPresenceUpdate_t callback.
@param hSteamIDFriend The Steam ID of the user to request the rich presence of.
@url https://partner.steamgames.com/doc/api/ISteamFriends#RequestFriendRichPresence
*/
extern "C" DLL_EXPORT void RequestFriendRichPresence(int hSteamIDFriend)
{
	CheckInitialized(NORETURN);
	FriendRichPresenceUpdateCallback.Register();
	SteamFriends()->RequestFriendRichPresence(SteamHandles()->GetSteamHandle(hSteamIDFriend));
}

/*
@desc Requests the persona name and optionally the avatar of a specified user (when requireNameOnly is 0).
@param hSteamIDUser A user Steam ID handle.
@param requireNameOnly When 0 the avatar for the user is also loaded; otherwise only the name is loaded.
@return 1 when the user data needs to be loaded and will be reported by the PersonaStateChange_t callback.  0 when the user data is already loaded.
@url https://partner.steamgames.com/doc/api/ISteamFriends#RequestUserInformation
*/
extern "C" DLL_EXPORT int RequestUserInformation(int hSteamIDUser, int requireNameOnly)
{
	CheckInitialized(false);
	PersonaStateChangeCallback.Register();
	AvatarImageLoadedCallback.Register();
	return SteamFriends()->RequestUserInformation(SteamHandles()->GetSteamHandle(hSteamIDUser), requireNameOnly != 0);
}

/*
@desc Sends a message to a Steam group chat room.
@param hSteamIDClanChat The Steam ID of the group chat to send the message to.
@param text The UTF-8 formatted message to send. This can be up to 2048 characters long.
@return 1 if the message was successfully sent; otherwise 0.
@url https://partner.steamgames.com/doc/api/ISteamFriends#SendClanChatMessage
*/
extern "C" DLL_EXPORT int SendClanChatMessage(int hSteamIDClanChat, const char *text)
{
	CheckInitialized(false);
	return SteamFriends()->SendClanChatMessage(SteamHandles()->GetSteamHandle(hSteamIDClanChat), text);
}

/*
@desc Let Steam know that the user is currently using voice chat in game.

This will suppress the microphone for all voice communication in the Steam UI.
@param speaking Did the user start speaking in game (1) or stopped speaking in game (0)?
@url https://partner.steamgames.com/doc/api/ISteamFriends#SetInGameVoiceSpeaking
*/
extern "C" DLL_EXPORT void SetInGameVoiceSpeaking(int speaking)
{
	CheckInitialized(NORETURN);
	SteamFriends()->SetInGameVoiceSpeaking(SteamUser()->GetSteamID(), speaking != 0);
}

/*
@desc Listens for Steam friends chat messages.
@param interceptEnabled Turn friends message interception on (1) or off (0)?
@url https://partner.steamgames.com/doc/api/ISteamFriends#SetListenForFriendsMessages
*/
extern "C" DLL_EXPORT void SetListenForFriendsMessages(int interceptEnabled)
{
	CheckInitialized(NORETURN);
	if (interceptEnabled)
	{
		GameConnectedFriendChatMsgCallback.Register();
	}
	else
	{
		GameConnectedFriendChatMsgCallback.Unregister();
	}
	SteamFriends()->SetListenForFriendsMessages(interceptEnabled != 0);
}

void ResponseWrapper<SetPersonaNameResponse_t>::SetResult() { m_eResult = m_result; }

class CSetPersonaNameCallResult : public CCallResultItem<SetPersonaNameResponse_t, ResponseWrapper<SetPersonaNameResponse_t>>
{
public:
	CSetPersonaNameCallResult(const char *pchPersonaName)
	{
		m_CallResultName = "SetPersonaName(" + std::string(pchPersonaName) + ")";
		m_hSteamAPICall = SteamFriends()->SetPersonaName(pchPersonaName);
	}
	int GetSuccess() { return m_Response.m_bSuccess; }
	int GetLocalSuccess() { return m_Response.m_bLocalSuccess; }
};

/*
@desc Sets the current users persona name, stores it on the server and publishes the changes to all friends who are online.
@param personaName The users new persona name. Can not be longer than k_cchPersonaNameMax bytes.
@callback-type callresult
@callback-getters GetSetPersonaNameSuccess, GetSetPersonaNameLocalSuccess
@return A [call result handle](Callbacks-and-Call-Results#call-results) on success; otherwise 0.
@url https://partner.steamgames.com/doc/api/ISteamFriends#SetPersonaName
@url https://partner.steamgames.com/doc/api/ISteamFriends#SetPersonaNameResponse_t
*/
extern "C" DLL_EXPORT int SetPersonaName(const char *personaName)
{
	CheckInitialized(0);
	return CallResults()->Add(new CSetPersonaNameCallResult(personaName));
}

/*
@desc Returns whether the name change completed successfully.
@param hCallResult A SetPersonaName call result handle.
@return 1 if name change completed successfully.
*/
extern "C" DLL_EXPORT int GetSetPersonaNameSuccess(int hCallResult)
{
	return GetCallResultValue(hCallResult, &CSetPersonaNameCallResult::GetSuccess);
}

/*
@desc Returns whether the name change was retained locally.
@param hCallResult A SetPersonaName call result handle.
@return 1 if name change was retained locally. We might not have been able to communicate with Steam.
*/
extern "C" DLL_EXPORT int GetSetPersonaNameLocalSuccess(int hCallResult)
{
	return GetCallResultValue(hCallResult, &CSetPersonaNameCallResult::GetLocalSuccess);
}

/*
@desc Mark a target user as 'played with'.
@param hSteamIDUserPlayedWith The other user that we have played with.
@url https://partner.steamgames.com/doc/api/ISteamFriends#SetPlayedWith
*/
extern "C" DLL_EXPORT void SetPlayedWith(int hSteamIDUserPlayedWith)
{
	CheckInitialized(NORETURN);
	SteamFriends()->SetPlayedWith(SteamHandles()->GetSteamHandle(hSteamIDUserPlayedWith));
}

/*
@desc Sets a Rich Presence key/value for the current user that is automatically shared to all friends playing the same game.
@param key The rich presence 'key' to set. This can not be longer than specified in k_cchMaxRichPresenceKeyLength.
@param value The rich presence 'value' to associate with pchKey. This can not be longer than specified in k_cchMaxRichPresenceValueLength.
If this is set to an empty string ("") or NULL then the key is removed if it's set.
@return 1 if the rich presence was set successfully; otherwise 0.
@url https://partner.steamgames.com/doc/api/ISteamFriends#SetRichPresence
*/
extern "C" DLL_EXPORT int SetRichPresence(const char *key, const char *value)
{
	CheckInitialized(false);
	return SteamFriends()->SetRichPresence(key, value);
}

//Callbacks
/*
@desc Triggered when an avatar is loaded if you have tried requesting it when it was unavailable.
@callback-type list
@callback-getters	GetAvatarImageLoadedUser
@return 1 when the callback has more responses to process; otherwise 0.
@url https://partner.steamgames.com/doc/api/ISteamFriends#AvatarImageLoaded_t
*/
extern "C" DLL_EXPORT int HasAvatarImageLoadedResponse()
{
	return AvatarImageLoadedCallback.HasResponse();
}

/*
@desc Returns the Steam ID handle for current AvatarImageLoaded_t callback response.
Call GetFriendAvatar with the returned user handle to get the image handle
@return The SteamID handle of the user whose avatar loaded.
*/
extern "C" DLL_EXPORT int GetAvatarImageLoadedUser()
{
	return SteamHandles()->GetPluginHandle(AvatarImageLoadedCallback.GetCurrent());
}

//ClanOfficerListResponse_t - call result for RequestClanOfficerList
//DownloadClanActivityCountsResult_t - call result for DownloadClanActivityCounts

/*
@desc Called when Rich Presence data has been updated for a user, this can happen automatically when friends in the 
same game update their rich presence, or after a call to RequestFriendRichPresence.
@callback-type list
@callback-getters GetFriendRichPresenceUpdateUser
@return 1 when the callback has more responses to process; otherwise 0.
@url https://partner.steamgames.com/doc/api/ISteamFriends#FriendRichPresenceUpdate_t
*/
extern "C" DLL_EXPORT int HasFriendRichPresenceUpdateResponse()
{
	return FriendRichPresenceUpdateCallback.HasResponse();
}

/*
@desc Returns the Steam ID handle for current FriendRichPresenceUpdate_t callback response.
@return A Steam ID handle.
*/
extern "C" DLL_EXPORT int GetFriendRichPresenceUpdateUser()
{
	return SteamHandles()->GetPluginHandle(FriendRichPresenceUpdateCallback.GetCurrent());
}

//FriendsEnumerateFollowingList_t - call result for EnumerateFollowingList
//FriendsGetFollowerCount_t - call result for GetFollowerCount
//FriendsIsFollowing_t - call result for IsFollowing

/*
@desc Called when a user has joined a Steam group chat that the we are in.
@callback-type list
@callback-getters GetGameConnectedChatJoinClan, GetGameConnectedChatJoinUser
@return 1 when the callback has more responses to process; otherwise 0.
@url https://partner.steamgames.com/doc/api/ISteamFriends#GameConnectedChatJoin_t
*/
extern "C" DLL_EXPORT int HasGameConnectedChatJoinResponse()
{
	return GameConnectedChatJoinCallback.HasResponse();
}

/*
@desc Returns the Steam ID of the chat that a user has joined..
@return A Steam ID handle.
*/
extern "C" DLL_EXPORT int GetGameConnectedChatJoinClan()
{
	return SteamHandles()->GetPluginHandle(GameConnectedChatJoinCallback.GetCurrent().m_steamIDClanChat);
}

/*
@desc Returns the Steam ID of the user that has joined the chat.
@return A Steam ID handle.
*/
extern "C" DLL_EXPORT int GetGameConnectedChatJoinUser()
{
	return SteamHandles()->GetPluginHandle(GameConnectedChatJoinCallback.GetCurrent().m_steamIDUser);
}

/*
@desc Called when a user has left a Steam group chat that the we are in.
@callback-type list
@callback-getters GetGameConnectedChatLeaveClan, GetGameConnectedChatLeaveUser, GetGameConnectedChatLeaveDropped, 
GetGameConnectedChatLeaveKicked
@return 1 when the callback has more responses to process; otherwise 0.
@url https://partner.steamgames.com/doc/api/ISteamFriends#GameConnectedChatLeave_t
*/
extern "C" DLL_EXPORT int HasGameConnectedChatLeaveResponse()
{
	return GameConnectedChatLeaveCallback.HasResponse();
}

/*
@desc Returns the Steam ID of the chat that a user has left.
@return A Steam ID handle.
*/
extern "C" DLL_EXPORT int GetGameConnectedChatLeaveClan()
{
	return SteamHandles()->GetPluginHandle(GameConnectedChatLeaveCallback.GetCurrent().m_steamIDClanChat);
}

/*
@desc Returns the Steam ID of the user that has left the chat.
@return A Steam ID handle.
*/
extern "C" DLL_EXPORT int GetGameConnectedChatLeaveUser()
{
	return SteamHandles()->GetPluginHandle(GameConnectedChatLeaveCallback.GetCurrent().m_steamIDUser);
}

/*
@desc Returns whether the user's connection to Steam dropped or they left via other means.
@return 1 if dropped; otherwise 0.
*/
extern "C" DLL_EXPORT int GetGameConnectedChatLeaveDropped()
{
	return GameConnectedChatLeaveCallback.GetCurrent().m_bDropped;
}

/*
@desc Returns whether or not the user was kicked by an officer.
@return 1 if kicked; otherwise 0.
*/
extern "C" DLL_EXPORT int GetGameConnectedChatLeaveKicked()
{
	return GameConnectedChatLeaveCallback.GetCurrent().m_bKicked;
}

/*
@desc Called when a chat message has been received in a Steam group chat that we are in.
@callback-type list
@callback-getters GetGameConnectedClanChatMessageEntryType, GetGameConnectedClanChatMessageClan, GetGameConnectedClanChatMessageUser,
GetGameConnectedClanChatMessageText
@return 1 when the callback has more responses to process; otherwise 0.
@url https://partner.steamgames.com/doc/api/ISteamFriends#GameConnectedClanChatMsg_t
*/
extern "C" DLL_EXPORT int HasGameConnectedClanChatMessageResponse()
{
	return GameConnectedClanChatMsgCallback.HasResponse();
}

/*
@desc Returns the type of chat entry that was received.
@return An EChatEntryType value.
@return-url https://partner.steamgames.com/doc/api/steam_api#EChatEntryType
*/
extern "C" DLL_EXPORT int GetGameConnectedClanChatMessageEntryType()
{
	return GameConnectedClanChatMsgCallback.GetCurrent().m_ChatEntryType;
}

/*
@desc Returns the Steam ID of the chat that the message was received in.
@return A Steam ID handle.
*/
extern "C" DLL_EXPORT int GetGameConnectedClanChatMessageClan()
{
	return SteamHandles()->GetPluginHandle(GameConnectedClanChatMsgCallback.GetCurrent().m_steamIDClanChat);
}

/*
@desc Returns the Steam ID of the user that sent the message.
@return A Steam ID handle.
*/
extern "C" DLL_EXPORT int GetGameConnectedClanChatMessageUser()
{
	return SteamHandles()->GetPluginHandle(GameConnectedClanChatMsgCallback.GetCurrent().m_steamIDUser);
}

/*
@desc Returns the chat message that was sent.
@return A string.
*/
extern "C" DLL_EXPORT char *GetGameConnectedClanChatMessageText()
{
	return utils::CreateString(GameConnectedClanChatMsgCallback.GetCurrent().m_Text);
}

/*
@desc Called when chat message has been received from a friend.
@callback-type list
@callback-getters GetGameConnectedFriendChatMessageEntryType, GetGameConnectedFriendChatMessageUser, GetGameConnectedFriendChatMessageText
@return 1 when the callback has more responses to process; otherwise 0.
@url https://partner.steamgames.com/doc/api/ISteamFriends#GameConnectedFriendChatMsg_t
*/
extern "C" DLL_EXPORT int HasGameConnectedFriendChatMessageResponse()
{
	return GameConnectedFriendChatMsgCallback.HasResponse();
}

/*
@desc Returns the type of chat entry that was received.
@return An EChatEntryType value.
@return-url https://partner.steamgames.com/doc/api/steam_api#EChatEntryType
*/
extern "C" DLL_EXPORT int GetGameConnectedFriendChatMessageEntryType()
{
	return GameConnectedFriendChatMsgCallback.GetCurrent().m_ChatEntryType;
}

/*
@desc Returns the Steam ID of the friend that sent the message.
@return A Steam ID handle.
*/
extern "C" DLL_EXPORT int GetGameConnectedFriendChatMessageUser()
{
	return SteamHandles()->GetPluginHandle(GameConnectedFriendChatMsgCallback.GetCurrent().m_steamIDUser);
}

/*
@desc Returns the chat message that was sent.
@return A string.
*/
extern "C" DLL_EXPORT char *GetGameConnectedFriendChatMessageText()
{
	return utils::CreateString(GameConnectedFriendChatMsgCallback.GetCurrent().m_Text);
}

/*
@desc Triggered when the user tries to join a lobby from their friends list or from an invite.
The game client should attempt to connect to specified lobby when this is received.
If the game isn't running yet then the game will be automatically launched with the command line parameter +connect_lobby <64-bit lobby Steam ID> instead.
@callback-type list
@callback-getters GetGameLobbyJoinRequestedLobby, GetGameLobbyJoinRequestedFriend
@return 1 when the callback has more responses to process; otherwise 0.
@url https://partner.steamgames.com/doc/api/ISteamFriends#GameLobbyJoinRequested_t
*/
extern "C" DLL_EXPORT int HasGameLobbyJoinRequestedResponse()
{
	return GameLobbyJoinRequestedCallback.HasResponse();
}

/*
@desc Gets the lobby to which the user was invited.
@return A Steam ID handle or 0.
*/
extern "C" DLL_EXPORT int GetGameLobbyJoinRequestedLobby()
{
	return SteamHandles()->GetPluginHandle(GameLobbyJoinRequestedCallback.GetCurrent().m_steamIDLobby);
}

/*
@desc Returns the friend they joined through. This will be invalid if not directly via a friend.
@return A Steam ID handle or 0.
*/
extern "C" DLL_EXPORT int GetGameLobbyJoinRequestedFriend()
{
	return SteamHandles()->GetPluginHandle(GameLobbyJoinRequestedCallback.GetCurrent().m_steamIDFriend);
}

/*
@desc Gets whether the Steam Overlay has been activated or deactivated.
@return 1 when the Steam overlay is active and 0 when it is not.
@url https://partner.steamgames.com/doc/api/ISteamFriends#GameOverlayActivated_t
*/
extern "C" DLL_EXPORT int IsGameOverlayActive()
{
	return GameOverlayActivatedCallback.IsGameOverlayActive();
}

/*
@desc Called when the user tries to join a game from their friends list or after a user accepts an invite by a friend with InviteUserToGame.
@callback-type list
@callback-getters GetGameRichPresenceJoinRequestedConnect, GetGameRichPresenceJoinRequestedFriend
@return 1 when the callback has more responses to process; otherwise 0.
@url https://partner.steamgames.com/doc/api/ISteamFriends#GameRichPresenceJoinRequested_t
*/
extern "C" DLL_EXPORT int HasGameRichPresenceJoinRequestedResponse()
{
	return GameRichPresenceJoinRequestedCallback.HasResponse();
}

/*
@desc The value associated with the "connect" Rich Presence key.
@return A string.
*/
extern "C" DLL_EXPORT char *GetGameRichPresenceJoinRequestedConnect()
{
	return utils::CreateString(GameRichPresenceJoinRequestedCallback.GetCurrent().m_rgchConnect);
}

/*
@desc The friend they joined through. This will be invalid if not directly via a friend.
@return A Steam ID handle.
*/
extern "C" DLL_EXPORT int GetGameRichPresenceJoinRequestedFriend()
{
	return SteamHandles()->GetPluginHandle(GameRichPresenceJoinRequestedCallback.GetCurrent().m_steamIDFriend);
}

/*
@desc Called when the user tries to join a different game server from their friends list.
The game client should attempt to connect to specified server when this is received.
@callback-type list
@callback-getters GetGameServerChangeRequestedPassword, GetGameServerChangeRequestedServer
@return 1 when the callback has more responses to process; otherwise 0.
@url https://partner.steamgames.com/doc/api/ISteamFriends#GameServerChangeRequested_t
*/
extern "C" DLL_EXPORT int HasGameServerChangeRequestedResponse()
{
	return GameServerChangeRequestedCallback.HasResponse();
}

/*
@desc Server password, if any.
@return A string.
*/
extern "C" DLL_EXPORT char *GetGameServerChangeRequestedPassword()
{
	return utils::CreateString(GameServerChangeRequestedCallback.GetCurrent().m_rgchPassword);
}

/*
@desc Server address (e.g. "127.0.0.1:27015", "tf2.valvesoftware.com")
@return A string.
*/
extern "C" DLL_EXPORT char *GetGameServerChangeRequestedServer()
{
	return utils::CreateString(GameServerChangeRequestedCallback.GetCurrent().m_rgchServer);
}

// This appears to be call resultonly.
///*
//@desc Posted when the user has attempted to join a Steam group chat via JoinClanChatRoom
//@return 1 when the callback has more responses to process; otherwise 0.
//@callback-type list
//@callback-getters GetJoinClanChatRoomCompletionResultEnterResponse, GetJoinClanChatRoomCompletionResultUser
//@url https://partner.steamgames.com/doc/api/ISteamFriends#JoinClanChatRoomCompletionResult_t
//*/
//extern "C" DLL_EXPORT int HasJoinClanChatRoomCompletionResultResponse()
//{
//	return JoinClanChatRoomCompletionResultCallback.HasResponse();
//}
//
///*
//@desc The result of the operation.
//@return An EChatRoomEnterResponse value.
//@return-url https://partner.steamgames.com/doc/api/steam_api#EChatRoomEnterResponse
//*/
//extern "C" DLL_EXPORT int GetJoinClanChatRoomCompletionResultEnterResponse()
//{
//	return (int)JoinClanChatRoomCompletionResultCallback.GetCurrent().m_eChatRoomEnterResponse;
//}
//
///*
//@desc The Steam ID of the chat that the user has joined.
//@return A Steeam ID handle.
//*/
//extern "C" DLL_EXPORT int GetJoinClanChatRoomCompletionResultUser()
//{
//	return SteamHandles()->GetPluginHandle(JoinClanChatRoomCompletionResultCallback.GetCurrent().m_steamIDClanChat);
//}

/*
@desc Triggered whenever a friend's status changes.
@callback-type list
@callback-getters	GetPersonaStateChangeFlags, GetPersonaStateChangeSteamID
@return 1 when the callback has more responses to process; otherwise 0.
@url https://partner.steamgames.com/doc/api/ISteamFriends#PersonaStateChange_t
*/
extern "C" DLL_EXPORT int HasPersonaStateChangeResponse()
{
	return PersonaStateChangeCallback.HasResponse();
}

/*
@desc Returns the bit-wise union of EPersonaChange values.
@return The EPersonaChange values of the user whose persona state changed.
@url https://partner.steamgames.com/doc/api/ISteamFriends#PersonaStateChange_t
@url https://partner.steamgames.com/doc/api/ISteamFriends#EPersonaChange
*/
extern "C" DLL_EXPORT int GetPersonaStateChangeFlags()
{
	return PersonaStateChangeCallback.GetCurrent().m_nChangeFlags;
}

/*
@desc Returns the SteamID handle of the user whose persona state changed.
@return The SteamID handle of the user whose persona state changed.
@url https://partner.steamgames.com/doc/api/ISteamFriends#PersonaStateChange_t
*/
extern "C" DLL_EXPORT int GetPersonaStateChangeSteamID()
{
	return SteamHandles()->GetPluginHandle(PersonaStateChangeCallback.GetCurrent().m_ulSteamID);
}

//SetPersonaNameResponse_t - call result for SetPersonaName
