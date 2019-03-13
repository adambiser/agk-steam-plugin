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

#include "SteamPlugin.h"

// Overlay methods
void SteamPlugin::OnGameOverlayActivated(GameOverlayActivated_t *pParam)
{
	agk::Log("Callback: OnGameOverlayActivated");
	g_IsGameOverlayActive = pParam->m_bActive != 0;
}

// Callback for RequestUserInformation and more.
void SteamPlugin::OnPersonaStateChange(PersonaStateChange_t *pParam)
{
	agk::Log("Callback: OnPersonaStateChange");
	if (m_PersonaStateChangedEnabled)
	{
		StoreCallbackResult(PersonaStateChange, *pParam);
	}
	if (m_AvatarImageLoadedEnabled)
	{
		if (pParam->m_nChangeFlags & k_EPersonaChangeAvatar)
		{
			// Allow HasAvatarImageLoaded to report avatar changes from here as well.
			StoreCallbackResult(AvatarImageLoadedUser, pParam->m_ulSteamID);
		}
	}
}

bool SteamPlugin::HasPersonaStateChange()
{
	GetNextCallbackResult(PersonaStateChange);
}

void SteamPlugin::OnAvatarImageLoaded(AvatarImageLoaded_t *pParam)
{
	agk::Log("Callback: OnAvatarImageLoaded");
	if (m_AvatarImageLoadedEnabled)
	{
		StoreCallbackResult(AvatarImageLoadedUser, pParam->m_steamID);
	}
}

bool SteamPlugin::HasAvatarImageLoadedUser()
{
	GetNextCallbackResult(AvatarImageLoadedUser);
}

// NOTE: The Steam API appears to implicitly call RequestUserInformation when needed.
int SteamPlugin::GetFriendAvatar(CSteamID steamID, EAvatarSize size)
{
	CheckInitialized(SteamFriends, 0);
	m_AvatarImageLoadedEnabled = true;
	//SteamFriends()->RequestUserInformation(steamID, false);
	int hImage = 0;
	switch (size)
	{
	case Small:
		hImage = SteamFriends()->GetSmallFriendAvatar(steamID);
		break;
	case Medium:
		hImage = SteamFriends()->GetMediumFriendAvatar(steamID);
		break;
	case Large:
		hImage = SteamFriends()->GetLargeFriendAvatar(steamID);
		break;
	default:
		agk::PluginError("Requested invalid avatar size.");
		return 0;
	}
	return hImage;
}
