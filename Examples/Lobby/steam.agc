#option_explicit

global server as SteamServerInfo
// A type to hold all of the Steam server information used.
// Your game should probably keep track of things using a similar method.
Type SteamServerInfo
	needToFindLobbies as integer
	hLobby as integer
	lobbyIndex as integer
EndType
server.needToFindLobbies = 1
server.lobbyIndex = -1 // An invalid lobby index.

global dict as KeyValuePair[]
Type KeyValuePair
	key as string
	value as string
EndType

#constant LOBBY_LIST_X		10
#constant LOBBY_LIST_Y		100
#constant LOBBY_LIST_WIDTH	200
#constant LOBBY_INFO_X		220
#constant LOBBY_INFO_Y		100
#constant LOBBY_INFO_WIDTH	794
#constant LIST_HEIGHT		440 // 20 lines

global lobbyListLabel as integer
global lobbyList as ScrollableTextArea
global lobbyInfoLabel as integer
global lobbyInfo as ScrollableTextArea
global memberListLabel as integer
global memberList as ScrollableTextArea
global chatRoomLabel as integer
global chatRoom as ScrollableTextArea
global chatBox as integer

#constant OWNER_COLOR		144, 255, 60, 255
#constant STATUS_COLOR		85, 215, 255, 255
#constant CHAT_COLOR		255, 255, 255, 255

#constant FIND_ALL_BUTTON		1
#constant FIND_AGK_BUTTON		2
#constant JOIN_BUTTON			3
#constant CREATE_BUTTON			4
#constant LEAVE_BUTTON			5
#constant ADD_DATA_BUTTON		6
#constant DELETE_DATA_BUTTON	7

global buttonText as string[6] = ["FIND ALL", "FIND AGK", "JOIN", "CREATE", "LEAVE", "ADD_DATA", "DELETE_DATA"]
x as integer
for x = 0 to buttonText.length
	CreateButton(x + 1, 752 + (x - 5) * 100, 40, ReplaceString(buttonText[x], "_", NEWLINE, -1))
next
SetButtonEnabled(FIND_ALL_BUTTON, 1)
SetButtonEnabled(FIND_AGK_BUTTON, 1)
SetButtonEnabled(CREATE_BUTTON, 1)

CreateLobbyUI()

