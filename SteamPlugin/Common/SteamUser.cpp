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

#include "stdafx.h"
#include "SteamUser.h"

CDurationControlCallback DurationControlCallback;

/* @page ISteamUser */

//AdvertiseGame - game server stuff
//BeginAuthSession - game server stuff

/*
@desc Checks if the current users looks like they are behind a NAT device.
@return 1 if the current user is behind a NAT, otherwise 0.
@url https://partner.steamgames.com/doc/api/ISteamUser#BIsBehindNAT
*/
extern "C" DLL_EXPORT int IsBehindNAT()
{
	CheckInitialized(false);
	return SteamUser()->BIsBehindNAT();
}

/*
@desc Checks whether the user's phone number is used to uniquely identify them.
@return 1 f the current user's phone uniquely verifies their identity; otherwise, 0.
@url https://partner.steamgames.com/doc/api/ISteamUser#BIsPhoneIdentifying
*/
extern "C" DLL_EXPORT int IsPhoneIdentifying()
{
	CheckInitialized(false);
	return SteamUser()->BIsPhoneIdentifying();
}

/*
@desc Checks whether the current user's phone number is awaiting (re)verification.
@return 1 if the it is requiring verification; otherwise, 0.
@url https://partner.steamgames.com/doc/api/ISteamUser#BIsPhoneRequiringVerification
*/
extern "C" DLL_EXPORT int IsPhoneRequiringVerification()
{
	CheckInitialized(false);
	return SteamUser()->BIsPhoneRequiringVerification();
}

/*
@desc Checks whether the current user has verified their phone number.
@return 1 if the current user has phone verification enabled; otherwise, 0.
@url https://partner.steamgames.com/doc/api/ISteamUser#BIsTwoFactorEnabled
*/
extern "C" DLL_EXPORT int IsPhoneVerified()
{
	CheckInitialized(false);
	return SteamUser()->BIsPhoneVerified();
}

/*
@desc Checks whether the current user has Steam Guard two factor authentication enabled on their account.
@return 1 if the current user has two factor authentication enabled; otherwise, 0.
@url https://partner.steamgames.com/doc/api/ISteamUser#IsPhoneVerified
*/
extern "C" DLL_EXPORT int IsTwoFactorEnabled()
{
	CheckInitialized(false);
	return SteamUser()->BIsTwoFactorEnabled();
}

/*
@desc Checks to see whether the user is logged on to Steam.
@return 1 when the user is logged on; otherwise 0.
@url https://partner.steamgames.com/doc/api/ISteamUser#BLoggedOn
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

class CGetDurationControlCallResult : public CCallResultItem<DurationControl_t>
{
public:
	CGetDurationControlCallResult()
	{
		m_CallResultName = "GetDurationControl()";
		m_hSteamAPICall = SteamUser()->GetDurationControl();
	}
	virtual ~CGetDurationControlCallResult() { }
	AppId_t GetAppID() { return m_Response.m_appid; }
	bool GetApplicable() { return m_Response.m_bApplicable; }
	int32 GetCSecsLast5h() { return m_Response.m_csecsLast5h; }
	int GetProgress() { return (int)m_Response.m_progress; }
	int GetNotification() { return (int)m_Response.m_notification; }
protected:
	void OnResponse(DurationControl_t *pCallResult, bool bFailure)
	{
		CCallResultItem::OnResponse(pCallResult, bFailure);
	}
};

/*
@desc Retrieves anti indulgence / duration control for current user / game combination.
@return A [call result handle](Callbacks-and-Call-Results#call-results) on success; otherwise 0.
@callback-type callresult
@callback-getters GetDurationControlAppID, GetDurationControlApplicable, GetDurationControlSecondsInLastFiveHours,
GetDurationControlProgress, GetDurationControlNotification
@url https://partner.steamgames.com/doc/api/ISteamUser#GetDurationControl
*/
extern "C" DLL_EXPORT int GetDurationControl()
{
	CheckInitialized(0);
	DurationControlCallback.Register();
	return CallResults()->Add(new CGetDurationControlCallResult());
}

