std::string ToJSON(DlcInstalled_t value)
{
    return std::string("{"
        "\"AppID\": " + std::to_string(value.m_nAppID) + "}");
}

std::string ToJSON(FileDetailsResult_t value)
{
    return std::string("{"
        "\"Result\": " + std::to_string(value.m_eResult) + ", "
		"\"FileSize\": " + std::to_string(value.m_ulFileSize) + ", "
		"\"FileSHA\": " + do_something(value.m_FileSHA) + ", "
		"\"Flags\": " + std::to_string(value.m_unFlags) + "}");
}

std::string ToJSON(AvatarImageLoaded_t value)
{
    return std::string("{"
        "\"SteamID\": " + std::to_string(GetPluginHandle(value.m_steamID)) + ", "
		"\"Image\": " + std::to_string(value.m_iImage) + ", "
		"\"Wide\": " + std::to_string(value.m_iWide) + ", "
		"\"Tall\": " + std::to_string(value.m_iTall) + "}");
}

std::string ToJSON(ClanOfficerListResponse_t value)
{
    return std::string("{"
        "\"SteamIDClan\": " + std::to_string(GetPluginHandle(value.m_steamIDClan)) + ", "
		"\"Officers\": " + std::to_string(value.m_cOfficers) + ", "
		"\"Success\": " + std::to_string(value.m_bSuccess) + "}");
}

std::string ToJSON(DownloadClanActivityCountsResult_t value)
{
    return std::string("{"
        "\"Success\": " + std::to_string(value.m_bSuccess) + "}");
}

std::string ToJSON(FriendRichPresenceUpdate_t value)
{
    return std::string("{"
        "\"SteamIDFriend\": " + std::to_string(GetPluginHandle(value.m_steamIDFriend)) + ", "
		"\"AppID\": " + std::to_string(value.m_nAppID) + "}");
}

std::string ToJSON(FriendsEnumerateFollowingList_t value)
{
    return std::string("{"
        "\"Result\": " + std::to_string(value.m_eResult) + ", "
		"\"SteamID\": " + do_something(value.m_rgSteamID) + ", "
		"\"ResultsReturned\": " + std::to_string(value.m_nResultsReturned) + ", "
		"\"TotalResultCount\": " + std::to_string(value.m_nTotalResultCount) + "}");
}

std::string ToJSON(FriendsGetFollowerCount_t value)
{
    return std::string("{"
        "\"Result\": " + std::to_string(value.m_eResult) + ", "
		"\"SteamID\": " + std::to_string(GetPluginHandle(value.m_steamID)) + ", "
		"\"Count\": " + std::to_string(value.m_nCount) + "}");
}

std::string ToJSON(FriendsIsFollowing_t value)
{
    return std::string("{"
        "\"Result\": " + std::to_string(value.m_eResult) + ", "
		"\"SteamID\": " + std::to_string(GetPluginHandle(value.m_steamID)) + ", "
		"\"IsFollowing\": " + std::to_string(value.m_bIsFollowing) + "}");
}

std::string ToJSON(GameConnectedChatJoin_t value)
{
    return std::string("{"
        "\"SteamIDClanChat\": " + std::to_string(GetPluginHandle(value.m_steamIDClanChat)) + ", "
		"\"SteamIDUser\": " + std::to_string(GetPluginHandle(value.m_steamIDUser)) + "}");
}

std::string ToJSON(GameConnectedChatLeave_t value)
{
    return std::string("{"
        "\"SteamIDClanChat\": " + std::to_string(GetPluginHandle(value.m_steamIDClanChat)) + ", "
		"\"SteamIDUser\": " + std::to_string(GetPluginHandle(value.m_steamIDUser)) + ", "
		"\"Kicked\": " + std::to_string(value.m_bKicked) + ", "
		"\"Dropped\": " + std::to_string(value.m_bDropped) + "}");
}

std::string ToJSON(GameConnectedClanChatMsg_t value)
{
    return std::string("{"
        "\"SteamIDClanChat\": " + std::to_string(GetPluginHandle(value.m_steamIDClanChat)) + ", "
		"\"SteamIDUser\": " + std::to_string(GetPluginHandle(value.m_steamIDUser)) + ", "
		"\"MessageID\": " + std::to_string(value.m_iMessageID) + "}");
}

std::string ToJSON(GameConnectedFriendChatMsg_t value)
{
    return std::string("{"
        "\"SteamIDUser\": " + std::to_string(GetPluginHandle(value.m_steamIDUser)) + ", "
		"\"MessageID\": " + std::to_string(value.m_iMessageID) + "}");
}

std::string ToJSON(GameLobbyJoinRequested_t value)
{
    return std::string("{"
        "\"SteamIDLobby\": " + std::to_string(GetPluginHandle(value.m_steamIDLobby)) + ", "
		"\"SteamIDFriend\": " + std::to_string(GetPluginHandle(value.m_steamIDFriend)) + "}");
}

std::string ToJSON(GameOverlayActivated_t value)
{
    return std::string("{"
        "\"Active\": " + std::to_string(value.m_bActive) + "}");
}

std::string ToJSON(GameRichPresenceJoinRequested_t value)
{
    return std::string("{"
        "\"SteamIDFriend\": " + std::to_string(GetPluginHandle(value.m_steamIDFriend)) + ", "
		"\"Connect\": " + std::string("\"" + EscapeJSON(value.m_rgchConnect) + "\"") + "}");
}

std::string ToJSON(GameServerChangeRequested_t value)
{
    return std::string("{"
        "\"Server\": " + std::string("\"" + EscapeJSON(value.m_rgchServer) + "\"") + ", "
		"\"Password\": " + std::string("\"" + EscapeJSON(value.m_rgchPassword) + "\"") + "}");
}

std::string ToJSON(JoinClanChatRoomCompletionResult_t value)
{
    return std::string("{"
        "\"SteamIDClanChat\": " + std::to_string(GetPluginHandle(value.m_steamIDClanChat)) + ", "
		"\"ChatRoomEnterResponse\": " + std::to_string(value.m_eChatRoomEnterResponse) + "}");
}

std::string ToJSON(PersonaStateChange_t value)
{
    return std::string("{"
        "\"SteamID\": " + std::to_string(value.m_ulSteamID) + ", "
		"\"ChangeFlags\": " + std::to_string(value.m_nChangeFlags) + "}");
}

std::string ToJSON(SetPersonaNameResponse_t value)
{
    return std::string("{"
        "\"Success\": " + std::to_string(value.m_bSuccess) + ", "
		"\"LocalSuccess\": " + std::to_string(value.m_bLocalSuccess) + ", "
		"\"Result\": " + std::to_string(value.m_result) + "}");
}

