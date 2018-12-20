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

// Structs
//--------------------------------------------------------------
// DLC information
// Obtainable from: GetDLCDataJSON, GetDLCDataByIndexJSON.
Type DLCData_t
	AppID as integer		// If 0, the DLC is hidden.
	Available as integer	// Boolean: If 1, the user owns the DLC.
	Name as string
EndType

// DLC download progress
// Obtainable from: GetDLCDownloadProgressJSON.
Type DownloadProgress_t
	AppID as integer			// If 0, the DLC is not currently downloading.
	BytesDownloaded as integer
	BytesTotal as integer
EndType

// Bit Picture Mode gamepad text input
// Obtainable from: GetGamepadTextInputDismissedInfoJSON
Type GamepadTextInputDismissedInfo_t
	Submitted as integer	// 1 when the user submitted text. 0 when the user cancels.
	Text as string
EndType

////////////////////////////////////////////////////////////////
// steam_api.h
// https://partner.steamgames.com/doc/api/steam_api
////////////////////////////////////////////////////////////////

// EAccountType
//--------------------------------------------------------------
// Steam account types. This is used internally in CSteamID.
#constant k_EAccountTypeInvalid			0	// Used for invalid Steam IDs.
#constant k_EAccountTypeIndividual		1	// Regular user account.
#constant k_EAccountTypeMultiseat		2	// Multiseat (e.g. cybercafe) account.
#constant k_EAccountTypeGameServer		3	// Persistent (not anonymous) game server account.
#constant k_EAccountTypeAnonGameServer	4	// Anonymous game server account.
#constant k_EAccountTypePending			5	// Pending.
#constant k_EAccountTypeContentServer	6	// Valve internal content server account.
#constant k_EAccountTypeClan			7	// Steam Group (clan).
#constant k_EAccountTypeChat			8	// Steam group chat or lobby.
#constant k_EAccountTypeConsoleUser		9	// Fake Steam ID for local PSN account on PS3 or Live account on 360, etc.
#constant k_EAccountTypeAnonUser		10	// Anonymous user account. (Used to create an account or reset a password)
#constant k_EAccountTypeMax				11	// Max of 16 items in this field

// EAppOwnershipFlags
//--------------------------------------------------------------
// Flags that represent the ownership information of an App ID. (Steam Internal usage only)
#constant k_EAppOwnershipFlags_None					0x0000	// Unknown.
#constant k_EAppOwnershipFlags_OwnsLicense			0x0001	// Owns a license for this app.
#constant k_EAppOwnershipFlags_FreeLicense			0x0002	// The user has not paid for the app.
#constant k_EAppOwnershipFlags_RegionRestricted		0x0004	// Owns app, but not allowed to play in current region.
#constant k_EAppOwnershipFlags_LowViolence			0x0008	// Only owns a low violence version.
#constant k_EAppOwnershipFlags_InvalidPlatform		0x0010	// App not supported on current platform.
#constant k_EAppOwnershipFlags_SharedLicense		0x0020	// License was shared by an authorized device via Steam Family Sharing.
#constant k_EAppOwnershipFlags_FreeWeekend			0x0040	// Owned by a free weekend licenses.
#constant k_EAppOwnershipFlags_RetailLicense		0x0080	// Has a retail license for game, (CD-Key etc) (Deprecated)
#constant k_EAppOwnershipFlags_LicenseLocked		0x0100	// Shared license is locked (in use) by other user.
#constant k_EAppOwnershipFlags_LicensePending		0x0200	// Owns app, but transaction is still pending. Can't install or play yet.
#constant k_EAppOwnershipFlags_LicenseExpired		0x0400	// Doesn't own app anymore since license expired.
#constant k_EAppOwnershipFlags_LicensePermanent		0x0800	// Permanent license, not borrowed, or guest or freeweekend etc.
#constant k_EAppOwnershipFlags_LicenseRecurring		0x1000	// Recurring license, user is charged periodically.
#constant k_EAppOwnershipFlags_LicenseCanceled		0x2000	// Mark as canceled, but might be still active if recurring.
#constant k_EAppOwnershipFlags_AutoGrant			0x4000	// Ownership is based on any kind of autogrant license.
#constant k_EAppOwnershipFlags_PendingGift			0x8000	// User has pending gift to redeem.
#constant k_EAppOwnershipFlags_RentalNotActivated	0x10000	// Rental hasn't been activated yet.
#constant k_EAppOwnershipFlags_Rental				0x20000	// Is a rental.
#constant k_EAppOwnershipFlags_SiteLicense			0x40000	// Is from a site license

// EAppReleaseState
//--------------------------------------------------------------
// Release state of an App ID. (Steam Internal usage only)
#constant k_EAppReleaseState_Unknown		0	// Unknown, can't get application information, or license info is missing.
#constant k_EAppReleaseState_Unavailable	1	// Even if user owns it, they can't see game at all.
#constant k_EAppReleaseState_Prerelease		2	// Can be purchased and is visible in games list, nothing else.
#constant k_EAppReleaseState_PreloadOnly	3	// Owners can preload app, not play it.
#constant k_EAppReleaseState_Released		4	// Owners can download and play app.

// EAppType
//--------------------------------------------------------------
// App ID type, more info is available on Types of Applications (Steam Internal usage only)
#constant k_EAppType_Invalid		0x000		// Unknown / invalid.
#constant k_EAppType_Game			0x001		// Playable game, default type.
#constant k_EAppType_Application	0x002		// Software application.
#constant k_EAppType_Tool			0x004		// SDKs, editors & dedicated servers.
#constant k_EAppType_Demo			0x008		// Game demo.
//~ #constant k_EAppType_Media_DEPRECATED	0x010	// Legacy - was used for game trailers, which are now just videos on the web.
#constant k_EAppType_DLC			0x020		// Downloadable Content.
#constant k_EAppType_Guide			0x040		// Game guide, PDF etc
#constant k_EAppType_Driver			0x080		// Hardware driver updater (ATI, Razor etc.)
#constant k_EAppType_Config			0x100		// Hidden app used to config Steam features (backpack, sales, etc.)
#constant k_EAppType_Hardware		0x200		// S hardware device (Steam Machine, Steam Controller, Steam Link, etc.)
#constant k_EAppType_Franchise		0x400		// A hub for collections of multiple apps, eg films, series, games.
#constant k_EAppType_Video			0x800		// A video component of either a Film or TVSeries (may be the feature, an episode, preview, making-of, etc.)
#constant k_EAppType_Plugin			0x1000		// Plug-in types for other Apps.
#constant k_EAppType_Music			0x2000		// Music files.
#constant k_EAppType_Series			0x4000		// Container app for video series.
#constant k_EAppType_Shortcut		0x40000000	// Just a shortcut, client side only.
#constant k_EAppType_DepotOnly		0x80000000	// Placeholder since depots and apps share the same namespace.

// EAuthSessionResponse
//--------------------------------------------------------------
// Callback return values for the ValidateAuthTicketResponse_t callback which is posted as a response to ISteamUser::BeginAuthSession and ISteamGameServer::BeginAuthSession.
#constant k_EAuthSessionResponseOK								0	// Steam has verified the user is online, the ticket is valid and ticket has not been reused.
#constant k_EAuthSessionResponseUserNotConnectedToSteam			1	// The user in question is not connected to steam.
#constant k_EAuthSessionResponseNoLicenseOrExpired				2	// The user doesn't have a license for this App ID or the ticket has expired.
#constant k_EAuthSessionResponseVACBanned						3	// The user is VAC banned for this game.
#constant k_EAuthSessionResponseLoggedInElseWhere				4	// The user account has logged in elsewhere and the session containing the game instance has been disconnected.
#constant k_EAuthSessionResponseVACCheckTimedOut				5	// VAC has been unable to perform anti-cheat checks on this user.
#constant k_EAuthSessionResponseAuthTicketCanceled				6	// The ticket has been canceled by the issuer.
#constant k_EAuthSessionResponseAuthTicketInvalidAlreadyUsed	7	// This ticket has already been used, it is not valid.
#constant k_EAuthSessionResponseAuthTicketInvalid				8	// This ticket is not from a user instance currently connected to steam.
#constant k_EAuthSessionResponsePublisherIssuedBan				9	// The user is banned for this game. The ban came via the web api and not VAC.

