#option_explicit

#constant OFFLINE_COLOR		127, 127, 127, 255
#constant ONLINE_COLOR		85, 165, 196, 255
#constant INGAME_COLOR		144, 186, 60, 255

// Holds everything related to the Steam server data.
global server as SteamServerInfo

// A type to hold all of the Steam server information used.
Type SteamServerInfo
	selectedGroup as integer
	friendGroups as string[]
	groupFriends as FriendInfo[]
EndType

Type FriendInfo
	hSteamID as integer
	name as string
	state as integer
	//~ level as integer // GetFriendSteamLevel doesn't seem to work like the Steam API docs say.
	gameinfo as FriendGameInfo_t
	avatarImageID as integer
	avatarHandle as integer
EndType

// Additional UI for this demo.
//~ #constant RESET_BUTTON			1
//~ 
//~ global buttonText as string[3] = ["RESET", "+5 Wins", "+1 Loss", "+100 Feet"]
//~ x as integer
//~ for x = 0 to buttonText.length
	//~ CreateButton(x + 1, 552 + x * 100, 440, ReplaceString(buttonText[x], "_", NEWLINE, -1))
//~ next
#constant GROUP_LIST_X		10
#constant GROUP_LIST_Y		100
#constant GROUP_LIST_WIDTH	200
#constant FRIEND_LIST_X		220
#constant FRIEND_LIST_Y		100
#constant FRIEND_LIST_WIDTH	794
#constant LIST_HEIGHT		440 // 20 lines

CreateFriendListUI()

//
// The main loop
//
do
	Sync()
	CheckMouseWheel()
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
	delta as float
	mouseX as float
	mouseY as float
	delta = GetRawMouseWheelDelta()
	mouseX = GetPointerX()
	mouseY = GetPointerY()
	// Scrolling lists.
	if delta <> 0
		if GetSpriteHitTest(ui.groupListBackground, mouseX, mouseY)
			UpdateGroupListUI(-delta)
		elseif GetSpriteHitTest(ui.friendListBackground, mouseX, mouseY)
			UpdateFriendListUI(-delta)
		endif
	endif
	if GetRawMouseLeftPressed()
		x as integer
		for x = 0 to ui.groupNameIDs.length
			if GetTextVisible(ui.groupNameIDs[x])
				if GetTextHitTest(ui.groupNameIDs[x], mouseX, mouseY)
					SelectFriendGroup(x)
					exit
				endif
			endif
		next
	endif
EndFunction

//
// Processes all asynchronous callbacks.
//
Function ProcessCallbacks()
	hSteamID as integer
	flags as integer
	index as integer
	while Steam.HasPersonaStateChanged()
		hSteamID = Steam.GetPersonaStateChangedUser()
		flags = Steam.GetPersonaStateChangedFlags()
		AddStatus("Persona State Change: " + Steam.GetFriendPersonaName(hSteamID) + ", flags: " + GetPersonaStateChangeFlagText(flags))
		index = GetFriendSteamIDIndex(hSteamID)
		if index >= 0
			// Take this friend out of the list and re-add in the sorted index.
			server.groupFriends.remove(index)
			// Technically this should/could just load what changed according to the PersonaStateChange flags,
			// but to keep this demo simple, reloading everything about the user instead.
			AddFriendInfoSorted(GetFriendInfo(hSteamID))
			UpdateFriendListUI(0)
			newIndex as integer
			newIndex = GetFriendSteamIDIndex(hSteamID)
			AddStatus(server.groupFriends[newIndex].name + " moved from index " + str(index) + " to index " + str(newIndex))
		endif
	endwhile
	while Steam.HasAvatarImageLoaded()
		hSteamID = Steam.GetAvatarImageLoadedUser()
		AddStatus("Avatar loaded for " + Steam.GetFriendPersonaName(hSteamID))
		// Load the new avatar.
		index = GetFriendSteamIDIndex(hSteamID)
		if index >= 0
			server.groupFriends[index].avatarHandle = Steam.GetFriendAvatar(hSteamID, AVATAR_SMALL)
			if server.groupFriends[index].avatarHandle > 0
				Steam.LoadImageFromHandle(server.groupFriends[index].avatarImageID, server.groupFriends[index].avatarHandle)
			endif
		endif
	endwhile
EndFunction

Type UIInfo
	topGroupIndex as integer
	groupListBackground as integer
	groupNameIDs as integer[21]
	topFriendIndex as integer
	friendListBackground as integer
	friendNameIDs as integer[10]
	friendAvatarBackground as integer[10]
	friendAvatarIDs as integer[10]
EndType
global ui as UIInfo

