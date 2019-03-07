#option_explicit

// Holds everything related to the Steam server data.
global server as SteamServerInfo

// A type to hold all of the Steam server information used.
Type SteamServerInfo
	currentLeaderboard as integer
	steamID as integer
	// instruction flags
	downloadRankCallback as integer
	downloadPageCallback as integer
	needToUploadScore as integer
	// Current user info
	rankTextID as integer
	avatarSpriteID as integer
	scoreTextID as integer
	currentRank as integer
	currentScore as integer
	// Entry info
	currentStartEntryNumber as integer // Remember: entries are 1-based
	entrySteamID as integer[]
	entryRankTextIDs as integer[]
	entryAvatarSpriteIDs as integer[]
	entryNameTextIDs as integer[]
	entryScoreTextIDs as integer[]
EndType

#constant LEADERBOARD_QUICKEST_WIN	"Quickest Win"
#constant LEADERBOARD_FEET_TRAVELED	"Feet Traveled"

Type LeaderboardInfoType
	name as string
	callback as integer
	handle as integer
EndType

global leaderboardInfo as LeaderboardInfoType[1]
LeaderboardInfo[0].name = LEADERBOARD_FEET_TRAVELED
LeaderboardInfo[1].name = LEADERBOARD_QUICKEST_WIN

// Additional UI for this demo.
#constant ENTRIES_PER_PAGE		10
#constant AVATAR_SIZE			32

//~ CreateTextEx(0, 80, "Click an icon to toggle the achievement.")
//~ CreateTextEx(512, 80, "____User_Stats____")
global leaderboardNameTextID as integer
leaderboardNameTextID = CreateTextEx(512, 80, "")
SetTextAlignment(leaderboardNameTextID, 1)
CreateTextEx(180, 10, "Current rank:")
CreateTextEx(0, 70, "Click an entry avatar or name to view the profile.")

#constant FIRST_BUTTON				1
#constant PREV_BUTTON				2
#constant NEXT_BUTTON				3
#constant LAST_BUTTON				4
#constant RANDOM_PAGE_BUTTON		5
#constant CHANGE_LEADERBOARD_BUTTON	6
#constant RANDOM_SCORE_BUTTON		7
#constant MAX_SCORE_BUTTON			8
#constant MIN_SCORE_BUTTON			9

global buttonText as string[8] = ["|<", "<<", ">>", ">|", "RANDOM_PAGE", "CHANGE_LEADERBOARD", "RANDOM_SCORE", "MAX_SCORE", "MIN_SCORE"]
x as integer
for x = 0 to 4
	CreateButton(x + 1, 352 + x * 100, 540, ReplaceString(buttonText[x], "_", NEWLINE, -1))
next
for x = 5 to 8
	CreateButton(x + 1, 752 + (x - 6) * 100, 40, ReplaceString(buttonText[x], "_", NEWLINE, -1))
next

CreateLeaderboardUI()

server.steamID = Steam.GetSteamID() // Store the current user steam ID.

// Start searching for the leaderboards.
for x = 0 to leaderboardInfo.length
	leaderboardInfo[x].callback = Steam.FindLeaderboard(leaderboardInfo[x].name)
	AddStatus("Finding '" + leaderboardInfo[x].name + "' leaderboard handle.  Callback handle = " + str(leaderboardInfo[x].callback))
next

