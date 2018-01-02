#option_explicit

////////////////////////////////////////////////////////////////
// Plugin constants
////////////////////////////////////////////////////////////////

// Plugin Callback State Value
//--------------------------------------------------------------
#constant STATE_SERVER_ERROR	-2	// A problem was reported by the Steam API,
#constant STATE_CLIENT_ERROR	-1	// There was a problem calling the command.
#constant STATE_IDLE			0	// The initial state of the callback. Safe to start callback commands while in this state.
#constant STATE_RUNNING			1	// Generally don't need to check for this state.  Callback state will transition to DONE or SERVER_ERROR.
#constant STATE_DONE			2	// Reported once as soon as the callback finishes.  The callback returns to IDLE state immediately.

// Avatar sizes
//--------------------------------------------------------------
#constant AVATAR_SMALL	0 // 32x32
#constant AVATAR_MEDIUM	1 // 64x64
#constant AVATAR_LARGE	2 // 128x128

////////////////////////////////////////////////////////////////
// ISteamFriends
// https://partner.steamgames.com/doc/api/ISteamFriends
////////////////////////////////////////////////////////////////

// EFriendFlags
//--------------------------------------------------------------
// Flags for enumerating friends list, or quickly checking the relationship between users.
#constant k_EFriendFlagNone					0x00	// None.
#constant k_EFriendFlagBlocked				0x01	// Users that the current user has blocked from contacting.
#constant k_EFriendFlagFriendshipRequested	0x02	// Users that have sent a friend invite to the current user.
#constant k_EFriendFlagImmediate			0x04	// The current user's "regular" friends.
#constant k_EFriendFlagClanMember			0x08	// Users that are in one of the same (small) Steam groups as the current user.
#constant k_EFriendFlagOnGameServer			0x10	// Users that are on the same game server; as set by SetPlayedWith.
#constant k_EFriendFlagRequestingFriendship	0x80	// Users that the current user has sent friend invites to.
#constant k_EFriendFlagRequestingInfo		0x100	// Users that are currently sending additional info about themselves after a call to RequestUserInformation
#constant k_EFriendFlagIgnored				0x200	// Users that the current user has ignored from contacting them.
#constant k_EFriendFlagIgnoredFriend		0x400	// Users that have ignored the current user; but the current user still knows about them.
#constant k_EFriendFlagChatMember			0x1000	// Users in one of the same chats.
#constant k_EFriendFlagAll					0xFFFF	// Returns all friend flags.

// FriendRelationship
//--------------------------------------------------------------
// Declares the set of relationships that Steam users may have.
#constant k_EFriendRelationshipNone					0	// The users have no relationship.
#constant k_EFriendRelationshipBlocked				1	// The user has just clicked Ignore on an friendship invite. This doesn't get stored.
#constant k_EFriendRelationshipRequestRecipient		2	// The user has requested to be friends with the current user.
#constant k_EFriendRelationshipFriend				3	// A "regular" friend.
#constant k_EFriendRelationshipRequestInitiator		4	// The current user has sent a friend invite.
#constant k_EFriendRelationshipIgnored				5	// The current user has explicit blocked this other user from comments/chat/etc. This is stored.
#constant k_EFriendRelationshipIgnoredFriend		6	// The user has ignored the current user.
//~ #constant k_EFriendRelationshipSuggested_DEPRECATED	7	// Deprecated -- Unused.
//~ #constant k_EFriendRelationshipMax					8	// The total number of friend relationships used for looping and verification.

// EOverlayToStoreFlag
//--------------------------------------------------------------
// These values are passed as parameters to the store with ActivateGameOverlayToStore and modify the behavior when the page opens.
#constant k_EOverlayToStoreFlag_None				0	// No
#constant k_EOverlayToStoreFlag_AddToCart			1	// Add the specified app ID to the users cart.
#constant k_EOverlayToStoreFlag_AddToCartAndShow	2	// Add the specified app ID to the users cart and show the store page.

