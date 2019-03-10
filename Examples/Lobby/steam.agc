#option_explicit

global server as SteamServerInfo
// A type to hold all of the Steam server information used.
// Your game should probably keep track of things using a similar method.
Type SteamServerInfo
	lobbyListCallResult as integer
	createLobbyCallResult as integer
	joinLobbyCallResult as integer
	hLobby as integer
	lobbyIndex as integer
	lobbies as integer[]
	isLobbyOwner as integer
	networkID as integer
EndType

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
#constant CREATE_GAME_BUTTON	8

global buttonText as string[7] = ["FIND ALL", "FIND AGK", "JOIN_LOBBY", "CREATE_LOBBY", "LEAVE", "ADD_DATA", "DELETE_DATA", "CREATE_SERVER"]
x as integer
for x = 0 to buttonText.length
	CreateButton(x + 1, 752 + (x - 5) * 100, 40, ReplaceString(buttonText[x], "_", NEWLINE, -1))
next
SetButtonEnabled(FIND_ALL_BUTTON, 1)
SetButtonEnabled(FIND_AGK_BUTTON, 1)
SetButtonEnabled(CREATE_BUTTON, 1)

CreateLobbyUI()

// Dump favorite game information.
AddStatus("Favorite games:")
AddStatus("AddFavoriteGame result: " + str(Steam.AddFavoriteGame(480, GetDeviceIP(), 12345, 12345, FavoriteFlagFavorite)))
for x = 0 to Steam.GetFavoriteGameCount() - 1
	AddStatus(Steam.GetFavoriteGameJSON(x))
next
AddStatus("RemoveFavoriteGame result: " + str(Steam.RemoveFavoriteGame(480, GetDeviceIP(), 12345, 12345, FavoriteFlagFavorite)))
AddStatus("New GetFavoriteGameCount: " + str(Steam.GetFavoriteGameCount()))

// Dump current friend lobbies
AddStatus("Checking for friend lobbies.")
friendCount as integer
friendCount = Steam.GetFriendCount(EFriendFlagImmediate)
index as integer
for index = 0 to friendCount - 1
	friendGameInfo as FriendGameInfo_t
	hSteamIDFriend as integer
	hSteamIDFriend = Steam.GetFriendByIndex(index, EFriendFlagImmediate)
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

// Check to see if this was started as a result of a game invitation.  If so, join the lobby to which the user was invited.
commandline as string[]
commandline.fromJSON(Steam.GetCommandLineArgsJSON())
for x = 0 to commandline.length
	if commandline[x] = "+connect_lobby" and x < commandline.length
		AddStatus("Received +connect_lobby commandline argument.")
		hLobby as integer
		hLobby = Steam.GetHandleFromSteamID64(commandline[x + 1])
		if hLobby
			AddStatus("Joining lobby: " + commandline[x + 1])
			server.joinLobbyCallResult = Steam.JoinLobby(hLobby)
		endif
	endif
next

FindLobbies()

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
	PerformVerticalScroll(memberList)
	PerformVerticalScroll(chatroom)
	// Lobby list
	if GetRawMouseLeftPressed()
		mouseX as float
		mouseY as float
		mouseX = GetRawMouseX()
		mouseY = GetRawMouseY()
		if GetTextHitTest(lobbyList.textID, mouseX, mouseY) and GetTextVisible(lobbyList.textID)
			ClearScrollableTextArea(lobbyInfo)
			server.lobbyIndex = Trunc((mouseY - GetTextY(lobbyList.textID)) / GetTextSize(lobbyList.textID))
			HighlightLobbyIndex()
			hLobby = server.lobbies[server.lobbyIndex] // Steam.GetLobbyByIndex(server.lobbyListCallResult, server.lobbyIndex)
			AddStatus("Loading information for lobby " + str(server.lobbyIndex) + ", handle: " + str(hLobby))
			AddLineToScrollableTextArea(lobbyInfo, "Lobby " + str(server.lobbyIndex) + " has " + str(Steam.GetNumLobbyMembers(hLobby)) + " members out of a limit of " + str(Steam.GetLobbyMemberLimit(hLobby)) + ".")
			// NOTE: This should typically only ever be used for debugging purposes. Devs should already know lobby data keys.
			AddStatus(Steam.GetLobbyDataJSON(hLobby))
			// GetLobbyDataJSON returns an array of key/value pairs as JSON.
			lobbyData as KeyValuePair[]
			lobbyData.fromjson(Steam.GetLobbyDataJSON(hLobby))
			AddLineToScrollableTextArea(lobbyInfo, "Data Count: " + str(lobbyData.length + 1)) // Array length is really ubounds in AGK!
			x as integer
			for x = 0 to lobbyData.length
				AddLineToScrollableTextArea(lobbyInfo, lobbyData[x].key + ": " + lobbyData[x].value)
			next
			SetButtonEnabled(JOIN_BUTTON, 1)
		endif
	endif
	if GetVirtualButtonPressed(FIND_ALL_BUTTON)
		SetButtonEnabled(JOIN_BUTTON, 0)
		FindLobbies()
	endif
	if GetVirtualButtonPressed(FIND_AGK_BUTTON)
		SetButtonEnabled(JOIN_BUTTON, 0)
		// Search the world for AGK lobbies.
		Steam.AddRequestLobbyListDistanceFilter(ELobbyDistanceFilterWorldwide)
		Steam.AddRequestLobbyListStringFilter("name", "agk", ELobbyComparisonEqual)
		FindLobbies()
	endif
	if GetVirtualButtonPressed(JOIN_BUTTON)
		AddStatus("Joining lobby " + str(server.lobbyIndex))
		server.joinLobbyCallResult = Steam.JoinLobby(server.lobbies[server.lobbyIndex]) // Steam.GetLobbyByIndex(server.lobbyListCallResult, server.lobbyIndex))
		SetButtonEnabled(JOIN_BUTTON, 1)
	endif
	if GetVirtualButtonPressed(CREATE_BUTTON)
		server.createLobbyCallResult = Steam.CreateLobby(ELobbyTypePublic, 8)
		AddStatus("Creating lobby.  Call Result: " + str(server.createLobbyCallResult))
	endif
	if GetVirtualButtonPressed(LEAVE_BUTTON)
		LeaveLobby()
	endif
	if GetVirtualButtonPressed(ADD_DATA_BUTTON)
		AddStatus("Setting some lobby and member data...")
		if server.isLobbyOwner
			Steam.SetLobbyData(server.hLobby, "lobby_data", "Here's some lobby data.")
		endif
		Steam.SetLobbyMemberData(server.hLobby, "member_data", "Here's some member data.")
	endif
	if GetVirtualButtonPressed(DELETE_DATA_BUTTON)
		AddStatus("Deleting lobby data")
		if server.isLobbyOwner
			if Steam.DeleteLobbyData(server.hLobby, "lobby_data")
				AddStatus("... Deleted!")
			endif
		endif
		// There is no DeleteLobbyMemberData!
		Steam.SetLobbyMemberData(server.hLobby, "member_data", "")
	endif
	if GetVirtualButtonPressed(CREATE_GAME_BUTTON)
		HostGameServer()
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

