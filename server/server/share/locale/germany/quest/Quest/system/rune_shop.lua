quest rune_shop begin
	state start begin
	
		function data()
			return {
				level_req = 70,
				rune_cost = {
					-- vnum de la runa = costos
					--[rune_vnum] = {{item_vnum_1, item_count_1}, {item_vnum_2, item_count_2}, ...}
					--[[Runa Ares]]				[88970] = {{88980, 300}, {30278, 1}},
					--[[Runa Era]]				[88971] = {{88980, 300}, {30278, 1}},
					--[[Runa Apollo]]			[88972] = {{88980, 300}, {30278, 1}},
					--[[Runa Afrodite]]			[88973] = {{88980, 300}, {30278, 1}},
					--[[Runa Poseidone]]		[88974] = {{88980, 300}, {30278, 1}},
					--[[Runa Atena]]			[88975] = {{88980, 300}, {30278, 1}},
					--[[Runa Zeus]]				[88976] = {{88980, 500}, {30280, 2}, {30278, 1}} --( esta es pvp asi que ponla mas cara que las demas )
				}
			}
		end
	
		function createTableSelectRunes()
			local data = rune_shop.data()
			local t_vnum = {}
			local t_name = {}
			local cont = 0
			for k, v in pairs(data.rune_cost) do
				local cell = 32+math.mod(k, 10)
				if pc.get_wear(cell) == nil then
					table.insert(t_vnum, k)
					table.insert(t_name, item_name(k))
				end
				cont = cont + 1
			end
			-- table.insert(t_vnum, gameforge[lang].common._close)
			-- table.insert(t_name, gameforge[lang].common._close)
			table.insert(t_vnum, "Close")
			table.insert(t_name, "Close")
			return t_vnum, t_name
		end
	
		function hasEnoughItems(rune_vnum)
			local data = rune_shop.data()
			for i = 1, table.getn(data.rune_cost[rune_vnum]) do
				local item_vnum = data.rune_cost[rune_vnum][i][1]
				local item_count = data.rune_cost[rune_vnum][i][2]
				if pc.count_item(item_vnum) < item_count then
					return false
				end
			end
			return true
		end
		
		function removeItemCost(rune_vnum)
			local data = rune_shop.data()
			for i = 1, table.getn(data.rune_cost[rune_vnum]) do
				local item_vnum = data.rune_cost[rune_vnum][i][1]
				local item_count = data.rune_cost[rune_vnum][i][2]
				pc.remove_item(item_vnum, item_count)
			end
		end
		
		function sayCost(rune_vnum)
			local data = rune_shop.data()
			for i = 1, table.getn(data.rune_cost[rune_vnum]) do
				local item_vnum = data.rune_cost[rune_vnum][i][1]
				local item_count = data.rune_cost[rune_vnum][i][2]
				say_reward(string.format("%s. x%s %s", i, item_count, item_name(item_vnum)))
			end
		end
		
		function hasAllRunes()
			local data = rune_shop.data()
			local cont = 0
			for k, v in pairs(data.rune_cost) do
				local cell = 32+math.mod(k, 10)
				if pc.get_wear(cell) == nil then
					return false
				end
			end
			return true
		end
	
		when letter begin
			local lang = pc.get_language()
			local v = find_npc_by_vnum(20026)
			if v != 0 then
				target.vid("__TARGET__", v, gameforge[lang].chat_npc_translate._9)
			end
			send_letter(gameforge[lang].chat_npc_translate._9)
		end
		
		when 20026.chat.gameforge[pc.get_language()].chat_npc_translate._9 begin
			target.delete("__TARGET__")
			local lang = pc.get_language()
			if rune_shop.hasAllRunes() then
				setskin(0)
				syschat(gameforge[lang].runa_shop._1) --"Ya tienes todas tus runas y ninguna está gastada."
				return
			end
			local data = rune_shop.data()
			if pc.level < data.level_req then
				setskin(0)
				syschat(string.format(gameforge[lang].runa_shop._2, data.level_req)) --"Debes ser al menos nivel %s para abrir la tienda."
				return
			end
			
			say_title(gameforge[lang].chat_npc_translate._9) --"Tienda de Runas"
			local runes_vnum, runes_name = rune_shop.createTableSelectRunes()
			local sel_rune_name = select_table(runes_name)
			if sel_rune_name >= table.getn(runes_name) then
				return
			end
			local rune_vnum = runes_vnum[sel_rune_name]
			say_title(string.format(gameforge[lang].runa_shop._3, item_name(rune_vnum))) --"Comprar %s"
			say(gameforge[lang].runa_shop._4) --"Objetos necesarios para la compra:"
			rune_shop.sayCost(rune_vnum)
			if select(gameforge[lang].runa_shop._buy, gameforge[lang].common._close) == 1 then
				if rune_shop.hasEnoughItems(rune_vnum) then
					rune_shop.removeItemCost(rune_vnum)
					pc.give_item2(rune_vnum)
				else
					syschat(gameforge[lang].runa_shop._5) --"No tienes el objetos necesarios."
				end
			end
		end
	end
end