quest sash begin
	state start begin
		when 60003.chat.gameforge[pc.get_language()].chat_npc_translate._14 begin
			setskin(NOWINDOW)
			npc.open_shop(58)
		end
		
		when 60003.chat.gameforge[pc.get_language()].chat_npc_translate._15 begin
			say_title(string.format("%s:", mob_name(60003)))
			say("")
			local lang = pc.get_language()
			say(gameforge[lang].sash._1)
			local confirm = select(gameforge[lang].common.yes, gameforge[lang].common.no)
			if confirm == 2 then
				return
			end
			
			pc.open_sash(true)
			setskin(NOWINDOW)
		end

		when 60003.chat.gameforge[pc.get_language()].chat_npc_translate._16 begin
			say_title(string.format("%s:", mob_name(60003)))
			say("")
			local lang = pc.get_language()
			say(gameforge[lang].sash._2)
			say(gameforge[lang].sash._3)
			local confirm = select(gameforge[lang].common.yes, gameforge[lang].common.no)
			if confirm == 2 then
				return
			end
			
			pc.open_sash(false)
			setskin(NOWINDOW)
		end
		
		when 60003.chat.gameforge[pc.get_language()].chat_npc_translate._17 begin
			say_title(string.format("%s:", mob_name(20406)))
			say("")
			local lang = pc.get_language()
			say(gameforge[lang].attr_transfer._1)
			say(gameforge[lang].attr_transfer._2)
			say("")
			say(gameforge[lang].attr_transfer._3)
			say(gameforge[lang].attr_transfer._4)
			say("")
			say(gameforge[lang].attr_transfer._5)
			say("")
			say(gameforge[lang].attr_transfer._6)
			say("")
			local confirm = select(gameforge[lang].common.yes, gameforge[lang].common.no)
			if confirm == 2 then
				return
			end
			
			command("attrtransfer open")
			setskin(NOWINDOW)
		end
	end
end

