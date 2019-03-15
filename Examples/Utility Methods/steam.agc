#option_explicit

#constant OPEN_URL_BUTTON			1
#constant OPEN_URL_MODAL_BUTTON		2

global buttonText as string[1] = ["Open URL", "Open URL_(Modal)"]
x as integer
for x = 0 to buttonText.length
	CreateButton(x + 1, 752 + (x - 5) * 100, 40, ReplaceString(buttonText[x], "_", NEWLINE, -1))
	SetButtonEnabled(x + 1, 1)
next

#constant URL_TO_OPEN	"https://store.steampowered.com/developer/fascimania"

infoTextID as integer
infoTextID = CreateTextEx(0, 100, "")

Steam.SetOverlayNotificationPosition(EPositionTopRight)
Steam.SetOverlayNotificationInset(50, 50)

// This should cause the Steam API to send API warnings to AGK's Debug Log,
// However, I have never seen a warning show up and don't know how to force
// one to fire, so I can only assume the plugin code is correct.
Steam.SetWarningMessageHook()

AddStatus("SteamID3: " + Steam.GetSteamID3(Steam.GetSteamID()))
AddStatus("SteamID64: " + Steam.GetSteamID64(Steam.GetSteamID()))
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
	if GetVirtualButtonPressed(OPEN_URL_BUTTON)
		AddStatus("Open URL")
		Steam.ActivateGameOverlayToWebPage(URL_TO_OPEN)
	elseif GetVirtualButtonPressed(OPEN_URL_MODAL_BUTTON)
		AddStatus("Open URL Modal")
		Steam.ActivateGameOverlayToWebPageModal(URL_TO_OPEN)
	endif
EndFunction

//
// Processes all asynchronous callbacks.
//
Function ProcessCallbacks()
	if Steam.HasIPCountryChangedResponse()
		AddStatus("IP Country Changed: " + Steam.GetIPCountry())
	endif
	if Steam.HasLowBatteryWarningResponse()
		AddStatus("Low battery warning!  " + str(Steam.GetMinutesBatteryLeft()) + " minute(s) remaining.")
	endif
	if Steam.IsSteamShuttingDown()
		AddStatus("Steam is shutting down.")
	endif
EndFunction
