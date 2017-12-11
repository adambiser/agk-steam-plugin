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
#include <stdio.h>
#include <steam_api.h>
#include "SteamPlugin.h"
#include <string>
#include "..\AGKLibraryCommands.h"

#pragma comment(lib, "steam_api.lib")

SteamPlugin::SteamPlugin() :
	m_AppID(0),
	m_SteamInitialized(false),
	//m_AvatarCallbackState(Idle),
	m_RequestStatsCallbackState(Idle),
	m_StatsInitialized(false),
	m_StoreStatsCallbackState(Idle),
	m_StatsStored(false),
	m_AchievementStored(false),
	m_hSteamLeaderboard(0),
	m_FindLeaderboardCallbackState(Idle),
	m_UploadLeaderboardScoreCallbackState(Idle),
	m_LeaderboardScoreStored(false),
	m_LeaderboardScoreChanged(false),
	m_LeaderboardUploadedScore(0),
	m_LeaderboardGlobalRankNew(0),
	m_LeaderboardGlobalRankPrevious(0),
	m_DownloadLeaderboardEntriesCallbackState(Idle),
	m_DownloadedLeaderboardEntryCount(0),
	m_CallbackAchievementStored(this, &SteamPlugin::OnAchievementStored),
	m_CallbackUserStatsReceived(this, &SteamPlugin::OnUserStatsReceived),
	m_CallbackUserStatsStored(this, &SteamPlugin::OnUserStatsStored),
	m_CallbackAvatarImageLoaded(this, &SteamPlugin::OnAvatarImageLoaded),
	m_CallbackAchievementIconFetched(this, &SteamPlugin::OnAchievementIconFetched),
	m_LobbyMatchListCallbackState(Idle),
	m_LobbyMatchListCount(0)
{
	// Nothing for now
}

SteamPlugin::~SteamPlugin(void)
{
	this->Shutdown();
}

bool SteamPlugin::Init()
{
	if (!m_SteamInitialized)
	{
		m_SteamInitialized = SteamAPI_Init();
		if (m_SteamInitialized)
		{
			m_AppID = SteamUtils()->GetAppID();
		}
	}
	return m_SteamInitialized;
}

void SteamPlugin::Shutdown()
{
	if (m_SteamInitialized)
	{
		SteamAPI_Shutdown();
		// Reset member variables.
		m_AppID = 0;
		m_SteamInitialized = false;
		//m_AvatarCallbackState = Idle;
		m_RequestStatsCallbackState = Idle;
		m_StatsInitialized = false;
		m_StoreStatsCallbackState = Idle;
		m_StatsStored = false;
		m_AchievementStored = false;
		m_hSteamLeaderboard = 0;
		m_FindLeaderboardCallbackState = Idle;
		m_UploadLeaderboardScoreCallbackState = Idle;
		m_LeaderboardScoreStored = false;
		m_LeaderboardScoreChanged = false;
		m_LeaderboardUploadedScore = 0;
		m_LeaderboardGlobalRankNew = 0;
		m_LeaderboardGlobalRankPrevious = 0;
		m_DownloadLeaderboardEntriesCallbackState = Idle;
		m_DownloadedLeaderboardEntryCount = 0;
		m_LobbyMatchListCallbackState = Idle;
		m_LobbyMatchListCount = 0;
	}
}

bool SteamPlugin::RestartAppIfNecessary(uint32 unOwnAppID)
{
	return SteamAPI_RestartAppIfNecessary(unOwnAppID);
}

int SteamPlugin::GetAppID()
{
	if (!m_SteamInitialized)
	{
		return 0;
	}
	return SteamUtils()->GetAppID();
}

bool SteamPlugin::LoggedOn()
{
	if (!m_SteamInitialized)
	{
		return false;
	}
	return SteamUser()->BLoggedOn();
}

void SteamPlugin::RunCallbacks()
{
	if (!m_SteamInitialized)
	{
		return;
	}
	SteamAPI_RunCallbacks();
}

void SteamPlugin::ActivateGameOverlay(const char *pchDialog)
{
	if (!m_SteamInitialized)
	{
		return;
	}
	SteamFriends()->ActivateGameOverlay(pchDialog);
}

