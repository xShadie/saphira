quest item_attr_mission begin
	state start begin
		function data()
			return {
				level_req = {20, 120},
				ask_attr = {
					--se elegirá aleatorio
					[1] = {{5, 12}, {21, 20}, {22, 20}, {15, 15}},
					[2] = {{5, 12}, {21, 20}, {22, 20}, {16, 15}},
					[3] = {{5, 12}, {21, 20}, {22, 20}, {17, 10}},
					[4] = {{5, 12}, {21, 20}, {22, 20}, {9, 20}},
					[5] = {{5, 12}, {21, 20}, {22, 20}, {12, 10}}
				},
				rewards = { --recompensa de un objeto aleatorio de la lista
					--item_vnum, item_count
					{27992, 3},
					{27993, 2},
					{27994, 1}
				}
			}
		end
		
		function isLevelRequired()
			return pc.level >= item_attr_mission.data().level_req[1] and pc.level <= item_attr_mission.data().level_req[2]
		end
		
		function setAttrRandom()
			local data = item_attr_mission.data()
			local rand = number(1, table.getn(data.ask_attr))
			for i = 1, table.getn(data.ask_attr[rand]) do
				pc.setqf(string.format("attr_type_%s", i), data.ask_attr[rand][i][1])
				pc.setqf(string.format("attr_value_%s", i), data.ask_attr[rand][i][2])
			end
			pc.setqf("id_ask_elected", rand)
		end
		
		function itemIsCorrect(bonus_list)
			local item_bonus = item_attr_mission.castAttrFormat()
			local count_required = table.getn(bonus_list)
			local count = 0
			for i = 1, 5 do
				if item_attr_mission.isBonusRight(item_bonus[i][1], item_bonus[i][2], bonus_list) then
					count = count+1
				end
			end
			return count == count_required
		end
		
		function castAttrFormat()
			local list_get_attr0 = item.get_attr0()
			local t = {}
			for i = 1, table.getn(list_get_attr0)/2 do
				table.insert(t, {})
			end
			for i = 1, table.getn(list_get_attr0) do
				table.insert(t[math.ceil(i/2)], list_get_attr0[i])
			end
			return t
		end

		function isBonusRight(id, value, list)
			for i = 1, table.getn(list) do
				if id == list[i][1] and value == list[i][2] then
					return true
				end
			end
			return false
		end
		
		when enter or login or levelup with item_attr_mission.isLevelRequired() begin
			if pc.getqf("saved_day") != tonumber(os.date("%d")) then
				item_attr_mission.setAttrRandom()
				pc.setqf("saved_day", tonumber(os.date("%d")))
				set_state(state_1)
			end
		end
	end
	
	state state_1 begin
		when letter begin
			local v = find_npc_by_vnum(20026)
			if v != 0 then
				target.vid("__TARGET__", v, "El poder preciso")
			end
			send_letter("El poder preciso")
		end
		
		-- when button or info begin
			-- say_title("El poder preciso")
			-- say("Busca al alquimista, hoy tiene una[ENTER]misión para ti.")
		-- end
		
		function sayAttr()
			local data = item_attr_mission.data()
			if pc.getqf("id_ask_elected") <= 0 then
				item_attr_mission.setAttrRandom()
			end
			for i = 1, table.getn(data.ask_attr[pc.getqf("id_ask_elected")]) do
				say_reward(
					string.format(
						"%s +%s%s",
						apply_human[pc.getqf(string.format("attr_type_%s", i))][2],
						pc.getqf(string.format("attr_value_%s", i)),
						apply_human[pc.getqf(string.format("attr_type_%s", i))][3]
					)
				)
			end
		end
		
		when 20026.chat."El poder preciso" begin
			target.delete("__TARGET__")
			say_title(mob_name(npc.get_race()))
			say("Necesito que me consigas un objeto[ENTER]con estos valores exactos:")
			item_attr_mission.sayAttr()
			say("Debe ser exacto porque lo requiero para[ENTER]una operación precisa.[ENTER]Pon el objeto encima de mí. Ayúdame")
			pc.setqf("agree", 1)
		end
		
		when letter begin
			local v = find_npc_by_vnum(20026)
			if v != 0 then
				target.vid("__TARGET__", v, "El poder preciso")
			end
			send_letter("El poder preciso")
		end
		
		when button or info begin
			say_title("El poder preciso")
			if pc.getqf("agree") == 1 then
				say_title(mob_name(npc.get_race()))
				say("Quiere un ítem con estos valores exactos:")
				item_attr_mission.sayAttr()
			else
				say("Busca al alquimista, hoy tiene una[ENTER]misión para ti.")
			end
		end
		
		function AttrQfToTable()
			local data = item_attr_mission.data()
			local t = {}
			for i = 1, table.getn(data.ask_attr[pc.getqf("id_ask_elected")]) do
				table.insert(t, {})
				table.insert(t[i], pc.getqf(string.format("attr_type_%s", i)))
				table.insert(t[i], pc.getqf(string.format("attr_value_%s", i)))
			end
			return t
		end
		
		when 20026.take begin
			target.delete("__TARGET__")
			if item.get_attr0()[1] <= 0 then
				syschat("<El poder preciso> el objeto no tiene bonificaciones")
				return
			end
			if item_attr_mission.itemIsCorrect(item_attr_mission.AttrQfToTable()) then
				say_title(mob_name(npc.get_race()))
				say("¡Felicidades![ENTER]Me has cumplido, y por eso te daré una gran[ENTER]recompensa, lo que esté a mi alcance.[ENTER][ENTER]Revisa tu pergamino...")
				item.remove()
				wait()
				pc.setqf("agree", 0)
				set_state(state_2)
			else
				syschat("Misión El poder preciso: el objeto no cumple con los requisitos.")
			end
		end
		
		when login or levelup begin
			if pc.getqf("saved_day") != tonumber(os.date("%d")) then
				set_state(start)
			end
		end
	end
	
	state state_2 begin
		when letter begin
			send_letter("Reclama tu recompensa")
		end
		
		when button or info begin
			local data = item_attr_mission.data()
			say_title("El poder preciso")
			say("Recibirás la recompensa por haber[ENTER]terminado la misión.[ENTER]")
			say_reward("Esta misión se puede hacer cada día.")
			wait()
			
			local rand = number(1, table.getn(data.rewards))
			pc.give_item2(data.rewards[rand][1], data.rewards[rand][2])
			
			clear_letter()
			set_state(start)
		end
	end
end