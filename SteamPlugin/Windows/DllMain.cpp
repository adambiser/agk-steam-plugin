/*
Copyright (c) 2017 Adam Biser <adambiser@gmail.com>

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

#define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <ctime>
#include <shellapi.h>
#include <vector>
#include <sstream>
#include "DllMain.h"
#include "SteamPlugin.h"
#include "..\AGKLibraryCommands.h"


/*
NOTE: Cannot use bool as an exported function return type because of AGK2 limitations.  Use int instead.
*/

static SteamPlugin *Steam;

/*
Check to see if the SteamPlugin has been initialized.
If it has not been, return a default value.
*/
#define CheckInitialized(returnValue)	\
	if (!Steam)							\
	{									\
		return returnValue;				\
	}
// Token for passing an empty returnValue into CheckInitialized();
#define NORETURN

/*
Converts a const char* to agk:string.
*/
char *CreateString(const char* text)
{
	if (text)
	{
		int length = (int)strlen(text) + 1;
		char *result = agk::CreateString(length);
		strcpy_s(result, length, text);
		return result;
	}
	char *result = agk::CreateString(1);
	strcpy_s(result, 1, "");
	return result;
}

char *CreateString(std::string text)
{
	return CreateString(text.c_str());
}

std::vector <CSteamID> m_CSteamIDs;

CSteamID GetSteamID(int handle)
{
	// Handles are 1-based!
	if (handle >= 0 && handle < (int)m_CSteamIDs.size())
	{
		return m_CSteamIDs[handle];
	}
	agk::PluginError("Invalid CSteamID handle.");
	return k_steamIDNil;
}

int GetSteamIDHandle(CSteamID steamID)
{
	int index = std::find(m_CSteamIDs.begin(), m_CSteamIDs.end(), steamID) - m_CSteamIDs.begin();
	if (index < (int) m_CSteamIDs.size())
	{
		// Handles are 1-based!
		return index;
	}
	m_CSteamIDs.push_back(steamID);
	return m_CSteamIDs.size() - 1;
}

void ClearSteamIDHandleList()
{
	m_CSteamIDs.clear();
	// Handle 0 is always k_steamIDNil.
	m_CSteamIDs.push_back(k_steamIDNil);
}

bool ParseIP(char *ipv4, uint32 *ip)
{
	int ip1, ip2, ip3, ip4;
	sscanf(ipv4, "%d.%d.%d.%d", &ip1, &ip2, &ip3, &ip4);
	if ((ip1 < 0 || ip1 > 255)
		|| (ip2 < 0 || ip2 > 255)
		|| (ip3 < 0 || ip3 > 255)
		|| (ip4 < 0 || ip4 > 255)
		)
	{
		agk::PluginError("Could not parse IP address.");
		return false;
	}
	*ip = (ip1 << 24) | (ip2 << 16) | (ip3 << 8) | (ip4);
	return true;
}

#define ConvertIPToString(ip) "\"" << ((ip >> 24) & 0xff) << "." << ((ip >> 16) & 0xff) << "." << ((ip >> 8) & 0xff) << "." << ((ip >> 0) & 0xff) << "\""

/*
This method should be called before attempting to do anything else with this plugin.
Returns 1 when the Steam API has been initialized.  Otherwise 0 is returned.
Note that this also calls RequestStats, so calling code does not need to do so.
The result of the RequestStats call has no effect on the return value.
*/
int Init()
{
	if (!Steam)
	{
		Steam = new SteamPlugin();
	}
	ClearSteamIDHandleList();
	if (Steam)
	{
		if (Steam->Init())
		{
			Steam->RequestStats();
			return true;
		}
	}
	return false;
}

/*
Shuts down the plugin and frees memory.
*/
void Shutdown()
{
	ClearSteamIDHandleList();
	if (Steam)
	{
		delete[] Steam;
	}
}

/*
Gets whether the Steam API has been initialized.
When it has, this method returns 1.  Otherwise 0 is returned.

*/
int SteamInitialized()
{
	CheckInitialized(false);
	return Steam->SteamInitialized();
}

int RestartAppIfNecessary(int ownAppID)
{
	bool doDelete = false;
	bool result = false;
	if (!Steam)
	{
		doDelete = true;
		Steam = new SteamPlugin();
	}
	if (Steam)
	{
		result = Steam->RestartAppIfNecessary(ownAppID);
	}
	if (doDelete)
	{
		delete[] Steam;
	}
	return result;
}

/*
Returns the AppID or 0 if the Steam API has not been not initialized or the AppID cannot be found.
*/
int GetAppID()
{
	CheckInitialized(0);
	return Steam->GetAppID();
}

char *GetAppName(int appID)
{
	if (Steam)
	{
		char name[256];
		if (Steam->GetAppName(appID, name, sizeof(name)))
		{
			return CreateString(name);
		}
	}
	return CreateString("NULL");
}