/*
@desc Sent for games with enabled anti indulgence / duration control, for enabled users. Lets the game know whether persistent rewards or XP should be granted at normal rate, half rate, or zero rate.

This callback is fired asynchronously in response to timers triggering. It is also fired in response to calls to GetDurationControl().
@callback-type list
@callback-getters	GetDurationControlResult, GetDurationControlAppID, GetDurationControlApplicable, GetDurationControlSecondsInLastFiveHours,
GetDurationControlProgress, GetDurationControlNotification
@return 1 when the callback has more responses to process; otherwise 0.
@url https://partner.steamgames.com/doc/api/ISteamUser#DurationControl_t
*/
extern "C" DLL_EXPORT int HasDurationControlResponse()
{
	return DurationControlCallback.HasResponse();
}

/*
@desc Returns the result of the current DurationControl_t callback response.
@return An EResult value.
@url https://partner.steamgames.com/doc/api/ISteamUser#DurationControl_t
@url https://partner.steamgames.com/doc/api/steam_api#EResult
*/
extern "C" DLL_EXPORT int GetDurationControlResult()
{
	return DurationControlCallback.GetCurrent().m_eResult;
}

/*
@desc Returns the appid generating playtime of the current DurationControl_t callback response.
@return An app ID.
@url https://partner.steamgames.com/doc/api/ISteamUser#DurationControl_t
*/
extern "C" DLL_EXPORT int GetDurationControlAppID()
{
	return DurationControlCallback.GetCurrent().m_appid;
}

/*
@desc Returns the appid generating playtime for the given call result.
@param hCallResult An GetDurationControl call result handle.
@return An app ID.
@url https://partner.steamgames.com/doc/api/ISteamUser#DurationControl_t
@plugin-name GetDurationControlAppID
*/
extern "C" DLL_EXPORT int GetDurationControlCallResultAppID(int hCallResult)
{
	return GetCallResultValue(hCallResult, &CGetDurationControlCallResult::GetAppID);
}

/*
@desc Returns whether the duration control is applicable to user + game combination for the current DurationControl_t callback response.
@return 1 when the duration control is applicable to user + game combination; otherwise 0.
@url https://partner.steamgames.com/doc/api/ISteamUser#DurationControl_t
*/
extern "C" DLL_EXPORT int GetDurationControlApplicable()
{
	return DurationControlCallback.GetCurrent().m_bApplicable;
}

/*
@desc Returns whether the duration control is applicable to user + game combination for the given call result.
@param hCallResult An GetDurationControl call result handle.
@return 1 when the duration control is applicable to user + game combination; otherwise 0.
@url https://partner.steamgames.com/doc/api/ISteamUser#DurationControl_t
@plugin-name GetDurationControlApplicable
*/
extern "C" DLL_EXPORT int GetDurationControlCallResultApplicable(int hCallResult)
{
	return GetCallResultValue(hCallResult, &CGetDurationControlCallResult::GetApplicable);
}

/*
@desc Returns playtime in trailing 5 hour window plus current session, in seconds, for the current DurationControl_t callback response.
@return An integer.
@url https://partner.steamgames.com/doc/api/ISteamUser#DurationControl_t
*/
extern "C" DLL_EXPORT int GetDurationControlSecondsInLastFiveHours()
{
	return DurationControlCallback.GetCurrent().m_csecsLast5h;
}

/*
@desc Returns playtime in trailing 5 hour window plus current session, in seconds, for the given call result.
@param hCallResult An GetDurationControl call result handle.
@return An integer.
@url https://partner.steamgames.com/doc/api/ISteamUser#DurationControl_t
@plugin-name GetDurationControlSecondsInLastFiveHours
*/
extern "C" DLL_EXPORT int GetDurationControlCallResultSecondsInLastFiveHours(int hCallResult)
{
	return GetCallResultValue(hCallResult, &CGetDurationControlCallResult::GetCSecsLast5h);
}

/*
@desc Returns the recommended progress for the current DurationControl_t callback response.
@return An EDurationControlProgress value.
@url https://partner.steamgames.com/doc/api/ISteamUser#DurationControl_t
@url https://partner.steamgames.com/doc/api/ISteamUser#EDurationControlProgress
*/
extern "C" DLL_EXPORT int GetDurationControlProgress()
{
	return DurationControlCallback.GetCurrent().m_progress;
}