//
// The main loop
//
do
	if Steam.IsGameOverlayActive()
		AddStatus("Game overlay activated.")
		while Steam.IsGameOverlayActive()
			Sync()
			// Very important to remember RunCallbacks here or IsGameOverlayActive will never deactivate!
			// There's no need to call ProcessCallbacks though because the plugin stores everything until
			// called so ProcessCallbacks below will handle everything that happens while in this loop.
			Steam.RunCallbacks()
		endwhile
		AddStatus("Game overlay deactivated.")
	endif
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
	entryCount as integer
	// Scrollable text.
	PerformVerticalScroll(statusArea)
	// Click user avatar to take to that page.
	if GetPointerPressed()
		mouseX as float
		mouseY as float
		mouseX = GetPointerX()
		mouseY = GetPointerY()
		if GetSpriteHitTest(server.avatarSpriteID, mouseX, mouseY)
			// Open leaderboard to rank page.
			DownloadPage(server.currentRank - Mod(server.currentRank - 1, ENTRIES_PER_PAGE)) // entries are 1-based
		else
			// Click an entry avatar or name to open the user profile.
			x as integer
			for x = 0 to server.entryAvatarSpriteIDs.length
				if GetSpriteHitTest(server.entryAvatarSpriteIDs[x], mouseX, mouseY) or GetTextHitTest(server.entryNameTextIDs[x], mouseX, mouseY)
					//~ OpenBrowser("http://steamcommunity.com/profiles/" + Steam.GetSteamID64(server.entrySteamID[x]))
					Steam.ActivateGameOverlayToUser("steamid", server.entrySteamID[x])
					continue
				endif
			next x
		endif
	endif
	// Navigation buttons.
	if GetVirtualButtonPressed(FIRST_BUTTON)
		DownloadPage(1)
	endif
	if GetVirtualButtonPressed(PREV_BUTTON)
		DownloadPage(server.currentStartEntryNumber - ENTRIES_PER_PAGE)
	endif
	if GetVirtualButtonPressed(NEXT_BUTTON)
		DownloadPage(server.currentStartEntryNumber + ENTRIES_PER_PAGE)
	endif
	if GetVirtualButtonPressed(LAST_BUTTON)
		entryCount = Steam.GetLeaderboardEntryCount(leaderboardInfo[server.currentLeaderboard].handle)
		DownloadPage(entryCount - Mod(entryCount - 1, ENTRIES_PER_PAGE)) // entries are 1-based
	endif
	if GetVirtualButtonPressed(RANDOM_PAGE_BUTTON) // Go to a random page... helps see how avatar loading works.
		entryCount = Steam.GetLeaderboardEntryCount(leaderboardInfo[server.currentLeaderboard].handle)
		page as integer
		page = Random(0, entryCount / ENTRIES_PER_PAGE)
		DownloadPage(page * ENTRIES_PER_PAGE + 1) // entries are 1-based
	endif
	// Score-setting buttons
	if GetVirtualButtonPressed(CHANGE_LEADERBOARD_BUTTON)
		inc server.currentLeaderboard
		if server.currentLeaderboard > leaderboardInfo.length
			server.currentLeaderboard = 0
		endif
		RefreshLeaderboardInfo()
		DownloadUserRank()
	endif
	if GetVirtualButtonPressed(RANDOM_SCORE_BUTTON)
		server.currentScore = Random(0, 2147483647)
		server.needToUploadScore = 1
	endif
	if GetVirtualButtonPressed(MAX_SCORE_BUTTON)
		server.currentScore = 2147483647
		server.needToUploadScore = 1
	endif
	if GetVirtualButtonPressed(MIN_SCORE_BUTTON)
		server.currentScore = 0
		server.needToUploadScore = 1
	endif
EndFunction