/*
Returns 1 when logged into Steam.  Otherise returns 0.
*/
int LoggedOn()
{
	CheckInitialized(false);
	return Steam->LoggedOn();
}

int IsSteamIDValid(int hSteamID)
{
	CheckInitialized(false);
	CSteamID steamID = GetSteamID(hSteamID);
	return steamID.IsValid();
}

int GetHandleFromSteamID64(char *steamID64)
{
	CheckInitialized(0);
	uint64 id = _atoi64(steamID64);
	return GetSteamIDHandle(CSteamID(id));
}

/*
Call every frame to allow the Steam API callbacks to process.
Should be used in conjunction with RequestStats and StoreStats to ensure the callbacks finish.
*/
void RunCallbacks()
{
	CheckInitialized(NORETURN);
	Steam->RunCallbacks();
}

char *GetCommandLineArgsJSON()
{
	std::ostringstream json;
	json << "[";
	LPWSTR *szArglist;
	int nArgs;
	szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);
	if (NULL != szArglist)
	{
		char arg[MAX_PATH];
		for (int i = 0; i < nArgs; i++)
		{
			if (i > 0)
			{
				json << ",";
			}
			wcstombs(arg, szArglist[i], MAX_PATH);
			json << "\"" << arg << "\"";
		}
		// Free memory.
		LocalFree(szArglist);
	}
	json << "]";
	return CreateString(json.str());
}

int IsGameOverlayActive()
{
	CheckInitialized(false);
	return Steam->IsGameOverlayActive();
}

void ActivateGameOverlay(const char *pchDialog)
{
	CheckInitialized(NORETURN);
	Steam->ActivateGameOverlay(pchDialog);
}

void ActivateGameOverlayInviteDialog(int hLobbySteamID)
{
	CheckInitialized(NORETURN);
	Steam->ActivateGameOverlayInviteDialog(GetSteamID(hLobbySteamID));
}

void ActivateGameOverlayToStore(int appID, int flag)
{
	CheckInitialized(NORETURN);
	Steam->ActivateGameOverlayToStore(appID, (EOverlayToStoreFlag)flag);
}

void ActivateGameOverlayToUser(const char *pchDialog, int hSteamID)
{
	CheckInitialized(NORETURN);
	Steam->ActivateGameOverlayToUser(pchDialog, GetSteamID(hSteamID));
}

void ActivateGameOverlayToWebPage(const char *url)
{
	CheckInitialized(NORETURN);
	Steam->ActivateGameOverlayToWebPage(url);
}

char *GetPersonaName()
{
	CheckInitialized(CreateString(NULL));
	return CreateString(Steam->GetPersonaName());
}

int GetSteamID()
{
	CheckInitialized(0);
	return GetSteamIDHandle(Steam->GetSteamID());
}

char *GetSteamID64(int hUserSteamID)
{
	CheckInitialized(CreateString(NULL));
	char id64[21]; // Max value is 18,446,744,073,709,551,615
	_i64toa(GetSteamID(hUserSteamID).ConvertToUint64(), id64, 10);
	return CreateString(id64);
}

int HasPersonaStateChanged()
{
	CheckInitialized(false);
	return Steam->HasPersonaStateChanged();
}

int GetPersonaStateChangedUser()
{
	CheckInitialized(0);
	return GetSteamIDHandle(Steam->GetPersonaStateChangedUser());
}

int GetPersonaStateChangedFlags()
{
	CheckInitialized(0);
	return Steam->GetPersonaStateChangedFlags();
}

int RequestUserInformation(int hUserSteamID, int requireNameOnly)
{
	CheckInitialized(false);
	return Steam->RequestUserInformation(GetSteamID(hUserSteamID), requireNameOnly != 0);
}

int HasAvatarImageLoaded()
{
	CheckInitialized(false);
	return Steam->HasAvatarImageLoaded();
}

int GetAvatarImageLoadedUser()
{
	CheckInitialized(0);
	return GetSteamIDHandle(Steam->GetAvatarImageLoadedUser());
}

int GetFriendAvatar(int hUserSteamID, int size)
{
	CheckInitialized(0);
	return Steam->GetFriendAvatar(GetSteamID(hUserSteamID), (EAvatarSize)size);
}

int GetFriendCount(int friendFlags)
{
	CheckInitialized(-1);
	return Steam->GetFriendCount((EFriendFlags)friendFlags);
}

int GetFriendByIndex(int index, int friendFlags)
{
	CheckInitialized(0);
	return GetSteamIDHandle(Steam->GetFriendByIndex(index, (EFriendFlags)friendFlags));
}

char *GetFriendListJSON(int friendFlags)
{
	std::ostringstream json;
	json << "[";
	if (Steam)
	{
		int friendCount = Steam->GetFriendCount((EFriendFlags)friendFlags);
		for (int x = 0; x < friendCount; x++)
		{
			if (x > 0)
			{
				json << ",";
			}
			json << GetSteamIDHandle(Steam->GetFriendByIndex(x, (EFriendFlags)friendFlags));
		}
	}
	json << "]";
	return CreateString(json.str());
}