// EBeginAuthSessionResult
//--------------------------------------------------------------
// Results returned from ISteamUser::BeginAuthSession and ISteamGameServer::BeginAuthSession.
#constant k_EBeginAuthSessionResultOK				0	// Ticket is valid for this game and this Steam ID.
#constant k_EBeginAuthSessionResultInvalidTicket	1	// The ticket is invalid.
#constant k_EBeginAuthSessionResultDuplicateRequest	2	// A ticket has already been submitted for this Steam ID.
#constant k_EBeginAuthSessionResultInvalidVersion	3	// Ticket is from an incompatible interface version.
#constant k_EBeginAuthSessionResultGameMismatch		4	// Ticket is not for this game.
#constant k_EBeginAuthSessionResultExpiredTicket	5	// Ticket has expired.

// EBroadcastUploadResult
//--------------------------------------------------------------
// Broadcast upload result from BroadcastUploadStop_t.
#constant k_EBroadcastUploadResultNone				0	// Broadcast state unknown.
#constant k_EBroadcastUploadResultOK				1	// Broadcast was good, no problems.
#constant k_EBroadcastUploadResultInitFailed		2	// Broadcast init failed.
#constant k_EBroadcastUploadResultFrameFailed		3	// Broadcast frame upload failed.
#constant k_EBroadcastUploadResultTimeout			4	// Broadcast upload timed out.
#constant k_EBroadcastUploadResultBandwidthExceeded	5	// Broadcast send too much data.
#constant k_EBroadcastUploadResultLowFPS			6	// Broadcast FPS is too low.
#constant k_EBroadcastUploadResultMissingKeyFrames	7	// Broadcast sending not enough key frames.
#constant k_EBroadcastUploadResultNoConnection		8	// Broadcast client failed to connect to relay.
#constant k_EBroadcastUploadResultRelayFailed		9	// Relay dropped the upload.
#constant k_EBroadcastUploadResultSettingsChanged	10	// The client changed broadcast settings.
#constant k_EBroadcastUploadResultMissingAudio		11	// Client failed to send audio data.
#constant k_EBroadcastUploadResultTooFarBehind		12	// Clients was too slow uploading.
#constant k_EBroadcastUploadResultTranscodeBehind	13	// Server failed to keep up with transcode.

// EChatEntryType
//--------------------------------------------------------------
// Chat Entry Types.
// Returned by ISteamFriends::GetFriendMessage, ISteamFriends::GetClanChatMessage and ISteamMatchmaking::GetLobbyChatEntry.
#constant k_EChatEntryTypeInvalid			0	// Invalid.
#constant k_EChatEntryTypeChatMsg			1	// Normal text message from another user.
#constant k_EChatEntryTypeTyping			2	// The other user is typing, not used in multi-user chat.
#constant k_EChatEntryTypeInviteGame		3	// Invite from other user into that users current game.
#constant k_EChatEntryTypeEmote				4	// Text emote message (Deprecated, should be treated as ChatMsg).
#constant k_EChatEntryTypeLeftConversation	6	// A user has left the conversation (closed the chat window).
#constant k_EChatEntryTypeEntered			7	// User has entered the conversation, used in multi-user chat and group chat.
#constant k_EChatEntryTypeWasKicked			8	// User was kicked (Data: Steam ID of the user performing the kick).
#constant k_EChatEntryTypeWasBanned			9	// User was banned (Data: Steam ID of the user performing the ban).
#constant k_EChatEntryTypeDisconnected		10	// User disconnected.
#constant k_EChatEntryTypeHistoricalChat	11	// A chat message from user's chat history or offline message.
#constant k_EChatEntryTypeLinkBlocked		14	// A link was removed by the chat filter.

// EChatRoomEnterResponse
//--------------------------------------------------------------
// Chat Room Enter Responses.
#constant k_EChatRoomEnterResponseSuccess			1	// Success.
#constant k_EChatRoomEnterResponseDoesntExist		2	// Chat doesn't exist (probably closed).
#constant k_EChatRoomEnterResponseNotAllowed		3	// General Denied - You don't have the permissions needed to join the chat.
#constant k_EChatRoomEnterResponseFull				4	// Chat room has reached its maximum size.
#constant k_EChatRoomEnterResponseError				5	// Unexpected Error.
#constant k_EChatRoomEnterResponseBanned			6	// You are banned from this chat room and may not join.
#constant k_EChatRoomEnterResponseLimited			7	// Joining this chat is not allowed because you are a limited user (no value on account).
#constant k_EChatRoomEnterResponseClanDisabled		8	// Attempt to join a clan chat when the clan is locked or disabled.
#constant k_EChatRoomEnterResponseCommunityBan		9	// Attempt to join a chat when the user has a community lock on their account.
#constant k_EChatRoomEnterResponseMemberBlockedYou	10	// Join failed - a user that is in the chat has blocked you from joining.
#constant k_EChatRoomEnterResponseYouBlockedMember	11	// Join failed - you have blocked a user that is already in the chat.

// EChatSteamIDInstanceFlags
//--------------------------------------------------------------
// Special flags for Chat accounts - they go in the top 8 bits of the Steam ID's "instance", leaving 12 for the actual instances
#constant k_EChatAccountInstanceMask	0x00000FFF	// Top 8 bits are flags.
#constant k_EChatInstanceFlagClan		0x0080000 	// The Steam ID is for a Steam group.	// (k_unSteamAccountInstanceMask + 1) >> 1
#constant k_EChatInstanceFlagLobby		0x0040000	// The Steam ID is for a Lobby.			// (k_unSteamAccountInstanceMask + 1) >> 2
#constant k_EChatInstanceFlagMMSLobby	0x0020000	// The Steam ID is for a MMS Lobby.		// (k_unSteamAccountInstanceMask + 1) >> 3

// EDenyReason
//--------------------------------------------------------------
// Result values when a client failed to join or has been kicked from a game server. Obtained from GSClientDeny_t and GSClientKick_t.
#constant k_EDenyInvalid					0	// Unknown.
#constant k_EDenyInvalidVersion				1	// The client and server are not the same version.
#constant k_EDenyGeneric					2	// Generic.
#constant k_EDenyNotLoggedOn				3	// The client is not logged on.
#constant k_EDenyNoLicense					4	// The client does not have a license to play this game.
#constant k_EDenyCheater					5	// The client is VAC banned.
#constant k_EDenyLoggedInElseWhere			6	// The client is logged in elsewhere.
#constant k_EDenyUnknownText				7	
#constant k_EDenyIncompatibleAnticheat		8	
#constant k_EDenyMemoryCorruption			9	
#constant k_EDenyIncompatibleSoftware		10	
#constant k_EDenySteamConnectionLost		11	// The server lost connection to steam.
#constant k_EDenySteamConnectionError		12	// The server had a general error connecting to Steam.
#constant k_EDenySteamResponseTimedOut		13	// The server timed out connecting to Steam.
#constant k_EDenySteamValidationStalled		14	// The client has not authed with Steam yet.
#constant k_EDenySteamOwnerLeftGuestUser	15	// The owner of the shared game has left, called for each guest of the owner.

// EGameIDType
//--------------------------------------------------------------
// Used to describe the type of CGameID.
#constant k_EGameIDTypeApp		0	// The Game ID is a regular steam app.
#constant k_EGameIDTypeGameMod	1	// The Game ID is a mod.
#constant k_EGameIDTypeShortcut	2	// The game ID is a shortcut.
#constant k_EGameIDTypeP2P		3	// The game ID is a P2P file.

