import ui
import uiscriptlocale
import app
import net
import dbg
import snd
import player
import mousemodule
import wndMgr
import skill
import playersettingmodule
import quest
import uitooltip
import constinfo
import chr
import uicharacterdetails
import emotion
import localeinfo

if app.ENABLE_SKILL_COLOR_SYSTEM:
	import uiskillcolor

if app.ENABLE_QUEST_RENEWAL:
	import math, uiquest

ENABLE_GUI_SELECT = True

SHOW_ONLY_ACTIVE_SKILL = False
SHOW_LIMIT_SUPPORT_SKILL_LIST = []
HIDE_SUPPORT_SKILL_POINT = False

if localeinfo.IsYMIR():
	SHOW_LIMIT_SUPPORT_SKILL_LIST = [121, 122, 123, 124, 126, 127, 129, 128, 131, 137, 138, 139, 140,141,142]
	if not localeinfo.IsCHEONMA():
		HIDE_SUPPORT_SKILL_POINT = True
		SHOW_LIMIT_SUPPORT_SKILL_LIST = [121, 122, 123, 124, 126, 127, 129, 128, 131, 137, 138, 139, 140,141,142]
elif localeinfo.IsJAPAN() or (localeinfo.IsEUROPE() and app.GetLocalePath() != "locale/ca") and (localeinfo.IsEUROPE() and app.GetLocalePath() != "locale/br"):
	HIDE_SUPPORT_SKILL_POINT = True
	if not app.ENABLE_NEW_SECONDARY_SKILLS:
		SHOW_LIMIT_SUPPORT_SKILL_LIST = [121, 122, 123, 124, 126, 127, 129, 128, 131, 137, 138, 139, 140]
else:
	HIDE_SUPPORT_SKILL_POINT = True

FACE_IMAGE_DICT = {
	playersettingmodule.RACE_WARRIOR_M	: "icon/face/warrior_m.tga",
	playersettingmodule.RACE_WARRIOR_W	: "icon/face/warrior_w.tga",
	playersettingmodule.RACE_ASSASSIN_M	: "icon/face/assassin_m.tga",
	playersettingmodule.RACE_ASSASSIN_W	: "icon/face/assassin_w.tga",
	playersettingmodule.RACE_SURA_M		: "icon/face/sura_m.tga",
	playersettingmodule.RACE_SURA_W		: "icon/face/sura_w.tga",
	playersettingmodule.RACE_SHAMAN_M	: "icon/face/shaman_m.tga",
	playersettingmodule.RACE_SHAMAN_W	: "icon/face/shaman_w.tga",
}
if app.ENABLE_WOLFMAN_CHARACTER:
	FACE_IMAGE_DICT.update({playersettingmodule.RACE_WOLFMAN_M  : "icon/face/wolfman_m.tga",})

def unsigned32(n):
	return n & 0xFFFFFFFFL



if app.ENABLE_QUEST_RENEWAL:
	quest_slot_listbar = {
		"name" : "Quest_Slot",
		"type" : "listbar",

		"x" : 0,
		"y" : 0,

		"width" : 150,
		"height" : 20,

		"text" : "Quest title",
		"align" : "left",

		"horizontal_align" : "left",
		"vertical_align" : "left",
		"text_horizontal_align" : "left",
		"all_align" : "left",

		"text_height": 40
	}

	quest_lable_expend_img_path_dict = {
		0: "d:/ymir work/ui/quest_re/tabcolor_1_main.tga",
		1: "d:/ymir work/ui/quest_re/tabcolor_2_sub.tga",
		2: "d:/ymir work/ui/quest_re/tabcolor_3_levelup.tga",
		3: "d:/ymir work/ui/quest_re/tabcolor_4_event.tga",
		4: "d:/ymir work/ui/quest_re/tabcolor_5_collection.tga",
		5: "d:/ymir work/ui/quest_re/tabcolor_6_system.tga",
		6: "d:/ymir work/ui/quest_re/tabcolor_7_scroll.tga",
		7: "d:/ymir work/ui/quest_re/tabcolor_8_daily.tga"
	}

	quest_label_dict = {
		0 : uiscriptlocale.QUEST_UI_TEXT_MAIN,
		1 : uiscriptlocale.QUEST_UI_TEXT_SUB,
		2 : uiscriptlocale.QUEST_UI_TEXT_LEVELUP,
		3 : uiscriptlocale.QUEST_UI_TEXT_EVENT,
		4 : uiscriptlocale.QUEST_UI_TEXT_COLLECTION,
		5 : uiscriptlocale.QUEST_UI_TEXT_SYSTEM,
		6 : uiscriptlocale.QUEST_UI_TEXT_SCROLL,
		7 : uiscriptlocale.QUEST_UI_TEXT_DAILY,
	}



