#option_explicit
////////////////////////////////////////////////////////////////////////////////
// NOTE:
// These constants come from the Steamworks SDK.
// However, the k_ prefix has been removed from the constant names.
// Prefixes used to indicate variable type are also removed. ie: un, cch
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Constants defined by the plugin.
//------------------------------------------------------------------------------

// Plugin Callback State Value
#constant STATE_SERVER_ERROR	-2	// A problem was reported by the Steam API,
#constant STATE_CLIENT_ERROR	-1	// There was a problem calling the command.
#constant STATE_IDLE			0	// The initial state of the callback. Safe to start callback commands while in this state.
#constant STATE_RUNNING			1	// Generally don't need to check for this state.  Callback state will transition to DONE or SERVER_ERROR.
#constant STATE_DONE			2	// Reported once as soon as the callback finishes.  The callback returns to IDLE state immediately.

// Avatar sizes
#constant AVATAR_SMALL	0 // 32x32
#constant AVATAR_MEDIUM	1 // 64x64
#constant AVATAR_LARGE	2 // 128x128

//------------------------------------------------------------------------------
// Structs
//------------------------------------------------------------------------------

// Associated Functions: GetLobbyDataJSON
Type KeyValuePair_t
	Key as string
	Value as string
EndType

// DLC information
// Associated Functions: GetDLCDataJSON
Type DLCData_t
	AppID as integer	// If 0, the DLC is hidden.
	Available as integer	// Boolean: If 1, the user owns the DLC.
	Name as string
EndType

// DLC download progress
// Associated Functions: GetDLCDownloadProgressJSON.
Type DownloadProgress_t
	AppID as integer	// If 0, the DLC is not currently downloading.
	BytesDownloaded as integer
	BytesTotal as integer
EndType

////////////////////////////////////////////////////////////////////////////////
// ISteamApps
// https://partner.steamgames.com/doc/api/ISteamApps
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Structs
//------------------------------------------------------------------------------

// Triggered after the current user gains ownership of DLC and that DLC is installed.
// Associated Functions: InstallDLC
Type DlcInstalled_t
	AppID as integer	// App ID of the DLC that was installed.
EndType

// Called after requesting the details of a specific file.
// Associated Functions: GetFileDetails
Type FileDetailsResult_t
	Result as integer	// EResult. Was the call successful?
	FileSize as integer	// The original file size in bytes.
	FileSHA as integer[]	// The original file SHA1 hash.
	Flags as integer
EndType

////////////////////////////////////////////////////////////////////////////////
// ISteamFriends
// https://partner.steamgames.com/doc/api/ISteamFriends
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Structs
//------------------------------------------------------------------------------

// Called when a large avatar is loaded if you have tried requesting it when it was unavailable.
// Associated Functions: GetLargeFriendAvatar
Type AvatarImageLoaded_t
	SteamID as integer	// The Steam ID that the avatar has been loaded for.
	Image as integer	// The Steam image handle of the now loaded image.
	Wide as integer	// Width of the loaded image.
	Tall as integer	// Height of the loaded image.
EndType

// Marks the return of a request officer list call.
// Associated Functions: RequestClanOfficerList
Type ClanOfficerListResponse_t
	SteamIDClan as integer	// The Steam group that we just got the officer list for.
	Officers as integer	// The number of officers in the group.
	Success as integer	// Was the call successful?
EndType

// Called when a Steam group activity has received.
// Associated Functions: DownloadClanActivityCounts
Type DownloadClanActivityCountsResult_t
	Success as integer	// Was the call successful?
EndType

// Called when Rich Presence data has been updated for a user, this can happen automatically when friends in the same game update their rich presence, or after a call to RequestFriendRichPresence.
// Associated Functions: RequestFriendRichPresence
Type FriendRichPresenceUpdate_t
	SteamIDFriend as integer	// The Steam ID of the user who's rich presence has changed.
	AppID as integer	// The App ID of the game.
EndType

// Returns the result of EnumerateFollowingList.
// Associated Functions: EnumerateFollowingList
Type FriendsEnumerateFollowingList_t
	Result as integer	// EResult. The result of the operation.
	SteamID as integer[]	// The list of users that we are following.
	ResultsReturned as integer	// The number of results returned in m_rgSteamID.
	TotalResultCount as integer	// The total number of people we are following.
EndType

// Returns the result of GetFollowerCount.
// Associated Functions: GetFollowerCount
Type FriendsGetFollowerCount_t
	Result as integer	// EResult. The result of the operation.
	SteamID as integer	// The Steam ID of the user we requested the follower count for.
	Count as integer	// The number of followers the user has.
EndType

// Returns the result of IsFollowing.
// Associated Functions: IsFollowing
Type FriendsIsFollowing_t
	Result as integer	// EResult. The result of the operation.
	SteamID as integer	// The Steam ID that was checked.
	IsFollowing as integer	// Are we following the user?
EndType

// Called when a user has joined a Steam group chat that the we are in.
// Associated Functions: JoinClanChatRoom
Type GameConnectedChatJoin_t
	SteamIDClanChat as integer	// The Steam ID of the chat that a user has joined.
	SteamIDUser as integer	// The Steam ID of the user that has joined the chat.
EndType

// Called when a user has left a Steam group chat that the we are in.
// Associated Functions: LeaveClanChatRoom
Type GameConnectedChatLeave_t
	SteamIDClanChat as integer	// The Steam ID of the chat that a user has left.
	SteamIDUser as integer	// The Steam ID of the user that has left the chat.
	Kicked as integer	// Was the user kicked by an officer (true), or not (false)?
	Dropped as integer	// Was the users connection to Steam dropped (true), or did they leave via other means (false)?
EndType

// Called when a chat message has been received in a Steam group chat that we are in.
// Associated Functions: JoinClanChatRoom
Type GameConnectedClanChatMsg_t
	SteamIDClanChat as integer	// The Steam ID of the chat that the message was received in.
	SteamIDUser as integer	// The Steam ID of the user that sent the message.
	MessageID as integer	// The index of the message to get the actual data from with GetClanChatMessage.
EndType

// Called when chat message has been received from a friend.
// Associated Functions: SetListenForFriendsMessages
Type GameConnectedFriendChatMsg_t
	SteamIDUser as integer	// The Steam ID of the friend that sent the message.
	MessageID as integer	// The index of the message to get the actual data from with GetFriendMessage.
EndType

// Called when the user tries to join a lobby from their friends list or from an invite.
Type GameLobbyJoinRequested_t
	SteamIDLobby as integer	// The Steam ID of the lobby to connect to.
	SteamIDFriend as integer	// The friend they joined through.
EndType

// Posted when the Steam Overlay activates or deactivates.
Type GameOverlayActivated_t
	Active as integer	// 1 if it's just been activated, otherwise 0.
EndType

// Called when the user tries to join a game from their friends list or after a user accepts an invite by a friend with InviteUserToGame.
// Associated Functions: InviteUserToGame
Type GameRichPresenceJoinRequested_t
	SteamIDFriend as integer	// The friend they joined through.
	Connect as string	// The value associated with the "connect" Rich Presence key.
EndType

// Called when the user tries to join a different game server from their friends list.
Type GameServerChangeRequested_t
	Server as string	// Server address.
	Password as string	// Server password, if any.
EndType

// Posted when the user has attempted to join a Steam group chat via JoinClanChatRoom
// Associated Functions: JoinClanChatRoom
Type JoinClanChatRoomCompletionResult_t
	SteamIDClanChat as integer	// The Steam ID of the chat that the user has joined.
	ChatRoomEnterResponse as integer	// EChatRoomEnterResponse. The result of the operation.
EndType

// Called whenever a friends' status changes.
// Associated Functions: RequestUserInformation
Type PersonaStateChange_t
	SteamID as integer	// Steam ID of the user who changed.
	ChangeFlags as integer	// A bit-wise union of EPersonaChange values.
EndType

// Reports the result of an attempt to change the current user's persona name.
// Associated Functions: SetPersonaName
Type SetPersonaNameResponse_t
	Success as integer	// true if name change completed successfully.
	LocalSuccess as integer	// true if name change was retained locally.
	Result as integer	// EResult. The result of the operation.
EndType

// Information about the game a friend is playing.
// Obtainable from: GetFriendGamePlayedJSON
// Modified from Steamworks SDK.
Type FriendGameInfo_t
	InGame as integer 	// Boolean
	GameAppID as integer	// The game ID that the friend is playing.
	GameIP as string	// The IP of the server the friend is playing on.
	GamePort as integer	// The port of the server the friend is playing on.
	QueryPort as integer	// The query port of the server the friend is playing on.
	SteamIDLobby as integer	// The Steam ID of the lobby the friend is in.
EndType

// Information about user sessions.
Type FriendSessionStateInfo_t
	OnlineSessionInstances as integer
	PublishedToFriendsSessionInstance as integer
EndType

//------------------------------------------------------------------------------
// Enums and Constants
//------------------------------------------------------------------------------

// EFriendFlags
// Flags for enumerating friends list, or quickly checking the relationship between users.
#constant EFriendFlagNone	0x00	// None.
#constant EFriendFlagBlocked	0x01	// Users that the current user has blocked from contacting.
#constant EFriendFlagFriendshipRequested	0x02	// Users that have sent a friend invite to the current user.
#constant EFriendFlagImmediate	0x04	// The current user's "regular" friends.
#constant EFriendFlagClanMember	0x08	// Users that are in one of the same (small) Steam groups as the current user.
#constant EFriendFlagOnGameServer	0x10	// Users that are on the same game server; as set by SetPlayedWith.
#constant EFriendFlagRequestingFriendship	0x80	// Users that the current user has sent friend invites to.
#constant EFriendFlagRequestingInfo	0x100	// Users that are currently sending additional info about themselves after a call to RequestUserInformation
#constant EFriendFlagIgnored	0x200	// Users that the current user has ignored from contacting them.
#constant EFriendFlagIgnoredFriend	0x400	// Users that have ignored the current user; but the current user still knows about them.
#constant EFriendFlagChatMember	0x1000	// Users in one of the same chats.
#constant EFriendFlagAll	0xFFFF	// Returns all friend flags.

// EFriendRelationship
// Declares the set of relationships that Steam users may have.
#constant EFriendRelationshipNone	0	// The users have no relationship.
#constant EFriendRelationshipBlocked	1	// The user has just clicked Ignore on an friendship invite.
#constant EFriendRelationshipRequestRecipient	2	// The user has requested to be friends with the current user.
#constant EFriendRelationshipFriend	3	// A "regular" friend.
#constant EFriendRelationshipRequestInitiator	4	// The current user has sent a friend invite.
#constant EFriendRelationshipIgnored	5	// The current user has explicit blocked this other user from comments/chat/etc.
#constant EFriendRelationshipIgnoredFriend	6	// The user has ignored the current user.
//#constant EFriendRelationshipSuggested_DEPRECATED	7	// Deprecated -- Unused.
//#constant EFriendRelationshipMax	8	// The total number of friend relationships used for looping and verification.

// EOverlayToStoreFlag
// These values are passed as parameters to the store with ActivateGameOverlayToStore and modify the behavior when the page opens.
#constant EOverlayToStoreFlag_None	0	// No
#constant EOverlayToStoreFlag_AddToCart	1	// Add the specified app ID to the users cart.
#constant EOverlayToStoreFlag_AddToCartAndShow	2	// Add the specified app ID to the users cart and show the store page.

// EPersonaChange
// used in PersonaStateChange_t::m_nChangeFlags to describe what's changed about a user
#constant EPersonaChangeName	0x0001
#constant EPersonaChangeStatus	0x0002
#constant EPersonaChangeComeOnline	0x0004
#constant EPersonaChangeGoneOffline	0x0008
#constant EPersonaChangeGamePlayed	0x0010
#constant EPersonaChangeGameServer	0x0020
#constant EPersonaChangeAvatar	0x0040
#constant EPersonaChangeJoinedSource	0x0080
#constant EPersonaChangeLeftSource	0x0100
#constant EPersonaChangeRelationshipChanged	0x0200
#constant EPersonaChangeNameFirstSet	0x0400
#constant EPersonaChangeFacebookInfo	0x0800
#constant EPersonaChangeNickname	0x1000
#constant EPersonaChangeSteamLevel	0x2000

// EPersonaState
// List of states a Steam friend can be in.
#constant EPersonaStateOffline	0	// Friend is not currently logged on.
#constant EPersonaStateOnline	1	// Friend is logged on.
#constant EPersonaStateBusy	2	// User is on, but busy.
#constant EPersonaStateAway	3	// Auto-away feature.
#constant EPersonaStateSnooze	4	// Auto-away for a long time.
#constant EPersonaStateLookingToTrade	5	// Online, trading.
#constant EPersonaStateLookingToPlay	6	// Online, wanting to play.
//#constant EPersonaStateMax	7	// The total number of states.

// EUserRestriction
// User restriction flags.
#constant nUserRestrictionNone	0	// No known chat/content restriction.
#constant nUserRestrictionUnknown	1	// We don't know yet, the user is offline.
#constant nUserRestrictionAnyChat	2	// User is not allowed to (or can't) send/recv any chat.
#constant nUserRestrictionVoiceChat	4	// User is not allowed to (or can't) send/recv voice chat.
#constant nUserRestrictionGroupChat	8	// User is not allowed to (or can't) send/recv group chat.
#constant nUserRestrictionRating	16	// User is too young according to rating in current region.
#constant nUserRestrictionGameInvites	32	// User cannot send or recv game invites, for example if they are on mobile.
#constant nUserRestrictionTrading	64	// User cannot participate in trading, for example if they are on a console or mobile.

////////////////////////////////////////////////////////////////////////////////
// ISteamGameServer
// https://partner.steamgames.com/doc/api/ISteamGameServer
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Structs
//------------------------------------------------------------------------------

// Sent when the game server attempted to be associated with a Steam Group.
// Associated Functions: AssociateWithClan
Type AssociateWithClanResult_t
	Result as integer	// EResult. The result of the operation.
EndType

// Sent when the game server is checking if the new player is a good fit for the server based on the frenemy system.
// Associated Functions: ComputeNewPlayerCompatibility
Type ComputeNewPlayerCompatibilityResult_t
	Result as integer	// EResult. The result of the operation.
	PlayersThatDontLikeCandidate as integer	// The number of current players that don't like playing with the specified player.
	PlayersThatCandidateDoesntLike as integer	// The number of players on the server that the specified player doesn't like playing with.
	ClanPlayersThatDontLikeCandidate as integer	// The number of players in the associated Steam group that don't like playing with the player.
	SteamIDCandidate as integer	// The Steam ID of the specified player.
EndType

// A client has been approved to connect to this game server.
Type GSClientApprove_t
	SteamID as integer	// The Steam ID of the user that is wanting to connect.
	OwnerSteamID as integer	// The Steam ID that owns the game, this will be different from m_SteamID if the game is being borrowed via Steam Family Sharing.
EndType

// Called when a user has been denied to connection to this game server.
Type GSClientDeny_t
	SteamID as integer	// The Steam ID of the user that attempted to connect.
	DenyReason as integer	// EDenyReason. The reason the player was denied.
	OptionalText as string	// An optional text message explaining the deny reason.
EndType

// Called when we have recieved the group status of a user.
// Associated Functions: RequestUserGroupStatus
Type GSClientGroupStatus_t
	SteamIDUser as integer	// The user whose group status we queried.
	SteamIDGroup as integer	// The group that we queried.
	Member as integer	// Is the user a member of the group (true) or not (false)?
	Officer as integer	// Is the user an officer in the group (true) or not (false)?
EndType

// Called when the game server should kick the user.
Type GSClientKick_t
	SteamID as integer	// The Steam ID of the player that should be kicked.
	DenyReason as integer	// EDenyReason. The reason the player is being kicked.
EndType

// Received when the game server requests to be displayed as secure (VAC protected)
Type GSPolicyResponse_t
	Secure as integer	// Is the server now set to secure (true) or unsecure (false)?.
EndType

////////////////////////////////////////////////////////////////////////////////
// ISteamGameServerStats
// https://partner.steamgames.com/doc/api/ISteamGameServerStats
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Structs
//------------------------------------------------------------------------------

// Result when getting the latests stats and achievements for a user from the server.
// Associated Functions: RequestUserStats
Type GSStatsReceived_t
	Result as integer	// EResult. Returns whether the call was successful or not.
	SteamIDUser as integer	// The user whose stats were retrieved.
EndType

// Result of a request to store the user stats.
// Associated Functions: StoreUserStats
Type GSStatsStored_t
	Result as integer	// EResult. Returns whether the call was successful or not.
	SteamIDUser as integer	// The user for whom the stats were stored
EndType

// Callback indicating that a user's stats have been unloaded.
Type GSStatsUnloaded_t
	SteamIDUser as integer	// User whose stats have been unloaded.
EndType

////////////////////////////////////////////////////////////////////////////////
// ISteamHTMLSurface
// https://partner.steamgames.com/doc/api/ISteamHTMLSurface
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Structs
//------------------------------------------------------------------------------

// A new browser was created and is ready for use.
// Associated Functions: CreateBrowser
Type HTML_BrowserReady_t
	BrowserHandle as integer	// This is the handle to the browser that was just created, which you can use with future calls to ISteamHTMLSurface.
EndType

// Called when page history status has changed the ability to go backwards and forward.
Type HTML_CanGoBackAndForward_t
	BrowserHandle as integer	// The handle of the surface that this callback is for.
	CanGoBack as integer	// Returns whether you can navigate backwards.
	CanGoForward as integer	// Returns whether you can navigate forward.
EndType

// Called when the current page in a browser gets a new title.
Type HTML_ChangedTitle_t
	BrowserHandle as integer	// The handle of the surface that this callback is for.
	Title as string	// The new title of the page.
EndType

// Called when the browser has been requested to close due to user interaction; usually because of a javascript
Type HTML_CloseBrowser_t
	BrowserHandle as integer	// The handle of the surface that this callback is for.
EndType

// Called when a browser surface has received a file open dialog from a
Type HTML_FileOpenDialog_t
	BrowserHandle as integer	// The handle of the surface that is navigating.
	Title as string	// The name of the dialog.
	InitialFile as string	// The filename that the page wants you to set by default.
