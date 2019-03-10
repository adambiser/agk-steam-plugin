#option_explicit

////////////////////////////////////////////////////////////////
// NOTE:
// These constants come from the Steamworks SDK.  However, the
// k_ prefix has been removed from the constant names.  Prefixes
// use to indicate variable type are also removed. ie: un, cch
////////////////////////////////////////////////////////////////

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
#constant EAccountTypeInvalid			0	// Used for invalid Steam IDs.
#constant EAccountTypeIndividual		1	// Regular user account.
#constant EAccountTypeMultiseat			2	// Multiseat (e.g. cybercafe) account.
#constant EAccountTypeGameServer		3	// Persistent (not anonymous) game server account.
#constant EAccountTypeAnonGameServer	4	// Anonymous game server account.
#constant EAccountTypePending			5	// Pending.
#constant EAccountTypeContentServer		6	// Valve internal content server account.
#constant EAccountTypeClan				7	// Steam Group (clan).
#constant EAccountTypeChat				8	// Steam group chat or lobby.
#constant EAccountTypeConsoleUser		9	// Fake Steam ID for local PSN account on PS3 or Live account on 360, etc.
#constant EAccountTypeAnonUser			10	// Anonymous user account. (Used to create an account or reset a password)
#constant EAccountTypeMax				11	// Max of 16 items in this field

// EAppOwnershipFlags
//--------------------------------------------------------------
// Flags that represent the ownership information of an App ID. (Steam Internal usage only)
#constant EAppOwnershipFlags_None				0x0000	// Unknown.
#constant EAppOwnershipFlags_OwnsLicense		0x0001	// Owns a license for this app.
#constant EAppOwnershipFlags_FreeLicense		0x0002	// The user has not paid for the app.
#constant EAppOwnershipFlags_RegionRestricted	0x0004	// Owns app, but not allowed to play in current region.
#constant EAppOwnershipFlags_LowViolence		0x0008	// Only owns a low violence version.
#constant EAppOwnershipFlags_InvalidPlatform	0x0010	// App not supported on current platform.
#constant EAppOwnershipFlags_SharedLicense		0x0020	// License was shared by an authorized device via Steam Family Sharing.
#constant EAppOwnershipFlags_FreeWeekend		0x0040	// Owned by a free weekend licenses.
#constant EAppOwnershipFlags_RetailLicense		0x0080	// Has a retail license for game, (CD-Key etc) (Deprecated)
#constant EAppOwnershipFlags_LicenseLocked		0x0100	// Shared license is locked (in use) by other user.
#constant EAppOwnershipFlags_LicensePending		0x0200	// Owns app, but transaction is still pending. Can't install or play yet.
#constant EAppOwnershipFlags_LicenseExpired		0x0400	// Doesn't own app anymore since license expired.
#constant EAppOwnershipFlags_LicensePermanent	0x0800	// Permanent license, not borrowed, or guest or freeweekend etc.
#constant EAppOwnershipFlags_LicenseRecurring	0x1000	// Recurring license, user is charged periodically.
#constant EAppOwnershipFlags_LicenseCanceled	0x2000	// Mark as canceled, but might be still active if recurring.
#constant EAppOwnershipFlags_AutoGrant			0x4000	// Ownership is based on any kind of autogrant license.
#constant EAppOwnershipFlags_PendingGift		0x8000	// User has pending gift to redeem.
#constant EAppOwnershipFlags_RentalNotActivated	0x10000	// Rental hasn't been activated yet.
#constant EAppOwnershipFlags_Rental				0x20000	// Is a rental.
#constant EAppOwnershipFlags_SiteLicense		0x40000	// Is from a site license

// EAppReleaseState
//--------------------------------------------------------------
// Release state of an App ID. (Steam Internal usage only)
#constant EAppReleaseState_Unknown		0	// Unknown, can't get application information, or license info is missing.
#constant EAppReleaseState_Unavailable	1	// Even if user owns it, they can't see game at all.
#constant EAppReleaseState_Prerelease	2	// Can be purchased and is visible in games list, nothing else.
#constant EAppReleaseState_PreloadOnly	3	// Owners can preload app, not play it.
#constant EAppReleaseState_Released		4	// Owners can download and play app.

// EAppType
//--------------------------------------------------------------
// App ID type, more info is available on Types of Applications (Steam Internal usage only)
#constant EAppType_Invalid		0x000		// Unknown / invalid.
#constant EAppType_Game			0x001		// Playable game, default type.
#constant EAppType_Application	0x002		// Software application.
#constant EAppType_Tool			0x004		// SDKs, editors & dedicated servers.
#constant EAppType_Demo			0x008		// Game demo.
//~ #constant EAppType_Media_DEPRECATED	0x010	// Legacy - was used for game trailers, which are now just videos on the web.
#constant EAppType_DLC			0x020		// Downloadable Content.
#constant EAppType_Guide		0x040		// Game guide, PDF etc
#constant EAppType_Driver		0x080		// Hardware driver updater (ATI, Razor etc.)
#constant EAppType_Config		0x100		// Hidden app used to config Steam features (backpack, sales, etc.)
#constant EAppType_Hardware		0x200		// S hardware device (Steam Machine, Steam Controller, Steam Link, etc.)
#constant EAppType_Franchise	0x400		// A hub for collections of multiple apps, eg films, series, games.
#constant EAppType_Video		0x800		// A video component of either a Film or TVSeries (may be the feature, an episode, preview, making-of, etc.)
#constant EAppType_Plugin		0x1000		// Plug-in types for other Apps.
#constant EAppType_Music		0x2000		// Music files.
#constant EAppType_Series		0x4000		// Container app for video series.
#constant EAppType_Shortcut		0x40000000	// Just a shortcut, client side only.
#constant EAppType_DepotOnly	0x80000000	// Placeholder since depots and apps share the same namespace.

