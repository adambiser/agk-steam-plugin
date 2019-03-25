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

#include "Common.h"

//AdvertiseGame - game server stuff
//BeginAuthSession - game server stuff

int IsBehindNAT()
{
	return SteamUser()->BIsBehindNAT();
}

int IsPhoneIdentifying()
{
	return SteamUser()->BIsPhoneIdentifying();
}

int IsPhoneRequiringVerification()
{
	return SteamUser()->BIsPhoneRequiringVerification();
}

int IsPhoneVerified()
{
	return SteamUser()->BIsPhoneVerified();
}

int IsTwoFactorEnabled()
{
	return SteamUser()->BIsTwoFactorEnabled();
}

int LoggedOn()
{
	return SteamUser()->BLoggedOn();
}

//CancelAuthTicket - game server stuff
//DecompressVoice - voice stuff
//EndAuthSession - game server stuff
//GetAuthSessionTicket - game server stuff
//GetAvailableVoice - voice stuff
//GetEncryptedAppTicket - encrypted app ticket stuff

int GetGameBadgeLevel(int series, int foil)
{
	return SteamUser()->GetGameBadgeLevel(series, foil != 0);
}

//GetHSteamUser - internal use

int GetPlayerSteamLevel()
{
	return SteamUser()->GetPlayerSteamLevel();
}

int GetSteamID()
{
	CheckInitialized(0);
	return SteamHandles()->GetPluginHandle(SteamUser()->GetSteamID());
}

// Not in the API, but grouping this near GetSteamID().
int IsSteamIDValid(int hSteamID)
{
	CheckInitialized(false);
	CSteamID steamID = SteamHandles()->GetSteamHandle(hSteamID);
	return steamID.IsValid();
}

// Not in the API, but grouping this near GetSteamID().
int GetAccountID(int hUserSteamID)
{
	CheckInitialized(0);
	CSteamID steamID = SteamHandles()->GetSteamHandle(hUserSteamID);
	if (steamID != k_steamIDNil)
	{
		return steamID.GetAccountID();
	}
	return 0;
}

// Not in the API, but grouping this near GetSteamID().
char *GetSteamID3(int hUserSteamID)
{
	CheckInitialized(NULL_STRING);
	CSteamID steamID = SteamHandles()->GetSteamHandle(hUserSteamID);
	if (steamID != k_steamIDNil)
	{
		std::string steam3("[");
		// Source: https://developer.valvesoftware.com/wiki/SteamID#Types_of_Steam_Accounts
		switch (steamID.GetEAccountType())
		{
		case k_EAccountTypeInvalid:			// I
		case k_EAccountTypeIndividual:		// U
		case k_EAccountTypeMultiseat:		// M
		case k_EAccountTypeGameServer:		// G
		case k_EAccountTypeAnonGameServer:	// A
		case k_EAccountTypePending:			// P
		case k_EAccountTypeContentServer:	// C
		case k_EAccountTypeClan:			// g
		case k_EAccountTypeChat:			// T / L / c
		case k_EAccountTypeAnonUser:		// a
			steam3 += "IUMGAPCgT?a"[steamID.GetEAccountType()];
			break;
		case k_EAccountTypeConsoleUser: // 9 = P2P, but this doesn't look right.  Fake anyway, just do the default below.
		default:
			steam3 += std::to_string(steamID.GetEAccountType());
			break;
		}
		steam3 += ":" + std::to_string(steamID.GetEUniverse()) + ":" + std::to_string(steamID.GetAccountID());
		switch (steamID.GetEAccountType())
		{
		case k_EAccountTypeAnonGameServer:
		case k_EAccountTypeAnonUser:
			steam3 += ":" + std::to_string(steamID.GetUnAccountInstance());
			break;
		}
		steam3 += "]";
		return utils::CreateString(steam3);
	}
	return NULL_STRING;
}

// Not in the API, but grouping this near GetSteamID().
char *GetSteamID64(int hUserSteamID)
{
	CheckInitialized(NULL_STRING);
	char id64[21]; // Max value is 18,446,744,073,709,551,615
	_i64toa(SteamHandles()->GetSteamHandle(hUserSteamID), id64, 10);
	return utils::CreateString(id64);
}

// Not in the API, but grouping this near GetSteamID().
int GetHandleFromSteamID64(const char *steamID64)
{
	CheckInitialized(0);
	uint64 id = _atoi64(steamID64);
	return SteamHandles()->GetPluginHandle(CSteamID(id));
}

//GetUserDataFolder - Deprecated

//GetVoice - voice stuff
//GetVoiceOptimalSampleRate - voice stuff
//InitiateGameConnection - old authentication, skip?
//RequestEncryptedAppTicket - encrypted app ticket stuff
//RequestStoreAuthURL - store stuff
//StartVoiceRecording - voice stuff
//StopVoiceRecording - voice stuff
//TerminateGameConnection - old authentication, skip?
//TrackAppUsageEvent - deprecated
//UserHasLicenseForApp - game server stuff... validate DLC for auth session

//Callbacks
