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

#include "SteamScreenshots.h"
#include "DllMain.h"

CScreenshotReadyCallback ScreenshotReadyCallback;
CScreenshotRequestedCallback ScreenshotRequestedCallback;

/* @page ISteamScreenshots */

/*
@desc Adds a screenshot to the user's Steam screenshot library from disk.
A thumbnail is automatically created.

Triggers a ScreenshotReady_t callback.
@param filename The absolute file path to the JPG, PNG, or TGA screenshot.
@param width The width of the screenshot.
@param height The height of the screenshot.
@return A screenshot handle or 0 if the file could not be saved.
@url https://partner.steamgames.com/doc/api/ISteamScreenshots#AddScreenshotToLibrary
*/
extern "C" DLL_EXPORT int AddScreenshotToLibrary(const char *filename, int width, int height)
{
	CheckInitialized(INVALID_SCREENSHOT_HANDLE);
	return SteamScreenshots()->AddScreenshotToLibrary(filename, NULL, width, height);
}

/*
@desc Adds a VR screenshot to the user's Steam screenshot library from disk in the supported type.

Triggers a ScreenshotReady_t callback.
@param type The type of VR screenshot that this is.
@param-url type https://partner.steamgames.com/doc/api/ISteamScreenshots#EVRScreenshotType
@param filename The absolute file path to a 2D JPG, PNG, or TGA version of the screenshot for the library view.
@param vrFilename The absolute file path to the VR screenshot, this should be the same type of screenshot specified in eType.
@return A screenshot handle or 0 if the file could not be saved.
@url https://partner.steamgames.com/doc/api/ISteamScreenshots#AddVRScreenshotToLibrary
*/
extern "C" DLL_EXPORT int AddVRScreenshotToLibrary(int type, const char *filename, const char *vrFilename)
{
	CheckInitialized(INVALID_SCREENSHOT_HANDLE);
	return SteamScreenshots()->AddVRScreenshotToLibrary((EVRScreenshotType)type, filename, vrFilename);
}

/*
@desc Toggles whether the overlay handles screenshots when the user presses the screenshot hotkey, or if the game handles them.

Hooking is disabled by default, and only ever enabled if you do so with this function.

If the hooking is enabled, then the ScreenshotRequested_t callback will be sent if the user presses the hotkey or when 
TriggerScreenshot is called, and then the game is expected to call WriteScreenshot or AddScreenshotToLibrary in response.
@param hook 1 to enable or 0 to disable hooking.
@url https://partner.steamgames.com/doc/api/ISteamScreenshots#HookScreenshots
*/
extern "C" DLL_EXPORT void HookScreenshots(int hook)
{
	CheckInitialized(NORETURN);
	SteamScreenshots()->HookScreenshots(hook != 0);
}

/*
@desc Checks if the app is hooking screenshots, or if the Steam Overlay is handling them.

This can be toggled with HookScreenshots.
@return 1 if the game is hooking screenshots and is expected to handle them; otherwise, 0.
@url https://partner.steamgames.com/doc/api/ISteamScreenshots#IsScreenshotsHooked
*/
extern "C" DLL_EXPORT int IsScreenshotsHooked()
{
	CheckInitialized(false);
	return SteamScreenshots()->IsScreenshotsHooked();
}

/*
@desc Sets optional metadata about a screenshot's location. For example, the name of the map it was taken on.
@param hScreenshot The handle to the screenshot to tag.
@param location The location in the game where this screenshot was taken. This can not be longer than 255 characters.
@return 1 if the location was successfully added to the screenshot; otherwise, 0.
@url https://partner.steamgames.com/doc/api/ISteamScreenshots#SetLocation
*/
extern "C" DLL_EXPORT int SetScreenshotLocation(int hScreenshot, const char *location)
{
	CheckInitialized(false);
	return SteamScreenshots()->SetLocation((ScreenshotHandle)hScreenshot, location);
}

/*
@desc Tags a published file as being visible in the screenshot.
@param hScreenshot The handle to the screenshot to tag.
@param hPublishedFile The handle to the workshop item ID that is in the screenshot.
@return 1 if the published file was successfully tagged in the screenshot; otherwise, 0.
@url https://partner.steamgames.com/doc/api/ISteamScreenshots#TagPublishedFile
*/
extern "C" DLL_EXPORT int TagScreenshotPublishedFile(int hScreenshot, int hPublishedFile)
{
	CheckInitialized(false);
	return SteamScreenshots()->TagPublishedFile((ScreenshotHandle)hScreenshot, (PublishedFileId_t)SteamHandles()->GetSteamHandle(hPublishedFile));
}

