import uiscriptlocale
import app

PATTERN_PATH			= "d:/ymir work/ui/pattern/"
ROOT_PATH				= "d:/ymir work/ui/public/"
CHATTING_PATH			= "d:/ymir work/ui/chat/"

WINDOW_WIDTH			= 254
WINDOW_HEIGHT			= 520- 60 - 48

PATTERN_WINDOW_WIDTH	= 232
PATTERN_WINDOW_HEIGHT	= 454- 60 - 48


PATTERN_X_COUNT = (PATTERN_WINDOW_WIDTH - 32) / 16
PATTERN_Y_COUNT = (PATTERN_WINDOW_HEIGHT - 32) / 16

window = {
	"name" : "ChatSettingWindow",
	"style" : ("movable", "float",),
	
	"x" : SCREEN_WIDTH / 2 - WINDOW_WIDTH / 2,
	"y" : SCREEN_HEIGHT / 2 - WINDOW_HEIGHT / 2,
	
	"width" : WINDOW_WIDTH,
	"height" : WINDOW_HEIGHT,
	
	"children" :
	(
		{
			"name" : "board",
			"type" : "board_with_titlebar",
			"style" : ("attach",),
			
			"x" : 0,
			"y" : 0,
			
			"width" : WINDOW_WIDTH,
			"height" : WINDOW_HEIGHT,
			
			"title" : uiscriptlocale.CHATTING_SETTING_TITLE,
			
			"children" :
			(
				## base pattern
				{
					"name" : "base_pattern",
					"type" : "window",
					"style" : ("attach", "ltr",),
					
					"x" : 10,
					"y" : 32,
					"width" : PATTERN_WINDOW_WIDTH,
					"height" :PATTERN_WINDOW_HEIGHT,
					
					"children" :
					(
						## LeftTop 1
						{
							"name" : "pattern_left_top_img",
							"type" : "image",
							"style" : ("ltr",),
							
							"x" : 0,
							"y" : 0,
							"image" : PATTERN_PATH + "border_A_left_top.tga",
						},
						## RightTop 2
						{
							"name" : "pattern_right_top_img",
							"type" : "image",
							"style" : ("ltr",),
							
							"x" : PATTERN_WINDOW_WIDTH - 16,
							"y" : 0,
							"image" : PATTERN_PATH + "border_A_right_top.tga",
						},
						## LeftBottom 3
						{
							"name" : "pattern_left_bottom_img",
							"type" : "image",
							"style" : ("ltr",),
							
							"x" : 0,
							"y" : PATTERN_WINDOW_HEIGHT - 16,
							"image" : PATTERN_PATH + "border_A_left_bottom.tga",
						},
						## RightBottom 4
						{
							"name" : "pattern_right_bottom_img",
							"type" : "image",
							"style" : ("ltr",),
							
							"x" : PATTERN_WINDOW_WIDTH - 16,
							"y" : PATTERN_WINDOW_HEIGHT - 16,
							"image" : PATTERN_PATH + "border_A_right_bottom.tga",
						},
						## topcenterImg 5
						{
							"name" : "pattern_top_cetner_img",
							"type" : "expanded_image",
							"style" : ("ltr",),
							
							"x" : 16,
							"y" : 0,
							"image" : PATTERN_PATH + "border_A_top.tga",
							"rect" : (0.0, 0.0, PATTERN_X_COUNT, 0),
						},
						## leftcenterImg 6
						{
							"name" : "pattern_left_center_img",
							"type" : "expanded_image",
							"style" : ("ltr",),
							
							"x" : 0,
							"y" : 16,
							"image" : PATTERN_PATH + "border_A_left.tga",
							"rect" : (0.0, 0.0, 0, PATTERN_Y_COUNT),
						},
						## rightcenterImg 7
						{
							"name" : "pattern_right_center_img",
							"type" : "expanded_image",
							"style" : ("ltr",),
							
							"x" : PATTERN_WINDOW_WIDTH - 16,
							"y" : 16,
							"image" : PATTERN_PATH + "border_A_right.tga",
							"rect" : (0.0, 0.0, 0, PATTERN_Y_COUNT),
						},
						## bottomcenterImg 8
						{
							"name" : "pattern_bottom_center_img",
							"type" : "expanded_image",
							"style" : ("ltr",),
							
							"x" : 16,
							"y" : PATTERN_WINDOW_HEIGHT - 16,
							"image" : PATTERN_PATH + "border_A_bottom.tga",
							"rect" : (0.0, 0.0, PATTERN_X_COUNT, 0),
						},
						## centerImg
						{
							"name" : "pattern_center_img",
							"type" : "expanded_image",
							"style" : ("ltr",),
							
							"x" : 16,
							"y" : 16,
							"image" : PATTERN_PATH + "border_A_center.tga",
							"rect" : (0.0, 0.0, PATTERN_X_COUNT, PATTERN_Y_COUNT),
						},
					),
				},

				## ä�� ����
				{
					"name" : "chatting_setting_menu_bg", "type" : "image", "x" : 15, "y" : 37,
					"image" : CHATTING_PATH + "chattingoption_menu_bg.sub",
					"children" :
					(
						{ "name" : "chatting_setting", "type" : "text", "x" : 0, "y" : 0, "text" : uiscriptlocale.CHATTING_SETTING_TITLE, "all_align":"center"	},
					),
				},
				## ��� ��Ӵٿ�
				{
					"name" : "chatting_setting_change_window", "type" : "window", "x" : 145, "y" : 64, "width" : 90, "height" : 18,			
					"children" :
					(
						{
							"name" : "language_select_img", "type" : "image", "x" : 0, "y" : 0,
							"image" : CHATTING_PATH + "chattingoption_dropdown_small_bg.sub",
						},
						{
							"name" : "cur_chatting_type_window", "type" : "window", "x" : 0, "y" : 0, "width" : 90-15, "height" : 18,
							"children" :
							(
								{"name":"cur_chatting_type_text", "type":"text", "x":0, "y":0, "text": uiscriptlocale.CHATTING_SETTING_DEFAULT_TITLE, "all_align" : "center"},
							),
						},
						{
							"name" : "chatting_type_select_button", "type" : "button", "x" : 218-145, "y" : 1,
							"default_image" : "d:/ymir work/ui/chat/chattingoption_dropdown_arrow_01.sub",
							"over_image"	: "d:/ymir work/ui/chat/chattingoption_dropdown_arrow_02.sub",
							"down_image"	: "d:/ymir work/ui/chat/chattingoption_dropdown_arrow_03.sub",
						},
						{
							"name" : "chatting_type_select_pivot_window", "type" : "window", "x" : 0, "y" : 18, "width" : 90, "height" : 0,
						},
					),
				},
				## ���
				{
					"name" : "chatting_setting_list_bg", "type" : "image", "x" : 18, "y" : 65,
					"image" : CHATTING_PATH + "chattingoption_sub_large_bg.sub",
					"children" :
					(
						{ "name" : "chatting_setting_list", "type" : "text", "x" : 0, "y" : 0, "text" : uiscriptlocale.CHATTING_SETTING_LIST, "all_align":"center"	},
					),
				},
				## �Ϲ�
				{
					"name" : "chatting_setting_talking_bg", "type" : "image", "x" : 18, "y" : 83,
					"image" : CHATTING_PATH + "chattingoption_sub_large_bg.sub",
					"children" :
					(
						{ "name" : "chatting_setting_talking", "type" : "text", "x" : 0, "y" : 0, "text" : uiscriptlocale.CHATTING_SETTING_TALKING, "all_align":"center"	},
					),
				},
				## ��Ƽ
				{
					"name" : "chatting_setting_party_bg", "type" : "image", "x" : 18, "y" : 101,
					"image" : CHATTING_PATH + "chattingoption_sub_large_bg.sub",
					"children" :
					(
						{ "name" : "chatting_setting_party", "type" : "text", "x" : 0, "y" : 0, "text" : uiscriptlocale.CHATTING_SETTING_PARTY, "all_align":"center"	},
					),
				},
				## ���
				{
					"name" : "chatting_setting_guild_bg", "type" : "image", "x" : 18, "y" : 119,
					"image" : CHATTING_PATH + "chattingoption_sub_large_bg.sub",
					"children" :
					(
						{ "name" : "chatting_setting_guild", "type" : "text", "x" : 0, "y" : 0, "text" : uiscriptlocale.CHATTING_SETTING_GUILD, "all_align":"center"	},
					),
				},
				## ��ħ
				{
					"name" : "chatting_setting_shout_bg", "type" : "image", "x" : 18, "y" : 137,
					"image" : CHATTING_PATH + "chattingoption_sub_large_bg.sub",
					"children" :
					(
						{ "name" : "chatting_setting_shout", "type" : "text", "x" : 0, "y" : 0, "text" : uiscriptlocale.CHATTING_SETTING_SHOUT, "all_align":"center"	},
					),
				},
				## �ý���
				{
					"name" : "chatting_setting_info_bg", "type" : "image", "x" : 18, "y" : 155,
					"image" : CHATTING_PATH + "chattingoption_sub_large_bg.sub",
					"children" :
					(
						{ "name" : "chatting_setting_info", "type" : "text", "x" : 0, "y" : 0, "text" : uiscriptlocale.CHATTING_SETTING_SYSTEM, "all_align":"center"	},
					),
				},
				## ����
				{
					"name" : "chatting_setting_notice_bg", "type" : "image", "x" : 18, "y" : 173,
					"image" : CHATTING_PATH + "chattingoption_sub_large_bg.sub",
					"children" :
					(
						{ "name" : "chatting_setting_notice", "type" : "text", "x" : 0, "y" : 0, "text" : uiscriptlocale.CHATTING_SETTING_NOTICE, "all_align":"center"	},
					),
				},
				## �ֻ��� ����
				{
					"name" : "dice_setting_menu_bg", "type" : "image", "x" : 15, "y" : 299- 60 - 48,
					"image" : "d:/ymir work/ui/chat/chattingoption_menu_bg.sub",
					"children" :
					(
						{ "name" : "dice_setting", "type" : "text", "x" : 0, "y" : 0, "text" : uiscriptlocale.CHATTING_SETTING_DICE_SETTING, "all_align":"center"	},
					),
				},
				## �ֻ���
				{
					"name" : "chatting_setting_dice_bg", "type" : "image", "x" : 18, "y" : 327- 60 - 48,
					"image" : CHATTING_PATH + "chattingoption_sub_large_bg.sub",
					"children" :
					(
						{ "name" : "chatting_setting_dice", "type" : "text", "x" : 0, "y" : 0, "text" : uiscriptlocale.CHATTING_SETTING_DICE, "all_align":"center"	},
					),
				},
				## �ý��� ���μ���
				{
					"name" : "system_setting_menu_bg", "type" : "image", "x" : 15, "y" : 348- 60 - 48,
					"image" : "d:/ymir work/ui/chat/chattingoption_menu_bg.sub",
					"children" :
					(
						{ "name" : "system_setting", "type" : "text", "x" : 0, "y" : 0, "text" : uiscriptlocale.CHATTING_SETTING_DETAIL, "all_align":"center"	},
					),
				},
				## ����ġ
				{
					"name" : "chatting_setting_exp_bg", "type" : "image", "x" : 18, "y" : 376- 60 - 48,
					"image" : CHATTING_PATH + "chattingoption_sub_large_bg.sub",
					"children" :
					(
						{ "name" : "chatting_setting_exp", "type" : "text", "x" : 0, "y" : 0, "text" : uiscriptlocale.CHATTING_SETTING_EXP, "all_align":"center"	},
					),
				},
				## ������
				{
					"name" : "chatting_setting_item_bg", "type" : "image", "x" : 18, "y" : 394- 60 - 48,
					"image" : CHATTING_PATH + "chattingoption_sub_large_bg.sub",
					"children" :
					(
						{ "name" : "chatting_setting_item", "type" : "text", "x" : 0, "y" : 0, "text" : uiscriptlocale.CHATTING_SETTING_ITEM, "all_align":"center"	},
					),
				},
				## ��ȭ
				{
					"name" : "chatting_setting_gold_bg", "type" : "image", "x" : 18, "y" : 412- 60 - 48,
					"image" : CHATTING_PATH + "chattingoption_sub_large_bg.sub",
					"children" :
					(
						{ "name" : "chatting_setting_gold", "type" : "text", "x" : 0, "y" : 0, "text" : uiscriptlocale.CHATTING_SETTING_GOLD, "all_align":"center"	},
					),
				},
				## ���̸� ���μ���
				{
					"name" : "tabname_setting_menu_bg", "type" : "image", "x" : 15, "y" : 433- 60 - 48,
					"image" : "d:/ymir work/ui/chat/chattingoption_menu_bg.sub",
					"children" :
					(
						{ "name" : "tabname_setting", "type" : "text", "x" : 0, "y" : 0, "text" : uiscriptlocale.CHATTING_SETTING_TABNAME, "all_align":"center"	},
					),
				},
				## EditLine
				{
					"name" : "tab_name_slot",
					"type" : "slotbar",
					"x" : 17,
					"y" : 461- 60 - 48,
					"width" : 123,
					"height" : 18,
					"children" :
					(
						{
							"name" : "tab_name_value",
							"type" : "editline",
							"x" : 2,
							"y" : 3,
							
							"width" : 160,
							
							"height" : 15,
							"input_limit" : 6,
							"check_width" : 1,
							"text" : "",
						},
					),
				},

				## tabname accpet Button
				{
					"name" : "tabname_accept_button",
					"type" : "button",

					"x" : 143,
					"y" : 460- 60 - 48,

					"default_image" : CHATTING_PATH + "chattingoption_enter_btn_01.sub",
					"over_image"	: CHATTING_PATH + "chattingoption_enter_btn_02.sub",
					"down_image"	: CHATTING_PATH + "chattingoption_enter_btn_03.sub",
				},

				## reset_button
				{
					"name" : "reset_button",
					"type" : "button",

					"x" : 10,
					"y" : 489- 60 - 48,

					"default_image"	: CHATTING_PATH + "chattingoption_bottom_button_01.sub",
					"over_image"	: CHATTING_PATH + "chattingoption_bottom_button_02.sub",
					"down_image"	: CHATTING_PATH + "chattingoption_bottom_button_03.sub",

					"text" : uiscriptlocale.CHATTING_SETTING_CLEAR,
				},
				## Save Button
				{
					"name" : "save_button",
					"type" : "button",

					"x" : 88,
					"y" : 489- 60 - 48,

					"default_image"	: CHATTING_PATH + "chattingoption_bottom_button_01.sub",
					"over_image"	: CHATTING_PATH + "chattingoption_bottom_button_02.sub",
					"down_image"	: CHATTING_PATH + "chattingoption_bottom_button_03.sub",

					"text" : uiscriptlocale.CHATTING_SETTING_SAVE,
				},
				## Cancle Button
				{
					"name" : "cancle_button",
					"type" : "button",

					"x" : 166,
					"y" : 489- 60 - 48,

					"default_image"	: CHATTING_PATH + "chattingoption_bottom_button_01.sub",
					"over_image"	: CHATTING_PATH + "chattingoption_bottom_button_02.sub",
					"down_image"	: CHATTING_PATH + "chattingoption_bottom_button_03.sub",

					"text" : uiscriptlocale.CHATTING_SETTING_CANCLE,
				},
			),
		},
	),
}