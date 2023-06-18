quest main_quest_lv65 begin
	state start begin
	end

	state run begin
		when enter or login or levelup with pc.get_level() >= 65 begin
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
			pc.setqf("2201", 125)
			pc.setqf("2202", 125)
			pc.setqf("2203", 125)
			pc.setqf("2204", 125)
			pc.setqf("2205", 125)
			pc.setqf("8014", 20)
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
			say(string.format("- %d %s,", pc.getqf("2201"), mob_name(2201)))
			say(string.format("- %d %s,", pc.getqf("2202"), mob_name(2202)))
			say(string.format("- %d %s,", pc.getqf("2203"), mob_name(2203)))
			say(string.format("- %d %s,", pc.getqf("2204"), mob_name(2204)))
			say(string.format("- %d %s,", pc.getqf("2205"), mob_name(2205)))
			say(string.format("- %d %s,", pc.getqf("8014"), mob_name(8014)))
		end

		when	2201.kill or
				2202.kill or
				2203.kill or
				2204.kill or
				2205.kill or
				8014.kill
				begin
			local n = string.format("%s", npc.get_race())
			local c = pc.getqf(n)
			if c > 0 then
				pc.setqf(n, c - 1)
			end
			local t =	pc.getqf("2201") + 
						pc.getqf("2202") + 
						pc.getqf("2203") + 
						pc.getqf("2204") + 
						pc.getqf("2205") +
						pc.getqf("8014")
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
			pc.give_item2(30094, 60)
			pc.give_item2(30095, 35)
			pc.give_item2(30096, 20)
			pc.give_item2(30279, 16)
			pc.give_item2(30277, 10)
			pc.give_item2(30284, 20)
			pc.give_item2(30271, 6)
			pc.give_item2(72310, 20)
			pc.give_item2(71094, 20)
			pc.give_item2(70401, 2)
			pc.give_item2(72320, 2)
			pc.give_item2(88960, 2)
			pc.give_item2(25041, 5)
			pc.give_item2(27992, 1)
			pc.give_item2(27993, 1)
			pc.give_item2(27994, 1)
			pc.give_item2(71084, 650)
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
			pc.delqf("2201")
			pc.delqf("2202")
			pc.delqf("2203")
			pc.delqf("2204")
			pc.delqf("2205")
			pc.delqf("8014")
			set_quest_state("main_quest_lv70", "run")
			set_state(__COMPLETE__)
			clear_letter()
		end
	end

	state __COMPLETE__ begin
	end
end

