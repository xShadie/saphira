quest evento_busqueda_cofre begin
	state start begin
		function Mapas()
			return
					{
						{["Mapa"] = "Desierto-Yongbi",	["Index"] = 63},
						{["Mapa"] = "Valle de Orcos",	["Index"] = 64},
						{["Mapa"] = "Bosque fantasma",	["Index"] = 67}
						-- [63] = "Desierto-Yongbi",
						-- [64] = "Valle de Orcos",
						-- [67] = "Bosque fantasma"
					}
		end
		when 20095.chat."Evento busqueda de cofres" with pc.is_gm() begin
			say("En que mapa quieres realizar el evento:")
			local info = evento_busqueda_cofre.Mapas()
			local mapas = {}
			local idx = {}
			for i = 1, table.getn(info) do
				table.insert(mapas,info[i]["Mapa"])
				table.insert(idx,info[i]["Index"])
			end
			table.insert(mapas,"Salir")
			local s = select_table(mapas)
			if s != table.getn(mapas) then
				if pc.get_map_index() != idx[s] then
					say("Debes estar en "..mapas[s].." para iniciar el evento.")
					return
				end
				game.set_event_flag("evento_cofres",5)
				regen_in_map(idx[s],"data/evento/cofres/"..idx[s]..".txt")
				game.set_event_flag("evento_cofres_mapa",idx[s])
				game.set_event_flag("evento_cofres_mapa2",s)
				notice_all("<Sistema - Evento busqueda de cofres> El evento ha iniciado, ve a "..mapas[s].." y busca los cofres.")
			end
			-- local mapa = 0
			-- if mapas == 3 then
				-- setskin(NOWINDOW)
				-- return
			-- elseif mapas == 1 then
				-- mapa = 64
				-- if pc.get_map_index() != mapa then
					-- say("Debes estar en el mapa en el que se realizara el evento para iniciarlo.")
					-- return
				-- end
				-- game.set_event_flag("evento_cofres",5)
				-- regen_in_map(mapa,"data/evento/cofres/valle.txt")
			-- elseif mapas == 2 then
				-- mapa = 63
				-- if pc.get_map_index() != mapa then
					-- say("Debes estar en el mapa en el que se realizara el evento para iniciarlo.")
					-- return
				-- end
				-- game.set_event_flag("evento_cofres",5)
				-- regen_in_map(mapa,"data/evento/cofres/desierto.txt")
			-- elseif mapas == 3 then
				-- mapa = 63
				-- if pc.get_map_index() != mapa then
					-- say("Debes estar en el mapa en el que se realizara el evento para iniciarlo.")
					-- return
				-- end
				-- game.set_event_flag("evento_cofres",5)
				-- regen_in_map(mapa,"data/evento/cofres/desierto.txt")

			-- end
			-- game.set_event_flag("evento_cofres_mapa",mapa)
			-- notice_all("<Sistema - Evento busqueda de cofres> El evento ha iniciado, ve a "..evento_busqueda_cofre.Mapas()[mapa].." y busca los cofres.")
		end
		when 9300.kill begin
			local cofres = game.get_event_flag("evento_cofres")-1
			game.set_event_flag("evento_cofres",cofres)
			mysql_direct_query("UPDATE player.player SET puntos_moon=puntos_moon+10 WHERE id = "..pc.get_player_id().." ;")
			syschat("<Sistema> Has recibido 10 puntos moon.")
			pc.give_item2(90072,1)
			if cofres != 0 then
				notice_all("<Sistema - Evento busqueda de cofres> "..pc.get_name().." ha encontrado un cofre, quedan "..cofres..".")
			else
				notice_all("<Sistema - Evento busqueda de cofres> "..pc.get_name().." ha encontrado el ultimo cofre, evento finalizado.")
			end
		end
		when login with game.get_event_flag("evento_cofres") != 0 begin
			local info = evento_busqueda_cofre.Mapas()
			notice("<Sistema - Evento busqueda de cofres> El evento ha iniciado, ve a "..info[game.get_event_flag("evento_cofres_mapa2")]["Mapa"].." quedan "..game.get_event_flag("evento_cofres").." cofres.")
		end
	end
end