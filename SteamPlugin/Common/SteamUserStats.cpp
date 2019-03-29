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

#include "DllMain.h"

/* @page ISteamUserStats */

#define MAX_STATS_HISTORY_DAYS	60

const int ACH_NAME_MAX_LENGTH = 128;
char g_MostAchievedAchievementInfoName[ACH_NAME_MAX_LENGTH];
float g_MostAchievedAchievementInfoPercent;
bool g_MostAchievedAchievementInfoUnlocked;
int g_MostAchievedAchievementInfoIterator;

void ClearMostAchievedAchievementInfo()
{
	g_MostAchievedAchievementInfoName[0] = 0;
	g_MostAchievedAchievementInfoPercent = 0;
	g_MostAchievedAchievementInfoUnlocked = false;
	g_MostAchievedAchievementInfoIterator = -1;
}

class CAttachLeaderboardUGCCallResult : public CCallResultItem<LeaderboardUGCSet_t>
{
public:
	CAttachLeaderboardUGCCallResult(SteamLeaderboard_t hSteamLeaderboard, UGCHandle_t hUGC)
	{
		m_CallResultName = "AttachLeaderboardUGC("
			+ std::to_string(hSteamLeaderboard) + ", "
			+ std::to_string(hUGC) + ")";
		m_hSteamAPICall = SteamUserStats()->AttachLeaderboardUGC(hSteamLeaderboard, hUGC);
	}
	SteamLeaderboard_t GetLeaderboard() { return m_Response.m_hSteamLeaderboard; }
};

/*
@desc Attaches a piece of user generated content the current user's entry on a leaderboard.

Use GetCallResultCode to determine whether this call was successful.
@param hLeaderboard A leaderboard handle.
@param hUGC Handle to a piece of user generated content that was shared using CloudFileShare or CreateUGCItem.
@callback-type callresult
@callback-getters GetAttachLeaderboardUGCLeaderboard
@return A [call result handle](Callbacks-and-Call-Results#call-results) on success; otherwise 0.
@url https://partner.steamgames.com/doc/api/ISteamUserStats#AttachLeaderboardUGC
@url https://partner.steamgames.com/doc/api/ISteamUserStats#LeaderboardUGCSet_t
*/
extern "C" DLL_EXPORT int AttachLeaderboardUGC(int hLeaderboard, int hUGC)
{
	CheckInitialized(0);
	SteamLeaderboard_t leaderboard = SteamHandles()->GetSteamHandle(hLeaderboard);
	if (!leaderboard)
	{
		agk::PluginError("AttachLeaderboardUGC: Invalid leaderboard handle.");
		return 0;
	}
	UGCHandle_t ugc = SteamHandles()->GetSteamHandle(hUGC);
	if (!ugc)
	{
		agk::PluginError("AttachLeaderboardUGC: Invalid UGC handle.");
		return 0;
	}
	return CallResults()->Add(new CAttachLeaderboardUGCCallResult(leaderboard, ugc));
}

/*
@desc Returns the leaderboard handle for the AttachLeaderboardUGC call result.
@param hCallResult An AttachLeaderboardUGC call result handle.
@return A leaderboard handle.
*/
extern "C" DLL_EXPORT int GetAttachLeaderboardUGCLeaderboard(int hCallResult)
{
	return GetCallResultValue(hCallResult, &CAttachLeaderboardUGCCallResult::GetLeaderboard);
}

/*
@desc Removes an achievement from the user.
Call StoreStats afterward to upload the stats to the server.

_This method is generally just for testing purposes._
@param name The 'API Name' of the achievement.
@return int: 1 when the call succeeds; otherwise 0.
@url https://partner.steamgames.com/doc/api/ISteamUserStats#ClearAchievement
*/
extern "C" DLL_EXPORT int ClearAchievement(const char *name)
{
	CheckInitialized(false);
	Callbacks()->UserAchievementStored.Register();
	return SteamUserStats()->ClearAchievement(name);
}

typedef WrappedResponse<LeaderboardScoresDownloaded_t, SteamLeaderboard_t, &LeaderboardScoresDownloaded_t::m_hSteamLeaderboard> WrappedLeaderboardScoresDownloaded_t;

class CDownloadLeaderboardEntriesCallResult : public CCallResultItem<LeaderboardScoresDownloaded_t, WrappedLeaderboardScoresDownloaded_t>
{
public:
	CDownloadLeaderboardEntriesCallResult(SteamLeaderboard_t hLeaderboard, ELeaderboardDataRequest eLeaderboardDataRequest, int nRangeStart, int nRangeEnd)
	{
		m_CallResultName = "DownloadLeaderboardEntries("
			+ std::to_string(hLeaderboard) + ", "
			+ std::to_string(eLeaderboardDataRequest) + ", "
			+ std::to_string(nRangeStart) + ", "
			+ std::to_string(nRangeEnd) + ")";
		m_hSteamAPICall = SteamUserStats()->DownloadLeaderboardEntries(hLeaderboard, eLeaderboardDataRequest, nRangeStart, nRangeEnd);
	}
	uint64 GetLeaderboardID() { return m_Response.m_hSteamLeaderboard; }
	int GetLeaderboardEntryCount() { return (int)m_Entries.size(); }
	bool IsValidIndex(int index) { return (index >= 0) && (index < (int)m_Entries.size()); }
	uint64 GetLeaderboardEntryUser(int index) { return m_Entries[index].m_steamIDUser.ConvertToUint64(); }
	int GetLeaderboardEntryGlobalRank(int index) { return m_Entries[index].m_nGlobalRank; }
	int GetLeaderboardEntryScore(int index) { return m_Entries[index].m_nScore; }
	int GetLeaderboardEntryDetailsCount(int index) { return m_Entries[index].m_cDetails; }
	int32 GetLeaderboardEntryDetails(int index, int detailIndex) { return m_Entries[index].m_Details[detailIndex]; }
	uint64 GetLeaderboardEntryUGCHandle(int index) { return m_Entries[index].m_hUGC; }
protected:
	struct PluginLeaderboardEntry_t : LeaderboardEntry_t
	{
		int32 m_Details[k_cLeaderboardDetailsMax];
		PluginLeaderboardEntry_t() {}
		PluginLeaderboardEntry_t(LeaderboardEntry_t from) : LeaderboardEntry_t(from) {}
	};
	std::vector<PluginLeaderboardEntry_t> m_Entries;
	void OnResponse(LeaderboardScoresDownloaded_t *pCallResult, bool bFailure)
	{
		CCallResultItem::OnResponse(pCallResult, bFailure);
		if (pCallResult->m_hSteamLeaderboardEntries != 0)
		{
			for (int index = 0; index < pCallResult->m_cEntryCount; index++)
			{
				LeaderboardEntry_t tempEntry;
				int32 details[k_cLeaderboardDetailsMax];
				if (SteamUserStats()->GetDownloadedLeaderboardEntry(pCallResult->m_hSteamLeaderboardEntries, index, &tempEntry, details, k_cLeaderboardDetailsMax))
				{
					PluginLeaderboardEntry_t entry(tempEntry);
					memcpy_s(entry.m_Details, k_cLeaderboardDetailsMax, details, k_cLeaderboardDetailsMax);
					m_Entries.push_back(entry);
				}
				else
				{
					// Report error.
					utils::Log(GetName() + ": GetDownloadedLeaderboardEntry failed for index " + std::to_string(index) + ".");
					SetResultCode(k_EResultFail);
					break;
				}
			}
		}
	}
};