std::string ToJSON(FriendGameInfo_t value)
{
    return std::string("{"
        "\"GameID\": " + std::to_string(value.m_gameID) + ", "
		"\"GameIP\": " + std::string("\"" + EscapeJSON(value.m_unGameIP) + "\"") + ", "
		"\"GamePort\": " + std::to_string(value.m_usGamePort) + ", "
		"\"QueryPort\": " + std::to_string(value.m_usQueryPort) + ", "
		"\"SteamIDLobby\": " + std::to_string(GetPluginHandle(value.m_steamIDLobby)) + "}");
}

std::string ToJSON(FriendSessionStateInfo_t value)
{
    return std::string("{"
        "\"OnlineSessionInstances\": " + std::to_string(value.m_uiOnlineSessionInstances) + ", "
		"\"PublishedToFriendsSessionInstance\": " + std::to_string(value.m_uiPublishedToFriendsSessionInstance) + "}");
}

std::string ToJSON(AssociateWithClanResult_t value)
{
    return std::string("{"
        "\"Result\": " + std::to_string(value.m_eResult) + "}");
}

std::string ToJSON(ComputeNewPlayerCompatibilityResult_t value)
{
    return std::string("{"
        "\"Result\": " + std::to_string(value.m_eResult) + ", "
		"\"PlayersThatDontLikeCandidate\": " + std::to_string(value.m_cPlayersThatDontLikeCandidate) + ", "
		"\"PlayersThatCandidateDoesntLike\": " + std::to_string(value.m_cPlayersThatCandidateDoesntLike) + ", "
		"\"ClanPlayersThatDontLikeCandidate\": " + std::to_string(value.m_cClanPlayersThatDontLikeCandidate) + ", "
		"\"SteamIDCandidate\": " + std::to_string(GetPluginHandle(value.m_SteamIDCandidate)) + "}");
}

std::string ToJSON(GSClientApprove_t value)
{
    return std::string("{"
        "\"SteamID\": " + std::to_string(GetPluginHandle(value.m_SteamID)) + ", "
		"\"OwnerSteamID\": " + std::to_string(GetPluginHandle(value.m_OwnerSteamID)) + "}");
}

std::string ToJSON(GSClientDeny_t value)
{
    return std::string("{"
        "\"SteamID\": " + std::to_string(GetPluginHandle(value.m_SteamID)) + ", "
		"\"DenyReason\": " + std::to_string(value.m_eDenyReason) + ", "
		"\"OptionalText\": " + std::string("\"" + EscapeJSON(value.m_rgchOptionalText) + "\"") + "}");
}

std::string ToJSON(GSClientGroupStatus_t value)
{
    return std::string("{"
        "\"SteamIDUser\": " + std::to_string(GetPluginHandle(value.m_SteamIDUser)) + ", "
		"\"SteamIDGroup\": " + std::to_string(GetPluginHandle(value.m_SteamIDGroup)) + ", "
		"\"Member\": " + std::to_string(value.m_bMember) + ", "
		"\"Officer\": " + std::to_string(value.m_bOfficer) + "}");
}

std::string ToJSON(GSClientKick_t value)
{
    return std::string("{"
        "\"SteamID\": " + std::to_string(GetPluginHandle(value.m_SteamID)) + ", "
		"\"DenyReason\": " + std::to_string(value.m_eDenyReason) + "}");
}

std::string ToJSON(GSPolicyResponse_t value)
{
    return std::string("{"
        "\"Secure\": " + std::to_string(value.m_bSecure) + "}");
}

std::string ToJSON(GSStatsReceived_t value)
{
    return std::string("{"
        "\"Result\": " + std::to_string(value.m_eResult) + ", "
		"\"SteamIDUser\": " + std::to_string(GetPluginHandle(value.m_steamIDUser)) + "}");
}

std::string ToJSON(GSStatsStored_t value)
{
    return std::string("{"
        "\"Result\": " + std::to_string(value.m_eResult) + ", "
		"\"SteamIDUser\": " + std::to_string(GetPluginHandle(value.m_steamIDUser)) + "}");
}

std::string ToJSON(GSStatsUnloaded_t value)
{
    return std::string("{"
        "\"SteamIDUser\": " + std::to_string(GetPluginHandle(value.m_steamIDUser)) + "}");
}

std::string ToJSON(HTML_BrowserReady_t value)
{
    return std::string("{"
        "\"BrowserHandle\": " + std::to_string(value.unBrowserHandle) + "}");
}

std::string ToJSON(HTML_CanGoBackAndForward_t value)
{
    return std::string("{"
        "\"BrowserHandle\": " + std::to_string(value.unBrowserHandle) + ", "
		"\"CanGoBack\": " + std::to_string(value.bCanGoBack) + ", "
		"\"CanGoForward\": " + std::to_string(value.bCanGoForward) + "}");
}

std::string ToJSON(HTML_ChangedTitle_t value)
{
    return std::string("{"
        "\"BrowserHandle\": " + std::to_string(value.unBrowserHandle) + ", "
		"\"Title\": " + std::string("\"" + EscapeJSON(value.pchTitle) + "\"") + "}");
}

std::string ToJSON(HTML_CloseBrowser_t value)
{
    return std::string("{"
        "\"BrowserHandle\": " + std::to_string(value.unBrowserHandle) + "}");
}

std::string ToJSON(HTML_FileOpenDialog_t value)
{
    return std::string("{"
        "\"BrowserHandle\": " + std::to_string(value.unBrowserHandle) + ", "
		"\"Title\": " + std::string("\"" + EscapeJSON(value.pchTitle) + "\"") + ", "
		"\"InitialFile\": " + std::string("\"" + EscapeJSON(value.pchInitialFile) + "\"") + "}");
}

std::string ToJSON(HTML_FinishedRequest_t value)
{
    return std::string("{"
        "\"BrowserHandle\": " + std::to_string(value.unBrowserHandle) + ", "
		"\"URL\": " + std::string("\"" + EscapeJSON(value.pchURL) + "\"") + ", "
		"\"PageTitle\": " + std::string("\"" + EscapeJSON(value.pchPageTitle) + "\"") + "}");
}

std::string ToJSON(HTML_HideToolTip_t value)
{
    return std::string("{"
        "\"BrowserHandle\": " + std::to_string(value.unBrowserHandle) + "}");
}

std::string ToJSON(HTML_HorizontalScroll_t value)
{
    return std::string("{"
        "\"BrowserHandle\": " + std::to_string(value.unBrowserHandle) + ", "
		"\"ScrollMax\": " + std::to_string(value.unScrollMax) + ", "
		"\"ScrollCurrent\": " + std::to_string(value.unScrollCurrent) + ", "
		"\"PageScale\": " + std::to_string(value.flPageScale) + ", "
		"\"Visible\": " + std::to_string(value.bVisible) + ", "
		"\"PageSize\": " + std::to_string(value.unPageSize) + "}");
}

