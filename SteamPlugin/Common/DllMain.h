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

#ifndef _DLLMAIN_H_
#define _DLLMAIN_H_
#pragma once

#include "..\AGKLibraryCommands.h"

#if defined(_WINDOWS)
#if defined(_WIN64)
#pragma comment(lib, "steam_api64.lib")
#else
#pragma comment(lib, "steam_api.lib")
#endif
#endif
#include "steam_api.h"

#include "CCallbacks.h"
#include "CCallResultItem.h"
#include "CSteamHandleVector.h"
#include "CRoomManager.h"
#include <mutex>
#include <string>
#include <sstream>
#include <iomanip>
#include <vector>

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

// From winnt.h
#ifndef _MAC
typedef wchar_t WCHAR;    // wc,   16-bit UNICODE character
#else
typedef unsigned short WCHAR;    // wc,   16-bit UNICODE character
#endif

namespace utils
{
	/*
	Converts a const char* to agk:string.
	*/
	static inline char *CreateString(const char* text)
	{
		if (text)
		{
			int length = (int)strlen(text) + 1;
			char *result = agk::CreateString(length);
			strcpy_s(result, length, text);
			return result;
		}
		char *result = agk::CreateString(1);
		strcpy_s(result, 1, "");
		return result;
	}

	static inline char *CreateString(std::string text)
	{
		return CreateString(text.c_str());
	}

	static inline char *CreateString(const WCHAR* text)
	{
		size_t length = wcslen(text) + 1;
		char *result = agk::CreateString((int)length);
		size_t size;
		wcstombs_s(&size, result, length, text, length);
		return result;
	}

	static inline char *CreateString(std::ostringstream& stream)
	{
		return CreateString(stream.str());
	}

	static inline void Log(const char *msg) // pass through without converting to std::string.
	{
		agk::Log(msg);
	}

	static inline void Log(std::string msg)
	{
		agk::Log(msg.c_str());
	}

	static inline void PluginError(std::string msg)
	{
		agk::PluginError(msg.c_str());
	}

	static inline std::string ToIPString(uint32 ip)
	{
		if (ip == 0)
		{
			return std::string();
		}
		return std::to_string((ip >> 24) & 0xff) + "." + std::to_string((ip >> 16) & 0xff) + "." + std::to_string((ip >> 8) & 0xff) + "." + std::to_string((ip >> 0) & 0xff);
	}

	static inline bool ParseIP(const char *ipv4, unsigned __int32 *ip)
	{
		if (strnlen_s(ipv4, 12) == 0)
		{
			*ip = 0;
			return true;
		}
		int ip1, ip2, ip3, ip4;
		sscanf(ipv4, "%d.%d.%d.%d", &ip1, &ip2, &ip3, &ip4);
		if ((ip1 < 0 || ip1 > 255)
			|| (ip2 < 0 || ip2 > 255)
			|| (ip3 < 0 || ip3 > 255)
			|| (ip4 < 0 || ip4 > 255)
			)
		{
			agk::PluginError("Could not parse IP address.");
			return false;
		}
		*ip = (ip1 << 24) | (ip2 << 16) | (ip3 << 8) | (ip4);
		return true;
	}

	static inline std::string GetSHA1(uint8 shabytes[20])
	{
		bool allZeroes = true;
		for (int x = 0; x < 20; x++)
		{
			if (shabytes[x] != 0)
			{
				allZeroes = false;
				break;
			}
		}
		if (allZeroes)
		{
			return std::string();
		}
		std::ostringstream sha;
		sha << std::uppercase << std::setfill('0') << std::hex;
		for (int x = 0; x < 20; x++)
		{
			sha << std::setw(2) << (int)shabytes[x];
		}
		return sha.str();
	}

	static inline std::string EscapeJSON(const std::string &input)
	{
		std::ostringstream output;
		for (auto c = input.cbegin(); c != input.cend(); c++)
		{
			switch (*c) {
			case '"':
				output << "\\\"";
				break;
			case '\\':
				output << "\\\\";
				break;
			case '\b':
				output << "\\b";
				break;
			case '\f':
				output << "\\f";
				break;
			case '\n':
				output << "\\n";
				break;
			case '\r':
				output << "\\r";
				break;
			case '\t':
				output << "\\t";
				break;
			default:
				if ('\x00' <= *c && *c <= '\x1f')
				{
					char buffer[8];
					sprintf_s(buffer, "\\u%04x", *c);
					output << buffer;
				}
				else
				{
					output << *c;
				}
				break;
			}
		}
		return output.str();
	}

	//template <typename T>
	//struct GetTypeNameHelper
	//{
	//	static const unsigned int FRONT_SIZE = sizeof("utils::GetTypeNameHelper<struct ") - 1u;
	//	static const unsigned int BACK_SIZE = sizeof(">::GetTypeName") - 1u;

	//	static const char* GetTypeName(void)
	//	{
	//		static const size_t size = sizeof(__FUNCTION__) - FRONT_SIZE - BACK_SIZE;
	//		static char typeName[size] = {};
	//		memcpy(typeName, __FUNCTION__ + FRONT_SIZE, size - 1u);
	//		return typeName;
	//	}
	//};

	//template <typename T>
	//static inline const char* GetTypeName(void)
	//{
	//	return GetTypeNameHelper<T>::GetTypeName();
	//}
};

#endif // _DLLMAIN_H_
