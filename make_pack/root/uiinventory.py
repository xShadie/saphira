import ui
import player
import mousemodule
import net
import app
import snd
import item
import player
import chat
import grp
import uiscriptlocale
import uirefine
import uiattachmetin
import uipickmoney
import uicommon
import uiprivateshopbuilder # ���λ��� ������ ItemMove ����
import constinfo
import ime
import wndMgr
import game
import localeinfo

if app.__ENABLE_NEW_OFFLINESHOP__:
	import offlineshop
	import uiofflineshop
if app.ENABLE_CAPITALE_SYSTEM:
	import uitooltip
	
if app.ENABLE_ACCE_SYSTEM:
	import acce

ITEM_MALL_BUTTON_ENABLE = True



ITEM_FLAG_APPLICABLE = 1 << 14

ENABLE_NEW_EQUIPMENT_SYSTEM = 0

class BeltInventoryWindow(ui.ScriptWindow):

	def __init__(self, wndInventory):
		import exception

		if not ENABLE_NEW_EQUIPMENT_SYSTEM:
			exception.Abort("What do you do?")
			return

		if not wndInventory:
			exception.Abort("wndInventory parameter must be set to InventoryWindow")
			return

		ui.ScriptWindow.__init__(self)

		self.isLoaded = 0
		self.wndInventory = wndInventory;

		self.wndBeltInventoryLayer = None
		self.wndBeltInventorySlot = None
		self.expandBtn = None
		self.minBtn = None

		self.__LoadWindow()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def Show(self, openBeltSlot = False):
		self.__LoadWindow()
		self.RefreshSlot()

		ui.ScriptWindow.Show(self)

		if openBeltSlot:
			self.OpenInventory()
		else:
			self.CloseInventory()
		
		self.SetTop()

	def Close(self):
		self.Hide()

	def IsOpeningInventory(self):
		return self.wndBeltInventoryLayer.IsShow()

	def OpenInventory(self):
		self.wndBeltInventoryLayer.Show()
		self.expandBtn.Hide()

	def CloseInventory(self):
		self.wndBeltInventoryLayer.Hide()
		self.expandBtn.Show()

	def GetBasePosition(self):
		x, y = self.wndInventory.GetGlobalPosition()
		return x - 148, y + 241

	def AdjustPositionAndSize(self):
		bx, by = self.GetBasePosition()

		if self.IsOpeningInventory():
			self.SetPosition(bx + 20, by)
			self.SetSize(self.ORIGINAL_WIDTH, self.GetHeight())

		else:
			self.SetPosition(bx + 138 + 20, by);
			self.SetSize(10, self.GetHeight())

	def __LoadWindow(self):
		if self.isLoaded == 1:
			return

		self.isLoaded = 1

		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "uiscript/beltinventorywindow.py")
		except:
			import exception
			exception.Abort("BeltWindow.LoadWindow.LoadObject")

		try:
			self.ORIGINAL_WIDTH = self.GetWidth()
			wndBeltInventorySlot = self.GetChild("BeltInventorySlot")
			self.wndBeltInventoryLayer = self.GetChild("BeltInventoryLayer")
			self.expandBtn = self.GetChild("ExpandBtn")
			self.minBtn = self.GetChild("MinimizeBtn")

			self.expandBtn.SetEvent(ui.__mem_func__(self.OpenInventory))
			self.minBtn.SetEvent(ui.__mem_func__(self.CloseInventory))

			if localeinfo.IsARABIC() :
				self.expandBtn.SetPosition(self.expandBtn.GetWidth() - 2, 15)
				self.wndBeltInventoryLayer.SetPosition(self.wndBeltInventoryLayer.GetWidth() - 5, 0)
				self.minBtn.SetPosition(self.minBtn.GetWidth() + 3, 15)

			for i in xrange(item.BELT_INVENTORY_SLOT_COUNT):
				slotNumber = item.BELT_INVENTORY_SLOT_START + i
				wndBeltInventorySlot.SetCoverButton(slotNumber,	"d:/ymir work/ui/game/quest/slot_button_01.sub",\
												"d:/ymir work/ui/game/quest/slot_button_01.sub",\
												"d:/ymir work/ui/game/quest/slot_button_01.sub",\
												"d:/ymir work/ui/game/belt_inventory/slot_disabled.tga", False, False)

		except:
			import exception
			exception.Abort("BeltWindow.LoadWindow.BindObject")

		## Equipment
		wndBeltInventorySlot.SetOverInItemEvent(ui.__mem_func__(self.wndInventory.OverInItem))
		wndBeltInventorySlot.SetOverOutItemEvent(ui.__mem_func__(self.wndInventory.OverOutItem))
		wndBeltInventorySlot.SetUnselectItemSlotEvent(ui.__mem_func__(self.wndInventory.UseItemSlot))
		wndBeltInventorySlot.SetUseSlotEvent(ui.__mem_func__(self.wndInventory.UseItemSlot))
		wndBeltInventorySlot.SetSelectEmptySlotEvent(ui.__mem_func__(self.wndInventory.SelectEmptySlot))
		wndBeltInventorySlot.SetSelectItemSlotEvent(ui.__mem_func__(self.wndInventory.SelectItemSlot))

		self.wndBeltInventorySlot = wndBeltInventorySlot

	def RefreshSlot(self):
		getItemVNum=player.GetItemIndex

		for i in xrange(item.BELT_INVENTORY_SLOT_COUNT):
			slotNumber = item.BELT_INVENTORY_SLOT_START + i
			self.wndBeltInventorySlot.SetItemSlot(slotNumber, getItemVNum(slotNumber), player.GetItemCount(slotNumber))
			self.wndBeltInventorySlot.SetAlwaysRenderCoverButton(slotNumber, True)

			avail = "0"

			if player.IsAvailableBeltInventoryCell(slotNumber):
				self.wndBeltInventorySlot.EnableCoverButton(slotNumber)
			else:
				self.wndBeltInventorySlot.DisableCoverButton(slotNumber)

		self.wndBeltInventorySlot.RefreshSlot()

class MenuWindow(ui.ScriptWindow):
	BUTTONS_COUNT = 6
	
	def Func(self, arg):
		if not self.inventory:
			return
		
		self.inventory.SetTop()
		if arg == 0:
			self.inventory.OnPressBtnTeleport()
		elif arg == 1:
			net.SendBattlePassAction(1)
		elif arg == 2:
			self.inventory.OpenSwitchbot()
		elif arg == 3:
			import event
			event.QuestButtonClick(int(constinfo.guildRankQuest))
		elif arg == 4:
			self.inventory.OpenRanking()
		elif arg == 5:
			self.inventory.OpenExtraInventory()

	def __init__(self, inventory):
		ui.ScriptWindow.__init__(self)
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "uiscript/inventory_menu.py")
		except:
			import exception
			exception.Abort("InventoryMenuWindow.LoadWindow.LoadObject")
		
		self.board = self.GetChild("board")
		self.buttons = []
		for i in xrange(self.BUTTONS_COUNT):
			self.buttons.append(self.GetChild("button%d" % (i + 1)))
			self.buttons[i].SetEvent(lambda arg = i : self.Func(arg))
		self.width = self.board.GetWidth()
		self.height = self.board.GetHeight()
		self.inventory = inventory
		self.AdjustPosition()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def Destroy(self):
		self.ClearDictionary()
		self.board = None
		self.buttons = []

	def Show(self):
		ui.ScriptWindow.Show(self)

	def Hide(self):
		ui.ScriptWindow.Hide(self)

	def AdjustPosition(self):
		(x, y) = self.inventory.GetGlobalPosition()
		self.SetPosition(x - self.width + 4, y + 32);

	def OnMoveWindow(self):
		(x, y) = self.GetGlobalPosition()
		self.inventory.AdjustPosition(x + self.width - 4, y - 32)