// EAuthSessionResponse
//--------------------------------------------------------------
// Callback return values for the ValidateAuthTicketResponse_t callback which is posted as a response to ISteamUser::BeginAuthSession and ISteamGameServer::BeginAuthSession.
#constant EAuthSessionResponseOK							0	// Steam has verified the user is online, the ticket is valid and ticket has not been reused.
#constant EAuthSessionResponseUserNotConnectedToSteam		1	// The user in question is not connected to steam.
#constant EAuthSessionResponseNoLicenseOrExpired			2	// The user doesn't have a license for this App ID or the ticket has expired.
#constant EAuthSessionResponseVACBanned						3	// The user is VAC banned for this game.
#constant EAuthSessionResponseLoggedInElseWhere				4	// The user account has logged in elsewhere and the session containing the game instance has been disconnected.
#constant EAuthSessionResponseVACCheckTimedOut				5	// VAC has been unable to perform anti-cheat checks on this user.
#constant EAuthSessionResponseAuthTicketCanceled			6	// The ticket has been canceled by the issuer.
#constant EAuthSessionResponseAuthTicketInvalidAlreadyUsed	7	// This ticket has already been used, it is not valid.
#constant EAuthSessionResponseAuthTicketInvalid				8	// This ticket is not from a user instance currently connected to steam.
#constant EAuthSessionResponsePublisherIssuedBan			9	// The user is banned for this game. The ban came via the web api and not VAC.

// EBeginAuthSessionResult
//--------------------------------------------------------------
// Results returned from ISteamUser::BeginAuthSession and ISteamGameServer::BeginAuthSession.
#constant EBeginAuthSessionResultOK				0	// Ticket is valid for this game and this Steam ID.
#constant EBeginAuthSessionResultInvalidTicket	1	// The ticket is invalid.
#constant EBeginAuthSessionResultDuplicateRequest	2	// A ticket has already been submitted for this Steam ID.
#constant EBeginAuthSessionResultInvalidVersion	3	// Ticket is from an incompatible interface version.
#constant EBeginAuthSessionResultGameMismatch		4	// Ticket is not for this game.
#constant EBeginAuthSessionResultExpiredTicket	5	// Ticket has expired.

// EBroadcastUploadResult
//--------------------------------------------------------------
// Broadcast upload result from BroadcastUploadStop_t.
#constant EBroadcastUploadResultNone				0	// Broadcast state unknown.
#constant EBroadcastUploadResultOK					1	// Broadcast was good, no problems.
#constant EBroadcastUploadResultInitFailed			2	// Broadcast init failed.
#constant EBroadcastUploadResultFrameFailed			3	// Broadcast frame upload failed.
#constant EBroadcastUploadResultTimeout				4	// Broadcast upload timed out.
#constant EBroadcastUploadResultBandwidthExceeded	5	// Broadcast send too much data.
#constant EBroadcastUploadResultLowFPS				6	// Broadcast FPS is too low.
#constant EBroadcastUploadResultMissingKeyFrames	7	// Broadcast sending not enough key frames.
#constant EBroadcastUploadResultNoConnection		8	// Broadcast client failed to connect to relay.
#constant EBroadcastUploadResultRelayFailed			9	// Relay dropped the upload.
#constant EBroadcastUploadResultSettingsChanged		10	// The client changed broadcast settings.
#constant EBroadcastUploadResultMissingAudio		11	// Client failed to send audio data.
#constant EBroadcastUploadResultTooFarBehind		12	// Clients was too slow uploading.
#constant EBroadcastUploadResultTranscodeBehind		13	// Server failed to keep up with transcode.

// EChatEntryType
//--------------------------------------------------------------
// Chat Entry Types.
// Returned by ISteamFriends::GetFriendMessage, ISteamFriends::GetClanChatMessage and ISteamMatchmaking::GetLobbyChatEntry.
#constant EChatEntryTypeInvalid				0	// Invalid.
#constant EChatEntryTypeChatMsg				1	// Normal text message from another user.
#constant EChatEntryTypeTyping				2	// The other user is typing, not used in multi-user chat.
#constant EChatEntryTypeInviteGame			3	// Invite from other user into that users current game.
#constant EChatEntryTypeEmote				4	// Text emote message (Deprecated, should be treated as ChatMsg).
#constant EChatEntryTypeLeftConversation	6	// A user has left the conversation (closed the chat window).
#constant EChatEntryTypeEntered				7	// User has entered the conversation, used in multi-user chat and group chat.
#constant EChatEntryTypeWasKicked			8	// User was kicked (Data: Steam ID of the user performing the kick).
#constant EChatEntryTypeWasBanned			9	// User was banned (Data: Steam ID of the user performing the ban).
#constant EChatEntryTypeDisconnected		10	// User disconnected.
#constant EChatEntryTypeHistoricalChat		11	// A chat message from user's chat history or offline message.
#constant EChatEntryTypeLinkBlocked			14	// A link was removed by the chat filter.

// EChatRoomEnterResponse
//--------------------------------------------------------------
// Chat Room Enter Responses.
#constant EChatRoomEnterResponseSuccess				1	// Success.
#constant EChatRoomEnterResponseDoesntExist			2	// Chat doesn't exist (probably closed).
#constant EChatRoomEnterResponseNotAllowed			3	// General Denied - You don't have the permissions needed to join the chat.
#constant EChatRoomEnterResponseFull				4	// Chat room has reached its maximum size.
#constant EChatRoomEnterResponseError				5	// Unexpected Error.
#constant EChatRoomEnterResponseBanned				6	// You are banned from this chat room and may not join.
#constant EChatRoomEnterResponseLimited				7	// Joining this chat is not allowed because you are a limited user (no value on account).
#constant EChatRoomEnterResponseClanDisabled		8	// Attempt to join a clan chat when the clan is locked or disabled.
#constant EChatRoomEnterResponseCommunityBan		9	// Attempt to join a chat when the user has a community lock on their account.
#constant EChatRoomEnterResponseMemberBlockedYou	10	// Join failed - a user that is in the chat has blocked you from joining.
#constant EChatRoomEnterResponseYouBlockedMember	11	// Join failed - you have blocked a user that is already in the chat.

