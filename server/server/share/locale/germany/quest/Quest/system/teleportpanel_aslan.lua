--+-------------------------------+------------+
--+-------------------------------+------------+
--|                                      		|                 |
--| Teleport-Panel Quest by Aslan 	| Version 1.0 |
--|                                      		|                 |
--+-------------------------------+------------+
--+-------------------------------+------------+

quest teleportpanel_aslan begin
	state start begin

---------------------------------------------------------------------------------------------------
---------------------------------     TELEPORT CONFIG     -----------------------------------------
---------------------------------------------------------------------------------------------------

		function MAP_DICT()
		return 
		{
			-- --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
			-- EMPIRES
			{["command"] = "m1_red",		["coord_red"] = { 474300, 954800}, 	["coord_yel"] = { 474300, 954800}, 	["coord_blu"] = { 474300, 954800},			["min_lv"] = 1,		["max_lv"] = 120,	["money_cost"] = 10000, 	["item_cost_vnum"] = 0, ["item_cost_count"] = 0 },
			{["command"] = "m1_yel",		["coord_red"] = { 63800, 166400}, 	["coord_yel"] = { 63800, 166400}, 	["coord_blu"] = { 63800, 166400}, 			["min_lv"] = 1,		["max_lv"] = 120,	["money_cost"] = 10000, 	["item_cost_vnum"] = 0, ["item_cost_count"] = 0 },
			{["command"] = "m1_blu",		["coord_red"] = { 959900, 269200}, 	["coord_yel"] = { 959900, 269200}, 	["coord_blu"] = { 959900, 269200}, 			["min_lv"] = 1,		["max_lv"] = 120,	["money_cost"] = 10000, 	["item_cost_vnum"] = 0, ["item_cost_count"] = 0 },
			
			{["command"] = "m2_red",		["coord_red"] = { 353100, 882900}, 	["coord_yel"] = { 353100, 882900}, 	["coord_blu"] = { 353100, 882900},			["min_lv"] = 1,		["max_lv"] = 120,	["money_cost"] = 10000, 	["item_cost_vnum"] = 0, ["item_cost_count"] = 0 },
			{["command"] = "m2_yel",		["coord_red"] = { 145500, 240000}, 	["coord_yel"] = { 145500, 240000}, 	["coord_blu"] = { 145500, 240000},			["min_lv"] = 1,		["max_lv"] = 120,	["money_cost"] = 10000, 	["item_cost_vnum"] = 0, ["item_cost_count"] = 0 },
			{["command"] = "m2_blu",		["coord_red"] = { 863900, 246000}, 	["coord_yel"] = { 863900, 246000}, 	["coord_blu"] = { 863900, 246000}, 			["min_lv"] = 1,		["max_lv"] = 120,	["money_cost"] = 10000, 	["item_cost_vnum"] = 0, ["item_cost_count"] = 0 },
			
			{["command"] = "guildland",	["coord_red"] = { 135600, 4300},		["coord_yel"] = { 221900, 9300},		["coord_blu"] = { 271800, 13000},				["min_lv"] = 20,	["max_lv"] = 120,	["money_cost"] = 25000,	["item_cost_vnum"] = 0, ["item_cost_count"] = 0 },

			{["command"] = "andra_map",	["coord_red"] = { 466700, 508700},		["coord_yel"] = { 466700, 508700},		["coord_blu"] = { 466700, 508700},				["min_lv"] = 1,	["max_lv"] = 120,	["money_cost"] = 10000,	["item_cost_vnum"] = 0, ["item_cost_count"] = 0 },
			
			-- --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
			-- NEUTRAL
			{["command"] = "seungryong_start",	["coord_red"] = { 402500, 673500},	["coord_yel"] = { 270000, 740200},		["coord_blu"] = { 320700, 808400},		["min_lv"] = 20,	["max_lv"] = 120,	["money_cost"] = 50000,	["item_cost_vnum"] = 0, ["item_cost_count"] = 0 },
			{["command"] = "seungryong_middle",	["coord_red"] = { 345900, 731900},	["coord_yel"] = { 317100, 731900},		["coord_blu"] = { 333100, 755900},		["min_lv"] = 20,	["max_lv"] = 120,	["money_cost"] = 50000,	["item_cost_vnum"] = 0, ["item_cost_count"] = 0 },
			
			{["command"] = "yongbi_start",		["coord_red"] = { 216800, 628400},		["coord_yel"] = { 220800, 501300},		["coord_blu"] = { 344000, 502500},		["min_lv"] = 20,	["max_lv"] = 120,	["money_cost"] = 50000,	["item_cost_vnum"] = 0, ["item_cost_count"] = 0 },
			{["command"] = "yongbi_middle",	["coord_red"] = { 297200, 547100},		["coord_yel"] = { 297200, 547100},		["coord_blu"] = { 297200, 547100},		["min_lv"] = 20,	["max_lv"] = 120,	["money_cost"] = 50000,	["item_cost_vnum"] = 0, ["item_cost_count"] = 0 },
			
			{["command"] = "sohan_start",		["coord_red"] = { 434000, 292400},		["coord_yel"] = { 376000, 174000},		["coord_blu"] = { 492800, 172000},		["min_lv"] = 20,	["max_lv"] = 120,	["money_cost"] = 50000,	["item_cost_vnum"] = 0, ["item_cost_count"] = 0 },
			{["command"] = "sohan_middle",		["coord_red"] = { 436500, 215300},		["coord_yel"] = { 436500, 215300},		["coord_blu"] = { 436500, 215300},		["min_lv"] = 20,	["max_lv"] = 120,	["money_cost"] = 50000,	["item_cost_vnum"] = 0, ["item_cost_count"] = 0 },
			
			{["command"] = "fireland_start",		["coord_red"] = { 599500, 756400},		["coord_yel"] = { 597700, 622300},		["coord_blu"] = { 731500, 689600},		["min_lv"] = 20,	["max_lv"] = 120,	["money_cost"] = 50000,	["item_cost_vnum"] = 0, ["item_cost_count"] = 0 },
			{["command"] = "fireland_middle",	["coord_red"] = { 604000, 682700},		["coord_yel"] = { 604000, 682700},		["coord_blu"] = { 604000, 682700},		["min_lv"] = 20,	["max_lv"] = 120,	["money_cost"] = 50000,	["item_cost_vnum"] = 0, ["item_cost_count"] = 0 },

			{["command"] = "snakefield_start",		["coord_red"] = { 1059100, 727300},		["coord_yel"] = { 1059100, 727300},		["coord_blu"] = { 1059100, 727300},	["min_lv"] = 40,	["max_lv"] = 120,	["money_cost"] = 75000,	["item_cost_vnum"] = 0, ["item_cost_count"] = 0 },
			{["command"] = "snakefield_middle",	["coord_red"] = { 1076100, 748900},		["coord_yel"] = { 1076100, 748900},		["coord_blu"] = { 1076100, 748900},	["min_lv"] = 40,	["max_lv"] = 120,	["money_cost"] = 75000,	["item_cost_vnum"] = 0, ["item_cost_count"] = 0 },
			
			{["command"] = "ghostwood_start",	["coord_red"] = { 289300, 5700},		["coord_yel"] = { 289300, 5700},		["coord_blu"] = { 289300, 5700},		["min_lv"] = 50,	["max_lv"] = 120,	["money_cost"] = 250000,	["item_cost_vnum"] = 0, ["item_cost_count"] = 0 },
			{["command"] = "ghostwood_end",		["coord_red"] = { 286600, 43900},		["coord_yel"] = { 286600, 43900},		["coord_blu"] = { 286600, 43900},		["min_lv"] = 50,	["max_lv"] = 120,	["money_cost"] = 250000,	["item_cost_vnum"] = 0, ["item_cost_count"] = 0 },
			
			{["command"] = "redwood_start",	["coord_red"] = { 1119000, 69700},		["coord_yel"] = { 1119000, 69700},		["coord_blu"] = { 1119000, 69700},		["min_lv"] = 50,	["max_lv"] = 120,	["money_cost"] = 250000,	["item_cost_vnum"] = 0, ["item_cost_count"] = 0 },
			{["command"] = "redwood_end",		["coord_red"] = { 1120100, 11200},		["coord_yel"] = { 1120100, 11200},		["coord_blu"] = { 1120100, 11200},		["min_lv"] = 50,	["max_lv"] = 120,	["money_cost"] = 250000,	["item_cost_vnum"] = 0, ["item_cost_count"] = 0 },
			
			-- {["command"] = "giants_start",		["coord_red"] = { 829000, 763500},		["coord_yel"] = { 829000, 763500},		["coord_blu"] = { 829000, 763500},		["min_lv"] = 60,	["max_lv"] = 120,	["money_cost"] = 250000,	["item_cost_vnum"] = 0, ["item_cost_count"] = 0 },
			-- {["command"] = "giants_end",		["coord_red"] = { 858500, 726200},		["coord_yel"] = { 858500, 726200},		["coord_blu"] = { 858500, 726200},		["min_lv"] = 60,	["max_lv"] = 120,	["money_cost"] = 250000,	["item_cost_vnum"] = 0, ["item_cost_count"] = 0 },
			
			-- --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
			-- MAPS LEVEL
			{["command"] = "sd1_start",		["coord_red"] = { 59900, 496300},			["coord_yel"] = { 59900, 496300},		["coord_blu"] = { 59900, 496300},		["min_lv"] = 40,	["max_lv"] = 120,	["money_cost"] = 100000,	["item_cost_vnum"] = 0, ["item_cost_count"] = 0 },
			{["command"] = "sd2_start",		["coord_red"] = { 704100, 464100},		["coord_yel"] = { 704100, 464100},		["coord_blu"] = { 704100, 464100},		["min_lv"] = 50,	["max_lv"] = 120,	["money_cost"] = 125000,	["item_cost_vnum"] = 0, ["item_cost_count"] = 0 },
			{["command"] = "sd3_start",		["coord_red"] = { 51200, 563200},		["coord_yel"] = { 51200, 563200},		["coord_blu"] = { 51200, 563200},		["min_lv"] = 65,	["max_lv"] = 120,	["money_cost"] = 150000,	["item_cost_vnum"] = 0, ["item_cost_count"] = 0 },

			{["command"] = "deamontower",		["coord_red"] = { 590300, 115500},	["coord_yel"] = { 590300, 115500},		["coord_blu"] = { 590300, 115500},		["min_lv"] = 40,	["max_lv"] = 120,	["money_cost"] = 200000,	["item_cost_vnum"] = 0, ["item_cost_count"] = 0 },
			--{["command"] = "devilscatacomb",	["coord_red"] = { 586200, 98100},		["coord_yel"] = { 586200, 98100},		["coord_blu"] = { 586200, 98100},		["min_lv"] = 20,	["max_lv"] = 120,	["money_cost"] = 200000,	["item_cost_vnum"] = 0, ["item_cost_count"] = 0 },
			
			{["command"] = "skipia1",			["coord_red"] = { 10000, 1206700},		["coord_yel"] = { 10000, 1206700},		["coord_blu"] = { 10000, 1206700},		["min_lv"] = 60,	["max_lv"] = 120,	["money_cost"] = 3000000,	["item_cost_vnum"] = 0, ["item_cost_count"] = 0 },
			{["command"] = "skipia2",			["coord_red"] = { 241700, 1274900},		["coord_yel"] = { 241700, 1274900},	["coord_blu"] = { 241700, 1274900},	["min_lv"] = 75,	["max_lv"] = 120,	["money_cost"] = 4000000,	["item_cost_vnum"] = 0, ["item_cost_count"] = 0 },
			
			{["command"] = "bahia_nefrit",	["coord_red"] = { 1087900, 1652300},		["coord_yel"] = { 1087900, 1652300},	["coord_blu"] = { 1087900, 1652300},	["min_lv"] = 90,	["max_lv"] = 120,	["money_cost"] = 5000000,	["item_cost_vnum"] = 0, ["item_cost_count"] = 0 },
			{["command"] = "map_trueno",	["coord_red"] = { 1134700, 1653900},		["coord_yel"] = { 1134700, 1653900},	["coord_blu"] = { 1134700, 1653900},	["min_lv"] = 90,	["max_lv"] = 120,	["money_cost"] = 5000000,	["item_cost_vnum"] = 0, ["item_cost_count"] = 0 },
			{["command"] = "map_gautama",	["coord_red"] = { 1225700, 1681600},		["coord_yel"] = { 1225700, 1681600},	["coord_blu"] = { 1225700, 1681600},	["min_lv"] = 90,	["max_lv"] = 120,	["money_cost"] = 5000000,	["item_cost_vnum"] = 0, ["item_cost_count"] = 0 },
			{["command"] = "map_cabo_drag",	["coord_red"] = { 1085000, 1784400},		["coord_yel"] = { 1085000, 1784400},	["coord_blu"] = { 1085000, 1784400},	["min_lv"] = 90,	["max_lv"] = 120,	["money_cost"] = 5000000,	["item_cost_vnum"] = 0, ["item_cost_count"] = 0 },
			{["command"] = "map_bosq_encant",	["coord_red"] = { 813000, 1502400},		["coord_yel"] = { 813000, 1502400},	["coord_blu"] = { 813000, 1502400},	["min_lv"] = 100,	["max_lv"] = 120,	["money_cost"] = 10000000,	["item_cost_vnum"] = 0, ["item_cost_count"] = 0 },
		}
		end

