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
#include <vector>
#include "DllMain.h"
#include "SteamPlugin.h"
#include "..\AGKLibraryCommands.h"

/*
NOTE: Cannot use bool as an exported function return type because of AGK2 limitations.  Use int instead.
*/

static SteamPlugin *Steam;

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
	if (Steam)
	{
		return Steam->SteamInitialized();
	}
	return false;
}

int RestartAppIfNecessary(int unOwnAppID)
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
		result = Steam->RestartAppIfNecessary(unOwnAppID);
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
	if (Steam)
	{
		return Steam->GetAppID();
	}
	return 0;
}

/*
Returns 1 when logged into Steam.  Otherise returns 0.
*/
int LoggedOn()
{
	if (Steam)
	{
		return Steam->LoggedOn();
	}
	return false;
}

/*
Call every frame to allow the Steam API callbacks to process.
Should be used in conjunction with RequestStats and StoreStats to ensure the callbacks finish.
*/
void RunCallbacks()
{
	if (Steam)
	{
		Steam->RunCallbacks();
	}
}

void ActivateGameOverlay(const char *pchDialog)
{
	if (Steam)
	{
		Steam->ActivateGameOverlay(pchDialog);
	}
}

int GetAvatar(int size)
{
	if (Steam)
	{
		return Steam->GetAvatar((EAvatarSize)size);
	}
	return 0;
}

char *GetPersonaName()
{
	if (Steam)
	{
		return CreateString(Steam->GetPersonaName());
	}
	return CreateString(0);
}

int GetFriendAvatar(int hUserSteamID, int size)
{
	if (Steam)
	{
		return Steam->GetFriendAvatar(GetSteamID(hUserSteamID), (EAvatarSize)size);
	}
	return 0;
}

char *GetFriendPersonaName(int hUserSteamID)
{
	if (Steam)
	{
		return CreateString(Steam->GetFriendPersonaName(GetSteamID(hUserSteamID)));
	}
	return CreateString(NULL);
}

int LoadImageFromHandle(int hImage)
{
	if (Steam)
	{
		return Steam->LoadImageFromHandle(hImage);
	}
	return 0;
}

void LoadImageIDFromHandle(int imageID, int hImage)
{
	if (Steam)
	{
		Steam->LoadImageFromHandle(imageID, hImage);
	}
}

/*
Requests the user stats.
Returns 1 if the process succeeds.  Otherwise, returns 0.
Use StatsInitialized() to determine when user stats are initialized.
*/
int RequestStats()
{
	if (Steam)
	{
		return Steam->RequestStats();
	}
	return false;
}

int GetRequestStatsCallbackState()
{
	if (Steam)
	{
		return Steam->GetRequestStatsCallbackState();
	}
	return STATE_CLIENT_ERROR;
}

/*
Returns 1 when user stats are initialized.  Otherwise, returns 0.
*/
int StatsInitialized()
{
	if (Steam)
	{
		return Steam->StatsInitialized();
	}
	return false;
}

/*
Stores the user stats and achievements in Steam.
Returns 1 if the process succeeds.  Otherwise returns 0.
Use StatsStored() to determine when user stats have been stored.
*/
int StoreStats()
{
	if (Steam)
	{
		return Steam->StoreStats();
	}
	return false;
}

/*
Resets all status and optionally the achievements.
Should generally only be used while testing.
*/
int ResetAllStats(int bAchievementsToo)
{
	if (Steam)
	{
		return Steam->ResetAllStats(bAchievementsToo != 0);
	}
	return false;
}

int GetStoreStatsCallbackState()
{
	if (Steam)
	{
		return Steam->GetStoreStatsCallbackState();
	}
	return STATE_CLIENT_ERROR;
}

/*
Returns 1 when user stats have been stored since the last StoreStats call.  Otherwise, returns 0.
*/
int StatsStored()
{
	if (Steam)
	{
		return Steam->StatsStored();
	}
	return false;
}

/*
Returns 1 when an achievement has been stored since the last StoreStats call.  Otherwise, returns 0.
*/
int AchievementStored()
{
	if (Steam)
	{
		return Steam->AchievementStored();
	}
	return false;
}

/*
Returns the number of achievements for the app or 0 if user stats have not been initialized.
Use StatsInitialized() to determine when user stats are initialized before calling this method.
*/
int GetNumAchievements()
{
	if (Steam)
	{
		return Steam->GetNumAchievements();
	}
	return 0;
}

