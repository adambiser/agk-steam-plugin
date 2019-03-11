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

if Steam.IsSteamInBigPictureMode()
	SetButtonEnabled(SINGLE_LINE_BUTTON, 1)
	SetButtonEnabled(MULTUPLE_LINE_BUTTON, 1)
	SetButtonEnabled(PASSWORD_BUTTON, 1)
else
	// To see how this works, the Spacewar install will need to be replaced with this test project.
	// 1) Make a backup of the original contents of steamapps\common\Spacewar.
	// 2) Copy this project folder's contents into steamapps\common\Spacewar
	// 3) Rename "Big Picture Mode.exe" to "SteamworksExample.exe" so the Steam client runs it.
	// The Steam client will now run this test project when you start Spacewar.
	// Connect a controller, start Big Picture Mode, and give it a try.
	Message("Steam needs to be in Big Picture Mode to test this.")
endif

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
	x as integer
	
	// Scrollable text.
	PerformVerticalScroll(statusArea)
	for x = SINGLE_LINE_BUTTON to PASSWORD_BUTTON
		if GetVirtualButtonPressed(x)
			if Steam.ShowGamepadTextInput(buttonModes[x - 1], buttonLineModes[x - 1], buttonInputDesc[x - 1], buttonInputLengths[x - 1], textinput[x - 1])
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
	if Steam.HasGamepadTextInputDismissedInfo()
		info as GamepadTextInputDismissedInfo_t
		info.fromJSON(Steam.GetGamepadTextInputDismissedInfoJSON())
		if info.Submitted
			textinput[lastButtonPressed - 1] = info.Text
			AddStatus("User entered: " + info.Text)
		else
			AddStatus("User cancelled text input.")
		endif
	endif
EndFunction