/*
@desc Downloads entries from a leaderboard.
@param hLeaderboard A leaderboard handle.
@param eLeaderboardDataRequest The type of data request to make.
@param-url eLeaderboardDataRequest https://partner.steamgames.com/doc/api/ISteamUserStats#ELeaderboardDataRequest
@param rangeStart The index to start downloading entries relative to eLeaderboardDataRequest.
@param rangeEnd The last index to retrieve entries for relative to eLeaderboardDataRequest.
@callback-type callresult
@callback-getters GetDownloadLeaderboardHandle, GetDownloadLeaderboardEntryCount, GetDownloadLeaderboardEntryUser, GetDownloadLeaderboardEntryGlobalRank, GetDownloadLeaderboardEntryScore
@return A [call result handle](Callbacks-and-Call-Results#call-results) on success; otherwise 0.
@url https://partner.steamgames.com/doc/api/ISteamUserStats#DownloadLeaderboardEntries
*/
extern "C" DLL_EXPORT int DownloadLeaderboardEntries(int hLeaderboard, int eLeaderboardDataRequest, int rangeStart, int rangeEnd)
{
	CheckInitialized(0);
	SteamLeaderboard_t leaderboard = SteamHandles()->GetSteamHandle(hLeaderboard);
	if (!leaderboard)
	{
		agk::PluginError("DownloadLeaderboardEntries: Invalid leaderboard handle.");
		return 0;
	}
	return CallResults()->Add(new CDownloadLeaderboardEntriesCallResult(leaderboard, (ELeaderboardDataRequest)eLeaderboardDataRequest, rangeStart, rangeEnd));
}

//TODO DownloadLeaderboardEntriesForUsers

/*
@desc Returns the handle to the leaderboard for the DownloadLeaderboardEntries call.
@param hCallResult A DownloadLeaderboardEntries call result handle.
@return A Steam ID handle.
@url https://partner.steamgames.com/doc/api/ISteamUserStats#LeaderboardScoresDownloaded_t
*/
extern "C" DLL_EXPORT int GetDownloadLeaderboardHandle(int hCallResult)
{
	return GetCallResultValue(hCallResult, &CDownloadLeaderboardEntriesCallResult::GetLeaderboardID);
}

/*
@desc Returns the entry count for the DownloadLeaderboardEntries call.
@param hCallResult A DownloadLeaderboardEntries call result handle.
@return The number of entries.
@url https://partner.steamgames.com/doc/api/ISteamUserStats#LeaderboardScoresDownloaded_t
*/
extern "C" DLL_EXPORT int GetDownloadLeaderboardEntryCount(int hCallResult)
{
	return GetCallResultValue(hCallResult, &CDownloadLeaderboardEntriesCallResult::GetLeaderboardEntryCount);
}

/*
@desc Returns the handle of the user of an entry for the DownloadLeaderboardEntries call.
@param hCallResult A DownloadLeaderboardEntries call result handle.
@param index The entry index.
@return A Steam ID handle.
@url https://partner.steamgames.com/doc/api/ISteamUserStats#LeaderboardScoresDownloaded_t
*/
extern "C" DLL_EXPORT int GetDownloadLeaderboardEntryUser(int hCallResult, int index)
{
	return GetCallResultValue(hCallResult, index, &CDownloadLeaderboardEntriesCallResult::GetLeaderboardEntryUser, __FUNCTION__);
}

/*
@desc Returns the global rank of an entry for the DownloadLeaderboardEntries call.
@param hCallResult A DownloadLeaderboardEntries call result handle.
@param index The entry index.
@return A Steam ID handle.
@url https://partner.steamgames.com/doc/api/ISteamUserStats#LeaderboardScoresDownloaded_t
*/
extern "C" DLL_EXPORT int GetDownloadLeaderboardEntryGlobalRank(int hCallResult, int index)
{
	return GetCallResultValue(hCallResult, index, &CDownloadLeaderboardEntriesCallResult::GetLeaderboardEntryGlobalRank, __FUNCTION__);
}

/*
@desc Returns the score of an entry for the DownloadLeaderboardEntries call.
@param hCallResult A DownloadLeaderboardEntries call result handle.
@param index The entry index.
@return An integer
@url https://partner.steamgames.com/doc/api/ISteamUserStats#LeaderboardScoresDownloaded_t
*/
extern "C" DLL_EXPORT int GetDownloadLeaderboardEntryScore(int hCallResult, int index)
{
	return GetCallResultValue(hCallResult, index, &CDownloadLeaderboardEntriesCallResult::GetLeaderboardEntryScore, __FUNCTION__);
}

/*
@desc Returns a JSON array of integers for the details of an entry for the DownloadLeaderboardEntries call.
@param hCallResult A DownloadLeaderboardEntries call result handle.
@param index The entry index.
@return A JSON array of integers.
@url https://partner.steamgames.com/doc/api/ISteamUserStats#LeaderboardScoresDownloaded_t
*/
extern "C" DLL_EXPORT char *GetDownloadLeaderboardEntryDetails(int hCallResult, int index)
{
	std::ostringstream json;
	json << "[";
	if (auto *callResult = CallResults()->Get<CDownloadLeaderboardEntriesCallResult>(hCallResult))
	{
		if (callResult->IsValidIndex(index))
		{
			int detailCount = callResult->GetLeaderboardEntryDetailsCount(index);
			for (int detailIndex = 0; detailIndex < detailCount; detailIndex++)
			{
				if (detailIndex > 0)
				{
					json << ", ";
				}
				json << callResult->GetLeaderboardEntryDetails(index, detailIndex);
			}
		}
		else
		{
			utils::PluginError("GetDownloadLeaderboardEntryDetails : Index out of range.");
		}
	}
	json << "]";
	return utils::CreateString(json);
}

/*
@desc Returns the UGC handle for the details of an entry for the DownloadLeaderboardEntries call.
@param hCallResult A DownloadLeaderboardEntries call result handle.
@param index The entry index.
@return A UGC handle or 0.
@url https://partner.steamgames.com/doc/api/ISteamUserStats#LeaderboardScoresDownloaded_t
*/
extern "C" DLL_EXPORT int GetDownloadLeaderboardEntryUGC(int hCallResult, int index)
{
	return GetCallResultValue(hCallResult, index, &CDownloadLeaderboardEntriesCallResult::GetLeaderboardEntryUGCHandle, __FUNCTION__);
}

class CFindLeaderboardCallResult : public CCallResultItem<LeaderboardFindResult_t, AlwaysOKResponse<LeaderboardFindResult_t>>
{
public:
	CFindLeaderboardCallResult(const char *pchLeaderboardName, ELeaderboardSortMethod eLeaderboardSortMethod = k_ELeaderboardSortMethodNone, ELeaderboardDisplayType eLeaderboardDisplayType = k_ELeaderboardDisplayTypeNone)
	{
		// No sort and/or no display type = find only.
		if (eLeaderboardSortMethod == k_ELeaderboardSortMethodNone || eLeaderboardDisplayType == k_ELeaderboardDisplayTypeNone)
		{
			m_CallResultName = "FindLeaderboard('" + std::string(pchLeaderboardName) + "')";
			m_hSteamAPICall = SteamUserStats()->FindLeaderboard(pchLeaderboardName);
		}
		else
		{
			m_CallResultName = "FindOrCreateLeaderboard('" + std::string(pchLeaderboardName) + "', "
				+ std::to_string(eLeaderboardSortMethod) + ", "
				+ std::to_string(eLeaderboardDisplayType) + ")";
			m_hSteamAPICall = SteamUserStats()->FindOrCreateLeaderboard(pchLeaderboardName, eLeaderboardSortMethod, eLeaderboardDisplayType);
		}
	}
	int GetLeaderboardFindResultFound() { return m_Response.m_bLeaderboardFound; }
	SteamLeaderboard_t GetLeaderboardFindResultHandle() { return m_Response.m_hSteamLeaderboard; }
};

/*
@desc Sends a request to find the handle for a leaderboard.
@param leaderboardName The name of the leaderboard to find.
@callback-type callresult
@callback-getters GetFindLeaderboardFound, GetFindLeaderboardHandle
@return A [call result handle](Callbacks-and-Call-Results#call-results) on success; otherwise 0.
@url https://partner.steamgames.com/doc/api/ISteamUserStats#FindLeaderboard
*/
extern "C" DLL_EXPORT int FindLeaderboard(const char *leaderboardName)
{
	CheckInitialized(0);
	return CallResults()->Add(new CFindLeaderboardCallResult(leaderboardName));
}

