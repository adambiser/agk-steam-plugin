// Project: AGKPluginTest 
// Created: 2017-10-31
// Copyright 2017 Adam Biser
#option_explicit

// show all errors
SetErrorMode(2)
// Set up window and display.
SetWindowTitle("AGKPluginTest")
SetWindowSize(1024, 768, 0)
SetVirtualResolution(1024, 768)
SetSyncRate(30, 0)
UseNewDefaultFonts(1)
SetPrintSize(20)

#import_plugin SteamPlugin As Steam

// Uncommenting these lines forces the app to be run through the Steam client (a simple DRM).
// Note that the existence of the steam_appid.txt file will cause this to always return 0 (useful for development).
//~ if Steam.RestartAppIfNecessary(480) // 480 is this game's appid.
	//~ end
//~ endif

CreateUI()

// Plugin Callback State Value
#constant STATE_SERVER_ERROR	-2
#constant STATE_CLIENT_ERROR	-1
#constant STATE_IDLE			0 // This is the initial state of the callback.
#constant STATE_RUNNING			1 // Generally don't need to check for this state.
#constant STATE_DONE			2 // Reported once as soon as the callback finishes and then changes to IDLE.

// Constants from https://partner.steamgames.com/doc/api/ISteamUserStats#ELeaderboardDataRequest
#constant LEADERBOARD_DATA_GLOBAL		0
#constant LEADERBOARD_DATA_AROUND_USER	1
#constant LEADERBOARD_DATA_FRIENDS		2

// Constants from https://partner.steamgames.com/doc/api/ISteamUserStats#ELeaderboardDisplayType
#constant LEADERBOARD_DISPLAY_NONE			0
#constant LEADERBOARD_DISPLAY_NUMERIC		1
#constant LEADERBOARD_DISPLAY_SECONDS		2
#constant LEADERBOARD_DISPLAY_MILLISECONDS	3

// Constants from https://partner.steamgames.com/doc/api/ISteamUserStats#ELeaderboardSortMethod
#constant LEADERBOARD_SORT_NONE			0
#constant LEADERBOARD_SORT_ASCENDING	1
#constant LEADERBOARD_SORT_DESCENDING	2

// Avatar sizes
#constant AVATAR_SMALL	0
#constant AVATAR_MEDIUM	1
#constant AVATAR_LARGE	2

global server as SteamServerInfo
// A type to hold all of the Steam server information used.
// Your game should probably keep track of things using a similar method.
Type SteamServerInfo
	achievements as string[]
	achievementIcons as integer[]
	achievementImageIDs as integer[]
	achievementSpriteIDs as integer[]
	leaderboardHandle as integer
	// Instructions
	needToStoreStats as integer
	needToUploadScore as integer
	downloadRank as integer
	downloadTop10 as integer
	downloadTop10Avatars as integer
	avatarHandle as integer
EndType
server.avatarHandle = -1 // Use -1 to indicate that the avatar needs to be loaded.

// Note: RequestStats is called within Init().
if not Steam.Init()
	Message("Could not initialize Steam API.  Closing.")
	Quit()
endif

// Show general information
SetTextString(text.appid, "AppID: " + str(Steam.GetAppID()))
SetTextString(text.loggedOn, "LoggedOn: " + TF(Steam.LoggedOn()))
SetTextString(text.steamID, "SteamID: " + Steam.GetPersonaName())

//
// The main loop
//
do
	Sync()
	CheckButtons()
	// Very important!  This MUST be called each frame to allow the Steam API callbacks to process.
	Steam.RunCallbacks()
	ProcessCallbacks()
loop

Steam.Shutdown()
Quit()

global feet as float