// ELaunchOptionType
//--------------------------------------------------------------
// Codes for well defined launch options, corresponds to "Launch Type" in the applications Launch Options which can be found on the General Installation Settings page.
// There is a special function BIsVRLaunchOptionType, to check a the type is any of the VR launch options.
#constant k_ELaunchOptionType_None			0	// Unspecified.
#constant k_ELaunchOptionType_Default		1	// Runs the app in default mode.
#constant k_ELaunchOptionType_SafeMode		2	// Runs the game in safe mode.
#constant k_ELaunchOptionType_Multiplayer	3	// Runs the game in multiplayer mode.
#constant k_ELaunchOptionType_Config		4	// Runs config tool for this game.
#constant k_ELaunchOptionType_OpenVR		5	// Runs game in VR mode using OpenVR.
#constant k_ELaunchOptionType_Server		6	// Runs dedicated server for this game.
#constant k_ELaunchOptionType_Editor		7	// Runs game editor.
#constant k_ELaunchOptionType_Manual		8	// Shows game manual.
#constant k_ELaunchOptionType_Benchmark		9	// Runs game benchmark.
#constant k_ELaunchOptionType_Option1		10	// Generic run option, uses description field for game name.
#constant k_ELaunchOptionType_Option2		11	// Generic run option, uses description field for game name.
#constant k_ELaunchOptionType_Option3		12	// Generic run option, uses description field for game name.
#constant k_ELaunchOptionType_OculusVR		13	// Runs game in VR mode using the Oculus SDK.
#constant k_ELaunchOptionType_OpenVROverlay	14	// Runs an OpenVR dashboard overlay.
#constant k_ELaunchOptionType_OSVR			15	// Runs game in VR mode using the OSVR SDK.
#constant k_ELaunchOptionType_Dialog		1000	// Show launch options dialog.

// EMarketingMessageFlags
//--------------------------------------------------------------
// Internal Steam marketing message flags that change how a client should handle them.
#constant k_EMarketingMessageFlagsNone					0	
#constant k_EMarketingMessageFlagsHighPriority			1	// 1 << 0
#constant k_EMarketingMessageFlagsPlatformWindows		2	// 1 << 1
#constant k_EMarketingMessageFlagsPlatformMac			4	// 1 << 2
#constant k_EMarketingMessageFlagsPlatformLinux			8	// 1 << 3
//~ #constant k_EMarketingMessageFlagsPlatformRestrictions	=	aggregate flags

// ENotificationPosition
//--------------------------------------------------------------
// Possible positions to have the overlay show notifications in. Used with ISteamUtils::SetOverlayNotificationPosition.
#constant k_EPositionTopLeft		0	// Top left corner.
#constant k_EPositionTopRight		1	// Top right corner.
#constant k_EPositionBottomLeft		2	// Bottom left corner.
#constant k_EPositionBottomRight	3	// Bottom right corner.

// EResult
//--------------------------------------------------------------
// Steam error result codes.
// These are frequently returned by functions, callbacks, and call results from both the Steamworks API and the Web API. An API may return arbitrary EResult codes, refer to the documentation for that API function or callback to see what it could return and what they mean in the context of that API call.
// This is similar to Win32's HRESULT type or POSIXs errno.
#constant k_EResultOK										1	// Success.
#constant k_EResultFail										2	// Generic failure.
#constant k_EResultNoConnection								3	// Your Steam client doesn't have a connection to the back-end.
#constant k_EResultInvalidPassword							5	// Password/ticket is invalid.
#constant k_EResultLoggedInElsewhere						6	// The user is logged in elsewhere.
#constant k_EResultInvalidProtocolVer						7	// Protocol version is incorrect.
#constant k_EResultInvalidParam								8	// A parameter is incorrect.
#constant k_EResultFileNotFound								9	// File was not found.
#constant k_EResultBusy										10	// Called method is busy - action not taken.
#constant k_EResultInvalidState								11	// Called object was in an invalid state.
#constant k_EResultInvalidName								12	// The name was invalid.
#constant k_EResultInvalidEmail								13	// The email was invalid.
#constant k_EResultDuplicateName							14	// The name is not unique.
#constant k_EResultAccessDenied								15	// Access is denied.
#constant k_EResultTimeout									16	// Operation timed out.
#constant k_EResultBanned									17	// The user is VAC2 banned.
#constant k_EResultAccountNotFound							18	// Account not found.
#constant k_EResultInvalidSteamID							19	// The Steam ID was invalid.
#constant k_EResultServiceUnavailable						20	// The requested service is currently unavailable.
#constant k_EResultNotLoggedOn								21	// The user is not logged on.
#constant k_EResultPending									22	// Request is pending, it may be in process or waiting on third party.
#constant k_EResultEncryptionFailure						23	// Encryption or Decryption failed.
#constant k_EResultInsufficientPrivilege					24	// Insufficient privilege.
#constant k_EResultLimitExceeded							25	// Too much of a good thing.
#constant k_EResultRevoked									26	// Access has been revoked (used for revoked guest passes.)
#constant k_EResultExpired									27	// License/Guest pass the user is trying to access is expired.
#constant k_EResultAlreadyRedeemed							28	// Guest pass has already been redeemed by account, cannot be used again.
#constant k_EResultDuplicateRequest							29	// The request is a duplicate and the action has already occurred in the past, ignored this time.
#constant k_EResultAlreadyOwned								30	// All the games in this guest pass redemption request are already owned by the user.
#constant k_EResultIPNotFound								31	// IP address not found.
#constant k_EResultPersistFailed							32	// Failed to write change to the data store.
#constant k_EResultLockingFailed							33	// Failed to acquire access lock for this operation.
#constant k_EResultLogonSessionReplaced						34	// The logon session has been replaced.
#constant k_EResultConnectFailed							35	// Failed to connect.
#constant k_EResultHandshakeFailed							36	// The authentication handshake has failed.
#constant k_EResultIOFailure								37	// There has been a genic IO failure.
#constant k_EResultRemoteDisconnect							38	// The remote server has disconnected.
#constant k_EResultShoppingCartNotFound						39	// Failed to find the shopping cart requested.
#constant k_EResultBlocked									40	// A user blocked the action.
#constant k_EResultIgnored									41	// The target is ignoring sender.
#constant k_EResultNoMatch									42	// Nothing matching the request found.
#constant k_EResultAccountDisabled							43	// The account is disabled.
#constant k_EResultServiceReadOnly							44	// This service is not accepting content changes right now.
#constant k_EResultAccountNotFeatured						45	// Account doesn't have value, so this feature isn't available.
#constant k_EResultAdministratorOK							46	// Allowed to take this action, but only because requester is admin.
#constant k_EResultContentVersion							47	// A Version mismatch in content transmitted within the Steam protocol.
#constant k_EResultTryAnotherCM								48	// The current CM can't service the user making a request, user should try another.
#constant k_EResultPasswordRequiredToKickSession			49	// You are already logged in elsewhere, this cached credential login has failed.
#constant k_EResultAlreadyLoggedInElsewhere					50	// The user is logged in elsewhere. (Use k_EResultLoggedInElsewhere instead!)
#constant k_EResultSuspended								51	// Long running operation has suspended/paused. (eg. content download.)
#constant k_EResultCancelled								52	// Operation has been canceled, typically by user. (eg. a content download.)
#constant k_EResultDataCorruption							53	// Operation canceled because data is ill formed or unrecoverable.
#constant k_EResultDiskFull									54	// Operation canceled - not enough disk space.
#constant k_EResultRemoteCallFailed							55	// The remote or IPC call has failed.
#constant k_EResultPasswordUnset							56	// Password could not be verified as it's unset server side.
#constant k_EResultExternalAccountUnlinked					57	// External account (PSN, Facebook...) is not linked to a Steam account.
#constant k_EResultPSNTicketInvalid							58	// PSN ticket was invalid.
#constant k_EResultExternalAccountAlreadyLinked				59	// External account (PSN, Facebook...) is already linked to some other account, must explicitly request to replace/delete the link first.
#constant k_EResultRemoteFileConflict						60	// The sync cannot resume due to a conflict between the local and remote files.
#constant k_EResultIllegalPassword							61	// The requested new password is not allowed.
#constant k_EResultSameAsPreviousValue						62	// New value is the same as the old one. This is used for secret question and answer.
#constant k_EResultAccountLogonDenied						63	// Account login denied due to 2nd factor authentication failure.
#constant k_EResultCannotUseOldPassword						64	// The requested new password is not legal.
#constant k_EResultInvalidLoginAuthCode						65	// Account login denied due to auth code invalid.
#constant k_EResultAccountLogonDeniedNoMail					66	// Account login denied due to 2nd factor auth failure - and no mail has been sent.
#constant k_EResultHardwareNotCapableOfIPT					67	// The users hardware does not support Intel's Identity Protection Technology (IPT).
#constant k_EResultIPTInitError								68	// Intel's Identity Protection Technology (IPT) has failed to initialize.
#constant k_EResultParentalControlRestricted				69	// Operation failed due to parental control restrictions for current user.
#constant k_EResultFacebookQueryError						70	// Facebook query returned an error.
#constant k_EResultExpiredLoginAuthCode						71	// Account login denied due to an expired auth code.
#constant k_EResultIPLoginRestrictionFailed					72	// The login failed due to an IP resriction.
#constant k_EResultAccountLockedDown						73	// The current users account is currently locked for use. This is likely due to a hijacking and pending ownership verification.
#constant k_EResultAccountLogonDeniedVerifiedEmailRequired	74	// The logon failed because the accounts email is not verified.
#constant k_EResultNoMatchingURL							75	// There is no URL matching the provided values.
#constant k_EResultBadResponse								76	// Bad Response due to a Parse failure, missing field, etc.
#constant k_EResultRequirePasswordReEntry					77	// The user cannot complete the action until they re-enter their password.
#constant k_EResultValueOutOfRange							78	// The value entered is outside the acceptable range.
#constant k_EResultUnexpectedError							79	// Something happened that we didn't expect to ever happen.
#constant k_EResultDisabled									80	// The requested service has been configured to be unavailable.
#constant k_EResultInvalidCEGSubmission						81	// The files submitted to the CEG server are not valid.
#constant k_EResultRestrictedDevice							82	// The device being used is not allowed to perform this action.
#constant k_EResultRegionLocked								83	// The action could not be complete because it is region restricted.
#constant k_EResultRateLimitExceeded						84	// Temporary rate limit exceeded, try again later, different from k_EResultLimitExceeded which may be permanent.
#constant k_EResultAccountLoginDeniedNeedTwoFactor			85	// Need two-factor code to login.
#constant k_EResultItemDeleted								86	// The thing we're trying to access has been deleted.
#constant k_EResultAccountLoginDeniedThrottle				87	// Login attempt failed, try to throttle response to possible attacker.
#constant k_EResultTwoFactorCodeMismatch					88	// Two factor authentication (Steam Guard) code is incorrect.
#constant k_EResultTwoFactorActivationCodeMismatch			89	// The activation code for two-factor authentication (Steam Guard) didn't match.
#constant k_EResultAccountAssociatedToMultiplePartners		90	// The current account has been associated with multiple partners.
#constant k_EResultNotModified								91	// The data has not been modified.
#constant k_EResultNoMobileDevice							92	// The account does not have a mobile device associated with it.
#constant k_EResultTimeNotSynced							93	// The time presented is out of range or tolerance.
#constant k_EResultSmsCodeFailed							94	// SMS code failure - no match, none pending, etc.
#constant k_EResultAccountLimitExceeded						95	// Too many accounts access this resource.
#constant k_EResultAccountActivityLimitExceeded				96	// Too many changes to this account.
#constant k_EResultPhoneActivityLimitExceeded				97	// Too many changes to this phone.
#constant k_EResultRefundToWallet							98	// Cannot refund to payment method, must use wallet.
#constant k_EResultEmailSendFailure							99	// Cannot send an email.
#constant k_EResultNotSettled								100	// Can't perform operation until payment has settled.
#constant k_EResultNeedCaptcha								101	// The user needs to provide a valid captcha.
#constant k_EResultGSLTDenied								102	// A game server login token owned by this token's owner has been banned.
#constant k_EResultGSOwnerDenied							103	// Game server owner is denied for some other reason such as account locked, community ban, vac ban, missing phone, etc.
#constant k_EResultInvalidItemType							104	// The type of thing we were requested to act on is invalid.
#constant k_EResultIPBanned									105	// The IP address has been banned from taking this action.
#constant k_EResultGSLTExpired								106	// This Game Server Login Token (GSLT) has expired from disuse; it can be reset for use.
#constant k_EResultInsufficientFunds						107	// user doesn't have enough wallet funds to complete the action
#constant k_EResultTooManyPending							108	// There are too many of this thing pending already