Function CreateFriendListUI()
	x as integer
	y as integer
	// Scrollable list of groups on the left.
	CreateTextEx(GROUP_LIST_X, GROUP_LIST_Y - 20, "Groups (Scrollable, Clickable)")
	ui.groupListBackground = CreateSprite(CreateImageColor(32, 32, 32, 255))
	SetSpritePosition(ui.groupListBackground, GROUP_LIST_X, GROUP_LIST_Y)
	SetSpriteSize(ui.groupListBackground, GROUP_LIST_WIDTH, LIST_HEIGHT)
	for x = 0 to ui.groupNameIDs.length
		y = GROUP_LIST_Y + x * 20
		ui.groupNameIDs[x] = CreateTextEx(GROUP_LIST_X, y, "Group")
	next
	// Scrollable List of friends on the right.
	CreateTextEx(FRIEND_LIST_X, FRIEND_LIST_Y - 20, "Friends (Scrollable)")
	ui.friendListBackground = CreateSprite(CreateImageColor(32, 32, 32, 255))
	SetSpritePosition(ui.friendListBackground, FRIEND_LIST_X, FRIEND_LIST_Y)
	SetSpriteSize(ui.friendListBackground, FRIEND_LIST_WIDTH, LIST_HEIGHT)
	for x = 0 to ui.friendNameIDs.length
		y = FRIEND_LIST_Y + x * 40
		ui.friendNameIDs[x] = CreateTextEx(FRIEND_LIST_X + 42, y + 10, "Friend")
		ui.friendAvatarBackground[x] = CreateSprite(CreateImageColor(255, 255, 255, 255))
		SetSpritePosition(ui.friendAvatarBackground[x], FRIEND_LIST_X + 2, y + 2)
		SetSpriteSize(ui.friendAvatarBackground[x], 36, 36)
		ui.friendAvatarIDs[x] = CreateSprite(CreateImageColor(0, 0, 0, 0))
		SetSpritePosition(ui.friendAvatarIDs[x], FRIEND_LIST_X + 4, y + 4)
		SetSpriteSize(ui.friendAvatarIDs[x], 32, 32)
	next
	// Load lists.
	RefreshFriendGroupList()
EndFunction

//
// Reload the list of friend group names.
//
Function RefreshFriendGroupList()
	server.friendGroups.length = -1
	// Make index 0 represent the "All friends" list.
	server.friendGroups.insert("All Friends (" + str(Steam.GetFriendCount(k_EFriendFlagImmediate)) + ")")
	groupCount as integer
	groupCount = Steam.GetFriendsGroupCount()
	x as integer
	for x = 0 to groupCount - 1
		groupID as integer
		groupID = Steam.GetFriendsGroupIDByIndex(x)
		server.friendGroups.insert(Steam.GetFriendsGroupName(groupID) + " (" + str(Steam.GetFriendsGroupMembersCount(groupID)) + ")")
	next
	UpdateGroupListUI(-ui.topGroupIndex) // Scroll to top of group list
	SelectFriendGroup(0) // Show the all friends group.
EndFunction

//
// Updates the group list on the left (for mouse wheel scrolling)
//
Function UpdateGroupListUI(delta as integer)
	inc ui.topGroupIndex, delta
	if ui.topGroupIndex < 0
		ui.topGroupIndex = 0
	elseif server.friendGroups.length <= ui.groupNameIDs.length
		ui.topGroupIndex = 0
	elseif ui.topGroupIndex + (ui.groupNameIDs.length) > server.friendGroups.length
		ui.topGroupIndex = server.friendGroups.length - ui.groupNameIDs.length
	endif
	x as integer
	for x = ui.topGroupIndex to ui.topGroupIndex + ui.groupNameIDs.length
		if x > server.friendGroups.length
			SetTextVisible(ui.groupNameIDs[x - ui.topGroupIndex], 0)
		else
			SetTextString(ui.groupNameIDs[x - ui.topGroupIndex], server.friendGroups[x])
			SetTextVisible(ui.groupNameIDs[x - ui.topGroupIndex], 1)
		endif
	next
EndFunction

//
// Loads the friends in the selected group.
//
Function SelectFriendGroup(index as integer)
	server.selectedGroup = index
	friendListJSON as string
	// Index 0 in this app is a hard-coded "All friends" list.
	if index = 0
		friendListJSON = Steam.GetFriendListJSON(k_EFriendFlagImmediate)
	else
		// Subtract 1 because in this app index 0 = all friends
		friendListJSON = Steam.GetFriendsGroupMembersListJSON(Steam.GetFriendsGroupIDByIndex(index - 1))
	endif
	server.groupFriends.length = -1
	groupFriendIDs as integer[]
	groupFriendIDs.fromJSON(friendListJSON)
	x as integer
	for x = 0 to groupFriendIDs.length
		// Load the friend info and add it to the friend list (sorted)
		AddFriendInfoSorted(GetFriendInfo(groupFriendIDs[x]))
	next
	UpdateFriendListUI(-ui.topFriendIndex) // Scroll to top of friend list.
