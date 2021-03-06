#option_explicit

// Cloud files are stored at
// %STEAMPATH%\userdata\[ACCOUNT_ID]\[APP_ID]\remote

#constant TOGGLE_CLOUD_ENABLED_BUTTON	1
#constant SYNC_WRITE_BUTTON				2
#constant SYNC_READ_BUTTON				3
#constant ASYNC_WRITE_BUTTON			4
#constant ASYNC_READ_BUTTON				5
#constant DELETE_BUTTON					6
#constant OPEN_FOLDER_BUTTON			7

global buttonText as string[6] = ["TOGGLE_ENABLED", "SYNC_WRITE", "SYNC_READ", "ASYNC_WRITE", "ASYNC_READ", "DELETE", "OPEN_FOLDER"]
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

AddStatus(Steam.GetSteamID3(Steam.GetSteamID()))
AddStatus("Cloud enabled for app: " + TF(Steam.IsCloudEnabledForApp()))
AddStatus("Cloud enabled for account: " + TF(Steam.IsCloudEnabledForAccount()))
AddStatus("Quota: Available: " + str(Steam.GetCloudQuotaAvailable()) + ", Total: " + str(Steam.GetCloudQuotaTotal()))
AddStatus("Files in cloud: " + str(Steam.GetCloudFileCount()))
AddStatus("File 0 = " + Steam.GetCloudFileName(0))
AddStatus(TEST_FILE_NAME + " exists: " + TF(Steam.CloudFileExists(TEST_FILE_NAME)))
if Steam.CloudFileExists(TEST_FILE_NAME)
	AddStatus(TEST_FILE_NAME + " size: " + str(Steam.GetCloudFileSize(TEST_FILE_NAME)))
	AddStatus(TEST_FILE_NAME + " timestamp: " + GetDateFromUnix(Steam.GetCloudFileTimestamp(TEST_FILE_NAME)))
	AddStatus(TEST_FILE_NAME + " persisted: " + TF(Steam.CloudFilePersisted(TEST_FILE_NAME)))
	AddStatus(TEST_FILE_NAME + " platforms: " + GetSyncPlatformList(Steam.GetCloudFileSyncPlatforms(TEST_FILE_NAME)))
endif

AddStatus("Files:")
for x = 0 to Steam.GetCloudFileCount() - 1
	AddStatus("> " + Steam.GetCloudFileName(x) + " - size: " + str(Steam.GetCloudFileSize(x)))
next

Function DisplayUGCInfo()
	x as integer
	AddStatus("GetCachedUGCCount: " + str(Steam.GetCloudCachedUGCCount()))
	for x = 0 to Steam.GetCloudCachedUGCCount() - 1
		hUGC as integer
		hUGC = Steam.GetCloudCachedUGCHandle(x)
		AddStatus("UGC " + str(x) + ": Handle = " + str(hUGC) + ", AppID = " + str(Steam.GetCloudUGCDetailsAppID(hUGC)) + ", File Name = " + Steam.GetCloudUGCDetailsFileName(hUGC) + ", Size = " + str(Steam.GetCloudUGCDetailsFileSize(hUGC)) + ", Owner = " + Steam.GetFriendPersonaName(Steam.GetCloudUGCDetailsOwner(hUGC)))
	next	
EndFunction

