quest main_quest_lv1 begin
	state start begin
		when login or levelup with pc.get_level() >= 1 begin
			set_state(gototeacher)
		end
	end

	state gototeacher begin
		when letter begin
			local lang = pc.get_language()
			local v = find_npc_by_vnum(20354)
			if v != 0 then
				target.vid("__TARGET__", v, gameforge[lang].main_quest.title_1)
			end
			
			send_letter(gameforge[lang].main_quest.title_1)
		end

		when button or info begin
			local lang = pc.get_language()
			say_title(string.format("%s:", gameforge[lang].main_quest.title_1))
			say("")
			say(string.format(gameforge[lang].main_quest._1, mob_name(20354)))
			say(gameforge[lang].main_quest._2)
		end

		when __TARGET__.target.click begin
			target.delete("__TARGET__")
			local lang = pc.get_language()
			say_title(string.format("%s:", mob_name(20354)))
			say("")
			say(string.format(gameforge[lang].main_quest._3, pc.get_name()))
			say(gameforge[lang].main_quest._4)
			say("")
			say(gameforge[lang].main_quest._5)
			say(gameforge[lang].main_quest._6)
			say(gameforge[lang].main_quest._7)
			say(gameforge[lang].main_quest._8)
			say("")
			say(gameforge[lang].main_quest._9)
			say(gameforge[lang].main_quest._9_2)
			say(string.format(gameforge[lang].main_quest._10, mob_name(8512)))
			say(gameforge[lang].main_quest._9)
			say(gameforge[lang].main_quest._9_2)
			pc.setqf("101", 15)
			pc.setqf("171", 15)
			pc.setqf("102", 10)
			pc.setqf("172", 10)
			pc.setqf("103", 8)
			pc.setqf("173", 8)
			pc.setqf("104", 8)
			pc.setqf("174", 8)
			set_state(gotomission)
		end
	end

	state gotomission begin
		when letter begin
			send_letter(gameforge[pc.get_language()].main_quest.subtitle_1)
		end

		when button or info begin
			local lang = pc.get_language()
			say_title(string.format("%s:", gameforge[lang].main_quest.subtitle_1))
			say("")
			say(gameforge[lang].main_quest._37)
			say(string.format("- %d %s,", pc.getqf("101"), mob_name(101)))
			say(string.format("- %d %s,", pc.getqf("171"), mob_name(171)))
			say(string.format("- %d %s,", pc.getqf("102"), mob_name(102)))
			say(string.format("- %d %s,", pc.getqf("172"), mob_name(172)))
			say(string.format("- %d %s,", pc.getqf("103"), mob_name(103)))
			say(string.format("- %d %s.", pc.getqf("173"), mob_name(173)))
			say(string.format("- %d %s.", pc.getqf("104"), mob_name(104)))
			say(string.format("- %d %s.", pc.getqf("174"), mob_name(174)))
		end

		when 101.kill or 171.kill or 102.kill or 172.kill or 103.kill or 173.kill or 104.kill or 174.kill begin
			local n = string.format("%s", npc.get_race())
			local c = pc.getqf(n)
			if c > 0 then
				pc.setqf(n, c - 1)
			end
			
			local t = pc.getqf("101") + pc.getqf("171") + pc.getqf("102") + pc.getqf("172") + pc.getqf("103") + pc.getqf("173") + pc.getqf("104") + pc.getqf("174")
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
				target.vid("__TARGET__", v, gameforge[lang].main_quest.donetitle_1)
			end
			
			send_letter(gameforge[lang].main_quest.donetitle_1)
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
			pc.give_item2(30094, 2)
			pc.give_item2(71084, 50)
			pc.give_exp2(1000)
			--pc.change_gold(5000000)
			say_title(string.format("%s:", mob_name(20354)))
			say("")
			local lang = pc.get_language()
			say(string.format(gameforge[lang].main_quest._51, pc.get_name()))
			say(gameforge[lang].main_quest._52)
			say("")
			say_reward(gameforge[lang].main_quest._58)
			say_reward(gameforge[lang].main_quest._59)
			say_reward(gameforge[lang].main_quest._60)
			say_reward(gameforge[lang].main_quest._mission_orc_reward_exp1)
			--say_reward(gameforge[lang].main_quest._61)
			pc.delqf("101")
			pc.delqf("171")
			pc.delqf("102")
			pc.delqf("172")
			pc.delqf("103")
			pc.delqf("173")
			pc.delqf("104")
			pc.delqf("174")
			set_quest_state("main_quest_lv5", "run")
			set_state(__COMPLETE__)
			clear_letter()
		end
	end

	state __COMPLETE__ begin
	end
end

