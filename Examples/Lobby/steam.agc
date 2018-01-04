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

#constant OWNER_COLOR		144, 255, 60, 255
#constant STATUS_COLOR		85, 165, 196, 255
#constant CHAT_COLOR		255, 255, 255, 255

#constant REFRESH_BUTTON	1
#constant JOIN_BUTTON		2
#constant CREATE_BUTTON		3
#constant LEAVE_BUTTON		4

global buttonText as string[3] = ["REFRESH", "JOIN", "CREATE", "LEAVE"]
x as integer
for x = 0 to buttonText.length
	CreateButton(x + 1, 752 + (x - 5) * 100, 40, ReplaceString(buttonText[x], "_", NEWLINE, -1))
next
SetButtonEnabled(REFRESH_BUTTON, 1)
SetButtonEnabled(CREATE_BUTTON, 1)

CreateLobbyUI()

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
			SetTextString(lobbyInfo.textID, "")
			server.lobbyIndex = Trunc((mouseY - GetTextY(lobbyList.textID)) / GetTextSize(lobbyList.textID))
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
	if GetVirtualButtonPressed(REFRESH_BUTTON)
		SetButtonEnabled(JOIN_BUTTON, 0)
		server.needToFindLobbies = 1
	endif
	if GetVirtualButtonPressed(JOIN_BUTTON)
		AddStatus("Joining lobby " + str(server.lobbyIndex))
		Steam.JoinLobby(Steam.GetLobbyByIndex(server.lobbyIndex))
	endif
	if GetVirtualButtonPressed(CREATE_BUTTON)
		AddStatus("Creating lobby")
		if Steam.CreateLobby(k_ELobbyTypePrivate, 4) = 0
			AddStatus("Failed")
		endif
	endif
	if GetVirtualButtonPressed(LEAVE_BUTTON)
		LeaveLobby()
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
				SetTextString(lobbyList.textID, "")
				SetTextString(lobbyInfo.textID, "")
				server.lobbyIndex = -1 // An invalid lobby index.
				Steam.RequestLobbyList()
			endif
		endcase
		case STATE_DONE
			server.needToFindLobbies = 0
			AddStatus("-------------------Lobby Match List-------------------")
			AddStatus("Lobbies Found: " + str(Steam.GetLobbyMatchListCount()))
			for x = 0 to Steam.GetLobbyMatchListCount() - 1
				hLobby = Steam.GetLobbyByIndex(x)
				name as string
				name = "Lobby " + str(x) + " (" + str(Steam.GetNumLobbyMembers(hLobby)) + "/" + str(Steam.GetLobbyMemberLimit(hLobby)) + ")"
				AddLineToScrollableTextArea(lobbyList, name)
			next
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
				//~ Steam.SendLobbyChatMessage(server.hLobby, "Test 1")
				//~ Steam.SendLobbyChatMessage(server.hLobby, "Test 2")
				//~ AddStatus("Setting some lobby and member data...")
				//~ Steam.SetLobbyData(server.hLobby, "lobby_data", "Here's some lobby data.")
				//~ Steam.SetLobbyMemberData(server.hLobby, "member_data", "And here's some member data.")
				//~ server.deleteLobbyData = 1 // flag to demo lobby data removal later.
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
			//~ lobbyData as string
			//~ lobbyData = Steam.GetLobbyData(server.hLobby, "lobby_data")
			//~ AddStatus("    Lobby data: " + lobbyData)
			AddStatus("    Lobby data: " + Steam.GetLobbyDataJson(server.hLobby))
			//~ if lobbyData <> ""
				//~ server.deleteLobbyData = 1
				//~ if server.deleteLobbyData
					//~ server.deleteLobbyData = 0
					//~ AddStatus("    Deleting lobby data")
					//~ if Steam.DeleteLobbyData(server.hLobby, "lobby_data")
						//~ AddStatus("    Deleted")
					//~ endif
				//~ endif
			//~ endif
		else
			AddStatus("    Member " + Steam.GetFriendPersonaName(Steam.GetLobbyDataUpdatedID()) + " data: " + Steam.GetLobbyMemberData(server.hLobby, Steam.GetLobbyDataUpdatedID(), "member_data"))
		endif
	endwhile
	//~ if frame
		//~ AddStatus("Frame")
	//~ endif
	// Don't delete or set lobby data in the HasLobbyDataUpdated loop!
	//~ if server.deleteLobbyData
		//~ server.deleteLobbyData = 0
		//~ AddStatus("    Deleting lobby data")
		//~ if Steam.DeleteLobbyData(server.hLobby, "lobby_data")
			//~ AddStatus("    Deleted")
		//~ endif
	//~ endif
	//~ AddStatus("    Lobby data: " + Steam.GetLobbyData(server.hLobby, "lobby_data"))
	// Show any new chat messages.
	while Steam.HasLobbyChatMessage()
		AddChatLine(CHAT_COLOR, Steam.GetFriendPersonaName(Steam.GetLobbyChatMessageUser()) + ": " + Steam.GetLobbyChatMessageText())
	endwhile
	// Process any new lobby updates.
	while Steam.HasLobbyChatUpdate()
		text as string
		text = "Lobby update: " + Steam.GetFriendPersonaName(Steam.GetLobbyChatUpdateUserChanged())
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
	SetTextString(memberList.textID, "")
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
	SetChatRoomVisible(0)
EndFunction

Function SetChatRoomVisible(visible as integer)
	SetTextVisible(lobbyListLabel, visible ~~ 1)
	SetScrollableTextAreaVisible(lobbyList, visible ~~ 1)
	SetTextVisible(lobbyInfoLabel, visible ~~ 1)
	SetScrollableTextAreaVisible(lobbyInfo, visible ~~ 1)
	SetTextVisible(memberListLabel, visible)
	SetScrollableTextAreaVisible(memberList, visible)
	SetTextVisible(chatRoomLabel, visible)
	SetScrollableTextAreaVisible(chatRoom, visible)
	if visible
		SetTextString(chatRoom.textID, "")
		SetTextColor(chatRoom.textID, 255, 255, 255, 255)
		SetButtonEnabled(JOIN_BUTTON, 0)
		SetButtonEnabled(CREATE_BUTTON, 0)
		SetButtonEnabled(LEAVE_BUTTON, 1)
	else
		SetButtonEnabled(JOIN_BUTTON, server.lobbyIndex >= 0)
		SetButtonEnabled(CREATE_BUTTON, 1)
		SetButtonEnabled(LEAVE_BUTTON, 0)
	endif
EndFunction