EndType

// Called when a browser has finished loading a page.
Type HTML_FinishedRequest_t
	BrowserHandle as integer	// The handle of the surface that this call was for.
	URL as string	// The URL that was loaded.
	PageTitle as string	// The title of the page that got loaded.
EndType

// Called when a a browser wants to hide a tooltip.
Type HTML_HideToolTip_t
	BrowserHandle as integer	// The handle of the surface that this callback is for.
EndType

// Provides details on the visibility and size of the horizontal scrollbar.
// Associated Functions: SetHorizontalScroll
Type HTML_HorizontalScroll_t
	BrowserHandle as integer	// The handle of the surface that this callback is for.
	ScrollMax as integer	// The maximum amount you can scroll horizontally.
	ScrollCurrent as integer	// The current horizontal scroll position.
	PageScale as float	// The current page scale.
	Visible as integer	// Whether the horizontal scrollbar is visible.
	PageSize as integer	// The total width of the page in pixels.
EndType

// Called when the browser wants to display a Javascript alert dialog, call JSDialogResponse when the user dismisses this dialog; or right away to ignore it.
Type HTML_JSAlert_t
	BrowserHandle as integer	// The handle of the surface that this call was for.
	Message as string	// The message associated with the dialog.
EndType

// Called when the browser wants to display a Javascript confirmation dialog, call JSDialogResponse when the user dismisses this dialog; or right away to ignore it.
Type HTML_JSConfirm_t
	BrowserHandle as integer	// The handle of the surface that this call was for.
	Message as string	// The message associated with the dialog.
EndType

// Result of a call to GetLinkAtPosition
// Associated Functions: GetLinkAtPosition
Type HTML_LinkAtPosition_t
	BrowserHandle as integer	// The handle of the surface that this call was for.
	X as integer	// Unused.
	Y as integer	// Unused.
	URL as string	// The URL found at this position.
	Input as integer	// Was the position an input field?
	LiveLink as integer
EndType

// Called when a browser surface has a pending paint.
Type HTML_NeedsPaint_t
	BrowserHandle as integer	// The handle of the surface that this callback is for.
	BGRA as string	// A pointer to the B8G8R8A8 image data for this surface, valid until SteamAPI_RunCallbacks is next called.
	Wide as integer	// The total width of the pBGRA texture.
	Tall as integer	// The total height of the pBGRA texture.
	UpdateX as integer	// The offset in X for the damage rect for this update.
	UpdateY as integer	// The offset in Y for the damage rect for this update.
	UpdateWide as integer	// The width of the damage rect for this update.
	UpdateTall as integer	// The height of the damage rect for this update.
	ScrollX as integer	// The horizontal scroll position the browser was at when this texture was rendered.
	ScrollY as integer	// The veritcal scroll position the browser was at when this texture was rendered.
	PageScale as float	// The scale factor the browser was at when this texture was rendered.
	PageSerial as integer	// Incremented on each new page load, you can use this to reject draws while navigating to new pages.
EndType

// A browser has created a new HTML window.
Type HTML_NewWindow_t
	BrowserHandle as integer	// The handle of the surface that this callback is for.
	URL as string	// The URL to load in the new window.
	X as integer	// The x pos into the page to display the popup.
	Y as integer	// The y pos into the page to display the popup.
	Wide as integer	// The total width of the pBGRA texture.
	Tall as integer	// The total height of the pBGRA texture.
EndType

// The browser has requested to load a url in a new tab.
Type HTML_OpenLinkInNewTab_t
	BrowserHandle as integer	// The handle of the surface that this callback is for.
	URL as string	// The URL that the browser wants to load.
EndType

// Results from a search.
// Associated Functions: Find
Type HTML_SearchResults_t
	BrowserHandle as integer	// The handle of the surface that this callback is for.
	Results as integer	// The number of matching results found.
	CurrentMatch as integer	// The ordinal of the current match relative to unResults.
EndType

// Called when a browser wants to change the mouse cursor.
Type HTML_SetCursor_t
	BrowserHandle as integer	// The handle of the surface that this callback is for.
	MouseCursor as integer	// The EMouseCursor to display.
EndType

// Called when a browser wants to display a tooltip.
Type HTML_ShowToolTip_t
	BrowserHandle as integer	// The handle of the surface that this callback is for.
	Msg as string	// The text of the tooltip that wants to be displayed.
EndType

// Called when a browser wants to navigate to a new page.
// Associated Functions: LoadURL
Type HTML_StartRequest_t
	BrowserHandle as integer	// The handle of the surface that is navigating.
	URL as string	// The url it wants to navigate to.
	Target as string	// The html link target type (i.e _blank, _self, _parent, _top ).
	PostData as string	// Any posted data for the request.
	IsRedirect as integer	// True if this was a http/html redirect from the last load request.
EndType

// Called when a browser wants you to display an informational message.
Type HTML_StatusText_t
	BrowserHandle as integer	// The handle of the surface that this callback is for.
	Msg as string	// The text of the status message to display.
EndType

// Called when the text of an existing tooltip has been updated.
Type HTML_UpdateToolTip_t
	BrowserHandle as integer	// The handle of the surface that this callback is for.
	Msg as string	// The new text of the tooltip.
EndType

// Called when the browser is navigating to a new url
Type HTML_URLChanged_t
	BrowserHandle as integer	// The handle of the surface that this callback is for.
	URL as string	// The url that the browser is navigating to.
	PostData as string	// Any posted data for the request, NULL if there was none.
	IsRedirect as integer	// true if this was a http/html redirect from the last load request, otherwise false.
	PageTitle as string	// The title of the page.
	NewNavigation as integer	// This is true if the page has changed rather than just a call to the browser history API.
EndType

// Provides details on the visibility and size of the vertical scrollbar.
// Associated Functions: SetVerticalScroll
Type HTML_VerticalScroll_t
	BrowserHandle as integer	// The handle of the surface that this callback is for.
	ScrollMax as integer	// The maximum amount you can scroll vertically.
	ScrollCurrent as integer	// The current vertical scroll position.
	PageScale as float	// The current page scale.
	Visible as integer	// Whether the vertical scrollbar is visible.
	PageSize as integer	// The total height of the page in pixels.
EndType

//------------------------------------------------------------------------------
// Enums and Constants
//------------------------------------------------------------------------------

// EHTMLKeyModifiers
// Used to let the browser know what keys are pressed with: KeyDown , KeyDown and KeyDown.
#constant EHTMLKeyModifier_None	0	// No modifiers are pressed.
#constant EHTMLKeyModifier_AltDown	1	// One of the alt keys are pressed.	// 1 << 0
#constant EHTMLKeyModifier_CtrlDown	2	// One of the ctrl keys are pressed.	// 1 << 1
#constant EHTMLKeyModifier_ShiftDown	4	// One of the shift keys are pressed.	// 1 << 2

// EHTMLMouseButton
// Used to let the browser know when a mouse button is pressed with: MouseUp , MouseDown and MouseDoubleClick.
#constant EHTMLMouseButton_Left	0	// The left button is pressed.
#constant EHTMLMouseButton_Right	1	// The right button is pressed.
#constant EHTMLMouseButton_Middle	2	// The middle button is pressed.

// EMouseCursor
// This lists the mouse cursors that the HTML surface will tell you to render.
#constant dc_user	0
#constant dc_none	1
#constant dc_arrow	2
#constant dc_ibeam	3
#constant dc_hourglass	4
#constant dc_waitarrow	5
#constant dc_crosshair	6
#constant dc_up	7
#constant dc_sizenw	8
#constant dc_sizese	9
#constant dc_sizene	10
#constant dc_sizesw	11
#constant dc_sizew	12
#constant dc_sizee	13
#constant dc_sizen	14
#constant dc_sizes	15
#constant dc_sizewe	16
#constant dc_sizens	17
#constant dc_sizeall	18
#constant dc_no	19
#constant dc_hand	20
#constant dc_blank	21	// Don't show any custom cursor, just use your default.
#constant dc_middle_pan	22
#constant dc_north_pan	23
#constant dc_north_east_pan	24
#constant dc_east_pan	25
#constant dc_south_east_pan	26
#constant dc_south_pan	27
#constant dc_south_west_pan	28
#constant dc_west_pan	29
#constant dc_north_west_pan	30
#constant dc_alias	31
#constant dc_cell	32
#constant dc_colresize	33
#constant dc_copycur	34
#constant dc_verticaltext	35
#constant dc_rowresize	36
#constant dc_zoomin	37
#constant dc_zoomout	38
#constant dc_help	39
#constant dc_custom	40
#constant dc_last	41	// Only used to iterate over all cursors.

////////////////////////////////////////////////////////////////////////////////
// ISteamHTTP
// https://partner.steamgames.com/doc/api/ISteamHTTP
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Structs
//------------------------------------------------------------------------------

// Result when an HTTP request completes.
// Associated Functions: SendHTTPRequest
Type HTTPRequestCompleted_t
	Request as integer	// Handle for the request that has completed.
	ContextValue as integer	// Context value that the user defined on the request with SetHTTPRequestContextValue that this callback is associated with.
	RequestSuccessful as integer	// This will be true if the request actually got any sort of response from the server (even an error).
	StatusCode as integer	// EHTTPStatusCode. Will be the HTTP status code value returned by the server.k_EHTTPStatusCode200OK is the normal OK response, if you get something else you probably need to treat it as a failure.
	BodySize as integer	// The size of the request body in bytes.
EndType

// Triggered when a chunk of data is received from a streaming HTTP request.
// Associated Functions: SendHTTPRequestAndStreamResponse
Type HTTPRequestDataReceived_t
	Request as integer	// Handle value for the request that has received data.
	ContextValue as integer	// Context value that the user defined on the request that this callback is associated with, 0 ifno context value was set.
	Offset as integer	// Offset to provide to GetHTTPStreamingResponseBodyData to get this chunk of data
	BytesReceived as integer	// Size in bytes to provide to GetHTTPStreamingResponseBodyData to get this chunk of data
EndType

// Triggered when HTTP headers are received from a streaming HTTP request.
// Associated Functions: SendHTTPRequestAndStreamResponse
Type HTTPRequestHeadersReceived_t
	Request as integer	// Handle value for the request that has received headers.
	ContextValue as integer	// Context value that the user defined on the request that this callback is associated with, 0 ifno context value was set.
EndType

//------------------------------------------------------------------------------
// Enums and Constants
//------------------------------------------------------------------------------

// EHTTPMethod
// Used to set the HTTP request method in CreateHTTPRequest.
#constant EHTTPMethodInvalid	0	// Invalid.
#constant EHTTPMethodGET	1	// The HTTP Method is set to GET.
#constant EHTTPMethodHEAD	2	// The HTTP Method is set to HEAD.
#constant EHTTPMethodPOST	3	// The HTTP Method is set to POST.
#constant EHTTPMethodPUT	4	// The HTTP Method is set to PUT.
#constant EHTTPMethodDELETE	5	// The HTTP Method is set to DELETE.
#constant EHTTPMethodOPTIONS	6	// The HTTP Method is set to OPTIONS.
#constant EHTTPMethodPATCH	7	// The HTTP Method is set to PATCH.

// EHTTPStatusCode
// HTTP response status codes that the server can send in response to a request.
#constant EHTTPStatusCodeInvalid	0	// Invalid status code.
#constant EHTTPStatusCode100Continue	100
#constant EHTTPStatusCode101SwitchingProtocols	101
#constant EHTTPStatusCode200OK	200
#constant EHTTPStatusCode201Created	201
#constant EHTTPStatusCode202Accepted	202
#constant EHTTPStatusCode203NonAuthoritative	203
#constant EHTTPStatusCode204NoContent	204
#constant EHTTPStatusCode205ResetContent	205
#constant EHTTPStatusCode206PartialContent	206
#constant EHTTPStatusCode300MultipleChoices	300
#constant EHTTPStatusCode301MovedPermanently	301
#constant EHTTPStatusCode302Found	302
#constant EHTTPStatusCode303SeeOther	303
#constant EHTTPStatusCode304NotModified	304
#constant EHTTPStatusCode305UseProxy	305
#constant EHTTPStatusCode307TemporaryRedirect	307
#constant EHTTPStatusCode400BadRequest	400
#constant EHTTPStatusCode401Unauthorized	401	// You probably want 403 or something else.
#constant EHTTPStatusCode402PaymentRequired	402	// This is reserved for future HTTP specs, not really supported by clients.
#constant EHTTPStatusCode403Forbidden	403
#constant EHTTPStatusCode404NotFound	404
#constant EHTTPStatusCode405MethodNotAllowed	405
#constant EHTTPStatusCode406NotAcceptable	406
#constant EHTTPStatusCode407ProxyAuthRequired	407
#constant EHTTPStatusCode408RequestTimeout	408
#constant EHTTPStatusCode409Conflict	409
#constant EHTTPStatusCode410Gone	410
#constant EHTTPStatusCode411LengthRequired	411
#constant EHTTPStatusCode412PreconditionFailed	412
#constant EHTTPStatusCode413RequestEntityTooLarge	413
#constant EHTTPStatusCode414RequestURITooLong	414
#constant EHTTPStatusCode415UnsupportedMediaType	415
#constant EHTTPStatusCode416RequestedRangeNotSatisfiable	416
#constant EHTTPStatusCode417ExpectationFailed	417
#constant EHTTPStatusCode4xxUnknown	418	// 418 is reserved, so we use it to mean unknown.
#constant EHTTPStatusCode429TooManyRequests	429
#constant EHTTPStatusCode500InternalServerError	500
#constant EHTTPStatusCode501NotImplemented	501
#constant EHTTPStatusCode502BadGateway	502
#constant EHTTPStatusCode503ServiceUnavailable	503
#constant EHTTPStatusCode504GatewayTimeout	504
#constant EHTTPStatusCode505HTTPVersionNotSupported	505
#constant EHTTPStatusCode5xxUnknown	599

////////////////////////////////////////////////////////////////////////////////
// ISteamInput
// https://partner.steamgames.com/doc/api/ISteamInput
////////////////////////////////////////////////////////////////////////////////

Type InputMotionData_t
	// Sensor-fused absolute rotation; will drift in heading
	QuatX as float
	QuatY as float
	QuatZ as float
	QuatW as float
	// Positional acceleration
	AccelX as float
	AccelY as float
	AccelZ as float
	// Angular velocity
	VelX as float
	VelY as float
	VelZ as float
EndType

//------------------------------------------------------------------------------
// Enums and Constants
//------------------------------------------------------------------------------

// EInputActionOrigin
// Inputs the player binds to actions in the Steam Input Configurator.
// The chief purpose of these values is to direct which on-screen button glyphs should appear for a given action, such as "Press [A] to Jump".
// Steam Controller
#constant EInputActionOrigin_None								0	
#constant EInputActionOrigin_SteamController_A						1	// (Valve Steam Controller) digital face button A
#constant EInputActionOrigin_SteamController_B						2	// (Valve Steam Controller) digital face button B
#constant EInputActionOrigin_SteamController_X						3	// (Valve Steam Controller) digital face button X
#constant EInputActionOrigin_SteamController_Y						4	// (Valve Steam Controller) digital face button Y
#constant EInputActionOrigin_SteamController_LeftBumper				5	// (Valve Steam Controller) digital left shoulder button // (aka "left bumper")
#constant EInputActionOrigin_SteamController_RightBumper			6	// (Valve Steam Controller) digital right shoulder button // (aka "right bumper")
#constant EInputActionOrigin_SteamController_LeftGrip				7	// (Valve Steam Controller) digital left grip paddle
#constant EInputActionOrigin_SteamController_RightGrip				8	// (Valve Steam Controller) digital right grip paddle
#constant EInputActionOrigin_SteamController_Start					9	// (Valve Steam Controller) digital start button
#constant EInputActionOrigin_SteamController_Back					10	// (Valve Steam Controller) digital back button
#constant EInputActionOrigin_SteamController_LeftPad_Touch			11	// (Valve Steam Controller) left haptic touchpad, in simple contact with a finger
#constant EInputActionOrigin_SteamController_LeftPad_Swipe			12	// (Valve Steam Controller) left haptic touchpad, touch input on any axis
#constant EInputActionOrigin_SteamController_LeftPad_Click			13	// (Valve Steam Controller) left haptic touchpad, digital click // (for the whole thing)
#constant EInputActionOrigin_SteamController_LeftPad_DPadNorth		14	// (Valve Steam Controller) left haptic touchpad, digital click // (upper quadrant)
#constant EInputActionOrigin_SteamController_LeftPad_DPadSouth		15	// (Valve Steam Controller) left haptic touchpad, digital click // (lower quadrant)
#constant EInputActionOrigin_SteamController_LeftPad_DPadWest		16	// (Valve Steam Controller) left haptic touchpad, digital click // (left quadrant)
#constant EInputActionOrigin_SteamController_LeftPad_DPadEast		17	// (Valve Steam Controller) left haptic touchpad, digital click // (right quadrant)
#constant EInputActionOrigin_SteamController_RightPad_Touch			18	// (Valve Steam Controller) right haptic touchpad, in simple contact with a finger
#constant EInputActionOrigin_SteamController_RightPad_Swipe			19	// (Valve Steam Controller) right haptic touchpad, touch input on any axis
#constant EInputActionOrigin_SteamController_RightPad_Click			20	// (Valve Steam Controller) right haptic touchpad, digital click // (for the whole thing)
#constant EInputActionOrigin_SteamController_RightPad_DPadNorth		21	// (Valve Steam Controller) right haptic touchpad, digital click // (upper quadrant)
#constant EInputActionOrigin_SteamController_RightPad_DPadSouth		22	// (Valve Steam Controller) right haptic touchpad, digital click // (lower quadrant)
#constant EInputActionOrigin_SteamController_RightPad_DPadWest		23	// (Valve Steam Controller) right haptic touchpad, digital click // (left quadrant)
#constant EInputActionOrigin_SteamController_RightPad_DPadEast		24	// (Valve Steam Controller) right haptic touchpad, digital click // (right quadrant)
#constant EInputActionOrigin_SteamController_LeftTrigger_Pull		25	// (Valve Steam Controller) left analog trigger, pulled by any amount // (analog value)
#constant EInputActionOrigin_SteamController_LeftTrigger_Click		26	// (Valve Steam Controller) left analog trigger, pulled in all the way // (digital value)
#constant EInputActionOrigin_SteamController_RightTrigger_Pull		27	// (Valve Steam Controller) right analog trigger, pulled by any amount // (analog value)
#constant EInputActionOrigin_SteamController_RightTrigger_Click		28	// (Valve Steam Controller) right analog trigger, pulled in all the way // (digital value)
#constant EInputActionOrigin_SteamController_LeftStick_Move			29	// (Valve Steam Controller) left joystick, movement on any axis // (analog value)
#constant EInputActionOrigin_SteamController_LeftStick_Click		30	// (Valve Steam Controller) left joystick, clicked in // (digital value)
#constant EInputActionOrigin_SteamController_LeftStick_DPadNorth	31	// (Valve Steam Controller) left joystick, digital movement // (upper quadrant)
#constant EInputActionOrigin_SteamController_LeftStick_DPadSouth	32	// (Valve Steam Controller) left joystick, digital movement // (lower quadrant)
#constant EInputActionOrigin_SteamController_LeftStick_DPadWest		33	// (Valve Steam Controller) left joystick, digital movement // (left quadrant)
#constant EInputActionOrigin_SteamController_LeftStick_DPadEast		34	// (Valve Steam Controller) left joystick, digital movement // (right quadrant)
#constant EInputActionOrigin_SteamController_Gyro_Move				35	// (Valve Steam Controller) gyroscope, analog movement in any axis
#constant EInputActionOrigin_SteamController_Gyro_Pitch				36	// (Valve Steam Controller) gyroscope, analog movement on the Pitch axis // (point head up to ceiling, point head down to floor)
#constant EInputActionOrigin_SteamController_Gyro_Yaw				37	// (Valve Steam Controller) gyroscope, analog movement on the Yaw axis // (turn head left to face one wall, turn head right to face other)
#constant EInputActionOrigin_SteamController_Gyro_Roll				38	// (Valve Steam Controller) gyroscope, analog movement on the Roll axis // (tilt head left towards shoulder, tilt head right towards other)
#constant EInputActionOrigin_SteamController_Reserved0				39
#constant EInputActionOrigin_SteamController_Reserved1				40
#constant EInputActionOrigin_SteamController_Reserved2				41
#constant EInputActionOrigin_SteamController_Reserved3				42
#constant EInputActionOrigin_SteamController_Reserved4				43
#constant EInputActionOrigin_SteamController_Reserved5				44
#constant EInputActionOrigin_SteamController_Reserved6				45
#constant EInputActionOrigin_SteamController_Reserved7				46
#constant EInputActionOrigin_SteamController_Reserved8				47
#constant EInputActionOrigin_SteamController_Reserved9				48
#constant EInputActionOrigin_SteamController_Reserved10				49

