import ui
import chat
import item
import uitooltip
import player
import grp
import cube_renewal
import mousemodule
import app
import wndMgr
import uicommon
import localeinfo

if app.ENABLE_GAYA_SYSTEM:
	import uisystemgems

def FReturnInfo(func,index):

	if app.ENABLE_GAYA_SYSTEM and not app.ENABLE_CUBE_RENEWAL_COPY_WORLDARD:
		(vnum_reward,count_reward,item_reward_stackable,vnum_material_1,count_material_1,vnum_material_2,count_material_2,vnum_material_3,count_material_3,vnum_material_4,count_material_4,vnum_material_5,count_material_5,gold,percent,category,gaya) = cube_renewal.GetDates(index)
	
	elif not app.ENABLE_GAYA_SYSTEM and app.ENABLE_CUBE_RENEWAL_COPY_WORLDARD:
		(vnum_reward,count_reward,item_reward_stackable,vnum_material_1,count_material_1,vnum_material_2,count_material_2,vnum_material_3,count_material_3,vnum_material_4,count_material_4,vnum_material_5,count_material_5,gold,percent,category,index_copy) = cube_renewal.GetDates(index)

	elif app.ENABLE_GAYA_SYSTEM and app.ENABLE_CUBE_RENEWAL_COPY_WORLDARD:
		(vnum_reward,count_reward,item_reward_stackable,vnum_material_1,count_material_1,vnum_material_2,count_material_2,vnum_material_3,count_material_3,vnum_material_4,count_material_4,vnum_material_5,count_material_5,gold,percent,category,gaya,index_copy) = cube_renewal.GetDates(index)

	else:
		(vnum_reward,count_reward,item_reward_stackable,vnum_material_1,count_material_1,vnum_material_2,count_material_2,vnum_material_3,count_material_3,vnum_material_4,count_material_4,vnum_material_5,count_material_5,gold,percent,category) = cube_renewal.GetDates(index)
	
	info = {
			"vnum_reward" : vnum_reward,
			"count_reward" : count_reward,
			"item_reward_stackable" : item_reward_stackable,
			"vnum_material_1" : vnum_material_1,
			"count_material_1" : count_material_1,
			"vnum_material_2" : vnum_material_2,
			"count_material_2" : count_material_2,
			"vnum_material_3" : vnum_material_3,
			"count_material_3" : count_material_3,
			"vnum_material_4" : vnum_material_4,
			"count_material_4" : count_material_4,
			"vnum_material_5" : vnum_material_5,
			"count_material_5" : count_material_5,
			"gold" : gold,
			"percent" : percent,
			"category" : category
	}
	if app.ENABLE_GAYA_SYSTEM:
		info["gaya"] = gaya

	if app.ENABLE_CUBE_RENEWAL_COPY_WORLDARD:
		info["index_copy"] = index_copy

	return info[func]

class CubeRenewalItem(ui.Window):

	def __init__(self, getParentEvent):
		ui.Window.__init__(self)
		self.SetParent(getParentEvent().GetBoard())
		
		self.index = -1
		self.type = -1

		self.count_all = -1

		self.date_cube = None

		self.index = -1

		self.button = ui.Button()
		self.button.SetParent(self)
		self.button.SetEvent(self.Select)
		self.button.Show()

		self.image = ui.ImageBox()
		self.image.SetParent(self)
		self.image.SetPosition(7,3)
		self.image.Show()

		self.text = ui.TextLine()
		self.text.SetParent(self)
		self.text.SetPosition(40,2)
		self.text.Show()

		self.count_total = ui.TextLine()
		self.count_total.SetParent(self)
		self.count_total.SetPosition(16,2)
		self.count_total.Hide()

		self.getParentEvent = getParentEvent

		self.SetSize(280, 17)

	def SetName(self, name):
		self.name = name
		self.text.SetText(name)

	def SetCountTotal(self, count):

		self.count_all = count

		if count <= 0:
			self.count_total.Hide()
			self.text.SetPosition(17,2)
			return

		self.count_total.SetText("[%d]"%(count*self.GetCountReward()))
		self.count_total.SetPackedFontColor(grp.GenerateColor(0.5411, 0.7254, 0.5568, 1.0))
		self.count_total.Show()

		w, h = self.count_total.GetTextSize()
		self.text.SetPosition(w+20,2)

	def GetCount(self):
		return self.count_all

	def SetType(self,type):
		self.type = type

	def AppendIndex(self,index):
		self.index = index

	def GetIndex(self):
		return self.index

	def GetDates(self,func):
		return FReturnInfo(func,self.GetIndex())

	def GetVnumReward(self):
		return self.GetDates("vnum_reward")

	def GetCountReward(self):
		return self.GetDates("count_reward")

	def GetItemStackable(self):
		return self.GetDates("item_reward_stackable")

	def GetVnumMaterial(self,index):
		return self.GetDates("vnum_material_%d"%index)

	def GetCountMaterial(self,index):
		return self.GetDates("count_material_%d"%index)

	def GetGold(self):
		return self.GetDates("gold")

	def GetPorcentaje(self):
		return self.GetDates("percent")

	if app.ENABLE_GAYA_SYSTEM:
		def GetGaya(self):
			return self.GetDates("gaya")

	if app.ENABLE_CUBE_RENEWAL_COPY_WORLDARD:
		def GetIndexCopy(self):
			return self.GetDates("index_copy")

	def GetName(self):
		return self.name

	def GetType(self):
		return self.type

	def GetStepWidth(self):
		return 0

	def Select(self):
		self.getParentEvent().OnSelectItem(self)