char *GetFriendGamePlayedJSON(int hUserSteamID)
{
	std::ostringstream json;
	json << "{";
	if (Steam)
	{
		FriendGameInfo_t pFriendGameInfo;
		bool ingame = Steam->GetFriendGamePlayed(GetSteamID(hUserSteamID), &pFriendGameInfo);
		json << "\"InGame\": " << (int)ingame;
		if (ingame)
		{
			json << ",\"GameAppID\": " << pFriendGameInfo.m_gameID.AppID();
			json << ",\"GameIP\": " << pFriendGameInfo.m_unGameIP;
			json << ",\"GamePort\": " << pFriendGameInfo.m_usGamePort;
			json << ",\"QueryPort\": " << pFriendGameInfo.m_usQueryPort;
			json << ",\"SteamIDLobby\": " << GetSteamIDHandle(pFriendGameInfo.m_steamIDLobby);
		}
	}
	json << "}";
	return CreateString(json.str());
}

char *GetFriendPersonaName(int hUserSteamID)
{
	CheckInitialized(CreateString(NULL));
	return CreateString(Steam->GetFriendPersonaName(GetSteamID(hUserSteamID)));
}

int GetFriendPersonaState(int hUserSteamID)
{
	CheckInitialized(-1);
	return Steam->GetFriendPersonaState(GetSteamID(hUserSteamID));
}

int GetFriendRelationship(int hUserSteamID)
{
	CheckInitialized(-1);
	return Steam->GetFriendRelationship(GetSteamID(hUserSteamID));
}

int GetFriendSteamLevel(int hUserSteamID)
{
	CheckInitialized(0);
	return Steam->GetFriendSteamLevel(GetSteamID(hUserSteamID));
}

char *GetPlayerNickname(int hUserSteamID)
{
	CheckInitialized(CreateString(NULL));
	return CreateString(Steam->GetPlayerNickname(GetSteamID(hUserSteamID)));
}

int HasFriend(int hUserSteamID, int iFriendFlags)
{
	CheckInitialized(false);
	return Steam->HasFriend(GetSteamID(hUserSteamID), (EFriendFlags)iFriendFlags);
}

int GetFriendsGroupCount()
{
	CheckInitialized(0);
	return Steam->GetFriendsGroupCount();
}

int GetFriendsGroupIDByIndex(int index)
{
	CheckInitialized(k_FriendsGroupID_Invalid);
	return Steam->GetFriendsGroupIDByIndex(index);
}

int GetFriendsGroupMembersCount(int friendsGroupID)
{
	CheckInitialized(0);
	return Steam->GetFriendsGroupMembersCount(friendsGroupID);
}

char *GetFriendsGroupMembersListJSON(int friendsGroupID) // return a json array of SteamID handles
{
	std::ostringstream json;
	json << "[";
	if (Steam)
	{
		int memberCount = Steam->GetFriendsGroupMembersCount(friendsGroupID);
		if (memberCount > 0)
		{
			std::vector<CSteamID> friends(memberCount);
			Steam->GetFriendsGroupMembersList(friendsGroupID, friends.data(), memberCount);
			for (int x = 0; x < memberCount; x++)
			{
				if (x > 0)
				{
					json << ",";
				}
				json << GetSteamIDHandle(friends[x]);
			}
		}
	}
	json << "]";
	return CreateString(json.str());
}

char *GetFriendsGroupName(int friendsGroupID)
{
	CheckInitialized(CreateString(NULL));
	return CreateString(Steam->GetFriendsGroupName(friendsGroupID));
}

int LoadImageFromHandle(int hImage)
{
	CheckInitialized(0);
	return Steam->LoadImageFromHandle(hImage);
}

void LoadImageIDFromHandle(int imageID, int hImage)
{
	CheckInitialized(NORETURN);
	Steam->LoadImageFromHandle(imageID, hImage);
}

/*
Requests the user stats.
Returns 1 if the process succeeds.  Otherwise, returns 0.
Use StatsInitialized() to determine when user stats are initialized.
*/
int RequestStats()
{
	CheckInitialized(false);
	return Steam->RequestStats();
}

int GetRequestStatsCallbackState()
{
	CheckInitialized(STATE_CLIENT_ERROR);
	return Steam->GetRequestStatsCallbackState();
}

/*
Returns 1 when user stats are initialized.  Otherwise, returns 0.
*/
int StatsInitialized()
{
	CheckInitialized(false);
	return Steam->StatsInitialized();
}

/*
Stores the user stats and achievements in Steam.
Returns 1 if the process succeeds.  Otherwise returns 0.
Use StatsStored() to determine when user stats have been stored.
*/
int StoreStats()
{
	CheckInitialized(false);
	return Steam->StoreStats();
}

