import ui, app, grp, playersettingmodule, wndMgr, localeinfo, chr, player, chat, constinfo, net, uicommon

PVP_DUEL_ARENA_COUNT = 8
IMG_DIR = "d:/ymir work/ui/game/pvp_duel/"

WINDOW_ALPHA_PERCENT = 0.6

class TextToolTip(ui.Window):
	def __init__(self):
		ui.Window.__init__(self, "TOP_MOST")
		self.SetWindowName("TextToolTip")
		textLine = ui.TextLine()
		textLine.SetParent(self)
		textLine.SetHorizontalAlignCenter()
		textLine.SetOutline()
		textLine.Show()
		self.textLine = textLine
	def __del__(self):
		ui.Window.__del__(self)
	def SetText(self, text):
		self.textLine.SetText(text)
	def OnRender(self):
		(mouseX, mouseY) = wndMgr.GetMousePosition()
		self.textLine.SetPosition(mouseX, mouseY - 15)

class DuelInfoWindow(ui.Window):
	def __del__(self):
		ui.Window.__del__(self)
	def Destroy(self):
		self.children = {}
	def __init__(self):
		ui.Window.__init__(self)
		self.Destroy()
		self.__LoadWindow()
	def IsGM(self):
		playerName = player.GetName()
		if playerName == "dracaryS" or playerName.find("[") != -1:
			return True
		return False
	def __LoadWindow(self):
		global PVP_DUEL_ARENA_COUNT

		self.AddFlag("not_pick")

		#arenaTitleBar = ui.Bar()
		#arenaTitleBar.SetParent(self)
		#arenaTitleBar.AddFlag("not_pick")
		#arenaTitleBar.SetSize(5+43+5+123+8+37+5+123+5+43+5,23)
		#arenaTitleBar.SetPosition(0, -25)
		#arenaTitleBar.SetColor(grp.GenerateColor(68.0/255.0, 68.0/255.0, 68.0/255.0, WINDOW_ALPHA_PERCENT))
		#arenaTitleBar.Show()
		#self.children["arenaTitleBar"] = arenaTitleBar

		#arenaTitle = ui.TextLine()
		#arenaTitle.SetParent(arenaTitleBar)
		#arenaTitle.AddFlag("not_pick")
		#arenaTitle.SetHorizontalAlignLeft()
		#arenaTitle.SetPosition(3, 3)
		#arenaTitle.SetFontName("Tahoma:14")
		#arenaTitle.SetText("")
		#arenaTitle.SetOutline()
		#arenaTitle.Show()
		#self.children["arenaTitle"] = arenaTitle
		
		isGM = self.IsGM()

		for j in xrange(PVP_DUEL_ARENA_COUNT):
			arenaBar = ui.Bar()
			arenaBar.SetParent(self)
			arenaBar.AddFlag("not_pick")
			arenaBar.SetSize(5+43+5+123+8+37+5+123+5+43+5,46)
			arenaBar.SetPosition(0, j*(arenaBar.GetHeight()+5))
			arenaBar.SetColor(grp.GenerateColor(68.0/255.0, 68.0/255.0, 68.0/255.0, WINDOW_ALPHA_PERCENT))
			arenaBar.Show()
			self.children["arenaBar%d"%j] = arenaBar

			avatar0 = ui.ExpandedImageBox()
			avatar0.SetParent(arenaBar)
			avatar0.AddFlag("not_pick")
			avatar0.SetAlpha(WINDOW_ALPHA_PERCENT)
			avatar0.SetPosition(5, 2)
			self.children["avatar0%d"%j] = avatar0

			avatar0Camera = ui.Button()
			avatar0Camera.SetParent(avatar0)
			avatar0Camera.SetUpVisual(IMG_DIR+"eye.tga")
			avatar0Camera.SetOverVisual(IMG_DIR+"eye.tga")
			avatar0Camera.SetDownVisual(IMG_DIR+"eye.tga")
			avatar0Camera.SAFE_SetEvent(self.SetCameraMode, j, 0)
			avatar0Camera.Show()
			self.children["avatar0%dCamera"%j] = avatar0Camera

			name0 = ui.TextLine()
			name0.SetParent(arenaBar)
			name0.AddFlag("not_pick")
			name0.SetPosition(5+43+5, 7)
			name0.SetHorizontalAlignLeft()
			name0.SetOutline()
			self.children["name0%d"%j] = name0

			if isGM:
				removeBtn0 = ui.Button()
				removeBtn0.SetParent(arenaBar)
				removeBtn0.SetUpVisual("d:/ymir work/ui/public/small_button_01.sub")
				removeBtn0.SetOverVisual("d:/ymir work/ui/public/small_button_02.sub")
				removeBtn0.SetDownVisual("d:/ymir work/ui/public/small_button_03.sub")
				#removeBtn0.SetPosition(5+43+5+name0.GetTextSize()[0]+10, 7)
				removeBtn0.SetPosition((arenaBar.GetWidth()/2)-removeBtn0.GetWidth()-40,5)
				removeBtn0.SetText("Remove")
				removeBtn0.SAFE_SetEvent(self.RemovePlayer, j, 0)
				self.children["removeBtn0%d"%j]=removeBtn0

			gauge0 = ui.AniImageBox()
			gauge0.SetParent(arenaBar)
			gauge0.SetPosition(5+43+5, 26)
			gauge0.SetSize(123,11)
			gauge0.isPoisoned = -1
			gauge0.curPoint = 0
			gauge0.maxPoint = 0
			self.children["gauge0%d"%j] = gauge0

			score0 = ui.TextLine()
			score0.SetParent(arenaBar)
			score0.AddFlag("not_pick")
			score0.SetPosition(5+43+5+123, 5)
			score0.SetFontName("Tahoma:18")
			score0.SetHorizontalAlignRight()
			score0.SetOutline()
			self.children["score0%d"%j] = score0

			loadingImage = ui.ExpandedImageBox()
			loadingImage.SetParent(arenaBar)
			loadingImage.AddFlag("not_pick")
			loadingImage.LoadImage(IMG_DIR+"loading.tga")
			loadingImage.SetPosition(5+43+5+123+5, 1)
			loadingImage.SetAlpha(WINDOW_ALPHA_PERCENT)
			loadingImage.rotation = 0
			#loadingImage.Show()
			self.children["loadingImage%d"%j] = loadingImage

			loadingSecond = ui.TextLine()
			loadingSecond.SetParent(arenaBar)
			loadingSecond.AddFlag("not_pick")
			loadingSecond.SetPosition(5+43+5+123+5+20, 14)
			loadingSecond.SetHorizontalAlignCenter()
			loadingSecond.SetOutline()
			loadingSecond.SetFontName("Tahoma:15")
			loadingSecond.SetText("10")
			#loadingSecond.second = app.GetGlobalTimeStamp()+10
			loadingSecond.second = 0
			#loadingSecond.Show()
			self.children["loadingSecond%d"%j] = loadingSecond

			vsImage = ui.ExpandedImageBox()
			vsImage.SetParent(arenaBar)
			vsImage.AddFlag("not_pick")
			vsImage.LoadImage(IMG_DIR+"vs.tga")
			vsImage.SetPosition(5+43+5+123+5, 1)
			vsImage.SetAlpha(WINDOW_ALPHA_PERCENT)
			#vsImage.Show()
			self.children["vsImage%d"%j] = vsImage

			gauge1 = ui.AniImageBox()
			gauge1.SetParent(arenaBar)
			gauge1.SetPosition(5+43+5+123+8+vsImage.GetWidth()+5, 26)
			gauge1.SetSize(123,11)
			gauge1.isPoisoned = -1
			gauge1.curPoint = 0
			gauge1.maxPoint = 0
			self.children["gauge1%d"%j] = gauge1

			score1 = ui.TextLine()
			score1.AddFlag("not_pick")
			score1.SetParent(arenaBar)
			score1.SetPosition(5+43+5+123+8+vsImage.GetWidth()+5, 5)
			score1.SetFontName("Tahoma:18")
			score1.SetHorizontalAlignLeft()
			score1.SetOutline()
			self.children["score1%d"%j] = score1

			name1 = ui.TextLine()
			name1.SetParent(arenaBar)
			name1.AddFlag("not_pick")
			name1.SetPosition(5+43+5+123+8+vsImage.GetWidth()+5+123, 7)
			name1.SetHorizontalAlignRight()
			name1.SetOutline()
			self.children["name1%d"%j] = name1
			
			if isGM:
				removeBtn1 = ui.Button()
				removeBtn1.SetParent(arenaBar)
				removeBtn1.SetUpVisual("d:/ymir work/ui/public/small_button_01.sub")
				removeBtn1.SetOverVisual("d:/ymir work/ui/public/small_button_02.sub")
				removeBtn1.SetDownVisual("d:/ymir work/ui/public/small_button_03.sub")
				removeBtn1.SetPosition((arenaBar.GetWidth()/2)+(removeBtn1.GetWidth()/2)+25, 7)
				removeBtn1.SetText("Remove")
				removeBtn1.SAFE_SetEvent(self.RemovePlayer, j, 1)
				self.children["removeBtn1%d"%j]=removeBtn1

			avatar1 = ui.ExpandedImageBox()
			avatar1.SetParent(arenaBar)
			avatar1.AddFlag("not_pick")
			avatar1.SetAlpha(WINDOW_ALPHA_PERCENT)
			avatar1.SetPosition(5+43+5+123+8+vsImage.GetWidth()+5+123+5, 2)
			self.children["avatar1%d"%j] = avatar1

			avatar1Camera = ui.Button()
			avatar1Camera.SetParent(avatar1)
			avatar1Camera.SetUpVisual(IMG_DIR+"eye.tga")
			avatar1Camera.SetOverVisual(IMG_DIR+"eye.tga")
			avatar1Camera.SetDownVisual(IMG_DIR+"eye.tga")
			avatar1Camera.SAFE_SetEvent(self.SetCameraMode, j, 1)
			avatar1Camera.Show()
			self.children["avatar1%dCamera"%j] = avatar1Camera

			self.ClearArena(j)

			self.SetPosition(5,wndMgr.GetScreenHeight()-(PVP_DUEL_ARENA_COUNT*(arenaBar.GetHeight()+5))-150)
			self.SetSize(arenaBar.GetWidth(), (PVP_DUEL_ARENA_COUNT*(arenaBar.GetHeight()+5)))

		self.Show()
		self.SetTop()

	def RemovePlayer(self, pvpArenaIndex, pvpPlayerIndex):
		if self.IsGM() == False:
			return
		clickPlayerName = ""
		if self.children.has_key("name%d%d"%(pvpPlayerIndex, pvpArenaIndex)):
			clickPlayerName = self.children["name%d%d"%(pvpPlayerIndex, pvpArenaIndex)].GetText()
		if clickPlayerName == "":
			return

		removePlayerQuestionDialog = uicommon.QuestionDialog()
		removePlayerQuestionDialog.SetText(localeinfo.PVP_TOURNAMENT_REMOVE_PLAYER%clickPlayerName)
		removePlayerQuestionDialog.SetAcceptEvent(lambda arg=TRUE: self.AnswerRemovePlayer(arg))
		removePlayerQuestionDialog.SetCancelEvent(lambda arg=FALSE: self.AnswerRemovePlayer(arg))
		removePlayerQuestionDialog.Open()
		removePlayerQuestionDialog.clickPlayerName = clickPlayerName
		self.children["removePlayerQuestionDialog"] = removePlayerQuestionDialog

	def AnswerRemovePlayer(self, flag):
		if flag:
			net.SendChatPacket("/pvp_duel remove "+self.children["removePlayerQuestionDialog"].clickPlayerName)
		self.children["removePlayerQuestionDialog"].Hide()
		del self.children["removePlayerQuestionDialog"]

	def SetCameraMode(self, pvpArenaIndex, pvpPlayerIndex, isAuto = False):
		(currentCameraPlayer,mainPlayer) = ("","")
		if not self.children.has_key("CURRENT_CAMERA_MODE"):
			self.children["CURRENT_CAMERA_MODE"] = ""
			self.children["CURRENT_CAMERA_MODE_INDEX"] = 0
			self.children["CURRENT_CAMERA_MODE_PLAYER"] = 0
			self.children["MAIN_CAMERA_MODE"] = player.GetName()
		else:
			currentCameraPlayer = self.children["CURRENT_CAMERA_MODE"]
			mainPlayer = self.children["MAIN_CAMERA_MODE"]

		clickPlayerName = ""
		if self.children.has_key("name%d%d"%(pvpPlayerIndex, pvpArenaIndex)):
			clickPlayerName = self.children["name%d%d"%(pvpPlayerIndex, pvpArenaIndex)].GetText()

		if clickPlayerName == "":
			return
		elif currentCameraPlayer != "" and clickPlayerName == currentCameraPlayer and isAuto == False:
			self.ExitCameraMode()
			return
		elif clickPlayerName == mainPlayer:
			return

		if chr.SetMainInstance(clickPlayerName, mainPlayer, True):
			self.children["CURRENT_CAMERA_MODE_INDEX"] = pvpArenaIndex
			self.children["CURRENT_CAMERA_MODE_PLAYER"] = pvpPlayerIndex
			self.children["CURRENT_CAMERA_MODE"] = clickPlayerName
			net.SendChatPacket("/camera_mode camera_on "+clickPlayerName)

			interface = constinfo.GetInterfaceInstance()
			if interface != None:
				if interface.wndGameButton:
					interface.wndGameButton.UpdateCameraMode()
		else:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.PVP_TOURNAMENT_CAMERA_TRY_NEAR%clickPlayerName)

	def ExitCameraMode(self):
		if self.children["CURRENT_CAMERA_MODE"] != "":
			self.children["CURRENT_CAMERA_MODE"] = ""
			self.children["CURRENT_CAMERA_MODE_INDEX"] = 0
			self.children["CURRENT_CAMERA_MODE_PLAYER"] = 0
			mainPlayer = self.children["MAIN_CAMERA_MODE"]
			chr.SetMainInstance(mainPlayer, mainPlayer, False)

			interface = constinfo.GetInterfaceInstance()
			if interface != None:
				if interface.wndGameButton:
					interface.wndGameButton.UpdateCameraMode()

			net.SendChatPacket("/camera_mode camera_off")

	def SetHPTooltip(self, showStatus, pvpArenaIndex, pvpPlayerIndex):
		if not self.children.has_key("tooltipHP"):
			self.children["tooltipHP"] = TextToolTip()
		tooltipHP = self.children["tooltipHP"]

		if showStatus == False:
			if tooltipHP.IsShow():
				tooltipHP.Hide()
		else:
			gaugePtr = self.children["gauge%d%d"%(pvpPlayerIndex,pvpArenaIndex)]
			tooltipHP.SetText("%s : %s / %s" % (localeinfo.TASKBAR_HP, localeinfo.NumberToDecimalString(gaugePtr.curPoint), localeinfo.NumberToDecimalString(gaugePtr.maxPoint)))
			if not tooltipHP.IsShow():
				tooltipHP.Show()

	def ClearArena(self, arenaIndex):
		if self.children.has_key("CURRENT_CAMERA_MODE_INDEX"):
			if self.children["CURRENT_CAMERA_MODE_INDEX"] == arenaIndex:
				self.ExitCameraMode()
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.PVP_TOURNAMENT_CAMERA_CLOSE_AUTO)

		self.SetHP(arenaIndex, 0, 0, 0, 0, 0)
		self.SetHP(arenaIndex, 1, 0, 0, 0, 0)
		self.SetRace(arenaIndex, 0, -1)
		self.SetRace(arenaIndex, 1, -1)
		self.SetScore(arenaIndex, 0, -1)
		self.SetScore(arenaIndex, 1, -1)
		self.SetName(arenaIndex, 0, "")
		self.SetName(arenaIndex, 1, "")

	def SetStartTime(self, arenaIndex, serverStartTime):
		if self.children.has_key("loadingSecond%d"%arenaIndex):
			self.children["loadingSecond%d"%arenaIndex].second = serverStartTime

		#if self.children.has_key("CURRENT_CAMERA_MODE_INDEX") and self.children.has_key("CURRENT_CAMERA_MODE_PLAYER"):
		#	self.SetCameraMode(self.children["CURRENT_CAMERA_MODE_INDEX"], self.children["CURRENT_CAMERA_MODE_PLAYER"], True)

	def SetRace(self, arenaIndex, playerIndex, playerRace):
		if self.children.has_key("avatar%d%d"%(playerIndex,arenaIndex)):
			raceImage = self.children["avatar%d%d"%(playerIndex, arenaIndex)]

			if not playersettingmodule.FACE_DICT.has_key(playerRace) or playerRace == -1:
				if raceImage.IsShow():
					raceImage.Hide()
			else:
				raceImages = playersettingmodule.FACE_DICT[playerRace]
				raceImage.LoadImage(raceImages)
				if not raceImage.IsShow():
					raceImage.Show()

	def SetScore(self, arenaIndex, playerIndex, playerScore):
		if self.children.has_key("score%d%d"%(playerIndex,arenaIndex)):
			scoreText = self.children["score%d%d"%(playerIndex, arenaIndex)]
			if playerScore == -1:
				scoreText.Hide()
			else:
				scoreText.SetText(str(playerScore))
				scoreText.Show()

	def SetName(self, arenaIndex, playerIndex, playerName):
		
		if self.children.has_key("name%d%d"%(playerIndex,arenaIndex)):
			nameText = self.children["name%d%d"%(playerIndex, arenaIndex)]
			if playerName == "":
				if nameText.IsShow():
					nameText.Hide()
				if self.children.has_key("removeBtn%d%d"%(playerIndex, arenaIndex)) and self.IsGM() == True:
					self.children["removeBtn%d%d"%(playerIndex,arenaIndex)].Hide()
			else:
				nameText.SetText(playerName)
				if not nameText.IsShow():
					nameText.Show()
				if self.children.has_key("removeBtn%d%d"%(playerIndex, arenaIndex)) and self.IsGM() == True:
					self.children["removeBtn%d%d"%(playerIndex,arenaIndex)].Show()


	def SetHP(self, arenaIndex, playerIndex, playerHPPercent, playerMinHP, playerMaxHP, isPoisoned):
		if self.children.has_key("gauge%d%d"%(playerIndex,arenaIndex)):
			gaugeImage = self.children["gauge%d%d"%(playerIndex, arenaIndex)]
			gaugeImage.curPoint = playerMinHP
			gaugeImage.maxPoint = playerMaxHP
			needChangeImages = False
			if playerHPPercent == 0:
				if gaugeImage.IsShow():
					gaugeImage.Hide()
				return
			if gaugeImage.isPoisoned == -1 or isPoisoned != gaugeImage.isPoisoned:
				gaugeImage.isPoisoned = isPoisoned
				needChangeImages = True

			if needChangeImages:
				gaugeImage.ClearImages()
				gauge_img = "hp_red/"
				if gaugeImage.isPoisoned:
					gauge_img = "hp_green/"
				for j in range(1, 9):
					gaugeImage.AppendImage(IMG_DIR+gauge_img+str("%02d"%j)+".tga")
				if playerIndex == 1:
					gaugeImage.SetPosition(5+43+5+123+8+37+5, 26)
				else:
					gaugeImage.SetPosition(5+43+5, 26)

			#reverse
			if playerIndex == 1:
				gaugeImage.SetPercentageReverse(playerHPPercent, 100)
			else:
				gaugeImage.SetPercentage(playerHPPercent, 100)
			if not gaugeImage.IsShow():
				gaugeImage.Show()

	def OnUpdate(self):
		hideHP=True
		for j in xrange(PVP_DUEL_ARENA_COUNT):
			for x in xrange(2):
				gauge = self.children["gauge%d%d"%(x,j)]
				if True == gauge.IsIn():
					self.SetHPTooltip(True, j, x)
					hideHP = False
					break

			if self.children.has_key("loadingImage%d"%j) and self.children.has_key("loadingSecond%d"%j) and self.children.has_key("vsImage%d"%j):
				loadingSecond = self.children["loadingSecond%d"%j]
				loadingImage = self.children["loadingImage%d"%j]
				vsImage = self.children["vsImage%d"%j]

				leftTime = loadingSecond.second-app.GetGlobalTimeStamp()
				if leftTime < 0:
					if loadingSecond.IsShow():
						loadingSecond.Hide()
					if loadingImage.IsShow():
						loadingImage.Hide()
					if not vsImage.IsShow():
						vsImage.Show()
				else:
					if vsImage.IsShow():
						vsImage.Hide()

					loadingSecond.SetText(str(leftTime))
					if not loadingSecond.IsShow():
						loadingSecond.Show()

					rotation = loadingImage.rotation
					rotation += 5
					loadingImage.SetRotation(rotation)
					if rotation > 360:
						rotation = 0
					loadingImage.rotation = rotation
					if not loadingImage.IsShow():
						loadingImage.Show()
		if hideHP:
			self.SetHPTooltip(False, 0, 0)

