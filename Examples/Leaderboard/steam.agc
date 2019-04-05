#option_explicit

// Holds everything related to the Steam server data.
global server as SteamServerInfo

// A type to hold all of the Steam server information used.
Type SteamServerInfo
	currentLeaderboard as integer
	steamID as integer
	// instruction flags
	downloadRankCallResult as integer
	downloadPageCallResult as integer
	uploadScoreCallResult as integer
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
	callResult as integer
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
leaderboardNameTextID = CreateTextEx(512, 80, "LOADING")
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
	leaderboardInfo[x].callResult = Steam.FindLeaderboard(leaderboardInfo[x].name)
	AddStatus("Finding '" + leaderboardInfo[x].name + "' leaderboard handle.  CallResult handle = " + str(leaderboardInfo[x].callResult))
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
		UploadScore(Random(0, 2147483647))
	endif
	if GetVirtualButtonPressed(MAX_SCORE_BUTTON)
		UploadScore(2147483647)
	endif
	if GetVirtualButtonPressed(MIN_SCORE_BUTTON)
		UploadScore(0)
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
	if Steam.HasUserStatsReceivedResponse()
		if Steam.GetUserStatsReceivedResult() = EResultOK
			AddStatus("User stats received for SteamID: " + str(Steam.GetUserStatsReceivedUser()))
			// Enable all stats buttons.
			for x = 0 to buttonText.length
				SetButtonEnabled(x + 1, 1)
			next
		else
			AddStatus("Error loading user stats.  Result: " + str(Steam.GetUserStatsReceivedResult()))
		endif
	endif
	//
	// Nothing else can be done until user stats are loaded.
	//
	if not Steam.StatsInitialized()
		ExitFunction
	endif
	//
	// Process FindLeaderboard CallResult.
	//
	result as integer
	for x = 0 to leaderboardInfo.length
		if leaderboardInfo[x].callResult
			result = Steam.GetCallResultCode(leaderboardInfo[x].callResult)
			if result
				AddStatus("Find Leaderboard result = " + str(result))
				if result = EResultOK
					//~ foundLeaderboard as LeaderboardFindResult_t
					//~ foundLeaderboard.fromjson(Steam.GetCallResultJSON(leaderboardInfo[x].callResult))
					leaderboardInfo[x].handle = Steam.GetFindLeaderboardHandle(leaderboardInfo[x].callResult)
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
						// Technically the CallResult will go to STATE_SERVER_ERROR when the handle is 0.
						AddStatus("GetLeaderboardHandle error!")
					endif
				else
					AddStatus("ERROR: FindLeaderboard.")
				endif
				// We're done with the CallResult.  Delete it.
				Steam.DeleteCallResult(leaderboardInfo[x].callResult)
				leaderboardInfo[x].callResult = 0
			endif
		endif
	next
	//
	// The rest of this function requires a valid leaderboard handle.
	//
	if leaderboardInfo[server.currentLeaderboard].handle = 0
		ExitFunction
	endif
	//
	// Process UploadLeaderboardScore CallResult.
	//
	if server.uploadScoreCallResult
		result = Steam.GetCallResultCode(server.uploadScoreCallResult)
		if result
			AddStatus("UploadLeaderboardScore call result code: " + str(result))
			if result = EResultOK
				server.currentRank = Steam.GetUploadLeaderboardScoreRankNew(server.uploadScoreCallResult)
				server.currentScore = Steam.GetUploadLeaderboardScoreValue(server.uploadScoreCallResult)
				SetTextString(server.rankTextID, "#" + str(server.currentRank))
				SetTextString(server.scoreTextID, str(server.currentScore))
				AddStatus("LeaderboardScoreChanged: " + TF(Steam.GetUploadLeaderboardScoreChanged(server.uploadScoreCallResult)))
				// These matter only when LeaderboardScoreChanged is true.
				AddStatus("GetLeaderboardGlobalRank - New: " + str(server.currentRank) + ", Previous: " + str(Steam.GetUploadLeaderboardScoreRankPrevious(server.uploadScoreCallResult)))
				// Open leaderboard to new rank page.
				DownloadPage(server.currentRank - Mod(server.currentRank - 1, ENTRIES_PER_PAGE)) // entries are 1-based
			else
				AddStatus("ERROR: UploadLeaderboardScore.  Did you upload scores too often?")
			endif
			// We're done with the CallResult.  Delete it.
			Steam.DeleteCallResult(server.uploadScoreCallResult)
			server.uploadScoreCallResult = 0
		endif
	endif
	//
	// Process DownloadLeaderboardEntries CallResult: user rank.
	//
	if server.downloadRankCallResult
		result = Steam.GetCallResultCode(server.downloadRankCallResult)
		if result
			AddStatus("DownloadLeaderboardEntries: User Rank call result code: " + str(result))
			if result = EResultOK
				if Steam.GetDownloadLeaderboardEntryCount(server.downloadRankCallResult) > 0
					server.currentRank = Steam.GetDownloadLeaderboardEntryGlobalRank(server.downloadRankCallResult, 0)
					server.currentScore = Steam.GetDownloadLeaderboardEntryScore(server.downloadRankCallResult, 0)
					SetTextString(server.rankTextID, "#" + str(server.currentRank))
					SetTextString(server.scoreTextID, str(server.currentScore))
					// Open leaderboard to current rank page.
					DownloadPage(server.currentRank - Mod(server.currentRank - 1, ENTRIES_PER_PAGE)) // entries are 1-based
				else
					SetTextString(server.rankTextID, "NONE")
					SetTextString(server.scoreTextID, "NONE")
					AddStatus("User has no global rank.")
					// Show first page.
					DownloadPage(1)
				endif
				LoadEntryAvatar(server.steamID, server.avatarSpriteID)
			else
				AddStatus("ERROR: DownloadLeaderboardEntries: User rank.")
			endif
			Steam.DeleteCallResult(server.downloadRankCallResult)
			server.downloadRankCallResult = 0
		endif
	endif
	//
	// Process DownloadLeaderboardEntries CallResult: Entry page
	//
	if server.downloadPageCallResult
		result = Steam.GetCallResultCode(server.downloadPageCallResult)
		if result
			AddStatus("DownloadLeaderboardEntries: User Rank call result code: " + str(result))
			if result = EResultOK
				RefreshLeaderboardEntryList(server.downloadPageCallResult)
			else
				AddStatus("ERROR: DownloadLeaderboardEntries: Page.")
			endif
			// Clear the CallResult when done.
			Steam.DeleteCallResult(server.downloadPageCallResult)
			server.downloadPageCallResult = 0
		endif
	endif
	//
	// Check for loaded avatar images.
	//
	while Steam.HasAvatarImageLoadedResponse()
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

