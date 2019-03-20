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

#ifndef _COMMON_H
#define _COMMON_H
#pragma once

#include "..\AGKLibraryCommands.h"
#include "CCallbacks.h"
#include "CCallResultItem.h"
#include "CSteamHandleVector.h"
#include "StructClear.h"
#include "Utils.h"
#if defined(_WINDOWS)
#if defined(_WIN64)
#pragma comment(lib, "steam_api64.lib")
#else
#pragma comment(lib, "steam_api.lib")
#endif
#endif
#include "steam_api.h"
#include <mutex>
#include <vector>

#define AVATAR_SMALL	0 // 32x32
#define AVATAR_MEDIUM	1 // 64x64
#define AVATAR_LARGE	2 // 128x128

extern uint64 g_AppID;
extern bool g_SteamInitialized;
extern bool g_StoringStats;
extern std::mutex g_JoinedLobbiesMutex;
extern std::vector<CSteamID> g_JoinedLobbies;

extern int g_InputCount;
extern InputAnalogActionData_t g_InputAnalogActionData;
extern InputDigitalActionData_t g_InputDigitalActionData;
extern InputMotionData_t g_InputMotionData;

/*
Check to see if the SteamPlugin has been initialized.
If it has not been, return a default value.
*/
#define CheckInitialized(returnValue)	\
	if (!g_SteamInitialized)			\
	{									\
		return returnValue;				\
	}

// Token for passing an empty returnValue into CheckInitialized();
#define NORETURN
#define NULL_STRING agk::CreateString(0)

#define Clamp(value, min, max)	\
	if (value < min)			\
	{							\
		value = min;			\
	}							\
	else if (value > max)		\
	{							\
		value = max;			\
	}

inline bool InRange(int value, int min, int max)
{
	return (min <= value && value <= max);
}

#endif // _COMMON_H
