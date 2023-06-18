import app
import ui
import grp
import net
import guild
import messenger
import constinfo
import uitooltip
import uigameoption
import localeinfo

import uicommon
from _weakref import proxy
import wndMgr

FRIEND = 0
GUILD = 1

class MessengerItem(ui.Window):

	def __init__(self, getParentEvent):
		ui.Window.__init__(self)

		self.SetParent(getParentEvent())
		self.AddFlag("float")

		self.name = ""
		self.image = ui.ImageBox()
		self.image.AddFlag("not_pick")
		self.image.SetParent(self)
		self.image.Show()
		self.text = ui.TextLine()
		self.text.SetParent(self)
		self.text.SetPosition(20, 2)
		self.text.Show()

		self.lovePoint = -1
		self.lovePointToolTip = None

		self.isSelected = False

		self.getParentEvent = getParentEvent

	def SetName(self, name):
		self.name = name
		if name:
			self.text.SetText(name)
			self.SetSize(20 + 6*len(name) + 4, 16)

			if localeinfo.IsARABIC():
				self.text.SetPosition(20 + 6*len(name) + 4, 2)

	def SetLovePoint(self, lovePoint):
		self.lovePoint = lovePoint

	def Select(self):
		self.isSelected = True

	def UnSelect(self):
		self.isSelected = False

	def GetName(self):
		return self.name

	def GetStepWidth(self):
		return 0

	# Whisper
	def CanWhisper(self):
		return False

	def IsOnline(self):
		return False

	def IsMobile(self):
		return False

	def OnWhisper(self):
		pass

	def OnMobileMessage(self):
		pass

	# Remove
	def CanRemove(self):
		return False

	def OnRemove(self):
		return False

	# Warp
	def CanWarp(self):
		return False

	def OnWarp(self):
		pass

	def OnMouseOverIn(self):
		if -1 != self.lovePoint:
			if not self.lovePointToolTip:
				self.lovePointToolTip = uitooltip.ToolTip(100)
				self.lovePointToolTip.SetTitle(self.name)
				self.lovePointToolTip.AppendTextLine(localeinfo.AFF_LOVE_POINT % (self.lovePoint))
				self.lovePointToolTip.ResizeToolTip()
			self.lovePointToolTip.ShowToolTip()

	def OnMouseOverOut(self):
		if self.lovePointToolTip:
			self.lovePointToolTip.HideToolTip()

	def OnMouseLeftButtonDown(self):
		self.getParentEvent().OnSelectItem(self)

	def OnMouseLeftButtonDoubleClick(self):
		self.getParentEvent().OnDoubleClickItem(self)

	def OnRender(self):
		if self.isSelected:
			x, y = self.GetGlobalPosition()
			grp.SetColor(grp.GenerateColor(0.0, 0.0, 0.7, 0.7))
			grp.RenderBar(x+16, y, self.GetWidth()-16, self.GetHeight())

class MessengerMemberItem(MessengerItem):

	STATE_OFFLINE = 0
	STATE_ONLINE = 1
	STATE_MOBILE = 2

	IMAGE_FILE_NAME = {	"ONLINE" : "d:/ymir work/ui/game/windows/messenger_list_online.sub",
						"OFFLINE" : "d:/ymir work/ui/game/windows/messenger_list_offline.sub",
						"MOBILE" : "d:/ymir work/ui/game/windows/messenger_list_mobile.sub", }

	def __init__(self, getParentEvent):
		MessengerItem.__init__(self, getParentEvent)
		self.key = None
		self.state = self.STATE_OFFLINE
		self.mobileFlag = False
		self.Offline()

	def GetStepWidth(self):
		return 15

	def SetKey(self, key):
		self.key = key

	def IsSameKey(self, key):
		return self.key == key

	def IsOnline(self):
		if self.STATE_ONLINE == self.state:
			return True

		return False

	def IsMobile(self):
		if self.STATE_MOBILE == self.state:
			return True

		return False

	def Online(self):
		self.image.LoadImage(self.IMAGE_FILE_NAME["ONLINE"])
		self.state = self.STATE_ONLINE

	def Offline(self):
		if self.mobileFlag:
			self.image.LoadImage(self.IMAGE_FILE_NAME["MOBILE"])
			self.state = self.STATE_MOBILE

		else:
			self.image.LoadImage(self.IMAGE_FILE_NAME["OFFLINE"])
			self.state = self.STATE_OFFLINE

	def SetMobile(self, flag):
		self.mobileFlag = flag

		if not self.IsOnline():
			self.Offline()

	def CanWhisper(self):
		if self.IsOnline():
			return True

		return False

	def OnWhisper(self):
		if self.IsOnline():
			self.getParentEvent().whisperButtonEvent(self.GetName())

	def OnMobileMessage(self):
		if not uigameoption.MOBILE:
			return

		if not self.IsMobile():
			return

		self.getParentEvent().SendMobileMessage(self.GetName())

	def Select(self):
		MessengerItem.Select(self)

