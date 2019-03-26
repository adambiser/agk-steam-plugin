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

/* @page ISteamUser */

//AdvertiseGame - game server stuff
//BeginAuthSession - game server stuff

/*
@desc Checks if the current users looks like they are behind a NAT device.
@return 1 if the current user is behind a NAT, otherwise 0.
@api ISteamUser#BIsBehindNAT
*/
extern "C" DLL_EXPORT int IsBehindNAT()
{
	CheckInitialized(false);
	return SteamUser()->BIsBehindNAT();
}

/*
@desc Checks whether the user's phone number is used to uniquely identify them.
@return 1 f the current user's phone uniquely verifies their identity; otherwise, 0.
@api ISteamUser#BIsPhoneIdentifying
*/
extern "C" DLL_EXPORT int IsPhoneIdentifying()
{
	CheckInitialized(false);
	return SteamUser()->BIsPhoneIdentifying();
}

/*
@desc Checks whether the current user's phone number is awaiting (re)verification.
@return 1 if the it is requiring verification; otherwise, 0.
@api ISteamUser#BIsPhoneRequiringVerification
*/
extern "C" DLL_EXPORT int IsPhoneRequiringVerification()
{
	CheckInitialized(false);
	return SteamUser()->BIsPhoneRequiringVerification();
}

/*
@desc Checks whether the current user has verified their phone number.
@return 1 if the current user has phone verification enabled; otherwise, 0.
@api ISteamUser#BIsTwoFactorEnabled
*/
extern "C" DLL_EXPORT int IsPhoneVerified()
{
	CheckInitialized(false);
	return SteamUser()->BIsPhoneVerified();
}

/*
@desc Checks whether the current user has Steam Guard two factor authentication enabled on their account.
@return 1 if the current user has two factor authentication enabled; otherwise, 0.
@api ISteamUser#IsPhoneVerified
*/
extern "C" DLL_EXPORT int IsTwoFactorEnabled()
{
	CheckInitialized(false);
	return SteamUser()->BIsTwoFactorEnabled();
}

/*
@desc Checks to see whether the user is logged on to Steam.
@return 1 when the user is logged on; otherwise 0.
@api ISteamUser#BLoggedOn
*/
extern "C" DLL_EXPORT int LoggedOn()
{
	CheckInitialized(false);
	return SteamUser()->BLoggedOn();
}

//CancelAuthTicket - game server stuff
//DecompressVoice - voice stuff
//EndAuthSession - game server stuff
//GetAuthSessionTicket - game server stuff
//GetAvailableVoice - voice stuff
//GetEncryptedAppTicket - encrypted app ticket stuff

/*
@desc Gets the level of the users Steam badge for your game.
The user can have two different badges for a series; the regular badge (max level 5) and the foil badge (max level 1).
@param series If you only have one set of cards, the series will be 1.
@param foil Check if they have received the foil badge.
@return The user's badge level.
@api ISteamUser#GetGameBadgeLevel
*/
extern "C" DLL_EXPORT int GetGameBadgeLevel(int series, int foil)
{
	CheckInitialized(0);
	return SteamUser()->GetGameBadgeLevel(series, foil != 0);
}

//GetHSteamUser - internal use

/*
@desc Gets the Steam level of the user, as shown on their Steam community profile.
@return The level of the current user.
*/
extern "C" DLL_EXPORT int GetPlayerSteamLevel()
{
	CheckInitialized(0);
	return SteamUser()->GetPlayerSteamLevel();
}

/*
@desc Gets a handle to the Steam ID of the account currently logged into the Steam client.
@return A SteamID handle
@api ISteamUser#GetSteamID
*/
extern "C" DLL_EXPORT int GetSteamID()
{
	CheckInitialized(0);
	return SteamHandles()->GetPluginHandle(SteamUser()->GetSteamID());
}

// Not in the API, but grouping this near GetSteamID().
/*
@desc Checks to see if a Steam ID handle is valid.
@param hSteamID The Steam ID handle to check.
@return 1 when the Steam ID handle is valid; otherwise 0.
*/
extern "C" DLL_EXPORT int IsSteamIDValid(int hSteamID)
{
	CheckInitialized(false);
	CSteamID steamID = SteamHandles()->GetSteamHandle(hSteamID);
	return steamID.IsValid();
}

// Not in the API, but grouping this near GetSteamID().
/*
@desc Gets the the account ID (profile number) for the given Steam ID handle.
@param hSteamIDUser A user Steam ID handle.
@return The account ID.
*/
extern "C" DLL_EXPORT int GetAccountID(int hSteamIDUser)
{
	CheckInitialized(0);
	CSteamID steamID = SteamHandles()->GetSteamHandle(hSteamIDUser);
	if (steamID != k_steamIDNil)
	{
		return steamID.GetAccountID();
	}
	return 0;
}

// Not in the API, but grouping this near GetSteamID().
/*
@desc Gets the the SteamID3 for the given Steam ID handle.
@param hSteamIDUser A user Steam ID handle.
@return A string containing the SteamID3.
*/
extern "C" DLL_EXPORT char *GetSteamID3(int hSteamIDUser)
{
	CheckInitialized(NULL_STRING);
	CSteamID steamID = SteamHandles()->GetSteamHandle(hSteamIDUser);
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
/*
@desc Gets the the SteamID64 (profile number) for the given Steam ID handle.
@param hSteamIDUser A user Steam ID handle.
@return A string containing the 64-bit SteamID64 in base 10.
*/
extern "C" DLL_EXPORT char *GetSteamID64(int hSteamIDUser)
{
	CheckInitialized(NULL_STRING);
	char id64[21]; // Max value is 18,446,744,073,709,551,615
	_i64toa(SteamHandles()->GetSteamHandle(hSteamIDUser), id64, 10);
	return utils::CreateString(id64);
}

// Not in the API, but grouping this near GetSteamID().
/*
@desc Returns the handle for the given SteamID64 string.
@param steamID64 A SteamID64 string.
@return A Steam ID handle or 0.
*/
extern "C" DLL_EXPORT int GetHandleFromSteamID64(const char *steamID64)
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