class InventoryWindow(ui.ScriptWindow):
	SLOTNAMES = {
					187 : localeinfo.SLOTNAME_10,
					188 : localeinfo.SLOTNAME_11,
					199 : localeinfo.SLOTNAME_1,
					200 : localeinfo.SLOTNAME_2,
					207 : localeinfo.SLOTNAME_3,
					202 : localeinfo.SLOTNAME_4,
					204 : localeinfo.SLOTNAME_5,
					206 : localeinfo.SLOTNAME_6,
					201 : localeinfo.SLOTNAME_7,
					208 : localeinfo.SLOTNAME_8,
					209 : localeinfo.SLOTNAME_9,
	}
	
	USE_TYPE_TUPLE = ("USE_CLEAN_SOCKET", "USE_CHANGE_ATTRIBUTE", "USE_ADD_ATTRIBUTE", "USE_ADD_ATTRIBUTE2", "USE_ADD_ACCESSORY_SOCKET", "USE_PUT_INTO_ACCESSORY_SOCKET", "USE_PUT_INTO_BELT_SOCKET", "USE_PUT_INTO_RING_SOCKET", "USE_CHANGE_COSTUME_ATTR", "USE_RESET_COSTUME_ATTR")
	if app.ENABLE_ATTR_COSTUMES:
		removeAttrDialog = None
	
	if app.ENABLE_NEW_PET_EDITS:
		enchantPetDialog = None
	
	questionDialog = None
	tooltipItem = None
	wndBelt = None
	dlgPickMoney = None
	interface = None
	if app.WJ_ENABLE_TRADABLE_ICON:
		bindWnds = []
	
	sellingSlotNumber = -1
	isLoaded = 0
	wndmenu = None
	isOpenedBeltWindowWhenClosingInventory = 0		# �κ��丮 ���� �� ��Ʈ �κ��丮�� �����־����� ����-_-; ���̹� ����

	def OpenRanking(self):
		if self.interface:
			self.interface.OpenRanking()

	def OpenSwitchbot(self):
		if self.interface:
			self.interface.ToggleSwitchbotWindow()

	def OnPressBtnTeleport(self):
		if self.interface:
			self.interface.ToggleMapTeleporter()

	def AdjustPosition(self, x, y):
		self.SetPosition(x, y)

	def __init__(self):
		ui.ScriptWindow.__init__(self)

		self.isOpenedBeltWindowWhenClosingInventory = 0		# �κ��丮 ���� �� ��Ʈ �κ��丮�� �����־����� ����-_-; ���̹� ����

		self.inventoryPageIndex = 0
		self.__LoadWindow()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def Show(self):
		self.__LoadWindow()

		ui.ScriptWindow.Show(self)
		if self.wndBelt:
			self.wndBelt.Show(self.isOpenedBeltWindowWhenClosingInventory)
		
		if self.wndmenu:
			self.wndmenu.Show()

	if app.WJ_ENABLE_TRADABLE_ICON:
		def BindWindow(self, wnd):
			self.bindWnds.append(wnd)

	def BindInterfaceClass(self, interface):
		self.interface = interface

	def __LoadWindow(self):
		if self.isLoaded == 1:
			return

		self.isLoaded = 1

		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "uiscript/inventorywindowex.py")
		except:
			import exception
			exception.Abort("InventoryWindow.LoadWindow.LoadObject")

		try:
			self.bkImg = self.GetChild("Equipment_Base")
			wndItem = self.GetChild("ItemSlot")
			wndEquip = self.GetChild("EquipmentSlot")
			wndCostume = self.GetChild("CostumeSlot")
			if app.ENABLE_HIDE_COSTUME_SYSTEM:
				self.visibleButtonList = []
				self.visibleButtonList.append(self.GetChild("BodyToolTipButton"))
				self.visibleButtonList.append(self.GetChild("HairToolTipButton"))
				self.visibleButtonList.append(self.GetChild("AcceToolTipButton"))
				self.visibleButtonList.append(self.GetChild("WeaponToolTipButton"))
			
			self.GetChild("TitleBar").SetCloseEvent(ui.__mem_func__(self.Close))
			if app.ENABLE_CAPITALE_SYSTEM:
				self.wndMoneySlot = None
				self.wndMoney = None
			else:
				self.wndMoney = self.GetChild("Money")
				self.wndMoneySlot = self.GetChild("Money_Slot")
			if app.__ENABLE_EXTEND_INVEN_SYSTEM__:
				self.EX_INVEN_COVER_IMG_CLOSE = []
				self.EX_INVEN_COVER_IMG_OPEN = []
				for i in xrange(9):
					self.EX_INVEN_COVER_IMG_OPEN.append(self.GetChild("cover_open_" + str(i)))
					self.EX_INVEN_COVER_IMG_CLOSE.append(self.GetChild("cover_close_" + str(i)))
			
			# self.mallButton = self.GetChild2("MallButton")
			self.RuneButton = self.GetChild2("Rune")
			self.DSSButton = self.GetChild2("DSSButton")
			if app.ENABLE_EXTRA_INVENTORY:
				self.ExtendIntentory = self.GetChild2("ExtendIntentory")
				self.ExtendIntentory.SetEvent(self.OpenExtraInventory)
			self.costumeButton = self.GetChild2("CostumeButton")
			#if app.ENABLE_SORT_INVEN:
			#	self.separateButton = self.GetChild("SeparateButton")
			#	self.separateButton.SetEvent(self.Sort_Inventory)
			#	self.separateButton.Hide()

			self.inventoryTab = []
			for i in xrange(player.INVENTORY_PAGE_COUNT):
				self.inventoryTab.append(self.GetChild("Inventory_Tab_%02d" % (i+1)))

			self.equipmentTab = []
			self.equipmentTab.append(self.GetChild("Equipment_Tab_01"))
			self.equipmentTab.append(self.GetChild("Equipment_Tab_02"))

			if self.costumeButton and not app.ENABLE_COSTUME_SYSTEM:
				self.costumeButton.Hide()
				self.costumeButton.Destroy()
				self.costumeButton = 0

			if app.__ENABLE_EXTEND_INVEN_SYSTEM__:
				for i in xrange(9):
					self.EX_INVEN_COVER_IMG_CLOSE[i].Hide()
					self.EX_INVEN_COVER_IMG_OPEN[i].Hide()

			# Belt Inventory Window
			self.wndBelt = None
			if ENABLE_NEW_EQUIPMENT_SYSTEM:
				self.wndBelt = BeltInventoryWindow(self)
		except:
			import exception
			exception.Abort("InventoryWindow.LoadWindow.BindObject")

		## Item
		wndItem.SetSelectEmptySlotEvent(ui.__mem_func__(self.SelectEmptySlot))
		wndItem.SetSelectItemSlotEvent(ui.__mem_func__(self.SelectItemSlot))
		wndItem.SetUnselectItemSlotEvent(ui.__mem_func__(self.UseItemSlot))
		wndItem.SetUseSlotEvent(ui.__mem_func__(self.UseItemSlot))
		wndItem.SetOverInItemEvent(ui.__mem_func__(self.OverInItem))
		wndItem.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))
		wndItem.SetOverInEmptySlotEvent(ui.__mem_func__(self.OverOutItem))
		wndItem.SetOverOutEmptySlotEvent(ui.__mem_func__(self.OverOutItem))
		
		## Equipment
		wndEquip.SetSelectEmptySlotEvent(ui.__mem_func__(self.SelectEmptySlot))
		wndEquip.SetSelectItemSlotEvent(ui.__mem_func__(self.SelectItemSlot))
		wndEquip.SetUnselectItemSlotEvent(ui.__mem_func__(self.UseItemSlot))
		wndEquip.SetUseSlotEvent(ui.__mem_func__(self.UseItemSlot))
		wndEquip.SetOverInItemEvent(ui.__mem_func__(self.OverInItem))
		wndEquip.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))
		wndEquip.SetOverInEmptySlotEvent(ui.__mem_func__(self.OverInEmptySlot))
		wndEquip.SetOverOutEmptySlotEvent(ui.__mem_func__(self.OverOutEmptySlot))
		
		## Costume
		wndCostume.SetSelectEmptySlotEvent(ui.__mem_func__(self.SelectEmptySlot))
		wndCostume.SetSelectItemSlotEvent(ui.__mem_func__(self.SelectItemSlot))
		wndCostume.SetUnselectItemSlotEvent(ui.__mem_func__(self.UseItemSlot))
		wndCostume.SetUseSlotEvent(ui.__mem_func__(self.UseItemSlot))
		wndCostume.SetOverInItemEvent(ui.__mem_func__(self.OverInItem))
		wndCostume.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))
		wndCostume.SetOverInEmptySlotEvent(ui.__mem_func__(self.OverInEmptySlot))
		wndCostume.SetOverOutEmptySlotEvent(ui.__mem_func__(self.OverOutEmptySlot))

		## Hide Costume
		if app.ENABLE_HIDE_COSTUME_SYSTEM:
			self.visibleButtonList[0].SetToggleUpEvent(ui.__mem_func__(self.VisibleCostume), 1, 0)
			self.visibleButtonList[1].SetToggleUpEvent(ui.__mem_func__(self.VisibleCostume), 2, 0)
			if app.ENABLE_ACCE_SYSTEM:
				self.visibleButtonList[2].SetToggleUpEvent(ui.__mem_func__(self.VisibleCostume), 3, 0)
			
			if app.ENABLE_WEAPON_COSTUME_SYSTEM:
				self.visibleButtonList[3].SetToggleUpEvent(ui.__mem_func__(self.VisibleCostume), 4, 0)
			
			self.visibleButtonList[0].SetToggleDownEvent(ui.__mem_func__(self.VisibleCostume), 1, 1)
			self.visibleButtonList[1].SetToggleDownEvent(ui.__mem_func__(self.VisibleCostume), 2, 1)
			if app.ENABLE_ACCE_SYSTEM:
				self.visibleButtonList[2].SetToggleDownEvent(ui.__mem_func__(self.VisibleCostume), 3, 1)
			
			if app.ENABLE_WEAPON_COSTUME_SYSTEM:
				self.visibleButtonList[3].SetToggleDownEvent(ui.__mem_func__(self.VisibleCostume), 4, 1)
		
		## PickMoneyDialog
		dlgPickMoney = uipickmoney.PickMoneyDialog()
		dlgPickMoney.LoadDialog()
		dlgPickMoney.Hide()
		
		## RefineDialog
		self.refineDialog = uirefine.RefineDialog()
		self.refineDialog.Hide()

		## AttachMetinDialog
		if app.WJ_ENABLE_TRADABLE_ICON:  
			self.attachMetinDialog = uiattachmetin.AttachMetinDialog(self)
			self.BindWindow(self.attachMetinDialog)
		else:
			self.attachMetinDialog = uiattachmetin.AttachMetinDialog()
		self.attachMetinDialog.Hide()

		## MoneySlot
		if not app.ENABLE_CAPITALE_SYSTEM:
			self.wndMoneySlot.SetEvent(ui.__mem_func__(self.OpenPickMoneyDialog))

		for i in xrange(player.INVENTORY_PAGE_COUNT):
			self.inventoryTab[i].SetEvent(lambda arg=i: self.SetInventoryPage(arg))
		self.inventoryTab[0].Down()

		self.equipmentTab[0].SetEvent(lambda arg=0: self.SetEquipmentPage(arg))
		self.equipmentTab[1].SetEvent(lambda arg=1: self.SetEquipmentPage(arg))
		self.equipmentTab[0].Down()
		if not app.ENABLE_STOLE_REAL:
			for item in self.equipmentTab:
				item.Hide()

		self.wndItem = wndItem
		self.wndEquip = wndEquip
		self.wndCostume = wndCostume
		if not app.ENABLE_STOLE_REAL:
			self.wndCostume.Hide()
		
		self.dlgPickMoney = dlgPickMoney
		if app.__ENABLE_EXTEND_INVEN_SYSTEM__:
			for i in xrange(9):
				self.EX_INVEN_COVER_IMG_OPEN[i].SetEvent(ui.__mem_func__(self.Env_key))

		# MallButton
		# if self.mallButton:
			# self.mallButton.SetEvent(ui.__mem_func__(self.ClickMallButton))

		if self.DSSButton:
			self.DSSButton.SetEvent(ui.__mem_func__(self.ClickDSSButton))
		
		if self.RuneButton:
			if app.ENABLE_RUNE_SYSTEM:
				self.RuneButton.SetEvent(ui.__mem_func__(self.ClickRuneButton))
			else:
				self.RuneButton.Hide()
		
		if app.ENABLE_EXTRA_INVENTORY:
			if self.ExtendIntentory:
				self.ExtendIntentory.SetEvent(ui.__mem_func__(self.OpenExtraInventory))

		if app.ENABLE_HIGHLIGHT_SYSTEM:
			self.listHighlightedSlot = []

		if app.ENABLE_ACCE_SYSTEM or app.ENABLE_STOLE_COSTUME:
			self.listAttachedAcces = []
		
		if app.ENABLE_ATTR_TRANSFER_SYSTEM:
			self.listAttachedTransfer = []
			for i in xrange(3):
				self.listAttachedTransfer.append(999)
		
		self.wndmenu = MenuWindow(self)
		
		## Refresh
		self.SetInventoryPage(0)
		self.SetEquipmentPage(0)
		self.RefreshItemSlot()
		self.RefreshStatus()

	def Destroy(self):
		self.ClearDictionary()

		self.dlgPickMoney.Destroy()
		self.dlgPickMoney = 0
		self.refineDialog.Destroy()
		self.refineDialog = 0

		self.attachMetinDialog.Destroy()
		self.attachMetinDialog = 0
		self.visibleButtonList = []
		self.tooltipItem = None
		self.wndItem = 0
		self.wndEquip = 0
		self.wndCostume = 0
		self.dlgPickMoney = 0
		self.wndMoney = 0
		self.wndMoneySlot = 0
		self.questionDialog = None
		# self.mallButton = None
		#if app.ENABLE_SORT_INVEN:
		#	self.separateButton = None
		self.DSSButton = None
		self.RuneButton = None
		if app.__ENABLE_EXTEND_INVEN_SYSTEM__:
			self.EX_INVEN_COVER_IMG_CLOSE = None
			self.EX_INVEN_COVER_IMG_OPEN = None
		self.DSSButton = None		
		self.interface = None
		if app.WJ_ENABLE_TRADABLE_ICON:
			self.bindWnds = []
		
		if self.wndBelt:
			self.wndBelt.Destroy()
			self.wndBelt = None
		
		if self.wndmenu:
			self.wndmenu.Destroy()
			self.wndmenu = None
		
		self.inventoryTab = []
		self.equipmentTab = []
		if app.ENABLE_ATTR_TRANSFER_SYSTEM:
			self.listAttachedTransfer = []

	def Hide(self):
		if self.wndmenu != None:
			self.wndmenu.Hide()
		
		if constinfo.GET_ITEM_QUESTION_DIALOG_STATUS():
			self.OnCloseQuestionDialog()
			return
		if None != self.tooltipItem:
			self.tooltipItem.HideToolTip()
		if self.wndBelt:
			self.isOpenedBeltWindowWhenClosingInventory = self.wndBelt.IsOpeningInventory()		# �κ��丮 â�� ���� �� ��Ʈ �κ��丮�� ���� �־��°�?
			print "Is Opening Belt Inven?? ", self.isOpenedBeltWindowWhenClosingInventory
			self.wndBelt.Close()

		if self.dlgPickMoney:
			self.dlgPickMoney.Close()

		wndMgr.Hide(self.hWnd)


	def Close(self):
		self.Hide()

	if app.ENABLE_HIGHLIGHT_SYSTEM:
		def HighlightSlot(self, slot):
			if not slot in self.listHighlightedSlot:
				self.listHighlightedSlot.append(slot)

	if app.__ENABLE_EXTEND_INVEN_SYSTEM__:
		def UpdateInven(self):
			page = self.inventoryPageIndex
			for i in xrange(9):
				inv_plus = player.GetEnvanter() + i
				inv_pluss = player.GetEnvanter() - i
				if page == 2:
					if player.GetEnvanter() > 8:
						self.EX_INVEN_COVER_IMG_OPEN[i].Hide()
						self.EX_INVEN_COVER_IMG_CLOSE[i].Hide()
					else:
						self.EX_INVEN_COVER_IMG_OPEN[player.GetEnvanter()].Show()
						self.EX_INVEN_COVER_IMG_CLOSE[player.GetEnvanter()].Hide()
						if inv_pluss >= 0:
							self.EX_INVEN_COVER_IMG_OPEN[inv_pluss].Hide()
							self.EX_INVEN_COVER_IMG_CLOSE[inv_pluss].Hide()
						if inv_plus < 9:
							self.EX_INVEN_COVER_IMG_CLOSE[inv_plus].Show()
							self.EX_INVEN_COVER_IMG_OPEN[inv_plus].Hide()	
				elif page == 3:
					if player.GetEnvanter() < 9:	
						self.EX_INVEN_COVER_IMG_OPEN[i].Hide()
						self.EX_INVEN_COVER_IMG_CLOSE[i].Show()
					elif player.GetEnvanter() > 17:
						self.EX_INVEN_COVER_IMG_OPEN[i].Hide()
						self.EX_INVEN_COVER_IMG_CLOSE[i].Hide()
					else:
						self.EX_INVEN_COVER_IMG_OPEN[player.GetEnvanter()-9].Show()
						self.EX_INVEN_COVER_IMG_CLOSE[player.GetEnvanter()-9].Hide()
						if inv_pluss >= 0:
							self.EX_INVEN_COVER_IMG_OPEN[inv_pluss-9].Hide()
						if inv_plus < 18:
							self.EX_INVEN_COVER_IMG_CLOSE[inv_plus-9].Show()
				else:
					self.EX_INVEN_COVER_IMG_OPEN[i].Hide()
					self.EX_INVEN_COVER_IMG_CLOSE[i].Hide()
					
		def Expansion_env(self):
			net.Envanter_expansion()
			self.OnCloseQuestionDialog()
			
		def Env_key(self):
			if player.GetEnvanter() < 18:
				needkeys = (2,2,2,2,3,3,4,4,4,5,5,5,6,6,6,7,7,7)
				self.questionDialog = uicommon.QuestionDialog()
				self.questionDialog.SetText(localeinfo.ENVANTER_EXPANS_1 % needkeys[player.GetEnvanter()])
				self.questionDialog.SetAcceptEvent(ui.__mem_func__(self.Expansion_env))
				self.questionDialog.SetCancelEvent(ui.__mem_func__(self.OnCloseQuestionDialog))
				self.questionDialog.Open()

	def SetInventoryPage(self, page):
		self.inventoryPageIndex = page
		for i in xrange(player.INVENTORY_PAGE_COUNT):
			if i!=page:
				self.inventoryTab[i].SetUp()
		
		if app.__ENABLE_EXTEND_INVEN_SYSTEM__:
			self.UpdateInven()
		
		if app.ENABLE_HIGHLIGHT_SYSTEM:
			for i in xrange(player.INVENTORY_PAGE_SIZE):
				slotNumber = self.__InventoryLocalSlotPosToGlobalSlotPos(i)
				itemCount = player.GetItemCount(slotNumber)
				if 0 != itemCount:
					itemVnum = player.GetItemIndex(slotNumber)
					if self.wndItem.IsActiveSlot(i):
						self.wndItem.DeactivateSlot(i)
		
		self.RefreshBagSlotWindow()

	if app.ENABLE_HIDE_COSTUME_SYSTEM:
		def RefreshVisibleCostume(self):
			body = constinfo.HIDDEN_BODY_COSTUME
			hair = constinfo.HIDDEN_HAIR_COSTUME
			if app.ENABLE_ACCE_COSTUME_SYSTEM:
				acce = constinfo.HIDDEN_ACCE_COSTUME
			if app.ENABLE_WEAPON_COSTUME_SYSTEM:
				weapon = constinfo.HIDDEN_WEAPON_COSTUME

			if body == 1:
				self.visibleButtonList[0].SetToolTipText(localeinfo.SHOW_COSTUME)
				self.visibleButtonList[0].Down()
			else:
				self.visibleButtonList[0].SetToolTipText(localeinfo.HIDE_COSTUME)
				self.visibleButtonList[0].SetUp()

			if hair == 1:
				self.visibleButtonList[1].SetToolTipText(localeinfo.SHOW_COSTUME)
				self.visibleButtonList[1].Down()
			else:
				self.visibleButtonList[1].SetToolTipText(localeinfo.HIDE_COSTUME)
				self.visibleButtonList[1].SetUp()

			if app.ENABLE_ACCE_COSTUME_SYSTEM:
				if acce == 1:
					self.visibleButtonList[2].SetToolTipText(localeinfo.SHOW_COSTUME)
					self.visibleButtonList[2].Down()
				else:
					self.visibleButtonList[2].SetToolTipText(localeinfo.HIDE_COSTUME)
					self.visibleButtonList[2].SetUp()

			if app.ENABLE_WEAPON_COSTUME_SYSTEM:
				if weapon == 1:
					self.visibleButtonList[3].SetToolTipText(localeinfo.SHOW_COSTUME)
					self.visibleButtonList[3].Down()
				else:
					self.visibleButtonList[3].SetToolTipText(localeinfo.HIDE_COSTUME)
					self.visibleButtonList[3].SetUp()

		def VisibleCostume(self, part, hidden):
			net.SendChatPacket("/hide_costume %d %d" % (part, hidden))

	def SetEquipmentPage(self, page):
		self.equipmentPageIndex = page
		self.equipmentTab[1-page].SetUp()
		if app.ENABLE_STOLE_REAL:
			if self.bkImg:
				if page == 0:
					self.wndEquip.Show()
					self.wndCostume.Hide()
					self.bkImg.LoadImage("d:/ymir work/ui/equipment_bg_with_talisman.tga")
					self.DSSButton.Show()
					self.RuneButton.Show()
					self.ExtendIntentory.Show()
					for item in self.visibleButtonList:
						item.Hide()
				else:
					self.wndEquip.Hide()
					self.wndCostume.Show()
					self.bkImg.LoadImage("d:/ymir work/ui/costumi_bg.tga")
					self.DSSButton.Hide()
					self.RuneButton.Hide()
					self.ExtendIntentory.Hide()
					for item in self.visibleButtonList:
						item.Show()
		
		self.RefreshEquipSlotWindow()

	# def ClickMallButton(self):
		# print "click_mall_button"
		# net.SendChatPacket("/click_mall")
		
	if app.ENABLE_EXTRA_INVENTORY:
		def OpenExtraInventory(self):
			self.interface.ToggleExtraInventoryWindow()

	if app.ENABLE_SORT_INVEN:
		def Sort_Inventory(self):
			#prevent offlineshop invalid position
			if app.__ENABLE_NEW_OFFLINESHOP__:
				if uiofflineshop.IsBuildingShop() or uiofflineshop.IsBuildingAuction():
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.OFFLINESHOP_CANT_SELECT_ITEM_DURING_BUILING)
					return
			
			net.SendChatPacket("/item_check")

		def Sort_InventoryDone(self):
			if app.ENABLE_HIGHLIGHT_SYSTEM:
				del self.listHighlightedSlot[:]
			if app.ENABLE_ACCE_SYSTEM or app.ENABLE_STOLE_COSTUME:
				del self.listAttachedAcces[:]
			self.RefreshItemSlot()

	if app.ENABLE_RUNE_SYSTEM:
		def ClickRuneButton(self):
			if self.interface:
				self.interface.Rune()

	# DSSButton
	def ClickDSSButton(self):
		print "click_dss_button"
		self.interface.ToggleDragonSoulWindow()

	def OpenPickMoneyDialog(self):

		if mousemodule.mouseController.isAttached():

			attachedSlotPos = mousemodule.mouseController.GetAttachedSlotNumber()
			if player.SLOT_TYPE_SAFEBOX == mousemodule.mouseController.GetAttachedType():

				if player.ITEM_MONEY == mousemodule.mouseController.GetAttachedItemIndex():
					net.SendSafeboxWithdrawMoneyPacket(mousemodule.mouseController.GetAttachedItemCount())
					snd.PlaySound("sound/ui/money.wav")

			mousemodule.mouseController.DeattachObject()

		else:
			curMoney = player.GetElk()

			if curMoney <= 0:
				return

			self.dlgPickMoney.SetTitleName(localeinfo.PICK_MONEY_TITLE)
			self.dlgPickMoney.SetAcceptEvent(ui.__mem_func__(self.OnPickMoney))
			self.dlgPickMoney.Open(curMoney)
			self.dlgPickMoney.SetMax(9) # �κ��丮 990000 ���� ���� ����

	def OnPickMoney(self, money):
		mousemodule.mouseController.AttachMoney(self, player.SLOT_TYPE_INVENTORY, money)

	def OnPickItem(self, count):
		itemSlotIndex = self.dlgPickMoney.itemGlobalSlotIndex
		if app.__ENABLE_NEW_OFFLINESHOP__:
			if uiofflineshop.IsBuildingShop() and uiofflineshop.IsSaleSlot(player.INVENTORY, itemSlotIndex):
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.OFFLINESHOP_CANT_SELECT_ITEM_DURING_BUILING)
				return
		
		division = self.dlgPickMoney.division
		if division:
			net.SendItemDivisionPacket(itemSlotIndex, player.INVENTORY)
		else:
			selectedItemVNum = player.GetItemIndex(player.INVENTORY, itemSlotIndex)
			mousemodule.mouseController.AttachObject(self, player.SLOT_TYPE_INVENTORY, itemSlotIndex, selectedItemVNum, count)

	def __InventoryLocalSlotPosToGlobalSlotPos(self, local):
		if player.IsEquipmentSlot(local) or player.IsCostumeSlot(local) or (ENABLE_NEW_EQUIPMENT_SYSTEM and player.IsBeltInventorySlot(local)):
			return local

		return self.inventoryPageIndex*player.INVENTORY_PAGE_SIZE + local

	def GetInventoryPageIndex(self):
		return self.inventoryPageIndex

	if app.ENABLE_ATTR_TRANSFER_SYSTEM:
		def TransferSlot(self, main, target):
			self.listAttachedTransfer[main] = target
			self.RefreshMarkSlots()
			self.RefreshBagSlotWindow()

		def ClearTransferSlot(self):
			for i in xrange(3):
				self.listAttachedTransfer[i] = 999
			
			self.RefreshMarkSlots()
			self.RefreshBagSlotWindow()

	if app.WJ_ENABLE_TRADABLE_ICON:
		def RefreshMarkSlots(self, localIndex=None):
			if not self.interface:
				return
			
			unusable = False
			onTopWnd = self.interface.GetOnTopWindow()
			if localIndex:
				slotNumber = self.__InventoryLocalSlotPosToGlobalSlotPos(localIndex)
				if onTopWnd == player.ON_TOP_WND_NONE:
					self.wndItem.SetUsableSlotOnTopWnd(localIndex)
				elif onTopWnd == player.ON_TOP_WND_SHOP:
					if player.IsAntiFlagBySlot(slotNumber, item.ANTIFLAG_SELL):
						self.wndItem.SetUnusableSlotOnTopWnd(localIndex)
						unusable = True
					else:
						self.wndItem.SetUsableSlotOnTopWnd(localIndex)
				elif onTopWnd == player.ON_TOP_WND_EXCHANGE:
					if player.IsAntiFlagBySlot(slotNumber, item.ANTIFLAG_GIVE):
						self.wndItem.SetUnusableSlotOnTopWnd(localIndex)
						unusable = True
					else:
						self.wndItem.SetUsableSlotOnTopWnd(localIndex)
				elif onTopWnd == player.ON_TOP_WND_PRIVATE_SHOP:
					if player.IsAntiFlagBySlot(slotNumber, item.ITEM_ANTIFLAG_MYSHOP):
						self.wndItem.SetUnusableSlotOnTopWnd(localIndex)
						unusable = True
					else:
						self.wndItem.SetUsableSlotOnTopWnd(localIndex)
				elif onTopWnd == player.ON_TOP_WND_SAFEBOX:
					if player.IsAntiFlagBySlot(slotNumber, item.ANTIFLAG_SAFEBOX):
						self.wndItem.SetUnusableSlotOnTopWnd(localIndex)
						unusable = True
					else:
						self.wndItem.SetUsableSlotOnTopWnd(localIndex)
				
				if app.ENABLE_ACCE_SYSTEM and not unusable:
					self.acceSlot = []
					for r in xrange(acce.WINDOW_MAX_MATERIALS):
						(isHere, iCell) = acce.GetAttachedItem(r)
						if isHere:
							if iCell == slotNumber:
								if not slotNumber in self.acceSlot:
									self.acceSlot.append(slotNumber)
								
								self.wndItem.SetUnusableSlotOnTopWnd(localIndex)
								unusable = True
							else:
								for j in self.acceSlot:
									if j != slotNumber:
										self.wndItem.SetUsableSlotOnTopWnd(localIndex)
				
				if app.ENABLE_ATTR_TRANSFER_SYSTEM and not unusable:
					self.transferSlot = []
					for r in xrange(len(self.listAttachedTransfer)):
						idx = self.listAttachedTransfer[r]
						if idx != 999:
							if idx == slotNumber:
								if not slotNumber in self.acceSlot:
									self.transferSlot.append(slotNumber)
								
								self.wndItem.SetUnusableSlotOnTopWnd(localIndex)
								unusable = True
							else:
								for j in self.transferSlot:
									if j != slotNumber:
										self.wndItem.SetUsableSlotOnTopWnd(localIndex)
				
				return
			
			for i in xrange(player.INVENTORY_PAGE_SIZE):
				slotNumber = self.__InventoryLocalSlotPosToGlobalSlotPos(i)
				if onTopWnd == player.ON_TOP_WND_NONE:
					self.wndItem.SetUsableSlotOnTopWnd(i)
				elif onTopWnd == player.ON_TOP_WND_SHOP:
					if player.IsAntiFlagBySlot(slotNumber, item.ANTIFLAG_SELL):
						self.wndItem.SetUnusableSlotOnTopWnd(i)
						unusable = True
					else:
						self.wndItem.SetUsableSlotOnTopWnd(i)
				elif onTopWnd == player.ON_TOP_WND_EXCHANGE:
					if player.IsAntiFlagBySlot(slotNumber, item.ANTIFLAG_GIVE):
						self.wndItem.SetUnusableSlotOnTopWnd(i)
						unusable = True
					else:
						self.wndItem.SetUsableSlotOnTopWnd(i)
				elif onTopWnd == player.ON_TOP_WND_PRIVATE_SHOP:
					if player.IsAntiFlagBySlot(slotNumber, item.ITEM_ANTIFLAG_MYSHOP):
						self.wndItem.SetUnusableSlotOnTopWnd(i)
						unusable = True
					else:
						self.wndItem.SetUsableSlotOnTopWnd(i)
				elif onTopWnd == player.ON_TOP_WND_SAFEBOX:
					if player.IsAntiFlagBySlot(slotNumber, item.ANTIFLAG_SAFEBOX):
						self.wndItem.SetUnusableSlotOnTopWnd(i)
						unusable = True
					else:
						self.wndItem.SetUsableSlotOnTopWnd(i)
				
				if app.ENABLE_ACCE_SYSTEM and not unusable:
					self.acceSlot = []
					for r in xrange(acce.WINDOW_MAX_MATERIALS):
						(isHere, iCell) = acce.GetAttachedItem(r)
						if isHere:
							if iCell == slotNumber:
								if not slotNumber in self.acceSlot:
									self.acceSlot.append(slotNumber)
								
								self.wndItem.SetUnusableSlotOnTopWnd(i)
								unusable = True
							else:
								for j in self.acceSlot:
									if j != slotNumber:
										self.wndItem.SetUsableSlotOnTopWnd(i)
				
				if app.ENABLE_ATTR_TRANSFER_SYSTEM and not unusable:
					self.transferSlot = []
					for r in xrange(len(self.listAttachedTransfer)):
						idx = self.listAttachedTransfer[r]
						if idx != 999:
							if idx == slotNumber:
								if not slotNumber in self.transferSlot:
									self.transferSlot.append(idx)
								
								self.wndItem.SetUnusableSlotOnTopWnd(i)
								unusable = True
							else:
								for j in self.transferSlot:
									if j != slotNumber:
										self.wndItem.SetUsableSlotOnTopWnd(i)

	def RefreshBagSlotWindow(self):
		getItemVNum=player.GetItemIndex
		getItemCount=player.GetItemCount
		setItemVNum=self.wndItem.SetItemSlot
		for i in xrange(player.INVENTORY_PAGE_SIZE):
			slotNumber = self.__InventoryLocalSlotPosToGlobalSlotPos(i)
			itemCount = getItemCount(slotNumber)
			if 0 == itemCount:
				self.wndItem.ClearSlot(i)
				continue
			elif 1 == itemCount:
				itemCount = 0
			
			itemVnum = getItemVNum(slotNumber)
			if not itemVnum:
				continue
			
			setItemVNum(i, itemVnum, itemCount)
			
			if app.ENABLE_HIGHLIGHT_SYSTEM:
				wasActivated = False
				if slotNumber in self.listHighlightedSlot:
					self.wndItem.ActivateSlotEffect(i, (255.00 / 255.0), (20.00 / 255.0), (22.00 / 255.0), 1.0)
					wasActivated = True
				else:
					self.wndItem.DeactivateSlotEffect(i)
			
			if not wasActivated and app.ENABLE_ACCE_SYSTEM:
				slotNumberChecked = 0
				for j in xrange(acce.WINDOW_MAX_MATERIALS):
					(isHere, iCell) = acce.GetAttachedItem(j)
					if isHere:
						if iCell == slotNumber:
							if app.ENABLE_HIGHLIGHT_SYSTEM:
								self.wndItem.ActivateSlot(i)
							
							wasActivated = True
							if not slotNumber in self.listAttachedAcces:
								self.listAttachedAcces.append(slotNumber)
							
							slotNumberChecked = 1
							break
					else:
						if slotNumber in self.listAttachedAcces and not slotNumberChecked:
							self.listAttachedAcces.remove(slotNumber)
			
			if not wasActivated and app.ENABLE_ATTR_TRANSFER_SYSTEM:
				self.transferSlot = []
				slotNumberChecked = 0
				for r in xrange(len(self.listAttachedTransfer)):
					idx = self.listAttachedTransfer[r]
					if idx != 999:
						if idx == slotNumber:
							if app.ENABLE_HIGHLIGHT_SYSTEM:
								self.wndItem.ActivateSlot(i)
							
							wasActivated = True
							if not slotNumber in self.transferSlot:
								self.transferSlot.append(idx)
							
							slotNumberChecked = 1
							break
						else:
							if slotNumber in self.transferSlot and not slotNumberChecked:
								self.transferSlot.remove(slotNumber)
			
			if not wasActivated and constinfo.IS_AUTO_POTION(itemVnum):
				metinSocket = [player.GetItemMetinSocket(slotNumber, j) for j in xrange(player.METIN_SOCKET_MAX_NUM)]
				isActivated = 0 != metinSocket[0]
				if isActivated:
					if app.ENABLE_HIGHLIGHT_SYSTEM:
						self.wndItem.ActivateSlot(i)
					
					wasActivated = True
					
					potionType = 0;
					if constinfo.IS_AUTO_POTION_HP(itemVnum):
						potionType = player.AUTO_POTION_TYPE_HP
					elif constinfo.IS_AUTO_POTION_SP(itemVnum):
						potionType = player.AUTO_POTION_TYPE_SP
					
					usedAmount = int(metinSocket[1])
					totalAmount = int(metinSocket[2])
					player.SetAutoPotionInfo(potionType, isActivated, (totalAmount - usedAmount), totalAmount, self.__InventoryLocalSlotPosToGlobalSlotPos(i))
			
			if not wasActivated and app.ENABLE_HIGHLIGHT_SYSTEM and app.ENABLE_NEW_USE_POTION and player.GetItemTypeBySlot(slotNumber) == item.ITEM_TYPE_USE and player.GetItemSubTypeBySlot(slotNumber) == item.USE_NEW_POTIION:
				metinSocket = [player.GetItemMetinSocket(slotNumber, j) for j in xrange(player.METIN_SOCKET_MAX_NUM)]
				isActivated = 0 != metinSocket[1]
				if isActivated:
					self.wndItem.ActivateSlot(i)
					wasActivated = True
			
			if not wasActivated and app.ENABLE_HIGHLIGHT_SYSTEM and app.ENABLE_SOUL_SYSTEM and player.GetItemTypeBySlot(slotNumber) == item.ITEM_TYPE_SOUL:
				metinSocket = [player.GetItemMetinSocket(slotNumber, j) for j in xrange(player.METIN_SOCKET_MAX_NUM)]
				if 0 != metinSocket[1]:
					self.wndItem.ActivateSlot(i)
					wasActivated = True
			
			if not wasActivated and app.ENABLE_HIGHLIGHT_SYSTEM and app.NEW_PET_SYSTEM and constinfo.IS_PET_SEAL(itemVnum):
				metinSocket = [player.GetItemMetinSocket(slotNumber, j) for j in xrange(player.METIN_SOCKET_MAX_NUM)]
				isActivated = 0 != metinSocket[0]
				if isActivated:
					self.wndItem.ActivateSlot(i)
					wasActivated = True
			
			if not wasActivated and app.ENABLE_HIGHLIGHT_SYSTEM and constinfo.IS_PET_SEAL_OLD(itemVnum):
				metinSocket = [player.GetItemMetinSocket(slotNumber, j) for j in xrange(player.METIN_SOCKET_MAX_NUM)]
				isActivated = 0 != metinSocket[2]
				if isActivated:
					self.wndItem.ActivateSlot(i)
					wasActivated = True
			
			if not wasActivated and app.ENABLE_HIGHLIGHT_SYSTEM:
				self.wndItem.DeactivateSlot(i)
			
			if app.WJ_ENABLE_TRADABLE_ICON:
				self.RefreshMarkSlots(i)
		
		self.wndItem.RefreshSlot()
		if self.wndBelt:
			self.wndBelt.RefreshSlot()
		
		if app.WJ_ENABLE_TRADABLE_ICON:
			map(lambda wnd:wnd.RefreshLockedSlot(), self.bindWnds)

	def RefreshEquipSlotWindow(self):
		getItemVNum=player.GetItemIndex
		getItemCount=player.GetItemCount
		setItemVNum=self.wndEquip.SetItemSlot
		if app.ENABLE_SOUL_SYSTEM and self.equipmentPageIndex == 1:
			for i in xrange(item.COSTUME_SLOT_COUNT):
				slotNumber = item.COSTUME_SLOT_START + i
				self.wndCostume.SetItemSlot(slotNumber, getItemVNum(slotNumber), 0)
			
			if app.ENABLE_WEAPON_COSTUME_SYSTEM:
				self.wndCostume.SetItemSlot(item.COSTUME_SLOT_WEAPON, getItemVNum(item.COSTUME_SLOT_WEAPON), 0)
			
			if app.ENABLE_COSTUME_PET:
				self.wndCostume.SetItemSlot(player.COSTUME_PETSKIN_SLOT, getItemVNum(player.COSTUME_PETSKIN_SLOT), 0)
			
			if app.ENABLE_COSTUME_MOUNT:
				self.wndCostume.SetItemSlot(player.COSTUME_MOUNTSKIN_SLOT, getItemVNum(player.COSTUME_MOUNTSKIN_SLOT), 0)
			
			if app.ENABLE_COSTUME_EFFECT:
				self.wndCostume.SetItemSlot(player.COSTUME_EFFECT_BODY_SLOT, getItemVNum(player.COSTUME_EFFECT_BODY_SLOT), 0)
				self.wndCostume.SetItemSlot(player.COSTUME_EFFECT_WEAPON_SLOT, getItemVNum(player.COSTUME_EFFECT_WEAPON_SLOT), 0)
		else:
			for i in xrange(player.EQUIPMENT_PAGE_COUNT):
				slotNumber = player.EQUIPMENT_SLOT_START + i
				itemCount = getItemCount(slotNumber)
				if itemCount <= 1:
					itemCount = 0
				
				setItemVNum(slotNumber, getItemVNum(slotNumber), itemCount)
		
			if app.ENABLE_NEW_EQUIPMENT_SYSTEM:
				for i in xrange(player.NEW_EQUIPMENT_SLOT_COUNT):
					slotNumber = player.NEW_EQUIPMENT_SLOT_START + i
					itemCount = getItemCount(slotNumber)
					if itemCount <= 1:
						itemCount = 0
					setItemVNum(slotNumber, getItemVNum(slotNumber), itemCount)
					print "ENABLE_NEW_EQUIPMENT_SYSTEM", slotNumber, itemCount, getItemVNum(slotNumber)
			
			if app.ENABLE_STOLE_REAL:
				slotNumber = item.EQUIPMENT_STOLE
				itemCount = getItemCount(slotNumber)
				if itemCount <= 1:
					itemCount = 0
				
				setItemVNum(slotNumber, getItemVNum(slotNumber), itemCount)
		
		self.wndEquip.RefreshSlot()

	def RefreshItemSlot(self):
		if app.ENABLE_HIGHLIGHT_SYSTEM:
			for i in xrange(player.INVENTORY_PAGE_SIZE):
				slotNumber = self.__InventoryLocalSlotPosToGlobalSlotPos(i)
				itemCount = player.GetItemCount(slotNumber)
				if 0 != itemCount:
					itemVnum = player.GetItemIndex(slotNumber)
					if self.wndItem.IsActiveSlot(i):
						self.wndItem.DeactivateSlot(i)
		
		self.RefreshBagSlotWindow()
		self.RefreshEquipSlotWindow()

	def RefreshStatus(self):
		if not app.ENABLE_CAPITALE_SYSTEM:
			money = player.GetElk()
			self.wndMoney.SetText(localeinfo.NumberToMoneyString(money))

	def SetItemToolTip(self, tooltipItem):
		self.tooltipItem = tooltipItem

	def SellItem(self):
		if app.__ENABLE_NEW_OFFLINESHOP__:
			if uiofflineshop.IsBuildingShop() and uiofflineshop.IsSaleSlot(player.INVENTORY, self.sellingSlotNumber):
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.OFFLINESHOP_CANT_SELECT_ITEM_DURING_BUILING)
				return

		if self.sellingSlotitemIndex == player.GetItemIndex(self.sellingSlotNumber):
			if self.sellingSlotitemCount == player.GetItemCount(self.sellingSlotNumber):
				## ��ȥ���� �ȸ��� �ϴ� ��� �߰��ϸ鼭 ���� type �߰�
				net.SendShopSellPacketNew(self.sellingSlotNumber, self.questionDialog.count, player.INVENTORY)
				snd.PlaySound("sound/ui/money.wav")
		self.OnCloseQuestionDialog()

	def OnDetachMetinFromItem(self):
		if None == self.questionDialog:
			return

		#net.SendItemUseToItemPacket(self.questionDialog.sourcePos, self.questionDialog.targetPos)
		self.__SendUseItemToItemPacket(self.questionDialog.sourcePos, self.questionDialog.targetPos)
		self.OnCloseQuestionDialog()

	def OnCloseQuestionDialog(self):
		if not self.questionDialog:
			return

		self.questionDialog.Close()
		self.questionDialog = None
		constinfo.SET_ITEM_QUESTION_DIALOG_STATUS(0)

	## Slot Event
	def SelectEmptySlot(self, selectedSlotPos):
		if constinfo.GET_ITEM_QUESTION_DIALOG_STATUS() == 1:
			return

		selectedSlotPos = self.__InventoryLocalSlotPosToGlobalSlotPos(selectedSlotPos)

		if mousemodule.mouseController.isAttached():

			attachedSlotType = mousemodule.mouseController.GetAttachedType()
			attachedSlotPos = mousemodule.mouseController.GetAttachedSlotNumber()
			attachedItemCount = mousemodule.mouseController.GetAttachedItemCount()
			attachedItemIndex = mousemodule.mouseController.GetAttachedItemIndex()
			if app.__ENABLE_NEW_OFFLINESHOP__:
				if uiofflineshop.IsBuildingShop() and uiofflineshop.IsSaleSlot(player.SlotTypeToInvenType(attachedSlotType),attachedSlotPos):
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.OFFLINESHOP_CANT_SELECT_ITEM_DURING_BUILING)
					return
			


			if player.SLOT_TYPE_INVENTORY == attachedSlotType:
				#@fixme011 BEGIN (block ds equip)
				attachedInvenType = player.SlotTypeToInvenType(attachedSlotType)
				if player.IsDSEquipmentSlot(attachedInvenType, attachedSlotPos):
					mousemodule.mouseController.DeattachObject()
					return
				#@fixme011 END

				itemCount = player.GetItemCount(attachedSlotPos)
				attachedCount = mousemodule.mouseController.GetAttachedItemCount()
				self.__SendMoveItemPacket(attachedSlotPos, selectedSlotPos, attachedCount)

				if item.IsRefineScroll(attachedItemIndex):
					self.wndItem.SetUseMode(False)

			elif player.SLOT_TYPE_PRIVATE_SHOP == attachedSlotType:
				mousemodule.mouseController.RunCallBack("INVENTORY")

			elif player.SLOT_TYPE_SHOP == attachedSlotType:
				net.SendShopBuyPacket(attachedSlotPos)
			elif app.ENABLE_SWITCHBOT and player.SLOT_TYPE_SWITCHBOT == attachedSlotType:
				attachedCount = mousemodule.mouseController.GetAttachedItemCount()
				net.SendItemMovePacket(player.SWITCHBOT, attachedSlotPos, player.INVENTORY, selectedSlotPos, attachedCount)
			elif player.SLOT_TYPE_SAFEBOX == attachedSlotType:

				if player.ITEM_MONEY == attachedItemIndex:
					net.SendSafeboxWithdrawMoneyPacket(mousemodule.mouseController.GetAttachedItemCount())
					snd.PlaySound("sound/ui/money.wav")

				else:
					net.SendSafeboxCheckoutPacket(attachedSlotPos, selectedSlotPos)

			elif player.SLOT_TYPE_MALL == attachedSlotType:
				net.SendMallCheckoutPacket(attachedSlotPos, selectedSlotPos)

			mousemodule.mouseController.DeattachObject()

	def SelectItemSlot(self, itemSlotIndex):
		if constinfo.GET_ITEM_QUESTION_DIALOG_STATUS() == 1:
			return

		itemSlotIndex = self.__InventoryLocalSlotPosToGlobalSlotPos(itemSlotIndex)

		if mousemodule.mouseController.isAttached():
			attachedSlotType = mousemodule.mouseController.GetAttachedType()
			attachedSlotPos = mousemodule.mouseController.GetAttachedSlotNumber()
			attachedItemVID = mousemodule.mouseController.GetAttachedItemIndex()

			if app.ENABLE_EXTRA_INVENTORY:
				if attachedSlotType == player.SLOT_TYPE_INVENTORY or attachedSlotType == player.SLOT_TYPE_EXTRA_INVENTORY:
					#@fixme011 BEGIN (block ds equip)
					attachedInvenType = player.SlotTypeToInvenType(attachedSlotType)
					if player.IsDSEquipmentSlot(attachedInvenType, attachedSlotPos):
						mousemodule.mouseController.DeattachObject()
						return
					#@fixme011 END
					
					if attachedItemVID == player.GetItemIndex(itemSlotIndex)  and attachedItemVID >= 70000 and attachedItemVID <= 99999:
						self.__SendMoveItemPacket(attachedSlotPos, itemSlotIndex, 0)
					else:
						self.__DropSrcItemToDestItemInInventory(attachedItemVID, attachedSlotPos, itemSlotIndex, attachedSlotType)
			else:
				if player.SLOT_TYPE_INVENTORY == attachedSlotType:
					#@fixme011 BEGIN (block ds equip)
					attachedInvenType = player.SlotTypeToInvenType(attachedSlotType)
					if player.IsDSEquipmentSlot(attachedInvenType, attachedSlotPos):
						mousemodule.mouseController.DeattachObject()
						return
					#@fixme011 END
					self.__DropSrcItemToDestItemInInventory(attachedItemVID, attachedSlotPos, itemSlotIndex)
			
			mousemodule.mouseController.DeattachObject()
		else:
			curCursorNum = app.GetCursor()
			if app.SELL == curCursorNum:
				self.__SellItem(itemSlotIndex)

			elif app.BUY == curCursorNum:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.SHOP_BUY_INFO)

			elif app.IsPressed(app.DIK_LALT):
				link = player.GetItemLink(itemSlotIndex)
				ime.PasteString(link)

			elif app.IsPressed(app.DIK_LSHIFT):
				itemCount = player.GetItemCount(itemSlotIndex)
				if itemCount > 1:
					self.dlgPickMoney.SetTitleName(localeinfo.PICK_ITEM_TITLE)
					self.dlgPickMoney.SetAcceptEvent(ui.__mem_func__(self.OnPickItem))
					self.dlgPickMoney.Open(itemCount, 1, False)
					self.dlgPickMoney.itemGlobalSlotIndex = itemSlotIndex
				else:
					selectedItemVNum = player.GetItemIndex(itemSlotIndex)
					mousemodule.mouseController.AttachObject(self, player.SLOT_TYPE_INVENTORY, itemSlotIndex, selectedItemVNum)
			elif app.IsPressed(app.DIK_LCONTROL):
				itemIndex = player.GetItemIndex(itemSlotIndex)

				if True == item.CanAddToQuickSlotItem(itemIndex):
					player.RequestAddToEmptyLocalQuickSlot(player.SLOT_TYPE_INVENTORY, itemSlotIndex)
				else:
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.QUICKSLOT_REGISTER_DISABLE_ITEM)

			else:
				selectedItemVNum = player.GetItemIndex(itemSlotIndex)
				itemCount = player.GetItemCount(itemSlotIndex)
				mousemodule.mouseController.AttachObject(self, player.SLOT_TYPE_INVENTORY, itemSlotIndex, selectedItemVNum, itemCount)

				if self.__IsUsableItemToItem(selectedItemVNum, itemSlotIndex):
					self.wndItem.SetUseMode(True)
				else:
					self.wndItem.SetUseMode(False)

				snd.PlaySound("sound/ui/pick.wav")


	if app.NEW_PET_SYSTEM:
		def UseTransportBox(self):
			self.__SendUseItemToItemPacket(self.questionDialog.src, self.questionDialog.dst)
			self.OnCloseQuestionDialog()
		
		def UseProtein(self):
			self.__SendUseItemToItemPacket(self.questionDialog.src, self.questionDialog.dst)
			self.OnCloseQuestionDialog()

	def __DropSrcItemToDestItemInInventory(self, srcItemVID, srcItemSlotPos, dstItemSlotPos, destWindow = 0):
		if app.ENABLE_EXTRA_INVENTORY:
			if srcItemSlotPos == dstItemSlotPos and not item.IsMetin(srcItemVID):
				return
		else:
			if srcItemSlotPos == dstItemSlotPos:
				return

		if app.ENABLE_COSTUME_TIME_EXTENDER:
			if constinfo.IS_COSTUME_TIME_EXTENDER(player.GetItemIndex(srcItemSlotPos)):
				item.SelectItem(player.GetItemIndex(dstItemSlotPos))
				if item.GetItemType() == item.ITEM_TYPE_COSTUME and (item.GetItemSubType() == item.COSTUME_TYPE_BODY or item.GetItemSubType() == item.COSTUME_TYPE_HAIR or item.GetItemSubType() == item.COSTUME_TYPE_WEAPON):
					self.__SendUseItemToItemPacket(srcItemSlotPos, dstItemSlotPos)

		if app.__ENABLE_NEW_OFFLINESHOP__:
			if uiofflineshop.IsBuildingShop() and (uiofflineshop.IsSaleSlot(player.INVENTORY, srcItemSlotPos) or uiofflineshop.IsSaleSlot(player.INVENTORY , dstItemSlotPos)):
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.OFFLINESHOP_CANT_SELECT_ITEM_DURING_BUILING)
				return
		
		if app.NEW_PET_SYSTEM:
			if srcItemVID >= 55701 and srcItemVID <= 55711 and player.GetItemIndex(dstItemSlotPos) == 55002:
				self.questionDialog = uicommon.QuestionDialog()
				self.questionDialog.SetText(localeinfo.PETSPECIAL_QUESTION1)
				self.questionDialog.SetAcceptEvent(ui.__mem_func__(self.UseTransportBox))
				self.questionDialog.SetCancelEvent(ui.__mem_func__(self.OnCloseQuestionDialog))
				self.questionDialog.Open()
				self.questionDialog.src = srcItemSlotPos
				self.questionDialog.dst = dstItemSlotPos
				
			if srcItemVID == 55001 and player.GetItemIndex(dstItemSlotPos) >= 55701 and player.GetItemIndex(dstItemSlotPos) <= 55711:
				self.questionDialog = uicommon.QuestionDialog()
				self.questionDialog.SetText(localeinfo.PETSPECIAL_QUESTION2)
				self.questionDialog.SetAcceptEvent(ui.__mem_func__(self.UseProtein))
				self.questionDialog.SetCancelEvent(ui.__mem_func__(self.OnCloseQuestionDialog))
				self.questionDialog.Open()
				self.questionDialog.src = srcItemSlotPos
				self.questionDialog.dst = dstItemSlotPos
		
		if app.ENABLE_SOULBIND_SYSTEM and item.IsSealScroll(srcItemVID):
			self.__SendUseItemToItemPacket(srcItemSlotPos, dstItemSlotPos)
		elif item.IsRefineScroll(srcItemVID):
			self.RefineItem(srcItemSlotPos, dstItemSlotPos)
			self.wndItem.SetUseMode(False)
		elif item.IsMetin(srcItemVID):
			self.AttachMetinToItem(srcItemSlotPos, dstItemSlotPos)
		elif item.IsDetachScroll(srcItemVID):
			self.DetachMetinFromItem(srcItemSlotPos, dstItemSlotPos)
		elif item.IsKey(srcItemVID):
			self.__SendUseItemToItemPacket(srcItemSlotPos, dstItemSlotPos)
		elif (player.GetItemFlags(srcItemSlotPos) & ITEM_FLAG_APPLICABLE) == ITEM_FLAG_APPLICABLE:
			self.__SendUseItemToItemPacket(srcItemSlotPos, dstItemSlotPos)
		elif item.GetUseType(srcItemVID) in self.USE_TYPE_TUPLE:
			if destWindow == player.SLOT_TYPE_EXTRA_INVENTORY:
				self.__SendUseItemToItemPacketExtra(srcItemSlotPos, dstItemSlotPos)
			else:
				self.__SendUseItemToItemPacket(srcItemSlotPos, dstItemSlotPos)
		else:
			if app.ENABLE_ATTR_COSTUMES and (item.GetUseType(srcItemVID) == "USE_CHANGE_ATTR_COSTUME" or item.GetUseType(srcItemVID) == "USE_ADD_ATTR_COSTUME1" or item.GetUseType(srcItemVID) == "USE_ADD_ATTR_COSTUME2"):
				if destWindow == player.SLOT_TYPE_EXTRA_INVENTORY:
					self.__SendUseItemToItemPacketExtra(srcItemSlotPos, dstItemSlotPos)
				else:
					self.__SendUseItemToItemPacket(srcItemSlotPos, dstItemSlotPos)
				return
			elif app.ENABLE_ATTR_COSTUMES and item.GetUseType(srcItemVID) == "USE_REMOVE_ATTR_COSTUME":
				if destWindow == player.SLOT_TYPE_EXTRA_INVENTORY:
					self.__SendRemoveAttrCostume(srcItemSlotPos, dstItemSlotPos)
				return
			elif app.ENABLE_NEW_PET_EDITS and item.GetUseType(srcItemVID) == "USE_PET_REVIVE" and self.__CanUseSrcItemToDstItem(srcItemVID, srcItemSlotPos, dstItemSlotPos) == True:
				if destWindow == player.SLOT_TYPE_EXTRA_INVENTORY:
					self.__SendUseItemToItemPacketExtra(srcItemSlotPos, dstItemSlotPos)
				else:
					self.__SendRevivePet(srcItemSlotPos, dstItemSlotPos)
				return
			elif app.ENABLE_NEW_PET_EDITS and item.GetUseType(srcItemVID) == "USE_PET_ENCHANT" and self.__CanUseSrcItemToDstItem(srcItemVID, srcItemSlotPos, dstItemSlotPos) == True:
				if destWindow == player.SLOT_TYPE_EXTRA_INVENTORY:
					self.__SendUseItemToItemPacketExtra(srcItemSlotPos, dstItemSlotPos)
				else:
					self.__SendEnchantPet(srcItemSlotPos, dstItemSlotPos)
				return
			elif app.ENABLE_REMOTE_ATTR_SASH_REMOVE and item.GetUseType(srcItemVID) == "USE_ATTR_SASH_REMOVE":
				if destWindow == player.SLOT_TYPE_EXTRA_INVENTORY:
					self.__SendUseItemToItemPacketExtra(srcItemSlotPos, dstItemSlotPos)
				else:
					self.__SendRemoveAttrSash(srcItemSlotPos, dstItemSlotPos)
				return
			elif app.ENABLE_STOLE_COSTUME and item.GetUseType(srcItemVID) == "USE_ENCHANT_STOLE":
				if destWindow == player.SLOT_TYPE_EXTRA_INVENTORY:
					self.__SendUseItemToItemPacketExtra(srcItemSlotPos, dstItemSlotPos)
				else:
					self.__SendUseItemToItemPacket(srcItemSlotPos, dstItemSlotPos)
				return
			#snd.PlaySound("sound/ui/drop.wav")

			## �̵���Ų ���� ���� ������ ��� �������� ����ؼ� ���� ��Ų�� - [levites]
			if player.IsEquipmentSlot(dstItemSlotPos):

				## ��� �ִ� �������� ����϶���
				if item.IsEquipmentVID(srcItemVID):
					self.__UseItem(srcItemSlotPos)

			else:
				if destWindow == player.SLOT_TYPE_EXTRA_INVENTORY:
					self.__SendUseItemToItemPacketExtra(srcItemSlotPos, dstItemSlotPos)
				else:
					self.__SendMoveItemPacket(srcItemSlotPos, dstItemSlotPos, 0)
				#net.SendItemMovePacket(srcItemSlotPos, dstItemSlotPos, 0)

	if app.ENABLE_REMOTE_ATTR_SASH_REMOVE:
		def __OnRemoveAttrSash(self):
			if self.questionDialog:
				self.__SendUseItemToItemPacket(self.questionDialog.src, self.questionDialog.dst)
				self.OnCloseQuestionDialog()

		def __SendRemoveAttrSash(self, srcItemSlotPos, dstItemSlotPos):
				self.questionDialog = uicommon.QuestionDialog()
				self.questionDialog.SetText(localeinfo.ALERT_SASH_REMOVE_ATTR)
				self.questionDialog.SetAcceptEvent(ui.__mem_func__(self.__OnRemoveAttrSash))
				self.questionDialog.SetCancelEvent(ui.__mem_func__(self.OnCloseQuestionDialog))
				self.questionDialog.Open()
				self.questionDialog.src = srcItemSlotPos
				self.questionDialog.dst = dstItemSlotPos

	if app.ENABLE_NEW_PET_EDITS:
		def __SendEnchantPet(self, srcItemSlotPos, dstItemSlotPos):
			self.OnCloseEnchantPet()
			
			import uipetenchant
			self.enchantPetDialog = uipetenchant.Main()
			self.enchantPetDialog.Open(self, srcItemSlotPos, dstItemSlotPos)

		def OnCloseEnchantPet(self):
			if self.enchantPetDialog:
				self.enchantPetDialog.CloseAsk()
				self.enchantPetDialog = None

		def __OnRevivePet(self):
			if self.questionDialog:
				self.__SendUseItemToItemPacket(self.questionDialog.src, self.questionDialog.dst)
				self.OnCloseQuestionDialog()

		def __SendRevivePet(self, srcItemSlotPos, dstItemSlotPos):
				self.questionDialog = uicommon.QuestionDialog()
				self.questionDialog.SetText(localeinfo.ALERT_REVIVE_PET)
				self.questionDialog.SetAcceptEvent(ui.__mem_func__(self.__OnRevivePet))
				self.questionDialog.SetCancelEvent(ui.__mem_func__(self.OnCloseQuestionDialog))
				self.questionDialog.Open()
				self.questionDialog.src = srcItemSlotPos
				self.questionDialog.dst = dstItemSlotPos

	if app.ENABLE_ATTR_COSTUMES:
		def __SendRemoveAttrCostume(self, srcItemSlotPos, dstItemSlotPos):
			dstItemVNum = player.GetItemIndex(dstItemSlotPos)
			if dstItemVNum == 0:
				return
			
			item.SelectItem(dstItemVNum)
			if item.GetItemType() != item.ITEM_TYPE_COSTUME:
				return
			
			toContinue = False
			for i in xrange(player.ATTRIBUTE_SLOT_RARE_NUM):
				i += player.ATTRIBUTE_SLOT_NORM_NUM
				(t, v) = player.GetItemAttribute(dstItemSlotPos, i)
				if t != 0:
					toContinue = True
					break
			
			if toContinue:
				self.OnCloseRemoveAttrCostume()
				
				import uiattrdialog
				self.removeAttrDialog = uiattrdialog.Remove()
				self.removeAttrDialog.Open(self, srcItemSlotPos, dstItemSlotPos)

		def OnCloseRemoveAttrCostume(self):
			if self.removeAttrDialog:
				self.removeAttrDialog.CloseAsk()
				self.removeAttrDialog = None

	def __SellItem(self, itemSlotPos):
		if app.__ENABLE_NEW_OFFLINESHOP__:
			if uiofflineshop.IsBuildingShop():
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.OFFLINESHOP_CANT_SELECT_ITEM_DURING_BUILING)
				return
		
		
		if not player.IsEquipmentSlot(itemSlotPos):
			self.sellingSlotNumber = itemSlotPos
			itemIndex = player.GetItemIndex(itemSlotPos)
			itemCount = player.GetItemCount(itemSlotPos)


			self.sellingSlotitemIndex = itemIndex
			self.sellingSlotitemCount = itemCount

			item.SelectItem(itemIndex)
			## ��Ƽ �÷��� �˻� ������ �߰�
			## 20140220
			if item.IsAntiFlag(item.ANTIFLAG_SELL):
				popup = uicommon.PopupDialog()
				popup.SetText(localeinfo.SHOP_CANNOT_SELL_ITEM)
				popup.SetAcceptEvent(self.__OnClosePopupDialog)
				popup.Open()
				self.popup = popup
				return

			itemPrice = item.GetISellItemPrice()

			if item.Is1GoldItem():
				itemPrice = itemCount / itemPrice / 5
			else:
				itemPrice = itemPrice * itemCount / 5

			item.GetItemName(itemIndex)
			itemName = item.GetItemName()

			self.questionDialog = uicommon.QuestionDialog()
			self.questionDialog.SetText(localeinfo.DO_YOU_SELL_ITEM(itemName, itemCount, itemPrice))
			self.questionDialog.SetAcceptEvent(ui.__mem_func__(self.SellItem))
			self.questionDialog.SetCancelEvent(ui.__mem_func__(self.OnCloseQuestionDialog))
			self.questionDialog.Open()
			self.questionDialog.count = itemCount

			constinfo.SET_ITEM_QUESTION_DIALOG_STATUS(1)

	def __OnClosePopupDialog(self):
		self.pop = None

	def RefineItem(self, scrollSlotPos, targetSlotPos):
		if app.__ENABLE_NEW_OFFLINESHOP__:
			if uiofflineshop.IsBuildingShop():
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.OFFLINESHOP_CANT_SELECT_ITEM_DURING_BUILING)
				return
		


		scrollIndex = player.GetItemIndex(scrollSlotPos)
		targetIndex = player.GetItemIndex(targetSlotPos)

		if player.REFINE_OK != player.CanRefine(scrollIndex, targetSlotPos):
			return

		###########################################################
		self.__SendUseItemToItemPacket(scrollSlotPos, targetSlotPos)
		#net.SendItemUseToItemPacket(scrollSlotPos, targetSlotPos)
		return
		###########################################################

		###########################################################
		#net.SendRequestRefineInfoPacket(targetSlotPos)
		#return
		###########################################################

		result = player.CanRefine(scrollIndex, targetSlotPos)

		if player.REFINE_ALREADY_MAX_SOCKET_COUNT == result:
			#snd.PlaySound("sound/ui/jaeryun_fail.wav")
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.REFINE_FAILURE_NO_MORE_SOCKET)

		elif player.REFINE_NEED_MORE_GOOD_SCROLL == result:
			#snd.PlaySound("sound/ui/jaeryun_fail.wav")
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.REFINE_FAILURE_NEED_BETTER_SCROLL)

		elif player.REFINE_CANT_MAKE_SOCKET_ITEM == result:
			#snd.PlaySound("sound/ui/jaeryun_fail.wav")
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.REFINE_FAILURE_SOCKET_DISABLE_ITEM)

		elif player.REFINE_NOT_NEXT_GRADE_ITEM == result:
			#snd.PlaySound("sound/ui/jaeryun_fail.wav")
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.REFINE_FAILURE_UPGRADE_DISABLE_ITEM)

		elif player.REFINE_CANT_REFINE_METIN_TO_EQUIPMENT == result:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.REFINE_FAILURE_EQUIP_ITEM)

		if player.REFINE_OK != result:
			return

		self.refineDialog.Open(scrollSlotPos, targetSlotPos)

	def DetachMetinFromItem(self, scrollSlotPos, targetSlotPos):
		scrollIndex = player.GetItemIndex(scrollSlotPos)
		targetIndex = player.GetItemIndex(targetSlotPos)

		if not player.CanDetach(scrollIndex, targetSlotPos):
			if app.ENABLE_ACCE_SYSTEM:
				item.SelectItem(scrollIndex)
				if item.GetValue(0) == acce.CLEAN_ATTR_VALUE0:
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.ACCE_FAILURE_CLEAN)
				else:
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.REFINE_FAILURE_METIN_INSEPARABLE_ITEM)
			else:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.REFINE_FAILURE_METIN_INSEPARABLE_ITEM)
			return

		self.questionDialog = uicommon.QuestionDialog()
		self.questionDialog.SetText(localeinfo.REFINE_DO_YOU_SEPARATE_METIN)
		if app.ENABLE_ACCE_SYSTEM:
			item.SelectItem(targetIndex)
			if item.GetItemType() == item.ITEM_TYPE_COSTUME and item.GetItemSubType() == item.COSTUME_TYPE_ACCE:
				item.SelectItem(scrollIndex)
				if item.GetValue(0) == acce.CLEAN_ATTR_VALUE0:
					self.questionDialog.SetText(localeinfo.ACCE_DO_YOU_CLEAN)
		
		
		
		self.questionDialog.SetAcceptEvent(ui.__mem_func__(self.OnDetachMetinFromItem))
		self.questionDialog.SetCancelEvent(ui.__mem_func__(self.OnCloseQuestionDialog))
		self.questionDialog.Open()
		self.questionDialog.sourcePos = scrollSlotPos
		self.questionDialog.targetPos = targetSlotPos

	def AttachMetinToItem(self, metinSlotPos, targetSlotPos):
		if app.ENABLE_EXTRA_INVENTORY:
			metinIndex = player.GetItemIndex(player.EXTRA_INVENTORY, metinSlotPos)
		else:
			metinIndex = player.GetItemIndex(metinSlotPos)
		targetIndex = player.GetItemIndex(targetSlotPos)

		item.SelectItem(metinIndex)
		itemName = item.GetItemName()

		result = player.CanAttachMetin(metinIndex, targetSlotPos)

		if player.ATTACH_METIN_NOT_MATCHABLE_ITEM == result:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.REFINE_FAILURE_CAN_NOT_ATTACH(itemName))

		if player.ATTACH_METIN_NO_MATCHABLE_SOCKET == result:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.REFINE_FAILURE_NO_SOCKET(itemName))

		elif player.ATTACH_METIN_NOT_EXIST_GOLD_SOCKET == result:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.REFINE_FAILURE_NO_GOLD_SOCKET(itemName))

		elif player.ATTACH_METIN_CANT_ATTACH_TO_EQUIPMENT == result:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.REFINE_FAILURE_EQUIP_ITEM)

		if player.ATTACH_METIN_OK != result:
			return

		self.attachMetinDialog.Open(metinSlotPos, targetSlotPos)

	def OverOutItem(self):
		self.wndItem.SetUsableItem(False)
		if None != self.tooltipItem:
			self.tooltipItem.HideToolTip()

	def OverOutEmptySlot(self):
		self.wndItem.SetUsableItem(False)
		if None != self.tooltipItem:
			self.tooltipItem.HideToolTip()

	def OverInEmptySlot(self, overSlotPos):
		if self.tooltipItem != None:
			try:
				slotName = self.SLOTNAMES[overSlotPos]
				self.tooltipItem.ClearToolTip()
				self.tooltipItem.AppendTextLine(slotName, grp.GenerateColor(0.7607, 0.7607, 0.7607, 1.0))
				self.tooltipItem.ShowToolTip()
			except:
				self.OverOutItem()

	def OverInItem(self, overSlotPos):
		if app.ENABLE_HIGHLIGHT_SYSTEM:
			slotNumber = self.__InventoryLocalSlotPosToGlobalSlotPos(overSlotPos)
			self.wndItem.DeactivateSlotEffect(overSlotPos)
			try:
				self.listHighlightedSlot.remove(slotNumber)
			except:
				pass
		
		overSlotPos = self.__InventoryLocalSlotPosToGlobalSlotPos(overSlotPos)
		self.wndItem.SetUsableItem(False)
		
		if mousemodule.mouseController.isAttached():
			attachedItemType = mousemodule.mouseController.GetAttachedType()
		
			if app.ENABLE_EXTRA_INVENTORY:
				if attachedItemType == player.SLOT_TYPE_INVENTORY or attachedItemType == player.SLOT_TYPE_EXTRA_INVENTORY:
					attachedSlotPos = mousemodule.mouseController.GetAttachedSlotNumber()
					attachedItemVNum = mousemodule.mouseController.GetAttachedItemIndex()
		
					if attachedItemVNum == player.ITEM_MONEY:
						pass
					elif self.__CanUseSrcItemToDstItem(attachedItemVNum, attachedSlotPos, overSlotPos):
						self.wndItem.SetUsableItem(True)
						self.wndItem.SetUseMode(True)
						self.ShowToolTip(overSlotPos)
						return
					elif attachedItemVNum == player.GetItemIndex(overSlotPos) and attachedItemVNum >= 70000 and attachedItemVNum <= 99999:
						self.wndItem.SetUsableItem(False)
						self.wndItem.SetUseMode(False)
						self.ShowToolTip(overSlotPos)
						return
			else:
				if player.SLOT_TYPE_INVENTORY == attachedItemType:
					attachedSlotPos = mousemodule.mouseController.GetAttachedSlotNumber()
					attachedItemVNum = mousemodule.mouseController.GetAttachedItemIndex()
					
					if attachedItemVNum==player.ITEM_MONEY: # @fixme005
						pass
					elif self.__CanUseSrcItemToDstItem(attachedItemVNum, attachedSlotPos, overSlotPos):
						self.wndItem.SetUsableItem(True)
						self.ShowToolTip(overSlotPos)
						return
		
		self.ShowToolTip(overSlotPos)


	def __IsUsableItemToItem(self, srcItemVNum, srcSlotPos):
		"�ٸ� �����ۿ� ����� �� �ִ� �������ΰ�?"

		if app.ENABLE_COSTUME_TIME_EXTENDER:
			if constinfo.IS_COSTUME_TIME_EXTENDER(srcItemVNum):
				return True

		if app.NEW_PET_SYSTEM:
			if srcItemVNum >= 55701 and srcItemVNum <= 55711:
				return True
			if srcItemVNum == 55001:
				return True

		if item.IsRefineScroll(srcItemVNum):
			return True
		elif item.IsMetin(srcItemVNum):
			return True
		elif item.IsDetachScroll(srcItemVNum):
			return True
		elif item.IsKey(srcItemVNum):
			return True
		elif (player.GetItemFlags(srcSlotPos) & ITEM_FLAG_APPLICABLE) == ITEM_FLAG_APPLICABLE:
			return True
		else:
			if item.GetUseType(srcItemVNum) in self.USE_TYPE_TUPLE:
				return True
			
			if app.ENABLE_ATTR_COSTUMES and (item.GetUseType(srcItemVNum) == "USE_CHANGE_ATTR_COSTUME" or item.GetUseType(srcItemVNum) == "USE_ADD_ATTR_COSTUME1" or item.GetUseType(srcItemVNum) == "USE_ADD_ATTR_COSTUME2" or item.GetUseType(srcItemVNum) == "USE_REMOVE_ATTR_COSTUME"):
				return True
			
			if app.ENABLE_NEW_PET_EDITS and (item.GetUseType(srcItemVNum) == "USE_PET_REVIVE" or item.GetUseType(srcItemVNum) == "USE_PET_ENCHANT"):
				return True
			
			if app.ENABLE_REMOTE_ATTR_SASH_REMOVE and item.GetUseType(srcItemVNum) == "USE_ATTR_SASH_REMOVE":
				return True
			
			if app.ENABLE_STOLE_COSTUME and item.GetUseType(srcItemVNum) == "USE_ENCHANT_STOLE":
				return True
		
		return False

	def __CanUseSrcItemToDstItem(self, srcItemVNum, srcSlotPos, dstSlotPos):
		"��� �����ۿ� ����� �� �ִ°�?"
		if app.NEW_PET_SYSTEM:
			if srcItemVNum >= 55701 and  srcItemVNum <= 55711 and player.GetItemIndex(dstSlotPos) == 55002:
				return True		
			if srcItemVNum == 55001 and player.GetItemIndex(dstSlotPos) >= 55701 and player.GetItemIndex(dstSlotPos) <= 55711:
				return True

		if srcSlotPos == dstSlotPos:
			return False
			
		if app.ENABLE_COSTUME_TIME_EXTENDER:
			if constinfo.IS_COSTUME_TIME_EXTENDER(srcItemVNum):
				item.SelectItem(player.GetItemIndex(dstSlotPos))
				if item.GetItemType() == item.ITEM_TYPE_COSTUME and (item.GetItemSubType() == item.COSTUME_TYPE_BODY or item.GetItemSubType() == item.COSTUME_TYPE_HAIR or item.GetItemSubType() == item.COSTUME_TYPE_WEAPON):
					return True

		#if app.ENABLE_EXTRA_INVENTORY:
		#	if srcSlotPos == dstSlotPos:
		#		return False
		#	elif item.IsMetin(srcItemVNum):
		#		if player.ATTACH_METIN_OK == player.CanAttachMetin(srcItemVNum, dstSlotPos):
		#			itemVnumDest = player.GetItemIndex(dstSlotPos)
		#			return True
		#else:
		if srcSlotPos == dstSlotPos:
			return False
		
		if item.IsRefineScroll(srcItemVNum):
			if player.REFINE_OK == player.CanRefine(srcItemVNum, dstSlotPos):
				return True
		elif item.IsMetin(srcItemVNum):
			if player.ATTACH_METIN_OK == player.CanAttachMetin(srcItemVNum, dstSlotPos):
				return True
		elif item.IsDetachScroll(srcItemVNum):
			if player.DETACH_METIN_OK == player.CanDetach(srcItemVNum, dstSlotPos):
				return True
		elif item.IsKey(srcItemVNum):
			if player.CanUnlock(srcItemVNum, dstSlotPos):
				return True

		elif (player.GetItemFlags(srcSlotPos) & ITEM_FLAG_APPLICABLE) == ITEM_FLAG_APPLICABLE:
			return True

		else:
			useType=item.GetUseType(srcItemVNum)
			if "USE_CLEAN_SOCKET" == useType:
				if self.__CanCleanBrokenMetinStone(dstSlotPos):
					return True
			elif "USE_CHANGE_ATTRIBUTE" == useType:
				if self.__CanChangeItemAttrList(dstSlotPos):
					return True
			elif "USE_ADD_ATTRIBUTE" == useType:
				if self.__CanAddItemAttr(dstSlotPos):
					return True
			elif "USE_ADD_ATTRIBUTE2" == useType:
				if self.__CanAddItemAttr(dstSlotPos):
					return True
			elif "USE_ADD_ACCESSORY_SOCKET" == useType:
				if self.__CanAddAccessorySocket(dstSlotPos):
					return True
				else:
					dstItemVNum = player.GetItemIndex(dstSlotPos)
					item.SelectItem(dstItemVNum)
					if item.ITEM_TYPE_BELT == item.GetItemType():
						return True
			elif "USE_PUT_INTO_ACCESSORY_SOCKET" == useType:
				if self.__CanPutAccessorySocket(dstSlotPos, srcItemVNum):
					return True
			elif "USE_PUT_INTO_BELT_SOCKET" == useType:
				dstItemVNum = player.GetItemIndex(dstSlotPos)
				item.SelectItem(dstItemVNum)
				if item.ITEM_TYPE_BELT == item.GetItemType():
					return True
			elif "USE_CHANGE_COSTUME_ATTR" == useType:
				if self.__CanChangeCostumeAttrList(dstSlotPos):
					return True
			elif "USE_RESET_COSTUME_ATTR" == useType:
				if self.__CanResetCostumeAttr(dstSlotPos):
					return True
			elif app.ENABLE_ATTR_COSTUMES and item.GetUseType(srcItemVNum) == "USE_CHANGE_ATTR_COSTUME":
				dstItemVNum = player.GetItemIndex(dstSlotPos)
				if dstItemVNum == 0:
					return False
				
				item.SelectItem(dstItemVNum)
				if item.GetItemType() != item.ITEM_TYPE_COSTUME:
					return False
				
				for i in xrange(player.ATTRIBUTE_SLOT_NORM_NUM):
					(t, v) = player.GetItemAttribute(dstSlotPos, i)
					if t != 0:
						return True
				
				return False
			elif app.ENABLE_ATTR_COSTUMES and (item.GetUseType(srcItemVNum) == "USE_ADD_ATTR_COSTUME1" or item.GetUseType(srcItemVNum) == "USE_ADD_ATTR_COSTUME2"):
				dstItemVNum = player.GetItemIndex(dstSlotPos)
				if dstItemVNum == 0:
					return False
				
				item.SelectItem(dstItemVNum)
				if item.GetItemType() != item.ITEM_TYPE_COSTUME:
					return False
				
				for i in xrange(player.ATTRIBUTE_SLOT_RARE_NUM):
					i += player.ATTRIBUTE_SLOT_NORM_NUM
					(t, v) = player.GetItemAttribute(dstSlotPos, i)
					if t == 0:
						return True
				
				return False
			elif app.ENABLE_ATTR_COSTUMES and item.GetUseType(srcItemVNum) == "USE_REMOVE_ATTR_COSTUME":
				dstItemVNum = player.GetItemIndex(dstSlotPos)
				if dstItemVNum == 0:
					return False
				
				item.SelectItem(dstItemVNum)
				if item.GetItemType() != item.ITEM_TYPE_COSTUME:
					return False
				
				for i in xrange(player.ATTRIBUTE_SLOT_RARE_NUM):
					i += player.ATTRIBUTE_SLOT_NORM_NUM
					(t, v) = player.GetItemAttribute(dstSlotPos, i)
					if t != 0:
						return True
				
				return False
			elif app.ENABLE_NEW_PET_EDITS and (item.GetUseType(srcItemVNum) == "USE_PET_REVIVE" or item.GetUseType(srcItemVNum) == "USE_PET_ENCHANT"):
				dstItemVNum = player.GetItemIndex(dstSlotPos)
				if dstItemVNum == 0:
					return False
				
				item.SelectItem(dstItemVNum)
				if constinfo.IS_PET_SEAL(dstItemVNum):
					return True
				
				return False
			elif app.ENABLE_REMOTE_ATTR_SASH_REMOVE and item.GetUseType(srcItemVNum) == "USE_ATTR_SASH_REMOVE":
				dstItemVNum = player.GetItemIndex(dstSlotPos)
				if dstItemVNum == 0:
					return False
				
				item.SelectItem(dstItemVNum)
				if item.GetItemType() != item.ITEM_TYPE_COSTUME:
					return False
				
				if item.GetItemSubType() != item.COSTUME_TYPE_ACCE:
					return False
				
				metinSlot = [player.GetItemMetinSocket(player.INVENTORY, dstSlotPos, i) for i in xrange(player.METIN_SOCKET_MAX_NUM)]
				itemAbsorbedVnum = int(metinSlot[acce.ABSORBED_SOCKET])
				if itemAbsorbedVnum:
					return True
				
				return False
			elif app.ENABLE_STOLE_COSTUME and item.GetUseType(srcItemVNum) == "USE_ENCHANT_STOLE":
				dstItemVNum = player.GetItemIndex(dstSlotPos)
				if dstItemVNum == 0:
					return False
				
				item.SelectItem(dstItemVNum)
				if item.GetItemType() != item.ITEM_TYPE_COSTUME:
					return False
				
				if item.GetItemSubType() != item.COSTUME_TYPE_STOLE:
					return False
				
				grade = item.GetValue(0)
				if grade < 1 or grade > 4:
					return False
				
				maxAttr = player.ATTRIBUTE_SLOT_NORM_NUM + player.ATTRIBUTE_SLOT_RARE_NUM
				for i in xrange(maxAttr):
					(t, v) = player.GetItemAttribute(dstSlotPos, i)
					if t != 0:
						return True
				
				return False
		
		return False

	def __CanCleanBrokenMetinStone(self, dstSlotPos):
		dstItemVNum = player.GetItemIndex(dstSlotPos)
		if dstItemVNum == 0:
			return False

		item.SelectItem(dstItemVNum)

		if item.ITEM_TYPE_WEAPON != item.GetItemType():
			return False

		for i in xrange(player.METIN_SOCKET_MAX_NUM):
			if player.GetItemMetinSocket(dstSlotPos, i) == constinfo.ERROR_METIN_STONE:
				return True

		return False

	def __CanChangeItemAttrList(self, dstSlotPos):
		dstItemVNum = player.GetItemIndex(dstSlotPos)
		if dstItemVNum == 0:
			return False

		item.SelectItem(dstItemVNum)

		if not item.GetItemType() in (item.ITEM_TYPE_WEAPON, item.ITEM_TYPE_ARMOR):
			return False

		for i in xrange(player.METIN_SOCKET_MAX_NUM):
			if player.GetItemAttribute(dstSlotPos, i) != 0:
				return True

		return False

	def __CanChangeCostumeAttrList(self, dstSlotPos):
		dstItemVNum = player.GetItemIndex(dstSlotPos)
		if dstItemVNum == 0:
			return False

		item.SelectItem(dstItemVNum)

		if item.GetItemType() != item.ITEM_TYPE_COSTUME:
			return False

		for i in xrange(player.METIN_SOCKET_MAX_NUM):
			if player.GetItemAttribute(dstSlotPos, i) != 0:
				return True

		return False

	def __CanResetCostumeAttr(self, dstSlotPos):
		dstItemVNum = player.GetItemIndex(dstSlotPos)
		if dstItemVNum == 0:
			return False

		item.SelectItem(dstItemVNum)

		if item.GetItemType() != item.ITEM_TYPE_COSTUME:
			return False

		for i in xrange(player.METIN_SOCKET_MAX_NUM):
			if player.GetItemAttribute(dstSlotPos, i) != 0:
				return True

		return False

	def __CanPutAccessorySocket(self, dstSlotPos, mtrlVnum):
		dstItemVNum = player.GetItemIndex(dstSlotPos)
		if dstItemVNum == 0:
			return False
		
		item.SelectItem(dstItemVNum)

		if item.GetItemType() != item.ITEM_TYPE_ARMOR:
			return False

		if not item.GetItemSubType() in (item.ARMOR_WRIST, item.ARMOR_NECK, item.ARMOR_EAR):
			return False

		curCount = player.GetItemMetinSocket(dstSlotPos, 0)
		maxCount = player.GetItemMetinSocket(dstSlotPos, 1)
		if mtrlVnum != constinfo.GET_ACCESSORY_MATERIAL_VNUM(dstItemVNum, item.GetItemSubType()):
			if app.ENABLE_INFINITE_RAFINES:
				if mtrlVnum != constinfo.GET_ACCESSORY_MATERIAL_VNUM2(dstItemVNum, item.GetItemSubType()):
					return False
			else:
				return False

		if curCount>=maxCount:
			return False

		return True

	def __CanAddAccessorySocket(self, dstSlotPos):
		dstItemVNum = player.GetItemIndex(dstSlotPos)
		if dstItemVNum == 0:
			return False

		item.SelectItem(dstItemVNum)

		if item.GetItemType() != item.ITEM_TYPE_ARMOR:
			return False

		if not item.GetItemSubType() in (item.ARMOR_WRIST, item.ARMOR_NECK, item.ARMOR_EAR):
			return False

		curCount = player.GetItemMetinSocket(dstSlotPos, 0)
		maxCount = player.GetItemMetinSocket(dstSlotPos, 1)

		ACCESSORY_SOCKET_MAX_SIZE = 3
		if maxCount >= ACCESSORY_SOCKET_MAX_SIZE:
			return False

		return True

	def __CanAddItemAttr(self, dstSlotPos):
		dstItemVNum = player.GetItemIndex(dstSlotPos)
		if dstItemVNum == 0:
			return False

		item.SelectItem(dstItemVNum)

		if not item.GetItemType() in (item.ITEM_TYPE_WEAPON, item.ITEM_TYPE_ARMOR):
			return False

		attrCount = 0
		for i in xrange(player.METIN_SOCKET_MAX_NUM):
			if player.GetItemAttribute(dstSlotPos, i) != 0:
				attrCount += 1

		if attrCount<4:
			return True

		return False

	def ShowToolTip(self, slotIndex):
		if None != self.tooltipItem:
			self.tooltipItem.SetInventoryItem(slotIndex)

			if app.__ENABLE_NEW_OFFLINESHOP__:
				if uiofflineshop.IsBuildingShop() or uiofflineshop.IsBuildingAuction():
					self.__AddTooltipSaleMode(slotIndex)
	if app.__ENABLE_NEW_OFFLINESHOP__:
		def __AddTooltipSaleMode(self, slot):
			if player.IsEquipmentSlot(slot):
				return

			itemIndex = player.GetItemIndex(slot)
			if itemIndex !=0:
				item.SelectItem(itemIndex)
				if item.IsAntiFlag(item.ANTIFLAG_MYSHOP) or item.IsAntiFlag(item.ANTIFLAG_GIVE):
					return
				
				self.tooltipItem.AddRightClickForSale()
	def OnTop(self):
		if None != self.tooltipItem:
			self.tooltipItem.SetTop()
		
		if app.WJ_ENABLE_TRADABLE_ICON:
			map(lambda wnd:wnd.RefreshLockedSlot(), self.bindWnds)
			self.RefreshMarkSlots()

	def OnPressEscapeKey(self):
		self.Close()
		return True

	def UseItemSlot(self, slotIndex):
		if app.__ENABLE_NEW_OFFLINESHOP__:
			if uiofflineshop.IsBuildingShop():
				globalSlot 	= self.__InventoryLocalSlotPosToGlobalSlotPos(slotIndex)
				itemIndex 	= player.GetItemIndex(globalSlot)
				
				item.SelectItem(itemIndex)
				
				if not item.IsAntiFlag(item.ANTIFLAG_GIVE) and not item.IsAntiFlag(item.ANTIFLAG_MYSHOP):
					offlineshop.ShopBuilding_AddInventoryItem(globalSlot)
				
				else:
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.OFFLINESHOP_CANT_SELECT_ITEM_DURING_BUILING)
				
				return

			elif uiofflineshop.IsBuildingAuction():
				globalSlot = self.__InventoryLocalSlotPosToGlobalSlotPos(slotIndex)
				itemIndex = player.GetItemIndex(globalSlot)

				item.SelectItem(itemIndex)

				if not item.IsAntiFlag(item.ANTIFLAG_GIVE) and not item.IsAntiFlag(item.ANTIFLAG_MYSHOP):
					offlineshop.AuctionBuilding_AddInventoryItem(globalSlot)
				else:
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.OFFLINESHOP_CANT_SELECT_ITEM_DURING_BUILING)

				return
		
		
		curCursorNum = app.GetCursor()
		if app.SELL == curCursorNum:
			return

		if constinfo.GET_ITEM_QUESTION_DIALOG_STATUS():
			return

		slotIndex = self.__InventoryLocalSlotPosToGlobalSlotPos(slotIndex)

		if app.ENABLE_DRAGON_SOUL_SYSTEM:
			if self.wndDragonSoulRefine.IsShow():
				self.wndDragonSoulRefine.AutoSetItem((player.INVENTORY, slotIndex), 1)
				return
		if app.ENABLE_ACCE_SYSTEM:
			if self.isShowAcceWindow():
				acce.Add(player.INVENTORY, slotIndex, 255)
				return
		
		if app.IsPressed(app.DIK_LCONTROL) or app.IsPressed(app.DIK_RCONTROL):
				itemCount = player.GetItemCount(slotIndex)
				if not itemCount:
					self.__UseItem(slotIndex)
				else:
					for i in xrange(itemCount):
						self.__UseItem(slotIndex)
		else:
			if app.ENABLE_NEW_PET_EDITS:
				itemVnum = player.GetItemIndex(slotIndex)
				if itemVnum == 55002:
					metinSocket = [player.GetItemMetinSocket(slotIndex, j) for j in xrange(player.METIN_SOCKET_MAX_NUM)]
					if metinSocket[0] != 0:
						self.questionDialog = uicommon.QuestionDialog()
						self.questionDialog.SetText(localeinfo.PETBOX_RIMUOVE)
						self.questionDialog.SetAcceptEvent(ui.__mem_func__(self.__UsePetBoxDialog_OnAccept))
						self.questionDialog.SetCancelEvent(ui.__mem_func__(self.__UseItemQuestionDialog_OnCancel))
						self.questionDialog.Open()
						self.questionDialog.slotIndex = slotIndex
						constinfo.SET_ITEM_QUESTION_DIALOG_STATUS(1)
						return
				
			self.__UseItem(slotIndex)
		
		mousemodule.mouseController.DeattachObject()
		self.OverOutItem()

	if app.ENABLE_NEW_PET_EDITS:
		def __UsePetBoxDialog_OnAccept(self):
			self.__UseItem(self.questionDialog.slotIndex)
			self.__UseItemQuestionDialog_OnCancel()

	def __UseItem(self, slotIndex):
		if app.__ENABLE_NEW_OFFLINESHOP__:
			if uiofflineshop.IsBuildingShop() and uiofflineshop.IsSaleSlot(player.INVENTORY, slotIndex):
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.OFFLINESHOP_CANT_SELECT_ITEM_DURING_BUILING)
				return

		ItemVNum = player.GetItemIndex(slotIndex)
		item.SelectItem(ItemVNum)
		if app.__ENABLE_EXTEND_INVEN_SYSTEM__:
			if ItemVNum == 72320:
				self.Env_key()
		
		if ItemVNum == 53004 or ItemVNum == 53005:
			constinfo.SUPPORT_CALL = 0
		elif ItemVNum == 53001 or ItemVNum == 53003:
			constinfo.SUPPORT_CALL = 1

		if item.IsFlag(item.ITEM_FLAG_CONFIRM_WHEN_USE):
			self.questionDialog = uicommon.QuestionDialog()
			self.questionDialog.SetText(localeinfo.INVENTORY_REALLY_USE_ITEM)
			self.questionDialog.SetAcceptEvent(ui.__mem_func__(self.__UseItemQuestionDialog_OnAccept))
			self.questionDialog.SetCancelEvent(ui.__mem_func__(self.__UseItemQuestionDialog_OnCancel))
			self.questionDialog.Open()
			self.questionDialog.slotIndex = slotIndex

			constinfo.SET_ITEM_QUESTION_DIALOG_STATUS(1)

		else:
			self.__SendUseItemPacket(slotIndex)
			#net.SendItemUsePacket(slotIndex)

	def __UseItemQuestionDialog_OnCancel(self):
		self.OnCloseQuestionDialog()

	def __UseItemQuestionDialog_OnAccept(self):
		self.__SendUseItemPacket(self.questionDialog.slotIndex)
		self.OnCloseQuestionDialog()

	def __SendUseItemToItemPacket(self, srcSlotPos, dstSlotPos):
		# ���λ��� ���� �ִ� ���� ������ ��� ����
		if uiprivateshopbuilder.IsBuildingPrivateShop():
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.USE_ITEM_FAILURE_PRIVATE_SHOP)
			return
		
		net.SendItemUseToItemPacket(srcSlotPos, dstSlotPos)

	def __SendUseItemPacket(self, slotPos):
		# ���λ��� ���� �ִ� ���� ������ ��� ����
		if uiprivateshopbuilder.IsBuildingPrivateShop():
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.USE_ITEM_FAILURE_PRIVATE_SHOP)
			return

		net.SendItemUsePacket(slotPos)

	def __SendMoveItemPacket(self, srcSlotPos, dstSlotPos, srcItemCount):
		if uiprivateshopbuilder.IsBuildingPrivateShop():
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.MOVE_ITEM_FAILURE_PRIVATE_SHOP)
			return
		
		net.SendItemMovePacket(srcSlotPos, dstSlotPos, srcItemCount)

	def __SendUseItemToItemPacketExtra(self, srcSlotPos, dstSlotPos):
		# ���λ��� ���� �ִ� ���� ������ ��� ����
		if uiprivateshopbuilder.IsBuildingPrivateShop():
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.USE_ITEM_FAILURE_PRIVATE_SHOP)
			return
		
		net.SendItemUseToItemPacket(player.EXTRA_INVENTORY, srcSlotPos, player.INVENTORY, dstSlotPos)

	def SetDragonSoulRefineWindow(self, wndDragonSoulRefine):
		if app.ENABLE_DRAGON_SOUL_SYSTEM:
			self.wndDragonSoulRefine = wndDragonSoulRefine
			
	if app.ENABLE_ACCE_SYSTEM:
		def SetAcceWindow(self, wndAcceCombine, wndAcceAbsorption):
			self.wndAcceCombine = wndAcceCombine
			self.wndAcceAbsorption = wndAcceAbsorption

		def isShowAcceWindow(self):
			if self.wndAcceCombine:
				if self.wndAcceCombine.IsShow():
					return 1

			if self.wndAcceAbsorption:
				if self.wndAcceAbsorption.IsShow():
					return 1
			
			return 0

	def OnMoveWindow(self, x, y):
		if self.wndmenu != None:
			self.wndmenu.AdjustPosition()
		
		if self.wndBelt:
			self.wndBelt.AdjustPositionAndSize()