class CubeRenewalCategoriaItem(CubeRenewalItem):

	def __init__(self, getParentEvent):
		self.OpenFunc = False
		self.SubCategorias = []
		self.index = -1

		CubeRenewalItem.__init__(self, getParentEvent)

		self.button.SetUpVisual("d:/ymir work/ui/game/cube/cube_menu_tab1.sub")
		self.button.SetOverVisual("d:/ymir work/ui/game/cube/cube_menu_tab1.sub")
		self.button.SetDownVisual("d:/ymir work/ui/game/cube/cube_menu_tab1.sub")

		self.SetType(1)

	def SetCategoria(self,index,name):
		self.SetName(name)
		self.IndexCategoria(index)

	def AppendSubCategorias(self, date_cube):
		subcategorias = CubeRenewalSubCategoriaItem(self.getParentEvent)
		subcategorias.AppendIndex(date_cube)
		subcategorias.LoadInfo()
		subcategorias.Select()
		self.SubCategorias.append(subcategorias)

	def GetListSubCategorias(self):
		return self.SubCategorias

	def IndexCategoria(self,index):
		self.index = index

	def GetIndexCategoria(self):
		return self.index

	def FindVnumSubCategoria(self, key):
		list = filter(lambda argMember, argKey=key: argMember.IsSameVnum(argKey), self.SubCategorias)
		if list:
			return list[0]

		return None

	def Open(self):
		self.image.LoadImage("d:/ymir work/ui/game/cube/cube_menu_tab1_minus.sub")
		self.OpenFunc = True

	def Close(self):
		self.image.LoadImage("d:/ymir work/ui/game/cube/cube_menu_tab1_plus.sub")
		self.OpenFunc = False
		map(ui.Window.Hide, self.SubCategorias)

	def IsOpen(self):
		return self.OpenFunc

	def Select(self):
		if self.OpenFunc:
			self.Close()
		else:
			self.Open()

		for x in self.GetListSubCategorias():
			x.Select()

		self.getParentEvent().OnRefresh()
		
class CubeRenewalSubCategoriaItem(CubeRenewalItem):

	def __init__(self, getParentEvent):

		self.OpenFunc = False
		self.SubCategoriaObjects = []

		CubeRenewalItem.__init__(self, getParentEvent)

		self.button.SetUpVisual("d:/ymir work/ui/game/cube/cube_menu_tab2.sub")
		self.button.SetOverVisual("d:/ymir work/ui/game/cube/cube_menu_tab2.sub")
		self.button.SetDownVisual("d:/ymir work/ui/game/cube/cube_menu_tab2.sub")

		self.image.SetPosition(7,4)
		self.text.SetPosition(25,2)
		self.text.SetPackedFontColor(0xffa28b5f)

		self.SetType(2)

	def LoadInfo(self):
		item.SelectItem(self.GetVnumReward())
		self.SetName(item.GetItemName())

	def AppendSubCategoriaObjects(self, date_cube):
		subcategoriaobj = CubeRenewalSubCategoriaObjects(self.getParentEvent)
		subcategoriaobj.AppendIndex(date_cube)
		subcategoriaobj.LoadInfo()
		self.SubCategoriaObjects.append(subcategoriaobj)

	def IsSameVnum(self, vnum):
		return self.GetVnumReward() == vnum

	def Open(self):
		self.image.LoadImage("d:/ymir work/ui/game/cube/cube_menu_tab2_minus.sub")
		self.OpenFunc = True

	def Close(self):
		self.image.LoadImage("d:/ymir work/ui/game/cube/cube_menu_tab2_plus.sub")
		self.OpenFunc = False
		map(ui.Window.Hide, self.SubCategoriaObjects)

	def IsOpen(self):
		return self.OpenFunc

	def GetListSubCategoriasObjects(self):
		return self.SubCategoriaObjects

	def Select(self):
		if self.OpenFunc:
			self.Close()
		else:
			self.Open()

		self.getParentEvent().OnRefresh()

