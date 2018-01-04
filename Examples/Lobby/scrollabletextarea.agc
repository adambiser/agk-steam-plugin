#option_explicit

#constant SCROLLABLE_BACKGROUND_COLOR	32, 32, 32, 255

Type ScrollableTextArea
	backgroundID as integer // sprite
	textID as integer
	x as integer
	y as integer
	width as integer
	height as integer
EndType

Function CreateScrollableTextArea(x as integer, y as integer, width as integer, height as integer)
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
	SetTextMaxWidth(area.textID, width)
EndFunction area

Function PerformVerticalScroll(area as ScrollableTextArea)
	if GetSpriteVisible(area.backgroundID) = 0
		ExitFunction 0
	endif
	delta as float
	mouseX as float
	mouseY as float
	delta = GetRawMouseWheelDelta()
	mouseX = GetPointerX()
	mouseY = GetPointerY()
	if delta = 0
		ExitFunction 0
	endif
	if GetSpriteHitTest(area.backgroundID, mouseX, mouseY) = 0
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
EndFunction 1

Function SetScrollableTextAreaVisible(area as ScrollableTextArea, visible as integer)
	SetSpriteVisible(area.backgroundID, visible)
	SetTextVisible(area.textID, visible)
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
EndFunction
