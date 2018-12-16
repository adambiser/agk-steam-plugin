#option_explicit

#constant DETECT_BUTTON			1
#constant VIBRATE_BUTTON		2
#constant HAPTIC_BUTTON			3

global buttonText as string[2] = ["DETECT", "VIBRATE", "HAPTIC_(VSC, DS4)"]

x as integer
for x = 0 to buttonText.length
	CreateButton(x + 1, 752 + (x - 5) * 100, 40, ReplaceString(buttonText[x], "_", NEWLINE, -1))
	SetButtonEnabled(x + 1, 1)
next
SetButtonEnabled(VIBRATE_BUTTON, 0)
SetButtonEnabled(HAPTIC_BUTTON, 0)

global joysticks as integer[]

//~ CompleteRawJoystickDetection()
//~ AddStatus("Checking raw joysticks...")
//~ for x = 1 to 8
	//~ AddStatus("AGK Controller " + str(x) + ": Exists: " + TF(GetRawJoystickExists(x)) + ", Connected: " + TF(GetRawJoystickConnected(x)) + ", Name: " + GetRawJoystickName(x))
	//~ if GetRawJoystickExists(x) and GetRawJoystickConnected(x)
		//~ joysticks.insert(x)
	//~ endif
//~ next x

AddStatus("Init Steam Controller: " + TF(Steam.InitSteamController()))

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
	j as integer
	b as integer

	//~ for x = 0 to joysticks.length
		//~ j = joysticks[x]
		//~ for b = 1 to 64
			//~ if GetRawJoystickButtonPressed(j, b)
				//~ AddStatus("Controller " + str(j) + " Button " + str(b) + " = pressed")
			//~ elseif GetRawJoystickButtonReleased(j, b)
				//~ AddStatus("Controller " + str(j) + " Button " + str(b) + " = released")
			//~ endif
		//~ next
	//~ next
	// Scrollable text.
	PerformVerticalScroll(statusArea)
	if GetVirtualButtonPressed(DETECT_BUTTON)
		controllerCount as integer
		controllerCount = Steam.GetConnectedControllers()
		AddStatus("Connected controllers: " + str(controllerCount))
		SetButtonEnabled(VIBRATE_BUTTON, controllerCount > 0)
		SetButtonEnabled(HAPTIC_BUTTON, controllerCount > 0)
		for x = 0 to controllerCount - 1
			AddStatus("Controller type " + str(x) + ": " + GetControllerTypeName(Steam.GetControllerType(x)))
		next
	endif
	if GetVirtualButtonPressed(VIBRATE_BUTTON)
		AddStatus("Vibrating controller 0...")
		Steam.TriggerControllerVibration(0, 2000, 1000)
	endif
	if GetVirtualButtonPressed(HAPTIC_BUTTON)
		AddStatus("Sending haptic pulse to controller 0 left pad...")
		Steam.TriggerControllerHapticPulse(0, k_ESteamControllerPad_Left, 1000)
	endif
EndFunction

//
// Processes all asynchronous callbacks.
//
Function ProcessCallbacks()
	index as integer
EndFunction

Function GetControllerTypeName(value as integer)
	select value
		case k_ESteamInputType_Unknown
			ExitFunction "Unknown"
		endcase
		case k_ESteamInputType_SteamController
			ExitFunction "Steam Controller"
		endcase
		case k_ESteamInputType_XBox360Controller
			ExitFunction "XBox 360 Controller"
		endcase
		case k_ESteamInputType_XBoxOneController
			ExitFunction "XBox One Controller"
		endcase
		case k_ESteamInputType_GenericXInput
			ExitFunction "XInput Controller"
		endcase
		case k_ESteamInputType_PS4Controller
			ExitFunction "PS4 Controller"
		endcase
	endselect
EndFunction "???"
