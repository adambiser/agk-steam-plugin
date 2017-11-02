// Project: AGKPluginTest 
// Created: 2017-10-31
#option_explicit

// show all errors
SetErrorMode(2)

// set window properties
SetWindowTitle("AGKPluginTest")
SetWindowSize(1024, 768, 0)
SetWindowAllowResize(1) // allow the user to resize the window

// set display properties
SetVirtualResolution(1024, 768) // doesn't have to match the window
SetOrientationAllowed(1, 1, 1, 1) // allow both portrait and landscape on mobile devices
SetSyncRate(30, 0) // 30fps instead of 60 to save battery
SetScissor(0,0,0,0) // use the maximum available screen space, no black borders
UseNewDefaultFonts(1) // since version 2.0.22 we can use nicer default fonts
SetPrintSize(20)

timeout as float
x as integer

// Set up scrolling status text
#constant STATUS_HEIGHT 550
CreateText(1, "")
SetTextPosition(1, 0, 0)
SetTextScissor(1, 0, 0, 1024, STATUS_HEIGHT)
SetTextSize(1, 20)

global achievements as string[]

Function AddStatus(text as string)
	SetTextString(1, GetTextString(1) + text + CHR(10))
	// Scroll the text upward.
	height as float
	height = GetTextTotalHeight(1)
	if height < STATUS_HEIGHT
		height = STATUS_HEIGHT
	endif
	SetTextPosition(1, 0, STATUS_HEIGHT - height)
EndFunction

Function TF(value as integer)
	if value
		ExitFunction "TRUE"
	endif
EndFunction "FALSE"

// This section demonstrates the SteamPlugin functionality.

#import_plugin SteamPlugin As Steam

