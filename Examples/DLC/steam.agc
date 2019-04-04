#option_explicit

#constant RUN_480_BUTTON		1
#constant DLC_START_BUTTON		2

// This array stores the valid DLC AppIDs for this app.
global dlcAppIDs as integer[]
// This array stores the list of DLCs we're waiting to download.
global dlcsToDownload as integer[]
global dlcsToUninstall as integer[]

global buttonText as string[0] = ["RUN 480"]
x as integer
for x = 0 to buttonText.length
	CreateButton(x + 1, 752 + (x - 5) * 100, 40, ReplaceString(buttonText[x], "_", NEWLINE, -1))
	SetButtonEnabled(x + 1, 1)
next

global dlcDownloadBar as ProgressBar
dlcDownloadBar = CreateProgressBar(0, 200, 100, 500, "DLC Download:")

AddStatus("App Owner: " + Steam.GetFriendPersonaName(Steam.GetAppOwner()))
AddStatus("App 480 Installed: " + TF(Steam.IsAppInstalled(480)))
AddStatus("Installed Depots: " + Steam.GetInstalledDepotsJSON(480, 10))
// This will be 0 when the app is not installed.
AddStatus("App BuildID: " + str(Steam.GetAppBuildID()))
AddStatus("Current Beta Name: " + Steam.GetCurrentBetaName())
AddStatus("App Install Dir: " + Steam.GetAppInstallDir(480))
// Shameless plugs
AddStatus("SpaceWar Purchase Time: " + GetDateFromUnix(Steam.GetEarliestPurchaseUnixTime(480)))
AddStatus("Toxic Terror Purchase Time: " + GetDateFromUnix(Steam.GetEarliestPurchaseUnixTime(486630)))
AddStatus("Boot Hill Blaster Purchase Time: " + GetDateFromUnix(Steam.GetEarliestPurchaseUnixTime(743660)))
AddStatus("Available Game Languages: " + Steam.GetAvailableGameLanguages())
// This will be blank when not installed.
AddStatus("Current Game Language: " + Steam.GetCurrentGameLanguage())
AddStatus("Cybercafe: " + TF(Steam.IsCybercafe()))
AddStatus("Low Violence: " + TF(Steam.IsLowViolence()))
AddStatus("VAC Banned: " + TF(Steam.IsVACBanned()))
AddStatus("Subscribed: " + TF(Steam.IsSubscribed()))
AddStatus("Subscribed From Free Weekend: " + TF(Steam.IsSubscribedFromFreeWeekend()))
//~ AddStatus("Half-Life 2 Subscribed: " + TF(Steam.IsSubscribedApp(220)))
AddStatus("DLC Count: " + str(Steam.GetDLCCount()))
AddStatus("GetLaunchQueryParam at startup.  param1 = " + Steam.GetLaunchQueryParam("param1"))
AddStatus("GetLaunchCommandLine: " + Steam.GetLaunchCommandLine())

// For reference: https://steamdb.info/app/480/
for x = 0 to Steam.GetDLCCount() - 1
	appID as integer
	available as integer
	name as string
	appID = Steam.GetDLCDataByIndexAppID(x)
	available = Steam.GetDLCDataByIndexAvailable(x)
	name = Steam.GetDLCDataByIndexName(x)
	AddStatus("DLC " + str(x) + ".  AppID: " + str(appID) + " - " + name)
	if appID // Check whether DLC is hidden
		AddStatus("Owned: " + TF(available))
		AddStatus("Installed: " + TF(Steam.IsDLCInstalled(appID))) // DLC app id
		// Add button to toggle DLC installation
		dlcAppIDs.insert(appID)
		CreateButton(DLC_START_BUTTON + x, 752 + (DLC_START_BUTTON + x - 6) * 100, 40, "DLC" + NEWLINE + str(appID))
		// Enable the DLC button if the DLC is available/owned.
		if available
			SetButtonEnabled(DLC_START_BUTTON + x, 1)
		endif
	endif
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
	x as integer

	// Scrollable text.
	PerformVerticalScroll(statusArea)
	if GetVirtualButtonPressed(RUN_480_BUTTON)
		AddStatus("Calling Steam run 480 url")
		OpenBrowser("steam://run/480//?param1=value1&param2=value2")
	endif
	for x = 0 to dlcAppIDs.length
		if GetVirtualButtonPressed(DLC_START_BUTTON + x)
			ToggleDLCInstall(x)
		endif
	next x
EndFunction