// EChatSteamIDInstanceFlags
//--------------------------------------------------------------
// Special flags for Chat accounts - they go in the top 8 bits of the Steam ID's "instance", leaving 12 for the actual instances
#constant EChatAccountInstanceMask	0x00000FFF	// Top 8 bits are flags.
#constant EChatInstanceFlagClan		0x0080000 	// The Steam ID is for a Steam group.	// (k_unSteamAccountInstanceMask + 1) >> 1
#constant EChatInstanceFlagLobby	0x0040000	// The Steam ID is for a Lobby.			// (k_unSteamAccountInstanceMask + 1) >> 2
#constant EChatInstanceFlagMMSLobby	0x0020000	// The Steam ID is for a MMS Lobby.		// (k_unSteamAccountInstanceMask + 1) >> 3

// EDenyReason
//--------------------------------------------------------------
// Result values when a client failed to join or has been kicked from a game server. Obtained from GSClientDeny_t and GSClientKick_t.
#constant EDenyInvalid					0	// Unknown.
#constant EDenyInvalidVersion			1	// The client and server are not the same version.
#constant EDenyGeneric					2	// Generic.
#constant EDenyNotLoggedOn				3	// The client is not logged on.
#constant EDenyNoLicense				4	// The client does not have a license to play this game.
#constant EDenyCheater					5	// The client is VAC banned.
#constant EDenyLoggedInElseWhere		6	// The client is logged in elsewhere.
#constant EDenyUnknownText				7	
#constant EDenyIncompatibleAnticheat	8	
#constant EDenyMemoryCorruption			9	
#constant EDenyIncompatibleSoftware		10	
#constant EDenySteamConnectionLost		11	// The server lost connection to steam.
#constant EDenySteamConnectionError		12	// The server had a general error connecting to Steam.
#constant EDenySteamResponseTimedOut	13	// The server timed out connecting to Steam.
#constant EDenySteamValidationStalled	14	// The client has not authed with Steam yet.
#constant EDenySteamOwnerLeftGuestUser	15	// The owner of the shared game has left, called for each guest of the owner.

// EGameIDType
//--------------------------------------------------------------
// Used to describe the type of CGameID.
#constant EGameIDTypeApp		0	// The Game ID is a regular steam app.
#constant EGameIDTypeGameMod	1	// The Game ID is a mod.
#constant EGameIDTypeShortcut	2	// The game ID is a shortcut.
#constant EGameIDTypeP2P		3	// The game ID is a P2P file.

// ELaunchOptionType
//--------------------------------------------------------------
// Codes for well defined launch options, corresponds to "Launch Type" in the applications Launch Options which can be found on the General Installation Settings page.
// There is a special function BIsVRLaunchOptionType, to check a the type is any of the VR launch options.
#constant ELaunchOptionType_None			0	// Unspecified.
#constant ELaunchOptionType_Default			1	// Runs the app in default mode.
#constant ELaunchOptionType_SafeMode		2	// Runs the game in safe mode.
#constant ELaunchOptionType_Multiplayer		3	// Runs the game in multiplayer mode.
#constant ELaunchOptionType_Config			4	// Runs config tool for this game.
#constant ELaunchOptionType_OpenVR			5	// Runs game in VR mode using OpenVR.
#constant ELaunchOptionType_Server			6	// Runs dedicated server for this game.
#constant ELaunchOptionType_Editor			7	// Runs game editor.
#constant ELaunchOptionType_Manual			8	// Shows game manual.
#constant ELaunchOptionType_Benchmark		9	// Runs game benchmark.
#constant ELaunchOptionType_Option1			10	// Generic run option, uses description field for game name.
#constant ELaunchOptionType_Option2			11	// Generic run option, uses description field for game name.
#constant ELaunchOptionType_Option3			12	// Generic run option, uses description field for game name.
#constant ELaunchOptionType_OculusVR		13	// Runs game in VR mode using the Oculus SDK.
#constant ELaunchOptionType_OpenVROverlay	14	// Runs an OpenVR dashboard overlay.
#constant ELaunchOptionType_OSVR			15	// Runs game in VR mode using the OSVR SDK.
#constant ELaunchOptionType_Dialog			1000	// Show launch options dialog.

// EMarketingMessageFlags
//--------------------------------------------------------------
// Internal Steam marketing message flags that change how a client should handle them.
#constant EMarketingMessageFlagsNone				0	
#constant EMarketingMessageFlagsHighPriority		1 << 0
#constant EMarketingMessageFlagsPlatformWindows		1 << 1
#constant EMarketingMessageFlagsPlatformMac			1 << 2
#constant EMarketingMessageFlagsPlatformLinux		1 << 3
//~ #constant EMarketingMessageFlagsPlatformRestrictions	=	aggregate flags

// ENotificationPosition
//--------------------------------------------------------------
// Possible positions to have the overlay show notifications in. Used with ISteamUtils::SetOverlayNotificationPosition.
#constant EPositionTopLeft		0	// Top left corner.
#constant EPositionTopRight		1	// Top right corner.
#constant EPositionBottomLeft	2	// Bottom left corner.
#constant EPositionBottomRight	3	// Bottom right corner.