AddStatus("Steam Initialized: " + TF(Steam.Init()))
if Steam.SteamInitialized()
	AddStatus("GetAppID: " + str(Steam.GetAppID()))
	AddStatus("LoggedOn: " + TF(Steam.LoggedOn()))
	
	// RequestStats is already called within Init().
	// Wait up to 2 seconds for current user stats to load.
	timeout = Timer() + 2
	while Timer() < timeout
		Steam.RunCallbacks() // Must be called to allow the callbacks (RequestStats) to process.
		if Steam.StatsInitialized()
			exit
		endif
		Sleep(100)
	endwhile
	// Show achievement info if loaded.
	AddStatus("StatsInitialized: " + TF(Steam.StatsInitialized()))
	if Steam.StatsInitialized()
		AddVirtualButton(99, 950, 50, 100)
		SetVirtualButtonText(99, "Show Stats")
		AddVirtualButton(100, 950, 150, 100)
		SetVirtualButtonText(100, "Reset Stats")
		AddVirtualButton(98, 950, 250, 100)
		SetVirtualButtonText(98, "Clear Status")
		// NOTE: Apps should already know the number of achievements.  GetNumAchievements is generally just for this demo.
		AddStatus("GetNumAchievements: " + str(Steam.GetNumAchievements()))
		for x = 0 to Steam.GetNumAchievements() - 1
			// NOTE: Apps should already have these names.  GetAchievementID is generally just for this demo.
			achievements.insert(Steam.GetAchievementID(x))
		next
		// Create virtual buttons for setting achievements.
		for x = 0 to achievements.length
			AddVirtualButton(x + 1, 50 + x * 100, 700, 100)
			SetVirtualButtonText(x + 1, "Ach " + str(x))
		next
		// User Stats
		AddVirtualButton(90, 50, 600, 100)
		SetVirtualButtonText(90, "Add Win")
		AddVirtualButton(91, 150, 600, 100)
		SetVirtualButtonText(91, "Add Loss")
		AddVirtualButton(92, 250, 600, 100)
		SetVirtualButtonText(92, "Random" + CHR(10) + "Floats")
		// Funky place to define a function, but AGK allows it.
		Function ShowStats()
			AddStatus("-------------------Stats-------------------")
			x as integer
			for x = 0 to achievements.length
				AddStatus("GetAchievementID " + str(x) + ": " + achievements[x] + ", set = " + TF(Steam.GetAchievement(achievements[x])))
			next
			AddStatus("NumGames: " + str(Steam.GetStatInt("NumGames")))
			AddStatus("NumWins: " + str(Steam.GetStatInt("NumWins")))
			AddStatus("NumLosses: " + str(Steam.GetStatInt("NumLosses")))
			AddStatus("FeetTraveled: " + str(Steam.GetStatFloat("FeetTraveled")))
			AddStatus("MaxFeetTraveled: " + str(Steam.GetStatFloat("MaxFeetTraveled")))
			AddStatus("AverageSpeed: " + str(Steam.GetStatFloat("AverageSpeed")))
		EndFunction
		ShowStats()
	endif
	// Interface
	SetTextVisible(1, 1)
	storeStats as integer
	waitForStats as integer
	do
		Sync()
		for x = 1 to achievements.length + 1
			if GetVirtualButtonPressed(x)
				// NOTE: Apps should already have these names.  GetAchievementID is generally just for testing purposes.
				AddStatus("BUTTON: Setting achievement: " + achievements[x - 1])
				// ClearAchievement is also generally not needed.  Using it to force SetAchievement/StoreStats to show the notification.
				// Actually, just use the Reset button to see how things work.
				//~ Steam.ClearAchievement(name)
				// These two lines grant the achievement for the user and then notifies Steam.
				Steam.SetAchievement(achievements[x - 1])
				storeStats = 1
			endif
		next
		if GetVirtualButtonPressed(90) // Win
			AddStatus("Increment NumGames: " + TF(Steam.SetStatInt("NumGames", Steam.GetStatInt("NumGames") + 1)))
			AddStatus("Increment NumWins: " + TF(Steam.SetStatInt("NumWins", Steam.GetStatInt("NumWins") + 1)))
			storeStats = 1
		endif
		if GetVirtualButtonPressed(91) // Loss
			AddStatus("Increment NumGames: " + TF(Steam.SetStatInt("NumGames", Steam.GetStatInt("NumGames") + 1)))
			AddStatus("Increment NumLosses: " + TF(Steam.SetStatInt("NumLosses", Steam.GetStatInt("NumLosses") + 1)))
			storeStats = 1
		endif
		if GetVirtualButtonPressed(92) // Random float data
			feet as float
			feet = feet + 100 //Random(1, 200) / 2.0
			AddStatus("Adding to FeetTraveled, +" + str(feet) + ": " + TF(Steam.SetStatFloat("FeetTraveled", Steam.GetStatFloat("FeetTraveled") + feet)))
			AddStatus("New FeetTraveled: " + str(Steam.GetStatFloat("FeetTraveled")))
			if feet > Steam.GetStatFloat("MaxFeetTraveled")
				AddStatus("Set MaxFeetTraveled to " + str(feet) + ": " + TF(Steam.SetStatFloat("MaxFeetTraveled", feet)))
			endif
			AddStatus("Call UpdateAvgRateStat: " + TF(Steam.UpdateAvgRateStat("AverageSpeed", feet, 1.0)))
			AddStatus("UpdateAvgRateStat: " + str(Steam.GetStatFloat("AverageSpeed")))
			storeStats = 1
		endif
		if GetVirtualButtonPressed(98) // Clear status
			SetTextString(1, "")
		endif
		if GetVirtualButtonPressed(99) // Show stats
			ShowStats()
		endif
		// Clear stats (generally for testing purposes only)
		if GetVirtualButtonPressed(100)
			AddStatus("BUTTON: Resetting stats")
			// Note: This calls StoreStats itself, so just wait for the callback.
			Steam.ResetAllStats(1)
			waitForStats = 1
		endif
		if storeStats
			AddStatus("Storing stats")
			storeStats = 0
			Steam.StoreStats()
			waitForStats = 1
		endif
		if waitForStats
			waitForStats = 0
			// Wait for stats and achievement to be stored.  This step doesn't appear
			// to be necessary unless you really want to know if and when they get stored.
			timeout = Timer() + 2
			while Timer() < timeout
				Steam.RunCallbacks() // Call to allow the StoreStats callbacks to report.
				if Steam.StatsStored()
					exit
				endif
				Sync()
			endwhile
			AddStatus("StatsStored: " + TF(Steam.StatsStored()))
			AddStatus("AchievementStored: " + TF(Steam.AchievementStored()))
		endif
	loop
	// Cleanup
	for x = 1 to Steam.GetNumAchievements()
		DeleteVirtualButton(x)
	next
	DeleteVirtualButton(100)
	DeleteText(1)
else
	Message("Could not initialize Steam API.  Closing.")
endif

Steam.Shutdown()