class MessengerGroupItem(MessengerItem):

	IMAGE_FILE_NAME = {	"OPEN" : "d:/ymir work/ui/game/windows/messenger_list_open.sub",
						"CLOSE" : "d:/ymir work/ui/game/windows/messenger_list_close.sub", }

	def __init__(self, getParentEvent):
		self.isOpen = False
		self.memberList = []
		if app.ENABLE_MESSENGER_TEAM:
			self.isTeam = False
		if app.ENABLE_MESSENGER_HELPER:
			self.isHelper = False
		
		MessengerItem.__init__(self, getParentEvent)

	def AppendMember(self, member, key, name):
		member.SetKey(key)
		member.SetName(name)
		self.memberList.append(member)
		return member

	def RemoveMember(self, item):
		for i in xrange(len(self.memberList)):
			if item == self.memberList[i]:
				del self.memberList[i]
				return

	def ClearMember(self):
		self.memberList = []

	def FindMember(self, key):
		list = filter(lambda argMember, argKey=key: argMember.IsSameKey(argKey), self.memberList)
		if list:
			return list[0]

		return None

	if app.ENABLE_MESSENGER_TEAM:
		def SetTeam(self):
			self.isTeam = True

	if app.ENABLE_MESSENGER_HELPER:
		def SetHelper(self):
			self.isHelper = True

	def GetLoginMemberList(self):
		if app.ENABLE_MESSENGER_HELPER:
			if self.isHelper:
				return filter(MessengerMemberItemHelper.IsOnline, self.memberList)
		
		if app.ENABLE_MESSENGER_TEAM:
			if self.isTeam:
				return filter(MessengerMemberItemTeam.IsOnline, self.memberList)
			else:
				return filter(MessengerMemberItem.IsOnline, self.memberList)
		else:
			return filter(MessengerMemberItem.IsOnline, self.memberList)

	def GetLogoutMemberList(self):
		return filter(lambda arg: not arg.IsOnline(), self.memberList)

	def IsOpen(self):
		return self.isOpen

	def Open(self):
		self.image.LoadImage(self.IMAGE_FILE_NAME["OPEN"])
		self.isOpen = True

	def Close(self):
		self.image.LoadImage(self.IMAGE_FILE_NAME["CLOSE"])
		self.isOpen = False

		map(ui.Window.Hide, self.memberList)

	def Select(self):

		if self.IsOpen():
			self.Close()
		else:
			self.Open()

		MessengerItem.Select(self)
		self.getParentEvent().OnRefreshList()

class MessengerFriendItem(MessengerMemberItem):

	def __init__(self, getParentEvent):
		MessengerMemberItem.__init__(self, getParentEvent)

	def CanRemove(self):
		return True

	def OnRemove(self):
		messenger.RemoveFriend(self.key)
		net.SendMessengerRemovePacket(self.key, self.name)
		return True

