#option_explicit

infoTextID as integer
infoTextID = CreateTextEx(0, 100, "")

Steam.SetOverlayNotificationPosition(k_EPositionTopRight)
Steam.SetOverlayNotificationInset(50, 50)

// This should cause the Steam API to send API warnings to AGK's Debug Log,
// However, I have never seen a warning show up and don't know how to force
// one to fire, so I can only assume the plugin code is correct.
Steam.SetWarningMessageHook()

AddStatus("IP Country: " + Steam.GetIPCountry())
AddStatus("Steam UI Language: " + Steam.GetSteamUILanguage())

info as string
//
// The main loop
//
do
	info = "Inter-Process Communication Call Count: " + str(Steam.GetIPCCallCount()) + NEWLINE
	info = info + "Seconds Since App Active: " + str(Steam.GetSecondsSinceAppActive()) + NEWLINE
	info = info + "Seconds Since Last Mouse Move: " + str(Steam.GetSecondsSinceComputerActive()) + NEWLINE
	info = info + "Server Real Time: " + GetDateFromUnix(Steam.GetServerRealTime()) + NEWLINE
	info = info + "Overlay Enabled: " + TF(Steam.IsOverlayEnabled()) + NEWLINE
	info = info + "Current Battery Power: " + str(Steam.GetCurrentBatteryPower()) + NEWLINE
	SetTextString(infoTextID, info)
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
EndFunction

//
// Processes all asynchronous callbacks.
//
Function ProcessCallbacks()
	if Steam.HasIPCountryChanged()
		AddStatus("IP Country Changed: " + Steam.GetIPCountry())
	endif
	if Steam.HasLowBatteryWarning()
		AddStatus("Low battery warning!  " + str(Steam.GetMinutesBatteryLeft()) + " minute(s) remaining.")
	endif
	if Steam.IsSteamShuttingDown()
		AddStatus("Steam is shutting down.")
	endif
EndFunction
