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

#ifndef _CALLBACKS_H
#define _CALLBACKS_H
#pragma once

#include "stdafx.h"

extern bool g_SteamInitialized;

class CallbackBase
{
protected:
	static std::vector<CallbackBase*> m_AllCallbacks;
public:
	CallbackBase()
	{
		m_AllCallbacks.push_back(this);
	};
	virtual ~CallbackBase() {}
	virtual void Reset() {}
	static void ResetAll()
	{
		for (auto const& callback : m_AllCallbacks)
		{
			callback->Reset();
		}
	}
};

template <class derived_type, class callback_type>
class BoolCallbackBase : public CallbackBase
{
public:
	BoolCallbackBase() : CallbackBase()
	{
		m_Callback.Register(this, &BoolCallbackBase<derived_type, callback_type>::OnResponse);
	};
	~BoolCallbackBase() {}
	virtual void Reset() // final
	{
		//Derived().Clear();
		m_bHadResponse = false;
	}
	/*virtual */bool HasResponse() //final
	{
		if (m_bHadResponse)
		{
			m_bHadResponse = false;
			return true;
		}
		//Derived().Clear();
		Reset();
		return false;
	}
	// "Optional" overrides for derived classes.
	//template<typename T = derived_type>
	//auto OnResponse(callback_type *pParam) -> decltype(std::declval<T>().OnResponse(pParam), void()) {
	//	Derived().OnResponse(pParam);
	//}
	//template<typename T = derived_type>
	//auto Clear() -> decltype(std::declval<T>().Clear(), void()) {
	//	Derived().Clear();
	//}
protected:
	derived_type& Derived()
	{
		return *static_cast<derived_type*>(this);
	}
	bool m_bHadResponse;
	CCallbackManual<BoolCallbackBase<derived_type, callback_type>, callback_type> m_Callback;

	//typedef BoolCallbackBase<derived_type, callback_type> base;
	//STEAM_CALLBACK(base, OnResponse, callback_type)
	//{
	//	//Derived().OnResponse(pParam);
	//}
	void OnResponse(callback_type * /*pParam*/)
	{
		m_bHadResponse = true;
	}
	//void Clear()
	//{
	//	m_bHadResponse = false;
	//}
};

template <class derived_type, class callback_type, class list_type = callback_type>
class ListCallbackBase : public CallbackBase
{
public:
	ListCallbackBase() : CallbackBase(), m_bRegistered(false) {};
	virtual ~ListCallbackBase() {}
	virtual void Register() // final
	{
		agk::Log("Inside");
		//Derived().RegisterInner();
		if (!g_SteamInitialized)
		{
			return;
		}
		if (!m_bRegistered)
		{
			m_bRegistered = true;
			m_Callback.Register(this, &ListCallbackBase<derived_type, callback_type, list_type>::OnResponse);
		}
		agk::Log("Done");
	}
	virtual void Unregister() // final
	{
		//Derived().UnregisterInner();
		if (!g_SteamInitialized)
		{
			return;
		}
		if (m_bRegistered)
		{
			m_bRegistered = false;
			m_Callback.Unregister();
		}
	}
	virtual void OnResponse(callback_type *pParam) = 0;
	// "Optional" overrides for derived classes.
	//template<typename T = derived_type>
	//auto RegisterInner() -> decltype(std::declval<T>().RegisterInner(), void()) {
	//	Derived().RegisterInner();
	//}
	//template<typename T = derived_type>
	//auto UnregisterInner() -> decltype(std::declval<T>().UnregisterInner(), void()) {
	//	Derived().UnregisterInner();
	//}
	// Clears the callback response list and current value.
	virtual void Reset() final
	{
		m_Mutex.lock();
		Derived().Clear(m_CurrentValue);
		for (auto iter = m_List.begin(); iter != m_List.end(); iter++) {
			Derived().Clear(*iter);
		}
		m_List.clear();
		Unregister();
		m_Mutex.unlock();
	}
	// Only for use in the callback function.
	virtual void StoreResponse(list_type result) final
	{
		m_Mutex.lock();
		m_List.push_back(result);
		m_Mutex.unlock();
	}
	virtual bool HasResponse() final
	{
		if (!g_SteamInitialized)
		{
			return false;
		}
		// Register the callback if the game is checking for responses.
		if (!m_bRegistered)
		{
			Register();
		}
		m_Mutex.lock();
		if (m_List.size() > 0)
		{
			m_CurrentValue = m_List.front();
			m_List.pop_front();
			m_Mutex.unlock();
			return true;
		}
		Derived().Clear(m_CurrentValue);
		m_Mutex.unlock();
		return false;
	}
	virtual list_type GetCurrent() const final
	{
		return m_CurrentValue;
	}
	virtual bool IsRegistered() const final
	{
		return m_bRegistered;
	}
protected:
	derived_type& Derived()
	{
		return *static_cast<derived_type*>(this);
	}
	//void RegisterInner()
	//{
	//	if (!g_SteamInitialized)
	//	{
	//		return;
	//	}
	//	if (!m_bRegistered)
	//	{
	//		m_bRegistered = true;
	//		m_Callback.Register(this, &ListCallbackBase<derived_type, callback_type, list_type>::OnResponseInner);
	//	}
	//}
	//void UnregisterInner()
	//{
	//	if (!g_SteamInitialized)
	//	{
	//		return;
	//	}
	//	if (m_bRegistered)
	//	{
	//		m_bRegistered = false;
	//		m_Callback.Unregister();
	//	}
	//}
	//void OnResponseInner(callback_type *pParam)
	//{
	//	Derived().OnResponse(pParam);
	//}
	list_type m_CurrentValue;
	bool m_bRegistered;
	std::list<list_type> m_List;
	std::mutex m_Mutex;
	CCallbackManual<ListCallbackBase<derived_type, callback_type, list_type>, callback_type> m_Callback;
};

#endif // _CALLBACKS_H
