// Project: Achievements and Stats 
// Created: 2017-12-14
// Copyright 2017 Adam Biser
#option_explicit

#constant OFFLINE_COLOR		127, 127, 127, 256
#constant ONLINE_COLOR		85, 165, 196, 256
#constant INGAME_COLOR		144, 186, 60, 256

// Holds everything related to the Steam server data.
global server as SteamServerInfo

// A type to hold all of the Steam server information used.
Type SteamServerInfo
	selectedGroup as integer
	friendGroups as string[]
	groupFriendIDs as integer[]
EndType

// Additional UI for this demo.
//~ #constant RESET_BUTTON			1
//~ #constant ADD_WINS_BUTTON		2
//~ #constant ADD_LOSSES_BUTTON		3
//~ #constant ADD_DISTANCE_BUTTON	4
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
#constant LIST_HEIGHT		400 // 20 lines

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
// Check and handle virtual buttons.
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
		if GetSpriteHitTest(ui.groupListSpriteID, mouseX, mouseY)
			UpdateGroupListUI(-delta)
		elseif GetSpriteHitTest(ui.friendListSpriteID, mouseX, mouseY)
			UpdateFriendListUI(-delta)
		endif
	endif
	if GetRawMouseLeftPressed()
		x as integer
		for x = 0 to ui.groupListTextIDs.length
			if GetTextVisible(ui.groupListTextIDs[x])
				if GetTextHitTest(ui.groupListTextIDs[x], mouseX, mouseY)
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
	while Steam.HasPersonaStateChanged()
		hSteamID = Steam.GetPersonaStateChangedUser()
		flags = Steam.GetPersonaStateChangedFlags()
		AddStatus("Persona State Change: " + Steam.GetFriendPersonaName(hSteamID) + ", flags: " + GetPersonaStateChangeFlagText(flags))
		UpdateFriendStatus(hSteamID)
	endwhile
	while Steam.HasAvatarImageLoaded()
		hSteamID = Steam.GetAvatarImageLoadedUser()
		// Update the avatar if shown.
		index as integer
		index = GetFriendSteamIDIndex(hSteamID)
		if index = -1
			ExitFunction
		endif
		uiindex as integer
		uiindex = index - ui.topFriendIndex
		if uiindex >= 0 and uiindex <= ui.friendListTextIDs.length
			LoadUserAvatar(hSteamID, ui.friendListSpriteIDs[uiindex])
		endif
	endwhile
EndFunction

Type UIInfo
	groupListSpriteID as integer
	groupListTextIDs as integer[19]
	friendListSpriteID as integer
	friendListTextIDs as integer[9]
	friendListSpriteIDs as integer[9]
	topGroupIndex as integer
	topFriendIndex as integer
EndType
global ui as UIInfo

Function CreateFriendListUI()
	x as integer
	y as integer
	// Scrollable list of groups on the left.
	CreateTextEx(GROUP_LIST_X, GROUP_LIST_Y - 20, "Groups (Scrollable)")
	ui.groupListSpriteID = CreateSprite(CreateImageColor(32, 32, 32, 255))
	SetSpritePosition(ui.groupListSpriteID, GROUP_LIST_X, GROUP_LIST_Y)
	SetSpriteSize(ui.groupListSpriteID, GROUP_LIST_WIDTH, LIST_HEIGHT)
	for x = 0 to ui.groupListTextIDs.length
		y = GROUP_LIST_Y + x * 20
		ui.groupListTextIDs[x] = CreateTextEx(GROUP_LIST_X, y, "Group")
	next
	// Scrollable List of friends on the right.
	CreateTextEx(FRIEND_LIST_X, FRIEND_LIST_Y - 20, "Friends (Scrollable)")
	ui.friendListSpriteID = CreateSprite(CreateImageColor(32, 32, 32, 255))
	SetSpritePosition(ui.friendListSpriteID, FRIEND_LIST_X, FRIEND_LIST_Y)
	SetSpriteSize(ui.friendListSpriteID, FRIEND_LIST_WIDTH, LIST_HEIGHT)
	for x = 0 to ui.friendListTextIDs.length
		y = FRIEND_LIST_Y + x * 40
		ui.friendListTextIDs[x] = CreateTextEx(FRIEND_LIST_X + 40, y + 10, "Friend")
		ui.friendListSpriteIDs[x] = CreateSprite(CreateImageColor(0, 0, 0, 0))
		SetSpritePosition(ui.friendListSpriteIDs[x], FRIEND_LIST_X + 2, y + 4)
		SetSpriteSize(ui.friendListSpriteIDs[x], 32, 32)
	next
	// Load lists.
	RefreshFriendGroupList()
EndFunction