// ESteamUserStatType
//--------------------------------------------------------------
// Fields used internally to store user stats.
#constant k_ESteamUserStatTypeINVALID			0	// Invalid.
#constant k_ESteamUserStatTypeINT				1	// 32-bit int stat.
#constant k_ESteamUserStatTypeFLOAT				2	// 32-bit float stat.
#constant k_ESteamUserStatTypeAVGRATE			3	// Read as FLOAT, set with count / session length.
#constant k_ESteamUserStatTypeACHIEVEMENTS		4	// Standard user achievement.
//~ #constant k_ESteamUserStatTypeGROUPACHIEVEMENTS	5	// Deprecated.
//~ #constant k_ESteamUserStatTypeMAX				6	// Total number of user stat types, used for sanity checks.

// EUniverse
//--------------------------------------------------------------
// Steam universes. Each universe is a self-contained Steam instance.
#constant k_EUniverseInvalid	0	// Invalid.
#constant k_EUniversePublic		1	// The standard public universe.
#constant k_EUniverseBeta		2	// Beta universe used inside Valve.
#constant k_EUniverseInternal	3	// Internal universe used inside Valve.
#constant k_EUniverseDev		4	// Dev universe used inside Valve.
#constant k_EUniverseMax		5	// Total number of universes, used for sanity checks.

// EUserHasLicenseForAppResult
//--------------------------------------------------------------
// Result of ISteamUser::UserHasLicenseForApp.
#constant k_EUserHasLicenseResultHasLicense			0	// The user has a license for specified app.
#constant k_EUserHasLicenseResultDoesNotHaveLicense	1	// The user does not have a license for the specified app.
#constant k_EUserHasLicenseResultNoAuth				2	// The user has not been authenticated.

// EVoiceResult
//--------------------------------------------------------------
// Results for use with the Steam Voice functions.
#constant k_EVoiceResultOK						0	// The call has completed successfully.
#constant k_EVoiceResultNotInitialized			1	// The Steam Voice interface has not been initialized.
#constant k_EVoiceResultNotRecording			2	// Steam Voice is not currently recording.
#constant k_EVoiceResultNoData					3	// There is no voice data available.
#constant k_EVoiceResultBufferTooSmall			4	// The provided buffer is too small to receive the data.
#constant k_EVoiceResultDataCorrupted			5	// The voice data has been corrupted.
#constant k_EVoiceResultRestricted				6	// The user is chat restricted.
//~ #constant k_EVoiceResultUnsupportedCodec		7	// Deprecated.
//~ #constant k_EVoiceResultReceiverOutOfDate		8	// Deprecated.
//~ #constant k_EVoiceResultReceiverDidNotAnswer	9	// Deprecated.

// EVRHMDType
//--------------------------------------------------------------
// Code points for VR HMD vendors and models. Use the special functions BIsOculusHMD and BIsViveHMD to check for a specific brand.
#constant k_eEVRHMDType_None			-1	// Unknown vendor and model.
#constant k_eEVRHMDType_Unknown			0	// Unknown vendor and model.
#constant k_eEVRHMDType_HTC_Dev			1	// Original HTC dev kits.
#constant k_eEVRHMDType_HTC_VivePre		2	// HTC Vive Pre dev kits.
#constant k_eEVRHMDType_HTC_Vive		3	// HTC Vive Consumer Release.
#constant k_eEVRHMDType_HTC_Unknown		20	// Unknown HTC HMD.
#constant k_eEVRHMDType_Oculus_DK1		21	// Oculus Rift Development Kit 1.
#constant k_eEVRHMDType_Oculus_DK2		22	// Oculus Rift Development Kit 2
#constant k_eEVRHMDType_Oculus_Rift		23	// Oculus Rift Consumer Version 1.
#constant k_eEVRHMDType_Oculus_Unknown	40	// Unknown Oculus HMD.