/*
@desc Gets a leaderboard by name, it will create it if it's not yet created.
Leaderboards created with this function will not automatically show up in the Steam Community.
You must manually set the Community Name field in the App Admin panel of the Steamworks website.

_Note: If either eLeaderboardSortMethod OR eLeaderboardDisplayType are 0 (none), an error is raised.
@param leaderboardName The name of the leaderboard to find.
@param eLeaderboardSortMethod The sort order of the new leaderboard if it's created.
@param-url eLeaderboardSortMethod https://partner.steamgames.com/doc/api/ISteamUserStats#ELeaderboardSortMethod
@param eLeaderboardDisplayType The display type (used by the Steam Community web site) of the new leaderboard if it's created.
@param-url eLeaderboardDisplayType https://partner.steamgames.com/doc/api/ISteamUserStats#ELeaderboardDisplayType
@callback-type callresult
@callback-getters GetFindLeaderboardFound, GetFindLeaderboardHandle
@return A [call result handle](Callbacks-and-Call-Results#call-results) on success; otherwise 0.
@url https://partner.steamgames.com/doc/api/ISteamUserStats#FindOrCreateLeaderboard
*/
extern "C" DLL_EXPORT int FindOrCreateLeaderboard(const char *leaderboardName, int eLeaderboardSortMethod, int eLeaderboardDisplayType)
{
	CheckInitialized(0);
	if (eLeaderboardSortMethod == k_ELeaderboardSortMethodNone || eLeaderboardDisplayType == k_ELeaderboardDisplayTypeNone)
	{
		agk::PluginError("FindOrCreateLeaderboard: eLeaderboardSortMethod and eLeaderboardDisplayType cannot be 0.");
		return 0;
	}
	return CallResults()->Add(new CFindLeaderboardCallResult(leaderboardName, (ELeaderboardSortMethod)eLeaderboardSortMethod, (ELeaderboardDisplayType) eLeaderboardDisplayType));
}

/*
@desc Returns whether the leaderboard was found for the FindLeaderboard call.
@param hCallResult A FindLeaderboard call result handle.
@return 1 if found; otherwise 0.
@url https://partner.steamgames.com/doc/api/ISteamUserStats#LeaderboardFindResult_t
*/
extern "C" DLL_EXPORT int GetFindLeaderboardFound(int hCallResult)
{
	return GetCallResultValue(hCallResult, &CFindLeaderboardCallResult::GetLeaderboardFindResultFound);
}

/*
@desc Returns the found leaderboard for the FindLeaderboard call.
@param hCallResult A FindLeaderboard call result handle.
@return A Steam ID handle.
@url https://partner.steamgames.com/doc/api/ISteamUserStats#LeaderboardFindResult_t
*/
extern "C" DLL_EXPORT int GetFindLeaderboardHandle(int hCallResult)
{
	return GetCallResultValue(hCallResult, &CFindLeaderboardCallResult::GetLeaderboardFindResultHandle);
}

/*
@desc Gets whether the user has achieved this achievement.
@param name The 'API Name' of the achievement.
@return 1 when the user has achieved this achievement; otherwise 0.
@url https://partner.steamgames.com/doc/api/ISteamUserStats#GetAchievement
*/
extern "C" DLL_EXPORT int GetAchievement(const char *name)
{
	CheckInitialized(false);
	bool result = false;
	if (SteamUserStats()->GetAchievement(name, &result))
	{
		return result;
	}
	agk::PluginError("GetAchievement failed.");
	return false;
}

/*
@desc Returns the percentage of users who have unlocked the specified achievement.

You must have called RequestGlobalAchievementPercentages and it needs to return successfully via its callback prior to calling this.

_If RequestGlobalAchievementPercentages has not been called or if the specified 'API Name' does not exist in the global achievement percentages, an error will be raised._
@param name The 'API Name' of the achievement.
@return The percentage of people that have unlocked this achievement from 0 to 100 or -1 if there is an error.
@url https://partner.steamgames.com/doc/api/ISteamUserStats#GetAchievementAchievedPercent
*/
extern "C" DLL_EXPORT float GetAchievementAchievedPercent(const char *name)
{
	CheckInitialized(-1.0f);
	float pflPercent;
	if (SteamUserStats()->GetAchievementAchievedPercent(name, &pflPercent))
	{
		return pflPercent;
	}
	agk::PluginError("GetAchievementAchievedPercent failed.");
	return -1.0f;
}

/*
@desc Gets the time at which an achievement was unlocked, if ever.
@param name The 'API Name' of the achievement.
@return The unload time in Unix time.
@url https://partner.steamgames.com/doc/api/ISteamUserStats#GetAchievementAndUnlockTime
*/
extern "C" DLL_EXPORT int GetAchievementUnlockTime(const char *name)
{
	CheckInitialized(0);
	bool pbAchieved;
	uint32 punUnlockTime;
	if (SteamUserStats()->GetAchievementAndUnlockTime(name, &pbAchieved, &punUnlockTime))
	{
		if (pbAchieved)
		{
			return punUnlockTime;
		}
	}
	return 0;
}

/*
@desc Gets the localized achievement name.
@param name The 'API Name' of the achievement.
@return The localized achievement name.
@url https://partner.steamgames.com/doc/api/ISteamUserStats#GetAchievementDisplayAttribute
*/
extern "C" DLL_EXPORT char *GetAchievementDisplayName(const char *name)
{
	CheckInitialized(NULL_STRING);
	return utils::CreateString(SteamUserStats()->GetAchievementDisplayAttribute(name, "name"));
}

/*
@desc Gets the localized achievement description.
@param name The 'API Name' of the achievement.
@return The localized achievement description.
@url https://partner.steamgames.com/doc/api/ISteamUserStats#GetAchievementDisplayAttribute
*/
extern "C" DLL_EXPORT char *GetAchievementDisplayDesc(const char *name)
{
	CheckInitialized(NULL_STRING);
	return utils::CreateString(SteamUserStats()->GetAchievementDisplayAttribute(name, "desc"));
}

/*
@desc Gets whether an achievement is hidden.
@param name The 'API Name' of the achievement.
@return 1 when the achievement is hidden, 0 when it is not hidden.
@url https://partner.steamgames.com/doc/api/ISteamUserStats#GetAchievementDisplayAttribute
*/
extern "C" DLL_EXPORT int GetAchievementDisplayHidden(const char *name)
{
	CheckInitialized(false);
	return (strcmp(SteamUserStats()->GetAchievementDisplayAttribute(name, "hidden"), "1") == 0);
}

/*
@desc Gets the achievement icon for the current user's current achievement state.
@param name The 'API Name' of the achievement.
@return 0 when no icon is set, -1 when the icon needs to download via callback, or an image handle. [Similar to how avatars work.](Avatars)
@url https://partner.steamgames.com/doc/api/ISteamUserStats#GetAchievementIcon
*/
extern "C" DLL_EXPORT int GetAchievementIcon(const char *name)
{
	CheckInitialized(0);
	return Callbacks()->GetAchievementIcon(name);
}

/*
@desc Gets the achievement ID for the achievement index.

_This method is generally just for testing purposes since the app should already know the achievement IDs._
@param index Index of the achievement.
@return Gets the 'API name' for an achievement index between 0 and GetNumAchievements.
@url https://partner.steamgames.com/doc/api/ISteamUserStats#GetAchievementName
*/
extern "C" DLL_EXPORT char *GetAchievementAPIName(int index)
{
	CheckInitialized(NULL_STRING);
	return utils::CreateString(SteamUserStats()->GetAchievementName(index));
}

/*
@desc Gets the lifetime totals for an aggregated integer stat.

You must have called RequestGlobalStats and it needs to return successfully via its callback prior to calling this.

_Note: Since AppGameKit is limited to 32-bit integers, this value is returned as a string._

_If there is a problem getting the value, an error is raised._
@param name The 'API Name' of the stat.
@return The lifetime totals for a stat.
@url https://partner.steamgames.com/doc/api/ISteamUserStats#GetGlobalStat
*/
extern "C" DLL_EXPORT char *GetGlobalStatInt64AsString(const char *name)
{
	CheckInitialized(NULL_STRING);
	int64 result = 0;
	if (SteamUserStats()->GetGlobalStat(name, &result))
	{
		return utils::CreateString(std::to_string(result));
	}
	agk::PluginError("GetGlobalStat failed.");
	return NULL_STRING;
}