// Used to keep track of reported errors so they are only shown once.
global errorReported as integer[3]
#constant ERROR_REQUESTSTATS	0
#constant ERROR_FINDLEADERBOARD	1
#constant ERROR_UPLOADSCORE		2
#constant ERROR_DOWNLOADENTRIES	3

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
	select Steam.GetRequestStatsCallbackState()
		case STATE_DONE
			AddStatus("User stats initialized.")
			// Enable all stats buttons.
			for x = 0 to buttonText.length
				SetButtonEnabled(x + 1, 1)
			next
			//~ SetButtonEnabled(4, 1)
		endcase
		case STATE_SERVER_ERROR, STATE_CLIENT_ERROR
			if not errorReported[ERROR_REQUESTSTATS]
				errorReported[ERROR_REQUESTSTATS] = 1
				AddStatus("Error loading user stats.")
			endif
		endcase
	endselect
	//
	// Nothing else can be done until user stats are loaded.
	//
	if not Steam.StatsInitialized()
		ExitFunction
	endif
	//
	// Process FindLeaderboard callback.
	//
	for x = 0 to leaderboardInfo.length
		if leaderboardInfo[x].callback
			select Steam.GetCallbackState(leaderboardInfo[x].callback)
				case STATE_DONE
					leaderboardInfo[x].handle = Steam.GetLeaderboardHandle(leaderboardInfo[x].callback)
					// If the leaderboard is not found, the handle is 0.
					if leaderboardInfo[x].handle <> 0
						// Refresh some information if the currently-viewed leaderboard has loaded.
						if x = server.currentLeaderboard
							RefreshLeaderboardInfo()
							DownloadUserRank()
						endif
						AddStatus("Leaderboard '" + leaderboardInfo[x].name + "' handle: " + str(leaderboardInfo[x].handle))
						AddStatus("Leaderboard entry count: " + str(Steam.GetLeaderboardEntryCount(leaderboardInfo[x].handle)))
						AddStatus("Leaderboard display type: " + str(Steam.GetLeaderboardDisplayType(leaderboardInfo[x].handle)))
						AddStatus("Leaderboard sort: " + str(Steam.GetLeaderboardSortMethod(leaderboardInfo[x].handle)))
					else
						// Technically the callback will go to STATE_SERVER_ERROR when the handle is 0.
						AddStatus("GetLeaderboardHandle error!")
					endif
					// We're done with the callback.  Delete it.
					Steam.DeleteCallback(leaderboardInfo[x].callback)
					leaderboardInfo[x].callback = 0
				endcase
				case STATE_SERVER_ERROR, STATE_CLIENT_ERROR
					AddStatus("ERROR: FindLeaderboard.")
					// We're done with the callback.  Delete it.
					Steam.DeleteCallback(leaderboardInfo[x].callback)
					leaderboardInfo[x].callback = 0
				endcase
			endselect
		endif
	next
	//
	// The rest of this function requires a valid leaderboard handle.
	//
	if leaderboardInfo[server.currentLeaderboard].handle = 0
		ExitFunction
	endif
	//
	// Process UploadLeaderboardScore callback.
	//
	select Steam.GetUploadLeaderboardScoreCallbackState()
		case STATE_IDLE
			if server.needToUploadScore
				// NOTE: Uploading scores to Steam is rate limited to 10 uploads per 10 minutes and you may only have one outstanding call to this function at a time.
				//~ Steam.UploadLeaderboardScore(leaderboardInfo[server.currentLeaderboard].handle, server.currentScore)
				// UploadLeaderboardScoreForceUpdate forces the server to accept the score even if it's worse than before.
				Steam.UploadLeaderboardScoreForceUpdate(leaderboardInfo[server.currentLeaderboard].handle, server.currentScore)
				AddStatus("Uploading leaderboard score (force update): " + str(server.currentScore))
			endif
		endcase
		case STATE_DONE
			// Steam.LeaderboardScoreStored() will return 1 when STATE_DONE and 0 for STATE_SERVER_ERROR.
			server.needToUploadScore = 0
			server.currentRank = Steam.GetLeaderboardGlobalRankNew()
			SetTextString(server.rankTextID, "#" + str(server.currentRank))
			SetTextString(server.scoreTextID, str(Steam.GetLeaderboardUploadedScore()))
			server.currentScore = Steam.GetLeaderboardUploadedScore()
			AddStatus("LeaderboardScoreChanged: " + TF(Steam.LeaderboardScoreChanged()))
			// These matter only when LeaderboardScoreChanged is true.
			AddStatus("GetLeaderboardGlobalRank - New: " + str(Steam.GetLeaderboardGlobalRankNew()) + ", Previous: " + str(Steam.GetLeaderboardGlobalRankPrevious()))
			// Open leaderboard to new rank page.
			DownloadPage(server.currentRank - Mod(server.currentRank - 1, ENTRIES_PER_PAGE)) // entries are 1-based
		endcase
		case STATE_SERVER_ERROR, STATE_CLIENT_ERROR
			if not errorReported[ERROR_UPLOADSCORE]
				errorReported[ERROR_UPLOADSCORE] = 1
				AddStatus("ERROR: UploadLeaderboardScore.  Did you upload scores too often?")
			endif
		endcase
	endselect
	//
	// Process DownloadLeaderboardEntries callback.
	//
	if server.downloadRankCallback
		Select Steam.GetCallbackState(server.downloadRankCallback)
			case STATE_DONE
				if Steam.GetDownloadedLeaderboardEntryCount(server.downloadRankCallback)
					server.currentRank = Steam.GetDownloadedLeaderboardEntryGlobalRank(server.downloadRankCallback, 0)
					SetTextString(server.rankTextID, "#" + str(Steam.GetDownloadedLeaderboardEntryGlobalRank(server.downloadRankCallback, 0)))
					SetTextString(server.scoreTextID, str(Steam.GetDownloadedLeaderboardEntryScore(server.downloadRankCallback, 0)))
					server.currentScore = Steam.GetDownloadedLeaderboardEntryScore(server.downloadRankCallback, 0)
					// Open leaderboard to current rank page.
					DownloadPage(server.currentRank - Mod(server.currentRank - 1, ENTRIES_PER_PAGE)) // entries are 1-based
				else
					AddStatus("User has no global rank.")
					// Show first page.
					DownloadPage(1)
				endif
				LoadEntryAvatar(server.steamID, server.avatarSpriteID)
				Steam.DeleteCallback(server.downloadRankCallback)
				server.downloadRankCallback = 0
			endcase
			case STATE_SERVER_ERROR, STATE_CLIENT_ERROR
				AddStatus("ERROR: DownloadLeaderboardEntries: User rank.")
				Steam.DeleteCallback(server.downloadRankCallback)
				server.downloadRankCallback = 0
			endcase
		endselect
	endif
	if server.downloadPageCallback
		Select Steam.GetCallbackState(server.downloadPageCallback)
			case STATE_DONE
				RefreshLeaderboardUI()
				// Clear the callback when done.
				Steam.DeleteCallback(server.downloadPageCallback)
				server.downloadPageCallback = 0
			endcase
			case STATE_SERVER_ERROR, STATE_CLIENT_ERROR
				AddStatus("ERROR: DownloadLeaderboardEntries: Page.")
				// Clear the callback when done.
				Steam.DeleteCallback(server.downloadPageCallback)
				server.downloadPageCallback = 0
			endcase
		endselect
	endif
	//
	// Check for loaded avatar images.
	//
	while Steam.HasAvatarImageLoaded()
		hSteamID = Steam.GetAvatarImageLoadedUser()
		AddStatus("Received avatar for steam handle " + str(hSteamID))
		if hSteamID = server.steamID
			// Current user's avatar image.
			LoadEntryAvatar(hSteamID, server.avatarSpriteID)
		else
			// See if this steam id is on the current leaderboard page.  If not, ignore it.
			for x = 0 to server.entrySteamID.length
				if server.entrySteamID[x] = hSteamID
					LoadEntryAvatar(hSteamID, server.entryAvatarSpriteIDs[x])
				endif
			next
		endif
	endwhile