// Dump current friend lobbies
AddStatus("Checking for friend lobbies.")
friendCount as integer
friendCount = Steam.GetFriendCount(k_EFriendFlagImmediate)
index as integer
for index = 0 to friendCount - 1
	friendGameInfo as FriendGameInfo_t
	hSteamIDFriend as integer
	hSteamIDFriend = Steam.GetFriendByIndex(index, k_EFriendFlagImmediate)
	friendGameInfo.fromJSON(Steam.GetFriendGamePlayedJSON(hSteamIDFriend))
	if friendGameInfo.InGame 
		if Steam.IsSteamIDValid(friendGameInfo.SteamIDLobby)
			AddStatus(Steam.GetFriendPersonaName(hSteamIDFriend) + " is in lobby handle " + str(friendGameInfo.SteamIDLobby))
		// friendGameInfo.m_steamIDLobby is a valid lobby, you can join it or use RequestLobbyData() get it's metadata
		else
			AddStatus(Steam.GetFriendPersonaName(hSteamIDFriend) + " is in-game, but not in a lobby.")
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
	hLobby as integer
	// Scrollable text.
	PerformVerticalScroll(statusArea)
	PerformVerticalScroll(lobbyList)
	PerformVerticalScroll(lobbyInfo)
	// Lobby list
	if GetRawMouseLeftPressed()
		mouseX as float
		mouseY as float
		mouseX = GetRawMouseX()
		mouseY = GetRawMouseY()
		if GetTextHitTest(lobbyList.textID, mouseX, mouseY)
			ClearScrollableTextArea(lobbyInfo)
			server.lobbyIndex = Trunc((mouseY - GetTextY(lobbyList.textID)) / GetTextSize(lobbyList.textID))
			HighlightLobbyIndex()
			hLobby = Steam.GetLobbyByIndex(server.lobbyIndex)
			AddStatus("Loading information for lobby " + str(server.lobbyIndex) + ", handle: " + str(hLobby))
			AddLineToScrollableTextArea(lobbyInfo, "Lobby " + str(server.lobbyIndex) + " has " + str(Steam.GetNumLobbyMembers(hLobby)) + " members out of a limit of " + str(Steam.GetLobbyMemberLimit(hLobby)) + ".")
			// NOTE: This should typically only ever be used for debugging purposes. Devs should already know lobby data keys.
			count as integer
			count = Steam.GetLobbyDataCount(hLobby)
			AddLineToScrollableTextArea(lobbyInfo, "Data Count: " + str(count))
			x as integer
			for x = 0 to count - 1
				kv as KeyValuePair
				// GetLobbyDataByIndex returns a key/value pair as JSON.
				kv.fromJson(Steam.GetLobbyDataByIndex(hLobby, x))
				AddLineToScrollableTextArea(lobbyInfo, kv.key + ": " + kv.value)
			next
			SetButtonEnabled(JOIN_BUTTON, 1)
		endif
	endif
	if GetVirtualButtonPressed(FIND_ALL_BUTTON)
		SetButtonEnabled(JOIN_BUTTON, 0)
		server.needToFindLobbies = 1
	endif
	if GetVirtualButtonPressed(FIND_AGK_BUTTON)
		SetButtonEnabled(JOIN_BUTTON, 0)
		// Search the world for AGK lobbies.
		Steam.AddRequestLobbyListDistanceFilter(k_ELobbyDistanceFilterWorldwide)
		Steam.AddRequestLobbyListStringFilter("name", "agk", k_ELobbyComparisonEqual)
		server.needToFindLobbies = 1
	endif
	if GetVirtualButtonPressed(JOIN_BUTTON)
		AddStatus("Joining lobby " + str(server.lobbyIndex))
		Steam.JoinLobby(Steam.GetLobbyByIndex(server.lobbyIndex))
	endif
	if GetVirtualButtonPressed(CREATE_BUTTON)
		AddStatus("Creating lobby")
		if Steam.CreateLobby(k_ELobbyTypePublic, 8) = 0
			AddStatus("Failed")
		endif
	endif
	if GetVirtualButtonPressed(LEAVE_BUTTON)
		LeaveLobby()
	endif
	if GetVirtualButtonPressed(ADD_DATA_BUTTON)
		AddStatus("Setting some lobby and member data...")
		Steam.SetLobbyData(server.hLobby, "lobby_data", "Here's some lobby data.")
		Steam.SetLobbyMemberData(server.hLobby, "member_data", "And here's some member data.")
	endif
	if GetVirtualButtonPressed(DELETE_DATA_BUTTON)
		AddStatus("Deleting lobby data")
		if Steam.DeleteLobbyData(server.hLobby, "lobby_data")
			AddStatus("... Deleted!")
		endif
	endif
	if GetEditBoxVisible(chatBox)
		if GetRawKeyPressed(KEY_ENTER)
			msg as string
			msg = GetEditBoxText(chatBox)
			if len(msg) > 0
				SetEditBoxText(chatBox, "")
				Steam.SendLobbyChatMessage(server.hLobby, msg)
			endif
			// Must call both Sync and SetEditBoxFocus to set focus back to the edit box.
			Sync()
			SetEditBoxFocus(chatBox, 1)
		endif
	endif
EndFunction

// Used to keep track of reported errors so they are only shown once.
global errorReported as integer[0]
#constant ERROR_LOBBYMATCH		0

