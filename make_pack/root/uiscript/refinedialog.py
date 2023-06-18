import uiscriptlocale
import localeinfo

BOARD_X = 500
BOARD_Y = 250

COLOR_LINE = 0xff5b5e5e
COLOR_PERCENTAGE = 0xffaaf698
LARGE_VALUE_FILE = "d:/ymir work/ui/public/Parameter_Slot_05.sub"

window = {
	"name" : "RefineDialog",
	"style" : ("movable", "float",),

	"x" : SCREEN_WIDTH - 400,
	"y" : 70 * 800 / SCREEN_HEIGHT,

	"width" : 0,
	"height" : 0,

	"children" :
	(
		{
			"name" : "Board",
			"type" : "board",
			"style" : ("attach",),

			"x" : 0,
			"y" : 0,

			"width" : 0,
			"height" : 0,

			"children" :
			(
				{
					"name" : "TitleBar",
					"type" : "titlebar",
					"style" : ("attach",),

					"x" : 8,
					"y" : 8,

					"width" : 0,
					"color" : "red",

					"children" :
					(
						{
							"name" : "TitleName",
							"type" : "text",
							"text" : uiscriptlocale.REFINE_TTILE,
							"horizontal_align" : "center",
							"text_horizontal_align" : "center",
							"x" : 0,
							"y" : 3,
						},
					),
				},
				
				{
					"name" : "Background",
					"type" : "bar",
					"x" : 7,
					"y" : 32,
					"width" : BOARD_X-20,
					"height" : BOARD_Y-40,
					"color" : 0x77000000,
				},
				
				{
					"name" : "AttachNextItem",
					"type" : "image",
					"x" : -115,
					"y" : -15,
					"horizontal_align" : "center",
					"vertical_align" : "center",
					"image" : "d:/ymir work/ui/game/windows/attach_metin_arrow.sub",
				},
				
				{
					"name": "ContentDesign",
					"type":"horizontalbar",
					"x": 12,
					"y": 35,
					"width": BOARD_X-20,
					"children" :
					(
						{
							"name": "curItem",
							"type":  "text",
							"x": 5,
							"y": 2,
							"text": localeinfo.REFINE_CURRENT_ITEM,
						},
						{
							"name": "nextItem",
							"type":  "text",
							"x": 160,
							"y": 2,
							"text": localeinfo.REFINE_NEXT_ITEM,
						},
						
						{
							"name": "needItems",
							"type":  "text",
							"x": BOARD_X-205,
							"y": 2,
							"text": localeinfo.REFINE_NEED_ITEMS,
						},

						{
							"name" : "LINE_LEFT",
							"type" : "line",
							"x" : 0,
							"y" : 20,
							"width" : 0,
							"height" : BOARD_Y-67,
							"color" : COLOR_LINE,
						},

						{
							"name" : "LINE_RIGHT",
							"type" : "line",
							"x" : BOARD_X-26,
							"y" : 20,
							"width" : 0,
							"height" : BOARD_Y-67,
							"color" : COLOR_LINE,
						},

						{
							"name" : "LINE_DOWN",
							"type" : "line",
							"x" : 0,
							"y" : BOARD_Y-48,
							"width" : BOARD_X-26,
							"height" : 0,
							"color" : COLOR_LINE,
						},

						{
							"name" : "LINE_UP",
							"type" : "line",
							"x" : 0,
							"y" : 20,
							"width" : BOARD_X-26,
							"height" : 0,
							"color" : COLOR_LINE,
						},
					),
				},

				{
					"name" : "LINE_LEFT",
					"type" : "line",
					"x" : 8,
					"y" : 30,
					"width" : 0,
					"height" : BOARD_Y-40,
					"color" : COLOR_LINE,
				},

				{
					"name" : "LINE_RIGHT",
					"type" : "line",
					"x" : BOARD_X-10,
					"y" : 30,
					"width" : 0,
					"height" : BOARD_Y-40,
					"color" : COLOR_LINE,
				},

				{
					"name" : "LINE_UP",
					"type" : "line",
					"x" : 8,
					"y" : 30,
					"width" : BOARD_X-17,
					"height" : 0,
					"color" : COLOR_LINE,
				},

				{
					"name" : "LINE_DOWN",
					"type" : "line",
					"x" : 8,
					"y" : BOARD_Y-10,
					"width" : BOARD_X-17,
					"height" : 0,
					"color" : COLOR_LINE,
				},	
				
				{
					"name" : "SEPARATOR_ITEM_RIGHT",
					"type" : "line",
					"x" : 240,
					"y" : 55,
					"width" : 0,
					"height" : BOARD_X/2-140,
					"color" : COLOR_LINE,
				},
				
				{
					"name" : "SEPARATOR_ITEM_UPGRADE_LEFT",
					"type" : "line",
					"x" : 249,
					"y" : 55,
					"width" : 0,
					"height" : BOARD_X/2-67,
					"color" : COLOR_LINE,
				},
				
				{
					"name" : "SEPARATOR_ITEM_DOWN",
					"type" : "line",
					"x" : 12,
					"y" : BOARD_Y-85,
					"width" : BOARD_X/2-21,
					"height" : 0,
					"color" : COLOR_LINE,
				},	
				
				
				{
					"name": "DesignIncrease",
					"type":"horizontalbar",
					"x": 14,
					"y": 172,
					"width": 235,
					"children" :
					(
						{
							"name": "SuccessPercentage",
							"type":  "text",
							"x": 5,
							"y": 2,
							"text": "",
						},
						
						{
							"name": "SuccessPercentageIncreased",
							"type":  "text",
							"x": 165,
							"y": 2,
							"color" : 0xffe2ff75,
							"text": "",
						},
						
						{
							"name" : "low",
							"type" : "text",
							"x" : 38,
							"y" : 30,
							"color" : COLOR_PERCENTAGE,
							"fontname": "Tahoma:16",
							"text" : localeinfo.REFINE_PERCENTAGE_LOW,
						},
						{
							"name" : "medium",
							"type" : "text",
							"x" : 38+80,
							"y" : 30,
							"color" : COLOR_PERCENTAGE,
							"fontname": "Tahoma:16",
							"text" : localeinfo.REFINE_PERCENTAGE_MEDIUM,
						},	
						{
							"name" : "extra",
							"type" : "text",
							"x" : 38+80+80,
							"y" : 30,
							"color" : COLOR_PERCENTAGE,
							"fontname": "Tahoma:16",
							"text" : localeinfo.REFINE_PERCENTAGE_EXTRA,
						},
					),
				},
				
				{
					"name" : "Slot",
					"type" : "slot",
					"x" : 0,
					"y" : 185,
					"width" : 200,
					"height" : 100,
					"slot" : (
						{
							"index": 0,
							"x": 20,
							"y": 12,
							"width": 32,
							"height": 32
						},
						{
							"index": 1,
							"x": 20 + 80,
							"y": 12,
							"width": 32,
							"height": 32
						},
						{
							"index": 2,
							"x": 20 + 80 + 80,
							"y": 12,
							"width": 32,
							"height": 32
						},
					),
				},
				
				{
					"name": "DesignCost",
					"type":"horizontalbar",
					"x": 250,
					"y": 200,
					"width": 235,
					"children" :
					(
						{
							"name": "textLine",
							"type": "text",
							"x": 5,
							"y": 2,
							"text": localeinfo.REFINE_COST_UPGRADE,
						},
					),
				},
				
				{
					"name" : "SlotCost",
					"type" : "button",
					"x" : 170,
					"y" : 32,
					"horizontal_align":"center",
					"vertical_align":"bottom",
					"default_image" : LARGE_VALUE_FILE,
					"over_image" : LARGE_VALUE_FILE,
					"down_image" : LARGE_VALUE_FILE,
					"children" :
					(
						{
							"name":"Money_Icon",
							"type": "image",
							"x": -18,
							"y": 2,
							"image": "d:/ymir work/ui/game/windows/money_icon.sub",
						},
						{
							"name" : "Cost",
							"type" : "text",
							"x" : 3,
							"y" : 3,
							"horizontal_align" : "right",
							"text_horizontal_align" : "right",
							"text" : "",
						},
					),
				},

				{
					"name" : "AcceptButton",
					"type" : "button",
					"x" : 90,
					"y" : 142,
					"text" : uiscriptlocale.OK,
					"default_image" : "d:/ymir work/ui/public/Large_Button_01.sub",
					"over_image" : "d:/ymir work/ui/public/Large_Button_02.sub",
					"down_image" : "d:/ymir work/ui/public/Large_Button_03.sub",
				},
				{
					"name" : "CancelButton",
					"type" : "button",

					"x" : 90,
					"y" : 59,

					"text" : uiscriptlocale.CANCEL,
					"default_image" : "d:/ymir work/ui/public/Large_Button_01.sub",
					"over_image" : "d:/ymir work/ui/public/Large_Button_02.sub",
					"down_image" : "d:/ymir work/ui/public/Large_Button_03.sub",
				},
			),
		},
	),
}