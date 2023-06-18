quest main_quest_lv60 begin
	state start begin
	end

	state run begin
		when enter or login or levelup with pc.get_level() >= 60 begin
			set_state(gototeacher)
		end
	end

	state gototeacher begin
		when letter begin
			local lang = pc.get_language()
			local v = find_npc_by_vnum(20409)
			if v != 0 then
				target.vid("__TARGET__", v, gameforge[lang].main_quest._black_sohan_mission_II)
			end
			send_letter(gameforge[lang].main_quest._black_sohan_mission_II)
		end
		when button or info begin
			local lang = pc.get_language()
			say_title(string.format("%s:", gameforge[lang].main_quest._black_sohan_mission_II))
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
			say(gameforge[lang].main_quest._mission_sohan_29)
			say("")
			say(gameforge[lang].main_quest._mission_sohan_30)
			pc.setqf("1101", 100)
			pc.setqf("1102", 100)
			pc.setqf("1103", 100)
			pc.setqf("1104", 100)
			pc.setqf("1105", 100)
			pc.setqf("1106", 100)
			pc.setqf("1107", 100)
			pc.setqf("1151", 75)
			pc.setqf("1152", 75)
			pc.setqf("1153", 75)
			pc.setqf("1154", 75)
			pc.setqf("1155", 75)
			pc.setqf("1156", 75)
			pc.setqf("1157", 75)
			pc.setqf("8011", 15)
			pc.setqf("8013", 15)
			set_state(gotomission1)
		end
	end
	
	state gotomission1 begin
		when letter begin
			send_letter(gameforge[pc.get_language()].main_quest._black_sohan_mission_II)
		end
			
		when button or info begin
			local lang = pc.get_language()
			say_title(string.format("%s:", gameforge[lang].main_quest._black_sohan_mission_II))
			say("")
			say(gameforge[lang].main_quest._37)
			say(string.format("- %d %s,", pc.getqf("1101"), mob_name(1101)))
			say(string.format("- %d %s,", pc.getqf("1102"), mob_name(1102)))
			say(string.format("- %d %s,", pc.getqf("1103"), mob_name(1103)))
			say(string.format("- %d %s,", pc.getqf("1104"), mob_name(1104)))
			say(string.format("- %d %s,", pc.getqf("1105"), mob_name(1105)))
			say(string.format("- %d %s,", pc.getqf("1106"), mob_name(1106)))
			say(string.format("- %d %s,", pc.getqf("1107"), mob_name(1107)))
			say(string.format("- %d %s,", pc.getqf("1151"), mob_name(1151)))
			say(string.format("- %d %s,", pc.getqf("1152"), mob_name(1152)))
			say(string.format("- %d %s,", pc.getqf("1153"), mob_name(1153)))
			say(string.format("- %d %s,", pc.getqf("1154"), mob_name(1154)))
			say(string.format("- %d %s,", pc.getqf("1155"), mob_name(1155)))
			say(string.format("- %d %s,", pc.getqf("1156"), mob_name(1156)))
			say(string.format("- %d %s,", pc.getqf("1157"), mob_name(1157)))
			say(string.format("- %d %s,", pc.getqf("8011"), mob_name(8011)))
			say(string.format("- %d %s,", pc.getqf("8013"), mob_name(8013)))
		end

		when	1101.kill or
				1102.kill or
				1103.kill or
				1104.kill or
				1105.kill or
				1106.kill or
				1107.kill or
				1151.kill or
				1152.kill or
				1153.kill or
				1154.kill or
				1155.kill or
				1156.kill or
				1157.kill or
				8011.kill or
				8013.kill
				begin
			local n = string.format("%s", npc.get_race())
			local c = pc.getqf(n)
			if c > 0 then
				pc.setqf(n, c - 1)
			end
			local t =	pc.getqf("1101") + 
						pc.getqf("1102") + 
						pc.getqf("1103") + 
						pc.getqf("1104") + 
						pc.getqf("1105") + 
						pc.getqf("1106") + 
						pc.getqf("1107") +
						pc.getqf("1151") +
						pc.getqf("1152") +
						pc.getqf("1153") +
						pc.getqf("1154") +
						pc.getqf("1155") +
						pc.getqf("1156") +
						pc.getqf("1157") +
						pc.getqf("8011") +
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
				target.vid("__TARGET__", v, gameforge[lang].main_quest._black_sohan_mission_II_end)
			end
			send_letter(gameforge[lang].main_quest._black_sohan_mission_II_end)
		end

		when button or info begin
			local lang = pc.get_language()
			say_title(string.format("%s:", gameforge[lang].main_quest._black_sohan_mission_II_end))
			say("")
			say(gameforge[lang].main_quest._black_sohan_mission_end1_2)
			say(gameforge[lang].main_quest._mission_sohan_02)
			say(string.format(gameforge[lang].main_quest._50, mob_name(20409)))
		end

		when __TARGET__.target.click begin
			target.delete("__TARGET__")
			pc.give_item2(30094, 50)
			pc.give_item2(30095, 30)
			pc.give_item2(30096, 16)
			pc.give_item2(30279, 14)
			pc.give_item2(30277, 7)
			pc.give_item2(30284, 18)
			pc.give_item2(30271, 4)
			pc.give_item2(72310, 18)
			pc.give_item2(71094, 18)
			pc.give_item2(70401, 2)
			pc.give_item2(72320, 2)
			pc.give_item2(88960, 2)
			pc.give_item2(25041, 4)
			pc.give_item2(71084, 600)
			pc.give_exp2(50000000)
			--pc.change_gold(80000000)
			say_title(string.format("%s:", mob_name(20409)))
			say("")
			local lang = pc.get_language()
			say(string.format(gameforge[lang].main_quest._black_soul_mission_end3, pc.get_name()))
			say(gameforge[lang].main_quest._black_soul_mission_end4)
			say("")
			say_reward(gameforge[lang].main_quest._58)
			say_reward(gameforge[lang].main_quest._mission_sohan_reward_15)
			say_reward(gameforge[lang].main_quest._mission_sohan_reward_16)
			say_reward(gameforge[lang].main_quest._mission_sohan_reward_17)
			say_reward(gameforge[lang].main_quest._mission_sohan_reward_18)
			say_reward(gameforge[lang].main_quest._mission_sohan_reward_19)
			say_reward(gameforge[lang].main_quest._mission_sohan_reward_20)
			say_reward(gameforge[lang].main_quest._mission_sohan_reward_21)
			say_reward(gameforge[lang].main_quest._mission_sohan_reward_22)
			say_reward(gameforge[lang].main_quest._mission_sohan_reward_23)
			say_reward(gameforge[lang].main_quest._mission_sohan_reward_24)
			say_reward(gameforge[lang].main_quest._mission_sohan_reward_25)
			say_reward(gameforge[lang].main_quest._mission_sohan_reward_26)
			say_reward(gameforge[lang].main_quest._mission_sohan_reward_27)
			say_reward(gameforge[lang].main_quest._mission_sohan_reward_28)
			say_reward(gameforge[lang].main_quest._mission_orc_reward_exp60)
			pc.delqf("1101")
			pc.delqf("1102")
			pc.delqf("1103")
			pc.delqf("1104")
			pc.delqf("1105")
			pc.delqf("1106")
			pc.delqf("1107")
			pc.delqf("1151")
			pc.delqf("1152")
			pc.delqf("1153")
			pc.delqf("1154")
			pc.delqf("1155")
			pc.delqf("1156")
			pc.delqf("1157")
			pc.delqf("8011")
			pc.delqf("8013")
			set_quest_state("main_quest_lv60_Special", "run")
			set_state(__COMPLETE__)
			clear_letter()
		end
	end

	state __COMPLETE__ begin
	end
end

