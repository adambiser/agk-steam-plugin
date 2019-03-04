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
