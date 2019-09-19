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

#ifndef _STEAMSCREENSHOTS_H_
#define _STEAMSCREENSHOTS_H_
#pragma once

#include "CCallbacks.h"

// ScreenshotReady_t
class CScreenshotReadyCallback : public ListCallbackBase<CScreenshotReadyCallback, ScreenshotReady_t>
{
public:
	void OnResponse(ScreenshotReady_t *pParam)
	{
		agk::Log("Callback: OnScreenshotReady");
		StoreResponse(*pParam);
	}
	void Clear(ScreenshotReady_t &value)
	{
		value.m_hLocal = INVALID_SCREENSHOT_HANDLE;
		value.m_eResult = (EResult)0;
	}
};

extern CScreenshotReadyCallback ScreenshotReadyCallback;

// ScreenshotRequested_t
class CScreenshotRequestedCallback : public BoolCallbackBase<CScreenshotRequestedCallback, ScreenshotRequested_t>
{
public:
	void OnResponse(ScreenshotRequested_t *pParam)
	{
		agk::Log("Callback: OnScreenshotRequested");
		BoolCallbackBase::OnResponse(pParam);
	}
};

extern CScreenshotRequestedCallback ScreenshotRequestedCallback;

#endif // _STEAMSCREENSHOTS_H_