/*
@desc Gets the lifetime totals for an aggregated float stat.

You must have called RequestGlobalStats and it needs to return successfully via its callback prior to calling this.

_Note: Since AppGameKit is limited to floats, this value is returned as a string._

_If there is a problem getting the value, an error is raised._
@param name The 'API Name' of the stat.
@param precision The precision of the returned string.
@return The lifetime totals for a stat.
@url https://partner.steamgames.com/doc/api/ISteamUserStats#GetGlobalStat
*/
extern "C" DLL_EXPORT char *GetGlobalStatDoubleAsString(const char *name, int precision)
{
	CheckInitialized(NULL_STRING);
	double result = 0.0;
	if (SteamUserStats()->GetGlobalStat(name, &result))
	{
		try
		{
			std::stringstream ss;
			ss << std::fixed << std::setprecision(precision) << result;
			return utils::CreateString(ss.str());
		}
		catch (...)
		{ }
	}
	agk::PluginError("GetGlobalStat failed.");
	return NULL_STRING;
}

/*
@desc Gets the daily history for an aggregated int stat.

Returned as a JSON array of strings, starting with today's value.
@param name The 'API Name' of the stat.
@return A JSON array of int64 values as strings.
@url https://partner.steamgames.com/doc/api/ISteamUserStats#GetGlobalStatHistory
*/
extern "C" DLL_EXPORT char *GetGlobalStatHistoryInt64JSON(const char *name)
{
	std::ostringstream json;
	json << "[";
	if (g_SteamInitialized)
	{
		int64 pData[MAX_STATS_HISTORY_DAYS];
		int count = SteamUserStats()->GetGlobalStatHistory(name, pData, MAX_STATS_HISTORY_DAYS * 8);
		for (int index = 0; index < count; index++)
		{
			if (index > 0)
			{
				json << ", ";
			}
			json << "'" << pData[index] << "'";
		}
	}
	json << "]";
	return utils::CreateString(json);
}

/*
@desc Gets the daily history for an aggregated floar stat.

Returned as a JSON array of strings, starting with today's value.
@param name The 'API Name' of the stat.
@param precision The precision of the returned string.
@return A JSON array of double values as strings.
@url https://partner.steamgames.com/doc/api/ISteamUserStats#GetGlobalStatHistory
*/
extern "C" DLL_EXPORT char *GetGlobalStatHistoryDoubleJSON(const char *name, int precision)
{
	std::ostringstream json;
	json << "[";
	if (g_SteamInitialized)
	{
		double pData[MAX_STATS_HISTORY_DAYS];
		int count = SteamUserStats()->GetGlobalStatHistory(name, pData, MAX_STATS_HISTORY_DAYS * 8);
		for (int index = 0; index < count; index++)
		{
			if (index > 0)
			{
				json << ", ";
			}
			json << "\"" << std::fixed << std::setprecision(precision) << pData[index] << "\"";
		}
	}
	json << "]";
	return utils::CreateString(json);
}

/*
@desc Returns the display type of a leaderboard.
@param hLeaderboard A leaderboard handle.
@return An ELeaderboardDisplayType value.
@url https://partner.steamgames.com/doc/api/ISteamUserStats#GetLeaderboardDisplayType
@url https://partner.steamgames.com/doc/api/ISteamUserStats#ELeaderboardDisplayType
*/
extern "C" DLL_EXPORT int GetLeaderboardDisplayType(int hLeaderboard)
{
	CheckInitialized(0);
	return SteamUserStats()->GetLeaderboardDisplayType(SteamHandles()->GetSteamHandle(hLeaderboard));
}

/*
@desc Returns the total number of entries in a leaderboard.
@param hLeaderboard A leaderboard handle.
@return The number of entries in the leaderboard.
@url https://partner.steamgames.com/doc/api/ISteamUserStats#GetLeaderboardEntryCount
*/
extern "C" DLL_EXPORT int GetLeaderboardEntryCount(int hLeaderboard)
{
	CheckInitialized(0);
	return SteamUserStats()->GetLeaderboardEntryCount(SteamHandles()->GetSteamHandle(hLeaderboard));
}

/*
@desc Returns the leaderboard name.
@param hLeaderboard A leaderboard handle.
@return The leaderboard name.
@url https://partner.steamgames.com/doc/api/ISteamUserStats#GetLeaderboardName
*/
extern "C" DLL_EXPORT char *GetLeaderboardName(int hLeaderboard)
{
	CheckInitialized(NULL_STRING);
	return utils::CreateString(SteamUserStats()->GetLeaderboardName(SteamHandles()->GetSteamHandle(hLeaderboard)));
}

/*
@desc Returns the sort order of a leaderboard.
@param hLeaderboard A leaderboard handle.
@return ELeaderboardSortMethod value.
@url https://partner.steamgames.com/doc/api/ISteamUserStats#GetLeaderboardSortMethod
@url https://partner.steamgames.com/doc/api/ISteamUserStats#ELeaderboardSortMethod
*/
extern "C" DLL_EXPORT int GetLeaderboardSortMethod(int hLeaderboard)
{
	CheckInitialized(0);
	return SteamUserStats()->GetLeaderboardSortMethod(SteamHandles()->GetSteamHandle(hLeaderboard));
}

/*
@desc Gets the info on the most achieved achievement for the game.
Use GetMostAchievedAchievementInfoName, GetMostAchievedAchievementInfoPercent, and GetMostAchievedAchievementInfoAchieved to get this info.

You must have called RequestGlobalAchievementPercentages and it needs to return successfully via its callback prior to calling this.

_Note: The plugin keeps track of the iterator mentioned in the Steamworks documentation.__

Usage:
```
if Steam.GetMostAchievedAchievementInfo()
repeat
Log("Name: " + Steam.GetMostAchievedAchievementInfoName() + ", percent: " + str(Steam.GetMostAchievedAchievementInfoPercent()) + ", unlocked: " + str(Steam.GetMostAchievedAchievementInfoUnlocked()))
until not Steam.GetNextMostAchievedAchievementInfo()
endif
```
@return 1 If achievement info was loaded; otherwise 0.
@url https://partner.steamgames.com/doc/api/ISteamUserStats#GetMostAchievedAchievementInfo
*/
extern "C" DLL_EXPORT int GetMostAchievedAchievementInfo()
{
	CheckInitialized(false);
	g_MostAchievedAchievementInfoIterator = SteamUserStats()->GetMostAchievedAchievementInfo(g_MostAchievedAchievementInfoName, ACH_NAME_MAX_LENGTH, &g_MostAchievedAchievementInfoPercent, &g_MostAchievedAchievementInfoUnlocked);
	if (g_MostAchievedAchievementInfoIterator == -1)
	{
		ClearMostAchievedAchievementInfo();
		return false;
	}
	return true;
}

/*
@desc Gets the info on the next most achieved achievement for the game.
Use GetMostAchievedAchievementInfoName, GetMostAchievedAchievementInfoPercent, and GetMostAchievedAchievementInfoAchieved to get this info.

You must have called RequestGlobalAchievementPercentages and it needs to return successfully via its callback prior to calling this.

_Note: The plugin keeps track of the iterator mentioned in the Steamworks documentation.__

Usage:
```
if Steam.GetMostAchievedAchievementInfo()
repeat
Log("Name: " + Steam.GetMostAchievedAchievementInfoName() + ", percent: " + str(Steam.GetMostAchievedAchievementInfoPercent()) + ", unlocked: " + str(Steam.GetMostAchievedAchievementInfoUnlocked()))
until not Steam.GetNextMostAchievedAchievementInfo()
endif
```
@return 1 If achievement info was loaded; otherwise 0.
@url https://partner.steamgames.com/doc/api/ISteamUserStats#GetNextMostAchievedAchievementInfo
*/
extern "C" DLL_EXPORT int GetNextMostAchievedAchievementInfo()
{
	CheckInitialized(false);
	if (g_MostAchievedAchievementInfoIterator == -1)
	{
		return false;
	}
	g_MostAchievedAchievementInfoIterator = SteamUserStats()->GetNextMostAchievedAchievementInfo(g_MostAchievedAchievementInfoIterator, g_MostAchievedAchievementInfoName, ACH_NAME_MAX_LENGTH, &g_MostAchievedAchievementInfoPercent, &g_MostAchievedAchievementInfoUnlocked);
	if (g_MostAchievedAchievementInfoIterator == -1)
	{
		ClearMostAchievedAchievementInfo();
		return false;
	}
	return true;
}

