#option_explicit

#constant ICON_SIZE		64

// Holds everything related to the Steam server data.
global server as SteamServerInfo

// A type to hold all of the Steam server information used.
Type SteamServerInfo
	achievements as string[]
	achievementIcons as integer[] // When this is -1, check the Steam plugin to see if the icon has been loaded.
	achievementSpriteIDs as integer[]
	achievementTextIDs as integer[]
	// Instructions
	needToStoreStats as integer
	needToUploadScore as integer
EndType

// Additional UI for this demo.
CreateTextEx(0, 80, "Click an icon to toggle the achievement.")
CreateTextEx(512, 80, "____User_Stats____")
global userStatsTextID as integer
userStatsTextID = CreateTextEx(512, 100, "")

#constant RESET_BUTTON			1
#constant ADD_WINS_BUTTON		2
#constant ADD_LOSSES_BUTTON		3
#constant ADD_DISTANCE_BUTTON	4

global buttonText as string[3] = ["RESET", "+5 Wins", "+1 Loss", "+100 Feet"]
x as integer
for x = 0 to buttonText.length
	CreateButton(x + 1, 552 + x * 100, 440, ReplaceString(buttonText[x], "_", NEWLINE, -1))
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

global feet as float

//
// Check and handle input.
//
Function CheckInput()
	x as integer
	// Scrollable text.
	PerformVerticalScroll(statusArea)
	// Click to toggle achievements
	if GetPointerPressed()
		mouseX as float
		mouseY as float
		mouseX = GetPointerX()
		mouseY = GetPointerY()
		for x = 0 to server.achievementSpriteIDs.length
			if GetSpriteHitTest(server.achievementSpriteIDs[x], mouseX, mouseY)
				if Steam.GetAchievement(server.achievements[x])
					// ClearAchievement is not generally needed.  This is just for the demo.
					AddStatus("Clearing achievement: " + server.achievements[x])
					Steam.ClearAchievement(server.achievements[x])
				else
					AddStatus("Setting achievement: " + server.achievements[x])
					Steam.SetAchievement(server.achievements[x])
				endif
				server.achievementIcons[x] = -1 // Set to -1 so we know the icon needs to be reloaded.
				server.needToStoreStats = 1
			endif
		next
	endif
	if GetVirtualButtonPressed(RESET_BUTTON)
		AddStatus("BUTTON: " + buttonText[RESET_BUTTON - 1])
		// ResetAllStats is generally just for testing purposes.
		AddStatus("Resetting all achievements and user stats.")
		// Note: This calls StoreStats itself, so just wait for the callback.
		Steam.ResetAllStats(1)
		// Refresh all achievement icons.
		for x = 0 to server.achievements.length
			server.achievementIcons[x] = -1 // Set to -1 to indicate that the icon needs loaded.
		next
	endif
	if GetVirtualButtonPressed(ADD_WINS_BUTTON)
		AddStatus("BUTTON: " + buttonText[ADD_WINS_BUTTON - 1])
		AddStatus("Increment NumGames: " + TF(Steam.SetStatInt("NumGames", Steam.GetStatInt("NumGames") + 5)))
		AddStatus("Increment NumWins: " + TF(Steam.SetStatInt("NumWins", Steam.GetStatInt("NumWins") + 5)))
		// Note that notification only shows when current progress is less than the max.
		numWins as integer
		numWins = Steam.GetStatInt("NumWins")
		if numWins < 100
			if mod(Steam.GetStatInt("NumWins"), 25) = 0
				AddStatus("Multiple of 25. Indicating progress.")
				Steam.IndicateAchievementProgress("ACH_WIN_100_GAMES", Steam.GetStatInt("NumWins"), 100)
			endif
		elseif numWins = 100
			AddStatus("Setting achievement: ACH_WIN_100_GAMES")
			Steam.SetAchievement("ACH_WIN_100_GAMES")
			// Need to request the achievement icon again.
			server.achievementIcons[GetAchievementIndex("ACH_WIN_100_GAMES")] = -1
		endif
		server.needToStoreStats = 1
	endif
	if GetVirtualButtonPressed(ADD_LOSSES_BUTTON)
		AddStatus("BUTTON: " + buttonText[ADD_LOSSES_BUTTON - 1])
		AddStatus("Increment NumGames: " + TF(Steam.SetStatInt("NumGames", Steam.GetStatInt("NumGames") + 1)))
		AddStatus("Increment NumLosses: " + TF(Steam.SetStatInt("NumLosses", Steam.GetStatInt("NumLosses") + 1)))
		server.needToStoreStats = 1
	endif
	if GetVirtualButtonPressed(ADD_DISTANCE_BUTTON)
		AddStatus("BUTTON: " + buttonText[ADD_DISTANCE_BUTTON - 1])
		feet = feet + 100
		AddStatus("Adding to FeetTraveled, +" + str(feet) + ": " + TF(Steam.SetStatFloat("FeetTraveled", Steam.GetStatFloat("FeetTraveled") + feet)))
		AddStatus("New FeetTraveled: " + str(Steam.GetStatFloat("FeetTraveled")))
		if feet > Steam.GetStatFloat("MaxFeetTraveled")
			AddStatus("Set MaxFeetTraveled to " + str(feet) + ": " + TF(Steam.SetStatFloat("MaxFeetTraveled", feet)))
		endif
		AddStatus("Call UpdateAvgRateStat: " + TF(Steam.UpdateAvgRateStat("AverageSpeed", feet, 1.0)))
		AddStatus("UpdateAvgRateStat: " + str(Steam.GetStatFloat("AverageSpeed")))
		server.needToStoreStats = 1
	endif
