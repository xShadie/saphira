quest main_quest_lv65 begin
	state start begin
		when 9010.chat.gameforge[pc.get_language()].main_quest._black_sohan_mission_I begin
			say("Hola")
		end
	end
end