// EPersonaChange
//--------------------------------------------------------------
// used in PersonaStateChange_t::m_nChangeFlags to describe what's changed about a user
// these flags describe what the client has learned has changed recently, so on startup you'll see a name, avatar & relationship change for every friend
#constant k_EPersonaChangeName					0x0001	
#constant k_EPersonaChangeStatus				0x0002	
#constant k_EPersonaChangeComeOnline			0x0004	
#constant k_EPersonaChangeGoneOffline			0x0008	
#constant k_EPersonaChangeGamePlayed			0x0010	
#constant k_EPersonaChangeGameServer			0x0020	
#constant k_EPersonaChangeAvatar				0x0040	
#constant k_EPersonaChangeJoinedSource			0x0080	
#constant k_EPersonaChangeLeftSource			0x0100	
#constant k_EPersonaChangeRelationshipChanged	0x0200	
#constant k_EPersonaChangeNameFirstSet			0x0400	
#constant k_EPersonaChangeFacebookInfo			0x0800	
#constant k_EPersonaChangeNickname				0x1000	
#constant k_EPersonaChangeSteamLevel			0x2000	

// EPersonaState
//--------------------------------------------------------------
// List of states a Steam friend can be in.
#constant k_EPersonaStateOffline		0	// Friend is not currently logged on.
#constant k_EPersonaStateOnline			1	// Friend is logged on.
#constant k_EPersonaStateBusy			2	// User is on, but busy.
#constant k_EPersonaStateAway			3	// Auto-away feature.
#constant k_EPersonaStateSnooze			4	// Auto-away for a long time.
#constant k_EPersonaStateLookingToTrade	5	// Online, trading.
#constant k_EPersonaStateLookingToPlay	6	// Online, wanting to play.
//~ #constant k_EPersonaStateMax			7	// The total number of states. Only used for looping and validation.

// EUserRestriction
//--------------------------------------------------------------
// User restriction flags. These are returned by GetUserRestrictions.
#constant k_nUserRestrictionNone		0	// No known chat/content restriction.
#constant k_nUserRestrictionUnknown		1	// We don't know yet, the user is offline.
#constant k_nUserRestrictionAnyChat		2	// User is not allowed to (or can't) send/recv any chat.
#constant k_nUserRestrictionVoiceChat	4	// User is not allowed to (or can't) send/recv voice chat.
#constant k_nUserRestrictionGroupChat	8	// User is not allowed to (or can't) send/recv group chat.
#constant k_nUserRestrictionRating		16	// User is too young according to rating in current region.
#constant k_nUserRestrictionGameInvites	32	// User cannot send or recv game invites, for example if they are on mobile.
#constant k_nUserRestrictionTrading		64	// User cannot participate in trading, for example if they are on a console or mobile.

// Structs
//--------------------------------------------------------------
// Information about the game a friend is playing.
// Obtainable from: GetFriendGamePlayed.
Type FriendGameInfo_t
	InGame as integer // boolean
	GameAppID as integer
	GameIP as integer
	GamePort as integer
	QueryPort as integer
	SteamIDLobby as integer // CSteamID handle
EndType

// Constants
//--------------------------------------------------------------
//~ #constant k_cchMaxFriendsGroupName			64		// The maximum length that a friends group name can be (not including the null-terminator!)
//~ #constant k_cchMaxRichPresenceKeyLength		64		// The maximum length that a rich presence key can be.
//~ #constant k_cchMaxRichPresenceKeys			20		// The maximum amount of rich presence keys that can be set.
//~ #constant k_cchMaxRichPresenceValueLength	256		// The maximum length that a rich presence value can be.
//~ #constant k_cchPersonaNameMax				128		// Maximum number of UTF-8 bytes in a users persona (display) name.
//~ #constant k_cEnumerateFollowersMax			50		// The maximum number of followers that will be returned in a FriendsEnumerateFollowingList_t call result at once.
//~ #constant k_cFriendsGroupLimit				100		// Deprecated - Unused.
//~ #constant k_cubChatMetadataMax				8192	// Maximum size in bytes that chat room, lobby, or chat/lobby member metadata may have.
//~ #constant k_cwchPersonaNameMax				32		// The maximum amount of UTF-16 characters in a users persona (display) name.
//~ #constant k_FriendsGroupID_Invalid			-1		// Invalid friends group identifier.
//~ #constant STEAMFRIENDS_INTERFACE_VERSION	"SteamFriends015"	