if app.ENABLE_MESSENGER_HELPER:
	class MessengerItemHelper(ui.Window):
		def __init__(self, getParentEvent):
			ui.Window.__init__(self)
			
			self.SetParent(getParentEvent())
			self.AddFlag("float")
			self.sizeFlag = False
			
			self.name = ""
			self.image = ui.ImageBox()
			self.image.AddFlag("not_pick")
			self.image.SetParent(self)
			self.image.SetPosition(30, 2)
			self.image.Show()
			
			self.image2 = ui.ImageBox()
			self.image2.AddFlag("not_pick")
			self.image2.SetParent(self)
			self.image2.SetPosition(-14, 5)
			self.image2.LoadImage("d:/ymir work/ui/game/flag/en.tga")
			self.image2.Show()
			
			self.image3 = ui.ImageBox()
			self.image3.AddFlag("not_pick")
			self.image3.SetParent(self)
			self.image3.SetPosition(8, 5)
			self.image3.Show()
			
			self.text = ui.TextLine()
			self.text.SetParent(self)
			self.text.SetPosition(50, 2)
			self.text.Show()
			
			self.lovePoint = -1
			self.lovePointToolTip = None
			self.isSelected = False
			self.getParentEvent = getParentEvent

		def SetName(self, name):
			self.name = name
			if name:
				self.text.SetText(name)
				self.SetSize(20 + 6*len(name) + 4, 16)

		def SetLovePoint(self, lovePoint):
			self.lovePoint = lovePoint

		def Select(self):
			self.isSelected = True

		def UnSelect(self):
			self.isSelected = False

		def GetName(self):
			return self.name

		def GetStepWidth(self):
			return 0

		def CanWhisper(self):
			return False

		def IsOnline(self):
			return False

		def IsMobile(self):
			return False

		def OnWhisper(self):
			pass

		def OnMobileMessage(self):
			pass

		def CanRemove(self):
			return False

		def OnRemove(self):
			return False

		def CanWarp(self):
			return False

		def OnWarp(self):
			pass

		def OnMouseOverIn(self):
			if -1 != self.lovePoint:
				if not self.lovePointToolTip:
					self.lovePointToolTip = uitooltip.ToolTip(100)
					self.lovePointToolTip.SetTitle(self.name)
					self.lovePointToolTip.AppendTextLine(localeinfo.AFF_LOVE_POINT % (self.lovePoint))
					self.lovePointToolTip.ResizeToolTip()
				
				self.lovePointToolTip.ShowToolTip()

		def OnMouseOverOut(self):
			if self.lovePointToolTip:
				self.lovePointToolTip.HideToolTip()

		def OnMouseLeftButtonDown(self):
			self.getParentEvent().OnSelectItem(self)

		def OnMouseLeftButtonDoubleClick(self):
			self.getParentEvent().OnDoubleClickItem(self)

		def OnRender(self):
			if self.isSelected:
				x, y = self.GetGlobalPosition()
				grp.SetColor(grp.GenerateColor(0.0, 0.0, 0.7, 0.7))
				if self.sizeFlag:
					grp.RenderBar(x+16+6, y + 2, self.GetWidth()-16, self.GetHeight())
				else:
					grp.RenderBar(x+16+30, y + 2, self.GetWidth()-16, self.GetHeight())

	class MessengerMemberItemHelper(MessengerItemHelper):
		STATE_OFFLINE = 0
		STATE_ONLINE = 1
		STATE_MOBILE = 2
		
		IMAGE_FILE_NAME = {	"ONLINE" : "d:/ymir work/ui/game/windows/messenger_list_online.sub",
							"OFFLINE" : "d:/ymir work/ui/game/windows/messenger_list_offline.sub",
							"MOBILE" : "d:/ymir work/ui/game/windows/messenger_list_mobile.sub", }

		def __init__(self, getParentEvent):
			MessengerItemHelper.__init__(self, getParentEvent)
			self.key = None
			self.state = self.STATE_OFFLINE
			self.mobileFlag = False
			self.Offline()

		def GetStepWidth(self):
			return 15

		def SetKey(self, key):
			self.key = key

		def IsSameKey(self, key):
			return self.key == key

		def IsOnline(self):
			if self.STATE_ONLINE == self.state:
				return True
			
			return False

		def IsMobile(self):
			if self.STATE_MOBILE == self.state:
				return True
			
			return False

		def Online(self):
			self.image.LoadImage(self.IMAGE_FILE_NAME["ONLINE"])
			self.state = self.STATE_ONLINE

		def SetLang(self, lang):
			self.image3.LoadImage("d:/ymir work/ui/game/flag/%s.tga" % (lang))
			
			if lang == "EN" or lang == "en":
				self.image.SetPosition(6, 2)
				self.image2.SetPosition(-14, 5)
				self.text.SetPosition(26, 2)
				self.image3.Hide()
				self.sizeFlag = True
			else:
				self.image.SetPosition(30, 2)
				self.image2.SetPosition(-14, 5)
				self.text.SetPosition(50, 2)
				self.image3.Show()
				self.image2.Show()
				self.sizeFlag = False

		def Offline(self):
			if self.mobileFlag:
				self.image.LoadImage(self.IMAGE_FILE_NAME["MOBILE"])
				self.state = self.STATE_MOBILE
			else:
				self.image.LoadImage(self.IMAGE_FILE_NAME["OFFLINE"])
				self.state = self.STATE_OFFLINE

		def SetMobile(self, flag):
			self.mobileFlag = flag
			if not self.IsOnline():
				self.Offline()

		def CanWhisper(self):
			if self.IsOnline():
				return True
			
			return False

		def OnWhisper(self):
			if self.IsOnline():
				self.getParentEvent().whisperButtonEvent(self.GetName())

		def OnMobileMessage(self):
			if not uigameoption.MOBILE:
				return
			
			if not self.IsMobile():
				return
			
			self.getParentEvent().SendMobileMessage(self.GetName())

		def Select(self):
			MessengerItemHelper.Select(self)

	class MessengerHelperItem(MessengerMemberItemHelper):
		def __init__(self, getParentEvent):
			MessengerMemberItemHelper.__init__(self, getParentEvent)

		def CanRemove(self):
			return False

		def OnRemove(self):
			return False