class CharacterWindow(ui.ScriptWindow):

	ACTIVE_PAGE_SLOT_COUNT = 8
	SUPPORT_PAGE_SLOT_COUNT = 12

	PAGE_SLOT_COUNT = 12
	PAGE_HORSE = 2

	SKILL_GROUP_NAME_DICT = {
		playersettingmodule.JOB_WARRIOR	: { 1 : localeinfo.SKILL_GROUP_WARRIOR_1,	2 : localeinfo.SKILL_GROUP_WARRIOR_2, },
		playersettingmodule.JOB_ASSASSIN	: { 1 : localeinfo.SKILL_GROUP_ASSASSIN_1,	2 : localeinfo.SKILL_GROUP_ASSASSIN_2, },
		playersettingmodule.JOB_SURA		: { 1 : localeinfo.SKILL_GROUP_SURA_1,		2 : localeinfo.SKILL_GROUP_SURA_2, },
		playersettingmodule.JOB_SHAMAN		: { 1 : localeinfo.SKILL_GROUP_SHAMAN_1,	2 : localeinfo.SKILL_GROUP_SHAMAN_2, },
	}
	if app.ENABLE_WOLFMAN_CHARACTER:
		SKILL_GROUP_NAME_DICT.update({playersettingmodule.JOB_WOLFMAN		: { 1 : localeinfo.JOB_WOLFMAN1,	2 : localeinfo.JOB_WOLFMAN2, },})

	STAT_DESCRIPTION =	{
		"HTH" : localeinfo.STAT_TOOLTIP_CON,
		"INT" : localeinfo.STAT_TOOLTIP_INT,
		"STR" : localeinfo.STAT_TOOLTIP_STR,
		"DEX" : localeinfo.STAT_TOOLTIP_DEX,
	}

	if app.ENABLE_QUEST_RENEWAL:
		MAX_QUEST_PAGE_HEIGHT = 293.5


	STAT_MINUS_DESCRIPTION = localeinfo.STAT_MINUS_DESCRIPTION

	def __init__(self):
		self.chDetailsWnd = None
		self.isOpenedDetailsWnd = False
		
		if app.ENABLE_QUEST_RENEWAL:
			self.isQuestCategoryLoad = False
	

		if app.ENABLE_SKILL_COLOR_SYSTEM:
			self.skillColorWnd = None
			self.skillColorButton = []

		ui.ScriptWindow.__init__(self)
		self.state = "STATUS"
		## CHARACTER WINDOW RENEWAL
		self.substate = "BASE"

		self.isLoaded = 0

		self.toolTipSkill = 0
		self.__Initialize()
		self.__LoadWindow()

		if app.ENABLE_RENEWAL_ADD_STATS:
			self.statusPlusCommandDict={
				"HTH" : "/stat_val ht ",
				"INT" : "/stat_val iq ",
				"STR" : "/stat_val st ",
				"DEX" : "/stat_val dx ",
			}
		else:
			self.statusPlusCommandDict={
				"HTH" : "/stat ht",
				"INT" : "/stat iq",
				"STR" : "/stat st",
				"DEX" : "/stat dx",
			}

		self.statusMinusCommandDict={
			"HTH-" : "/stat- ht",
			"INT-" : "/stat- iq",
			"STR-" : "/stat- st",
			"DEX-" : "/stat- dx",
		}

		# self.statusPlusCommandDict={
			# "HTH" : "/stat ht",
			# "INT" : "/stat iq",
			# "STR" : "/stat st",
			# "DEX" : "/stat dx",
		# }
		
		# self.status2PlusCommandDict={
			# "HTH" : "/stat2 ht",
			# "INT" : "/stat2 iq",
			# "STR" : "/stat2 st",
			# "DEX" : "/stat2 dx",
		# }
		
		# self.statusMinusCommandDict={
			# "HTH-" : "/stat- ht",
			# "INT-" : "/stat- iq",
			# "STR-" : "/stat- st",
			# "DEX-" : "/stat- dx",
		# }

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def __Initialize(self):
		self.refreshToolTip = 0
		self.curSelectedSkillGroup = 0
		self.canUseHorseSkill = -1
		self.toolTip = None
		self.toolTipJob = None
		self.toolTipAlignment = None
		self.toolTipSkill = None

		self.faceImage = None
		self.statusPlusValue = None
		self.activeSlot = None
		self.tabDict = None
		self.tabButtonDict = None
		self.pageDict = None
		self.titleBarDict = None
		self.statusPlusButtonDict = None
		self.statusMinusButtonDict = None

		self.skillPageDict = None
		if app.ENABLE_QUEST_RENEWAL:
			self.questScrollBar = None
			self.questLastScrollPosition = 0
			self.questPage = None
			self.questTitleBar = None
			self.questSlotList = []
			self.questCategory = {}
			self.questCategoryList = []

			self.questColorList = {
				"green" : 0xFF83C055,
				"blue": 0xFF45678D,
				"golden": 0xFFCAB62F,
				"default_title": 0xFFCEC6B5
			}

			self.questOpenedCategories = []
			self.questMaxOpenedCategories = 1

			self.questClicked = []
			self.questIndexMap = {}
			self.questCounterList = []
			self.questClockList = []
			self.questSeparatorList = []

			self.displayY = 0
			self.baseCutY = 0
			self.questCategoryRenderPos = []
		else:
			self.questShowingStartIndex = 0
			self.questScrollBar = None
			self.questSlot = None
			self.questNameList = None
			self.questLastTimeList = None
			self.questLastCountList = None
		self.skillGroupButton = ()

		self.activeSlot = None
		self.activeSkillPointValue = None
		self.supportSkillPointValue = None
		self.skillGroupButton1 = None
		self.skillGroupButton2 = None
		if app.ENABLE_NEW_PASSIVE_SKILLS:
			self.skillGroupButton3 = None
		
		self.activeSkillGroupName = None

		self.guildNameSlot = None
		self.guildNameValue = None
		self.characterNameSlot = None
		self.characterNameValue = None

		self.emotionToolTip = None
		self.soloEmotionSlot = None
		self.specialEmotionSlot = None
		self.dualEmotionSlot = None
		## CHARACTER WINDOW RENEWAL
		self.toolTipConquerorInfoButton = None
		
		self.tabSungmaButtonDict = None
		self.SungmaButton = None
		## CHARACTER WINDOW RENEWAL
		
		if app.ENABLE_SKILL_COLOR_SYSTEM:
			self.skillColorWnd = None
			self.skillColorButton = []

	def OnTop(self):
		if self.chDetailsWnd:
			self.chDetailsWnd.SetTop()
		
		if app.ENABLE_SKILL_COLOR_SYSTEM:
			if self.skillColorWnd:
				self.skillColorWnd.SetTop()

	def Hide(self):
		if self.chDetailsWnd:
			self.isOpenedDetailsWnd = self.chDetailsWnd.IsShow()
			self.chDetailsWnd.Close()
		
		if app.ENABLE_SKILL_COLOR_SYSTEM:
			if self.skillColorWnd and self.skillColorWnd.IsShow():
				self.skillColorWnd.Close()
		
		wndMgr.Hide(self.hWnd)

	def Show(self):
		self.__LoadWindow()
		self.__InitCharacterDetailsUIButton()
		if self.chDetailsWnd and self.isOpenedDetailsWnd:
			self.chDetailsWnd.Show()
		
		ui.ScriptWindow.Show(self)

	def __LoadScript(self, fileName):
		pyScrLoader = ui.PythonScriptLoader()
		pyScrLoader.LoadScriptFile(self, fileName)

	def __BindObject(self):
		self.toolTip = uitooltip.ToolTip()
		self.toolTipJob = uitooltip.ToolTip()
		self.toolTipAlignment = uitooltip.ToolTip(130)
		## CHARACTER WINDOW RENEWAL
		self.toolTipConquerorInfoButton = uitooltip.ToolTip()
		## CHARACTER WINDOW RENEWAL
		self.faceImage = self.GetChild("Face_Image")

		faceSlot=self.GetChild("Face_Slot")
		if 949 == app.GetDefaultCodePage():
			faceSlot.SetStringEvent("MOUSE_OVER_IN", self.__ShowJobToolTip)
			faceSlot.SetStringEvent("MOUSE_OVER_OUT", self.__HideJobToolTip)
		## CHARACTER WINDOW RENEWAL
		self.statusPlusLabel = self.GetChild("Status_Plus_Label")
		## CHARACTER WINDOW RENEWAL
		self.statusPlusValue = self.GetChild("Status_Plus_Value")

		self.characterNameSlot = self.GetChild("Character_Name_Slot")
		self.characterNameValue = self.GetChild("Character_Name")
		self.guildNameSlot = self.GetChild("Guild_Name_Slot")
		self.guildNameValue = self.GetChild("Guild_Name")
		self.characterNameSlot.SetStringEvent("MOUSE_OVER_IN", self.__ShowAlignmentToolTip)
		self.characterNameSlot.SetStringEvent("MOUSE_OVER_OUT", self.__HideAlignmentToolTip)

		self.activeSlot = self.GetChild("Skill_Active_Slot")
		self.activeSkillPointValue = self.GetChild("Active_Skill_Point_Value")
		self.supportSkillPointValue = self.GetChild("Support_Skill_Point_Value")
		self.supportSkillPointValue.Hide()
		self.skillGroupButton1 = self.GetChild("Skill_Group_Button_1")
		self.skillGroupButton2 = self.GetChild("Skill_Group_Button_2")
		if app.ENABLE_NEW_PASSIVE_SKILLS:
			self.skillGroupButton3 = self.GetChild("Skill_Group_Button_3")
		
		self.activeSkillGroupName = self.GetChild("Active_Skill_Group_Name")
		if app.ENABLE_QUEST_RENEWAL:
			self.questScrollBar = self.GetChild("Quest_ScrollBar")
			self.questPage = self.GetChild("Quest_Page")
			self.questTitleBar = self.GetChild("Quest_TitleBar")
			self.quest_page_board_window = self.GetChild("quest_page_board_window")
			self.quest_object_board_window = self.GetChild("quest_object_board_window")




		self.tabDict = {
			"STATUS"	: self.GetChild("Tab_01"),
			"SKILL"		: self.GetChild("Tab_02"),
			"EMOTICON"	: self.GetChild("Tab_03"),
			"QUEST"		: self.GetChild("Tab_04"),
		}

		self.tabButtonDict = {
			"STATUS"	: self.GetChild("Tab_Button_01"),
			"SKILL"		: self.GetChild("Tab_Button_02"),
			"EMOTICON"	: self.GetChild("Tab_Button_03"),
			"QUEST"		: self.GetChild("Tab_Button_04")
		}

		self.pageDict = {
			"STATUS"	: self.GetChild("Character_Page"),
			"SKILL"		: self.GetChild("Skill_Page"),
			"EMOTICON"	: self.GetChild("Emoticon_Page"),
			"QUEST"		: self.GetChild("Quest_Page")
		}

		self.titleBarDict = {
			"STATUS"	: self.GetChild("Character_TitleBar"),
			"SKILL"		: self.GetChild("Skill_TitleBar"),
			"EMOTICON"	: self.GetChild("Emoticon_TitleBar"),
			"QUEST"		: self.GetChild("Quest_TitleBar")
		}

		self.statusPlusButtonDict = {
			"HTH"		: self.GetChild("HTH_Plus"),
			"INT"		: self.GetChild("INT_Plus"),
			"STR"		: self.GetChild("STR_Plus"),
			"DEX"		: self.GetChild("DEX_Plus"),
		}

		self.statusMinusButtonDict = {
			"HTH-"		: self.GetChild("HTH_Minus"),
			"INT-"		: self.GetChild("INT_Minus"),
			"STR-"		: self.GetChild("STR_Minus"),
			"DEX-"		: self.GetChild("DEX_Minus"),
		}

		self.skillPageDict = {
			"ACTIVE" : self.GetChild("Skill_Active_Slot"),
			"SUPPORT" : self.GetChild("Skill_ETC_Slot"),
			"HORSE" : self.GetChild("Skill_Active_Slot"),
		}

		self.skillPageStatDict = {
			"SUPPORT"	: player.SKILL_SUPPORT,
			"ACTIVE"	: player.SKILL_ACTIVE,
			"HORSE"		: player.SKILL_HORSE,
		}

		self.skillGroupButton = (
			self.GetChild("Skill_Group_Button_1"),
			self.GetChild("Skill_Group_Button_2"),
		)

		## CHARACTER WINDOW RENEWAL
		self.tabSungmaButtonDict = {
			"BASE"		: self.GetChild("change_base_button"),
			#"SUNGMA"	: self.GetChild("change_conqueror_button")
		}
		
		self.SungmaPageDict = {
			"BASE" : self.GetChild("base_info"),
			"SUNGMA" : self.GetChild("sungma_info"),
		}
		
		self.statusConquerorPlusButtonDict = {
			"SMH_STR" : self.GetChild("sungma_str_plus"),
			"SMH_HP" : self.GetChild("sungma_hp_plus"),
			"SMH_MOVE" : self.GetChild("sungma_move_plus"),
			"SMH_INMUNE" : self.GetChild("sungma_immune_plus"),
		}	
		## TabButton1 (Character)
		self.GetChild("Tab_Button_01").SetShowToolTipEvent(ui.__mem_func__(self.__ShowToolTipButton), localeinfo.STAT_TOOLTIP_TAB_CHARACTER)
		self.GetChild("Tab_Button_01").SetHideToolTipEvent(ui.__mem_func__(self.__HideToolTip))
		## TabButton2 (Skill)
		self.GetChild("Tab_Button_02").SetShowToolTipEvent(ui.__mem_func__(self.__ShowToolTipButton), localeinfo.STAT_TOOLTIP_TAB_SKILL)
		self.GetChild("Tab_Button_02").SetHideToolTipEvent(ui.__mem_func__(self.__HideToolTip))
		## TabButton3 (Emoticon)
		self.GetChild("Tab_Button_03").SetShowToolTipEvent(ui.__mem_func__(self.__ShowToolTipButton), localeinfo.STAT_TOOLTIP_TAB_EMOTICON)
		self.GetChild("Tab_Button_03").SetHideToolTipEvent(ui.__mem_func__(self.__HideToolTip))
		## TabButton4 (Quest)
		self.GetChild("Tab_Button_04").SetShowToolTipEvent(ui.__mem_func__(self.__ShowToolTipButton), localeinfo.STAT_TOOLTIP_TAB_QUEST)
		self.GetChild("Tab_Button_04").SetHideToolTipEvent(ui.__mem_func__(self.__HideToolTip))

		## Level
		self.GetChild("Lv_ToolTip").SetShowToolTipEvent(ui.__mem_func__(self.__ShowToolTipButton), localeinfo.STAT_TOOLTIP_LEVEL)
		self.GetChild("Lv_ToolTip").SetHideToolTipEvent(ui.__mem_func__(self.__HideToolTip))
		## EXP
		self.GetChild("Exp_ToolTip").SetShowToolTipEvent(ui.__mem_func__(self.__ShowToolTipButton), localeinfo.STAT_TOOLTIP_EXP)
		self.GetChild("Exp_ToolTip").SetHideToolTipEvent(ui.__mem_func__(self.__HideToolTip))

		## Base Level
		self.GetChild("change_base_button").SetShowToolTipEvent(ui.__mem_func__(self.__ShowToolTipButton), localeinfo.STAT_TOOLTIP_BASE_LEVEL)
		self.GetChild("change_base_button").SetHideToolTipEvent(ui.__mem_func__(self.__HideToolTip))
		## Conqueror Level
		#self.GetChild("change_conqueror_button").SetShowToolTipEvent(ui.__mem_func__(self.__ShowToolTipButton), localeinfo.STAT_TOOLTIP_CONQUEROR_LEVEL)
		#self.GetChild("change_conqueror_button").SetHideToolTipEvent(ui.__mem_func__(self.__HideToolTip))
		## Passive Relic
		#self.passive_expanded_btn = self.GetChild("passive_expanded_btn")
		#self.passive_expanded_btn.SetShowToolTipEvent(ui.__mem_func__(self.__ShowToolTipButton), localeinfo.STAT_TOOLTIP_PASSIVE)
		#self.passive_expanded_btn.SetHideToolTipEvent(ui.__mem_func__(self.__HideToolTip))
		#self.passive_expanded_btn.Hide()
		## CON (Constitution)
		self.GetChild("HTH_IMG").OnMouseOverIn = lambda arg = localeinfo.STAT_TOOLTIP_IMG_CON : ui.__mem_func__(self.__ShowToolTip)(arg)
		self.GetChild("HTH_IMG").OnMouseOverOut = lambda : ui.__mem_func__(self.__HideToolTip)()
		## INT (Intelligence)
		self.GetChild("INT_IMG").OnMouseOverIn = lambda arg = localeinfo.STAT_TOOLTIP_IMG_INT : ui.__mem_func__(self.__ShowToolTip)(arg)
		self.GetChild("INT_IMG").OnMouseOverOut = lambda : ui.__mem_func__(self.__HideToolTip)()
		## STR (Strength)
		self.GetChild("STR_IMG").OnMouseOverIn = lambda arg = localeinfo.STAT_TOOLTIP_IMG_STR : ui.__mem_func__(self.__ShowToolTip)(arg)
		self.GetChild("STR_IMG").OnMouseOverOut = lambda : ui.__mem_func__(self.__HideToolTip)()
		## DEX (Dexterity)
		self.GetChild("DEX_IMG").OnMouseOverIn = lambda arg = localeinfo.STAT_TOOLTIP_IMG_DEX : ui.__mem_func__(self.__ShowToolTip)(arg)
		self.GetChild("DEX_IMG").OnMouseOverOut = lambda : ui.__mem_func__(self.__HideToolTip)()

		## HP (Health)
		self.GetChild("HEL_IMG").OnMouseOverIn = lambda arg = localeinfo.STAT_TOOLTIP_HP : ui.__mem_func__(self.__ShowToolTip)(arg)
		self.GetChild("HEL_IMG").OnMouseOverOut = lambda : ui.__mem_func__(self.__HideToolTip)()
		## SP (Stamina)
		self.GetChild("SP_IMG").OnMouseOverIn = lambda arg = localeinfo.STAT_TOOLTIP_SP : ui.__mem_func__(self.__ShowToolTip)(arg)
		self.GetChild("SP_IMG").OnMouseOverOut = lambda : ui.__mem_func__(self.__HideToolTip)()
		## ATT (Attack)
		self.GetChild("ATT_IMG").OnMouseOverIn = lambda arg = localeinfo.STAT_TOOLTIP_ATT : ui.__mem_func__(self.__ShowToolTip)(arg)
		self.GetChild("ATT_IMG").OnMouseOverOut = lambda : ui.__mem_func__(self.__HideToolTip)()
		## DEF (Defense)
		self.GetChild("DEF_IMG").OnMouseOverIn = lambda arg = localeinfo.STAT_TOOLTIP_DEF : ui.__mem_func__(self.__ShowToolTip)(arg)
		self.GetChild("DEF_IMG").OnMouseOverOut = lambda : ui.__mem_func__(self.__HideToolTip)()

		## MSPD (Move Speed)
		self.GetChild("MSPD_IMG").OnMouseOverIn = lambda arg = localeinfo.STAT_TOOLTIP_MOVE_SPEED : ui.__mem_func__(self.__ShowToolTip)(arg)
		self.GetChild("MSPD_IMG").OnMouseOverOut = lambda : ui.__mem_func__(self.__HideToolTip)()
		## ASPD (Attack Speed)
		self.GetChild("ASPD_IMG").OnMouseOverIn = lambda arg = localeinfo.STAT_TOOLTIP_ATT_SPEED : ui.__mem_func__(self.__ShowToolTip)(arg)
		self.GetChild("ASPD_IMG").OnMouseOverOut = lambda : ui.__mem_func__(self.__HideToolTip)()
		## CSPD (Cast Speed)
		self.GetChild("CSPD_IMG").OnMouseOverIn = lambda arg = localeinfo.STAT_TOOLTIP_CAST_SPEED : ui.__mem_func__(self.__ShowToolTip)(arg)
		self.GetChild("CSPD_IMG").OnMouseOverOut = lambda : ui.__mem_func__(self.__HideToolTip)()
		## MATT (Magic Attack)
		self.GetChild("MATT_IMG").OnMouseOverIn = lambda arg = localeinfo.STAT_TOOLTIP_MAG_ATT : ui.__mem_func__(self.__ShowToolTip)(arg)
		self.GetChild("MATT_IMG").OnMouseOverOut = lambda : ui.__mem_func__(self.__HideToolTip)()
		## MDEF (Magic Defense)
		self.GetChild("MDEF_IMG").OnMouseOverIn = lambda arg = localeinfo.STAT_TOOLTIP_MAG_DEF : ui.__mem_func__(self.__ShowToolTip)(arg)
		self.GetChild("MDEF_IMG").OnMouseOverOut = lambda : ui.__mem_func__(self.__HideToolTip)()
		## EF (Evasion Resistance)
		self.GetChild("ER_IMG").OnMouseOverIn = lambda arg = localeinfo.STAT_TOOLTIP_DODGE_PER : ui.__mem_func__(self.__ShowToolTip)(arg)
		self.GetChild("ER_IMG").OnMouseOverOut = lambda : ui.__mem_func__(self.__HideToolTip)()
		## Active Skill Point Label
		self.GetChild("Active_Skill_Point_Label").OnMouseOverIn = lambda arg = localeinfo.STAT_TOOLTIP_POINT : ui.__mem_func__(self.__ShowToolTip)(arg)
		self.GetChild("Active_Skill_Point_Label").OnMouseOverOut = lambda : ui.__mem_func__(self.__HideToolTip)()
		## Support Skill Point Label
		self.GetChild("Support_Skill_ToolTip").OnMouseOverIn = lambda arg = localeinfo.STAT_TOOLTIP_SUPPORT_SKILL : ui.__mem_func__(self.__ShowToolTip)(arg)
		self.GetChild("Support_Skill_ToolTip").OnMouseOverOut = lambda : ui.__mem_func__(self.__HideToolTip)()

		self.GetChild("Action_Bar_Img").OnMouseOverIn = lambda arg = localeinfo.STAT_TOOLTIP_ACTION : ui.__mem_func__(self.__ShowToolTip)(arg)
		self.GetChild("Action_Bar_Img").OnMouseOverOut = lambda : ui.__mem_func__(self.__HideToolTip)()
		self.GetChild("Reaction_Bar_Img").OnMouseOverIn = lambda arg = localeinfo.STAT_TOOLTIP_REACTION : ui.__mem_func__(self.__ShowToolTip)(arg)
		self.GetChild("Reaction_Bar_Img").OnMouseOverOut = lambda : ui.__mem_func__(self.__HideToolTip)()
		## Char Status Info
		#self.GetChild("Char_Info_Status_img").OnMouseOverIn = lambda arg = localeinfo.STAT_TOOLTIP_STAT : ui.__mem_func__(self.__ShowToolTip)(arg)
		#self.GetChild("Char_Info_Status_img").OnMouseOverOut = lambda : ui.__mem_func__(self.__HideToolTip)()
		## Status Plus Points
		self.GetChild("Status_Plus_Label").OnMouseOverIn = lambda arg = localeinfo.STAT_TOOLTIP_POINT : ui.__mem_func__(self.__ShowToolTip)(arg)
		self.GetChild("Status_Plus_Label").OnMouseOverOut = lambda : ui.__mem_func__(self.__HideToolTip)()
		## CHARACTER WINDOW RENEWAL
		## CHARACTER WINDOW RENEWAL COMENTADO
		self.tooltips = {
			## "name_of_object": "tooltip_text"
			#"HTH_IMG": localeinfo.STAT_TOOLTIP_IMG_CON,
			#"INT_IMG": localeinfo.STAT_TOOLTIP_IMG_INT,
			#"STR_IMG": localeinfo.STAT_TOOLTIP_IMG_STR,
			#"DEX_IMG": localeinfo.STAT_TOOLTIP_IMG_DEX,
			#"SUNGMA_STR_IMG": localeinfo.STAT_TOOLTIP_SUNGMA_STR,
			#"SUNGMA_HP_IMG": localeinfo.STAT_TOOLTIP_SUNGMA_HP,
			#"SUNGMA_MOVE_IMG": localeinfo.STAT_TOOLTIP_SUNGMA_MOVE,
			#"SUNGMA_IMMUNE_IMG": localeinfo.STAT_TOOLTIP_SUNGMA_IMMUNE,
			#"HEL_IMG": localeinfo.STAT_TOOLTIP_HP,
			#"SP_IMG": localeinfo.STAT_TOOLTIP_SP,
			#"ATT_IMG": localeinfo.STAT_TOOLTIP_ATT,
			#"DEF_IMG": localeinfo.STAT_TOOLTIP_DEF,
			#"MSPD_IMG": localeinfo.STAT_TOOLTIP_MOVE_SPEED,
			#"ASPD_IMG": localeinfo.STAT_TOOLTIP_ATT_SPEED,
			#"CSPD_IMG": localeinfo.STAT_TOOLTIP_CAST_SPEED,
			#"MATT_IMG": localeinfo.STAT_TOOLTIP_MAG_ATT,
			#"MDEF_IMG": localeinfo.STAT_TOOLTIP_MAG_DEF,
			#"ER_IMG": localeinfo.STAT_TOOLTIP_DODGE_PER,
			#"Active_Skill_Point_Label": localeinfo.STAT_TOOLTIP_POINT,
			#"Tab_Button_02": localeinfo.STAT_TOOLTIP_TAB_SKILL,
			#"Tab_Button_03": localeinfo.STAT_TOOLTIP_TAB_EMOTICON,
			#"Tab_Button_04": localeinfo.STAT_TOOLTIP_TAB_QUEST,
			#"Lv_ToolTip": localeinfo.STAT_TOOLTIP_LEVEL,
			#"Exp_ToolTip": localeinfo.STAT_TOOLTIP_EXP,
			#"change_base_button": localeinfo.STAT_TOOLTIP_BASE_LEVEL,
			## "change_conqueror_button": localeinfo.STAT_TOOLTIP_CONQUEROR_LEVEL,
			## "passive_expanded_btn": localeinfo.STAT_TOOLTIP_PASSIVE,
			#"Status_Plus_Label": localeinfo.STAT_TOOLTIP_POINT_STATUS,
			#"Support_Skill_ToolTip": localeinfo.STAT_TOOLTIP_SUPPORT_SKILL,
			#"Action_Bar_Img": localeinfo.STAT_TOOLTIP_ACTION,
			#"Reaction_Bar_Img": localeinfo.STAT_TOOLTIP_REACTION,
			## "Special_Action_Bar_Img": localeinfo.STAT_TOOLTIP_SPECIAL_ACTION,
		}
		## CHARACTER WINDOW RENEWAL COMENTADO
		for obj, tooltip in self.tooltips.items():
			obj = self.GetChild(obj)
			obj.SetOverInEvent(lambda arg=tooltip : self.ShowToolTip(arg))
			obj.SetOverOutEvent(self.HideToolTip)


		self.soloEmotionSlot = self.GetChild("SoloEmotionSlot")
		self.specialEmotionSlot = self.GetChild("SpecialEmotionSlot")
		self.dualEmotionSlot = self.GetChild("DualEmotionSlot")
		self.__SetEmotionSlot()


		if app.ENABLE_QUEST_RENEWAL:
			self.questScrollBar.SetParent(self.quest_page_board_window)
			for i in xrange(quest.QUEST_CATEGORY_MAX_NUM):
				self.questCategory = ui.SubTitleBar()
				self.questCategory.SetParent(self.questPage)
				self.questCategory.MakeSubTitleBar(210, "red")
				self.questCategory.SetText(quest_label_dict[i])
				self.questCategory.SetSize(210, 16)
				self.questCategory.SetPosition(13, 0)
				self.questCategoryList.append(self.questCategory)

				# self.questCategoryList.append(self.questCategory[i])
				# self.questCategoryList.append(self.GetChild("Quest_Category_0" + str(i)))
				self.questCategoryRenderPos.append(0)

			self.questScrollBar.SetParent(self.questPage)
			self.RearrangeQuestCategories(xrange(quest.QUEST_CATEGORY_MAX_NUM))
		else:


			self.questShowingStartIndex = 0
			self.questScrollBar = self.GetChild("Quest_ScrollBar")
			self.questScrollBar.SetScrollEvent(ui.__mem_func__(self.OnQuestScroll))
			self.questSlot = self.GetChild("Quest_Slot")
			for i in xrange(quest.QUEST_MAX_NUM):
				self.questSlot.HideSlotBaseImage(i)
				self.questSlot.SetCoverButton(i,\
												"d:/ymir work/ui/game/quest/slot_button_01.sub",\
												"d:/ymir work/ui/game/quest/slot_button_02.sub",\
												"d:/ymir work/ui/game/quest/slot_button_03.sub",\
												"d:/ymir work/ui/game/quest/slot_button_03.sub", True)
			
			self.questNameList = []
			self.questLastTimeList = []
			self.questLastCountList = []
			for i in xrange(quest.QUEST_MAX_NUM):
				self.questNameList.append(self.GetChild("Quest_Name_0" + str(i)))
				self.questLastTimeList.append(self.GetChild("Quest_LastTime_0" + str(i)))
				self.questLastCountList.append(self.GetChild("Quest_LastCount_0" + str(i)))
			
			
		self.MainBoard = self.GetChild("board")
		self.ExpandBtn = ui.MakeButton(self.MainBoard, 240, 120, "", "d:/ymir work/ui/game/belt_inventory/", "btn_minimize_normal.tga", "btn_minimize_over.tga", "btn_minimize_down.tga")
		self.ExpandBtn.SetEvent(ui.__mem_func__(self.__ClickExpandButton))
		self.MinimizeBtn = ui.MakeButton(self.MainBoard, 240, 120, "", "d:/ymir work/ui/game/belt_inventory/", "btn_expand_normal.tga", "btn_expand_over.tga", "btn_expand_down.tga")
		self.MinimizeBtn.SetEvent(ui.__mem_func__(self.__ClickMinimizeButton))
			
			
	def __InitCharacterDetailsUIButton(self):
		self.ExpandBtn.Show()
		self.MinimizeBtn.Hide()

	def __ClickExpandButton(self):
		#print "__ClickExpandButton"	
		x, y = self.GetGlobalPosition()
		if not self.chDetailsWnd:
			self.chDetailsWnd = uicharacterdetails.CharacterDetailsUI(self)
			self.chDetailsWnd.Show()
		else:
			self.chDetailsWnd.Show()
		
		self.ExpandBtn.Hide()
		self.MinimizeBtn.Show()
		if app.ENABLE_SKILL_COLOR_SYSTEM:
			if self.skillColorWnd and self.skillColorWnd.IsShow():
				self.skillColorWnd.Hide()

	def __ClickMinimizeButton(self):
		self.chDetailsWnd.Hide()
		self.MinimizeBtn.Hide()
		self.ExpandBtn.Show()

	def AdjustPosition(self, x, y, width):
		self.SetPosition(x - width, y)

	def OnMoveWindow(self, x, y):
		#print "OnMoveWindow x %s y %s" % (x, y)
		if self.chDetailsWnd:
			self.chDetailsWnd.AdjustPosition(x, y)

	def __SetSkillSlotEvent(self):
		for skillPageValue in self.skillPageDict.itervalues():
			skillPageValue.SetSlotStyle(wndMgr.SLOT_STYLE_NONE)
			skillPageValue.SetSelectItemSlotEvent(ui.__mem_func__(self.SelectSkill))
			skillPageValue.SetSelectEmptySlotEvent(ui.__mem_func__(self.SelectEmptySlot))
			skillPageValue.SetUnselectItemSlotEvent(ui.__mem_func__(self.ClickSkillSlot))
			skillPageValue.SetUseSlotEvent(ui.__mem_func__(self.ClickSkillSlot))
			skillPageValue.SetOverInItemEvent(ui.__mem_func__(self.OverInItem))
			skillPageValue.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))
			skillPageValue.SetPressedSlotButtonEvent(ui.__mem_func__(self.OnPressedSlotButton))
			skillPageValue.AppendSlotButton("d:/ymir work/ui/game/windows/btn_plus_up.sub",\
											"d:/ymir work/ui/game/windows/btn_plus_over.sub",\
											"d:/ymir work/ui/game/windows/btn_plus_down.sub")

	def __SetEmotionSlot(self):
		self.emotionToolTip = uitooltip.ToolTip()
		for slot in (self.soloEmotionSlot, self.dualEmotionSlot, self.specialEmotionSlot):
			slot.SetSlotStyle(wndMgr.SLOT_STYLE_NONE)
			slot.SetSelectItemSlotEvent(ui.__mem_func__(self.__SelectEmotion))
			slot.SetUnselectItemSlotEvent(ui.__mem_func__(self.__ClickEmotionSlot))
			slot.SetUseSlotEvent(ui.__mem_func__(self.__ClickEmotionSlot))
			slot.SetOverInItemEvent(ui.__mem_func__(self.__OverInEmotion))
			slot.SetOverOutItemEvent(ui.__mem_func__(self.__OverOutEmotion))
			slot.AppendSlotButton("d:/ymir work/ui/game/windows/btn_plus_up.sub", "d:/ymir work/ui/game/windows/btn_plus_over.sub", "d:/ymir work/ui/game/windows/btn_plus_down.sub")

		for slotIdx, datadict in emotion.EMOTION_DICT.items():
			emotionIdx = slotIdx

			slot = self.soloEmotionSlot
			if slotIdx > 50:
				slot = self.dualEmotionSlot
			elif slotIdx >= 19:
				slot = self.specialEmotionSlot
			
			slot.SetEmotionSlot(slotIdx, emotionIdx)
			slot.SetCoverButton(slotIdx)

	def __SelectEmotion(self, slotIndex):
		if not slotIndex in emotion.EMOTION_DICT:
			return

		if app.IsPressed(app.DIK_LCONTROL):
			player.RequestAddToEmptyLocalQuickSlot(player.SLOT_TYPE_EMOTION, slotIndex)
			return

		mousemodule.mouseController.AttachObject(self, player.SLOT_TYPE_EMOTION, slotIndex, slotIndex)

	def __ClickEmotionSlot(self, slotIndex):
		print "click emotion"
		if not slotIndex in emotion.EMOTION_DICT:
			return

		print "check acting"
		if player.IsActingEmotion():
			return

		command = emotion.EMOTION_DICT[slotIndex]["command"]
		print "command", command

		if slotIndex > 50:
			vid = player.GetTargetVID()

			if 0 == vid or vid == player.GetMainCharacterIndex() or chr.IsNPC(vid) or chr.IsEnemy(vid):
				import chat
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.EMOTION_CHOOSE_ONE)
				return

			command += " " + chr.GetNameByVID(vid)

		print "send_command", command
		net.SendChatPacket(command)

	def ActEmotion(self, emotionIndex):
		self.__ClickEmotionSlot(emotionIndex)

	def __OverInEmotion(self, slotIndex):
		if self.toolTip == None:
			self.Close()
			
		if self.emotionToolTip:

			if not slotIndex in emotion.EMOTION_DICT:
				return

			self.emotionToolTip.ClearToolTip()
			self.emotionToolTip.SetTitle(emotion.EMOTION_DICT[slotIndex]["name"])
			self.emotionToolTip.AlignHorizonalCenter()
			self.emotionToolTip.ShowToolTip()

	def __OverOutEmotion(self):
		if self.emotionToolTip:
			self.emotionToolTip.HideToolTip()

	def __BindEvent(self):
		for i in xrange(len(self.skillGroupButton)):
			self.skillGroupButton[i].SetEvent(lambda arg=i: self.__SelectSkillGroup(arg))
		
		if app.ENABLE_NEW_PASSIVE_SKILLS:
			self.skillGroupButton3.SetEvent(lambda arg=i: self.__SelectSkillGroup(3))

		self.RefreshQuest()
		self.__HideJobToolTip()

		for (tabKey, tabButton) in self.tabButtonDict.items():
			tabButton.SetEvent(ui.__mem_func__(self.__OnClickTabButton), tabKey)
		## CHARACTER WINDOW RENEWAL COMENTADO
		for (tabKey, tabButton) in self.tabSungmaButtonDict.items():
			tabButton.SetEvent(ui.__mem_func__(self.__OnClickTabSungmaButton), tabKey)
		## CHARACTER WINDOW RENEWAL COMENTADO
		for (statusPlusKey, statusPlusButton) in self.statusPlusButtonDict.items():
			statusPlusButton.SAFE_SetEvent(self.__OnClickStatusPlusButton, statusPlusKey)
			## CHARACTER WINDOW RENEWAL COMENTADO
			statusPlusButton.SetShowToolTipEvent(ui.__mem_func__(self.__OverInStatButton), statusPlusKey)
			statusPlusButton.SetHideToolTipEvent(ui.__mem_func__(self.__OverOutStatButton), statusPlusKey)
			## CHARACTER WINDOW RENEWAL COMENTADO
		for (statusMinusKey, statusMinusButton) in self.statusMinusButtonDict.items():
			statusMinusButton.SAFE_SetEvent(self.__OnClickStatusMinusButton, statusMinusKey)
			## CHARACTER WINDOW RENEWAL COMENTADO
			statusMinusButton.SetShowToolTipEvent(ui.__mem_func__(self.__OverInStatMinusButton), statusMinusKey)
			statusMinusButton.SetHideToolTipEvent(ui.__mem_func__(self.__OverOutStatMinusButton), statusMinusKey)
			## CHARACTER WINDOW RENEWAL COMENTADO
		for titleBarValue in self.titleBarDict.itervalues():
			titleBarValue.SetCloseEvent(ui.__mem_func__(self.Close))

		if app.ENABLE_QUEST_RENEWAL:
			self.questTitleBar.SetCloseEvent(ui.__mem_func__(self.Close))
			self.questScrollBar.SetScrollEvent(ui.__mem_func__(self.__OnScrollQuest))

			for i in xrange(quest.QUEST_CATEGORY_MAX_NUM):
				self.questCategoryList[i].SetEvent(ui.__mem_func__(self.__OnClickQuestCategoryButton), i)
		else:
			self.questSlot.SetSelectItemSlotEvent(ui.__mem_func__(self.__SelectQuest))

	def __LoadWindow(self):
		if self.isLoaded == 1:
			return

		self.isLoaded = 1

		try:
		## CHARACTER WINDOW RENEWAL COMENTADO
			if localeinfo.IsARABIC() or localeinfo.IsVIETNAM() or localeinfo.IsJAPAN():
				self.__LoadScript(uiscriptlocale.LOCALE_UISCRIPT_PATH + "characterwindow.py")
			else:
				self.__LoadScript("uiscript/characterwindow.py")
		## CHARACTER WINDOW RENEWAL COMENTADO
			self.__BindObject()
			self.__BindEvent()
		except:
			import exception
			exception.Abort("CharacterWindow.__LoadWindow")

		#self.tabButtonDict["EMOTICON"].Disable()
		self.SetState("STATUS")
		## CHARACTER WINDOW RENEWAL COMENTADO
		self.SetSubState("BASE")
		## CHARACTER WINDOW RENEWAL COMENTADO
	def Destroy(self):
		self.ClearDictionary()

		self.__Initialize()


	def Close(self):
		if 0 != self.toolTipSkill and self.toolTipSkill != None:
			self.toolTipSkill.HideToolTip()
		if self.chDetailsWnd and self.chDetailsWnd.IsShow():
			self.chDetailsWnd.Hide()

		if app.ENABLE_SKILL_COLOR_SYSTEM:
			if self.skillColorWnd and self.skillColorWnd.IsShow():
				self.skillColorWnd.Hide()

		self.Hide()

	def SetSkillToolTip(self, toolTipSkill):
		self.toolTipSkill = toolTipSkill
	if app.ENABLE_RENEWAL_ADD_STATS:
		def __OnClickStatusPlusButton(self, statusKey):
			#self.AppendSpace(5)
			#self.AppendTextLine("|Eemoji/key_ctrl|e + |Eemoji/key_lclick|e - Add x10",self.COLOR_RENDER_TARGET)
			cmd = self.statusPlusCommandDict[statusKey]
	
			if app.IsPressed(app.DIK_LCONTROL):
				cmd = cmd + "10"
			else:
				cmd = cmd + "1"
				
			net.SendChatPacket(cmd)
	else:
		def __OnClickStatusPlusButton(self, statusKey):
			try:
				if app.IsPressed(app.DIK_LCONTROL) or app.IsPressed(app.DIK_RCONTROL):
					statusPlusCommand=self.status2PlusCommandDict[statusKey]
					net.SendChatPacket(statusPlusCommand)
				else:
					statusPlusCommand=self.statusPlusCommandDict[statusKey]
					net.SendChatPacket(statusPlusCommand)
			except KeyError, msg:
				dbg.TraceError("CharacterWindow.__OnClickStatusPlusButton KeyError: %s", msg)

	def __OnClickStatusMinusButton(self, statusKey):
		try:
			statusMinusCommand=self.statusMinusCommandDict[statusKey]
			net.SendChatPacket(statusMinusCommand)
		except KeyError, msg:
			dbg.TraceError("CharacterWindow.__OnClickStatusMinusButton KeyError: %s", msg)


	def __OnClickTabButton(self, stateKey):
		self.SetState(stateKey)
	## CHARACTER WINDOW RENEWAL COMENTADO
	def __OnClickTabSungmaButton(self, stateKey):
		self.SetSubState(stateKey)
	## CHARACTER WINDOW RENEWAL COMENTADO
	def SetState(self, stateKey):

		self.state = stateKey


		if app.ENABLE_QUEST_RENEWAL:
			if stateKey != "QUEST":
				self.questPage.Hide()
			else:
				self.__LoadQuestCategory()

		for (tabKey, tabButton) in self.tabButtonDict.items():
			if stateKey!=tabKey:
				tabButton.SetUp()

		for tabValue in self.tabDict.itervalues():
			tabValue.Hide()

		for pageValue in self.pageDict.itervalues():
			pageValue.Hide()

		for titleBarValue in self.titleBarDict.itervalues():
			titleBarValue.Hide()

		self.titleBarDict[stateKey].Show()
		self.tabDict[stateKey].Show()
		self.pageDict[stateKey].Show()