// EResult
//--------------------------------------------------------------
// Steam error result codes.
// These are frequently returned by functions, callbacks, and call results from both the Steamworks API and the Web API. An API may return arbitrary EResult codes, refer to the documentation for that API function or callback to see what it could return and what they mean in the context of that API call.
// This is similar to Win32's HRESULT type or POSIXs errno.
#constant EResultOK											1	// Success.
#constant EResultFail										2	// Generic failure.
#constant EResultNoConnection								3	// Your Steam client doesn't have a connection to the back-end.
//#constant EResultNoConnectionRetry						4	// OBSOLETE - removed
#constant EResultInvalidPassword							5	// Password/ticket is invalid.
#constant EResultLoggedInElsewhere							6	// The user is logged in elsewhere.
#constant EResultInvalidProtocolVer							7	// Protocol version is incorrect.
#constant EResultInvalidParam								8	// A parameter is incorrect.
#constant EResultFileNotFound								9	// File was not found.
#constant EResultBusy										10	// Called method is busy - action not taken.
#constant EResultInvalidState								11	// Called object was in an invalid state.
#constant EResultInvalidName								12	// The name was invalid.
#constant EResultInvalidEmail								13	// The email was invalid.
#constant EResultDuplicateName								14	// The name is not unique.
#constant EResultAccessDenied								15	// Access is denied.
#constant EResultTimeout									16	// Operation timed out.
#constant EResultBanned										17	// The user is VAC2 banned.
#constant EResultAccountNotFound							18	// Account not found.
#constant EResultInvalidSteamID								19	// The Steam ID was invalid.
#constant EResultServiceUnavailable							20	// The requested service is currently unavailable.
#constant EResultNotLoggedOn								21	// The user is not logged on.
#constant EResultPending									22	// Request is pending, it may be in process or waiting on third party.
#constant EResultEncryptionFailure							23	// Encryption or Decryption failed.
#constant EResultInsufficientPrivilege						24	// Insufficient privilege.
#constant EResultLimitExceeded								25	// Too much of a good thing.
#constant EResultRevoked									26	// Access has been revoked (used for revoked guest passes.)
#constant EResultExpired									27	// License/Guest pass the user is trying to access is expired.
#constant EResultAlreadyRedeemed							28	// Guest pass has already been redeemed by account, cannot be used again.
#constant EResultDuplicateRequest							29	// The request is a duplicate and the action has already occurred in the past, ignored this time.
#constant EResultAlreadyOwned								30	// All the games in this guest pass redemption request are already owned by the user.
#constant EResultIPNotFound									31	// IP address not found.
#constant EResultPersistFailed								32	// Failed to write change to the data store.
#constant EResultLockingFailed								33	// Failed to acquire access lock for this operation.
#constant EResultLogonSessionReplaced						34	// The logon session has been replaced.
#constant EResultConnectFailed								35	// Failed to connect.
#constant EResultHandshakeFailed							36	// The authentication handshake has failed.
#constant EResultIOFailure									37	// There has been a genic IO failure.
#constant EResultRemoteDisconnect							38	// The remote server has disconnected.
#constant EResultShoppingCartNotFound						39	// Failed to find the shopping cart requested.
#constant EResultBlocked									40	// A user blocked the action.
#constant EResultIgnored									41	// The target is ignoring sender.
#constant EResultNoMatch									42	// Nothing matching the request found.
#constant EResultAccountDisabled							43	// The account is disabled.
#constant EResultServiceReadOnly							44	// This service is not accepting content changes right now.
#constant EResultAccountNotFeatured							45	// Account doesn't have value, so this feature isn't available.
#constant EResultAdministratorOK							46	// Allowed to take this action, but only because requester is admin.
#constant EResultContentVersion								47	// A Version mismatch in content transmitted within the Steam protocol.
#constant EResultTryAnotherCM								48	// The current CM can't service the user making a request, user should try another.
#constant EResultPasswordRequiredToKickSession				49	// You are already logged in elsewhere, this cached credential login has failed.
#constant EResultAlreadyLoggedInElsewhere					50	// The user is logged in elsewhere. (Use EResultLoggedInElsewhere instead!)
#constant EResultSuspended									51	// Long running operation has suspended/paused. (eg. content download.)
#constant EResultCancelled									52	// Operation has been canceled, typically by user. (eg. a content download.)
#constant EResultDataCorruption								53	// Operation canceled because data is ill formed or unrecoverable.
#constant EResultDiskFull									54	// Operation canceled - not enough disk space.
#constant EResultRemoteCallFailed							55	// The remote or IPC call has failed.
#constant EResultPasswordUnset								56	// Password could not be verified as it's unset server side.
#constant EResultExternalAccountUnlinked					57	// External account (PSN, Facebook...) is not linked to a Steam account.
#constant EResultPSNTicketInvalid							58	// PSN ticket was invalid.
#constant EResultExternalAccountAlreadyLinked				59	// External account (PSN, Facebook...) is already linked to some other account, must explicitly request to replace/delete the link first.
#constant EResultRemoteFileConflict							60	// The sync cannot resume due to a conflict between the local and remote files.
#constant EResultIllegalPassword							61	// The requested new password is not allowed.
#constant EResultSameAsPreviousValue						62	// New value is the same as the old one. This is used for secret question and answer.
#constant EResultAccountLogonDenied							63	// Account login denied due to 2nd factor authentication failure.
#constant EResultCannotUseOldPassword						64	// The requested new password is not legal.
#constant EResultInvalidLoginAuthCode						65	// Account login denied due to auth code invalid.
#constant EResultAccountLogonDeniedNoMail					66	// Account login denied due to 2nd factor auth failure - and no mail has been sent.
#constant EResultHardwareNotCapableOfIPT					67	// The users hardware does not support Intel's Identity Protection Technology (IPT).
#constant EResultIPTInitError								68	// Intel's Identity Protection Technology (IPT) has failed to initialize.
#constant EResultParentalControlRestricted					69	// Operation failed due to parental control restrictions for current user.
#constant EResultFacebookQueryError							70	// Facebook query returned an error.
#constant EResultExpiredLoginAuthCode						71	// Account login denied due to an expired auth code.
#constant EResultIPLoginRestrictionFailed					72	// The login failed due to an IP resriction.
#constant EResultAccountLockedDown							73	// The current users account is currently locked for use. This is likely due to a hijacking and pending ownership verification.
#constant EResultAccountLogonDeniedVerifiedEmailRequired	74	// The logon failed because the accounts email is not verified.
#constant EResultNoMatchingURL								75	// There is no URL matching the provided values.
#constant EResultBadResponse								76	// Bad Response due to a Parse failure, missing field, etc.
#constant EResultRequirePasswordReEntry						77	// The user cannot complete the action until they re-enter their password.
#constant EResultValueOutOfRange							78	// The value entered is outside the acceptable range.
#constant EResultUnexpectedError							79	// Something happened that we didn't expect to ever happen.
#constant EResultDisabled									80	// The requested service has been configured to be unavailable.
#constant EResultInvalidCEGSubmission						81	// The files submitted to the CEG server are not valid.
#constant EResultRestrictedDevice							82	// The device being used is not allowed to perform this action.
#constant EResultRegionLocked								83	// The action could not be complete because it is region restricted.
#constant EResultRateLimitExceeded							84	// Temporary rate limit exceeded, try again later, different from EResultLimitExceeded which may be permanent.
#constant EResultAccountLoginDeniedNeedTwoFactor			85	// Need two-factor code to login.
#constant EResultItemDeleted								86	// The thing we're trying to access has been deleted.
#constant EResultAccountLoginDeniedThrottle					87	// Login attempt failed, try to throttle response to possible attacker.
#constant EResultTwoFactorCodeMismatch						88	// Two factor authentication (Steam Guard) code is incorrect.
#constant EResultTwoFactorActivationCodeMismatch			89	// The activation code for two-factor authentication (Steam Guard) didn't match.
#constant EResultAccountAssociatedToMultiplePartners		90	// The current account has been associated with multiple partners.
#constant EResultNotModified								91	// The data has not been modified.
#constant EResultNoMobileDevice								92	// The account does not have a mobile device associated with it.
#constant EResultTimeNotSynced								93	// The time presented is out of range or tolerance.
#constant EResultSmsCodeFailed								94	// SMS code failure - no match, none pending, etc.
#constant EResultAccountLimitExceeded						95	// Too many accounts access this resource.
#constant EResultAccountActivityLimitExceeded				96	// Too many changes to this account.
#constant EResultPhoneActivityLimitExceeded					97	// Too many changes to this phone.
#constant EResultRefundToWallet								98	// Cannot refund to payment method, must use wallet.
#constant EResultEmailSendFailure							99	// Cannot send an email.
#constant EResultNotSettled									100	// Can't perform operation until payment has settled.
#constant EResultNeedCaptcha								101	// The user needs to provide a valid captcha.
#constant EResultGSLTDenied									102	// A game server login token owned by this token's owner has been banned.
#constant EResultGSOwnerDenied								103	// Game server owner is denied for some other reason such as account locked, community ban, vac ban, missing phone, etc.
#constant EResultInvalidItemType							104	// The type of thing we were requested to act on is invalid.
#constant EResultIPBanned									105	// The IP address has been banned from taking this action.
#constant EResultGSLTExpired								106	// This Game Server Login Token (GSLT) has expired from disuse; it can be reset for use.
#constant EResultInsufficientFunds							107	// user doesn't have enough wallet funds to complete the action
#constant EResultTooManyPending								108	// There are too many of this thing pending already
#constant EResultNoSiteLicensesFound						109	// No site licenses found
#constant EResultWGNetworkSendExceeded						110	// the WG couldn't send a response because we exceeded max network send size
#constant EResultAccountNotFriends							111	// the user is not mutually friends
#constant EResultLimitedUserAccount							112	// the user is limited
#constant EResultCantRemoveItem								113	// item can't be removed