// PS4 Dual Shock
#constant EInputActionOrigin_PS4_X							50	// (Sony Dualshock 4) digital face button X
#constant EInputActionOrigin_PS4_Circle						51	// (Sony Dualshock 4) digital face button Circle
#constant EInputActionOrigin_PS4_Triangle					52	// (Sony Dualshock 4) digital face button Triangle
#constant EInputActionOrigin_PS4_Square						53	// (Sony Dualshock 4) digital face button Square
#constant EInputActionOrigin_PS4_LeftBumper					54	// (Sony Dualshock 4) digital left shoulder button // (aka "left bumper")
#constant EInputActionOrigin_PS4_RightBumper				55	// (Sony Dualshock 4) digital right shoulder button // (aka "right bumper")
#constant EInputActionOrigin_PS4_Options					56	// (Sony Dualshock 4) digital options button // (aka "Start")
#constant EInputActionOrigin_PS4_Share						57	// (Sony Dualshock 4) digital back button // (aka "Back")
#constant EInputActionOrigin_PS4_LeftPad_Touch				58	// (Sony Dualshock 4) left half of the touchpad, in simple contact with a finger
#constant EInputActionOrigin_PS4_LeftPad_Swipe				59	// (Sony Dualshock 4) left half of the touchpad, touch input on any axis
#constant EInputActionOrigin_PS4_LeftPad_Click				60	// (Sony Dualshock 4) left half of the touchpad, digital click // (for the whole thing)
#constant EInputActionOrigin_PS4_LeftPad_DPadNorth			61	// (Sony Dualshock 4) left half of the touchpad, digital click // (upper quadrant)
#constant EInputActionOrigin_PS4_LeftPad_DPadSouth			62	// (Sony Dualshock 4) left half of the touchpad, digital click // (lower quadrant)
#constant EInputActionOrigin_PS4_LeftPad_DPadWest			63	// (Sony Dualshock 4) left half of the touchpad, digital click // (left quadrant)
#constant EInputActionOrigin_PS4_LeftPad_DPadEast			64	// (Sony Dualshock 4) left half of the touchpad, digital click // (right quadrant)
#constant EInputActionOrigin_PS4_RightPad_Touch				65	// (Sony Dualshock 4) right half of the touchpad, in simple contact with a finger
#constant EInputActionOrigin_PS4_RightPad_Swipe				66	// (Sony Dualshock 4) right half of the touchpad, touch input on any axis
#constant EInputActionOrigin_PS4_RightPad_Click				67	// (Sony Dualshock 4) right half of the touchpad, digital click // (for the whole thing)
#constant EInputActionOrigin_PS4_RightPad_DPadNorth			68	// (Sony Dualshock 4) right half of the touchpad, digital click // (upper quadrant)
#constant EInputActionOrigin_PS4_RightPad_DPadSouth			69	// (Sony Dualshock 4) right half of the touchpad, digital click // (lower quadrant)
#constant EInputActionOrigin_PS4_RightPad_DPadWest			70	// (Sony Dualshock 4) right half of the touchpad, digital click // (left quadrant)
#constant EInputActionOrigin_PS4_RightPad_DPadEast			71	// (Sony Dualshock 4) right half of the touchpad, digital click // (right quadrant)
#constant EInputActionOrigin_PS4_CenterPad_Touch			72	// (Sony Dualshock 4) unified touchpad, in simple contact with a finger
#constant EInputActionOrigin_PS4_CenterPad_Swipe			73	// (Sony Dualshock 4) unified touchpad, touch input on any axis
#constant EInputActionOrigin_PS4_CenterPad_Click			74	// (Sony Dualshock 4) unified touchpad, digital click // (for the whole thing)
#constant EInputActionOrigin_PS4_CenterPad_DPadNorth		75	// (Sony Dualshock 4) unified touchpad, digital click // (upper quadrant)
#constant EInputActionOrigin_PS4_CenterPad_DPadSouth		76	// (Sony Dualshock 4) unified touchpad, digital click // (lower quadrant)
#constant EInputActionOrigin_PS4_CenterPad_DPadWest			77	// (Sony Dualshock 4) unified touchpad, digital click // (left quadrant)
#constant EInputActionOrigin_PS4_CenterPad_DPadEast			78	// (Sony Dualshock 4) unified touchpad, digital click // (right quadrant)
#constant EInputActionOrigin_PS4_LeftTrigger_Pull			79	// (Sony Dualshock 4) left analog trigger, pulled by any amount // (analog value)
#constant EInputActionOrigin_PS4_LeftTrigger_Click			80	// (Sony Dualshock 4) left analog trigger, pulled in all the way // (digital value)
#constant EInputActionOrigin_PS4_RightTrigger_Pull			81	// (Sony Dualshock 4) right analog trigger, pulled by any amount // (analog value)
#constant EInputActionOrigin_PS4_RightTrigger_Click			82	// (Sony Dualshock 4) right analog trigger, pulled in all the way // (digital value)
#constant EInputActionOrigin_PS4_LeftStick_Move				83	// (Sony Dualshock 4) left joystick, movement on any axis // (analog value)
#constant EInputActionOrigin_PS4_LeftStick_Click			84	// (Sony Dualshock 4) left joystick, clicked in // (digital value)
#constant EInputActionOrigin_PS4_LeftStick_DPadNorth		85	// (Sony Dualshock 4) left joystick, digital movement // (upper quadrant)
#constant EInputActionOrigin_PS4_LeftStick_DPadSouth		86	// (Sony Dualshock 4) left joystick, digital movement // (lower quadrant)
#constant EInputActionOrigin_PS4_LeftStick_DPadWest			87	// (Sony Dualshock 4) left joystick, digital movement // (left quadrant)
#constant EInputActionOrigin_PS4_LeftStick_DPadEast			88	// (Sony Dualshock 4) left joystick, digital movement // (right quadrant)
#constant EInputActionOrigin_PS4_RightStick_Move			89	// (Sony Dualshock 4) right joystick, movement on any axis // (analog value)
#constant EInputActionOrigin_PS4_RightStick_Click			90	// (Sony Dualshock 4) right joystick, clicked in // (digital value)
#constant EInputActionOrigin_PS4_RightStick_DPadNorth		91	// (Sony Dualshock 4) right joystick, digital movement // (upper quadrant)
#constant EInputActionOrigin_PS4_RightStick_DPadSouth		92	// (Sony Dualshock 4) right joystick, digital movement // (lower quadrant)
#constant EInputActionOrigin_PS4_RightStick_DPadWest		93	// (Sony Dualshock 4) right joystick, digital movement // (left quadrant)
#constant EInputActionOrigin_PS4_RightStick_DPadEast		94	// (Sony Dualshock 4) right joystick, digital movement // (right quadrant)
#constant EInputActionOrigin_PS4_DPad_North					95	// (Sony Dualshock 4) digital pad, pressed // (upper quadrant)
#constant EInputActionOrigin_PS4_DPad_South					96	// (Sony Dualshock 4) digital pad, pressed // (lower quadrant)
#constant EInputActionOrigin_PS4_DPad_West					97	// (Sony Dualshock 4) digital pad, pressed // (left quadrant)
#constant EInputActionOrigin_PS4_DPad_East					98	// (Sony Dualshock 4) digital pad, pressed // (right quadrant)
#constant EInputActionOrigin_PS4_Gyro_Move					99	// (Sony Dualshock 4) gyroscope, analog movement in any axis
#constant EInputActionOrigin_PS4_Gyro_Pitch					100	// (Sony Dualshock 4) gyroscope, analog movement on the Pitch axis // (point head up to ceiling, point head down to floor)
#constant EInputActionOrigin_PS4_Gyro_Yaw					101	// (Sony Dualshock 4) gyroscope, analog movement on the Yaw axis // (turn head left to face one wall, turn head right to face other)
#constant EInputActionOrigin_PS4_Gyro_Roll					102	// (Sony Dualshock 4) gyroscope, analog movement on the Roll axis // (tilt head left towards shoulder, tilt head right towards other shoulder)
#constant EInputActionOrigin_PS4_Reserved0					103
#constant EInputActionOrigin_PS4_Reserved1					104
#constant EInputActionOrigin_PS4_Reserved2					105
#constant EInputActionOrigin_PS4_Reserved3					106
#constant EInputActionOrigin_PS4_Reserved4					107
#constant EInputActionOrigin_PS4_Reserved5					108
#constant EInputActionOrigin_PS4_Reserved6					109
#constant EInputActionOrigin_PS4_Reserved7					110
#constant EInputActionOrigin_PS4_Reserved8					111
#constant EInputActionOrigin_PS4_Reserved9					112
#constant EInputActionOrigin_PS4_Reserved10					113

// XBox One
#constant EInputActionOrigin_XBoxOne_A						114	// (XB1) digital face button A
#constant EInputActionOrigin_XBoxOne_B						115	// (XB1) digital face button B
#constant EInputActionOrigin_XBoxOne_X						116	// (XB1) digital face button X
#constant EInputActionOrigin_XBoxOne_Y						117	// (XB1) digital face button Y
#constant EInputActionOrigin_XBoxOne_LeftBumper				118	// (XB1) digital left shoulder button // (aka "left bumper")
#constant EInputActionOrigin_XBoxOne_RightBumper			119	// (XB1) digital right shoulder button // (aka "right bumper")
#constant EInputActionOrigin_XBoxOne_Menu					120	// (XB1) digital menu button // (aka "start")
#constant EInputActionOrigin_XBoxOne_View					121	// (XB1) digital view button // (aka "back")
#constant EInputActionOrigin_XBoxOne_LeftTrigger_Pull		122	// (XB1) left analog trigger, pulled by any amount // (analog value)
#constant EInputActionOrigin_XBoxOne_LeftTrigger_Click		123	// (XB1) left analog trigger, pulled in all the way // (digital value)
#constant EInputActionOrigin_XBoxOne_RightTrigger_Pull		124	// (XB1) right analog trigger, pulled by any amount // (analog value)
#constant EInputActionOrigin_XBoxOne_RightTrigger_Click		125	// (XB1) right analog trigger, pulled in all the way // (digital value)
#constant EInputActionOrigin_XBoxOne_LeftStick_Move			126	// (XB1) left joystick, movement on any axis // (analog value)
#constant EInputActionOrigin_XBoxOne_LeftStick_Click		127	// (XB1) left joystick, clicked in // (digital value)
#constant EInputActionOrigin_XBoxOne_LeftStick_DPadNorth	128	// (XB1) left joystick, digital movement // (upper quadrant)
#constant EInputActionOrigin_XBoxOne_LeftStick_DPadSouth	129	// (XB1) left joystick, digital movement // (lower quadrant)
#constant EInputActionOrigin_XBoxOne_LeftStick_DPadWest		130	// (XB1) left joystick, digital movement // (left quadrant)
#constant EInputActionOrigin_XBoxOne_LeftStick_DPadEast		131	// (XB1) left joystick, digital movement // (right quadrant)
#constant EInputActionOrigin_XBoxOne_RightStick_Move		132	// (XB1) right joystick, movement on any axis // (analog value)
#constant EInputActionOrigin_XBoxOne_RightStick_Click		133	// (XB1) right joystick, clicked in // (digital value)
#constant EInputActionOrigin_XBoxOne_RightStick_DPadNorth	134	// (XB1) right joystick, digital movement // (upper quadrant)
#constant EInputActionOrigin_XBoxOne_RightStick_DPadSouth	135	// (XB1) right joystick, digital movement // (lower quadrant)
#constant EInputActionOrigin_XBoxOne_RightStick_DPadWest	136	// (XB1) right joystick, digital movement // (left quadrant)
#constant EInputActionOrigin_XBoxOne_RightStick_DPadEast	137	// (XB1) right joystick, digital movement // (right quadrant)
#constant EInputActionOrigin_XBoxOne_DPad_North				138	// (XB1) digital pad, pressed // (upper quadrant)
#constant EInputActionOrigin_XBoxOne_DPad_South				139	// (XB1) digital pad, pressed // (lower quadrant)
#constant EInputActionOrigin_XBoxOne_DPad_West				140	// (XB1) digital pad, pressed // (left quadrant)
#constant EInputActionOrigin_XBoxOne_DPad_East				141	// (XB1) digital pad, pressed // (right quadrant)
#constant EInputActionOrigin_XBoxOne_Reserved0				142
#constant EInputActionOrigin_XBoxOne_Reserved1				143
#constant EInputActionOrigin_XBoxOne_Reserved2				144
#constant EInputActionOrigin_XBoxOne_Reserved3				145
#constant EInputActionOrigin_XBoxOne_Reserved4				146
#constant EInputActionOrigin_XBoxOne_Reserved5				147
#constant EInputActionOrigin_XBoxOne_Reserved6				148
#constant EInputActionOrigin_XBoxOne_Reserved7				149
#constant EInputActionOrigin_XBoxOne_Reserved8				150
#constant EInputActionOrigin_XBoxOne_Reserved9				151
#constant EInputActionOrigin_XBoxOne_Reserved10				152

// XBox 360
#constant EInputActionOrigin_XBox360_A						153	// (X360) digital face button A
#constant EInputActionOrigin_XBox360_B						154	// (X360) digital face button B
#constant EInputActionOrigin_XBox360_X						155	// (X360) digital face button X
#constant EInputActionOrigin_XBox360_Y						156	// (X360) digital face button Y
#constant EInputActionOrigin_XBox360_LeftBumper				157	// (X360) digital left shoulder button // (aka "left bumper")
#constant EInputActionOrigin_XBox360_RightBumper			158	// (X360) digital right shoulder button // (aka "right bumper")
#constant EInputActionOrigin_XBox360_Start					159	// (X360) digital start button
#constant EInputActionOrigin_XBox360_Back					160	// (X360) digital back button
#constant EInputActionOrigin_XBox360_LeftTrigger_Pull		161	// (X360) left analog trigger, pulled by any amount // (analog value)
#constant EInputActionOrigin_XBox360_LeftTrigger_Click		162	// (X360) left analog trigger, pulled in all the way // (digital value)
#constant EInputActionOrigin_XBox360_RightTrigger_Pull		163	// (X360) right analog trigger, pulled by any amount // (analog value)
#constant EInputActionOrigin_XBox360_RightTrigger_Click		164	// (X360) right analog trigger, pulled in all the way // (digital value)
#constant EInputActionOrigin_XBox360_LeftStick_Move			165	// (X360) left joystick, movement on any axis // (analog value)
#constant EInputActionOrigin_XBox360_LeftStick_Click		166	// (X360) left joystick, clicked in // (digital value)
#constant EInputActionOrigin_XBox360_LeftStick_DPadNorth	167	// (X360) left joystick, digital movement // (upper quadrant)
#constant EInputActionOrigin_XBox360_LeftStick_DPadSouth	168	// (X360) left joystick, digital movement // (lower quadrant)
#constant EInputActionOrigin_XBox360_LeftStick_DPadWest		169	// (X360) left joystick, digital movement // (left quadrant)
#constant EInputActionOrigin_XBox360_LeftStick_DPadEast		170	// (X360) left joystick, digital movement // (right quadrant)
#constant EInputActionOrigin_XBox360_RightStick_Move		171	// (X360) right joystick, movement on any axis // (analog value)
#constant EInputActionOrigin_XBox360_RightStick_Click		172	// (X360) right joystick, clicked in // (digital value)
#constant EInputActionOrigin_XBox360_RightStick_DPadNorth	173	// (X360) right joystick, digital movement // (upper quadrant)
#constant EInputActionOrigin_XBox360_RightStick_DPadSouth	174	// (X360) right joystick, digital movement // (lower quadrant)
#constant EInputActionOrigin_XBox360_RightStick_DPadWest	175	// (X360) right joystick, digital movement // (left quadrant)
#constant EInputActionOrigin_XBox360_RightStick_DPadEast	176	// (X360) right joystick, digital movement // (right quadrant)
#constant EInputActionOrigin_XBox360_DPad_North				177	// (X360) digital pad, pressed // (upper quadrant)
#constant EInputActionOrigin_XBox360_DPad_South				178	// (X360) digital pad, pressed // (lower quadrant)
#constant EInputActionOrigin_XBox360_DPad_West				179	// (X360) digital pad, pressed // (left quadrant)
#constant EInputActionOrigin_XBox360_DPad_East				180	// (X360) digital pad, pressed // (right quadrant)
#constant EInputActionOrigin_XBox360_Reserved0				181
#constant EInputActionOrigin_XBox360_Reserved1				182
#constant EInputActionOrigin_XBox360_Reserved2				183
#constant EInputActionOrigin_XBox360_Reserved3				184
#constant EInputActionOrigin_XBox360_Reserved4				185
#constant EInputActionOrigin_XBox360_Reserved5				186
#constant EInputActionOrigin_XBox360_Reserved6				187
#constant EInputActionOrigin_XBox360_Reserved7				188
#constant EInputActionOrigin_XBox360_Reserved8				189
#constant EInputActionOrigin_XBox360_Reserved9				190
#constant EInputActionOrigin_XBox360_Reserved10				191

