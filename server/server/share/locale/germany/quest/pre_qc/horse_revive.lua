quest horse_revive begin
	state start begin
		when 20349.chat.gameforge[pc.get_language()].chat_npc_translate._1 begin
			command("cube open")
			setskin(NOWINDOW)
		end

		when 20349.chat.gameforge[pc.get_language()].chat_npc_translate._2 with horse.get_grade() == 3 and horse.is_dead() begin
			say_title(string.format("%s:", mob_name(npc.get_race())))
			say("")
			local lang = pc.get_language()
			say(gameforge[lang].horse_revive._1)
			say(gameforge[lang].horse_revive._2)
			select(gameforge[lang].common.wait)
			say(gameforge[lang].horse_revive._3)
			say("[DELAY value=400]. . .[/DELAY]")
			horse.summon()
			select(gameforge[lang].common.wait)
			say(gameforge[lang].horse_revive._4)
			say(gameforge[lang].horse_revive._5)
			horse.revive()
		end

		when 20349.chat.gameforge[pc.get_language()].chat_npc_translate._2 with horse.get_grade() == 2 and horse.is_dead() begin
			say_title(string.format("%s:", mob_name(npc.get_race())))
			say("")
			local lang = pc.get_language()
			say(gameforge[lang].horse_revive._1)
			say(gameforge[lang].horse_revive._2)
			select(gameforge[lang].common.wait)
			say(gameforge[lang].horse_revive._3)
			say("[DELAY value=400]. . .[/DELAY]")
			horse.summon()
			select(gameforge[lang].common.wait)
			say(gameforge[lang].horse_revive._4)
			say(gameforge[lang].horse_revive._5)
			horse.revive()
		end

		when 20349.chat.gameforge[pc.get_language()].chat_npc_translate._2 with horse.get_grade() == 1 and horse.is_dead() begin
			say_title(string.format("%s:", mob_name(npc.get_race())))
			say("")
			local lang = pc.get_language()
			say(gameforge[lang].horse_revive._1)
			say(gameforge[lang].horse_revive._2)
			select(gameforge[lang].common.wait)
			say(gameforge[lang].horse_revive._3)
			say("[DELAY value=400]. . .[/DELAY]")
			horse.summon()
			select(gameforge[lang].common.wait)
			say(gameforge[lang].horse_revive._4)
			say(gameforge[lang].horse_revive._5)
			horse.revive()
		end
	end
end