void SteamPlugin::OnAvatarImageLoaded(AvatarImageLoaded_t *pParam)
{
	//avatarsMap.insert_or_assign(pParam->m_steamID, pParam->m_iImage);
	// Only store results that are expected.
	auto search = avatarsMap.find(pParam->m_steamID);
	if (search != avatarsMap.end())
	{
		search->second = pParam->m_iImage;
	}
}

int SteamPlugin::GetAvatar(EAvatarSize size)
{
	if (!m_SteamInitialized)
	{
		return 0;
	}
	return GetFriendAvatar(SteamUser()->GetSteamID(), size);
}

int SteamPlugin::GetFriendAvatar(CSteamID steamID, EAvatarSize size)
{
	if (!m_SteamInitialized)
	{
		return 0;
	}
	auto search = avatarsMap.find(steamID);
	if (search != avatarsMap.end())
	{
		// If we have got a result from the callback, remove it from the wait list.
		if ((search->second) != -1)
		{
			avatarsMap.erase(search);
		}
		return search->second;
	}
	int hImage = 0;
	switch (size)
	{
	case Small:
		hImage = SteamFriends()->GetSmallFriendAvatar(steamID);
		break;
	case Medium:
		hImage = SteamFriends()->GetMediumFriendAvatar(steamID);
		break;
	case Large:
		hImage = SteamFriends()->GetLargeFriendAvatar(steamID);
		break;
	default:
		agk::PluginError("Requested invalid avatar size.");
		return 0;
	}
	if (hImage == -1)
	{
		avatarsMap.insert_or_assign(steamID, hImage);
	}
	return hImage;
}

const char *SteamPlugin::GetPersonaName()
{
	if (!m_SteamInitialized)
	{
		return 0;
	}
	return SteamFriends()->GetPersonaName();
}

const char *SteamPlugin::GetFriendPersonaName(CSteamID steamID)
{
	if (!m_SteamInitialized)
	{
		return 0;
	}
	return SteamFriends()->GetFriendPersonaName(steamID);
}

int SteamPlugin::LoadImageFromHandle(int hImage)
{
	return SteamPlugin::LoadImageFromHandle(0, hImage);
}

int SteamPlugin::LoadImageFromHandle(int imageID, int hImage)
{
	if (hImage == -1 || hImage == 0)
	{
		return 0;
	}
	uint32 width;
	uint32 height;
	if (!SteamUtils()->GetImageSize(hImage, &width, &height))
	{
		agk::PluginError("GetImageSize failed.");
		return 0;
	}
	// Get the actual raw RGBA data from Steam and turn it into a texture in our game engine
	const int imageSizeInBytes = width * height * 4;
	uint8 *imageBuffer = new uint8[imageSizeInBytes];
	if (SteamUtils()->GetImageRGBA(hImage, imageBuffer, imageSizeInBytes))
	{
		unsigned int memID = agk::CreateMemblock(imageSizeInBytes + 4 * 3);
		agk::SetMemblockInt(memID, 0, width);
		agk::SetMemblockInt(memID, 4, height);
		agk::SetMemblockInt(memID, 8, 32); // bitdepth always 32
		for (int index = 0, offset = 12; index < imageSizeInBytes; index++, offset++)
		{
			agk::SetMemblockByte(memID, offset, imageBuffer[index]);
		}
		if (imageID)
		{
			agk::CreateImageFromMemblock(imageID, memID);
		}
		else
		{
			imageID = agk::CreateImageFromMemblock(memID);
		}
		agk::DeleteMemblock(memID);
	}
	else
	{
		imageID = 0;
		agk::PluginError("GetImageRGBA failed.");
	}
	// Free memory.
	delete[] imageBuffer;
	return imageID;
}

// Callback for RequestStats.
void SteamPlugin::OnUserStatsReceived(UserStatsReceived_t *pCallback)
{
	if (pCallback->m_nGameID == m_AppID)
	{
		if (pCallback->m_eResult == k_EResultOK)
		{
			m_RequestStatsCallbackState = Done;
			m_StatsInitialized = true;
		}
		else
		{
			m_RequestStatsCallbackState = ServerError;
		}
	}
}

bool SteamPlugin::RequestStats()
{
	// Set to false so that we can detect when the callback finishes for this request.
	m_StatsInitialized = false;
	if (NULL == SteamUserStats() || NULL == SteamUser())
	{
		return false;
	}
	if (!SteamUser()->BLoggedOn())
	{
		return false;
	}
	// Fail when the callback is currently running.
	if (m_RequestStatsCallbackState == Running)
	{
		return false;
	}
	if (!SteamUserStats()->RequestCurrentStats())
	{
		m_RequestStatsCallbackState = ClientError;
		return false;
	}
	m_RequestStatsCallbackState = Running;
	return true;
}