/*
Resets all status and optionally the achievements.
Should generally only be used while testing.
*/
int ResetAllStats(int bAchievementsToo)
{
	CheckInitialized(false);
	return Steam->ResetAllStats(bAchievementsToo != 0);
}

int GetStoreStatsCallbackState()
{
	CheckInitialized(STATE_CLIENT_ERROR);
	return Steam->GetStoreStatsCallbackState();
}

/*
Returns 1 when user stats have been stored since the last StoreStats call.  Otherwise, returns 0.
*/
int StatsStored()
{
	CheckInitialized(false);
	return Steam->StatsStored();
}

/*
Returns 1 when an achievement has been stored since the last StoreStats call.  Otherwise, returns 0.
*/
int AchievementStored()
{
	CheckInitialized(false);
	return Steam->AchievementStored();
}

/*
Returns the number of achievements for the app or 0 if user stats have not been initialized.
Use StatsInitialized() to determine when user stats are initialized before calling this method.
*/
int GetNumAchievements()
{
	CheckInitialized(0);
	return Steam->GetNumAchievements();
}

/*
Gets the achievement ID for the given achievement index.
If user stats have not been initialized, an empty string is returned.
Use StatsInitialized() to determine when user stats are initialized before calling this method.
*/
char *GetAchievementID(int index)
{
	CheckInitialized(CreateString(NULL));
	return CreateString(Steam->GetAchievementID(index));
}

char *GetAchievementDisplayName(const char *pchName)
{
	CheckInitialized(CreateString(NULL));
	return CreateString(Steam->GetAchievementDisplayAttribute(pchName, "name"));
}

char *GetAchievementDisplayDesc(const char *pchName)
{
	CheckInitialized(CreateString(NULL));
	return CreateString(Steam->GetAchievementDisplayAttribute(pchName, "desc"));
}

int GetAchievementDisplayHidden(const char *pchName)
{
	CheckInitialized(false);
	return (strcmp(Steam->GetAchievementDisplayAttribute(pchName, "hidden"), "1") == 0);
}

int GetAchievementIcon(const char *pchName)
{
	CheckInitialized(0);
	return Steam->GetAchievementIcon(pchName);
}

/*
Returns 1 if the user has achieved this achievement.
If user stats have not been initialized 0 is returned.
Use StatsInitialized() to determine when user stats are initialized before calling this method.
If the call fails, 0 is returned and the error is reported to the AGK interpreter.
*/
int GetAchievement(const char *pchName)
{
	CheckInitialized(false);
	bool result = false;
	if (Steam->GetAchievement(pchName, &result))
	{
		return result;
	}
	agk::PluginError("GetAchievement failed.");
	return false;
}

int GetAchievementUnlockTime(const char *pchName)
{
	CheckInitialized(0);
	bool pbAchieved;
	uint32 punUnlockTime;
	if (SteamUserStats()->GetAchievementAndUnlockTime(pchName, &pbAchieved, &punUnlockTime))
	{
		if (pbAchieved)
		{
			return punUnlockTime;
		}
	}
	return 0;
}



/*
Marks an achievement as achieved.
Returns 1 if the process succeeds.  Otherwise, returns 0.
Use StatsInitialized() to determine when user stats are initialized before calling this method.
*/
int SetAchievement(const char *pchName)
{
	CheckInitialized(false);
	return Steam->SetAchievement(pchName);
}

/*
Raises a notification about achievemnt progress for progress stat achievements.
The notification only shows when current progress is less than the max.
SetStat still needs to be used to set the progress stat value.
*/
int IndicateAchievementProgress(const char *pchName, int nCurProgress, int nMaxProgress)
{
	CheckInitialized(false);
	return Steam->IndicateAchievementProgress(pchName, nCurProgress, nMaxProgress);
}

/*
Clears an achievement.
Returns 1 if the process succeeds.  Otherwise, returns 0.
Use StatsInitialized() to determine when user stats are initialized before calling this method.
*/
int ClearAchievement(const char *pchName)
{
	CheckInitialized(false);
	return Steam->ClearAchievement(pchName);
}

/*
Gets an integer user stat value.
If there is a problem, 0 is returned and the error is reported to the AGK interpreter.
*/
int GetStatInt(const char *pchName)
{
	CheckInitialized(0);
	int result = 0;
	if (Steam->GetStat(pchName, &result)) {
		return result;
	}
	agk::PluginError("GetStat failed.");
	return 0;
}

/*
Gets a float user stat value.
If there is a problem, 0 is returned and the error is reported to the AGK interpreter.
*/
float GetStatFloat(const char *pchName)
{
	CheckInitialized(0.0);
	float result = 0.0;
	if (Steam->GetStat(pchName, &result)) {
		return result;
	}
	agk::PluginError("GetStat failed.");
	return 0.0;
}