/*
Gets the achievement ID for the given achievement index.
If user stats have not been initialized, an empty string is returned.
Use StatsInitialized() to determine when user stats are initialized before calling this method.
*/
char *GetAchievementID(int index)
{
	if (Steam)
	{
		return CreateString(Steam->GetAchievementID(index));
	}
	return CreateString(0);
}

char *GetAchievementDisplayName(const char *pchName)
{
	if (Steam)
	{
		return CreateString(Steam->GetAchievementDisplayAttribute(pchName, "name"));
	}
	return CreateString(0);
}

char *GetAchievementDisplayDesc(const char *pchName)
{
	if (Steam)
	{
		return CreateString(Steam->GetAchievementDisplayAttribute(pchName, "desc"));
	}
	return CreateString(0);
}

int GetAchievementDisplayHidden(const char *pchName)
{
	if (Steam)
	{
		return (strcmp(Steam->GetAchievementDisplayAttribute(pchName, "hidden"), "1") == 0);
	}
	return 0;
}

int GetAchievementIcon(const char *pchName)
{
	if (Steam)
	{
		return Steam->GetAchievementIcon(pchName);
	}
	return 0;
}

/*
Returns 1 if the user has achieved this achievement.
If user stats have not been initialized 0 is returned.
Use StatsInitialized() to determine when user stats are initialized before calling this method.
If the call fails, 0 is returned and the error is reported to the AGK interpreter.
*/
int GetAchievement(const char *pchName)
{
	if (Steam)
	{
		bool result = false;
		if (Steam->GetAchievement(pchName, &result))
		{
			return result;
		}
		agk::PluginError("GetAchievement failed.");
	}
	return false;
}