// Constants
//--------------------------------------------------------------
// These are constants which are defined for use with steam_api.
#constant BREAKPAD_INVALID_HANDLE			0
#constant k_cchGameExtraInfoMax				64			// The maximum size (in UTF-8 bytes, including the null terminator) of the pchExtraInfo parameter of ISteamUser::TrackAppUsageEvent.
//~ #constant k_cubSaltSize						8			// Only used internally in Steam.
//~ #constant k_GIDNil							0xffffffffffffffff	// Only used internally in Steam.
#constant k_HAuthTicketInvalid				0			// An invalid user authentication ticket.
//~ #constant k_JobIDNil						0xffffffffffffffff	// Only used internally in Steam.
//~ #constant k_steamIDLanModeGS				?			// Steam ID comes from a user game connection to an sv_lan GS
#constant k_steamIDNil						0			// Generic invalid CSteamID.
//~ #constant k_steamIDNonSteamGS				?			// This Steam ID can come from a user game connection to a GS that isn't using the steam authentication system but still 
														//~ // wants to support the "Join Game" option in the friends list
//~ #constant k_steamIDNotInitYetGS				?			// This Steam ID can come from a user game connection to a GS that has just booted but hasnt yet even initialized its steam3
														//~ // component and started logging on.
//~ #constant k_steamIDOutofDateGS				?			// This Steam ID comes from a user game connection to an out of date GS that hasnt implemented the protocol to provide its Steam ID
//~ #constant k_TxnIDNil						k_GIDNil	// Only used internally in Steam.
//~ #constant k_TxnIDUnknown					0			// Only used internally in Steam.
#constant k_uAPICallInvalid					0x0			// An Invalid Steam API Call handle.
#constant k_uAppIdInvalid					0x0			// An Invalid App ID.
//~ #constant k_uBundleIdInvalid				0			// Only used internally in Steam.
//~ #constant k_uCellIDInvalid					0xFFFFFFFF	// Only used internally in Steam.
#constant k_uDepotIdInvalid					0x0			// An Invalid Depot ID.
//~ #constant k_ulAssetClassIdInvalid			0x0			// Only used internally in Steam.
//~ #constant k_uManifestIdInvalid				0			// Only used internally in Steam.
#constant k_unSteamAccountIDMask			0xFFFFFFFF	// Used in CSteamID to mask out the AccountID_t.
#constant k_unSteamAccountInstanceMask		0x000FFFFF	// Used in CSteamID to mask out the account instance.
#constant k_unSteamUserConsoleInstance		2			// Used by CSteamID to identify users logged in from a console.
#constant k_unSteamUserDesktopInstance		1			// Used by CSteamID to identify users logged in from the desktop client.
#constant k_unSteamUserWebInstance			4			// Used by CSteamID to identify users logged in from the web.
//~ #constant k_uPackageIdFreeSub				0x0			// Only used internally in Steam.
//~ #constant k_uPackageIdInvalid				0xFFFFFFFF	// Only used internally in Steam.
//~ #constant k_uPartnerIdInvalid				0			// Only used internally in Steam.
//~ #constant k_uPhysicalItemIdInvalid			0x0			// Only used internally in Steam.
#constant QUERY_PORT_ERROR					0xFFFE		// We were unable to get the query port for this server.
#constant QUERY_PORT_NOT_INITIALIZED		0xFFFF		// Constants used for query ports.  We haven't asked the GS for this query port's actual value yet.

////////////////////////////////////////////////////////////////
// ISteamController
// https://partner.steamgames.com/doc/api/ISteamController
////////////////////////////////////////////////////////////////

