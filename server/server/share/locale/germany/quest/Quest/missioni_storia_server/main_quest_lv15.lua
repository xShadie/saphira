quest main_quest_lv15 begin
	state start begin
	end

	state run begin
		when enter or login or levelup with pc.get_level() >= 15 begin
			set_state(gototeacher)
		end
	end

	state gototeacher begin
		when letter begin
			local lang = pc.get_language()
			local v = find_npc_by_vnum(20354)
			if v != 0 then
				target.vid("__TARGET__", v, gameforge[lang].main_quest.title_4)
			end
			
			send_letter(gameforge[lang].main_quest.title_4)
		end

		when button or info begin
			local lang = pc.get_language()
			say_title(string.format("%s:", gameforge[lang].main_quest.title_4))
			say("")
			say(string.format(gameforge[lang].main_quest._1, mob_name(20354)))
			say(gameforge[lang].main_quest._2)
		end

		when __TARGET__.target.click begin
			target.delete("__TARGET__")
			local lang = pc.get_language()
			say_title(string.format("%s:", mob_name(20354)))
			say("")
			say(gameforge[lang].main_quest._17)
			say(gameforge[lang].main_quest._18)
			say("")
			say(gameforge[lang].main_quest._19)
			say(gameforge[lang].main_quest._20)
			say(gameforge[lang].main_quest._21)
			say("")
			say(gameforge[lang].main_quest._22)
			pc.setqf("111", 20)
			pc.setqf("181", 20)
			pc.setqf("112", 15)
			pc.setqf("182", 15)
			pc.setqf("113", 15)
			pc.setqf("183", 15)
			pc.setqf("8003", 8)
			set_state(gotomission)
		end
	end
	
	state gotomission begin
		when letter begin
			send_letter(gameforge[pc.get_language()].main_quest.subtitle_4)
		end

		when button or info begin
			local lang = pc.get_language()
			say_title(string.format("%s:", gameforge[lang].main_quest.subtitle_4))
			say("")
			say(gameforge[lang].main_quest._37)
			say(string.format("- %d %s,", pc.getqf("111"), mob_name(111)))
			say(string.format("- %d %s,", pc.getqf("181"), mob_name(181)))
			say(string.format("- %d %s,", pc.getqf("112"), mob_name(112)))
			say(string.format("- %d %s,", pc.getqf("182"), mob_name(182)))
			say(string.format("- %d %s,", pc.getqf("113"), mob_name(113)))
			say(string.format("- %d %s.", pc.getqf("183"), mob_name(183)))
			say(string.format("- %d %s.", pc.getqf("8003"), mob_name(8003)))
		end

		when 111.kill or 181.kill or 112.kill or 182.kill or 113.kill or 183.kill or 8003.kill begin
			local n = string.format("%s", npc.get_race())
			local c = pc.getqf(n)
			if c > 0 then
				pc.setqf(n, c - 1)
			end
			
			local t = pc.getqf("111") + pc.getqf("181") + pc.getqf("112") + pc.getqf("182") + pc.getqf("113") + pc.getqf("183") + pc.getqf("8003")
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
				target.vid("__TARGET__", v, gameforge[lang].main_quest.donetitle_4)
			end
			
			send_letter(gameforge[lang].main_quest.donetitle_4)
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
			pc.give_item2(30094, 8)
			pc.give_item2(71084, 150)
			pc.give_exp2(150000)
			--pc.change_gold(60000000)
			say_title(string.format("%s:", mob_name(20354)))
			say("")
			local lang = pc.get_language()
			say(string.format(gameforge[lang].main_quest._51, pc.get_name()))
			say(gameforge[lang].main_quest._52)
			say("")
			say_reward(gameforge[lang].main_quest._58)
			say_reward(gameforge[lang].main_quest._67)
			say_reward(gameforge[lang].main_quest._68)
			say_reward(gameforge[lang].main_quest._mission_orc_reward_exp15)
			--say_reward(gameforge[lang].main_quest._81)
			pc.delqf("111")
			pc.delqf("181")
			pc.delqf("112")
			pc.delqf("182")
			pc.delqf("113")
			pc.delqf("183")
			pc.delqf("8003")
			set_quest_state("main_quest_lv20", "run")
			set_state(__COMPLETE__)
			clear_letter()
		end
	end

	state __COMPLETE__ begin
	end
end

