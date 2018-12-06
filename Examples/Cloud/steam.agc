#option_explicit

#constant TOGGLE_CLOUD_ENABLED_BUTTON	1
#constant SYNC_WRITE_BUTTON				2
#constant SYNC_READ_BUTTON				3
#constant ASYNC_WRITE_BUTTON			4
#constant ASYNC_READ_BUTTON				5
#constant DELETE_BUTTON					6

// This array stores the valid DLC AppIDs for this app.
global dlcAppIDs as integer[]
// This array stores the list of DLCs we're waiting to download.
global dlcsToDownload as integer[]
global dlcsToUninstall as integer[]

global buttonText as string[5] = ["TOGGLE_ENABLED", "SYNC_WRITE", "SYNC_READ", "ASYNC_WRITE", "ASYNC_READ", "DELETE"]
x as integer
for x = 0 to buttonText.length
	CreateButton(x + 1, 752 + (x - 5) * 100, 40, ReplaceString(buttonText[x], "_", NEWLINE, -1))
	SetButtonEnabled(x + 1, 1)
next

#constant TEST_FILE_NAME "hello.txt"

global dlcUploadBar as ProgressBar
global dlcDownloadBar as ProgressBar
dlcUploadBar = CreateProgressBar(0, 200, 100, 500, "File Upload:")
dlcDownloadBar = CreateProgressBar(0, 200, 130, 500, "File Download:")

global cloudFiles as CloudFileInfo_t[]

AddStatus(Steam.GetSteamID3(Steam.GetSteamID()))
AddStatus("Cloud enabled for app: " + TF(Steam.IsCloudEnabledForApp()))
AddStatus("Cloud enabled for account: " + TF(Steam.IsCloudEnabledForAccount()))
AddStatus("Quota: " + Steam.GetCloudQuotaJSON())
AddStatus("Files in cloud: " + str(Steam.GetCloudFileCount()))
AddStatus("File 0 = " + Steam.GetCloudFileName(0))
AddStatus(TEST_FILE_NAME + " exists: " + TF(Steam.CloudFileExists(TEST_FILE_NAME)))
AddStatus(TEST_FILE_NAME + " size: " + str(Steam.GetCloudFileSize(TEST_FILE_NAME)))
AddStatus(TEST_FILE_NAME + " timestamp: " + GetDateFromUnix(Steam.GetCloudFileTimestamp(TEST_FILE_NAME)))
AddStatus(TEST_FILE_NAME + " persisted: " + TF(Steam.CloudFilePersisted(TEST_FILE_NAME)))
AddStatus(TEST_FILE_NAME + " platforms: " + GetSyncPlatformList(Steam.GetCloudFileSyncPlatforms(TEST_FILE_NAME)))


cloudFiles.fromJSON(Steam.GetCloudFileListJSON())
AddStatus("Files: " + cloudFiles.toJSON())


// For reference: https://steamdb.info/app/480/
// GetDLCDataJSON is the same as using GetDLCDataByIndexJSON for indices 0 to GetDLCCount.
//~ for x = 0 to Steam.GetDLCCount() - 1
	//~ dlcData as DLCData_t
	//~ dlcData.fromjson(Steam.GetDLCDataByIndexJSON(x))
	//~ if dlcData.AppID // Check whether DLC is hidden
		//~ AddStatus("DLC " + str(dlcData.AppID) + ": " + dlcData.Name)
		//~ AddStatus("Owned: " + TF(dlcData.Available)) // DLC app id
		//~ AddStatus("Installed: " + TF(Steam.IsDLCInstalled(dlcData.AppID))) // DLC app id
		//~ // Add button to toggle DLC installation
		//~ dlcAppIDs.insert(dlcData.AppID)
		//~ CreateButton(DLC_START_BUTTON + x, 752 + (DLC_START_BUTTON + x - 6) * 100, 40, "DLC" + NEWLINE + str(dlcData.AppID))
		//~ // Enable the DLC button if the DLC is available/owned.
		//~ if dlcData.Available
			//~ SetButtonEnabled(DLC_START_BUTTON + x, 1)
		//~ endif
	//~ endif