if app.ENABLE_MESSENGER_TEAM:
	class MessengerItemTeam(ui.Window):
		def __init__(self, getParentEvent):
			ui.Window.__init__(self)
			
			self.SetParent(getParentEvent())
			self.AddFlag("float")
			self.sizeFlag = False
			
			self.name = ""
			self.image = ui.ImageBox()
			self.image.AddFlag("not_pick")
			self.image.SetParent(self)
			self.image.SetPosition(30, 2)
			self.image.Show()
			
			self.image2 = ui.ImageBox()
			self.image2.AddFlag("not_pick")
			self.image2.SetParent(self)
			self.image2.SetPosition(-14, 5)
			self.image2.LoadImage("d:/ymir work/ui/game/flag/en.tga")
			self.image2.Show()
			
			self.image3 = ui.ImageBox()
			self.image3.AddFlag("not_pick")
			self.image3.SetParent(self)
			self.image3.SetPosition(8, 5)
			self.image3.Show()
			
			self.text = ui.TextLine()
			self.text.SetParent(self)
			self.text.SetPosition(50, 2)
			self.text.Show()
			
			self.lovePoint = -1
			self.lovePointToolTip = None
			self.isSelected = False
			self.getParentEvent = getParentEvent

		def SetName(self, name):
			self.name = name
			if name:
				self.text.SetText(name)
				self.SetSize(20 + 6*len(name) + 4, 16)

		def SetLovePoint(self, lovePoint):
			self.lovePoint = lovePoint

		def Select(self):
			self.isSelected = True

		def UnSelect(self):
			self.isSelected = False

		def GetName(self):
			return self.name

		def GetStepWidth(self):
			return 0

		def CanWhisper(self):
			return False

		def IsOnline(self):
			return False

		def IsMobile(self):
			return False

		def OnWhisper(self):
			pass

		def OnMobileMessage(self):
			pass

		def CanRemove(self):
			return False

		def OnRemove(self):
			return False

		def CanWarp(self):
			return False

		def OnWarp(self):
			pass

		def OnMouseOverIn(self):
			if -1 != self.lovePoint:
				if not self.lovePointToolTip:
					self.lovePointToolTip = uitooltip.ToolTip(100)
					self.lovePointToolTip.SetTitle(self.name)
					self.lovePointToolTip.AppendTextLine(localeinfo.AFF_LOVE_POINT % (self.lovePoint))
					self.lovePointToolTip.ResizeToolTip()
				
				self.lovePointToolTip.ShowToolTip()

		def OnMouseOverOut(self):
			if self.lovePointToolTip:
				self.lovePointToolTip.HideToolTip()

		def OnMouseLeftButtonDown(self):
			self.getParentEvent().OnSelectItem(self)

		def OnMouseLeftButtonDoubleClick(self):
			self.getParentEvent().OnDoubleClickItem(self)

		def OnRender(self):
			if self.isSelected:
				x, y = self.GetGlobalPosition()
				grp.SetColor(grp.GenerateColor(0.0, 0.0, 0.7, 0.7))
				if self.sizeFlag:
					grp.RenderBar(x+16+6, y + 2, self.GetWidth()-16, self.GetHeight())
				else:
					grp.RenderBar(x+16+30, y + 2, self.GetWidth()-16, self.GetHeight())

	class MessengerMemberItemTeam(MessengerItemTeam):
		STATE_OFFLINE = 0
		STATE_ONLINE = 1
		STATE_MOBILE = 2
		
		IMAGE_FILE_NAME = {	"ONLINE" : "d:/ymir work/ui/game/windows/messenger_list_online.sub",
							"OFFLINE" : "d:/ymir work/ui/game/windows/messenger_list_offline.sub",
							"MOBILE" : "d:/ymir work/ui/game/windows/messenger_list_mobile.sub", }

		def __init__(self, getParentEvent):
			MessengerItemTeam.__init__(self, getParentEvent)
			self.key = None
			self.state = self.STATE_OFFLINE
			self.mobileFlag = False
			self.Offline()

		def GetStepWidth(self):
			return 15

		def SetKey(self, key):
			self.key = key

		def IsSameKey(self, key):
			return self.key == key

		def IsOnline(self):
			if self.STATE_ONLINE == self.state:
				return True
			
			return False

		def IsMobile(self):
			if self.STATE_MOBILE == self.state:
				return True
			
			return False

		def Online(self):
			self.image.LoadImage(self.IMAGE_FILE_NAME["ONLINE"])
			self.state = self.STATE_ONLINE

		def SetLang(self, lang):
			self.image3.LoadImage("d:/ymir work/ui/game/flag/%s.tga" % (lang))
			
			if lang == "EN" or lang == "en":
				self.image.SetPosition(6, 2)
				self.image2.SetPosition(-14, 5)
				self.text.SetPosition(26, 2)
				self.image3.Hide()
				self.sizeFlag = True
			else:
				self.image.SetPosition(30, 2)
				self.image2.SetPosition(-14, 5)
				self.text.SetPosition(50, 2)
				self.image3.Show()
				self.image2.Show()
				self.sizeFlag = False

		def Offline(self):
			if self.mobileFlag:
				self.image.LoadImage(self.IMAGE_FILE_NAME["MOBILE"])
				self.state = self.STATE_MOBILE
			else:
				self.image.LoadImage(self.IMAGE_FILE_NAME["OFFLINE"])
				self.state = self.STATE_OFFLINE

		def SetMobile(self, flag):
			self.mobileFlag = flag
			if not self.IsOnline():
				self.Offline()

		def CanWhisper(self):
			if self.IsOnline():
				return True
			
			return False

		def OnWhisper(self):
			if self.IsOnline():
				self.getParentEvent().whisperButtonEvent(self.GetName())

		def OnMobileMessage(self):
			if not uigameoption.MOBILE:
				return
			
			if not self.IsMobile():
				return
			
			self.getParentEvent().SendMobileMessage(self.GetName())

		def Select(self):
			MessengerItemTeam.Select(self)

	class MessengerTeamItem(MessengerMemberItemTeam):
		def __init__(self, getParentEvent):
			MessengerMemberItemTeam.__init__(self, getParentEvent)

		def CanRemove(self):
			return False

		def OnRemove(self):
			return False

