#option_explicit

#constant GAME_SERVER_PORT	12345

#constant GAME_SERVER_COLOR	255, 255, 0, 255

Function HostGameServer()
	server.networkID = HostNetwork("AGKSteamPluginTest", Steam.GetPersonaName(), GAME_SERVER_PORT)
	AddChatLine(GAME_SERVER_COLOR, "Hosting game server.  hLobby: " + str(server.hLobby) + ", IP: " + GetDeviceIP() + ", Port: " + str(GAME_SERVER_PORT))
	// TODO Use SteamGameServer or somehow get public IP.
	Steam.SetLobbyGameServer(server.hLobby, GetDeviceIP(), GAME_SERVER_PORT, 0)
EndFunction

Function ConnectGameServer(hLobby as integer, IP as string, Port as integer)
	AddChatLine(GAME_SERVER_COLOR, "Connecting to " + IP + ":" + str(Port) + "...")
	if not server.isLobbyOwner
		server.networkID = JoinNetwork(IP, Port, Steam.GetPersonaName())
	endif
	memberCount as integer
	memberCount = Steam.GetNumLobbyMembers(hLobby)
	timeout as float
	timeout = Timer() + 3 // 3 second timeout.
	clientCount as integer
	clientCount = 0
	do
		if clientCount <> GetNetworkNumClients(server.networkID)
			clientCount = GetNetworkNumClients(server.networkID)
			AddChatLine(GAME_SERVER_COLOR, "Found " + str(clientCount) + " out of " + str(memberCount) + " clients.")
			if clientCount = memberCount
				AddChatLine(GAME_SERVER_COLOR, "Starting game session...")
				AddChatLine(GAME_SERVER_COLOR, "Press ESC to end session.")
				exit
			endif
		endif
		// Test for exit conditions.
		if Timer() > timeout
			AddChatLine(GAME_SERVER_COLOR, "Timed out while waiting for all players.")
			clientCount = 0
			exit
		endif
		if not IsNetworkActive(server.networkID)
			AddChatLine(GAME_SERVER_COLOR, "Network became inactive.")
			clientCount = 0
			exit
		endif
		if GetRawKeyPressed(KEY_ESCAPE) 
			AddChatLine(GAME_SERVER_COLOR, "User aborted.")
			clientCount = 0
			exit
		endif
		Sync()
		Steam.RunCallbacks()
	loop
	if clientCount = memberCount and IsNetworkActive(server.networkID)
		messageID as integer
		messageID = CreateNetworkMessage()
		AddNetworkMessageString(messageID, "Hello from " + Steam.GetPersonaName())
		// NOTE: Those who send network messages do not receive them.  See AGK docs.
		//~ AddChatLine(GAME_SERVER_COLOR, GetNetworkMessageString(messageID))
		SendNetworkMessage(server.networkID, 0, messageID)
		do
			do
				messageID = GetNetworkMessage(server.networkID)
				if not messageID
					exit
				endif
				AddChatLine(GAME_SERVER_COLOR, GetNetworkMessageString(messageID))
				DeleteNetworkMessage(messageID)
			loop
			Sync()
			Steam.RunCallbacks()
			if GetRawKeyPressed(KEY_ESCAPE)
				exit
			endif
		loop
	endif
	CloseNetwork(server.networkID)
	server.networkID = 0
	AddChatLine(GAME_SERVER_COLOR, "Disconnected.")
	SetButtonEnabled(CREATE_GAME_BUTTON, 1)
	// Sync again to clear key pressed.
	Sync()
EndFunction
