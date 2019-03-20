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

int IsMusicEnabled()
{
	CheckInitialized(0);
	return SteamMusic()->BIsEnabled();
}

int IsMusicPlaying()
{
	CheckInitialized(0);
	return SteamMusic()->BIsPlaying();
}

int GetMusicPlaybackStatus()
{
	CheckInitialized(AudioPlayback_Undefined);
	return SteamMusic()->GetPlaybackStatus();
}

float GetMusicVolume()
{
	CheckInitialized(0.0);
	return SteamMusic()->GetVolume();
}

void PauseMusic()
{
	CheckInitialized(NORETURN);
	SteamMusic()->Pause();
}

void PlayMusic()
{
	CheckInitialized(NORETURN);
	SteamMusic()->Play();
}

void PlayNextSong()
{
	CheckInitialized(NORETURN);
	SteamMusic()->PlayNext();
}

void PlayPreviousSong()
{
	CheckInitialized(NORETURN);
	SteamMusic()->PlayPrevious();
}

void SetMusicVolume(float volume)
{
	CheckInitialized(NORETURN);
	SteamMusic()->SetVolume(volume);
}

//Callbacks
int HasMusicPlaybackStatusChangedResponse()
{
	CheckInitialized(0);
	return Callbacks()->HasPlaybackStatusHasChangedResponse();
}

int HasMusicVolumeChangedResponse()
{
	CheckInitialized(0);
	return Callbacks()->HasVolumeHasChangedResponse();
}
