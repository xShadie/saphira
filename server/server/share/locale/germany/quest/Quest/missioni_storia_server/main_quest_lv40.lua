quest main_quest_lv40 begin
	state start begin
	end

	state run begin
		when enter or login or levelup with pc.get_level() >= 40 begin
			set_state(gototeacher)
		end
	end

	state gototeacher begin
		when letter begin
			local lang = pc.get_language()
			local v = find_npc_by_vnum(20354)
			if v != 0 then
				target.vid("__TARGET__", v, gameforge[lang].main_quest.title_10)
			end
			
			send_letter(gameforge[lang].main_quest.title_10)
		end

		when button or info begin
			local lang = pc.get_language()
			say_title(string.format("%s:", gameforge[lang].main_quest.title_10))
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
			pc.setqf("551", 50)
			pc.setqf("552", 50)
			pc.setqf("553", 50)
			pc.setqf("554", 50)
			pc.setqf("531", 8)
			pc.setqf("532", 8)
			pc.setqf("533", 8)
			pc.setqf("534", 8)
			pc.setqf("8006", 15)
			pc.setqf("8007", 15)
			set_state(gotomission)
		end
	end

	state gotomission begin
		when letter begin
			send_letter(gameforge[pc.get_language()].main_quest.subtitle_10)
		end

		when button or info begin
			local lang = pc.get_language()
			say_title(string.format("%s:", gameforge[lang].main_quest.subtitle_10))
			say("")
			say(gameforge[lang].main_quest._37)
			say(string.format("- %d %s,", pc.getqf("551"), mob_name(551)))
			say(string.format("- %d %s,", pc.getqf("552"), mob_name(552)))
			say(string.format("- %d %s,", pc.getqf("553"), mob_name(553)))
			say(string.format("- %d %s,", pc.getqf("554"), mob_name(554)))
			say(string.format("- %d %s,", pc.getqf("531"), mob_name(531)))
			say(string.format("- %d %s,", pc.getqf("532"), mob_name(532)))
			say(string.format("- %d %s,", pc.getqf("533"), mob_name(533)))
			say(string.format("- %d %s,", pc.getqf("534"), mob_name(534)))
			say(string.format("- %d %s,", pc.getqf("8006"), mob_name(8006)))
			say(string.format("- %d %s,", pc.getqf("8007"), mob_name(8007)))
		end

		when	551.kill or
				552.kill or
				553.kill or
				554.kill or
				531.kill or
				532.kill or
				533.kill or
				534.kill or
				8006.kill or
				8007.kill begin
			local n = string.format("%s", npc.get_race())
			local c = pc.getqf(n)
			if c > 0 then
				pc.setqf(n, c - 1)
			end
			
			local t =	pc.getqf("551") +
						pc.getqf("552") +
						pc.getqf("553") +
						pc.getqf("554") +
						pc.getqf("531") +
						pc.getqf("532") +
						pc.getqf("533") +
						pc.getqf("534") +
						pc.getqf("8006") +
						pc.getqf("8007")
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
				target.vid("__TARGET__", v, gameforge[lang].main_quest.donetitle_10)
			end
			
			send_letter(gameforge[lang].main_quest.donetitle_10)
		end

		when button or info begin
			local lang = pc.get_language()
			say_title(string.format("%s:", gameforge[lang].main_quest.donetitle_10))
			say("")
			say(gameforge[lang].main_quest._49)
			say(string.format(gameforge[lang].main_quest._50, mob_name(20354)))
		end

		when __TARGET__.target.click begin
			target.delete("__TARGET__")
			pc.give_item2(30094, 25)
			pc.give_item2(30095, 12)
			pc.give_item2(30096, 6)
			pc.give_item2(30279, 6)
			pc.give_item2(30284, 8)
			pc.give_item2(72310, 8)
			pc.give_item2(71094, 8)
			pc.give_item2(88960, 2)
			pc.give_item2(71084, 400)
			pc.give_exp2(7000000)
			--pc.change_gold(80000000)
			say_title(string.format("%s:", mob_name(20354)))
			say("")
			local lang = pc.get_language()
			say(string.format(gameforge[lang].main_quest._51, pc.get_name()))
			say(gameforge[lang].main_quest._52)
			say("")
			say_reward(gameforge[lang].main_quest._58)
			say_reward(gameforge[lang].main_quest._city2_reward_23)
			say_reward(gameforge[lang].main_quest._city2_reward_24)
			say_reward(gameforge[lang].main_quest._city2_reward_25)
			say_reward(gameforge[lang].main_quest._city2_reward_26)
			say_reward(gameforge[lang].main_quest._city2_reward_27)
			say_reward(gameforge[lang].main_quest._city2_reward_28)
			say_reward(gameforge[lang].main_quest._city2_reward_29)
			say_reward(gameforge[lang].main_quest._city2_reward_30)
			say_reward(gameforge[lang].main_quest._city2_reward_31)
			say_reward(gameforge[lang].main_quest._mission_orc_reward_exp40)
			--say_reward(gameforge[lang].main_quest._70)
			pc.delqf("551")
			pc.delqf("552")
			pc.delqf("553")
			pc.delqf("554")
			pc.delqf("531")
			pc.delqf("532")
			pc.delqf("533")
			pc.delqf("534")
			pc.delqf("8006")
			pc.delqf("8007")
			set_quest_state("main_quest_lv40_Jefes", "run")
			set_state(__COMPLETE__)
			clear_letter()
		end
	end

	state __COMPLETE__ begin
	end
end