class CubeRenewalSubCategoriaObjects(CubeRenewalItem):
	def __init__(self, getParentEvent):
		CubeRenewalItem.__init__(self, getParentEvent)

		self.button.SetUpVisual("d:/ymir work/ui/game/cube/cube_menu_tab3_default.sub")
		self.button.SetOverVisual("d:/ymir work/ui/game/cube/cube_menu_tab3_select.sub")
		self.button.SetDownVisual("d:/ymir work/ui/game/cube/cube_menu_tab3_select.sub")
		self.SetType(3)
		self.text.SetPosition(17,2)

	def LoadInfo(self):
		item.SelectItem(self.GetVnumReward())
		self.SetName(item.GetItemName()+' (%d%%)'%(self.GetPorcentaje()))
		self.CountTotal()

	def CountTotal(self):
		count_items = []
		count_all_items = 0

		for i in xrange(1,6):
			if self.GetVnumMaterial(i) != 0:
				if player.GetItemCountByVnum(self.GetVnumMaterial(i)) >= self.GetCountMaterial(i):
					number = player.GetItemCountByVnum(self.GetVnumMaterial(i)) / (self.GetCountMaterial(i))
					count_items.append(number)
	
				if app.ENABLE_EXTRA_INVENTORY:
					if player.GetItemCountbyVnumExtraInventory(self.GetVnumMaterial(i)) >= self.GetCountMaterial(i):
						number = player.GetItemCountbyVnumExtraInventory(self.GetVnumMaterial(i)) / (self.GetCountMaterial(i))
						count_items.append(number)
						
				count_all_items += 1

		if len(count_items) >= count_all_items:
			self.SetCountTotal(min(count_items))
		else:
			self.SetCountTotal(0)

	def UpdatePorcentaje(self):
		item.SelectItem(self.GetVnumReward())
		porcentaje_actual = 0

		if self.GetPorcentUpdate() != 0:
			if self.GetPorcentaje()+self.GetPorcentUpdate() > 100:
				porcentaje_actual = 100
			else:
				porcentaje_actual = self.GetPorcentaje()+self.GetPorcentUpdate()
		else:
			porcentaje_actual = self.GetPorcentaje()

		self.SetName(item.GetItemName()+' (%d%%)'%(porcentaje_actual))

	def GetPorcentUpdate(self):
		return self.getParentEvent().GetPorcentNew()

	def UnSelect(self):
		self.button.SetUp()
		self.button.Enable()

	def SetSelect(self):
		self.button.Down()
		self.button.Disable()

	def Select(self):
		self.getParentEvent().SelectObject(self)

