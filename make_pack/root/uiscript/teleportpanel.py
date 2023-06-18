import uiscriptlocale
import localeinfo
import constinfo

TELEPORT_PANEL_IMG_PATH = "d:/ymir work/ui/public/teleportpanel/"

GOLD_COLOR	= 0xFFFEE3AE

BOARD_WIDTH = 680
BOARD_HEIGT = 500

window = {
	"name" : "TeleportPanel",

	"x" : (SCREEN_WIDTH / 2) - (BOARD_WIDTH / 2),
	"y" : (SCREEN_HEIGHT / 2) - (BOARD_HEIGT / 2) ,

	"style" : ("movable", "float",),

	"width" : BOARD_WIDTH,
	"height" : BOARD_HEIGT,

	"children" :
	(
		{
			"name" : "board",
			"type" : "board_with_titlebar",

			"x" : 0,
			"y" : 0,

			"width" : BOARD_WIDTH,
			"height" : BOARD_HEIGT,
			"title" : uiscriptlocale.TELEPORT_PANEL_TITLE,
			"children" :
			(
				{
					"name" : "MapArea",
					"type" : "border_a",

					"x" : 15,
					"y" : 35 + 29,

					"width" : 450,
					"height" : BOARD_HEIGT - 50 - 29,
					"children" :
					(
						{
							"name" : "ScrollBarBG",
							"type" : "bar",
							"x" : 16, "y" : 5,
							"color" : 0xFF0C0C0C,
							"width" : 11, "height" : BOARD_HEIGT - 50 - 29 - 7 - 5,
							"horizontal_align" : "right",
						},
						{
							"name" : "ScrollBar",
							"type" : "scrollbar_flat",
							"x" : 16, "y" : 5,
							"size" : BOARD_HEIGT - 50 - 29 - 7 - 5,
							"horizontal_align" : "right",
						},
					),
				},
				
				## Tab Area
				
				{
					"name" : "Tab_01",
					"type" : "image",

					"x" : 15,
					"y" : 35,

					"width" : 450,
					"height" : 32,

					"image" : TELEPORT_PANEL_IMG_PATH+"tab_1.sub",
				},
				{
					"name" : "Tab_02",
					"type" : "image",

					"x" : 15,
					"y" : 35,

					"width" : 450,
					"height" : 32,

					"image" : TELEPORT_PANEL_IMG_PATH+"tab_2.sub",
				},
				{
					"name" : "Tab_03",
					"type" : "image",

					"x" : 15,
					"y" : 35,

					"width" : 450,
					"height" : 32,

					"image" : TELEPORT_PANEL_IMG_PATH+"tab_3.sub",
				},
				## RadioButton
				{
					"name" : "Tab_Button_01",
					"type" : "radio_button",

					"x" : 20,
					"y" : 39,

					"width" : 144,
					"height" : 21,
				},
				{
					"name" : "Tab_Button_02",
					"type" : "radio_button",

					"x" : 170,
					"y" : 39,

					"width" : 143,
					"height" : 21,
				},
				{
					"name" : "Tab_Button_03",
					"type" : "radio_button",

					"x" : 319,
					"y" : 39,

					"width" : 143,
					"height" : 21,
				},
				
				{ "name" : "Tab_Button_Text_01", "type":"text", "x": 15 + 76, "y": 48, "text" : uiscriptlocale.TELEPORT_PANEL_TAB_EMPIRES, "text_horizontal_align":"center", "text_vertical_align":"center", "outline" : 1, "fontsize":"LARGE", },
				{ "name" : "Tab_Button_Text_02", "type":"text", "x": 15 + 226, "y": 48, "text" : uiscriptlocale.TELEPORT_PANEL_TAB_NEUTRAL, "text_horizontal_align":"center", "text_vertical_align":"center", "outline" : 1, "fontsize":"LARGE", },
				{ "name" : "Tab_Button_Text_03", "type":"text", "x": 15 + 375, "y": 48, "text" : uiscriptlocale.TELEPORT_PANEL_TAB_DUNGEONS, "text_horizontal_align":"center", "text_vertical_align":"center", "outline" : 1, "fontsize":"LARGE", },
				
				## Info Area
				
				{
					"name" : "InfoArea",
					"type" : "border_a",

					"x" : 15 + 450 + 10,
					"y" : 35,

					"width" : 190,
					"height" : BOARD_HEIGT - 50,
					"children" :
					(
						{
							"name" : "Titlebar1", "type" : "image",
							"x" : 3, "y" : 3,
							"image" : TELEPORT_PANEL_IMG_PATH+"titlebar.sub",
							"children" :
							(
								{ "name" : "Titlebar1_Text", "type":"text", "x": 0, "y": -1, "text" : uiscriptlocale.TELEPORT_PANEL_INFO_TITLE, "color" : GOLD_COLOR, "all_align":"center", "outline" : 1},
							),
						},
						{
							"name" : "slider_picture_1", "type" : "image",
							"x" : 3, "y" : 24,
							"image" : TELEPORT_PANEL_IMG_PATH+"slider/picture_1.tga",
						},
						{
							"name" : "slider_picture_2", "type" : "image",
							"x" : 3, "y" : 24,
							"image" : TELEPORT_PANEL_IMG_PATH+"slider/picture_2.tga",
						},
						{
							"name" : "info_layer", "type" : "image",
							"x" : 3, "y" : 24,
							"image" : TELEPORT_PANEL_IMG_PATH+"info_layer.tga",
							"children" :
							(
								{ "name" : "text_info_boss", "type":"text", "x": 0, "y": 155, "text" : uiscriptlocale.TELEPORT_PANEL_INFO_BOSS, "color" : GOLD_COLOR, "horizontal_align":"center", "text_horizontal_align" : "center", "text_vertical_align" : "center", "outline" : 1},
								{ "name" : "text_info_minlv", "type":"text", "x": 0, "y": 170, "text" : uiscriptlocale.TELEPORT_PANEL_INFO_MINLV, "color" : GOLD_COLOR, "horizontal_align":"center", "text_horizontal_align" : "center", "text_vertical_align" : "center", "outline" : 1},
								{ "name" : "text_info_maxlv", "type":"text", "x": 0, "y": 185, "text" : uiscriptlocale.TELEPORT_PANEL_INFO_MAXLV, "color" : GOLD_COLOR, "horizontal_align":"center", "text_horizontal_align" : "center", "text_vertical_align" : "center", "outline" : 1},
								{ "name" : "text_info_cost", "type":"text", "x": 0, "y": 200, "text" : uiscriptlocale.TELEPORT_PANEL_INFO_COST, "color" : GOLD_COLOR, "horizontal_align":"center", "text_horizontal_align" : "center", "text_vertical_align" : "center", "outline" : 1},
								{ "name" : "text_info_required_item_desc", "type":"text", "x": 0, "y": 216, "text" : uiscriptlocale.TELEPORT_PANEL_INFO_ITEM_COST_DESC, "color" : GOLD_COLOR, "horizontal_align":"center", "text_horizontal_align" : "center", "text_vertical_align" : "center", "outline" : 1},
								{ "name" : "text_info_required_item_text", "type":"text", "x": 0, "y": 233, "text" : uiscriptlocale.TELEPORT_PANEL_INFO_ITEM_COST_TEXT, "horizontal_align":"center", "text_horizontal_align" : "center", "text_vertical_align" : "center", "outline" : 1},
								{
									"name" : "item_slot", "type" : "image", "x" : 75, "y" : 244,
									"image" : TELEPORT_PANEL_IMG_PATH+"item_slot.tga",
									"children" :
									(
										{
											"name" : "item_icon", "type" : "image", "x" : 3, "y" : 3,
										},
									),
								},
							),
						},
						{
							"name" : "Titlebar2", "type" : "image",
							"x" : 3, "y" : 309,
							"image" : TELEPORT_PANEL_IMG_PATH+"titlebar.sub",
							"children" :
							(
								{ "name" : "Titlebar2_Text", "type":"text", "x": 0, "y": -1, "text" : "", "color" : GOLD_COLOR, "all_align":"center", "outline" : 1},
							),
						},
					),
				},
			),
		},
	),
}