/*
@desc Returns the 'API Name' of the achievement as loaded by GetMostAchievedAchievementInfo or GetNextMostAchievedAchievementInfo.
@return 'API Name' of an achievement or an empty string.
@url https://partner.steamgames.com/doc/api/ISteamUserStats#GetMostAchievedAchievementInfo
@url https://partner.steamgames.com/doc/api/ISteamUserStats#GetNextMostAchievedAchievementInfo
*/
extern "C" DLL_EXPORT char *GetMostAchievedAchievementInfoName()
{
	return utils::CreateString(g_MostAchievedAchievementInfoName);
}

/*
@desc Returns the percentage of people that have unlocked this achievement from 0 to 100 as loaded by GetMostAchievedAchievementInfo or GetNextMostAchievedAchievementInfo.
@return A float.
@url https://partner.steamgames.com/doc/api/ISteamUserStats#GetMostAchievedAchievementInfo
@url https://partner.steamgames.com/doc/api/ISteamUserStats#GetNextMostAchievedAchievementInfo
*/
extern "C" DLL_EXPORT float GetMostAchievedAchievementInfoPercent()
{
	return g_MostAchievedAchievementInfoPercent;
}

/*
@desc Returns an integer indicating whether the current user has unlocked this achievement as loaded by GetMostAchievedAchievementInfo or GetNextMostAchievedAchievementInfo.
@return 1 if the current user has unlocked this achievement; otherwise 0;
@url https://partner.steamgames.com/doc/api/ISteamUserStats#GetMostAchievedAchievementInfo
@url https://partner.steamgames.com/doc/api/ISteamUserStats#GetNextMostAchievedAchievementInfo
*/
extern "C" DLL_EXPORT int GetMostAchievedAchievementInfoUnlocked()
{
	return g_MostAchievedAchievementInfoUnlocked;
}

/*
@desc Gets the number of achievements for the app.

_This method is generally just for testing purposes since the app should already know what the achievements are._
@return The number of achievements.
@url https://partner.steamgames.com/doc/api/ISteamUserStats#GetNumAchievements
*/
extern "C" DLL_EXPORT int GetNumAchievements()
{
	CheckInitialized(0);
	return SteamUserStats()->GetNumAchievements();
}

class CGetNumberOfCurrentPlayersCallResult : public CCallResultItem<NumberOfCurrentPlayers_t, SuccessResponse<NumberOfCurrentPlayers_t>>
{
public:
	CGetNumberOfCurrentPlayersCallResult()
	{
		m_CallResultName = "GetNumberOfCurrentPlayers()";
		m_hSteamAPICall = SteamUserStats()->GetNumberOfCurrentPlayers();
	}
	int GetNumberOfPlayers() { return m_Response.m_cPlayers; }
};

/*
@desc Asynchronously retrieves the total number of players currently playing the current game. Both online and in offline mode.
@callback-type callresult
@callback-getters GetNumberOfCurrentPlayersResult
@return A [call result handle](Callbacks-and-Call-Results#call-results) on success; otherwise 0.
@url https://partner.steamgames.com/doc/api/ISteamUserStats#GetNumberOfCurrentPlayers
@url https://partner.steamgames.com/doc/api/ISteamUserStats#NumberOfCurrentPlayers_t
*/
extern "C" DLL_EXPORT int GetNumberOfCurrentPlayers()
{
	CheckInitialized(0);
	return CallResults()->Add(new CGetNumberOfCurrentPlayersCallResult());
}

/*
@desc Returns the number of current players returned by the GetNumberOfCurrentPlayers call.
@param hCallResult A GetNumberOfCurrentPlayers call result.
@return An integer.
@url https://partner.steamgames.com/doc/api/ISteamUserStats#GetNumberOfCurrentPlayers
@url https://partner.steamgames.com/doc/api/ISteamUserStats#NumberOfCurrentPlayers_t
*/
extern "C" DLL_EXPORT int GetNumberOfCurrentPlayersResult(int hCallResult)
{
	return GetCallResultValue(hCallResult, &CGetNumberOfCurrentPlayersCallResult::GetNumberOfPlayers);
}

/*
@desc Gets the current value of an integer stat for the current user.
If the stat is not defined as an integer, an error will be raised.
@param name The 'API Name' of the stat.
@return The value of the stat.
@url https://partner.steamgames.com/doc/api/ISteamUserStats#GetStat
*/
extern "C" DLL_EXPORT int GetStatInt(const char *name)
{
	CheckInitialized(0);
	int result = 0;
	if (SteamUserStats()->GetStat(name, &result))
	{
		return result;
	}
	agk::PluginError("GetStat failed.");
	return 0;
}

/*
@desc Gets the current value of a float stat for the current user.
If the stat is not defined as a float, an error will be raised.
@param name The 'API Name' of the stat.
@return The value of the stat.
@url https://partner.steamgames.com/doc/api/ISteamUserStats#GetStat
*/
extern "C" DLL_EXPORT float GetStatFloat(const char *name)
{
	CheckInitialized(0.0);
	float result = 0.0;
	if (SteamUserStats()->GetStat(name, &result))
	{
		return result;
	}
	agk::PluginError("GetStat failed.");
	return 0.0;
}

//GetTrophySpaceRequiredBeforeInstall - deprecated

/*
@desc Gets whether the specified user has achieved this achievement.
@param hSteamID The Steam ID handle of the user.
@param name The 'API Name' of the achievement.
@return 1 when the user has achieved this achievement; otherwise 0.
@url https://partner.steamgames.com/doc/api/ISteamUserStats#GetUserAchievement
*/
extern "C" DLL_EXPORT int GetUserAchievement(int hSteamID, const char *name)
{
	CheckInitialized(false);
	bool result = false;
	if (SteamUserStats()->GetUserAchievement(SteamHandles()->GetSteamHandle(hSteamID), name, &result))
	{
		return result;
	}
	agk::PluginError("GetUserAchievement failed.");
	return false;
}

/*
@desc Gets the time at which an achievement was unlocked, if ever.
@param hSteamID The Steam ID handle of the user.
@param name The 'API Name' of the achievement.
@return The unload time in Unix time.
@url https://partner.steamgames.com/doc/api/ISteamUserStats#GetUserAchievementAndUnlockTime
*/
extern "C" DLL_EXPORT int GetUserAchievementUnlockTime(int hSteamID, const char *name)
{
	CheckInitialized(0);
	bool pbAchieved;
	uint32 punUnlockTime;
	if (SteamUserStats()->GetUserAchievementAndUnlockTime(SteamHandles()->GetSteamHandle(hSteamID), name, &pbAchieved, &punUnlockTime))
	{
		if (pbAchieved)
		{
			return punUnlockTime;
		}
	}
	return 0;
}

/*
@desc Gets the current value of an integer stat for the specified user.
If the stat is not defined as an integer, an error will be raised.
@param hSteamID The Steam ID handle of the user.
@param name The 'API Name' of the stat.
@return The value of the stat.
@url https://partner.steamgames.com/doc/api/ISteamUserStats#GetUserStat
*/
extern "C" DLL_EXPORT int GetUserStatInt(int hSteamID, const char *name)
{
	CheckInitialized(0);
	int result = 0;
	if (SteamUserStats()->GetUserStat(SteamHandles()->GetSteamHandle(hSteamID), name, &result))
	{
		return result;
	}
	agk::PluginError("GetUserStat failed.");
	return 0;
}

/*
@desc Gets the current value of a float stat for the specified user.
If the stat is not defined as a float, an error will be raised.
@param hSteamID The Steam ID handle of the user.
@param name The 'API Name' of the stat.
@return The value of the stat.
@url https://partner.steamgames.com/doc/api/ISteamUserStats#GetUserStat
*/
extern "C" DLL_EXPORT float GetUserStatFloat(int hSteamID, const char *name)
{
	CheckInitialized(0.0);
	float result = 0.0;
	if (SteamUserStats()->GetUserStat(SteamHandles()->GetSteamHandle(hSteamID), name, &result))
	{
		return result;
	}
	agk::PluginError("GetUserStat failed.");
	return 0.0;
}

//GetUserStatsData - deprecated