// EControllerActionOrigin
//--------------------------------------------------------------
// Inputs the player binds to actions in the Steam Controller Configurator.
// The chief purpose of these values is to direct which on-screen button glyphs should appear for a given action, such as "Press [A] to Jump".
#constant k_EControllerActionOrigin_None							0	
#constant k_EControllerActionOrigin_A								1	// (Valve Steam Controller) digital face button A
#constant k_EControllerActionOrigin_B								2	// (Valve Steam Controller) digital face button B
#constant k_EControllerActionOrigin_X								3	// (Valve Steam Controller) digital face button X
#constant k_EControllerActionOrigin_Y								4	// (Valve Steam Controller) digital face button Y
#constant k_EControllerActionOrigin_LeftBumper						5	// (Valve Steam Controller) digital left shoulder button // (aka "left bumper")
#constant k_EControllerActionOrigin_RightBumper						6	// (Valve Steam Controller) digital right shoulder button // (aka "right bumper")
#constant k_EControllerActionOrigin_LeftGrip						7	// (Valve Steam Controller) digital left grip paddle
#constant k_EControllerActionOrigin_RightGrip						8	// (Valve Steam Controller) digital right grip paddle
#constant k_EControllerActionOrigin_Start							9	// (Valve Steam Controller) digital start button
#constant k_EControllerActionOrigin_Back							10	// (Valve Steam Controller) digital back button
#constant k_EControllerActionOrigin_LeftPad_Touch					11	// (Valve Steam Controller) left haptic touchpad, in simple contact with a finger
#constant k_EControllerActionOrigin_LeftPad_Swipe					12	// (Valve Steam Controller) left haptic touchpad, touch input on any axis
#constant k_EControllerActionOrigin_LeftPad_Click					13	// (Valve Steam Controller) left haptic touchpad, digital click // (for the whole thing)
#constant k_EControllerActionOrigin_LeftPad_DPadNorth				14	// (Valve Steam Controller) left haptic touchpad, digital click // (upper quadrant)
#constant k_EControllerActionOrigin_LeftPad_DPadSouth				15	// (Valve Steam Controller) left haptic touchpad, digital click // (lower quadrant)
#constant k_EControllerActionOrigin_LeftPad_DPadWest				16	// (Valve Steam Controller) left haptic touchpad, digital click // (left quadrant)
#constant k_EControllerActionOrigin_LeftPad_DPadEast				17	// (Valve Steam Controller) left haptic touchpad, digital click // (right quadrant)
#constant k_EControllerActionOrigin_RightPad_Touch					18	// (Valve Steam Controller) right haptic touchpad, in simple contact with a finger
#constant k_EControllerActionOrigin_RightPad_Swipe					19	// (Valve Steam Controller) right haptic touchpad, touch input on any axis
#constant k_EControllerActionOrigin_RightPad_Click					20	// (Valve Steam Controller) right haptic touchpad, digital click // (for the whole thing)
#constant k_EControllerActionOrigin_RightPad_DPadNorth				21	// (Valve Steam Controller) right haptic touchpad, digital click // (upper quadrant)
#constant k_EControllerActionOrigin_RightPad_DPadSouth				22	// (Valve Steam Controller) right haptic touchpad, digital click // (lower quadrant)
#constant k_EControllerActionOrigin_RightPad_DPadWest				23	// (Valve Steam Controller) right haptic touchpad, digital click // (left quadrant)
#constant k_EControllerActionOrigin_RightPad_DPadEast				24	// (Valve Steam Controller) right haptic touchpad, digital click // (right quadrant)
#constant k_EControllerActionOrigin_LeftTrigger_Pull				25	// (Valve Steam Controller) left analog trigger, pulled by any amount // (analog value)
#constant k_EControllerActionOrigin_LeftTrigger_Click				26	// (Valve Steam Controller) left analog trigger, pulled in all the way // (digital value)
#constant k_EControllerActionOrigin_RightTrigger_Pull				27	// (Valve Steam Controller) right analog trigger, pulled by any amount // (analog value)
#constant k_EControllerActionOrigin_RightTrigger_Click				28	// (Valve Steam Controller) right analog trigger, pulled in all the way // (digital value)
#constant k_EControllerActionOrigin_LeftStick_Move					29	// (Valve Steam Controller) left joystick, movement on any axis // (analog value)
#constant k_EControllerActionOrigin_LeftStick_Click					30	// (Valve Steam Controller) left joystick, clicked in // (digital value)
#constant k_EControllerActionOrigin_LeftStick_DPadNorth				31	// (Valve Steam Controller) left joystick, digital movement // (upper quadrant)
#constant k_EControllerActionOrigin_LeftStick_DPadSouth				32	// (Valve Steam Controller) left joystick, digital movement // (lower quadrant)
#constant k_EControllerActionOrigin_LeftStick_DPadWest				33	// (Valve Steam Controller) left joystick, digital movement // (left quadrant)
#constant k_EControllerActionOrigin_LeftStick_DPadEast				34	// (Valve Steam Controller) left joystick, digital movement // (right quadrant)
#constant k_EControllerActionOrigin_Gyro_Move						35	// (Valve Steam Controller) gyroscope, analog movement in any axis
#constant k_EControllerActionOrigin_Gyro_Pitch						36	// (Valve Steam Controller) gyroscope, analog movement on the Pitch axis // (point head up to ceiling, point head down to floor)
#constant k_EControllerActionOrigin_Gyro_Yaw						37	// (Valve Steam Controller) gyroscope, analog movement on the Yaw axis // (turn head left to face one wall, turn head right to face other)
#constant k_EControllerActionOrigin_Gyro_Roll						38	// (Valve Steam Controller) gyroscope, analog movement on the Roll axis // (tilt head left towards shoulder, tilt head right towards other)
#constant k_EControllerActionOrigin_PS4_X							39	// (Sony Dualshock 4) digital face button X
#constant k_EControllerActionOrigin_PS4_Circle						40	// (Sony Dualshock 4) digital face button Circle
#constant k_EControllerActionOrigin_PS4_Triangle					41	// (Sony Dualshock 4) digital face button Triangle
#constant k_EControllerActionOrigin_PS4_Square						42	// (Sony Dualshock 4) digital face button Square
#constant k_EControllerActionOrigin_PS4_LeftBumper					43	// (Sony Dualshock 4) digital left shoulder button // (aka "left bumper")
#constant k_EControllerActionOrigin_PS4_RightBumper					44	// (Sony Dualshock 4) digital right shoulder button // (aka "right bumper")
#constant k_EControllerActionOrigin_PS4_Options						45	// (Sony Dualshock 4) digital options button // (aka "Start")
#constant k_EControllerActionOrigin_PS4_Share						46	// (Sony Dualshock 4) digital back button // (aka "Back")
#constant k_EControllerActionOrigin_PS4_LeftPad_Touch				47	// (Sony Dualshock 4) left half of the touchpad, in simple contact with a finger
#constant k_EControllerActionOrigin_PS4_LeftPad_Swipe				48	// (Sony Dualshock 4) left half of the touchpad, touch input on any axis
#constant k_EControllerActionOrigin_PS4_LeftPad_Click				49	// (Sony Dualshock 4) left half of the touchpad, digital click // (for the whole thing)
#constant k_EControllerActionOrigin_PS4_LeftPad_DPadNorth			50	// (Sony Dualshock 4) left half of the touchpad, digital click // (upper quadrant)
#constant k_EControllerActionOrigin_PS4_LeftPad_DPadSouth			51	// (Sony Dualshock 4) left half of the touchpad, digital click // (lower quadrant)
#constant k_EControllerActionOrigin_PS4_LeftPad_DPadWest			52	// (Sony Dualshock 4) left half of the touchpad, digital click // (left quadrant)
#constant k_EControllerActionOrigin_PS4_LeftPad_DPadEast			53	// (Sony Dualshock 4) left half of the touchpad, digital click // (right quadrant)
#constant k_EControllerActionOrigin_PS4_RightPad_Touch				54	// (Sony Dualshock 4) right half of the touchpad, in simple contact with a finger
#constant k_EControllerActionOrigin_PS4_RightPad_Swipe				55	// (Sony Dualshock 4) right half of the touchpad, touch input on any axis
#constant k_EControllerActionOrigin_PS4_RightPad_Click				56	// (Sony Dualshock 4) right half of the touchpad, digital click // (for the whole thing)
#constant k_EControllerActionOrigin_PS4_RightPad_DPadNorth			57	// (Sony Dualshock 4) right half of the touchpad, digital click // (upper quadrant)
#constant k_EControllerActionOrigin_PS4_RightPad_DPadSouth			58	// (Sony Dualshock 4) right half of the touchpad, digital click // (lower quadrant)
#constant k_EControllerActionOrigin_PS4_RightPad_DPadWest			59	// (Sony Dualshock 4) right half of the touchpad, digital click // (left quadrant)
#constant k_EControllerActionOrigin_PS4_RightPad_DPadEast			60	// (Sony Dualshock 4) right half of the touchpad, digital click // (right quadrant)
#constant k_EControllerActionOrigin_PS4_CenterPad_Touch				61	// (Sony Dualshock 4) unified touchpad, in simple contact with a finger
#constant k_EControllerActionOrigin_PS4_CenterPad_Swipe				62	// (Sony Dualshock 4) unified touchpad, touch input on any axis
#constant k_EControllerActionOrigin_PS4_CenterPad_Click				63	// (Sony Dualshock 4) unified touchpad, digital click // (for the whole thing)
#constant k_EControllerActionOrigin_PS4_CenterPad_DPadNorth			64	// (Sony Dualshock 4) unified touchpad, digital click // (upper quadrant)
#constant k_EControllerActionOrigin_PS4_CenterPad_DPadSouth			65	// (Sony Dualshock 4) unified touchpad, digital click // (lower quadrant)
#constant k_EControllerActionOrigin_PS4_CenterPad_DPadWest			66	// (Sony Dualshock 4) unified touchpad, digital click // (left quadrant)
#constant k_EControllerActionOrigin_PS4_CenterPad_DPadEast			67	// (Sony Dualshock 4) unified touchpad, digital click // (right quadrant)
#constant k_EControllerActionOrigin_PS4_LeftTrigger_Pull			68	// (Sony Dualshock 4) left analog trigger, pulled by any amount // (analog value)
#constant k_EControllerActionOrigin_PS4_LeftTrigger_Click			69	// (Sony Dualshock 4) left analog trigger, pulled in all the way // (digital value)
#constant k_EControllerActionOrigin_PS4_RightTrigger_Pull			70	// (Sony Dualshock 4) right analog trigger, pulled by any amount // (analog value)
#constant k_EControllerActionOrigin_PS4_RightTrigger_Click			71	// (Sony Dualshock 4) right analog trigger, pulled in all the way // (digital value)
#constant k_EControllerActionOrigin_PS4_LeftStick_Move				72	// (Sony Dualshock 4) left joystick, movement on any axis // (analog value)
#constant k_EControllerActionOrigin_PS4_LeftStick_Click				73	// (Sony Dualshock 4) left joystick, clicked in // (digital value)
#constant k_EControllerActionOrigin_PS4_LeftStick_DPadNorth			74	// (Sony Dualshock 4) left joystick, digital movement // (upper quadrant)
#constant k_EControllerActionOrigin_PS4_LeftStick_DPadSouth			75	// (Sony Dualshock 4) left joystick, digital movement // (lower quadrant)
#constant k_EControllerActionOrigin_PS4_LeftStick_DPadWest			76	// (Sony Dualshock 4) left joystick, digital movement // (left quadrant)
#constant k_EControllerActionOrigin_PS4_LeftStick_DPadEast			77	// (Sony Dualshock 4) left joystick, digital movement // (right quadrant)
#constant k_EControllerActionOrigin_PS4_RightStick_Move				78	// (Sony Dualshock 4) right joystick, movement on any axis // (analog value)
#constant k_EControllerActionOrigin_PS4_RightStick_Click			79	// (Sony Dualshock 4) right joystick, clicked in // (digital value)
#constant k_EControllerActionOrigin_PS4_RightStick_DPadNorth		80	// (Sony Dualshock 4) right joystick, digital movement // (upper quadrant)
#constant k_EControllerActionOrigin_PS4_RightStick_DPadSouth		81	// (Sony Dualshock 4) right joystick, digital movement // (lower quadrant)
#constant k_EControllerActionOrigin_PS4_RightStick_DPadWest			82	// (Sony Dualshock 4) right joystick, digital movement // (left quadrant)
#constant k_EControllerActionOrigin_PS4_RightStick_DPadEast			83	// (Sony Dualshock 4) right joystick, digital movement // (right quadrant)
#constant k_EControllerActionOrigin_PS4_DPad_North					84	// (Sony Dualshock 4) digital pad, pressed // (upper quadrant)
#constant k_EControllerActionOrigin_PS4_DPad_South					85	// (Sony Dualshock 4) digital pad, pressed // (lower quadrant)
#constant k_EControllerActionOrigin_PS4_DPad_West					86	// (Sony Dualshock 4) digital pad, pressed // (left quadrant)
#constant k_EControllerActionOrigin_PS4_DPad_East					87	// (Sony Dualshock 4) digital pad, pressed // (right quadrant)
#constant k_EControllerActionOrigin_PS4_Gyro_Move					88	// (Sony Dualshock 4) gyroscope, analog movement in any axis
#constant k_EControllerActionOrigin_PS4_Gyro_Pitch					89	// (Sony Dualshock 4) gyroscope, analog movement on the Pitch axis // (point head up to ceiling, point head down to floor)
#constant k_EControllerActionOrigin_PS4_Gyro_Yaw					90	// (Sony Dualshock 4) gyroscope, analog movement on the Yaw axis // (turn head left to face one wall, turn head right to face other)
#constant k_EControllerActionOrigin_PS4_Gyro_Roll					91	// (Sony Dualshock 4) gyroscope, analog movement on the Roll axis // (tilt head left towards shoulder, tilt head right towards other shoulder)
#constant k_EControllerActionOrigin_XBoxOne_A						92	// (XB1) digital face button A
#constant k_EControllerActionOrigin_XBoxOne_B						93	// (XB1) digital face button B
#constant k_EControllerActionOrigin_XBoxOne_X						94	// (XB1) digital face button X
#constant k_EControllerActionOrigin_XBoxOne_Y						95	// (XB1) digital face button Y
#constant k_EControllerActionOrigin_XBoxOne_LeftBumper				96	// (XB1) digital left shoulder button // (aka "left bumper")
#constant k_EControllerActionOrigin_XBoxOne_RightBumper				97	// (XB1) digital right shoulder button // (aka "right bumper")
#constant k_EControllerActionOrigin_XBoxOne_Menu					98	// (XB1) digital menu button // (aka "start")
#constant k_EControllerActionOrigin_XBoxOne_View					99	// (XB1) digital view button // (aka "back")
#constant k_EControllerActionOrigin_XBoxOne_LeftTrigger_Pull		100	// (XB1) left analog trigger, pulled by any amount // (analog value)
#constant k_EControllerActionOrigin_XBoxOne_LeftTrigger_Click		101	// (XB1) left analog trigger, pulled in all the way // (digital value)
#constant k_EControllerActionOrigin_XBoxOne_RightTrigger_Pull		102	// (XB1) right analog trigger, pulled by any amount // (analog value)
#constant k_EControllerActionOrigin_XBoxOne_RightTrigger_Click		103	// (XB1) right analog trigger, pulled in all the way // (digital value)
#constant k_EControllerActionOrigin_XBoxOne_LeftStick_Move			104	// (XB1) left joystick, movement on any axis // (analog value)
#constant k_EControllerActionOrigin_XBoxOne_LeftStick_Click			105	// (XB1) left joystick, clicked in // (digital value)
#constant k_EControllerActionOrigin_XBoxOne_LeftStick_DPadNorth		106	// (XB1) left joystick, digital movement // (upper quadrant)
#constant k_EControllerActionOrigin_XBoxOne_LeftStick_DPadSouth		107	// (XB1) left joystick, digital movement // (lower quadrant)
#constant k_EControllerActionOrigin_XBoxOne_LeftStick_DPadWest		108	// (XB1) left joystick, digital movement // (left quadrant)
#constant k_EControllerActionOrigin_XBoxOne_LeftStick_DPadEast		109	// (XB1) left joystick, digital movement // (right quadrant)
#constant k_EControllerActionOrigin_XBoxOne_RightStick_Move			110	// (XB1) right joystick, movement on any axis // (analog value)
#constant k_EControllerActionOrigin_XBoxOne_RightStick_Click		111	// (XB1) right joystick, clicked in // (digital value)
#constant k_EControllerActionOrigin_XBoxOne_RightStick_DPadNorth	112	// (XB1) right joystick, digital movement // (upper quadrant)
#constant k_EControllerActionOrigin_XBoxOne_RightStick_DPadSouth	113	// (XB1) right joystick, digital movement // (lower quadrant)
#constant k_EControllerActionOrigin_XBoxOne_RightStick_DPadWest		114	// (XB1) right joystick, digital movement // (left quadrant)
#constant k_EControllerActionOrigin_XBoxOne_RightStick_DPadEast		115	// (XB1) right joystick, digital movement // (right quadrant)
#constant k_EControllerActionOrigin_XBoxOne_DPad_North				116	// (XB1) digital pad, pressed // (upper quadrant)
#constant k_EControllerActionOrigin_XBoxOne_DPad_South				117	// (XB1) digital pad, pressed // (lower quadrant)
#constant k_EControllerActionOrigin_XBoxOne_DPad_West				118	// (XB1) digital pad, pressed // (left quadrant)
#constant k_EControllerActionOrigin_XBoxOne_DPad_East				119	// (XB1) digital pad, pressed // (right quadrant)
#constant k_EControllerActionOrigin_XBox360_A						120	// (X360) digital face button A
#constant k_EControllerActionOrigin_XBox360_B						121	// (X360) digital face button B
#constant k_EControllerActionOrigin_XBox360_X						122	// (X360) digital face button X
#constant k_EControllerActionOrigin_XBox360_Y						123	// (X360) digital face button Y
#constant k_EControllerActionOrigin_XBox360_LeftBumper				124	// (X360) digital left shoulder button // (aka "left bumper")
#constant k_EControllerActionOrigin_XBox360_RightBumper				125	// (X360) digital right shoulder button // (aka "right bumper")
#constant k_EControllerActionOrigin_XBox360_Start					126	// (X360) digital start button
#constant k_EControllerActionOrigin_XBox360_Back					127	// (X360) digital back button
#constant k_EControllerActionOrigin_XBox360_LeftTrigger_Pull		128	// (X360) left analog trigger, pulled by any amount // (analog value)
#constant k_EControllerActionOrigin_XBox360_LeftTrigger_Click		129	// (X360) left analog trigger, pulled in all the way // (digital value)
#constant k_EControllerActionOrigin_XBox360_RightTrigger_Pull		130	// (X360) right analog trigger, pulled by any amount // (analog value)
#constant k_EControllerActionOrigin_XBox360_RightTrigger_Click		131	// (X360) right analog trigger, pulled in all the way // (digital value)
#constant k_EControllerActionOrigin_XBox360_LeftStick_Move			132	// (X360) left joystick, movement on any axis // (analog value)
#constant k_EControllerActionOrigin_XBox360_LeftStick_Click			133	// (X360) left joystick, clicked in // (digital value)
#constant k_EControllerActionOrigin_XBox360_LeftStick_DPadNorth		134	// (X360) left joystick, digital movement // (upper quadrant)
#constant k_EControllerActionOrigin_XBox360_LeftStick_DPadSouth		135	// (X360) left joystick, digital movement // (lower quadrant)
#constant k_EControllerActionOrigin_XBox360_LeftStick_DPadWest		136	// (X360) left joystick, digital movement // (left quadrant)
#constant k_EControllerActionOrigin_XBox360_LeftStick_DPadEast		137	// (X360) left joystick, digital movement // (right quadrant)
#constant k_EControllerActionOrigin_XBox360_RightStick_Move			138	// (X360) right joystick, movement on any axis // (analog value)
#constant k_EControllerActionOrigin_XBox360_RightStick_Click		139	// (X360) right joystick, clicked in // (digital value)
#constant k_EControllerActionOrigin_XBox360_RightStick_DPadNorth	140	// (X360) right joystick, digital movement // (upper quadrant)
#constant k_EControllerActionOrigin_XBox360_RightStick_DPadSouth	141	// (X360) right joystick, digital movement // (lower quadrant)
#constant k_EControllerActionOrigin_XBox360_RightStick_DPadWest		142	// (X360) right joystick, digital movement // (left quadrant)
#constant k_EControllerActionOrigin_XBox360_RightStick_DPadEast		143	// (X360) right joystick, digital movement // (right quadrant)
#constant k_EControllerActionOrigin_XBox360_DPad_North				144	// (X360) digital pad, pressed // (upper quadrant)
#constant k_EControllerActionOrigin_XBox360_DPad_South				145	// (X360) digital pad, pressed // (lower quadrant)
#constant k_EControllerActionOrigin_XBox360_DPad_West				146	// (X360) digital pad, pressed // (left quadrant)
#constant k_EControllerActionOrigin_XBox360_DPad_East				147	// (X360) digital pad, pressed // (right quadrant)
#constant k_EControllerActionOrigin_Count							196	// The number of values in this enum, useful for iterating.