EndFunction

//
// Creates a new FriendInfo with the current information for a user.
//
Function GetFriendInfo(hSteamID as integer)
	info as FriendInfo
	info.hSteamID = hSteamID
	info.name = Steam.GetFriendPersonaName(hSteamID)
	info.state = Steam.GetFriendPersonaState(hSteamID)
	//~ info.level = Steam.GetFriendSteamLevel(hSteamID)
	info.gameinfo.fromJSON(Steam.GetFriendGamePlayedJSON(hSteamID))
	info.avatarImageID = CreateImageColor(0, 0, 0, 0)
	info.avatarHandle = Steam.GetFriendAvatar(hSteamID, AVATAR_SMALL)
	if info.avatarHandle > 0
		Steam.LoadImageFromHandle(info.avatarImageID, info.avatarHandle)
	endif
EndFunction info

//
// Adds a FriendInfo item to the list of friends in the group, sorting In-Game, Online, then Offline, each set alphabetical.
//
Function AddFriendInfoSorted(info as FriendInfo)
	// Sort order: In-game, online, offline, each group alphabetically.
	index as integer
	infoGroup as integer
	infoGroup = GetFriendInfoSortGroup(info)
	for index = 0 to server.groupFriends.length
		check as FriendInfo
		check = server.groupFriends[index]
		// Online go first.
		checkGroup as integer
		checkGroup = GetFriendInfoSortGroup(check)
		if infoGroup < checkGroup
			// Found next group below, use this index.
			exit
		elseif infoGroup = checkGroup
			// Same group, compare names.
			if Upper(info.name) < Upper(check.name)
				// Found name below, use this index
				exit
			endif
		endif
	next
	if index <= server.groupFriends.length
		server.groupFriends.insert(info, index)
	else
		server.groupFriends.insert(info)
	endif
EndFunction

//
// Determine which group a user is in.  Used for sorting.
// 0 = In-game, 1 = Online, 2 = Offline.
//
Function GetFriendInfoSortGroup(info as FriendInfo)
	if info.state <> k_EPersonaStateOffline
		if info.gameinfo.InGame
			ExitFunction 0 // ingame group
		else
			ExitFunction 1 // online group
		endif
	endif
EndFunction 2 // offline group

Function UpdateFriendListUI(delta as integer)
	inc ui.topFriendIndex, delta
	if ui.topFriendIndex < 0
		ui.topFriendIndex = 0
	elseif server.groupFriends.length <= ui.friendNameIDs.length
		ui.topFriendIndex = 0
	elseif ui.topFriendIndex + (ui.friendNameIDs.length) > server.groupFriends.length
		ui.topFriendIndex = server.groupFriends.length - ui.friendNameIDs.length
	endif
	x as integer
	for x = 0 to ui.friendNameIDs.length
		friendIndex as integer
		friendIndex = x + ui.topFriendIndex
		if friendIndex > server.groupFriends.length
			SetTextVisible(ui.friendNameIDs[x], 0)
			SetSpriteVisible(ui.friendAvatarBackground[x], 0)
			SetSpriteVisible(ui.friendAvatarIDs[x], 0)
			SetSpriteImage(ui.friendAvatarIDs[x], 0)
		else
			UpdateFriendStatus(server.groupFriends[friendIndex].hSteamID)
		endif
	next
EndFunction

