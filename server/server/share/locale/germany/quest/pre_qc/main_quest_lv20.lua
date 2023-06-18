quest main_quest_lv20 begin
	state start begin
	end

	state run begin
		when enter or login or levelup with pc.get_level() >= 20 begin
			set_state(gototeacher)
		end
	end

	state gototeacher begin
		when letter begin
			local lang = pc.get_language()
			local v = find_npc_by_vnum(20354)
			if v != 0 then
				target.vid("__TARGET__", v, gameforge[lang].main_quest.title_5)
			end
			
			send_letter(gameforge[lang].main_quest.title_5)
		end

		when button or info begin
			local lang = pc.get_language()
			say_title(string.format("%s:", gameforge[lang].main_quest.title_5))
			say("")
			say(string.format(gameforge[lang].main_quest._1, mob_name(20354)))
			say(gameforge[lang].main_quest._2)
		end

		when __TARGET__.target.click begin
			target.delete("__TARGET__")
			local lang = pc.get_language()
			say_title(string.format("%s:", mob_name(20354)))
			say("")
			say(gameforge[lang].main_quest._23)
			say(gameforge[lang].main_quest._24)
			say("")
			say(gameforge[lang].main_quest._25)
			pc.setqf("114", 20)
			pc.setqf("184", 20)
			pc.setqf("115", 15)
			pc.setqf("185", 15)
			pc.setqf("301", 15)
			pc.setqf("302", 15)
			pc.setqf("303", 10)
			pc.setqf("8004", 8)
			set_state(gotomission)
		end
	end

	state gotomission begin
		when letter begin
			send_letter(gameforge[pc.get_language()].main_quest.subtitle_5)
		end

		when button or info begin
			local lang = pc.get_language()
			say_title(string.format("%s:", gameforge[lang].main_quest.subtitle_5))
			say("")
			say(gameforge[lang].main_quest._37)
			say(string.format("- %d %s,", pc.getqf("114"), mob_name(114)))
			say(string.format("- %d %s,", pc.getqf("184"), mob_name(184)))
			say(string.format("- %d %s,", pc.getqf("115"), mob_name(115)))
			say(string.format("- %d %s,", pc.getqf("185"), mob_name(185)))
			say(string.format("- %d %s,", pc.getqf("301"), mob_name(301)))
			say(string.format("- %d %s,", pc.getqf("302"), mob_name(302)))
			say(string.format("- %d %s,", pc.getqf("303"), mob_name(303)))
			say(string.format("- %d %s,", pc.getqf("8004"), mob_name(8004)))
		end

		when 114.kill or 184.kill or 115.kill or 185.kill or 301.kill or 302.kill or 303.kill or 8004.kill begin
			local n = string.format("%s", npc.get_race())
			local c = pc.getqf(n)
			if c > 0 then
				pc.setqf(n, c - 1)
			end
			
			local t = pc.getqf("114") + pc.getqf("184") + pc.getqf("115") + pc.getqf("185") + pc.getqf("301") + pc.getqf("302") + pc.getqf("303") + pc.getqf("8004")
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
				target.vid("__TARGET__", v, gameforge[lang].main_quest.donetitle_5)
			end
			
			send_letter(gameforge[lang].main_quest.donetitle_5)
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
			pc.give_item2(30094, 10)
			pc.give_item2(30095, 2)
			pc.give_item2(71084, 200)
			pc.give_exp2(300000)
			--pc.change_gold(80000000)
			say_title(string.format("%s:", mob_name(20354)))
			say("")
			local lang = pc.get_language()
			say(string.format(gameforge[lang].main_quest._51, pc.get_name()))
			say(gameforge[lang].main_quest._52)
			say("")
			say_reward(gameforge[lang].main_quest._58)
			say_reward(gameforge[lang].main_quest._69)
			say_reward(gameforge[lang].main_quest._70)
			say_reward(gameforge[lang].main_quest._70_1)
			say_reward(gameforge[lang].main_quest._mission_orc_reward_exp20)
			--say_reward(gameforge[lang].main_quest._70)
			pc.delqf("114")
			pc.delqf("184")
			pc.delqf("115")
			pc.delqf("185")
			pc.delqf("301")
			pc.delqf("302")
			pc.delqf("303")
			pc.delqf("8004")
			set_quest_state("main_quest_lv20_Jefes", "run")
			set_state(__COMPLETE__)
			clear_letter()
		end
	end

	state __COMPLETE__ begin
	end
end