---------------------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------------------
		---------------------------------------------------------------------------------------------
		----------------------------- Basic Quest | - | No Edit this -----------------------------------

		when login begin
			cmdchat('teleportpanel '..string.format("SET_QUEST_ID(%d)", q.getcurrentquestindex()))
		end
		
		when button begin
			local MAP_DICT = teleportpanel_aslan.MAP_DICT()
			local cmd = teleportpanel_aslan.get_quest_cmd()
			for i = 1, table.getn(MAP_DICT), 1 do
				if MAP_DICT[i]["command"] == cmd[2] then
					if pc.can_warp() == false then
						syschat("Du kannst dich gerade nicht teleportieren.")
						return
					end
					
					if pc.get_level() < MAP_DICT[i]["min_lv"] and pc.get_level() > MAP_DICT[i]["max_lv"] then
						syschat("Du bist ausserhalb der Levellimits.")
						return
					end
					
					if pc.get_gold() < MAP_DICT[i]["money_cost"] then
						syschat("Du hast nicht genug Yang.")
						return
					end
					
					if MAP_DICT[i]["item_cost_vnum"] != 0 and pc.count_item(MAP_DICT[i]["item_cost_vnum"]) < MAP_DICT[i]["item_cost_count"] then
						syschat("Du hast nicht genug Gegenstände.")
						return
					else
						pc.remove_item(MAP_DICT[i]["item_cost_vnum"], MAP_DICT[i]["item_cost_count"])
					end
					
					pc.change_gold(-MAP_DICT[i]["money_cost"])
					if pc.get_empire() == 1 then
						pc.warp(MAP_DICT[i]["coord_red"][1], MAP_DICT[i]["coord_red"][2])
					elseif pc.get_empire() == 2 then
						pc.warp(MAP_DICT[i]["coord_yel"][1], MAP_DICT[i]["coord_yel"][2])
					elseif pc.get_empire() == 3 then
						pc.warp(MAP_DICT[i]["coord_blu"][1], MAP_DICT[i]["coord_blu"][2])
					end
					return
				end
			end
		end
		
		---------------------------------------------------------------------------------------------
		---------------------------------------------------------------------------------------------
		---------------------------------------------------------------------------------------------
		--------------------- CLIENT to QUEST functions | - | No Edit this ----------------------------
		
		function get_quest_cmd()
			cmdchat('GET_INPUT_BEGIN')
			local ret = input(cmdchat('teleportpanel GET_QUEST_CMD()'))
			cmdchat('GET_INPUT_END')
			-- syschat("cmd:"..ret)
			return teleportpanel_aslan.split_(ret,'#')
		end

		function split_(string_, sep)
			local sep, fields = sep or ":", {}
			local pattern = string.format("([^%s]+)", sep)
			string.gsub(string_,pattern, function(c) fields[table.getn(fields)+1] = c end)
			return fields
		end
	end
end
