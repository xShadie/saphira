quest main_quest_lv45 begin
	state start begin
	end

	state run begin
		when enter or login or levelup with pc.get_level() >= 45 begin
			set_state(gototeacher)
		end
	end

	state gototeacher begin
		when letter begin
			local lang = pc.get_language()
			local v = find_npc_by_vnum(20356)
			if v != 0 then
				target.vid("__TARGET__", v, gameforge[lang].main_quest._black_soul_mission_I)
			end
			
			send_letter(gameforge[lang].main_quest._black_soul_mission_I)
		end

		when button or info begin
			local lang = pc.get_language()
			say_title(string.format("%s:", gameforge[lang].main_quest._black_soul_mission_I))
			say("")
			say(string.format(gameforge[lang].main_quest._1, mob_name(20356)))
			say(gameforge[lang].main_quest._mission_orc_01)
			say(gameforge[lang].main_quest._mission_orc_02)
		end

		when __TARGET__.target.click begin
			target.delete("__TARGET__")
			local lang = pc.get_language()
			say_title(string.format("%s:", mob_name(20356)))
			say("")
			say(gameforge[lang].main_quest._mission_orc_03)
			say("")
			say(gameforge[lang].main_quest._mission_orc_04)
			wait()
			say(gameforge[lang].main_quest._mission_orc_05)
			say(gameforge[lang].main_quest._mission_orc_06)
			say(gameforge[lang].main_quest._mission_orc_07)
			say(gameforge[lang].main_quest._mission_orc_08)
			say_item_vnum(30012)
			say(gameforge[lang].main_quest._mission_orc_09)
			set_state(gotomission1)
		end
	end

	state gotomission1 begin
		when letter begin
			send_letter(gameforge[pc.get_language()].main_quest._black_soul_mission_I)
		end
		when button or info begin
			local lang = pc.get_language()
			say_title(string.format("%s:", mob_name(20356)))
			say(gameforge[lang].main_quest._mission_orc_10)
			say(gameforge[lang].main_quest._mission_orc_11)
			say_item_vnum(30012)
			say("")
			say(gameforge[lang].main_quest._mission_orc_12)
		end
		when	701.kill or
				702.kill or 
				703.kill or 
				704.kill or 
				705.kill or 
				706.kill or 
				707.kill or 
				731.kill or 
				732.kill or 
				733.kill or 
				734.kill or 
				735.kill or 
				736.kill or 
				737.kill or 
				751.kill or 
				752.kill or 
				753.kill or 
				754.kill or 
				755.kill or 
				756.kill or 
				757.kill
				begin
			local lang = pc.get_language()
			local s = number(1, 1000)
			if s <= 5 then
				pc.give_item2(30012, 1)
				say(gameforge[lang].main_quest._mission_orc_13)
				say_item_vnum(30012)
				say(gameforge[lang].main_quest._mission_orc_14)
			end
		end
		when 20356.chat."Epazote" with pc.count_item(30012) > 0 begin
			local lang = pc.get_language()
			say_title(string.format("%s:", mob_name(20356)))
			say(gameforge[lang].main_quest._mission_orc_15)
			say("")
			say(gameforge[lang].main_quest._mission_orc_16)
			say("")
			say(gameforge[lang].main_quest._mission_orc_17)
			say("")
			say(gameforge[lang].main_quest._mission_orc_18)
			pc.remove_item("30012", 200)
			pc.setqf("601", 50)
			pc.setqf("602", 50)
			pc.setqf("603", 50)
			pc.setqf("604", 50)
			pc.setqf("631", 50)
			pc.setqf("632", 50)
			pc.setqf("633", 50)
			pc.setqf("634", 50)
			pc.setqf("635", 50)
			pc.setqf("8008", 15)
			set_state(gotomission2)
		end
	end
	state gotomission2 begin
		when letter begin
			send_letter(gameforge[pc.get_language()].main_quest._black_soul_mission_I)
		end
			
		when button or info begin
			local lang = pc.get_language()
			say_title(string.format("%s:", gameforge[lang].main_quest._black_soul_mission_I))
			say("")
			say(gameforge[lang].main_quest._37)
			say(string.format("- %d %s,", pc.getqf("601"), mob_name(601)))
			say(string.format("- %d %s,", pc.getqf("602"), mob_name(602)))
			say(string.format("- %d %s,", pc.getqf("603"), mob_name(603)))
			say(string.format("- %d %s,", pc.getqf("604"), mob_name(604)))
			say(string.format("- %d %s,", pc.getqf("631"), mob_name(631)))
			say(string.format("- %d %s,", pc.getqf("632"), mob_name(632)))
			say(string.format("- %d %s,", pc.getqf("633"), mob_name(633)))
			say(string.format("- %d %s,", pc.getqf("634"), mob_name(634)))
			say(string.format("- %d %s,", pc.getqf("635"), mob_name(635)))
			say(string.format("- %d %s,", pc.getqf("8008"), mob_name(8008)))
		end

		when	601.kill or
				602.kill or
				603.kill or
				604.kill or
				631.kill or
				632.kill or
				633.kill or
				634.kill or
				635.kill or
				8008.kill
				begin
			local n = string.format("%s", npc.get_race())
			local c = pc.getqf(n)
			if c > 0 then
				pc.setqf(n, c - 1)
			end
			local t =	pc.getqf("601") + 
						pc.getqf("602") + 
						pc.getqf("603") + 
						pc.getqf("604") + 
						pc.getqf("631") + 
						pc.getqf("632") + 
						pc.getqf("633") + 
						pc.getqf("634") + 
						pc.getqf("635") + 
						pc.getqf("8008")
			if t == 0 then
				set_state(gotoreward)
			end
		end
	end

	state gotoreward begin
		when letter begin
			local lang = pc.get_language()
			local v = find_npc_by_vnum(20356)
			if v != 0 then
				target.vid("__TARGET__", v, gameforge[lang].main_quest._black_soul_mission_I_end)
			end
			send_letter(gameforge[lang].main_quest._black_soul_mission_I_end)
		end

		when button or info begin
			local lang = pc.get_language()
			say_title(string.format("%s:", gameforge[lang].main_quest._black_soul_mission_I_end))
			say("")
			say(gameforge[lang].main_quest._black_soul_mission_end1)
			say(gameforge[lang].main_quest._black_soul_mission_end2)
			say(string.format(gameforge[lang].main_quest._50, mob_name(20356)))
		end

		when __TARGET__.target.click begin
			target.delete("__TARGET__")
			pc.give_item2(30094, 25)
			pc.give_item2(30095, 12)
			pc.give_item2(30096, 6)
			pc.give_item2(30279, 8)
			pc.give_item2(30284, 10)
			pc.give_item2(72310, 10)
			pc.give_item2(71094, 10)
			pc.give_item2(70401, 1)
			pc.give_item2(72320, 1)
			pc.give_item2(88960, 2)
			pc.give_item2(25041, 2)
			pc.give_item2(71084, 450)
			pc.give_exp2(15000000)
			--pc.change_gold(80000000)
			say_title(string.format("%s:", mob_name(20356)))
			say("")
			local lang = pc.get_language()
			say(string.format(gameforge[lang].main_quest._black_soul_mission_end3, pc.get_name()))
			say(gameforge[lang].main_quest._black_soul_mission_end4)
			say("")
			say_reward(gameforge[lang].main_quest._58)
			say_reward(gameforge[lang].main_quest._mission_orc_reward_01)
			say_reward(gameforge[lang].main_quest._mission_orc_reward_02)
			say_reward(gameforge[lang].main_quest._mission_orc_reward_03)
			say_reward(gameforge[lang].main_quest._mission_orc_reward_04)
			say_reward(gameforge[lang].main_quest._mission_orc_reward_05)
			say_reward(gameforge[lang].main_quest._mission_orc_reward_06)
			say_reward(gameforge[lang].main_quest._mission_orc_reward_07)
			say_reward(gameforge[lang].main_quest._mission_orc_reward_08)
			say_reward(gameforge[lang].main_quest._mission_orc_reward_09)
			say_reward(gameforge[lang].main_quest._mission_orc_reward_10)
			say_reward(gameforge[lang].main_quest._mission_orc_reward_11)
			say_reward(gameforge[lang].main_quest._mission_orc_reward_12)
			say_reward(gameforge[lang].main_quest._mission_orc_reward_exp45)
			pc.delqf("601")
			pc.delqf("602")
			pc.delqf("603")
			pc.delqf("604")
			pc.delqf("631")
			pc.delqf("632")
			pc.delqf("633")
			pc.delqf("634")
			pc.delqf("635")
			pc.delqf("8008")
			set_quest_state("main_quest_lv50", "run")
			set_state(__COMPLETE__)
			clear_letter()
		end
	end

	state __COMPLETE__ begin
	end
end

