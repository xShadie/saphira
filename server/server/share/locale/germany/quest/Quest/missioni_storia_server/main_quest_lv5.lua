quest main_quest_lv5 begin
	state start begin
	end

	state run begin
		when enter or login or levelup with pc.get_level() >= 5 begin
			set_state(gototeacher)
		end
	end

	state gototeacher begin
		when letter begin
			local lang = pc.get_language()
			local v = find_npc_by_vnum(20354)
			if v != 0 then
				target.vid("__TARGET__", v, gameforge[lang].main_quest.title_2)
			end
			
			send_letter(gameforge[lang].main_quest.title_2)
		end

		when button or info begin
			local lang = pc.get_language()
			say_title(string.format("%s:", gameforge[lang].main_quest.title_2))
			say("")
			say(string.format(gameforge[lang].main_quest._1, mob_name(20354)))
			say(gameforge[lang].main_quest._2)
		end

		when __TARGET__.target.click begin
			target.delete("__TARGET__")
			local lang = pc.get_language()
			say_title(string.format("%s:", mob_name(20354)))
			say("")
			say(gameforge[lang].main_quest._11)
			say(gameforge[lang].main_quest._12)
			say("")
			say(gameforge[lang].main_quest._13)
			say(gameforge[lang].main_quest._14)
			pc.setqf("105", 15)
			pc.setqf("175", 15)
			pc.setqf("106", 10)
			pc.setqf("176", 10)
			pc.setqf("107", 8)
			pc.setqf("177", 8)
			pc.setqf("8001", 5)
			set_state(gotomission)
		end
	end

	state gotomission begin
		when letter begin
			send_letter(gameforge[pc.get_language()].main_quest.subtitle_2)
		end

		when button or info begin
			local lang = pc.get_language()
			say_title(string.format("%s:", gameforge[lang].main_quest.subtitle_2))
			say("")
			say(gameforge[lang].main_quest._37)
			say(string.format("- %d %s,", pc.getqf("105"), mob_name(105)))
			say(string.format("- %d %s,", pc.getqf("175"), mob_name(175)))
			say(string.format("- %d %s,", pc.getqf("106"), mob_name(106)))
			say(string.format("- %d %s,", pc.getqf("176"), mob_name(176)))
			say(string.format("- %d %s.", pc.getqf("107"), mob_name(107)))
			say(string.format("- %d %s.", pc.getqf("177"), mob_name(177)))
			say(string.format("- %d %s.", pc.getqf("8001"), mob_name(8001)))
		end

		when 105.kill or 175.kill or 106.kill or 176.kill or 107.kill or 177.kill or 8001.kill begin
			local n = string.format("%s", npc.get_race())
			local c = pc.getqf(n)
			if c > 0 then
				pc.setqf(n, c - 1)
			end
			
			local t = pc.getqf("105") + pc.getqf("175") + pc.getqf("106") + pc.getqf("176") + pc.getqf("107") + pc.getqf("177") + pc.getqf("8001")
			if t == 0 then
				set_state(gotoreward)
			end
		end
	end

	state gotoreward begin
		when letter begin
			local lang = pc.get_language()
			local v = find_npc_by_vnum(20354)
			if v != 0 then
				target.vid("__TARGET__", v, gameforge[lang].main_quest.donetitle_2)
			end
			
			send_letter(gameforge[lang].main_quest.donetitle_2)
		end

		when button or info begin
			local lang = pc.get_language()
			say_title(string.format("%s:", gameforge[lang].main_quest.donetitle_1))
			say("")
			say(gameforge[lang].main_quest._49)
			say(string.format(gameforge[lang].main_quest._50, mob_name(20354)))
		end

		when __TARGET__.target.click begin
			target.delete("__TARGET__")
			pc.give_item2(30094, 3)
			pc.give_item2(71084, 80)
			pc.give_exp2(5000)
			--pc.change_gold(20000000)
			say_title(string.format("%s:", mob_name(20354)))
			say("")
			local lang = pc.get_language()
			say(string.format(gameforge[lang].main_quest._51, pc.get_name()))
			say(gameforge[lang].main_quest._52)
			say("")
			say_reward(gameforge[lang].main_quest._58)
			say_reward(gameforge[lang].main_quest._62)
			say_reward(gameforge[lang].main_quest._63)
			say_reward(gameforge[lang].main_quest._mission_orc_reward_exp5)
			--say_reward(gameforge[lang].main_quest._64)
			pc.delqf("105")
			pc.delqf("175")
			pc.delqf("106")
			pc.delqf("176")
			pc.delqf("107")
			pc.delqf("177")
			pc.delqf("8001")
			set_quest_state("main_quest_lv10", "run")
			set_state(__COMPLETE__)
			clear_letter()
		end
	end

	state __COMPLETE__ begin
	end
end