EndFunction

Function DownloadUserRank()
	server.downloadRankCallback = Steam.DownloadLeaderboardEntries(leaderboardInfo[server.currentLeaderboard].handle, k_ELeaderboardDataRequestGlobalAroundUser, 0, 0)
	AddStatus("Downloading user rank.  Callback handle = " + str(server.downloadRankCallback))
EndFunction

Function DownloadPage(startEntryNumber as integer)
	if startEntryNumber < 1
		// Don't go before the first page.
		ExitFunction
	elseif server.currentStartEntryNumber > Steam.GetLeaderboardEntryCount(leaderboardInfo[server.currentLeaderboard].handle)
		// Don't go after the last page.
		ExitFunction
	endif
	server.currentStartEntryNumber = startEntryNumber
	server.downloadPageCallback = Steam.DownloadLeaderboardEntries(leaderboardInfo[server.currentLeaderboard].handle, k_ELeaderboardDataRequestGlobal, startEntryNumber, startEntryNumber + ENTRIES_PER_PAGE - 1)
	AddStatus("Downloading page of entries.  Callback handle = " + str(server.downloadPageCallback))
EndFunction

Function CreateLeaderboardUI()
	server.rankTextID = CreateTextEx(340, 10, "")
	SetTextAlignment(server.rankTextID, 2) // right align scores
	server.avatarSpriteID = CreateSprite(CreateImageColor(0, 0, 0, 255))
	SetSpritePosition(server.avatarSpriteID, 350, 5)
	SetSpriteSize(server.avatarSpriteID, AVATAR_SIZE, AVATAR_SIZE)
	server.scoreTextID = CreateTextEx(500, 10, "")
	SetTextAlignment(server.scoreTextID, 2) // right align scores
	// entries
	index as integer
	for index = 0 to ENTRIES_PER_PAGE - 1
		server.entrySteamID.insert(0)
		avatarY as integer
		avatarY = 100 + index * 40
		textY as integer
		textY = avatarY + 5
		textID as integer
		textID = CreateTextEx(140, textY, "")
		SetTextAlignment(textID, 2) // right align rank
		server.entryRankTextIDs.insert(textID)
		spriteID as integer
		spriteID = CreateSprite(CreateImageColor(0, 0, 0, 255))
		SetSpritePosition(spriteID, 150, avatarY)
		SetSpriteSize(spriteID, AVATAR_SIZE, AVATAR_SIZE)
		server.entryAvatarSpriteIDs.insert(spriteID)
		textID = CreateTextEx(190, textY, "")
		server.entryNameTextIDs.insert(textID)
		textID = CreateTextEx(WINDOW_WIDTH - 150, textY, "")
		SetTextAlignment(textID, 2) // right align score
		server.entryScoreTextIDs.insert(textID)
	next