EndFunction

// Used to keep track of reported errors so they are only shown once.
global errorReported as integer[1]
#constant ERROR_REQUESTSTATS	0
#constant ERROR_STORESTATS		1

//
// Processes all asynchronous callbacks.
//
Function ProcessCallbacks()
	x as integer
	hSteamID as integer
	//
	// Process RequestStats callback.
	// Loading user stats is the first step.  Have to wait for them to load before doing anything else.
	//
	if Steam.HasNewUserStatsReceived()
		AddStatus("HasNewUserStatsReceived.")
		if Steam.GetNewUserStatsReceivedResult() = EResultOK
			AddStatus("User stats initialized.")
			// Enable all stats buttons.
			for x = 0 to buttonText.length
				SetButtonEnabled(x + 1, 1)
			next
			LoadAchievements()
			RefreshInformation()
		else
			errorReported[ERROR_REQUESTSTATS] = 1
			AddStatus("Error loading user stats.")
		endif
	endif
	//
	// Nothing else can be done until user stats are loaded.
	//
	if not Steam.StatsInitialized()
		ExitFunction
	endif
	//
	// Process StoreStats/ResetStats callback.
	//
	//~ select Steam.GetStoreStatsCallbackState()
		//~ case STATE_IDLE // Only StoreStats when in this state.
			//~ if server.needToStoreStats
				//~ Steam.StoreStats()
				//~ AddStatus("Storing user stats.")
			//~ endif
		//~ endcase
		//~ case STATE_DONE
			//~ server.needToStoreStats = 0
			//~ AddStatus("StatsStored: " + TF(Steam.StatsStored()) + ", AchievementStored: " + TF(Steam.AchievementStored()))
			//~ RefreshInformation()
		//~ endcase
		//~ case STATE_SERVER_ERROR, STATE_CLIENT_ERROR
			//~ if not errorReported[ERROR_STORESTATS]
				//~ errorReported[ERROR_STORESTATS] = 1
				//~ AddStatus("Error storing user stats.")
			//~ endif
		//~ endcase
	//~ endselect
	//
	// Process achievement icon loading.
	//
	// Don't load the achievement icon while stats need to be stored!  Otherwise it might reload the old icon state.
	if server.needToStoreStats = 0
		for x = 0 to server.achievementIcons.length
			// A value of -1 indicates that we're waiting for the icon to download, so try getting the handle again.
			if server.achievementIcons[x] = -1
				server.achievementIcons[x] = LoadAchievementIcon(server.achievements[x], server.achievementSpriteIDs[x])
				AddStatus("GetAchievementIcon for " + server.achievements[x] + " = " + str(server.achievementIcons[x]))
			endif
		next
	endif