/*
@desc Raises a notification about achievemnt progress for progress stat achievements.
The notification only shows when current progress is less than the max.

SetStat still needs to be used to set the progress stat value.

Steamworks documentation suggests only using this at intervals in the progression rather than every step of the way.
ie: Every 25 wins out of 100.
@param name The 'API Name' of the achievement.
@param curProgress The current progress.
@param maxProgress The progress required to unlock the achievement.
@callbacks HasUserAchievementStoredResponse
@return 1 when the call succeeds; otherwise 0.
@url https://partner.steamgames.com/doc/api/ISteamUserStats#IndicateAchievementProgress
*/
extern "C" DLL_EXPORT int IndicateAchievementProgress(const char *name, int curProgress, int maxProgress)
{
	CheckInitialized(false);
	return SteamUserStats()->IndicateAchievementProgress(name, curProgress, maxProgress);
}

//InstallPS3Trophies - ignore

/*
@desc _[This command is initiates a callback.](Callbacks-and-Call-Results#callbacks)_

Sends a request for current user's stats to Steam.

_This command is called within Init so AppGameKit code will likely never need to call this command explicitly._
@return 1 when sending the request succeeds; otherwise 0.  This is not an indication of whether user stats are initialized.  See StatsInitialized.
@url https://partner.steamgames.com/doc/api/ISteamUserStats#RequestCurrentStats
*/
extern "C" DLL_EXPORT int RequestCurrentStats()
{
	CheckInitialized(false);
	Callbacks()->UserStatsReceived.Register();
	return SteamUserStats()->RequestCurrentStats();
}

class CRequestGlobalAchievementPercentagesCallResult : public CCallResultItem<GlobalAchievementPercentagesReady_t>
{
public:
	CRequestGlobalAchievementPercentagesCallResult()
	{
		m_CallResultName = "RequestGlobalAchievementPercentages()";
		m_hSteamAPICall = SteamUserStats()->RequestGlobalAchievementPercentages();
	}
protected:
	void OnResponse(GlobalAchievementPercentagesReady_t *pCallResult, bool bFailure)
	{
		if (pCallResult->m_nGameID != g_AppID)
		{
			utils::Log(GetName() + ":  Received result for another app id: " + std::to_string(pCallResult->m_nGameID));
			return;
		}
		CCallResultItem::OnResponse(pCallResult, bFailure);
	}
};

/*
@desc Asynchronously fetch the data for the percentage of players who have received each achievement for the current game globally.
@callback-type callresult
@callback-getters GetCallResultCode
@return A [call result handle](Callbacks-and-Call-Results#call-results) on success; otherwise 0.
@url https://partner.steamgames.com/doc/api/ISteamUserStats#RequestGlobalAchievementPercentages
@url https://partner.steamgames.com/doc/api/ISteamUserStats#GlobalAchievementPercentagesReady_t
*/
extern "C" DLL_EXPORT int RequestGlobalAchievementPercentages()
{
	CheckInitialized(0);
	return CallResults()->Add(new CRequestGlobalAchievementPercentagesCallResult());
}

class CRequestGlobalStatsCallResult : public CCallResultItem<GlobalStatsReceived_t>
{
public:
	CRequestGlobalStatsCallResult(int nHistoryDays)
	{
		m_CallResultName = "RequestGlobalStats(" + std::to_string(nHistoryDays) + ")";
		m_hSteamAPICall = SteamUserStats()->RequestGlobalStats(nHistoryDays);
	}
protected:
	void OnResponse(GlobalStatsReceived_t *pCallResult, bool bFailure)
	{
		if (pCallResult->m_nGameID != g_AppID)
		{
			utils::Log(GetName() + ":  Received result for another app id: " + std::to_string(pCallResult->m_nGameID));
			return;
		}
		CCallResultItem::OnResponse(pCallResult, bFailure);
	}
};

/*
@desc Asynchronously fetches global stats data, which is available for stats marked as "aggregated" in the App Admin panel of the Steamworks website.
@param historyDays How many days of day-by-day history to retrieve in addition to the overall totals. The limit is 60.
@callback-type callresult
@callback-getters GetCallResultCode
@return A [call result handle](Callbacks-and-Call-Results#call-results) on success; otherwise 0.
@url https://partner.steamgames.com/doc/api/ISteamUserStats#RequestGlobalStats
@url https://partner.steamgames.com/doc/api/ISteamUserStats#GlobalStatsReceived_t
*/
extern "C" DLL_EXPORT int RequestGlobalStats(int historyDays)
{
	CheckInitialized(0);
	return CallResults()->Add(new CRequestGlobalStatsCallResult(historyDays));
}

/*
@desc _[This command is initiates a callback.](Callbacks-and-Call-Results#callbacks)_

Sends a request for another user's stats to Steam.
@param hSteamID The Steam ID handle of the user to load stats for.
@return 1 when sending the request succeeds; otherwise 0.  This is not an indication of whether user stats are initialized.  See StatsInitialized.
@url https://partner.steamgames.com/doc/api/ISteamUserStats#RequestCurrentStats
*/
extern "C" DLL_EXPORT int RequestUserStats(int hSteamID)
{
	CheckInitialized(0);
	Callbacks()->UserStatsReceived.Register();
	Callbacks()->UserStatsUnloaded.Register();
	// This returns a SteamAPICall_t, but the UserStatsReceived_t callback also reports the result.
	// Using the existing callback rather than adding another call result to handle this.
	return SteamUserStats()->RequestUserStats(SteamHandles()->GetSteamHandle(hSteamID)) != k_uAPICallInvalid;
}

/*
@desc _[This command is initiates a callback.](Callbacks-and-Call-Results#callbacks)_

Resets user stats and optionally all achievements (when bAchievementsToo is 1).  This command also triggers the StoreStats callback.
@param achievementsToo When 1 then achievements are also cleared.
@return 1 when sending the request to clear user stats succeeds; otherwise 0.
@url https://partner.steamgames.com/doc/api/ISteamUserStats#ResetAllStats
*/
extern "C" DLL_EXPORT int ResetAllStats(int achievementsToo)
{
	CheckInitialized(false);
	Callbacks()->UserAchievementStored.Register();
	Callbacks()->UserStatsStored.Register();
	return SteamUserStats()->ResetAllStats(achievementsToo != 0);
}

/*
@desc Gives an achievement to the user.

Call StoreStats afterward to notify the user of the achievement.  Otherwise, they will be notified after the game exits.
@param name The 'API Name' of the achievement.
@return 1 when the call succeeds; otherwise 0.
@url https://partner.steamgames.com/doc/api/ISteamUserStats#SetAchievement
*/
extern "C" DLL_EXPORT int SetAchievement(const char *name)
{
	CheckInitialized(false);
	Callbacks()->UserAchievementStored.Register();
	return SteamUserStats()->SetAchievement(name);
}

/*
@desc Sets the value of an integer stat for the current user.
@param name The 'API Name' of the stat.
@param value The new value of the stat. This must be an absolute value, it will not increment or decrement for you.
@return 1 if setting the value succeeds; otherwise 0.
@url https://partner.steamgames.com/doc/api/ISteamUserStats#SetStat
*/
extern "C" DLL_EXPORT int SetStatInt(const char *name, int value)
{
	CheckInitialized(false);
	return SteamUserStats()->SetStat(name, value);
}

/*
@desc Sets the value of a float stat for the current user.
@param name The 'API Name' of the stat.
@param value The new value of the stat. This must be an absolute value, it will not increment or decrement for you.
@return 1 if setting the value succeeds; otherwise 0.
@url https://partner.steamgames.com/doc/api/ISteamUserStats#SetStat
*/
extern "C" DLL_EXPORT int SetStatFloat(const char *name, float value)
{
	CheckInitialized(false);
	return SteamUserStats()->SetStat(name, value);
}

//SetUserStatsData - deprecated

/*
@desc Stores user stats online.
@callbacks HasUserAchievementStoredResponse, HasUserStatsReceivedResponse
@return 1 when sending the request to store user stats succeeds; otherwise 0.
@url https://partner.steamgames.com/doc/api/ISteamUserStats#StoreStats
*/
extern "C" DLL_EXPORT int StoreStats()
{
	CheckInitialized(false);
	if (g_StoringStats)
	{
		return false;
	}
	g_StoringStats = true;
	Callbacks()->UserStatsStored.Register();
	return SteamUserStats()->StoreStats();
}

