import uiscriptlocale
import item, player
import app

COSTUME_START_INDEX = item.COSTUME_SLOT_START

import player
EQUIPMENT_START_INDEX = player.EQUIPMENT_SLOT_START

BOARD_WIDTH = 176
BOARD_HEIGHT = 565-2

window = {
	"name" : "InventoryWindow",

	## 600 - (width + 오른쪽으로 부터 띄우기 24 px)
	"x" : SCREEN_WIDTH - BOARD_WIDTH,
	"y" : SCREEN_HEIGHT - BOARD_HEIGHT - 59,
	#"style" : ("movable", "float","not_pick",),
	"style" : ("movable", "float",),
	"width" : BOARD_WIDTH,
	"height" : BOARD_HEIGHT,
	"children" :
	(
		## Inventory, Equipment Slots
		{
			"name" : "board",
			"type" : "board",
			#"style" : ("attach",),

			"x" : 0,
			"y" : 0,

			"width" : BOARD_WIDTH,
			"height" : BOARD_HEIGHT,

			"children" :
			(
				## Title
				{
					"name" : "TitleBar",
					"type" : "titlebar",
					"style" : ("attach",),

					#"x" : 8+38,## 38 is the width of the new sort button -> Move the titlebard to the right
					"x" : 8,
					"y" : 7,

					#"width" : 161-38,## 38 is the width of the new sort button -> Decrease the width of the titlebar
					"width" : 161,
					"color" : "yellow",

					"children" :
					(
						{ "name":"TitleName", "type":"text", "x":0, "y":3, "text":uiscriptlocale.INVENTORY_TITLE, "text_horizontal_align":"center", "horizontal_align":"center" },
					),
				},
				
				## Separate
				#{
				#	"name" : "SeparateBaseImage",
				#	"type" : "image",
				#	"style" : ("attach",),
				#
				#	"x" : 8,
				#	"y" : 7,
				#
				#	"image" : "d:/ymir work/ui/pattern/titlebar_inv_refresh_baseframe.tga",
				#
				#	"children" :
				#	(
				#		# Separate Button (38x24) SORT_IVNENTORY
				#		{
				#			"name" : "SeparateButton",
				#			"type" : "button",
				#
				#			"x" : 11,
				#			"y" : 3,
				#
				#			"tooltip_text" : uiscriptlocale.INVENTORY_SEPARATE,
				#
				#			"default_image" : "d:/ymir work/ui/game/inventory/refresh_small_button_01.sub",
				#			"over_image" : "d:/ymir work/ui/game/inventory/refresh_small_button_02.sub",
				#			"down_image" : "d:/ymir work/ui/game/inventory/refresh_small_button_03.sub",
				#			"disable_image" : "d:/ymir work/ui/game/inventory/refresh_small_button_04.sub",
				#		},
				#	),
				#},

				## Equipment Slot
				{
					"name" : "Equipment_Base",
					"type" : "image",

					"x" : 10,
					"y" : 33,

					"image" : "d:/ymir work/ui/equipment_bg_with_talisman.tga",

					"children" :
					(

						{
							"name" : "EquipmentSlot",
							"type" : "slot",

							"x" : 3,
							"y" : 3,

							"width" : 150,
							"height" : 182,

							"slot" : (
										{"index":EQUIPMENT_START_INDEX+0, "x":39, "y":37, "width":32, "height":64},
										{"index":EQUIPMENT_START_INDEX+1, "x":39, "y":2, "width":32, "height":32},
										{"index":EQUIPMENT_START_INDEX+2, "x":39, "y":145, "width":32, "height":32},
										{"index":EQUIPMENT_START_INDEX+3, "x":75, "y":67, "width":32, "height":32},
										{"index":EQUIPMENT_START_INDEX+4, "x":3, "y":3, "width":32, "height":96},
										{"index":EQUIPMENT_START_INDEX+5, "x":114, "y":67, "width":32, "height":32},
										{"index":EQUIPMENT_START_INDEX+6, "x":114, "y":35, "width":32, "height":32},
										{"index":EQUIPMENT_START_INDEX+7, "x":2, "y":145, "width":32, "height":32},
										{"index":EQUIPMENT_START_INDEX+8, "x":75, "y":145, "width":32, "height":32},
										{"index":EQUIPMENT_START_INDEX+9, "x":114, "y":2, "width":32, "height":32},
										{"index":EQUIPMENT_START_INDEX+10, "x":75, "y":35, "width":32, "height":32},
										{"index":EQUIPMENT_START_INDEX+18, "x":75, "y":2, "width":32, "height":32},
										#Talisman Slot
										{"index":EQUIPMENT_START_INDEX+25, "x":3, "y":106, "width":32, "height":32},
										## 새 반지1
										##{"index":item.EQUIPMENT_RING1, "x":2, "y":106, "width":32, "height":32},
										## 새 반지2
										##{"index":item.EQUIPMENT_RING2, "x":75, "y":106, "width":32, "height":32},
										## 새 벨트
										{"index":item.EQUIPMENT_BELT, "x":39, "y":106, "width":32, "height":32},
									),
						},
						{
							"name" : "CostumeSlot",
							"type" : "slot",

							"x" : 3,
							"y" : 3,

							"width" : 150,
							"height" : 182,

							"slot" : (
										{"index":COSTUME_START_INDEX+0, "x":60, "y":56, "width":32, "height":64},
										{"index":COSTUME_START_INDEX+1, "x":60, "y": 10, "width":32, "height":32},
										{"index":COSTUME_START_INDEX+2, "x":107, "y":142, "width":32, "height":32},
										{"index":COSTUME_START_INDEX+3, "x":107, "y":10, "width":32, "height":32},
										{"index":item.COSTUME_SLOT_WEAPON, "x":12, "y":24, "width":32, "height":96},
										{"index":player.COSTUME_PETSKIN_SLOT, "x":107, "y":54, "width":32, "height":32},
										{"index":player.COSTUME_MOUNTSKIN_SLOT, "x":107, "y":98, "width":32, "height":32},
										{"index":player.COSTUME_EFFECT_BODY_SLOT, "x":60, "y":142, "width":32, "height":32},
										{"index":player.COSTUME_EFFECT_WEAPON_SLOT, "x":12, "y":142, "width":32, "height":32},
									),
						},
						{
							"name" : "BodyToolTipButton",
							"type" : "toggle_button",

							"x" : 94,
							"y" : 57,
							"tooltip_text" : uiscriptlocale.HIDE_COSTUME,
							"tooltip_x" : 0,
							"tooltip_y" : - 14,
							"default_image" : "d:/ymir work/ui/pattern/visible_mark_01.tga",
							"over_image" : "d:/ymir work/ui/pattern/visible_mark_02.tga",
							"down_image" : "d:/ymir work/ui/pattern/visible_mark_03.tga",
						},
						{
							"name" : "HairToolTipButton",
							"type" : "toggle_button",
							"x" : 90,
							"y" : 0,
							"tooltip_text" : uiscriptlocale.HIDE_COSTUME,
							"tooltip_x" : - 52,
							"tooltip_y" : - 1,
							"default_image" : "d:/ymir work/ui/pattern/visible_mark_01.tga",
							"over_image" : "d:/ymir work/ui/pattern/visible_mark_02.tga",
							"down_image" : "d:/ymir work/ui/pattern/visible_mark_03.tga",
						},
						{
							"name" : "AcceToolTipButton",
							"type" : "toggle_button",
							"x" : 138,
							"y" : 0,
							"tooltip_text" : uiscriptlocale.HIDE_COSTUME,
							"tooltip_x" : - 55,
							"tooltip_y" : - 10,
							"default_image" : "d:/ymir work/ui/pattern/visible_mark_01.tga",
							"over_image" : "d:/ymir work/ui/pattern/visible_mark_02.tga",
							"down_image" : "d:/ymir work/ui/pattern/visible_mark_03.tga",
						},
						{
							"name" : "WeaponToolTipButton",
							"type" : "toggle_button",
							"x" : 45,
							"y" : 24,
							"tooltip_text" : uiscriptlocale.HIDE_COSTUME,
							"tooltip_x" : - 8,
							"tooltip_y" : - 25,
							"default_image" : "d:/ymir work/ui/pattern/visible_mark_01.tga",
							"over_image" : "d:/ymir work/ui/pattern/visible_mark_02.tga",
							"down_image" : "d:/ymir work/ui/pattern/visible_mark_03.tga",
						},
						## if app.ENABLE_EXTRA_INVENTORY:
						{
							"name" : "ExtendIntentory",
							"type" : "button",

							"x" : 114,
							"y" : 148,

						#	"tooltip_text" : uiscriptlocale.EXTRA_INVENTORY,
						#	"tooltip_x" : - 33,
						#	"tooltip_y" : - 12,
						#	"default_image" : "d:/ymir work/ui/extra_inventory/extra_inv_01.tga",
						#	"over_image" : "d:/ymir work/ui/extra_inventory/extra_inv_02.tga",
						#	"down_image" : "d:/ymir work/ui/extra_inventory/extra_inv_03.tga",
						},
						## Dragon Soul Button
						{
							"name" : "DSSButton",
							"type" : "button",

							"x" : 117,
							"y" : 148,

							"tooltip_text" : uiscriptlocale.TASKBAR_DRAGON_SOUL,

							"default_image" : "d:/ymir work/ui/new_buttons/dss_inventory_button_01.tga",
							"over_image" : "d:/ymir work/ui/new_buttons/dss_inventory_button_02.tga",
							"down_image" : "d:/ymir work/ui/new_buttons/dss_inventory_button_03.tga",
						},
						## Rune
						{
							"name" : "Rune",
							"type" : "button",

							"x" : 133,
							"y" : 148,

							"tooltip_text" : uiscriptlocale.Rune,

							"default_image" : "d:/ymir work/ui/new_buttons/rune_button_01.tga",
							"over_image" : "d:/ymir work/ui/new_buttons/rune_button_02.tga",
							"down_image" : "d:/ymir work/ui/new_buttons/rune_button_03.tga",
						},
						## Premium_Button
						#{
						#	"name" : "Rune",
						#	"type" : "button",

						#	"x" : 117,
						#	"y" : 148+15,

						#	"tooltip_text" : uiscriptlocale.Rune,

						#	"default_image" : "d:/ymir work/ui/new_buttons/premium_button_01.tga",
						#	"over_image" : "d:/ymir work/ui/new_buttons/premium_button_02.tga",
						#	"down_image" : "d:/ymir work/ui/new_buttons/premium_button_03.tga",
						#},
						## Special_Button
						#{
						#	"name" : "Rune",
						#	"type" : "button",

						#	"x" : 133,
						#	"y" : 148+15,

						#	"tooltip_text" : uiscriptlocale.Rune,

						#	"default_image" : "d:/ymir work/ui/new_buttons/special_button_01.tga",
						#	"over_image" : "d:/ymir work/ui/new_buttons/special_button_02.tga",
						#	"down_image" : "d:/ymir work/ui/new_buttons/special_button_03.tga",
						#},
					),
				},
				{
					"name" : "Equipment_Tab_01",
					"type" : "radio_button",
					"x" : 11,
					"y" : 33 + 192,
					"default_image" : "d:/ymir work/ui/game/windows/bottone/prova.tga",
					"over_image" : "d:/ymir work/ui/game/windows/bottone/prova1.tga",
					"down_image" : "d:/ymir work/ui/game/windows/bottone/prova2.tga",
					"tooltip_text" : uiscriptlocale.EQUIPMENT_PAGE_BUTTON_TOOLTIP_1,
					"children" :
					(
						{
							"name" : "Equipment_Tab_01_Print",
							"type" : "text",
							"x" : 0,
							"y" : 0,
							"all_align" : "center",
							# "text" : "I",
						},
					),
				},
				{
					"name" : "Equipment_Tab_02",
					"type" : "radio_button",
					"x" : 88,
					"y" : 33 + 192,
					"default_image" : "d:/ymir work/ui/game/windows/bottone/prova3.tga",
					"over_image" : "d:/ymir work/ui/game/windows/bottone/prova4.tga",
					"down_image" : "d:/ymir work/ui/game/windows/bottone/prova5.tga",
					"tooltip_text" : uiscriptlocale.EQUIPMENT_PAGE_BUTTON_TOOLTIP_2,
					"children" :
					(
						{
							"name" : "Equipment_Tab_02_Print",
							"type" : "text",
							"x" : 0,
							"y" : 0,
							"all_align" : "center",
							# "text" : "II",
						},
					),
				},
				{
					"name" : "Inventory_Tab_01",
					"type" : "radio_button",

					"x" : 10,
					"y" : 53 + 191,

					"default_image" : "d:/ymir work/ui/game/windows/tab_button_large_half_01.sub",
					"over_image" : "d:/ymir work/ui/game/windows/tab_button_large_half_02.sub",
					"down_image" : "d:/ymir work/ui/game/windows/tab_button_large_half_03.sub",
					"tooltip_text" : uiscriptlocale.INVENTORY_PAGE_BUTTON_TOOLTIP_1,

					"children" :
					(
						{
							"name" : "Inventory_Tab_01_Print",
							"type" : "text",

							"x" : 0,
							"y" : 0,

							"all_align" : "center",

							"text" : "I",
						},
					),
				},
				{
					"name" : "Inventory_Tab_02",
					"type" : "radio_button",

					#"x" : 10 + 78,
					"x" : 10 + 39,
					"y" : 53 + 191,

					"default_image" : "d:/ymir work/ui/game/windows/tab_button_large_half_01.sub",
					"over_image" : "d:/ymir work/ui/game/windows/tab_button_large_half_02.sub",
					"down_image" : "d:/ymir work/ui/game/windows/tab_button_large_half_03.sub",
					"tooltip_text" : uiscriptlocale.INVENTORY_PAGE_BUTTON_TOOLTIP_2,

					"children" :
					(
						{
							"name" : "Inventory_Tab_02_Print",
							"type" : "text",

							"x" : 0,
							"y" : 0,

							"all_align" : "center",

							"text" : "II",
						},
					),
				},
				
				{
					"name" : "Inventory_Tab_03",
					"type" : "radio_button",

					"x" : 10 + 39 + 39,
					"y" : 53 + 191,

					"default_image" : "d:/ymir work/ui/game/windows/tab_button_large_half_01.sub",
					"over_image" : "d:/ymir work/ui/game/windows/tab_button_large_half_02.sub",
					"down_image" : "d:/ymir work/ui/game/windows/tab_button_large_half_03.sub",
					"tooltip_text" : uiscriptlocale.INVENTORY_PAGE_BUTTON_TOOLTIP_3,

					"children" :
					(
						{
							"name" : "Inventory_Tab_03_Print",
							"type" : "text",

							"x" : 0,
							"y" : 0,

							"all_align" : "center",

							"text" : "III",
						},
					),
				},
				
				{
					"name" : "Inventory_Tab_04",
					"type" : "radio_button",

					"x" : 10 + 39 + 39 + 39,
					"y" : 53 + 191,

					"default_image" : "d:/ymir work/ui/game/windows/tab_button_large_half_01.sub",
					"over_image" : "d:/ymir work/ui/game/windows/tab_button_large_half_02.sub",
					"down_image" : "d:/ymir work/ui/game/windows/tab_button_large_half_03.sub",
					"tooltip_text" : uiscriptlocale.INVENTORY_PAGE_BUTTON_TOOLTIP_4,

					"children" :
					(
						{
							"name" : "Inventory_Tab_04_Print",
							"type" : "text",

							"x" : 0,
							"y" : 0,

							"all_align" : "center",

							"text" : "IV",
						},
					),
				},

				## Item Slot
				{
					"name" : "ItemSlot",
					"type" : "grid_table",

					"x" : 8,
					"y" : 266,

					"start_index" : 0,
					"x_count" : 5,
					"y_count" : 9,
					"x_step" : 32,
					"y_step" : 32,

					"image" : "d:/ymir work/ui/public/Slot_Base.sub"
				},
				{
					"name":"cover_open_0",
					"type":"button",
					"vertical_align":"bottom",
					
					"x":8,
					"y":339 - 24 -18,

					"default_image":"d:/ymir work/ui/ex_inven_cover_button_open.sub",
					"over_image":"d:/ymir work/ui/ex_inven_cover_button_open.sub",
					"down_image":"d:/ymir work/ui/ex_inven_cover_button_open.sub",
				},
				{
					"name":"cover_close_0",
					"type":"image",
					"vertical_align":"bottom",
					
					"x":8,
					"y":339 - 24 -18,

					"image":"d:/ymir work/ui/ex_inven_cover_button_close.sub",
				},
				{
					"name":"cover_open_1",
					"type":"button",
					"vertical_align":"bottom",
					
					"x":8,
					"y":339 - 24 - 32 -18,

					"default_image":"d:/ymir work/ui/ex_inven_cover_button_open.sub",
					"over_image":"d:/ymir work/ui/ex_inven_cover_button_open.sub",
					"down_image":"d:/ymir work/ui/ex_inven_cover_button_open.sub",
				},
				{
					"name":"cover_close_1",
					"type":"image",
					"vertical_align":"bottom",
					
					"x":8,
					"y":339 - 24 - 32 -18,

					"image":"d:/ymir work/ui/ex_inven_cover_button_close.sub",
				},
				{
					"name":"cover_open_2",
					"type":"button",
					"vertical_align":"bottom",
					
					"x":8,
					"y":339 - 24 - 64 -18,
					"default_image":"d:/ymir work/ui/ex_inven_cover_button_open.sub",
					"over_image":"d:/ymir work/ui/ex_inven_cover_button_open.sub",
					"down_image":"d:/ymir work/ui/ex_inven_cover_button_open.sub",
				},
				{
					"name":"cover_close_2",
					"type":"image",
					"vertical_align":"bottom",
					
					"x":8,
					"y":339 - 24 - 64 -18,

					"image":"d:/ymir work/ui/ex_inven_cover_button_close.sub",
				},
				{
					"name":"cover_open_3",
					"type":"button",
					"vertical_align":"bottom",
					
					"x":8,
					"y":339 - 24 - 96 -18,

					"default_image":"d:/ymir work/ui/ex_inven_cover_button_open.sub",
					"over_image":"d:/ymir work/ui/ex_inven_cover_button_open.sub",
					"down_image":"d:/ymir work/ui/ex_inven_cover_button_open.sub",
				},
				{
					"name":"cover_close_3",
					"type":"image",
					"vertical_align":"bottom",
					
					"x":8,
					"y":339 - 24 - 96 -18,

					"image":"d:/ymir work/ui/ex_inven_cover_button_close.sub",
				},
				{
					"name":"cover_open_4",
					"type":"button",
					"vertical_align":"bottom",
					
					"x":8,
					"y":339 - 24 - 128 -18,

					"default_image":"d:/ymir work/ui/ex_inven_cover_button_open.sub",
					"over_image":"d:/ymir work/ui/ex_inven_cover_button_open.sub",
					"down_image":"d:/ymir work/ui/ex_inven_cover_button_open.sub",
				},
				{
					"name":"cover_close_4",
					"type":"image",
					"vertical_align":"bottom",
					
					"x":8,
					"y":339 - 24 - 128 -18,

					"image":"d:/ymir work/ui/ex_inven_cover_button_close.sub",
				},
				{
					"name":"cover_open_5",
					"type":"button",
					"vertical_align":"bottom",
					
					"x":8,
					"y":339 - 24 - 160 -18,

					"default_image":"d:/ymir work/ui/ex_inven_cover_button_open.sub",
					"over_image":"d:/ymir work/ui/ex_inven_cover_button_open.sub",
					"down_image":"d:/ymir work/ui/ex_inven_cover_button_open.sub",
				},
				{
					"name":"cover_close_5",
					"type":"image",
					"vertical_align":"bottom",
					
					"x":8,
					"y":339 - 24 - 160 -18,

					"image":"d:/ymir work/ui/ex_inven_cover_button_close.sub",
				},
				{
					"name":"cover_open_6",
					"type":"button",
					"vertical_align":"bottom",
					
					"x":8,
					"y":339 - 24 - 192 -18,

					"default_image":"d:/ymir work/ui/ex_inven_cover_button_open.sub",
					"over_image":"d:/ymir work/ui/ex_inven_cover_button_open.sub",
					"down_image":"d:/ymir work/ui/ex_inven_cover_button_open.sub",
				},
				{
					"name":"cover_close_6",
					"type":"image",
					"vertical_align":"bottom",
					
					"x":8,
					"y":339 - 24 - 192 -18,

					"image":"d:/ymir work/ui/ex_inven_cover_button_close.sub",
				},
				{
					"name":"cover_open_7",
					"type":"button",
					"vertical_align":"bottom",
					
					"x":8,
					"y":339 - 24 - 224 -18,

					"default_image":"d:/ymir work/ui/ex_inven_cover_button_open.sub",
					"over_image":"d:/ymir work/ui/ex_inven_cover_button_open.sub",
					"down_image":"d:/ymir work/ui/ex_inven_cover_button_open.sub",
				},
				{
					"name":"cover_close_7",
					"type":"image",
					"vertical_align":"bottom",
					
					"x":8,
					"y":339 - 24 - 224 -18,

					"image":"d:/ymir work/ui/ex_inven_cover_button_close.sub",
				},
				{
					"name":"cover_open_8",
					"type":"button",
					"vertical_align":"bottom",
					
					"x":8,
					"y":339 - 24 - 256 -18,

					"default_image":"d:/ymir work/ui/ex_inven_cover_button_open.sub",
					"over_image":"d:/ymir work/ui/ex_inven_cover_button_open.sub",
					"down_image":"d:/ymir work/ui/ex_inven_cover_button_open.sub",
				},
				{
					"name":"cover_close_8",
					"type":"image",
					"vertical_align":"bottom",
					
					"x":8,
					"y":339 - 24 - 256 -18,

					"image":"d:/ymir work/ui/ex_inven_cover_button_close.sub",
				},
			),
		},
	),
}