// ESteamUserStatType
//--------------------------------------------------------------
// Fields used internally to store user stats.
#constant ESteamUserStatTypeINVALID				0	// Invalid.
#constant ESteamUserStatTypeINT					1	// 32-bit int stat.
#constant ESteamUserStatTypeFLOAT				2	// 32-bit float stat.
#constant ESteamUserStatTypeAVGRATE				3	// Read as FLOAT, set with count / session length.
#constant ESteamUserStatTypeACHIEVEMENTS		4	// Standard user achievement.
//~ #constant ESteamUserStatTypeGROUPACHIEVEMENTS	5	// Deprecated.
//~ #constant ESteamUserStatTypeMAX				6	// Total number of user stat types, used for sanity checks.

// EUniverse
//--------------------------------------------------------------
// Steam universes. Each universe is a self-contained Steam instance.
#constant EUniverseInvalid	0	// Invalid.
#constant EUniversePublic	1	// The standard public universe.
#constant EUniverseBeta		2	// Beta universe used inside Valve.
#constant EUniverseInternal	3	// Internal universe used inside Valve.
#constant EUniverseDev		4	// Dev universe used inside Valve.
#constant EUniverseMax		5	// Total number of universes, used for sanity checks.

// EUserHasLicenseForAppResult
//--------------------------------------------------------------
// Result of ISteamUser::UserHasLicenseForApp.
#constant EUserHasLicenseResultHasLicense			0	// The user has a license for specified app.
#constant EUserHasLicenseResultDoesNotHaveLicense	1	// The user does not have a license for the specified app.
#constant EUserHasLicenseResultNoAuth				2	// The user has not been authenticated.

// EVoiceResult
//--------------------------------------------------------------
// Results for use with the Steam Voice functions.
#constant EVoiceResultOK						0	// The call has completed successfully.
#constant EVoiceResultNotInitialized			1	// The Steam Voice interface has not been initialized.
#constant EVoiceResultNotRecording				2	// Steam Voice is not currently recording.
#constant EVoiceResultNoData					3	// There is no voice data available.
#constant EVoiceResultBufferTooSmall			4	// The provided buffer is too small to receive the data.
#constant EVoiceResultDataCorrupted				5	// The voice data has been corrupted.
#constant EVoiceResultRestricted				6	// The user is chat restricted.
//~ #constant EVoiceResultUnsupportedCodec		7	// Deprecated.
//~ #constant EVoiceResultReceiverOutOfDate		8	// Deprecated.
//~ #constant EVoiceResultReceiverDidNotAnswer	9	// Deprecated.

// EVRHMDType
//--------------------------------------------------------------
// Code points for VR HMD vendors and models. Use the special functions BIsOculusHMD and BIsViveHMD to check for a specific brand.
#constant eEVRHMDType_None				-1	// Unknown vendor and model.
#constant eEVRHMDType_Unknown			0	// Unknown vendor and model.
#constant eEVRHMDType_HTC_Dev			1	// Original HTC dev kits.
#constant eEVRHMDType_HTC_VivePre		2	// HTC Vive Pre dev kits.
#constant eEVRHMDType_HTC_Vive			3	// HTC Vive Consumer Release.
#constant eEVRHMDType_HTC_Unknown		20	// Unknown HTC HMD.
#constant eEVRHMDType_Oculus_DK1		21	// Oculus Rift Development Kit 1.
#constant eEVRHMDType_Oculus_DK2		22	// Oculus Rift Development Kit 2
#constant eEVRHMDType_Oculus_Rift		23	// Oculus Rift Consumer Version 1.
#constant eEVRHMDType_Oculus_Unknown	40	// Unknown Oculus HMD.