// Switch - Pro or Joycons used as a single input device.
// This does not apply to a single joycon
#constant EInputActionOrigin_Switch_A						192
#constant EInputActionOrigin_Switch_B						193
#constant EInputActionOrigin_Switch_X						194
#constant EInputActionOrigin_Switch_Y						195
#constant EInputActionOrigin_Switch_LeftBumper				196
#constant EInputActionOrigin_Switch_RightBumper				197
#constant EInputActionOrigin_Switch_Plus					198	//Start
#constant EInputActionOrigin_Switch_Minus					199	//Back
#constant EInputActionOrigin_Switch_Capture					200
#constant EInputActionOrigin_Switch_LeftTrigger_Pull		201
#constant EInputActionOrigin_Switch_LeftTrigger_Click		202
#constant EInputActionOrigin_Switch_RightTrigger_Pull		203
#constant EInputActionOrigin_Switch_RightTrigger_Click		204
#constant EInputActionOrigin_Switch_LeftStick_Move			205
#constant EInputActionOrigin_Switch_LeftStick_Click			206
#constant EInputActionOrigin_Switch_LeftStick_DPadNorth		207
#constant EInputActionOrigin_Switch_LeftStick_DPadSouth		208
#constant EInputActionOrigin_Switch_LeftStick_DPadWest		209
#constant EInputActionOrigin_Switch_LeftStick_DPadEast		210
#constant EInputActionOrigin_Switch_RightStick_Move			211
#constant EInputActionOrigin_Switch_RightStick_Click		212
#constant EInputActionOrigin_Switch_RightStick_DPadNorth	213
#constant EInputActionOrigin_Switch_RightStick_DPadSouth	214
#constant EInputActionOrigin_Switch_RightStick_DPadWest		215
#constant EInputActionOrigin_Switch_RightStick_DPadEast		216
#constant EInputActionOrigin_Switch_DPad_North				217
#constant EInputActionOrigin_Switch_DPad_South				218
#constant EInputActionOrigin_Switch_DPad_West				219
#constant EInputActionOrigin_Switch_DPad_East				220
#constant EInputActionOrigin_Switch_ProGyro_Move			221	// Primary Gyro in Pro Controller, or Right JoyCon
#constant EInputActionOrigin_Switch_ProGyro_Pitch			222	// Primary Gyro in Pro Controller, or Right JoyCon
#constant EInputActionOrigin_Switch_ProGyro_Yaw				223	// Primary Gyro in Pro Controller, or Right JoyCon
#constant EInputActionOrigin_Switch_ProGyro_Roll			224	// Primary Gyro in Pro Controller, or Right JoyCon
#constant EInputActionOrigin_Switch_Reserved0				225
#constant EInputActionOrigin_Switch_Reserved1				226
#constant EInputActionOrigin_Switch_Reserved2				227
#constant EInputActionOrigin_Switch_Reserved3				228
#constant EInputActionOrigin_Switch_Reserved4				229
#constant EInputActionOrigin_Switch_Reserved5				230
#constant EInputActionOrigin_Switch_Reserved6				231
#constant EInputActionOrigin_Switch_Reserved7				232
#constant EInputActionOrigin_Switch_Reserved8				233
#constant EInputActionOrigin_Switch_Reserved9				234
#constant EInputActionOrigin_Switch_Reserved10				235

// Switch JoyCon Specific
#constant EInputActionOrigin_Switch_RightGyro_Move			236	// Right JoyCon Gyro generally should correspond to Pro's single gyro
#constant EInputActionOrigin_Switch_RightGyro_Pitch			237	// Right JoyCon Gyro generally should correspond to Pro's single gyro
#constant EInputActionOrigin_Switch_RightGyro_Yaw			238	// Right JoyCon Gyro generally should correspond to Pro's single gyro
#constant EInputActionOrigin_Switch_RightGyro_Roll			239	// Right JoyCon Gyro generally should correspond to Pro's single gyro
#constant EInputActionOrigin_Switch_LeftGyro_Move			240
#constant EInputActionOrigin_Switch_LeftGyro_Pitch			241
#constant EInputActionOrigin_Switch_LeftGyro_Yaw			242
#constant EInputActionOrigin_Switch_LeftGyro_Roll			243
#constant EInputActionOrigin_Switch_LeftGrip_Lower			244	// Left JoyCon SR Button
#constant EInputActionOrigin_Switch_LeftGrip_Upper			245	// Left JoyCon SL Button
#constant EInputActionOrigin_Switch_RightGrip_Lower			246	// Right JoyCon SL Button
#constant EInputActionOrigin_Switch_RightGrip_Upper			247	// Right JoyCon SR Button
#constant EInputActionOrigin_Switch_Reserved11				248
#constant EInputActionOrigin_Switch_Reserved12				249
#constant EInputActionOrigin_Switch_Reserved13				250
#constant EInputActionOrigin_Switch_Reserved14				251
#constant EInputActionOrigin_Switch_Reserved15				252
#constant EInputActionOrigin_Switch_Reserved16				253
#constant EInputActionOrigin_Switch_Reserved17				254
#constant EInputActionOrigin_Switch_Reserved18				255
#constant EInputActionOrigin_Switch_Reserved19				256
#constant EInputActionOrigin_Switch_Reserved20				257

#constant EInputActionOrigin_Count							258	// The number of values in this enum, useful for iterating.

// EInputSourceMode
// The virtual input mode imposed by the configurator upon a input source. 
// For instance, the configurator can make an analog joystick behave like a Dpad with four digital inputs; the EInputSource would be EInputSource_Joystick and the EInputSourceMode would be EInputSourceMode_Dpad.
// The mode also changes the input data received by any associated actions.
#constant EInputSourceMode_None				0	// No input mode.
#constant EInputSourceMode_Dpad				1	// A digital pad -- four digital directional buttons fused together in a cross pattern, such that only one button from each axis can be pressed at any given time.
#constant EInputSourceMode_Buttons			2	// ???
#constant EInputSourceMode_FourButtons		3	// Four digital face buttons, any of which can be pressed simultaneously
#constant EInputSourceMode_AbsoluteMouse	4	
#constant EInputSourceMode_RelativeMouse	5	
#constant EInputSourceMode_JoystickMove		6	
#constant EInputSourceMode_JoystickMouse	7	
#constant EInputSourceMode_JoystickCamera	8	
#constant EInputSourceMode_ScrollWheel		9	
#constant EInputSourceMode_Trigger			10	
#constant EInputSourceMode_TouchMenu		11	
#constant EInputSourceMode_MouseJoystick	12	
#constant EInputSourceMode_MouseRegion		13	
#constant EInputSourceMode_RadialMenu		14	
#constant EInputSourceMode_SingleButton		15	
#constant EInputSourceMode_Switches			16	

// EXboxOrigin
#constant EXboxOrigin_A						0
#constant EXboxOrigin_B						1
#constant EXboxOrigin_X						2
#constant EXboxOrigin_Y						3
#constant EXboxOrigin_LeftBumper			4
#constant EXboxOrigin_RightBumper			5
#constant EXboxOrigin_Menu					6	//Start
#constant EXboxOrigin_View					7	//Back
#constant EXboxOrigin_LeftTrigger_Pull		8
#constant EXboxOrigin_LeftTrigger_Click		9
#constant EXboxOrigin_RightTrigger_Pull		10
#constant EXboxOrigin_RightTrigger_Click	11
#constant EXboxOrigin_LeftStick_Move		12
#constant EXboxOrigin_LeftStick_Click		13
#constant EXboxOrigin_LeftStick_DPadNorth	14
#constant EXboxOrigin_LeftStick_DPadSouth	15
#constant EXboxOrigin_LeftStick_DPadWest	16
#constant EXboxOrigin_LeftStick_DPadEast	17
#constant EXboxOrigin_RightStick_Move		18
#constant EXboxOrigin_RightStick_Click		19
#constant EXboxOrigin_RightStick_DPadNorth	20
#constant EXboxOrigin_RightStick_DPadSouth	21
#constant EXboxOrigin_RightStick_DPadWest	22
#constant EXboxOrigin_RightStick_DPadEast	23
#constant EXboxOrigin_DPad_North			24
#constant EXboxOrigin_DPad_South			25
#constant EXboxOrigin_DPad_West				26
#constant EXboxOrigin_DPad_East				27
#constant EXboxOrigin_Count					28

// ESteamInputType
// Represents the device model for a given piece of hardware.
#constant ESteamInputType_Unknown				0	// Catch-all for unrecognized devices
#constant ESteamInputType_SteamController		1	// Valve's Steam Controller
#constant ESteamInputType_XBox360Controller		2	// Microsoft's XBox 360 Controller
#constant ESteamInputType_XBoxOneController		3	// Microsoft's XBox One Controller
#constant ESteamInputType_GenericGamepad		4	// DirectInput controllers
#constant ESteamInputType_PS4Controller			5	// Sony's Playstation 4 Controller
#constant ESteamInputType_AppleMFiController	6	// Unused
#constant ESteamInputType_AndroidController		7	// Unused
#constant ESteamInputType_SwitchJoyConPair		8	// Unused
#constant ESteamInputType_SwitchJoyConSingle	9	// Unused
#constant ESteamInputType_SwitchProController	10
#constant ESteamInputType_MobileTouch			11	// Steam Link App On-screen Virtual Controller
#constant ESteamInputType_PS3Controller			12	// Currently uses PS4 Origins
#constant ESteamInputType_Count					13
//#constant ESteamInputType_MaximumPossibleValue	255

// ESteamControllerPad
// A touchpad region on a Steam Controller Device.
#constant ESteamControllerPad_Left	0	// The left touchpad region on a Steam Controller Device. Compatible models: VSC, DS4
#constant ESteamControllerPad_Right	1	// The right region on a Steam Controller Device. Compatible models: VSC, DS4

////////////////////////////////////////////////////////////////////////////////
// ISteamInventory
// https://partner.steamgames.com/doc/api/ISteamInventory
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Structs
//------------------------------------------------------------------------------

// Returned when you have requested the list of "eligible" promo items that can be manually granted to the given user.
// Associated Functions: RequestEligiblePromoItemDefinitionsIDs
Type SteamInventoryEligiblePromoItemDefIDs_t
	Result as integer	// EResult. k_EResultOK upon success, any other value indicates failure.
	SteamID as integer	// The Steam ID of the user who these items are for.
	NumEligiblePromoItemDefs as integer	// The number of eligible promo items available for this user.
	CachedData as integer	// Indicates that the data was retrieved from the cache and not the server.
EndType

// Triggered when GetAllItems successfully returns a result which is newer / fresher than the last known result.
Type SteamInventoryFullUpdate_t
	Handle as integer	// A new inventory result handle.
EndType

// This is fired whenever an inventory result transitions from k_EResultPending to any other completed state, see GetResultStatus for the complete list of states.
Type SteamInventoryResultReady_t
	Handle as integer	// The inventory result which is now ready.
	Result as integer	// EResult. The new status of the handle.
EndType

// Returned after StartPurchase is called.
Type SteamInventoryStartPurchaseResult_t
	Result as integer	// EResult. k_EResultOK upon success, any other value indicates failure.
	OrderID as integer	// The auto-generated order id for the initiated purchase.
	TransID as integer	// The auto-generated transaction id for the initiated purchase.
EndType

// Returned after RequestPrices is called.
Type SteamInventoryRequestPricesResult_t
	Result as integer	// EResult. k_EResultOK upon success, any other value indicates failure.
	Currency as string	// The string representing the user's local currency code.
EndType

Type SteamItemDetails_t
	ItemId as integer	// The globally unique item instance handle.
	Definition as integer	// The item definition number for this item.
	Quantity as integer	// The current quantity of the item.
	Flags as integer	// This is a bitmasked collection of ESteamItemFlags.
EndType

//------------------------------------------------------------------------------
// Enums and Constants
//------------------------------------------------------------------------------

// ESteamItemFlags
// These are bitflags that are set in SteamItemDetails_t.
#constant ESteamItemNoTrade	1	// This item is account-locked and cannot be traded or given away.	// 1 << 0
#constant ESteamItemRemoved	0x100	// The item has been destroyed, traded away, expired, or otherwise invalidated.	// 1 << 8
#constant ESteamItemConsumed	0x200	// The item quantity has been decreased by 1 via ConsumeItem API.	// 1 << 9

////////////////////////////////////////////////////////////////////////////////
// ISteamMatchmaking
// https://partner.steamgames.com/doc/api/ISteamMatchmaking
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Structs
//------------------------------------------------------------------------------

Type FavoritesListAccountsUpdated_t
	Result as integer	// EResult. 
EndType

// A server was added/removed from the favorites list, you should refresh now.
Type FavoritesListChanged_t
	IP as string	// An IP of 0 means reload the whole list, any other value means just one server.
	QueryPort as integer	// If m_nIP is set then this is the new servers query port, in host order.
	ConnPort as integer	// If m_nIP is set then this is the new servers connection port, in host order.
	AppID as integer	// If m_nIP is set then this is the App ID the game server belongs to.
	Flags as integer	// If m_nIP is set then this returns whether the the server is on the favorites list or the history list.
	Add as integer	// If m_nIP is set then this is whether the server was added to the list (true) or removed (false) from it.
	AccountId as integer
EndType

// A chat (text or binary) message for this lobby has been received. 
// Associated Functions: SendLobbyChatMsg
Type LobbyChatMsg_t
	SteamIDLobby as integer		// The Steam ID handle of the lobby this message was sent in.
	SteamIDUser as integer		// Steam ID of the user who sent this message. Note that it could have been the local user.
	ChatEntryType as integer	// Type of message received. This is actually a EChatEntryType.
	ChatEntry as string			// The text of the message.
EndType

// A lobby chat room state has changed, this is usually sent when a user has joined or left the lobby.
Type LobbyChatUpdate_t
	SteamIDLobby as integer	// The Steam ID of the lobby.
	SteamIDUserChanged as integer	// The user who's status in the lobby just changed - can be recipient.
	SteamIDMakingChange as integer	// Chat member who made the change.
	ChatMemberStateChange as integer	// Bitfield of EChatMemberStateChange values.
EndType

// Result of our request to create a Lobby.
// Associated Functions: CreateLobby
Type LobbyCreated_t
	Result as integer	// EResult. The result of the operation.
	SteamIDLobby as integer	// The Steam ID of the lobby that was created, 0 if failed.
EndType

// The lobby metadata has changed.
// Associated Functions: CreateLobby
Type LobbyDataUpdate_t
	SteamIDLobby as integer	// The Steam ID of the Lobby.
	SteamIDMember as integer	// Steam ID of either the member whose data changed, or the room itself.
	Success as integer	// true if the lobby data was successfully changed, otherwise false.
EndType

// Recieved upon attempting to enter a lobby.
// Associated Functions: CreateLobby
Type LobbyEnter_t
	SteamIDLobby as integer	// The steam ID of the Lobby you have entered.
	ChatPermissions as integer	// Unused - Always 0.
	Locked as integer	// If true, then only invited users may join.
	ChatRoomEnterResponse as integer	// This is actually a EChatRoomEnterResponse value.
EndType

// A game server has been set via SetLobbyGameServer for all of the members of the lobby to join.
// Associated Functions: SetLobbyGameServer
Type LobbyGameCreated_t
	SteamIDLobby as integer	// The lobby that set the game server.
	SteamIDGameServer as integer	// The Steam ID of the game server, if it's set.
	IP as string	// The IP address of the game server in host order, i.e 127.0.0.1 == 0x7f000001, if it's set.
	Port as integer	// The connection port of the game server, in host order, if it's set.
EndType

// Associated Fcuntion: GetLobbyGameServerJSON
Type LobbyGameServer_t
	GameServerIP as string			// IP of the game server (if any)
	GameServerPort as integer		// The port of the game server (if any)
	SteamIDGameServer as integer	// The game server.
EndType

// Someone has invited you to join a Lobby.
Type LobbyInvite_t
	SteamIDUser as integer	// Steam ID of the person that sent the invite.
	SteamIDLobby as integer	// Steam ID of the lobby we're invited to.
	GameID as integer	// Game ID of the lobby we're invited to.
EndType

// Favorite game information.
// Returned by GetFavoriteGameJSON.
// This isn't from the Steamworks SDK, but compensates for the values being returned via parameters.
// Associated functions: GetFavoriteGameJSON
Type FavoriteGame_t
	AppID as integer
	IP as string
	ConnPort as integer
	QueryPort as integer
	Flags as integer
	TimeLastPlayedOnServer as integer
EndType

//------------------------------------------------------------------------------
// Enums and Constants
//------------------------------------------------------------------------------

