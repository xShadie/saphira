import uiScriptLocale

window = {
	"name" : "OfflineShopWindow",

	"x" : 0,
	"y" : 0,

	"style" : ("float",),

	"width" : 345,
	"height" : 103,

	"children" :
	(
		{
			"name" : "board",
			"type" : "board",
			"style" : ("attach",),

			"x" : 0,
			"y" : 0,

			"width" : 345,
			"height" : 102,
			
			"children" :
			(
				## InfoBox
				{
					"name" : "InfoBox",
					"type" : "box",

					"x" : 10,
					"y" : 10,
					"width" : 186,
					"height" : 81,

					"color" : 0xFF6C6359,
				},
				## Box2
				{
					"name" : "Box2",
					"type" : "box",

					"x" : 202,
					"y" : 10,
					"width" : 131,
					"height" : 81,

					"color" : 0xFF6C6359,
					
					"children" : 
					(
						## LockButton
						{
							"name" : "LockButton",
							"type" : "button",

							"x" : 125,
							"y" : 6,

							"horizontal_align" : "right",
							
							"text" : uiScriptLocale.OFFLINE_SHOP_BUTTON_LOCK,

							"default_image" : "ui/button/button_default.tga",
							"over_image" : "ui/button/button_over.tga",
							"down_image" : "ui/button/button_over.tga",
						},
						## RenameButton
						{
							"name" : "RenameButton",
							"type" : "button",

							"x" : 125,
							"y" : 31,

							"horizontal_align" : "right",
							
							"text" : uiScriptLocale.OFFLINE_SHOP_BUTTON_RENAME,

							"default_image" : "ui/button/button_default.tga",
							"over_image" : "ui/button/button_over.tga",
							"down_image" : "ui/button/button_over.tga",
						},
						## CloseButton
						{
							"name" : "CloseButton",
							"type" : "button",

							"x" : 125,
							"y" : 56,

							"horizontal_align" : "right",
							
							"text" : uiScriptLocale.OFFLINE_SHOP_BUTTON_CLOSE,

							"default_image" : "ui/button/button_default.tga",
							"over_image" : "ui/button/button_over.tga",
							"down_image" : "ui/button/button_over.tga",
						},
					),
				},
				## ShopSign
				{
					"name" : "ShopSignSlot",
					"type" : "slotbar",

					"x" : 16,
					"y" : 17,

					"width" : 174,
					"height" : 17,

					"children" :
					(
						{
							"name" : "ShopSignText",
							"type" : "text",

							"x" : 3,
							"y" : 2,

							"horizontal_align" : "left",
							"text_horizontal_align" : "left",
							
							"text" : "",
						},
					),
				},
				## PositionInfoText
				{
					"name" : "PosInfoText",
					"type" : "text",
					
					"text" : "",
					
					"x" : 28,
					"y" : 44,
					
					"children" :
					(
						{
							"name" : "PointIcon",
							"type" : "image",

							"x" : -12,
							"y" : 1,

							"image" : "ui/point.tga",
						},
					),
				},
				## TimeLeftText
				{
					"name" : "TimeLeftText",
					"type" : "text",
					
					"text" : "",
					
					"x" : 28,
					"y" : 69,
					
					"children" :
					(
						{
							"name" : "HourglassImage",
							"type" : "image",

							"x" : -12,
							"y" : 1,

							"image" : "ui/hourglass.tga",
						},
					),
				},
			),
		},
	),
}
