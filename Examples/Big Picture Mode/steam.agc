#option_explicit

#constant SINGLE_LINE_BUTTON		1
#constant MULTUPLE_LINE_BUTTON		2
#constant PASSWORD_BUTTON			3

global buttonText as string[2] = ["SINGLE_LINE", "MULTIPLE_LINE", "PASSWORD"]
x as integer
for x = 0 to buttonText.length
	CreateButton(x + 1, 752 + (x - 5) * 100, 40, ReplaceString(buttonText[x], "_", NEWLINE, -1))
next

global buttonModes as integer[2] = [EGamepadTextInputModeNormal, EGamepadTextInputModeNormal, EGamepadTextInputModePassword]
global buttonLineModes as integer[2] = [EGamepadTextInputLineModeSingleLine, EGamepadTextInputLineModeMultipleLines, EGamepadTextInputLineModeSingleLine]
global buttonInputDesc as string[2] = ["Normal/Single Line/10 Char Max", "Normal/Multiple Line/100 Char Max", "Password/Single Line/10 Char Max"]
global buttonInputLengths as integer[2] = [10, 100, 10]
global textinput as string[2] = ["", "", ""]
global lastButtonPressed as integer

AddStatus("Waiting on Steam overlay to become enabled.")
overlayEnabled as integer = 0

// According to The Steamworks SDK, in order for Big Picture Mode to work,
// the game must be started from the Steam client.  For testing, they say
// to add the EXE as a non-Steam Game in the Steam client.
// However, this doesn't seem to be the case as of September 2023.
// IsSteamInBigPictureMode reports BPM properly, but the  gamepad text 
// input doesn't work in BPM.
// Instead, just start BPM in the Steam client and run the example from the 
// IDE or directly from its EXE.

AddStatus("Big Picture Mode = " + TF(Steam.IsSteamInBigPictureMode()))

//
// The main loop
//
do
	if not overlayEnabled
		if Steam.IsOverlayEnabled()
			AddStatus("Steam overlay is now enabled.")
			overlayEnabled = 1
			SetButtonEnabled(SINGLE_LINE_BUTTON, 1)
			SetButtonEnabled(MULTUPLE_LINE_BUTTON, 1)
			SetButtonEnabled(PASSWORD_BUTTON, 1)
		endif
	endif
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
	x as integer
	
	// Scrollable text.
	PerformVerticalScroll(statusArea)
	for x = SINGLE_LINE_BUTTON to PASSWORD_BUTTON
		if GetVirtualButtonPressed(x)
			if Steam.ShowGamepadTextInput(buttonModes[x - 1], buttonLineModes[x - 1], buttonInputDesc[x - 1], buttonInputLengths[x - 1], textinput[x - 1])
				AddStatus("ShowGamepadTextInput succeeded.")
				lastButtonPressed = x
			else
				AddStatus("ShowGamepadTextInput failed.")
			endif
		endif
	next
EndFunction

//
// Processes all asynchronous callbacks.
//
Function ProcessCallbacks()
	if Steam.HasGamepadTextInputDismissedResponse()
		AddStatus("Has response")
		if Steam.GetGamepadTextInputDismissedSubmitted()
			textinput[lastButtonPressed - 1] = Steam.GetEnteredGamepadTextInput()
			AddStatus("User entered: " + Steam.GetEnteredGamepadTextInput())
		else
			AddStatus("User cancelled text input.")
		endif
	endif
EndFunction