//~ next

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
	if GetVirtualButtonPressed(TOGGLE_CLOUD_ENABLED_BUTTON)
		AddStatus("Toggling cloud enabled for app")
		Steam.SetCloudEnabledForApp(Steam.IsCloudEnabledForApp() ~~ 1)
		AddStatus("Cloud enabled for app: " + TF(Steam.IsCloudEnabledForApp()))
		AddStatus("Cloud enabled for account: " + TF(Steam.IsCloudEnabledForAccount()))
	endif
	if GetVirtualButtonPressed(SYNC_WRITE_BUTTON)
		AddStatus("Sync write: " + TEST_FILE_NAME)
		SyncWrite(TEST_FILE_NAME)
	endif
	if GetVirtualButtonPressed(SYNC_READ_BUTTON)
		AddStatus("Sync read: " + TEST_FILE_NAME)
		SyncRead(TEST_FILE_NAME)
	endif
	if GetVirtualButtonPressed(ASYNC_WRITE_BUTTON)
		AddStatus("Async write: " + TEST_FILE_NAME)
		AsyncWrite(TEST_FILE_NAME)
	endif
	if GetVirtualButtonPressed(ASYNC_READ_BUTTON)
		AddStatus("Async read: " + TEST_FILE_NAME)
		AsyncRead(TEST_FILE_NAME)
	endif
	if GetVirtualButtonPressed(DELETE_BUTTON)
		AddStatus("Deleting: " + TEST_FILE_NAME)
		AddStatus("Success = " + TF(Steam.CloudFileDelete(TEST_FILE_NAME)))
		AddStatus(TEST_FILE_NAME + " exists: " + TF(Steam.CloudFileExists(TEST_FILE_NAME)))
	endif
EndFunction

Function SyncWrite(filename as string)
	memblock as integer
	memblock = CreateMemblock(7)
	SetMemblockString(memblock, 0, "Hello!")
	AddStatus("Success = " + TF(Steam.CloudFileWrite(filename, memblock)))
	// %STEAMPATH%\userdata\[USERID]\[APPID]\remote
	DeleteMemblock(memblock)
EndFunction

global asyncWriteFiles as string[]

Function AsyncWrite(filename as string)
	memblock as integer
	memblock = CreateMemblock(7)
	SetMemblockString(memblock, 0, "Hello!")
	AddStatus("Success = " + TF(Steam.CloudFileWriteAsync(filename, memblock)))
	AddStatus("Success = " + TF(Steam.CloudFileWriteAsync("test2.txt", memblock)))
	asyncWriteFiles.insert(filename)
	asyncWriteFiles.insert("test2.txt")
	// %STEAMPATH%\userdata\[USERID]\[APPID]\remote
	DeleteMemblock(memblock)
EndFunction

Function SyncRead(filename as string)
	memblock as integer
	memblock = Steam.CloudFileRead(filename)
	if memblock
		AddStatus("File Text: " + GetMemblockString(memblock, 0, 7))
		DeleteMemblock(memblock)
	else
		AddStatus("File does not exist") 
	endif
EndFunction

global asyncReadFiles as string[]

Function AsyncRead(filename as string)
	if Steam.CloudFileReadAsync(filename, 0, -1)
		asyncReadFiles.insert(filename)
	endif
	if Steam.CloudFileReadAsync("test2.txt", 0, -1)
		asyncReadFiles.insert("test2.txt")
	endif
EndFunction

// Used to keep track of reported errors so they are only shown once.
global errorReported as integer[1]
#constant ERROR_ASYNCWRITE		0
#constant ERROR_ASYNCREAD		1