// Constants
//--------------------------------------------------------------
// These are constants which are defined for use with steam_api.
//~ #constant BREAKPAD_INVALID_HANDLE		0
//~ #constant GameExtraInfoMax				64			// The maximum size (in UTF-8 bytes, including the null terminator) of the pchExtraInfo parameter of ISteamUser::TrackAppUsageEvent.
//~ #constant HAuthTicketInvalid			0			// An invalid user authentication ticket.
//~ #constant steamIDLanModeGS				?			// Steam ID comes from a user game connection to an sv_lan GS
//~ #constant steamIDNil					0			// Generic invalid CSteamID.
//~ #constant steamIDNonSteamGS				?			// This Steam ID can come from a user game connection to a GS that isn't using the steam authentication system but still 
														//~ // wants to support the "Join Game" option in the friends list
//~ #constant steamIDNotInitYetGS			?			// This Steam ID can come from a user game connection to a GS that has just booted but hasnt yet even initialized its steam3
														//~ // component and started logging on.
//~ #constant steamIDOutofDateGS			?			// This Steam ID comes from a user game connection to an out of date GS that hasnt implemented the protocol to provide its Steam ID
//~ #constant APICallInvalid				0x0			// An Invalid Steam API Call handle.
//~ #constant AppIdInvalid					0x0			// An Invalid App ID.
//~ #constant DepotIdInvalid					0x0			// An Invalid Depot ID.
//~ #constant SteamAccountIDMask			0xFFFFFFFF	// Used in CSteamID to mask out the AccountID_t.
//~ #constant SteamAccountInstanceMask		0x000FFFFF	// Used in CSteamID to mask out the account instance.
//~ #constant SteamUserConsoleInstance		2			// Used by CSteamID to identify users logged in from a console.
//~ #constant SteamUserDesktopInstance		1			// Used by CSteamID to identify users logged in from the desktop client.
//~ #constant SteamUserWebInstance			4			// Used by CSteamID to identify users logged in from the web.
#constant QUERY_PORT_ERROR					0xFFFE		// We were unable to get the query port for this server.
#constant QUERY_PORT_NOT_INITIALIZED		0xFFFF		// Constants used for query ports.  We haven't asked the GS for this query port's actual value yet.

////////////////////////////////////////////////////////////////
// ISteamFriends
// https://partner.steamgames.com/doc/api/ISteamFriends
////////////////////////////////////////////////////////////////

// EFriendFlags
//--------------------------------------------------------------
// Flags for enumerating friends list, or quickly checking the relationship between users.
#constant EFriendFlagNone					0x00	// None.
#constant EFriendFlagBlocked				0x01	// Users that the current user has blocked from contacting.
#constant EFriendFlagFriendshipRequested	0x02	// Users that have sent a friend invite to the current user.
#constant EFriendFlagImmediate				0x04	// The current user's "regular" friends.
#constant EFriendFlagClanMember				0x08	// Users that are in one of the same (small) Steam groups as the current user.
#constant EFriendFlagOnGameServer			0x10	// Users that are on the same game server; as set by SetPlayedWith.
#constant EFriendFlagRequestingFriendship	0x80	// Users that the current user has sent friend invites to.
#constant EFriendFlagRequestingInfo			0x100	// Users that are currently sending additional info about themselves after a call to RequestUserInformation
#constant EFriendFlagIgnored				0x200	// Users that the current user has ignored from contacting them.
#constant EFriendFlagIgnoredFriend			0x400	// Users that have ignored the current user; but the current user still knows about them.
#constant EFriendFlagChatMember				0x1000	// Users in one of the same chats.
#constant EFriendFlagAll					0xFFFF	// Returns all friend flags.

// FriendRelationship
//--------------------------------------------------------------
// Declares the set of relationships that Steam users may have.
#constant EFriendRelationshipNone					0	// The users have no relationship.
#constant EFriendRelationshipBlocked				1	// The user has just clicked Ignore on an friendship invite. This doesn't get stored.
#constant EFriendRelationshipRequestRecipient		2	// The user has requested to be friends with the current user.
#constant EFriendRelationshipFriend					3	// A "regular" friend.
#constant EFriendRelationshipRequestInitiator		4	// The current user has sent a friend invite.
#constant EFriendRelationshipIgnored				5	// The current user has explicit blocked this other user from comments/chat/etc. This is stored.
#constant EFriendRelationshipIgnoredFriend			6	// The user has ignored the current user.
//~ #constant EFriendRelationshipSuggested_DEPRECATED	7	// Deprecated -- Unused.
//~ #constant EFriendRelationshipMax					8	// The total number of friend relationships used for looping and verification.

// EOverlayToStoreFlag
//--------------------------------------------------------------
// These values are passed as parameters to the store with ActivateGameOverlayToStore and modify the behavior when the page opens.
#constant EOverlayToStoreFlag_None				0	// No
#constant EOverlayToStoreFlag_AddToCart			1	// Add the specified app ID to the users cart.
#constant EOverlayToStoreFlag_AddToCartAndShow	2	// Add the specified app ID to the users cart and show the store page.

// EPersonaChange
//--------------------------------------------------------------
// used in PersonaStateChange_t::m_nChangeFlags to describe what's changed about a user
// these flags describe what the client has learned has changed recently, so on startup you'll see a name, avatar & relationship change for every friend
#constant EPersonaChangeName				0x0001	
#constant EPersonaChangeStatus				0x0002	
#constant EPersonaChangeComeOnline			0x0004	
#constant EPersonaChangeGoneOffline			0x0008	
#constant EPersonaChangeGamePlayed			0x0010	
#constant EPersonaChangeGameServer			0x0020	
#constant EPersonaChangeAvatar				0x0040	
#constant EPersonaChangeJoinedSource		0x0080	
#constant EPersonaChangeLeftSource			0x0100	
#constant EPersonaChangeRelationshipChanged	0x0200	
#constant EPersonaChangeNameFirstSet		0x0400	
#constant EPersonaChangeFacebookInfo		0x0800	
#constant EPersonaChangeNickname			0x1000	
#constant EPersonaChangeSteamLevel			0x2000	