//
// Check and handle virtual buttons.
//
Function CheckButtons()
	if GetVirtualButtonPressed(CLEAR_STATUS_BUTTON)
		SetTextString(text.status, "")
	endif
	if GetVirtualButtonPressed(SHOW_STATS_BUTTON)
		ShowUserStats()
	endif
	if GetVirtualButtonPressed(RESET_STATS_BUTTON)
		// Clear stats (generally for testing purposes only)
		AddStatus("BUTTON: Resetting stats and achievements")
		// Note: This calls StoreStats itself, so just wait for the callback.
		Steam.ResetAllStats(1)
		// Refresh all achievement icons.
		for x = 0 to server.achievements.length
			server.achievementIcons[x] = -1 // Set to -1 to indicate that the icon needs loaded.
		next
	endif
	if GetVirtualButtonPressed(SHOW_GLOBAL_RANK_BUTTON)
		server.downloadRank = 1
	endif
	if GetVirtualButtonPressed(SHOW_TOP10_BUTTON)
		server.downloadTop10 = 1
	endif
	if GetVirtualButtonPressed(UPLOAD_SCORE_BUTTON)
		server.needToUploadScore = 1
	endif
	if GetVirtualButtonPressed(ADD_WINS_BUTTON)
		AddStatus("Increment NumGames: " + TF(Steam.SetStatInt("NumGames", Steam.GetStatInt("NumGames") + 25)))
		AddStatus("Increment NumWins: " + TF(Steam.SetStatInt("NumWins", Steam.GetStatInt("NumWins") + 25)))
		// Note that notification only shows when current progress is less than the max.
		Steam.IndicateAchievementProgress("ACH_WIN_100_GAMES", Steam.GetStatInt("NumWins"), 100)
		server.needToStoreStats = 1
	endif
	if GetVirtualButtonPressed(ADD_LOSSES_BUTTON)
		AddStatus("Increment NumGames: " + TF(Steam.SetStatInt("NumGames", Steam.GetStatInt("NumGames") + 1)))
		AddStatus("Increment NumLosses: " + TF(Steam.SetStatInt("NumLosses", Steam.GetStatInt("NumLosses") + 1)))
		server.needToStoreStats = 1
	endif
	if GetVirtualButtonPressed(ADD_DISTANCE_BUTTON)
		feet = feet + 100 //Random(1, 200) / 2.0
		AddStatus("Adding to FeetTraveled, +" + str(feet) + ": " + TF(Steam.SetStatFloat("FeetTraveled", Steam.GetStatFloat("FeetTraveled") + feet)))
		AddStatus("New FeetTraveled: " + str(Steam.GetStatFloat("FeetTraveled")))
		if feet > Steam.GetStatFloat("MaxFeetTraveled")
			AddStatus("Set MaxFeetTraveled to " + str(feet) + ": " + TF(Steam.SetStatFloat("MaxFeetTraveled", feet)))
		endif
		AddStatus("Call UpdateAvgRateStat: " + TF(Steam.UpdateAvgRateStat("AverageSpeed", feet, 1.0)))
		AddStatus("UpdateAvgRateStat: " + str(Steam.GetStatFloat("AverageSpeed")))
		server.needToStoreStats = 1
	endif
	x as integer
	for x = 0 to server.achievements.length
		if GetVirtualButtonPressed(FIRST_ACHIEVEMENT_BUTTON + x)
			// NOTE: Apps should already have these names.  GetAchievementID is generally just for testing purposes.
			AddStatus("BUTTON: Setting achievement: " + server.achievements[x])
			// ClearAchievement is also generally not needed.  Using it to force SetAchievement/StoreStats to show the notification.
			// Actually, just use the Reset button to see how things work.
			//Steam.ClearAchievement(name)
			// These two lines grant the achievement for the user and then notifies Steam.
			Steam.SetAchievement(server.achievements[x])
			server.needToStoreStats = 1
			// Also refresh the achievement icon.
			server.achievementIcons[x] = -1 // Set to -1 to indicate that the icon needs loaded.
		endif
	next
EndFunction

// Used to keep track of reported errors so they are only shown once.
global errorReported as integer[4]
#constant ERROR_REQUESTSTATS	0
#constant ERROR_STORESTATS		1
#constant ERROR_FINDLEADERBOARD	2
#constant ERROR_UPLOADSCORE		3
#constant ERROR_DOWNLOADENTRIES	4

