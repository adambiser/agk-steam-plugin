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

#ifndef DLLMAIN_H_
#define DLLMAIN_H_

#include "..\AGKLibraryCommands.h"

/*
NOTE: Cannot use bool as an exported function return type because of AGK2 limitations.  Use int instead.
*/

// These match ECallbackState in SteamPlugin.h
#define STATE_SERVER_ERROR	-2
#define STATE_CLIENT_ERROR	-1
#define STATE_IDLE			0
#define STATE_RUNNING		1
#define STATE_DONE			2

// General methods.
extern "C" DLL_EXPORT int Init();
extern "C" DLL_EXPORT void Shutdown();
extern "C" DLL_EXPORT int SteamInitialized();
extern "C" DLL_EXPORT int RestartAppIfNecessary(int unOwnAppID);
extern "C" DLL_EXPORT int GetAppID();
extern "C" DLL_EXPORT int LoggedOn();
extern "C" DLL_EXPORT char *GetPersonaName();
extern "C" DLL_EXPORT void RunCallbacks();
// General user stats methods.
extern "C" DLL_EXPORT int RequestStats();
extern "C" DLL_EXPORT int GetRequestStatsCallbackState();
extern "C" DLL_EXPORT int StatsInitialized();
extern "C" DLL_EXPORT int StoreStats();
extern "C" DLL_EXPORT int ResetAllStats(int bAchievementsToo);
extern "C" DLL_EXPORT int GetStoreStatsCallbackState();
extern "C" DLL_EXPORT int StatsStored();
extern "C" DLL_EXPORT int AchievementStored();
// Achievements methods.
extern "C" DLL_EXPORT int GetNumAchievements();
extern "C" DLL_EXPORT char *GetAchievementID(int index);
extern "C" DLL_EXPORT char *GetAchievementDisplayName(const char *pchName);
extern "C" DLL_EXPORT char *GetAchievementDisplayDesc(const char *pchName);
extern "C" DLL_EXPORT int GetAchievementDisplayHidden(const char *pchName);
extern "C" DLL_EXPORT int GetAchievement(const char *pchName);
extern "C" DLL_EXPORT int SetAchievement(const char *pchName);
extern "C" DLL_EXPORT int IndicateAchievementProgress(const char *pchName, int nCurProgress, int nMaxProgress);
extern "C" DLL_EXPORT int ClearAchievement(const char *pchName);
// User stats methods.
extern "C" DLL_EXPORT int GetStatInt(const char *pchName);
extern "C" DLL_EXPORT float GetStatFloat(const char *pchName);
extern "C" DLL_EXPORT int SetStatInt(const char *pchName, int nData);
extern "C" DLL_EXPORT int SetStatFloat(const char *pchName, float fData);
extern "C" DLL_EXPORT int UpdateAvgRateStat(const char *pchName, float flCountThisSession, float dSessionLength);
// Leaderboards
extern "C" DLL_EXPORT int FindLeaderboard(const char *pchLeaderboardName);
extern "C" DLL_EXPORT int GetFindLeaderboardCallbackState();
extern "C" DLL_EXPORT int GetLeaderboardHandle();
extern "C" DLL_EXPORT char *GetLeaderboardName(int hLeaderboard);
extern "C" DLL_EXPORT int GetLeaderboardEntryCount(int hLeaderboard);
extern "C" DLL_EXPORT int GetLeaderboardDisplayType(int hLeaderboard);
extern "C" DLL_EXPORT int GetLeaderboardSortMethod(int hLeaderboard);
extern "C" DLL_EXPORT int UploadLeaderboardScore(int hLeaderboard, int score);
extern "C" DLL_EXPORT int UploadLeaderboardScoreForceUpdate(int hLeaderboard, int score);
extern "C" DLL_EXPORT int GetUploadLeaderboardScoreCallbackState();
extern "C" DLL_EXPORT int LeaderboardScoreStored();
extern "C" DLL_EXPORT int LeaderboardScoreChanged();
extern "C" DLL_EXPORT int GetLeaderboardUploadedScore();
extern "C" DLL_EXPORT int GetLeaderboardGlobalRankNew();
extern "C" DLL_EXPORT int GetLeaderboardGlobalRankPrevious();
// https://partner.steamgames.com/doc/api/ISteamUserStats#ELeaderboardDataRequest
extern "C" DLL_EXPORT int DownloadLeaderboardEntries(int hLeaderboard, int eLeaderboardDataRequest, int nRangeStart, int nRangeEnd);
extern "C" DLL_EXPORT int GetDownloadLeaderboardEntriesCallbackState();
extern "C" DLL_EXPORT int GetDownloadedLeaderboardEntryCount();
extern "C" DLL_EXPORT int GetDownloadedLeaderboardEntryGlobalRank(int index);
extern "C" DLL_EXPORT int GetDownloadedLeaderboardEntryScore(int index);
extern "C" DLL_EXPORT char *GetDownloadedLeaderboardEntryPersonaName(int index);
extern "C" DLL_EXPORT int GetDownloadedLeaderboardEntryAvatar(int index, int size);
// Avatars and images
extern "C" DLL_EXPORT int GetAvatar(int size);
extern "C" DLL_EXPORT int LoadImageFromHandle(int hImage);
extern "C" DLL_EXPORT void LoadImageIDFromHandle(int imageID, int hImage);
extern "C" DLL_EXPORT int GetAchievementIcon(const char *pchName);

// Error reporting methods.
//extern "C" DLL_EXPORT void SetErrorMode(int mode);
//extern "C" DLL_EXPORT int HadError();
//extern "C" DLL_EXPORT char *GetLastError();
//extern "C" DLL_EXPORT char *ClearError();

#endif // DLLMAIN_H_