//
// Processes all asynchronous callbacks.
//
Function ProcessCallbacks()
	index as integer
	for index = asyncWriteFiles.length to 0 step -1
		select Steam.GetCloudFileWriteAsyncCallbackState(asyncWriteFiles[index])
			case STATE_DONE
				AddStatus(asyncWriteFiles[index] + " - Async file write finished with result: " + str(Steam.GetCloudFileWriteAsyncResult(asyncWriteFiles[index])))
				asyncWriteFiles.remove(index)
			endcase
			case STATE_SERVER_ERROR, STATE_CLIENT_ERROR
				if not errorReported[ERROR_ASYNCWRITE]
					errorReported[ERROR_ASYNCWRITE] = 1
					AddStatus("ERROR: FileWriteAsync.")
				endif
			endcase
		endselect
	next
	for index = asyncReadFiles.length to 0 step -1
		select Steam.GetCloudFileReadAsyncCallbackState(asyncReadFiles[index])
			case STATE_DONE
				AddStatus(asyncReadFiles[index] + " - Async file read finished with result: " + str(Steam.GetCloudFileReadAsyncResult(asyncReadFiles[index])))
				memblock as integer
				memblock = Steam.GetCloudFileReadAsyncMemblock(asyncReadFiles[index])
				if memblock
					AddStatus("File Text: " + GetMemblockString(memblock, 0, Steam.GetCloudFileSize(asyncReadFiles[index])))
					DeleteMemblock(memblock)
				else
					AddStatus("File does not exist") 
				endif
				asyncReadFiles.remove(index)
			endcase
			case STATE_SERVER_ERROR, STATE_CLIENT_ERROR
				if not errorReported[ERROR_ASYNCREAD]
					errorReported[ERROR_ASYNCREAD] = 1
					AddStatus("ERROR: FileReadRead.")
				endif
			endcase
		endselect
	next
	
	//~ x as integer
	//~ If Steam.HasNewLaunchQueryParameters() // Reports when a steam://run/ command is called while running.
		//~ AddStatus("HasNewLaunchQueryParameters.  param1 = " + Steam.GetLaunchQueryParam("param1"))
	//~ endif
	//~ // Check for newly installed DLCs.
	//~ while Steam.HasNewDLCInstalled()
		//~ appID as integer
		//~ appID = Steam.GetNewDLCInstalled()
		//~ AddStatus("DLC " + str(appID) + " has finished downloading.")
		//~ UpdateProgressBar(dlcDownloadBar, 0, 0, "Done")
		//~ for x = 0 to dlcsToDownload.length
			//~ if dlcsToDownload[x] = appID
				//~ dlcsToDownload.remove(x)
				//~ exit
			//~ endif
		//~ next
	//~ endwhile
	//~ // Check the status of downloading DLCs.
	//~ for x = 0 to dlcsToDownload.length
		//~ progress as DownloadProgress_t
		//~ progress.fromjson(Steam.GetDLCDownloadProgressJSON(dlcsToDownload[x]))
		//~ // If the AppID is 0, the DLC isn't being downloaded.
		//~ if progress.AppID
			//~ UpdateProgressBar(dlcDownloadBar, progress.BytesDownloaded, progress.BytesTotal, "")
		//~ endif
	//~ next
EndFunction

Function GetSyncPlatformList(flags as integer)
	platforms as string[]
	if flags = k_ERemoteStoragePlatformAll
		platforms.insert("All")
	endif
	if flags and k_ERemoteStoragePlatformWindows
		platforms.insert("Windows")
	endif
	if flags and k_ERemoteStoragePlatformOSX
		platforms.insert("Mac OSX")
	endif
	if flags and k_ERemoteStoragePlatformPS3
		platforms.insert("PlayStation 3")
	endif
	if flags and k_ERemoteStoragePlatformLinux
		platforms.insert("SteamOS/Linux")
	endif
	if flags and k_ERemoteStoragePlatformReserved2
		platforms.insert("Reserved2")
	endif
EndFunction platforms.toJSON()

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
