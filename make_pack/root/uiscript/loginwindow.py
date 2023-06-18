import app
import localeinfo

IMG_PATH = "grafica/login/"
SELECT_IMG_PATH = "grafica/select2/"

SCALE_X = float(SCREEN_WIDTH) / 1366.0
SCALE_Y = float(SCREEN_HEIGHT) / 768.0

ID_LIMIT_COUNT = 19
PW_LIMIT_COUNT = 16

MIDDLE_WIDTH = SCREEN_WIDTH / 2
MIDLLE_HEIGHT = SCREEN_HEIGHT / 2

window = {
	"name" : "LoginWindow",
	"sytle" : ("movable",),
	"x" : 0,
	"y" : 0,
	"width" : SCREEN_WIDTH,
	"height" : SCREEN_HEIGHT,
	"children" :
	(
		{
			"name" : "background",
			"type" : "expanded_image",
			"x" : 0,
			"y" : 0,
			"x_scale" : SCALE_X,
			"y_scale" : SCALE_Y,
			#"image" : "ui/azrael056.dds",
			#"image" : "d:/ymir work/ui/login_halloween.dds"
			"image" : "d:/ymir work/ui/login_christmas.dds"
		},
		{
			"name" : "bg_top",
			"type" : "bar",
			"x" : SCREEN_WIDTH - 440,
			"y" : (MIDLLE_HEIGHT) - 130/2 - 110 + 166,
			"width" : 410,
			"height" : 126,
			"color" : 0x00000000,
		},
		{
			"name" : "chstatus",
			"type" : "text",
			"x" : SCREEN_WIDTH - 210 - (209 / 2) - (167 / 2) + 16 + 145,
			"y" : (MIDLLE_HEIGHT) - 130/2 - 110 + 166 + 4 + 121 + 8,
			"text" : "##########",
		},
		#{
		#	"name" : "logo",
		#	"type" : "image",
		#	"x" : SCREEN_WIDTH - 210 - (419 / 2),
		#	"y" : (MIDLLE_HEIGHT) - 130/2 - 110,
		#	"image" : IMG_PATH + "center.png",
		#	"width" : 419,
		#	"height" : 156,
		#},
		{
			"name" : "bgsave_main",
			"type" : "image",
			"x" : 10,
			"y" : (MIDLLE_HEIGHT) - (130 / 2),
			"image" : IMG_PATH + "savelogin_lines.png",
			"children" :
			(
				{
					"name" : "save_txt1",
					"type" : "text",
					"x" : 100,
					"y" : 5,
					"text" : "1. Spazio Libero",
					"color" : 0xFF544D43,
				},
				{
					"name" : "deleteacc_btn1",
					"type" : "button",
					"x" : 28,
					"y" : 5+1,
					"default_image" : SELECT_IMG_PATH + "exit_norm.tga",
					"over_image" : SELECT_IMG_PATH + "exit_hover.tga",
					"down_image" : SELECT_IMG_PATH + "exit_down.tga",
				},
				{
		
					"name" : "loadacc_btn1",
					"type" : "button",
					"x" : 1,
					"y" : 5,
					"default_image" : IMG_PATH + "load_norm.tga",
					"over_image" : IMG_PATH + "load_hover.tga",
					"down_image" : IMG_PATH + "load_down.tga",
				},
				{
					"name" : "save_txt2",
					"type" : "text",
					"x" : 100,
					"y" : 5+50,
					"text" : "2. Spazio Libero",
					"color" : 0xFF544D43,
				},
				{
					"name" : "deleteacc_btn2",
					"type" : "button",
					"x" : 28,
					"y" : 5+38,
					"default_image" : SELECT_IMG_PATH + "exit_norm.tga",
					"over_image" : SELECT_IMG_PATH + "exit_hover.tga",
					"down_image" : SELECT_IMG_PATH + "exit_down.tga",
				},
				{
		
					"name" : "loadacc_btn2",
					"type" : "button",
					"x" : 1,
					"y" : 5+38,
					"default_image" : IMG_PATH + "load_norm.tga",
					"over_image" : IMG_PATH + "load_hover.tga",
					"down_image" : IMG_PATH + "load_down.tga",
				},
				{
					"name" : "save_txt3",
					"type" : "text",
					"x" : 100,
					"y" : 5+50+48,
					"text" : "3. Spazio Libero",
					"color" : 0xFF544D43,
				},
				{
					"name" : "deleteacc_btn3",
					"type" : "button",
					"x" : 28,
					"y" : 5+38+38+3,
					"default_image" : SELECT_IMG_PATH + "exit_norm.tga",
					"over_image" : SELECT_IMG_PATH + "exit_hover.tga",
					"down_image" : SELECT_IMG_PATH + "exit_down.tga",
				},
				{
			
					"name" : "loadacc_btn3",
					"type" : "button",
					"x" : 1,
					"y" : 5+38+38+2,
					"default_image" : IMG_PATH + "load_norm.tga",
					"over_image" : IMG_PATH + "load_hover.tga",
					"down_image" : IMG_PATH + "load_down.tga",
				},
				{
					"name" : "save_txt4",
					"type" : "text",
					"x" : 100,
					"y" : 5+50+50+34,
					"text" : "4. Spazio Libero",
					"color" : 0xFF544D43,
				},
				{
					"name" : "deleteacc_btn4",
					"type" : "button",
					"x" : 28,
					"y" : 5+38+38+38+6,
					"default_image" : SELECT_IMG_PATH + "exit_norm.tga",
					"over_image" : SELECT_IMG_PATH + "exit_hover.tga",
					"down_image" : SELECT_IMG_PATH + "exit_down.tga",
				},
				{
					"name" : "loadacc_btn4",
					"type" : "button",
					"x" : 1,
					"y" : 5+38+38+38+6,
					"default_image" : IMG_PATH + "load_norm.tga",
					"over_image" : IMG_PATH + "load_hover.tga",
					"down_image" : IMG_PATH + "load_down.tga",
				},
				{
					"name" : "save_txt5",
					"type" : "text",
					"x" : 100,
					"y" : 5+50+50+50+25,
					"text" : "5. Spazio Libero",
					"color" : 0xFF544D43,
				},
				{
					"name" : "deleteacc_btn5",
					"type" : "button",
					"x" : 28,
					"y" : 5+38+38+38+38+6,
					"default_image" : SELECT_IMG_PATH + "exit_norm.tga",
					"over_image" : SELECT_IMG_PATH + "exit_hover.tga",
					"down_image" : SELECT_IMG_PATH + "exit_down.tga",
				},
				{
					"name" : "loadacc_btn5",
					"type" : "button",
					"x" : 1,
					"y" : 5+38+38+38+38+6,
					"default_image" : IMG_PATH + "load_norm.tga",
					"over_image" : IMG_PATH + "load_hover.tga",
					"down_image" : IMG_PATH + "load_down.tga",
				},
				{
					"name" : "save_txt6",
					"type" : "text",
					"x" : 100,
					"y" : 5+50+50+50+50+16,
					"text" : "6. Spazio Libero",
					"color" : 0xFF544D43,
				},
				{
					"name" : "deleteacc_btn6",
					"type" : "button",
					"x" : 28,
					"y" : 5+38+38+38+38+38+8,
					"default_image" : SELECT_IMG_PATH + "exit_norm.tga",
					"over_image" : SELECT_IMG_PATH + "exit_hover.tga",
					"down_image" : SELECT_IMG_PATH + "exit_down.tga",
				},
				{
		
					"name" : "loadacc_btn6",
					"type" : "button",
					"x" : 1,
					"y" : 5+38+38+38+38+38+8,
					"default_image" : IMG_PATH + "load_norm.tga",
					"over_image" : IMG_PATH + "load_hover.tga",
					"down_image" : IMG_PATH + "load_down.tga",
				},
			)
		},
		{
			"name" : "bg_main",
			"type" : "image",
			"x" : SCREEN_WIDTH - 210 - (209 / 2),
			"y" : (MIDLLE_HEIGHT) - 130/2 - 110 + 166 + 4,
			"image" : IMG_PATH + "center2.png",
			"children" :
			(
				{
					"name" : "ID_EditLine",
					"type" : "editline",
					"x" : 20,
					"y" : 25,
					"width" : 260,
					"height" : 20,
					"input_limit" : ID_LIMIT_COUNT,
					"enable_codepage" : 0,
					"color" : 0xFFF4A74E,
				},
				{
					"name" : "Password_EditLine",
					"type" : "editline",
					"x" : 20,
					"y" : 86,
					"width" : 260,
					"height" : 20,
					"input_limit" : PW_LIMIT_COUNT,
					"secret_flag" : 1,
					"enable_codepage" : 0,
					"color" : 0xFFF4A74E,
				},
			),
		},
		{
			"name" : "exit_button",
			"type" : "button",
			"x" : SCREEN_WIDTH - 210 - (209 / 2) - (167 / 2) + 16 + 177,
			"y" : (MIDLLE_HEIGHT) - 130/2 - 110 + 166 + 4 + 121 + 24,
			"text" : localeinfo.UI_END,

			"default_image" : IMG_PATH + "login_norm.tga",
			"over_image" : IMG_PATH + "login_hover.tga",
			"down_image" : IMG_PATH + "login_down.tga",
		},
		{
			"name" : "LoginButton",
			"type" : "button",
			"x" : SCREEN_WIDTH - 210 - (209 / 2) - (167 / 2) + 16,
			"y" : (MIDLLE_HEIGHT) - 130/2 - 110 + 166 + 4 + 121 + 24,
			"text" : localeinfo.UI_GETIN,
			"default_image" : IMG_PATH + "login_norm.tga",
			"over_image" : IMG_PATH + "login_hover.tga",
			"down_image" : IMG_PATH + "login_down.tga",
		},
		{
			"name" : "LanguageBoard",
			"type" : "box",
			"x" : SCREEN_WIDTH - 210 - (209 / 2) + 34 - 34 - 30 - 66,
			"y" : (MIDLLE_HEIGHT) - 130/2 - 110 + 166 + 4 + 121 + 24 + 50,
			"width" : 404,
			"height" : 60,
			"color" : 0x00000000,
			"children" :
			(
				{
					"name" : "btn_lang_en", 
					"type" : "radio_button",
		
					"x" : 20 - 10,
					"y" : 20,
		
					"default_image" : "locale/general/ui/login/flags/en_default.tga",
					"over_image" : "locale/general/ui/login/flags/en_over.tga",
					"down_image" : "locale/general/ui/login/flags/en_down.tga",
				},
				{
					"name" : "btn_lang_ro", 
					"type" : "radio_button",
		
					"x" : 60 - 10,
					"y" : 20,
		
					"default_image" : "locale/general/ui/login/flags/ro_default.tga",
					"over_image" : "locale/general/ui/login/flags/ro_over.tga",
					"down_image" : "locale/general/ui/login/flags/ro_down.tga",
				},
				{
					"name" : "btn_lang_it", 
					"type" : "radio_button",
		
					"x" : 100 - 10,
					"y" : 20,
		
					"default_image" : "locale/general/ui/login/flags/it_default.tga",
					"over_image" : "locale/general/ui/login/flags/it_over.tga",
					"down_image" : "locale/general/ui/login/flags/it_down.tga",
				},
				{
					"name" : "btn_lang_tr", 
					"type" : "radio_button",
		
					"x" : 140 - 10,
					"y" : 20,
		
					"default_image" : "locale/general/ui/login/flags/tr_default.tga",
					"over_image" : "locale/general/ui/login/flags/tr_over.tga",
					"down_image" : "locale/general/ui/login/flags/tr_down.tga",
				},
				{
					"name" : "btn_lang_de", 
					"type" : "radio_button",
		
					"x" : 180 - 10,
					"y" : 20,
		
					"default_image" : "locale/general/ui/login/flags/de_default.tga",
					"over_image" : "locale/general/ui/login/flags/de_over.tga",
					"down_image" : "locale/general/ui/login/flags/de_down.tga",
				},
				{
					"name" : "btn_lang_pl", 
					"type" : "radio_button",
		
					"x" : 220 - 10,
					"y" : 20,
		
					"default_image" : "locale/general/ui/login/flags/pl_default.tga",
					"over_image" : "locale/general/ui/login/flags/pl_over.tga",
					"down_image" : "locale/general/ui/login/flags/pl_down.tga",
				},
				{
					"name" : "btn_lang_pt", 
					"type" : "radio_button",
		
					"x" : 260 - 10,
					"y" : 20,
		
					"default_image" : "locale/general/ui/login/flags/pt_default.tga",
					"over_image" : "locale/general/ui/login/flags/pt_over.tga",
					"down_image" : "locale/general/ui/login/flags/pt_down.tga",
				},
				{
					"name" : "btn_lang_es", 
					"type" : "radio_button",
		
					"x" : 300 - 10,
					"y" : 20,
		
					"default_image" : "locale/general/ui/login/flags/es_default.tga",
					"over_image" : "locale/general/ui/login/flags/es_over.tga",
					"down_image" : "locale/general/ui/login/flags/es_down.tga",
				},
				{
					"name" : "btn_lang_cz", 
					"type" : "radio_button",
		
					"x" : 340 - 10,
					"y" : 20,
		
					"default_image" : "locale/general/ui/login/flags/cz_default.tga",
					"over_image" : "locale/general/ui/login/flags/cz_over.tga",
					"down_image" : "locale/general/ui/login/flags/cz_down.tga",
				},
				{
					"name" : "btn_lang_hu", 
					"type" : "radio_button",
		
					"x" : 380 - 10,
					"y" : 20,
		
					"default_image" : "locale/general/ui/login/flags/hu_default.tga",
					"over_image" : "locale/general/ui/login/flags/hu_over.tga",
					"down_image" : "locale/general/ui/login/flags/hu_down.tga",
				},
			),
		},
		{
		
			"name" : "facebook_btn",
			"type" : "button",
			"x" : SCREEN_WIDTH - 74,
			"y" : 0,
			"default_image" : IMG_PATH + "facebook.tga",
			"over_image" : IMG_PATH + "facebook1.tga",
			"down_image" : IMG_PATH + "facebook2.tga",
		
		},
		{
			
			"name" : "instagram_btn",
			"type" : "button",
			"x" : SCREEN_WIDTH - 74 - 66,
			"y" : 0,
			"default_image" : IMG_PATH + "instagram.tga",
			"over_image" : IMG_PATH + "instagram1.tga",
			"down_image" : IMG_PATH + "instagram2.tga",
		
		},	
		{
		
			"name" : "discord_btn",
			"type" : "button",
			"x" : SCREEN_WIDTH - 74 - (66 * 2),
			"y" : 0,
			"default_image" : IMG_PATH + "discord.tga",
			"over_image" : IMG_PATH + "discord1.tga",
			"down_image" : IMG_PATH + "discord2.tga",
		
		},
		{
		
			"name" : "website_btn",
			"type" : "button",
			"x" : SCREEN_WIDTH - 74 - (66 * 3),
			"y" : 0,
			"default_image" : IMG_PATH + "elitepvpers.tga",
			"over_image" : IMG_PATH + "elitepvpers1.tga",
			"down_image" : IMG_PATH + "elitepvpers2.tga",
		
		},	
		{
		
			"name" : "tiktok_btn",
			"type" : "button",
			"x" : SCREEN_WIDTH - 74 - (66 * 4),
			"y" : 0,
			"default_image" : IMG_PATH + "tiktok.tga",
			"over_image" : IMG_PATH + "tiktok1.tga",
			"down_image" : IMG_PATH + "tiktok2.tga",
		
		},
		{
		
			"name" : "youtube_btn",
			"type" : "button",
			"x" : SCREEN_WIDTH - 74 - (66 * 5),
			"y" : 0,
			"default_image" : IMG_PATH + "youtube.tga",
			"over_image" : IMG_PATH + "youtube1.tga",
			"down_image" : IMG_PATH + "youtube2.tga",
			
		},
	),
}

