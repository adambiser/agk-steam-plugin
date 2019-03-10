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

#ifndef _CCALLRESULTITEM_H_
#define _CCALLRESULTITEM_H_
#pragma once

#include <steam_api.h>
#include "DllMain.h"
#include "AGKUtils.h"

class CCallResultItem
{
public:
	CCallResultItem() : 
		m_State(Idle),
		m_eResult((EResult)0),
		m_Running(false) {}
	virtual ~CCallResultItem(void) {}
	// Return a 'name' for the CCallResultItem object, usually indicates type and some useful parameter values.
	virtual std::string GetName() = 0;
	ECallbackState GetState()
	{
		// Immediately return to Idle after reporting Done.
		if (m_State == Done)
		{
			m_State = Idle;
			return Done;
		}
		return m_State;
	}
	void Run()
	{
		if (m_Running)
		{
			return;
		}
		m_Running = true;
		try
		{
			utils::Log(GetName() + ": Calling.");
			Call();
		}
		catch (std::string e)
		{
			utils::PluginError(e);
			m_State = ClientError;
			m_eResult = k_EResultFail;
		}
		catch (...)
		{
			utils::PluginError(GetName() + ": Call threw an unexpected error.");
			m_State = ClientError;
			m_eResult = k_EResultFail;
		}
	}
	virtual EResult GetResultCode() { return m_eResult; }
	virtual std::string GetResultJSON() { return std::string("{}"); }
protected:
	// Throw std::string for errors
	virtual void Call() = 0;
	ECallbackState m_State;
	EResult m_eResult;
	bool m_Running;
	SteamAPICall_t m_hSteamAPICall;
};

#endif // _CCALLRESULTITEM_H_
