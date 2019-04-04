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

#ifndef _STEAMAPPS_H_
#define _STEAMAPPS_H_
#pragma once

#include "CCallbacks.h"

// AppProofOfPurchaseKeyResponse_t - Only used internally in Steam.

// InstallDLC
class CDlcInstalledCallback : public ListCallbackBase<CDlcInstalledCallback, DlcInstalled_t>
{
public:
	void OnResponse(DlcInstalled_t *pParam)
	{
		utils::Log("Callback: OnDlcInstalled.  AppID = " + std::to_string(pParam->m_nAppID));
		StoreResponse(*pParam);
	}
	void Clear(DlcInstalled_t &value)
	{
		value.m_nAppID = 0;
	}
};

// FileDetailsResult_t - call result for GetFileDetails

class CNewUrlLaunchParametersCallback : public BoolCallbackBase<CNewUrlLaunchParametersCallback, NewUrlLaunchParameters_t>
{
public:
	void OnResponse(NewUrlLaunchParameters_t *pParam)
	{
		agk::Log("Callback: OnNewUrlLaunchParameters");
		BoolCallbackBase::OnResponse(pParam);
	}
};

// NewLaunchQueryParameters_t - Removed in SDK v1.43
// RegisterActivationCodeResponse_t - Only used internally in Steam.

#endif // _STEAMAPPS_H_
