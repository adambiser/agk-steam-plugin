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

#include "DllMain.h"
#include <map>

class CCallResultItemBase
{
public:
	CCallResultItemBase() :
		m_eResult((EResult)0),
		m_CallResultName("Unnamed Call Result"),
		m_bRunning(false),
		m_hSteamAPICall(k_uAPICallInvalid) {}
	virtual ~CCallResultItemBase(void) {}
	// Return a 'name' for the CCallResultItemBase object, usually indicates type and some useful parameter values.
	std::string GetName() { return m_CallResultName; }
	EResult GetResultCode() { return m_eResult; }
protected:
	SteamAPICall_t m_hSteamAPICall;
	std::string m_CallResultName;
	// Throw std::string for errors.
	virtual SteamAPICall_t Call()
	{
		throw std::string(GetName() + ": Call is not defined.");
	}
	virtual void Register() = 0;
	// Called from CCallResultMap.Add().
	friend void FriendRegister(CCallResultItemBase &callResult)
	{
		callResult.Register();
	}
	void SetResultCode(EResult eResult, bool bFailure = false);
	bool m_bRunning;
private:
	EResult m_eResult;
};

template <class callback_type, class response_type = callback_type>
class CCallResultItem : public CCallResultItemBase
{
public:
	CCallResultItem() : iCallback(callback_type::k_iCallback){}
	virtual ~CCallResultItem(void)
	{
		m_CallResult.Cancel();
	}
	int GetCallResultType() { return iCallback; }
protected:
	response_type m_Response;
	int iCallback;
	void Register()
	{
		if (m_hSteamAPICall == k_uAPICallInvalid)
		{
			throw std::string(GetName() + ": Call returned k_uAPICallInvalid.");
		}
		m_CallResult.Set(m_hSteamAPICall, this, &CCallResultItem<callback_type, response_type>::OnResponse);
	}
	virtual void OnResponse(callback_type *pCallResult, bool bFailure)
	{
		m_Response = *pCallResult;
		SetResultCode(m_Response.m_eResult, bFailure);
		utils::Log(GetName() + ":  Result code = " + std::to_string(m_Response.m_eResult));
	}
private:
	CCallResult<CCallResultItem<callback_type, response_type>, callback_type> m_CallResult;
};

/*
Map for maintaining all existing call results.
*/
class CCallResultMap
{
public:
	CCallResultMap() {}
	virtual ~CCallResultMap(void) {}

	int Add(CCallResultItemBase *callResult);
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
	std::map<int, CCallResultItemBase*> m_Items;
	int m_CurrentHandle;
};

// Use this to access the call result map.
CCallResultMap *CallResults();

template <typename callresult_type>
int GetCallResultValue(int hCallResult, int(callresult_type::*function)(void))
{
	if (auto *callResult = CallResults()->Get<callresult_type>(hCallResult))
	{
		return (callResult->*function)();
	}
	return 0;
}

template <typename callresult_type>
int GetCallResultValue(int hCallResult, int index, int(callresult_type::*function)(int), char *functionName)
{
	if (auto *callResult = CallResults()->Get<callresult_type>(hCallResult))
	{
		if (callResult->IsValidIndex(index))
		{
			return (callResult->*function)(index);
		}
		utils::PluginError(functionName + std::string(": Index out of range."));
	}
	return 0;
}

template <typename callresult_type>
int GetCallResultValue(int hCallResult, uint64(callresult_type::*function)(void))
{
	if (auto *callResult = CallResults()->Get<callresult_type>(hCallResult))
	{
		return SteamHandles()->GetPluginHandle((callResult->*function)());
	}
	return 0;
}

template <typename callresult_type>
int GetCallResultValue(int hCallResult, CSteamID(callresult_type::*function)(void))
{
	if (auto *callResult = CallResults()->Get<callresult_type>(hCallResult))
	{
		return SteamHandles()->GetPluginHandle((callResult->*function)());
	}
	return 0;
}

template <typename callresult_type>
int GetCallResultValue(int hCallResult, int index, uint64(callresult_type::*function)(int), char *functionName)
{
	if (auto *callResult = CallResults()->Get<callresult_type>(hCallResult))
	{
		if (callResult->IsValidIndex(index))
		{
			return SteamHandles()->GetPluginHandle((callResult->*function)(index));
		}
		utils::PluginError(functionName + std::string(": Index out of range."));
	}
	return 0;
}

template <typename callresult_type>
int GetCallResultValue(int hCallResult, int index, CSteamID(callresult_type::*function)(int), char *functionName)
{
	if (auto *callResult = CallResults()->Get<callresult_type>(hCallResult))
	{
		if (callResult->IsValidIndex(index))
		{
			return SteamHandles()->GetPluginHandle((callResult->*function)(index));
		}
		utils::PluginError(functionName + std::string(": Index out of range."));
	}
	return 0;
}

template <typename callresult_type>
char *GetCallResultValue(int hCallResult, std::string(callresult_type::*function)(void))
{
	if (auto *callResult = CallResults()->Get<callresult_type>(hCallResult))
	{
		return utils::CreateString((callResult->*function)());
	}
	return agk::CreateString(0);
}

template <typename callresult_type>
char *GetCallResultValue(int hCallResult, char *(callresult_type::*function)(void))
{
	if (auto *callResult = CallResults()->Get<callresult_type>(hCallResult))
	{
		return utils::CreateString((callResult->*function)());
	}
	return agk::CreateString(0);
}

// Used to wrap Steamworks structs that don't have an m_eResult member.
// Check m_bSuccess for the eResult value.
template <class T>
struct SuccessResponse : T
{
	EResult m_eResult;
	SuccessResponse<T>() : m_eResult((EResult)0) {}
	SuccessResponse<T>(T &from) : T(from), m_eResult(from.m_bSuccess ? k_EResultOK : k_EResultFail) {}
};

// Check <member_name> for the eResult value.
template <class T, class member_type, member_type T::*member_name>
struct WrappedResponse : T
{
	EResult m_eResult;
	WrappedResponse<T, member_type, member_name>() : m_eResult((EResult)0) {}
	WrappedResponse<T, member_type, member_name>(T &from) : T(from), m_eResult((from.*member_name != 0) ? k_EResultOK : k_EResultFail) {}
};

// eResult is always k_EResultOK.
template <class T>
struct AlwaysOKResponse : T
{
	EResult m_eResult;
	AlwaysOKResponse<T>() : m_eResult((EResult)0) {}
	AlwaysOKResponse<T>(T &from) : T(from), m_eResult(k_EResultOK) {}
};

#endif // _CCALLRESULTITEM_H_