// EChatMemberStateChange
// Flags describing how a users lobby state has changed.
#constant EChatMemberStateChangeEntered	0x0001	// This user has joined or is joining the lobby.
#constant EChatMemberStateChangeLeft	0x0002	// This user has left or is leaving the lobby.
#constant EChatMemberStateChangeDisconnected	0x0004	// User disconnected without leaving the lobby first.
#constant EChatMemberStateChangeKicked	0x0008	// The user has been kicked.
#constant EChatMemberStateChangeBanned	0x0010	// The user has been kicked and banned.

// ELobbyComparison
// Lobby search filter options.
#constant ELobbyComparisonEqualToOrLessThan	-2	// The lobbies value must be equal to or less than this one.
#constant ELobbyComparisonLessThan	-1	// The lobbies value must be less than this one.
#constant ELobbyComparisonEqual	0	// The lobbies value must match this this one exactly.
#constant ELobbyComparisonGreaterThan	1	// The lobbies value must be greater than this one.
#constant ELobbyComparisonEqualToOrGreaterThan	2	// The lobbies value must be equal to or greater than this one.
#constant ELobbyComparisonNotEqual	3	// The lobbies value must not match this this.

// ELobbyDistanceFilter
// Lobby search distance filters when requesting the lobby list.
#constant ELobbyDistanceFilterClose	0	// Only lobbies in the same immediate region will be returned.
#constant ELobbyDistanceFilterDefault	1	// Only lobbies in the same region or nearby regions will be returned.
#constant ELobbyDistanceFilterFar	2	// For games that don't have many latency requirements, will return lobbies about half-way around the globe.
#constant ELobbyDistanceFilterWorldwide	3	// No filtering, will match lobbies as far as India to NY (not recommended, expect multiple seconds of latency between the clients).

// ELobbyType
// Specifies the lobby type, this is set from CreateLobby and SetLobbyType.
#constant ELobbyTypePrivate	0	// The only way to join the lobby is from an invite.
#constant ELobbyTypeFriendsOnly	1	// Joinable by friends and invitees, but does not show up in the lobby list.
#constant ELobbyTypePublic	2	// Returned by search and visible to friends.
#constant ELobbyTypeInvisible	3	// Returned by search, but not visible to other friends.

#constant FavoriteFlagNone						0x00	// This favorite game server has no flags set.
#constant FavoriteFlagFavorite					0x01	// This favorite game server entry is for the favorites list.
#constant FavoriteFlagHistory					0x02	// This favorite game server entry is for the history list.

////////////////////////////////////////////////////////////////////////////////
// ISteamMatchmakingServers
// https://partner.steamgames.com/doc/api/ISteamMatchmakingServers
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Structs
//------------------------------------------------------------------------------

// Store key/value pair used in matchmaking queries.
Type MatchMakingKeyValuePair_t
	Key as string	// Filter operation code.
	Value as string	// Operation operand.
EndType

//------------------------------------------------------------------------------
// Enums and Constants
//------------------------------------------------------------------------------

// EMatchMakingServerResponse
// Provided as the return response when refreshing Matchmaking Servers.
#constant EServerResponded	0	// The server responded successfully.
#constant EServerFailedToRespond	1	// The server failed to respond.
#constant ENoServersListedOnMasterServer	2	// Only for the internet query type, returned in response callback if no servers of this type match.

////////////////////////////////////////////////////////////////////////////////
// ISteamMusic
// https://partner.steamgames.com/doc/api/ISteamMusic
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Structs
//------------------------------------------------------------------------------

Type VolumeHasChanged_t
	NewVolume as float
EndType

//------------------------------------------------------------------------------
// Enums and Constants
//------------------------------------------------------------------------------

// AudioPlayback_Status
// Specifies the current playback status.
#constant AudioPlayback_Undefined	0	// The Steam music interface probably isn't enabled.
#constant AudioPlayback_Playing	1	// Steam Music is currently playing.
#constant AudioPlayback_Paused	2	// Steam Music is currently paused.
#constant AudioPlayback_Idle	3	// Steam Music is currently stopped.

////////////////////////////////////////////////////////////////////////////////
// ISteamMusicRemote
// https://partner.steamgames.com/doc/api/ISteamMusicRemote
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Structs
//------------------------------------------------------------------------------

Type MusicPlayerSelectsPlaylistEntry_t
	ID as integer
EndType

Type MusicPlayerSelectsQueueEntry_t
	ID as integer
EndType

Type MusicPlayerWantsLooped_t
	Looped as integer
EndType

Type MusicPlayerWantsPlayingRepeatStatus_t
	PlayingRepeatStatus as integer
EndType

Type MusicPlayerWantsShuffled_t
	Shuffled as integer
EndType

Type MusicPlayerWantsVolume_t
	NewVolume as float
EndType

////////////////////////////////////////////////////////////////////////////////
// ISteamNetworking
// https://partner.steamgames.com/doc/api/ISteamNetworking
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Structs
//------------------------------------------------------------------------------

// Called when packets can't get through to the specified user.
Type P2PSessionConnectFail_t
	SteamIDRemote as integer	// User we were trying to send the packets to.
	P2PSessionError as integer	// Indicates the reason why we're having trouble.
EndType

// A user wants to communicate with us over the P2P channel via the SendP2PPacket.
// Associated Functions: SendP2PPacket
Type P2PSessionRequest_t
	SteamIDRemote as integer	// The user who wants to start a P2P session with us.
EndType

// Called when the status of a socket has changed, used as part of the CreateListenSocket and CreateP2PConnectionSocket calls.
Type SocketStatusCallback_t
	Socket as integer	// the socket used to send/receive data to the remote host
	ListenSocket as integer	// this is the server socket that we were listening on; NULL if this was an outgoing connection
	SteamIDRemote as integer	// remote Steam ID we have connected to, if it has one
	SNetSocketState as integer	// socket state, ESNetSocketState
EndType

// Connection state to a specified user, returned by GetP2PSessionState.
Type P2PSessionState_t
	ConnectionActive as integer	// Do we have an active open connection with the user (true) or not (false)?
	Connecting as integer	// Are we currently trying to establish a connection with the user (true) or not (false)?
	P2PSessionError as integer	// Last error recorded on the socket.
	UsingRelay as integer	// Is this connection going through a Steam relay server (true) or not (false)?
	BytesQueuedForSend as integer	// The number of bytes queued up to be sent to the user.
	PacketsQueuedForSend as integer	// The number of packets queued up to be sent to the user.
	RemoteIP as string	// The IP of remote host if set.
	RemotePort as integer	// The Port of remote host if set.
EndType

//------------------------------------------------------------------------------
// Enums and Constants
//------------------------------------------------------------------------------

// EP2PSend
// Specifies the send type of SendP2PPacket.
#constant EP2PSendUnreliable	0	// Basic UDP send.
#constant EP2PSendUnreliableNoDelay	1	// As above, but if the underlying P2P connection isn't yet established the packet will just be thrown away.
#constant EP2PSendReliable	2	// Reliable message send.
#constant EP2PSendReliableWithBuffering	3	// As above, but applies the Nagle algorithm to the send - sends will accumulate until the current MTU size (typically ~1200 bytes, but can change) or ~200ms has passed (Nagle algorithm).

// EP2PSessionError
// List of possible errors returned by SendP2PPacket , these will be sent in the P2PSessionConnectFail_t callback.
#constant EP2PSessionErrorNone	0	// There was no error.
#constant EP2PSessionErrorNotRunningApp	1	// The target user is not running the same game.
#constant EP2PSessionErrorNoRightsToApp	2	// The local user doesn't own the app that is running.
#constant EP2PSessionErrorDestinationNotLoggedIn	3	// Target user isn't connected to Steam.
#constant EP2PSessionErrorTimeout	4	// The connection timed out because the target user didn't respond, perhaps they aren't calling AcceptP2PSessionWithUser.
#constant EP2PSessionErrorMax	5	// Unused.

// ESNetSocketConnectionType
// Describes how the socket is currently connected.
#constant ESNetSocketConnectionTypeNotConnected	0
#constant ESNetSocketConnectionTypeUDP	1
#constant ESNetSocketConnectionTypeUDPRelay	2

// ESNetSocketState
// Connection progress indicators, used by CreateP2PConnectionSocket.
#constant ESNetSocketStateInvalid	0
#constant ESNetSocketStateConnected	1	// communication is valid
#constant ESNetSocketStateInitiated	10	// states while establishing a connectionthe connection state machine has started
#constant ESNetSocketStateLocalCandidatesFound	11	// p2p connectionswe've found our local IP info
#constant ESNetSocketStateReceivedRemoteCandidates	12	// we've received information from the remote machine, via the Steam back-end, about their IP info
#constant ESNetSocketStateChallengeHandshake	15	// direct connectionswe've received a challenge packet from the server
#constant ESNetSocketStateDisconnecting	21	// failure statesthe API shut it down, and we're in the process of telling the other end
#constant ESNetSocketStateLocalDisconnect	22	// the API shut it down, and we've completed shutdown
#constant ESNetSocketStateTimeoutDuringConnect	23	// we timed out while trying to creating the connection
#constant ESNetSocketStateRemoteEndDisconnected	24	// the remote end has disconnected from us
#constant ESNetSocketStateConnectionBroken	25	// connection has been broken; either the other end has disappeared or our local network connection has broke

////////////////////////////////////////////////////////////////////////////////
// ISteamNetworkingSockets
// https://partner.steamgames.com/doc/api/ISteamNetworkingSockets
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// ISteamParties
// https://partner.steamgames.com/doc/api/ISteamParties
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Structs
//------------------------------------------------------------------------------

// This callback is used as a call response for JoinParty.
Type JoinPartyCallback_t
	Result as integer	// EResult. The result of the attempt to join the party.
	BeaconID as integer	// Beacon ID used in the attempt.
	SteamIDBeaconOwner as integer	// Creator of the beacon used in the attempt.
	ConnectString as string	// If successful, a "join game" string for your game to use to complete the process of joining the desired party
EndType

// This callback is used as a call response for CreateBeacon.
Type CreateBeaconCallback_t
	Result as integer	// EResult. The result of the attempt to create a beacon.
	BeaconID as integer	// Beacon ID of the newly created beacon.
EndType

// After creating a beacon, when a user "follows" that beacon Steam will send you this callback to know that you should be prepared for the user to join your game.
Type ReservationNotificationCallback_t
	BeaconID as integer	// Beacon ID of your beacon.
	SteamIDJoiner as integer	// SteamID of the user following your beacon.
EndType

// Call result for ChangeNumOpenSlots.
Type ChangeNumOpenSlotsCallback_t
	Result as integer	// EResult. The result of the attempt to change the number of open slots.
EndType

Type SteamPartyBeaconLocation_t
	LocationType as integer	// ESteamPartyBeaconLocationType. The location type.
	LocationID as integer	// Opaque identifier of this location.
EndType

//------------------------------------------------------------------------------
// Enums and Constants
//------------------------------------------------------------------------------

// ESteamPartyBeaconLocationType
// Types of locations where beacons can be posted.
#constant ESteamPartyBeaconLocationType_Invalid	0	// Invalid location type.
#constant ESteamPartyBeaconLocationType_ChatGroup	1	// A Steam Chat Room Group.
//#constant ESteamPartyBeaconLocationType_Max		// Value is always one greater than the largest valid location type value.

// ESteamPartyBeaconLocationData
// Types of extended metadata for beacon locations.
#constant ESteamPartyBeaconLocationDataInvalid	0	// Invalid location data type.
#constant ESteamPartyBeaconLocationDataName	1	// The name, if any, of the location
#constant ESteamPartyBeaconLocationDataIconURLSmall	2	// If the location has an associated icon, this is the URL for the small format icon image.	// 
#constant ESteamPartyBeaconLocationDataIconURLMedium	3	// If the location has an associated icon, this is the URL for the medium format icon image.	// 
#constant ESteamPartyBeaconLocationDataIconURLLarge	4	// If the location has an associated icon, this is the URL for the small large icon image.	// 

////////////////////////////////////////////////////////////////////////////////
// ISteamRemoteStorage
// https://partner.steamgames.com/doc/api/ISteamRemoteStorage
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Structs
//------------------------------------------------------------------------------

// Associated Functions: UGCDownload
Type RemoteStorageDownloadUGCResult_t
	Result as integer	// EResult. The result of the operation.
	File as integer	// The handle to the file that was attempted to be downloaded.
	AppID as integer	// ID of the app that created this file.
	SizeInBytes as integer	// The size of the file that was downloaded, in bytes.
	FileName as string	// The name of the file that was downloaded.
	SteamIDOwner as integer	// Steam ID of the user who created this content.
EndType

// Response when reading a file asyncrounously with FileReadAsync.
// Associated Functions: FileReadAsync
Type RemoteStorageFileReadAsyncComplete_t
	FileReadAsync as integer	// Call handle of the async read which was made, must be passed to FileReadAsyncComplete to get the data.
	Result as integer	// EResult. The result of the operation.
	Offset as integer	// Offset into the file this read was at.
	Read as integer	// Amount of bytes read - will be the <= the amount requested.
EndType

// Associated Functions: FileShare
Type RemoteStorageFileShareResult_t
	Result as integer	// EResult. The result of the operation
	File as integer	// The handle that can be shared with users and features
	Filename as string	// The name of the file that was shared
EndType

// Response when writing a file asyncrounously with FileWriteAsync.
// Associated Functions: FileWriteAsync
Type RemoteStorageFileWriteAsyncComplete_t
	Result as integer	// EResult. The result of the operation.
EndType

Type RemoteStoragePublishedFileSubscribed_t
	PublishedFileId as integer	// The published file id
	AppID as integer	// ID of the app that will consume this file.
EndType

Type RemoteStoragePublishedFileUnsubscribed_t
	PublishedFileId as integer	// The published file id
	AppID as integer	// ID of the app that will consume this file.
EndType

// Called when the user has subscribed to a piece of UGC.
// Associated Functions: SubscribePublishedFile
Type RemoteStorageSubscribePublishedFileResult_t
	Result as integer	// EResult. The result of the operation.
	PublishedFileId as integer	// The workshop item that the user subscribed to.
EndType

// Called when the user has unsubscribed from a piece of UGC.
// Associated Functions: UnsubscribePublishedFile
Type RemoteStorageUnsubscribePublishedFileResult_t
	Result as integer	// EResult. The result of the operation.
	PublishedFileId as integer	// The workshop item that the user unsubscribed from.
EndType

// Cloud file list information
// Obtainable from: GetCloudFileListJSON
Type CloudFileInfo_t
	Name as string		// The file name.
	Size as integer		// The size of the file.
EndType

// Cloud quota information
// Obtainable from: GetCloudQuotaJSON
Type CloudQuota_t
	Available as integer	// The number of bytes available.
	Total as integer		// Total amount of bytes the user has access to.
EndType

//------------------------------------------------------------------------------
// Enums and Constants
//------------------------------------------------------------------------------

// ERemoteStoragePlatform
// Sync Platforms flags.
#constant ERemoteStoragePlatformNone	0	// This file will not be downloaded on any platform.
#constant ERemoteStoragePlatformWindows	1	// This file will download on Windows.	// (1 << 0)
#constant ERemoteStoragePlatformOSX	2	// This file will download on macOS.	// (1 << 1)
#constant ERemoteStoragePlatformPS3	4	// This file will download on the Playstation 3.	// (1 << 2)
#constant ERemoteStoragePlatformLinux	8	// This file will download on SteamOS/Linux.	// (1 << 3)
#constant ERemoteStoragePlatformReserved2	16	// Reserved.	// (1 << 4)
#constant ERemoteStoragePlatformAll	0xffffffff	// This file will download on every platform.

// ERemoteStoragePublishedFileVisibility
// Possible visibility states that a Workshop item can be in.
#constant ERemoteStoragePublishedFileVisibilityPublic	0	// Visible to everyone.
#constant ERemoteStoragePublishedFileVisibilityFriendsOnly	1	// Visible to friends only.
#constant ERemoteStoragePublishedFileVisibilityPrivate	2	// Only visible to the creator.

// EUGCReadAction
// Possible UGC Read Actions used with UGCRead.
#constant EUGCRead_ContinueReadingUntilFinished	0	// Keeps the file handle open unless the last byte is read.
#constant EUGCRead_ContinueReading	1	// Keeps the file handle open.
#constant EUGCRead_Close	2	// Frees the file handle.

// EWorkshopFileType
// The way that a shared file will be shared with the community.
#constant EWorkshopFileTypeFirst	0	// Only used for enumerating.
#constant EWorkshopFileTypeCommunity	0	// Normal Workshop item that can be subscribed to.
#constant EWorkshopFileTypeMicrotransaction	1	// Workshop item that is meant to be voted on for the purpose of selling in-game.
#constant EWorkshopFileTypeCollection	2	// A collection of Workshop items.
#constant EWorkshopFileTypeArt	3	// Artwork.
#constant EWorkshopFileTypeVideo	4	// External video.
#constant EWorkshopFileTypeScreenshot	5	// Screenshot.
#constant EWorkshopFileTypeGame	6	// Unused, used to be for Greenlight game entries
#constant EWorkshopFileTypeSoftware	7	// Unused, used to be for Greenlight software entries.
#constant EWorkshopFileTypeConcept	8	// Unused, used to be for Greenlight concepts.
#constant EWorkshopFileTypeWebGuide	9	// Steam web guide.
#constant EWorkshopFileTypeIntegratedGuide	10	// Application integrated guide.
#constant EWorkshopFileTypeMerch	11	// Workshop merchandise meant to be voted on for the purpose of being sold.
#constant EWorkshopFileTypeControllerBinding	12	// Steam Controller bindings.
//#constant EWorkshopFileTypeSteamworksAccessInvite	13	// Only used internally in Steam.
#constant EWorkshopFileTypeSteamVideo	14	// Steam video.
#constant EWorkshopFileTypeGameManagedItem	15	// Managed completely by the game, not the user, and not shown on the web.
#constant EWorkshopFileTypeMax	16	// Only used for enumerating.

////////////////////////////////////////////////////////////////////////////////
// ISteamScreenshots
// https://partner.steamgames.com/doc/api/ISteamScreenshots
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Structs
//------------------------------------------------------------------------------