std::string ToJSON(HTML_JSAlert_t value)
{
    return std::string("{"
        "\"BrowserHandle\": " + std::to_string(value.unBrowserHandle) + ", "
		"\"Message\": " + std::string("\"" + EscapeJSON(value.pchMessage) + "\"") + "}");
}

std::string ToJSON(HTML_JSConfirm_t value)
{
    return std::string("{"
        "\"BrowserHandle\": " + std::to_string(value.unBrowserHandle) + ", "
		"\"Message\": " + std::string("\"" + EscapeJSON(value.pchMessage) + "\"") + "}");
}

std::string ToJSON(HTML_LinkAtPosition_t value)
{
    return std::string("{"
        "\"BrowserHandle\": " + std::to_string(value.unBrowserHandle) + ", "
		"\"X\": " + std::to_string(value.x) + ", "
		"\"Y\": " + std::to_string(value.y) + ", "
		"\"URL\": " + std::string("\"" + EscapeJSON(value.pchURL) + "\"") + ", "
		"\"Input\": " + std::to_string(value.bInput) + ", "
		"\"LiveLink\": " + std::to_string(value.bLiveLink) + "}");
}

std::string ToJSON(HTML_NeedsPaint_t value)
{
    return std::string("{"
        "\"BrowserHandle\": " + std::to_string(value.unBrowserHandle) + ", "
		"\"BGRA\": " + std::string("\"" + EscapeJSON(value.pBGRA) + "\"") + ", "
		"\"Wide\": " + std::to_string(value.unWide) + ", "
		"\"Tall\": " + std::to_string(value.unTall) + ", "
		"\"UpdateX\": " + std::to_string(value.unUpdateX) + ", "
		"\"UpdateY\": " + std::to_string(value.unUpdateY) + ", "
		"\"UpdateWide\": " + std::to_string(value.unUpdateWide) + ", "
		"\"UpdateTall\": " + std::to_string(value.unUpdateTall) + ", "
		"\"ScrollX\": " + std::to_string(value.unScrollX) + ", "
		"\"ScrollY\": " + std::to_string(value.unScrollY) + ", "
		"\"PageScale\": " + std::to_string(value.flPageScale) + ", "
		"\"PageSerial\": " + std::to_string(value.unPageSerial) + "}");
}

std::string ToJSON(HTML_NewWindow_t value)
{
    return std::string("{"
        "\"BrowserHandle\": " + std::to_string(value.unBrowserHandle) + ", "
		"\"URL\": " + std::string("\"" + EscapeJSON(value.pchURL) + "\"") + ", "
		"\"X\": " + std::to_string(value.unX) + ", "
		"\"Y\": " + std::to_string(value.unY) + ", "
		"\"Wide\": " + std::to_string(value.unWide) + ", "
		"\"Tall\": " + std::to_string(value.unTall) + "}");
}

std::string ToJSON(HTML_OpenLinkInNewTab_t value)
{
    return std::string("{"
        "\"BrowserHandle\": " + std::to_string(value.unBrowserHandle) + ", "
		"\"URL\": " + std::string("\"" + EscapeJSON(value.pchURL) + "\"") + "}");
}

std::string ToJSON(HTML_SearchResults_t value)
{
    return std::string("{"
        "\"BrowserHandle\": " + std::to_string(value.unBrowserHandle) + ", "
		"\"Results\": " + std::to_string(value.unResults) + ", "
		"\"CurrentMatch\": " + std::to_string(value.unCurrentMatch) + "}");
}

std::string ToJSON(HTML_SetCursor_t value)
{
    return std::string("{"
        "\"BrowserHandle\": " + std::to_string(value.unBrowserHandle) + ", "
		"\"MouseCursor\": " + std::to_string(value.eMouseCursor) + "}");
}

std::string ToJSON(HTML_ShowToolTip_t value)
{
    return std::string("{"
        "\"BrowserHandle\": " + std::to_string(value.unBrowserHandle) + ", "
		"\"Msg\": " + std::string("\"" + EscapeJSON(value.pchMsg) + "\"") + "}");
}

std::string ToJSON(HTML_StartRequest_t value)
{
    return std::string("{"
        "\"BrowserHandle\": " + std::to_string(value.unBrowserHandle) + ", "
		"\"URL\": " + std::string("\"" + EscapeJSON(value.pchURL) + "\"") + ", "
		"\"Target\": " + std::string("\"" + EscapeJSON(value.pchTarget) + "\"") + ", "
		"\"PostData\": " + std::string("\"" + EscapeJSON(value.pchPostData) + "\"") + ", "
		"\"IsRedirect\": " + std::to_string(value.bIsRedirect) + "}");
}

std::string ToJSON(HTML_StatusText_t value)
{
    return std::string("{"
        "\"BrowserHandle\": " + std::to_string(value.unBrowserHandle) + ", "
		"\"Msg\": " + std::string("\"" + EscapeJSON(value.pchMsg) + "\"") + "}");
}

std::string ToJSON(HTML_UpdateToolTip_t value)
{
    return std::string("{"
        "\"BrowserHandle\": " + std::to_string(value.unBrowserHandle) + ", "
		"\"Msg\": " + std::string("\"" + EscapeJSON(value.pchMsg) + "\"") + "}");
}

std::string ToJSON(HTML_URLChanged_t value)
{
    return std::string("{"
        "\"BrowserHandle\": " + std::to_string(value.unBrowserHandle) + ", "
		"\"URL\": " + std::string("\"" + EscapeJSON(value.pchURL) + "\"") + ", "
		"\"PostData\": " + std::string("\"" + EscapeJSON(value.pchPostData) + "\"") + ", "
		"\"IsRedirect\": " + std::to_string(value.bIsRedirect) + ", "
		"\"PageTitle\": " + std::string("\"" + EscapeJSON(value.pchPageTitle) + "\"") + ", "
		"\"NewNavigation\": " + std::to_string(value.bNewNavigation) + "}");
}

std::string ToJSON(HTML_VerticalScroll_t value)
{
    return std::string("{"
        "\"BrowserHandle\": " + std::to_string(value.unBrowserHandle) + ", "
		"\"ScrollMax\": " + std::to_string(value.unScrollMax) + ", "
		"\"ScrollCurrent\": " + std::to_string(value.unScrollCurrent) + ", "
		"\"PageScale\": " + std::to_string(value.flPageScale) + ", "
		"\"Visible\": " + std::to_string(value.bVisible) + ", "
		"\"PageSize\": " + std::to_string(value.unPageSize) + "}");
}