class MessengerGuildItem(MessengerMemberItem):

	def __init__(self, getParentEvent):
		MessengerMemberItem.__init__(self, getParentEvent)

	def CanWarp(self):
		if not self.IsOnline():
			return False
		return True

	def OnWarp(self):
		net.SendGuildUseSkillPacket(155, self.key)

	def CanRemove(self):
		for i in xrange(guild.ENEMY_GUILD_SLOT_MAX_COUNT):
			if guild.GetEnemyGuildName(i) != "":
				return False

		if guild.MainPlayerHasAuthority(guild.AUTH_REMOVE_MEMBER):
			if guild.IsMemberByName(self.name):
				return True

		return False

	def OnRemove(self):
		net.SendGuildRemoveMemberPacket(self.key)
		return True

class MessengerFriendGroup(MessengerGroupItem):

	def __init__(self, getParentEvent):
		MessengerGroupItem.__init__(self, getParentEvent)
		self.SetName(localeinfo.MESSENGER_FRIEND)

	def AppendMember(self, key, name):
		item = MessengerFriendItem(self.getParentEvent)
		return MessengerGroupItem.AppendMember(self, item, key, name)

if app.ENABLE_MESSENGER_HELPER:
	class MessengerHelperGroup(MessengerGroupItem):
		def __init__(self, getParentEvent):
			MessengerGroupItem.__init__(self, getParentEvent)
			self.SetName("Helpers")
			self.SetHelper()

		def AppendMember(self, key, name):
			item = MessengerHelperItem(self.getParentEvent)
			return MessengerGroupItem.AppendMember(self, item, key, name)

if app.ENABLE_MESSENGER_TEAM:
	class MessengerTeamGroup(MessengerGroupItem):
		def __init__(self, getParentEvent):
			MessengerGroupItem.__init__(self, getParentEvent)
			self.SetName("Team")
			self.SetTeam()

		def AppendMember(self, key, name):
			item = MessengerTeamItem(self.getParentEvent)
			return MessengerGroupItem.AppendMember(self, item, key, name)

class MessengerGuildGroup(MessengerGroupItem):

	def __init__(self, getParentEvent):
		MessengerGroupItem.__init__(self, getParentEvent)
		self.SetName(localeinfo.MESSENGER_GUILD)
		self.AddFlag("float")

	def AppendMember(self, key, name):
		item = MessengerGuildItem(self.getParentEvent)
		return MessengerGroupItem.AppendMember(self, item, key, name)

class MessengerFamilyGroup(MessengerGroupItem):

	def __init__(self, getParentEvent):
		MessengerGroupItem.__init__(self, getParentEvent)
		self.SetName(localeinfo.MESSENGER_FAMILY)
		self.AddFlag("float")

		self.lover = None

	def AppendMember(self, key, name):
		item = MessengerGuildItem(self.getParentEvent)
		self.lover = item
		return MessengerGroupItem.AppendMember(self, item, key, name)

	def GetLover(self):
		return self.lover

###################################################################################################
###################################################################################################
###################################################################################################