/*
Sets an integer user stat value.
Returns 1 if the call succeeds.  Otherwise returns 0.
*/
int SetStatInt(const char *pchName, int nData)
{
	CheckInitialized(false);
	return Steam->SetStat(pchName, nData);
}

/*
Sets a float user stat value.
Returns 1 if the call succeeds.  Otherwise returns 0.
*/
int SetStatFloat(const char *pchName, float fData)
{
	CheckInitialized(false);
	return Steam->SetStat(pchName, fData);
}

/*
Updates an average rate user stat.  Steam takes care of the averaging.  Use GetStatFloat to get the result.
Returns 1 if the call succeeds.  Otherwise returns 0.
*/
int UpdateAvgRateStat(const char *pchName, float flCountThisSession, float dSessionLength)
{
	CheckInitialized(false);
	return Steam->UpdateAvgRateStat(pchName, flCountThisSession, (double) dSessionLength);
}

int FindLeaderboard(const char *pchLeaderboardName)
{
	CheckInitialized(false);
	return Steam->FindLeaderboard(pchLeaderboardName);
}

int GetFindLeaderboardCallbackState()
{
	CheckInitialized(STATE_CLIENT_ERROR);
	return Steam->GetFindLeaderboardCallbackState();
}

int GetLeaderboardHandle()
{
	CheckInitialized(0);
	return (int) Steam->GetLeaderboardHandle();
}

char *GetLeaderboardName(int hLeaderboard)
{
	CheckInitialized(CreateString(NULL));
	return CreateString(Steam->GetLeaderboardName(hLeaderboard));
}

int GetLeaderboardEntryCount(int hLeaderboard)
{
	CheckInitialized(0);
	return Steam->GetLeaderboardEntryCount(hLeaderboard);
}

int GetLeaderboardDisplayType(int hLeaderboard)
{
	CheckInitialized(0);
	return Steam->GetLeaderboardDisplayType(hLeaderboard);
}

int GetLeaderboardSortMethod(int hLeaderboard)
{
	CheckInitialized(0);
	return Steam->GetLeaderboardSortMethod(hLeaderboard);
}

int UploadLeaderboardScore(int hLeaderboard, int score)
{
	CheckInitialized(false);
	return Steam->UploadLeaderboardScore((SteamLeaderboard_t)hLeaderboard, k_ELeaderboardUploadScoreMethodKeepBest, score);
}

int UploadLeaderboardScoreForceUpdate(int hLeaderboard, int score)
{
	CheckInitialized(false);
	return Steam->UploadLeaderboardScore((SteamLeaderboard_t)hLeaderboard, k_ELeaderboardUploadScoreMethodForceUpdate, score);
}

int GetUploadLeaderboardScoreCallbackState()
{
	CheckInitialized(STATE_CLIENT_ERROR);
	return Steam->GetUploadLeaderboardScoreCallbackState();
}

int LeaderboardScoreStored()
{
	CheckInitialized(false);
	return Steam->LeaderboardScoreStored();
}

int LeaderboardScoreChanged()
{
	CheckInitialized(false);
	return Steam->LeaderboardScoreChanged();
}

int GetLeaderboardUploadedScore()
{
	CheckInitialized(0);
	return Steam->GetLeaderboardUploadedScore();
}

int GetLeaderboardGlobalRankNew()
{
	CheckInitialized(0);
	return Steam->GetLeaderboardGlobalRankNew();
}

int GetLeaderboardGlobalRankPrevious()
{
	CheckInitialized(0);
	return Steam->GetLeaderboardGlobalRankPrevious();
}

int DownloadLeaderboardEntries(int hLeaderboard, int eLeaderboardDataRequest, int nRangeStart, int nRangeEnd)
{
	CheckInitialized(false);
	return Steam->DownloadLeaderboardEntries(hLeaderboard, (ELeaderboardDataRequest) eLeaderboardDataRequest, nRangeStart, nRangeEnd);
}

int GetDownloadLeaderboardEntriesCallbackState()
{
	CheckInitialized(STATE_CLIENT_ERROR);
	return Steam->GetDownloadLeaderboardEntriesCallbackState();
}

int GetDownloadedLeaderboardEntryCount()
{
	CheckInitialized(0);
	return Steam->GetDownloadedLeaderboardEntryCount();
}

int GetDownloadedLeaderboardEntryGlobalRank(int index)
{
	CheckInitialized(0);
	return Steam->GetDownloadedLeaderboardEntryGlobalRank(index);
}

int GetDownloadedLeaderboardEntryScore(int index)
{
	CheckInitialized(0);
	return Steam->GetDownloadedLeaderboardEntryScore(index);
}

int GetDownloadedLeaderboardEntryUser(int index)
{
	CheckInitialized(0);
	return GetSteamIDHandle(Steam->GetDownloadedLeaderboardEntryUser(index));
}

