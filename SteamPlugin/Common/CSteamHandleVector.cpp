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

#include "CSteamHandleVector.h"
#include "../AGKLibraryCommands.h"

CSteamHandleVector *SteamHandles()
{
	static CSteamHandleVector *handles = new CSteamHandleVector();
	return handles;
}

uint64 CSteamHandleVector::GetSteamHandle(int pluginHandle)
{
	if (pluginHandle >= 0 && pluginHandle < (int)m_SteamHandles.size())
	{
		return m_SteamHandles[pluginHandle];
	}
	agk::PluginError("Invalid Steam uint64 handle.");
	return 0;
}

int CSteamHandleVector::GetPluginHandle(uint64 handle)
{
	int index = (int)(std::find(m_SteamHandles.begin(), m_SteamHandles.end(), handle) - m_SteamHandles.begin());
	if (index < (int)m_SteamHandles.size())
	{
		// Handles are 1-based!
		return index;
	}
	m_SteamHandles.push_back(handle);
	return (int)m_SteamHandles.size() - 1;
}

int CSteamHandleVector::GetPluginHandle(CSteamID steamID)
{
	return GetPluginHandle(steamID.ConvertToUint64());
}

void CSteamHandleVector::Clear()
{
	m_SteamHandles.clear();
	// Handle 0 is always k_steamIDNil.
	m_SteamHandles.push_back(k_steamIDNil.ConvertToUint64());
}
