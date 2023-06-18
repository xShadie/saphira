ROOT = "d:/ymir work/ui/public/"

window = {
	"name" : "QuestDialog",
	"style" : ("float",),#"movable", 

	"x" : 0,
	"y" : 0,

	"width" : 800,
	"height" : 450,

	"children" :
	(
		{
			"name" : "board",
			"type" : "thinboard",
			"style" : ("attach", "ignore_size",),

			"x" : 0,
			"y" : 0,

			"horizontal_align" : "center",
			"vertical_align" : "center",

			"width" : 350,
			"height" : 300,
			"children" : 
			(
				{
					"name" : "board2",
					"type" : "image",
					"x" : -80, "y" : -120,
					#"image" : "locale/it/ui/questboard_andra/questboard.tga",
					"image" : "locale/es/ui/wol2/quest.tga",
				},
			),
		},
	),
}
