quest main_quest_lv20_Jefes_test begin
	state start begin
		when enter or login with pc.get_name() == "Thor" begin
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
			if c <= 0 then
				return
			else
				pc.setqf(n, c - 1)
			end
			
			if pc.getqf(n) <= 0 then
				chat(string.format("%s Terminado", mob_name(n)))
			end
			
			local t =	pc.getqf("191") +
						pc.getqf("192") +
						pc.getqf("193") +
						pc.getqf("194") +
						pc.getqf("394")
			if t <= 0 then
				chat("Has Finalizado todos.")
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

