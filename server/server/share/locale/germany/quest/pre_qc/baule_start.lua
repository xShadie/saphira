quest baule_start begin
	state start begin
		when 50187.use begin -- BAUL INICIO
			local lang = pc.get_language()
			if pc.get_level() >= 1 then
				if not pc.can_warp() then
					syschat(gameforge[lang].reset_scroll._35)
					return
				end
				
				pc.remove_item(50187, 1)
				chat(string.format(gameforge[lang].baule_start._1, item_name(50187)))
				chat(gameforge[lang].baule_start._2)
				pc.give_item2(50190, 1)--PROXIMO BAUL lvl 30
				pc.give_item2(72725, 1)--Elixir del Sol (G)
				pc.give_item2(72729, 1)--Elixir de la Luna (G)
				pc.give_item2(53256, 1)--PET (30dias) Inicio
				pc.give_item2(71127, 1)--MONTURA (30dias) Inicio
				pc.give_item2(70038, 1)--CAPA
				pc.give_item2(88928, 1)--Elixir de delocidad de ataque
				pc.give_item2(88918, 1)--Rocio blanco (Defensa)
				pc.give_item2(71085, 1)--Agregar 5 bonus
				pc.give_item2(71148, 1)--Anillo de fuerza de voluntad (30dias)
				pc.give_item2(88952, 1)--Lucha crítica (2h)
				pc.give_item2(88953, 1)--Pelea perforante (2h)
				pc.give_item2(88954, 1)--Ataque del Dios Dragón (2h)
				pc.give_item2(88955, 1)--Defensa del Dios Dragón (2h)
				pc.give_item2(88956, 1)--Inteligencia del Dios Dragón (2h)
				pc.give_item2(88957, 1)--Vida del Dios Dragón (2h)
				pc.give_item2(88947, 1)--Ampolla amarilla (2h) "Fuerza vs Metin"
				if pc.get_sex() == 0 then -- Male
					--Atuendos
					pc.give_item2(8500, 1) -- Costume Body
					pc.give_item2(8503, 1) -- Costume Hair
				elseif pc.get_sex() == 1 then -- Female
					pc.give_item2(8501, 1) -- Costume Body
					pc.give_item2(8502, 1) -- Costume Hair
				end
				local job = pc.get_job()
				if job == 0 then -- Warrior
					pc.give_item2(8109, 1)
					pc.give_item2(8129, 1)
				elseif job == 1 then -- Assassin
					pc.give_item2(8109, 1)
					pc.give_item2(8159, 1)
					pc.give_item2(8169, 1)
				elseif job == 2 then -- Sura
					pc.give_item2(8119, 1)
				elseif job == 3 then -- Shaman
					pc.give_item2(8139, 1)
					pc.give_item2(8149, 1)
				end
			else
				say_title(string.format("%s:", item_name(50187)))
				say("")
				say(string.format(gameforge[lang].baule_start._3, 1))
			end
		end



		when 50190.use begin -- BAUL LV30
			local lang = pc.get_language()
			if pc.get_level() >= 30 then
				if not pc.can_warp() then
					syschat(gameforge[lang].reset_scroll._35)
					return
				end
				
				pc.remove_item(50190, 1)
				chat(string.format(gameforge[lang].baule_start._1, item_name(50190)))
				chat(gameforge[lang].baule_start._2)
				pc.give_item2(50192, 1)--Proximo baul lvl 50
				pc.give_item2(88963, 2)--Encantar Disfraz x2
				pc.give_item2(88960, 1)--Bocadillo especial
				pc.give_item2(88952, 1)--Lucha crítica (2h)
				pc.give_item2(88953, 1)--Pelea perforante (2h)
				pc.give_item2(88954, 1)--Ataque del Dios Dragón (2h)
				pc.give_item2(88955, 1)--Defensa del Dios Dragón (2h)
				pc.give_item2(88956, 1)--Inteligencia del Dios Dragón (2h)
				pc.give_item2(88957, 1)--Vida del Dios Dragón (2h)
				pc.give_item2(88947, 1)--Ampolla amarilla (2h) "Fuerza vs Metin"
				pc.give_item2(88946, 1)--Ampolla Naranja (2h) "Fuerza vs Monstruo 10%"
				pc.give_item2(72004, 1)--Anillo Exp 1h No comerciable
				pc.give_item2(72007, 1)--Guante Ladrón 1h No comerciable
			else
				say_title(string.format("%s:", item_name(50190)))
				say("")
				say(string.format(gameforge[lang].baule_start._3, 30))
			end
		end
		
		when 50192.use begin -- BAUL LV50
			local lang = pc.get_language()
			if pc.get_level() >= 50 then
				if not pc.can_warp() then
					syschat(gameforge[lang].reset_scroll._35)
					return
				end
				
				pc.remove_item(50192, 1)
				chat(string.format(gameforge[lang].baule_start._1, item_name(50192)))
				chat(gameforge[lang].baule_start._2)
				pc.give_item2(50194, 1)--Proximo baul lvl 75
				pc.give_item2(88963, 3)--Encantar Disfraz x3
				pc.give_item2(88960, 3)--Bocadillo especial x3
				pc.give_item2(88952, 1)--Lucha crítica (2h)
				pc.give_item2(88953, 1)--Pelea perforante (2h)
				pc.give_item2(88954, 1)--Ataque del Dios Dragón (2h)
				pc.give_item2(88955, 1)--Defensa del Dios Dragón (2h)
				pc.give_item2(88956, 1)--Inteligencia del Dios Dragón (2h)
				pc.give_item2(88957, 1)--Vida del Dios Dragón (2h)
				pc.give_item2(88947, 1)--Ampolla amarilla (2h) "Fuerza vs Metin"
				pc.give_item2(88946, 1)--Ampolla Naranja (2h) "Fuerza vs Monstruo 10%"
				pc.give_item2(88948, 1)--Ampolla Azul (2h) "Fuerza vs Jefes 10%"
				pc.give_item2(30279, 5)--Cristal Solar x5
				pc.give_item2(30277, 5)--Cristal Dioses x5
				pc.give_item2(72004, 1)--Anillo Exp 1h No comerciable
				pc.give_item2(72007, 1)--Guante Ladrón 1h No comerciable
				
			else
				say_title(string.format("%s:", item_name(50192)))
				say("")
				say(string.format(gameforge[lang].baule_start._3, 50))
			end
		end
		
		when 50194.use begin -- BAUL LV75
			local lang = pc.get_language()
			if pc.get_level() >= 75 then
				if not pc.can_warp() then
					syschat(gameforge[lang].reset_scroll._35)
					return
				end
				
				pc.remove_item(50194, 1)
				chat(string.format(gameforge[lang].baule_start._1, item_name(50194)))
				chat(gameforge[lang].baule_start._2)
				pc.give_item2(50195, 1)--Proximo baul lvl 100
				pc.give_item2(88966, 2)--Refuerza los talismanes x2
				pc.give_item2(88963, 5)--Encantar Disfraz x5
				pc.give_item2(88965, 2)--Encantar talismanes x2
				pc.give_item2(88960, 5)--Bocadillo especial x5
				pc.give_item2(88952, 1)--Lucha crítica (2h)
				pc.give_item2(88953, 1)--Pelea perforante (2h)
				pc.give_item2(88954, 1)--Ataque del Dios Dragón (2h)
				pc.give_item2(88955, 1)--Defensa del Dios Dragón (2h)
				pc.give_item2(88956, 1)--Inteligencia del Dios Dragón (2h)
				pc.give_item2(88957, 1)--Vida del Dios Dragón (2h)
				pc.give_item2(88947, 1)--Ampolla amarilla (2h) "Fuerza vs Metin"
				pc.give_item2(88946, 1)--Ampolla Naranja (2h) "Fuerza vs Monstruo 10%"
				pc.give_item2(88948, 1)--Ampolla Azul (2h) "Fuerza vs Jefes 10%"
				pc.give_item2(88951, 1)--Ampolla Morado (2h) "HP +4000"
				pc.give_item2(30279, 5)--Cristal Solar x5
				pc.give_item2(30277, 5)--Cristal Dioses x5
				pc.give_item2(72004, 1)--Anillo Exp 1h No comerciable
				pc.give_item2(72007, 1)--Guante Ladrón 1h No comerciable
			else
				say_title(string.format("%s:", item_name(50194)))
				say("")
				say(string.format(gameforge[lang].baule_start._3, 75))
			end
		end
		
		when 50195.use begin -- BAUL LV100
			local lang = pc.get_language()
			if pc.get_level() >= 100 then
				if not pc.can_warp() then
					syschat(gameforge[lang].reset_scroll._35)
					return
				end
				
				pc.remove_item(50195, 1)
				chat(string.format(gameforge[lang].baule_start._1, item_name(50195)))
				chat(gameforge[lang].baule_start._2)
				pc.give_item2(50196, 1)--Proximo baul lvl 120
				pc.give_item2(88966, 5)--Refuerza los talismanes x5
				pc.give_item2(88963, 5)--Encantar Disfraz x5
				pc.give_item2(88965, 5)--Encantar talismanes x5
				pc.give_item2(88960, 5)--Bocadillo especial x5
				pc.give_item2(88952, 1)--Lucha crítica (2h)
				pc.give_item2(88953, 1)--Pelea perforante (2h)
				pc.give_item2(88954, 1)--Ataque del Dios Dragón (2h)
				pc.give_item2(88955, 1)--Defensa del Dios Dragón (2h)
				pc.give_item2(88956, 1)--Inteligencia del Dios Dragón (2h)
				pc.give_item2(88957, 1)--Vida del Dios Dragón (2h)
				pc.give_item2(88947, 1)--Ampolla amarilla (2h) "Fuerza vs Metin"
				pc.give_item2(88946, 1)--Ampolla Naranja (2h) "Fuerza vs Monstruo 10%"
				pc.give_item2(88948, 1)--Ampolla Azul (2h) "Fuerza vs Jefes 10%"
				pc.give_item2(88951, 1)--Ampolla Morado (2h) "HP +4000"
				pc.give_item2(30279, 10)--Cristal Solar x10
				pc.give_item2(30277, 10)--Cristal Dioses x10
				pc.give_item2(72004, 1)--Anillo Exp 1h No comerciable
				pc.give_item2(72007, 1)--Guante Ladrón 1h No comerciable
			else
				say_title(string.format("%s:", item_name(50195)))
				say("")
				say(string.format(gameforge[lang].baule_start._3, 100))
			end
		end


		when 50196.use begin -- BAUL LV120
			local lang = pc.get_language()
			if pc.get_level() >= 120 then
				if not pc.can_warp() then
					syschat(gameforge[lang].reset_scroll._35)
					return
				end
				
				pc.remove_item(50196, 1)
				chat(string.format(gameforge[lang].baule_start._1, item_name(50196)))
				chat(gameforge[lang].baule_start._2)
				pc.give_item2(88966, 10)--Refuerza los talismanes x10
				pc.give_item2(88963, 10)--Encantar Disfraz x10
				pc.give_item2(88965, 10)--Encantar talismanes x10
				pc.give_item2(88960, 10)--Bocadillo especial x10
				pc.give_item2(86071, 10)--Complemento de proteina x10
				pc.give_item2(88958, 2)--Pergamino del alma x2
				pc.give_item2(70402, 5)--Especial de Disfraz de brujas (5%) x5
				pc.give_item2(70605, 1)--Bonificación por congelación
				pc.give_item2(70606, 1)--Bonificación por descongelar
				pc.give_item2(88952, 1)--Lucha crítica (2h)
				pc.give_item2(88953, 1)--Pelea perforante (2h)
				pc.give_item2(88954, 1)--Ataque del Dios Dragón (2h)
				pc.give_item2(88955, 1)--Defensa del Dios Dragón (2h)
				pc.give_item2(88956, 1)--Inteligencia del Dios Dragón (2h)
				pc.give_item2(88957, 1)--Vida del Dios Dragón (2h)
				pc.give_item2(88946, 1)--Ampolla Naranja (2h) "Fuerza vs Monstruo 10%"
				pc.give_item2(88947, 1)--Ampolla amarilla (2h) "Fuerza vs Metin 10%"
				pc.give_item2(88948, 1)--Ampolla azul (2h) "Fuerza vs Jefe 10%"
				pc.give_item2(88949, 1)--Ampolla roja (2h) "Fuerza vs MedioHumanos 10%"
				pc.give_item2(88951, 1)--Ampolla violeta (2h) "Max HP +4000"
				pc.give_item2(30279, 20)--Cristal Solar x20
				pc.give_item2(30277, 20)--Cristal Dioses x20
			else
				say_title(string.format("%s:", item_name(50196)))
				say("")
				say(string.format(gameforge[lang].baule_start._3, 120))
			end
		end