// Callback for StoreStats and ResetAllStats.
void SteamPlugin::OnUserStatsStored(UserStatsStored_t *pCallback)
{
	if (pCallback->m_nGameID == m_AppID)
	{
		if (pCallback->m_eResult == k_EResultOK)
		{
			m_StoreStatsCallbackState = Done;
			m_StatsStored = true;
		}
		else
		{
			m_StoreStatsCallbackState = ServerError;
		}
	}
}

// Callback for StoreStats.
void SteamPlugin::OnAchievementStored(UserAchievementStored_t *pCallback)
{
	if (pCallback->m_nGameID == m_AppID)
	{
		m_AchievementStored = true;
	}
}

bool SteamPlugin::StoreStats()
{
	if (!m_StatsInitialized)
	{
		return false;
	}
	// Fail when the callback is currently running.
	if (m_StoreStatsCallbackState == Running)
	{
		return false;
	}
	m_StatsStored = false;
	m_AchievementStored = false;
	if (!SteamUserStats()->StoreStats())
	{
		m_StoreStatsCallbackState = ClientError;
		return false;
	}
	m_StoreStatsCallbackState = Running;
	return true;
}

bool SteamPlugin::ResetAllStats(bool bAchievementsToo)
{
	if (!m_StatsInitialized)
	{
		return false;
	}
	// Fail when the callback is currently running.
	if (m_StoreStatsCallbackState == Running)
	{
		return false;
	}
	m_StatsStored = false;
	m_AchievementStored = false;
	if (!SteamUserStats()->ResetAllStats(bAchievementsToo))
	{
		m_StoreStatsCallbackState = ClientError;
		return false;
	}
	m_StoreStatsCallbackState = Running;
	return true;
}

int SteamPlugin::GetNumAchievements()
{
	if (!m_StatsInitialized)
	{
		return 0;
	}
	return SteamUserStats()->GetNumAchievements();
}

const char *SteamPlugin::GetAchievementID(int index)
{
	if (!m_StatsInitialized)
	{
		return 0;
	}
	return SteamUserStats()->GetAchievementName(index);
}

const char *SteamPlugin::GetAchievementDisplayAttribute(const char *pchName, const char *pchKey)
{
	if (!m_StatsInitialized)
	{
		return 0;
	}
	return SteamUserStats()->GetAchievementDisplayAttribute(pchName, pchKey);
}

// Callback for GetAchievementIcon.
void SteamPlugin::OnAchievementIconFetched(UserAchievementIconFetched_t *pParam)
{
	if (pParam->m_nGameID.AppID() == m_AppID)
	{
		// Only store the results for values that are expected.
		std::string name = pParam->m_rgchAchievementName;
		auto search = achievementIconsMap.find(name);
		if (search != achievementIconsMap.end())
		{
			search->second = pParam->m_nIconHandle;
		}
	}
}

/*
In the Steam API, GetAchievementIcon reports failure and "no icon" the same way.
Instead this checks some failure conditions so that it can return 0 for failure and -1 when waiting on a callback.
*/
int SteamPlugin::GetAchievementIcon(const char *pchName)
{
	if (!m_StatsInitialized)
	{
		return 0;
	}
	// See if we're waiting for a callback result for this icon.
	std::string name = pchName;
	auto search = achievementIconsMap.find(name);
	if (search != achievementIconsMap.end())
	{
		// If we have a result from the callback, remove it from the wait list.
		if ((search->second) != -1)
		{
			achievementIconsMap.erase(search);
		}
		return search->second;
	}
	int hImage = SteamUserStats()->GetAchievementIcon(pchName);
	if (hImage == 0)
	{
		// Check to see if this is a valid achievement name.
		bool pbAchieved;
		if (!SteamUserStats()->GetAchievement(pchName, &pbAchieved))
		{
			// Not valid, return 0 for no icon.
			return 0;
		}
		hImage = -1; // Use -1 to indicate that we're waiting on a callback to provide the handle.
		achievementIconsMap.insert_or_assign(name, hImage);
	}
	return hImage;
}

