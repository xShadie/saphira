quest main_quest_lv30 begin
	state start begin
	end

	state run begin
		when enter or login or levelup with pc.get_level() >= 30 begin
			set_state(gototeacher)
		end
	end

	state gototeacher begin
		when letter begin
			local lang = pc.get_language()
			local v = find_npc_by_vnum(20354)
			if v != 0 then
				target.vid("__TARGET__", v, gameforge[lang].main_quest.title_8)
			end
			
			send_letter(gameforge[lang].main_quest.title_8)
		end

		when button or info begin
			local lang = pc.get_language()
			say_title(string.format("%s:", gameforge[lang].main_quest.title_8))
			say("")
			say(string.format(gameforge[lang].main_quest._1, mob_name(20354)))
			say(gameforge[lang].main_quest._2)
		end

		when __TARGET__.target.click begin
			target.delete("__TARGET__")
			local lang = pc.get_language()
			say_title(string.format("%s:", mob_name(20354)))
			say("")
			say(gameforge[lang].main_quest._city2_06)
			say(gameforge[lang].main_quest._city2_07)
			say(gameforge[lang].main_quest._city2_08)
			say("")
			say(gameforge[lang].main_quest._city2_09)
			say("")
			say(gameforge[lang].main_quest._city2_10)
			pc.setqf("431", 40)
			pc.setqf("432", 40)
			pc.setqf("433", 40)
			pc.setqf("434", 40)
			pc.setqf("435", 30)
			pc.setqf("436", 30)
			pc.setqf("501", 20)
			pc.setqf("502", 20)
			pc.setqf("8006", 15)
			set_state(gotomission)
		end
	end

	state gotomission begin
		when letter begin
			send_letter(gameforge[pc.get_language()].main_quest.subtitle_8)
		end

		when button or info begin
			local lang = pc.get_language()
			say_title(string.format("%s:", gameforge[lang].main_quest.subtitle_8))
			say("")
			say(gameforge[lang].main_quest._37)
			say(string.format("- %d %s,", pc.getqf("431"), mob_name(431)))
			say(string.format("- %d %s,", pc.getqf("432"), mob_name(432)))
			say(string.format("- %d %s,", pc.getqf("433"), mob_name(433)))
			say(string.format("- %d %s,", pc.getqf("434"), mob_name(434)))
			say(string.format("- %d %s,", pc.getqf("435"), mob_name(435)))
			say(string.format("- %d %s,", pc.getqf("436"), mob_name(436)))
			say(string.format("- %d %s,", pc.getqf("501"), mob_name(501)))
			say(string.format("- %d %s,", pc.getqf("502"), mob_name(502)))
			say(string.format("- %d %s,", pc.getqf("8006"), mob_name(8006)))
		end

		when 431.kill or 432.kill or 433.kill or 434.kill or 435.kill or 436.kill or 501.kill or 502.kill or 8006.kill begin
			local n = string.format("%s", npc.get_race())
			local c = pc.getqf(n)
			if c > 0 then
				pc.setqf(n, c - 1)
			end
			
			local t = pc.getqf("431") + pc.getqf("432") + pc.getqf("433") + pc.getqf("434") + pc.getqf("435") + pc.getqf("436") + pc.getqf("501") + pc.getqf("502") + pc.getqf("8006")
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
				target.vid("__TARGET__", v, gameforge[lang].main_quest.donetitle_8)
			end
			
			send_letter(gameforge[lang].main_quest.donetitle_8)
		end

		when button or info begin
			local lang = pc.get_language()
			say_title(string.format("%s:", gameforge[lang].main_quest.donetitle_8))
			say("")
			say(gameforge[lang].main_quest._49)
			say(string.format(gameforge[lang].main_quest._50, mob_name(20354)))
		end

		when __TARGET__.target.click begin
			target.delete("__TARGET__")
			pc.give_item2(30094, 15)
			pc.give_item2(30095, 6)
			pc.give_item2(30096, 3)
			pc.give_item2(30279, 2)
			pc.give_item2(30284, 4)
			pc.give_item2(72310, 4)
			pc.give_item2(71094, 4)
			pc.give_item2(88960, 1)
			pc.give_item2(71084, 300)
			pc.give_exp2(2500000)
			--pc.change_gold(80000000)
			say_title(string.format("%s:", mob_name(20354)))
			say("")
			local lang = pc.get_language()
			say(string.format(gameforge[lang].main_quest._51, pc.get_name()))
			say(gameforge[lang].main_quest._52)
			say("")
			say_reward(gameforge[lang].main_quest._58)
			say_reward(gameforge[lang].main_quest._city2_reward_05)
			say_reward(gameforge[lang].main_quest._city2_reward_06)
			say_reward(gameforge[lang].main_quest._city2_reward_07)
			say_reward(gameforge[lang].main_quest._city2_reward_08)
			say_reward(gameforge[lang].main_quest._city2_reward_09)
			say_reward(gameforge[lang].main_quest._city2_reward_10)
			say_reward(gameforge[lang].main_quest._city2_reward_11)
			say_reward(gameforge[lang].main_quest._city2_reward_12)
			say_reward(gameforge[lang].main_quest._city2_reward_13)
			say_reward(gameforge[lang].main_quest._mission_orc_reward_exp30)
			--say_reward(gameforge[lang].main_quest._70)
			pc.delqf("431")
			pc.delqf("432")
			pc.delqf("433")
			pc.delqf("434")
			pc.delqf("435")
			pc.delqf("436")
			pc.delqf("501")
			pc.delqf("502")
			pc.delqf("8006")
			set_quest_state("main_quest_lv35", "run")
			set_state(__COMPLETE__)
			clear_letter()
		end
	end

	state __COMPLETE__ begin
	end
end

