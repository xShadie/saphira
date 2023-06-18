quest zodiaco_in_arrivo begin
	state start begin
		when 20438.chat.gameforge[pc.get_language()].chat_npc_translate._18 begin
			say_title(string.format("%s:", mob_name(20438)))
			say("")
			local lang = pc.get_language()
			say(gameforge[lang].zodiaco_in_arrivo._1)
			say("")
			say(gameforge[lang].zodiaco_in_arrivo._2)
			say(gameforge[lang].zodiaco_in_arrivo._3)
			say("")
			say(gameforge[lang].zodiaco_in_arrivo._4)
			say("")
			say(gameforge[lang].zodiaco_in_arrivo._5)
		end
	end
end