Type LobbyCreated_t
	hLobby as integer
	Result as integer
EndType
Type LobbyDataUpdate_t
	hLobby as integer
	hMember as integer
	Success as integer
EndType
Type LobbyEnter_t
	hLobby as integer
	Locked as integer
	ChatRoomEnterResponse as integer
	ChatPermissions as integer // unused, always 0.
EndType

//
// Processes all asynchronous callbacks.
//
Function ProcessCallbacks()
	x as integer
	hSteamID as integer
	hLobby as integer
	gameserver as LobbyGameCreated_t
	//
	// Lobby Match-making
	//
	result as integer
	if server.lobbyListCallResult
		result = Steam.GetCallResultCode(server.lobbyListCallResult)
		if result
			AddStatus("RequestLobbyList call result code: " + str(result))
			AddStatus("JSON: " + Steam.GetCallResultJSON(server.lobbyListCallResult))
			if result = EResultOk
				server.lobbies.fromjson(Steam.GetCallResultJSON(server.lobbyListCallResult))
				for x = 0 to server.lobbies.length
					hLobby = server.lobbies[x]
					name as string
					name = "Lobby " + str(x) + " (" + str(Steam.GetNumLobbyMembers(hLobby)) + "/" + str(Steam.GetLobbyMemberLimit(hLobby)) + ")"
					AddLineToScrollableTextArea(lobbyList, name)
				next
				HighlightLobbyIndex()
			else
				AddStatus("Error getting lobby match list: " + str(result))
			endif
			// Done with the call result.  Delete it.
			Steam.DeleteCallResult(server.lobbyListCallResult)
			server.lobbyListCallResult = 0
		endif
	endif
	createdLobby as LobbyCreated_t
	if server.createLobbyCallResult
		result = Steam.GetCallResultCode(server.createLobbyCallResult)
		if result
			AddStatus("CreateLobby call result code: " + str(result))
			AddStatus("JSON: " + Steam.GetCallResultJSON(server.createLobbyCallResult))
			if result = EResultOk
				createdLobby.fromjson(Steam.GetCallResultJSON(server.createLobbyCallResult))
				AddStatus("Lobby created.  Handle: " + str(createdLobby.hLobby))
			else
				AddStatus("Error creating lobby: " + str(result))
			endif
			// Done with the call result.  Delete it.
			Steam.DeleteCallResult(server.createLobbyCallResult)
			server.createLobbyCallResult = 0
		endif
	endif
	// Check the JoinLobby CallResult and report results.
	// This demo uses HasLobbyEnterResponse since it will report for both CreateLobby and JoinLobby calls.
	joinedLobby as LobbyEnter_t
	if server.joinLobbyCallResult
		result = Steam.GetCallResultCode(server.joinLobbyCallResult)
		if result
			AddStatus("JoinLobby call result code: " + str(result))
			AddStatus("JSON: " + Steam.GetCallResultJSON(server.joinLobbyCallResult))
			if result = EResultOk
				joinedLobby.fromjson(Steam.GetCallResultJSON(server.joinLobbyCallResult))
				AddStatus("Lobby joined.  Handle: " + str(joinedLobby.hLobby))
			else
				AddStatus("Error joining lobby: " + str(result))
			endif
			// Done with the call result.  Delete it.
			Steam.DeleteCallResult(server.joinLobbyCallResult)
			server.joinLobbyCallResult = 0
		endif
	endif
	// Check the general JoinLobby callback.
	while Steam.HasLobbyEnterResponse()
		joinedLobby.fromjson(Steam.GetLobbyEnterResponseJSON())
		if joinedLobby.ChatRoomEnterResponse = EChatRoomEnterResponseSuccess
			// Store the joined lobby handle.
			server.hLobby = joinedLobby.hLobby
			AddStatus("Joined lobby. Handle: " + str(server.hLobby))
			SetChatRoomVisible(1)
			RefreshMemberList()
			// If the lobby owner is the current user, set some lobby data.
			if server.isLobbyOwner
				Steam.SetLobbyData(server.hLobby, "name", "agk")
			endif
			AddStatus("GameServer: " + Steam.GetLobbyGameServerJSON(server.hLobby))
		else
			AddStatus("Failed to join lobby.  Response = " + str(joinedLobby.ChatRoomEnterResponse))
		endif
	endwhile
	lobbyDataUpdate as LobbyDataUpdate_t
	while Steam.HasLobbyDataUpdateResponse()
		frame = 1
		lobbyDataUpdate.fromjson(Steam.GetLobbyDataUpdateResponseJSON())
		AddStatus("Lobby data updated.  JSON: " + Steam.GetLobbyDataUpdateResponseJSON())
		// When hMember matches hLobby, the lobby data updated,
		//   otherwise hMember is a member and that member's data updated.
		if lobbyDataUpdate.hLobby = lobbyDataUpdate.hMember
			AddStatus("Lobby data: " + Steam.GetLobbyDataJson(server.hLobby))
		else
			AddStatus("Member " + Steam.GetFriendPersonaName(lobbyDataUpdate.hMember) + " member_data: '" + Steam.GetLobbyMemberData(server.hLobby, lobbyDataUpdate.hMember, "member_data") + "'")
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
			case EChatMemberStateChangeEntered
				text = text + " entered the lobby."
			endcase
			case EChatMemberStateChangeLeft
				text = text + " left the lobby."
			endcase
			case EChatMemberStateChangeDisconnected
				text = text + " disconnected."
			endcase
			case EChatMemberStateChangeKicked
				text = text + " was kicked by " + Steam.GetFriendPersonaName(Steam.GetLobbyChatUpdateUserMakingChange()) + "."
			endcase
			case EChatMemberStateChangeBanned
				text = text + " was banned by " + Steam.GetFriendPersonaName(Steam.GetLobbyChatUpdateUserMakingChange()) + "."
			endcase
			case default
				text = text + " has unknown state: " + str(Steam.GetLobbyChatUpdateUserState())
			endcase
		endselect
		text = text + "  Lobby handle = " + str(Steam.GetLobbyChatUpdateLobby())
		AddChatLine(STATUS_COLOR, text)
		RefreshMemberList()
	endwhile
	// Process game server creation.
	if Steam.HasLobbyGameCreated()
		gameserver.fromJSON(Steam.GetLobbyGameCreatedJSON())
		AddStatus("Game server created: " + Steam.GetLobbyGameCreatedJSON())
		if gameserver.IP <> "0.0.0.0"
			ConnectGameServer(gameserver)
		endif
	endif
	// Process lobby invitations
	if Steam.HasGameLobbyJoinRequest()
		server.lobbyIndex = -1
		hLobby = Steam.GetGameLobbyJoinRequestedLobby()
		AddStatus("Received invitation to join lobby " + str(hLobby))
		AddStatus("Joining lobby")
		server.joinLobbyCallResult = Steam.JoinLobby(hLobby)
	endif
EndFunction

Function FindLobbies()
	// If we have existing results, clear them.  We're only dealing with one set at a time in this example.
	if server.lobbyListCallResult
		Steam.DeleteCallResult(server.lobbyListCallResult)
	endif
	ClearScrollableTextArea(lobbyList)
	ClearScrollableTextArea(lobbyInfo)
	server.lobbyIndex = -1 // An invalid lobby index.
	SetButtonEnabled(JOIN_BUTTON, 0)
	server.lobbyListCallResult = Steam.RequestLobbyList()
	AddStatus("Finding lobbies.  Call Result: " + str(server.lobbyListCallResult))
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
	server.isLobbyOwner = (hOwner = Steam.GetSteamID())
	if server.isLobbyOwner
		SetButtonEnabled(CREATE_GAME_BUTTON, 1)
	endif
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
	SetButtonEnabled(CREATE_GAME_BUTTON, 0)
	if visible
		ClearScrollableTextArea(chatRoom)
		SetTextColor(chatRoom.textID, 255, 255, 255, 255)
		SetEditBoxFocus(chatBox, 1)
	endif
EndFunction