EndFunction

Function RefreshLeaderboardInfo()
	handle as integer
	handle = leaderboardInfo[server.currentLeaderboard].handle
	if handle = 0
		SetTextString(leaderboardNameTextID, "LOADING")
		ExitFunction
	endif
	SetTextString(leaderboardNameTextID, Steam.GetLeaderboardName(handle))
EndFunction

Function RefreshLeaderboardUI()
	AddStatus("Refreshing leaderboard UI.")
	index as integer
	for index = 0 to server.entryRankTextIDs.length
		SetLeaderboardEntryVisible(index, 0)
	next
	for index = 0 to Steam.GetDownloadedLeaderboardEntryCount(server.downloadPageCallback) - 1
		hSteamID as integer
		hSteamID = Steam.GetDownloadedLeaderboardEntryUser(server.downloadPageCallback, index)
		server.entrySteamID[index] = hSteamID
		SetTextString(server.entryRankTextIDs[index], "#" + str(Steam.GetDownloadedLeaderboardEntryGlobalRank(server.downloadPageCallback, index)))
		SetTextString(server.entryNameTextIDs[index], Steam.GetFriendPersonaName(hSteamID))
		SetTextString(server.entryScoreTextIDs[index], str(Steam.GetDownloadedLeaderboardEntryScore(server.downloadPageCallback, index)))
		LoadEntryAvatar(hSteamID, server.entryAvatarSpriteIDs[index])
		SetLeaderboardEntryVisible(index, 1)
	next
EndFunction

Function SetLeaderboardEntryVisible(index as integer, visible as integer)
	if not visible
		server.entrySteamID[index] = 0
	endif
	SetTextVisible(server.entryRankTextIDs[index], visible)
	SetSpriteVisible(server.entryAvatarSpriteIDs[index], visible)
	SetTextVisible(server.entryNameTextIDs[index], visible)
	SetTextVisible(server.entryScoreTextIDs[index], visible)
EndFunction

Function LoadEntryAvatar(hSteamID as integer, spriteID as integer)
	avatarHandle as integer
	avatarHandle = Steam.GetFriendAvatar(hSteamID, AVATAR_SMALL)
	if avatarHandle > 0
		Steam.LoadImageFromHandle(GetSpriteImageID(spriteID), avatarHandle)
	endif
EndFunction
