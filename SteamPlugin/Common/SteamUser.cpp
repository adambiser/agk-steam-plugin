#include "SteamPlugin.h"

CSteamID SteamPlugin::GetSteamID()
{
	CheckInitialized(SteamUser, k_steamIDNil);
	return SteamUser()->GetSteamID();
}

bool SteamPlugin::LoggedOn()
{
	CheckInitialized(SteamUser, 0);
	return SteamUser()->BLoggedOn();
}