// EControllerSourceMode
//--------------------------------------------------------------
// The virtual input mode imposed by the configurator upon a controller source. 
// For instance, the configurator can make an analog joystick behave like a Dpad with four digital inputs; the EControllerSource would be k_EControllerSource_Joystick and the EControllerSourceMode would be k_EControllerSourceMode_Dpad.
// The mode also changes the input data received by any associated actions.
#constant k_EControllerSourceMode_None				0	// No input mode.
#constant k_EControllerSourceMode_Dpad				1	// A digital pad -- four digital directional buttons fused together in a cross pattern, such that only one button from each axis can be pressed at any given time.
#constant k_EControllerSourceMode_Buttons			2	// ???
#constant k_EControllerSourceMode_FourButtons		3	// Four digital face buttons, any of which can be pressed simultaneously
#constant k_EControllerSourceMode_AbsoluteMouse		4	
#constant k_EControllerSourceMode_RelativeMouse		5	
#constant k_EControllerSourceMode_JoystickMove		6	
#constant k_EControllerSourceMode_JoystickMouse		7	
#constant k_EControllerSourceMode_JoystickCamera	8	
#constant k_EControllerSourceMode_ScrollWheel		9	
#constant k_EControllerSourceMode_Trigger			10	
#constant k_EControllerSourceMode_TouchMenu			11	
#constant k_EControllerSourceMode_MouseJoystick		12	
#constant k_EControllerSourceMode_MouseRegion		13	
#constant k_EControllerSourceMode_RadialMenu		14	
#constant k_EControllerSourceMode_SingleButton		15	
#constant k_EControllerSourceMode_Switches			16	