std::string ToJSON(HTTPRequestCompleted_t value)
{
    return std::string("{"
        "\"Request\": " + std::to_string(value.m_hRequest) + ", "
		"\"ContextValue\": " + std::to_string(value.m_ulContextValue) + ", "
		"\"RequestSuccessful\": " + std::to_string(value.m_bRequestSuccessful) + ", "
		"\"StatusCode\": " + std::to_string(value.m_eStatusCode) + ", "
		"\"BodySize\": " + std::to_string(value.m_unBodySize) + "}");
}

std::string ToJSON(HTTPRequestDataReceived_t value)
{
    return std::string("{"
        "\"Request\": " + std::to_string(value.m_hRequest) + ", "
		"\"ContextValue\": " + std::to_string(value.m_ulContextValue) + ", "
		"\"Offset\": " + std::to_string(value.m_cOffset) + ", "
		"\"BytesReceived\": " + std::to_string(value.m_cBytesReceived) + "}");
}

std::string ToJSON(HTTPRequestHeadersReceived_t value)
{
    return std::string("{"
        "\"Request\": " + std::to_string(value.m_hRequest) + ", "
		"\"ContextValue\": " + std::to_string(value.m_ulContextValue) + "}");
}

std::string ToJSON(SteamInventoryEligiblePromoItemDefIDs_t value)
{
    return std::string("{"
        "\"Result\": " + std::to_string(value.m_result) + ", "
		"\"SteamID\": " + std::to_string(GetPluginHandle(value.m_steamID)) + ", "
		"\"NumEligiblePromoItemDefs\": " + std::to_string(value.m_numEligiblePromoItemDefs) + ", "
		"\"CachedData\": " + std::to_string(value.m_bCachedData) + "}");
}

std::string ToJSON(SteamInventoryFullUpdate_t value)
{
    return std::string("{"
        "\"Handle\": " + std::to_string(value.m_handle) + "}");
}

std::string ToJSON(SteamInventoryResultReady_t value)
{
    return std::string("{"
        "\"Handle\": " + std::to_string(value.m_handle) + ", "
		"\"Result\": " + std::to_string(value.m_result) + "}");
}

std::string ToJSON(SteamInventoryStartPurchaseResult_t value)
{
    return std::string("{"
        "\"Result\": " + std::to_string(value.m_result) + ", "
		"\"OrderID\": " + std::to_string(value.m_ulOrderID) + ", "
		"\"TransID\": " + std::to_string(value.m_ulTransID) + "}");
}

std::string ToJSON(SteamInventoryRequestPricesResult_t value)
{
    return std::string("{"
        "\"Result\": " + std::to_string(value.m_result) + ", "
		"\"Currency\": " + std::string("\"" + EscapeJSON(value.m_rgchCurrency) + "\"") + "}");
}

std::string ToJSON(SteamItemDetails_t value)
{
    return std::string("{"
        "\"ItemId\": " + std::to_string(value.m_itemId) + ", "
		"\"Definition\": " + std::to_string(value.m_iDefinition) + ", "
		"\"Quantity\": " + std::to_string(value.m_unQuantity) + ", "
		"\"Flags\": " + std::to_string(value.m_unFlags) + "}");
}

std::string ToJSON(FavoritesListAccountsUpdated_t value)
{
    return std::string("{"
        "\"Result\": " + std::to_string(value.m_eResult) + "}");
}

std::string ToJSON(FavoritesListChanged_t value)
{
    return std::string("{"
        "\"IP\": " + std::string("\"" + EscapeJSON(value.m_nIP) + "\"") + ", "
		"\"QueryPort\": " + std::to_string(value.m_nQueryPort) + ", "
		"\"ConnPort\": " + std::to_string(value.m_nConnPort) + ", "
		"\"AppID\": " + std::to_string(value.m_nAppID) + ", "
		"\"Flags\": " + std::to_string(value.m_nFlags) + ", "
		"\"Add\": " + std::to_string(value.m_bAdd) + ", "
		"\"AccountId\": " + std::to_string(value.m_unAccountId) + "}");
}

std::string ToJSON(LobbyChatMsg_t value)
{
    return std::string("{"
        "\"SteamIDLobby\": " + std::to_string(value.m_ulSteamIDLobby) + ", "
		"\"SteamIDUser\": " + std::to_string(value.m_ulSteamIDUser) + ", "
		"\"ChatEntryType\": " + std::to_string(value.m_eChatEntryType) + ", "
		"\"ChatID\": " + std::to_string(value.m_iChatID) + "}");
}

std::string ToJSON(LobbyChatUpdate_t value)
{
    return std::string("{"
        "\"SteamIDLobby\": " + std::to_string(value.m_ulSteamIDLobby) + ", "
		"\"SteamIDUserChanged\": " + std::to_string(value.m_ulSteamIDUserChanged) + ", "
		"\"SteamIDMakingChange\": " + std::to_string(value.m_ulSteamIDMakingChange) + ", "
		"\"ChatMemberStateChange\": " + std::to_string(value.m_rgfChatMemberStateChange) + "}");
}

std::string ToJSON(LobbyCreated_t value)
{
    return std::string("{"
        "\"Result\": " + std::to_string(value.m_eResult) + ", "
		"\"SteamIDLobby\": " + std::to_string(value.m_ulSteamIDLobby) + "}");
}

std::string ToJSON(LobbyDataUpdate_t value)
{
    return std::string("{"
        "\"SteamIDLobby\": " + std::to_string(value.m_ulSteamIDLobby) + ", "
		"\"SteamIDMember\": " + std::to_string(value.m_ulSteamIDMember) + ", "
		"\"Success\": " + std::to_string(value.m_bSuccess) + "}");
}

std::string ToJSON(LobbyEnter_t value)
{
    return std::string("{"
        "\"SteamIDLobby\": " + std::to_string(value.m_ulSteamIDLobby) + ", "
		"\"ChatPermissions\": " + std::to_string(value.m_rgfChatPermissions) + ", "
		"\"Locked\": " + std::to_string(value.m_bLocked) + ", "
		"\"ChatRoomEnterResponse\": " + std::to_string(value.m_EChatRoomEnterResponse) + "}");
}

std::string ToJSON(LobbyGameCreated_t value)
{
    return std::string("{"
        "\"SteamIDLobby\": " + std::to_string(value.m_ulSteamIDLobby) + ", "
		"\"SteamIDGameServer\": " + std::to_string(value.m_ulSteamIDGameServer) + ", "
		"\"IP\": " + std::string("\"" + EscapeJSON(value.m_unIP) + "\"") + ", "
		"\"Port\": " + std::to_string(value.m_usPort) + "}");
}

