quest main_quest_lv25 begin
	state start begin
	end

	state run begin
		when enter or login or levelup with pc.get_level() >= 25 begin
			set_state(gototeacher)
		end
	end

	state gototeacher begin
		when letter begin
			local lang = pc.get_language()
			local v = find_npc_by_vnum(20354)
			if v != 0 then
				target.vid("__TARGET__", v, gameforge[lang].main_quest.title_7)
			end
			
			send_letter(gameforge[lang].main_quest.title_7)
		end

		when button or info begin
			local lang = pc.get_language()
			say_title(string.format("%s:", gameforge[lang].main_quest.title_7))
			say("")
			say(string.format(gameforge[lang].main_quest._1, mob_name(20354)))
			say(gameforge[lang].main_quest._2)
		end

		when __TARGET__.target.click begin
			target.delete("__TARGET__")
			local lang = pc.get_language()
			say_title(string.format("%s:", mob_name(20354)))
			say("")
			say(gameforge[lang].main_quest._city2_01)
			say(gameforge[lang].main_quest._city2_02)
			say("")
			say(gameforge[lang].main_quest._city2_03)
			say("")
			say(gameforge[lang].main_quest._city2_04)
			say("")
			say(gameforge[lang].main_quest._city2_05)
			pc.setqf("401", 30)
			pc.setqf("402", 30)
			pc.setqf("403", 30)
			pc.setqf("404", 30)
			pc.setqf("405", 20)
			pc.setqf("406", 15)
			pc.setqf("8005", 10)
			set_state(gotomission)
		end
	end

	state gotomission begin
		when letter begin
			send_letter(gameforge[pc.get_language()].main_quest.subtitle_7)
		end

		when button or info begin
			local lang = pc.get_language()
			say_title(string.format("%s:", gameforge[lang].main_quest.subtitle_7))
			say("")
			say(gameforge[lang].main_quest._37)
			say(string.format("- %d %s,", pc.getqf("401"), mob_name(401)))
			say(string.format("- %d %s,", pc.getqf("402"), mob_name(402)))
			say(string.format("- %d %s,", pc.getqf("403"), mob_name(403)))
			say(string.format("- %d %s,", pc.getqf("404"), mob_name(404)))
			say(string.format("- %d %s,", pc.getqf("405"), mob_name(405)))
			say(string.format("- %d %s,", pc.getqf("406"), mob_name(406)))
			say(string.format("- %d %s,", pc.getqf("8005"), mob_name(8005)))
		end

		when 401.kill or 402.kill or 403.kill or 404.kill or 405.kill or 406.kill or 8005.kill begin
			local n = string.format("%s", npc.get_race())
			local c = pc.getqf(n)
			if c > 0 then
				pc.setqf(n, c - 1)
			end
			
			local t = pc.getqf("401") + pc.getqf("402") + pc.getqf("403") + pc.getqf("404") + pc.getqf("405") + pc.getqf("406") + pc.getqf("8005")
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
				target.vid("__TARGET__", v, gameforge[lang].main_quest.donetitle_7)
			end
			
			send_letter(gameforge[lang].main_quest.donetitle_7)
		end

		when button or info begin
			local lang = pc.get_language()
			say_title(string.format("%s:", gameforge[lang].main_quest.donetitle_7))
			say("")
			say(gameforge[lang].main_quest._49)
			say(string.format(gameforge[lang].main_quest._50, mob_name(20354)))
		end

		when __TARGET__.target.click begin
			target.delete("__TARGET__")
			pc.give_item2(30094, 12)
			pc.give_item2(30095, 4)
			pc.give_item2(30096, 2)
			pc.give_item2(71084, 250)
			pc.give_exp2(1200000)
			--pc.change_gold(80000000)
			say_title(string.format("%s:", mob_name(20354)))
			say("")
			local lang = pc.get_language()
			say(string.format(gameforge[lang].main_quest._51, pc.get_name()))
			say(gameforge[lang].main_quest._52)
			say("")
			say_reward(gameforge[lang].main_quest._58)
			say_reward(gameforge[lang].main_quest._city2_reward_01)
			say_reward(gameforge[lang].main_quest._city2_reward_02)
			say_reward(gameforge[lang].main_quest._city2_reward_03)
			say_reward(gameforge[lang].main_quest._city2_reward_04)
			say_reward(gameforge[lang].main_quest._mission_orc_reward_exp25)
			--say_reward(gameforge[lang].main_quest._70)
			pc.delqf("401")
			pc.delqf("402")
			pc.delqf("403")
			pc.delqf("404")
			pc.delqf("405")
			pc.delqf("406")
			pc.delqf("8005")
			set_quest_state("main_quest_lv30", "run")
			set_state(__COMPLETE__)
			clear_letter()
		end
	end

	state __COMPLETE__ begin
	end
end