// A screenshot successfully written or otherwise added to the library and can now be tagged.
// Associated Functions: WriteScreenshot
Type ScreenshotReady_t
	Handle as integer	// The screenshot handle that has been written.
	Result as integer	// EResult. The result of the operation.
EndType

//------------------------------------------------------------------------------
// Enums and Constants
//------------------------------------------------------------------------------

// EVRScreenshotType
// Describes the type of VR screenshots.
#constant EVRScreenshotType_None	0
#constant EVRScreenshotType_Mono	1
#constant EVRScreenshotType_Stereo	2
#constant EVRScreenshotType_MonoCubemap	3
#constant EVRScreenshotType_MonoPanorama	4
#constant EVRScreenshotType_StereoPanorama	5

////////////////////////////////////////////////////////////////////////////////
// ISteamUGC
// https://partner.steamgames.com/doc/api/ISteamUGC
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Structs
//------------------------------------------------------------------------------

// The result of a call to AddAppDependency.
// Associated Functions: AddAppDependency
Type AddAppDependencyResult_t
	Result as integer	// EResult. The result of the operation.
	PublishedFileId as integer	// The parent workshop item that the dependency was added to.
	AppID as integer	// The app/dlc.
EndType

// The result of a call to AddDependency.
// Associated Functions: AddDependency
Type AddUGCDependencyResult_t
	Result as integer	// EResult. The result of the operation.
	PublishedFileId as integer	// The parent workshop item that the dependency was added to.
	ChildPublishedFileId as integer	// The child workshop item which was added as a dependency to the parent item.
EndType

// Called when a new workshop item has been created.
// Associated Functions: CreateItem
Type CreateItemResult_t
	Result as integer	// EResult. The result of the operation.
	PublishedFileId as integer	// The new items unique ID.
	UserNeedsToAcceptWorkshopLegalAgreement as integer	// Does the user need to accept the Steam Workshop legal agreement (true) or not (false)?
EndType

// Called when a workshop item has been downloaded.
// Associated Functions: DownloadItem
Type DownloadItemResult_t
	AppID as integer	// The app ID associated with this workshop item.
	PublishedFileId as integer	// The workshop item that has finished downloading.
	Result as integer	// EResult. The result of the operation.
EndType

// Called when getting the app dependencies for an item.
// Associated Functions: GetAppDependencies
Type GetAppDependenciesResult_t
	Result as integer	// EResult. The result of the operation.
	PublishedFileId as integer	// The workshop item to get app dependencies for.
	AppIDs as integer	// Array of app dependencies
	NumAppDependencies as integer	// Number of returned app dependencies in m_rgAppIDs
	TotalNumAppDependencies as integer	// Total app dependencies found.
EndType

// Called when an attempt at deleting an item completes.
// Associated Functions: DeleteItem
Type DeleteItemResult_t
	Result as integer	// EResult. The result of the operation.
	PublishedFileId as integer	// The workshop item which was being deleted.
EndType

// Called when getting the users vote status on an item.
// Associated Functions: GetUserItemVote
Type GetUserItemVoteResult_t
	PublishedFileId as integer	// The workshop item to get the vote status.
	Result as integer	// EResult. The result of the operation.
	VotedUp as integer	// Has the user voted the item up?
	VotedDown as integer	// Has the user voted the item down?
	VoteSkipped as integer	// Has the user skipped voting on this item?
EndType

// Called when a workshop item has been installed or updated.
Type ItemInstalled_t
	AppID as integer	// The app ID associated with this workshop item.
	PublishedFileId as integer	// The workshop item that has finished installing.
EndType

// Purpose: The result of a call to RemoveAppDependency.
// Associated Functions: RemoveAppDependency
Type RemoveAppDependencyResult_t
	Result as integer	// EResult. The result of the operation.
	PublishedFileId as integer	// The parent workshop item that the dependency was removed from.
	AppID as integer	// The app/dlc.
EndType

// Purpose: The result of a call to RemoveDependency.
// Associated Functions: RemoveDependency
Type RemoveUGCDependencyResult_t
	Result as integer	// EResult. The result of the operation.
	PublishedFileId as integer	// The parent workshop item that the dependency was removed from.
	ChildPublishedFileId as integer	// The child workshop item which was removed as a dependency from the parent item.
EndType

// Called when the user has voted on an item.
// Associated Functions: SetUserItemVote
Type SetUserItemVoteResult_t
	PublishedFileId as integer	// The workshop item that the user voted on.
	Result as integer	// EResult. The result of the operation.
	VoteUp as integer	// Was the vote up (true) or down (false)?
EndType

// Called when workshop item playtime tracking has started.
// Associated Functions: StartPlaytimeTracking
Type StartPlaytimeTrackingResult_t
	Result as integer	// EResult. The result of the operation.
EndType

// Called when a UGC query request completes.
// Associated Functions: SendQueryUGCRequest
Type SteamUGCQueryCompleted_t
	Handle as integer	// The UGC query handle associated with this call result.
	Result as integer	// EResult. The result of the operation.
	NumResultsReturned as integer	// The number of items returned.
	TotalMatchingResults as integer	// The total number of items that matched the query.
	CachedData as integer	// Indicates whether this data was retrieved from the local on-disk cache.
EndType

// Called when workshop item playtime tracking has stopped.
// Associated Functions: StopPlaytimeTracking
Type StopPlaytimeTrackingResult_t
	Result as integer	// EResult. The result of the operation.
EndType

// Called when an item update has completed.
// Associated Functions: SubmitItemUpdate
Type SubmitItemUpdateResult_t
	Result as integer	// EResult. The result of the operation.
	UserNeedsToAcceptWorkshopLegalAgreement as integer	// Does the user need to accept the Steam Workshop legal agreement?
EndType

// Called when the user has added or removed an item to/from their favorites.
// Associated Functions: AddItemToFavorites
Type UserFavoriteItemsListChanged_t
	PublishedFileId as integer	// The item which was added/removed.
	Result as integer	// EResult. The result of the operation.
	WasAddRequest as integer	// Was it an added (true) or removed (false) from the users favorites?
EndType

// Details for a single published item/UGC.
Type SteamUGCDetails_t
	PublishedFileId as integer	// The globally unique item handle to this piece of UGC.
	Result as integer	// EResult. The result of the operation.
	FileType as integer	// EWorkshopFileType. The type of the item.
	CreatorAppID as integer	// App Id of the app that created this item.
	ConsumerAppID as integer	// App Id of the app that will consume this item.
	Title as string	// The title of the item.
	Description as string	// The description of the item.
	SteamIDOwner as integer	// Steam ID of the user who created this content.
	TimeCreated as integer	// Time when the published item was created, provided in Unix epoch format (time since Jan 1st, 1970).
	TimeUpdated as integer	// Time when the published item was last updated, provided in Unix epoch format (time since Jan 1st, 1970).
	TimeAddedToUserList as integer	// Time when the user added the published item to their list (not always applicable), provided in Unix epoch format (time since Jan 1st, 1970).
	Visibility as integer	// ERemoteStoragePublishedFileVisibility. The visibility of the item.
	Banned as integer	// Whether the item was banned.
	AcceptedForUse as integer	// Whether the developer of this app has specifically flagged this item as accepted in the Workshop.
	TagsTruncated as integer	// Whether the list of tags was too long to be returned in the provided buffer, and were therefore truncated.
	Tags as string	// Comma separated list of all tags associated with this item.
	File as integer	// The handle of the primary file.
	PreviewFile as integer	// The handle of the preview file.
	FileName as string	// The cloud filename of the primary file.
	FileSize as integer	// The file size of the primary file.
	PreviewFileSize as integer	// The file size of the preview file.
	URL as string	// The URL associated with this item.
	VotesUp as integer	// Number of votes up.
	VotesDown as integer	// Number of votes down.
	Score as float	// The bayesian average for up votes / total votes, between [0,1].
	NumChildren as integer	// The number of items in the collection if m_eFileType is k_EWorkshopFileTypeCollection, or the number of items this specific item has a dependency on (see AddDependency).
EndType

//------------------------------------------------------------------------------
// Enums and Constants
//------------------------------------------------------------------------------

// EItemPreviewType
// Flags that specify the type of preview an item has.
#constant EItemPreviewType_Image	0	// Standard web viewable image file.
#constant EItemPreviewType_YouTubeVideo	1	// YouTube video ID.
#constant EItemPreviewType_Sketchfab	2	// Sketchfab model ID.
#constant EItemPreviewType_EnvironmentMap_HorizontalCross	3	// Standard image file containing a map of an environment in a horizontal cross layout (see below).
#constant EItemPreviewType_EnvironmentMap_LatLong	4	// Standard image file containing a map of an environment in a LatLong layout.
#constant EItemPreviewType_ReservedMax	255	// Reserved.

// EItemState
// Specifies an items state.
#constant EItemStateNone	0	// The item is not tracked on client.
#constant EItemStateSubscribed	1	// The current user is subscribed to this item.
#constant EItemStateLegacyItem	2	// The item was created with the old workshop functions in ISteamRemoteStorage.
#constant EItemStateInstalled	4	// Item is installed and usable (but maybe out of date).
#constant EItemStateNeedsUpdate	8	// The items needs an update.
#constant EItemStateDownloading	16	// The item update is currently downloading.
#constant EItemStateDownloadPending	32	// DownloadItem was called for this item, the content isn't available until DownloadItemResult_t is fired.

// EItemStatistic
// Used to retrieve item statistics with GetQueryUGCStatistic.
#constant EItemStatistic_NumSubscriptions	0	// Gets the number of subscriptions.
#constant EItemStatistic_NumFavorites	1	// Gets the number of favorites.
#constant EItemStatistic_NumFollowers	2	// Gets the number of followers.
#constant EItemStatistic_NumUniqueSubscriptions	3	// Gets the number of unique subscriptions.
#constant EItemStatistic_NumUniqueFavorites	4	// Gets the number of unique favorites.
#constant EItemStatistic_NumUniqueFollowers	5	// Gets the number of unique followers.
#constant EItemStatistic_NumUniqueWebsiteViews	6	// Gets the number of unique views the item has on its Steam Workshop page.
#constant EItemStatistic_ReportScore	7	// Gets the number of times the item has been reported.
#constant EItemStatistic_NumSecondsPlayed	8	// Gets the total number of seconds this item has been used across all players.
#constant EItemStatistic_NumPlaytimeSessions	9	// Gets the total number of play sessions this item has been used in.
#constant EItemStatistic_NumComments	10	// Gets the number of comments on the items steam has on its Steam Workshop page.
#constant EItemStatistic_NumSecondsPlayedDuringTimePeriod	11	// Gets the number of seconds this item has been used over the given time period.
#constant EItemStatistic_NumPlaytimeSessionsDuringTimePeriod	12	// Gets the number of sessions this item has been used in over the given time period.

// EItemUpdateStatus
// Specifies the status of a UGCUpdateHandle_t after a call to GetItemUpdateProgress , returned by GetItemUpdateProgress.
#constant EItemUpdateStatusInvalid	0	// The item update handle was invalid, the job might be finished, a SubmitItemUpdateResult_t call result should have been returned for it.
#constant EItemUpdateStatusPreparingConfig	1	// The item update is processing configuration data.
#constant EItemUpdateStatusPreparingContent	2	// The item update is reading and processing content files.
#constant EItemUpdateStatusUploadingContent	3	// The item update is uploading content changes to Steam.
#constant EItemUpdateStatusUploadingPreviewFile	4	// The item update is uploading new preview file image.
#constant EItemUpdateStatusCommittingChanges	5	// The item update is committing all changes.

// EUGCMatchingUGCType
// Specifies the types of UGC to obtain from a call to CreateQueryUserUGCRequest or CreateQueryAllUGCRequest.
#constant EUGCMatchingUGCType_Items	0	// Both Microtransaction items and Ready-to-use items.
#constant EUGCMatchingUGCType_Items_Mtx	1	// Microtransaction items.
#constant EUGCMatchingUGCType_Items_ReadyToUse	2	// Regular in game items that players have uploaded.
#constant EUGCMatchingUGCType_Collections	3	// Shared collections of UGC.
#constant EUGCMatchingUGCType_Artwork	4	// Artwork which has been shared.
#constant EUGCMatchingUGCType_Videos	5	// Videos which have been shared.
#constant EUGCMatchingUGCType_Screenshots	6	// Screenshots which have been shared.
#constant EUGCMatchingUGCType_AllGuides	7	// Both web guides and integrated guides.
#constant EUGCMatchingUGCType_WebGuides	8	// Guides that are only available on the steam community.
#constant EUGCMatchingUGCType_IntegratedGuides	9	// Guides that you can use within your game.
#constant EUGCMatchingUGCType_UsableInGame	10	// Ready-to-use items and integrated guides.
#constant EUGCMatchingUGCType_ControllerBindings	11	// Controller Bindings which have been shared.
#constant EUGCMatchingUGCType_GameManagedItems	12	// Game managed items (Not managed by users.
#constant EUGCMatchingUGCType_All	0xffffffff	// Return everything.	// ~0

// EUGCQuery
// Used with CreateQueryAllUGCRequest to specify the sorting and filtering for queries across all available UGC.
#constant EUGCQuery_RankedByVote	0	// Sort by vote popularity all-time
#constant EUGCQuery_RankedByPublicationDate	1	// Sort by publication date descending
#constant EUGCQuery_AcceptedForGameRankedByAcceptanceDate	2	// Sort by date accepted (for mtx items)
#constant EUGCQuery_RankedByTrend	3	// Sort by vote popularity within the given "trend" period  (set in SetRankedByTrendDays)
#constant EUGCQuery_FavoritedByFriendsRankedByPublicationDate	4	// Filter to items the user's friends have favorited, sorted by publication date descending
#constant EUGCQuery_CreatedByFriendsRankedByPublicationDate	5	// Filter to items created by friends, sorted by publication date descending
#constant EUGCQuery_RankedByNumTimesReported	6	// Sort by report weight descending
#constant EUGCQuery_CreatedByFollowedUsersRankedByPublicationDate	7	// Filter to items created by users that the current user has followed, sorted by publication date descending
#constant EUGCQuery_NotYetRated	8	// Filtered to the user's voting queue
#constant EUGCQuery_RankedByTotalVotesAsc	9	// Sort by total # of votes ascending (used internally for building the user's voting queue)
#constant EUGCQuery_RankedByVotesUp	10	// Sort by number of votes up descending.
#constant EUGCQuery_RankedByTextSearch	11	// Sort by keyword text search relevancy
#constant EUGCQuery_RankedByTotalUniqueSubscriptions	12	// Sort by lifetime total unique # of subscribers descending
#constant EUGCQuery_RankedByPlaytimeTrend	13	// Sort by total playtime in the "trend" period descending (set with SetRankedByTrendDays)
#constant EUGCQuery_RankedByTotalPlaytime	14	// Sort by total lifetime playtime descending.
#constant EUGCQuery_RankedByAveragePlaytimeTrend	15	// Sort by average playtime in the "trend" period descending (set in SetRankedByTrendDays)
#constant EUGCQuery_RankedByLifetimeAveragePlaytime	16	// Soft by lifetime average playtime descending.
#constant EUGCQuery_RankedByPlaytimeSessionsTrend	17	// Sort by number of play sessions in the "trend" period descending (set in SetRankedByTrendDays)
#constant EUGCQuery_RankedByLifetimePlaytimeSessions	18	// Sort by number of lifetime play sessions descending.

// EUserUGCList
// Used with CreateQueryUserUGCRequest to obtain different lists of published UGC for a user.
#constant EUserUGCList_Published	0	// List of files the user has published.
#constant EUserUGCList_VotedOn	1	// List of files the user has voted on.
#constant EUserUGCList_VotedUp	2	// List of files the user has voted up.
#constant EUserUGCList_VotedDown	3	// List of files the user has voted down.
//#constant EUserUGCList_WillVoteLater	4	// Deprecated.
#constant EUserUGCList_Favorited	5	// List of files the user has favorited.
#constant EUserUGCList_Subscribed	6	// List of files the user has subscribed to.
#constant EUserUGCList_UsedOrPlayed	7	// List of files the user has spent time in game with.
#constant EUserUGCList_Followed	8	// List of files the user is following updates for.

// EUserUGCListSortOrder
// Used with CreateQueryUserUGCRequest to specify the sort order for user published UGC lists.
#constant EUserUGCListSortOrder_CreationOrderDesc	0	// Returns items by creation date.
#constant EUserUGCListSortOrder_CreationOrderAsc	1	// Returns items by creation date.
#constant EUserUGCListSortOrder_TitleAsc	2	// Returns items by name.
#constant EUserUGCListSortOrder_LastUpdatedDesc	3	// Returns the most recently updated items first.
#constant EUserUGCListSortOrder_SubscriptionDateDesc	4	// Returns the most recently subscribed items first.
#constant EUserUGCListSortOrder_VoteScoreDesc	5	// Returns the items with the more recent score updates first.
#constant EUserUGCListSortOrder_ForModeration	6	// Returns the items that have been reported for moderation.

////////////////////////////////////////////////////////////////////////////////
// ISteamUser
// https://partner.steamgames.com/doc/api/ISteamUser
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Structs
//------------------------------------------------------------------------------

// Sent by the Steam server to the client telling it to disconnect from the specified game server, which it may be in the process of or already connected to.
Type ClientGameServerDeny_t
	AppID as integer	// The App ID this call is for.
	GameServerIP as string	// The IP of the game server that is telling us to disconnect, in host order, i.e 127.0.0.1 == 0x7f000001.
	GameServerPort as integer	// The port of the game server that is telling us to disconnect, in host order.
	Secure as integer	// Is the game server VAC secure (true) or not (false)?
	Reason as integer
EndType

// Called when an encrypted application ticket has been received.
// Associated Functions: RequestEncryptedAppTicket
Type EncryptedAppTicketResponse_t
	Result as integer	// EResult. Was the call successful?
EndType

// Sent to your game in response to a steam://gamewebcallback/ command from a user clicking a link in the Steam overlay browser.
Type GameWebCallback_t
	URL as string	// The complete url that the user clicked on.
EndType