/*
@desc Returns whether stats are currently being stored.
@return 1 while stats are being stored; otherwise 0.
@url https://partner.steamgames.com/doc/api/ISteamUserStats#StoreStats
*/
extern "C" DLL_EXPORT int IsStoringStats()
{
	return (int)g_StoringStats;
}

/*
@desc Updates an AVGRATE stat with new values.  The value of these fields can be read with GetStatFloat.
@param name The 'API Name' of the stat.
@param countThisSession The value accumulation since the last call to this method.
@param sessionLength The amount of time in seconds since the last call to this method.
@return 1 if setting the stat succeeds; otherwise 0.
@url https://partner.steamgames.com/doc/api/ISteamUserStats#UpdateAvgRateStat
*/
extern "C" DLL_EXPORT int UpdateAvgRateStat(const char *name, float countThisSession, float sessionLength)
{
	CheckInitialized(false);
	return SteamUserStats()->UpdateAvgRateStat(name, countThisSession, (double)sessionLength);
}

class CUploadLeaderboardScoreCallResult : public CCallResultItem<LeaderboardScoreUploaded_t, SuccessResponse<LeaderboardScoreUploaded_t>>
{
public:
	CUploadLeaderboardScoreCallResult(SteamLeaderboard_t hLeaderboard, ELeaderboardUploadScoreMethod eLeaderboardUploadScoreMethod, int nScore)
	{
		m_CallResultName = "UploadLeaderboardScore("
			+ std::to_string(hLeaderboard) + ", "
			+ std::to_string(eLeaderboardUploadScoreMethod) + ", "
			+ std::to_string(nScore) + ")";
		m_hSteamAPICall = SteamUserStats()->UploadLeaderboardScore(hLeaderboard, eLeaderboardUploadScoreMethod, nScore, s_Details, s_DetailCount);
		// If the call succeeds, clear the details.
		if (m_hSteamAPICall != k_uAPICallInvalid)
		{
			s_DetailCount = 0;
		}
	}
	int GetLeaderboardScoreUploadedSuccess() { return m_Response.m_bSuccess; }
	uint64 GetLeaderboardScoreUploadedHandle() { return m_Response.m_hSteamLeaderboard; }
	int GetLeaderboardScoreUploadedScore() { return m_Response.m_nScore; }
	int GetLeaderboardScoreUploadedScoreChanged() { return m_Response.m_bScoreChanged; }
	int GetLeaderboardScoreUploadedRankNew() { return m_Response.m_nGlobalRankNew; }
	int GetLeaderboardScoreUploadedRankPrevious() { return m_Response.m_nGlobalRankPrevious; }
	static bool AddDetail(int32 detail)
	{
		if (s_DetailCount == k_cLeaderboardDetailsMax)
		{
			return false;
		}
		s_Details[s_DetailCount] = detail;
		s_DetailCount++;
		return true;
	}
protected:
	static int32 s_Details[k_cLeaderboardDetailsMax];
	static int32 s_DetailCount;
};

int32 CUploadLeaderboardScoreCallResult::s_Details[k_cLeaderboardDetailsMax];
int32 CUploadLeaderboardScoreCallResult::s_DetailCount = 0;

/*
@desc Adds a detail to be sent in the next UploadLeaderboardScore call.  A maximum of 64 details can be added.
Details are optional.
@param detail The detail element to add.
@return 1 if the detail was successfully added; otherwise 0 meaning that the maximum number of details has been reached.
@url https://partner.steamgames.com/doc/api/ISteamUserStats#UploadLeaderboardScore
*/
extern "C" DLL_EXPORT int AddUploadLeaderboardScoreDetail(int detail)
{
	return CUploadLeaderboardScoreCallResult::AddDetail(detail);
}

int UploadLeaderboardScore(int hLeaderboard, ELeaderboardUploadScoreMethod eLeaderboardUploadScoreMethod, int score)
{
	CheckInitialized(0);
	SteamLeaderboard_t leaderboard = SteamHandles()->GetSteamHandle(hLeaderboard);
	if (!leaderboard)
	{
		agk::PluginError("UploadLeaderboardScore: Invalid leaderboard handle.");
		return 0;
	}
	return CallResults()->Add(new CUploadLeaderboardScoreCallResult(leaderboard, eLeaderboardUploadScoreMethod, score));
}

/*
@desc Uploads a score to a leaderboard.  The leaderboard will keep the user's best score.
@param hLeaderboard A leaderboard handle.
@param score The score to upload.
@callback-type callresult
@callback-getters GetUploadLeaderboardScoreSuccess, GetUploadLeaderboardScoreHandle, GetUploadLeaderboardScoreValue,
GetUploadLeaderboardScoreChanged, GetUploadLeaderboardScoreRankNew, GetUploadLeaderboardScoreRankPrevious
@return A [call result handle](Callbacks-and-Call-Results#call-results) on success; otherwise 0.
@url https://partner.steamgames.com/doc/api/ISteamUserStats#UploadLeaderboardScore
@url https://partner.steamgames.com/doc/api/ISteamUserStats#ELeaderboardUploadScoreMethod
*/
extern "C" DLL_EXPORT int UploadLeaderboardScore(int hLeaderboard, int score)
{
	return UploadLeaderboardScore(hLeaderboard, k_ELeaderboardUploadScoreMethodKeepBest, score);
}

/*
@desc Uploads a score to a leaderboard.  Forces the server to accept the uploaded score and replace any existing score.
@param hLeaderboard A leaderboard handle.
@param score The score to upload.
@callback-type callresult
@callback-getters GetUploadLeaderboardScoreSuccess, GetUploadLeaderboardScoreHandle, GetUploadLeaderboardScoreValue,
GetUploadLeaderboardScoreChanged, GetUploadLeaderboardScoreRankNew, GetUploadLeaderboardScoreRankPrevious
@return A [call result handle](Callbacks-and-Call-Results#call-results) on success; otherwise 0.
@url https://partner.steamgames.com/doc/api/ISteamUserStats#UploadLeaderboardScore
@url https://partner.steamgames.com/doc/api/ISteamUserStats#ELeaderboardUploadScoreMethod
*/
extern "C" DLL_EXPORT int UploadLeaderboardScoreForceUpdate(int hLeaderboard, int score)
{
	return UploadLeaderboardScore(hLeaderboard, k_ELeaderboardUploadScoreMethodForceUpdate, score);
}

/*
@desc Returns the success of the UploadLeaderboardScore call.
@param hCallResult A UploadLeaderboardScore call result.
@return 1 on success; otherwise 0.
@url https://partner.steamgames.com/doc/api/ISteamUserStats#LeaderboardScoreUploaded_t
*/
extern "C" DLL_EXPORT int GetUploadLeaderboardScoreSuccess(int hCallResult)
{
	return GetCallResultValue(hCallResult, &CUploadLeaderboardScoreCallResult::GetLeaderboardScoreUploadedSuccess);
}

/*
@desc Returns the leaderboard handle of the UploadLeaderboardScore call.
@param hCallResult A UploadLeaderboardScore call result.
@return A Steam ID handle.
@url https://partner.steamgames.com/doc/api/ISteamUserStats#LeaderboardScoreUploaded_t
*/
extern "C" DLL_EXPORT int GetUploadLeaderboardScoreHandle(int hCallResult)
{
	return GetCallResultValue(hCallResult, &CUploadLeaderboardScoreCallResult::GetLeaderboardScoreUploadedHandle);
}

/*
@desc Returns the score of the UploadLeaderboardScore call.
@param hCallResult A UploadLeaderboardScore call result.
@return An integer.
@url https://partner.steamgames.com/doc/api/ISteamUserStats#LeaderboardScoreUploaded_t
*/
extern "C" DLL_EXPORT int GetUploadLeaderboardScoreValue(int hCallResult)
{
	return GetCallResultValue(hCallResult, &CUploadLeaderboardScoreCallResult::GetLeaderboardScoreUploadedScore);
}