////////////////////////////////////////////////////////////////
// ISteamMatchmaking
// https://partner.steamgames.com/doc/api/ISteamMatchmaking
////////////////////////////////////////////////////////////////

// EChatMemberStateChange
//--------------------------------------------------------------
// Flags describing how a users lobby state has changed. This is provided from LobbyChatUpdate_t.
#constant k_EChatMemberStateChangeEntered		0x0001	// This user has joined or is joining the lobby.
#constant k_EChatMemberStateChangeLeft			0x0002	// This user has left or is leaving the lobby.
#constant k_EChatMemberStateChangeDisconnected	0x0004	// User disconnected without leaving the lobby first.
#constant k_EChatMemberStateChangeKicked		0x0008	// The user has been kicked.
#constant k_EChatMemberStateChangeBanned		0x0010	// The user has been kicked and banned.

// ELobbyComparison
//--------------------------------------------------------------
// Lobby search filter options. These can be set with AddRequestLobbyListStringFilter and AddRequestLobbyListNearValueFilter.
#constant k_ELobbyComparisonEqualToOrLessThan		-2	// The lobbies value must be equal to or less than this one.
#constant k_ELobbyComparisonLessThan				-1	// The lobbies value must be less than this one.
#constant k_ELobbyComparisonEqual					0	// The lobbies value must match this this one exactly.
#constant k_ELobbyComparisonGreaterThan				1	// The lobbies value must be greater than this one.
#constant k_ELobbyComparisonEqualToOrGreaterThan	2	// The lobbies value must be equal to or greater than this one.
#constant k_ELobbyComparisonNotEqual				3	// The lobbies value must not match this this.

// ELobbyDistanceFilter
//--------------------------------------------------------------
// Lobby search distance filters when requesting the lobby list. Lobby results are sorted from closest to farthest. This can be set with AddRequestLobbyListDistanceFilter.
#constant k_ELobbyDistanceFilterClose		0	// Only lobbies in the same immediate region will be returned.
#constant k_ELobbyDistanceFilterDefault		1	// Only lobbies in the same region or nearby regions will be returned.
#constant k_ELobbyDistanceFilterFar			2	// For games that don't have many latency requirements, will return lobbies about half-way around the globe.
#constant k_ELobbyDistanceFilterWorldwide	3	// No filtering, will match lobbies as far as India to NY (not recommended, expect multiple seconds of latency between the clients).

// ELobbyType
//--------------------------------------------------------------
// Specifies the lobby type, this is set from CreateLobby and SetLobbyType.
#constant k_ELobbyTypePrivate		0	// The only way to join the lobby is from an invite.
#constant k_ELobbyTypeFriendsOnly	1	// Joinable by friends and invitees, but does not show up in the lobby list.
#constant k_ELobbyTypePublic		2	// Returned by search and visible to friends.
#constant k_ELobbyTypeInvisible		3	// Returned by search, but not visible to other friends.
										// This is useful if you want a user in two lobbies, for example matching groups together. 
										// A user can be in only one regular lobby, and up to two invisible lobbies.

// Constants
//--------------------------------------------------------------
//~ #constant HSERVERQUERY_INVALID						0xffffffff	
//~ #constant k_nMaxLobbyKeyLength						255		// Maximum number of characters a lobby metadata key can be.
//~ #constant k_unFavoriteFlagFavorite					0x01	// This favorite game server entry is for the favorites list.
//~ #constant k_unFavoriteFlagHistory					0x02	// This favorite game server entry is for the history list.
//~ #constant k_unFavoriteFlagNone						0x00	// This favorite game server has no flags set.
//~ #constant STEAMMATCHMAKINGSERVERS_INTERFACE_VERSION	"SteamMatchMakingServers002"	
//~ #constant STEAMMATCHMAKING_INTERFACE_VERSION		"SteamMatchMaking009"	

