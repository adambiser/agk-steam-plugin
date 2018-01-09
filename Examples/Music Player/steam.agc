#option_explicit

global volume as float

#constant PLAY_BUTTON			1
#constant PAUSE_BUTTON			2
#constant PREV_SONG_BUTTON		3
#constant NEXT_SONG_BUTTON		4
#constant DEC_VOLUME_BUTTON		5
#constant INC_VOLUME_BUTTON		6

global buttonText as string[5] = ["PLAY", "PAUSE", "PREV_SONG", "NEXT_SONG", "DECREASE_VOLUME", "INCREASE_VOLUME"]
x as integer
for x = 0 to buttonText.length
	CreateButton(x + 1, 752 + (x - 5) * 100, 40, ReplaceString(buttonText[x], "_", NEWLINE, -1))
	SetButtonEnabled(x + 1, 1)
next

// Dump favorite game information.
AddStatus("Music enabled: " + TF(Steam.IsMusicEnabled()))
AddStatus("Music playing: " + TF(Steam.IsMusicPlaying()))
AddStatus("Status: " + GetMusicPlaybackStatusText(Steam.GetMusicPlaybackStatus()))
volume = Steam.GetMusicVolume()
AddStatus("Volume: " + str(volume))

//
// The main loop
//
do
	Sync()
	CheckInput()
	// Very important!  This MUST be called each frame to allow the Steam API callbacks to process.
	Steam.RunCallbacks()
	ProcessCallbacks()
	if GetRawKeyPressed(KEY_ESCAPE) 
		exit
	endif
loop

//
// Check and handle input.
//
Function CheckInput()
	// Scrollable text.
	PerformVerticalScroll(statusArea)
	if GetVirtualButtonPressed(PLAY_BUTTON)
		AddStatus("PlayMusic")
		Steam.PlayMusic()
	endif
	if GetVirtualButtonPressed(PAUSE_BUTTON)
		AddStatus("PauseMusic")
		Steam.PauseMusic()
	endif
	if GetVirtualButtonPressed(PREV_SONG_BUTTON)
		AddStatus("PlayPreviousSong")
		Steam.PlayPreviousSong()
	endif
	if GetVirtualButtonPressed(NEXT_SONG_BUTTON)
		AddStatus("PlayNextSong")
		Steam.PlayNextSong()
	endif
	if GetVirtualButtonPressed(DEC_VOLUME_BUTTON)
		AddStatus("Decrease volume")
		dec volume, 0.1
		if volume < 0
			volume = 0
		endif
		Steam.SetMusicVolume(volume)
	endif
	if GetVirtualButtonPressed(INC_VOLUME_BUTTON)
		AddStatus("Increase volume")
		inc volume, 0.1
		if volume > 1
			volume = 1
		endif
		Steam.SetMusicVolume(volume)
	endif
EndFunction

// Processes all asynchronous callbacks.
//
Function ProcessCallbacks()
	if Steam.HasMusicPlaybackStatusChanged()
		AddStatus("Status changed: " + GetMusicPlaybackStatusText(Steam.GetMusicPlaybackStatus()))
	endif
	if Steam.HasMusicVolumeChanged()
		volume = Steam.GetMusicVolume()
		AddStatus("Volume changed: " + str(volume))
	endif
EndFunction

Function GetMusicPlaybackStatusText(status as integer)
	select status
		case AudioPlayback_Undefined
			ExitFunction "Undefined"
		endcase
		case AudioPlayback_Playing
			ExitFunction "Playing"
		endcase
		case AudioPlayback_Paused
			ExitFunction "Paused"
		endcase
		case AudioPlayback_Idle
			ExitFunction "Idle"
		endcase
	endselect
EndFunction "Invalid"