DisplayUGCInfo()

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
		SyncRead("test2.txt")
	endif
	if GetVirtualButtonPressed(ASYNC_WRITE_BUTTON)
		AsyncWrite(TEST_FILE_NAME)
		AsyncWrite("test2.txt")
	endif
	if GetVirtualButtonPressed(ASYNC_READ_BUTTON)
		AsyncRead(TEST_FILE_NAME)
		AsyncRead("test2.txt")
	endif
	if GetVirtualButtonPressed(DELETE_BUTTON)
		AddStatus("Deleting: " + TEST_FILE_NAME)
		AddStatus("Success = " + TF(Steam.CloudFileDelete(TEST_FILE_NAME)))
		AddStatus(TEST_FILE_NAME + " exists: " + TF(Steam.CloudFileExists(TEST_FILE_NAME)))
	endif
	if GetVirtualButtonPressed(OPEN_FOLDER_BUTTON)
		path as string
		if GetDeviceBaseName() = "windows"
			path = JoinPaths(Steam.GetSteamPath(), "userdata/" + str(Steam.GetAccountID(Steam.GetSteamID())) + "/" + str(Steam.GetAppID()) + "/remote")
			path = ReplaceString(path, "/", "\", -1) // Must use backslash for explorer.
			AddStatus(path)
			RunApp("explorer", path)
		elseif GetDeviceBaseName() = "linux"
			path = JoinPaths(Steam.GetSteamPath(), "userdata/" + str(Steam.GetAccountID(Steam.GetSteamID())) + "/" + str(Steam.GetAppID()) + "/remote")
			//path = ReplaceString(path, "/", "\", -1) // Must use backslash for explorer.
			AddStatus(path)
			RunApp("nautilus", path)
		else
			AddStatus("OS not supported!")
		endif
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

global asyncWriteCallResults as integer[]

Function CreateTempFileMemblock(Text as string)
	filename as string = "temp.tmp"
	file as integer
	file = OpenToWrite(filename)
	WriteLine(file, str(GetUnixTime()))
	WriteLine(file, text)
	CloseFile(file)
	memblock as integer
	memblock = CreateMemblockFromFile(filename)
	DeleteFile(filename)
EndFunction memblock

Function AsyncWrite(filename as string)
	AddStatus("Async write: " + filename)
	memblock as integer
	memblock = CreateTempFileMemblock("Hello from " + filename  + "!")
	hCallResult as integer
	hCallResult = Steam.CloudFileWriteAsync(filename, memblock)
	AddStatus("Call Result: " + str(hCallResult))
	asyncWriteCallResults.insert(hCallResult)
	DeleteMemblock(memblock)
EndFunction

Function SyncRead(filename as string)
	memblock as integer
	memblock = Steam.CloudFileRead(filename)
	if memblock
		AddStatus("File Text: " + GetMemblockString(memblock, 0, GetMemblockSize(memblock)))
		DeleteMemblock(memblock)
	else
		AddStatus("File does not exist") 
	endif
EndFunction

global asyncReadCallResults as integer[]

Function AsyncRead(filename as string)
	AddStatus("Async read: " + filename)
	AddStatus("File size: " + str(Steam.GetCloudFileSize(filename)))
	hCallResult as integer
	hCallResult = Steam.CloudFileReadAsync(filename, 0, -1)
	if hCallResult
		AddStatus("Call Result: " + str(hCallResult))
		asyncReadCallResults.insert(hCallResult)
	else
		AddStatus("Failed")
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
	filename as string
	result as integer
	for index = asyncWriteCallResults.length to 0 step -1
		result = Steam.GetCallResultCode(asyncWriteCallResults[index])
		if result
			if result = EResultOK
				filename = Steam.GetCloudFileWriteAsyncFileName(asyncWriteCallResults[index])
				AddStatus("Finished async write for " + filename)
			else
				filename = Steam.GetCloudFileWriteAsyncFileName(asyncWriteCallResults[index])
				AddStatus("ERROR: FileWriteAsync: " + filename + ", result = " + str(result))
			endif
			// We're done with the CallResult.  Delete it.
			Steam.DeleteCallResult(asyncWriteCallResults[index])
			asyncWriteCallResults.remove(index)
		endif
	next
	for index = asyncReadCallResults.length to 0 step -1
		result = Steam.GetCallResultCode(asyncReadCallResults[index])
		if result
			if result = EResultOK
				filename = Steam.GetCloudFileReadAsyncFileName(asyncReadCallResults[index])
				AddStatus("Finished async read for " + filename)
				memblock as integer
				memblock = Steam.GetCloudFileReadAsyncMemblock(asyncReadCallResults[index])
				if memblock
					AddStatus("Length: " + str(GetMemblockSize(memblock)))
					AddStatus("File Text: " + GetMemblockString(memblock, 0, Steam.GetCloudFileSize(filename)))
					// NOTE: Deleting the FileReadAsync call result will delete its memblock.
				else
					AddStatus("File does not exist!") 
				endif
			else
				filename = Steam.GetCloudFileReadAsyncFileName(asyncReadCallResults[index])
				AddStatus("ERROR: FileReadAsync: " + filename)
			endif
			// We're done with the CallResult.  Delete it.
			Steam.DeleteCallResult(asyncReadCallResults[index])
			asyncReadCallResults.remove(index)
		endif
	next
EndFunction

Function GetSyncPlatformList(flags as integer)
	platforms as string[]
	if flags = ERemoteStoragePlatformAll
		platforms.insert("All")
	endif
	if flags and ERemoteStoragePlatformWindows
		platforms.insert("Windows")
	endif
	if flags and ERemoteStoragePlatformOSX
		platforms.insert("Mac OSX")
	endif
	if flags and ERemoteStoragePlatformPS3
		platforms.insert("PlayStation 3")
	endif
	if flags and ERemoteStoragePlatformLinux
		platforms.insert("SteamOS/Linux")
	endif
	if flags and ERemoteStoragePlatformReserved2
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