////////////////////////////////////////////////////////////////
// ISteamUserStats
// https://partner.steamgames.com/doc/api/ISteamUserStats
////////////////////////////////////////////////////////////////

// ELeaderboardDataRequest
//--------------------------------------------------------------
// Type of data request, used when downloading leaderboard entries with DownloadLeaderboardEntries.
#constant k_ELeaderboardDataRequestGlobal			0	// Used to query for a sequential range of leaderboard entries by leaderboard rank. The start 
														// and end parameters control the requested range.
														// For example, you can display the top 10 on a leaderboard for your game by setting start to 1 and end to 10.
#constant k_ELeaderboardDataRequestGlobalAroundUser	1	// Used to retrieve leaderboard entries relative a user's entry.
														// The start parameter is the number of entries to retrieve before the current user's entry, and the end 
														// parameter is the number of entries after the current user's entry. The current user's entry is always included. 
														// For example, if the current user is #5 on the leaderboard, setting start to -2 and end to 2 will 
														// return 5 entries: ranks #3 through #7. If there are not enough entries in the leaderboard before 
														// or after the user's entry, Steam will adjust the range to try to return the number of entries requested. 
														// For example, if the user is #1 on the leaderboard and start is set to -2, end is set to 2, 
														// Steam will return the first 5 entries in the leaderboard.
#constant k_ELeaderboardDataRequestFriends			2	// Used to retrieve all leaderboard entries for friends of the current user. The start and end parameters are ignored.
//~ #constant k_ELeaderboardDataRequestUsers			3	// Used internally, do not use with DownloadLeaderboardEntries! Doing so is undefined behavior.

// ELeaderboardDisplayType
//--------------------------------------------------------------
// The display type used by the Steam Community web site to know how to format the leaderboard scores when displayed. You can set the display type when 
// creating a leaderboard with FindOrCreateLeaderboard or in the Steamworks partner backend. You can retrieve the display type for a given leaderboard 
// with GetLeaderboardDisplayType.
//~ #constant k_ELeaderboardDisplayTypeNone				0	// This is only ever used when a leaderboard is invalid, you should never set this yourself.
#constant k_ELeaderboardDisplayTypeNumeric			1	// The score is just a simple numerical value.
#constant k_ELeaderboardDisplayTypeTimeSeconds		2	// The score represents a time, in seconds.
#constant k_ELeaderboardDisplayTypeTimeMilliSeconds	3	// The score represents a time, in milliseconds.

// ELeaderboardSortMethod
//--------------------------------------------------------------
// The sort method used to set whether a higher or lower score is better. You can set the sort method when creating a leaderboard with FindOrCreateLeaderboard 
// or in App Admin on the Steamworks website. You can retrieve the sort method for a given leaderboard with GetLeaderboardSortMethod.
//~ #constant k_ELeaderboardSortMethodNone			0	// Only ever used when a leaderboard is invalid, you should never set this yourself.
#constant k_ELeaderboardSortMethodAscending		1	// The top-score is the lowest number.
#constant k_ELeaderboardSortMethodDescending	2	// The top-score is the highest number.

// ELeaderboardUploadScoreMethod
//--------------------------------------------------------------
//~ #constant k_ELeaderboardUploadScoreMethodNone			0	
#constant k_ELeaderboardUploadScoreMethodKeepBest		1	// Leaderboard will keep user's best score
#constant k_ELeaderboardUploadScoreMethodForceUpdate	2	// Leaderboard will always replace score with specified

// Constants
//--------------------------------------------------------------
//~ #constant k_cchLeaderboardNameMax			128	// Maximum number of bytes for a leaderboard name (UTF-8 encoded).
//~ #constant k_cchStatNameMax					128	// Maximum number of bytes for stat and achievement names (UTF-8 encoded).
//~ #constant k_cLeaderboardDetailsMax			64	// Maximum number of details that you can store for a single leaderboard entry.
//~ #constant STEAMUSERSTATS_INTERFACE_VERSION	"STEAMUSERSTATS_INTERFACE_VERSION011"	