if app.ENABLE_CAPITALE_SYSTEM:		
	class ExpandedMoneyTaskBar(ui.ScriptWindow):
		def __init__(self):
			ui.Window.__init__(self)
			self.SetWindowName("ExpandedMoneyTaskBar")
			toolTipMoneyTitle = None
			if app.ENABLE_GAYA_SYSTEM:
				toolTipGayaTitle = None
			self.firstTime = 0

		def LoadWindow(self):
			try:
				pyScrLoader = ui.PythonScriptLoader()
				pyScrLoader.LoadScriptFile(self, "uiscript/expandedmoneytaskbar.py")

			except:
				import exception
				exception.Abort("ExpandedMoneyTaskBar.LoadWindow.LoadObject")

			self.expandedMoneyTaskBarBoard = self.GetChild("ExpanedMoneyTaskBar_Board")
			self.money = self.GetChild("Money")
			self.wndMoneySlotIcon = self.GetChild("Money_Icon")
			self.wndMoneySlotIcon.SetStringEvent("MOUSE_OVER_IN", ui.__mem_func__(self.__ShowMoneyTitleToolTip))
			self.wndMoneySlotIcon.SetStringEvent("MOUSE_OVER_OUT", ui.__mem_func__(self.__HideMoneyTitleToolTip))
			self.__SetCapitalSystemToolTip()
			if app.ENABLE_GAYA_SYSTEM:
				self.Gem = self.GetChild("Gem")
				self.wndGemSlotIcon = self.GetChild("Gem_Icon")
				self.wndGemSlotIcon.SetStringEvent("MOUSE_OVER_IN", ui.__mem_func__(self.__ShowGayaTitleToolTip))
				self.wndGemSlotIcon.SetStringEvent("MOUSE_OVER_OUT", ui.__mem_func__(self.__HideGayaTitleToolTip))

		def Show(self):
			ui.ScriptWindow.Show(self)

		def Close(self):
			self.Hide()

		def RefreshStatus(self):
			#self.money.SetText(localeinfo.NumberToMoneyString(player.GetMoney()))
			#fix: invalid literal for float(): 362.676.059 Yang // caused by:
			#def SetText(self, newValue):
			#	self.newValue = double(newValue)
			#@ui.py - class CoolRefreshTextLine(TextLine):
			self.money.SetText(player.GetMoney())
			if app.ENABLE_GAYA_SYSTEM:
				#self.Gem.SetText(localeinfo.NumberToGayaString(player.GetGaya()))
				self.Gem.SetText(player.GetGaya())

		def SetTop(self):
			super(ExpandedMoneyTaskBar, self).SetTop()

		def SetToggleButtonEvent(self, eButton, kEventFunc):
			self.toggleButtonDict[eButton].SetEvent(kEventFunc)

		
		def __SetCapitalSystemToolTip(self):
			if app.ENABLE_GAYA_SYSTEM:
				self.toolTipGayaTitle = uitooltip.ToolTip(20)
				self.toolTipGayaTitle.AutoAppendTextLine(localeinfo.GAYA_SYSTEM_UNIT_GAYA, uitooltip.ToolTip.GAYA_PRICE_COLOR)
				self.toolTipGayaTitle.AlignHorizonalCenter()
			self.toolTipMoneyTitle = uitooltip.ToolTip(20)
			self.toolTipMoneyTitle.AutoAppendTextLine(localeinfo.CHEQUE_SYSTEM_UNIT_YANG, uitooltip.ToolTip.PRICE_INFO_COLOR)
			self.toolTipMoneyTitle.AlignHorizonalCenter()

		def __ShowMoneyTitleToolTip(self):
			self.toolTipMoneyTitle.ShowToolTip()

		def __HideMoneyTitleToolTip(self):
			self.toolTipMoneyTitle.HideToolTip()

		if app.ENABLE_GAYA_SYSTEM:
			def __ShowGayaTitleToolTip(self):
				self.toolTipGayaTitle.ShowToolTip()

			def __HideGayaTitleToolTip(self):
				self.toolTipGayaTitle.HideToolTip()

		#def OnPressEscapeKey(self):
		#	self.Close()
		#	return True
