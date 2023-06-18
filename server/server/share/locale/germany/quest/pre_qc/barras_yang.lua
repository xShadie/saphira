quest barras_yang begin
	state start begin
		when 22110.use or 22111.use or 22112.use or 22113.use or 22114.use begin
			local yang = {
				[22110] = 100000000,
				[22111] = 500000000,
				[22112] = 20000000000,
				[22113] = 100000000000,
				[22114] = 500000000000,
			}
			if pc.getgold() + yang[item.get_vnum()] < 100000000000000 then
				pc.changegold(yang[item.get_vnum()])
				pc.remove_item(item.get_vnum(), 1)
			else
				chat("You can't convert the ticket to yang because you exceed yang on inventory")
			end
		end
		when 9005.chat.gameforge[pc.get_language()].magazzino._20 begin
			say_title(gameforge[pc.get_language()].magazzino._21)
			say(gameforge[pc.get_language()].magazzino._22)
			local yangbar = {100000000, 500000000, 20000000000, 100000000000, 500000000000}
			local item = {22110, 22111, 22112, 22113, 22114}
			local bas = {"100kk","500kk","20kkk","100kkk","500kkk","Salir"}
			local s = select_table(bas)
			if s >= table.getn(bas) or s < 1 then return end
			if pc.get_gold() >= yangbar[s] then
				pc.give_item2(item[s], 1)
				pc.changegold(-yangbar[s])
			end
		end
	end
end