Function UpdateFriendStatus(hSteamID as integer)
	// Find index of hSteamID.
	index as integer
	index = GetFriendSteamIDIndex(hSteamID)
	if index = -1
		ExitFunction
	endif
	// Update ui if shown.
	uiindex as integer
	uiindex = index - ui.topFriendIndex
	if uiindex >= 0 and uiindex <= ui.friendNameIDs.length
		info as friendInfo
		info = server.groupFriends[index]
		text as string
		text = info.name
		if info.state = k_EPersonaStateOffline
			SetTextColor(ui.friendNameIDs[uiindex], OFFLINE_COLOR)
			SetSpriteColor(ui.friendAvatarBackground[uiindex], OFFLINE_COLOR)
		elseif info.gameinfo.InGame
			SetTextColor(ui.friendNameIDs[uiindex], INGAME_COLOR)
			SetSpriteColor(ui.friendAvatarBackground[uiindex], INGAME_COLOR)
			// Note: GetAppName is a restricted interface that can only be used by approved apps.
			// Space War (480) is one such approved app.
			appname as string
			appname = Steam.GetAppName(info.gameinfo.GameAppID)
			if len(appname)
				text = text + " [In-Game: " + appname + "]"
			else
				text = text + " [In-Game]"
			endif
		else
			SetTextColor(ui.friendNameIDs[uiindex], ONLINE_COLOR)
			SetSpriteColor(ui.friendAvatarBackground[uiindex], ONLINE_COLOR)
			text = text + " [" + GetFriendPersonaStateText(info.state) + "]"
		endif
		//~ text = text + " - Level " + str(info.level)
		SetTextString(ui.friendNameIDs[uiindex], text)
		SetTextVisible(ui.friendNameIDs[uiindex], 1)
		if info.avatarHandle > 0
			Steam.LoadImageFromHandle(info.avatarImageID, info.avatarHandle)
			SetSpriteImage(ui.friendAvatarIDs[uiindex], info.avatarImageID)
			SetSpriteVisible(ui.friendAvatarIDs[uiindex], 1)
			SetSpriteVisible(ui.friendAvatarBackground[uiindex], 1)
		else
			SetSpriteVisible(ui.friendAvatarIDs[uiindex], 0)
		endif
	endif	
EndFunction

Function GetFriendSteamIDIndex(hSteamID as integer)
	x as integer
	for x = 0 to server.groupFriends.length
		if server.groupFriends[x].hSteamID = hSteamID
			ExitFunction x
		endif
	next
EndFunction -1

Function HasFlag(value as integer, flag as integer)
	result as integer
	result = ((value && flag) = flag)
EndFunction result

Function GetPersonaStateChangeFlagText(flags as integer)
	names as string[]
	if HasFlag(flags, k_EPersonaChangeName)
		names.insert("Name") // GetFriendPersonaName
	endif
	if HasFlag(flags, k_EPersonaChangeStatus)
		names.insert("Status") // GetFriendPersonaState ?
	endif
	if HasFlag(flags, k_EPersonaChangeComeOnline)
		names.insert("ComeOnline") // GetFriendPersonaState
	endif
	if HasFlag(flags, k_EPersonaChangeGoneOffline)
		names.insert("GoneOffline") // GetFriendPersonaState
	endif
	if HasFlag(flags, k_EPersonaChangeGamePlayed)
		names.insert("GamePlayed") // GetFriendGamePlayedJSON
	endif
	if HasFlag(flags, k_EPersonaChangeGameServer)
		names.insert("GameServer")
	endif
	if HasFlag(flags, k_EPersonaChangeAvatar)
		names.insert("Avatar") // Use HasAvatarImageLoaded, GetAvatarImageLoadedUser, and GetFriendAvatar.
	endif
	if HasFlag(flags, k_EPersonaChangeJoinedSource)
		names.insert("JoinedSource")
	endif
	if HasFlag(flags, k_EPersonaChangeLeftSource)
		names.insert("LeftSource")
	endif
	if HasFlag(flags, k_EPersonaChangeRelationshipChanged)
		names.insert("RelationshipChanged")
	endif
	if HasFlag(flags, k_EPersonaChangeNameFirstSet)
		names.insert("NameFirstSet") // ??
	endif
	if HasFlag(flags, k_EPersonaChangeFacebookInfo)
		names.insert("FacebookInfo") // No way to get this information?
	endif
	if HasFlag(flags, k_EPersonaChangeNickname)
		names.insert("Nickname") // GetPlayerNickname
	endif
	if HasFlag(flags, k_EPersonaChangeSteamLevel)
		names.insert("SteamLevel") // GetFriendSteamLevel
	endif
	result as string
	x as integer
	for x = 0 to names.length
		if x > 0
			result = result + ", "
		endif
		result = result + names[x]
	next
EndFunction result

Function GetFriendPersonaStateText(personaState as integer)
	select personaState
		case k_EPersonaStateOffline
			ExitFunction "Offline"
		endcase
		case k_EPersonaStateOnline
			ExitFunction "Online"
		endcase
		case k_EPersonaStateBusy
			ExitFunction "Busy"
		endcase
		case k_EPersonaStateAway
			ExitFunction "Away"
		endcase
		case k_EPersonaStateSnooze
			ExitFunction "Snooze"
		endcase
		case k_EPersonaStateLookingToTrade
			ExitFunction "Looking to Trade"
		endcase
		case k_EPersonaStateLookingToPlay
			ExitFunction "Looking to Play"
		endcase
	endselect	
EndFunction "Unknown"
