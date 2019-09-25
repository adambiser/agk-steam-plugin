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

#ifndef _STDAFX_H_
#define _STDAFX_H_
#pragma once

#include "../AGKLibraryCommands.h"

#if defined(_WINDOWS)
#if defined(_WIN64)
#pragma comment(lib, "steam_api64.lib")
#else
#pragma comment(lib, "steam_api.lib")
#endif
#endif
#ifndef _MSC_VER // Linux
#include <limits.h>
// This won't link in GCC unless steam_api is visible.  Undo the visibility set in AGKLibraryCommands and redo it afterwards.
// Also needed for atoll() from stdlib.
#pragma GCC visibility pop
#include <stdlib.h>
#define _MAX_PATH PATH_MAX
#endif
#include "steam/steam_api.h"
#ifndef _MSC_VER
#pragma GCC visibility push(hidden)
#endif
#include <string>
#include <list>
#include <map>
#include <mutex>
#include <sstream>
#include <vector>
#include <algorithm>
#include <inttypes.h>

#include "CSteamHandleVector.h"
#include "CCallbacks.h"
#include "CCallResultItem.h"
#include "CRoomManager.h"

#define AVATAR_SMALL	0 // 32x32
#define AVATAR_MEDIUM	1 // 64x64
#define AVATAR_LARGE	2 // 128x128

#define MEMBLOCK_IMAGE_HEADER_LENGTH	12

extern bool g_SteamInitialized;
extern uint32 g_AppID;
/* @ignore docs */
extern "C" DLL_EXPORT int RequestCurrentStats();
extern void ClearMostAchievedAchievementInfo();
extern void ResetSteamInput();

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

#endif // _STDAFX_H_