// Result when creating an auth session ticket.
// Associated Functions: GetAuthSessionTicket
Type GetAuthSessionTicketResponse_t
	AuthTicket as integer	// The handle to the ticket that was created.
	Result as integer	// EResult. The result of the operation.
EndType

// Called when the callback system for this client is in an error state (and has flushed pending callbacks)
Type IPCFailure_t
	FailureType as integer	// This is a EFailureType.
EndType

// Called when a user has responded to a microtransaction authorization request.
Type MicroTxnAuthorizationResponse_t
	AppID as integer	// App ID for this microtransaction
	OrderID as integer	// Order ID provided for the microtransaction.
	Authorized as integer	// Did the user authorize the transaction (1) or not (0)?
EndType

// Called when a connection attempt has failed.
Type SteamServerConnectFailure_t
	Result as integer	// EResult. The reason why the connection failed.
	StillRetrying as integer	// Is the Steam client still trying to connect to the server?
EndType

// Called if the client has lost connection to the Steam servers.
Type SteamServersDisconnected_t
	Result as integer	// EResult. The reason we were disconnected from Steam.
EndType

// Response when we have recieved the authentication URL after a call to RequestStoreAuthURL.
// Associated Functions: RequestStoreAuthURL
Type StoreAuthURLResponse_t
	URL as string	// The authentication URL.
EndType

// Called when an auth ticket has been validated.
// Associated Functions: BeginAuthSession
Type ValidateAuthTicketResponse_t
	SteamID as integer	// The Steam ID of the entity that provided the auth ticket.
	AuthSessionResponse as integer	// EAuthSessionResponse. The result of the validation.
	OwnerSteamID as integer	// The Steam ID that owns the game, this will be different from m_SteamID if the game is being borrowed via Steam Family Sharing.
EndType

//------------------------------------------------------------------------------
// Enums and Constants
//------------------------------------------------------------------------------

// EFailureType
// Specifies what type of failure happened in IPCFailure_t.
#constant EFailureFlushedCallbackQueue	0
#constant EFailurePipeFail	1

////////////////////////////////////////////////////////////////////////////////
// ISteamUserStats
// https://partner.steamgames.com/doc/api/ISteamUserStats
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Structs
//------------------------------------------------------------------------------

// Called when the global achievement percentages have been received from the server.
// Associated Functions: RequestGlobalAchievementPercentages
Type GlobalAchievementPercentagesReady_t
	GameID as integer	// Game ID which these achievement percentages are for.
	Result as integer	// EResult. Result of the request.
EndType

// Called when the global stats have been received from the server.
// Associated Functions: RequestGlobalStats
Type GlobalStatsReceived_t
	GameID as integer	// Game ID which these global stats are for.
	Result as integer	// EResult. The result of the request.
EndType

// Result when finding a leaderboard.
// Associated Functions: FindOrCreateLeaderboard
Type LeaderboardFindResult_t
	SteamLeaderboard as integer	// Handle to the leaderboard that was serarched for.
	LeaderboardFound as integer	// Was the leaderboard found?
EndType

// Called when scores for a leaderboard have been downloaded and are ready to be retrieved.
// Associated Functions: DownloadLeaderboardEntries
Type LeaderboardScoresDownloaded_t
	SteamLeaderboard as integer	// Handle to the leaderboard that these entries belong to.
	SteamLeaderboardEntries as integer	// The handle to pass into GetDownloadedLeaderboardEntry to retrieve the info for each downloaded entry.
	EntryCount as integer	// The number of entries downloaded.
EndType

// Result indicating that a leaderboard score has been uploaded.
// Associated Functions: UploadLeaderboardScore
Type LeaderboardScoreUploaded_t
	Success as integer	// Was the call successful?
	SteamLeaderboard as integer	// Handle to the leaderboard that this score was uploaded to.
	Score as integer	// The score that was attempted to set.
	ScoreChanged as integer	// true if the score on the leaderboard changed otherwise false if the existing score was better.
	GlobalRankNew as integer	// The new global rank of the user on this leaderboard.
	GlobalRankPrevious as integer	// The previous global rank of the user on this leaderboard; 0 if the user had no existing entry in the leaderboard.
EndType

// Result indicating that user generated content has been attached to one of the current user's leaderboard entries.
// Associated Functions: AttachLeaderboardUGC
Type LeaderboardUGCSet_t
	Result as integer	// EResult. The result of the operation.
	SteamLeaderboard as integer	// Handle to the leaderboard that the UGC was attached to.
EndType

// Gets the current number of players for the current AppId.
// Associated Functions: GetNumberOfCurrentPlayers
Type NumberOfCurrentPlayers_t
	Success as integer	// Was the call successful?
	Players as integer	// Number of players currently playing.
EndType

// Called when PS3 trophies are available.
Type PS3TrophiesInstalled_t
	GameID as integer	// Game that these stats are for.
	Result as integer	// EResult. The result of the operation.
	RequiredDiskSpace as integer	// If m_eResult is k_EResultDiskFull, then this will contain the amount of space needed to install trophies.
EndType

// Result of an achievement icon that has been fetched
// Associated Functions: GetAchievementIcon
Type UserAchievementIconFetched_t
	GameID as integer	// The Game ID this achievement is for.
	AchievementName as string	// The name of the achievement that this callback is for.
	Achieved as integer	// Returns whether the icon for the achieved (true) or unachieved (false) version.
	IconHandle as integer	// Handle to the image, which can be used with ISteamUtils::GetImageRGBA to get the image data.
EndType

// Result of a request to store the achievements on the server, or an "indicate progress" call.
// Associated Functions: StoreStats
Type UserAchievementStored_t
	GameID as integer	// Game ID that this achievement is for.
	GroupAchievement as integer	// Unused.
	AchievementName as string	// Name of the achievement.
	CurProgress as integer	// Current progress towards the achievement.
	MaxProgress as integer	// The total amount of progress required to unlock.
EndType

// Called when the latest stats and achievements for a specific user (including the local user) have been received from the server.
// Associated Functions: RequestCurrentStats
Type UserStatsReceived_t
	GameID as integer	// Game ID that these stats are for.
	Result as integer	// EResult. Returns whether the call was successful or not.
	SteamIDUser as integer	// The user whose stats were retrieved.
EndType

// Result of a request to store the user stats.
// Associated Functions: StoreStats
Type UserStatsStored_t
	GameID as integer	// Game ID that these stats are for.
	Result as integer	// EResult. Returns whether the call was successful or not.
EndType

// Callback indicating that a user's stats have been unloaded.
Type UserStatsUnloaded_t
	SteamIDUser as integer	// User whose stats have been unloaded.
EndType

// A single entry in a leaderboard, as returned by GetDownloadedLeaderboardEntry.
Type LeaderboardEntry_t
	SteamIDUser as integer	// User who this entry belongs to.
	GlobalRank as integer	// The global rank of this entry ranging from [1..
	Score as integer	// The raw score as set in the leaderboard.
	Details as integer	// The number of details available for this entry.
	UGC as integer	// Handle for the UGC attached to the entry.
EndType

//------------------------------------------------------------------------------
// Enums and Constants
//------------------------------------------------------------------------------

// ELeaderboardDataRequest
// Type of data request, used when downloading leaderboard entries with DownloadLeaderboardEntries.
#constant ELeaderboardDataRequestGlobal	0	// Used to query for a sequential range of leaderboard entries by leaderboard rank.
#constant ELeaderboardDataRequestGlobalAroundUser	1	// Used to retrieve leaderboard entries relative a user's entry.
#constant ELeaderboardDataRequestFriends	2	// Used to retrieve all leaderboard entries for friends of the current user.
//#constant ELeaderboardDataRequestUsers	3	// Used internally, do not use with DownloadLeaderboardEntries! Doing so is undefined behavior.

// ELeaderboardDisplayType
// The display type used by the Steam Community web site to know how to format the leaderboard scores when displayed.
#constant ELeaderboardDisplayTypeNone	0	// This is only ever used when a leaderboard is invalid, you should never set this yourself.
#constant ELeaderboardDisplayTypeNumeric	1	// The score is just a simple numerical value.
#constant ELeaderboardDisplayTypeTimeSeconds	2	// The score represents a time, in seconds.
#constant ELeaderboardDisplayTypeTimeMilliSeconds	3	// The score represents a time, in milliseconds.

// ELeaderboardSortMethod
// The sort method used to set whether a higher or lower score is better.
#constant ELeaderboardSortMethodNone	0	// Only ever used when a leaderboard is invalid, you should never set this yourself.
#constant ELeaderboardSortMethodAscending	1	// The top-score is the lowest number.
#constant ELeaderboardSortMethodDescending	2	// The top-score is the highest number.

// ELeaderboardUploadScoreMethod
#constant ELeaderboardUploadScoreMethodNone	0
#constant ELeaderboardUploadScoreMethodKeepBest	1	// Leaderboard will keep user's best score
#constant ELeaderboardUploadScoreMethodForceUpdate	2	// Leaderboard will always replace score with specified

////////////////////////////////////////////////////////////////////////////////
// ISteamUtils
// https://partner.steamgames.com/doc/api/ISteamUtils
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Structs
//------------------------------------------------------------------------------

// CallResult for CheckFileSignature.
// Associated Functions: CheckFileSignature
Type CheckFileSignature_t
	CheckFileSignature as integer	// ECheckFileSignature. Contains the result of the file signature check.k_ECheckFileSignatureNoSignaturesFoundForThisApp - This app has not been configured on the signing tab of the partner site to enable this function.k_ECheckFileSignatureNoSignaturesFoundForThisFile - This file is not listed on the signing tab for the partner site.k_ECheckFileSignatureFileNotFound - The file does not exist on disk.k_ECheckFileSignatureInvalidSignature - The file exists, and the signing tab has been set for this file, but the file is either not signed or the signature does not match.k_ECheckFileSignatureValidSignature - The file is signed and the signature is valid.
EndType

// Called when the big picture gamepad text input has been closed.
Type GamepadTextInputDismissed_t
	Submitted as integer	// true if user entered & accepted text (Call GetEnteredGamepadTextInput to receive the text), false if input was canceled.
	SubmittedText as string	// Contains the text if there was any submitted.
EndType

// Called when running on a laptop and less than 10 minutes of battery is left, and then fires then every minute afterwards.
Type LowBatteryPower_t
	MinutesBatteryLeft as integer	// The estimated amount of battery life left in minutes.
EndType

// Called when a SteamAPICall_t has completed (or failed)
Type SteamAPICallCompleted_t
	AsyncCall as integer	// The handle of the Steam API Call that completed.
	Callback as integer	// This is the k_iCallback constant which uniquely identifies the completed callback.
	Param as integer	// The size in bytes of the completed callback.
EndType

//------------------------------------------------------------------------------
// Enums and Constants
//------------------------------------------------------------------------------

// ECheckFileSignature
// The result of a call to CheckFileSignature
#constant ECheckFileSignatureInvalidSignature	0
#constant ECheckFileSignatureValidSignature	1
#constant ECheckFileSignatureFileNotFound	2
#constant ECheckFileSignatureNoSignaturesFoundForThisApp	3
#constant ECheckFileSignatureNoSignaturesFoundForThisFile	4

// EGamepadTextInputLineMode
// Controls number of allowed lines for the Big Picture gamepad text entry
#constant EGamepadTextInputLineModeSingleLine	0
#constant EGamepadTextInputLineModeMultipleLines	1

// EGamepadTextInputMode
// Input modes for the Big Picture gamepad text entry
#constant EGamepadTextInputModeNormal	0
#constant EGamepadTextInputModePassword	1

// ESteamAPICallFailure
// Steam API call failure results returned by GetAPICallFailureReason.
#constant ESteamAPICallFailureNone	-1	// No failure.
#constant ESteamAPICallFailureSteamGone	0	// The local Steam process has stopped responding, it may have been forcefully closed or is frozen.
#constant ESteamAPICallFailureNetworkFailure	1	// The network connection to the Steam servers has been lost, or was already broken.
#constant ESteamAPICallFailureInvalidHandle	2	// The SteamAPICall_t handle passed in no longer exists.
#constant ESteamAPICallFailureMismatchedCallback	3	// GetAPICallResult was called with the wrong callback type for this API call.

////////////////////////////////////////////////////////////////////////////////
// ISteamVideo
// https://partner.steamgames.com/doc/api/ISteamVideo
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Structs
//------------------------------------------------------------------------------

// Automatically called whenever the user stops broadcasting.
Type BroadcastUploadStop_t
	Result as integer	// EBroadcastUploadResult. The reason why the broadcast has stopped.
EndType

// Triggered when the OPF Details for 360 video playback are retrieved.
// Associated Functions: GetOPFSettings
Type GetOPFSettingsResult_t
	Result as integer	// EResult. The result of the operation.
	VideoAppID as integer	// The app ID of the video that we got the details of.
EndType

// Provides the result of a call to GetVideoURL.
// Associated Functions: GetVideoURL
Type GetVideoURLResult_t
	Result as integer	// EResult. The result of the operation.	 k_EResultOK if the call is successful.
	VideoAppID as integer	// The App ID provided in the original call to GetVideoURL.
	URL as string	// If the call was successful this contains the URL to the MPEG-DASH Standard schema formatted xml document which can be used to stream the content.
EndType

////////////////////////////////////////////////////////////////////////////////
// SteamEncryptedAppTicket
// https://partner.steamgames.com/doc/api/SteamEncryptedAppTicket
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// steam_api.h
// https://partner.steamgames.com/doc/api/steam_api
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Enums and Constants
//------------------------------------------------------------------------------

// EAuthSessionResponse
// Callback return values for the ValidateAuthTicketResponse_t callback which is posted as a response to ISteamUser::BeginAuthSession and ISteamGameServer::BeginAuthSession.
#constant EAuthSessionResponseOK	0	// Steam has verified the user is online, the ticket is valid and ticket has not been reused.
#constant EAuthSessionResponseUserNotConnectedToSteam	1	// The user in question is not connected to steam.
#constant EAuthSessionResponseNoLicenseOrExpired	2	// The user doesn't have a license for this App ID or the ticket has expired.
#constant EAuthSessionResponseVACBanned	3	// The user is VAC banned for this game.
#constant EAuthSessionResponseLoggedInElseWhere	4	// The user account has logged in elsewhere and the session containing the game instance has been disconnected.
#constant EAuthSessionResponseVACCheckTimedOut	5	// VAC has been unable to perform anti-cheat checks on this user.
#constant EAuthSessionResponseAuthTicketCanceled	6	// The ticket has been canceled by the issuer.
#constant EAuthSessionResponseAuthTicketInvalidAlreadyUsed	7	// This ticket has already been used, it is not valid.
#constant EAuthSessionResponseAuthTicketInvalid	8	// This ticket is not from a user instance currently connected to steam.
#constant EAuthSessionResponsePublisherIssuedBan	9	// The user is banned for this game.

// EBeginAuthSessionResult
// Results returned from ISteamUser::BeginAuthSession and ISteamGameServer::BeginAuthSession.
#constant EBeginAuthSessionResultOK	0	// Ticket is valid for this game and this Steam ID.
#constant EBeginAuthSessionResultInvalidTicket	1	// The ticket is invalid.
#constant EBeginAuthSessionResultDuplicateRequest	2	// A ticket has already been submitted for this Steam ID.
#constant EBeginAuthSessionResultInvalidVersion	3	// Ticket is from an incompatible interface version.
#constant EBeginAuthSessionResultGameMismatch	4	// Ticket is not for this game.
#constant EBeginAuthSessionResultExpiredTicket	5	// Ticket has expired.

// EBroadcastUploadResult
// Broadcast upload result from BroadcastUploadStop_t.
#constant EBroadcastUploadResultNone	0	// Broadcast state unknown.
#constant EBroadcastUploadResultOK	1	// Broadcast was good, no problems.
#constant EBroadcastUploadResultInitFailed	2	// Broadcast init failed.
#constant EBroadcastUploadResultFrameFailed	3	// Broadcast frame upload failed.
#constant EBroadcastUploadResultTimeout	4	// Broadcast upload timed out.
#constant EBroadcastUploadResultBandwidthExceeded	5	// Broadcast send too much data.
#constant EBroadcastUploadResultLowFPS	6	// Broadcast FPS is too low.
#constant EBroadcastUploadResultMissingKeyFrames	7	// Broadcast sending not enough key frames.
#constant EBroadcastUploadResultNoConnection	8	// Broadcast client failed to connect to relay.
#constant EBroadcastUploadResultRelayFailed	9	// Relay dropped the upload.
#constant EBroadcastUploadResultSettingsChanged	10	// The client changed broadcast settings.
#constant EBroadcastUploadResultMissingAudio	11	// Client failed to send audio data.
#constant EBroadcastUploadResultTooFarBehind	12	// Clients was too slow uploading.
#constant EBroadcastUploadResultTranscodeBehind	13	// Server failed to keep up with transcode.

// EChatEntryType
// Chat Entry Types.
#constant EChatEntryTypeInvalid	0	// Invalid.
#constant EChatEntryTypeChatMsg	1	// Normal text message from another user.
#constant EChatEntryTypeTyping	2	// The other user is typing, not used in multi-user chat.
#constant EChatEntryTypeInviteGame	3	// Invite from other user into that users current game.
//#constant EChatEntryTypeEmote	4	// Text emote message (Deprecated, should be treated as ChatMsg).
#constant EChatEntryTypeLeftConversation	6	// A user has left the conversation (closed the chat window).
#constant EChatEntryTypeEntered	7	// User has entered the conversation, used in multi-user chat and group chat.
#constant EChatEntryTypeWasKicked	8	// User was kicked (Data: Steam ID of the user performing the kick).
#constant EChatEntryTypeWasBanned	9	// User was banned (Data: Steam ID of the user performing the ban).
#constant EChatEntryTypeDisconnected	10	// User disconnected.
#constant EChatEntryTypeHistoricalChat	11	// A chat message from user's chat history or offline message.
#constant EChatEntryTypeLinkBlocked	14	// A link was removed by the chat filter.