void AddRequestLobbyListDistanceFilter(int eLobbyDistanceFilter)
{
	CheckInitialized(NORETURN);
	return Steam->AddRequestLobbyListDistanceFilter((ELobbyDistanceFilter)eLobbyDistanceFilter);
}

void AddRequestLobbyListFilterSlotsAvailable(int slotsAvailable)
{
	CheckInitialized(NORETURN);
	return Steam->AddRequestLobbyListFilterSlotsAvailable(slotsAvailable);
}

void AddRequestLobbyListNearValueFilter(char *pchKeyToMatch, int valueToBeCloseTo)
{
	CheckInitialized(NORETURN);
	return Steam->AddRequestLobbyListNearValueFilter(pchKeyToMatch, valueToBeCloseTo);
}

void AddRequestLobbyListNumericalFilter(char *pchKeyToMatch, int valueToMatch, int eComparisonType)
{
	CheckInitialized(NORETURN);
	return Steam->AddRequestLobbyListNumericalFilter(pchKeyToMatch, valueToMatch, (ELobbyComparison)eComparisonType);
}

void AddRequestLobbyListResultCountFilter(int maxResults)
{
	CheckInitialized(NORETURN);
	return Steam->AddRequestLobbyListResultCountFilter(maxResults);
}

void AddRequestLobbyListStringFilter(char *pchKeyToMatch, char *pchValueToMatch, int eComparisonType)
{
	CheckInitialized(NORETURN);
	return Steam->AddRequestLobbyListStringFilter(pchKeyToMatch, pchValueToMatch, (ELobbyComparison)eComparisonType);
}

int RequestLobbyList()
{
	CheckInitialized(0);
	return Steam->RequestLobbyList();
}

int GetLobbyMatchListCallbackState()
{
	CheckInitialized(STATE_CLIENT_ERROR);
	return Steam->GetLobbyMatchListCallbackState();
}

int GetLobbyMatchListCount() {
	CheckInitialized(0);
	return Steam->GetLobbyMatchListCount();
}

int GetLobbyByIndex(int index)
{
	CheckInitialized(0);
	return GetSteamIDHandle(Steam->GetLobbyByIndex(index));
}

int CreateLobby(int eLobbyType, int maxMembers)
{
	CheckInitialized(false);
	return Steam->CreateLobby((ELobbyType)eLobbyType, maxMembers);
}

int GetLobbyCreateCallbackState()
{
	CheckInitialized(0);
	return Steam->GetLobbyCreateCallbackState();
}

int GetLobbyCreatedID()
{
	CheckInitialized(0);
	return GetSteamIDHandle(Steam->GetLobbyCreatedID());
}

int GetLobbyCreatedResult()
{
	CheckInitialized(0);
	return Steam->GetLobbyCreatedResult();
}

int SetLinkedLobby(int hLobbySteamID, int hLobbyDependentSteamID)
{
	CheckInitialized(false);
	return Steam->SetLinkedLobby(GetSteamID(hLobbySteamID), GetSteamID(hLobbyDependentSteamID));
}

int SetLobbyJoinable(int hLobbySteamID, bool lobbyJoinable)
{
	CheckInitialized(false);
	return Steam->SetLobbyJoinable(GetSteamID(hLobbySteamID), lobbyJoinable);
}

int SetLobbyType(int hLobbySteamID, int eLobbyType)
{
	CheckInitialized(false);
	return Steam->SetLobbyType(GetSteamID(hLobbySteamID), (ELobbyType)eLobbyType);
}

int JoinLobby(int hLobbySteamID)
{
	CheckInitialized(false);
	return Steam->JoinLobby(GetSteamID(hLobbySteamID));
}

int GetLobbyEnterCallbackState()
{
	CheckInitialized(STATE_CLIENT_ERROR);
	return Steam->GetLobbyEnterCallbackState();
}

int GetLobbyEnterID()
{
	CheckInitialized(0);
	return GetSteamIDHandle(Steam->GetLobbyEnterID());
}

int GetLobbyEnterBlocked()
{
	CheckInitialized(false);
	return Steam->GetLobbyEnterBlocked();
}

int GetLobbyEnterResponse()
{
	CheckInitialized(0);
	return Steam->GetLobbyEnterResponse();
}

void LeaveLobby(int hLobbySteamID)
{
	CheckInitialized(NORETURN);
	Steam->LeaveLobby(GetSteamID(hLobbySteamID));
}

// Lobby methods: Game server
char *GetLobbyGameServerJSON(int hLobbySteamID)
{
	std::ostringstream json;
	json << "{";
	if (Steam)
	{
		uint32 unGameServerIP;
		uint16 unGameServerPort;
		CSteamID steamIDGameServer;
		if (Steam->GetLobbyGameServer(GetSteamID(hLobbySteamID), &unGameServerIP, &unGameServerPort, &steamIDGameServer))
		{
			json << "\"hLobby\": " << hLobbySteamID; // Add this for completeness.
			json << ",\"IP\": " << ConvertIPToString(unGameServerIP);
			json << ",\"Port\": " << unGameServerPort;
			json << ",\"hGameServer\": " << GetSteamIDHandle(steamIDGameServer);
		}
	}
	json << "}";
	return CreateString(json.str());
}

