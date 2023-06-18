import ui
import net
import app
import item
import wndMgr
import constinfo
import localeinfo
import uiscriptlocale
import uitooltip
import nonplayer
import player
import chat
import time
import grp
import chr
import event
import game

IMG_PATH = "d:/ymir work/ui/public/teleportpanel/"
MAP_IMG_PATH = "d:/ymir work/ui/public/teleportpanel/map_img/"
SLIDER_IMG_PATH = "d:/ymir work/ui/public/teleportpanel/slider/"

SLIDER_TIME_TO_REFRESH = 10			# in seconds
SLIDER_ANI_DURATION = 3 				# in seconds
SLIDER_DICT = {
	0 : SLIDER_IMG_PATH + "picture_1.tga",
	1 : SLIDER_IMG_PATH + "picture_2.tga",
	2 : SLIDER_IMG_PATH + "picture_3.tga",
	3 : SLIDER_IMG_PATH + "picture_4.tga",
}

# Note: Always write the map pictures in the MAP_DICT without the additions "_normal.tga", "_hover.tga" and "_down.tga" !!!!

MAP_DICT = {
	"EMPIRES" : {
		0 : {
			"map_img" : "empires_map1", "desc_text" : uiscriptlocale.TELEPORT_MAP_MAP_1, "map_desc" : uiscriptlocale.TELEPORT_MAP_MAP_1_DESC, 
			"boss_vnum" : 0, "min_lv" : 1, "max_lv" : 120, "money_cost" : 10000, "item_cost" : 0, "item_cost_count" : 0,
			"BUTTONS" : { # Max Buttons = 3
				0 : {"command" : "m1_red" , "button_text" : uiscriptlocale.TELEPORT_BUTTON_RED_EMPIRE, "button_text_color" : 0xFFFF2828, "button_disable_text_color" : 0xFFB21B1B },
				1 : {"command" : "m1_yel" , "button_text" : uiscriptlocale.TELEPORT_BUTTON_YELLOW_EMPIRE, "button_text_color" : 0xFFFFD903, "button_disable_text_color" : 0xFFBB9F00 },
				2 : {"command" : "m1_blu" , "button_text" : uiscriptlocale.TELEPORT_BUTTON_BLUE_EMPIRE, "button_text_color" : 0xFF007BFF, "button_disable_text_color" : 0xFF0050A6 },
			},
		},
		1 : {
			"map_img" : "empires_map2", "desc_text" : uiscriptlocale.TELEPORT_MAP_MAP_2, "map_desc" : uiscriptlocale.TELEPORT_MAP_MAP_2_DESC,  
			"boss_vnum" : 0, "min_lv" : 1, "max_lv" : 120, "money_cost" : 10000, "item_cost" : 0, "item_cost_count" : 0,
			"BUTTONS" : { # Max Buttons = 3
				0 : {"command" : "m2_red" , "button_text" : uiscriptlocale.TELEPORT_BUTTON_RED_EMPIRE, "button_text_color" : 0xFFFF2828, "button_disable_text_color" : 0xFFB21B1B },
				1 : {"command" : "m2_yel" , "button_text" : uiscriptlocale.TELEPORT_BUTTON_YELLOW_EMPIRE, "button_text_color" : 0xFFFFD903, "button_disable_text_color" : 0xFFBB9F00 },
				2 : {"command" : "m2_blu" , "button_text" : uiscriptlocale.TELEPORT_BUTTON_BLUE_EMPIRE, "button_text_color" : 0xFF007BFF, "button_disable_text_color" : 0xFF0050A6 },
			}
		},
		2 : {
			"map_img" : "empires_guilds", "desc_text" : uiscriptlocale.TELEPORT_MAP_GUILDZONE, "map_desc" : uiscriptlocale.TELEPORT_MAP_GUILDZONE_DESC, 
			"boss_vnum" : 0, "min_lv" : 20, "max_lv" : 120, "money_cost" : 25000, "item_cost" : 0, "item_cost_count" : 0,
			"BUTTONS" : { # Max Buttons = 3
				0 : {"command" : "guildland" , "button_text" : uiscriptlocale.TELEPORT_BUTTON_START, "button_text_color" : 0, "button_disable_text_color" : 0 },
			}
		},
		3 : {
			"map_img" : "empires_map1", "desc_text" : uiscriptlocale.TELEPORT_MAP_ANDRA, "map_desc" : "",  
			"boss_vnum" : 0, "min_lv" : 1, "max_lv" : 120, "money_cost" : 10000, "item_cost" : 0, "item_cost_count" : 0,
			"BUTTONS" : { # Max Buttons = 3
				0 : {"command" : "andra_map" , "button_text" : uiscriptlocale.TELEPORT_BUTTON_START, "button_text_color" : 0, "button_disable_text_color" : 0 },
			}
		},
	},
	"NEUTRAL" : {
		0 : {
			"map_img" : "neutral_seungryong", "desc_text" : uiscriptlocale.TELEPORT_MAP_SEUNGRYONG, "map_desc" : uiscriptlocale.TELEPORT_MAP_SEUNGRYONG_DESC,  
			"boss_vnum" : 691, "min_lv" : 20, "max_lv" : 120, "money_cost" : 50000, "item_cost" : 0, "item_cost_count" : 0,
			"BUTTONS" : { # Max Buttons = 3
				0 : {"command" : "seungryong_start" , "button_text" : uiscriptlocale.TELEPORT_BUTTON_START, "button_text_color" : 0, "button_disable_text_color" : 0 },
				1 : {"command" : "seungryong_middle" , "button_text" : uiscriptlocale.TELEPORT_BUTTON_MIDDLE, "button_text_color" : 0, "button_disable_text_color" : 0 },
			}
		},
		1 : {
			"map_img" : "neutral_yongbi_desert", "desc_text" : uiscriptlocale.TELEPORT_MAP_YONGBI, "map_desc" : uiscriptlocale.TELEPORT_MAP_YONGBI_DESC,  
			"boss_vnum" : 2191, "min_lv" : 20, "max_lv" : 120, "money_cost" : 50000, "item_cost" : 0, "item_cost_count" : 0,
			"BUTTONS" : { # Max Buttons = 3
				0 : {"command" : "yongbi_start" , "button_text" : uiscriptlocale.TELEPORT_BUTTON_START, "button_text_color" : 0, "button_disable_text_color" : 0 },
				1 : {"command" : "yongbi_middle" , "button_text" : uiscriptlocale.TELEPORT_BUTTON_MIDDLE, "button_text_color" : 0, "button_disable_text_color" : 0 },
			}
		},
		2 : {
			"map_img" : "neutral_mount_sohan", "desc_text" : uiscriptlocale.TELEPORT_MAP_SOHAN, "map_desc" : uiscriptlocale.TELEPORT_MAP_SOHAN_DESC,  
			"boss_vnum" : 1901, "min_lv" : 20, "max_lv" : 120, "money_cost" : 50000, "item_cost" : 0, "item_cost_count" : 0,
			"BUTTONS" : { # Max Buttons = 3
				0 : {"command" : "sohan_start" , "button_text" : uiscriptlocale.TELEPORT_BUTTON_START, "button_text_color" : 0, "button_disable_text_color" : 0 },
				1 : {"command" : "sohan_middle" , "button_text" : uiscriptlocale.TELEPORT_BUTTON_MIDDLE, "button_text_color" : 0, "button_disable_text_color" : 0 },
			}
		},
		3 : {
			"map_img" : "neutral_fireland", "desc_text" : uiscriptlocale.TELEPORT_MAP_DOYYUMHWAN, "map_desc" : uiscriptlocale.TELEPORT_MAP_DOYYUMHWAN_DESC,  
			"boss_vnum" : 2206, "min_lv" : 20, "max_lv" : 120, "money_cost" : 50000, "item_cost" : 0, "item_cost_count" : 0,
			"BUTTONS" : { # Max Buttons = 3
				0 : {"command" : "fireland_start" , "button_text" : uiscriptlocale.TELEPORT_BUTTON_START, "button_text_color" : 0, "button_disable_text_color" : 0 },
				1 : {"command" : "fireland_middle" , "button_text" : uiscriptlocale.TELEPORT_BUTTON_MIDDLE, "button_text_color" : 0, "button_disable_text_color" : 0 },
			}
		},
		# 4 : {
			# "map_img" : "neutral_snakefield", "desc_text" : uiscriptlocale.TELEPORT_MAP_SNAKEFIELD, "map_desc" : uiscriptlocale.TELEPORT_MAP_SNAKEFIELD_DESC,  
			# "boss_vnum" : 0, "min_lv" : 40, "max_lv" : 120, "money_cost" : 75000, "item_cost" : 0, "item_cost_count" : 0,
			# "BUTTONS" : { # Max Buttons = 3
				# 0 : {"command" : "snakefield_start" , "button_text" : uiscriptlocale.TELEPORT_BUTTON_START, "button_text_color" : 0, "button_disable_text_color" : 0 },
				# 1 : {"command" : "snakefield_middle" , "button_text" : uiscriptlocale.TELEPORT_BUTTON_MIDDLE, "button_text_color" : 0, "button_disable_text_color" : 0 },
			# }
		# },
		4 : {
			"map_img" : "neutral_ghostwood", "desc_text" : uiscriptlocale.TELEPORT_MAP_GHOSTWOOD, "map_desc" : uiscriptlocale.TELEPORT_MAP_GHOSTWOOD_DESC,  
			"boss_vnum" : 2306, "min_lv" : 50, "max_lv" : 120, "money_cost" : 250000, "item_cost" : 0, "item_cost_count" : 0,
			"BUTTONS" : { # Max Buttons = 3
				0 : {"command" : "ghostwood_start" , "button_text" : uiscriptlocale.TELEPORT_BUTTON_START, "button_text_color" : 0, "button_disable_text_color" : 0 },
				1 : {"command" : "ghostwood_end" , "button_text" : uiscriptlocale.TELEPORT_BUTTON_END, "button_text_color" : 0, "button_disable_text_color" : 0 },
			}
		},
		5 : {
			"map_img" : "neutral_redwood", "desc_text" : uiscriptlocale.TELEPORT_MAP_REDWOOD, "map_desc" : uiscriptlocale.TELEPORT_MAP_REDWOOD_DESC,  
			"boss_vnum" : 2307, "min_lv" : 50, "max_lv" : 120, "money_cost" : 250000, "item_cost" : 0, "item_cost_count" : 0,
			"BUTTONS" : { # Max Buttons = 3
				0 : {"command" : "redwood_start" , "button_text" : uiscriptlocale.TELEPORT_BUTTON_START, "button_text_color" : 0, "button_disable_text_color" : 0 },
				1 : {"command" : "redwood_end" , "button_text" : uiscriptlocale.TELEPORT_BUTTON_END, "button_text_color" : 0, "button_disable_text_color" : 0 },
			}
		},
		# 7 : {
			# "map_img" : "neutral_land_of_giants", "desc_text" : uiscriptlocale.TELEPORT_MAP_LAND_OF_GIANTS, "map_desc" : uiscriptlocale.TELEPORT_MAP_LAND_OF_GIANTS_DESC,  
			# "boss_vnum" : 0, "min_lv" : 60, "max_lv" : 120, "money_cost" : 250000, "item_cost" : 0, "item_cost_count" : 0,
			# "BUTTONS" : { # Max Buttons = 3
				# 0 : {"command" : "giants_start" , "button_text" : uiscriptlocale.TELEPORT_BUTTON_START, "button_text_color" : 0, "button_disable_text_color" : 0 },
				# 1 : {"command" : "giants_end" , "button_text" : uiscriptlocale.TELEPORT_BUTTON_END, "button_text_color" : 0, "button_disable_text_color" : 0 },
			# }
		# },
	},
	"DUNGEON" : {
		0 : {
			"map_img" : "dungeon_spiderdungeon01", "desc_text" : uiscriptlocale.TELEPORT_MAP_SPIDERDUNGEON_01, "map_desc" : "",  
			"boss_vnum" : 2091, "min_lv" : 40, "max_lv" : 120, "money_cost" : 100000, "item_cost" : 0, "item_cost_count" : 0,
			"BUTTONS" : { # Max Buttons = 3
				0 : {"command" : "sd1_start" , "button_text" : uiscriptlocale.TELEPORT_MAP_SPIDERDUNGEON_01_1, "button_text_color" : 0, "button_disable_text_color" : 0 },
			}
		},
		1 : {
			"map_img" : "dungeon_spiderdungeon02", "desc_text" : uiscriptlocale.TELEPORT_MAP_SPIDERDUNGEON_02, "map_desc" : "",  
			"boss_vnum" : 2091, "min_lv" : 50, "max_lv" : 120, "money_cost" : 125000, "item_cost" : 0, "item_cost_count" : 0,
			"BUTTONS" : { # Max Buttons = 3
				0 : {"command" : "sd2_start" , "button_text" : uiscriptlocale.TELEPORT_BUTTON_START, "button_text_color" : 0, "button_disable_text_color" : 0 },
			}
		},
		2 : {
			"map_img" : "dungeon_spiderdungeon03", "desc_text" : uiscriptlocale.TELEPORT_MAP_SPIDERDUNGEON_03, "map_desc" : "",  
			"boss_vnum" : 0, "min_lv" : 65, "max_lv" : 120, "money_cost" : 150000, "item_cost" : 0, "item_cost_count" : 0,
			"BUTTONS" : { # Max Buttons = 3
				0 : {"command" : "sd3_start" , "button_text" : uiscriptlocale.TELEPORT_BUTTON_START, "button_text_color" : 0, "button_disable_text_color" : 0 },
			}
		},
		3 : {
			"map_img" : "dungeon_deamontower", "desc_text" : uiscriptlocale.TELEPORT_MAP_DEAMONTOWER, "map_desc" : "",  
			"boss_vnum" : 1093, "min_lv" : 40, "max_lv" : 120, "money_cost" : 200000, "item_cost" : 0, "item_cost_count" : 0,
			"BUTTONS" : { # Max Buttons = 3
				0 : {"command" : "deamontower" , "button_text" : uiscriptlocale.TELEPORT_BUTTON_ENTRY, "button_text_color" : 0, "button_disable_text_color" : 0 },
			}
		},
		# 3 : {
			# "map_img" : "dungeon_devilscatacomb", "desc_text" : uiscriptlocale.TELEPORT_MAP_DEVILS_CATACOMB, "map_desc" : "",  
			# "boss_vnum" : 0, "min_lv" : 75, "max_lv" : 120, "money_cost" : 250000, "item_cost" : 0, "item_cost_count" : 0,
			# "BUTTONS" : { # Max Buttons = 3
				# 0 : {"command" : "devilscatacomb" , "button_text" : uiscriptlocale.TELEPORT_BUTTON_ENTRY, "button_text_color" : 0, "button_disable_text_color" : 0 },
			# }
		# },
		4 : {
			"map_img" : "dungeon_skipia", "desc_text" : uiscriptlocale.TELEPORT_MAP_SKIPIA_DUNGEON, "map_desc" : "",  
			"boss_vnum" : 1192, "min_lv" : 60, "max_lv" : 120, "money_cost" : 3000000, "item_cost" : 0, "item_cost_count" : 0,
			"BUTTONS" : { # Max Buttons = 3
				0 : {"command" : "skipia1" , "button_text" : uiscriptlocale.TELEPORT_BUTTON_ENTRY, "button_text_color" : 0, "button_disable_text_color" : 0 },
				#2 : {"command" : "skipia_boss" , "button_text" : uiscriptlocale.TELEPORT_BUTTON_SKIPIA_BOSS, "button_text_color" : 0, "button_disable_text_color" : 0 },
			}
		},
		5 : {
			"map_img" : "dungeon_skipia02", "desc_text" : uiscriptlocale.TELEPORT_MAP_SKIPIA_DUNGEON_02, "map_desc" : "",  
			"boss_vnum" : 2495, "min_lv" : 75, "max_lv" : 120, "money_cost" : 4000000, "item_cost" : 0, "item_cost_count" : 0,
			"BUTTONS" : { # Max Buttons = 3
				0 : {"command" : "skipia2" , "button_text" : uiscriptlocale.TELEPORT_BUTTON_ENTRY, "button_text_color" : 0, "button_disable_text_color" : 0 },
				#2 : {"command" : "skipia_boss" , "button_text" : uiscriptlocale.TELEPORT_BUTTON_SKIPIA_BOSS, "button_text_color" : 0, "button_disable_text_color" : 0 },
			}
		},
		6 : {
			"map_img" : "bahia_nefrit", "desc_text" : uiscriptlocale.TELEPORT_MAP_BAHIA_NEFRIT, "map_desc" : "",  
			"boss_vnum" : 3491, "min_lv" : 90, "max_lv" : 120, "money_cost" : 5000000, "item_cost" : 0, "item_cost_count" : 0,
			"BUTTONS" : { # Max Buttons = 3
				0 : {"command" : "bahia_nefrit" , "button_text" : uiscriptlocale.TELEPORT_BUTTON_ENTRY, "button_text_color" : 0, "button_disable_text_color" : 0 },
			}
		},
		7 : {
			"map_img" : "mont_trueno", "desc_text" : uiscriptlocale.TELEPORT_MAP_TRUENO, "map_desc" : "",  
			"boss_vnum" : 3191, "min_lv" : 90, "max_lv" : 120, "money_cost" : 5000000, "item_cost" : 0, "item_cost_count" : 0,
			"BUTTONS" : { # Max Buttons = 3
				0 : {"command" : "map_trueno" , "button_text" : uiscriptlocale.TELEPORT_BUTTON_ENTRY, "button_text_color" : 0, "button_disable_text_color" : 0 },
			}
		},
		8 : {
			"map_img" : "gautama", "desc_text" : uiscriptlocale.TELEPORT_MAP_GAUTAMA, "map_desc" : "",  
			"boss_vnum" : 3390, "min_lv" : 90, "max_lv" : 120, "money_cost" : 5000000, "item_cost" : 0, "item_cost_count" : 0,
			"BUTTONS" : { # Max Buttons = 3
				0 : {"command" : "map_gautama" , "button_text" : uiscriptlocale.TELEPORT_BUTTON_ENTRY, "button_text_color" : 0, "button_disable_text_color" : 0 },
			}
		},
		9 : {
			"map_img" : "cabo_fuego_dragon", "desc_text" : uiscriptlocale.TELEPORT_MAP_CABO_DRAG, "map_desc" : "",  
			"boss_vnum" : 3290, "min_lv" : 90, "max_lv" : 120, "money_cost" : 5000000, "item_cost" : 0, "item_cost_count" : 0,
			"BUTTONS" : { # Max Buttons = 3
				0 : {"command" : "map_cabo_drag" , "button_text" : uiscriptlocale.TELEPORT_BUTTON_ENTRY, "button_text_color" : 0, "button_disable_text_color" : 0 },
			}
		},
		10 : {
			"map_img" : "bosque_encantado", "desc_text" : uiscriptlocale.TELEPORT_MAP_BOSQ_ENCANT, "map_desc" : "",  
			"boss_vnum" : 6407, "min_lv" : 100, "max_lv" : 120, "money_cost" : 10000000, "item_cost" : 0, "item_cost_count" : 0,
			"BUTTONS" : { # Max Buttons = 3
				0 : {"command" : "map_bosq_encant" , "button_text" : uiscriptlocale.TELEPORT_BUTTON_ENTRY, "button_text_color" : 0, "button_disable_text_color" : 0 },
			}
		},
		# 5 : {
			# "map_img" : "dungeon_blazingpurgatory", "desc_text" : uiscriptlocale.TELEPORT_MAP_BLAZING_PURGATORY, "map_desc" : "",  
			# "boss_vnum" : 0, "min_lv" : 80, "max_lv" : 120, "money_cost" : 75000, "item_cost" : 0, "item_cost_count" : 0,
			# "BUTTONS" : { # Max Buttons = 3
				# 0 : {"command" : "blazingp" , "button_text" : uiscriptlocale.TELEPORT_BUTTON_ENTRY, "button_text_color" : 0, "button_disable_text_color" : 0 },
			# }
		# },
		# 6 : {
			# "map_img" : "dungeon_nemere_warte", "desc_text" : uiscriptlocale.TELEPORT_MAP_NEMERES_WARTE, "map_desc" : "",  
			# "boss_vnum" : 0, "min_lv" : 80, "max_lv" : 120, "money_cost" : 75000, "item_cost" : 0, "item_cost_count" : 0,
			# "BUTTONS" : { # Max Buttons = 3
				# 0 : {"command" : "nemere" , "button_text" : uiscriptlocale.TELEPORT_BUTTON_ENTRY, "button_text_color" : 0, "button_disable_text_color" : 0 },
			# }
		# },
	},
}
	