bool SteamPlugin::GetAchievement(const char *pchName, bool *pbAchieved)
{
	if (!m_StatsInitialized)
	{
		return false;
	}
	return SteamUserStats()->GetAchievement(pchName, pbAchieved);
}

bool SteamPlugin::SetAchievement(const char *pchName)
{
	if (!m_StatsInitialized)
	{
		return false;
	}
	return SteamUserStats()->SetAchievement(pchName);
}

bool SteamPlugin::GetAchievementAndUnlockTime(const char *pchName, bool *pbAchieved, uint32 *punUnlockTime)
{
	if (!m_StatsInitialized)
	{
		return false;
	}
	return SteamUserStats()->GetAchievementAndUnlockTime(pchName, pbAchieved, punUnlockTime);
}

bool SteamPlugin::IndicateAchievementProgress(const char *pchName, uint32 nCurProgress, uint32 nMaxProgress)
{
	if (!m_StatsInitialized)
	{
		return false;
	}
	return SteamUserStats()->IndicateAchievementProgress(pchName, nCurProgress, nMaxProgress);
}

bool SteamPlugin::ClearAchievement(const char *pchName)
{
	if (!m_StatsInitialized)
	{
		return false;
	}
	return SteamUserStats()->ClearAchievement(pchName);
}

bool SteamPlugin::GetStat(const char *pchName, int32 *pData)
{
	if (!m_StatsInitialized)
	{
		return false;
	}
	return SteamUserStats()->GetStat(pchName, pData);
}

bool SteamPlugin::GetStat(const char *pchName, float *pData)
{
	if (!m_StatsInitialized)
	{
		return false;
	}
	return SteamUserStats()->GetStat(pchName, pData);
}

bool SteamPlugin::SetStat(const char *pchName, int32 nData)
{
	if (!m_StatsInitialized)
	{
		return false;
	}
	return SteamUserStats()->SetStat(pchName, nData);
}

bool SteamPlugin::SetStat(const char *pchName, float fData)
{
	if (!m_StatsInitialized)
	{
		return false;
	}
	return SteamUserStats()->SetStat(pchName, fData);
}

bool SteamPlugin::UpdateAvgRateStat(const char *pchName, float flCountThisSession, double dSessionLength)
{
	if (!m_StatsInitialized)
	{
		return false;
	}
	return SteamUserStats()->UpdateAvgRateStat(pchName, flCountThisSession, dSessionLength);
}

// Callback for FindLeaderboard.
void SteamPlugin::OnFindLeaderboard(LeaderboardFindResult_t *pCallback, bool bIOFailure)
{
	if (pCallback->m_bLeaderboardFound && !bIOFailure)
	{
		m_FindLeaderboardCallbackState = Done;
		m_hSteamLeaderboard = pCallback->m_hSteamLeaderboard;
	}
	else
	{
		// Did not find the leaderboard.
		m_FindLeaderboardCallbackState = ServerError;
	}
}

bool SteamPlugin::FindLeaderboard(const char *pchLeaderboardName)
{
	if (!m_StatsInitialized)
	{
		return false;
	}
	// Fail when the callback is currently running.
	if (m_FindLeaderboardCallbackState == Running)
	{
		return false;
	}
	try
	{
		m_hSteamLeaderboard = 0;
		SteamAPICall_t hSteamAPICall = SteamUserStats()->FindLeaderboard(pchLeaderboardName);
		m_CallResultFindLeaderboard.Set(hSteamAPICall, this, &SteamPlugin::OnFindLeaderboard);
		m_FindLeaderboardCallbackState = Running;
		return true;
	}
	catch (...)
	{
		m_FindLeaderboardCallbackState = ClientError;
		return false;
	}
}

const char * SteamPlugin::GetLeaderboardName(SteamLeaderboard_t hLeaderboard)
{
	if (!m_StatsInitialized || !hLeaderboard)
	{
		return NULL;
	}
	return SteamUserStats()->GetLeaderboardName(hLeaderboard);
}

int SteamPlugin::GetLeaderboardEntryCount(SteamLeaderboard_t hLeaderboard)
{
	if (!m_StatsInitialized || !hLeaderboard)
	{
		return 0;
	}
	return SteamUserStats()->GetLeaderboardEntryCount(hLeaderboard);
}

