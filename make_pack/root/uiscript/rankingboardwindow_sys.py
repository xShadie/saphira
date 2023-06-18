import uiscriptlocale

BACK_IMG_PATH = "d:/ymir work/ui/pattern/"

window = {
	"name" : "RankingWindow",
	"x" : 0,
	"y" : 0,
	"style" : ("movable","float",),
	"width" : 562,
	"height" : 424,
	"children" :
	(
		{
			"name" : "board",
			"type" : "board",
			"x" : 0,
			"y" : 0,
			"width" : 562,
			"height" : 424,
			"children" :
			(
				{
					"name" : "titlebar",
					"type" : "titlebar",
					"style" : ("attach",),
					"x" : 8,
					"y" : 7,
					"width" : 547,
					"color" : "yellow",
					"children" :
					(
						{
							"name" : "title_name",
							"type" : "text",
							"x" : 0,
							"y" : -1,
							"text" : uiscriptlocale.RANKING_TITLE,
							"all_align":"center"
						},
					),
				},
				{
					"name" : "explanation",
					"type" : "window",
					"x" : 14,
					"y" : 36,
					"width" : 169,
					"height" : 376,
					"children" :
					(
						{
							"name" : "left_top",
							"type" : "image",
							"x" : 0,
							"y" : 0,
							"image" : BACK_IMG_PATH+"border_A_left_top.tga",
						},
						{
							"name" : "right_top",
							"type" : "image",
							"x" : 153,
							"y" : 0,
							"image" : BACK_IMG_PATH+"border_A_right_top.tga",
						},
						{
							"name" : "left_bottom",
							"type" : "image",
							"x" : 0,
							"y" : 360,
							"image" : BACK_IMG_PATH+"border_A_left_bottom.tga",
						},
						{
							"name" : "right_bottom",
							"type" : "image",
							"x" : 153,
							"y" : 360,
							"image" : BACK_IMG_PATH+"border_A_right_bottom.tga",
						},
						{
							"name" : "left_center_img",
							"type" : "expanded_image",
							"x" : 0,
							"y" : 15,
							"image" : BACK_IMG_PATH+"border_A_left.tga",
							"rect" : (0.0, 0.0, 0, 21),
						},
						{
							"name" : "right_center_img",
							"type" : "expanded_image",
							"x" : 153,
							"y" : 15,
							"image" : BACK_IMG_PATH+"border_A_right.tga",
							"rect" : (0.0, 0.0, 0, 21),
						},
						{
							"name" : "top_center_img",
							"type" : "expanded_image",
							"x" : 15,
							"y" :  0,
							"image" : BACK_IMG_PATH+"border_A_top.tga",
							"rect" : (0.0, 0.0, 8, 0),
						},
						{
							"name" : "bottom_center_img",
							"type" : "expanded_image",
							"x" : 15,
							"y" : 360,
							"image" : BACK_IMG_PATH+"border_A_bottom.tga",
							"rect" : (0.0, 0.0, 8, 0),
						},
						{
							"name" : "center_img",
							"type" : "expanded_image",
							"x" : 15,
							"y" : 15,
							"image" : BACK_IMG_PATH+"border_A_center.tga",
							"rect" : (0.0, 0.0, 8, 21),
						},
						{
							"name" : "btn_cat0",
							"type" : "radio_button",
							"x" : 10,
							"y" : 5,
							"text" : "",
							"default_image" : "d:/ymir work/ui/public/ranking/btn_cat0_normal.sub",
							"over_image" : "d:/ymir work/ui/public/ranking/btn_cat0_over.sub",
							"down_image" : "d:/ymir work/ui/public/ranking/btn_cat0_down.sub",
						},
						{
							"name" : "btn_cat1",
							"type" : "radio_button",
							"x" : 10,
							"y" : 5 + ((36 * 1) + (5 * 1)),
							"text" : "",
							"default_image" : "d:/ymir work/ui/public/ranking/btn_cat0_normal.sub",
							"over_image" : "d:/ymir work/ui/public/ranking/btn_cat0_over.sub",
							"down_image" : "d:/ymir work/ui/public/ranking/btn_cat0_down.sub",
						},
						{
							"name" : "btn_cat2",
							"type" : "radio_button",
							"x" : 10,
							"y" : 5 + ((36 * 2) + (5 * 2)),
							"text" : "",
							"default_image" : "d:/ymir work/ui/public/ranking/btn_cat0_normal.sub",
							"over_image" : "d:/ymir work/ui/public/ranking/btn_cat0_over.sub",
							"down_image" : "d:/ymir work/ui/public/ranking/btn_cat0_down.sub",
						},
						{
							"name" : "btn_cat3",
							"type" : "radio_button",
							"x" : 10,
							"y" : 5 + ((36 * 3) + (5 * 3)),
							"text" : "",
							"default_image" : "d:/ymir work/ui/public/ranking/btn_cat0_normal.sub",
							"over_image" : "d:/ymir work/ui/public/ranking/btn_cat0_over.sub",
							"down_image" : "d:/ymir work/ui/public/ranking/btn_cat0_down.sub",
						},
						{
							"name" : "btn_cat4",
							"type" : "radio_button",
							"x" : 10,
							"y" : 5 + ((36 * 4) + (5 * 4)),
							"text" : "",
							"default_image" : "d:/ymir work/ui/public/ranking/btn_cat0_normal.sub",
							"over_image" : "d:/ymir work/ui/public/ranking/btn_cat0_over.sub",
							"down_image" : "d:/ymir work/ui/public/ranking/btn_cat0_down.sub",
						},
						{
							"name" : "btn_cat5",
							"type" : "radio_button",
							"x" : 10,
							"y" : 5 + ((36 * 5) + (5 * 5)),
							"text" : "",
							"default_image" : "d:/ymir work/ui/public/ranking/btn_cat0_normal.sub",
							"over_image" : "d:/ymir work/ui/public/ranking/btn_cat0_over.sub",
							"down_image" : "d:/ymir work/ui/public/ranking/btn_cat0_down.sub",
						},
						{
							"name" : "btn_cat6",
							"type" : "radio_button",
							"x" : 10,
							"y" : 5 + ((36 * 6) + (5 * 6)),
							"text" : "",
							"default_image" : "d:/ymir work/ui/public/ranking/btn_cat0_normal.sub",
							"over_image" : "d:/ymir work/ui/public/ranking/btn_cat0_over.sub",
							"down_image" : "d:/ymir work/ui/public/ranking/btn_cat0_down.sub",
						},
						{
							"name" : "btn_cat7",
							"type" : "radio_button",
							"x" : 10,
							"y" : 5 + ((36 * 7) + (5 * 7)),
							"text" : "",
							"default_image" : "d:/ymir work/ui/public/ranking/btn_cat0_normal.sub",
							"over_image" : "d:/ymir work/ui/public/ranking/btn_cat0_over.sub",
							"down_image" : "d:/ymir work/ui/public/ranking/btn_cat0_down.sub",
						},
						{
							"name" : "btn_cat8",
							"type" : "radio_button",
							"x" : 10,
							"y" : 5 + ((36 * 8) + (5 * 8)),
							"text" : "",
							"default_image" : "d:/ymir work/ui/public/ranking/btn_cat0_normal.sub",
							"over_image" : "d:/ymir work/ui/public/ranking/btn_cat0_over.sub",
							"down_image" : "d:/ymir work/ui/public/ranking/btn_cat0_down.sub",
						},
					),
				},
				{
					"name" : "ranking_list",
					"type" : "window",
					"x" : 214,
					"y" : 36,
					"width" : 312,
					"height" : 376,
					"children" :
					(
						{
							"name" : "left_top",
							"type" : "image",
							"x" : 0,
							"y" : 0,
							"image" : BACK_IMG_PATH+"border_A_left_top.tga",
						},
						{
							"name" : "right_top",
							"type" : "image",
							"x" : 296,
							"y" : 0,
							"image" : BACK_IMG_PATH+"border_A_right_top.tga",
						},
						{
							"name" : "left_bottom",
							"type" : "image",
							"x" : 0,
							"y" : 360,
							"image" : BACK_IMG_PATH+"border_A_left_bottom.tga",
						},
						{
							"name" : "right_bottom",
							"type" : "image",
							"x" : 296,
							"y" : 360,
							"image" : BACK_IMG_PATH+"border_A_right_bottom.tga",
						},
						{
							"name" : "left_center_img",
							"type" : "expanded_image",
							"x" : 0,
							"y" : 15,
							"image" : BACK_IMG_PATH+"border_A_left.tga",
							"rect" : (0.0, 0.0, 0, 21),
						},
						{
							"name" : "right_center_img",
							"type" : "expanded_image",
							"x" : 296,
							"y" : 15,
							"image" : BACK_IMG_PATH+"border_A_right.tga",
							"rect" : (0.0, 0.0, 0, 21),
						},
						{
							"name" : "top_center_img",
							"type" : "expanded_image",
							"x" : 15,
							"y" :  0,
							"image" : BACK_IMG_PATH+"border_A_top.tga",
							"rect" : (0.0, 0.0, 17, 0),
						},
						{
							"name" : "bottom_center_img",
							"type" : "expanded_image",
							"x" : 15,
							"y" : 360,
							"image" : BACK_IMG_PATH+"border_A_bottom.tga",
							"rect" : (0.0, 0.0, 17, 0),
						},
						{
							"name" : "center_img",
							"type" : "expanded_image",
							"x" : 15,
							"y" : 15,
							"image" : BACK_IMG_PATH+"border_A_center.tga",
							"rect" : (0.0, 0.0, 17, 21),
						},
					),	
				},
				{
					"name" : "list",
					"type" : "window",
					"x" : 217,
					"y" : 39,
					"width" : 376,
					"height" : 21,
					"children" :
					(
						{
							"name" : "sub_titlebar",
							"type" : "image",
							"x" : 0,
							"y" : 0,
							"image" : "d:/ymir work/ui/public/ranking/column_titlebar.sub",
							"children" :
							(
								{
									"name" : "column_rank",
									"type" : "text",
									"x" : 34,
									"y" : 5,
									"text" : uiscriptlocale.RANKING_RANK,
									"r" : 1.0,
									"g" : 1.0,
									"b" : 1.0,
									"a" : 1.0,
									"text_horizontal_align" : "center"
								},
								{
									"name" : "column_name",
									"type" : "text",
									"x" : 126,
									"y" : 5,
									"text" : uiscriptlocale.RANKING_NAME,
									"r" : 1.0,
									"g" : 1.0,
									"b" : 1.0,
									"a" : 1.0,
									"text_horizontal_align" : "center"
								},
								{
									"name" : "column_point",
									"type" : "text",
									"x" : 245,
									"y" : 5,
									"text" : uiscriptlocale.RANKING_POINTS,
									"r" : 1.0,
									"g" : 1.0,
									"b" : 1.0,
									"a" : 1.0,
									"text_horizontal_align" : "center"
								},
							),
						},
					),
				},
				{
					"name" : "scrollbar",
					"type" : "scrollbar",
					"x" : 532,
					"y" : 36,
					"size" : 376,
				},
				{
					"name" : "scrollbarCat",
					"type" : "scrollbar",
					"x" : 190,
					"y" : 36,
					"size" : 376,
				},
			),
		},
	),
}