int GetAchievementUnlockTime(const char *pchName)
{
	if (Steam)
	{
		bool pbAchieved;
		uint32 punUnlockTime;
		if (SteamUserStats()->GetAchievementAndUnlockTime(pchName, &pbAchieved, &punUnlockTime))
		{
			if (pbAchieved)
			{
				return punUnlockTime;
			}
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
	if (Steam)
	{
		return Steam->SetAchievement(pchName);
	}
	return false;
}

/*
Raises a notification about achievemnt progress for progress stat achievements.
The notification only shows when current progress is less than the max.
SetStat still needs to be used to set the progress stat value.
*/
int IndicateAchievementProgress(const char *pchName, int nCurProgress, int nMaxProgress)
{
	if (Steam)
	{
		return Steam->IndicateAchievementProgress(pchName, nCurProgress, nMaxProgress);
	}
	return false;
}

/*
Clears an achievement.
Returns 1 if the process succeeds.  Otherwise, returns 0.
Use StatsInitialized() to determine when user stats are initialized before calling this method.
*/
int ClearAchievement(const char *pchName)
{
	if (Steam)
	{
		return Steam->ClearAchievement(pchName);
	}
	return false;
}

/*
Gets an integer user stat value.
If there is a problem, 0 is returned and the error is reported to the AGK interpreter.
*/
int GetStatInt(const char *pchName)
{
	if (Steam)
	{
		int result = 0;
		if (Steam->GetStat(pchName, &result)) {
			return result;
		}
		agk::PluginError("GetStat failed.");
	}
	return 0;
}

/*
Gets a float user stat value.
If there is a problem, 0 is returned and the error is reported to the AGK interpreter.
*/
float GetStatFloat(const char *pchName)
{
	if (Steam)
	{
		float result = 0.0;
		if (Steam->GetStat(pchName, &result)) {
			return result;
		}
		agk::PluginError("GetStat failed.");
	}
	return 0.0;
}

/*
Sets an integer user stat value.
Returns 1 if the call succeeds.  Otherwise returns 0.
*/
int SetStatInt(const char *pchName, int nData)
{
	if (Steam)
	{
		return Steam->SetStat(pchName, nData);
	}
	return false;
}

/*
Sets a float user stat value.
Returns 1 if the call succeeds.  Otherwise returns 0.
*/
int SetStatFloat(const char *pchName, float fData)
{
	if (Steam)
	{
		return Steam->SetStat(pchName, fData);
	}
	return false;
}

/*
Updates an average rate user stat.  Steam takes care of the averaging.  Use GetStatFloat to get the result.
Returns 1 if the call succeeds.  Otherwise returns 0.
*/
int UpdateAvgRateStat(const char *pchName, float flCountThisSession, float dSessionLength)
{
	if (Steam)
	{
		return Steam->UpdateAvgRateStat(pchName, flCountThisSession, (double) dSessionLength);
	}
	return false;
}

int FindLeaderboard(const char *pchLeaderboardName)
{
	if (Steam)
	{
		return Steam->FindLeaderboard(pchLeaderboardName);
	}
	return false;
}

int GetFindLeaderboardCallbackState()
{
	if (Steam)
	{
		return Steam->GetFindLeaderboardCallbackState();
	}
	return STATE_CLIENT_ERROR;
}

int GetLeaderboardHandle()
{
	if (Steam)
	{
		return (int) Steam->GetLeaderboardHandle();
	}
	return 0;
}

char *GetLeaderboardName(int hLeaderboard)
{
	if (Steam)
	{
		return CreateString(Steam->GetLeaderboardName(hLeaderboard));
	}
	return CreateString(0);
}

int GetLeaderboardEntryCount(int hLeaderboard)
{
	if (Steam)
	{
		return Steam->GetLeaderboardEntryCount(hLeaderboard);
	}
	return 0;
}

int GetLeaderboardDisplayType(int hLeaderboard)
{
	if (Steam)
	{
		return Steam->GetLeaderboardDisplayType(hLeaderboard);
	}
	return 0;
}

int GetLeaderboardSortMethod(int hLeaderboard)
{
	if (Steam)
	{
		return Steam->GetLeaderboardSortMethod(hLeaderboard);
	}
	return 0;
}

int UploadLeaderboardScore(int hLeaderboard, int score)
{
	if (Steam)
	{
		return Steam->UploadLeaderboardScore((SteamLeaderboard_t)hLeaderboard, k_ELeaderboardUploadScoreMethodKeepBest, score);
	}
	return false;
}

int UploadLeaderboardScoreForceUpdate(int hLeaderboard, int score)
{
	if (Steam)
	{
		return Steam->UploadLeaderboardScore((SteamLeaderboard_t)hLeaderboard, k_ELeaderboardUploadScoreMethodForceUpdate, score);
	}
	return false;
}

int GetUploadLeaderboardScoreCallbackState()
{
	if (Steam)
	{
		return Steam->GetUploadLeaderboardScoreCallbackState();
	}
	return STATE_CLIENT_ERROR;
}

int LeaderboardScoreStored()
{
	if (Steam)
	{
		return Steam->LeaderboardScoreStored();
	}
	return false;
}

int LeaderboardScoreChanged()
{
	if (Steam)
	{
		return Steam->LeaderboardScoreChanged();
	}
	return false;
}

int GetLeaderboardUploadedScore()
{
	if (Steam)
	{
		return Steam->GetLeaderboardUploadedScore();
	}
	return 0;
}

int GetLeaderboardGlobalRankNew()
{
	if (Steam)
	{
		return Steam->GetLeaderboardGlobalRankNew();
	}
	return 0;
}

int GetLeaderboardGlobalRankPrevious()
{
	if (Steam)
	{
		return Steam->GetLeaderboardGlobalRankPrevious();
	}
	return 0;
}

int DownloadLeaderboardEntries(int hLeaderboard, int eLeaderboardDataRequest, int nRangeStart, int nRangeEnd)
{
	if (Steam)
	{
		return Steam->DownloadLeaderboardEntries(hLeaderboard, (ELeaderboardDataRequest) eLeaderboardDataRequest, nRangeStart, nRangeEnd);
	}
	return false;
}

int GetDownloadLeaderboardEntriesCallbackState()
{
	if (Steam)
	{
		return Steam->GetDownloadLeaderboardEntriesCallbackState();
	}
	return STATE_CLIENT_ERROR;
}

int GetDownloadedLeaderboardEntryCount()
{
	if (Steam)
	{
		return Steam->GetDownloadedLeaderboardEntryCount();
	}
	return 0;
}

int GetDownloadedLeaderboardEntryGlobalRank(int index)
{
	if (Steam)
	{
		return Steam->GetDownloadedLeaderboardEntryGlobalRank(index);
	}
	return 0;
}

int GetDownloadedLeaderboardEntryScore(int index)
{
	if (Steam)
	{
		return Steam->GetDownloadedLeaderboardEntryScore(index);
	}
	return 0;
}

int GetDownloadedLeaderboardEntryUser(int index)
{
	if (Steam)
	{
		return GetSteamIDHandle(Steam->GetDownloadedLeaderboardEntryUser(index));
	}
	return 0;
}

int RequestLobbyList()
{
	if (Steam)
	{
		return Steam->RequestLobbyList();
	}
	return 0;
}

int GetLobbyMatchListCallbackState()
{
	if (Steam)
	{
		return Steam->GetLobbyMatchListCallbackState();
	}
	return STATE_CLIENT_ERROR;
}

int GetLobbyMatchListCount() {
	if (Steam)
	{
		return Steam->GetLobbyMatchListCount();
	}
	return 0;
}

int GetLobbyByIndex(int iLobby)
{
	if (Steam)
	{
		return GetSteamIDHandle(Steam->GetLobbyByIndex(iLobby));
	}
	return 0;
}

int GetLobbyDataCount(int hLobbySteamID)
{
	if (Steam)
	{
		return Steam->GetLobbyDataCount(GetSteamID(hLobbySteamID));
	}
	return 0;
}

char *GetLobbyDataByIndex(int hLobbySteamID, int iLobbyData)
{
	if (Steam)
	{
		char key[k_nMaxLobbyKeyLength];
		char value[k_cubChatMetadataMax];
		if (Steam->GetLobbyDataByIndex(GetSteamID(hLobbySteamID), iLobbyData, key, k_nMaxLobbyKeyLength, value, k_cubChatMetadataMax))
		{
			std::string json = "{\"key\": \"";
			json.append(key);
			json.append("\", \"value\": \"");
			json.append(value);
			json.append("\"}");
			return CreateString(json.c_str());
		}
	}
	return CreateString(NULL);
}

int JoinLobby(int hLobbySteamID)
{
	if (Steam)
	{
		return Steam->JoinLobby(GetSteamID(hLobbySteamID));
	}
	return false;
}

int GetLobbyEnterCallbackState()
{
	if (Steam)
	{
		return Steam->GetLobbyEnterCallbackState();
	}
	return STATE_CLIENT_ERROR;
}

int GetLobbyEnterBlocked()
{
	if (Steam)
	{
		return Steam->GetLobbyEnterBlocked();
	}
	return false;
}

int GetLobbyEnterResponse()
{
	if (Steam)
	{
		return Steam->GetLobbyEnterResponse();
	}
	return 0;
}

void LeaveLobby(int hLobbySteamID)
{
	if (Steam)
	{
		Steam->LeaveLobby(GetSteamID(hLobbySteamID));
	}
}

int HasLobbyChatUpdate()
{
	if (Steam)
	{
		return Steam->HasLobbyChatUpdate();
	}
	return false;
}

int GetLobbyChatUpdateUserChanged()
{
	if (Steam)
	{
		return GetSteamIDHandle(Steam->GetLobbyChatUpdateUserChanged());
	}
	return 0;
}

int GetLobbyChatUpdateUserState()
{
	if (Steam)
	{
		return Steam->GetLobbyChatUpdateUserState();
	}
	return 0;
}

int GetLobbyChatUpdateUserMakingChange()
{
	if (Steam)
	{
		return GetSteamIDHandle(Steam->GetLobbyChatUpdateUserMakingChange());
	}
	return 0;
}

int GetLobbyOwner(int hLobbySteamID)
{
	if (Steam)
	{
		return GetSteamIDHandle(Steam->GetLobbyOwner(GetSteamID(hLobbySteamID)));
	}
	return 0;
}

int GetLobbyMemberLimit(int hLobbySteamID)
{
	if (Steam)
	{
		return Steam->GetLobbyMemberLimit(GetSteamID(hLobbySteamID));
	}
	return 0;
}

int GetNumLobbyMembers(int hLobbySteamID)
{
	if (Steam)
	{
		return Steam->GetNumLobbyMembers(GetSteamID(hLobbySteamID));
	}
	return 0;
}

int GetLobbyMemberByIndex(int hLobbySteamID, int iMember)
{
	if (Steam)
	{
		return GetSteamIDHandle(Steam->GetLobbyMemberByIndex(GetSteamID(hLobbySteamID), iMember));
	}
	return 0;
}

int HasLobbyChatMessage()
{
	if (Steam)
	{
		return Steam->HasLobbyChatMessage();
	}
	return false;
}

int GetLobbyChatMessageUser()
{
	if (Steam)
	{
		return GetSteamIDHandle(Steam->GetLobbyChatMessageUser());
	}
	return 0;
}

char *GetLobbyChatMessageText()
{
	if (Steam)
	{
		char msg[4096];
		Steam->GetLobbyChatMessageText(msg);
		return CreateString(msg);
	}
	return CreateString(NULL);
}

int SendLobbyChatMessage(int hLobbySteamID, char *msg)
{
	if (Steam)
	{
		return Steam->SendLobbyChatMessage(GetSteamID(hLobbySteamID), msg);
	}
	return 0;
}
