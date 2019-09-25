#option_explicit

#constant OPEN_URL_BUTTON			1
#constant OPEN_URL_MODAL_BUTTON		2
#constant DURATION_CONTROL_BUTTON	3

global buttonText as string[2] = ["Open URL", "Open URL_(Modal)", "Duration_Control"]
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

AddStatus("AppName: " + Steam.GetAppName(480))
AddStatus("SteamID3: " + Steam.GetSteamID3(Steam.GetSteamID()))
AddStatus("SteamID64: " + Steam.GetSteamID64(Steam.GetSteamID()))
AddStatus("IP Country: " + Steam.GetIPCountry())
AddStatus("Steam UI Language: " + Steam.GetSteamUILanguage())
AddStatus("AppInstallDir: " + Steam.GetAppInstallDir(480))

AddStatus("IsBehindNAT: " + TF(Steam.IsBehindNAT()))
AddStatus("IsPhoneIdentifying: " + TF(Steam.IsPhoneIdentifying()))
AddStatus("IsPhoneRequiringVerification: " + TF(Steam.IsPhoneRequiringVerification()))
AddStatus("IsPhoneVerified: " + TF(Steam.IsPhoneVerified()))
AddStatus("IsTwoFactorEnabled: " + TF(Steam.IsTwoFactorEnabled()))

AddStatus("CommandLineArgs: " + Steam.GetCommandLineArgsJSON())

AddStatus("InitFilterText: " + TF(Steam.InitFilterText()))

global filenames as string[1] = ["dummy.txt", "SteamworksExample.exe"]
global fileCallResults as integer[1]

for x = 0 to filenames.length
	fileCallResults[x] = Steam.GetFileDetails(filenames[x])
	AddStatus("GetFileDetails: " + filenames[x] + ", call result = " + str(fileCallResults[x]))
next

global numberOfPlayersCallResult as integer
numberOfPlayersCallResult = Steam.GetNumberOfCurrentPlayers()

global durationControlCallResult as integer
global marketEligibilityCallResult as integer
marketEligibilityCallResult = Steam.GetMarketEligibility()

info as string
filteredText as string
filteredText = Steam.FilterText("Fuck this", 0)

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
	info = info + "Filtered Text: " + filteredText + NEWLINE
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
	elseif GetVirtualButtonPressed(DURATION_CONTROL_BUTTON)
		if not durationControlCallResult
			AddStatus("GetDurationControl")
			durationControlCallResult = Steam.GetDurationControl()
		endif
	endif
EndFunction

//
// Processes all asynchronous callbacks.
//
Function ProcessCallbacks()
	if durationControlCallResult
		result = Steam.GetCallResultCode(durationControlCallResult)
		if result
			AddStatus("GetDurationControl call result, result code = " + str(result))
			AddStatus("* AppID = " + str(Steam.GetDurationControlAppID(durationControlCallResult)))
			AddStatus("* Applicable = " + TF(Steam.GetDurationControlApplicable(durationControlCallResult)))
			AddStatus("* SecondsInLastFiveHours = " + str(Steam.GetDurationControlSecondsInLastFiveHours(durationControlCallResult)))
			AddStatus("* Progress = " + str(Steam.GetDurationControlProgress(durationControlCallResult)))
			AddStatus("* Notification = " + str(Steam.GetDurationControlNotification(durationControlCallResult)))
			// Done with the call result.  Delete and clear.
			Steam.DeleteCallResult(durationControlCallResult)
			durationControlCallResult = 0
		endif
	endif
	while Steam.HasDurationControlResponse()
		AddStatus("Duration Control Callback:")
		AddStatus("* Result = " + str(Steam.GetDurationControlResult()))
		AddStatus("* AppID = " + str(Steam.GetDurationControlAppID()))
		AddStatus("* Applicable = " + TF(Steam.GetDurationControlApplicable()))
		AddStatus("* SecondsInLastFiveHours = " + str(Steam.GetDurationControlSecondsInLastFiveHours()))
		AddStatus("* Progress = " + str(Steam.GetDurationControlProgress()))
		AddStatus("* Notification = " + str(Steam.GetDurationControlNotification()))
	endwhile
	if marketEligibilityCallResult
		result = Steam.GetCallResultCode(marketEligibilityCallResult)
		if result
			AddStatus("GetMarketEligibility, result code =" + str(result))
			AddStatus("* Is Allowed = " + TF(Steam.GetMarketEligibilityIsAllowed(marketEligibilityCallResult)))
			AddStatus("* Not Allowed Reason = " + str(Steam.GetMarketEligibilityNotAllowedReason(marketEligibilityCallResult)))
			AddStatus("* Allowed At Time = " + GetDateFromUnix(Steam.GetMarketEligibilityAllowedAtTime(marketEligibilityCallResult)))
			AddStatus("* Steam Guard Required Days = " + str(Steam.GetMarketEligibilitySteamGuardRequiredDays(marketEligibilityCallResult)))
			AddStatus("* New Device Cooldown = " + str(Steam.GetMarketEligibilityNewDeviceCooldown(marketEligibilityCallResult)))
			Steam.DeleteCallResult(marketEligibilityCallResult)
			marketEligibilityCallResult = 0
		endif
	endif
	if Steam.HasIPCountryChangedResponse()
		AddStatus("IP Country Changed: " + Steam.GetIPCountry())
	endif
	if Steam.HasLowBatteryWarningResponse()
		AddStatus("Low battery warning!  " + str(Steam.GetMinutesBatteryLeft()) + " minute(s) remaining.")
	endif
	if Steam.IsSteamShuttingDown()
		AddStatus("Steam is shutting down.")
	endif
	result as integer
	x as integer
	for x = 0 to fileCallResults.length
		if fileCallResults[x]
			result = Steam.GetCallResultCode(fileCallResults[x])
			if result
				AddStatus("GetFileDetails: " + filenames[x] + ", result code = " + str(result))
				//~ if result = EResultOK
					AddStatus(filenames[x] + ": size = " + str(Steam.GetFileDetailsSize(fileCallResults[x])) + ", SHA = " + Steam.GetFileDetailsSHA1(fileCallResults[x]))
				//~ endif
				// Done with the call result.  Delete and clear.
				Steam.DeleteCallResult(fileCallResults[x])
				fileCallResults[x] = 0
			endif
		endif
	next
	if numberOfPlayersCallResult
		result = Steam.GetCallResultCode(numberOfPlayersCallResult)
		if result
			AddStatus("GetNumberOfCurrentPlayers: result code = " + str(result))
			if result = EResultOK
				AddStatus("Number of players: " + str(Steam.GetNumberOfCurrentPlayersResult(numberOfPlayersCallResult)))
			endif
			// Done with the call result.  Delete and clear.
			Steam.DeleteCallResult(numberOfPlayersCallResult)
			numberOfPlayersCallResult = 0
		endif
	endif
EndFunction