## CHARACTER WINDOW RENEWAL COMENTADO
	def ShowToolTip(self, tooltip):
		self.toolTipConquerorInfoButton.ClearToolTip()
		self.toolTipConquerorInfoButton.AutoAppendTextLine(tooltip)
		self.toolTipConquerorInfoButton.AlignHorizonalCenter()
		
		self.toolTipConquerorInfoButton.ShowToolTip()
		
	def HideToolTip(self):
		self.toolTipConquerorInfoButton.HideToolTip()
## CHARACTER WINDOW RENEWAL COMENTADO
	def GetState(self):
		return self.state

## CHARACTER WINDOW RENEWAL COMENTADO
	def SetSubState(self, stateSubKey):
		
		self.substate = stateSubKey

		for (tabKey, tabButton) in self.tabSungmaButtonDict.items():
			if stateSubKey!=tabKey:
				tabButton.SetUp()

		for pageValue in self.SungmaPageDict.itervalues():
			pageValue.Hide()

		self.SungmaPageDict[stateSubKey].Show()
## CHARACTER WINDOW RENEWAL COMENTADO

	def __GetTotalAtkText(self):
		minAtk=player.GetStatus(player.ATT_MIN)
		maxAtk=player.GetStatus(player.ATT_MAX)
		atkBonus=player.GetStatus(player.ATT_BONUS)
		attackerBonus=player.GetStatus(player.ATTACKER_BONUS)

		if minAtk==maxAtk:
			return "%d" % (minAtk+atkBonus+attackerBonus)
		else:
			return "%d-%d" % (minAtk+atkBonus+attackerBonus, maxAtk+atkBonus+attackerBonus)

	def __GetTotalMagAtkText(self):
		minMagAtk=player.GetStatus(player.MAG_ATT)+player.GetStatus(player.MIN_MAGIC_WEP)
		maxMagAtk=player.GetStatus(player.MAG_ATT)+player.GetStatus(player.MAX_MAGIC_WEP)

		if minMagAtk==maxMagAtk:
			return "%d" % (minMagAtk)
		else:
			return "%d-%d" % (minMagAtk, maxMagAtk)

	def __GetTotalDefText(self):
		defValue=player.GetStatus(player.DEF_GRADE)
		if constinfo.ADD_DEF_BONUS_ENABLE:
			defValue+=player.GetStatus(player.DEF_BONUS)
		return "%d" % (defValue)

	def RefreshStatus(self):
		if self.isLoaded==0:
			return

		try:
			self.GetChild("Level_Value").SetText(str(player.GetStatus(player.LEVEL)))
			self.GetChild("Exp_Value").SetText(str(unsigned32(player.GetEXP())))
			self.GetChild("RestExp_Value").SetText(str(unsigned32(player.GetStatus(player.NEXT_EXP)) - unsigned32(player.GetStatus(player.EXP))))
			self.GetChild("HP_Value").SetText(str(player.GetStatus(player.HP)) + '/' + str(player.GetStatus(player.MAX_HP)))
			self.GetChild("SP_Value").SetText(str(player.GetStatus(player.SP)) + '/' + str(player.GetStatus(player.MAX_SP)))

			self.GetChild("STR_Value").SetText(str(player.GetStatus(player.ST)))
			self.GetChild("DEX_Value").SetText(str(player.GetStatus(player.DX)))
			self.GetChild("HTH_Value").SetText(str(player.GetStatus(player.HT)))
			self.GetChild("INT_Value").SetText(str(player.GetStatus(player.IQ)))

			self.GetChild("ATT_Value").SetText(self.__GetTotalAtkText())
			self.GetChild("DEF_Value").SetText(self.__GetTotalDefText())

			self.GetChild("MATT_Value").SetText(self.__GetTotalMagAtkText())
			#self.GetChild("MATT_Value").SetText(str(player.GetStatus(player.MAG_ATT)))

			self.GetChild("MDEF_Value").SetText(str(player.GetStatus(player.MAG_DEF)))
			self.GetChild("ASPD_Value").SetText(str(player.GetStatus(player.ATT_SPEED)))
			self.GetChild("MSPD_Value").SetText(str(player.GetStatus(player.MOVING_SPEED)))
			self.GetChild("CSPD_Value").SetText(str(player.GetStatus(player.CASTING_SPEED)))
			self.GetChild("ER_Value").SetText(str(player.GetStatus(player.EVADE_RATE)))

		except:
			#import exception
			#exception.Abort("CharacterWindow.RefreshStatus.BindObject")
			## ������ ƨ�� ����
			pass

		self.__RefreshStatusPlusButtonList()
		self.__RefreshStatusMinusButtonList()
		self.RefreshAlignment()

		if self.refreshToolTip:
			self.refreshToolTip()
		
		if self.chDetailsWnd:
			if self.chDetailsWnd.IsShow():
				self.chDetailsWnd.Refresh()

	def __RefreshStatusPlusButtonList(self):
		if self.isLoaded==0:
			return

		statusPlusPoint=player.GetStatus(player.STAT)

		if statusPlusPoint>0:
## CHARACTER WINDOW RENEWAL COMENTADO		
			self.statusPlusValue.SetText(str(statusPlusPoint))
			self.statusPlusLabel.Show()
## CHARACTER WINDOW RENEWAL COMENTADO			
			self.ShowStatusPlusButtonList()
		else:
## CHARACTER WINDOW RENEWAL COMENTADO		
			self.statusPlusValue.SetText(str(0))
			self.statusPlusLabel.Hide()
## CHARACTER WINDOW RENEWAL COMENTADO			
			self.HideStatusPlusButtonList()

	def __RefreshStatusMinusButtonList(self):
		if self.isLoaded==0:
			return

		statusMinusPoint=self.__GetStatMinusPoint()

		if statusMinusPoint>0:
			self.__ShowStatusMinusButtonList()
		else:
			self.__HideStatusMinusButtonList()

	def RefreshAlignment(self):
		point, grade = player.GetAlignmentData()

		import colorinfo
		COLOR_DICT = {	0 : colorinfo.TITLE_RGB_GOOD_4,
						1 : colorinfo.TITLE_RGB_GOOD_3,
						2 : colorinfo.TITLE_RGB_GOOD_2,
						3 : colorinfo.TITLE_RGB_GOOD_1,
						4 : colorinfo.TITLE_RGB_NORMAL,
						5 : colorinfo.TITLE_RGB_EVIL_1,
						6 : colorinfo.TITLE_RGB_EVIL_2,
						7 : colorinfo.TITLE_RGB_EVIL_3,
						8 : colorinfo.TITLE_RGB_EVIL_4, }
		colorList = COLOR_DICT.get(grade, colorinfo.TITLE_RGB_NORMAL)
		gradeColor = ui.GenerateColor(colorList[0], colorList[1], colorList[2])

		self.toolTipAlignment.ClearToolTip()
		self.toolTipAlignment.AutoAppendTextLine(localeinfo.TITLE_NAME_LIST[grade], gradeColor)
		self.toolTipAlignment.AutoAppendTextLine(localeinfo.ALIGNMENT_NAME + str(point))
		self.toolTipAlignment.AlignHorizonalCenter()

	def __ShowStatusMinusButtonList(self):
		for (stateMinusKey, statusMinusButton) in self.statusMinusButtonDict.items():
			statusMinusButton.Show()

	def __HideStatusMinusButtonList(self):
		for (stateMinusKey, statusMinusButton) in self.statusMinusButtonDict.items():
			statusMinusButton.Hide()

	def ShowStatusPlusButtonList(self):
		for (statePlusKey, statusPlusButton) in self.statusPlusButtonDict.items():
			statusPlusButton.Show()

	def HideStatusPlusButtonList(self):
		for (statePlusKey, statusPlusButton) in self.statusPlusButtonDict.items():
			statusPlusButton.Hide()

	def SelectSkill(self, skillSlotIndex):

		if app.ENABLE_NEW_PASSIVE_SKILLS and self.skillGroupButton3.IsDown() and skillSlotIndex < 100:
			return

		mouseController = mousemodule.mouseController

		if False == mouseController.isAttached():

			srcSlotIndex = self.__RealSkillSlotToSourceSlot(skillSlotIndex)
			selectedSkillIndex = player.GetSkillIndex(srcSlotIndex)

			if skill.CanUseSkill(selectedSkillIndex):

				if app.IsPressed(app.DIK_LCONTROL):

					player.RequestAddToEmptyLocalQuickSlot(player.SLOT_TYPE_SKILL, srcSlotIndex)
					return

				mouseController.AttachObject(self, player.SLOT_TYPE_SKILL, srcSlotIndex, selectedSkillIndex)

		else:

			mouseController.DeattachObject()

	def SelectEmptySlot(self, SlotIndex):
		mousemodule.mouseController.DeattachObject()
