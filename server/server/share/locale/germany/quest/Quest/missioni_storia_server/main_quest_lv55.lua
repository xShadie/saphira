quest main_quest_lv55 begin
	state start begin
	end

	state run begin
		when enter or login or levelup with pc.get_level() >= 55 begin
			set_state(gototeacher)
		end
	end

	state gototeacher begin
		when letter begin
			local lang = pc.get_language()
			local v = find_npc_by_vnum(20409)
			if v != 0 then
				target.vid("__TARGET__", v, gameforge[lang].main_quest._black_sohan_mission_I)
			end
			send_letter(gameforge[lang].main_quest._black_sohan_mission_I)
		end

		when button or info begin
			local lang = pc.get_language()
			say_title(string.format("%s:", gameforge[lang].main_quest._black_sohan_mission_I))
			say("")
			say(string.format(gameforge[lang].main_quest._1, mob_name(20409)))
			say(gameforge[lang].main_quest._mission_sohan_01)
			say(gameforge[lang].main_quest._mission_sohan_02)
		end

		when __TARGET__.target.click begin
			target.delete("__TARGET__")
			local lang = pc.get_language()
			say_title(string.format("%s:", mob_name(20409)))
			say("")
			say(gameforge[lang].main_quest._mission_sohan_03)
			say("")
			say(gameforge[lang].main_quest._mission_sohan_04)
			wait()
			say("")
			say(gameforge[lang].main_quest._mission_sohan_05)
			say(gameforge[lang].main_quest._mission_sohan_06)
			say_item_vnum(30048)
			say(gameforge[lang].main_quest._mission_orc_09)
			pc.setqf("duration",0)
			pc.setqf("collect_count",0)
			pc.setqf("drink_drug",0)
			set_state(collect_1)
		end
	end

	state collect_1 begin
		when letter begin
			send_letter(gameforge[pc.get_language()].main_quest._black_sohan_mission_I)
		end
		when button or info begin
			local lang = pc.get_language()
			say_title(string.format("%s:", mob_name(20409)))
			say(gameforge[lang].main_quest._mission_sohan_07)
			say(gameforge[lang].main_quest._mission_sohan_08)
			say_item_vnum(30048)
			say("")
			say(gameforge[lang].main_quest._mission_sohan_06)
			say_reward(string.format(gameforge[lang].main_quest._mission_sohan_08_2, pc.getqf("collect_count")))
		end
		when 71035.use begin
			local lang = pc.get_language()
			if get_time() < pc.getqf("duration") then
				say(gameforge[lang].main_quest._elixir_1)
				return
			end
			if pc.getqf("drink_drug")==1 then
				say(gameforge[lang].main_quest._elixir_2)
				return
			end
			if pc.count_item(30048)==0 then
				say_title(string.format("%s:", mob_name(20409)))
				say(gameforge[lang].main_quest._elixir_3)
				say_item_vnum(30048)
				return
			end
			
			pc.remove_item(71035, 1)
			pc.setqf("drink_drug",1)
		end

		when	1101.kill or
				1131.kill or
				1151.kill or
				1171.kill with pc.count_item(30048) <= 20 begin
			local lang = pc.get_language()
			local s = number(1, 100)
			if s <= 5 then
				pc.give_item2(30048, 1)
				if pc.count_item(30048) >= 20 then
					say(gameforge[lang].main_quest._mission_sohan_12_1)
					say_item_vnum(30048)
					say_reward(string.format(gameforge[lang].main_quest._mission_sohan_08_2, pc.getqf("collect_count")))
				end
			end 
		end

		when 20409.chat.gameforge[pc.get_language()].chat_npc_translate._mission55_1 with pc.count_item(30048) > 0 begin
		--when 20354.chat.gameforge[pc.get_language()].main_quest._106_3 with pc.count_item(30032) > 0 begin
			local lang = pc.get_language()
			--if get_time() > pc.getqf("duration") then
			if pc.count_item(30048) > 0 then
				say_title(string.format("%s:", mob_name(20409)))
				say(gameforge[lang].main_quest._mission_sohan_09)
				pc.remove_item("30048", 1)
				--pc.setqf("duration", get_time()+60*15) -- 15 minutos
				wait()
				local pass_percent
				if pc.getqf("drink_drug")==0 then
					pass_percent=60
				else
					pass_percent=100
				end
				local s = number(1,100)
				if s <= pass_percent then
					if pc.getqf("collect_count") < 19 then
						local index =pc.getqf("collect_count")+1
						pc.setqf("collect_count",index)
						say_title(string.format("%s:", mob_name(20409)))
						say(string.format(gameforge[lang].main_quest._mission_sohan_10, 20-pc.getqf("collect_count")))
						pc.setqf("drink_drug",0)
						return
					end
					say_title(string.format("%s:", mob_name(20409)))
					say(gameforge[lang].main_quest._mission_sohan_coll_finish_1)
					pc.remove_item("30048", 200) -- Remove all
					pc.setqf("collect_count",0)
					pc.setqf("drink_drug",0)
					pc.setqf("duration",0)
					set_state(collect_2)
					return
				else
					say_title(string.format("%s:", mob_name(20409)))
					say(gameforge[lang].main_quest._mission_sohan_11)
					return
				end
			else
				say_title(gameforge.collect_herb_lv10._50_sayTitle)
				say_title(string.format("%s:", mob_name(20409)))
				say(gameforge[lang].main_quest._mission_sohan_12)
				return
			end
			--else
			--	say_title(string.format("%s:", mob_name(20354)))
			--	say(gameforge[lang].main_quest._111)
			--	return
			--end
		end
	end

	state collect_2 begin
		when letter begin
			local lang = pc.get_language()
			local v = find_npc_by_vnum(20409)
			if v != 0 then
				target.vid("__TARGET__", v, gameforge[lang].main_quest._black_sohan_mission_I)
			end
			send_letter(gameforge[lang].main_quest._black_sohan_mission_I)
		end
		when button or info begin
			local lang = pc.get_language()
			say_title(string.format("%s:", gameforge[lang].main_quest._black_sohan_mission_I))
			say("")
			say(string.format(gameforge[lang].main_quest._1, mob_name(20409)))
			say(gameforge[lang].main_quest._mission_sohan_01)
			say(gameforge[lang].main_quest._mission_sohan_02)
		end
		when __TARGET__.target.click begin
			target.delete("__TARGET__")
			local lang = pc.get_language()
			say_title(string.format("%s:", mob_name(20409)))
			say("")
			say(gameforge[lang].main_quest._mission_sohan_03)
			say("")
			say(gameforge[lang].main_quest._mission_sohan_13)
			wait()
			say("")
			say(gameforge[lang].main_quest._mission_sohan_14)
			say(gameforge[lang].main_quest._mission_sohan_15)
			say_item_vnum(30049)
			say(gameforge[lang].main_quest._mission_orc_09)
			pc.setqf("duration",0)
			pc.setqf("collect_count",0)
			pc.setqf("drink_drug",0)
			set_state(gotomission2)
		end
	end

	state gotomission2 begin
		when letter begin
			send_letter(gameforge[pc.get_language()].main_quest._black_sohan_mission_I)
		end
		when button or info begin
			local lang = pc.get_language()
			say_title(string.format("%s:", mob_name(20409)))
			say(gameforge[lang].main_quest._mission_sohan_15)
			say(gameforge[lang].main_quest._mission_sohan_16)
			say_item_vnum(30049)
			say("")
			say(gameforge[lang].main_quest._mission_sohan_14)
			say_reward(string.format(gameforge[lang].main_quest._mission_sohan_16_2, pc.getqf("collect_count")))
		end
		when 71035.use begin
			local lang = pc.get_language()
			if get_time() < pc.getqf("duration") then
				say(gameforge[lang].main_quest._elixir_1)
				return
			end
			if pc.getqf("drink_drug")==1 then
				say(gameforge[lang].main_quest._elixir_2)
				return
			end
			if pc.count_item(30049)==0 then
				say_title(string.format("%s:", mob_name(20409)))
				say(gameforge[lang].main_quest._elixir_3)
				say_item_vnum(30049)
				return
			end
			
			pc.remove_item(71035, 1)
			pc.setqf("drink_drug",1)
		end

		when	1102.kill or
				1132.kill or
				1152.kill or
				1172.kill with pc.count_item(30049) <= 20 begin
			local lang = pc.get_language()
			local s = number(1, 100)
			if s <= 5 then
				pc.give_item2(30049, 1)
				if pc.count_item(30049) >= 20 then
					say(gameforge[lang].main_quest._mission_sohan_20_1)
					say_item_vnum(30049)
					say_reward(string.format(gameforge[lang].main_quest._mission_sohan_20_2, pc.getqf("collect_count")))
				end
			end 
		end

		when 20409.chat.gameforge[pc.get_language()].chat_npc_translate._mission55_2 with pc.count_item(30049) > 0 begin
		--when 20354.chat.gameforge[pc.get_language()].main_quest._106_3 with pc.count_item(30032) > 0 begin
			local lang = pc.get_language()
			--if get_time() > pc.getqf("duration") then
			if pc.count_item(30049) > 0 then
				say_title(string.format("%s:", mob_name(20409)))
				say(gameforge[lang].main_quest._mission_sohan_17)
				pc.remove_item("30049", 1)
				--pc.setqf("duration", get_time()+60*15) -- 15 minutos
				wait()
				local pass_percent
				if pc.getqf("drink_drug")==0 then
					pass_percent=60
				else
					pass_percent=100
				end
				local s = number(1,100)
				if s <= pass_percent then
					if pc.getqf("collect_count") < 19 then
						local index =pc.getqf("collect_count")+1
						pc.setqf("collect_count",index)
						say_title(string.format("%s:", mob_name(20409)))
						say(string.format(gameforge[lang].main_quest._mission_sohan_18, 20-pc.getqf("collect_count")))
						pc.setqf("drink_drug",0)
						return
					end
					say_title(string.format("%s:", mob_name(20409)))
					say(gameforge[lang].main_quest._mission_sohan_coll_finish_2)
					pc.remove_item("30049", 200) -- Remove all
					pc.setqf("collect_count",0)
					pc.setqf("drink_drug",0)
					pc.setqf("duration",0)
					set_state(collect_3)
					return
				else
					say_title(string.format("%s:", mob_name(20409)))
					say(gameforge[lang].main_quest._mission_sohan_19)
					return
				end
			else
				say_title(gameforge.collect_herb_lv10._50_sayTitle)
				say_title(string.format("%s:", mob_name(20409)))
				say(gameforge[lang].main_quest._mission_sohan_20)
				return
			end
			--else
			--	say_title(string.format("%s:", mob_name(20354)))
			--	say(gameforge[lang].main_quest._111)
			--	return
			--end
		end
	end
	
	state collect_3 begin
		when letter begin
			local lang = pc.get_language()
			local v = find_npc_by_vnum(20409)
			if v != 0 then
				target.vid("__TARGET__", v, gameforge[lang].main_quest._black_sohan_mission_I)
			end
			send_letter(gameforge[lang].main_quest._black_sohan_mission_I)
		end
		when button or info begin
			local lang = pc.get_language()
			say_title(string.format("%s:", gameforge[lang].main_quest._black_sohan_mission_I))
			say("")
			say(string.format(gameforge[lang].main_quest._1, mob_name(20409)))
			say(gameforge[lang].main_quest._mission_sohan_01)
			say(gameforge[lang].main_quest._mission_sohan_02)
		end
		when __TARGET__.target.click begin
			target.delete("__TARGET__")
			local lang = pc.get_language()
			say_title(string.format("%s:", mob_name(20409)))
			say("")
			say(gameforge[lang].main_quest._mission_sohan_03)
			say("")
			say(gameforge[lang].main_quest._mission_sohan_21)
			wait()
			say("")
			say(gameforge[lang].main_quest._mission_sohan_22)
			say(gameforge[lang].main_quest._mission_sohan_23)
			say_item_vnum(30042)
			say(gameforge[lang].main_quest._mission_orc_09)
			pc.setqf("duration",0)
			pc.setqf("collect_count",0)
			pc.setqf("drink_drug",0)
			set_state(gotomission3)
		end
	end

	state gotomission3 begin
		when letter begin
			send_letter(gameforge[pc.get_language()].main_quest._black_sohan_mission_I)
		end
		when button or info begin
			local lang = pc.get_language()
			say_title(string.format("%s:", mob_name(20409)))
			say(gameforge[lang].main_quest._mission_sohan_24)
			say(gameforge[lang].main_quest._mission_sohan_25)
			say_item_vnum(30042)
			say("")
			say(gameforge[lang].main_quest._mission_sohan_22)
			say_reward(string.format(gameforge[lang].main_quest._mission_sohan_24_2, pc.getqf("collect_count")))
		end
		when 71035.use begin
			local lang = pc.get_language()
			if get_time() < pc.getqf("duration") then
				say(gameforge[lang].main_quest._elixir_1)
				return
			end
			if pc.getqf("drink_drug")==1 then
				say(gameforge[lang].main_quest._elixir_2)
				return
			end
			if pc.count_item(30042)==0 then
				say_title(string.format("%s:", mob_name(20409)))
				say(gameforge[lang].main_quest._elixir_3)
				say_item_vnum(30042)
				return
			end
			
			pc.remove_item(71035, 1)
			pc.setqf("drink_drug",1)
		end

		when	1103.kill or
				1133.kill or
				1153.kill or
				1173.kill with pc.count_item(30042) <= 20 begin
			local lang = pc.get_language()
			local s = number(1, 100)
			if s <= 5 then
				pc.give_item2(30042, 1)
				if pc.count_item(30042) >= 20 then
					say(gameforge[lang].main_quest._mission_sohan_20_1)
					say_item_vnum(30042)
					say_reward(string.format(gameforge[lang].main_quest._mission_sohan_28_1, pc.getqf("collect_count")))
				end
			end 
		end

		when 20409.chat.gameforge[pc.get_language()].chat_npc_translate._mission55_3 with pc.count_item(30042) > 0 begin
		--when 20354.chat.gameforge[pc.get_language()].main_quest._106_3 with pc.count_item(30032) > 0 begin
			local lang = pc.get_language()
			--if get_time() > pc.getqf("duration") then
			if pc.count_item(30042) > 0 then
				say_title(string.format("%s:", mob_name(20409)))
				say(gameforge[lang].main_quest._mission_sohan_25)
				pc.remove_item("30042", 1)
				--pc.setqf("duration", get_time()+60*15) -- 15 minutos
				wait()
				local pass_percent
				if pc.getqf("drink_drug")==0 then
					pass_percent=60
				else
					pass_percent=100
				end
				local s = number(1,100)
				if s <= pass_percent then
					if pc.getqf("collect_count") < 19 then
						local index =pc.getqf("collect_count")+1
						pc.setqf("collect_count",index)
						say_title(string.format("%s:", mob_name(20409)))
						say(string.format(gameforge[lang].main_quest._mission_sohan_26, 20-pc.getqf("collect_count")))
						pc.setqf("drink_drug",0)
						return
					end
					say_title(string.format("%s:", mob_name(20409)))
					say(gameforge[lang].main_quest._mission_sohan_coll_finish_3)
					pc.remove_item("30042", 200) -- Remove all
					pc.setqf("collect_count",0)
					pc.setqf("drink_drug",0)
					pc.setqf("duration",0)
					set_state(gotomission4)
					return
				else
					say_title(string.format("%s:", mob_name(20409)))
					say(gameforge[lang].main_quest._mission_sohan_27)
					return
				end
			else
				say_title(gameforge.collect_herb_lv10._50_sayTitle)
				say_title(string.format("%s:", mob_name(20409)))
				say(gameforge[lang].main_quest._mission_sohan_28)
				return
			end
			--else
			--	say_title(string.format("%s:", mob_name(20354)))
			--	say(gameforge[lang].main_quest._111)
			--	return
			--end
		end
	end

	state gotomission4 begin
		when letter begin
			local lang = pc.get_language()
			local v = find_npc_by_vnum(20409)
			if v != 0 then
				target.vid("__TARGET__", v, gameforge[lang].main_quest._black_sohan_mission_I)
			end
			send_letter(gameforge[lang].main_quest._black_sohan_mission_I)
		end
		when button or info begin
			local lang = pc.get_language()
			say_title(string.format("%s:", gameforge[lang].main_quest._black_sohan_mission_I))
			say("")
			say(string.format(gameforge[lang].main_quest._1, mob_name(20409)))
			say(gameforge[lang].main_quest._mission_orc_01)
			say(gameforge[lang].main_quest._mission_orc_02)
		end
		when __TARGET__.target.click begin
			target.delete("__TARGET__")
			local lang = pc.get_language()
			say_title(string.format("%s:", mob_name(20409)))
			say("")
			say(gameforge[lang].main_quest._mission_sohan_29)
			say("")
			say(gameforge[lang].main_quest._mission_sohan_30)
			pc.setqf("1101", 75)
			pc.setqf("1102", 75)
			pc.setqf("1103", 75)
			pc.setqf("1104", 75)
			pc.setqf("1105", 50)
			pc.setqf("1106", 50)
			pc.setqf("1107", 50)
			pc.setqf("8011", 15)
			set_state(gotomission5)
		end
	end
	
	state gotomission5 begin
		when letter begin
			send_letter(gameforge[pc.get_language()].main_quest._black_sohan_mission_I)
		end
			
		when button or info begin
			local lang = pc.get_language()
			say_title(string.format("%s:", gameforge[lang].main_quest._black_sohan_mission_I))
			say("")
			say(gameforge[lang].main_quest._37)
			say(string.format("- %d %s,", pc.getqf("1101"), mob_name(1101)))
			say(string.format("- %d %s,", pc.getqf("1102"), mob_name(1102)))
			say(string.format("- %d %s,", pc.getqf("1103"), mob_name(1103)))
			say(string.format("- %d %s,", pc.getqf("1104"), mob_name(1104)))
			say(string.format("- %d %s,", pc.getqf("1105"), mob_name(1105)))
			say(string.format("- %d %s,", pc.getqf("1106"), mob_name(1106)))
			say(string.format("- %d %s,", pc.getqf("1107"), mob_name(1107)))
			say(string.format("- %d %s,", pc.getqf("8011"), mob_name(8011)))
		end

		when	1101.kill or
				1102.kill or
				1103.kill or
				1104.kill or
				1105.kill or
				1106.kill or
				1107.kill or
				8011.kill
				begin
			local n = string.format("%s", npc.get_race())
			local c = pc.getqf(n)
			if c > 0 then
				pc.setqf(n, c - 1)
			end
			local t =	pc.getqf("1101") + 
						pc.getqf("1102") + 
						pc.getqf("1103") + 
						pc.getqf("1104") + 
						pc.getqf("1105") + 
						pc.getqf("1106") + 
						pc.getqf("1107") +
						pc.getqf("8011")
			if t == 0 then
				set_state(gotoreward)
			end
		end
	end

	state gotoreward begin
		when letter begin
			local lang = pc.get_language()
			local v = find_npc_by_vnum(20409)
			if v != 0 then
				target.vid("__TARGET__", v, gameforge[lang].main_quest._black_sohan_mission_I_end)
			end
			send_letter(gameforge[lang].main_quest._black_sohan_mission_I_end)
		end

		when button or info begin
			local lang = pc.get_language()
			say_title(string.format("%s:", gameforge[lang].main_quest._black_sohan_mission_I_end))
			say("")
			say(gameforge[lang].main_quest._black_sohan_mission_end1)
			say(gameforge[lang].main_quest._mission_sohan_02)
			say(string.format(gameforge[lang].main_quest._50, mob_name(20409)))
		end

		when __TARGET__.target.click begin
			target.delete("__TARGET__")
			pc.give_item2(30094, 40)
			pc.give_item2(30095, 20)
			pc.give_item2(30096, 10)
			pc.give_item2(30279, 12)
			pc.give_item2(30277, 5)
			pc.give_item2(30284, 15)
			pc.give_item2(30271, 2)
			pc.give_item2(72310, 14)
			pc.give_item2(71094, 14)
			pc.give_item2(70401, 2)
			pc.give_item2(72320, 2)
			pc.give_item2(88960, 2)
			pc.give_item2(25041, 4)
			pc.give_item2(71084, 550)
			pc.give_exp2(40000000)
			local lang = pc.get_language()
			say_title(string.format("%s:", mob_name(20409)))
			say("")
			say(string.format(gameforge[lang].main_quest._black_soul_mission_end3, pc.get_name()))
			say(gameforge[lang].main_quest._black_soul_mission_end4)
			say("")
			say_reward(gameforge[lang].main_quest._58)
			say_reward(gameforge[lang].main_quest._mission_sohan_reward_1)
			say_reward(gameforge[lang].main_quest._mission_sohan_reward_2)
			say_reward(gameforge[lang].main_quest._mission_sohan_reward_3)
			say_reward(gameforge[lang].main_quest._mission_sohan_reward_4)
			say_reward(gameforge[lang].main_quest._mission_sohan_reward_5)
			say_reward(gameforge[lang].main_quest._mission_sohan_reward_6)
			say_reward(gameforge[lang].main_quest._mission_sohan_reward_7)
			say_reward(gameforge[lang].main_quest._mission_sohan_reward_8)
			say_reward(gameforge[lang].main_quest._mission_sohan_reward_9)
			say_reward(gameforge[lang].main_quest._mission_sohan_reward_10)
			say_reward(gameforge[lang].main_quest._mission_sohan_reward_11)
			say_reward(gameforge[lang].main_quest._mission_sohan_reward_12)
			say_reward(gameforge[lang].main_quest._mission_sohan_reward_13)
			say_reward(gameforge[lang].main_quest._mission_sohan_reward_14)
			say_reward(gameforge[lang].main_quest._mission_orc_reward_exp55)
			pc.delqf("1101")
			pc.delqf("1102")
			pc.delqf("1103")
			pc.delqf("1104")
			pc.delqf("1105")
			pc.delqf("1106")
			pc.delqf("1107")
			pc.delqf("8011")
			set_quest_state("main_quest_lv60", "run")
			set_state(__COMPLETE__)
			clear_letter()
		end
	end

	state __COMPLETE__ begin
	end
end

