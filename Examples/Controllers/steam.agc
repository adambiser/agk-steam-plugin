#option_explicit

#constant DETECT_BUTTON			1
#constant BINDING_PANEL_BUTTON	2
#constant VIBRATE_BUTTON		3
#constant HAPTIC_BUTTON			4

global buttonText as string[3] = ["DETECT", "BINDING_PANEL", "VIBRATE", "HAPTIC_(VSC, DS4)"]

x as integer
for x = 0 to buttonText.length
	CreateButton(x + 1, 752 + (x - 5) * 100, 40, ReplaceString(buttonText[x], "_", NEWLINE, -1))
	SetButtonEnabled(x + 1, 1)
next
SetButtonEnabled(BINDING_PANEL_BUTTON, 0)
SetButtonEnabled(VIBRATE_BUTTON, 0)
SetButtonEnabled(HAPTIC_BUTTON, 0)

global ActionHandles as ActionHandlesType
Type ActionHandlesType
	// ship_controls
	turnLeft as integer
	turnRight as integer
	forwardThrust as integer
	backwardThrust as integer
	fireLasers as integer
	pauseMenu as integer
	// menu_controls
	menuUp as integer
	menuDown as integer
	menuLeft as integer
	menuRight as integer
	menuSelect as integer
	menuCancel as integer
	// analog
	analogControls as integer
EndType

global ActionSetHandles as ActionSetHandlesType
Type ActionSetHandlesType
	shipControls as integer
	menuControls as integer
EndType

//~ global joysticks as integer[]
//~ CompleteRawJoystickDetection()
//~ AddStatus("Checking raw joysticks...")
//~ for x = 1 to 8
	//~ AddStatus("AGK Controller " + str(x) + ": Exists: " + TF(GetRawJoystickExists(x)) + ", Connected: " + TF(GetRawJoystickConnected(x)) + ", Name: " + GetRawJoystickName(x))
	//~ if GetRawJoystickExists(x) and GetRawJoystickConnected(x)
		//~ joysticks.insert(x)
	//~ endif
//~ next x

AddStatus("Init Steam Controller: " + TF(Steam.InitSteamController()))

// ship_controls
ActionHandles.turnLeft = Steam.GetControllerDigitalActionHandle("turn_left")
ActionHandles.turnRight = Steam.GetControllerDigitalActionHandle("turn_right")
ActionHandles.forwardThrust = Steam.GetControllerDigitalActionHandle("forward_thrust")
ActionHandles.backwardThrust = Steam.GetControllerDigitalActionHandle("backward_thrust")
ActionHandles.fireLasers = Steam.GetControllerDigitalActionHandle("fire_lasers")
ActionHandles.pauseMenu = Steam.GetControllerDigitalActionHandle("pause_menu")
// menu_controls
ActionHandles.menuUp = Steam.GetControllerDigitalActionHandle("menu_up")
ActionHandles.menuDown = Steam.GetControllerDigitalActionHandle("menu_down")
ActionHandles.menuLeft = Steam.GetControllerDigitalActionHandle("menu_left")
ActionHandles.menuRight = Steam.GetControllerDigitalActionHandle("menu_right")
ActionHandles.menuSelect = Steam.GetControllerDigitalActionHandle("menu_select")
ActionHandles.menuCancel = Steam.GetControllerDigitalActionHandle("menu_cancel")
// analog
ActionHandles.analogControls = Steam.GetControllerAnalogActionHandle("analog_controls")

AddStatus("Action handles: " + ActionHandles.tojson())

ActionSetHandles.shipControls = Steam.GetControllerActionSetHandle("ship_controls")
ActionSetHandles.menuControls = Steam.GetControllerActionSetHandle("menu_controls")

AddStatus("Action set handles: " + ActionSetHandles.tojson())

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
	//~ Steam.RunControllerFrame()
	// Scrollable text.
	PerformVerticalScroll(statusArea)
	if GetVirtualButtonPressed(DETECT_BUTTON)
		controllerCount as integer
		controllerCount = Steam.GetConnectedControllers()
		AddStatus("Connected controllers: " + str(controllerCount))
		SetButtonEnabled(BINDING_PANEL_BUTTON, controllerCount > 0)
		SetButtonEnabled(VIBRATE_BUTTON, controllerCount > 0)
		SetButtonEnabled(HAPTIC_BUTTON, controllerCount > 0)
		for x = 1 to controllerCount
			AddStatus("Controller type " + str(x) + ": " + GetControllerInputTypeName(Steam.GetControllerInputType(x)))
		next
	endif
	if GetVirtualButtonPressed(BINDING_PANEL_BUTTON)
		AddStatus("Show binding panel for controller 1: " + TF(Steam.ShowBindingPanel(1)))
		while Steam.IsGameOverlayActive()
			AddStatus("..")
		endwhile
	endif
	if GetVirtualButtonPressed(VIBRATE_BUTTON)
		AddStatus("Vibrating controller 1...")
		Steam.TriggerControllerVibration(1, 2000, 1000)
	endif
	if GetVirtualButtonPressed(HAPTIC_BUTTON)
		AddStatus("Sending haptic pulse to controller 1 left pad...")
		Steam.TriggerControllerHapticPulse(1, k_ESteamControllerPad_Left, 1000)
	endif
EndFunction

//
// Processes all asynchronous callbacks.
//
Function ProcessCallbacks()
	index as integer
EndFunction

Function GetControllerInputTypeName(value as integer)
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