// EChatRoomEnterResponse
// Chat Room Enter Responses.
#constant EChatRoomEnterResponseSuccess	1	// Success.
#constant EChatRoomEnterResponseDoesntExist	2	// Chat doesn't exist (probably closed).
#constant EChatRoomEnterResponseNotAllowed	3	// General Denied - You don't have the permissions needed to join the chat.
#constant EChatRoomEnterResponseFull	4	// Chat room has reached its maximum size.
#constant EChatRoomEnterResponseError	5	// Unexpected Error.
#constant EChatRoomEnterResponseBanned	6	// You are banned from this chat room and may not join.
#constant EChatRoomEnterResponseLimited	7	// Joining this chat is not allowed because you are a limited user (no value on account).
#constant EChatRoomEnterResponseClanDisabled	8	// Attempt to join a clan chat when the clan is locked or disabled.
#constant EChatRoomEnterResponseCommunityBan	9	// Attempt to join a chat when the user has a community lock on their account.
#constant EChatRoomEnterResponseMemberBlockedYou	10	// Join failed - a user that is in the chat has blocked you from joining.
#constant EChatRoomEnterResponseYouBlockedMember	11	// Join failed - you have blocked a user that is already in the chat.

// EChatSteamIDInstanceFlags
// Special flags for Chat accounts - they go in the top 8 bits of the Steam ID's "instance", leaving 12 for the actual instances
#constant EChatAccountInstanceMask	0x00000FFF	// Top 8 bits are flags.
#constant EChatInstanceFlagClan	0x00000800	// The Steam ID is for a Steam group.	// ( k_unSteamAccountInstanceMask + 1 ) >> 1
#constant EChatInstanceFlagLobby	0x00000400	// The Steam ID is for a Lobby.	// ( k_unSteamAccountInstanceMask + 1 ) >> 2
#constant EChatInstanceFlagMMSLobby	0x00000200	// The Steam ID is for a MMS Lobby.	// ( k_unSteamAccountInstanceMask + 1 ) >> 3

// EDenyReason
// Result values when a client failed to join or has been kicked from a game server.
#constant EDenyInvalid	0	// Unknown.
#constant EDenyInvalidVersion	1	// The client and server are not the same version.
#constant EDenyGeneric	2	// Generic.
#constant EDenyNotLoggedOn	3	// The client is not logged on.
#constant EDenyNoLicense	4	// The client does not have a license to play this game.
#constant EDenyCheater	5	// The client is VAC banned.
#constant EDenyLoggedInElseWhere	6	// The client is logged in elsewhere.
#constant EDenyUnknownText	7
#constant EDenyIncompatibleAnticheat	8
#constant EDenyMemoryCorruption	9
#constant EDenyIncompatibleSoftware	10
#constant EDenySteamConnectionLost	11	// The server lost connection to steam.
#constant EDenySteamConnectionError	12	// The server had a general error connecting to Steam.
#constant EDenySteamResponseTimedOut	13	// The server timed out connecting to Steam.
#constant EDenySteamValidationStalled	14	// The client has not authed with Steam yet.
#constant EDenySteamOwnerLeftGuestUser	15	// The owner of the shared game has left, called for each guest of the owner.

// EGameIDType
// Used to describe the type of CGameID.
#constant EGameIDTypeApp	0	// The Game ID is a regular steam app.
#constant EGameIDTypeGameMod	1	// The Game ID is a mod.
#constant EGameIDTypeShortcut	2	// The game ID is a shortcut.
#constant EGameIDTypeP2P	3	// The game ID is a P2P file.

// ELaunchOptionType
// Codes for well defined launch options, corresponds to "Launch Type" in the applications Launch Options which can be found on the General Installation Settings page.
#constant ELaunchOptionType_None	0	// Unspecified.
#constant ELaunchOptionType_Default	1	// Runs the app in default mode.
#constant ELaunchOptionType_SafeMode	2	// Runs the game in safe mode.
#constant ELaunchOptionType_Multiplayer	3	// Runs the game in multiplayer mode.
#constant ELaunchOptionType_Config	4	// Runs config tool for this game.
#constant ELaunchOptionType_OpenVR	5	// Runs game in VR mode using OpenVR.
#constant ELaunchOptionType_Server	6	// Runs dedicated server for this game.
#constant ELaunchOptionType_Editor	7	// Runs game editor.
#constant ELaunchOptionType_Manual	8	// Shows game manual.
#constant ELaunchOptionType_Benchmark	9	// Runs game benchmark.
#constant ELaunchOptionType_Option1	10	// Generic run option, uses description field for game name.
#constant ELaunchOptionType_Option2	11	// Generic run option, uses description field for game name.
#constant ELaunchOptionType_Option3	12	// Generic run option, uses description field for game name.
#constant ELaunchOptionType_OculusVR	13	// Runs game in VR mode using the Oculus SDK.
#constant ELaunchOptionType_OpenVROverlay	14	// Runs an OpenVR dashboard overlay.
#constant ELaunchOptionType_OSVR	15	// Runs game in VR mode using the OSVR SDK.
#constant ELaunchOptionType_Dialog	1000	// Show launch options dialog.

// EMarketingMessageFlags
// Internal Steam marketing message flags that change how a client should handle them.
#constant EMarketingMessageFlagsNone	0
#constant EMarketingMessageFlagsHighPriority	1	// 	// 1 << 0
#constant EMarketingMessageFlagsPlatformWindows	2	// 	// 1 << 1
#constant EMarketingMessageFlagsPlatformMac	4	// 	// 1 << 2
#constant EMarketingMessageFlagsPlatformLinux	8	// 	// 1 << 3
//#constant EMarketingMessageFlagsPlatformRestrictions	=	// aggregate flags

// ENotificationPosition
// Possible positions to have the overlay show notifications in.
#constant EPositionTopLeft	0	// Top left corner.
#constant EPositionTopRight	1	// Top right corner.
#constant EPositionBottomLeft	2	// Bottom left corner.
#constant EPositionBottomRight	3	// Bottom right corner.

// EResult
// Steam error result codes.
#constant EResultOK	1	// Success.
#constant EResultFail	2	// Generic failure.
#constant EResultNoConnection	3	// Your Steam client doesn't have a connection to the back-end.
#constant EResultInvalidPassword	5	// Password/ticket is invalid.
#constant EResultLoggedInElsewhere	6	// The user is logged in elsewhere.
#constant EResultInvalidProtocolVer	7	// Protocol version is incorrect.
#constant EResultInvalidParam	8	// A parameter is incorrect.
#constant EResultFileNotFound	9	// File was not found.
#constant EResultBusy	10	// Called method is busy - action not taken.
#constant EResultInvalidState	11	// Called object was in an invalid state.
#constant EResultInvalidName	12	// The name was invalid.
#constant EResultInvalidEmail	13	// The email was invalid.
#constant EResultDuplicateName	14	// The name is not unique.
#constant EResultAccessDenied	15	// Access is denied.
#constant EResultTimeout	16	// Operation timed out.
#constant EResultBanned	17	// The user is VAC2 banned.
#constant EResultAccountNotFound	18	// Account not found.
#constant EResultInvalidSteamID	19	// The Steam ID was invalid.
#constant EResultServiceUnavailable	20	// The requested service is currently unavailable.
#constant EResultNotLoggedOn	21	// The user is not logged on.
#constant EResultPending	22	// Request is pending, it may be in process or waiting on third party.
#constant EResultEncryptionFailure	23	// Encryption or Decryption failed.
#constant EResultInsufficientPrivilege	24	// Insufficient privilege.
#constant EResultLimitExceeded	25	// Too much of a good thing.
#constant EResultRevoked	26	// Access has been revoked (used for revoked guest passes.
#constant EResultExpired	27	// License/Guest pass the user is trying to access is expired.
#constant EResultAlreadyRedeemed	28	// Guest pass has already been redeemed by account, cannot be used again.
#constant EResultDuplicateRequest	29	// The request is a duplicate and the action has already occurred in the past, ignored this time.
#constant EResultAlreadyOwned	30	// All the games in this guest pass redemption request are already owned by the user.
#constant EResultIPNotFound	31	// IP address not found.
#constant EResultPersistFailed	32	// Failed to write change to the data store.
#constant EResultLockingFailed	33	// Failed to acquire access lock for this operation.
#constant EResultLogonSessionReplaced	34	// The logon session has been replaced.
#constant EResultConnectFailed	35	// Failed to connect.
#constant EResultHandshakeFailed	36	// The authentication handshake has failed.
#constant EResultIOFailure	37	// There has been a generic IO failure.
#constant EResultRemoteDisconnect	38	// The remote server has disconnected.
#constant EResultShoppingCartNotFound	39	// Failed to find the shopping cart requested.
#constant EResultBlocked	40	// A user blocked the action.
#constant EResultIgnored	41	// The target is ignoring sender.
#constant EResultNoMatch	42	// Nothing matching the request found.
#constant EResultAccountDisabled	43	// The account is disabled.
#constant EResultServiceReadOnly	44	// This service is not accepting content changes right now.
#constant EResultAccountNotFeatured	45	// Account doesn't have value, so this feature isn't available.
#constant EResultAdministratorOK	46	// Allowed to take this action, but only because requester is admin.
#constant EResultContentVersion	47	// A Version mismatch in content transmitted within the Steam protocol.
#constant EResultTryAnotherCM	48	// The current CM can't service the user making a request, user should try another.
#constant EResultPasswordRequiredToKickSession	49	// You are already logged in elsewhere, this cached credential login has failed.
#constant EResultAlreadyLoggedInElsewhere	50	// The user is logged in elsewhere.
#constant EResultSuspended	51	// Long running operation has suspended/paused.
#constant EResultCancelled	52	// Operation has been canceled, typically by user.
#constant EResultDataCorruption	53	// Operation canceled because data is ill formed or unrecoverable.
#constant EResultDiskFull	54	// Operation canceled - not enough disk space.
#constant EResultRemoteCallFailed	55	// The remote or IPC call has failed.
#constant EResultPasswordUnset	56	// Password could not be verified as it's unset server side.
#constant EResultExternalAccountUnlinked	57	// External account (PSN, Facebook...) is not linked to a Steam account.
#constant EResultPSNTicketInvalid	58	// PSN ticket was invalid.
#constant EResultExternalAccountAlreadyLinked	59	// External account (PSN, Facebook...) is already linked to some other account, must explicitly request to replace/delete the link first.
#constant EResultRemoteFileConflict	60	// The sync cannot resume due to a conflict between the local and remote files.
#constant EResultIllegalPassword	61	// The requested new password is not allowed.
#constant EResultSameAsPreviousValue	62	// New value is the same as the old one.
#constant EResultAccountLogonDenied	63	// Account login denied due to 2nd factor authentication failure.
#constant EResultCannotUseOldPassword	64	// The requested new password is not legal.
#constant EResultInvalidLoginAuthCode	65	// Account login denied due to auth code invalid.
#constant EResultAccountLogonDeniedNoMail	66	// Account login denied due to 2nd factor auth failure - and no mail has been sent.
#constant EResultHardwareNotCapableOfIPT	67	// The users hardware does not support Intel's Identity Protection Technology (IPT).
#constant EResultIPTInitError	68	// Intel's Identity Protection Technology (IPT) has failed to initialize.
#constant EResultParentalControlRestricted	69	// Operation failed due to parental control restrictions for current user.
#constant EResultFacebookQueryError	70	// Facebook query returned an error.
#constant EResultExpiredLoginAuthCode	71	// Account login denied due to an expired auth code.
#constant EResultIPLoginRestrictionFailed	72	// The login failed due to an IP restriction.
#constant EResultAccountLockedDown	73	// The current users account is currently locked for use.
#constant EResultAccountLogonDeniedVerifiedEmailRequired	74	// The logon failed because the accounts email is not verified.
#constant EResultNoMatchingURL	75	// There is no URL matching the provided values.
#constant EResultBadResponse	76	// Bad Response due to a Parse failure, missing field, etc.
#constant EResultRequirePasswordReEntry	77	// The user cannot complete the action until they re-enter their password.
#constant EResultValueOutOfRange	78	// The value entered is outside the acceptable range.
#constant EResultUnexpectedError	79	// Something happened that we didn't expect to ever happen.
#constant EResultDisabled	80	// The requested service has been configured to be unavailable.
#constant EResultInvalidCEGSubmission	81	// The files submitted to the CEG server are not valid.
#constant EResultRestrictedDevice	82	// The device being used is not allowed to perform this action.
#constant EResultRegionLocked	83	// The action could not be complete because it is region restricted.
#constant EResultRateLimitExceeded	84	// Temporary rate limit exceeded, try again later, different from k_EResultLimitExceeded which may be permanent.
#constant EResultAccountLoginDeniedNeedTwoFactor	85	// Need two-factor code to login.
#constant EResultItemDeleted	86	// The thing we're trying to access has been deleted.
#constant EResultAccountLoginDeniedThrottle	87	// Login attempt failed, try to throttle response to possible attacker.
#constant EResultTwoFactorCodeMismatch	88	// Two factor authentication (Steam Guard) code is incorrect.
#constant EResultTwoFactorActivationCodeMismatch	89	// The activation code for two-factor authentication (Steam Guard) didn't match.
#constant EResultAccountAssociatedToMultiplePartners	90	// The current account has been associated with multiple partners.
#constant EResultNotModified	91	// The data has not been modified.
#constant EResultNoMobileDevice	92	// The account does not have a mobile device associated with it.
#constant EResultTimeNotSynced	93	// The time presented is out of range or tolerance.
#constant EResultSmsCodeFailed	94	// SMS code failure - no match, none pending, etc.
#constant EResultAccountLimitExceeded	95	// Too many accounts access this resource.
#constant EResultAccountActivityLimitExceeded	96	// Too many changes to this account.
#constant EResultPhoneActivityLimitExceeded	97	// Too many changes to this phone.
#constant EResultRefundToWallet	98	// Cannot refund to payment method, must use wallet.
#constant EResultEmailSendFailure	99	// Cannot send an email.
#constant EResultNotSettled	100	// Can't perform operation until payment has settled.
#constant EResultNeedCaptcha	101	// The user needs to provide a valid captcha.
#constant EResultGSLTDenied	102	// A game server login token owned by this token's owner has been banned.
#constant EResultGSOwnerDenied	103	// Game server owner is denied for some other reason such as account locked, community ban, vac ban, missing phone, etc.
#constant EResultInvalidItemType	104	// The type of thing we were requested to act on is invalid.
#constant EResultIPBanned	105	// The IP address has been banned from taking this action.
#constant EResultGSLTExpired	106	// This Game Server Login Token (GSLT) has expired from disuse; it can be reset for use.
#constant EResultInsufficientFunds	107	// user doesn't have enough wallet funds to complete the action
#constant EResultTooManyPending	108	// There are too many of this thing pending already
#constant EResultNoSiteLicensesFound	109	// No site licenses found
#constant EResultWGNetworkSendExceeded	110	// the WG couldn't send a response because we exceeded max network send size
#constant EResultAccountNotFriends	111	// the user is not mutually friends
#constant EResultLimitedUserAccount	112	// the user is limited
#constant EResultCantRemoveItem	113	// item can't be removed


// EUniverse
// Steam universes.
#constant EUniverseInvalid	0	// Invalid.
#constant EUniversePublic	1	// The standard public universe.
#constant EUniverseBeta	2	// Beta universe used inside Valve.
#constant EUniverseInternal	3	// Internal universe used inside Valve.
#constant EUniverseDev	4	// Dev universe used inside Valve.
#constant EUniverseMax	5	// Total number of universes, used for sanity checks.

// EUserHasLicenseForAppResult
// Result of ISteamUser::UserHasLicenseForApp.
#constant EUserHasLicenseResultHasLicense	0	// The user has a license for specified app.
#constant EUserHasLicenseResultDoesNotHaveLicense	1	// The user does not have a license for the specified app.
#constant EUserHasLicenseResultNoAuth	2	// The user has not been authenticated.

// EVoiceResult
// Results for use with the Steam Voice functions.
#constant EVoiceResultOK	0	// The call has completed successfully.
#constant EVoiceResultNotInitialized	1	// The Steam Voice interface has not been initialized.
#constant EVoiceResultNotRecording	2	// Steam Voice is not currently recording.
#constant EVoiceResultNoData	3	// There is no voice data available.
#constant EVoiceResultBufferTooSmall	4	// The provided buffer is too small to receive the data.
#constant EVoiceResultDataCorrupted	5	// The voice data has been corrupted.
#constant EVoiceResultRestricted	6	// The user is chat restricted.
//#constant EVoiceResultUnsupportedCodec	7	// Deprecated.
//#constant EVoiceResultReceiverOutOfDate	8	// Deprecated.
//#constant EVoiceResultReceiverDidNotAnswer	9	// Deprecated.

// EVRHMDType
// Code points for VR HMD vendors and models.
#constant EVRHMDType_None	-1	// Unknown vendor and model.
#constant EVRHMDType_Unknown	0	// Unknown vendor and model.
#constant EVRHMDType_HTC_Dev	1	// Original HTC dev kits.
#constant EVRHMDType_HTC_VivePre	2	// HTC Vive Pre dev kits.
#constant EVRHMDType_HTC_Vive	3	// HTC Vive Consumer Release.
#constant EVRHMDType_HTC_Unknown	20	// Unknown HTC HMD.
#constant EVRHMDType_Oculus_DK1	21	// Oculus Rift Development Kit 1.
#constant EVRHMDType_Oculus_DK2	22	// Oculus Rift Development Kit 2
#constant EVRHMDType_Oculus_Rift	23	// Oculus Rift Consumer Version 1.
#constant EVRHMDType_Oculus_Unknown	40	// Unknown Oculus HMD.

////////////////////////////////////////////////////////////////////////////////
// steam_gameserver.h
// https://partner.steamgames.com/doc/api/steam_gameserver
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Enums and Constants
//------------------------------------------------------------------------------

// EServerMode
// Used to set the authentication and VAC status of a game server when initializing with SteamGameServer_Init.
//#constant EServerModeInvalid	0	// DEPRECATED - DO NOT USE!
#constant EServerModeNoAuthentication	1	// Don't authenticate user logins and don't list on the server list.
#constant EServerModeAuthentication	2	// Authenticate users, list on the server list, don't run VAC on clients that connect.
#constant EServerModeAuthenticationAndSecure	3	// Authenticate users, list on the server list and VAC protect clients.