class PvPDuelAdminPanel(ui.BoardWithTitleBar):
	def __del__(self):
		ui.BoardWithTitleBar.__del__(self)
	def Destroy(self):
		self.children = {}
	def __init__(self):
		ui.BoardWithTitleBar.__init__(self)
		self.children = {}
		self.Destroy()
		self.__LoadWindow()
	def __LoadWindow(self):
		self.AddFlag("attach")
		self.AddFlag("movable")
		self.SetSize(350, 210)
		self.SetCenterPosition()
		self.SetTitleName(localeinfo.PVP_TOURNAMENT_PANEL_TITLE)

		createPvPbg = ui.ThinBoardCircle()
		createPvPbg.AddFlag("attach")
		createPvPbg.AddFlag("movable")
		createPvPbg.SetParent(self)
		createPvPbg.SetPosition(8, 30)
		createPvPbg.SetSize(self.GetWidth()-16,self.GetHeight()-38)
		createPvPbg.Show()
		self.children["createPvPbg"] = createPvPbg

		self.children["pvpType"] = 0

		START_Y_POSITION=20
		START_X_POSITION=15

		pvpTypeText = ui.TextLine()
		pvpTypeText.SetParent(createPvPbg)
		pvpTypeText.SetHorizontalAlignLeft()
		pvpTypeText.SetPosition(START_X_POSITION, START_Y_POSITION)
		pvpTypeText.SetText(localeinfo.PVP_TOURNAMENT_PVP_TYPE)
		pvpTypeText.Show()
		self.children["pvpTypeText"] = pvpTypeText

		pvpTypes = {
			0: localeinfo.PVP_TOURNAMENT_ALL_CHARACTER,
			1: localeinfo.PVP_TOURNAMENT_ONLY_WARRIOR,
			2: localeinfo.PVP_TOURNAMENT_ONLY_ASSASSIN,
			3: localeinfo.PVP_TOURNAMENT_ONLY_SURA,
			4: localeinfo.PVP_TOURNAMENT_ONLY_SHAMAN,
		}


		START_Y_POSITION_COMBO = START_Y_POSITION
		pvpTypeComboBox = ui.ComboBoxImage(createPvPbg,"d:/ymir work/ui/pattern/select_image.tga",START_X_POSITION+pvpTypeText.GetTextSize()[0]+10,START_Y_POSITION)
		for key, name in pvpTypes.iteritems():
			pvpTypeComboBox.InsertItem(key, name)
		pvpTypeComboBox.SetEvent(lambda x : self.__SetPvPType(x))
		pvpTypeComboBox.Show()

		START_Y_POSITION+=30

		pvpMinLvLText = ui.TextLine()
		pvpMinLvLText.SetParent(createPvPbg)
		pvpMinLvLText.SetHorizontalAlignLeft()
		pvpMinLvLText.SetPosition(START_X_POSITION, START_Y_POSITION)
		pvpMinLvLText.SetText(localeinfo.PVP_TOURNAMENT_MIN_LEVEL)
		pvpMinLvLText.Show()
		self.children["pvpMinLvLText"] = pvpMinLvLText

		pvpMinLvLEditLineSlot = ui.SlotBar()
		pvpMinLvLEditLineSlot.SetParent(createPvPbg)
		pvpMinLvLEditLineSlot.SetSize(pvpTypeComboBox.GetWidth()/4,pvpTypeComboBox.GetHeight()+2)
		pvpMinLvLEditLineSlot.SetPosition(START_X_POSITION+pvpMinLvLText.GetTextSize()[0]+15, START_Y_POSITION)
		pvpMinLvLEditLineSlot.Show()
		self.children["pvpMinLvLEditLineSlot"] = pvpMinLvLEditLineSlot

		pvpMinLvLEditLine = ui.EditLine()
		pvpMinLvLEditLine.SetParent(pvpMinLvLEditLineSlot)
		pvpMinLvLEditLine.SetPosition(2, 1)
		pvpMinLvLEditLine.SetSize(pvpMinLvLEditLineSlot.GetWidth(),pvpMinLvLEditLineSlot.GetHeight())
		pvpMinLvLEditLine.SetNumberMode()
		pvpMinLvLEditLine.SetText("1")
		pvpMinLvLEditLine.SetMax(3)
		pvpMinLvLEditLine.Show()
		self.children["pvpMinLvLEditLine"] = pvpMinLvLEditLine

		pvpMaxLvLText = ui.TextLine()
		pvpMaxLvLText.SetParent(createPvPbg)
		pvpMaxLvLText.SetHorizontalAlignLeft()
		pvpMaxLvLText.SetPosition(START_X_POSITION+pvpMinLvLText.GetTextSize()[0]+15+pvpMinLvLEditLineSlot.GetWidth()+15, START_Y_POSITION)
		pvpMaxLvLText.SetText(localeinfo.PVP_TOURNAMENT_MAX_LEVEL)
		pvpMaxLvLText.Show()
		self.children["pvpMaxLvLText"] = pvpMaxLvLText

		pvpMaxLvLEditLineSlot = ui.SlotBar()
		pvpMaxLvLEditLineSlot.SetParent(createPvPbg)
		pvpMaxLvLEditLineSlot.SetSize(pvpTypeComboBox.GetWidth()/4,pvpTypeComboBox.GetHeight()+2)
		pvpMaxLvLEditLineSlot.SetPosition(START_X_POSITION+pvpMinLvLText.GetTextSize()[0]+15+pvpMinLvLEditLineSlot.GetWidth()+15+pvpMaxLvLText.GetTextSize()[0]+15, START_Y_POSITION)
		pvpMaxLvLEditLineSlot.Show()
		self.children["pvpMaxLvLEditLineSlot"] = pvpMaxLvLEditLineSlot

		pvpMaxLvLEditLine = ui.EditLine()
		pvpMaxLvLEditLine.SetParent(pvpMaxLvLEditLineSlot)
		pvpMaxLvLEditLine.SetPosition(2, 1)
		pvpMaxLvLEditLine.SetSize(pvpMaxLvLEditLineSlot.GetWidth(),pvpMaxLvLEditLineSlot.GetHeight())
		pvpMaxLvLEditLine.SetNumberMode()
		pvpMaxLvLEditLine.SetText("120")
		pvpMaxLvLEditLine.SetMax(3)
		pvpMaxLvLEditLine.Show()
		self.children["pvpMaxLvLEditLine"] = pvpMaxLvLEditLine

		START_Y_POSITION+= 30

		pvpMaxRegisterText = ui.TextLine()
		pvpMaxRegisterText.SetParent(createPvPbg)
		pvpMaxRegisterText.SetHorizontalAlignLeft()
		pvpMaxRegisterText.SetPosition(START_X_POSITION, START_Y_POSITION)
		pvpMaxRegisterText.SetText(localeinfo.PVP_TOURNAMENT_MAX_REGISTER)
		pvpMaxRegisterText.Show()
		self.children["pvpMaxRegisterText"] = pvpMaxRegisterText

		pvpMaxRegisterEditLineSlot = ui.SlotBar()
		pvpMaxRegisterEditLineSlot.SetParent(createPvPbg)
		pvpMaxRegisterEditLineSlot.SetSize(pvpTypeComboBox.GetWidth()/4,pvpTypeComboBox.GetHeight()+2)
		pvpMaxRegisterEditLineSlot.SetPosition(START_X_POSITION+pvpMaxRegisterText.GetTextSize()[0]+15, START_Y_POSITION)
		pvpMaxRegisterEditLineSlot.Show()
		self.children["pvpMaxRegisterEditLineSlot"] = pvpMaxRegisterEditLineSlot

		pvpMaxRegisterEditLine = ui.EditLine()
		pvpMaxRegisterEditLine.SetParent(pvpMaxRegisterEditLineSlot)
		pvpMaxRegisterEditLine.SetPosition(2, 1)
		pvpMaxRegisterEditLine.SetSize(pvpMaxRegisterEditLineSlot.GetWidth(),pvpMaxRegisterEditLineSlot.GetHeight())
		pvpMaxRegisterEditLine.SetNumberMode()
		pvpMaxRegisterEditLine.SetText("100")
		pvpMaxRegisterEditLine.SetMax(3)
		pvpMaxRegisterEditLine.Show()
		self.children["pvpMaxRegisterEditLine"] = pvpMaxRegisterEditLine


		pvpRegisterTimeText = ui.TextLine()
		pvpRegisterTimeText.SetParent(createPvPbg)
		pvpRegisterTimeText.SetHorizontalAlignLeft()
		pvpRegisterTimeText.SetPosition(START_X_POSITION+pvpMaxRegisterText.GetTextSize()[0]+15+pvpMaxRegisterEditLine.GetWidth()+15, START_Y_POSITION)
		pvpRegisterTimeText.SetText(localeinfo.PVP_TOURNAMENT_REGISTER_TIME)
		pvpRegisterTimeText.Show()
		self.children["pvpRegisterTimeText"] = pvpRegisterTimeText

		pvpRegisterTimeEditLineSlot = ui.SlotBar()
		pvpRegisterTimeEditLineSlot.SetParent(createPvPbg)
		pvpRegisterTimeEditLineSlot.SetSize(pvpTypeComboBox.GetWidth()/4,pvpTypeComboBox.GetHeight()+2)
		pvpRegisterTimeEditLineSlot.SetPosition(START_X_POSITION+pvpMaxRegisterText.GetTextSize()[0]+15+pvpMaxRegisterEditLine.GetWidth()+15+pvpRegisterTimeText.GetTextSize()[0]+5, START_Y_POSITION)
		pvpRegisterTimeEditLineSlot.Show()
		self.children["pvpRegisterTimeEditLineSlot"] = pvpRegisterTimeEditLineSlot

		pvpRegisterTimeEditLine = ui.EditLine()
		pvpRegisterTimeEditLine.SetParent(pvpRegisterTimeEditLineSlot)
		pvpRegisterTimeEditLine.SetPosition(2, 1)
		pvpRegisterTimeEditLine.SetSize(pvpRegisterTimeEditLineSlot.GetWidth(),pvpRegisterTimeEditLineSlot.GetHeight())
		pvpRegisterTimeEditLine.SetNumberMode()
		pvpRegisterTimeEditLine.SetText("60")
		pvpRegisterTimeEditLine.SetMax(3)
		pvpRegisterTimeEditLine.Show()
		self.children["pvpRegisterTimeEditLine"] = pvpRegisterTimeEditLine

		START_Y_POSITION+= 30

		pvpBetText = ui.TextLine()
		pvpBetText.SetParent(createPvPbg)
		pvpBetText.SetHorizontalAlignLeft()
		pvpBetText.SetPosition(START_X_POSITION, START_Y_POSITION)
		pvpBetText.SetText(localeinfo.PVP_TOURNAMENT_PVP_BET)
		pvpBetText.Show()
		self.children["pvpBetText"] = pvpBetText

		pvpBetEditLineSlot = ui.SlotBar()
		pvpBetEditLineSlot.SetParent(createPvPbg)
		pvpBetEditLineSlot.SetSize(pvpTypeComboBox.GetWidth(),pvpTypeComboBox.GetHeight()+2)
		pvpBetEditLineSlot.SetPosition(START_X_POSITION+pvpBetText.GetTextSize()[0]+15, START_Y_POSITION)
		pvpBetEditLineSlot.Show()
		self.children["pvpBetEditLineSlot"] = pvpBetEditLineSlot

		pvpBetEditLine = ui.EditLine()
		pvpBetEditLine.SetParent(pvpBetEditLineSlot)
		pvpBetEditLine.SetPosition(2, 1)
		pvpBetEditLine.SetSize(pvpBetEditLineSlot.GetWidth(),pvpBetEditLineSlot.GetHeight())
		pvpBetEditLine.SetNumberMode()
		pvpBetEditLine.SetText("0")
		pvpBetEditLine.SetMax(20)
		pvpBetEditLine.Show()
		self.children["pvpBetEditLine"] = pvpBetEditLine
		
		START_Y_POSITION+= 30


		pvpTypeComboBox = ui.ComboBoxImage(createPvPbg,"d:/ymir work/ui/pattern/select_image.tga",START_X_POSITION+pvpTypeText.GetTextSize()[0]+10,START_Y_POSITION_COMBO)
		for key, name in pvpTypes.iteritems():
			pvpTypeComboBox.InsertItem(key, name)
		pvpTypeComboBox.SetEvent(lambda x : self.__SetPvPType(x))
		pvpTypeComboBox.Show()
		self.children["pvpTypeComboBox"] = pvpTypeComboBox
		self.__SetPvPType(self.children["pvpType"])

		acceptButton = ui.Button()
		acceptButton.SetParent(createPvPbg)
		acceptButton.SetPosition(createPvPbg.GetWidth()-200, START_Y_POSITION)
		acceptButton.SetUpVisual("d:/ymir work/ui/public/large_button_01.sub")
		acceptButton.SetOverVisual("d:/ymir work/ui/public/large_button_02.sub")
		acceptButton.SetDownVisual("d:/ymir work/ui/public/large_button_03.sub")
		acceptButton.SetText("Start")
		acceptButton.SAFE_SetEvent(self.__ClickAccepButton)
		acceptButton.Show()
		self.children["acceptButton"] = acceptButton

		closeButton = ui.Button()
		closeButton.SetParent(createPvPbg)
		closeButton.SetPosition(createPvPbg.GetWidth()-100, START_Y_POSITION)
		closeButton.SetUpVisual("d:/ymir work/ui/public/large_button_01.sub")
		closeButton.SetOverVisual("d:/ymir work/ui/public/large_button_02.sub")
		closeButton.SetDownVisual("d:/ymir work/ui/public/large_button_03.sub")
		closeButton.SetText("Close")
		closeButton.SAFE_SetEvent(self.Hide)
		closeButton.Show()
		self.children["closeButton"] = closeButton

	def __ClickAccepButton(self):
		pvpType = self.children["pvpType"]

		if pvpType < 0 or pvpType > 4:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.PVP_TOURNAMENT_PROBLEM_PVP_TYPE)
			return

		minLevel = self.children["pvpMinLvLEditLine"].GetText()
		if not minLevel.isdigit():
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.PVP_TOURNAMENT_PROBLEM_MIN_LEVEL)
			return

		maxLevel = self.children["pvpMaxLvLEditLine"].GetText()
		if not maxLevel.isdigit():
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.PVP_TOURNAMENT_PROBLEM_MAX_LEVEL)
			return

		registerCount = self.children["pvpMaxRegisterEditLine"].GetText()
		if not registerCount.isdigit():
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.PVP_TOURNAMENT_PROBLEM_REGISTER_COUNT)
			return

		registerTime = self.children["pvpRegisterTimeEditLine"].GetText()
		if not registerTime.isdigit():
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.PVP_TOURNAMENT_PROBLEM_REGISTER_TIME)
			return

		betPrice = self.children["pvpBetEditLine"].GetText()
		if not betPrice.isdigit():
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.PVP_TOURNAMENT_PROBLEM_BET_PRICE)
			return

		net.SendChatPacket("/pvp_duel start %d %s %s %s %s %s"%(pvpType, minLevel, maxLevel, registerCount, registerTime, betPrice))
		self.Hide()

	def OnPressEscapeKey(self):
		self.Hide()
		return True
	def __SetPvPType(self, pvpType):
		pvpTypes = {
			0: localeinfo.PVP_TOURNAMENT_ALL_CHARACTER,
			1: localeinfo.PVP_TOURNAMENT_ONLY_WARRIOR,
			2: localeinfo.PVP_TOURNAMENT_ONLY_ASSASSIN,
			3: localeinfo.PVP_TOURNAMENT_ONLY_SURA,
			4: localeinfo.PVP_TOURNAMENT_ONLY_SHAMAN,
		}
		self.children["pvpType"] = pvpType
		self.children["pvpTypeComboBox"].SetCurrentItem(pvpTypes[pvpType])
		self.children["pvpTypeComboBox"].CloseListBox()
