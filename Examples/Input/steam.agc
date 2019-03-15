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

#constant HINPUT	1	// Assuming hInput 1 for this example.

CreateTextEx(0, 80, digitalActionNames[SHOW_ACTION_NAME_INDEX] + ": ")
global buttonSprite as integer
buttonSprite = CreateSprite(0) // LoadImage("raw:C:\Program Files (x86)\Steam\tenfoot\resource\images\library\controller\api\xbox_button_dpad_w.png"))
SetSpritePosition(buttonSprite, 80, 80)
SetSpriteSize(buttonSprite, 64, 64)



AddStatus("Init Steam Input: " + TF(Steam.InitSteamInput()))
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
		Steam.ActivateActionSet(HINPUT, ActionSetHandles.shipControls)
		// Space War has an action set layer, but I don't know its name.  Pretending menu_controls is an action set layer for now.
		Steam.ActivateActionSetLayer(HINPUT, ActionSetHandles.menuControls) // Hit DETECT again to see this layer activated.
		for x = 0 to digitalActionHandles.length
			// Must call GetDigitalActionData before checking GetDigitalActionDataState and/or GetDigitalActionDataActive!
			// Note that this only indicates the current state.  You'd have to keep track of these values if you want to know when pressed or released.
			if Steam.GetDigitalActionDataState(HINPUT, digitalActionHandles[x])
				AddStatus("Digital action: " + digitalActionNames[x])
			endif
		next
		for x = 0 to analogActionHandles.length
			// Must call GetAnalogActionData before checking GetAnalogActionDataX or GetAnalogActionDataY!
			// Note that the Steam API does not have a dead zone value, so you'd have to implement one yourself.
			if Steam.GetAnalogActionDataX(HINPUT, analogActionHandles[x]) <> 0 or Steam.GetAnalogActionDataY(HINPUT, analogActionHandles[x]) <> 0
				AddStatus(analogActionNames[x] + ".x / .y = " + str(Steam.GetAnalogActionDataX(HINPUT, analogActionHandles[x])) + ", " + str(Steam.GetAnalogActionDataY(HINPUT, analogActionHandles[x])))
			endif
		next
		// Check for motion data.
		AddStatusIfNotZero("PosAccelX", Steam.GetMotionDataPosAccelX(HINPUT))
		AddStatusIfNotZero("PosAccelY", Steam.GetMotionDataPosAccelY(HINPUT))
		AddStatusIfNotZero("PosAccelZ", Steam.GetMotionDataPosAccelZ(HINPUT))
		AddStatusIfNotZero("RotQuatW", Steam.GetMotionDataRotQuatW(HINPUT))
		AddStatusIfNotZero("RotQuatX", Steam.GetMotionDataRotQuatX(HINPUT))
		AddStatusIfNotZero("RotQuatY", Steam.GetMotionDataRotQuatY(HINPUT))
		AddStatusIfNotZero("RotQuatZ", Steam.GetMotionDataRotQuatZ(HINPUT))
		AddStatusIfNotZero("RotVelX", Steam.GetMotionDataRotVelX(HINPUT))
		AddStatusIfNotZero("RotVelY", Steam.GetMotionDataRotVelY(HINPUT))
		AddStatusIfNotZero("RotVelZ", Steam.GetMotionDataRotVelZ(HINPUT))
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
			AddStatus("Input type " + str(x) + ": " + GetInputTypeName(Steam.GetInputTypeForHandle(x)))
			AddStatus("Input action set " + str(x) + ": " + str(Steam.GetCurrentActionSet(x)))
		next
		if controllerCount > 0
			origins as integer[]
			for x = 0 to Steam.GetDigitalActionOriginCount(HINPUT, ActionSetHandles.shipControls, digitalActionHandles[0]) - 1
				origins.insert(Steam.GetDigitalActionOriginValue(HINPUT, ActionSetHandles.shipControls, digitalActionHandles[0], x))
			next x
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
			origins.length = -1
			for x = 0 to Steam.GetAnalogActionOriginCount(HINPUT, ActionSetHandles.shipControls, analogActionHandles[0]) - 1
				origins.insert(Steam.GetAnalogActionOriginValue(HINPUT, ActionSetHandles.shipControls, analogActionHandles[0], x))
			next x
			if origins.length >= 0
				AddStatus(analogActionNames[0] + " origins: " + origins.tojson())
				AddStatus("Action origin: " + Steam.GetStringForActionOrigin(origins[0]))
			endif
			layers as integer[]
			for x = 0 to Steam.GetActiveActionSetLayerCount(HINPUT) - 1
				layers.insert(Steam.GetActiveActionSetLayerHandle(HINPUT, x))
			next
			AddStatus("Active Action Set Layers: " + layers.tojson())
		endif
	endif
	if GetVirtualButtonPressed(BINDING_PANEL_BUTTON)
		AddStatus("Show binding panel for input 1: " + TF(Steam.ShowBindingPanel(HINPUT)))
		// No real way of knowing when the binding panel closes!
		while not GetResumed() or GetPaused()
			Sync()
		endwhile
	endif
	if GetVirtualButtonPressed(VIBRATE_BUTTON)
		AddStatus("Vibrating input 1...")
		Steam.TriggerInputVibration(HINPUT, 2000, 1000)
	endif
	if GetVirtualButtonPressed(HAPTIC_BUTTON)
		AddStatus("Sending haptic pulse to input 1 left pad...")
		Steam.TriggerInputHapticPulse(HINPUT, ESteamControllerPad_Left, 1000)
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
		case ESteamInputType_Unknown
			ExitFunction "Unknown"
		endcase
		case ESteamInputType_SteamController
			ExitFunction "Steam Controller"
		endcase
		case ESteamInputType_XBox360Controller
			ExitFunction "XBox 360 Controller"
		endcase
		case ESteamInputType_XBoxOneController
			ExitFunction "XBox One Controller"
		endcase
		case ESteamInputType_GenericGamepad
			ExitFunction "DirectX Controller"
		endcase
		case ESteamInputType_PS4Controller
			ExitFunction "PS4 Controller"
		endcase
	endselect
EndFunction "???"
