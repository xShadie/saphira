quest main_quest_lv60_Special begin
	state start begin
	end
	state run begin
		when enter or login or levelup with pc.get_level() >= 60 begin
			set_state(piedra_preciosa1)
		end
	end

	state piedra_preciosa1 begin
		when letter begin
			local lang = pc.get_language()
			local v = find_npc_by_vnum(20409)
			if v != 0 then
				target.vid("__TARGET__", v, gameforge[lang].main_quest._black_sohan_mission_special60)
			end
			send_letter(gameforge[lang].main_quest._black_sohan_mission_special60)
		end

		when button or info begin
			local lang = pc.get_language()
			say_title(string.format("%s:", gameforge[lang].main_quest._black_sohan_mission_special60))
			say("")
			say(string.format(gameforge[lang].main_quest._1, mob_name(20409)))
			say(gameforge[lang].main_quest._mission_orc_01)
			say(gameforge[lang].main_quest._mission_orc_02)
		end

		when __TARGET__.target.click begin
			target.delete("__TARGET__")
			local lang = pc.get_language()
			say_title(string.format("%s:", mob_name(20409)))
			say("")
			say(gameforge[lang].main_quest._mission_sohan_31)
			say("")
			say(gameforge[lang].main_quest._mission_sohan_32)
			wait()
			say(gameforge[lang].main_quest._mission_sohan_33)
			say(gameforge[lang].main_quest._mission_sohan_34)
			say(gameforge[lang].main_quest._mission_sohan_35)
			say_item_vnum(33431)
			say(gameforge[lang].main_quest._mission_orc_09)
			set_state(piedra_esmeralda)
		end
	end

	state piedra_esmeralda begin
		when letter begin
			send_letter(gameforge[pc.get_language()].main_quest._black_sohan_mission_special60)
		end
		when button or info begin
			local lang = pc.get_language()
			say_title(string.format("%s:", mob_name(20409)))
			say(gameforge[lang].main_quest._mission_sohan_35)
			say(gameforge[lang].main_quest._mission_sohan_36)
			say(gameforge[lang].main_quest._mission_sohan_37)
			say_item_vnum(33431)
			say(gameforge[lang].main_quest._mission_sohan_38)
		end
		when	1101.kill or
				1131.kill or
				1151.kill or
				1171.kill
				begin
			local lang = pc.get_language()
			local s = number(1, 1000)
			if s <= 5 then
				pc.give_item2(33431, 1)
				say(gameforge[lang].main_quest._mission_sohan_39)
				say_item_vnum(33431)
				say(gameforge[lang].main_quest._mission_sohan_40)
			end
		end
		
		when 20359.take with item.get_vnum() == 33431 begin
			local lang = pc.get_language()
			say_item_vnum(33431)
			say(gameforge[lang].main_quest._mission_sohan_41)
			pc.remove_item("33431", 200)
			set_state(piedra_preciosa2)
		end
	end
	state piedra_preciosa2 begin
		when letter begin
			local lang = pc.get_language()
			local v = find_npc_by_vnum(20409)
			if v != 0 then
				target.vid("__TARGET__", v, gameforge[lang].main_quest._black_sohan_mission_special60)
			end
			send_letter(gameforge[lang].main_quest._black_sohan_mission_special60)
		end

		when button or info begin
			local lang = pc.get_language()
			say_title(string.format("%s:", gameforge[lang].main_quest._black_sohan_mission_special60))
			say("")
			say(string.format(gameforge[lang].main_quest._1, mob_name(20409)))
			say(gameforge[lang].main_quest._mission_orc_01)
			say(gameforge[lang].main_quest._mission_orc_02)
		end

		when __TARGET__.target.click begin
			target.delete("__TARGET__")
			local lang = pc.get_language()
			say_title(string.format("%s:", mob_name(20409)))
			say("")
			say(gameforge[lang].main_quest._mission_sohan_42)
			say("")
			say(gameforge[lang].main_quest._mission_sohan_43)
			say(gameforge[lang].main_quest._mission_sohan_44)
			say_item_vnum(33432)
			say(gameforge[lang].main_quest._mission_orc_09)
			set_state(piedra_rubi)
		end
	end

	state piedra_rubi begin
		when letter begin
			send_letter(gameforge[pc.get_language()].main_quest._black_sohan_mission_special60)
		end
		when button or info begin
			local lang = pc.get_language()
			say_title(string.format("%s:", mob_name(20409)))
			say(gameforge[lang].main_quest._mission_sohan_46)
			say(gameforge[lang].main_quest._mission_sohan_47)
			say(gameforge[lang].main_quest._mission_sohan_48)
			say_item_vnum(33432)
			say(gameforge[lang].main_quest._mission_sohan_49)
		end
		when	1102.kill or
				1132.kill or
				1152.kill or
				1172.kill
				begin
			local lang = pc.get_language()
			local s = number(1, 1000)
			if s <= 5 then
				pc.give_item2(33432, 1)
				say(gameforge[lang].main_quest._mission_sohan_39)
				say_item_vnum(33432)
				say(gameforge[lang].main_quest._mission_sohan_40)
			end
		end
		
		when 20359.take with item.get_vnum() == 33432 begin
			local lang = pc.get_language()
			say_item_vnum(33432)
			say(gameforge[lang].main_quest._mission_sohan_41)
			pc.remove_item("33432", 200)
			set_state(piedra_preciosa3)
		end
	end
	state piedra_preciosa3 begin
		when letter begin
			local lang = pc.get_language()
			local v = find_npc_by_vnum(20409)
			if v != 0 then
				target.vid("__TARGET__", v, gameforge[lang].main_quest._black_sohan_mission_special60)
			end
			send_letter(gameforge[lang].main_quest._black_sohan_mission_special60)
		end

		when button or info begin
			local lang = pc.get_language()
			say_title(string.format("%s:", gameforge[lang].main_quest._black_sohan_mission_special60))
			say("")
			say(string.format(gameforge[lang].main_quest._1, mob_name(20409)))
			say(gameforge[lang].main_quest._mission_orc_01)
			say(gameforge[lang].main_quest._mission_orc_02)
		end

		when __TARGET__.target.click begin
			target.delete("__TARGET__")
			local lang = pc.get_language()
			say_title(string.format("%s:", mob_name(20409)))
			say("")
			say(gameforge[lang].main_quest._mission_sohan_50)
			say("")
			say(gameforge[lang].main_quest._mission_sohan_51)
			say(gameforge[lang].main_quest._mission_sohan_52)
			say_item_vnum(33433)
			say(gameforge[lang].main_quest._mission_orc_09)
			set_state(piedra_calcita)
		end
	end

	state piedra_calcita begin
		when letter begin
			send_letter(gameforge[pc.get_language()].main_quest._black_sohan_mission_special60)
		end
		when button or info begin
			local lang = pc.get_language()
			say_title(string.format("%s:", mob_name(20409)))
			say(gameforge[lang].main_quest._mission_sohan_53)
			say(gameforge[lang].main_quest._mission_sohan_54)
			say(gameforge[lang].main_quest._mission_sohan_55)
			say_item_vnum(33433)
			say(gameforge[lang].main_quest._mission_sohan_56)
		end
		when	1103.kill or
				1133.kill or
				1153.kill or
				1173.kill
				begin
			local lang = pc.get_language()
			local s = number(1, 1000)
			if s <= 5 then
				pc.give_item2(33433, 1)
				say(gameforge[lang].main_quest._mission_sohan_57)
				say_item_vnum(33433)
				say(gameforge[lang].main_quest._mission_sohan_40)
			end
		end
		
		when 20359.take with item.get_vnum() == 33433 begin
			local lang = pc.get_language()
			say_item_vnum(33433)
			say(gameforge[lang].main_quest._mission_sohan_41)
			pc.remove_item("33433", 200)
			set_state(piedra_preciosa4)
		end
	end
	state piedra_preciosa4 begin
		when letter begin
			local lang = pc.get_language()
			local v = find_npc_by_vnum(20409)
			if v != 0 then
				target.vid("__TARGET__", v, gameforge[lang].main_quest._black_sohan_mission_special60)
			end
			send_letter(gameforge[lang].main_quest._black_sohan_mission_special60)
		end

		when button or info begin
			local lang = pc.get_language()
			say_title(string.format("%s:", gameforge[lang].main_quest._black_sohan_mission_special60))
			say("")
			say(string.format(gameforge[lang].main_quest._1, mob_name(20409)))
			say(gameforge[lang].main_quest._mission_orc_01)
			say(gameforge[lang].main_quest._mission_orc_02)
		end

		when __TARGET__.target.click begin
			target.delete("__TARGET__")
			local lang = pc.get_language()
			say_title(string.format("%s:", mob_name(20409)))
			say("")
			say(gameforge[lang].main_quest._mission_sohan_58)
			say("")
			say(gameforge[lang].main_quest._mission_sohan_59)
			say(gameforge[lang].main_quest._mission_sohan_60)
			say_item_vnum(33434)
			say(gameforge[lang].main_quest._mission_orc_09)
			set_state(piedra_Opalo)
		end
	end

	state piedra_Opalo begin
		when letter begin
			send_letter(gameforge[pc.get_language()].main_quest._black_sohan_mission_special60)
		end
		when button or info begin
			local lang = pc.get_language()
			say_title(string.format("%s:", mob_name(20409)))
			say(gameforge[lang].main_quest._mission_sohan_61)
			say(gameforge[lang].main_quest._mission_sohan_62)
			say(gameforge[lang].main_quest._mission_sohan_63)
			say_item_vnum(33434)
			say(gameforge[lang].main_quest._mission_sohan_64)
		end
		when	1104.kill or
				1134.kill or
				1154.kill or
				1174.kill
				begin
			local lang = pc.get_language()
			local s = number(1, 1000)
			if s <= 5 then
				pc.give_item2(33434, 1)
				say(gameforge[lang].main_quest._mission_sohan_65)
				say_item_vnum(33434)
				say(gameforge[lang].main_quest._mission_sohan_40)
			end
		end
		
		when 20359.take with item.get_vnum() == 33434 begin
			local lang = pc.get_language()
			say_item_vnum(33434)
			say(gameforge[lang].main_quest._mission_sohan_41)
			pc.remove_item("33434", 200)
			set_state(piedra_preciosa5)
		end
	end
	state piedra_preciosa5 begin
		when letter begin
			local lang = pc.get_language()
			local v = find_npc_by_vnum(20409)
			if v != 0 then
				target.vid("__TARGET__", v, gameforge[lang].main_quest._black_sohan_mission_special60)
			end
			send_letter(gameforge[lang].main_quest._black_sohan_mission_special60)
		end

		when button or info begin
			local lang = pc.get_language()
			say_title(string.format("%s:", gameforge[lang].main_quest._black_sohan_mission_special60))
			say("")
			say(string.format(gameforge[lang].main_quest._1, mob_name(20409)))
			say(gameforge[lang].main_quest._mission_orc_01)
			say(gameforge[lang].main_quest._mission_orc_02)
		end

		when __TARGET__.target.click begin
			target.delete("__TARGET__")
			local lang = pc.get_language()
			say_title(string.format("%s:", mob_name(20409)))
			say("")
			say(gameforge[lang].main_quest._mission_sohan_66)
			say("")
			say(gameforge[lang].main_quest._mission_sohan_67)
			say(gameforge[lang].main_quest._mission_sohan_68)
			say_item_vnum(33435)
			say(gameforge[lang].main_quest._mission_orc_09)
			set_state(piedra_Ambar)
		end
	end

	state piedra_Ambar begin
		when letter begin
			send_letter(gameforge[pc.get_language()].main_quest._black_sohan_mission_special60)
		end
		when button or info begin
			local lang = pc.get_language()
			say_title(string.format("%s:", mob_name(20409)))
			say(gameforge[lang].main_quest._mission_sohan_69)
			say(gameforge[lang].main_quest._mission_sohan_70)
			say(gameforge[lang].main_quest._mission_sohan_71)
			say_item_vnum(33435)
			say(gameforge[lang].main_quest._mission_sohan_72)
		end
		when	1105.kill or
				1135.kill or
				1155.kill or
				1175.kill
				begin
			local lang = pc.get_language()
			local s = number(1, 1000)
			if s <= 5 then
				pc.give_item2(33435, 1)
				say(gameforge[lang].main_quest._mission_sohan_73)
				say_item_vnum(33435)
				say(gameforge[lang].main_quest._mission_sohan_40)
			end
		end
		
		when 20359.take with item.get_vnum() == 33435 begin
			local lang = pc.get_language()
			say_item_vnum(33435)
			say(gameforge[lang].main_quest._mission_sohan_41)
			pc.remove_item("33435", 200)
			set_state(goto_kill_boss)
		end
	end

	state goto_kill_boss begin
		when letter begin
			send_letter(gameforge[pc.get_language()].main_quest._black_soul_mission_special50)
		end
		
		when button or info begin
			local lang = pc.get_language()
			say_title(string.format("%s:", gameforge[lang].main_quest._black_soul_mission_special50))
			say("")
			say(string.format(gameforge[lang].main_quest._mission_orc_55, mob_name(20409)))
			say(gameforge[lang].main_quest._mission_orc_56)
			set_state(boss_lider_sohan)
		end
	end

	state boss_lider_sohan begin
		when letter begin
			local lang = pc.get_language()
			local v = find_npc_by_vnum(20409)
			if v != 0 then
				target.vid("__TARGET__", v, gameforge[lang].main_quest._black_soul_mission_special50)
			end
			send_letter(gameforge[lang].main_quest._black_soul_mission_special50)
		end

		when button or info begin
			local lang = pc.get_language()
			say_title(string.format("%s:", gameforge[lang].main_quest._black_soul_mission_special50))
			say("")
			say(string.format(gameforge[lang].main_quest._mission_orc_55, mob_name(20409)))
			say(gameforge[lang].main_quest._mission_orc_56)
		end

		when __TARGET__.target.click begin
			target.delete("__TARGET__")
			local lang = pc.get_language()
			say_title(string.format("%s:", mob_name(20409)))
			say("")
			say(gameforge[lang].main_quest._23)
			say(gameforge[lang].main_quest._24)
			say("")
			say(gameforge[lang].main_quest._mission_orc_85)
			say(gameforge[lang].main_quest._mission_orc_86)
			pc.setqf("1171", 75)
			pc.setqf("1172", 75)
			pc.setqf("1173", 75)
			pc.setqf("1174", 75)
			pc.setqf("1175", 75)
			pc.setqf("1176", 75)
			pc.setqf("1177", 75)
			pc.setqf("1901", 10)
			pc.setqf("8013", 20)
			set_state(boss_lider_osc1)
		end
	end

	state boss_lider_osc1 begin
		when letter begin
			send_letter(gameforge[pc.get_language()].main_quest._black_sohan_mission_special60)
		end

		when button or info begin
			local lang = pc.get_language()
			say_title(string.format("%s:", gameforge[lang].main_quest._black_sohan_mission_special60))
			say("")
			say(gameforge[lang].main_quest._37)
			say(string.format("- %d %s,", pc.getqf("1171"), mob_name(1171)))
			say(string.format("- %d %s,", pc.getqf("1172"), mob_name(1172)))
			say(string.format("- %d %s,", pc.getqf("1173"), mob_name(1173)))
			say(string.format("- %d %s,", pc.getqf("1174"), mob_name(1174)))
			say(string.format("- %d %s,", pc.getqf("1175"), mob_name(1175)))
			say(string.format("- %d %s,", pc.getqf("1176"), mob_name(1176)))
			say(string.format("- %d %s,", pc.getqf("1177"), mob_name(1177)))
			say(string.format("- %d %s,", pc.getqf("1901"), mob_name(1901)))
			say(string.format("- %d %s,", pc.getqf("8013"), mob_name(8013)))
		end

		when	1171.kill or
				1172.kill or
				1173.kill or
				1174.kill or
				1175.kill or
				1176.kill or
				1177.kill or
				1901.kill or
				8013.kill
				begin
			local n = string.format("%s", npc.get_race())
			local c = pc.getqf(n)
			if c > 0 then
				pc.setqf(n, c - 1)
			end
			
			local t =	pc.getqf("1171") +
						pc.getqf("1172") +
						pc.getqf("1173") +
						pc.getqf("1174") +
						pc.getqf("1175") +
						pc.getqf("1176") +
						pc.getqf("1177") +
						pc.getqf("1901") +
						pc.getqf("8013")
			
			if t == 0 then
				set_state(gotoreward)
			end
		end
	end
	state gotoreward begin
		when letter begin
			local lang = pc.get_language()
			local v = find_npc_by_vnum(20409)
			if v != 0 then
				target.vid("__TARGET__", v, gameforge[lang].main_quest._black_soul_mission_end_special50)
			end
			
			send_letter(gameforge[lang].main_quest._black_soul_mission_end_special50)
		end

		when button or info begin
			local lang = pc.get_language()
			say_title(string.format("%s:", gameforge[lang].main_quest._black_soul_mission_end_special50))
			say("")
			say(gameforge[lang].main_quest._49)
			say(string.format(gameforge[lang].main_quest._mission_orc_55, mob_name(20409)))
		end

		when __TARGET__.target.click begin
			target.delete("__TARGET__")
			local lang = pc.get_language()
			say_title(string.format("%s:", mob_name(20409)))
			say("")
			say(string.format(gameforge[lang].main_quest._black_soul_mission_end3, pc.get_name()))
			say(gameforge[lang].main_quest._black_soul_mission_end4)
			local job = pc.get_job()
			if job == 0 then -- Warrior
				pc.give_item2(40100, 1)
				pc.give_item2(40097, 1)
			elseif job == 1 then -- Assassin
				pc.give_item2(40100, 1)
				pc.give_item2(40098, 1)
				pc.give_item2(40099, 1)
			elseif job == 2 then -- Sura
				pc.give_item2(40100, 1)
			elseif job == 3 then -- Shaman
				pc.give_item2(40095, 1)
				pc.give_item2(40096, 1)
			end
			pc.give_item2(30094, 80)
			pc.give_item2(30095, 40)
			pc.give_item2(30096, 24)
			pc.give_item2(30279, 30)
			pc.give_item2(30277, 15)
			pc.give_item2(30284, 30)
			pc.give_item2(30271, 10)
			pc.give_item2(72310, 20)
			pc.give_item2(71094, 20)
			pc.give_item2(70401, 6)
			pc.give_item2(72320, 4)
			pc.give_item2(88960, 8)
			pc.give_item2(25041, 10)
			pc.give_item2(50926, 10)
			pc.give_item2(39063, 1)
			pc.give_item2(72001, 1)
			pc.give_item2(72006, 1)
			pc.give_item2(71084, 800)
			pc.give_exp2(65000000)
			say_reward(gameforge[lang].main_quest._58)
			say_reward(gameforge[lang].main_quest._mission_sohan_reward_esp1)
			say_reward(gameforge[lang].main_quest._mission_sohan_reward_esp2)
			say_reward(gameforge[lang].main_quest._mission_sohan_reward_esp3)
			say_reward(gameforge[lang].main_quest._mission_sohan_reward_esp4)
			say_reward(gameforge[lang].main_quest._mission_sohan_reward_esp5)
			say_reward(gameforge[lang].main_quest._mission_sohan_reward_esp6)
			say_reward(gameforge[lang].main_quest._mission_sohan_reward_esp7)
			say_reward(gameforge[lang].main_quest._mission_sohan_reward_esp8)
			say_reward(gameforge[lang].main_quest._mission_sohan_reward_esp9)
			say_reward(gameforge[lang].main_quest._mission_sohan_reward_esp10)
			say_reward(gameforge[lang].main_quest._mission_sohan_reward_esp11)
			say_reward(gameforge[lang].main_quest._mission_sohan_reward_esp12)
			say_reward(gameforge[lang].main_quest._mission_sohan_reward_esp13)
			say_reward(gameforge[lang].main_quest._mission_sohan_reward_esp14)
			say_reward(gameforge[lang].main_quest._mission_sohan_reward_esp15)
			say_reward(gameforge[lang].main_quest._mission_sohan_reward_esp16)
			say_reward(gameforge[lang].main_quest._mission_sohan_reward_esp17)
			say_reward(gameforge[lang].main_quest._city2_Jefe_reward_09)
			say_reward(gameforge[lang].main_quest._city2_Jefe_reward_10)
			say_reward(gameforge[lang].main_quest._mission_orc_reward_exp50_esp)
			pc.delqf("1171")
			pc.delqf("1172")
			pc.delqf("1173")
			pc.delqf("1174")
			pc.delqf("1175")
			pc.delqf("1176")
			pc.delqf("1177")
			pc.delqf("1901")
			pc.delqf("8013")
			set_quest_state("main_quest_lv65", "run")
			set_state(__COMPLETE__)
			clear_letter()
		end
	end

	state __COMPLETE__ begin
	end
end