Function ToggleDLCInstall(index as integer)
	appID as integer
	appID = dlcAppIDs[index]
	// Check to see if we're already doing something with this DLC.
	x as integer
	for x = 0 to dlcsToDownload.length
		if dlcsToDownload[x] = appID
			AddStatus("Already installing DLC " + str(appID))
			ExitFunction
		endif
	next
	for x = 0 to dlcsToUninstall.length
		if dlcsToUninstall[x] = appID
			AddStatus("Already uninstalling DLC " + str(appID))
			ExitFunction
		endif
	next
	// Proceed with install or uninstall.
	installed as integer
	installed = Steam.IsDLCInstalled(appID)
	AddStatus("DLC " + str(appID) + " installed: " + TF(installed))
	if installed
		AddStatus("Uninstalling DLC " + str(appID))
		Steam.UninstallDLC(appID)
		dlcsToUninstall.insert(appID)
	else
		AddStatus("Installing DLC " + str(appID))
		Steam.InstallDLC(appID)
		dlcsToDownload.insert(appID)
		UpdateProgressBar(dlcDownloadBar, 0, 0, "Waiting")
	endif
EndFunction

//
// Processes all asynchronous callbacks.
//
Function ProcessCallbacks()
	x as integer
	If Steam.HasNewUrlLaunchParametersResponse() // Reports when a steam://run/ command is called while running.
		AddStatus("HasNewLaunchQueryParameters.  param1 = " + Steam.GetLaunchQueryParam("param1"))
		AddStatus("GetLaunchCommandLine: " + Steam.GetLaunchCommandLine())
	endif
	// Check the status of uninstalling DLCs.
	// DLCs appear to uninstall when the app exits.
	//~ for x = dlcsToUninstall.length to 0 step -1
		//~ if Steam.IsDLCInstalled(dlcsToUninstall[x]) = 0
			//~ AddStatus("DLC " + str(dlcsToUninstall[x]) + " uninstalled.")
			//~ dlcsToUninstall.remove(x)
		//~ endif
	//~ next
	// Check for newly installed DLCs.
	while Steam.HasDLCInstalledResponse()
		appID as integer
		appID = Steam.GetDLCInstalledAppID()
		AddStatus("DLC " + str(appID) + " has finished downloading.")
		UpdateProgressBar(dlcDownloadBar, 0, 0, "Done")
		for x = 0 to dlcsToDownload.length
			if dlcsToDownload[x] = appID
				dlcsToDownload.remove(x)
				exit
			endif
		next
	endwhile
	// Check the status of downloading DLCs.
	for x = 0 to dlcsToDownload.length
		bytesDownloaded as integer
		bytesTotal as integer
		bytesDownloaded = Steam.GetDLCDownloadProgressBytesDownloaded(dlcsToDownload[x])
		bytesTotal = Steam.GetDLCDownloadProgressBytesTotal(dlcsToDownload[x])
		// BytesDownloaded and BytesTotal both report -1 when the DLC is not downloading.
		if bytesTotal > -1
			UpdateProgressBar(dlcDownloadBar, bytesDownloaded, bytesTotal, "")
		endif
	next
EndFunction

//
// Progress Bar code
//
Type ProgressBar
	CaptionID as integer
	BackgroundID as integer
	BarID as integer
	ProgressTextID as integer
	LastValue as integer
EndType

Function CreateProgressBar(captionX as integer, barX as integer, y as integer, barWidth as integer, caption as string)
	bar as ProgressBar
	bar.CaptionID = CreateTextEx(captionX, y, caption)
	bar.BackgroundID = CreateSprite(CreateImageColor(64, 64, 64, 255))
	SetSpritePosition(bar.BackgroundID, barX, y)
	SetSpriteSize(bar.BackgroundID, barWidth, 20)
	bar.BarID = CreateSprite(CreateImageColor(255, 255, 255, 255))
	SetSpritePosition(bar.BarID, barX, y)
	SetSpriteSize(bar.BarID, 0, 20)
	bar.ProgressTextID = CreateTextEx(barX + barWidth / 2, y, "")
	SetTextAlignment(bar.ProgressTextID, 1) // center
EndFunction bar

Function UpdateProgressBar(bar ref as ProgressBar, value as integer, max as integer, status as string)
	percent as float
	if max > value
		percent = 1.0 * value / max
		if value <> bar.LastValue
			AddStatus("Downloaded " + str(value) + " of " + str(max))
		endif
	endif
	bar.LastValue = value
	SetSpriteSize(bar.BarID, percent * GetSpriteWidth(bar.BackgroundID), GetSpriteHeight(bar.BackgroundID))
	if status = ""
		status = str(value) + "/" + str(max)
	endif
	SetTextString(bar.ProgressTextID, status)
EndFunction