class TeleportPanel(ui.ScriptWindow):
	def __init__(self):
		self.__questReciveString = ""
		self.__questSendString = ""
		self.__questID = 0
		self.__commands = {
			'SET_QUEST_ID' : self.__SetQuestId,
			'GET_QUEST_CMD' : self.__GetQuestCmd,
		}
		
		#self.ClientToQuest = ClientQuestCommunication()
		ui.ScriptWindow.__init__(self)
		self.__LoadWindow()
		
	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def Show(self):
		self.seclectedMapOnState = ""
		self.selectedMap = 0
		self.sliderNextPictureTime = app.GetTime() + SLIDER_TIME_TO_REFRESH
		self.showStartIndex = 0
		self.startTimeSlidePicture = 0
		self.activePicture = 0
		self.sliderImgRotation = 1
		self.SetState("EMPIRES")
		self.GetChild("info_layer").Hide()
		ui.ScriptWindow.Show(self)

	def Close(self):
		self.Hide()
		
	def Destroy(self):
		ui.ScriptWindow.__del__(self)

	def __LoadWindow(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "UIScript/TeleportPanel.py")
		except:
			import exception
			exception.Abort("uiTeleportPanel.Open.TeleportPanel.py")
	
		try:
			self.GetChild("board").SetCloseEvent(self.Close)
				
			self.tabDict = {
				"EMPIRES"	: self.GetChild("Tab_01"),
				"NEUTRAL"	: self.GetChild("Tab_02"),
				"DUNGEON"	: self.GetChild("Tab_03"),
			}
			
			self.tabButtonDict = {
				"EMPIRES"	: self.GetChild("Tab_Button_01"),
				"NEUTRAL"	: self.GetChild("Tab_Button_02"),
				"DUNGEON"	: self.GetChild("Tab_Button_03"),
			}
			
			self.tabButtonTextDict = {
				"EMPIRES"	: self.GetChild("Tab_Button_Text_01"),
				"NEUTRAL"	: self.GetChild("Tab_Button_Text_02"),
				"DUNGEON"	: self.GetChild("Tab_Button_Text_03"),
			}
			
			for (stateKey, tabButton) in self.tabButtonDict.items():
				tabButton.SetEvent(ui.__mem_func__(self.__OnClickTabButton), stateKey)
			
			self.ScrollBar = self.GetChild("ScrollBar")
			self.ScrollBar.SetScrollEvent(ui.__mem_func__(self.OnScroll))
			
		except:
			import exception
			exception.Abort("uiTeleportPanel.LoadWindow.BindObject")
	

	def __OnClickTabButton(self, stateKey):
		self.SetState(stateKey)

	def SetState(self, stateKey):
		ACTIVE_COLOR	= 0xFFFEE3AE
		DISABLE_COLOR	= 0xFF9C8C6D
		
		self.state = stateKey

		for (tabKey, tabButton) in self.tabButtonDict.items():
			if stateKey!=tabKey:
				tabButton.SetUp()

		for tabValue in self.tabDict.itervalues():
			tabValue.Hide()
		
		for tabText in self.tabButtonTextDict.itervalues():
			tabText.SetPackedFontColor(DISABLE_COLOR)
			
		self.tabDict[stateKey].Show()
		self.tabButtonTextDict[stateKey].SetPackedFontColor(ACTIVE_COLOR)
		
		self.showStartIndex = 0
		self.ScrollBar.SetPos(0)
		if 4 < len(MAP_DICT[self.state]):
			self.ScrollBar.SetMiddleBarSize(float(3) / len(MAP_DICT[self.state]))
			self.ScrollBar.SetScrollStep(float(3)/len(MAP_DICT[self.state]))
			self.ScrollBar.Show()
		else:
			self.ScrollBar.Hide()

		self.ReloadMapArea()
	
	def ReloadMapArea(self):
		self.MapButtonList = []
		self.MapButtonTextList = []
		self.arrangeMap = 7
			
		for show in xrange(self.showStartIndex, self.showStartIndex + 4):
			if show >= len(MAP_DICT[self.state]):
				return
			self.MapButton = ui.RadioButton()
			self.MapButton.SetParent(self.GetChild("MapArea"))
			self.MapButton.SetPosition(6, self.arrangeMap)
			self.MapButton.SetUpVisual(MAP_IMG_PATH + MAP_DICT[self.state][show]["map_img"] + "_normal.tga")
			self.MapButton.SetOverVisual(MAP_IMG_PATH + MAP_DICT[self.state][show]["map_img"] + "_hover.tga")
			self.MapButton.SetDownVisual(MAP_IMG_PATH + MAP_DICT[self.state][show]["map_img"] + "_down.tga")
			self.MapButton.SetEvent(ui.__mem_func__(self.ClickMap), MAP_DICT[self.state][show], show)
			if self.state == self.seclectedMapOnState and self.selectedMap == MAP_DICT[self.state][show]:
				self.MapButton.Down()
			else:
				self.MapButton.SetUp()
		
			self.MapButton.Show()
			
			self.MapNameText = ui.TextLine()
			self.MapNameText.SetParent(self.MapButton)
			self.MapNameText.SetFontName(localeinfo.UI_DEF_FONT_LARGE)
			self.MapNameText.SetPosition(0, 75)
			self.MapNameText.SetHorizontalAlignCenter()
			self.MapNameText.SetWindowHorizontalAlignCenter()
			self.MapNameText.SetText(MAP_DICT[self.state][show]["desc_text"])
			self.MapNameText.Show()
			
			self.MapButtonList.append(self.MapButton)
			self.MapButtonTextList.append(self.MapNameText)
			self.arrangeMap += 103

	def ClickMap(self, selected, showindex):
		self.seclectedMapOnState = self.state
		self.selectedMap = selected
		for i in xrange(self.showStartIndex, self.showStartIndex + len(self.MapButtonList)):
			mapButtonIndex = i - self.showStartIndex
			if showindex == i:
				self.MapButtonList[mapButtonIndex].Down()
			else:
				self.MapButtonList[mapButtonIndex].SetUp()
		self.SetMapDesc(selected["map_desc"])
		
		self.GetChild("info_layer").Show()
		self.GetChild("Titlebar2_Text").SetText(selected["desc_text"])
		if selected["boss_vnum"] != 0:
			self.GetChild("text_info_boss").SetText(uiscriptlocale.TELEPORT_PANEL_INFO_BOSS % nonplayer.GetMonsterName(selected["boss_vnum"]))
		else:
			self.GetChild("text_info_boss").SetText(uiscriptlocale.TELEPORT_PANEL_INFO_BOSS_EMPTY)
		if selected["min_lv"] != 0:
			if player.GetStatus(player.LEVEL) > selected["min_lv"]:
				self.GetChild("text_info_minlv").SetText((uiscriptlocale.TELEPORT_PANEL_INFO_MINLV + "|cff5af053 %d") % selected["min_lv"])
			else:
				self.GetChild("text_info_minlv").SetText((uiscriptlocale.TELEPORT_PANEL_INFO_MINLV + "|cffd74949 %d") % selected["min_lv"])
		else:
			self.GetChild("text_info_minlv").SetText(uiscriptlocale.TELEPORT_PANEL_INFO_MINLV_EMPTY)
		if selected["max_lv"] != 0:
			if player.GetStatus(player.LEVEL) < selected["max_lv"]:
				self.GetChild("text_info_maxlv").SetText((uiscriptlocale.TELEPORT_PANEL_INFO_MAXLV + "|cff5af053 %d") % selected["max_lv"])
			else:
				self.GetChild("text_info_maxlv").SetText((uiscriptlocale.TELEPORT_PANEL_INFO_MAXLV + "|cffd74949 %d") % selected["max_lv"])
		else:
			self.GetChild("text_info_maxlv").SetText(uiscriptlocale.TELEPORT_PANEL_INFO_MAXLV_EMPTY)
		if selected["money_cost"] != 0:
			if player.GetMoney() >= selected["money_cost"]:
				self.GetChild("text_info_cost").SetText((uiscriptlocale.TELEPORT_PANEL_INFO_COST + "|cff5af053 %s") % localeinfo.NumberToMoneyString(int(selected["money_cost"])))
			else:
				self.GetChild("text_info_cost").SetText((uiscriptlocale.TELEPORT_PANEL_INFO_COST + "|cffd74949 %s") % localeinfo.NumberToMoneyString(int(selected["money_cost"])))
		else:
			self.GetChild("text_info_cost").SetText(uiscriptlocale.TELEPORT_PANEL_INFO_COST_EMPTY)
		if selected["item_cost"] != 0:
			item.SelectItem(selected["item_cost"])
			if player.GetItemCountByVnum(selected["item_cost"]) >= selected["item_cost_count"]:
				self.GetChild("text_info_required_item_text").SetText((uiscriptlocale.TELEPORT_PANEL_INFO_ITEM_COST_TEXT + " |cff5af053 ["+str(player.GetItemCountByVnum(selected["item_cost"]))+"]") % (selected["item_cost_count"], item.GetItemName()))
			else:
				self.GetChild("text_info_required_item_text").SetText((uiscriptlocale.TELEPORT_PANEL_INFO_ITEM_COST_TEXT + " |cffd74949 ["+str(player.GetItemCountByVnum(selected["item_cost"]))+"]") % (selected["item_cost_count"], item.GetItemName()))
			self.GetChild("item_icon").LoadImage(item.GetIconImageFileName())
			self.GetChild("item_icon").Show()
		else:
			self.GetChild("text_info_required_item_text").SetText(uiscriptlocale.TELEPORT_PANEL_INFO_ITEM_COST_TEXT_EMPTY)
			self.GetChild("item_icon").Hide()
	
		self.selectButtonList = []
		self.buttonYRange = 338
		for button in xrange(len(selected["BUTTONS"])):
			self.SelectButton = ui.Button()
			self.SelectButton.SetParent(self.GetChild("InfoArea"))
			#self.SelectButton.SetWindowHorizontalAlignCenter()
			self.SelectButton.SetPosition(13, self.buttonYRange)
			self.SelectButton.SetUpVisual(IMG_PATH + "button_default.sub")
			self.SelectButton.SetOverVisual(IMG_PATH + "button_hover.sub")
			self.SelectButton.SetDownVisual(IMG_PATH + "button_down.sub")
			self.SelectButton.SetDisableVisual(IMG_PATH + "button_disable.sub")
			self.SelectButton.SetText(selected["BUTTONS"][button]["button_text"])
			if selected["BUTTONS"][button]["button_text_color"] != 0:
				self.SelectButton.SetTextColor(selected["BUTTONS"][button]["button_text_color"])
			else:
				self.SelectButton.SetTextColor(0xFFFEE3AE)
			#self.SelectButton.SetOutline()
			if player.GetStatus(player.LEVEL) < selected["min_lv"] or player.GetStatus(player.LEVEL) > selected["max_lv"] or player.GetMoney() < selected["money_cost"] or player.GetItemCountByVnum(selected["item_cost"]) < selected["item_cost_count"]:
				self.SelectButton.Disable()
				if selected["BUTTONS"][button]["button_disable_text_color"] != 0:
					self.SelectButton.SetTextColor(selected["BUTTONS"][button]["button_disable_text_color"])
				else:
					self.SelectButton.SetTextColor(0xFFB29A6B)
			
			self.SelectButton.SetEvent(ui.__mem_func__(self.ClickButton), selected["BUTTONS"][button]["command"])
			self.SelectButton.Show()
			self.selectButtonList.append(self.SelectButton)
			self.buttonYRange += 37
	
	def ClickButton(self, command):
		self.SendQuestCommand('TELEPORTING#%s' % (str(command)))
		self.Close()

	def OnScroll(self):
		scrollLineCount = max(0, len(MAP_DICT[self.state]) - 4)
		startIndex = int(scrollLineCount * self.ScrollBar.GetPos())

		if startIndex != self.showStartIndex:
			self.showStartIndex = startIndex
			self.ReloadMapArea()
			
	def SetMapDesc(self, desc):
		self.childrenList = []
		lines = self.SplitDescription(desc, 35)
		if not lines:
			return
			
		self.toolTipHeight = 7
		for line in lines:
			textLine = ui.TextLine()
			textLine.SetParent(self.GetChild("info_layer"))
			textLine.SetPackedFontColor(0xFFFFFFFF)
			textLine.SetFontName(localeinfo.UI_DEF_FONT)
			textLine.SetText(line)
			textLine.SetOutline()
			#textLine.SetFeather(False)
			textLine.Show()

			textLine.SetPosition(0, self.toolTipHeight)
			textLine.SetWindowHorizontalAlignCenter()
			textLine.SetHorizontalAlignCenter()

			self.childrenList.append(textLine)

			self.toolTipHeight += 17

	def OnRunMouseWheel(self, nLen):
		if self.ScrollBar.IsShow():
			if nLen > 0:
				self.ScrollBar.OnUp()
			else:
				self.ScrollBar.OnDown()

	def OnUpdate(self):
		if self.sliderNextPictureTime < app.GetTime():
			self.sliderNextPictureTime = app.GetTime() + SLIDER_TIME_TO_REFRESH
			self.startTimeSlidePicture = time.clock()
			self.sliderImgRotation += 1
			if self.sliderImgRotation >= len(SLIDER_DICT):
				self.sliderImgRotation = 0
			if self.activePicture == 0:
				self.GetChild("slider_picture_1").LoadImage(SLIDER_DICT[self.sliderImgRotation])
			elif self.activePicture == 1:
				self.GetChild("slider_picture_2").LoadImage(SLIDER_DICT[self.sliderImgRotation])
	
		if self.startTimeSlidePicture != 0:
			progress = float(min((time.clock() - self.startTimeSlidePicture) / SLIDER_ANI_DURATION, 1))
			if progress < 1:
				alphaValueOff = float(1.0 - progress)
				alphaValueOn = float(0.0 + progress)
				
				if self.activePicture == 0:
					self.GetChild("slider_picture_2").SetAlpha(alphaValueOff)
					self.GetChild("slider_picture_1").SetAlpha(alphaValueOn)
				if self.activePicture == 1:
					self.GetChild("slider_picture_1").SetAlpha(alphaValueOff)
					self.GetChild("slider_picture_2").SetAlpha(alphaValueOn)
			else:
				self.startTimeSlidePicture = 0
				if self.activePicture == 0:
					self.activePicture = 1
				elif self.activePicture == 1:
					self.activePicture = 0

	def SplitDescription(self, desc, limit):
		total_tokens = desc.split()
		line_tokens = []
		line_len = 0
		lines = []
		for token in total_tokens:
			if "|" in token:
				sep_pos = token.find("|")
				line_tokens.append(token[:sep_pos])

				lines.append(" ".join(line_tokens))
				line_len = len(token) - (sep_pos + 1)
				line_tokens = [token[sep_pos+1:]]
			else:
				line_len += len(token)
				if len(line_tokens) + line_len > limit:
					lines.append(" ".join(line_tokens))
					line_len = len(token)
					line_tokens = [token]
				else:
					line_tokens.append(token)

		if line_tokens:
			lines.append(" ".join(line_tokens))

		return lines

	def SendQuestCommand(self, send_command):
		self.__questSendString = send_command
		event.QuestButtonClick(self.__questID)
	
	def ReceiveQuestCommand(self, recive_command):
		self.__questReciveString += recive_command
		close_pos = self.__questReciveString.find(')')
		if close_pos != -1:
			open_pos = self.__questReciveString.find('(')
			
			command = self.__questReciveString[:open_pos]
			args = self.__questReciveString[open_pos+1:close_pos].replace("#","").split(",")
			self.__questReciveString = ''
			if command in self.__commands:
				if args[0]:
					self.__commands[command](*args)
				else:
					self.__commands[command]()
			
	def __SetQuestId(self, quest_id):
		self.__questID = int(quest_id)
		
	def __GetQuestCmd(self):
		net.SendQuestInputStringPacket(self.__questSendString)
		self.__questSendString = ""