ELeaderboardDisplayType SteamPlugin::GetLeaderboardDisplayType(SteamLeaderboard_t hLeaderboard)
{
	if (!m_StatsInitialized || !hLeaderboard)
	{
		return k_ELeaderboardDisplayTypeNone;
	}
	return SteamUserStats()->GetLeaderboardDisplayType(hLeaderboard);
}

ELeaderboardSortMethod SteamPlugin::GetLeaderboardSortMethod(SteamLeaderboard_t hLeaderboard)
{
	if (!m_StatsInitialized || !hLeaderboard)
	{
		return k_ELeaderboardSortMethodNone;
	}
	return SteamUserStats()->GetLeaderboardSortMethod(hLeaderboard);
}

// Callback for UploadLeaderboardScore.
void SteamPlugin::OnUploadScore(LeaderboardScoreUploaded_t *pCallback, bool bIOFailure)
{
	if (pCallback->m_bSuccess && !bIOFailure)
	{
		m_UploadLeaderboardScoreCallbackState = Done;
		m_LeaderboardScoreStored = true;
	}
	else
	{
		m_UploadLeaderboardScoreCallbackState = ServerError;
	}
	// Report these values as Steam reports them.
	m_LeaderboardScoreChanged = (pCallback->m_bScoreChanged != 0);
	m_LeaderboardUploadedScore = pCallback->m_nScore;
	m_LeaderboardGlobalRankNew = pCallback->m_nGlobalRankNew;
	m_LeaderboardGlobalRankPrevious = pCallback->m_nGlobalRankPrevious;
}

bool SteamPlugin::UploadLeaderboardScore(SteamLeaderboard_t hLeaderboard, ELeaderboardUploadScoreMethod eLeaderboardUploadScoreMethod, int score)
{
	if (!m_StatsInitialized)
	{
		return false;
	}
	if (!hLeaderboard)
	{
		m_UploadLeaderboardScoreCallbackState = ClientError;
		return false;
	}
	// Fail when the callback is currently running.
	if (m_UploadLeaderboardScoreCallbackState == Running)
	{
		return false;
	}
	try
	{
		m_LeaderboardScoreStored = false;
		m_LeaderboardScoreChanged = false;
		m_LeaderboardUploadedScore = 0;
		m_LeaderboardGlobalRankNew = 0;
		m_LeaderboardGlobalRankPrevious = 0;
		SteamAPICall_t hSteamAPICall = SteamUserStats()->UploadLeaderboardScore(hLeaderboard, eLeaderboardUploadScoreMethod, score, NULL, 0);
		m_CallResultUploadScore.Set(hSteamAPICall, this, &SteamPlugin::OnUploadScore);
		m_UploadLeaderboardScoreCallbackState = Running;
		return true;
	}
	catch (...)
	{
		m_UploadLeaderboardScoreCallbackState = ClientError;
		return false;
	}
}

// Callback for DownloadLeaderboardEntries.
void SteamPlugin::OnDownloadScore(LeaderboardScoresDownloaded_t *pCallback, bool bIOFailure)
{
	if (!bIOFailure)
	{
		m_DownloadLeaderboardEntriesCallbackState = Done;
		m_DownloadedLeaderboardEntryCount = min(pCallback->m_cEntryCount, MAX_LEADERBOARD_ENTRIES);
		for (int index = 0; index < m_DownloadedLeaderboardEntryCount; index++)
		{
			SteamUserStats()->GetDownloadedLeaderboardEntry(pCallback->m_hSteamLeaderboardEntries, index, &m_DownloadedLeaderboardEntries[index], NULL, 0);
		}
	}
	else
	{
		m_DownloadLeaderboardEntriesCallbackState = ServerError;
	}
}

bool SteamPlugin::DownloadLeaderboardEntries(SteamLeaderboard_t hLeaderboard, ELeaderboardDataRequest eLeaderboardDataRequest, int nRangeStart, int nRangeEnd)
{
	if (!m_StatsInitialized)
	{
		return false;
	}
	if (!hLeaderboard)
	{
		m_DownloadLeaderboardEntriesCallbackState = ClientError;
		return false;
	}
	// Fail when the callback is currently running.
	if (m_DownloadLeaderboardEntriesCallbackState == Running)
	{
		return false;
	}
	try
	{
		m_DownloadedLeaderboardEntryCount = 0;
		SteamAPICall_t hSteamAPICall = SteamUserStats()->DownloadLeaderboardEntries(hLeaderboard, eLeaderboardDataRequest, nRangeStart, nRangeEnd);
		m_CallResultDownloadScore.Set(hSteamAPICall, this, &SteamPlugin::OnDownloadScore);
		m_DownloadLeaderboardEntriesCallbackState = Running;
		return true;
	}
	catch (...)
	{
		m_DownloadLeaderboardEntriesCallbackState = ClientError;
		return false;
	}
}