## CHARACTER WINDOW RENEWAL COMENTADO		
	def __ShowToolTip(self, desc):
		if not self.toolTip:
			return

		descLen = len(desc)
		self.toolTip.ClearToolTip()
		self.toolTip.SetThinBoardSize(11 * descLen)
		self.toolTip.AutoAppendTextLine(desc)
		self.toolTip.AlignHorizonalCenter()
		self.toolTip.Show()

	def __HideToolTip(self):
		self.__HideStatToolTip()

	def __ShowToolTipButton(self, desc):
		self.__ShowToolTip(desc)

	def __ShowToolTipImg(self, event_type, text):
		if "mouse_over_in" == event_type :
			textLen = len(text)

			self.toolTip.ClearToolTip()
			self.toolTip.SetThinBoardSize(11 * arglen)
			# self.toolTip.SetToolTipPosition(pos_x + 50, pos_y + 50)
			self.toolTip.AppendTextLine(text, 0xffffffff)
			self.toolTip.Show()
		elif "mouse_over_out" == event_type:
			self.__HideToolTip()

	def __TogglePassiveAttrWindow(self, event_type):
		pass

	def __ToolTipProgress(self):
		pass
## CHARACTER WINDOW RENEWAL COMENTADO		
	## ToolTip
	def OverInItem(self, slotNumber):

		if mousemodule.mouseController.isAttached():
			return

		if 0 == self.toolTipSkill or self.toolTipSkill == None:
			return

		srcSlotIndex = self.__RealSkillSlotToSourceSlot(slotNumber)
		if app.ENABLE_NEW_PASSIVE_SKILLS and self.skillGroupButton3.IsDown() and slotNumber < 100:
			srcSlotIndex = self.__RealSkillSlotToSourceSlot(slotNumber + 220)
			if srcSlotIndex-220 >= 40:
				srcSlotIndex -= 40
			elif srcSlotIndex-220 >= 20:
				srcSlotIndex -= 20
		
		skillIndex = player.GetSkillIndex(srcSlotIndex)
		skillLevel = player.GetSkillLevel(srcSlotIndex)
		skillGrade = player.GetSkillGrade(srcSlotIndex)
		skillType = skill.GetSkillType(skillIndex)

		## ACTIVE
		if skill.SKILL_TYPE_ACTIVE == skillType:
			overInSkillGrade = self.__GetSkillGradeFromSlot(slotNumber)
			if overInSkillGrade == skill.SKILL_GRADE_COUNT-1 and skillGrade == skill.SKILL_GRADE_COUNT:
				self.toolTipSkill.SetSkillNew(srcSlotIndex, skillIndex, skillGrade, skillLevel)
			elif overInSkillGrade == skillGrade:
				self.toolTipSkill.SetSkillNew(srcSlotIndex, skillIndex, overInSkillGrade, skillLevel)
			else:
				self.toolTipSkill.SetSkillOnlyName(srcSlotIndex, skillIndex, overInSkillGrade)

		else:
			self.toolTipSkill.SetSkillNew(srcSlotIndex, skillIndex, skillGrade, skillLevel)

	def OverOutItem(self):
		if self.toolTipSkill == None:
			self.Close()
		if 0 != self.toolTipSkill and self.toolTipSkill != None :
			self.toolTipSkill.HideToolTip()

	## Quest
	def __SelectQuest(self, slotIndex):
		if self.toolTip == None:
			self.Close()
		
		
		if app.ENABLE_QUEST_RENEWAL:
			questIndex = self.questIndexMap[slotIndex]

			if not questIndex in self.questClicked:
				self.questClicked.append(questIndex)
		else:
			questIndex = quest.GetQuestIndex(self.questShowingStartIndex + slotIndex)


		import event
		event.QuestButtonClick(-2147483648 + questIndex)

	def RefreshQuest(self):
		if self.toolTip == None:
			self.Close()

		if app.ENABLE_QUEST_RENEWAL:
			if self.isLoaded == 0 or self.state != "QUEST":
				return

			for cat in self.questOpenedCategories:
				self.RefreshQuestCategory(cat)

			self.RefreshQuestCategoriesCount()
		else:
			if self.isLoaded==0:
				return

		questCount = quest.GetQuestCount()
		questRange = range(quest.QUEST_MAX_NUM)

		if questCount > quest.QUEST_MAX_NUM:
			self.questScrollBar.Show()
		else:
			self.questScrollBar.Hide()

		for i in questRange[:questCount]:
			(questName, questIcon, questCounterName, questCounterValue) = quest.GetQuestData(self.questShowingStartIndex+i)
			self.questNameList[i].SetText(questName)
			self.questNameList[i].Show()
			self.questLastCountList[i].Show()
			self.questLastTimeList[i].Show()

			if len(questCounterName) > 0:
				self.questLastCountList[i].SetText("%s : %d" % (questCounterName, questCounterValue))
			else:
				self.questLastCountList[i].SetText("")

			## Icon
			self.questSlot.SetSlot(i, i, 1, 1, questIcon)

		for i in questRange[questCount:]:
			self.questNameList[i].Hide()
			self.questLastTimeList[i].Hide()
			self.questLastCountList[i].Hide()
			self.questSlot.ClearSlot(i)
			self.questSlot.HideSlotBaseImage(i)

		self.__UpdateQuestClock()

	def __UpdateQuestClock(self):
		if self.toolTip == None:
			self.Close()
		if "QUEST" == self.state:
			if app.ENABLE_QUEST_RENEWAL:
				for clock in self.questClockList:
					clockText = localeinfo.QUEST_UNLIMITED_TIME

					if clock.GetProperty("idx"):
						(lastName, lastTime) = quest.GetQuestLastTime(clock.GetProperty("idx"))

						if len(lastName) > 0:
							if lastTime <= 0:
								clockText = localeinfo.QUEST_TIMEOVER
							else:
								questLastMinute = lastTime / 60
								questLastSecond = lastTime % 60

								clockText = lastName + " : "

								if questLastMinute > 0:
									clockText += str(questLastMinute) + localeinfo.QUEST_MIN
									if questLastSecond > 0:
										clockText += " "

								if questLastSecond > 0:
									clockText += str(questLastSecond) + localeinfo.QUEST_SEC

					clock.SetText(clockText)
			else:
				# QUEST_LIMIT_COUNT_BUG_FIX
				for i in xrange(min(quest.GetQuestCount(), quest.QUEST_MAX_NUM)):
				# END_OF_QUEST_LIMIT_COUNT_BUG_FIX
					(lastName, lastTime) = quest.GetQuestLastTime(i)

					clockText = localeinfo.QUEST_UNLIMITED_TIME
					if len(lastName) > 0:

						if lastTime <= 0:
							clockText = localeinfo.QUEST_TIMEOVER

						else:
							questLastMinute = lastTime / 60
							questLastSecond = lastTime % 60

							clockText = lastName + " : "

							if questLastMinute > 0:
								clockText += str(questLastMinute) + localeinfo.QUEST_MIN
								if questLastSecond > 0:
									clockText += " "

							if questLastSecond > 0:
								clockText += str(questLastSecond) + localeinfo.QUEST_SEC

					self.questLastTimeList[i].SetText(clockText)


	def __GetStatMinusPoint(self):
		POINT_STAT_RESET_COUNT = 112
		return player.GetStatus(POINT_STAT_RESET_COUNT)

	def __OverInStatMinusButton(self, stat):
		try:
			self.__ShowStatToolTip(self.STAT_MINUS_DESCRIPTION[stat] % self.__GetStatMinusPoint())
		except KeyError:
			pass

		self.refreshToolTip = lambda arg=stat: self.__OverInStatMinusButton(arg)

	def __OverOutStatMinusButton(self):
		self.__HideStatToolTip()
		self.refreshToolTip = 0
        
	if app.ENABLE_RENEWAL_ADD_STATS:
		def __OverInStatButton(self, stat):
			try:
				self.__ShowStatToolTip(self.STAT_DESCRIPTION[stat], localeinfo.EMOJI_CHARACTER_STATS_ADD, True)
			except KeyError:
				pass
	else:
		def __OverInStatButton(self, stat):
			try:
				self.__ShowStatToolTip(self.STAT_DESCRIPTION[stat])
			except KeyError:
				pass

	def __OverOutStatButton(self):
		self.__HideStatToolTip()

	if app.ENABLE_RENEWAL_ADD_STATS:
		def __ShowStatToolTip(self, statDesc, statDesc2 = False, arg2 = False):
			self.toolTip.ClearToolTip()
			self.toolTip.AppendTextLine(statDesc)
			
			if arg2 == True:
				self.toolTip.AppendTextLine(statDesc2)
			self.toolTip.Show()
	else:
		def __ShowStatToolTip(self, statDesc):
			if self.toolTip == None:
				self.Close()
			if self.toolTip != None :
				self.toolTip.ClearToolTip()
				self.toolTip.AppendTextLine(localeinfo.INFO_INCREASE_CHAR_STAT)
				self.toolTip.AppendTextLine(statDesc)
				self.toolTip.Show()

	def __HideStatToolTip(self):
		if self.toolTip == None:
			self.Close()
		if self.toolTip != None :
			self.toolTip.Hide()

	def OnPressEscapeKey(self):
		self.Close()
		return True

	def OnUpdate(self):
		self.__UpdateQuestClock()

		if app.ENABLE_SKILL_COLOR_SYSTEM:
			if self.skillColorWnd:
				self.__UpdateSkillColorPosition()

	## Skill Process
	def __RefreshSkillPage(self, name, slotCount):
		global SHOW_LIMIT_SUPPORT_SKILL_LIST
		
		if app.ENABLE_NEW_PASSIVE_SKILLS and self.skillGroupButton3.IsDown():
			skillPage = self.skillPageDict["ACTIVE"]
			getSkillMaxLevel = skill.GetSkillMaxLevel
			
			for i in xrange(self.ACTIVE_PAGE_SLOT_COUNT+1):
				slotIndex = i + skillPage.GetStartIndex() + 220
				skillIndex = player.GetSkillIndex(slotIndex)
				for j in xrange(skill.SKILL_GRADE_COUNT):
					skillPage.ClearSlot(self.__GetRealSkillSlot(j, i))
				
				if skillIndex == 0:
					continue
				
				skillGrade = player.GetSkillGrade(slotIndex)
				skillLevel = player.GetSkillLevel(slotIndex)
				skillType = skill.GetSkillType(skillIndex)
				for j in xrange(skill.SKILL_GRADE_COUNT):
					realSlotIndex = self.__GetRealSkillSlot(j, slotIndex - 220)
					
					skillPage.SetSkillSlotNew(realSlotIndex, skillIndex, j, skillLevel)
					skillPage.SetCoverButton(realSlotIndex)
					
					if (skillGrade == skill.SKILL_GRADE_COUNT) and j == (skill.SKILL_GRADE_COUNT-1):
						skillPage.SetSlotCountNew(realSlotIndex, skillGrade, skillLevel)
					elif (not self.__CanUseSkillNow()) or (skillGrade != j):
						skillPage.SetSlotCount(realSlotIndex, 0)
						skillPage.DisableCoverButton(realSlotIndex)
					else:
						skillPage.SetSlotCountNew(realSlotIndex, skillGrade, skillLevel)
				
				skillPage.RefreshSlot()
			
			for i in xrange(12):
				skillPage = self.skillPageDict["SUPPORT"]
				slotIndex = i + 101
				skillPage.ClearSlot(slotIndex)
				skillIndex = player.GetSkillIndex(slotIndex)
				skillGrade = player.GetSkillGrade(slotIndex)
				skillLevel = player.GetSkillLevel(slotIndex)
				if skillIndex == 0:
					continue
				
				if player.SKILL_INDEX_RIDING == skillIndex:
					if 1 == skillGrade:
						skillLevel += 19
					elif 2 == skillGrade:
						skillLevel += 29
					elif 3 == skillGrade:
						skillLevel = 40

					skillPage.SetSkillSlotNew(slotIndex, skillIndex, max(skillLevel-1, 0), skillLevel)
					skillPage.SetSlotCount(slotIndex, skillLevel)
				else:
					realSlotIndex = self.__GetETCSkillRealSlotIndex(slotIndex)
					skillPage.SetSkillSlot(realSlotIndex, skillIndex, skillLevel)
					skillPage.SetSlotCountNew(realSlotIndex, skillGrade, skillLevel)
					
					if skill.CanUseSkill(skillIndex):
						skillPage.SetCoverButton(realSlotIndex)
				
				skillPage.RefreshSlot()
			
			return
	
		skillPage = self.skillPageDict[name]

		startSlotIndex = skillPage.GetStartIndex()
		if "ACTIVE" == name:
			if self.PAGE_HORSE == self.curSelectedSkillGroup:
				startSlotIndex += slotCount

		getSkillType=skill.GetSkillType
		getSkillIndex=player.GetSkillIndex
		getSkillGrade=player.GetSkillGrade
		getSkillLevel=player.GetSkillLevel
		getSkillLevelUpPoint=skill.GetSkillLevelUpPoint
		getSkillMaxLevel=skill.GetSkillMaxLevel
		for i in xrange(slotCount+1):
			slotIndex = i + startSlotIndex
			skillIndex = getSkillIndex(slotIndex)

			for j in xrange(skill.SKILL_GRADE_COUNT):
				skillPage.ClearSlot(self.__GetRealSkillSlot(j, i))

			if 0 == skillIndex:
				continue

			skillGrade = getSkillGrade(slotIndex)
			skillLevel = getSkillLevel(slotIndex)
			skillType = getSkillType(skillIndex)

			## �¸� ��ų ���� ó��
			if player.SKILL_INDEX_RIDING == skillIndex:
				if 1 == skillGrade:
					skillLevel += 19
				elif 2 == skillGrade:
					skillLevel += 29
				elif 3 == skillGrade:
					skillLevel = 40

				skillPage.SetSkillSlotNew(slotIndex, skillIndex, max(skillLevel-1, 0), skillLevel)
				skillPage.SetSlotCount(slotIndex, skillLevel)

			## ACTIVE
			elif skill.SKILL_TYPE_ACTIVE == skillType:
				for j in xrange(skill.SKILL_GRADE_COUNT):
					realSlotIndex = self.__GetRealSkillSlot(j, slotIndex)
					skillPage.SetSkillSlotNew(realSlotIndex, skillIndex, j, skillLevel)
					skillPage.SetCoverButton(realSlotIndex)

					if (skillGrade == skill.SKILL_GRADE_COUNT) and j == (skill.SKILL_GRADE_COUNT-1):
						skillPage.SetSlotCountNew(realSlotIndex, skillGrade, skillLevel)
					elif (not self.__CanUseSkillNow()) or (skillGrade != j):
						skillPage.SetSlotCount(realSlotIndex, 0)
						skillPage.DisableCoverButton(realSlotIndex)
					else:
						skillPage.SetSlotCountNew(realSlotIndex, skillGrade, skillLevel)

			## �׿�
			else:
				if not SHOW_LIMIT_SUPPORT_SKILL_LIST or skillIndex in SHOW_LIMIT_SUPPORT_SKILL_LIST:
					realSlotIndex = self.__GetETCSkillRealSlotIndex(slotIndex)
					skillPage.SetSkillSlot(realSlotIndex, skillIndex, skillLevel)
					skillPage.SetSlotCountNew(realSlotIndex, skillGrade, skillLevel)
					if skill.CanUseSkill(skillIndex):
						skillPage.SetCoverButton(realSlotIndex)

			skillPage.RefreshSlot()

		if app.ENABLE_SKILL_COLOR_SYSTEM:
			if "ACTIVE" == name:
				if self.PAGE_HORSE != self.curSelectedSkillGroup:
					self.__CreateSkillColorButton(skillPage)
				else:
					self.skillColorButton = []

	def RefreshSkill(self):

		if self.isLoaded==0:
			return

		if self.__IsChangedHorseRidingSkillLevel():
			self.RefreshCharacter()
			return


		global SHOW_ONLY_ACTIVE_SKILL
		if SHOW_ONLY_ACTIVE_SKILL:
			self.__RefreshSkillPage("ACTIVE", self.ACTIVE_PAGE_SLOT_COUNT)
		else:
			self.__RefreshSkillPage("ACTIVE", self.ACTIVE_PAGE_SLOT_COUNT)
			self.__RefreshSkillPage("SUPPORT", self.SUPPORT_PAGE_SLOT_COUNT)

		self.RefreshSkillPlusButtonList()

	def CanShowPlusButton(self, skillIndex, skillLevel, curStatPoint):

		if app.ENABLE_NEW_PASSIVE_SKILLS and self.skillGroupButton3.IsDown():
			return False

		## ��ų�� ������
		if 0 == skillIndex:
			return False

		## ������ ������ �����Ѵٸ�
		if not skill.CanLevelUpSkill(skillIndex, skillLevel):
			return False

		return True

	def __RefreshSkillPlusButton(self, name):
		global HIDE_SUPPORT_SKILL_POINT
		if HIDE_SUPPORT_SKILL_POINT and "SUPPORT" == name:
			return

		slotWindow = self.skillPageDict[name]
		slotWindow.HideAllSlotButton()

		slotStatType = self.skillPageStatDict[name]
		if 0 == slotStatType:
			return

		statPoint = player.GetStatus(slotStatType)
		startSlotIndex = slotWindow.GetStartIndex()
		if "HORSE" == name:
			startSlotIndex += self.ACTIVE_PAGE_SLOT_COUNT

		if statPoint > 0:
			for i in xrange(self.PAGE_SLOT_COUNT):
				slotIndex = i + startSlotIndex
				skillIndex = player.GetSkillIndex(slotIndex)
				skillGrade = player.GetSkillGrade(slotIndex)
				skillLevel = player.GetSkillLevel(slotIndex)

				if skillIndex == 0:
					continue
				if skillGrade != 0:
					continue

				if name == "HORSE":
					if player.GetStatus(player.LEVEL) >= skill.GetSkillLevelLimit(skillIndex):
						if skillLevel < 20:
							slotWindow.ShowSlotButton(self.__GetETCSkillRealSlotIndex(slotIndex))

				else:
					if "SUPPORT" == name:
						if not SHOW_LIMIT_SUPPORT_SKILL_LIST or skillIndex in SHOW_LIMIT_SUPPORT_SKILL_LIST:
							if self.CanShowPlusButton(skillIndex, skillLevel, statPoint):
								slotWindow.ShowSlotButton(slotIndex)
					else:
						if self.CanShowPlusButton(skillIndex, skillLevel, statPoint):
							slotWindow.ShowSlotButton(slotIndex)

	if app.ENABLE_SKILL_COLOR_SYSTEM:
		def __IsColorableSkill(self, skillIndex):
			if not constinfo.SKILL_COLOR_BUFF_ONLY:
				return True

			WARRIOR_SG = [
				[3, 4], # WARRIOR SG1
				[19] # WARRIOR SG2
			]
			NINJA_SG = [
				[34], # NINJA SG1
				[49] # NINJA SG2
			]
			SURA_SG = [
				[63, 64, 65], # SURA SG1
				[78, 79] # SURA SG2
			]
			SHAMAN_SG = [
				[94, 95, 96], # SHAMAN SG1
				[109, 110, 111] # SHAMAN SG1
			]

			if skillIndex in WARRIOR_SG[0] or skillIndex in WARRIOR_SG[1] or\
				skillIndex in NINJA_SG[0] or skillIndex in NINJA_SG[1] or\
				skillIndex in SURA_SG[0] or skillIndex in SURA_SG[1] or\
				skillIndex in SHAMAN_SG[0] or skillIndex in SHAMAN_SG[1]:
				return True
			return False

		def __CreateSkillColorButton(self, parent):
			self.skillColorButton = []

			xPos, yPos = 0, 0
			for idx in xrange(self.PAGE_SLOT_COUNT):
				skillSlot = idx
				if skillSlot < 6:
					if (skillSlot % 2) == 0:
						xPos = 75
						yPos = 4 + (skillSlot / 2 * 36)
					else:
						xPos = 187
						yPos = 4 + (skillSlot / 2 * 36)

					skillIndex = player.GetSkillIndex(skillSlot + 1)
					skillMaxGrade = 3

					if len(self.skillColorButton) == skillSlot:
						self.skillColorButton.append([])
						self.skillColorButton[skillSlot] = ui.Button()
						self.skillColorButton[skillSlot].SetParent(parent)
						self.skillColorButton[skillSlot].SetUpVisual("d:/ymir work/ui/skillcolor/skill_color_button_default.tga")
						self.skillColorButton[skillSlot].SetOverVisual("d:/ymir work/ui/skillcolor/skill_color_button_over.tga")
						self.skillColorButton[skillSlot].SetDownVisual("d:/ymir work/ui/skillcolor/skill_color_button_down.tga")
						self.skillColorButton[skillSlot].SetPosition(xPos, yPos)
						self.skillColorButton[skillSlot].SetEvent(lambda arg = skillSlot, arg2 = skillIndex: self.__OnPressSkillColorButton(arg, arg2))
						if player.GetSkillGrade(skillSlot + 1) >= skillMaxGrade and self.__IsColorableSkill(skillIndex):
							self.skillColorButton[skillSlot].Show()
						else:
							self.skillColorButton[skillSlot].Hide()
					else:
						self.skillColorButton[skillSlot].SetPosition(xPos, yPos)

		def __UpdateSkillColorPosition(self):
			x,y = self.GetGlobalPosition()
			self.skillColorWnd.SetPosition(x+250,y)

		def __OnPressSkillColorButton(self, skillSlot, skillIndex):
			self.skillColorWnd = uiskillcolor.SkillColorWindow(skillSlot, skillIndex)
			if self.skillColorWnd and not self.skillColorWnd.IsShow():
				self.skillColorWnd.Show()
				self.skillColorWnd.UpdateUseAvailable()
				
			if self.chDetailsWnd and self.chDetailsWnd.IsShow():
				self.chDetailsWnd.Hide()
				self.__InitCharacterDetailsUIButton()

	def RefreshItemSlot(self):
		if app.ENABLE_SKILL_COLOR_SYSTEM:
			if self.skillColorWnd and self.skillColorWnd.IsShow():
				self.skillColorWnd.UpdateUseAvailable()

	def RefreshSkillPlusButtonList(self):

		if self.isLoaded==0:
			return

		self.RefreshSkillPlusPointLabel()

		if not self.__CanUseSkillNow():
			return

		try:
			if self.PAGE_HORSE == self.curSelectedSkillGroup:
				self.__RefreshSkillPlusButton("HORSE")
			else:
				self.__RefreshSkillPlusButton("ACTIVE")

			self.__RefreshSkillPlusButton("SUPPORT")

		except:
			import exception
			exception.Abort("CharacterWindow.RefreshSkillPlusButtonList.BindObject")

	def RefreshSkillPlusPointLabel(self):
		if self.isLoaded==0:
			return

		if self.PAGE_HORSE == self.curSelectedSkillGroup:
			activeStatPoint = player.GetStatus(player.SKILL_HORSE)
			self.activeSkillPointValue.SetText(uiscriptlocale.UICHAR_STATUS_VALUE % (activeStatPoint))

		else:
			activeStatPoint = player.GetStatus(player.SKILL_ACTIVE)
			self.activeSkillPointValue.SetText(uiscriptlocale.UICHAR_STATUS_VALUE % (activeStatPoint))

		supportStatPoint = max(0, player.GetStatus(player.SKILL_SUPPORT))
		self.supportSkillPointValue.SetText(uiscriptlocale.UICHAR_STATUS_VALUE % (supportStatPoint))

	## Skill Level Up Button
	def OnPressedSlotButton(self, slotNumber):
		srcSlotIndex = self.__RealSkillSlotToSourceSlot(slotNumber)

		skillIndex = player.GetSkillIndex(srcSlotIndex)
		curLevel = player.GetSkillLevel(srcSlotIndex)
		maxLevel = skill.GetSkillMaxLevel(skillIndex)

		net.SendChatPacket("/skillup " + str(skillIndex))

	## Use Skill
	def ClickSkillSlot(self, slotIndex):
		if app.ENABLE_NEW_PASSIVE_SKILLS and self.skillGroupButton3.IsDown() and slotIndex < 100:
			return

		srcSlotIndex = self.__RealSkillSlotToSourceSlot(slotIndex)
		skillIndex = player.GetSkillIndex(srcSlotIndex)
		skillType = skill.GetSkillType(skillIndex)
		if not self.__CanUseSkillNow():
			if skill.SKILL_TYPE_ACTIVE == skillType:
				return

		for slotWindow in self.skillPageDict.values():
			if slotWindow.HasSlot(slotIndex):
				if skill.CanUseSkill(skillIndex):
					player.ClickSkillSlot(srcSlotIndex)
					return

		mousemodule.mouseController.DeattachObject()

	## FIXME : ��ų�� ��������� ���� ��ȣ�� ������ �ش� ������ ã�Ƽ� ������Ʈ �Ѵ�.
	##         �ſ� ���ո�. ���� ��ü�� �����ؾ� �ҵ�.
	def OnUseSkill(self, slotIndex, coolTime):

		skillIndex = player.GetSkillIndex(slotIndex)
		skillType = skill.GetSkillType(skillIndex)

		## ACTIVE
		if skill.SKILL_TYPE_ACTIVE == skillType:
			skillGrade = player.GetSkillGrade(slotIndex)
			slotIndex = self.__GetRealSkillSlot(skillGrade, slotIndex)
		## ETC
		else:
			slotIndex = self.__GetETCSkillRealSlotIndex(slotIndex)

		for slotWindow in self.skillPageDict.values():
			if slotWindow.HasSlot(slotIndex):
				slotWindow.SetSlotCoolTime(slotIndex, coolTime)
				return

	def OnActivateSkill(self, slotIndex):

		skillGrade = player.GetSkillGrade(slotIndex)
		slotIndex = self.__GetRealSkillSlot(skillGrade, slotIndex)

		for slotWindow in self.skillPageDict.values():
			if slotWindow.HasSlot(slotIndex):
				slotWindow.ActivateSlot(slotIndex)
				return

	def OnDeactivateSkill(self, slotIndex):

		skillGrade = player.GetSkillGrade(slotIndex)
		slotIndex = self.__GetRealSkillSlot(skillGrade, slotIndex)

		for slotWindow in self.skillPageDict.values():
			if slotWindow.HasSlot(slotIndex):
				slotWindow.DeactivateSlot(slotIndex)
				return

	def __ShowJobToolTip(self):
		self.toolTipJob.ShowToolTip()

	if app.SKILL_COOLTIME_UPDATE:
		def SkillClearCoolTime(self, slotIndex):
			slotIndex = self.__GetRealSkillSlot(player.GetSkillGrade(slotIndex), slotIndex)
			for slotWindow in self.skillPageDict.values():
				if slotWindow.HasSlot(slotIndex):
					slotWindow.SetSlotCoolTime(slotIndex, 0)

	def __HideJobToolTip(self):
		self.toolTipJob.HideToolTip()

	def __SetJobText(self, mainJob, subJob):
		if player.GetStatus(player.LEVEL)<5:
			subJob=0

		if 949 == app.GetDefaultCodePage():
			self.toolTipJob.ClearToolTip()

			try:
				jobInfoTitle=localeinfo.JOBINFO_TITLE[mainJob][subJob]
				jobInfoData=localeinfo.JOBINFO_DATA_LIST[mainJob][subJob]
			except IndexError:
				print "uicharacter.CharacterWindow.__SetJobText(mainJob=%d, subJob=%d)" % (mainJob, subJob)
				return

			self.toolTipJob.AutoAppendTextLine(jobInfoTitle)
			self.toolTipJob.AppendSpace(5)

			for jobInfoDataLine in jobInfoData:
				self.toolTipJob.AutoAppendTextLine(jobInfoDataLine)

			self.toolTipJob.AlignHorizonalCenter()

	def __ShowAlignmentToolTip(self):
		if self.toolTipAlignment == None:
			self.Close()
		if self.toolTipAlignment != None :
			self.toolTipAlignment.ShowToolTip()

	def __HideAlignmentToolTip(self):
		if self.toolTipAlignment == None:
			self.Close()
		if self.toolTipAlignment != None :
			self.toolTipAlignment.HideToolTip()

