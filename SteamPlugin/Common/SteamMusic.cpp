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

#include "SteamPlugin.h"

// Music methods
bool SteamPlugin::IsMusicEnabled()
{
	CheckInitialized(SteamMusic, false);
	return SteamMusic()->BIsEnabled();
}

bool SteamPlugin::IsMusicPlaying()
{
	CheckInitialized(SteamMusic, false);
	return SteamMusic()->BIsPlaying();
}

AudioPlayback_Status SteamPlugin::GetMusicPlaybackStatus()
{
	CheckInitialized(SteamMusic, AudioPlayback_Undefined);
	return SteamMusic()->GetPlaybackStatus();
}

float SteamPlugin::GetMusicVolume()
{
	CheckInitialized(SteamMusic, 0.0);
	return SteamMusic()->GetVolume();
}

void SteamPlugin::PauseMusic()
{
	CheckInitialized(SteamMusic, );
	SteamMusic()->Pause();
}

void SteamPlugin::PlayMusic()
{
	CheckInitialized(SteamMusic, );
	SteamMusic()->Play();
}

void SteamPlugin::PlayNextSong()
{
	CheckInitialized(SteamMusic, );
	SteamMusic()->PlayNext();
}

void SteamPlugin::PlayPreviousSong()
{
	CheckInitialized(SteamMusic, );
	SteamMusic()->PlayPrevious();
}

void SteamPlugin::SetMusicVolume(float flVolume)
{
	CheckInitialized(SteamMusic, );
	SteamMusic()->SetVolume(flVolume);
}

void SteamPlugin::OnPlaybackStatusHasChanged(PlaybackStatusHasChanged_t *pParam)
{
	agk::Log("Callback: Music Playback Status Changed");
	m_PlaybackStatusHasChanged = true;
}

bool SteamPlugin::HasMusicPlaybackStatusChanged()
{
	// Reports true only once per change.
	if (m_PlaybackStatusHasChanged)
	{
		m_PlaybackStatusHasChanged = false;
		return true;
	}
	return false;
}

void SteamPlugin::OnVolumeHasChanged(VolumeHasChanged_t *pParam)
{
	agk::Log("Callback: Music Volume Changed");
	m_VolumeHasChanged = true;
}

bool SteamPlugin::HasMusicVolumeChanged()
{
	// Reports true only once per change.
	bool temp = m_VolumeHasChanged;
	m_VolumeHasChanged = false;
	return temp;
}