class CubeRenewalWindows(ui.ScriptWindow):

	def __init__(self):
		ui.ScriptWindow.__init__(self)
		cube_renewal.SetCubeRenewalHandler(self)

		self.categoria = [
							[
								1,
								"WEAPON",
								localeinfo.CUBE_CATEGORY1
							],
							[
								2 ,
								"ARMOR",
								localeinfo.CUBE_CATEGORY2
							],
							[
								3 ,
								"JEWELRY",
								localeinfo.CUBE_CATEGORY3
							],
							[
								4,
								"ETC",
								localeinfo.CUBE_CATEGORY4
							],
							[
								5,
								"Bracciali_LV90",
								localeinfo.CUBE_CATEGORY5
							],
							[
								6,
								"JEWELRY_LV100",
								localeinfo.CUBE_CATEGORY6
							],
							[
								7,
								"Cinture_PvP",
								localeinfo.CUBE_CATEGORY7
							],
							[
								8,
								"Cinture_PvM",
								localeinfo.CUBE_CATEGORY8
							],
							[
								9,
								"Collane_PvP",
								localeinfo.CUBE_CATEGORY9
							],
							[
								10,
								"Orecchini_PvP",
								localeinfo.CUBE_CATEGORY10
							],
							[
								11,
								"Armi_90",
								localeinfo.CUBE_CATEGORY11
							],
							[
								12,
								"Armi_100",
								localeinfo.CUBE_CATEGORY12
							],
							[
								13,
								"Armi_110",
								localeinfo.CUBE_CATEGORY13
							],
							[
								14,
								"Armi_110_PvM",
								localeinfo.CUBE_CATEGORY14
							],
							[
								15,
								"Armature_PvM_80",
								localeinfo.CUBE_CATEGORY15
							],
							[
								16,
								"Armature_PvM_90",
								localeinfo.CUBE_CATEGORY16
							],
							[
								17,
								"Armature_PvM_110",
								localeinfo.CUBE_CATEGORY17
							],
							[
								18,
								"Armature_PvP_90",
								localeinfo.CUBE_CATEGORY18
							],
							[
								19,
								"Armature_PvP_105",
								localeinfo.CUBE_CATEGORY19
							],
							[
								20,
								"Armature_PvP_110",
								localeinfo.CUBE_CATEGORY20
							],
							[
								21,
								"Scarpe_PvP_105",
								localeinfo.CUBE_CATEGORY21
							],
							[
								22,
								"Scarpe_PvM_100",
								localeinfo.CUBE_CATEGORY22
							],
							[
								23,
								"Elmi_pvp_115",
								localeinfo.CUBE_CATEGORY23
							],
							[
								24,
								"Elmi_pvm_80",
								localeinfo.CUBE_CATEGORY24
							],
							[
								25,
								"Elmi_pvm_95",
								localeinfo.CUBE_CATEGORY25
							],
							[
								26,
								"Elmi_pvp_105",
								localeinfo.CUBE_CATEGORY26
							],
							[
								27,
								"Mount_craft",
								localeinfo.CUBE_CATEGORY27
							],
							[
								28,
								"Pet_craft",
								localeinfo.CUBE_CATEGORY28
							],
							[
								29,
								"pietre_perforazione",
								localeinfo.CUBE_CATEGORY29
							],
							[
								30,
								"pietre_Colpo_Mort",
								localeinfo.CUBE_CATEGORY30
							],
							[
								31,
								"pietre_Ripetiz",
								localeinfo.CUBE_CATEGORY31
							],
							[
								32,
								"pietre_Guerriero",
								localeinfo.CUBE_CATEGORY32
							],
							[
								33,
								"pietre_Ninja",
								localeinfo.CUBE_CATEGORY33
							],
							[
								34,
								"pietre_sura",
								localeinfo.CUBE_CATEGORY34
							],
							[
								36,
								"pietre_Shamana",
								localeinfo.CUBE_CATEGORY35
							],
							[
								37,
								"pietre_Mostro",
								localeinfo.CUBE_CATEGORY36
							],
							[
								38,
								"pietre_Evasione",
								localeinfo.CUBE_CATEGORY37
							],
							[
								39,
								"pietre_Inchino",
								localeinfo.CUBE_CATEGORY38
							],
							[
								40,
								"pietre_Magia",
								localeinfo.CUBE_CATEGORY39
							],
							[
								41,
								"pietre_Vitalità",
								localeinfo.CUBE_CATEGORY40
							],
							[
								42,
								"pietre_Difesa",
								localeinfo.CUBE_CATEGORY41
							],
							[
								43,
								"pietre_Fretta",
								localeinfo.CUBE_CATEGORY42
							],
							[
								44,
								"christmas2021_effects1",
								localeinfo.CUBE_CATEGORY43
							],
							[
								45,
								"christmas2021_effects2",
								localeinfo.CUBE_CATEGORY44
							],
							[
								46,
								"christmas2021_pet",
								localeinfo.CUBE_CATEGORY45
							],
							[
								47,
								"christmas2021_mounts",
								localeinfo.CUBE_CATEGORY46
							],
							[
								48,
								"christmas2021_costumes_stole",
								localeinfo.CUBE_CATEGORY47
							],
							[
								49,
								"christmas2021_costumes_hair",
								localeinfo.CUBE_CATEGORY48
							],
							[
								50,
								"christmas2021_costumes_armor",
								localeinfo.CUBE_CATEGORY49
							],
							[
								51,
								"christmas2021_costumes_weapons",
								localeinfo.CUBE_CATEGORY50
							],
							[
								52,
								"Accesory_PVM",
								localeinfo.CUBE_CATEGORY51
							],
							[
								999,
								"WORLDARD",
								localeinfo.CUBE_CATEGORY999
							]
		]

		self.showingItemList = []
		self.categoria_list = []
		self.startLine = 0
		self.MaxElements = 12
		self.cube_elements = {}
		self.selectedItem = None
		self.count_item_reward = 0

		self.vnum_item_improve = 79605
		self.max_count_item_improve = 40
		self.slot_item_improve = -1


		self.toolTip = uitooltip.ItemToolTip()
		self.LoadWindow()

	def __del__(self):
		if self.questionDialog:
			self.questionDialog.Close()
			self.questionDialog = None
		
		cube_renewal.SetCubeRenewalHandler(None)
		ui.ScriptWindow.__init__(self)

	def LoadWindow(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "uiscript/cuberenewalwindow.py")
		except:
			import exception
			exception.Abort("CubeRenewalWindows.LoadWindow.LoadObject")

		try:
			self.board = self.GetChild("item_list_board")
			self.item_slot = self.GetChild("item_slot")
			self.cube_list_scroll_bar = self.GetChild("cube_list_scroll_bar")
			self.yang_text = self.GetChild("yang_text")
			self.result_qty = self.GetChild("result_qty")
			self.button_ok = self.GetChild("button_ok")
			self.button_cancel = self.GetChild("button_cancel")

			if app.ENABLE_GAYA_SYSTEM:
				self.gaya_text = self.GetChild("gaya_text")

			self.qty_sub_button = self.GetChild("qty_sub_button")
			self.qty_add_button = self.GetChild("qty_add_button")

			self.imporve_slot = self.GetChild("imporve_slot")
			
			self.GetChild("board").SetCloseEvent(ui.__mem_func__(self.Close))

			for i in xrange(1,6):
				self.cube_elements["count_elements_%d"%i] = self.GetChild("material_qty_text_%d"%i)

		except:
			import exception
			exception.Abort("CubeRenewalWindows.LoadWindow.LoadElements")

		self.slot_improve = ui.SlotWindow()
		self.slot_improve.SetParent(self.imporve_slot)
		self.slot_improve.SetSize(32,32)
		self.slot_improve.SetPosition(6,5)
		self.slot_improve.SetSelectEmptySlotEvent(ui.__mem_func__(self.__OnSelectEmptySlot))
		self.slot_improve.SetSelectItemSlotEvent(ui.__mem_func__(self.__OnSelectItemSlot))
		self.slot_improve.AppendSlot(0,0,0,32,32)
		self.slot_improve.Show()

		self.button_cancel.SetEvent(self.Close)

		self.button_ok.SetEvent(self.AceptCube)
		self.result_qty.SetReturnEvent(self.AceptInputItem)
		self.result_qty.SetEscapeEvent(ui.__mem_func__(self.Close))
		self.result_qty.CanEdit(False)

		self.qty_sub_button.SetEvent(self.QtySubButton)
		self.qty_add_button.SetEvent(self.QtyAddButton)

		self.cube_list_scroll_bar.SetScrollEvent(ui.__mem_func__(self.OnScroll))

		self.icons_items = ui.SlotWindow()
		self.icons_items.SetParent(self.item_slot)
		self.icons_items.SetPosition(25,13)
		self.icons_items.SetSize((32*9),32*3)
		self.icons_items.SetOverInItemEvent(ui.__mem_func__(self.OverInItem))
		self.icons_items.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))
		self.icons_items.AppendSlot(0,0,0,32,32*3)

		for i in xrange(0,6):
			self.icons_items.AppendSlot(i+1,46*i+62,0,32,32*3)

		self.icons_items.Show()
		questionDialog = uicommon.QuestionDialog()
		questionDialog.SetWidth(350)
		questionDialog.SetText(localeinfo.CUBE_LOSE_BONUSES_ALERT)
		questionDialog.SetAcceptEvent(ui.__mem_func__(self.OnAcceptQuestionDialog))
		questionDialog.SetCancelEvent(ui.__mem_func__(self.OnCloseQuestionDialog))
		questionDialog.Close()
		self.questionDialog = questionDialog

	def OnRunMouseWheel(self, nLen):
		if self.cube_list_scroll_bar and self.cube_list_scroll_bar.IsShow():
			x, y = self.GetGlobalPosition()
			xMouse, yMouse = wndMgr.GetMousePosition()
			difX = xMouse - x
			difY = yMouse - y
			if difX >= 6 and difX <= 328 and difY >= 6 and difY <= 476:
				if nLen > 0:
					self.cube_list_scroll_bar.OnUp()
					return True
				else:
					self.cube_list_scroll_bar.OnDown()
					return True
		
		return False

	def BINARY_CUBE_RENEWAL_LOADING(self):
		self.ClearElements()
		self.LoadCategoria()
		self.LoadSubCategoria()
		self.LoadSubObjects()
		self.OnRefresh()

	def BINARY_CUBE_RENEWAL_UPDATE(self):
		if self.selectedItem:
			self.UpdateCountSubCategoria()
			self.UpdateMaterialCount()

	def ClearElements(self):
		if len(self.showingItemList) > 0:
			map(ui.Window.Hide, self.showingItemList)

		for group in self.categoria_list:
			group.Close()

			for categorias in group.GetListSubCategorias():
				categorias.Close()

		for i in xrange(0,6):
			self.icons_items.ClearSlot(i)
			if i > 0:
				self.cube_elements["count_elements_%d"%i].Hide()

		self.yang_text.Hide()
		if app.ENABLE_GAYA_SYSTEM:
			self.gaya_text.Hide()
		self.result_qty.Hide()

		self.categoria_list = []
		self.startLine = 0

		self.cube_list_scroll_bar.SetPos(0)

	def CheckListCategoriasTotal(self):
		list1 = []
		list2 = []

		for i2 in xrange(cube_renewal.CountDates()):
			list1.append(self.CheckCategoryIndex(FReturnInfo("category",i2)))

		for ia in list1:
			if ia not in list2:
				list2.append(ia)

		return list2

	def LoadCategoria(self):
		world = self.CheckListCategoriasTotal()
		for i in world:
			for i1 in xrange(len(self.categoria)):
				if self.categoria[i1][0] == i:
					member = CubeRenewalCategoriaItem(ui.__mem_func__(self.GetSelf))
					member.SetCategoria(self.categoria[i1][0],self.categoria[i1][2])
					member.Close()
					member.Show()
					self.categoria_list.append(member)

	def CheckCategoryIndex(self,name):
		vnum_default = 0
		for i in xrange(len(self.categoria)):
			if name == self.categoria[i][1]:
				return self.categoria[i][0]
		for i in xrange(len(self.categoria)):
			if "WORLDARD" == self.categoria[i][1]:
				return self.categoria[i][0]

	def LoadSubCategoria(self):
		for group in self.categoria_list:
			for i in xrange(cube_renewal.CountDates()):
				if self.CheckCategoryIndex(FReturnInfo("category",i)) == group.GetIndexCategoria():
					if not group.FindVnumSubCategoria(FReturnInfo("vnum_reward",i)):
						group.AppendSubCategorias(i)

	def LoadSubObjects(self):
		for group in self.categoria_list:
			for i in xrange(cube_renewal.CountDates()):
				if self.CheckCategoryIndex(FReturnInfo("category",i)) == group.GetIndexCategoria():
					for categorias in group.GetListSubCategorias():
						if FReturnInfo("vnum_reward",i) == categorias.GetVnumReward():
							categorias.AppendSubCategoriaObjects(i)

	def OnScroll(self):
		scrollLineCount = len(self.showingItemList) - (self.MaxElements)
		startLine = int(scrollLineCount * self.cube_list_scroll_bar.GetPos())


		if startLine != self.startLine:
			self.startLine = startLine
			self.SetExtraInfo()

	def __OnSelectEmptySlot(self,selectedSlotPos):
		isAttached = mousemodule.mouseController.isAttached()

		if not self.selectedItem:
			return

		if self.selectedItem.GetPorcentaje() >= 100:
			return

		if isAttached:
			attachedSlotType = mousemodule.mouseController.GetAttachedType()
			attachedSlotPos = mousemodule.mouseController.GetAttachedSlotNumber()
			mousemodule.mouseController.DeattachObject()


			itemVNum = player.GetItemIndex(attachedSlotPos)
			itemCount = player.GetItemCount(attachedSlotPos)

			if itemVNum == self.vnum_item_improve and itemCount <= self.max_count_item_improve:
				self.slot_improve.SetItemSlot(selectedSlotPos,itemVNum,itemCount)
				self.slot_item_improve = attachedSlotPos

	def __OnSelectItemSlot(self,selectedSlotPos):
		isAttached = mousemodule.mouseController.isAttached()

		if not self.selectedItem:
			return

		if self.selectedItem.GetPorcentaje() >= 100:
			return

		if isAttached:
			attachedSlotType = mousemodule.mouseController.GetAttachedType()
			attachedSlotPos = mousemodule.mouseController.GetAttachedSlotNumber()
			mousemodule.mouseController.DeattachObject()

			itemVNum = player.GetItemIndex(attachedSlotPos)
			itemCount = player.GetItemCount(attachedSlotPos)

			if itemVNum == self.vnum_item_improve and itemCount <= self.max_count_item_improve:
				self.slot_improve.SetItemSlot(selectedSlotPos,itemVNum,itemCount)
				self.slot_item_improve = attachedSlotPos
		else:
			self.slot_improve.SetItemSlot(0,0,0)
			self.slot_item_improve = -1

	def OnRefresh(self):
		self.showingItemList = []

		for group in self.categoria_list:
			self.showingItemList.append(group)

			if group.IsOpen():
				for categorias in group.GetListSubCategorias():
					self.showingItemList.append(categorias)

					if categorias.IsOpen():
						for a in categorias.GetListSubCategoriasObjects():
							self.showingItemList.append(a)

		self.SetExtraInfo()

	def SetExtraInfo(self):
		if self.MaxElements >= len(self.showingItemList):
			self.cube_list_scroll_bar.Hide()
			self.startLine = 0
		else:
			if self.showingItemList:
				self.cube_list_scroll_bar.SetMiddleBarSize(float(self.MaxElements) / float(len(self.showingItemList)))
			self.cube_list_scroll_bar.Show()

		map(ui.Window.Hide, self.showingItemList)

		yPos = 11
		heightLimit = 240

		for item in self.showingItemList[self.startLine:]:
			XPos = 0
			if item.GetType() == 2:
				XPos += 15
			if item.GetType() == 3:
				XPos += 35
			item.SetPosition(6 + XPos, yPos)
			item.SetTop()
			item.Show()

			yPos += 20

			if yPos > heightLimit:
				break

	def SelectObject(self,item):
		self.selectedItem = item
		if self.selectedItem:	
			self.UpdateInfoSelect()
			self.UpdateSelectSubCategoria()

	def UpdateInfoSelect(self):

		self.icons_items.ClearSlot(0)
		self.icons_items.SetItemSlot(0,self.selectedItem.GetVnumReward(),self.selectedItem.GetCountReward())

		for i in xrange(1,6):
			self.icons_items.ClearSlot(i)
			self.icons_items.SetItemSlot(i, self.selectedItem.GetVnumMaterial(i), self.selectedItem.GetCountMaterial(i))


		self.result_qty.KillFocus()
		self.result_qty.CanEdit(self.selectedItem.GetItemStackable())
		self.result_qty.SetText("%s"%(self.selectedItem.GetCountReward()))
		self.result_qty.Show()

		self.count_item_reward = self.selectedItem.GetCountReward()
	
		self.UpdateMaterialCount()
		self.UpdateSlotItemImprove()

	def UpdateSlotItemImprove(self):
		if self.selectedItem.GetPorcentaje() >= 100 and self.slot_item_improve != -1:
			self.slot_improve.SetItemSlot(0,0,0)
			self.slot_item_improve = -1

	def UpdateMaterialCount(self):
		count = (self.count_item_reward/(self.selectedItem.GetCountReward()))

		for i in xrange(1,6):
			if self.selectedItem.GetVnumMaterial(i) != 0:
				if player.GetItemCountByVnum(self.selectedItem.GetVnumMaterial(i)) >= (self.selectedItem.GetCountMaterial(i)*count):
					self.cube_elements["count_elements_%d"%i].SetPackedFontColor(grp.GenerateColor(0.5411, 0.7254, 0.5568, 1.0))
				else:
					if app.ENABLE_EXTRA_INVENTORY:
						if player.GetItemCountbyVnumExtraInventory(self.selectedItem.GetVnumMaterial(i)) >= (self.selectedItem.GetCountMaterial(i)*count):
							self.cube_elements["count_elements_%d"%i].SetPackedFontColor(grp.GenerateColor(0.5411, 0.7254, 0.5568, 1.0))
						else:
							self.cube_elements["count_elements_%d"%i].SetPackedFontColor(grp.GenerateColor(0.9, 0.4745, 0.4627, 1.0))
					else:
						self.cube_elements["count_elements_%d"%i].SetPackedFontColor(grp.GenerateColor(0.9, 0.4745, 0.4627, 1.0))
						
				count_total_refine = 0
				count_total_refine = player.GetItemCountByVnum(self.selectedItem.GetVnumMaterial(i))
				if count_total_refine == 0:
					count_total_refine = player.GetItemCountbyVnumExtraInventory(self.selectedItem.GetVnumMaterial(i))
				
				self.cube_elements["count_elements_%d"%i].SetText("%d/%d"%(count_total_refine,self.selectedItem.GetCountMaterial(i)*count))
				self.cube_elements["count_elements_%d"%i].Show()
			else:
				self.cube_elements["count_elements_%d"%i].Hide()

		self.yang_text.SetText(localeinfo.NumberToMoneyString(self.selectedItem.GetGold()*count))
		self.yang_text.Show()
		if player.GetElk() >= int(self.selectedItem.GetGold()*count):
			self.yang_text.SetPackedFontColor(grp.GenerateColor(0.7607, 0.7607, 0.7607, 1.0))
		else:
			self.yang_text.SetPackedFontColor(grp.GenerateColor(0.9, 0.4745, 0.4627, 1.0))

		if app.ENABLE_GAYA_SYSTEM:
			self.gaya_text.SetText(localeinfo.NumberToGayaString(self.selectedItem.GetGaya()*count))
			self.gaya_text.Show()
			if player.GetGaya() >= int(self.selectedItem.GetGaya()*count):
				self.gaya_text.SetPackedFontColor(grp.GenerateColor(0.7607, 0.7607, 0.7607, 1.0))
			else:
				self.gaya_text.SetPackedFontColor(grp.GenerateColor(0.9, 0.4745, 0.4627, 1.0))

	def ConvertInputResult(self):
		edit = int(self.result_qty.GetText())
		total = self.selectedItem.GetCountReward()
		convert = edit % total
		return convert

	def AceptInputItem(self):
		self.result_qty.KillFocus()

		result_total = int(self.result_qty.GetText())

		if int(self.result_qty.GetText()) < self.selectedItem.GetCountReward():
			result_total = self.selectedItem.GetCountReward()
		elif result_total > 200:
			result_total = 200
		else:
			result = self.ConvertInputResult()
			if result != 0:
				result_total = result_total-result

		self.count_item_reward = result_total
		self.result_qty.SetText("%d"%(result_total))

		count = (self.count_item_reward/(self.selectedItem.GetCountReward()))

		for i in xrange(0,6):
			self.icons_items.ClearSlot(i)
			if i == 0:
				self.icons_items.SetItemSlot(i,self.selectedItem.GetVnumReward(),self.count_item_reward)
			else:
				self.icons_items.SetItemSlot(i, self.selectedItem.GetVnumMaterial(i), self.selectedItem.GetCountMaterial(i)*count)

	def OnCloseQuestionDialog(self):
		if self.questionDialog and self.questionDialog.IsShow():
			self.questionDialog.Close()

	def OnAcceptQuestionDialog(self):
		if not self.questionDialog:
			return
		
		if self.selectedItem:
			index = self.selectedItem.GetIndex()
			self.CheckInputFocus()
			count = (self.count_item_reward/(self.selectedItem.GetCountReward()))
			index_improve = self.slot_item_improve
			cube_renewal.CubeRenewalMakeItem(index, count, index_improve)
		
		self.OnCloseQuestionDialog()

	def OnTop(self):
		if self.questionDialog and self.questionDialog.IsShow():
			self.questionDialog.SetTop()

	def AceptCube(self):
		if self.selectedItem:
			if self.selectedItem.GetVnumReward() in [300, 310, 1180, 2200, 3220, 5160, 7300, 19290, 19490, 19690, 19890]:
				self.OnCloseQuestionDialog()
				if self.questionDialog:
					self.questionDialog.Open()
			else:
				index = self.selectedItem.GetIndex()
				self.CheckInputFocus()
				count = (self.count_item_reward/(self.selectedItem.GetCountReward()))
				index_improve = self.slot_item_improve
				cube_renewal.CubeRenewalMakeItem(index,count,index_improve)

	def QtyAddButton(self):
		if self.selectedItem and self.selectedItem.GetItemStackable():
			self.result_qty.SetText("%d"%(self.selectedItem.GetCountReward()*self.selectedItem.GetCount()))
			self.AceptInputItem()

	def QtySubButton(self):
		if self.selectedItem and self.selectedItem.GetItemStackable():
			self.result_qty.SetText("%d"%(self.selectedItem.GetCountReward()))
			self.AceptInputItem()

	def CheckInputFocus(self):
		if self.result_qty.IsFocus():
			self.result_qty.SetText("%d"%(self.selectedItem.GetCountReward()))
			self.AceptInputItem()

	def OnSelectItem(self, item):
		self.selectedItem = item

		if self.selectedItem:
			self.selectedItem.Select()

	def OnUpdate(self):
		if self.showingItemList:
			self.UpdateCountSubCategoria()
		if self.selectedItem:
			self.UpdateMaterialCount()

		index_improve = self.slot_item_improve
		if index_improve != -1:
			itemVNum = player.GetItemIndex(index_improve)
			itemCount = player.GetItemCount(index_improve)
			if itemVNum == self.vnum_item_improve and itemCount <= self.max_count_item_improve:
				self.slot_improve.SetItemSlot(0,itemVNum,itemCount)
			else:
				self.slot_improve.SetItemSlot(0,0,0)
				self.slot_item_improve = -1
		else:

			self.slot_improve.SetItemSlot(0,0,0)
			self.slot_item_improve = -1

	def GetPorcentNew(self):
		index_improve = self.slot_item_improve
		if index_improve != -1:
			itemVNum = player.GetItemIndex(index_improve)
			itemCount = player.GetItemCount(index_improve)
			if itemVNum == self.vnum_item_improve and itemCount <= self.max_count_item_improve:
				return itemCount
		return 0

	def UpdateCountSubCategoria(self):
		for group in self.categoria_list:
			if group.IsOpen():
				for categorias in group.GetListSubCategorias():
					if categorias.IsOpen():
						for a in categorias.GetListSubCategoriasObjects():
							a.CountTotal()
							a.UpdatePorcentaje()

	def UpdateSelectSubCategoria(self):
		for group in self.categoria_list:
			for categorias in group.GetListSubCategorias():
				for a in categorias.GetListSubCategoriasObjects():
					a.UnSelect()
		self.selectedItem.SetSelect()

	def GetBoard(self):
		return self.board

	def GetSelf(self):
		return self		

	def OverInItem(self, index):
		self.toolTip.ClearToolTip()

		if self.selectedItem:
			if index == 0:
				vnum = self.selectedItem.GetVnumReward()
			else:
				vnum = self.selectedItem.GetVnumMaterial(index)


		if app.ENABLE_CUBE_RENEWAL_COPY_WORLDARD:
			subtype_item_inv = None
			check_item = -1

			for i in xrange(player.INVENTORY_PAGE_SIZE*4):
				item_vnum = player.GetItemIndex(i)

				if item_vnum == self.selectedItem.GetIndexCopy() and check_item == -1 and index == 0 and self.selectedItem.GetIndexCopy() != 0:
					item.SelectItem(item_vnum)
					subtype_item_inv = item.GetItemSubType()
					item.SelectItem(self.selectedItem.GetVnumReward())

					if item.GetItemType() == item.ITEM_TYPE_ARMOR or item.GetItemType() == item.ITEM_TYPE_WEAPON and item.GetItemSubType() == subtype_item_inv:
						check_item = i

			metinSlot = []
			AttrList = []

			if check_item != 1 and index == 0:
				for i in xrange(player.METIN_SOCKET_MAX_NUM):
					metinSlot.append(player.GetItemMetinSocket(check_item,i))

				for i in xrange(player.ATTRIBUTE_SLOT_MAX_NUM):
					AttrList.append(player.GetItemAttribute(check_item,i))
			else:
				
				check_item_material = -1

				if self.selectedItem.GetVnumMaterial(index) != 0:
					for i in xrange(player.INVENTORY_PAGE_SIZE*4):
						item_vnum = player.GetItemIndex(i)

						if item_vnum == self.selectedItem.GetVnumMaterial(index) and check_item_material == -1:
							check_item_material = i

					if check_item_material != -1:
						for c in xrange(player.METIN_SOCKET_MAX_NUM):
							metinSlot.append(player.GetItemMetinSocket(check_item_material,c))

						for b in xrange(player.ATTRIBUTE_SLOT_MAX_NUM):
							AttrList.append(player.GetItemAttribute(check_item_material,b))
					else:

						for c in xrange(player.METIN_SOCKET_MAX_NUM):
							metinSlot.append(0)

						for b in xrange(player.ATTRIBUTE_SLOT_MAX_NUM):
							AttrList.append((0,0))

		else:
			metinSlot = []
			for i in xrange(player.METIN_SOCKET_MAX_NUM):
				metinSlot.append(0)

			AttrList = []
			for i in xrange(player.ATTRIBUTE_SLOT_MAX_NUM):
				AttrList.append((0,0))

		lock_index = -1
		if app.ATTR_LOCK:
			for i in xrange(player.INVENTORY_PAGE_SIZE*4):
				item_vnum = player.GetItemIndex(i)

				if item_vnum == vnum and lock_index == -1:
					item.SelectItem(item_vnum)
					subtype_item_inv = item.GetItemSubType()
					item.SelectItem(self.selectedItem.GetVnumReward())

					if item.GetItemType() == item.ITEM_TYPE_ARMOR or item.GetItemType() == item.ITEM_TYPE_WEAPON and item.GetItemSubType() == subtype_item_inv:
						lock_index = i
			
		lockedattr = -1
		if lock_index != -1:
			lockedattr = player.GetItemAttrLocked(player.INVENTORY, lock_index)
		self.toolTip.AddItemData(vnum,metinSlot,AttrList,0,0,player.INVENTORY,-1,lockedattr)


		

	def OverOutItem(self):
		self.toolTip.Hide()

	def OnPressEscapeKey(self):
		self.Close()
		return True

	def Close(self):
		self.OnCloseQuestionDialog()
		cube_renewal.CubeRenewalClose()
		self.slot_item_improve = -1
		self.selectedItem = None
		self.Hide()