int SteamPlugin::GetDownloadedLeaderboardEntryGlobalRank(int index)
{
	if (index < 0 || index >= m_DownloadedLeaderboardEntryCount)
	{
		agk::PluginError("GetDownloadedLeaderboardEntryGlobalRank index out of bounds.");
		return 0;
	}
	return m_DownloadedLeaderboardEntries[index].m_nGlobalRank;
}

int SteamPlugin::GetDownloadedLeaderboardEntryScore(int index)
{
	if (index < 0 || index >= m_DownloadedLeaderboardEntryCount)
	{
		agk::PluginError("GetDownloadedLeaderboardEntryScore index out of bounds.");
		return 0;
	}
	return m_DownloadedLeaderboardEntries[index].m_nScore;
}

CSteamID SteamPlugin::GetDownloadedLeaderboardEntryUser(int index)
{
	if (index < 0 || index >= m_DownloadedLeaderboardEntryCount)
	{
		agk::PluginError("GetDownloadedLeaderboardEntryAvatar index out of bounds.");
		return k_steamIDNil;
	}
	return m_DownloadedLeaderboardEntries[index].m_steamIDUser;
}

// Callback for RequestLobbyList.
void SteamPlugin::OnLobbyMatchList(LobbyMatchList_t *pLobbyMatchList, bool bIOFailure)
{
	if (bIOFailure)
	{
		m_LobbyMatchListCallbackState = ServerError;
	}
	else
	{
		m_LobbyMatchListCallbackState = Done;
		m_LobbyMatchListCount = pLobbyMatchList->m_nLobbiesMatching;
	}
}

bool SteamPlugin::RequestLobbyList()
{
	m_LobbyMatchListCount = 0;
	if (!m_SteamInitialized)
	{
		return false;
	}
	try
	{
		SteamAPICall_t hSteamAPICall = SteamMatchmaking()->RequestLobbyList();
		m_CallResultLobbyMatchList.Set(hSteamAPICall, this, &SteamPlugin::OnLobbyMatchList);
		m_LobbyMatchListCallbackState = Running;
		return true;
	}
	catch (...)
	{
		m_FindLeaderboardCallbackState = ClientError;
		return false;
	}
}

CSteamID SteamPlugin::GetLobbyByIndex(int iLobby)
{
	if (!m_SteamInitialized)
	{
		return k_steamIDNil;
	}
	if (m_LobbyMatchListCount == 0 || iLobby >= m_LobbyMatchListCount)
	{
		return k_steamIDNil;
	}
	return SteamMatchmaking()->GetLobbyByIndex(iLobby);
}

int SteamPlugin::GetLobbyDataCount(CSteamID steamIDLobby)
{
	if (!m_SteamInitialized || (NULL == SteamMatchmaking()))
	{
		return 0;
	}
	return SteamMatchmaking()->GetLobbyDataCount(steamIDLobby);
}

bool SteamPlugin::GetLobbyDataByIndex(CSteamID steamIDLobby, int iLobbyData, char *pchKey, int cchKeyBufferSize, char *pchValue, int cchValueBufferSize)
{
	if (!m_SteamInitialized || (NULL == SteamMatchmaking()))
	{
		return false;
	}
	return SteamMatchmaking()->GetLobbyDataByIndex(steamIDLobby, iLobbyData, pchKey, cchKeyBufferSize, pchValue, cchValueBufferSize);
}

CSteamID SteamPlugin::GetLobbyOwner(CSteamID steamIDLobby)
{
	if (!m_SteamInitialized || (NULL == SteamMatchmaking()))
	{
		return k_steamIDNil;
	}
	CSteamID owner = SteamMatchmaking()->GetLobbyOwner(steamIDLobby);
	if (owner.IsValid())
		agk::Message(GetFriendPersonaName(owner));
	return SteamMatchmaking()->GetLobbyOwner(steamIDLobby);
}