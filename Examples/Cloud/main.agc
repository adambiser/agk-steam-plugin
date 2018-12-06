// Project: Cloud 
// Created: 2018-12-04
// Copyright 2018 Adam Biser
#option_explicit

#import_plugin SteamPlugin As Steam

// Uncommenting these lines forces the app to be run through the Steam client (a simple DRM).
// Note that the existence of the steam_appid.txt file will cause this to always return 0 (useful for development).
//~ if Steam.RestartAppIfNecessary(480) // 480 is your game's appid.
	//~ end
//~ endif

#constant KEY_ENTER		13
#constant KEY_ESCAPE	27
#constant NEWLINE		CHR(10)
#constant WINDOW_WIDTH	1024
#constant WINDOW_HEIGHT	768
#constant STATUS_X		0
#constant STATUS_Y		200
#constant STATUS_WIDTH	1024
#constant STATUS_HEIGHT	568

SetWindowTitle("Cloud")
SetWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT, 0)
SetVirtualResolution(WINDOW_WIDTH, WINDOW_HEIGHT)
SetSyncRate(30, 0)
UseNewDefaultFonts(1)
SetPrintSize(20)
SetErrorMode(2) // show all errors

#insert "../scrollabletextarea.agc"

global statusArea as ScrollableTextArea

CreateUI()

// Note: RequestStats is called within Init().
if not Steam.Init()
	Message("Could not initialize Steam API.  Closing.")
	Quit()
endif

// Show general information
CreateTextEx(0, 00, "AppID: " + str(Steam.GetAppID()))
CreateTextEx(0, 20, "LoggedOn: " + TF(Steam.LoggedOn()))
CreateTextEx(0, 40, "SteamID: " + Steam.GetPersonaName())

#insert "../steam_constants.agc"
// The main plugin usage are in these files.
#insert "steam.agc"

Steam.Shutdown()
Quit()

//---------------------------------------------------------------------
//
// Keep the UI stuff separate to highligh the plugin code.
//
global buttonColumns as integer [2] = [75, 225, 375]

Function CreateUI()
	CreateTextEx(STATUS_X, STATUS_Y - 25, "Status Messages (Scrollable)")
	statusArea = CreateScrollableTextArea(STATUS_X, STATUS_Y, STATUS_WIDTH, STATUS_HEIGHT)
EndFunction

Function CreateTextEx(x as float, y as float, text as string)
	id as integer
	id = CreateText(text)
	SetTextPosition(id, x, y)
	SetTextSize(id, 20)
EndFunction id

Function CreateButton(id as integer, x as integer, y as integer, txt as string)
	AddVirtualButton(id, x, y, 80)
	//~ SetVirtualButtonSize(id, 150, 80)
	SetVirtualButtonText(id, txt)
	SetButtonEnabled(id, 0)
EndFunction

Function SetButtonEnabled(id as integer, enabled as integer)
	SetVirtualButtonActive(id, enabled)
	if enabled
		SetVirtualButtonColor(id, 192, 192, 192)
	else
		SetVirtualButtonColor(id, 64, 64, 64)
	endif
EndFunction	

Function AddStatus(status as string)
	AddLineToScrollableTextArea(statusArea, GetDateFromUnix(GetUnixTime()) + ": " + status)
EndFunction

Function GetDateFromUnix(unix as integer)
	text as string
	text = PadStr(GetYearFromUnix(unix), 4)
	text = text + "-" + PadStr(GetMonthFromUnix(unix), 2)
	text = text + "-" + PadStr(GetDaysFromUnix(unix), 2)
	text = text + " " + PadStr(GetHoursFromUnix(unix), 2)
	text = text + ":" + PadStr(GetMinutesFromUnix(unix), 2)
	text = text + ":" + PadStr(GetSecondsFromUnix(unix), 2)
EndFunction text

Function PadStr(number as integer, length as integer)
	text as string
	text = str(number)
	if len(text) < length
		text = ReplaceString(Spaces(length - len(text)), " ", "0", -1) + text
	endif
EndFunction text

Function TF(value as integer)
	if value
		ExitFunction "TRUE"
	endif
EndFunction "FALSE"

Function Quit()
	// Cleanup
	x as integer
	for x = 1 to 100
		DeleteVirtualButton(x)
	next
	DeleteAllText()
	DeleteAllImages()
	DeleteAllSprites()
	end
EndFunction