/*
@desc Returns the recommended progress for the given call result.
@param hCallResult An GetDurationControl call result handle.
@return An EDurationControlProgress value.
@url https://partner.steamgames.com/doc/api/ISteamUser#DurationControl_t
@url https://partner.steamgames.com/doc/api/ISteamUser#EDurationControlProgress
@plugin-name GetDurationControlProgress
*/
extern "C" DLL_EXPORT int GetDurationControlCallResultProgress(int hCallResult)
{
	return GetCallResultValue(hCallResult, &CGetDurationControlCallResult::GetProgress);
}

/*
@desc Returns the notification to show, if any (always k_EDurationControlNotification_None for API calls), for the current DurationControl_t callback response.
@return An EDurationControlProgress value.
@url https://partner.steamgames.com/doc/api/ISteamUser#DurationControl_t
@url https://partner.steamgames.com/doc/api/ISteamUser#EDurationControlNotification
*/
extern "C" DLL_EXPORT int GetDurationControlNotification()
{
	return DurationControlCallback.GetCurrent().m_notification;
}

/*
@desc Returns the notification to show, if any (always k_EDurationControlNotification_None for API calls), for the given call result.
@param hCallResult An GetDurationControl call result handle.
@return An EDurationControlProgress value.
@url https://partner.steamgames.com/doc/api/ISteamUser#DurationControl_t
@url https://partner.steamgames.com/doc/api/ISteamUser#EDurationControlNotification
@plugin-name GetDurationControlNotification
*/
extern "C" DLL_EXPORT int GetDurationControlCallResultNotification(int hCallResult)
{
	return GetCallResultValue(hCallResult, &CGetDurationControlCallResult::GetNotification);
}

//GetEncryptedAppTicket - encrypted app ticket stuff

/*
@desc Gets the level of the users Steam badge for your game.
The user can have two different badges for a series; the regular badge (max level 5) and the foil badge (max level 1).
@param series If you only have one set of cards, the series will be 1.
@param foil Check if they have received the foil badge.
@return The user's badge level.
@url https://partner.steamgames.com/doc/api/ISteamUser#GetGameBadgeLevel
*/
extern "C" DLL_EXPORT int GetGameBadgeLevel(int series, int foil)
{
	CheckInitialized(0);
	return SteamUser()->GetGameBadgeLevel(series, foil != 0);
}

//GetHSteamUser - internal use

template<>
void ResponseWrapper<MarketEligibilityResponse_t>::SetResult() { m_eResult = k_EResultOK; }

class CGetMarketEligibilityCallResult : public CCallResultItem<MarketEligibilityResponse_t, ResponseWrapper<MarketEligibilityResponse_t>>
{
public:
	CGetMarketEligibilityCallResult()
	{
		m_CallResultName = "GetMarketEligibility()";
		m_hSteamAPICall = SteamUser()->GetMarketEligibility();
	}
	virtual ~CGetMarketEligibilityCallResult() { }
	bool IsAllowed() { return m_Response.m_bAllowed; }
	int GetNotAllowedReason() { return (int)m_Response.m_eNotAllowedReason; }
	int GetAllowedAtTime() { return (int)m_Response.m_rtAllowedAtTime; }
	int GetSteamGuardRequiredDays() { return m_Response.m_cdaySteamGuardRequiredDays; }
	int GetNewDeviceCooldown() { return m_Response.m_cdayNewDeviceCooldown; }
protected:
	void OnResponse(MarketEligibilityResponse_t *pCallResult, bool bFailure)
	{
		CCallResultItem::OnResponse(pCallResult, bFailure);
	}
};

/*
@desc Checks whether or not an account is allowed to use the market.
@return A [call result handle](Callbacks-and-Call-Results#call-results) on success; otherwise 0.
@callback-type callresult
@callback-getters GetDurationControlAppID, GetDurationControlApplicable, GetDurationControlSecondsInLastFiveHours,
GetDurationControlProgress, GetDurationControlNotification
@url https://partner.steamgames.com/doc/api/ISteamUser#MarketEligibilityResponse_t
*/
extern "C" DLL_EXPORT int GetMarketEligibility()
{
	// Not in API docs, see https://partner.steamgames.com/doc/webapi/IEconMarketService#GetMarketEligibility
	CheckInitialized(0);
	return CallResults()->Add(new CGetMarketEligibilityCallResult());
}

