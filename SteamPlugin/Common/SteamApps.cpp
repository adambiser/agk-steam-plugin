#include "SteamPlugin.h"

// App/DLC methods
bool SteamPlugin::GetDLCDataByIndex(int iDLC, AppId_t *pAppID, bool *pbAvailable, char *pchName, int cchNameBufferSize)
{
	CheckInitialized(SteamApps, false);
	return SteamApps()->BGetDLCDataByIndex(iDLC, pAppID, pbAvailable, pchName, cchNameBufferSize);
}

bool SteamPlugin::IsAppInstalled(AppId_t appID)
{
	CheckInitialized(SteamApps, false);
	return SteamApps()->BIsAppInstalled(appID);
}

bool SteamPlugin::IsCybercafe()
{
	CheckInitialized(SteamApps, false);
	return SteamApps()->BIsCybercafe();
}

bool SteamPlugin::IsDlcInstalled(AppId_t appID)
{
	CheckInitialized(SteamApps, false);
	return SteamApps()->BIsDlcInstalled(appID);
}

bool SteamPlugin::IsLowViolence()
{
	CheckInitialized(SteamApps, false);
	return SteamApps()->BIsLowViolence();
}

bool SteamPlugin::IsSubscribed()
{
	CheckInitialized(SteamApps, false);
	return SteamApps()->BIsSubscribed();
}

bool SteamPlugin::IsSubscribedApp(AppId_t appID)
{
	CheckInitialized(SteamApps, false);
	return SteamApps()->BIsSubscribedApp(appID);
}

bool SteamPlugin::IsSubscribedFromFamilySharing()
{
	CheckInitialized(SteamApps, false);
	return SteamApps()->BIsSubscribedFromFamilySharing();
}

bool SteamPlugin::IsSubscribedFromFreeWeekend()
{
	CheckInitialized(SteamApps, false);
	return SteamApps()->BIsSubscribedFromFreeWeekend();
}

bool SteamPlugin::IsVACBanned()
{
	CheckInitialized(SteamApps, false);
	return SteamApps()->BIsVACBanned();
}

int SteamPlugin::GetAppBuildId()
{
	CheckInitialized(SteamApps, 0);
	return SteamApps()->GetAppBuildId();
}

uint32 SteamPlugin::GetAppInstallDir(AppId_t appID, char *pchFolder, uint32 cchFolderBufferSize)
{
	CheckInitialized(SteamApps, 0);
	return SteamApps()->GetAppInstallDir(appID, pchFolder, cchFolderBufferSize);
}

CSteamID SteamPlugin::GetAppOwner()
{
	CheckInitialized(SteamApps, k_steamIDNil);
	return SteamApps()->GetAppOwner();
}

const char * SteamPlugin::GetAvailableGameLanguages()
{
	CheckInitialized(SteamApps, NULL);
	return SteamApps()->GetAvailableGameLanguages();
}

bool SteamPlugin::GetCurrentBetaName(char *pchName, int cchNameBufferSize)
{
	CheckInitialized(SteamApps, false);
	return SteamApps()->GetCurrentBetaName(pchName, cchNameBufferSize);
}

const char * SteamPlugin::GetCurrentGameLanguage()
{
	CheckInitialized(SteamApps, NULL);
	return SteamApps()->GetCurrentGameLanguage();
}

int SteamPlugin::GetDLCCount()
{
	CheckInitialized(SteamApps, 0);
	return SteamApps()->GetDLCCount();
}

bool SteamPlugin::GetDlcDownloadProgress(AppId_t nAppID, uint64 *punBytesDownloaded, uint64 *punBytesTotal)
{
	CheckInitialized(SteamApps, false);
	return SteamApps()->GetDlcDownloadProgress(nAppID, punBytesDownloaded, punBytesTotal);
}

uint32 SteamPlugin::GetEarliestPurchaseUnixTime(AppId_t nAppID)
{
	CheckInitialized(SteamApps, 0);
	return SteamApps()->GetEarliestPurchaseUnixTime(nAppID);
}

//SteamAPICall_t SteamPlugin::GetFileDetails(const char*pszFileName); // FileDetailsResult_t call result.

uint32 SteamPlugin::GetInstalledDepots(AppId_t appID, DepotId_t *pvecDepots, uint32 cMaxDepots)
{
	CheckInitialized(SteamApps, 0);
	return SteamApps()->GetInstalledDepots(appID, pvecDepots, cMaxDepots);
}

const char * SteamPlugin::GetLaunchQueryParam(const char *pchKey)
{
	CheckInitialized(SteamApps, NULL);
	return SteamApps()->GetLaunchQueryParam(pchKey);
}

void SteamPlugin::OnNewLaunchQueryParameters(NewUrlLaunchParameters_t *pParam)
{
	agk::Log("Callback: New Launch Query Parameters");
	m_HasNewLaunchQueryParameters = true;
}

bool SteamPlugin::HasNewLaunchQueryParameters()
{
	// Reports true only once per change.
	if (m_HasNewLaunchQueryParameters)
	{
		m_HasNewLaunchQueryParameters = false;
		return true;
	}
	return false;
}

int SteamPlugin::GetLaunchCommandLine(char *pchCommandLine, int cubCommandLine)
{
	CheckInitialized(SteamApps, 0);
	return SteamApps()->GetLaunchCommandLine(pchCommandLine, cubCommandLine);
}

void SteamPlugin::OnDlcInstalled(DlcInstalled_t *pParam)
{
	agk::Log("Callback: DLC Installed");
	// Don't store unless the main program is using this functionality.
	if (m_OnDlcInstalledEnabled)
	{
		m_DlcInstalledList.push_back(pParam->m_nAppID);
	}
}

bool SteamPlugin::HasNewDlcInstalled()
{
	m_OnDlcInstalledEnabled = true;
	if (m_DlcInstalledList.size() > 0)
	{
		m_NewDlcInstalled = m_DlcInstalledList.front();
		m_DlcInstalledList.pop_front();
		return true;
	}
	m_NewDlcInstalled = 0;
	return false;
}

void SteamPlugin::InstallDLC(AppId_t nAppID)
{
	// Triggers a DlcInstalled_t callback.
	CheckInitialized(SteamApps, );
	m_OnDlcInstalledEnabled = true;
	return SteamApps()->InstallDLC(nAppID);
}

bool SteamPlugin::MarkContentCorrupt(bool bMissingFilesOnly)
{
	CheckInitialized(SteamApps, false);
	return SteamApps()->MarkContentCorrupt(bMissingFilesOnly);
}

void SteamPlugin::UninstallDLC(AppId_t nAppID)
{
	CheckInitialized(SteamApps, );
	return SteamApps()->UninstallDLC(nAppID);
}
