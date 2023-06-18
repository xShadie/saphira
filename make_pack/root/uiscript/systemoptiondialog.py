import uiscriptlocale
import app

ROOT_PATH = "d:/ymir work/ui/public/"

TEMPORARY_X = +13
TEXT_TEMPORARY_X = -10
BUTTON_TEMPORARY_X = 5
PVP_X = -10

window = {
	"name" : "SystemOptionDialog",
	"style" : ("movable", "float",),
	"x" : 0,
	"y" : 0,
	"width" : 311,
	"height" : 242,
	"children" :
	(
		{
			"name" : "board",
			"type" : "board",
			"x" : 0,
			"y" : 0,
			"width" : 311,
			"height" : 242,
			"children" :
			(
				{
					"name" : "titlebar",
					"type" : "titlebar",
					"style" : ("attach",),
					"x" : 8,
					"y" : 8,
					"width" : 290,
					"color" : "gray",
					"children" :
					(
						{ 
							"name":"titlename",
							"type":"text",
							"x":0,
							"y":3, 
							"horizontal_align":"center",
							"text_horizontal_align":"center",
							"text": uiscriptlocale.SYSTEMOPTION_TITLE, 
						 },
					),
				},
				{
					"name" : "music_name",
					"type" : "text",
					"x" : 30 - 15,
					"y" : 75,
					"text" : uiscriptlocale.OPTION_MUSIC,
				},
				{
					"name" : "music_volume_controller",
					"type" : "sliderbar",
					"x" : 110 - 15,
					"y" : 75,
				},
				{
					"name" : "bgm_button",
					"type" : "button",
					"x" : 26 - 15,
					"y" : 100,
					"text" : uiscriptlocale.OPTION_MUSIC_CHANGE,
					"default_image" : ROOT_PATH + "Middle_Button_01.sub",
					"over_image" : ROOT_PATH + "Middle_Button_02.sub",
					"down_image" : ROOT_PATH + "Middle_Button_03.sub",
				},
				{
					"name" : "bgm_file",
					"type" : "text",
					"x" : 100 - 15,
					"y" : 102,
					"text" : uiscriptlocale.OPTION_MUSIC_DEFAULT_THEMA,
				},
				{
					"name" : "sound_name",
					"type" : "text",
					"x" : 30 - 15,
					"y" : 50,
					"text" : uiscriptlocale.OPTION_SOUND,
				},
				{
					"name" : "sound_volume_controller",
					"type" : "sliderbar",
					"x" : 110 - 15,
					"y" : 50,
				},
				{
					"name" : "camera_mode",
					"type" : "text",
					"x" : 40 + TEXT_TEMPORARY_X - 15,
					"y" : 125 + 2,
					"text" : uiscriptlocale.OPTION_CAMERA_DISTANCE,
				},
				{
					"name" : "camera_short",
					"type" : "radio_button",
					"x" : 110 - 15,
					"y" : 125,
					"text" : uiscriptlocale.OPTION_CAMERA_DISTANCE_SHORT,
					"default_image" : ROOT_PATH + "Middle_Button_01.sub",
					"over_image" : ROOT_PATH + "Middle_Button_02.sub",
					"down_image" : ROOT_PATH + "Middle_Button_03.sub",
				},
				{
					"name" : "camera_long",
					"type" : "radio_button",
					"x" : 110+70 - 15,
					"y" : 125,
					"text" : uiscriptlocale.OPTION_CAMERA_DISTANCE_LONG,
					"default_image" : ROOT_PATH + "Middle_Button_01.sub",
					"over_image" : ROOT_PATH + "Middle_Button_02.sub",
					"down_image" : ROOT_PATH + "Middle_Button_03.sub",
				},
				{
					"name" : "camera_long_long",
					"type" : "radio_button",
					"x" : 180 + 70 - 15,
					"y" : 125,
					"text" : uiscriptlocale.OPTION_CAMERA_DISTANCE_LONG_LONG,
					"default_image" : ROOT_PATH + "Middle_Button_01.sub",
					"over_image" : ROOT_PATH + "Middle_Button_02.sub",
					"down_image" : ROOT_PATH + "Middle_Button_03.sub",
				},
				{
					"name" : "camera_perspective",
					"type" : "text",
					"x" : 30 - 15,
					"y" : 125 + 4 + (1 * 25),
					"text" : uiscriptlocale.CAMERA_PERSPECTIVE,
				},
				{
					"name" : "camera_perspective_controller",
					"type" : "sliderbar",
					"x" : 110 - 15,
					"y" : 125 + 4 + (1 * 25),
				},
				{
					"name" : "fog_mode",
					"type" : "text",
					"x" : 30 - 15,
					"y" : 129 + 2 + (2 * 25),
					"text" : uiscriptlocale.OPTION_FOG,
				},
				{
					"name" : "fog_level0",
					"type" : "radio_button",
					"x" : 110 - 15,
					"y" : 129 + (2 * 25),
					"text" : uiscriptlocale.OPTION_FOG_DENSE,
					"default_image" : ROOT_PATH + "small_Button_01.sub",
					"over_image" : ROOT_PATH + "small_Button_02.sub",
					"down_image" : ROOT_PATH + "small_Button_03.sub",
				},
				{
					"name" : "fog_level1",
					"type" : "radio_button",
					"x" : 110+50 - 15,
					"y" : 129 + (2 * 25),
					"text" : uiscriptlocale.OPTION_FOG_MIDDLE,
					"default_image" : ROOT_PATH + "small_Button_01.sub",
					"over_image" : ROOT_PATH + "small_Button_02.sub",
					"down_image" : ROOT_PATH + "small_Button_03.sub",
				},
				
				{
					"name" : "fog_level2",
					"type" : "radio_button",
					"x" : 110 + 100 - 15,
					"y" : 129 + (2 * 25),
					"text" : uiscriptlocale.OPTION_FOG_LIGHT,
					"default_image" : ROOT_PATH + "small_Button_01.sub",
					"over_image" : ROOT_PATH + "small_Button_02.sub",
					"down_image" : ROOT_PATH + "small_Button_03.sub",
				},
				{
					"name" : "tiling_mode",
					"type" : "text",
					"x" : 40 + TEXT_TEMPORARY_X - 15,
					"y" : 129 + 2 + (3 * 25),
					"text" : uiscriptlocale.OPTION_TILING,
				},
				{
					"name" : "tiling_cpu",
					"type" : "radio_button",
					"x" : 110 - 15,
					"y" : 129 + (3 * 25),
					"text" : uiscriptlocale.OPTION_TILING_CPU,
					"default_image" : ROOT_PATH + "small_Button_01.sub",
					"over_image" : ROOT_PATH + "small_Button_02.sub",
					"down_image" : ROOT_PATH + "small_Button_03.sub",
				},
				{
					"name" : "tiling_gpu",
					"type" : "radio_button",
					"x" : 110+50 - 15,
					"y" : 129 + (3 * 25),
					"text" : uiscriptlocale.OPTION_TILING_GPU,
					"default_image" : ROOT_PATH + "small_Button_01.sub",
					"over_image" : ROOT_PATH + "small_Button_02.sub",
					"down_image" : ROOT_PATH + "small_Button_03.sub",
				},
				{
					"name" : "tiling_apply",
					"type" : "button",
					"x" : 110+100 - 15,
					"y" : 129 + (3 * 25),
					"text" : uiscriptlocale.OPTION_TILING_APPLY,
					"default_image" : ROOT_PATH + "middle_Button_01.sub",
					"over_image" : ROOT_PATH + "middle_Button_02.sub",
					"down_image" : ROOT_PATH + "middle_Button_03.sub",
				},
			),
		},
	),
}
