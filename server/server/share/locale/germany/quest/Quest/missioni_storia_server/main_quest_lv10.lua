quest main_quest_lv10 begin
	state start begin
	end

	state run begin
		when enter or login or levelup with pc.get_level() >= 10 begin
			set_state(gototeacher)
		end
	end

	state gototeacher begin
		when letter begin
			local lang = pc.get_language()
			local v = find_npc_by_vnum(20354)
			if v != 0 then
				target.vid("__TARGET__", v, gameforge[lang].main_quest.title_3)
			end
			
			send_letter(gameforge[lang].main_quest.title_3)
		end

		when button or info begin
			local lang = pc.get_language()
			say_title(string.format("%s:", gameforge[lang].main_quest.title_3))
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
			say(gameforge[lang].main_quest._15)
			say(gameforge[lang].main_quest._16)
			pc.setqf("108", 15)
			pc.setqf("178", 15)
			pc.setqf("109", 10)
			pc.setqf("179", 10)
			pc.setqf("110", 10)
			pc.setqf("180", 10)
			pc.setqf("8002", 5)
			set_state(gotomission)
		end
	end
	
	state gotomission begin
		when letter begin
			send_letter(gameforge[pc.get_language()].main_quest.subtitle_3)
		end

		when button or info begin
			local lang = pc.get_language()
			say_title(string.format("%s:", gameforge[lang].main_quest.subtitle_3))
			say("")
			say(gameforge[lang].main_quest._37)
			say(string.format("- %d %s,", pc.getqf("108"), mob_name(108)))
			say(string.format("- %d %s,", pc.getqf("178"), mob_name(178)))
			say(string.format("- %d %s,", pc.getqf("109"), mob_name(109)))
			say(string.format("- %d %s,", pc.getqf("179"), mob_name(179)))
			say(string.format("- %d %s,", pc.getqf("110"), mob_name(110)))
			say(string.format("- %d %s,", pc.getqf("180"), mob_name(180)))
			say(string.format("- %d %s,", pc.getqf("8002"), mob_name(8002)))
		end

		when 108.kill or 178.kill or 109.kill or 179.kill or 110.kill or 180.kill or 8002.kill begin
			local n = string.format("%s", npc.get_race())
			local c = pc.getqf(n)
			if c > 0 then
				pc.setqf(n, c - 1)
			end
			
			local t = pc.getqf("108") + pc.getqf("178") + pc.getqf("109") + pc.getqf("179") + pc.getqf("110") + pc.getqf("180") + pc.getqf("8002")
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
				target.vid("__TARGET__", v, gameforge[lang].main_quest.donetitle_3)
			end
			
			send_letter(gameforge[lang].main_quest.donetitle_3)
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
			pc.give_item2(30094, 5)
			pc.give_item2(71084, 100)
			pc.give_exp2(40000)
			--pc.change_gold(40000000)
			say_title(string.format("%s:", mob_name(20354)))
			say("")
			local lang = pc.get_language()
			say(string.format(gameforge[lang].main_quest._51, pc.get_name()))
			say(gameforge[lang].main_quest._52)
			say("")
			say_reward(gameforge[lang].main_quest._58)
			say_reward(gameforge[lang].main_quest._65)
			say_reward(gameforge[lang].main_quest._66)
			say_reward(gameforge[lang].main_quest._mission_orc_reward_exp10)
			--say_reward(gameforge[lang].main_quest._67)
			pc.delqf("108")
			pc.delqf("178")
			pc.delqf("109")
			pc.delqf("179")
			pc.delqf("110")
			pc.delqf("180")
			pc.delqf("8002")
			set_quest_state("main_quest_lv15", "run")
			set_state(__COMPLETE__)
			clear_letter()
		end
	end

	state __COMPLETE__ begin
	end
end