global frame as integer
//
// Processes all asynchronous callbacks.
//
Function ProcessCallbacks()
	x as integer
	hSteamID as integer
	hLobby as integer
	//
	// Lobby Match-making
	//
	select Steam.GetLobbyMatchListCallbackState()
		case STATE_IDLE
			if server.needToFindLobbies
				AddStatus("Finding lobbies...")
				ClearScrollableTextArea(lobbyList)
				ClearScrollableTextArea(lobbyInfo)
				server.lobbyIndex = -1 // An invalid lobby index.
				Steam.RequestLobbyList()
			endif
		endcase
		case STATE_DONE
			server.needToFindLobbies = 0
			AddStatus("Matching lobbies: " + str(Steam.GetLobbyMatchListCount()))
			for x = 0 to Steam.GetLobbyMatchListCount() - 1
				hLobby = Steam.GetLobbyByIndex(x)
				name as string
				name = "Lobby " + str(x) + " (" + str(Steam.GetNumLobbyMembers(hLobby)) + "/" + str(Steam.GetLobbyMemberLimit(hLobby)) + ")"
				AddLineToScrollableTextArea(lobbyList, name)
			next
			HighlightLobbyIndex()
		endcase
		case STATE_SERVER_ERROR, STATE_CLIENT_ERROR
			if not errorReported[ERROR_LOBBYMATCH]
				errorReported[ERROR_LOBBYMATCH] = 1
				AddStatus("Error getting lobby match list.")
			endif
		endcase
	endselect
	select Steam.GetLobbyEnterCallbackState()
		case STATE_DONE // Only care about the DONE state.
			if Steam.GetLobbyEnterResponse() = k_EChatRoomEnterResponseSuccess
				// Store the joined lobby handle.
				server.hLobby = Steam.GetLobbyEnterID()
				AddStatus("Joined lobby. Handle: " + str(server.hLobby))
				SetChatRoomVisible(1)
				RefreshMemberList()
				if Steam.GetLobbyOwner(server.hLobby) = Steam.GetSteamID()
					Steam.SetLobbyData(server.hLobby, "name", "agk")
				endif
			else
				AddStatus("Failed to join lobby.  Response = " + str(Steam.GetLobbyEnterResponse()))
			endif
		endcase
	endselect
	frame as integer
	while Steam.HasLobbyDataUpdated()
		frame = 1
		AddStatus("Lobby data updated.  Lobby handle: " + str(Steam.GetLobbyDataUpdatedLobby()) + ", updated handle: " + str(Steam.GetLobbyDataUpdatedID()))
		// When GetLobbyDataUpdatedLobby matches GetLobbyDataUpdatedID, the lobby data updated,
		//   otherwise GetLobbyDataUpdatedID is a member and that member's data updated.
		if Steam.GetLobbyDataUpdatedLobby() = Steam.GetLobbyDataUpdatedID()
			AddStatus("Lobby data: " + Steam.GetLobbyDataJson(server.hLobby))
		else
			AddStatus("Member " + Steam.GetFriendPersonaName(Steam.GetLobbyDataUpdatedID()) + " member_data: '" + Steam.GetLobbyMemberData(server.hLobby, Steam.GetLobbyDataUpdatedID(), "member_data") + "'")
		endif
	endwhile
	// Show any new chat messages.
	while Steam.HasLobbyChatMessage()
		AddChatLine(CHAT_COLOR, Steam.GetFriendPersonaName(Steam.GetLobbyChatMessageUser()) + ": " + Steam.GetLobbyChatMessageText())
	endwhile
	// Process any new lobby updates.
	while Steam.HasLobbyChatUpdate()
		text as string
		text = Steam.GetFriendPersonaName(Steam.GetLobbyChatUpdateUserChanged())
		select Steam.GetLobbyChatUpdateUserState()
			case k_EChatMemberStateChangeEntered
				text = text + " entered the lobby."
			endcase
			case k_EChatMemberStateChangeLeft
				text = text + " left the lobby."
			endcase
			case k_EChatMemberStateChangeDisconnected
				text = text + " disconnected."
			endcase
			case k_EChatMemberStateChangeKicked
				text = text + " was kicked by " + Steam.GetFriendPersonaName(Steam.GetLobbyChatUpdateUserMakingChange()) + "."
			endcase
			case k_EChatMemberStateChangeBanned
				text = text + " was banned by " + Steam.GetFriendPersonaName(Steam.GetLobbyChatUpdateUserMakingChange()) + "."
			endcase
			case default
				text = text + " has unknown state: " + str(Steam.GetLobbyChatUpdateUserState())
			endcase
		endselect
		AddChatLine(STATUS_COLOR, text)
		RefreshMemberList()
	endwhile
EndFunction

Function AddChatLine(red as integer, green as integer, blue as integer, alpha as integer, text as string)
	timeStamp as string
	timeStamp = GetDateFromUnix(GetUnixTime()) + ": "
	start as integer
	start = Len(GetTextString(chatRoom.textID) + timeStamp)
	AddLineToScrollableTextArea(chatRoom, timeStamp + text)
	x as integer
	for x = start to Len(GetTextString(chatRoom.textID)) - 1
		SetTextCharColor(chatRoom.textID, x, red, green, blue, alpha)
	next
EndFunction

Function RefreshMemberList()
	ClearScrollableTextArea(memberList)
	SetTextColor(memberList.textID, 255, 255, 255, 255)
	// Can only show owner and members after joining.
	hOwner as integer
	hOwner = Steam.GetLobbyOwner(server.hLobby)
	AddLineToScrollableTextArea(memberList, Steam.GetFriendPersonaName(hOwner))
	x as integer
	for x = 0 to Len(GetTextString(memberList.textID)) - 1
		SetTextCharColor(memberList.textID, x, OWNER_COLOR)
	next
	for x = 0 to Steam.GetNumLobbyMembers(server.hLobby) - 1
		hSteamID as integer
		hSteamID = Steam.GetLobbyMemberByIndex(server.hLobby, x)
		if hSteamID = hOwner
			continue
		endif
		AddLineToScrollableTextArea(memberList, Steam.GetFriendPersonaName(hSteamID))
	next
