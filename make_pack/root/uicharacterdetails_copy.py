import ui
import player
import item
import uitooltip
import wndMgr
import app
import localeinfo

class CharacterDetailsUI(ui.ScriptWindow):
	def __init__(self, parent):
		self.uiCharacterStatus = parent		
		ui.ScriptWindow.__init__(self)
		self.toolTip = uitooltip.ToolTip()
		
		self.__LoadScript()
	
	def __del__(self):
		self.uiCharacterStatus = None
		self.toolTip = None
		ui.ScriptWindow.__del__(self)
		
	def __LoadScript(self):
		
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "uiscript/characterdetailswindow.py")
		except:
			import exception
			exception.Abort("CharacterDetailsUI.__LoadScript")
		
		## CharacterWindow.py �� width = 253
		self.Width = 253 - 3
		self.board = self.GetChild("MainBoard")
		self.GetChild("TitleBar").CloseButtonHide()
		self.ScrollBar = self.GetChild("ScrollBar")
		self.ScrollBar.SetScrollEvent(ui.__mem_func__(self.OnScroll))
		
		## ��µǴ� UI �ִ� ����
		self.UI_MAX_COUNT = 13
		self.UI_MAX_VIEW_COUNT = 6
		
		## UI KEY & VALUE
		self.INFO_TEXT	= 0
		self.INFO_TOOLTIP = 1
		self.INFO_VALUE	= 2
		self.CATEGORY_STARTLINE	= -1
		self.CATEGORY_ENDLINE	= -2
		
		## Child ����
		self.labelList		= []
		self.labelValueList	= []
		self.labelTextList	= []
		self.horizonBarList	= []
		self.horizonBarNameList = []
		
		for i in xrange(self.UI_MAX_COUNT):
			self.labelList.append( self.GetChild("label%s"%i) )
			self.labelValueList.append( self.GetChild("labelvalue%s"%i) )
			self.labelTextList.append( self.GetChild("labelname%s"%i) )
			self.horizonBarList.append( self.GetChild("horizontalbar%s"%i) )
			self.horizonBarNameList.append( self.GetChild("horizontalbarName%s"%i) )
		
		self.__Initialize()
		
	def __Initialize(self):
		self.InfoList = []
		
		self.InfoList.append( [ localeinfo.DETAILS_CATE_1, "", self.CATEGORY_STARTLINE ] )
		self.InfoList.append([localeinfo.NEW_DETAILS_7, localeinfo.NEW_DETAILS_7_TOOLTIP, 160])
		self.InfoList.append([localeinfo.NEW_DETAILS_6, localeinfo.NEW_DETAILS_6_TOOLTIP, 159])
		self.InfoList.append( [ localeinfo.DETAILS_5, localeinfo.DETAILS_TOOLTIP_5, 53 ] )
		self.InfoList.append( [ localeinfo.DETAILS_7, localeinfo.DETAILS_TOOLTIP_7, 44 ] )
		self.InfoList.append( [ localeinfo.DETAILS_3, localeinfo.DETAILS_TOOLTIP_3, 45 ] )
		self.InfoList.append( [ localeinfo.DETAILS_9, localeinfo.DETAILS_TOOLTIP_9, 48 ] )
		self.InfoList.append( [ localeinfo.DETAILS_4, localeinfo.DETAILS_TOOLTIP_4, 47 ] )
		self.InfoList.append( [ localeinfo.DETAILS_8, localeinfo.DETAILS_TOOLTIP_8, 46 ] )
		self.InfoList.append([localeinfo.NEW_DETAILS_16, "", 171])
		self.InfoList.append([localeinfo.DETAILS_79, "", 162])
		self.InfoList.append([localeinfo.NEW_DETAILS_9, "", 170])
		self.InfoList.append([localeinfo.NEW_DETAILS_8, localeinfo.NEW_DETAILS_8_TOOLTIP, 161])
		self.InfoList.append( [ "", "", self.CATEGORY_ENDLINE ] )
		
		self.InfoList.append( [ localeinfo.DETAILS_CATE_2, "", self.CATEGORY_STARTLINE ] )
		self.InfoList.append( [ localeinfo.DETAILS_36, localeinfo.DETAILS_TOOLTIP_36, 54 ] )
		self.InfoList.append( [ localeinfo.DETAILS_37, localeinfo.DETAILS_TOOLTIP_37, 55 ] )
		self.InfoList.append( [ localeinfo.DETAILS_38, localeinfo.DETAILS_TOOLTIP_38, 56 ] )
		self.InfoList.append( [ localeinfo.DETAILS_39, localeinfo.DETAILS_TOOLTIP_39, 57 ] )
		self.InfoList.append( [ localeinfo.DETAILS_1, localeinfo.DETAILS_TOOLTIP_1, 43 ] )
		self.InfoList.append( [ localeinfo.DETAILS_41, localeinfo.DETAILS_TOOLTIP_41, 59 ] )
		self.InfoList.append( [ localeinfo.DETAILS_42, localeinfo.DETAILS_TOOLTIP_42, 60 ] )
		self.InfoList.append( [ localeinfo.DETAILS_43, localeinfo.DETAILS_TOOLTIP_43, 61 ] )
		self.InfoList.append( [ localeinfo.DETAILS_44, localeinfo.DETAILS_TOOLTIP_44, 62 ] )
		self.InfoList.append([localeinfo.NEW_DETAILS_1, localeinfo.NEW_DETAILS_1_TOOLTIP, 156])
		self.InfoList.append( [ "", "", self.CATEGORY_ENDLINE ] )
		
		self.InfoList.append( [ localeinfo.DETAILS_CATE_3, "", self.CATEGORY_STARTLINE ] )
		self.InfoList.append( [ localeinfo.DETAILS_30, localeinfo.DETAILS_TOOLTIP_30, 146 ] )
		self.InfoList.append( [ localeinfo.DETAILS_33, localeinfo.DETAILS_TOOLTIP_33, 50 ] )
		self.InfoList.append( [ localeinfo.DETAILS_31, localeinfo.DETAILS_TOOLTIP_31, 51 ] )
		self.InfoList.append( [ localeinfo.DETAILS_34, localeinfo.DETAILS_TOOLTIP_34, 147 ] )
		self.InfoList.append( [ localeinfo.DETAILS_35, localeinfo.DETAILS_TOOLTIP_35, 148 ] )
		self.InfoList.append( [ localeinfo.DETAILS_32, localeinfo.DETAILS_TOOLTIP_32, 149 ] )
		self.InfoList.append( [ localeinfo.DETAILS_24, localeinfo.DETAILS_TOOLTIP_24, 76 ] )
		self.InfoList.append( [ localeinfo.DETAILS_27, localeinfo.DETAILS_TOOLTIP_27, 75 ] )
		self.InfoList.append( [ localeinfo.DETAILS_25, localeinfo.DETAILS_TOOLTIP_25, 133 ] )
		self.InfoList.append( [ localeinfo.DETAILS_28, localeinfo.DETAILS_TOOLTIP_28, 78 ] )
		self.InfoList.append( [ localeinfo.DETAILS_29, localeinfo.DETAILS_TOOLTIP_29, 134 ] )
		self.InfoList.append( [ localeinfo.DETAILS_26, localeinfo.DETAILS_TOOLTIP_26, 135 ] )
		self.InfoList.append( [ "", "", self.CATEGORY_ENDLINE ] )
		
		self.InfoList.append( [ localeinfo.DETAILS_CATE_4, "", self.CATEGORY_STARTLINE ] )
		self.InfoList.append( [ localeinfo.DETAILS_71, "", 93 ] )
		self.InfoList.append( [ localeinfo.DETAILS_16, localeinfo.DETAILS_TOOLTIP_16, 121 ] )
		self.InfoList.append( [ localeinfo.DETAILS_14, localeinfo.DETAILS_TOOLTIP_14, 122 ] )
		self.InfoList.append( [ localeinfo.DETAILS_20, localeinfo.DETAILS_TOOLTIP_20, 40 ] )
		self.InfoList.append( [ localeinfo.DETAILS_21, localeinfo.DETAILS_TOOLTIP_21, 41 ] )
		self.InfoList.append( [ localeinfo.DETAILS_53, localeinfo.DETAILS_TOOLTIP_53, 38 ] )
		self.InfoList.append( [ localeinfo.DETAILS_54, localeinfo.DETAILS_TOOLTIP_54, 39 ] )
		self.InfoList.append( [ localeinfo.DETAILS_55, localeinfo.DETAILS_TOOLTIP_55, 37 ] )
		self.InfoList.append( [ "", "", self.CATEGORY_ENDLINE ] )
		
		self.InfoList.append( [ localeinfo.DETAILS_CATE_5, "", self.CATEGORY_STARTLINE ] )
		self.InfoList.append( [ localeinfo.DETAILS_46, localeinfo.DETAILS_TOOLTIP_46, 69 ] )
		self.InfoList.append( [ localeinfo.DETAILS_47, localeinfo.DETAILS_TOOLTIP_47, 70 ] )
		self.InfoList.append( [ localeinfo.DETAILS_48, localeinfo.DETAILS_TOOLTIP_48, 71 ] )
		self.InfoList.append( [ localeinfo.DETAILS_50, localeinfo.DETAILS_TOOLTIP_50, 72 ] )
		self.InfoList.append( [ localeinfo.DETAILS_51, localeinfo.DETAILS_TOOLTIP_51, 73 ] )
		self.InfoList.append( [ localeinfo.DETAILS_52, localeinfo.DETAILS_TOOLTIP_52, 74 ] )
		self.InfoList.append([localeinfo.NEW_DETAILS_10, localeinfo.NEW_DETAILS_10_TOOLTIP, 150])
		self.InfoList.append([localeinfo.NEW_DETAILS_11, localeinfo.NEW_DETAILS_11_TOOLTIP, 151])
		self.InfoList.append([localeinfo.NEW_DETAILS_12, localeinfo.NEW_DETAILS_12_TOOLTIP, 152])
		self.InfoList.append([localeinfo.NEW_DETAILS_13, localeinfo.NEW_DETAILS_13_TOOLTIP, 153])
		self.InfoList.append([localeinfo.NEW_DETAILS_14, localeinfo.NEW_DETAILS_14_TOOLTIP, 154])
		self.InfoList.append([localeinfo.NEW_DETAILS_15, localeinfo.NEW_DETAILS_15_TOOLTIP, 155])
		self.InfoList.append( [ localeinfo.DETAILS_76, localeinfo.DETAILS_TOOLTIP_76, 77 ] )
		self.InfoList.append( [ localeinfo.DETAILS_22, "", 136 ] )
		self.InfoList.append( [ localeinfo.DETAILS_23, "", 137 ] )
		self.InfoList.append( [ localeinfo.DETAILS_72, "", 98 ] )
		self.InfoList.append( [ localeinfo.DETAILS_15, localeinfo.DETAILS_TOOLTIP_15, 124 ] )
		self.InfoList.append( [ localeinfo.DETAILS_17, localeinfo.DETAILS_TOOLTIP_17, 123 ] )
		self.InfoList.append( [ localeinfo.DETAILS_56, localeinfo.DETAILS_TOOLTIP_56, 81 ] )
		self.InfoList.append( [ localeinfo.DETAILS_65, localeinfo.DETAILS_TOOLTIP_65, 79 ] )
		self.InfoList.append( [ localeinfo.DETAILS_63, localeinfo.DETAILS_TOOLTIP_63, 67 ] )
		self.InfoList.append( [ localeinfo.DETAILS_64, localeinfo.DETAILS_TOOLTIP_64, 68 ] )
		self.InfoList.append( [ "", "", self.CATEGORY_ENDLINE ] )
		
		self.InfoList.append( [ localeinfo.DETAILS_CATE_6, "", self.CATEGORY_STARTLINE ] )
		self.InfoList.append( [ localeinfo.DETAILS_61, "", 32 ] )
		self.InfoList.append( [ localeinfo.DETAILS_62, "", 33 ] )
		self.InfoList.append( [ localeinfo.DETAILS_59, "", 63 ] )
		self.InfoList.append( [ localeinfo.DETAILS_60, "", 64 ] )
		self.InfoList.append( [ localeinfo.DETAILS_66, "", 87 ] )
		self.InfoList.append( [ localeinfo.DETAILS_67, "", 82 ] )
		self.InfoList.append( [ "", "", self.CATEGORY_ENDLINE ] )
		
		self.InfoList.append( [ localeinfo.DETAILS_CATE_7, "", self.CATEGORY_STARTLINE ] )
		self.InfoList.append( [ localeinfo.DETAILS_70, localeinfo.DETAILS_TOOLTIP_70, 85 ] )
		self.InfoList.append( [ localeinfo.DETAILS_69, localeinfo.DETAILS_TOOLTIP_69, 84 ] )
		self.InfoList.append( [ localeinfo.DETAILS_68, localeinfo.DETAILS_TOOLTIP_68, 83 ] )
		# self.InfoList.append([localeinfo.NEW_DETAILS_2, localeinfo.NEW_DETAILS_2_TOOLTIP, 157])
		# self.InfoList.append([localeinfo.NEW_DETAILS_3, localeinfo.NEW_DETAILS_3_TOOLTIP, 105])
		# self.InfoList.append([localeinfo.NEW_DETAILS_4, localeinfo.NEW_DETAILS_4_TOOLTIP, 106])
		# self.InfoList.append([localeinfo.NEW_DETAILS_5, localeinfo.NEW_DETAILS_5_TOOLTIP, 158])
		self.InfoList.append( [ "", "", self.CATEGORY_ENDLINE ] )
		
		self.Diff = len(self.InfoList) - (self.UI_MAX_VIEW_COUNT )
		stepSize = 1.0 / self.Diff
		self.ScrollBar.SetScrollStep( stepSize )
		self.ScollPos = 0
		self.RefreshLabel()
		
	def Show(self):
		ui.ScriptWindow.Show(self)
		self.SetTop()
		
	def Close(self):
		self.Hide()
	
	def AdjustPosition(self, x, y):
		self.SetPosition(x + self.Width, y)

	def OnRunMouseWheel(self, nLen):
		if self.board and self.ScrollBar:
			isIn = False
			if self.board.IsIn():
				isIn = True
			
			if self.ScrollBar.IsIn():
				isIn = True
			
			if not isIn:
				for i in self.labelList:
					if i.IsIn():
						isIn = True
						break;
				
				if not isIn:
					for i in self.labelValueList:
						if i.IsIn():
							isIn = True
							break;
					
					if not isIn:
						for i in self.labelTextList:
							if i.IsIn():
								isIn = True
								break;
						
						if not isIn:
							for i in self.horizonBarList:
								if i.IsIn():
									isIn = True
									break;
							
							if not isIn:
								for i in self.horizonBarNameList:
									if i.IsIn():
										isIn = True
										break;
			
			if isIn:
				if nLen > 0:
					self.ScrollBar.OnUp2()
					return True
				else:
					self.ScrollBar.OnDown2()
					return True
		
		return False

	def OnScroll(self):
		self.RefreshLabel()

	def RefreshLabel(self):
		self.ScollPos = int(self.ScrollBar.GetPos() * self.Diff)
		self.LabelLineCount = 0
		self.startline_endlinecount = 0
		for i in xrange(self.UI_MAX_COUNT) :
			idx = i + self.ScollPos
			
			if idx < len(self.InfoList) :
				text = self.InfoList[idx][self.INFO_TEXT]
				type = self.InfoList[idx][self.INFO_VALUE]
				
				if type == self.CATEGORY_STARTLINE:
					self.__LabelTitleLine(i+self.LabelLineCount, text)
					self.startline_endlinecount += 1
				elif type == self.CATEGORY_ENDLINE:
					self.__EmptyLine(i+self.LabelLineCount)
					self.startline_endlinecount += 1
				else:
					value = player.GetStatus(type)
					self.__LabelLine(i+self.LabelLineCount, text, value)
					self.LabelLineCount += 1
			else:
				self.__EmptyLine(i+self.LabelLineCount)

	def __LabelTitleLine(self, idx, text):
		if(idx < self.UI_MAX_COUNT):
			self.labelList[idx].Hide()
			self.labelTextList[idx].Hide()
			self.horizonBarList[idx].Show()
			self.horizonBarNameList[idx].SetText( text )
				
	def __EmptyLine(self, idx):
		if(idx < self.UI_MAX_COUNT):
			self.labelList[idx].Hide()
			self.labelTextList[idx].Hide()
			self.horizonBarList[idx].Hide()
		
	def __LabelLine(self, idx, text, value):
		if(idx < self.UI_MAX_COUNT):
			self.labelTextList[idx].Show()
			self.horizonBarList[idx].Hide()
			self.labelList[idx].Hide()
		
			self.labelTextList[idx].SetText( text )
		
			if(idx+1 < self.UI_MAX_COUNT):
				self.labelList[idx+1].Show()
				self.horizonBarList[idx+1].Hide()
				self.labelTextList[idx+1].Hide()
				self.labelValueList[idx+1].SetText( str(value) )

	def OnTop(self):
		if self.uiCharacterStatus:
			self.uiCharacterStatus.SetTop()

	def OnMoveWindow(self, x, y):
		#print "OnMoveWindow x %s y %s" % (x, y)
		if self.uiCharacterStatus:
			self.uiCharacterStatus.AdjustPosition(x, y, self.Width)