EndFunction

Function LoadAchievements()
	// NOTE: Apps should already know the number of achievements.  GetNumAchievements is generally just for this demo.
	AddStatus("GetNumAchievements: " + str(Steam.GetNumAchievements()))
	index as integer
	for index = 0 to Steam.GetNumAchievements() - 1
		// NOTE: Apps should already have these names.  GetAchievementID is generally just for this demo.
		name as string
		name = Steam.GetAchievementAPIName(index)
		server.achievements.insert(name)
		// Create controls to show the achievement icon and information.
		imageID as integer
		imageID = CreateImageColor(0, 0, 0, 0)
		spriteID as integer
		spriteID = CreateSprite(imageID)
		lineY as integer
		lineY = 100 + (ICON_SIZE + 8) * index
		SetSpriteSize(spriteID, ICON_SIZE, ICON_SIZE)
		SetSpritePosition(spriteID, 10, lineY)
		server.achievementSpriteIDs.insert(spriteID)
		text as string
		text = "ID: " + name + ", Hidden: " + TF(Steam.GetAchievementDisplayHidden(name)) + NEWLINE
		text = text + "Name: " + Steam.GetAchievementDisplayName(name) + NEWLINE
		text = text + "Desc: " + Steam.GetAchievementDisplayDesc(name) + NEWLINE
		textID as integer
		textID = CreateTextEx(80, lineY, text)
		SetTextSize(textID, 16)
		textID = CreateTextEx(80, lineY + 16 * 3, "Status: ")
		SetTextSize(textID, 16)
		server.achievementTextIDs.insert(textID)
		server.achievementIcons.insert(-1) // Set to -1 to indicate that the icon needs to be loaded.
	next
EndFunction

Function RefreshInformation()
	AddStatus("Refreshing achievements and user stats information display.")
	// Update achievements
	x as integer
	for x = 0 to server.achievements.length
		lockStatus as string
		if Steam.GetAchievement(server.achievements[x])
			lockStatus = "Unlocked on " + GetDateFromUnix(Steam.GetAchievementUnlockTime(server.achievements[x]))
		else
			lockStatus = "Locked"
		endif
		SetTextString(server.achievementTextIDs[x], "Status: " + lockStatus)
	next
	// Update user stats display
	text as string
	// Game should know the stat names.
	text = text + "NumGames: " + str(Steam.GetStatInt("NumGames")) + NEWLINE
	text = text + "NumWins: " + str(Steam.GetStatInt("NumWins")) + NEWLINE
	text = text + "NumLosses: " + str(Steam.GetStatInt("NumLosses")) + NEWLINE
	text = text + "FeetTraveled: " + str(Steam.GetStatFloat("FeetTraveled")) + NEWLINE
	text = text + "MaxFeetTraveled: " + str(Steam.GetStatFloat("MaxFeetTraveled")) + NEWLINE
	text = text + "AverageSpeed: " + str(Steam.GetStatFloat("AverageSpeed")) + NEWLINE
	SetTextString(userStatsTextID, text)
EndFunction

Function GetAchievementIndex(achID as string)
	x as integer
	for x = 0 to server.achievements.length
		if server.achievements[x] = achID
			ExitFunction x
		endif
	next
EndFunction -1

Function LoadAchievementIcon(achID as string, spriteID as integer)
	imageHandle as integer
	imageHandle = Steam.GetAchievementIcon(achID)
	if imageHandle = 0
		SetSpriteVisible(spriteID, 0)
	elseif imageHandle > 0
		Steam.LoadImageFromHandle(GetSpriteImageID(spriteID), imageHandle)
		SetSpriteVisible(spriteID, 1)
	endif
EndFunction imageHandle
