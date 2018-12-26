#option_explicit

global buttonText as string[-1] // = []
x as integer
for x = 0 to buttonText.length
	CreateButton(x + 1, 752 + (x - 5) * 100, 40, ReplaceString(buttonText[x], "_", NEWLINE, -1))
	SetButtonEnabled(x + 1, 1)
next

AddStatus(Steam.GetSteamPath())
AddStatus("Steam emulation detected: " + TF(Steam.IsSteamEmulated()))
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
EndFunction

//
// Processes all asynchronous callbacks.
//
Function ProcessCallbacks()
	x as integer
EndFunction