std::string ToJSON(LobbyInvite_t value)
{
    return std::string("{"
        "\"SteamIDUser\": " + std::to_string(value.m_ulSteamIDUser) + ", "
		"\"SteamIDLobby\": " + std::to_string(value.m_ulSteamIDLobby) + ", "
		"\"GameID\": " + std::to_string(value.m_ulGameID) + "}");
}

std::string ToJSON(MatchMakingKeyValuePair_t value)
{
    return std::string("{"
        "\"Key\": " + std::string("\"" + EscapeJSON(value.m_szKey) + "\"") + ", "
		"\"Value\": " + std::string("\"" + EscapeJSON(value.m_szValue) + "\"") + "}");
}

std::string ToJSON(VolumeHasChanged_t value)
{
    return std::string("{"
        "\"NewVolume\": " + std::to_string(value.m_flNewVolume) + "}");
}

std::string ToJSON(MusicPlayerSelectsPlaylistEntry_t value)
{
    return std::string("{"
        "\"ID\": " + std::to_string(value.nID) + "}");
}

std::string ToJSON(MusicPlayerSelectsQueueEntry_t value)
{
    return std::string("{"
        "\"ID\": " + std::to_string(value.nID) + "}");
}

std::string ToJSON(MusicPlayerWantsLooped_t value)
{
    return std::string("{"
        "\"Looped\": " + std::to_string(value.m_bLooped) + "}");
}

std::string ToJSON(MusicPlayerWantsPlayingRepeatStatus_t value)
{
    return std::string("{"
        "\"PlayingRepeatStatus\": " + std::to_string(value.m_nPlayingRepeatStatus) + "}");
}

std::string ToJSON(MusicPlayerWantsShuffled_t value)
{
    return std::string("{"
        "\"Shuffled\": " + std::to_string(value.m_bShuffled) + "}");
}

std::string ToJSON(MusicPlayerWantsVolume_t value)
{
    return std::string("{"
        "\"NewVolume\": " + std::to_string(value.m_flNewVolume) + "}");
}

std::string ToJSON(P2PSessionConnectFail_t value)
{
    return std::string("{"
        "\"SteamIDRemote\": " + std::to_string(GetPluginHandle(value.m_steamIDRemote)) + ", "
		"\"P2PSessionError\": " + std::to_string(value.m_eP2PSessionError) + "}");
}

std::string ToJSON(P2PSessionRequest_t value)
{
    return std::string("{"
        "\"SteamIDRemote\": " + std::to_string(GetPluginHandle(value.m_steamIDRemote)) + "}");
}

std::string ToJSON(SocketStatusCallback_t value)
{
    return std::string("{"
        "\"Socket\": " + std::to_string(value.m_hSocket) + ", "
		"\"ListenSocket\": " + std::to_string(value.m_hListenSocket) + ", "
		"\"SteamIDRemote\": " + std::to_string(GetPluginHandle(value.m_steamIDRemote)) + ", "
		"\"SNetSocketState\": " + std::to_string(value.m_eSNetSocketState) + "}");
}

std::string ToJSON(P2PSessionState_t value)
{
    return std::string("{"
        "\"ConnectionActive\": " + std::to_string(value.m_bConnectionActive) + ", "
		"\"Connecting\": " + std::to_string(value.m_bConnecting) + ", "
		"\"P2PSessionError\": " + std::to_string(value.m_eP2PSessionError) + ", "
		"\"UsingRelay\": " + std::to_string(value.m_bUsingRelay) + ", "
		"\"BytesQueuedForSend\": " + std::to_string(value.m_nBytesQueuedForSend) + ", "
		"\"PacketsQueuedForSend\": " + std::to_string(value.m_nPacketsQueuedForSend) + ", "
		"\"RemoteIP\": " + std::string("\"" + EscapeJSON(value.m_nRemoteIP) + "\"") + ", "
		"\"RemotePort\": " + std::to_string(value.m_nRemotePort) + "}");
}

std::string ToJSON(JoinPartyCallback_t value)
{
    return std::string("{"
        "\"Result\": " + std::to_string(value.m_eResult) + ", "
		"\"BeaconID\": " + std::to_string(value.m_ulBeaconID) + ", "
		"\"SteamIDBeaconOwner\": " + std::to_string(GetPluginHandle(value.m_SteamIDBeaconOwner)) + ", "
		"\"ConnectString\": " + std::string("\"" + EscapeJSON(value.m_rgchConnectString) + "\"") + "}");
}

std::string ToJSON(CreateBeaconCallback_t value)
{
    return std::string("{"
        "\"Result\": " + std::to_string(value.m_eResult) + ", "
		"\"BeaconID\": " + std::to_string(value.m_ulBeaconID) + "}");
}

std::string ToJSON(ReservationNotificationCallback_t value)
{
    return std::string("{"
        "\"BeaconID\": " + std::to_string(value.m_ulBeaconID) + ", "
		"\"SteamIDJoiner\": " + std::to_string(GetPluginHandle(value.m_steamIDJoiner)) + "}");
}

std::string ToJSON(ChangeNumOpenSlotsCallback_t value)
{
    return std::string("{"
        "\"Result\": " + std::to_string(value.m_eResult) + "}");
}

std::string ToJSON(SteamPartyBeaconLocation_t value)
{
    return std::string("{"
        "\"LocationType\": " + std::to_string(value.m_eType) + ", "
		"\"LocationID\": " + std::to_string(value.m_ulLocationID) + "}");
}

std::string ToJSON(RemoteStorageDownloadUGCResult_t value)
{
    return std::string("{"
        "\"Result\": " + std::to_string(value.m_eResult) + ", "
		"\"File\": " + std::to_string(value.m_hFile) + ", "
		"\"AppID\": " + std::to_string(value.m_nAppID) + ", "
		"\"SizeInBytes\": " + std::to_string(value.m_nSizeInBytes) + ", "
		"\"FileName\": " + std::string("\"" + EscapeJSON(value.m_pchFileName) + "\"") + ", "
		"\"SteamIDOwner\": " + std::to_string(value.m_ulSteamIDOwner) + "}");
}

std::string ToJSON(RemoteStorageFileReadAsyncComplete_t value)
{
    return std::string("{"
        "\"FileReadAsync\": " + std::to_string(value.m_hFileReadAsync) + ", "
		"\"Result\": " + std::to_string(value.m_eResult) + ", "
		"\"Offset\": " + std::to_string(value.m_nOffset) + ", "
		"\"Read\": " + std::to_string(value.m_cubRead) + "}");
}