/*
@desc Returns whether the score on the leaderboard changed for the UploadLeaderboardScore call.
@param hCallResult A UploadLeaderboardScore call result.
@return 1 if the score on the leaderboard changed; otherwise 0.
@url https://partner.steamgames.com/doc/api/ISteamUserStats#LeaderboardScoreUploaded_t
*/
extern "C" DLL_EXPORT int GetUploadLeaderboardScoreChanged(int hCallResult)
{
	return GetCallResultValue(hCallResult, &CUploadLeaderboardScoreCallResult::GetLeaderboardScoreUploadedScoreChanged);
}

/*
@desc Returns the new rank of the user on the leaderboard for the UploadLeaderboardScore call.
@param hCallResult A UploadLeaderboardScore call result.
@return The user's new rank.
@url https://partner.steamgames.com/doc/api/ISteamUserStats#LeaderboardScoreUploaded_t
*/
extern "C" DLL_EXPORT int GetUploadLeaderboardScoreRankNew(int hCallResult)
{
	return GetCallResultValue(hCallResult, &CUploadLeaderboardScoreCallResult::GetLeaderboardScoreUploadedRankNew);
}

/*
@desc Returns the previous rank of the user on the leaderboard of the UploadLeaderboardScore call.
@param hCallResult A UploadLeaderboardScore call result.
@return The user's previous rank or 0 if the user had no previous rank.
@url https://partner.steamgames.com/doc/api/ISteamUserStats#LeaderboardScoreUploaded_t
*/
extern "C" DLL_EXPORT int GetUploadLeaderboardScoreRankPrevious(int hCallResult)
{
	return GetCallResultValue(hCallResult, &CUploadLeaderboardScoreCallResult::GetLeaderboardScoreUploadedRankPrevious);
}

//Callbacks
//GlobalAchievementPercentagesReady_t - Call Result
//GlobalStatsReceived_t - Call Result
//LeaderboardFindResult_t - Call result for FindOrCreateLeaderboard, FindLeaderboard
//LeaderboardScoresDownloaded_t - Call result for DownloadLeaderboardEntries, DownloadLeaderboardEntriesForUsers
//LeaderboardScoreUploaded_t - Call result for UploadLeaderboardScore
//LeaderboardUGCSet_t - Call result for AttachLeaderboardUGC
//NumberOfCurrentPlayers_t - Call result
//PS3TrophiesInstalled_t - ignore

/*
@desc Triggered by request to store the achievements on the server, or an "indicate progress" call.
@callback-type list
@callback-getters GetUserAchievementStoredName, GetUserAchievementStoredCurrentProgress, GetUserAchievementStoredMaxProgress
@return 1 when achievements have been stored online; otherwise 0.
@url https://partner.steamgames.com/doc/api/ISteamUserStats#UserAchievementStored_t
*/
extern "C" DLL_EXPORT int HasUserAchievementStoredResponse()
{
	return Callbacks()->UserAchievementStored.HasResponse();
}

// Unused
//int GetUserAchievementStoredIsGroup()
//{
//	return Callbacks()->GetUserAchievementStored().m_bGroupAchievement;
//}

/*
@desc Returns the name of the achievement for the current UserAchievementStored_t callback response.
@return The achievement name.
@url https://partner.steamgames.com/doc/api/ISteamUserStats#UserAchievementStored_t
*/
extern "C" DLL_EXPORT char *GetUserAchievementStoredName()
{
	return utils::CreateString(Callbacks()->UserAchievementStored.GetCurrent().m_rgchAchievementName);
}

/*
@desc Returns the current progress of the achievement for the current UserAchievementStored_t callback response.
@return An integer.
@url https://partner.steamgames.com/doc/api/ISteamUserStats#UserAchievementStored_t
*/
extern "C" DLL_EXPORT int GetUserAchievementStoredCurrentProgress()
{
	return (int)Callbacks()->UserAchievementStored.GetCurrent().m_nCurProgress;
}

/*
@desc Returns the maximum progress of the achievement for the current UserAchievementStored_t callback response.
@return An integer.
@url https://partner.steamgames.com/doc/api/ISteamUserStats#UserAchievementStored_t
*/
extern "C" DLL_EXPORT int GetUserAchievementStoredMaxProgress()
{
	return (int)Callbacks()->UserAchievementStored.GetCurrent().m_nMaxProgress;
}

/*
@desc Triggered when the latest stats and achievements for a specific user (including the local user) have been received from the server.
@return 1 when the callback has more responses to process; otherwise 0.
@callback-type list
@callback-getters GetUserStatsReceivedResult, GetUserStatsReceivedUser
@url https://partner.steamgames.com/doc/api/ISteamUserStats#UserStatsReceived_t
*/
extern "C" DLL_EXPORT int HasUserStatsReceivedResponse()
{
	return Callbacks()->UserStatsReceived.HasResponse();
}

// This plugin only reports for the current app id.
//int GetUserStatsReceivedGameAppID()
//{
//	return ((CGameID)(int)Callbacks()->GetUserStatsReceived().m_nGameID).AppID();
//}

/*
@desc Returns whether the call was successful for the current UserStatsReceived_t callback response.
@return 1 when the call is successful; otherwise 0.
@url https://partner.steamgames.com/doc/api/ISteamUserStats#UserStatsReceived_t
*/
extern "C" DLL_EXPORT int GetUserStatsReceivedResult()
{
	return (int)Callbacks()->UserStatsReceived.GetCurrent().m_eResult;
}

/*
@desc Returns the user whose stats were retrieved for the current UserStatsReceived_t callback response.
@return A Steam ID handle.
@url https://partner.steamgames.com/doc/api/ISteamUserStats#UserStatsReceived_t
*/
extern "C" DLL_EXPORT int GetUserStatsReceivedUser()
{
	return SteamHandles()->GetPluginHandle(Callbacks()->UserStatsReceived.GetCurrent().m_steamIDUser);
}

/*
@desc Checks to see whether user stats have been initialized after a RequestCurrentStats call.
@return 1 when users stats are initialized; otherwise 0.
*/
extern "C" DLL_EXPORT int StatsInitialized()
{
	return Callbacks()->StatsInitialized();
}

/*
@desc Checks to see whether a user's stats have been initialized after a RequestUserStats call.
@param hSteamID The Steam ID handle of the user to check.
@return 1 when the user's stats are initialized; otherwise 0.
@plugin-name StatsInitialized
*/
extern "C" DLL_EXPORT int StatsInitializedForUser(int hSteamID)
{
	return Callbacks()->StatsInitializedForUser(SteamHandles()->GetSteamHandle(hSteamID));
}

/*
@desc Triggered when the stats and achievements for a user have been unloaded.
@return 1 when the callback has more responses to process; otherwise 0.
@callback-type list
@callback-getters GetUserStatsUnloadedUser
@url https://partner.steamgames.com/doc/api/ISteamUserStats#UserStatsUnloaded_t
*/
extern "C" DLL_EXPORT int HasUserStatsUnloadedResponse()
{
	return Callbacks()->UserStatsUnloaded.HasResponse();
}

/*
@desc Returns the user whose stats were unloaded for the current UserStatsUnloaded_t callback response.
@return A Steam ID handle.
@url https://partner.steamgames.com/doc/api/ISteamUserStats#UserStatsUnloaded_t
*/
extern "C" DLL_EXPORT int GetUserStatsUnloadedUser()
{
	return SteamHandles()->GetPluginHandle(Callbacks()->UserStatsUnloaded.GetCurrent());
}

/*
@desc Triggered by a request to store the user stats.
@callback-type list
@callback-getters GetUserStatsStoredResult
@return 1 when users stats have been stored online; otherwise 0.
@url https://partner.steamgames.com/doc/api/ISteamUserStats#UserStatsStored_t
*/
extern "C" DLL_EXPORT int HasUserStatsStoredResponse()
{
	if (Callbacks()->UserStatsStored.HasResponse())
	{
		g_StoringStats = false;
		return true;
	}
	return false;
}

/*
@desc Returns whether the call was successful for the current UserStatsStored_t callback response.
@return 1 when the call is successful; otherwise 0.
@url https://partner.steamgames.com/doc/api/ISteamUserStats#UserStatsStored_t
*/
extern "C" DLL_EXPORT int GetUserStatsStoredResult()
{
	return (int)Callbacks()->UserStatsStored.GetCurrent().m_eResult;
}