//
// Processes all asynchronous callbacks.
//
Function ProcessCallbacks()
	x as integer
	//
	// Process RequestStats callback.
	// Loading user stats is the first step.  Have to wait for them to load before doing anything else.
	//
	select Steam.GetRequestStatsCallbackState()
		case STATE_DONE
			AddStatus("User status initialized.")
			// Enable stats buttons.
			SetButtonEnabled(SHOW_STATS_BUTTON, 1)
			SetButtonEnabled(RESET_STATS_BUTTON, 1)
			SetButtonEnabled(SHOW_GLOBAL_RANK_BUTTON, 1)
			SetButtonEnabled(SHOW_TOP10_BUTTON, 1)
			SetButtonEnabled(UPLOAD_SCORE_BUTTON, 1)
			SetButtonEnabled(ADD_WINS_BUTTON, 1)
			SetButtonEnabled(ADD_LOSSES_BUTTON, 1)
			SetButtonEnabled(ADD_DISTANCE_BUTTON, 1)
			LoadAchievements()
			ShowUserStats()
		endcase
		case STATE_SERVER_ERROR, STATE_CLIENT_ERROR
			if not errorReported[ERROR_REQUESTSTATS]
				errorReported[ERROR_REQUESTSTATS] = 1
				AddStatus("Error loading user stats.")
			endif
		endcase
	endselect
	//
	// Download the large avatar for the current user.
	// This is a little different from regular callbacks.
	// If the image is already cached, GetAvatar returns a handle and the DONE state is never reported.
	// If the image needs to be cached (large avatars), then a callback is started and behaves like other callbacks.
	//
	if server.avatarHandle = -1
		server.avatarHandle = Steam.GetAvatar(AVATAR_LARGE)
		AddStatus("GetAvatar = " + str(server.avatarHandle))
		if server.avatarHandle > 0
			avatarImageID as integer
			avatarImageID = Steam.LoadImageFromHandle(server.avatarHandle)
			AddStatus("Avatar ImageID: " + str(avatarImageID))
			if avatarImageID
				sprite as integer
				sprite = CreateSprite(avatarImageID)
				SetSpritePosition(sprite, 0, 768 - GetImageHeight(avatarImageID))
			endif
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
	select Steam.GetStoreStatsCallbackState()
		case STATE_IDLE // Only StoreStats when in this state.
			if server.needToStoreStats
				Steam.StoreStats()
				AddStatus("Storing user stats.")
			endif
		endcase
		case STATE_DONE
			server.needToStoreStats = 0
			AddStatus("StatsStored: " + TF(Steam.StatsStored()))
			AddStatus("AchievementStored: " + TF(Steam.AchievementStored()))
		endcase
		case STATE_SERVER_ERROR, STATE_CLIENT_ERROR
			if not errorReported[ERROR_STORESTATS]
				errorReported[ERROR_STORESTATS] = 1
				AddStatus("Error storing user stats.")
			endif
		endcase
	endselect
	//
	// Process achievement icon callback.
	//
	// Don't load the achievement icon while stats need to be stored!  Might reload the old icon state.
	if server.needToStoreStats = 0
		for x = 0 to server.achievementIcons.length
			// A value of 0-1 indicates that we're waiting for the icon to download, so try getting the handle again.
			if server.achievementIcons[x] = -1
				server.achievementIcons[x] = Steam.GetAchievementIcon(server.achievements[x])
				//~ AddStatus("GetAchievementIcon for " + str(x) + " = " + str(server.achievementIcons[x]))
				if server.achievementIcons[x] = 0
					SetSpriteVisible(server.achievementSpriteIDs[x], 0)
				elseif server.achievementIcons[x] > 0
					Steam.LoadImageFromHandle(server.achievementImageIDs[x], server.achievementIcons[x])
					SetSpritePosition(server.achievementSpriteIDs[x], 0 + x * GetImageWidth(server.achievementImageIDs[x]), 300)
					SetSpriteSize(server.achievementSpriteIDs[x], GetImageWidth(server.achievementImageIDs[x]), GetImageHeight(server.achievementImageIDs[x]))
					SetSpriteVisible(server.achievementSpriteIDs[x], 1)
				endif
			endif
		next
	endif
	//
	// Process FindLeaderboard callback.
	//
	select Steam.GetFindLeaderboardCallbackState()
		case STATE_IDLE
			if server.leaderboardHandle = 0
				// Leaderboard names are NOT case sensitive.
				Steam.FindLeaderboard("Feet Traveled")
				AddStatus("Finding 'Feet Traveled' leaderboard handle")
			endif
		endcase
		case STATE_DONE
			server.leaderboardHandle = Steam.GetLeaderboardHandle()
			// If the leaderboard is not found, the handle is 0.
			if server.leaderboardHandle <> 0
				AddStatus("Leaderboard handle: " + str(server.leaderboardHandle))
				AddStatus("Leaderboard name: " + Steam.GetLeaderboardName(server.leaderboardHandle))
				AddStatus("Leaderboard entry count: " + str(Steam.GetLeaderboardEntryCount(server.leaderboardHandle)))
				AddStatus("Leaderboard display type: " + str(Steam.GetLeaderboardDisplayType(server.leaderboardHandle)))
				AddStatus("Leaderboard sort: " + str(Steam.GetLeaderboardSortMethod(server.leaderboardHandle)))
				// Set these flags to demonstrate downloading rank and uploading a score.
				server.downloadRank = 1
				server.downloadTop10 = 1
				server.needToUploadScore = 1
				server.downloadTop10Avatars = 1
			else
				// Technically the callback will go to STATE_SERVER_ERROR when the handle is 0.
				AddStatus("GetLeaderboardHandle error!")
			endif
		endcase
		case STATE_SERVER_ERROR, STATE_CLIENT_ERROR
			if not errorReported[ERROR_FINDLEADERBOARD]
				errorReported[ERROR_FINDLEADERBOARD] = 1
				AddStatus("ERROR: FindLeaderboard.")
			endif
		endcase
	endselect
	//
	// The rest of this function requires a valid leaderboard handle.
	//
	if server.leaderboardHandle = 0
		ExitFunction
	endif
	//
	// Process UploadLeaderboardScore callback.
	//
	select Steam.GetUploadLeaderboardScoreCallbackState()
		case STATE_IDLE
			if server.needToUploadScore
				score as integer
				score = Val(GetEditBoxText(1))
				if str(score) <> GetEditBoxText(1)
					Message("Score needs to be a number.")
					server.needToUploadScore = 0
				else
					// NOTE: Uploading scores to Steam is rate limited to 10 uploads per 10 minutes and you may only have one outstanding call to this function at a time.
					Steam.UploadLeaderboardScoreForceUpdate(server.leaderboardHandle, score)
					AddStatus("Uploading leaderboard score (force update).")
				endif
			endif
		endcase
		case STATE_DONE
			// Steam.LeaderboardScoreStored() will return 1 when STATE_DONE and 0 for STATE_SERVER_ERROR.
			server.needToUploadScore = 0
			AddStatus("-------------------Upload Results-------------------")
			AddStatus("LeaderboardScoreStored")
			AddStatus("LeaderboardScoreChanged: " + TF(Steam.LeaderboardScoreChanged()))
			AddStatus("GetLeaderboardUploadedScore: " + str(Steam.GetLeaderboardUploadedScore()))
			// These matter only when LeaderboardScoreChanged is true.
			AddStatus("GetLeaderboardGlobalRank - New: " + str(Steam.GetLeaderboardGlobalRankNew()) + ", Previous: " + str(Steam.GetLeaderboardGlobalRankPrevious()))
			AddStatus("--------------------------------------")
		endcase
		case STATE_SERVER_ERROR, STATE_CLIENT_ERROR
			if not errorReported[ERROR_UPLOADSCORE]
				errorReported[ERROR_UPLOADSCORE] = 1
				AddStatus("ERROR: UploadLeaderboardScore.")
			endif
		endcase
	endselect
	//
	// Process DownloadLeaderboardEntries callback.
	//
	select Steam.GetDownloadLeaderboardEntriesCallbackState()
		case STATE_IDLE
			if server.downloadRank
				// 0 to 0 around user will give only the user's entry.
				Steam.DownloadLeaderboardEntries(server.leaderboardHandle, LEADERBOARD_DATA_AROUND_USER, 0, 0)
				AddStatus("Downloading user rank.")
			elseif server.downloadTop10
				Steam.DownloadLeaderboardEntries(server.leaderboardHandle, LEADERBOARD_DATA_GLOBAL, 1, 10)
				AddStatus("Downloading top 10.")
			endif
		endcase
		case STATE_DONE
			if server.downloadRank
				server.downloadRank = 0 // Clear flag
				if Steam.GetDownloadedLeaderboardEntryCount()
					AddStatus("User rank: #" + str(Steam.GetDownloadedLeaderboardEntryGlobalRank(0)) + ", Score: " + str(Steam.GetDownloadedLeaderboardEntryScore(0)) + ", User: " + Steam.GetDownloadedLeaderboardEntryPersonaName(0))
				else
					AddStatus("User has no global rank.")
				endif
			elseif server.downloadTop10
				server.downloadTop10 = 0 // Clear flag
				AddStatus("--- Global leaders ---")
				AddStatus("GetDownloadedLeaderboardEntryCount: " + str(Steam.GetDownloadedLeaderboardEntryCount()))
				for x = 0 to Steam.GetDownloadedLeaderboardEntryCount() - 1
					AddStatus("#" + str(Steam.GetDownloadedLeaderboardEntryGlobalRank(x)) + ", Score: " + str(Steam.GetDownloadedLeaderboardEntryScore(x)) + ", User: " + Steam.GetDownloadedLeaderboardEntryPersonaName(x))
				next
				// Load an avatar from the leaderboard.
				// From how Steam docs read, small and medium images are already cached when the leaderboard is requested
				if server.downloadTop10Avatars
					leaderAvatarHandle as integer
					leaderAvatarImageID as integer
					for x = 0 to Steam.GetDownloadedLeaderboardEntryCount() - 1
						leaderAvatarHandle = Steam.GetDownloadedLeaderboardEntryAvatar(x, AVATAR_MEDIUM)
						if leaderAvatarHandle > 0
							leaderAvatarImageID = Steam.LoadImageFromHandle(leaderAvatarHandle)
							if leaderAvatarImageID
								sprite = CreateSprite(leaderAvatarImageID)
								SetSpritePosition(sprite, 205 + mod(x, 4) * GetImageWidth(leaderAvatarImageID), 768 - GetImageHeight(leaderAvatarImageID) * (3 - x / 4))
							endif
						endif
					next
				endif
			endif
		endcase
		case STATE_SERVER_ERROR, STATE_CLIENT_ERROR
			if not errorReported[ERROR_DOWNLOADENTRIES]
				errorReported[ERROR_DOWNLOADENTRIES] = 1
				AddStatus("ERROR: DownloadLeaderboardEntries.")
			endif
		endcase
	endselect