std::string ToJSON(RemoteStorageFileShareResult_t value)
{
    return std::string("{"
        "\"Result\": " + std::to_string(value.m_eResult) + ", "
		"\"File\": " + std::to_string(value.m_hFile) + ", "
		"\"Filename\": " + std::string("\"" + EscapeJSON(value.m_rgchFilename) + "\"") + "}");
}

std::string ToJSON(RemoteStorageFileWriteAsyncComplete_t value)
{
    return std::string("{"
        "\"Result\": " + std::to_string(value.m_eResult) + "}");
}

std::string ToJSON(RemoteStoragePublishedFileSubscribed_t value)
{
    return std::string("{"
        "\"PublishedFileId\": " + std::to_string(value.m_nPublishedFileId) + ", "
		"\"AppID\": " + std::to_string(value.m_nAppID) + "}");
}

std::string ToJSON(RemoteStoragePublishedFileUnsubscribed_t value)
{
    return std::string("{"
        "\"PublishedFileId\": " + std::to_string(value.m_nPublishedFileId) + ", "
		"\"AppID\": " + std::to_string(value.m_nAppID) + "}");
}

std::string ToJSON(RemoteStorageSubscribePublishedFileResult_t value)
{
    return std::string("{"
        "\"Result\": " + std::to_string(value.m_eResult) + ", "
		"\"PublishedFileId\": " + std::to_string(value.m_nPublishedFileId) + "}");
}

std::string ToJSON(RemoteStorageUnsubscribePublishedFileResult_t value)
{
    return std::string("{"
        "\"Result\": " + std::to_string(value.m_eResult) + ", "
		"\"PublishedFileId\": " + std::to_string(value.m_nPublishedFileId) + "}");
}

std::string ToJSON(ScreenshotReady_t value)
{
    return std::string("{"
        "\"Handle\": " + std::to_string(value.m_hLocal) + ", "
		"\"Result\": " + std::to_string(value.m_eResult) + "}");
}

std::string ToJSON(AddAppDependencyResult_t value)
{
    return std::string("{"
        "\"Result\": " + std::to_string(value.m_eResult) + ", "
		"\"PublishedFileId\": " + std::to_string(value.m_nPublishedFileId) + ", "
		"\"AppID\": " + std::to_string(value.m_nAppID) + "}");
}

std::string ToJSON(AddUGCDependencyResult_t value)
{
    return std::string("{"
        "\"Result\": " + std::to_string(value.m_eResult) + ", "
		"\"PublishedFileId\": " + std::to_string(value.m_nPublishedFileId) + ", "
		"\"ChildPublishedFileId\": " + std::to_string(value.m_nChildPublishedFileId) + "}");
}

std::string ToJSON(CreateItemResult_t value)
{
    return std::string("{"
        "\"Result\": " + std::to_string(value.m_eResult) + ", "
		"\"PublishedFileId\": " + std::to_string(value.m_nPublishedFileId) + ", "
		"\"UserNeedsToAcceptWorkshopLegalAgreement\": " + std::to_string(value.m_bUserNeedsToAcceptWorkshopLegalAgreement) + "}");
}

std::string ToJSON(DownloadItemResult_t value)
{
    return std::string("{"
        "\"AppID\": " + std::to_string(value.m_unAppID) + ", "
		"\"PublishedFileId\": " + std::to_string(value.m_nPublishedFileId) + ", "
		"\"Result\": " + std::to_string(value.m_eResult) + "}");
}

std::string ToJSON(GetAppDependenciesResult_t value)
{
    return std::string("{"
        "\"Result\": " + std::to_string(value.m_eResult) + ", "
		"\"PublishedFileId\": " + std::to_string(value.m_nPublishedFileId) + ", "
		"\"AppIDs\": " + std::to_string(value.m_rgAppIDs) + ", "
		"\"NumAppDependencies\": " + std::to_string(value.m_nNumAppDependencies) + ", "
		"\"TotalNumAppDependencies\": " + std::to_string(value.m_nTotalNumAppDependencies) + "}");
}

std::string ToJSON(DeleteItemResult_t value)
{
    return std::string("{"
        "\"Result\": " + std::to_string(value.m_eResult) + ", "
		"\"PublishedFileId\": " + std::to_string(value.m_nPublishedFileId) + "}");
}

std::string ToJSON(GetUserItemVoteResult_t value)
{
    return std::string("{"
        "\"PublishedFileId\": " + std::to_string(value.m_nPublishedFileId) + ", "
		"\"Result\": " + std::to_string(value.m_eResult) + ", "
		"\"VotedUp\": " + std::to_string(value.m_bVotedUp) + ", "
		"\"VotedDown\": " + std::to_string(value.m_bVotedDown) + ", "
		"\"VoteSkipped\": " + std::to_string(value.m_bVoteSkipped) + "}");
}

std::string ToJSON(ItemInstalled_t value)
{
    return std::string("{"
        "\"AppID\": " + std::to_string(value.m_unAppID) + ", "
		"\"PublishedFileId\": " + std::to_string(value.m_nPublishedFileId) + "}");
}

std::string ToJSON(RemoveAppDependencyResult_t value)
{
    return std::string("{"
        "\"Result\": " + std::to_string(value.m_eResult) + ", "
		"\"PublishedFileId\": " + std::to_string(value.m_nPublishedFileId) + ", "
		"\"AppID\": " + std::to_string(value.m_nAppID) + "}");
}

std::string ToJSON(RemoveUGCDependencyResult_t value)
{
    return std::string("{"
        "\"Result\": " + std::to_string(value.m_eResult) + ", "
		"\"PublishedFileId\": " + std::to_string(value.m_nPublishedFileId) + ", "
		"\"ChildPublishedFileId\": " + std::to_string(value.m_nChildPublishedFileId) + "}");
}

std::string ToJSON(SetUserItemVoteResult_t value)
{
    return std::string("{"
        "\"PublishedFileId\": " + std::to_string(value.m_nPublishedFileId) + ", "
		"\"Result\": " + std::to_string(value.m_eResult) + ", "
		"\"VoteUp\": " + std::to_string(value.m_bVoteUp) + "}");
}

std::string ToJSON(StartPlaytimeTrackingResult_t value)
{
    return std::string("{"
        "\"Result\": " + std::to_string(value.m_eResult) + "}");
}

std::string ToJSON(SteamUGCQueryCompleted_t value)
{
    return std::string("{"
        "\"Handle\": " + std::to_string(value.m_handle) + ", "
		"\"Result\": " + std::to_string(value.m_eResult) + ", "
		"\"NumResultsReturned\": " + std::to_string(value.m_unNumResultsReturned) + ", "
		"\"TotalMatchingResults\": " + std::to_string(value.m_unTotalMatchingResults) + ", "
		"\"CachedData\": " + std::to_string(value.m_bCachedData) + "}");
}

