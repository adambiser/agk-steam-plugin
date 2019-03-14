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
#include "Utils.h"
#include <map>

class CCallResultItem
{
public:
	CCallResultItem() : 
		m_eResult((EResult)0),
		m_bRunning(false) {}
	virtual ~CCallResultItem(void) {}
	// Return a 'name' for the CCallResultItem object, usually indicates type and some useful parameter values.
	std::string GetName() { return m_CallResultName; }
	EResult GetResultCode() { return m_eResult; }
protected:
	std::string m_CallResultName;
	SteamAPICall_t m_hSteamAPICall;
	// Throw std::string for errors.
	virtual void Call() = 0;
	// Called from CCallResultMap.Add().
	friend void FriendCall(CCallResultItem &callResult)
	{
		callResult.Call();
	}
	EResult m_eResult;
	bool m_bRunning;
};

/*
Map for maintaining all existing call results.
*/
class CCallResultMap
{
public:
	CCallResultMap() {}
	virtual ~CCallResultMap(void) {}

	int Add(CCallResultItem *callResult);
	// Template methods MUST be defined in the header file!
	template <class T> T *Get(int handle)
	{
		auto search = m_Items.find(handle);
		if (search == m_Items.end())
		{
			utils::PluginError("Invalid call result handle: " + std::to_string(handle) + ".");
			return NULL;
		}
		T *callResult = dynamic_cast<T*>(m_Items[handle]);
		if (!callResult)
		{
			utils::PluginError("Call result handle " + std::to_string(handle) + " was not the expected call result type.");
			return NULL;
		}
		return callResult;
	}
	void Delete(int handle);
	void Clear();
private:
	std::map<int, CCallResultItem*> m_Items;
	int m_CurrentHandle;
};

// Use this to access the call result map.
CCallResultMap *CallResults();

#endif // _CCALLRESULTITEM_H_
