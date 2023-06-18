quest main_quest_lv40_Jefes begin
	state start begin
	end

	state run begin
		when enter or login or levelup with pc.get_level() >= 40 begin
			set_state(information)
		end
	end

	state information begin
		when letter begin
			local lang = pc.get_language()
			local v = find_npc_by_vnum(20354)
			if v != 0 then
				target.vid("__TARGET__", v, gameforge[lang].main_quest.title_11)
			end
			send_letter(gameforge[lang].main_quest.title_11)
		end

		when button or info begin
			local lang = pc.get_language()
			say_title(string.format("%s:", gameforge[lang].main_quest.title_11))
			say("")
			say(string.format(gameforge[lang].main_quest._1, mob_name(20354)))
			say(gameforge[lang].main_quest._2)
		end
		
		when __TARGET__.target.click begin
			target.delete("__TARGET__")
			local lang = pc.get_language()
			say_title(string.format("%s:", mob_name(20354)))
			say("")
			say(gameforge[lang].main_quest._city2_Jefe_100)
			wait()
			say_title(string.format("%s:", mob_name(20354)))
			say(gameforge[lang].main_quest._city2_Jefe_101)
			wait()
			say(gameforge[lang].main_quest._city2_Jefe_101_1)
			wait()
			say_title(string.format("%s:", mob_name(20354)))
			say(gameforge[lang].main_quest._city2_Jefe_102)
			set_state(collect_1)
			pc.setqf("duration",0)
			pc.setqf("collect_count",0)
			pc.setqf("drink_drug",0)
		end
	end

	state collect_1 begin
		when letter begin
			send_letter(gameforge[pc.get_language()].main_quest._city2_Jefe_103)
		end

		when button or info begin
			local lang = pc.get_language()
			say_title(string.format("%s:", gameforge[lang].main_quest._city2_Jefe_104))
			say(gameforge[lang].main_quest._city2_Jefe_105)
			say_item_vnum(30105)
			say_reward(string.format(gameforge[lang].main_quest._city2_Jefe_collect_30105, pc.getqf("collect_count")))
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
			if pc.count_item(30105)==0 then
				say_title(string.format("%s:", mob_name(20354)))
				say(gameforge[lang].main_quest._city2_Jefe_elixir_3)
				say_item_vnum(30105)
				return
			end
			
			pc.remove_item(71035, 1)
			pc.setqf("drink_drug",1)
		end

		when 401.kill or 402.kill or 403.kill or 404.kill or 405.kill or 406.kill begin
			local s = number(1, 100)
			if s <= 5 then
				pc.give_item2(30105, 1)
			end 
		end

		when 20354.chat.gameforge[pc.get_language()].chat_npc_translate._mission40_1 with pc.count_item(30105) > 0 begin
		--when 20354.chat.gameforge[pc.get_language()].main_quest._106_3 with pc.count_item(30032) > 0 begin
			local lang = pc.get_language()
			--if get_time() > pc.getqf("duration") then
			if pc.count_item(30105) > 0 then
				say_title(string.format("%s:", mob_name(20354)))
				say(gameforge[lang].main_quest._city2_Jefe_107)
				pc.remove_item("30105", 1)
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
						say(string.format(gameforge[lang].main_quest._city2_Jefe_108, 10-pc.getqf("collect_count")))
						pc.setqf("drink_drug",0)
						return
					end
					say_title(string.format("%s:", mob_name(20354)))
					say(gameforge[lang].main_quest._city2_Jefe_finish_collect_1)
					pc.remove_item("30105", 200) -- Remove all
					pc.setqf("collect_count",0)
					pc.setqf("drink_drug",0)
					pc.setqf("duration",0)
					set_state(collect_2)
					return
				else
					say_title(string.format("%s:", mob_name(20354)))
					say(gameforge[lang].main_quest._city2_Jefe_109)
					return
				end
			else
				say_title(gameforge.collect_herb_lv10._50_sayTitle)
				say_title(string.format("%s:", mob_name(20354)))
				say(gameforge[lang].main_quest._city2_Jefe_110)
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
			send_letter(gameforge[pc.get_language()].main_quest._city2_Jefe_103)
		end

		when button or info begin
			local lang = pc.get_language()
			say_title(string.format("%s:", gameforge[lang].main_quest._city2_Jefe_104_2))
			say(gameforge[lang].main_quest._city2_Jefe_105_2)
			say_item_vnum(30021)
			say_reward(string.format(gameforge[lang].main_quest._city2_Jefe_collect_30021, pc.getqf("collect_count")))
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
			if pc.count_item(30021)==0 then
				say_title(string.format("%s:", mob_name(20354)))
				say(gameforge[lang].main_quest._city2_Jefe_elixir_3)
				say_item_vnum(30021)
				return
			end
			
			pc.remove_item(71035, 1)
			pc.setqf("drink_drug",1)
		end

		when 431.kill or 432.kill or 433.kill or 434.kill or 435.kill or 436.kill begin
			local s = number(1, 100)
			if s <= 5 then
				pc.give_item2(30021, 1)
			end 
		end

		when 20354.chat.gameforge[pc.get_language()].chat_npc_translate._mission40_2 with pc.count_item(30021) > 0 begin
			local lang = pc.get_language()
			--if get_time() > pc.getqf("duration") then
			if pc.count_item(30021) > 0 then
				say_title(string.format("%s:", mob_name(20354)))
				say(gameforge[lang].main_quest._city2_Jefe_107_2)
				pc.remove_item("30021", 1)
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
						say(string.format(gameforge[lang].main_quest._city2_Jefe_108_2, 10-pc.getqf("collect_count")))
						pc.setqf("drink_drug",0)
						return
					end
					say_title(string.format("%s:", mob_name(20354)))
					say(gameforge[lang].main_quest._city2_Jefe_finish_collect_2)
					pc.remove_item("30021", 200) -- Remove all
					pc.setqf("collect_count",0)
					pc.setqf("drink_drug",0)
					pc.setqf("duration",0)
					set_state(collect_3)
					return
				else
					say_title(string.format("%s:", mob_name(20354)))
					say(gameforge[lang].main_quest._city2_Jefe_109_2)
					return
				end
			else
				say_title(gameforge.collect_herb_lv10._50_sayTitle)
				say_title(string.format("%s:", mob_name(20354)))
				say(gameforge[lang].main_quest._city2_Jefe_110_2)
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
			say_title(string.format("%s:", gameforge[lang].main_quest._city2_Jefe_104_3))
			say(gameforge[lang].main_quest._city2_Jefe_105_3)
			say_item_vnum(30030)
			say_reward(string.format(gameforge[lang].main_quest._city2_Jefe_collect_30030, pc.getqf("collect_count")))
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
			if pc.count_item(30030)==0 then
				say_title(string.format("%s:", mob_name(20354)))
				say(gameforge[lang].main_quest._city2_Jefe_elixir_3)
				say_item_vnum(30030)
				return
			end
			
			pc.remove_item(71035, 1)
			pc.setqf("drink_drug",1)
		end

		when 451.kill or 452.kill or 453.kill or 454.kill or 455.kill or 456.kill begin
			local s = number(1, 100)
			if s <= 5 then
				pc.give_item2(30030, 1)
			end 
		end

		when 20354.chat.gameforge[pc.get_language()].chat_npc_translate._mission40_3 with pc.count_item(30030) > 0 begin
			local lang = pc.get_language()
			--if get_time() > pc.getqf("duration") then
			if pc.count_item(30030) > 0 then
				say_title(string.format("%s:", mob_name(20354)))
				say(gameforge[lang].main_quest._city2_Jefe_107_3)
				pc.remove_item("30030", 1)
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
					if pc.getqf("collect_count") < 14 then
						local index =pc.getqf("collect_count")+1
						pc.setqf("collect_count",index)
						say_title(string.format("%s:", mob_name(20354)))
						say(string.format(gameforge[lang].main_quest._city2_Jefe_108_3, 15-pc.getqf("collect_count")))
						pc.setqf("drink_drug",0)
						return
					end
					say_title(string.format("%s:", mob_name(20354)))
					say(gameforge[lang].main_quest._city2_Jefe_finish_collect_3)
					pc.remove_item("30030", 200) -- Remove all
					pc.setqf("collect_count",0)
					pc.setqf("drink_drug",0)
					pc.setqf("duration",0)
					set_state(collect_4)
					return
				else
					say_title(string.format("%s:", mob_name(20354)))
					say(gameforge[lang].main_quest._city2_Jefe_109_3)
					return
				end
			else
				say_title(gameforge.collect_herb_lv10._50_sayTitle)
				say_title(string.format("%s:", mob_name(20354)))
				say(gameforge[lang].main_quest._city2_Jefe_110_3)
				return
			end
			--else
			--	say_title(string.format("%s:", mob_name(20354)))
			--	say(gameforge[lang].main_quest._111)
			--	return
			--end
		end
	end
	
	state collect_4 begin
		when letter begin
			send_letter(gameforge[pc.get_language()].main_quest._103)
		end

		when button or info begin
			local lang = pc.get_language()
			say_title(string.format("%s:", gameforge[lang].main_quest._city2_Jefe_104_4))
			say(gameforge[lang].main_quest._city2_Jefe_105_4)
			say_item_vnum(30041)
			say_reward(string.format(gameforge[lang].main_quest._city2_Jefe_collect_30041, pc.getqf("collect_count")))
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
			if pc.count_item(30041)==0 then
				say_title(string.format("%s:", mob_name(20354)))
				say(gameforge[lang].main_quest._city2_Jefe_elixir_3)
				say_item_vnum(30041)
				return
			end
			
			pc.remove_item(71035, 1)
			pc.setqf("drink_drug",1)
		end

		when 551.kill or 552.kill or 553.kill or 554.kill begin
			local s = number(1, 100)
			if s <= 5 then
				pc.give_item2(30041, 1)
			end 
		end

		when 20354.chat.gameforge[pc.get_language()].chat_npc_translate._mission40_4 with pc.count_item(30041) > 0 begin
			local lang = pc.get_language()
			--if get_time() > pc.getqf("duration") then
			if pc.count_item(30041) > 0 then
				say_title(string.format("%s:", mob_name(20354)))
				say(gameforge[lang].main_quest._city2_Jefe_107_4)
				pc.remove_item("30041", 1)
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
					if pc.getqf("collect_count") < 14 then
						local index =pc.getqf("collect_count")+1
						pc.setqf("collect_count",index)
						say_title(string.format("%s:", mob_name(20354)))
						say(string.format(gameforge[lang].main_quest._city2_Jefe_108_4, 15-pc.getqf("collect_count")))
						pc.setqf("drink_drug",0)
						return
					end
					say_title(string.format("%s:", mob_name(20354)))
					say(gameforge[lang].main_quest._city2_Jefe_finish_collect_4)
					pc.remove_item("30041", 200) -- Remove all
					pc.setqf("collect_count",0)
					pc.setqf("drink_drug",0)
					pc.setqf("duration",0)
					set_state(gotoinfomation)
					return
				else
					say_title(string.format("%s:", mob_name(20354)))
					say(gameforge[lang].main_quest._city2_Jefe_109_4)
					return
				end
			else
				say_title(gameforge.collect_herb_lv10._50_sayTitle)
				say_title(string.format("%s:", mob_name(20354)))
				say(gameforge[lang].main_quest._city2_Jefe_110_4)
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
			send_letter(gameforge[pc.get_language()].main_quest.title_11)
		end
		
		when button or info begin
			local lang = pc.get_language()
			say_title(string.format("%s:", gameforge[lang].main_quest.title_11))
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
				target.vid("__TARGET__", v, gameforge[lang].main_quest.title_11)
			end
			send_letter(gameforge[lang].main_quest.title_11)
		end

		when button or info begin
			local lang = pc.get_language()
			say_title(string.format("%s:", gameforge[lang].main_quest.title_11))
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
			pc.setqf("491", 5)
			pc.setqf("492", 5)
			pc.setqf("493", 5)
			pc.setqf("494", 5)
			pc.setqf("591", 10)
			set_state(gotomission)
		end
	end

	state gotomission begin
		when letter begin
			send_letter(gameforge[pc.get_language()].main_quest.subtitle_11)
		end

		when button or info begin
			local lang = pc.get_language()
			say_title(string.format("%s:", gameforge[lang].main_quest.subtitle_11))
			say("")
			say(gameforge[lang].main_quest._37)
			say(string.format("- %d %s,", pc.getqf("491"), mob_name(491)))
			say(string.format("- %d %s,", pc.getqf("492"), mob_name(492)))
			say(string.format("- %d %s,", pc.getqf("493"), mob_name(493)))
			say(string.format("- %d %s,", pc.getqf("494"), mob_name(494)))
			say(string.format("- %d %s,", pc.getqf("591"), mob_name(591)))
		end

		when 491.kill or 492.kill or 493.kill or 494.kill or 591.kill begin
			local n = string.format("%s", npc.get_race())
			local c = pc.getqf(n)
			if c > 0 then
				pc.setqf(n, c - 1)
			end
			
			local t = pc.getqf("491") + pc.getqf("492") + pc.getqf("493") + pc.getqf("494") + pc.getqf("591")
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
				target.vid("__TARGET__", v, gameforge[lang].main_quest.donetitle_11)
			end
			
			send_letter(gameforge[lang].main_quest.donetitle_11)
		end

		when button or info begin
			local lang = pc.get_language()
			say_title(string.format("%s:", gameforge[lang].main_quest.donetitle_11))
			say("")
			say(gameforge[lang].main_quest._49)
			say(string.format(gameforge[lang].main_quest._50, mob_name(20354)))
		end

		when __TARGET__.target.click begin
			target.delete("__TARGET__")
			pc.give_item2(30094, 30) --Bolsa de Yang pequeña
			pc.give_item2(30095, 15) --Bolsa de Yang mediana
			pc.give_item2(30096, 8) --Bolsa de Yang grande
			pc.give_item2(30279, 10) --Cristal solar
			pc.give_item2(30284, 15) --Piedra sin espíritu
			pc.give_item2(72310, 12) --Pergamino de exorcismo
			pc.give_item2(71094, 12) --Lectura concentrada
			pc.give_item2(70401, 2) --Encantar Disfraz
			pc.give_item2(72006, 1) --Guantes de Ladrón
			pc.give_item2(72001, 1) --Anillo de experiencia
			pc.give_item2(72320, 2) --Ampliación de inventario
			pc.give_item2(88960, 4) -- Complemento especial para el pet
			pc.give_item2(25041, 5) -- Piedra mágica
			pc.give_item2(88922, 1) -- Lucha crítica
			pc.give_item2(88923, 1) -- Pelea perforante
			pc.give_item2(88924, 1) -- Ataque del Dios Dragón
			pc.give_item2(88925, 1) -- Defensa del Dios Dragón
			pc.give_item2(88926, 1) -- Intel. del Dios Dragón
			pc.give_item2(88927, 1) -- Vida del Dios Dragón
			pc.give_item2(71084, 600) -- Objeto Encantado
			pc.give_exp2(10000000)
			--pc.change_gold(80000000)
			say_title(string.format("%s:", mob_name(20354)))
			say("")
			local lang = pc.get_language()
			say(string.format(gameforge[lang].main_quest._51, pc.get_name()))
			say(gameforge[lang].main_quest._52)
			say("")
			say_reward(gameforge[lang].main_quest._city2_Jefe_reward_01)
			say_reward(gameforge[lang].main_quest._city2_Jefe_reward_02)
			say_reward(gameforge[lang].main_quest._city2_Jefe_reward_03)
			say_reward(gameforge[lang].main_quest._city2_Jefe_reward_04)
			say_reward(gameforge[lang].main_quest._city2_Jefe_reward_05)
			say_reward(gameforge[lang].main_quest._city2_Jefe_reward_06)
			say_reward(gameforge[lang].main_quest._city2_Jefe_reward_07)
			say_reward(gameforge[lang].main_quest._city2_Jefe_reward_08)
			say_reward(gameforge[lang].main_quest._city2_Jefe_reward_09)
			say_reward(gameforge[lang].main_quest._city2_Jefe_reward_10)
			say_reward(gameforge[lang].main_quest._city2_Jefe_reward_11)
			say_reward(gameforge[lang].main_quest._city2_Jefe_reward_12)
			--say_reward(gameforge[lang].main_quest._75)
			say_reward(gameforge[lang].main_quest._city2_Jefe_reward_13)
			say_reward(gameforge[lang].main_quest._city2_Jefe_reward_14)
			say_reward(gameforge[lang].main_quest._mission_orc_reward_critc)
			say_reward(gameforge[lang].main_quest._mission_orc_reward_critc1)
			say_reward(gameforge[lang].main_quest._mission_orc_reward_critc2)
			say_reward(gameforge[lang].main_quest._mission_orc_reward_critc3)
			say_reward(gameforge[lang].main_quest._mission_orc_reward_critc4)
			say_reward(gameforge[lang].main_quest._mission_orc_reward_critc5)
			say_reward(gameforge[lang].main_quest._mission_orc_reward_exp40_esp)
			--say_reward(gameforge[lang].main_quest._70)
			pc.delqf("491")
			pc.delqf("492")
			pc.delqf("493")
			pc.delqf("494")
			pc.delqf("591")
			set_quest_state("main_quest_lv45", "run")
			set_state(__COMPLETE__)
			clear_letter()
		end
	end

	state __COMPLETE__ begin
	end
end

