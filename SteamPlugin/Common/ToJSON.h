/*
Copyright (c) 2019 Adam Biser <adambiser@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#ifndef _TOJSON_H
#define _TOJSON_H
#pragma once

#include <string>
#include "DllMain.h"
#include "steam_api.h"
//#include "steam_gameserver.h"

std::string EscapeJSON(const std::string &input);

std::string ToJSON(CSteamID *pSteamIDs, int count);
std::string ToJSON(uint32 *pValues, int count);
std::string ToJSON(char *key, char *value);

std::string ToJSON(DlcInstalled_t value);
std::string ToJSON(DLCData_t value);
std::string ToJSON(DownloadProgress_t value);
std::string ToJSON(FavoriteGameInfo_t value);
//std::string ToJSON(FileDetailsResult_t value);
std::string ToJSON(AvatarImageLoaded_t value);
std::string ToJSON(ClanOfficerListResponse_t value);
std::string ToJSON(DownloadClanActivityCountsResult_t value);
std::string ToJSON(FriendRichPresenceUpdate_t value);
//std::string ToJSON(FriendsEnumerateFollowingList_t value);
std::string ToJSON(FriendsGetFollowerCount_t value);
std::string ToJSON(FriendsIsFollowing_t value);
std::string ToJSON(GameConnectedChatJoin_t value);
std::string ToJSON(GameConnectedChatLeave_t value);
std::string ToJSON(GameConnectedClanChatMsg_t value);
std::string ToJSON(GameConnectedFriendChatMsg_t value);
std::string ToJSON(GameLobbyJoinRequested_t value);
std::string ToJSON(GameOverlayActivated_t value);
std::string ToJSON(GameRichPresenceJoinRequested_t value);
std::string ToJSON(GameServerChangeRequested_t value);
std::string ToJSON(JoinClanChatRoomCompletionResult_t value);
std::string ToJSON(PersonaStateChange_t value);
std::string ToJSON(SetPersonaNameResponse_t value);
std::string ToJSON(bool ingame, FriendGameInfo_t value);
std::string ToJSON(FriendSessionStateInfo_t value);
#ifdef ISTEAMGAMESERVER_H
std::string ToJSON(AssociateWithClanResult_t value);
std::string ToJSON(ComputeNewPlayerCompatibilityResult_t value);
std::string ToJSON(GSClientApprove_t value);
std::string ToJSON(GSClientDeny_t value);
std::string ToJSON(GSClientGroupStatus_t value);
std::string ToJSON(GSClientKick_t value);
std::string ToJSON(GSPolicyResponse_t value);
std::string ToJSON(GSStatsReceived_t value);
std::string ToJSON(GSStatsStored_t value);
std::string ToJSON(GSStatsUnloaded_t value);
#endif
std::string ToJSON(HTML_BrowserReady_t value);
std::string ToJSON(HTML_CanGoBackAndForward_t value);
std::string ToJSON(HTML_ChangedTitle_t value);
std::string ToJSON(HTML_CloseBrowser_t value);
std::string ToJSON(HTML_FileOpenDialog_t value);
std::string ToJSON(HTML_FinishedRequest_t value);
std::string ToJSON(HTML_HideToolTip_t value);
std::string ToJSON(HTML_HorizontalScroll_t value);
std::string ToJSON(HTML_JSAlert_t value);
std::string ToJSON(HTML_JSConfirm_t value);
std::string ToJSON(HTML_LinkAtPosition_t value);
std::string ToJSON(HTML_NeedsPaint_t value);
std::string ToJSON(HTML_NewWindow_t value);
std::string ToJSON(HTML_OpenLinkInNewTab_t value);
std::string ToJSON(HTML_SearchResults_t value);
std::string ToJSON(HTML_SetCursor_t value);
std::string ToJSON(HTML_ShowToolTip_t value);
std::string ToJSON(HTML_StartRequest_t value);
std::string ToJSON(HTML_StatusText_t value);
std::string ToJSON(HTML_UpdateToolTip_t value);
std::string ToJSON(HTML_URLChanged_t value);
std::string ToJSON(HTML_VerticalScroll_t value);
std::string ToJSON(HTTPRequestCompleted_t value);
std::string ToJSON(HTTPRequestDataReceived_t value);
std::string ToJSON(HTTPRequestHeadersReceived_t value);
std::string ToJSON(SteamInventoryEligiblePromoItemDefIDs_t value);
std::string ToJSON(SteamInventoryFullUpdate_t value);
std::string ToJSON(SteamInventoryResultReady_t value);
std::string ToJSON(SteamInventoryStartPurchaseResult_t value);
std::string ToJSON(SteamInventoryRequestPricesResult_t value);
std::string ToJSON(SteamItemDetails_t value);
std::string ToJSON(FavoritesListAccountsUpdated_t value);
std::string ToJSON(FavoritesListChanged_t value);
std::string ToJSON(LobbyChatMsg_t value);
std::string ToJSON(LobbyChatUpdate_t value);
std::string ToJSON(LobbyCreated_t value);
std::string ToJSON(LobbyDataUpdate_t value);
std::string ToJSON(LobbyEnter_t value);
std::string ToJSON(LobbyGameCreated_t value);
std::string ToJSON(LobbyGameServer_t value);
std::string ToJSON(LobbyInvite_t value);
std::string ToJSON(MatchMakingKeyValuePair_t value);
std::string ToJSON(VolumeHasChanged_t value);
std::string ToJSON(MusicPlayerSelectsPlaylistEntry_t value);
std::string ToJSON(MusicPlayerSelectsQueueEntry_t value);
std::string ToJSON(MusicPlayerWantsLooped_t value);
std::string ToJSON(MusicPlayerWantsPlayingRepeatStatus_t value);
std::string ToJSON(MusicPlayerWantsShuffled_t value);
std::string ToJSON(MusicPlayerWantsVolume_t value);
std::string ToJSON(P2PSessionConnectFail_t value);
std::string ToJSON(P2PSessionRequest_t value);
std::string ToJSON(SocketStatusCallback_t value);
std::string ToJSON(P2PSessionState_t value);
std::string ToJSON(JoinPartyCallback_t value);
std::string ToJSON(CreateBeaconCallback_t value);
std::string ToJSON(ReservationNotificationCallback_t value);
std::string ToJSON(ChangeNumOpenSlotsCallback_t value);
std::string ToJSON(SteamPartyBeaconLocation_t value);
std::string ToJSON(RemoteStorageDownloadUGCResult_t value);
std::string ToJSON(RemoteStorageFileReadAsyncComplete_t value);
std::string ToJSON(RemoteStorageFileShareResult_t value);
std::string ToJSON(RemoteStorageFileWriteAsyncComplete_t value);
std::string ToJSON(RemoteStoragePublishedFileSubscribed_t value);
std::string ToJSON(RemoteStoragePublishedFileUnsubscribed_t value);
std::string ToJSON(RemoteStorageSubscribePublishedFileResult_t value);
std::string ToJSON(RemoteStorageUnsubscribePublishedFileResult_t value);
std::string ToJSON(ScreenshotReady_t value);
std::string ToJSON(AddAppDependencyResult_t value);
std::string ToJSON(AddUGCDependencyResult_t value);
std::string ToJSON(CreateItemResult_t value);
std::string ToJSON(DownloadItemResult_t value);
//std::string ToJSON(GetAppDependenciesResult_t value);
std::string ToJSON(DeleteItemResult_t value);
std::string ToJSON(GetUserItemVoteResult_t value);
std::string ToJSON(ItemInstalled_t value);
std::string ToJSON(RemoveAppDependencyResult_t value);
std::string ToJSON(RemoveUGCDependencyResult_t value);
std::string ToJSON(SetUserItemVoteResult_t value);
std::string ToJSON(StartPlaytimeTrackingResult_t value);
std::string ToJSON(SteamUGCQueryCompleted_t value);
std::string ToJSON(StopPlaytimeTrackingResult_t value);
std::string ToJSON(SubmitItemUpdateResult_t value);
std::string ToJSON(UserFavoriteItemsListChanged_t value);
std::string ToJSON(SteamUGCDetails_t value);
std::string ToJSON(ClientGameServerDeny_t value);
std::string ToJSON(EncryptedAppTicketResponse_t value);
std::string ToJSON(GameWebCallback_t value);
std::string ToJSON(GetAuthSessionTicketResponse_t value);
std::string ToJSON(IPCFailure_t value);
std::string ToJSON(MicroTxnAuthorizationResponse_t value);
std::string ToJSON(SteamServerConnectFailure_t value);
std::string ToJSON(SteamServersDisconnected_t value);
std::string ToJSON(StoreAuthURLResponse_t value);
std::string ToJSON(ValidateAuthTicketResponse_t value);
std::string ToJSON(GlobalAchievementPercentagesReady_t value);
std::string ToJSON(GlobalStatsReceived_t value);
std::string ToJSON(LeaderboardFindResult_t value);
std::string ToJSON(LeaderboardScoresDownloaded_t value);
std::string ToJSON(LeaderboardScoreUploaded_t value);
std::string ToJSON(LeaderboardUGCSet_t value);
std::string ToJSON(NumberOfCurrentPlayers_t value);
std::string ToJSON(PS3TrophiesInstalled_t value);
std::string ToJSON(UserAchievementIconFetched_t value);
std::string ToJSON(UserAchievementStored_t value);
std::string ToJSON(UserStatsReceived_t value);
std::string ToJSON(UserStatsStored_t value);
std::string ToJSON(UserStatsUnloaded_t value);
std::string ToJSON(LeaderboardEntry_t value);
std::string ToJSON(CheckFileSignature_t value);
std::string ToJSON(GamepadTextInputDismissed_t value);
std::string ToJSON(LowBatteryPower_t value);
std::string ToJSON(SteamAPICallCompleted_t value);
std::string ToJSON(BroadcastUploadStop_t value);
std::string ToJSON(GetOPFSettingsResult_t value);
std::string ToJSON(GetVideoURLResult_t value);

#endif // _TOJSON_H
