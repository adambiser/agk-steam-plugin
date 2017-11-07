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

char *GetPersonaName()
{
	if (Steam)
	{
		return CreateString(Steam->GetPersonaName());
	}
	return CreateString(0);
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

int RequestingStats()
{
	if (Steam)
	{
		return Steam->RequestingStats();
	}
	return false;
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

int StoringStats()
{
	if (Steam)
	{
		return Steam->StoringStats();
	}
	return false;
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
char* GetAchievementID(int index)
{
	if (Steam)
	{
		return CreateString(Steam->GetAchievementID(index));
	}
	return CreateString(0);
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

int FindingLeaderboard()
{
	if (Steam)
	{
		return Steam->FindingLeaderboard();
	}
	return false;
}

int GetLeaderboardHandle()
{
	if (Steam)
	{
		return (int) Steam->GetLeaderboardHandle();
	}
	return 0;
}

int UploadLeaderboardScore(int hLeaderboard, int score)
{
	if (Steam)
	{
		return Steam->UploadLeaderboardScore((SteamLeaderboard_t) hLeaderboard, score);
	}
	return false;
}

int UploadingLeaderboardScore()
{
	if (Steam)
	{
		return Steam->UploadingLeaderboardScore();
	}
	return false;
}

int LeaderboardScoreStored()
{
	if (Steam)
	{
		return Steam->LeaderboardScoreStored();
	}
	return false;
}

int DownloadLeaderboardEntries(int hLeaderboard, int eLeaderboardDataRequest, int nRangeStart, int nRangeEnd)
{
	if (Steam)
	{
		return Steam->DownloadLeaderboardEntries(hLeaderboard, (ELeaderboardDataRequest) eLeaderboardDataRequest, nRangeStart, nRangeEnd);
	}
	return false;
}

int DownloadingLeaderboardEntries()
{
	if (Steam)
	{
		return Steam->DownloadingLeaderboardEntries();
	}
	return false;
}

int GetNumLeaderboardEntries()
{
	if (Steam)
	{
		return Steam->GetNumLeaderboardEntries();
	}
	return 0;
}

int GetLeaderboardEntryGlobalRank(int index)
{
	if (Steam)
	{
		return Steam->GetLeaderboardEntryGlobalRank(index);
	}
	return 0;
}

int GetLeaderboardEntryScore(int index)
{
	if (Steam)
	{
		return Steam->GetLeaderboardEntryScore(index);
	}
	return 0;
}

char *GetLeaderboardEntryPersonaName(int index)
{
	if (Steam)
	{
		return CreateString(Steam->GetLeaderboardEntryPersonaName(index));
	}
	return CreateString(0);
}