int SetLobbyGameServer(int hLobbySteamID, char *gameServerIP, int gameServerPort, int hGameServerSteamID)
{
	CheckInitialized(false);
	if (gameServerPort < 0 || gameServerPort > 0xffff)
	{
		agk::PluginError("SetLobbyGameServer: Invalid game server port.");
		return false;
	}
	uint32 ip;
	if (!ParseIP(gameServerIP, &ip))
	{
		return false;
	}
	Steam->SetLobbyGameServer(GetSteamID(hLobbySteamID), ip, gameServerPort, GetSteamID(hGameServerSteamID));
	return true;
}

// Lobby methods: Data
char *GetLobbyData(int hLobbySteamID, char *key)
{
	CheckInitialized(CreateString(NULL));
	return CreateString(Steam->GetLobbyData(GetSteamID(hLobbySteamID), key));
}

int GetLobbyDataCount(int hLobbySteamID)
{
	CheckInitialized(0);
	return Steam->GetLobbyDataCount(GetSteamID(hLobbySteamID));
}

char *GetLobbyDataByIndexJSON(int hLobbySteamID, int index)
{
	if (Steam)
	{
		char key[k_nMaxLobbyKeyLength];
		char value[k_cubChatMetadataMax];
		if (Steam->GetLobbyDataByIndex(GetSteamID(hLobbySteamID), index, key, k_nMaxLobbyKeyLength, value, k_cubChatMetadataMax))
		{
			std::string json = "{\"Key\": \"";
			json.append(key);
			json.append("\", \"Value\": \"");
			json.append(value);
			json.append("\"}");
			return CreateString(json);
		}
	}
	return CreateString(NULL);
}

char *GetLobbyDataJSON(int hLobbySteamID)
{
	CheckInitialized(CreateString(NULL));
	CSteamID steamIDLobby = GetSteamID(hLobbySteamID);
	std::string json = "[";
	char key[k_nMaxLobbyKeyLength];
	char value[k_cubChatMetadataMax];
	for (int index = 0; index < Steam->GetLobbyDataCount(steamIDLobby); index++)
	{
		if (Steam->GetLobbyDataByIndex(steamIDLobby, index, key, k_nMaxLobbyKeyLength, value, k_cubChatMetadataMax))
		{
			if (index > 0)
			{
				json.append(", ");
			}
			json.append("{\"Key\": \"");
			json.append(key);
			json.append("\", \"Value\": \"");
			json.append(value);
			json.append("\"}");
		}
	}
	json.append("]");
	return CreateString(json);
}

void SetLobbyData(int hLobbySteamID, char *key, char *value)
{
	CheckInitialized(NORETURN);
	Steam->SetLobbyData(GetSteamID(hLobbySteamID), key, value);
}

int DeleteLobbyData(int hLobbySteamID, char *key)
{
	CheckInitialized(false);
	return Steam->DeleteLobbyData(GetSteamID(hLobbySteamID), key);
}

int RequestLobbyData(int hLobbySteamID)
{
	CheckInitialized(false);
	return Steam->RequestLobbyData(GetSteamID(hLobbySteamID));
}

int HasLobbyDataUpdated()
{
	CheckInitialized(false);
	return Steam->HasLobbyDataUpdated();
}

int GetLobbyDataUpdatedLobby()
{
	CheckInitialized(0);
	return GetSteamIDHandle(Steam->GetLobbyDataUpdatedLobby());
}

int GetLobbyDataUpdatedID()
{
	CheckInitialized(0);
	return GetSteamIDHandle(Steam->GetLobbyDataUpdatedID());
}

char *GetLobbyMemberData(int hLobbySteamID, int hUserSteamID, char *key)
{
	CheckInitialized(CreateString(NULL));
	return CreateString(Steam->GetLobbyMemberData(GetSteamID(hLobbySteamID), GetSteamID(hUserSteamID), key));
}

void SetLobbyMemberData(int hLobbySteamID, char *key, char *value)
{
	CheckInitialized(NORETURN);
	return Steam->SetLobbyMemberData(GetSteamID(hLobbySteamID), key, value);
}

int GetLobbyOwner(int hLobbySteamID)
{
	CheckInitialized(0);
	return GetSteamIDHandle(Steam->GetLobbyOwner(GetSteamID(hLobbySteamID)));
}

int SetLobbyOwner(int hLobbySteamID, int hNewOwnerSteamID)
{
	CheckInitialized(0);
	return Steam->SetLobbyOwner(GetSteamID(hLobbySteamID), GetSteamID(hNewOwnerSteamID));
}

