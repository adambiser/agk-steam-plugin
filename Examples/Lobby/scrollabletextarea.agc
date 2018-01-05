#option_explicit

global scrollableTextAreaMouseDownID as integer
global scrollableTextAreaMouseDownOffset as float

#constant SCROLLABLE_BACKGROUND_COLOR	32, 32, 32, 255
#constant THUMB_BACKGROUND_COLOR		255, 255, 255, 255

#constant THUMB_WIDTH	16

Type ScrollableTextArea
	backgroundID as integer // sprite
	textID as integer
	x as float
	y as float
	width as float
	height as float
	thumbID as integer
EndType

Function CreateScrollableTextArea(x as float, y as float, width as float, height as float)
	area as ScrollableTextArea
	area.x = x
	area.y = y
	area.width = width
	area.height = height
	area.backgroundID = CreateSprite(CreateImageColor(SCROLLABLE_BACKGROUND_COLOR))
	SetSpritePosition(area.backgroundID, x, y)
	SetSpriteSize(area.backgroundID, width, height)
	// Set up scrolling status text
	area.textID = CreateText("")
	SetTextPosition(area.textID, x, y)
	SetTextScissor(area.textID, x, y, x + width, y + height)
	SetTextSize(area.textID, 18)
	SetTextMaxWidth(area.textID, width - THUMB_WIDTH)
	// Create thumb
	area.thumbID = CreateSprite(CreateImageColor(THUMB_BACKGROUND_COLOR))
	SetSpritePosition(area.thumbID, x + width - THUMB_WIDTH, y)
	SetSpriteSize(area.thumbID, THUMB_WIDTH, THUMB_WIDTH)
EndFunction area

Function ClearScrollableTextArea(area as ScrollableTextArea)
	SetTextString(area.textID, "")
	SetTextPosition(area.textID, area.x, area.y)
	RefreshScrollableTextAreaThumbSize(area)
EndFunction

Function RefreshScrollableTextAreaThumbSize(area as ScrollableTextArea)
	if not GetSpriteVisible(area.backgroundID)
		SetSpriteVisible(area.thumbID, 0)
		ExitFunction
	endif
	height as integer
	height = GetTextTotalHeight(area.textID)
	if height <= area.height
		SetSpriteVisible(area.thumbID, 0)
	else
		height = area.height * (area.height / height)
		if height < THUMB_WIDTH
			height = THUMB_WIDTH
		endif
		SetSpriteVisible(area.thumbID, 1)
		SetSpriteSize(area.thumbID, THUMB_WIDTH, height)
		RefreshScrollableTextAreaThumbPosition(area)
	endif
EndFunction

Function RefreshScrollableTextAreaThumbPosition(area as ScrollableTextArea)
	height as float
	height = GetTextTotalHeight(area.textID) - area.height
	above as float
	above = area.y - GetTextY(area.textID)
	SetSpriteY(area.thumbID, area.y + (area.height - GetSpriteHeight(area.thumbID)) * (above / height))
EndFunction

Function PerformVerticalScroll(area as ScrollableTextArea)
	if scrollableTextAreaMouseDownID = area.thumbID
		if GetRawMouseLeftReleased()
			scrollableTextAreaMouseDownID = 0
		elseif GetRawMouseLeftState()
			// Drag the scrollbar thumb around.
			thumbY as float
			thumbY = GetRawMouseY() - scrollableTextAreaMouseDownOffset
			if thumbY < area.y
				thumbY = area.y
			elseif thumbY + GetSpriteHeight(area.thumbID) > area.y + area.height
				thumbY = area.y + area.height - GetSpriteHeight(area.thumbID)
			endif
			SetSpriteY(area.thumbID, thumbY)
			textY as float
			textY = area.y - (thumbY - area.y) / (area.height - GetSpriteHeight(area.thumbID)) * (GetTextTotalHeight(area.textID) - area.height)
			SetTextY(area.textID, textY)
		endif
	endif
	if GetSpriteVisible(area.backgroundID) = 0
		ExitFunction 0
	endif
	mouseX as float
	mouseY as float
	mouseX = GetRawMouseX()
	mouseY = GetRawMouseY()
	if GetSpriteHitTest(area.backgroundID, mouseX, mouseY) = 0
		ExitFunction 0
	endif
	delta as float
	delta = GetRawMouseWheelDelta()
	if delta = 0
		if GetRawMouseLeftPressed()
			if GetSpriteHitTest(area.thumbID, mouseX, mouseY)
				scrollableTextAreaMouseDownID = area.thumbID
				scrollableTextAreaMouseDownOffset = mouseY - GetSpriteY(area.thumbID)
			endif
		endif
		ExitFunction 0
	endif
	newY as float
	newY = GetTextY(area.textID) + GetTextSize(area.textID) * delta
	if newY >= area.y
		// Don't allow the top of the text to go below the top of the text area.
		newY = area.y
	else
		height as float
		height = GetTextTotalHeight(area.textID)
		if height < area.height
			height = area.height
		endif
		// Don't allow the bottom of the text to go above the bottom of the text area.
		if newY + height < area.y + area.height
			newY = (area.y + area.height) - height
		endif
	endif
	SetTextY(area.textID, newY)
	RefreshScrollableTextAreaThumbPosition(area)
EndFunction 1

Function SetScrollableTextAreaVisible(area as ScrollableTextArea, visible as integer)
	SetSpriteVisible(area.backgroundID, visible)
	SetTextVisible(area.textID, visible)
	RefreshScrollableTextAreaThumbSize(area)
EndFunction

Function AddLineToScrollableTextArea(area as ScrollableTextArea, text as string)
	SetTextString(area.textID, GetTextString(area.textID) + text + NEWLINE)
	// Scroll the text upward.
	height as float
	height = GetTextTotalHeight(area.textID)
	if height < area.height
		height = area.height
	endif
	SetTextPosition(area.textID, area.x, area.y - (height - area.height))
	RefreshScrollableTextAreaThumbSize(area)
EndFunction