## CHARACTER WINDOW RENEWAL COMENTADO
	def __ShowHTHToolTip(self):
		self.toolTipConquerorInfoButton.ClearToolTip()
		self.toolTipConquerorInfoButton.AutoAppendTextLine(localeinfo.STAT_TOOLTIP_IMG_CON)
		self.toolTipConquerorInfoButton.AlignHorizonalCenter()
		
		self.toolTipConquerorInfoButton.ShowToolTip()

	def __HideHTHToolTip(self):
		self.toolTipConquerorInfoButton.HideToolTip()

	def __ShowINTToolTip(self):
		self.toolTipConquerorInfoButton.ClearToolTip()
		self.toolTipConquerorInfoButton.AutoAppendTextLine(localeinfo.STAT_TOOLTIP_IMG_INT)
		self.toolTipConquerorInfoButton.AlignHorizonalCenter()
		
		self.toolTipConquerorInfoButton.ShowToolTip()

	def __HideINTToolTip(self):
		self.toolTipConquerorInfoButton.HideToolTip()

	def __ShowSTRToolTip(self):
		self.toolTipConquerorInfoButton.ClearToolTip()
		self.toolTipConquerorInfoButton.AutoAppendTextLine(localeinfo.STAT_TOOLTIP_IMG_STR)
		self.toolTipConquerorInfoButton.AlignHorizonalCenter()
		
		self.toolTipConquerorInfoButton.ShowToolTip()

	def __HideSTRToolTip(self):
		self.toolTipConquerorInfoButton.HideToolTip()
		
	def __ShowDEXToolTip(self):
		self.toolTipConquerorInfoButton.ClearToolTip()
		self.toolTipConquerorInfoButton.AutoAppendTextLine(localeinfo.STAT_TOOLTIP_IMG_DEX)
		self.toolTipConquerorInfoButton.AlignHorizonalCenter()
		
		self.toolTipConquerorInfoButton.ShowToolTip()

	def __HideDEXToolTip(self):
		self.toolTipConquerorInfoButton.HideToolTip()

	###############################################################################
	def __ShowBaseToolTip(self):
		self.toolTipConquerorInfoButton.ClearToolTip()
		self.toolTipConquerorInfoButton.AutoAppendTextLine(localeinfo.STAT_TOOLTIP_BASE_LEVEL)
		self.toolTipConquerorInfoButton.AlignHorizonalCenter()
		
		self.toolTipConquerorInfoButton.ShowToolTip()

	def __HideBaseToolTip(self):
		self.toolTipConquerorInfoButton.HideToolTip()
	###
	def __ShowSungmaToolTip(self):
		self.toolTipConquerorInfoButton.ClearToolTip()
		self.toolTipConquerorInfoButton.AutoAppendTextLine(localeinfo.STAT_TOOLTIP_CONQUEROR_LEVEL)
		self.toolTipConquerorInfoButton.AlignHorizonalCenter()
		
		self.toolTipConquerorInfoButton.ShowToolTip()

	def __HideSungmaToolTip(self):
		self.toolTipConquerorInfoButton.HideToolTip()			
	
	###
	
	def __ShowMSPDToolTip(self):
		self.toolTipConquerorInfoButton.ClearToolTip()
		self.toolTipConquerorInfoButton.AutoAppendTextLine(localeinfo.STAT_TOOLTIP_MOVE_SPEED)
		self.toolTipConquerorInfoButton.AlignHorizonalCenter()
		
		self.toolTipConquerorInfoButton.ShowToolTip()

	def __HideMSPDToolTip(self):
		self.toolTipConquerorInfoButton.HideToolTip()
	####
	def __ShowASPDToolTip(self):
		self.toolTipConquerorInfoButton.ClearToolTip()
		self.toolTipConquerorInfoButton.AutoAppendTextLine(localeinfo.STAT_TOOLTIP_ATT_SPEED)
		self.toolTipConquerorInfoButton.AlignHorizonalCenter()
		
		self.toolTipConquerorInfoButton.ShowToolTip()

	def __HideASPDToolTip(self):
		self.toolTipConquerorInfoButton.HideToolTip()
	###	
	def __ShowCSPDToolTip(self):
		self.toolTipConquerorInfoButton.ClearToolTip()
		self.toolTipConquerorInfoButton.AutoAppendTextLine(localeinfo.STAT_TOOLTIP_CAST_SPEED)
		self.toolTipConquerorInfoButton.AlignHorizonalCenter()
		
		self.toolTipConquerorInfoButton.ShowToolTip()

	def __HideCSPDToolTip(self):
		self.toolTipConquerorInfoButton.HideToolTip()
		
	###	
	def __ShowMATTToolTip(self):
		self.toolTipConquerorInfoButton.ClearToolTip()
		self.toolTipConquerorInfoButton.AutoAppendTextLine(localeinfo.STAT_TOOLTIP_MAG_ATT)
		self.toolTipConquerorInfoButton.AlignHorizonalCenter()
		
		self.toolTipConquerorInfoButton.ShowToolTip()

	def __HideMATTToolTip(self):
		self.toolTipConquerorInfoButton.HideToolTip()
		
	###	
	def __ShowMDEFToolTip(self):
		self.toolTipConquerorInfoButton.ClearToolTip()
		self.toolTipConquerorInfoButton.AutoAppendTextLine(localeinfo.STAT_TOOLTIP_MAG_DEF)
		self.toolTipConquerorInfoButton.AlignHorizonalCenter()
		
		self.toolTipConquerorInfoButton.ShowToolTip()

	def __HideMDEFToolTip(self):
		self.toolTipConquerorInfoButton.HideToolTip()
	#### CHARACTER WINDOW RENEWAL COMENTADO#########################################

	def RefreshCharacter(self):

		if self.isLoaded==0:
			return

		## Name
		try:
			characterName = player.GetName()
			guildName = player.GetGuildName()
			self.characterNameValue.SetText(characterName)
			self.guildNameValue.SetText(guildName)
			if not guildName:
				if localeinfo.IsARABIC():
					self.characterNameSlot.SetPosition(190, 34)
				else:
					self.characterNameSlot.SetPosition(109, 34)

				self.guildNameSlot.Hide()
			else:
				if localeinfo.IsJAPAN():
					self.characterNameSlot.SetPosition(143, 34)
				else:
					self.characterNameSlot.SetPosition(153, 34)
				self.guildNameSlot.Show()
		except:
			import exception
			exception.Abort("CharacterWindow.RefreshCharacter.BindObject")

		race = net.GetMainActorRace()
		group = net.GetMainActorSkillGroup()
		empire = net.GetMainActorEmpire()

		## Job Text
		job = chr.RaceToJob(race)
		self.__SetJobText(job, group)

		## FaceImage
		try:
			faceImageName = FACE_IMAGE_DICT[race]

			try:
				self.faceImage.LoadImage(faceImageName)
			except:
				print "CharacterWindow.RefreshCharacter(race=%d, faceImageName=%s)" % (race, faceImageName)
				self.faceImage.Hide()

		except KeyError:
			self.faceImage.Hide()

		## GroupName
		self.__SetSkillGroupName(race, group)

		## Skill
		if 0 == group:
			self.__SelectSkillGroup(0)

		else:
			self.__SetSkillSlotData(race, group, empire)

			if self.__CanUseHorseSkill():
				self.__SelectSkillGroup(0)

	def __SetSkillGroupName(self, race, group):
		if app.ENABLE_NEW_PASSIVE_SKILLS:
			if net.GetMainActorSkillGroup() == 0:
				self.skillGroupButton3.SetPosition(95, 2)
			elif self.__CanUseHorseSkill():
				self.skillGroupButton3.SetPosition(50, 2)
				self.skillGroupButton2.SetPosition(95, 2)
		
		job = chr.RaceToJob(race)

		if not self.SKILL_GROUP_NAME_DICT.has_key(job):
			return

		nameList = self.SKILL_GROUP_NAME_DICT[job]

		if 0 == group:
			self.skillGroupButton1.SetText(nameList[1])
			self.skillGroupButton2.SetText(nameList[2])
			self.skillGroupButton1.Show()
			self.skillGroupButton2.Show()
			self.activeSkillGroupName.Hide()

			## �������� ������ �ϳ��̹Ƿ� �Ѱ��� ǥ��
			if app.ENABLE_WOLFMAN_CHARACTER and playersettingmodule.RACE_WOLFMAN_M == race:
				self.skillGroupButton2.Hide()
		else:

			if self.__CanUseHorseSkill():
				self.activeSkillGroupName.Hide()
				self.skillGroupButton1.SetText(nameList.get(group, "Noname"))
				self.skillGroupButton2.SetText(localeinfo.SKILL_GROUP_HORSE)
				self.skillGroupButton1.Show()
				self.skillGroupButton2.Show()

			else:
				self.activeSkillGroupName.SetText(nameList.get(group, "Noname"))
				self.activeSkillGroupName.Show()
				if app.ENABLE_NEW_PASSIVE_SKILLS:
					if not self.skillGroupButton3.IsDown() and not self.skillGroupButton2.IsDown():
						self.skillGroupButton1.Down()
					
					self.skillGroupButton1.Show()
					self.skillGroupButton1.SetText("")
				else:
					self.skillGroupButton1.Hide()
				
				self.skillGroupButton2.Hide()

	def __SetSkillSlotData(self, race, group, empire=0):
		## SkillIndex
		playersettingmodule.RegisterSkill(race, group, empire)

		## Event
		self.__SetSkillSlotEvent()

		## Refresh
		self.RefreshSkill()

	def __SelectSkillGroup(self, index):
		if app.ENABLE_NEW_PASSIVE_SKILLS:
			if index == 3:
				self.skillGroupButton3.Down()
				if app.ENABLE_SKILL_COLOR_SYSTEM:
					self.skillColorButton = []
				
				skillPage = self.skillPageDict["ACTIVE"]
				getSkillMaxLevel=skill.GetSkillMaxLevel
				
				for i in xrange(self.ACTIVE_PAGE_SLOT_COUNT+1):
					slotIndex = i + skillPage.GetStartIndex() + 220
					skillIndex = player.GetSkillIndex(slotIndex)
					for j in xrange(skill.SKILL_GRADE_COUNT):
						skillPage.ClearSlot(self.__GetRealSkillSlot(j, i))
					
					if skillIndex == 0:
						continue
					
					skillGrade = player.GetSkillGrade(slotIndex)
					skillLevel = player.GetSkillLevel(slotIndex)
					skillType = skill.GetSkillType(skillIndex)
					for j in xrange(skill.SKILL_GRADE_COUNT):
						realSlotIndex = self.__GetRealSkillSlot(j, slotIndex - 220)
						
						skillPage.SetSkillSlotNew(realSlotIndex, skillIndex, j, skillLevel)
						skillPage.SetCoverButton(realSlotIndex)
						
						if (skillGrade == skill.SKILL_GRADE_COUNT) and j == (skill.SKILL_GRADE_COUNT-1):
							skillPage.SetSlotCountNew(realSlotIndex, skillGrade, skillLevel)
						elif (not self.__CanUseSkillNow()) or (skillGrade != j):
							skillPage.SetSlotCount(realSlotIndex, 0)
							skillPage.DisableCoverButton(realSlotIndex)
						else:
							skillPage.SetSlotCountNew(realSlotIndex, skillGrade, skillLevel)
				
				for btn in self.skillGroupButton:
					btn.SetUp()
				
				return
			else:
				self.skillGroupButton3.SetUp()
		
		for btn in self.skillGroupButton:
			btn.SetUp()
		self.skillGroupButton[index].Down()

		if self.__CanUseHorseSkill():
			if 0 == index:
				index = net.GetMainActorSkillGroup()-1
			elif 1 == index:
				index = self.PAGE_HORSE
		elif app.ENABLE_NEW_PASSIVE_SKILLS and 0 == index and net.GetMainActorSkillGroup() != 0:
			index = net.GetMainActorSkillGroup()-1
		
		self.curSelectedSkillGroup = index
		self.__SetSkillSlotData(net.GetMainActorRace(), index+1, net.GetMainActorEmpire())

	def __CanUseSkillNow(self):
		if 0 == net.GetMainActorSkillGroup():
			return False

		return True

	def __CanUseHorseSkill(self):

		slotIndex = player.GetSkillSlotIndex(player.SKILL_INDEX_RIDING)

		if not slotIndex:
			return False

		grade = player.GetSkillGrade(slotIndex)
		level = player.GetSkillLevel(slotIndex)
		if level < 0:
			level *= -1
		if grade >= 1 and level >= 1:
			return True

		return False

	def __IsChangedHorseRidingSkillLevel(self):
		ret = False

		if -1 == self.canUseHorseSkill:
			self.canUseHorseSkill = self.__CanUseHorseSkill()

		if self.canUseHorseSkill != self.__CanUseHorseSkill():
			ret = True

		self.canUseHorseSkill = self.__CanUseHorseSkill()
		return ret

	def __GetRealSkillSlot(self, skillGrade, skillSlot):
		return skillSlot + min(skill.SKILL_GRADE_COUNT-1, skillGrade)*skill.SKILL_GRADE_STEP_COUNT

	def __GetETCSkillRealSlotIndex(self, skillSlot):
		if skillSlot > 100:
			return skillSlot
		return skillSlot % self.ACTIVE_PAGE_SLOT_COUNT

	def __RealSkillSlotToSourceSlot(self, realSkillSlot):
		if realSkillSlot > 100:
			return realSkillSlot
		if self.PAGE_HORSE == self.curSelectedSkillGroup:
			return realSkillSlot + self.ACTIVE_PAGE_SLOT_COUNT
		return realSkillSlot % skill.SKILL_GRADE_STEP_COUNT

	def __GetSkillGradeFromSlot(self, skillSlot):
		return int(skillSlot / skill.SKILL_GRADE_STEP_COUNT)

	def SelectSkillGroup(self, index):
		self.__SelectSkillGroup(index)

	def OnQuestScroll(self):
		if self.toolTip == None:
			self.Close()
		questCount = quest.GetQuestCount()
		scrollLineCount = max(0, questCount - quest.QUEST_MAX_NUM)
		startIndex = int(scrollLineCount * self.questScrollBar.GetPos())

		if startIndex != self.questShowingStartIndex:
			self.questShowingStartIndex = startIndex
			self.RefreshQuest()
			
	if app.ENABLE_QUEST_RENEWAL:
		def __OnScrollQuest(self):
			if self.state != "QUEST":
				return

			curPos = self.questScrollBar.GetPos()
			if math.fabs(curPos - self.questLastScrollPosition) >= 0.001:
				self.RerenderQuestPage()
				self.questLastScrollPosition = curPos

		def ResetQuestScroll(self):
			self.questScrollBar.Hide()

			if self.questScrollBar.GetPos() != 0:
				self.questScrollBar.SetPos(0)

		def RerenderQuestPage(self):
			overflowingY = self.displayY - self.MAX_QUEST_PAGE_HEIGHT
			if overflowingY < 0:
				overflowingY = 0

			self.baseCutY = math.ceil(overflowingY * self.questScrollBar.GetPos())
			self.displayY = 0
			self.RearrangeQuestCategories(xrange(quest.QUEST_CATEGORY_MAX_NUM))
			self.RefreshQuestCategory()

			if overflowingY > 0:
				if (len(self.questOpenedCategories)) == 0:
					self.ResetQuestScroll()
				else:
					self.questScrollBar.Show()
			else:
				self.ResetQuestScroll()

		def __LoadQuestCategory(self):
			self.questPage.Show()

			if self.isLoaded == 0:
				return

			for i in xrange(quest.QUEST_CATEGORY_MAX_NUM):
				category = self.questCategoryList[i]

				categoryName = category.GetProperty("name")
				if not categoryName:
					category.SetProperty("name", category.GetText())
					categoryName = category.GetText()

				questCount = self.GetQuestCountInCategory(i)
				self.questCategoryList[i].SetTextAlignLeft(categoryName + " (" + str(questCount) + ")")
				self.questCategoryList[i].SetTextColor(self.GetQuestCategoryColor(i))
				self.questCategoryList[i].SetQuestLabel(quest_lable_expend_img_path_dict[i], self.GetQuestCountInCategory(i))
				self.questCategoryList[i].Show()

			self.RefreshQuestCategory()
			if self.isQuestCategoryLoad == False:
				self.questScrollBar.Hide()
			else:
				self.RerenderQuestPage()

			self.isQuestCategoryLoad = True

		def GetQuestCategoryColor(self, category):
			return self.questColorList["default_title"]

		def GetQuestProperties(self, questName):
			findString = {
				"*" : "blue",
				"&" : "green",
				"~" : "golden"
			}

			if questName[0] in findString:
				return (questName[1:], findString[questName[0]])

			return (questName, None)

		def IsQuestCategoryOpen(self, category):
			return (category in self.questOpenedCategories)

		def ToggleCategory(self, category):
			if self.IsQuestCategoryOpen(category):
				self.CloseQuestCategory(category)
			else:
				self.OpenQuestCategory(category)

		def RearrangeQuestCategories(self, categoryRange):
			i = 0
			for i in categoryRange:
				if (self.displayY - self.baseCutY) >= 0 and (self.displayY - self.baseCutY) < self.MAX_QUEST_PAGE_HEIGHT - 20:
					self.questCategoryList[i].SetPosition(13, (self.displayY - self.baseCutY) + 10)
					self.questCategoryList[i].Show()
				else:
					self.questCategoryList[i].Hide()

				self.displayY += 20
				self.questCategoryRenderPos[i] = self.displayY

		def CloseQuestCategory(self, category):
			self.questCategoryList[category].CloseCategory(self.GetQuestCountInCategory(category))

			if category in self.questOpenedCategories:
				self.questOpenedCategories.remove(category)

			for currentSlot in self.questSlotList:
				if currentSlot.GetProperty("category") == category:
					currentSlot.Hide()
					self.displayY -= currentSlot.GetHeight()

			self.RerenderQuestPage()

		def OpenQuestCategory(self, category):
			if self.GetQuestCountInCategory(category) == 0:
				return

			while len(self.questOpenedCategories) >= self.questMaxOpenedCategories:
				openedCategories = self.questOpenedCategories.pop()
				self.CloseQuestCategory(openedCategories)

			self.questCategoryList[category].OpenCategory(self.GetQuestCountInCategory(category))
			self.questOpenedCategories.append(category)
			self.RefreshQuestCategory(category)
			self.RerenderQuestPage()

		def RefreshQuestCategory(self, category = -1):
			if self.isLoaded == 0 or self.state != "QUEST":
				return

			categories = []
			if category == -1:
				categories = self.questOpenedCategories
			elif not category in self.questOpenedCategories:
				self.OpenQuestCategory(category)
				return
			else:
				categories.append(category)

			for currentCategory in categories:
				self.displayY = self.questCategoryRenderPos[currentCategory]

				self.LoadCategory(currentCategory)
				self.RearrangeQuestCategories(xrange(currentCategory + 1, quest.QUEST_CATEGORY_MAX_NUM))

		def RefreshQuestCategoriesCount(self):
			for category in xrange(quest.QUEST_CATEGORY_MAX_NUM):
				categoryName = self.questCategoryList[category].GetProperty("name")
				questCount = self.GetQuestCountInCategory(category)
				self.questCategoryList[category].SetTextAlignLeft(categoryName + " (" + str(questCount) + ")")

		def RefreshQuest(self):
			if self.isLoaded == 0 or self.state != "QUEST":
				return

			for category in self.questOpenedCategories:
				self.RefreshQuestCategory(category)

			self.RefreshQuestCategoriesCount()

		def CreateQuestSlot(self, name):
			for questSlot in self.questSlotList:
				if questSlot.GetWindowName() == name:
					return questSlot

			pyScrLoader = ui.PythonScriptLoader()
			slot = ui.ListBar()
			pyScrLoader.LoadElementListBar(slot, quest_slot_listbar, self.questPage)

			slot.SetParent(self.quest_page_board_window)
			slot.SetWindowName(name)
			slot.Hide()
			self.questSlotList.append(slot)
			return slot

		def SetQuest(self, slot, questID, questName, questCounterName, questCounterValue):
			(name, color) = self.GetQuestProperties(questName)
			slot.SetTextAlignLeft(name, 20)
			if color:
				slot.SetTextColor(self.questColorList[color])
			slot.SetEvent(ui.__mem_func__(self.__SelectQuest), questID)
			slot.SetWindowHorizontalAlignLeft()
			slot.Show()

		def LoadCategory(self, category):
			self.questIndexMap = {}
			self.questCounterList = []
			self.questClockList = []
			self.questSeparatorList = []

			for questSlot in self.questSlotList:
				questSlot.Hide()

			questCount = 0
			for questIdx in self.GetQuestsInCategory(category):
				questCount += 1
				(questID, questIndex, questName, questCategory, _, questCounterName, questCounterValue) = questIdx
				(lastName, lastTime) = quest.GetQuestLastTime(questID)

				slot = self.CreateQuestSlot("QuestSlotList_" + str(questCategory) + "_" + str(questID))

				slot.SetPosition(0, (self.displayY - self.baseCutY))
				slot.SetParent(self.quest_page_board_window)
				baseDisplayY = self.displayY

				## -- Quest Counter
				hasCounter = False
				if questCounterName != "":
					self.displayY += 15

					counter = ui.TextLine()
					counter.SetParent(slot)
					counter.SetPosition(20, 20 - 2.5)
					counter.SetText(questCounterName + ": " + str(questCounterValue))
					counter.Show()

					self.questCounterList.append(counter)
					hasCounter = True
				## -- Quest Counter

				## -- Quest Clock
				self.displayY += 15

				clockText = localeinfo.QUEST_UNLIMITED_TIME
				if len(lastName) > 0:
					if lastTime <= 0:
						clockText = localeinfo.QUEST_TIMEOVER
					else:
						questLastMinute = lastTime / 60
						questLastSecond = lastTime % 60

						clockText = lastName + " : "

						if questLastMinute > 0:
							clockText += str(questLastMinute) + localeinfo.QUEST_MIN
							if questLastSecond > 0:
								clockText += " "

						if questLastSecond > 0:
							clockText += str(questLastSecond) + localeinfo.QUEST_SEC

				clock = ui.TextLine()
				clock.SetParent(slot)
				clock.SetPosition(20, 20 + (int(hasCounter) * 14) - 2.5)
				clock.SetText(clockText)
				clock.SetProperty("idx", questID)
				self.questClockList.append(clock)
				clock.Show()
				## -- Quest Clock

				## -- Quest Separator
				self.displayY += 5
				if questCount < self.GetQuestCountInCategory(category):
					seperator = ui.ImageBox()
					seperator.SetParent(slot)
					seperator.SetPosition(4, 20 + (int(hasCounter) * 14 - 2.5) + 15)
					seperator.LoadImage("d:/ymir work/ui/quest_re/quest_list_line_01.tga")
					seperator.Show()
					self.questSeparatorList.append(seperator)
				## -- Quest Separator

				slot.SetProperty("category", questCategory)

				if questIndex in self.questClicked:
					slot.OnClickEvent()

				if (baseDisplayY - self.baseCutY) + 2 >= 0 and (baseDisplayY - self.baseCutY) + 2 < self.MAX_QUEST_PAGE_HEIGHT - 30:
					self.questIndexMap[questID] = questIndex
					self.SetQuest(slot, questID, questName, questCounterName, questCounterValue)

				self.displayY += 15

			newList = []
			for questSlot in self.questSlotList:
				if questSlot.IsShow():
					newList.append(questSlot)

			self.questSlotList = newList

		def __OnClickQuestCategoryButton(self, category):
			self.ToggleCategory(category)

		def GetQuestsInCategory(self, category, retCount = False):
			questList = []
			count = 0
			for i in xrange(quest.GetQuestCount()):
				(questIndex, questName, questCategory, questIcon, questCounterName, questCounterValue) = quest.GetQuestData(i)
				if questCategory == category:
					count += 1
					questList.append((i, questIndex, questName, questCategory, questIcon, questCounterName, questCounterValue))

			if retCount:
				return count

			return questList

		def GetQuestCountInCategory(self, category):
			return self.GetQuestsInCategory(category, True)
			

	
	
			