std::string ToJSON(StopPlaytimeTrackingResult_t value)
{
    return std::string("{"
        "\"Result\": " + std::to_string(value.m_eResult) + "}");
}

std::string ToJSON(SubmitItemUpdateResult_t value)
{
    return std::string("{"
        "\"Result\": " + std::to_string(value.m_eResult) + ", "
		"\"UserNeedsToAcceptWorkshopLegalAgreement\": " + std::to_string(value.m_bUserNeedsToAcceptWorkshopLegalAgreement) + "}");
}

std::string ToJSON(UserFavoriteItemsListChanged_t value)
{
    return std::string("{"
        "\"PublishedFileId\": " + std::to_string(value.m_nPublishedFileId) + ", "
		"\"Result\": " + std::to_string(value.m_eResult) + ", "
		"\"WasAddRequest\": " + std::to_string(value.m_bWasAddRequest) + "}");
}

std::string ToJSON(SteamUGCDetails_t value)
{
    return std::string("{"
        "\"PublishedFileId\": " + std::to_string(value.m_nPublishedFileId) + ", "
		"\"Result\": " + std::to_string(value.m_eResult) + ", "
		"\"FileType\": " + std::to_string(value.m_eFileType) + ", "
		"\"CreatorAppID\": " + std::to_string(value.m_nCreatorAppID) + ", "
		"\"ConsumerAppID\": " + std::to_string(value.m_nConsumerAppID) + ", "
		"\"Title\": " + std::string("\"" + EscapeJSON(value.m_rgchTitle) + "\"") + ", "
		"\"Description\": " + std::string("\"" + EscapeJSON(value.m_rgchDescription) + "\"") + ", "
		"\"SteamIDOwner\": " + std::to_string(value.m_ulSteamIDOwner) + ", "
		"\"TimeCreated\": " + std::to_string(value.m_rtimeCreated) + ", "
		"\"TimeUpdated\": " + std::to_string(value.m_rtimeUpdated) + ", "
		"\"TimeAddedToUserList\": " + std::to_string(value.m_rtimeAddedToUserList) + ", "
		"\"Visibility\": " + std::to_string(value.m_eVisibility) + ", "
		"\"Banned\": " + std::to_string(value.m_bBanned) + ", "
		"\"AcceptedForUse\": " + std::to_string(value.m_bAcceptedForUse) + ", "
		"\"TagsTruncated\": " + std::to_string(value.m_bTagsTruncated) + ", "
		"\"Tags\": " + std::string("\"" + EscapeJSON(value.m_rgchTags) + "\"") + ", "
		"\"File\": " + std::to_string(value.m_hFile) + ", "
		"\"PreviewFile\": " + std::to_string(value.m_hPreviewFile) + ", "
		"\"FileName\": " + std::string("\"" + EscapeJSON(value.m_pchFileName) + "\"") + ", "
		"\"FileSize\": " + std::to_string(value.m_nFileSize) + ", "
		"\"PreviewFileSize\": " + std::to_string(value.m_nPreviewFileSize) + ", "
		"\"URL\": " + std::string("\"" + EscapeJSON(value.m_rgchURL) + "\"") + ", "
		"\"VotesUp\": " + std::to_string(value.m_unVotesUp) + ", "
		"\"VotesDown\": " + std::to_string(value.m_unVotesDown) + ", "
		"\"Score\": " + std::to_string(value.m_flScore) + ", "
		"\"NumChildren\": " + std::to_string(value.m_unNumChildren) + "}");
}

std::string ToJSON(ClientGameServerDeny_t value)
{
    return std::string("{"
        "\"AppID\": " + std::to_string(value.m_uAppID) + ", "
		"\"GameServerIP\": " + std::string("\"" + EscapeJSON(value.m_unGameServerIP) + "\"") + ", "
		"\"GameServerPort\": " + std::to_string(value.m_usGameServerPort) + ", "
		"\"Secure\": " + std::to_string(value.m_bSecure) + ", "
		"\"Reason\": " + std::to_string(value.m_uReason) + "}");
}

std::string ToJSON(EncryptedAppTicketResponse_t value)
{
    return std::string("{"
        "\"Result\": " + std::to_string(value.m_eResult) + "}");
}

std::string ToJSON(GameWebCallback_t value)
{
    return std::string("{"
        "\"URL\": " + std::string("\"" + EscapeJSON(value.m_szURL) + "\"") + "}");
}

std::string ToJSON(GetAuthSessionTicketResponse_t value)
{
    return std::string("{"
        "\"AuthTicket\": " + std::to_string(value.m_hAuthTicket) + ", "
		"\"Result\": " + std::to_string(value.m_eResult) + "}");
}

std::string ToJSON(IPCFailure_t value)
{
    return std::string("{"
        "\"FailureType\": " + std::to_string(value.m_eFailureType) + "}");
}

std::string ToJSON(MicroTxnAuthorizationResponse_t value)
{
    return std::string("{"
        "\"AppID\": " + std::to_string(value.m_unAppID) + ", "
		"\"OrderID\": " + std::to_string(value.m_ulOrderID) + ", "
		"\"Authorized\": " + std::to_string(value.m_bAuthorized) + "}");
}

std::string ToJSON(SteamServerConnectFailure_t value)
{
    return std::string("{"
        "\"Result\": " + std::to_string(value.m_eResult) + ", "
		"\"StillRetrying\": " + std::to_string(value.m_bStillRetrying) + "}");
}

std::string ToJSON(SteamServersDisconnected_t value)
{
    return std::string("{"
        "\"Result\": " + std::to_string(value.m_eResult) + "}");
}

std::string ToJSON(StoreAuthURLResponse_t value)
{
    return std::string("{"
        "\"URL\": " + std::string("\"" + EscapeJSON(value.m_szURL) + "\"") + "}");
}

std::string ToJSON(ValidateAuthTicketResponse_t value)
{
    return std::string("{"
        "\"SteamID\": " + std::to_string(GetPluginHandle(value.m_SteamID)) + ", "
		"\"AuthSessionResponse\": " + std::to_string(value.m_eAuthSessionResponse) + ", "
		"\"OwnerSteamID\": " + std::to_string(GetPluginHandle(value.m_OwnerSteamID)) + "}");
}

std::string ToJSON(GlobalAchievementPercentagesReady_t value)
{
    return std::string("{"
        "\"GameID\": " + std::to_string(value.m_nGameID) + ", "
		"\"Result\": " + std::to_string(value.m_eResult) + "}");
}