// ESteamInputType
//--------------------------------------------------------------
// Represents the device model for a given piece of hardware.
#constant k_ESteamInputType_Unknown				0	// Catch-all for unrecognized devices
#constant k_ESteamInputType_SteamController		1	// Valve's Steam Controller
#constant k_ESteamInputType_XBox360Controller	2	// Microsoft's XBox 360 Controller
#constant k_ESteamInputType_XBoxOneController	3	// Microsoft's XBox One Controller
#constant k_ESteamInputType_GenericXInput		4	// Any generic 3rd-party XInput device
#constant k_ESteamInputType_PS4Controller		5	// Sony's Playstation 4 Controller

// ESteamControllerPad
//--------------------------------------------------------------
// A touchpad region on a Steam Controller Device.
#constant k_ESteamControllerPad_Left	0	// The left touchpad region on a Steam Controller Device. Compatible models: VSC, DS4
#constant k_ESteamControllerPad_Right	1	// The right region on a Steam Controller Device. Compatible models: VSC, DS4


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

// Structs
//--------------------------------------------------------------
// Favorite game information.
// Obtainable from: GetFavoriteGameJSON.
Type FavoriteGameInfo_t
	AppID as integer
	IP as string
	ConnectPort as integer
	QueryPort as integer
	Flags as integer
	TimeLastPlayedOnServer as integer
EndType

// Game server information
// Obtainable from: GetLobbyGameServerJSON
Type GameServerInfo_t // aka LobbyGameCreated_t
	hLobby as integer
	IP as string
	Port as integer
	hGameServer as integer
EndType

// Constants
//--------------------------------------------------------------
//~ #constant HSERVERQUERY_INVALID						0xffffffff	
//~ #constant k_nMaxLobbyKeyLength						255		// Maximum number of characters a lobby metadata key can be.
#constant k_unFavoriteFlagFavorite					0x01	// This favorite game server entry is for the favorites list.
#constant k_unFavoriteFlagHistory					0x02	// This favorite game server entry is for the history list.
#constant k_unFavoriteFlagNone						0x00	// This favorite game server has no flags set.
//~ #constant STEAMMATCHMAKINGSERVERS_INTERFACE_VERSION	"SteamMatchMakingServers002"	
//~ #constant STEAMMATCHMAKING_INTERFACE_VERSION		"SteamMatchMaking009"	


////////////////////////////////////////////////////////////////
// ISteamMusic
// https://partner.steamgames.com/doc/api/ISteamMusic
////////////////////////////////////////////////////////////////

// AudioPlayback_Status
//--------------------------------------------------------------
// Specifies the current playback status.
#constant AudioPlayback_Undefined	0	// The Steam music interface probably isn't enabled.
#constant AudioPlayback_Playing		1	// Steam Music is currently playing.
#constant AudioPlayback_Paused		2	// Steam Music is currently paused.
#constant AudioPlayback_Idle		3	// Steam Music is currently stopped.

// Constants
//--------------------------------------------------------------
//~ #constant STEAMMUSIC_INTERFACE_VERSION	"STEAMMUSIC_INTERFACE_VERSION001"

////////////////////////////////////////////////////////////////
// ISteamRemoteStorage
// https://partner.steamgames.com/doc/api/ISteamRemoteStorage
////////////////////////////////////////////////////////////////

// ERemoteStoragePlatform
//--------------------------------------------------------------
// Sync Platforms flags. These can be used with SetSyncPlatforms to restrict a file to a specific OS.
#constant k_ERemoteStoragePlatformNone			0x0	// This file will not be downloaded on any platform.
#constant k_ERemoteStoragePlatformWindows		0x1	// This file will download on Windows.
#constant k_ERemoteStoragePlatformOSX			0x2	// This file will download on macOS.
#constant k_ERemoteStoragePlatformPS3			0x4	// This file will download on the Playstation 3.
#constant k_ERemoteStoragePlatformLinux			0x8	// This file will download on SteamOS/Linux.
#constant k_ERemoteStoragePlatformReserved2		0x16	// Reserved.
#constant k_ERemoteStoragePlatformAll 			0xffffffff	// This file will download on every platform. This is the default.


// Structs
//--------------------------------------------------------------
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

////////////////////////////////////////////////////////////////
// ISteamUtils
// https://partner.steamgames.com/doc/api/ISteamUtils
////////////////////////////////////////////////////////////////

// EGamepadTextInputLineMode
//--------------------------------------------------------------
// Controls number of allowed lines for the Big Picture gamepad text entry
#constant k_EGamepadTextInputLineModeSingleLine		0
#constant k_EGamepadTextInputLineModeMultipleLines	1

// EGamepadTextInputMode
//--------------------------------------------------------------
// Input modes for the Big Picture gamepad text entry
#constant k_EGamepadTextInputModeNormal		0
#constant k_EGamepadTextInputModePassword	1

// Constants
//--------------------------------------------------------------
//~ #constant STEAMUTILS_INTERFACE_VERSION	"SteamUtils009"
