#option_explicit

infoTextID as integer
infoTextID = CreateTextEx(0, 100, "")

Steam.SetOverlayNotificationPosition(k_EPositionTopRight)
Steam.SetOverlayNotificationInset(50, 50)

AddStatus("IP Country: " + Steam.GetIPCountry())
AddStatus("Steam UI Language: " + Steam.GetSteamUILanguage())

info as string
//
// The main loop
//
do
	info = "Inter-Process Communication Call Count: " + str(Steam.GetIPCCallCount()) + NEWLINE
	info = info + "Seconds Since Steam Started: " + str(Steam.GetSecondsSinceAppActive()) + NEWLINE
	info = info + "Seconds Since Last Mouse Move: " + str(Steam.GetSecondsSinceComputerActive()) + NEWLINE
	info = info + "Server Real Time: " + GetDateFromUnix(Steam.GetServerRealTime()) + NEWLINE
	info = info + "Overlay Enabled: " + TF(Steam.IsOverlayEnabled()) + NEWLINE
	info = info + "Battery Power: " + str(Steam.GetCurrentBatteryPower()) + NEWLINE
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
	// No callbacks in this example.
EndFunction