Function RefreshFriendGroupList()
	x as integer
	for x = 0 to ui.groupListTextIDs.length
		SetTextVisible(ui.groupListTextIDs[x], 0)
	next
	// Reload the list of friend group names.
	server.friendGroups.length = -1
	// Make index 0 represent the "All friends" list.
	server.friendGroups.insert("All Friends (" + str(Steam.GetFriendCount(k_EFriendFlagImmediate)) + ")")
	groupCount as integer
	groupCount = Steam.GetFriendsGroupCount()
	for x = 0 to groupCount - 1
		groupID as integer
		groupID = Steam.GetFriendsGroupIDByIndex(x)
		server.friendGroups.insert(Steam.GetFriendsGroupName(groupID) + " (" + str(Steam.GetFriendsGroupMembersCount(groupID)) + ")")
	next
	UpdateGroupListUI(-ui.topGroupIndex) // Scroll to top of group list
	SelectFriendGroup(0) // Show all friends.
EndFunction

Function UpdateGroupListUI(delta as integer)
	inc ui.topGroupIndex, delta
	if ui.topGroupIndex < 0
		ui.topGroupIndex = 0
	elseif server.friendGroups.length <= ui.groupListTextIDs.length
		ui.topGroupIndex = 0
	elseif ui.topGroupIndex + (ui.groupListTextIDs.length) > server.friendGroups.length
		ui.topGroupIndex = server.friendGroups.length - ui.groupListTextIDs.length
	endif
	x as integer
	for x = ui.topGroupIndex to ui.topGroupIndex + ui.groupListTextIDs.length
		if x > server.friendGroups.length
			exit
		endif
		SetTextString(ui.groupListTextIDs[x - ui.topGroupIndex], server.friendGroups[x])
		SetTextVisible(ui.groupListTextIDs[x - ui.topGroupIndex], 1)
	next
EndFunction

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
	server.groupFriendIDs.fromJSON(friendListJSON)
	UpdateFriendListUI(-ui.topFriendIndex) // Scroll to top of friend list.
EndFunction

Function UpdateFriendListUI(delta as integer)
	inc ui.topFriendIndex, delta
	if ui.topFriendIndex < 0
		ui.topFriendIndex = 0
	elseif server.groupFriendIDs.length <= ui.friendListTextIDs.length
		ui.topFriendIndex = 0
	elseif ui.topFriendIndex + (ui.friendListTextIDs.length) > server.groupFriendIDs.length
		ui.topFriendIndex = server.groupFriendIDs.length - ui.friendListTextIDs.length
	endif
	x as integer
	for x = 0 to ui.friendListTextIDs.length
		friendIndex as integer
		friendIndex = x + ui.topFriendIndex
		if friendIndex > server.groupFriendIDs.length
			SetTextVisible(ui.friendListTextIDs[x], 0)
			SetSpriteVisible(ui.friendListSpriteIDs[x], 0)
		else
			UpdateFriendStatus(server.groupFriendIDs[friendIndex])
			SetTextVisible(ui.friendListTextIDs[x], 1)
			SetSpriteVisible(ui.friendListSpriteIDs[x], 1)
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
	if uiindex >= 0 and uiindex <= ui.friendListTextIDs.length
		name as string
		state as integer
		gameinfo as FriendGameInfo_t
		name = Steam.GetFriendPersonaName(hSteamID)
		state = Steam.GetFriendPersonaState(hSteamID)
		gameinfo.fromJSON(Steam.GetFriendGamePlayedJSON(hSteamID))
		text as string
		text = name
		if state = k_EPersonaStateOffline
			SetTextColor(ui.friendListTextIDs[uiindex], OFFLINE_COLOR)
		elseif gameinfo.InGame
			SetTextColor(ui.friendListTextIDs[uiindex], INGAME_COLOR)
			text = text + " [In-Game: " + Steam.GetAppName(gameinfo.GameAppID) + "]"
		else
			SetTextColor(ui.friendListTextIDs[uiindex], ONLINE_COLOR)
			text = text + " [" + GetFriendPersonaStateText(state) + "]"
		endif
		SetTextString(ui.friendListTextIDs[uiindex], text)
		// TODO: It would be better to cache the friend avatars.
		LoadUserAvatar(hSteamID, ui.friendListSpriteIDs[uiindex])
	endif	
EndFunction

Function GetFriendSteamIDIndex(hSteamID as integer)
	x as integer
	for x = 0 to server.groupFriendIDs.length
		if server.groupFriendIDs[x] = hSteamID
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

Function LoadUserAvatar(hSteamID as integer, spriteID as integer)
	avatarHandle as integer
	avatarHandle = Steam.GetFriendAvatar(hSteamID, AVATAR_SMALL)
	if avatarHandle > 0
		Steam.LoadImageFromHandle(GetSpriteImageID(spriteID), avatarHandle)
	endif
EndFunction