EndFunction

Function LeaveLobby()
	AddStatus("Leaving  lobby.")
	SetChatRoomVisible(0)
	Steam.LeaveLobby(server.hLobby)
	server.hLobby = 0
EndFunction

Function HighlightLobbyIndex()
	SetTextColor(lobbyList.textID, 255, 255, 255, 255)
	if server.lobbyIndex = -1
		ExitFunction
	endif
	firstPos as integer
	lastPos as integer
	text as string
	text = GetTextString(lobbyList.textID)
	index as integer
	for index = 0 to server.lobbyIndex
		firstPos = lastPos
		lastPos = FindString(text, NEWLINE, 1, firstPos + 1)
		if lastPos = 0
			ExitFunction
		endif
	next
	for index = firstPos to lastPos - 1
		SetTextCharColor(lobbyList.textID, index, OWNER_COLOR)
	next
EndFunction

//~ Type UIInfo
//~ EndType
//~ global ui as UIInfo

Function CreateLobbyUI()
	// Lobby list.
	lobbyListLabel = CreateTextEx(LOBBY_LIST_X, LOBBY_LIST_Y - 20, "Lobbies (Scrollable, Clickable)")
	lobbyList = CreateScrollableTextArea(LOBBY_LIST_X, LOBBY_LIST_Y, LOBBY_LIST_WIDTH, LIST_HEIGHT)
	lobbyInfoLabel = CreateTextEx(LOBBY_INFO_X, LOBBY_INFO_Y - 20, "Lobby Info (Scrollable)")
	lobbyInfo = CreateScrollableTextArea(LOBBY_INFO_X, LOBBY_INFO_Y, LOBBY_INFO_WIDTH, LIST_HEIGHT)
	// Chat room
	memberListLabel = CreateTextEx(LOBBY_LIST_X, LOBBY_LIST_Y - 20, "Members (Scrollable)")
	memberList = CreateScrollableTextArea(LOBBY_LIST_X, LOBBY_LIST_Y, LOBBY_LIST_WIDTH, LIST_HEIGHT)
	chatRoomLabel = CreateTextEx(LOBBY_INFO_X, LOBBY_INFO_Y - 20, "Chat Room (Scrollable)")
	chatRoom = CreateScrollableTextArea(LOBBY_INFO_X, LOBBY_INFO_Y, LOBBY_INFO_WIDTH, LIST_HEIGHT)
	chatBox = CreateEditBox()
	SetEditBoxSize(chatbox, LOBBY_INFO_WIDTH, 20)
	SetEditBoxTextSize(chatbox, 20)
	SetEditBoxPosition(chatBox, LOBBY_INFO_X, LOBBY_INFO_Y + LIST_HEIGHT)
	SetChatRoomVisible(0)
EndFunction

Function SetChatRoomVisible(visible as integer)
	SetTextVisible(lobbyListLabel, not visible)
	SetScrollableTextAreaVisible(lobbyList, not visible)
	SetTextVisible(lobbyInfoLabel, not visible)
	SetScrollableTextAreaVisible(lobbyInfo, not visible)
	SetTextVisible(memberListLabel, visible)
	SetScrollableTextAreaVisible(memberList, visible)
	SetTextVisible(chatRoomLabel, visible)
	SetScrollableTextAreaVisible(chatRoom, visible)
	SetEditBoxVisible(chatBox, visible)
	SetButtonEnabled(FIND_ALL_BUTTON, not visible)
	SetButtonEnabled(FIND_AGK_BUTTON, not visible)
	SetButtonEnabled(JOIN_BUTTON, not visible and server.lobbyIndex >= 0)
	SetButtonEnabled(CREATE_BUTTON, not visible)
	SetButtonEnabled(LEAVE_BUTTON, visible)
	SetButtonEnabled(ADD_DATA_BUTTON, visible)
	SetButtonEnabled(DELETE_DATA_BUTTON, visible)
	if visible
		ClearScrollableTextArea(chatRoom)
		SetTextColor(chatRoom.textID, 255, 255, 255, 255)
		SetEditBoxFocus(chatBox, 1)
	endif
EndFunction