-----------------------------------------------------------------------------------------------------------------------------------------

		-- when 50188.use begin
			-- local lang = pc.get_language()
			-- if pc.get_level() >= 10 then
				-- if not pc.can_warp() then
					-- syschat(gameforge[lang].reset_scroll._35)
					-- return
				-- end
				
				-- pc.remove_item(50188, 1)
				-- chat(string.format(gameforge[lang].baule_start._1, item_name(50188)))
				-- chat(gameforge[lang].baule_start._2)
				-- pc.give_item2(50190, 1)
				-- pc.give_item2(88966, 5)--Refuerza los talismanes x5
				-- pc.give_item2(88961, 5)--Encantar alquimia x5
				-- pc.give_item2(88963, 5)--Encantar Disfraz x5
				-- pc.give_item2(88965, 5)--Encantar talismanes x5
				-- pc.give_item2(88960, 1)-- Bocadillo especial
				-- pc.give_item2(88957, 1)
				-- pc.give_item2(88960, 1)
			-- else
				-- say_title(string.format("%s:", item_name(50188)))
				-- say("")
				-- say(string.format(gameforge[lang].baule_start._3, 10))
			-- end
		-- end

		-- when 50190.use begin
			-- local lang = pc.get_language()
			-- if pc.get_level() >= 30 then
				-- if not pc.can_warp() then
					-- syschat(gameforge[lang].reset_scroll._35)
					-- return
				-- end
				
				-- pc.remove_item(50190,1)
				-- chat(string.format(gameforge[lang].baule_start._1, item_name(50190)))
				-- chat(gameforge[lang].baule_start._2)
				-- pc.give_item2(50192, 1)
				-- pc.give_item2(88952, 1)
				-- pc.give_item2(88953, 1)
				-- pc.give_item2(88963, 1)
				-- pc.give_item2(88960, 1)
				-- pc.give_item2(88959, 100)
			-- else
				-- say_title(string.format("%s:", item_name(50190)))
				-- say("")
				-- say(string.format(gameforge[lang].baule_start._3, 30))
			-- end
		-- end

		-- when 50192.use begin
			-- local lang = pc.get_language()
			-- if pc.get_level() >= 50 then
				-- if not pc.can_warp() then
					-- syschat(gameforge[lang].reset_scroll._35)
					-- return
				-- end
				
				-- pc.remove_item(50192, 1)
				-- chat(string.format(gameforge[lang].baule_start._1, item_name(50192)))
				-- chat(gameforge[lang].baule_start._2)
				-- pc.give_item2(50194, 1)
				-- pc.give_item2(88954, 1)
				-- pc.give_item2(88955, 1)
				-- pc.give_item2(88956, 1)
				-- pc.give_item2(88957, 1)
				-- pc.give_item2(88963, 1)
				-- pc.give_item2(88946, 1)
				-- pc.give_item2(88941, 1)
				-- pc.give_item2(88942, 1)
				-- pc.give_item2(88943, 1)
				-- pc.give_item2(88944, 1)
				-- pc.give_item2(88945, 1)
			-- else
				-- say_title(string.format("%s:", item_name(50192)))
				-- say("")
				-- say(string.format(gameforge[lang].baule_start._3, 50))
			-- end
		-- end

		-- when 50194.use begin
			-- local lang = pc.get_language()
			-- if pc.get_level() >= 75 then
				-- if not pc.can_warp() then
					-- syschat(gameforge[lang].reset_scroll._35)
					-- return
				-- end
				
				-- pc.remove_item(50194, 1)
				-- chat(string.format(gameforge[lang].baule_start._1, item_name(50194)))
				-- chat(gameforge[lang].baule_start._2)
				-- pc.give_item2(50195, 1)
				-- pc.give_item2(88960, 1)
				-- pc.give_item2(88964, 1)
				-- pc.give_item2(88965, 1)
				-- pc.give_item2(88966, 1)
				-- pc.give_item2(88961, 1)
				-- pc.give_item2(88947, 1)
			-- else
				-- say_title(string.format("%s:", item_name(50194)))
				-- say("")
				-- say(string.format(gameforge[lang].baule_start._3, 75))
			-- end
		-- end

		-- when 50195.use begin
			-- local lang = pc.get_language()
			-- if pc.get_level() >= 100 then
				-- if not pc.can_warp() then
					-- syschat(gameforge[lang].reset_scroll._35)
					-- return
				-- end
				
				-- pc.remove_item(50195, 1)
				-- chat(string.format(gameforge[lang].baule_start._1, item_name(50195)))
				-- chat(gameforge[lang].baule_start._2)
				-- pc.give_item2(50196, 1)
				-- pc.give_item2(88964, 1)
				-- pc.give_item2(88965, 1)
				-- pc.give_item2(88966, 1)
				-- pc.give_item2(88958, 1)
				-- pc.give_item2(88962, 1)
				-- pc.give_item2(88948, 1)
			-- else
				-- say_title(string.format("%s:", item_name(50195)))
				-- say("")
				-- say(string.format(gameforge[lang].baule_start._3, 100))
			-- end
		-- end

		-- when 50196.use begin
			-- local lang = pc.get_language()
			-- if pc.get_level() >= 120 then
				-- if not pc.can_warp() then
					-- syschat(gameforge[lang].reset_scroll._35)
					-- return
				-- end
				
				-- pc.remove_item(50196, 1)
				-- chat(string.format(gameforge[lang].baule_start._1, item_name(50196)))
				-- chat(gameforge[lang].baule_start._2)
				-- pc.give_item2(88949, 1)
				-- pc.give_item2(88950, 1)
				-- pc.give_item2(88951, 1)
				-- pc.give_item2(88960, 1)
				-- pc.give_item2(88958, 1)
				-- pc.give_item2(88961, 1)
				-- pc.give_item2(88962, 1)
				-- pc.give_item2(88965, 1)
			-- else
				-- say_title(string.format("%s:", item_name(50196)))
				-- say("")
				-- say(string.format(gameforge[lang].baule_start._3, 120))
			-- end
		-- end
	end
end

