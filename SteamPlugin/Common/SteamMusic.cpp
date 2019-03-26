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

#include "Common.h"

/* @page ISteamMusic */

/*
@desc Checks if Steam Music is enabled.
@return 1 if Steam Music is enabled; otherwise 0.
@api ISteamMusic#BIsEnabled
*/
extern "C" DLL_EXPORT int IsMusicEnabled()
{
	CheckInitialized(0);
	return SteamMusic()->BIsEnabled();
}

/*
@desc Checks if Steam Music is active. This does not necessarily mean that a song is currently playing, it may be paused.
@return 1 if Steam Music is active; otherwise 0.
@api ISteamMusic#BIsPlaying
*/
extern "C" DLL_EXPORT int IsMusicPlaying()
{
	CheckInitialized(0);
	return SteamMusic()->BIsPlaying();
}

/*
@desc Gets the current status of the Steam Music player.
@return An AudioPlayback_Status value
@return-url https://partner.steamgames.com/doc/api/ISteamMusic#AudioPlayback_Status
@api ISteamMusic#GetPlaybackStatus
*/
extern "C" DLL_EXPORT int GetMusicPlaybackStatus()
{
	CheckInitialized(AudioPlayback_Undefined);
	return SteamMusic()->GetPlaybackStatus();
}

/*
@desc Gets the current volume of the Steam Music player.
@return The volume is returned as a percentage between 0.0 and 1.0.
@api ISteamMusic#GetVolume
*/
extern "C" DLL_EXPORT float GetMusicVolume()
{
	CheckInitialized(0.0);
	return SteamMusic()->GetVolume();
}

/*
@desc Pause the Steam Music player.
@api ISteamMusic#Pause
*/
extern "C" DLL_EXPORT void PauseMusic()
{
	CheckInitialized(NORETURN);
	SteamMusic()->Pause();
}

/*
@desc Have the Steam Music player resume playing.
@api ISteamMusic#Play
*/
extern "C" DLL_EXPORT void PlayMusic()
{
	CheckInitialized(NORETURN);
	SteamMusic()->Play();
}

/*
@desc Have the Steam Music player skip to the next song.
@api ISteamMusic#PlayNext
*/
extern "C" DLL_EXPORT void PlayNextSong()
{
	CheckInitialized(NORETURN);
	SteamMusic()->PlayNext();
}

/*
@desc Have the Steam Music player play the previous song.
@api ISteamMusic#PlayPrevious
*/
extern "C" DLL_EXPORT void PlayPreviousSong()
{
	CheckInitialized(NORETURN);
	SteamMusic()->PlayPrevious();
}

/*
@desc Sets the volume of the Steam Music player.
@param volume The volume percentage to set from 0.0 to 1.0.
@api ISteamMusic#SetVolume
*/
extern "C" DLL_EXPORT void SetMusicVolume(float volume)
{
	CheckInitialized(NORETURN);
	SteamMusic()->SetVolume(volume);
}

//Callbacks
/*
@desc Notifies the caller that the music playback status has changed since the last call.
@callback-type bool
@return 1 when a change has occurred; otherwise 0.
@api ISteamMusic#PlaybackStatusHasChanged_t
*/
extern "C" DLL_EXPORT int HasMusicPlaybackStatusChangedResponse()
{
	CheckInitialized(0);
	return Callbacks()->HasPlaybackStatusHasChangedResponse();
}

/*
@desc Notifies the caller that the music volume has changed since the last call.
@callback-type bool
@return 1 when a change has occurred; otherwise 0.
@api ISteamMusic#VolumeHasChanged_t
*/
extern "C" DLL_EXPORT int HasMusicVolumeChangedResponse()
{
	CheckInitialized(0);
	return Callbacks()->HasVolumeHasChangedResponse();
}