// EPersonaState
//--------------------------------------------------------------
// List of states a Steam friend can be in.
#constant EPersonaStateOffline			0	// Friend is not currently logged on.
#constant EPersonaStateOnline			1	// Friend is logged on.
#constant EPersonaStateBusy				2	// User is on, but busy.
#constant EPersonaStateAway				3	// Auto-away feature.
#constant EPersonaStateSnooze			4	// Auto-away for a long time.
#constant EPersonaStateLookingToTrade	5	// Online, trading.
#constant EPersonaStateLookingToPlay	6	// Online, wanting to play.
//~ #constant EPersonaStateMax			7	// The total number of states. Only used for looping and validation.

// EUserRestriction
//--------------------------------------------------------------
// User restriction flags. These are returned by GetUserRestrictions.
#constant nUserRestrictionNone			0	// No known chat/content restriction.
#constant nUserRestrictionUnknown		1	// We don't know yet, the user is offline.
#constant nUserRestrictionAnyChat		2	// User is not allowed to (or can't) send/recv any chat.
#constant nUserRestrictionVoiceChat		4	// User is not allowed to (or can't) send/recv voice chat.
#constant nUserRestrictionGroupChat		8	// User is not allowed to (or can't) send/recv group chat.
#constant nUserRestrictionRating		16	// User is too young according to rating in current region.
#constant nUserRestrictionGameInvites	32	// User cannot send or recv game invites, for example if they are on mobile.
#constant nUserRestrictionTrading		64	// User cannot participate in trading, for example if they are on a console or mobile.

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
//~ #constant cchMaxFriendsGroupName			64		// The maximum length that a friends group name can be (not including the null-terminator!)
//~ #constant cchMaxRichPresenceKeyLength		64		// The maximum length that a rich presence key can be.
//~ #constant cchMaxRichPresenceKeys			20		// The maximum amount of rich presence keys that can be set.
//~ #constant cchMaxRichPresenceValueLength		256		// The maximum length that a rich presence value can be.
//~ #constant cchPersonaNameMax					128		// Maximum number of UTF-8 bytes in a users persona (display) name.
//~ #constant cEnumerateFollowersMax			50		// The maximum number of followers that will be returned in a FriendsEnumerateFollowingList_t call result at once.
//~ #constant cFriendsGroupLimit				100		// Deprecated - Unused.
//~ #constant cubChatMetadataMax				8192	// Maximum size in bytes that chat room, lobby, or chat/lobby member metadata may have.
//~ #constant cwchPersonaNameMax				32		// The maximum amount of UTF-16 characters in a users persona (display) name.
//~ #constant FriendsGroupID_Invalid			-1		// Invalid friends group identifier.
//~ #constant STEAMFRIENDS_INTERFACE_VERSION	"SteamFriends015"	

////////////////////////////////////////////////////////////////
// ISteamInput
// https://partner.steamgames.com/doc/api/ISteamInput
////////////////////////////////////////////////////////////////

// EInputActionOrigin
//--------------------------------------------------------------
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
//#constant EInputActionOrigin_MaximumPossibleValue			32767	// Origins are currently a maximum of 16 bits.

// EInputSourceMode
//--------------------------------------------------------------
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
//--------------------------------------------------------------
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
//--------------------------------------------------------------
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
//--------------------------------------------------------------
// A touchpad region on a Steam Controller Device.
#constant ESteamControllerPad_Left	0	// The left touchpad region on a Steam Controller Device. Compatible models: VSC, DS4
#constant ESteamControllerPad_Right	1	// The right region on a Steam Controller Device. Compatible models: VSC, DS4


////////////////////////////////////////////////////////////////
// ISteamMatchmaking
// https://partner.steamgames.com/doc/api/ISteamMatchmaking
////////////////////////////////////////////////////////////////

// EChatMemberStateChange
//--------------------------------------------------------------
// Flags describing how a users lobby state has changed. This is provided from LobbyChatUpdate_t.
#constant EChatMemberStateChangeEntered			0x0001	// This user has joined or is joining the lobby.
#constant EChatMemberStateChangeLeft			0x0002	// This user has left or is leaving the lobby.
#constant EChatMemberStateChangeDisconnected	0x0004	// User disconnected without leaving the lobby first.
#constant EChatMemberStateChangeKicked			0x0008	// The user has been kicked.
#constant EChatMemberStateChangeBanned			0x0010	// The user has been kicked and banned.

// ELobbyComparison
//--------------------------------------------------------------
// Lobby search filter options. These can be set with AddRequestLobbyListStringFilter and AddRequestLobbyListNearValueFilter.
#constant ELobbyComparisonEqualToOrLessThan		-2	// The lobbies value must be equal to or less than this one.
#constant ELobbyComparisonLessThan				-1	// The lobbies value must be less than this one.
#constant ELobbyComparisonEqual					0	// The lobbies value must match this this one exactly.
#constant ELobbyComparisonGreaterThan			1	// The lobbies value must be greater than this one.
#constant ELobbyComparisonEqualToOrGreaterThan	2	// The lobbies value must be equal to or greater than this one.
#constant ELobbyComparisonNotEqual				3	// The lobbies value must not match this this.

// ELobbyDistanceFilter
//--------------------------------------------------------------
// Lobby search distance filters when requesting the lobby list. Lobby results are sorted from closest to farthest. This can be set with AddRequestLobbyListDistanceFilter.
#constant ELobbyDistanceFilterClose			0	// Only lobbies in the same immediate region will be returned.
#constant ELobbyDistanceFilterDefault		1	// Only lobbies in the same region or nearby regions will be returned.
#constant ELobbyDistanceFilterFar			2	// For games that don't have many latency requirements, will return lobbies about half-way around the globe.
#constant ELobbyDistanceFilterWorldwide		3	// No filtering, will match lobbies as far as India to NY (not recommended, expect multiple seconds of latency between the clients).