EndFunction

Function LoadAchievements()
	// NOTE: Apps should already know the number of achievements.  GetNumAchievements is generally just for this demo.
	AddStatus("GetNumAchievements: " + str(Steam.GetNumAchievements()))
	index as integer
	for index = 0 to Steam.GetNumAchievements() - 1
		name as string
		name = Steam.GetAchievementID(index)
		// NOTE: Apps should already have these names.  GetAchievementID is generally just for this demo.
		server.achievements.insert(name)
		// Create achievement buttons here.
		CreateButton(FIRST_ACHIEVEMENT_BUTTON + index, buttonColumns[mod(index, 3)], 485 + (index / 3) * 60, name)
		SetVirtualButtonSize(FIRST_ACHIEVEMENT_BUTTON + index, 150, 60)
		SetButtonEnabled(FIRST_ACHIEVEMENT_BUTTON + index, 1)
		// Load achievement icon.
		imageID as integer
		imageID = CreateImageColor(0, 0, 0, 0)
		spriteID as integer
		spriteID = CreateSprite(imageID)
		//~ SetSpriteSize(spriteID, 48, 48) // achievement icons are 48x48
		//~ SetSpritePosition(spriteID, 0 + index * 48, 300)
		server.achievementIcons.insert(-1) // Set to -1 to indicate that the icon needs loaded.
		server.achievementImageIDs.insert(imageID)
		server.achievementSpriteIDs.insert(spriteID)
	next