int GetLobbyMemberLimit(int hLobbySteamID)
{
	CheckInitialized(0);
	return Steam->GetLobbyMemberLimit(GetSteamID(hLobbySteamID));
}

int SetLobbyMemberLimit(int hLobbySteamID, int cMaxMembers)
{
	CheckInitialized(0);
	return Steam->SetLobbyMemberLimit(GetSteamID(hLobbySteamID), cMaxMembers);
}

int GetNumLobbyMembers(int hLobbySteamID)
{
	CheckInitialized(0);
	return Steam->GetNumLobbyMembers(GetSteamID(hLobbySteamID));
}

int GetLobbyMemberByIndex(int hLobbySteamID, int index)
{
	CheckInitialized(0);
	return GetSteamIDHandle(Steam->GetLobbyMemberByIndex(GetSteamID(hLobbySteamID), index));
}

int HasLobbyChatUpdate()
{
	CheckInitialized(false);
	return Steam->HasLobbyChatUpdate();
}

int GetLobbyChatUpdateUserChanged()
{
	CheckInitialized(0);
	return GetSteamIDHandle(Steam->GetLobbyChatUpdateUserChanged());
}

int GetLobbyChatUpdateUserState()
{
	CheckInitialized(0);
	return Steam->GetLobbyChatUpdateUserState();
}

int GetLobbyChatUpdateUserMakingChange()
{
	CheckInitialized(0);
	return GetSteamIDHandle(Steam->GetLobbyChatUpdateUserMakingChange());
}

int HasLobbyChatMessage()
{
	CheckInitialized(false);
	return Steam->HasLobbyChatMessage();
}

int GetLobbyChatMessageUser()
{
	CheckInitialized(0);
	return GetSteamIDHandle(Steam->GetLobbyChatMessageUser());
}

char *GetLobbyChatMessageText()
{
	CheckInitialized(CreateString(NULL));
	char msg[4096];
	Steam->GetLobbyChatMessageText(msg);
	return CreateString(msg);
}

int SendLobbyChatMessage(int hLobbySteamID, char *msg)
{
	CheckInitialized(0);
	return Steam->SendLobbyChatMessage(GetSteamID(hLobbySteamID), msg);
}

// Lobby methods: Favorite games
int AddFavoriteGame(int appID, char *ipv4, int connectPort, int queryPort, int flags) //, int time32LastPlayedOnServer)
{
	CheckInitialized(0);
	if (connectPort < 0 || connectPort > 0xffff)
	{
		agk::PluginError("AddFavoriteGame: Invalid connection port.");
		return 0;
	}
	if (queryPort < 0 || queryPort > 0xffff)
	{
		agk::PluginError("AddFavoriteGame: Invalid query port.");
		return 0;
	}
	uint32 ip;
	if (!ParseIP(ipv4, &ip))
	{
		return 0;
	}
	std::time_t now = std::time(0);
	//agk::Message(agk::Str((int)now));
	return Steam->AddFavoriteGame(appID, (uint32)ip, connectPort, queryPort, flags, (uint32)now);
}

int GetFavoriteGameCount()
{
	CheckInitialized(0);
	return Steam->GetFavoriteGameCount();
}

char *GetFavoriteGameJSON(int index)
{
	std::ostringstream json;
	json << "{";
	if (Steam)
	{
		AppId_t nAppID;
		uint32 nIP;
		uint16 nConnPort;
		uint16 nQueryPort;
		uint32 unFlags;
		uint32 rTime32LastPlayedOnServer;
		if (Steam->GetFavoriteGame(index, &nAppID, &nIP, &nConnPort, &nQueryPort, &unFlags, &rTime32LastPlayedOnServer))
		{
			json << "\"AppID\": " << nAppID;
			json << ",\"IP\": " << ConvertIPToString(nIP);
			json << ",\"ConnectPort\": " << nConnPort;
			json << ",\"QueryPort\": " << nQueryPort;
			json << ",\"Flags\": " << unFlags;
			json << ",\"TimeLastPlayedOnServer\": " << (int)rTime32LastPlayedOnServer;
		}
	}
	json << "}";
	return CreateString(json.str());
}

int RemoveFavoriteGame(int appID, char *ipv4, int connectPort, int queryPort, int flags)
{
	CheckInitialized(0);
	if (connectPort < 0 || connectPort > 0xffff)
	{
		agk::PluginError("RemoveFavoriteGame: Invalid connection port.");
		return 0;
	}
	if (queryPort < 0 || queryPort > 0xffff)
	{
		agk::PluginError("RemoveFavoriteGame: Invalid query port.");
		return 0;
	}
	uint32 ip;
	if (!ParseIP(ipv4, &ip))
	{
		return 0;
	}
	return Steam->RemoveFavoriteGame(appID, ip, connectPort, queryPort, flags);
}
