quest arboreal_craft begin
	state start begin
		when 20413.chat.gameforge[pc.get_language()].chat_npc_translate._8 with pc.get_level() >= 90 begin
			command("cube open")
			setskin(NOWINDOW)
		end
	end
end