std::string ToJSON(GlobalStatsReceived_t value)
{
    return std::string("{"
        "\"GameID\": " + std::to_string(value.m_nGameID) + ", "
		"\"Result\": " + std::to_string(value.m_eResult) + "}");
}

std::string ToJSON(LeaderboardFindResult_t value)
{
    return std::string("{"
        "\"SteamLeaderboard\": " + std::to_string(GetPluginHandle(value.m_hSteamLeaderboard)) + ", "
		"\"LeaderboardFound\": " + std::to_string(value.m_bLeaderboardFound) + "}");
}

std::string ToJSON(LeaderboardScoresDownloaded_t value)
{
    return std::string("{"
        "\"SteamLeaderboard\": " + std::to_string(GetPluginHandle(value.m_hSteamLeaderboard)) + ", "
		"\"SteamLeaderboardEntries\": " + std::to_string(value.m_hSteamLeaderboardEntries) + ", "
		"\"EntryCount\": " + std::to_string(value.m_cEntryCount) + "}");
}

std::string ToJSON(LeaderboardScoreUploaded_t value)
{
    return std::string("{"
        "\"Success\": " + std::to_string(value.m_bSuccess) + ", "
		"\"SteamLeaderboard\": " + std::to_string(GetPluginHandle(value.m_hSteamLeaderboard)) + ", "
		"\"Score\": " + std::to_string(value.m_nScore) + ", "
		"\"ScoreChanged\": " + std::to_string(value.m_bScoreChanged) + ", "
		"\"GlobalRankNew\": " + std::to_string(value.m_nGlobalRankNew) + ", "
		"\"GlobalRankPrevious\": " + std::to_string(value.m_nGlobalRankPrevious) + "}");
}

std::string ToJSON(LeaderboardUGCSet_t value)
{
    return std::string("{"
        "\"Result\": " + std::to_string(value.m_eResult) + ", "
		"\"SteamLeaderboard\": " + std::to_string(GetPluginHandle(value.m_hSteamLeaderboard)) + "}");
}

std::string ToJSON(NumberOfCurrentPlayers_t value)
{
    return std::string("{"
        "\"Success\": " + std::to_string(value.m_bSuccess) + ", "
		"\"Players\": " + std::to_string(value.m_cPlayers) + "}");
}

std::string ToJSON(PS3TrophiesInstalled_t value)
{
    return std::string("{"
        "\"GameID\": " + std::to_string(value.m_nGameID) + ", "
		"\"Result\": " + std::to_string(value.m_eResult) + ", "
		"\"RequiredDiskSpace\": " + std::to_string(value.m_ulRequiredDiskSpace) + "}");
}

std::string ToJSON(UserAchievementIconFetched_t value)
{
    return std::string("{"
        "\"GameID\": " + std::to_string(value.m_nGameID) + ", "
		"\"AchievementName\": " + std::string("\"" + EscapeJSON(value.m_rgchAchievementName) + "\"") + ", "
		"\"Achieved\": " + std::to_string(value.m_bAchieved) + ", "
		"\"IconHandle\": " + std::to_string(value.m_nIconHandle) + "}");
}

std::string ToJSON(UserAchievementStored_t value)
{
    return std::string("{"
        "\"GameID\": " + std::to_string(value.m_nGameID) + ", "
		"\"GroupAchievement\": " + std::to_string(value.m_bGroupAchievement) + ", "
		"\"AchievementName\": " + std::string("\"" + EscapeJSON(value.m_rgchAchievementName) + "\"") + ", "
		"\"CurProgress\": " + std::to_string(value.m_nCurProgress) + ", "
		"\"MaxProgress\": " + std::to_string(value.m_nMaxProgress) + "}");
}

std::string ToJSON(UserStatsReceived_t value)
{
    return std::string("{"
        "\"GameID\": " + std::to_string(value.m_nGameID) + ", "
		"\"Result\": " + std::to_string(value.m_eResult) + ", "
		"\"SteamIDUser\": " + std::to_string(GetPluginHandle(value.m_steamIDUser)) + "}");
}

std::string ToJSON(UserStatsStored_t value)
{
    return std::string("{"
        "\"GameID\": " + std::to_string(value.m_nGameID) + ", "
		"\"Result\": " + std::to_string(value.m_eResult) + "}");
}

std::string ToJSON(UserStatsUnloaded_t value)
{
    return std::string("{"
        "\"SteamIDUser\": " + std::to_string(GetPluginHandle(value.m_steamIDUser)) + "}");
}

std::string ToJSON(LeaderboardEntry_t value)
{
    return std::string("{"
        "\"SteamIDUser\": " + std::to_string(GetPluginHandle(value.m_steamIDUser)) + ", "
		"\"GlobalRank\": " + std::to_string(value.m_nGlobalRank) + ", "
		"\"Score\": " + std::to_string(value.m_nScore) + ", "
		"\"Details\": " + std::to_string(value.m_cDetails) + ", "
		"\"UGC\": " + std::to_string(value.m_hUGC) + "}");
}

std::string ToJSON(CheckFileSignature_t value)
{
    return std::string("{"
        "\"CheckFileSignature\": " + std::to_string(value.m_eCheckFileSignature) + "}");
}

std::string ToJSON(GamepadTextInputDismissed_t value)
{
    return std::string("{"
        "\"Submitted\": " + std::to_string(value.m_bSubmitted) + ", "
		"\"SubmittedText\": " + std::to_string(value.m_unSubmittedText) + "}");
}

std::string ToJSON(LowBatteryPower_t value)
{
    return std::string("{"
        "\"MinutesBatteryLeft\": " + std::to_string(value.m_nMinutesBatteryLeft) + "}");
}

std::string ToJSON(SteamAPICallCompleted_t value)
{
    return std::string("{"
        "\"AsyncCall\": " + std::to_string(value.m_hAsyncCall) + ", "
		"\"Callback\": " + std::to_string(value.m_iCallback) + ", "
		"\"Param\": " + std::to_string(value.m_cubParam) + "}");
}

std::string ToJSON(BroadcastUploadStop_t value)
{
    return std::string("{"
        "\"Result\": " + std::to_string(value.m_eResult) + "}");
}

std::string ToJSON(GetOPFSettingsResult_t value)
{
    return std::string("{"
        "\"Result\": " + std::to_string(value.m_eResult) + ", "
		"\"VideoAppID\": " + std::to_string(value.m_unVideoAppID) + "}");
}

std::string ToJSON(GetVideoURLResult_t value)
{
    return std::string("{"
        "\"Result\": " + std::to_string(value.m_eResult) + ", "
		"\"VideoAppID\": " + std::to_string(value.m_unVideoAppID) + ", "
		"\"URL\": " + std::string("\"" + EscapeJSON(value.m_rgchURL) + "\"") + "}");
}

