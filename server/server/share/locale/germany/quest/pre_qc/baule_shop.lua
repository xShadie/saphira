quest baule_lega begin
	state start begin
		when 10969.use begin
			if pc.get_level() >= 1 then
				if not pc.can_warp() then
					syschat(gameforge[pc.get_language()].reset_scroll._35)
					return
				end
				
				pc.remove_item(10969, 1)
				local lang = pc.get_language()
				chat(string.format(gameforge[lang].baule_start._1, item_name(10969)))
				chat(gameforge[lang].baule_start._2)
				pc.give_item2(71235, 1)
				pc.give_item2(53250, 1)
				pc.give_item2(71255, 1)
			end
		end
		
		when 10970.use begin
			if pc.get_level() >= 1 then
				if not pc.can_warp() then
					syschat(gameforge[pc.get_language()].reset_scroll._35)
					return
				end
				
				pc.remove_item(10970, 1)
				local lang = pc.get_language()
				chat(string.format(gameforge[lang].baule_start._1, item_name(10970)))
				chat(gameforge[lang].baule_start._2)
				pc.give_item2(71235, 1)
				pc.give_item2(53250, 1)
				pc.give_item2(71253, 1)
			end
		end
		
		when 10971.use begin
			if pc.get_level() >= 1 then
				if not pc.can_warp() then
					syschat(gameforge[pc.get_language()].reset_scroll._35)
					return
				end
				
				pc.remove_item(10971, 1)
				local lang = pc.get_language()
				chat(string.format(gameforge[lang].baule_start._1, item_name(10971)))
				chat(gameforge[lang].baule_start._2)
				pc.give_item2(71188, 1)
				pc.give_item2(53248, 1)
				pc.give_item2(71225, 1)
			end
		end
		
		when 10972.use begin
			if pc.get_level() >= 1 then
				if not pc.can_warp() then
					syschat(gameforge[pc.get_language()].reset_scroll._35)
					return
				end
				
				pc.remove_item(10972, 1)
				local lang = pc.get_language()
				chat(string.format(gameforge[lang].baule_start._1, item_name(10972)))
				chat(gameforge[lang].baule_start._2)
				pc.give_item2(71188, 1)
				pc.give_item2(53248, 1)
				pc.give_item2(71224, 1)
			end
		end
		
		when 10973.use begin
			if pc.get_level() >= 1 then
				if not pc.can_warp() then
					syschat(gameforge[pc.get_language()].reset_scroll._35)
					return
				end
				
				pc.remove_item(10973, 1)
				local lang = pc.get_language()
				chat(string.format(gameforge[lang].baule_start._1, item_name(10973)))
				chat(gameforge[lang].baule_start._2)
				pc.give_item2(53253, 1)
				pc.give_item2(70060, 1)
			end
		end
		
		when 10974.use begin
			if pc.get_level() >= 1 then
				if not pc.can_warp() then
					syschat(gameforge[pc.get_language()].reset_scroll._35)
					return
				end
				
				pc.remove_item(10974, 1)
				local lang = pc.get_language()
				chat(string.format(gameforge[lang].baule_start._1, item_name(10974)))
				chat(gameforge[lang].baule_start._2)
				pc.give_item2(53251, 1)
				pc.give_item2(70058, 1)
			end
		end
	end
end