EndFunction

Function ShowUserStats()
	AddStatus("-------------------Achievements-------------------")
	// Game should know the achievement names.
	x as integer
	for x = 0 to server.achievements.length
		AddStatus("GetAchievementID " + str(x) + ": " + server.achievements[x] + ", set = " + TF(Steam.GetAchievement(server.achievements[x])))
		// Achievement display attributes.
		AddStatus(Steam.GetAchievementDisplayName(server.achievements[x]) + ": " + Steam.GetAchievementDisplayDesc(server.achievements[x]))
		AddStatus("Hidden: " + str(Steam.GetAchievementDisplayHidden(server.achievements[x])))
	next
	AddStatus("-------------------Stats-------------------")
	// Game should know the stat names.
	AddStatus("NumGames: " + str(Steam.GetStatInt("NumGames")))
	AddStatus("NumWins: " + str(Steam.GetStatInt("NumWins")))
	AddStatus("NumLosses: " + str(Steam.GetStatInt("NumLosses")))
	AddStatus("FeetTraveled: " + str(Steam.GetStatFloat("FeetTraveled")))
	AddStatus("MaxFeetTraveled: " + str(Steam.GetStatFloat("MaxFeetTraveled")))
	AddStatus("AverageSpeed: " + str(Steam.GetStatFloat("AverageSpeed")))
	AddStatus("--------------------------------------")
