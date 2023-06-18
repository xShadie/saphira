quest official_gaya begin
	state start begin
		when 20503.chat.gameforge[pc.get_language()].chat_npc_translate._12 begin
			npc.open_shop(55)
			setskin(NOWINDOW)
		end
		
		when 20503.chat.gameforge[pc.get_language()].chat_npc_translate._13 begin
			say_title(string.format("%s:", mob_name(20503)))
			say("")
			local lang = pc.get_language()
			say(gameforge[lang].official_gaya._1)
			if pc.count_item(50926) >= 10 then
				say(gameforge[lang].official_gaya._2)
				say(gameforge[lang].official_gaya._3)
				say(gameforge[lang].official_gaya._4)
				say("")
				say(gameforge[lang].official_gaya._5)
				say("")
				local option = select(gameforge[lang].official_gaya._6, gameforge[lang].official_gaya._7)
				if option == 1 then
					game.open_gaya()
				end
			else
				say(gameforge[lang].official_gaya._8)
				say(gameforge[lang].official_gaya._9)
				return
			end
		end
		
		when 20504.click begin
			game.open_gaya_market()
		end
	end
end

