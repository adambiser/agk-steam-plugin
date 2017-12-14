// Project: Leaderboard
// Created: 2017-12-14
// Copyright 2017 Adam Biser
#option_explicit

#import_plugin SteamPlugin As Steam

// Uncommenting these lines forces the app to be run through the Steam client (a simple DRM).
// Note that the existence of the steam_appid.txt file will cause this to always return 0 (useful for development).
//~ if Steam.RestartAppIfNecessary(480) // 480 is your game's appid.
	//~ end
//~ endif

#constant KEY_ESCAPE	27
#constant NEWLINE		CHR(10)
#constant WINDOW_WIDTH	1024
#constant WINDOW_HEIGHT	768
#constant STATUS_X		0
#constant STATUS_Y		600
#constant STATUS_WIDTH	1024
#constant STATUS_HEIGHT	168

SetWindowTitle("Leaderboard")
SetWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT, 0)
SetVirtualResolution(WINDOW_WIDTH, WINDOW_HEIGHT)
SetSyncRate(30, 0)
UseNewDefaultFonts(1)
SetPrintSize(20)
SetErrorMode(2) // show all errors

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

// The main plugin usage is in this file.
#insert "steam.agc"

Steam.Shutdown()
Quit()

//---------------------------------------------------------------------
//
// Keep the UI stuff separate to highligh the plugin code.
//
// The type holds IDs for all text controls.
global text as TextControls
Type TextControls
	status as integer
EndType

global buttonColumns as integer [2] = [75, 225, 375]

Function CreateUI()
	CreateTextEx(STATUS_X, STATUS_Y - 25, "Status Messages (Scrollable)")
	spriteID as integer
	spriteID = CreateSprite(CreateImageColor(32, 32, 32, 255))
	SetSpritePosition(spriteID, STATUS_X, STATUS_Y)
	SetSpriteSize(spriteID, STATUS_WIDTH, STATUS_HEIGHT)
	// Set up scrolling status text
	text.status = CreateText("")
	SetTextPosition(text.status, STATUS_X, STATUS_Y)
	SetTextScissor(text.status, STATUS_X, STATUS_Y, STATUS_X + STATUS_WIDTH, STATUS_Y + STATUS_HEIGHT)
	SetTextSize(text.status, 18)
	SetTextVisible(text.status, 1)
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
	SetTextString(text.status, GetTextString(text.status) + GetDateFromUnix(GetUnixTime()) + ": " + status + NEWLINE)
	// Scroll the text upward.
	height as float
	height = GetTextTotalHeight(text.status)
	if height < STATUS_HEIGHT
		height = STATUS_HEIGHT
	endif
	SetTextPosition(text.status, STATUS_X, STATUS_Y - (height - STATUS_HEIGHT))
EndFunction

// Scrollable status window.
Function CheckMouseWheel()
	delta as float
	delta = GetRawMouseWheelDelta()
	if delta <> 0
		mouseX as float
		mouseY as float
		mouseX = GetPointerX()
		mouseY = GetPointerY()
		if mouseX < STATUS_X or mouseY < STATUS_Y or mouseX >= STATUS_X + STATUS_WIDTH or mouseY >= STATUS_Y + STATUS_HEIGHT
			ExitFunction
		endif
		newY as float
		newY = GetTextY(text.status) + GetTextSize(text.status) * delta
		if newY >= STATUS_Y //WINDOW_HEIGHT
			newY = STATUS_Y
		else
			height as float
			height = GetTextTotalHeight(text.status)
			if height < STATUS_HEIGHT
				height = STATUS_HEIGHT
			endif
			//~ if newY - GetTextSize(text.status) + height < STATUS_Y
			if newY + height < WINDOW_HEIGHT
				newY = WINDOW_HEIGHT - height
			endif
		endif
		SetTextY(text.status, newY)
	endif
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
