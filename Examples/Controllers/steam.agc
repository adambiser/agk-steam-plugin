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

#constant SHOW_ACTION_NAME_INDEX 4

CreateTextEx(0, 80, digitalActionNames[SHOW_ACTION_NAME_INDEX] + ": ")
global buttonSprite as integer
buttonSprite = CreateSprite(0) // LoadImage("raw:C:\Program Files (x86)\Steam\tenfoot\resource\images\library\controller\api\xbox_button_dpad_w.png"))
SetSpritePosition(buttonSprite, 80, 80)
SetSpriteSize(buttonSprite, 64, 64)



AddStatus("Init Steam Controller: " + TF(Steam.InitSteamController()))
AddStatus("Digital Action Handles:")
for x = 0 to digitalActionNames.length
	digitalActionHandles[x] = Steam.GetDigitalActionHandle(digitalActionNames[x])
	AddStatus(TAB + digitalActionNames[x] + " = " + str(digitalActionHandles[x]))
next

AddStatus("Analog Action Handles:")
for x = 0 to analogActionNames.length
	analogActionHandles[x] = Steam.GetAnalogActionHandle(analogActionNames[x])
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

	// Check action data
	Steam.RunFrame()
	if controllerCount
		Steam.ActivateActionSet(1, ActionSetHandles.shipControls)
		// Space War has an action set layer, but I don't know its name.  Pretending menu_controls is an action set layer for now.
		Steam.ActivateActionSetLayer(1, ActionSetHandles.menuControls) // Hit DETECT again to see this layer activated.
		for x = 0 to digitalActionHandles.length
			// Must call GetDigitalActionData before checking GetDigitalActionDataState and/or GetDigitalActionDataActive!
			// Note that this only indicates the current state.  You'd have to keep track of these values if you want to know when pressed or released.
			Steam.GetDigitalActionData(1, digitalActionHandles[x])
			if Steam.GetDigitalActionDataState()
				AddStatus("Digital action: " + digitalActionNames[x])
			endif
		next
		for x = 0 to analogActionHandles.length
			// Must call GetAnalogActionData before checking GetAnalogActionDataX or GetAnalogActionDataY!
			// Note that the Steam API does not have a dead zone value, so you'd have to implement one yourself.
			Steam.GetAnalogActionData(1, analogActionHandles[x])
			if Steam.GetAnalogActionDataX() <> 0 or Steam.GetAnalogActionDataY() <> 0
				AddStatus(analogActionNames[x] + ".x / .y = " + str(Steam.GetAnalogActionDataX()) + ", " + str(Steam.GetAnalogActionDataY()))
			endif
		next
		// Check for motion data.
		Steam.GetMotionData(1)
		AddStatusIfNotZero("PosAccelX", Steam.GetMotionDataPosAccelX())
		AddStatusIfNotZero("PosAccelY", Steam.GetMotionDataPosAccelY())
		AddStatusIfNotZero("PosAccelZ", Steam.GetMotionDataPosAccelZ())
		AddStatusIfNotZero("RotQuatW", Steam.GetMotionDataRotQuatW())
		AddStatusIfNotZero("RotQuatX", Steam.GetMotionDataRotQuatX())
		AddStatusIfNotZero("RotQuatY", Steam.GetMotionDataRotQuatY())
		AddStatusIfNotZero("RotQuatZ", Steam.GetMotionDataRotQuatZ())
		AddStatusIfNotZero("RotVelX", Steam.GetMotionDataRotVelX())
		AddStatusIfNotZero("RotVelY", Steam.GetMotionDataRotVelY())
		AddStatusIfNotZero("RotVelZ", Steam.GetMotionDataRotVelZ())
	endif
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
		if controllerCount > 0
			origins as integer[]
			origins.fromjson(Steam.GetDigitalActionOriginsJSON(1, ActionSetHandles.shipControls, digitalActionHandles[SHOW_ACTION_NAME_INDEX]))
			AddStatus(digitalActionNames[SHOW_ACTION_NAME_INDEX] + " origins: " + origins.tojson())
			for x = 0 to origins.length
				AddStatus("Action origin " + str(x) + ": " + Steam.GetStringForActionOrigin(origins[x]))
				glyphPath as String
				glyphPath = Steam.GetGlyphForActionOrigin(origins[x])
				AddStatus("Glyph path " + str(x) +": " + glyphPath)
				if x = 0
					SetSpriteImage(buttonSprite, LoadImage("raw:" + glyphPath))
				endif
			next
			origins.fromjson(Steam.GetAnalogActionOriginsJSON(1, ActionSetHandles.shipControls, analogActionHandles[0]))
			if origins.length >= 0
				AddStatus(analogActionNames[0] + " origins: " + origins.tojson())
				AddStatus("Action origin: " + Steam.GetStringForActionOrigin(origins[0]))
			endif
			layers as integer[]
			layers.fromjson(Steam.GetActiveActionSetLayersJSON(1))
			AddStatus("Active Action Set Layers: " + layers.tojson())
		endif
	endif
	if GetVirtualButtonPressed(BINDING_PANEL_BUTTON)
		AddStatus("Show binding panel for controller 1: " + TF(Steam.ShowBindingPanel(1)))
		// No real way of knowing when the binding panel closes!
		while not GetResumed() or GetPaused()
			Sync()
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

Function AddStatusIfNotZero(label as string, value as float)
	if value <> 0
		AddStatus(label + ": " + str(value))
	endif
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
