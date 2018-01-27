#option_explicit

REMSTART

Note:
Nothing in this example has been tested.  I don't own a VR device and don't have the SteamVR Tool installed.
This example and the plugin commands it shows may or may not be useful for others, but they are here for completeness.
 
REMEND

#constant START_OPENVR_BUTTON		1

global buttonText as string[0] = ["START_OPEN VR"]
x as integer
for x = 0 to buttonText.length
	CreateButton(x + 1, 752 + (x - 5) * 100, 40, ReplaceString(buttonText[x], "_", NEWLINE, -1))
	SetButtonEnabled(x + 1, 1)
next

AddStatus("Steam running in VR mode: " + TF(Steam.IsSteamRunningInVR()))

// IsVRHeadsetStreamingEnabled
// SetVRHeadsetStreamingEnabled(bool)

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
	if GetVirtualButtonPressed(START_OPENVR_BUTTON)
		Steam.StartVRDashboard()
	endif
EndFunction

//
// Processes all asynchronous callbacks.
//
Function ProcessCallbacks()
	// No callbacks
EndFunction
