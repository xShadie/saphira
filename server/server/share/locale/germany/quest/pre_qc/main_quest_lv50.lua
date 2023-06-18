quest main_quest_lv50 begin
	state start begin
	end

	state run begin
		when enter or login or levelup with pc.get_level() >= 50 begin
			set_state(gototeacher)
		end
	end

	state gototeacher begin
		when letter begin
			local lang = pc.get_language()
			local v = find_npc_by_vnum(20356)
			if v != 0 then
				target.vid("__TARGET__", v, gameforge[lang].main_quest._black_soul_mission_II)
			end
			send_letter(gameforge[lang].main_quest._black_soul_mission_II)
		end

		when button or info begin
			local lang = pc.get_language()
			say_title(string.format("%s:", gameforge[lang].main_quest._black_soul_mission_II))
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
			say(gameforge[lang].main_quest._mission_orc_19)
			say("")
			say(gameforge[lang].main_quest._mission_orc_20)
			wait()
			say(gameforge[lang].main_quest._mission_orc_21)
			say(gameforge[lang].main_quest._mission_orc_22)
			say(gameforge[lang].main_quest._mission_orc_23)
			say_item_vnum(30033)
			say(gameforge[lang].main_quest._mission_orc_09)
			set_state(gotomission1)
		end
	end

	state gotomission1 begin
		when letter begin
			send_letter(gameforge[pc.get_language()].main_quest._black_soul_mission_II)
		end
		when button or info begin
			local lang = pc.get_language()
			say_title(string.format("%s:", mob_name(20356)))
			say(gameforge[lang].main_quest._mission_orc_24)
			say(gameforge[lang].main_quest._mission_orc_23)
			say_item_vnum(30033)
			say("")
			say(gameforge[lang].main_quest._mission_orc_25)
		end
		when	731.kill or
				732.kill or 
				733.kill or 
				734.kill or 
				735.kill
				begin
			local lang = pc.get_language()
			local s = number(1, 1000)
			if s <= 5 then
				pc.give_item2(30033, 1)
				say(gameforge[lang].main_quest._mission_orc_26)
				say_item_vnum(30033)
				say(gameforge[lang].main_quest._mission_orc_27)
				say(gameforge[lang].main_quest._mission_orc_28)
			end
		end
		
		when 20353.take with item.get_vnum() == 30033 begin
			local lang = pc.get_language()
			say_item_vnum(30033)
			say(gameforge[lang].main_quest._mission_orc_29)
			pc.remove_item("30033", 200)
			set_state(gotomission2)
		end
	end
	state gotomission2 begin
		when letter begin
			local lang = pc.get_language()
			local v = find_npc_by_vnum(20356)
			if v != 0 then
				target.vid("__TARGET__", v, gameforge[lang].main_quest._black_soul_mission_II)
			end
			send_letter(gameforge[lang].main_quest._black_soul_mission_II)
		end
		when button or info begin
			local lang = pc.get_language()
			say_title(string.format("%s:", gameforge[lang].main_quest._black_soul_mission_II))
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
			say(gameforge[lang].main_quest._mission_orc_30)
			say("")
			say(gameforge[lang].main_quest._mission_orc_31)
			wait()
			say(gameforge[lang].main_quest._mission_orc_32)
			say_item_vnum(30147)
			say(gameforge[lang].main_quest._mission_orc_09)
			set_state(gotomission3)
		end
	end
	state gotomission3 begin
		when letter begin
			send_letter(gameforge[pc.get_language()].main_quest._black_soul_mission_II)
		end
		when button or info begin
			local lang = pc.get_language()
			say_title(string.format("%s:", mob_name(20356)))
			say(gameforge[lang].main_quest._mission_orc_35)
			say(gameforge[lang].main_quest._mission_orc_36)
			say_item_vnum(30147)
			say("")
			say(gameforge[lang].main_quest._mission_orc_33)
		end
		when	771.kill or
				772.kill or 
				773.kill or 
				774.kill or 
				775.kill or
				776.kill or
				777.kill
				begin
			local lang = pc.get_language()
			local s = number(1, 1000)
			if s <= 5 then
				pc.give_item2(30147, 1)
				say(gameforge[lang].main_quest._mission_orc_37)
				say_item_vnum(30147)
				say(gameforge[lang].main_quest._mission_orc_27)
				say(gameforge[lang].main_quest._mission_orc_28)
			end
		end
		
		when 20353.take with item.get_vnum() == 30147 begin
			local lang = pc.get_language()
			say_item_vnum(30147)
			say(gameforge[lang].main_quest._mission_orc_29)
			pc.remove_item("30147", 200)
			set_state(gotomission4)
		end
	end
	state gotomission4 begin
		when letter begin
			local lang = pc.get_language()
			local v = find_npc_by_vnum(20356)
			if v != 0 then
				target.vid("__TARGET__", v, gameforge[lang].main_quest._black_soul_mission_II)
			end
			send_letter(gameforge[lang].main_quest._black_soul_mission_II)
		end
		when button or info begin
			local lang = pc.get_language()
			say_title(string.format("%s:", gameforge[lang].main_quest._black_soul_mission_II))
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
			say(gameforge[lang].main_quest._mission_orc_30)
			say("")
			say(gameforge[lang].main_quest._mission_orc_38)
			say("")
			say(gameforge[lang].main_quest._mission_orc_18)
			pc.setqf("636", 75)
			pc.setqf("637", 75)
			pc.setqf("656", 75)
			pc.setqf("657", 75)
			pc.setqf("701", 50)
			pc.setqf("702", 50)
			pc.setqf("703", 50)
			pc.setqf("704", 50)
			pc.setqf("705", 50)
			pc.setqf("706", 50)
			pc.setqf("707", 50)
			pc.setqf("731", 50)
			pc.setqf("732", 50)
			pc.setqf("733", 50)
			pc.setqf("734", 50)
			pc.setqf("735", 50)
			pc.setqf("8008", 15)
			pc.setqf("8009", 15)
			set_state(gotomission5)
		end
	end
	state gotomission5 begin
		when letter begin
			send_letter(gameforge[pc.get_language()].main_quest._black_soul_mission_II)
		end
			
		when button or info begin
			local lang = pc.get_language()
			say_title(string.format("%s:", gameforge[lang].main_quest._black_soul_mission_II))
			say("")
			say(gameforge[lang].main_quest._37)
			say(string.format("- %d %s,", pc.getqf("636"), mob_name(636)))
			say(string.format("- %d %s,", pc.getqf("637"), mob_name(637)))
			say(string.format("- %d %s,", pc.getqf("656"), mob_name(656)))
			say(string.format("- %d %s,", pc.getqf("657"), mob_name(657)))
			say(string.format("- %d %s,", pc.getqf("701"), mob_name(701)))
			say(string.format("- %d %s,", pc.getqf("702"), mob_name(702)))
			say(string.format("- %d %s,", pc.getqf("703"), mob_name(703)))
			say(string.format("- %d %s,", pc.getqf("704"), mob_name(704)))
			say(string.format("- %d %s,", pc.getqf("705"), mob_name(705)))
			say(string.format("- %d %s,", pc.getqf("706"), mob_name(706)))
			say(string.format("- %d %s,", pc.getqf("707"), mob_name(707)))
			say(string.format("- %d %s,", pc.getqf("731"), mob_name(731)))
			say(string.format("- %d %s,", pc.getqf("732"), mob_name(732)))
			say(string.format("- %d %s,", pc.getqf("733"), mob_name(733)))
			say(string.format("- %d %s,", pc.getqf("734"), mob_name(734)))
			say(string.format("- %d %s,", pc.getqf("735"), mob_name(735)))
			say(string.format("- %d %s,", pc.getqf("8008"), mob_name(8008)))
			say(string.format("- %d %s,", pc.getqf("8009"), mob_name(8009)))
		end

		when	636.kill or
				637.kill or
				656.kill or
				657.kill or
				701.kill or
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
				8008.kill or
				8009.kill
				begin
			local n = string.format("%s", npc.get_race())
			local c = pc.getqf(n)
			if c > 0 then
				pc.setqf(n, c - 1)
			end
			local t =	pc.getqf("636") + 
						pc.getqf("637") + 
						pc.getqf("656") + 
						pc.getqf("657") + 
						pc.getqf("701") + 
						pc.getqf("702") + 
						pc.getqf("703") + 
						pc.getqf("704") + 
						pc.getqf("705") + 
						pc.getqf("706") + 
						pc.getqf("707") + 
						pc.getqf("731") + 
						pc.getqf("732") + 
						pc.getqf("733") + 
						pc.getqf("734") + 
						pc.getqf("735") + 
						pc.getqf("8008") +
						pc.getqf("8009")
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
				target.vid("__TARGET__", v, gameforge[lang].main_quest._black_soul_mission_II_end)
			end
			send_letter(gameforge[lang].main_quest._black_soul_mission_II_end)
		end

		when button or info begin
			local lang = pc.get_language()
			say_title(string.format("%s:", gameforge[lang].main_quest._black_soul_mission_II_end))
			say("")
			say(gameforge[lang].main_quest._black_soul_mission_end1_2)
			say(gameforge[lang].main_quest._black_soul_mission_end2)
			say(string.format(gameforge[lang].main_quest._50, mob_name(20356)))
		end

		when __TARGET__.target.click begin
			local lang = pc.get_language()
			target.delete("__TARGET__")
			say_title(string.format("%s:", mob_name(20356)))
			say(string.format(gameforge[lang].main_quest._black_soul_mission_end3, pc.get_name()))
			say(gameforge[lang].main_quest._black_soul_mission_end4)
			-- say("")
			-- say(gameforge[lang].main_quest._mission_orc_reward_choose)
			-- local rew_extra = select(	gameforge[lang].main_quest._mission_orc_reward_extra1 ,
										-- gameforge[lang].main_quest._mission_orc_reward_extra2 ,
										-- gameforge[lang].main_quest._mission_orc_reward_extra3)
				-- if rew_extra == 1 then
					-- pc.give_item2(18000, 1)
				-- elseif rew_extra == 2 then
					-- pc.give_item2(18010, 1)
				-- elseif rew_extra == 3 then
					-- pc.give_item2(18020, 1)
				-- end
			pc.give_item2(30094, 30)
			pc.give_item2(30095, 15)
			pc.give_item2(30096, 8)
			pc.give_item2(30279, 10)
			pc.give_item2(30277, 2)
			pc.give_item2(30284, 12)
			pc.give_item2(72310, 12)
			pc.give_item2(71094, 12)
			pc.give_item2(70401, 2)
			pc.give_item2(72320, 1)
			pc.give_item2(88960, 2)
			pc.give_item2(25041, 4)
			pc.give_item2(71084, 500)
			pc.give_item2(18006, 1)
			pc.give_exp2(20000000)
			--pc.change_gold(80000000)
			say_reward(gameforge[lang].main_quest._58)
			say_reward(gameforge[lang].main_quest._mission_orc_reward_13)
			say_reward(gameforge[lang].main_quest._mission_orc_reward_14)
			say_reward(gameforge[lang].main_quest._mission_orc_reward_15)
			say_reward(gameforge[lang].main_quest._mission_orc_reward_16)
			say_reward(gameforge[lang].main_quest._mission_orc_reward_17)
			say_reward(gameforge[lang].main_quest._mission_orc_reward_18)
			--say_reward(gameforge[lang].main_quest._mission_orc_reward_19)
			say_reward(gameforge[lang].main_quest._mission_orc_reward_20)
			say_reward(gameforge[lang].main_quest._mission_orc_reward_21)
			say_reward(gameforge[lang].main_quest._mission_orc_reward_22)
			say_reward(gameforge[lang].main_quest._mission_orc_reward_23)
			say_reward(gameforge[lang].main_quest._mission_orc_reward_24)
			say_reward(gameforge[lang].main_quest._mission_orc_reward_25)
			say_reward(gameforge[lang].main_quest._mission_orc_reward_26)
			say_reward(gameforge[lang].main_quest._mission_orc_reward_extra1)
			say_reward(gameforge[lang].main_quest._mission_orc_reward_exp50)
			pc.delqf("636")
			pc.delqf("637")
			pc.delqf("656")
			pc.delqf("657")
			pc.delqf("701")
			pc.delqf("702")
			pc.delqf("703")
			pc.delqf("704")
			pc.delqf("705")
			pc.delqf("706")
			pc.delqf("707")
			pc.delqf("731")
			pc.delqf("732")
			pc.delqf("733")
			pc.delqf("734")
			pc.delqf("735")
			pc.delqf("8008")
			pc.delqf("8009")
			set_quest_state("main_quest_lv50_Special", "run")
			set_state(__COMPLETE__)
			clear_letter()
		end
	end

	state __COMPLETE__ begin
	end
end