Function UploadScore(score as integer)
	server.currentScore = score
	// Add some details for our score.  These are optionaly and you get to decide what they mean.
	x as integer
	for x = 0 to 9
		Steam.AddUploadLeaderboardScoreDetail(x)
	next
	// NOTE: Uploading scores to Steam is rate limited to 10 uploads per 10 minutes and you may only have one outstanding call to this function at a time.
	//~ Steam.UploadLeaderboardScore(leaderboardInfo[server.currentLeaderboard].handle, score)
	//~ AddStatus("Uploading leaderboard score: " + str(score))
	// UploadLeaderboardScoreForceUpdate forces the server to accept the score even if it's worse than before.
	server.uploadScoreCallResult = Steam.UploadLeaderboardScoreForceUpdate(leaderboardInfo[server.currentLeaderboard].handle, score)
	AddStatus("Uploading leaderboard score (force update): " + str(score) + ".  CallResult handle = " + str(server.uploadScoreCallResult))
EndFunction

Function DownloadUserRank()
	server.downloadRankCallResult = Steam.DownloadLeaderboardEntries(leaderboardInfo[server.currentLeaderboard].handle, ELeaderboardDataRequestGlobalAroundUser, 0, 0)
	AddStatus("Downloading user rank.  CallResult handle = " + str(server.downloadRankCallResult))
EndFunction

Function DownloadPage(startEntryNumber as integer)
	if startEntryNumber < 1
		// Don't go before the first page.
		ExitFunction
	elseif startEntryNumber > Steam.GetLeaderboardEntryCount(leaderboardInfo[server.currentLeaderboard].handle)
		// Don't go after the last page.
		ExitFunction
	endif
	server.currentStartEntryNumber = startEntryNumber
	server.downloadPageCallResult = Steam.DownloadLeaderboardEntries(leaderboardInfo[server.currentLeaderboard].handle, ELeaderboardDataRequestGlobal, startEntryNumber, startEntryNumber + ENTRIES_PER_PAGE - 1)
	AddStatus("Downloading page of entries.  CallResult handle = " + str(server.downloadPageCallResult))
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
	AddStatus("handle: " + Steam.GetLeaderboardName(handle))
	SetTextString(leaderboardNameTextID, Steam.GetLeaderboardName(handle))
EndFunction

Function RefreshLeaderboardEntryList(hCallResult as integer)
	AddStatus("Refreshing leaderboard entry list.")
	index as integer
	for index = 0 to server.entryRankTextIDs.length
		SetLeaderboardEntryVisible(index, 0)
	next
	for index = 0 to Steam.GetDownloadLeaderboardEntryCount(hCallResult) - 1
		hSteamID as integer
		hSteamID = Steam.GetDownloadLeaderboardEntryUser(hCallResult, index)
		server.entrySteamID[index] = hSteamID
		SetTextString(server.entryRankTextIDs[index], "#" + str(Steam.GetDownloadLeaderboardEntryGlobalRank(hCallResult, index)))
		SetTextString(server.entryNameTextIDs[index], Steam.GetFriendPersonaName(hSteamID))
		SetTextString(server.entryScoreTextIDs[index], str(Steam.GetDownloadLeaderboardEntryScore(hCallResult, index)))
		AddStatus("Entry details for " + str(Steam.GetDownloadLeaderboardEntryGlobalRank(hCallResult, index)) + ": " + Steam.GetDownloadLeaderboardEntryDetails(hCallResult, index))
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