// ELobbyType
//--------------------------------------------------------------
// Specifies the lobby type, this is set from CreateLobby and SetLobbyType.
#constant ELobbyTypePrivate			0	// The only way to join the lobby is from an invite.
#constant ELobbyTypeFriendsOnly		1	// Joinable by friends and invitees, but does not show up in the lobby list.
#constant ELobbyTypePublic			2	// Returned by search and visible to friends.
#constant ELobbyTypeInvisible		3	// Returned by search, but not visible to other friends.
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
Type LobbyGameCreated_t
	hLobby as integer
	IP as string
	Port as integer
	hGameServer as integer
EndType

// Constants
//--------------------------------------------------------------
//~ #constant HSERVERQUERY_INVALID						0xffffffff	
//~ #constant nMaxLobbyKeyLength						255		// Maximum number of characters a lobby metadata key can be.
#constant FavoriteFlagFavorite					0x01	// This favorite game server entry is for the favorites list.
#constant FavoriteFlagHistory					0x02	// This favorite game server entry is for the history list.
#constant FavoriteFlagNone						0x00	// This favorite game server has no flags set.
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
#constant ERemoteStoragePlatformNone		0x0	// This file will not be downloaded on any platform.
#constant ERemoteStoragePlatformWindows		0x1	// This file will download on Windows.
#constant ERemoteStoragePlatformOSX			0x2	// This file will download on macOS.
#constant ERemoteStoragePlatformPS3			0x4	// This file will download on the Playstation 3.
#constant ERemoteStoragePlatformLinux		0x8	// This file will download on SteamOS/Linux.
#constant ERemoteStoragePlatformReserved2	0x16	// Reserved.
#constant ERemoteStoragePlatformAll 		0xffffffff	// This file will download on every platform. This is the default.


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
#constant ELeaderboardDataRequestGlobal				0	// Used to query for a sequential range of leaderboard entries by leaderboard rank. The start 
														// and end parameters control the requested range.
														// For example, you can display the top 10 on a leaderboard for your game by setting start to 1 and end to 10.
#constant ELeaderboardDataRequestGlobalAroundUser	1	// Used to retrieve leaderboard entries relative a user's entry.
														// The start parameter is the number of entries to retrieve before the current user's entry, and the end 
														// parameter is the number of entries after the current user's entry. The current user's entry is always included. 
														// For example, if the current user is #5 on the leaderboard, setting start to -2 and end to 2 will 
														// return 5 entries: ranks #3 through #7. If there are not enough entries in the leaderboard before 
														// or after the user's entry, Steam will adjust the range to try to return the number of entries requested. 
														// For example, if the user is #1 on the leaderboard and start is set to -2, end is set to 2, 
														// Steam will return the first 5 entries in the leaderboard.
#constant ELeaderboardDataRequestFriends			2	// Used to retrieve all leaderboard entries for friends of the current user. The start and end parameters are ignored.
//~ #constant ELeaderboardDataRequestUsers			3	// Used internally, do not use with DownloadLeaderboardEntries! Doing so is undefined behavior.

// ELeaderboardDisplayType
//--------------------------------------------------------------
// The display type used by the Steam Community web site to know how to format the leaderboard scores when displayed. You can set the display type when 
// creating a leaderboard with FindOrCreateLeaderboard or in the Steamworks partner backend. You can retrieve the display type for a given leaderboard 
// with GetLeaderboardDisplayType.
//~ #constant ELeaderboardDisplayTypeNone				0	// This is only ever used when a leaderboard is invalid, you should never set this yourself.
#constant ELeaderboardDisplayTypeNumeric			1	// The score is just a simple numerical value.
#constant ELeaderboardDisplayTypeTimeSeconds		2	// The score represents a time, in seconds.
#constant ELeaderboardDisplayTypeTimeMilliSeconds	3	// The score represents a time, in milliseconds.

// ELeaderboardSortMethod
//--------------------------------------------------------------
// The sort method used to set whether a higher or lower score is better. You can set the sort method when creating a leaderboard with FindOrCreateLeaderboard 
// or in App Admin on the Steamworks website. You can retrieve the sort method for a given leaderboard with GetLeaderboardSortMethod.
//~ #constant ELeaderboardSortMethodNone			0	// Only ever used when a leaderboard is invalid, you should never set this yourself.
#constant ELeaderboardSortMethodAscending	1	// The top-score is the lowest number.
#constant ELeaderboardSortMethodDescending	2	// The top-score is the highest number.

// ELeaderboardUploadScoreMethod
//--------------------------------------------------------------
//~ #constant ELeaderboardUploadScoreMethodNone			0	
#constant ELeaderboardUploadScoreMethodKeepBest		1	// Leaderboard will keep user's best score
#constant ELeaderboardUploadScoreMethodForceUpdate	2	// Leaderboard will always replace score with specified

// Constants
//--------------------------------------------------------------
//~ #constant cchLeaderboardNameMax				128	// Maximum number of bytes for a leaderboard name (UTF-8 encoded).
//~ #constant cchStatNameMax					128	// Maximum number of bytes for stat and achievement names (UTF-8 encoded).
//~ #constant cLeaderboardDetailsMax			64	// Maximum number of details that you can store for a single leaderboard entry.
//~ #constant STEAMUSERSTATS_INTERFACE_VERSION	"STEAMUSERSTATS_INTERFACE_VERSION011"	

////////////////////////////////////////////////////////////////
// ISteamUtils
// https://partner.steamgames.com/doc/api/ISteamUtils
////////////////////////////////////////////////////////////////

// EGamepadTextInputLineMode
//--------------------------------------------------------------
// Controls number of allowed lines for the Big Picture gamepad text entry
#constant EGamepadTextInputLineModeSingleLine		0
#constant EGamepadTextInputLineModeMultipleLines	1

// EGamepadTextInputMode
//--------------------------------------------------------------
// Input modes for the Big Picture gamepad text entry
#constant EGamepadTextInputModeNormal		0
#constant EGamepadTextInputModePassword		1

// Constants
//--------------------------------------------------------------
//~ #constant STEAMUTILS_INTERFACE_VERSION	"SteamUtils009"
