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

#ifndef _STEAMUSER_H_
#define _STEAMUSER_H_
#pragma once

#include "CCallbacks.h"

//ClientGameServerDeny_t - game server stuff
//DurationControl_t
//EncryptedAppTicketResponse_t - Call result for RequestEncryptedAppTicket.
//GameWebCallback_t - web sign up stuff.
//GetAuthSessionTicketResponse_t - Call result for GetAuthSessionTicket.
//MarketEligibilityResponse_t - Call result for GetMarketEligibility

// GetDurationControl
class CDurationControlCallback : public ListCallbackBase<CDurationControlCallback, DurationControl_t>
{
public:
	void OnResponse(DurationControl_t *pParam)
	{
		agk::Log("Callback: OnDurationControl");
		StoreResponse(*pParam);
	}
	void Clear(DurationControl_t &value)
	{
		value.m_appid = 0;
		value.m_bApplicable = false;
		value.m_csecsLast5h = 0;
		value.m_eResult = (EResult)0;
		value.m_notification = (EDurationControlNotification)0;
		value.m_progress = (EDurationControlProgress)0;
	}
};


//IPCFailure_t - Steam client had a fatal error.

//LicensesUpdated_t - Called whenever the users licenses (owned packages) changes.

//MicroTxnAuthorizationResponse_t - micro transaction stuff.

//SteamServerConnectFailure_t - Called when a connection attempt has failed.
//SteamServersConnected_t - Called when a connections to the Steam back-end has been established.
//SteamServersDisconnected_t - Called if the client has lost connection to the Steam servers.

//StoreAuthURLResponse_t - Call result for RequestStoreAuthURL
//ValidateAuthTicketResponse_t - Call result for BeginAuthSession

#endif // _STEAMUSER_H_
