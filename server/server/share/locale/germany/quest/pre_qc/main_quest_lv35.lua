quest main_quest_lv35 begin
	state start begin
	end

	state run begin
		when enter or login or levelup with pc.get_level() >= 35 begin
			set_state(gototeacher)
		end
	end

	state gototeacher begin
		when letter begin
			local lang = pc.get_language()
			local v = find_npc_by_vnum(20354)
			if v != 0 then
				target.vid("__TARGET__", v, gameforge[lang].main_quest.title_9)
			end
			
			send_letter(gameforge[lang].main_quest.title_9)
		end

		when button or info begin
			local lang = pc.get_language()
			say_title(string.format("%s:", gameforge[lang].main_quest.title_9))
			say("")
			say(string.format(gameforge[lang].main_quest._1, mob_name(20354)))
			say(gameforge[lang].main_quest._2)
		end

		when __TARGET__.target.click begin
			target.delete("__TARGET__")
			local lang = pc.get_language()
			say_title(string.format("%s:", mob_name(20354)))
			say("")
			say(gameforge[lang].main_quest._city2_11)
			say("")
			say(gameforge[lang].main_quest._city2_12)
			say("")
			say(gameforge[lang].main_quest._city2_13)
			say("")
			say(gameforge[lang].main_quest._city2_14)
			pc.setqf("451", 50)
			pc.setqf("452", 50)
			pc.setqf("453", 50)
			pc.setqf("454", 50)
			pc.setqf("455", 40)
			pc.setqf("456", 40)
			pc.setqf("503", 30)
			pc.setqf("504", 30)
			pc.setqf("8007", 15)
			set_state(gotomission)
		end
	end

	state gotomission begin
		when letter begin
			send_letter(gameforge[pc.get_language()].main_quest.subtitle_9)
		end

		when button or info begin
			local lang = pc.get_language()
			say_title(string.format("%s:", gameforge[lang].main_quest.subtitle_9))
			say("")
			say(gameforge[lang].main_quest._37)
			say(string.format("- %d %s,", pc.getqf("451"), mob_name(451)))
			say(string.format("- %d %s,", pc.getqf("452"), mob_name(452)))
			say(string.format("- %d %s,", pc.getqf("453"), mob_name(453)))
			say(string.format("- %d %s,", pc.getqf("454"), mob_name(454)))
			say(string.format("- %d %s,", pc.getqf("455"), mob_name(455)))
			say(string.format("- %d %s,", pc.getqf("456"), mob_name(456)))
			say(string.format("- %d %s,", pc.getqf("503"), mob_name(503)))
			say(string.format("- %d %s,", pc.getqf("504"), mob_name(504)))
			say(string.format("- %d %s,", pc.getqf("8007"), mob_name(8007)))
		end

		when 451.kill or 452.kill or 453.kill or 454.kill or 455.kill or 456.kill or 503.kill or 504.kill or 8007.kill begin
			local n = string.format("%s", npc.get_race())
			local c = pc.getqf(n)
			if c > 0 then
				pc.setqf(n, c - 1)
			end
			
			local t = pc.getqf("451") + pc.getqf("452") + pc.getqf("453") + pc.getqf("454") + pc.getqf("455") + pc.getqf("456") + pc.getqf("503") + pc.getqf("504") + pc.getqf("8007")
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
				target.vid("__TARGET__", v, gameforge[lang].main_quest.donetitle_9)
			end
			
			send_letter(gameforge[lang].main_quest.donetitle_9)
		end

		when button or info begin
			local lang = pc.get_language()
			say_title(string.format("%s:", gameforge[lang].main_quest.donetitle_9))
			say("")
			say(gameforge[lang].main_quest._49)
			say(string.format(gameforge[lang].main_quest._50, mob_name(20354)))
		end

		when __TARGET__.target.click begin
			target.delete("__TARGET__")
			pc.give_item2(30094, 20)
			pc.give_item2(30095, 10)
			pc.give_item2(30096, 5)
			pc.give_item2(30279, 4)
			pc.give_item2(30284, 6)
			pc.give_item2(72310, 6)
			pc.give_item2(71094, 6)
			pc.give_item2(88960, 2)
			pc.give_item2(71084, 350)
			pc.give_exp2(5000000)
			--pc.change_gold(80000000)
			say_title(string.format("%s:", mob_name(20354)))
			say("")
			local lang = pc.get_language()
			say(string.format(gameforge[lang].main_quest._51, pc.get_name()))
			say(gameforge[lang].main_quest._52)
			say("")
			say_reward(gameforge[lang].main_quest._58)
			say_reward(gameforge[lang].main_quest._city2_reward_14)
			say_reward(gameforge[lang].main_quest._city2_reward_15)
			say_reward(gameforge[lang].main_quest._city2_reward_16)
			say_reward(gameforge[lang].main_quest._city2_reward_17)
			say_reward(gameforge[lang].main_quest._city2_reward_18)
			say_reward(gameforge[lang].main_quest._city2_reward_19)
			say_reward(gameforge[lang].main_quest._city2_reward_20)
			say_reward(gameforge[lang].main_quest._city2_reward_21)
			say_reward(gameforge[lang].main_quest._city2_reward_22)
			say_reward(gameforge[lang].main_quest._mission_orc_reward_exp35)
			--say_reward(gameforge[lang].main_quest._70)
			pc.delqf("451")
			pc.delqf("452")
			pc.delqf("453")
			pc.delqf("454")
			pc.delqf("455")
			pc.delqf("456")
			pc.delqf("503")
			pc.delqf("504")
			pc.delqf("8006")
			set_quest_state("main_quest_lv40", "run")
			set_state(__COMPLETE__)
			clear_letter()
		end
	end

	state __COMPLETE__ begin
	end
end