/*
@desc Tags a Steam user as being visible in the screenshot.
@param hScreenshot The handle to the screenshot to tag.
@param hSteamID The handle of the Steam ID of a user that is in the screenshot.
@return 1 if the user was successfully tagged in the screenshot; otherwise, 0.
@url https://partner.steamgames.com/doc/api/ISteamScreenshots#TagUser
*/
extern "C" DLL_EXPORT int TagScreenshotUser(int hScreenshot, int hSteamID)
{
	CheckInitialized(false);
	return SteamScreenshots()->TagUser((ScreenshotHandle)hScreenshot, SteamHandles()->GetSteamHandle(hSteamID));
}

/*
@desc Either causes the Steam Overlay to take a screenshot, or tells your screenshot manager that a screenshot needs to be taken, depending on the value of IsScreenshotsHooked.

If hooking has been enabled, this triggers a ScreenshotRequested_t callback.
Otherwise this triggers a ScreenshotReady_t callback when the screenshot has been saved and added to the library.
@url https://partner.steamgames.com/doc/api/ISteamScreenshots#TriggerScreenshot
*/
extern "C" DLL_EXPORT void TriggerScreenshot()
{
	CheckInitialized(NORETURN);
	SteamScreenshots()->TriggerScreenshot();
}

#define RGBA_SIZE	4
#define RGB_SIZE	3

/*
@desc Writes a screenshot to the user's Steam screenshot library given the image ID.
@param imageID An image ID.
@return A screenshot handle or 0 if there is an error.
@url https://partner.steamgames.com/doc/api/ISteamScreenshots#WriteScreenshot
*/
extern "C" DLL_EXPORT int WriteScreenshot(int imageID)
{
	CheckInitialized(INVALID_SCREENSHOT_HANDLE);

	int nWidth = (int)agk::GetImageWidth(imageID);
	int nHeight = (int)agk::GetImageHeight(imageID);
	uint32 cubRGB = nWidth * nHeight * RGB_SIZE;
	uint8 *pubRGB = new uint8[cubRGB];
	// Convert RGBA to RGB.
	unsigned int memblock = agk::CreateMemblockFromImage(imageID);
	unsigned char *src = agk::GetMemblockPtr(memblock) + MEMBLOCK_IMAGE_HEADER_LENGTH;
	for (uint32 dst = 0; dst < cubRGB; src += RGBA_SIZE, dst += RGB_SIZE)
	{
		//memcpy_s(pubRGB + dst, RGB_SIZE, src, RGB_SIZE);
		memcpy(pubRGB + dst, src, RGB_SIZE);
	}
	agk::DeleteMemblock(memblock);
	ScreenshotHandle handle = SteamScreenshots()->WriteScreenshot(pubRGB, cubRGB, nWidth, nHeight);
	delete[] pubRGB;
	return handle;
}

// Callbacks
/*
@desc Triggered after a screenshot was successfully written or otherwise added to the library and can now be tagged.
@callback-type list
@callback-getters	GetScreenshotReadyScreenshotHandle, GetScreenshotReadyResult
@return 1 when the callback has more responses to process; otherwise 0.
@url https://partner.steamgames.com/doc/api/ISteamApps#ScreenshotReady_t
*/
extern "C" DLL_EXPORT int HasScreenshotReadyResponse()
{
	CheckInitialized(false);
	return ScreenshotReadyCallback.HasResponse();
}

/*
@desc Returns the screenshot handle for the current ScreenshotReady_t callback response.
@return A screenshot handle.
@url https://partner.steamgames.com/doc/api/ISteamApps#ScreenshotReady_t
*/
extern "C" DLL_EXPORT int GetScreenshotReadyScreenshotHandle()
{
	CheckInitialized(INVALID_SCREENSHOT_HANDLE);
	return ScreenshotReadyCallback.GetCurrent().m_hLocal;
}

/*
@desc Returns whether the call was successful for the current ScreenshotReady_t callback response.
@return An EResult value.
@return-url https://partner.steamgames.com/doc/api/steam_api#EResult
@url https://partner.steamgames.com/doc/api/ISteamApps#ScreenshotReady_t
*/
extern "C" DLL_EXPORT int GetScreenshotReadyResult()
{
	CheckInitialized(INVALID_SCREENSHOT_HANDLE);
	return (int)ScreenshotReadyCallback.GetCurrent().m_eResult;
}

/*
@desc Triggered when a screenshot has been requested by the user from the Steam screenshot hotkey.
This will only be called if HookScreenshots has been enabled, in which case Steam will not take the screenshot itself.
@callback-type bool
@return 1 when a screenshot has been requested.
@url https://partner.steamgames.com/doc/api/ISteamApps#ScreenshotRequested_t
*/
extern "C" DLL_EXPORT int HasScreenshotRequestedResponse()
{
	CheckInitialized(false);
	return ScreenshotRequestedCallback.HasResponse();
}

