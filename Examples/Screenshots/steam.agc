#option_explicit

#constant TAKE_SCREENSHOT_BUTTON	1
#constant TOGGLE_HOOK_BUTTON		2
#constant ADD_SCREENSHOT_BUTTON		3

global buttonText as string[2] = ["TAKE_SCREENSHOT", "TOGGLE_HOOK", "ADD_SCREENSHOT"]
x as integer
for x = 0 to buttonText.length
	CreateButton(x + 1, 752 + (x - 5) * 100, 40, ReplaceString(buttonText[x], "_", NEWLINE, -1))
	SetButtonEnabled(x + 1, 1)
next

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
	hook as integer
	imageID as integer
	handle as integer
	
	// Scrollable text.
	PerformVerticalScroll(statusArea)
	if GetVirtualButtonPressed(TAKE_SCREENSHOT_BUTTON)
		Steam.TriggerScreenshot()
	elseif GetVirtualButtonPressed(TOGGLE_HOOK_BUTTON)
		hook = Steam.IsScreenshotsHooked() ~~ 1
		Steam.HookScreenshots(hook)
		AddStatus("Toggle Screenshot Hooking: " + TF(Steam.IsScreenshotsHooked()))
	elseif GetVirtualButtonPressed(ADD_SCREENSHOT_BUTTON)
		imageID = CreateGeneratedImage()
		SaveImage(imageID, "temp.png")
		handle = Steam.AddScreenshotToLibrary(GetWritePath() + GetFolder() + "temp.png", GetImageWidth(imageID), GetImageHeight(imageID))
		DeleteImage(imageID)
		AddStatus("Added screenshot, handle = " + str(handle))
	endif
EndFunction

//
// Processes all asynchronous callbacks.
//
Function ProcessCallbacks()
	handle as integer
	// When the screenshot hook is disabled, Steam Overlay does the screenshot.
	if Steam.HasScreenshotReadyResponse()
		handle = Steam.GetScreenshotReadyScreenshotHandle()
		AddStatus("Screenshot Ready!  Result = " + str(Steam.GetScreenshotReadyResult()) + ", handle = " + str(handle))
		AddScreenshotTags(handle)
	endif
	// When the screenshot hook is enabled, the game provides the screenshot.
	if Steam.HasScreenshotRequestedResponse()
		AddStatus("Screenshot Requested.")
		// Generate a screenshot.
		imageID as integer
		//~ imageID = CreateScreenshotImage()
		// It can be any image.
		imageID = CreateGeneratedImage()
		handle = Steam.WriteScreenshot(imageID)
		DeleteImage(imageID)
		AddStatus("Wrote screenshot, handle = " + str(handle))
		AddScreenshotTags(handle)
	endif
EndFunction

Function CreateScreenshotImage()
	imageID as integer
	imageID = CreateRenderImage(GetDeviceWidth(), GetDeviceHeight(), 0, 0)
	SetRenderToImage(imageID, 0)
	Render()
	SetRenderToScreen()
EndFunction imageID

Function CreateGeneratedImage()
	imageID as integer
	oldVirtualWidth as integer
	oldVirtualHeight as integer
	oldVirtualWidth = GetVirtualWidth()
	oldVirtualHeight = GetVirtualHeight()
	imageID = CreateRenderImage(256, 256, 0, 0)
	SetRenderToImage(imageID, 0)
	SetVirtualResolution(256, 256)
	// Erase the background
	DrawBox(0, 0, 256, 256, 0, 0, 0, 0, 1)
	DrawBox(64, 64, 192, 192, 0x0000ff, 0x00ff00, 0xff0000, 0xffffff, 1)
	SetRenderToScreen()
	SetVirtualResolution(oldVirtualWidth, oldVirtualHeight)
EndFunction imageID

Function AddScreenshotTags(handle as integer)
	result as integer
	// This is optional stuff.
	if handle
		result = Steam.SetScreenshotLocation(handle, "In the example program.")
		AddStatus("SetScreenshotLocation: result = " + str(result))
	endif	
EndFunction
