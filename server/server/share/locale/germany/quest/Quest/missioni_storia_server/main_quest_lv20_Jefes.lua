quest main_quest_lv20_Jefes begin
	state start begin
	end

	state run begin
		when enter or login or levelup with pc.get_level() >= 20 begin
			set_state(information)
		end
	end

	state information begin
		when letter begin
			local lang = pc.get_language()
			local v = find_npc_by_vnum(20354)
			if v != 0 then
				target.vid("__TARGET__", v, gameforge[lang].main_quest.title_6)
			end
			send_letter(gameforge[lang].main_quest.title_6)
		end

		when button or info begin
			local lang = pc.get_language()
			say_title(string.format("%s:", gameforge[lang].main_quest.title_6))
			say("")
			say(string.format(gameforge[lang].main_quest._1, mob_name(20354)))
			say(gameforge[lang].main_quest._2)
		end
		
		when __TARGET__.target.click begin
			target.delete("__TARGET__")
			local lang = pc.get_language()
			say_title(string.format("%s:", mob_name(20354)))
			say("")
			say(gameforge[lang].main_quest._100)
			wait()
			say_title(string.format("%s:", mob_name(20354)))
			say(gameforge[lang].main_quest._101)
			wait()
			say(gameforge[lang].main_quest._101_1)
			wait()
			say_title(string.format("%s:", mob_name(20354)))
			say(gameforge[lang].main_quest._102)
			set_state(collect_1)
			pc.setqf("duration",0)
			pc.setqf("collect_count",0)
			pc.setqf("drink_drug",0)
		end
	end

	state collect_1 begin
		when letter begin
			send_letter(gameforge[pc.get_language()].main_quest._103)
		end

		when button or info begin
			local lang = pc.get_language()
			say_title(string.format("%s:", gameforge[lang].main_quest._104))
			say(gameforge[lang].main_quest._105)
			say_item_vnum(30011)
			say_reward(string.format(gameforge[lang].main_quest._collect_30011, pc.getqf("collect_count")))
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
			if pc.count_item(30011)==0 then
				say_title(string.format("%s:", mob_name(20354)))
				say(gameforge[lang].main_quest._elixir_3)
				say_item_vnum(30011)
				return
			end
			
			pc.remove_item(71035, 1)
			pc.setqf("drink_drug",1)
		end

		when 301.kill or 302.kill or 303.kill or 304.kill begin
			local s = number(1, 100)
			if s <= 5 then
				pc.give_item2(30011, 1)
			end 
		end

		when 20354.chat.gameforge[pc.get_language()].chat_npc_translate._mission20_1 with pc.count_item(30011) > 0 begin
		--when 20354.chat.gameforge[pc.get_language()].main_quest._106_3 with pc.count_item(30032) > 0 begin
			local lang = pc.get_language()
			--if get_time() > pc.getqf("duration") then
			if pc.count_item(30011) > 0 then
				say_title(string.format("%s:", mob_name(20354)))
				say(gameforge[lang].main_quest._107)
				pc.remove_item("30011", 1)
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
					if pc.getqf("collect_count") < 9 then
						local index =pc.getqf("collect_count")+1
						pc.setqf("collect_count",index)
						say_title(string.format("%s:", mob_name(20354)))
						say(string.format(gameforge[lang].main_quest._108, 10-pc.getqf("collect_count")))
						pc.setqf("drink_drug",0)
						return
					end
					say_title(string.format("%s:", mob_name(20354)))
					say(gameforge[lang].main_quest._finish_collect_1)
					pc.remove_item("30011", 200) -- Remove all
					pc.setqf("collect_count",0)
					pc.setqf("drink_drug",0)
					pc.setqf("duration",0)
					set_state(collect_2)
					return
				else
					say_title(string.format("%s:", mob_name(20354)))
					say(gameforge[lang].main_quest._109)
					return
				end
			else
				say_title(gameforge.collect_herb_lv10._50_sayTitle)
				say_title(string.format("%s:", mob_name(20354)))
				say(gameforge[lang].main_quest._110)
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
			send_letter(gameforge[pc.get_language()].main_quest._103)
		end

		when button or info begin
			local lang = pc.get_language()
			say_title(string.format("%s:", gameforge[lang].main_quest._104_2))
			say(gameforge[lang].main_quest._105_2)
			say_item_vnum(30031)
			say_reward(string.format(gameforge[lang].main_quest._collect_30031, pc.getqf("collect_count")))
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
			if pc.count_item(30031)==0 then
				say_title(string.format("%s:", mob_name(20354)))
				say(gameforge[lang].main_quest._elixir_3)
				say_item_vnum(30031)
				return
			end
			
			pc.remove_item(71035, 1)
			pc.setqf("drink_drug",1)
		end

		when 331.kill or 332.kill or 333.kill or 334.kill begin
			local s = number(1, 100)
			if s <= 5 then
				pc.give_item2(30031, 1)
			end 
		end

		when 20354.chat.gameforge[pc.get_language()].chat_npc_translate._mission20_2 with pc.count_item(30031) > 0 begin
			local lang = pc.get_language()
			--if get_time() > pc.getqf("duration") then
			if pc.count_item(30031) > 0 then
				say_title(string.format("%s:", mob_name(20354)))
				say(gameforge[lang].main_quest._107_2)
				pc.remove_item("30031", 1)
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
					if pc.getqf("collect_count") < 9 then
						local index =pc.getqf("collect_count")+1
						pc.setqf("collect_count",index)
						say_title(string.format("%s:", mob_name(20354)))
						say(string.format(gameforge[lang].main_quest._108_2, 10-pc.getqf("collect_count")))
						pc.setqf("drink_drug",0)
						return
					end
					say_title(string.format("%s:", mob_name(20354)))
					say(gameforge[lang].main_quest._finish_collect_2)
					pc.remove_item("30031", 200) -- Remove all
					pc.setqf("collect_count",0)
					pc.setqf("drink_drug",0)
					pc.setqf("duration",0)
					set_state(collect_3)
					return
				else
					say_title(string.format("%s:", mob_name(20354)))
					say(gameforge[lang].main_quest._109_2)
					return
				end
			else
				say_title(gameforge.collect_herb_lv10._50_sayTitle)
				say_title(string.format("%s:", mob_name(20354)))
				say(gameforge[lang].main_quest._110_2)
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
			send_letter(gameforge[pc.get_language()].main_quest._103)
		end

		when button or info begin
			local lang = pc.get_language()
			say_title(string.format("%s:", gameforge[lang].main_quest._104_3))
			say(gameforge[lang].main_quest._105_3)
			say_item_vnum(30032)
			say_reward(string.format(gameforge[lang].main_quest._collect_30032, pc.getqf("collect_count")))
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
			if pc.count_item(30032)==0 then
				say_title(string.format("%s:", mob_name(20354)))
				say(gameforge[lang].main_quest._elixir_3)
				say_item_vnum(30032)
				return
			end
			
			pc.remove_item(71035, 1)
			pc.setqf("drink_drug",1)
		end

		when 351.kill or 352.kill or 353.kill or 354.kill begin
			local s = number(1, 100)
			if s <= 5 then
				pc.give_item2(30032, 1)
			end 
		end

		when 20354.chat.gameforge[pc.get_language()].chat_npc_translate._mission20_3 with pc.count_item(30032) > 0 begin
			local lang = pc.get_language()
			--if get_time() > pc.getqf("duration") then
			if pc.count_item(30032) > 0 then
				say_title(string.format("%s:", mob_name(20354)))
				say(gameforge[lang].main_quest._107_3)
				pc.remove_item("30032", 1)
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
					if pc.getqf("collect_count") < 9 then
						local index =pc.getqf("collect_count")+1
						pc.setqf("collect_count",index)
						say_title(string.format("%s:", mob_name(20354)))
						say(string.format(gameforge[lang].main_quest._108_3, 10-pc.getqf("collect_count")))
						pc.setqf("drink_drug",0)
						return
					end
					say_title(string.format("%s:", mob_name(20354)))
					say(gameforge[lang].main_quest._finish_collect_3)
					pc.remove_item("30032", 200) -- Remove all
					pc.setqf("collect_count",0)
					pc.setqf("drink_drug",0)
					pc.setqf("duration",0)
					set_state(gotoinfomation)
					return
				else
					say_title(string.format("%s:", mob_name(20354)))
					say(gameforge[lang].main_quest._109_3)
					return
				end
			else
				say_title(gameforge.collect_herb_lv10._50_sayTitle)
				say_title(string.format("%s:", mob_name(20354)))
				say(gameforge[lang].main_quest._110_3)
				return
			end
			--else
			--	say_title(string.format("%s:", mob_name(20354)))
			--	say(gameforge[lang].main_quest._111)
			--	return
			--end
		end
	end

	state gotoinfomation begin
		when letter begin
			send_letter(gameforge[pc.get_language()].main_quest.title_6)
		end
		
		when button or info begin
			local lang = pc.get_language()
			say_title(string.format("%s:", gameforge[lang].main_quest.title_6))
			say("")
			say(string.format(gameforge[lang].main_quest._1, mob_name(20354)))
			say(gameforge[lang].main_quest._2)
			set_state(gototeacher)
		end
	end

	state gototeacher begin
		when letter begin
			local lang = pc.get_language()
			local v = find_npc_by_vnum(20354)
			if v != 0 then
				target.vid("__TARGET__", v, gameforge[lang].main_quest.title_6)
			end
			send_letter(gameforge[lang].main_quest.title_6)
		end

		when button or info begin
			local lang = pc.get_language()
			say_title(string.format("%s:", gameforge[lang].main_quest.title_6))
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
			pc.setqf("191", 5)
			pc.setqf("192", 5)
			pc.setqf("193", 5)
			pc.setqf("194", 5)
			pc.setqf("394", 5)
			set_state(gotomission)
		end
	end

	state gotomission begin
		when letter begin
			send_letter(gameforge[pc.get_language()].main_quest.subtitle_6)
		end

		when button or info begin
			local lang = pc.get_language()
			say_title(string.format("%s:", gameforge[lang].main_quest.subtitle_6))
			say("")
			say(gameforge[lang].main_quest._37)
			say(string.format("- %d %s,", pc.getqf("191"), mob_name(191)))
			say(string.format("- %d %s,", pc.getqf("192"), mob_name(192)))
			say(string.format("- %d %s,", pc.getqf("193"), mob_name(193)))
			say(string.format("- %d %s,", pc.getqf("194"), mob_name(194)))
			say(string.format("- %d %s,", pc.getqf("394"), mob_name(394)))
		end

		when 191.kill or 192.kill or 193.kill or 194.kill or 394.kill begin
			local n = string.format("%s", npc.get_race())
			local c = pc.getqf(n)
			if c > 0 then
				pc.setqf(n, c - 1)
			end
			
			local t = pc.getqf("191") + pc.getqf("192") + pc.getqf("193") + pc.getqf("194") + pc.getqf("394")
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
				target.vid("__TARGET__", v, gameforge[lang].main_quest.donetitle_6)
			end
			
			send_letter(gameforge[lang].main_quest.donetitle_6)
		end

		when button or info begin
			local lang = pc.get_language()
			say_title(string.format("%s:", gameforge[lang].main_quest.donetitle_6))
			say("")
			say(gameforge[lang].main_quest._49)
			say(string.format(gameforge[lang].main_quest._50, mob_name(20354)))
		end

		when __TARGET__.target.click begin
			target.delete("__TARGET__")
			pc.give_item2(30094, 15) --Bolsa de Yang pequeña
			pc.give_item2(30095, 6) --Bolsa de Yang mediana
			pc.give_item2(30095, 2) --Bolsa de Yang grande
			pc.give_item2(71084, 400) -- Objeto Encantado
			pc.give_item2(88967, 1) -- Pet Create(Huevo)
			pc.give_item2(88960, 2) -- Complemento especial para el pet
			pc.give_item2(88922, 1) -- Lucha crítica
			pc.give_item2(88923, 1) -- Pelea perforante
			pc.give_item2(88924, 1) -- Ataque del Dios Dragón
			pc.give_item2(88925, 1) -- Defensa del Dios Dragón
			pc.give_item2(88926, 1) -- Intel. del Dios Dragón
			pc.give_item2(88927, 1) -- Vida del Dios Dragón
			pc.give_item2(72001, 1) --Anillo de experiencia
			pc.give_exp2(400000)
			
			--pc.change_gold(80000000)
			say_title(string.format("%s:", mob_name(20354)))
			say("")
			local lang = pc.get_language()
			say(string.format(gameforge[lang].main_quest._51, pc.get_name()))
			say(gameforge[lang].main_quest._52)
			say("")
			say_reward(gameforge[lang].main_quest._58)
			say_reward(gameforge[lang].main_quest._71)
			say_reward(gameforge[lang].main_quest._72)
			say_reward(gameforge[lang].main_quest._73)
			say_reward(gameforge[lang].main_quest._74)
			say_reward(gameforge[lang].main_quest._75)
			say_reward(gameforge[lang].main_quest._76)
			say_reward(gameforge[lang].main_quest._city2_Jefe_reward_10)
			say_reward(gameforge[lang].main_quest._mission_orc_reward_critc)
			say_reward(gameforge[lang].main_quest._mission_orc_reward_critc1)
			say_reward(gameforge[lang].main_quest._mission_orc_reward_critc2)
			say_reward(gameforge[lang].main_quest._mission_orc_reward_critc3)
			say_reward(gameforge[lang].main_quest._mission_orc_reward_critc4)
			say_reward(gameforge[lang].main_quest._mission_orc_reward_critc5)
			say_reward(gameforge[lang].main_quest._mission_orc_reward_exp20_esp)
			--say_reward(gameforge[lang].main_quest._70)
			pc.delqf("191")
			pc.delqf("192")
			pc.delqf("193")
			pc.delqf("194")
			pc.delqf("394")
			set_quest_state("main_quest_lv25", "run")
			set_state(__COMPLETE__)
			clear_letter()
		end
	end

	state __COMPLETE__ begin
	end
end

