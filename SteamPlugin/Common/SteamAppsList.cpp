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

#include "stdafx.h"

/* @page ISteamAppsList
This is a restricted interface that can only be used by approved apps.
*/

/*
@desc Returns the app name for the given appid.
@param appID The AppID to get the name for.
@return The app name or an empty string if the app has not been approved for this method.
@url https://partner.steamgames.com/doc/api/ISteamAppList#GetAppName
*/
extern "C" DLL_EXPORT char *GetAppName(int appID)
{
	CheckInitialized(NULL_STRING);
	char name[256];
	// Note: This does not behave how the SDK says it does.  See comments in the code below.
	int length = SteamAppList()->GetAppName(appID, name, sizeof(name));
	// Length of 0 means there is no app of that id.
	if (length == 0)
	{
		return utils::CreateString("Not found");
	}
	// Length > 0 means the name was found.
	if (length > 0)
	{
		name[length] = 0;
		return utils::CreateString(name);
	}
	// Length < 0 means the name needs to be cached.  Ask again later.
	return agk::CreateString(0);
}
