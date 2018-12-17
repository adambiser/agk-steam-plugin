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

#constant TAB chr(9)

global controllerCount as integer

global digitalActionNames as string[11] = ["turn_left", "turn_right", "forward_thrust", "backward_thrust", "fire_lasers", "pause_menu", "menu_up", "menu_down", "menu_left", "menu_right", "menu_select", "menu_cancel"]
global digitalActionHandles as integer[11]

global analogActionNames as string[0] = ["analog_controls"]
global analogActionHandles as integer[0]

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
AddStatus("Digital Action Handles:")
for x = 0 to digitalActionNames.length
	digitalActionHandles[x] = Steam.GetDigitalActionHandle(digitalActionNames[x])
	AddStatus(TAB + digitalActionNames[x] + " = " + str(digitalActionHandles[x]))
next

AddStatus("Analog Action Handles:")
for x = 0 to analogActionNames.length
	analogActionHandles[x] = Steam.GetDigitalActionHandle(analogActionNames[x])
	AddStatus(TAB + analogActionNames[x] + " = " + str(analogActionHandles[x]))
next

ActionSetHandles.shipControls = Steam.GetActionSetHandle("ship_controls")
ActionSetHandles.menuControls = Steam.GetActionSetHandle("menu_controls")

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
	//~ j as integer
	//~ b as integer

	Steam.RunFrame()
	if controllerCount
		Steam.ActivateActionSet(1, ActionSetHandles.shipControls)
		for x = 0 to digitalActionHandles.length
			if Steam.GetDigitalActionDataState(1, digitalActionHandles[x])
				AddStatus("Digital action: " + digitalActionNames[x])
			endif
		next
	endif
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
		controllerCount = Steam.GetConnectedControllers()
		AddStatus("Connected controllers: " + str(controllerCount))
		SetButtonEnabled(BINDING_PANEL_BUTTON, controllerCount > 0)
		SetButtonEnabled(VIBRATE_BUTTON, controllerCount > 0)
		SetButtonEnabled(HAPTIC_BUTTON, controllerCount > 0)
		for x = 1 to controllerCount
			AddStatus("Controller type " + str(x) + ": " + GetInputTypeName(Steam.GetInputTypeForHandle(x)))
			AddStatus("Controller action set " + str(x) + ": " + str(Steam.GetCurrentActionSet(x)))
			
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

Function GetInputTypeName(value as integer)
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