/*
@desc Returns whether or not the user can use the market.
@param hCallResult An GetMarketEligibility call result handle.
@return 1 if the user can use the market; otherwise 0.
@url https://partner.steamgames.com/doc/api/ISteamUser#MarketEligibilityResponse_t
*/
extern "C" DLL_EXPORT int GetMarketEligibilityIsAllowed(int hCallResult)
{
	return GetCallResultValue(hCallResult, &CGetMarketEligibilityCallResult::IsAllowed);
}

/*
@desc Returns the reasons a user may not use the Community Market.
@param hCallResult An GetMarketEligibility call result handle.
@return A EMarketNotAllowedReasonFlags value.
@return-url https://partner.steamgames.com/doc/api/ISteamUser#EMarketNotAllowedReasonFlags
@url https://partner.steamgames.com/doc/api/ISteamUser#MarketEligibilityResponse_t
*/
extern "C" DLL_EXPORT int GetMarketEligibilityNotAllowedReason(int hCallResult)
{
	return GetCallResultValue(hCallResult, &CGetMarketEligibilityCallResult::GetNotAllowedReason);
}

/*
@desc Returns when the user is allowed to use the market again.
@param hCallResult An GetMarketEligibility call result handle.
@return A Unix time.
@url https://partner.steamgames.com/doc/api/ISteamUser#MarketEligibilityResponse_t
*/
extern "C" DLL_EXPORT int GetMarketEligibilityAllowedAtTime(int hCallResult)
{
	return GetCallResultValue(hCallResult, &CGetMarketEligibilityCallResult::GetAllowedAtTime);
}

/*
@desc Returns the number of days any user is required to have had Steam Guard before they can use the market
@param hCallResult An GetMarketEligibility call result handle.
@return An integer.
@url https://partner.steamgames.com/doc/api/ISteamUser#MarketEligibilityResponse_t
*/
extern "C" DLL_EXPORT int GetMarketEligibilitySteamGuardRequiredDays(int hCallResult)
{
	return GetCallResultValue(hCallResult, &CGetMarketEligibilityCallResult::GetSteamGuardRequiredDays);
}

/*
@desc Returns the number of days after initial device authorization a user must wait before using the market on that device
@param hCallResult An GetMarketEligibility call result handle.
@return An integer
@url https://partner.steamgames.com/doc/api/ISteamUser#MarketEligibilityResponse_t
*/
extern "C" DLL_EXPORT int GetMarketEligibilityNewDeviceCooldown(int hCallResult)
{
	return GetCallResultValue(hCallResult, &CGetMarketEligibilityCallResult::GetNewDeviceCooldown);
}

/*
@desc Gets the Steam level of the user, as shown on their Steam community profile.
@return The level of the current user.
@url https://partner.steamgames.com/doc/api/ISteamUser#GetPlayerSteamLevel
*/
extern "C" DLL_EXPORT int GetPlayerSteamLevel()
{
	CheckInitialized(0);
	return SteamUser()->GetPlayerSteamLevel();
}

/*
@desc Advise steam china duration control system about the online state of the game.
This will prevent offline gameplay time from counting against a user's playtime limits.
@param newState	The new state
@return 1 when the call succeeds; otherwise 0.
@url https://partner.steamgames.com/doc/api/ISteamUser#BSetDurationControlOnlineState
@url https://partner.steamgames.com/doc/api/ISteamUser#EDurationControlOnlineState
*/
extern "C" DLL_EXPORT int SetDurationControlOnlineState(int newState)
{
	CheckInitialized(0);
	return SteamUser()->BSetDurationControlOnlineState((EDurationControlOnlineState)newState);
}

/*
@desc Gets a handle to the Steam ID of the account currently logged into the Steam client.
@return A SteamID handle
@url https://partner.steamgames.com/doc/api/ISteamUser#GetSteamID
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
@url https://developer.valvesoftware.com/wiki/SteamID#Types_of_Steam_Accounts
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
		default:
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
	//_i64toa(SteamHandles()->GetSteamHandle(hSteamIDUser), id64, 10);
	// Windows and Linux
	snprintf(id64, 21, "%llu", SteamHandles()->GetSteamHandle(hSteamIDUser));
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
	//uint64 id = _atoi64(steamID64);
	uint64 id = atoll(steamID64);
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