class MessengerWindow(ui.ScriptWindow):

	START_POSITION = 40

	class ResizeButton(ui.DragButton):

		def OnMouseOverIn(self):
			app.SetCursor(app.VSIZE)

		def OnMouseOverOut(self):
			app.SetCursor(app.NORMAL)

	def __init__(self):
		ui.ScriptWindow.__init__(self)
		messenger.SetMessengerHandler(self)
		self.scrollBar = None
		self.board = None
		self.groupList = []
		self.showingItemList = []
		self.selectedItem = None
		self.whisperButtonEvent = lambda *arg: None
		self.familyGroup = None

		self.guildButtonEvent = None

		self.showingPageSize = 0
		self.startLine = 0
		self.hasMobilePhoneNumber = True

		self.isLoaded = 0
		if app.ENABLE_MULTI_LANGUAGE:
			self.listLangName = {}
		
		self.__AddGroup()
		messenger.RefreshGuildMember()

	def Show(self):
		if self.isLoaded==0:
			self.isLoaded=1

			self.__LoadWindow()
			self.OnRefreshList()
			self.OnResizeDialog()

		ui.ScriptWindow.Show(self)

	def __LoadWindow(self):

		pyScrLoader = ui.PythonScriptLoader()
		pyScrLoader.LoadScriptFile(self, "uiscript/messengerwindow.py")

		try:
			self.board = self.GetChild("board")
			self.scrollBar = self.GetChild("ScrollBar")
			self.whisperButton = self.GetChild("WhisperButton")
			self.mobileButton = self.GetChild("MobileButton")
			self.removeButton = self.GetChild("RemoveButton")
			self.addFriendButton = self.GetChild("AddFriendButton")
			self.guildButton = self.GetChild("GuildButton")
		except:
			import exception
			exception.Abort("MessengerWindow.__LoadWindow.__Bind")

		self.board.SetCloseEvent(ui.__mem_func__(self.Close))
		self.scrollBar.SetScrollEvent(ui.__mem_func__(self.OnScroll))
		self.whisperButton.SetEvent(ui.__mem_func__(self.OnPressWhisperButton))
		self.mobileButton.SetEvent(ui.__mem_func__(self.OnPressMobileButton))
		self.removeButton.SetEvent(ui.__mem_func__(self.OnPressRemoveButton))
		self.addFriendButton.SetEvent(ui.__mem_func__(self.OnPressAddFriendButton))
		self.guildButton.SetEvent(ui.__mem_func__(self.OnPressGuildButton))

		if not uigameoption.MOBILE: #constinfo.SEND_MOBILE_PHONE_MESSAGE_ENABLE:
			self.mobileButton.Hide()
			width = self.GetWidth()
			height = self.GetHeight()
			self.addFriendButton.SetPosition(-60, 30)
			self.whisperButton.SetPosition(-20, 30)
			self.removeButton.SetPosition(20, 30)
			self.guildButton.SetPosition(60, 30)

		self.whisperButton.Disable()
		self.mobileButton.Disable()
		self.removeButton.Disable()

		resizeButton = self.ResizeButton()
		resizeButton.AddFlag("restrict_x")
		resizeButton.SetParent(self)
		resizeButton.SetSize(self.GetWidth(), 10)
		resizeButton.SetWindowVerticalAlignBottom()
		resizeButton.SetPosition(0, 0)
		resizeButton.Show()
		self.resizeButton = resizeButton
		self.resizeButton.SetMoveEvent(ui.__mem_func__(self.OnResizeDialog))
		self.resizeButton.SetPosition(0, 300)

		for list in self.groupList:
			list.SetTop()

	def OnRunMouseWheel(self, nLen):
		if self.scrollBar == None:
			return False
		
		if not self.scrollBar.IsShow():
			return False
		
		x, y = self.GetGlobalPosition()
		xMouse, yMouse = wndMgr.GetMousePosition()
		difX = xMouse - x
		difY = yMouse - y
		if difY > 0 and difY <= self.GetHeight() and difX > 0 and difX <= 161:
			if nLen > 0:
				self.scrollBar.OnUp()
			else:
				self.scrollBar.OnDown()
			
			return True
		
		return False

	def __del__(self):
		messenger.SetMessengerHandler(None)
		ui.ScriptWindow.__del__(self)

	def Destroy(self):
		self.board = None
		self.scrollBar = None
		self.resizeButton = None
		self.friendNameBoard = None
		self.questionDialog = None
		self.popupDialog = None
		self.inputDialog = None
		self.familyGroup = None

		self.whisperButton = None
		self.mobileButton = None
		self.removeButton = None
		if app.ENABLE_MULTI_LANGUAGE:
			self.listLangName = {}

	def OnCloseQuestionDialog(self):
		self.questionDialog.Close()
		self.questionDialog = None
		return True

	def Close(self):
		self.questionDialog = None
		self.Hide()

	def SetSize(self, width, height):
		ui.ScriptWindow.SetSize(self, width, height)
		if self.board:
			self.board.SetSize(width, height)

	def OnResizeDialog(self):
		x, y = self.resizeButton.GetLocalPosition()
		if y < 140:
			self.resizeButton.SetPosition(x, 140)
			return
		self.SetSize(self.GetWidth(), y + self.resizeButton.GetHeight())

		self.showingPageSize = y - (self.START_POSITION + 26)
		self.scrollBar.SetScrollBarSize(self.showingPageSize)

		self.__LocateMember()

		self.resizeButton.TurnOffCallBack()
		self.UpdateRect()
		self.resizeButton.TurnOnCallBack()

	def __LocateMember(self):

		if self.isLoaded==0:
			return

		if self.showingPageSize/20 >= len(self.showingItemList):
			self.scrollBar.Hide()
			self.startLine = 0
		else:
			if self.showingItemList:
				self.scrollBar.SetMiddleBarSize(float(self.showingPageSize/20) / float(len(self.showingItemList)))
			self.scrollBar.Show()

		#####

		yPos = self.START_POSITION
		heightLimit = self.GetHeight() - (self.START_POSITION + 13)

		map(ui.Window.Hide, self.showingItemList)

		for item in self.showingItemList[self.startLine:]:
			item.SetPosition(20 + item.GetStepWidth(), yPos)
			item.SetTop()
			item.Show()

			yPos += 20
			if yPos > heightLimit:
				break

	def __AddGroup(self):
		member = MessengerFriendGroup(ui.__mem_func__(self.GetSelf))
		member.Open()
		member.Show()
		self.groupList.append(member)

		member = MessengerGuildGroup(ui.__mem_func__(self.GetSelf))
		member.Open()
		member.Show()
		self.groupList.append(member)

		if app.ENABLE_MESSENGER_TEAM:
			member = MessengerTeamGroup(ui.__mem_func__(self.GetSelf))
			member.Open()
			member.Show()
			self.groupList.append(member)

		if app.ENABLE_MESSENGER_HELPER:
			member = MessengerHelperGroup(ui.__mem_func__(self.GetSelf))
			member.Open()
			member.Show()
			self.groupList.append(member)

	def __AddFamilyGroup(self):
		member = MessengerFamilyGroup(ui.__mem_func__(self.GetSelf))
		member.Open()
		member.Show()

		self.familyGroup = member

	def ClearGuildMember(self):
		self.groupList[GUILD].ClearMember()

	def SetWhisperButtonEvent(self, event):
		self.whisperButtonEvent=event

	def SetGuildButtonEvent(self, event):
		self.guildButtonEvent=event

	def SendMobileMessage(self, name):
		if not uigameoption.MOBILE:
			return

		if not self.hasMobilePhoneNumber:
			questionDialog = uicommon.QuestionDialog2()
			questionDialog.SetText1(localeinfo.MESSENGER_INPUT_MOBILE_PHONE_NUMBER_1)
			questionDialog.SetText2(localeinfo.MESSENGER_INPUT_MOBILE_PHONE_NUMBER_2)
			questionDialog.SetAcceptEvent(ui.__mem_func__(self.OnAcceptInputMobilePhoneNumber))
			questionDialog.SetCancelEvent(ui.__mem_func__(self.OnCancelInputMobilePhoneNumber))
			questionDialog.SetWidth(400)
			questionDialog.Open()
			self.questionDialog = questionDialog
			return

		## Input Sending Mobile Message
		inputDialog = uicommon.InputDialog()
		inputDialog.SetTitle(localeinfo.MESSENGER_SEND_MOBILE_MESSAGE_TITLE)
		inputDialog.SetMaxLength(50)
		inputDialog.SetAcceptEvent(ui.__mem_func__(self.OnInputMobileMessage))
		inputDialog.SetCancelEvent(ui.__mem_func__(self.OnCloseInputDialog))
		inputDialog.name = name
		inputDialog.Open()
		self.inputDialog = inputDialog

	def OnAcceptInputMobilePhoneNumber(self):
		if not uigameoption.MOBILE:
			return

		## Input Mobile Phone Number
		inputDialog = uicommon.InputDialog()
		inputDialog.SetTitle(localeinfo.MESSENGER_INPUT_MOBILE_PHONE_NUMBER_TITLE)
		inputDialog.SetMaxLength(13)
		inputDialog.SetAcceptEvent(ui.__mem_func__(self.OnInputMobilePhoneNumber))
		inputDialog.SetCancelEvent(ui.__mem_func__(self.OnCloseInputDialog))
		inputDialog.Open()
		self.inputDialog = inputDialog
		self.OnCancelInputMobilePhoneNumber()

	def OnCancelInputMobilePhoneNumber(self):
		if not uigameoption.MOBILE:
			return
		self.questionDialog.Close()
		self.questionDialog = None
		return True

	def OnInputMobilePhoneNumber(self):
		if not uigameoption.MOBILE:
			return

		text = self.inputDialog.GetText()

		if not text:
			return

		text.replace('-', '')
		net.SendChatPacket("/mobile " + text)
		self.OnCloseInputDialog()
		return True

	def OnInputMobileMessage(self):
		if not uigameoption.MOBILE:
			return

		text = self.inputDialog.GetText()

		if not text:
			return

		net.SendMobileMessagePacket(self.inputDialog.name, text)
		self.OnCloseInputDialog()
		return True

	def OnCloseInputDialog(self):
		self.inputDialog.Close()
		self.inputDialog = None
		return True

	def OnPressGuildButton(self):
		self.guildButtonEvent()

	def OnPressAddFriendButton(self):
		friendNameBoard = uicommon.InputDialog()
		friendNameBoard.SetTitle(localeinfo.MESSENGER_ADD_FRIEND)
		friendNameBoard.SetAcceptEvent(ui.__mem_func__(self.OnAddFriend))
		friendNameBoard.SetCancelEvent(ui.__mem_func__(self.OnCancelAddFriend))
		friendNameBoard.Open()
		self.friendNameBoard = friendNameBoard

	def OnAddFriend(self):
		text = self.friendNameBoard.GetText()
		if text:
			net.SendMessengerAddByNamePacket(text)
		self.friendNameBoard.Close()
		self.friendNameBoard = None
		return True

	def OnCancelAddFriend(self):
		self.friendNameBoard.Close()
		self.friendNameBoard = None
		return True

	def OnPressWhisperButton(self):
		if self.selectedItem:
			self.selectedItem.OnWhisper()

	def OnPressMobileButton(self):
		if self.selectedItem:
			self.selectedItem.OnMobileMessage()

	def OnPressRemoveButton(self):
		if self.selectedItem:
			if self.selectedItem.CanRemove():
				self.questionDialog = uicommon.QuestionDialog()
				self.questionDialog.SetText(localeinfo.MESSENGER_DO_YOU_DELETE)
				self.questionDialog.SetAcceptEvent(ui.__mem_func__(self.OnRemove))
				self.questionDialog.SetCancelEvent(ui.__mem_func__(self.OnCloseQuestionDialog))
				self.questionDialog.Open()

	def OnRemove(self):
		if self.selectedItem:
			if self.selectedItem.CanRemove():
				map(lambda arg, argDeletingItem=self.selectedItem: arg.RemoveMember(argDeletingItem), self.groupList)
				self.selectedItem.OnRemove()
				self.selectedItem.UnSelect()
				self.selectedItem = None
				self.OnRefreshList()

		self.OnCloseQuestionDialog()

	def OnScroll(self):
		scrollLineCount = len(self.showingItemList) - (self.showingPageSize/20)
		startLine = int(scrollLineCount * self.scrollBar.GetPos())

		if startLine != self.startLine:
			self.startLine = startLine
			self.__LocateMember()

	def OnPressEscapeKey(self):
		self.Close()
		return True

	## CallBack
	def OnSelectItem(self, item):

		if self.selectedItem:
			if item != self.selectedItem:
				self.selectedItem.UnSelect()

		self.selectedItem = item

		if self.selectedItem:
			self.selectedItem.Select()

			if self.selectedItem.CanWhisper():
				self.whisperButton.Enable()
			else:
				self.whisperButton.Disable()

			if self.selectedItem.IsMobile():
				self.mobileButton.Enable()
			else:
				self.mobileButton.Disable()

			if self.selectedItem.CanRemove():
				self.removeButton.Enable()
			else:
				self.removeButton.Disable()

	def OnDoubleClickItem(self, item):

		if not self.selectedItem:
			return

		if self.selectedItem.IsOnline():
			self.OnPressWhisperButton()

		elif self.selectedItem.IsMobile():
			self.OnPressMobileButton()

	def GetSelf(self):
		return self

	def OnRefreshList(self):
		self.showingItemList = []

		if self.familyGroup:
			self.showingItemList.append(self.familyGroup)
			if self.familyGroup.GetLover():
				self.showingItemList.append(self.familyGroup.GetLover())

		for group in self.groupList:

			self.showingItemList.append(group)

			if group.IsOpen():

				loginMemberList = group.GetLoginMemberList()
				logoutMemberList = group.GetLogoutMemberList()

				if loginMemberList or logoutMemberList:
					for member in loginMemberList:
						self.showingItemList.append(member)
					for member in logoutMemberList:
						self.showingItemList.append(member)

				else:
					item = MessengerItem(ui.__mem_func__(self.GetSelf))
					item.SetName(localeinfo.MESSENGER_EMPTY_LIST)
					self.showingItemList.append(item)

		self.__LocateMember()

	def RefreshMessenger(self):
		self.OnRefreshList()

	## EventHandler
	if app.ENABLE_MULTI_LANGUAGE:
		def GetTeamLang(self, name):
			if not self.listLangName.has_key(name):
				return "none"
			
			return str(self.listLangName[name])

		def AppendLanguage(self, name, lang):
			self.listLangName[name] = lang

	def __AddList(self, groupIndex, key, name):
		group = self.groupList[groupIndex]
		member = group.FindMember(key)
		if not member:
			member = group.AppendMember(key, name)
			self.OnSelectItem(None)
		
		if app.ENABLE_MESSENGER_TEAM and app.ENABLE_MULTI_LANGUAGE:
			if groupIndex == 2:
				j = name
				if not j:
					j = key
				
				lang = self.GetTeamLang(j)
				if lang != "none":
					member.SetLang(lang)
		
		if app.ENABLE_MESSENGER_HELPER and app.ENABLE_MULTI_LANGUAGE:
			if groupIndex == 3:
				j = name
				if not j:
					j = key
				
				lang = self.GetTeamLang(j)
				if lang != "none":
					member.SetLang(lang)
		
		return member

	def OnRemoveList(self, groupIndex, key):
		group = self.groupList[groupIndex]
		group.RemoveMember(group.FindMember(key))
		self.OnRefreshList()

	def OnRemoveAllList(self, groupIndex):
		group = self.groupList[groupIndex]
		group.ClearMember()
		self.OnRefreshList()

	def OnLogin(self, groupIndex, key, name=None):
		if not name:
			name = key
		group = self.groupList[groupIndex]
		member = self.__AddList(groupIndex, key, name)
		member.SetName(name)
		member.Online()
		self.OnRefreshList()

	def OnLogout(self, groupIndex, key, name=None):
		group = self.groupList[groupIndex]
		member = self.__AddList(groupIndex, key, name)
		if not name:
			name = key
		member.SetName(name)
		member.Offline()
		self.OnRefreshList()

	def OnMobile(self, groupIndex, key, mobileFlag):
		group = self.groupList[groupIndex]
		member = group.FindMember(key)
		if not member:
			return
		member.SetMobile(mobileFlag)
		self.OnRefreshList()

	def OnAddLover(self, name, lovePoint):
		if not self.familyGroup:
			self.__AddFamilyGroup()

		member = self.familyGroup.AppendMember(0, name)

		member.SetName(name)
		member.SetLovePoint(lovePoint)
		member.Offline()
		self.OnRefreshList()

	def OnUpdateLovePoint(self, lovePoint):
		if not self.familyGroup:
			return

		lover = self.familyGroup.GetLover()
		if not lover:
			return

		lover.SetLovePoint(lovePoint)

	def OnLoginLover(self):
		if not self.familyGroup:
			return

		lover = self.familyGroup.GetLover()
		if not lover:
			return

		lover.Online()

	def OnLogoutLover(self):
		if not self.familyGroup:
			return

		lover = self.familyGroup.GetLover()
		if not lover:
			return

		lover.Offline()

	def ClearLoverInfo(self):
		if not self.familyGroup:
			return

		self.familyGroup.ClearMember()
		self.familyGroup = None
		self.OnRefreshList()
