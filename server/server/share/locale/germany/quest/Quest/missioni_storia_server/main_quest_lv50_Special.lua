quest main_quest_lv50_Special begin
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
				target.vid("__TARGET__", v, gameforge[lang].main_quest._black_soul_mission_special50)
			end
			send_letter(gameforge[lang].main_quest._black_soul_mission_special50)
		end

		when button or info begin
			local lang = pc.get_language()
			say_title(string.format("%s:", gameforge[lang].main_quest._black_soul_mission_special50))
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
			say(gameforge[lang].main_quest._mission_orc_40)
			wait()
			say(gameforge[lang].main_quest._mission_orc_41)
			say(gameforge[lang].main_quest._mission_orc_42)
			say(gameforge[lang].main_quest._mission_orc_43)
			say_item_vnum(30007)
			say(gameforge[lang].main_quest._mission_orc_09)
			pc.setqf("duration",0)
			pc.setqf("collect_count",0)
			pc.setqf("drink_drug",0)
			set_state(collect_1)
		end
	end

	state collect_1 begin
		when letter begin
			send_letter(gameforge[pc.get_language()].main_quest._black_soul_mission_special50)
		end
		when button or info begin
			local lang = pc.get_language()
			say_title(string.format("%s:", mob_name(20356)))
			say(gameforge[lang].main_quest._mission_orc_44)
			say(gameforge[lang].main_quest._mission_orc_43)
			say_item_vnum(30007)
			say("")
			say(gameforge[lang].main_quest._mission_orc_45)
			say_reward(string.format(gameforge[lang].main_quest._mission_orc_46_2, pc.getqf("collect_count")))
		end
		when 71035.use begin
			local lang = pc.get_language()
			if get_time() < pc.getqf("duration") then
				say(gameforge[lang].main_quest._city2_Jefe_elixir_1)
				return
			end
			if pc.getqf("drink_drug")==1 then
				say(gameforge[lang].main_quest._city2_Jefe_elixir_2)
				return
			end
			if pc.count_item(30007)==0 then
				say_title(string.format("%s:", mob_name(20356)))
				say(gameforge[lang].main_quest._city2_Jefe_elixir_3)
				say_item_vnum(30007)
				return
			end
			
			pc.remove_item(71035, 1)
			pc.setqf("drink_drug",1)
		end
		when	636.kill or
				637.kill or 
				651.kill or 
				656.kill or 
				657.kill or 
				689.kill with pc.count_item(30007) <= 25 begin
			local lang = pc.get_language()
			local s = number(1, 100)
			if s <= 5 then
				pc.give_item2(30007, 1)
				if pc.count_item(30007) >= 25 then
					say(gameforge[lang].main_quest._mission_orc_46)
					say_item_vnum(30007)
					csay.red(gameforge[lang].main_quest._mission_orc_47)
				end
			end
		end
		when 20356.chat.gameforge[pc.get_language()].chat_npc_translate._mission50_1 with pc.count_item(30007) > 0 begin
			local lang = pc.get_language()
			if pc.count_item(30007) > 0 then
				say_title(string.format("%s:", mob_name(20356)))
				say(gameforge[lang].main_quest._mission_orc_48)
				pc.remove_item("30007", 1)
				wait()
				local pass_percent
				if pc.getqf("drink_drug")==0 then
					pass_percent=60
				else
					pass_percent=100
				end
				local s = number(1,100)
				if s <= pass_percent then
					if pc.getqf("collect_count") < 24 then
						local index =pc.getqf("collect_count")+1
						pc.setqf("collect_count",index)
						say_title(string.format("%s:", mob_name(20356)))
						say(string.format(gameforge[lang].main_quest._mission_orc_49, 25-pc.getqf("collect_count")))
						pc.setqf("drink_drug",0)
						return
					end
					say_title(string.format("%s:", mob_name(20356)))
					say(gameforge[lang].main_quest._mission_orc_50)
					pc.remove_item("30007", 200) -- Remove all
					pc.setqf("collect_count",0)
					pc.setqf("drink_drug",0)
					pc.setqf("duration",0)
					set_state(mision_1)
					return
				else
					say_title(string.format("%s:", mob_name(20356)))
					say(gameforge[lang].main_quest._mission_orc_51)
					return
				end
			else
				say_title(gameforge.collect_herb_lv10._50_sayTitle)
				say_title(string.format("%s:", mob_name(20356)))
				say(gameforge[lang].main_quest._mission_orc_52)
				return
			end
		end
	end
	state mision_1 begin
		when letter begin
			send_letter(gameforge[pc.get_language()].main_quest._black_soul_mission_special50)
		end
		
		when button or info begin
			local lang = pc.get_language()
			say_title(string.format("%s:", gameforge[lang].main_quest._black_soul_mission_special50))
			say("")
			say(string.format(gameforge[lang].main_quest._mission_orc_55, mob_name(20356)))
			say(gameforge[lang].main_quest._mission_orc_56)
			set_state(boss_orc)
		end
	end

	state boss_orc begin
		when letter begin
			local lang = pc.get_language()
			local v = find_npc_by_vnum(20356)
			if v != 0 then
				target.vid("__TARGET__", v, gameforge[lang].main_quest._black_soul_mission_special50)
			end
			send_letter(gameforge[lang].main_quest._black_soul_mission_special50)
		end

		when button or info begin
			local lang = pc.get_language()
			say_title(string.format("%s:", gameforge[lang].main_quest._black_soul_mission_special50))
			say("")
			say(string.format(gameforge[lang].main_quest._mission_orc_55, mob_name(20356)))
			say(gameforge[lang].main_quest._mission_orc_56)
		end

		when __TARGET__.target.click begin
			target.delete("__TARGET__")
			local lang = pc.get_language()
			say_title(string.format("%s:", mob_name(20356)))
			say("")
			say(gameforge[lang].main_quest._23)
			say(gameforge[lang].main_quest._24)
			say("")
			say(gameforge[lang].main_quest._mission_orc_57)
			say(gameforge[lang].main_quest._mission_orc_58)
			pc.setqf("691", 10)
			pc.setqf("8009", 15)
			set_state(boss_orc1)
		end
	end

	state boss_orc1 begin
		when letter begin
			send_letter(gameforge[pc.get_language()].main_quest._mission_orc_59)
		end

		when button or info begin
			local lang = pc.get_language()
			say_title(string.format("%s:", gameforge[lang].main_quest._mission_orc_59))
			say("")
			say(gameforge[lang].main_quest._37)
			say(string.format("- %d %s,", pc.getqf("691"), mob_name(691)))
			say(string.format("- %d %s,", pc.getqf("8009"), mob_name(8009)))
		end

		when	691.kill or
				8009.kill
				begin
			local n = string.format("%s", npc.get_race())
			local c = pc.getqf(n)
			if c > 0 then
				pc.setqf(n, c - 1)
			end
			
			local t =	pc.getqf("691") +
						pc.getqf("8009")
			if t == 0 then
				set_state(boss_orc_finish)
			end
		end
	end
	state boss_orc_finish begin
		when letter begin
			local lang = pc.get_language()
			local v = find_npc_by_vnum(20356)
			if v != 0 then
				target.vid("__TARGET__", v, gameforge[lang].main_quest._black_soul_mission_special50)
			end
			
			send_letter(gameforge[lang].main_quest._black_soul_mission_special50)
		end

		when button or info begin
			local lang = pc.get_language()
			say_title(string.format("%s:", gameforge[lang].main_quest._black_soul_mission_special50))
			say("")
			say(gameforge[lang].main_quest._49)
			say(string.format(gameforge[lang].main_quest._mission_orc_55, mob_name(20356)))
		end

		when __TARGET__.target.click begin
			target.delete("__TARGET__")
			local lang = pc.get_language()
			say_title(string.format("%s:", mob_name(20356)))
			say("")
			say(string.format(gameforge[lang].main_quest._51, pc.get_name()))
			say(gameforge[lang].main_quest._52)
			wait()
			say("")
			say("")
			say(gameforge[lang].main_quest._mission_orc_60)
			say(gameforge[lang].main_quest._mission_orc_61)
			pc.delqf("691")
			pc.delqf("8009")
			set_state(gototeacher_2)
		end
	end
		
	state gototeacher_2 begin
		when letter begin
			local lang = pc.get_language()
			local v = find_npc_by_vnum(20356)
			if v != 0 then
				target.vid("__TARGET__", v, gameforge[lang].main_quest._black_soul_mission_special50)
			end
			send_letter(gameforge[lang].main_quest._black_soul_mission_special50)
		end

		when button or info begin
			local lang = pc.get_language()
			say_title(string.format("%s:", gameforge[lang].main_quest._black_soul_mission_special50))
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
			say(gameforge[lang].main_quest._mission_orc_62)
			wait()
			say(gameforge[lang].main_quest._mission_orc_63)
			say(gameforge[lang].main_quest._mission_orc_42)
			say(gameforge[lang].main_quest._mission_orc_64)
			say_item_vnum(30060)
			say(gameforge[lang].main_quest._mission_orc_09)
			pc.setqf("duration",0)
			pc.setqf("collect_count",0)
			pc.setqf("drink_drug",0)
			set_state(collect_2)
		end
	end
		
	state collect_2 begin
		when letter begin
			send_letter(gameforge[pc.get_language()].main_quest._black_soul_mission_special50)
		end
		when button or info begin
			local lang = pc.get_language()
			say_title(string.format("%s:", mob_name(20356)))
			say(gameforge[lang].main_quest._mission_orc_65)
			say(gameforge[lang].main_quest._mission_orc_64)
			say_item_vnum(30060)
			say("")
			say(gameforge[lang].main_quest._mission_orc_63)
			say_reward(string.format(gameforge[lang].main_quest._mission_orc_66_2, pc.getqf("collect_count")))
		end
		when 71035.use begin
			local lang = pc.get_language()
			if get_time() < pc.getqf("duration") then
				say(gameforge[lang].main_quest._city2_Jefe_elixir_1)
				return
			end
			if pc.getqf("drink_drug")==1 then
				say(gameforge[lang].main_quest._city2_Jefe_elixir_2)
				return
			end
			if pc.count_item(30060)==0 then
				say_title(string.format("%s:", mob_name(20356)))
				say(gameforge[lang].main_quest._city2_Jefe_elixir_3)
				say_item_vnum(30060)
				return
			end
			pc.remove_item(71035, 1)
			pc.setqf("drink_drug",1)
		end
		when	1301.kill or
				1302.kill or 
				1305.kill or 
				1331.kill or 
				1332.kill or 
				1335.kill
				with pc.count_item(30060) <= 25
				begin
			local lang = pc.get_language()
			local s = number(1, 100)
			if s <= 5 then
				pc.give_item2(30060, 1)
				if pc.count_item(30060) >= 25 then
					say(gameforge[lang].main_quest._mission_orc_66)
					say_item_vnum(30060)
					csay.red(gameforge[lang].main_quest._mission_orc_47)
				end
			end
		end
		when 20356.chat.gameforge[pc.get_language()].chat_npc_translate._mission50_2 with pc.count_item(30060) > 0 begin
			local lang = pc.get_language()
			if pc.count_item(30060) > 0 then
				say_title(string.format("%s:", mob_name(20356)))
				say(gameforge[lang].main_quest._mission_orc_67)
				pc.remove_item("30060", 1)
				wait()
				local pass_percent
				if pc.getqf("drink_drug")==0 then
					pass_percent=60
				else
					pass_percent=100
				end
				local s = number(1,100)
				if s <= pass_percent then
					if pc.getqf("collect_count") < 24 then
						local index =pc.getqf("collect_count")+1
						pc.setqf("collect_count",index)
						say_title(string.format("%s:", mob_name(20356)))
						say(string.format(gameforge[lang].main_quest._mission_orc_68, 25-pc.getqf("collect_count")))
						pc.setqf("drink_drug",0)
						return
					end
					say_title(string.format("%s:", mob_name(20356)))
					say(gameforge[lang].main_quest._mission_orc_69)
					pc.remove_item("30060", 200) -- Remove all
					pc.setqf("collect_count",0)
					pc.setqf("drink_drug",0)
					pc.setqf("duration",0)
					set_state(mision_2)
					return
				else
					say_title(string.format("%s:", mob_name(20356)))
					say(gameforge[lang].main_quest._mission_orc_70)
					return
				end
			else
				say_title(gameforge.collect_herb_lv10._50_sayTitle)
				say_title(string.format("%s:", mob_name(20356)))
				say(gameforge[lang].main_quest._mission_orc_71)
				return
			end
		end
	end
	state mision_2 begin
		when letter begin
			send_letter(gameforge[pc.get_language()].main_quest._black_soul_mission_special50)
		end
		
		when button or info begin
			local lang = pc.get_language()
			say_title(string.format("%s:", gameforge[lang].main_quest._black_soul_mission_special50))
			say("")
			say(string.format(gameforge[lang].main_quest._mission_orc_55, mob_name(20356)))
			say(gameforge[lang].main_quest._mission_orc_56)
			set_state(boss_lider_osc)
		end
	end

	state boss_lider_osc begin
		when letter begin
			local lang = pc.get_language()
			local v = find_npc_by_vnum(20356)
			if v != 0 then
				target.vid("__TARGET__", v, gameforge[lang].main_quest._black_soul_mission_special50)
			end
			send_letter(gameforge[lang].main_quest._black_soul_mission_special50)
		end

		when button or info begin
			local lang = pc.get_language()
			say_title(string.format("%s:", gameforge[lang].main_quest._black_soul_mission_special50))
			say("")
			say(string.format(gameforge[lang].main_quest._mission_orc_55, mob_name(20356)))
			say(gameforge[lang].main_quest._mission_orc_56)
		end

		when __TARGET__.target.click begin
			target.delete("__TARGET__")
			local lang = pc.get_language()
			say_title(string.format("%s:", mob_name(20356)))
			say("")
			say(gameforge[lang].main_quest._23)
			say(gameforge[lang].main_quest._24)
			say("")
			say(gameforge[lang].main_quest._mission_orc_72)
			say(gameforge[lang].main_quest._mission_orc_73)
			pc.setqf("791", 10)
			set_state(boss_lider_osc1)
		end
	end

	state boss_lider_osc1 begin
		when letter begin
			send_letter(gameforge[pc.get_language()].main_quest._mission_orc_74)
		end

		when button or info begin
			local lang = pc.get_language()
			say_title(string.format("%s:", gameforge[lang].main_quest._mission_orc_74))
			say("")
			say(gameforge[lang].main_quest._37)
			say(string.format("- %d %s,", pc.getqf("791"), mob_name(791)))
		end

		when	791.kill
				begin
			local n = string.format("%s", npc.get_race())
			local c = pc.getqf(n)
			if c > 0 then
				pc.setqf(n, c - 1)
			end
			
			local t =	pc.getqf("791")
			if t == 0 then
				set_state(boss_lider_osc_finish)
			end
		end
	end
	state boss_lider_osc_finish begin
		when letter begin
			local lang = pc.get_language()
			local v = find_npc_by_vnum(20356)
			if v != 0 then
				target.vid("__TARGET__", v, gameforge[lang].main_quest._black_soul_mission_special50)
			end
			
			send_letter(gameforge[lang].main_quest._black_soul_mission_special50)
		end

		when button or info begin
			local lang = pc.get_language()
			say_title(string.format("%s:", gameforge[lang].main_quest._black_soul_mission_special50))
			say("")
			say(gameforge[lang].main_quest._49)
			say(string.format(gameforge[lang].main_quest._mission_orc_55, mob_name(20356)))
		end

		when __TARGET__.target.click begin
			target.delete("__TARGET__")
			local lang = pc.get_language()
			say_title(string.format("%s:", mob_name(20356)))
			say("")
			say(string.format(gameforge[lang].main_quest._51, pc.get_name()))
			say(gameforge[lang].main_quest._52)
			say("")
			pc.delqf("791")
			set_state(gototeacher_3)
		end
	end
		
	state gototeacher_3 begin
		when letter begin
			local lang = pc.get_language()
			local v = find_npc_by_vnum(20356)
			if v != 0 then
				target.vid("__TARGET__", v, gameforge[lang].main_quest._black_soul_mission_special50)
			end
			send_letter(gameforge[lang].main_quest._black_soul_mission_special50)
		end

		when button or info begin
			local lang = pc.get_language()
			say_title(string.format("%s:", gameforge[lang].main_quest._black_soul_mission_special50))
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
			say(gameforge[lang].main_quest._mission_orc_75)
			wait()
			say(gameforge[lang].main_quest._mission_orc_76)
			say(gameforge[lang].main_quest._mission_orc_42)
			say(gameforge[lang].main_quest._mission_orc_77)
			say_item_vnum(30059)
			say(gameforge[lang].main_quest._mission_orc_09)
			pc.setqf("duration",0)
			pc.setqf("collect_count",0)
			pc.setqf("drink_drug",0)
			set_state(collect_3)
		end
	end
		
	state collect_3 begin
		when letter begin
			send_letter(gameforge[pc.get_language()].main_quest._black_soul_mission_special50)
		end
		when button or info begin
			local lang = pc.get_language()
			say_title(string.format("%s:", mob_name(20356)))
			say(gameforge[lang].main_quest._mission_orc_78)
			say(gameforge[lang].main_quest._mission_orc_77)
			say_item_vnum(30059)
			say("")
			say(gameforge[lang].main_quest._mission_orc_76)
			say_reward(string.format(gameforge[lang].main_quest._mission_orc_79_2, pc.getqf("collect_count")))
		end
		when 71035.use begin
			local lang = pc.get_language()
			if get_time() < pc.getqf("duration") then
				say(gameforge[lang].main_quest._city2_Jefe_elixir_1)
				return
			end
			if pc.getqf("drink_drug")==1 then
				say(gameforge[lang].main_quest._city2_Jefe_elixir_2)
				return
			end
			if pc.count_item(30059)==0 then
				say_title(string.format("%s:", mob_name(20356)))
				say(gameforge[lang].main_quest._city2_Jefe_elixir_3)
				say_item_vnum(30059)
				return
			end
			pc.remove_item(71035, 1)
			pc.setqf("drink_drug",1)
		end
		when	2001.kill or
				2002.kill or 
				2003.kill or 
				2004.kill or 
				2005.kill
				with pc.count_item(30059) <= 25
				begin
			local lang = pc.get_language()
			local s = number(1, 100)
			if s <= 5 then
				pc.give_item2(30059, 1)
				if pc.count_item(30059) >= 25 then
					say(gameforge[lang].main_quest._mission_orc_79)
					say_item_vnum(30059)
					csay.red(gameforge[lang].main_quest._mission_orc_47)
				end
			end
		end
		when 20356.chat.gameforge[pc.get_language()].chat_npc_translate._mission50_3 with pc.count_item(30059) > 0 begin
			local lang = pc.get_language()
			if pc.count_item(30059) > 0 then
				say_title(string.format("%s:", mob_name(20356)))
				say(gameforge[lang].main_quest._mission_orc_80)
				pc.remove_item("30059", 1)
				wait()
				local pass_percent
				if pc.getqf("drink_drug")==0 then
					pass_percent=60
				else
					pass_percent=100
				end
				local s = number(1,100)
				if s <= pass_percent then
					if pc.getqf("collect_count") < 24 then
						local index =pc.getqf("collect_count")+1
						pc.setqf("collect_count",index)
						say_title(string.format("%s:", mob_name(20356)))
						say(string.format(gameforge[lang].main_quest._mission_orc_81, 25-pc.getqf("collect_count")))
						pc.setqf("drink_drug",0)
						return
					end
					say_title(string.format("%s:", mob_name(20356)))
					say(gameforge[lang].main_quest._mission_orc_82)
					pc.remove_item("30059", 200) -- Remove all
					pc.setqf("collect_count",0)
					pc.setqf("drink_drug",0)
					pc.setqf("duration",0)
					set_state(mision_3)
					return
				else
					say_title(string.format("%s:", mob_name(20356)))
					say(gameforge[lang].main_quest._mission_orc_83)
					return
				end
			else
				say_title(gameforge.collect_herb_lv10._50_sayTitle)
				say_title(string.format("%s:", mob_name(20356)))
				say(gameforge[lang].main_quest._mission_orc_84)
				return
			end
		end
	end
	state mision_3 begin
		when letter begin
			send_letter(gameforge[pc.get_language()].main_quest._black_soul_mission_special50)
		end
		
		when button or info begin
			local lang = pc.get_language()
			say_title(string.format("%s:", gameforge[lang].main_quest._black_soul_mission_special50))
			say("")
			say(string.format(gameforge[lang].main_quest._mission_orc_55, mob_name(20356)))
			say(gameforge[lang].main_quest._mission_orc_56)
			set_state(boss_tortuga)
		end
	end

	state boss_tortuga begin
		when letter begin
			local lang = pc.get_language()
			local v = find_npc_by_vnum(20356)
			if v != 0 then
				target.vid("__TARGET__", v, gameforge[lang].main_quest._black_soul_mission_special50)
			end
			send_letter(gameforge[lang].main_quest._black_soul_mission_special50)
		end

		when button or info begin
			local lang = pc.get_language()
			say_title(string.format("%s:", gameforge[lang].main_quest._black_soul_mission_special50))
			say("")
			say(string.format(gameforge[lang].main_quest._mission_orc_55, mob_name(20356)))
			say(gameforge[lang].main_quest._mission_orc_56)
		end

		when __TARGET__.target.click begin
			target.delete("__TARGET__")
			local lang = pc.get_language()
			say_title(string.format("%s:", mob_name(20356)))
			say("")
			say(gameforge[lang].main_quest._23)
			say(gameforge[lang].main_quest._24)
			say("")
			say(gameforge[lang].main_quest._mission_orc_85)
			say(gameforge[lang].main_quest._mission_orc_86)
			pc.setqf("2191", 10)
			pc.setqf("8010", 15)
			set_state(boss_tortuga_finish)
		end
	end

	state boss_tortuga_finish begin
		when letter begin
			send_letter(gameforge[pc.get_language()].main_quest._mission_orc_87)
		end

		when button or info begin
			local lang = pc.get_language()
			say_title(string.format("%s:", gameforge[lang].main_quest._mission_orc_87))
			say("")
			say(gameforge[lang].main_quest._37)
			say(string.format("- %d %s,", pc.getqf("2191"), mob_name(2191)))
			say(string.format("- %d %s,", pc.getqf("8010"), mob_name(8010)))
		end

		when	2191.kill or
				8010.kill
				begin
			local n = string.format("%s", npc.get_race())
			local c = pc.getqf(n)
			if c > 0 then
				pc.setqf(n, c - 1)
			end
			
			local t =	pc.getqf("2191") +
						pc.getqf("8010")
			
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
				target.vid("__TARGET__", v, gameforge[lang].main_quest._black_soul_mission_end_special50)
			end
			
			send_letter(gameforge[lang].main_quest._black_soul_mission_end_special50)
		end

		when button or info begin
			local lang = pc.get_language()
			say_title(string.format("%s:", gameforge[lang].main_quest._black_soul_mission_end_special50))
			say("")
			say(gameforge[lang].main_quest._49)
			say(string.format(gameforge[lang].main_quest._mission_orc_55, mob_name(20356)))
		end

		when __TARGET__.target.click begin
			target.delete("__TARGET__")
			local lang = pc.get_language()
			say_title(string.format("%s:", mob_name(20356)))
			say("")
			say(string.format(gameforge[lang].main_quest._black_soul_mission_end3, pc.get_name()))
			say(gameforge[lang].main_quest._black_soul_mission_end4)
			say("")
			if pc.get_sex() == 0 then -- Male
				--Atuendos
				pc.give_item2(49130, 1) -- Costume Body
				pc.give_item2(49924, 1) -- Costume Hair
			elseif pc.get_sex() == 1 then -- Female
				pc.give_item2(49129, 1) -- Costume Body
				pc.give_item2(49923, 1) -- Costume Hair
			end
			pc.give_item2(30094, 50)
			pc.give_item2(30095, 25)
			pc.give_item2(30096, 12)
			pc.give_item2(30279, 20)
			pc.give_item2(30277, 5)
			pc.give_item2(30284, 25)
			pc.give_item2(30271, 5)
			pc.give_item2(72310, 15)
			pc.give_item2(71094, 15)
			pc.give_item2(70401, 4)
			pc.give_item2(72320, 4)
			pc.give_item2(88960, 6)
			pc.give_item2(25041, 8)
			pc.give_item2(72001, 1)
			pc.give_item2(72006, 1)
			pc.give_item2(71084, 700)
			pc.give_exp2(30000000)
			say_reward(gameforge[lang].main_quest._58)
			say_reward(gameforge[lang].main_quest._mission_orc_reward_41)
			say_reward(gameforge[lang].main_quest._mission_orc_reward_42)
			say_reward(gameforge[lang].main_quest._mission_orc_reward_43)
			say_reward(gameforge[lang].main_quest._mission_orc_reward_44)
			say_reward(gameforge[lang].main_quest._mission_orc_reward_45)
			say_reward(gameforge[lang].main_quest._mission_orc_reward_46)
			say_reward(gameforge[lang].main_quest._mission_orc_reward_47)
			say_reward(gameforge[lang].main_quest._mission_orc_reward_48)
			say_reward(gameforge[lang].main_quest._mission_orc_reward_49)
			say_reward(gameforge[lang].main_quest._mission_orc_reward_50)
			say_reward(gameforge[lang].main_quest._mission_orc_reward_51)
			say_reward(gameforge[lang].main_quest._mission_orc_reward_52)
			say_reward(gameforge[lang].main_quest._mission_orc_reward_53)
			say_reward(gameforge[lang].main_quest._mission_orc_reward_54)
			say_reward(gameforge[lang].main_quest._city2_Jefe_reward_09)
			say_reward(gameforge[lang].main_quest._city2_Jefe_reward_10)
			say_reward(gameforge[lang].main_quest._mission_orc_reward_special)
			say_reward(gameforge[lang].main_quest._mission_orc_reward_special2)
			say_reward(gameforge[lang].main_quest._mission_orc_reward_exp50_esp)
			pc.delqf("2191")
			pc.delqf("8010")
			set_quest_state("main_quest_lv55", "run")
			set_state(__COMPLETE__)
			clear_letter()
		end
	end

	state __COMPLETE__ begin
	end
end