EndFunction

//---------------------------------------------------------------------
//
// Keep the UI stuff separate to highligh the plugin code.
//
// The type holds IDs for all text controls.
global text as TextControls
Type TextControls
	status as integer
	appid as integer
	loggedOn as integer
	steamID as integer
EndType

#constant CLEAR_STATUS_BUTTON		1
#constant SHOW_STATS_BUTTON			2
#constant RESET_STATS_BUTTON		3
#constant SHOW_GLOBAL_RANK_BUTTON	4
#constant SHOW_TOP10_BUTTON			5
#constant UPLOAD_SCORE_BUTTON		6
#constant ADD_WINS_BUTTON			7
#constant ADD_LOSSES_BUTTON			8
#constant ADD_DISTANCE_BUTTON		9
#constant FIRST_ACHIEVEMENT_BUTTON	20

global buttonColumns as integer [2] = [75, 225, 375]

Function CreateUI()
	// Set up scrolling status text
	#constant STATUS_HEIGHT 768
	text.status = CreateText("")
	SetTextPosition(text.status, 500, 0)
	SetTextScissor(text.status, 500, 0, 1024, STATUS_HEIGHT)
	SetTextSize(text.status, 18)
	SetTextVisible(text.status, 1)
	// AppID
	text.appid = CreateText("appid")
	SetTextPosition(text.appid, 0, 0)
	SetTextSize(text.appid, 20)
	// loggedOn
	text.loggedOn = CreateText("loggedOn")
	SetTextPosition(text.loggedOn, 0, 20)
	SetTextSize(text.loggedOn, 20)
	// steamID
	text.steamID = CreateText("steamID")
	SetTextPosition(text.steamID, 0, 40)
	SetTextSize(text.steamID, 20)
	// Buttons
	CreateButton(CLEAR_STATUS_BUTTON, buttonColumns[2], 50, "Clear Status")
	SetButtonEnabled(CLEAR_STATUS_BUTTON, 1)
	
	CreateButton(SHOW_STATS_BUTTON, buttonColumns[0], 150, "Show Stats")
	CreateButton(RESET_STATS_BUTTON, buttonColumns[1], 150, "Reset Stats")
	
	CreateButton(SHOW_GLOBAL_RANK_BUTTON, buttonColumns[0], 250, "Global Rank")
	CreateButton(SHOW_TOP10_BUTTON, buttonColumns[1], 250, "Top 10")
	CreateButton(UPLOAD_SCORE_BUTTON, buttonColumns[2], 250, "Upload Score")
	CreateEditBox(1)
	SetEditBoxPosition(1, buttonColumns[2] - 75, 300)
	SetEditBoxText(1, "100")

	CreateButton(ADD_WINS_BUTTON, buttonColumns[0], 400, "Add 25 Wins")
	CreateButton(ADD_LOSSES_BUTTON, buttonColumns[1], 400, "Add 1 Loss")
	CreateButton(ADD_DISTANCE_BUTTON, buttonColumns[2], 400, "Add Distance")
EndFunction

Function CreateButton(id as integer, x as integer, y as integer, txt as string)
	AddVirtualButton(id, x, y, 80)
	SetVirtualButtonSize(id, 150, 80)
	SetVirtualButtonText(id, txt)
	SetButtonEnabled(id, 0)
EndFunction

Function SetButtonEnabled(id as integer, enabled as integer)
	SetVirtualButtonActive(id, enabled)
	if enabled
		SetVirtualButtonColor(id, 192, 192, 192)
	else
		SetVirtualButtonColor(id, 64, 64, 64)
	endif
EndFunction	

Function AddStatus(status as string)
	SetTextString(text.status, GetTextString(text.status) + status + CHR(10))
	// Scroll the text upward.
	height as float
	height = GetTextTotalHeight(text.status)
	if height < STATUS_HEIGHT
		height = STATUS_HEIGHT
	endif
	SetTextPosition(text.status, GetTextX(text.status), STATUS_HEIGHT - height)
EndFunction

Function TF(value as integer)
	if value
		ExitFunction "TRUE"
	endif
EndFunction "FALSE"

Function Quit()
	// Cleanup
	x as integer
	for x = 1 to Steam.GetNumAchievements()
		DeleteVirtualButton(x)
	next
	DeleteVirtualButton(100)
	DeleteText(1)
	end
EndFunction
