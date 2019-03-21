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

#include "CCallResultItem.h"

void CCallResultItemBase::SetResultCode(EResult eResult, bool bFailure)
{
	if (bFailure && eResult <= k_EResultOK)
	{
		// Failure is true, but eResult doesn't indicate failure?  Why?  Can this happen?
		utils::Log(GetName() + ": bFailure is true and eResult is " + std::to_string(eResult) + ", so... fail?");
		m_eResult = k_EResultFail;
	}
	else if (!bFailure && eResult == 0)
	{
		// This happens for call result parameter types that don't have a m_eResult member variable.
		utils::Log(GetName() + ": bFailure is false and eResult is 0, so... OK?");
		m_eResult = k_EResultOK;
	}
	else
	{
		m_eResult = eResult;
	}
}

// Add the call result and run it.
int CCallResultMap::Add(CCallResultItemBase *callResult)
{
	if (callResult)
	{
		try
		{
			utils::Log(callResult->GetName() + ": Calling.");
			FriendCreateAPICall(*callResult);
			// Find the next unassigned handle.
			while (m_Items.find(++m_CurrentHandle) != m_Items.end());
			m_Items[m_CurrentHandle] = callResult;
			return m_CurrentHandle;
		}
		catch (std::string e)
		{
			utils::PluginError(e);
		}
		catch (...)
		{
			utils::PluginError(callResult->GetName() + ": Call threw an unexpected error.");
		}
	}
	return 0;
}

void CCallResultMap::Delete(int handle)
{
	auto search = m_Items.find(handle);
	if (search != m_Items.end())
	{
		delete search->second;
		m_Items.erase(search);
	}
}

void CCallResultMap::Clear()
{
	for (auto iter = m_Items.begin(); iter != m_Items.end(); iter++) {
		delete iter->second;
	}
	m_Items.clear();
	m_CurrentHandle = 0;
}

CCallResultMap *CallResults()
{
	// Construct on first use.
	static CCallResultMap *map = new CCallResultMap();
	return map;
}
