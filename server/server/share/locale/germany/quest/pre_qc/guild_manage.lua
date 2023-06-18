quest guild_building begin
	state start begin
		when	11000.chat.gameforge[pc.get_language()].guild.manage or
				11002.chat.gameforge[pc.get_language()].guild.manage or
				11004.chat.gameforge[pc.get_language()].guild.manage
				with pc.hasguild() and not pc.isguildmaster() and (pc.is_gm() or npc.empire == pc.empire) begin
			local lang = pc.get_language()
			-- local registered = MeleyLair.IsRegistered()
			-- local result1, result2 = MeleyLair.IsCooldown()
			-- if registered then
				-- say(string.format(gameforge[pc.get_language()].guild.manage18))
				-- return
			-- end
			-- if result1 == 7 then
				-- local hours = string.format("%02.f", math.floor(result2 / 3600));
				-- local minutes = string.format("%02.f", math.floor(result2 / 60 - (hours * 60)));
				-- local seconds = string.format("%02.f", math.floor(result2 - hours * 3600 - minutes * 60));
				-- local timeConv = string.format(hours..":"..minutes..":"..seconds)
				-- say(string.format(gameforge[pc.get_language()].guild.manage19, timeConv))
				-- return
			-- end
			-- ??
			say_title(gameforge[pc.get_language()].guild.manage1)
			say("")
			say(gameforge[pc.get_language()].guild.manage2)
			say(gameforge[pc.get_language()].guild.manage3)
			say("")
			say(gameforge[pc.get_language()].guild.manage4)
			say(gameforge[pc.get_language()].guild.manage5)
			say("")
			local s = select(gameforge[pc.get_language()].common.yes, gameforge[pc.get_language()].common.no)
			if s==1 then
				say_title(gameforge[pc.get_language()].guild.manage1)
				say("")
				say(gameforge[pc.get_language()].guild.manage6)
				say("")
				say(gameforge[pc.get_language()].guild.manage7)
				say("")
				pc.remove_from_guild()
				-- pc.setqf("new_withdraw_time",get_global_time())
			end
		end
		when	11000.chat.gameforge[pc.get_language()].guild.manage8 or
				11002.chat.gameforge[pc.get_language()].guild.manage8 or
				11004.chat.gameforge[pc.get_language()].guild.manage8
				with pc.hasguild() and pc.isguildmaster() and (pc.is_gm() or npc.empire == pc.empire) begin
			local lang = pc.get_language()
			-- local registered = MeleyLair.IsRegistered()
			-- local result1, result2 = MeleyLair.IsCooldown()
			-- if registered then
				-- say(string.format(gameforge[pc.get_language()].guild.manage16))
				-- return
			-- end
			-- if result1 == 7 then
				-- local hours = string.format("%02.f", math.floor(result2 / 3600));
				-- local minutes = string.format("%02.f", math.floor(result2 / 60 - (hours * 60)));
				-- local seconds = string.format("%02.f", math.floor(result2 - hours * 3600 - minutes * 60));
				-- local timeConv = string.format(hours..":"..minutes..":"..seconds)
				-- say(string.format(gameforge[pc.get_language()].guild.manage17, timeConv))
				-- return
			-- end
			-- ??
			say_title(gameforge[pc.get_language()].guild.manage1)
			say("")
			say(gameforge[pc.get_language()].guild.manage9)
			say("[DELAY value;150]        [/DELAY]")
			say(gameforge[pc.get_language()].guild.manage10)
			say(gameforge[pc.get_language()].guild.manage11)
			say(gameforge[pc.get_language()].guild.manage12)
			say("")
			say(gameforge[pc.get_language()].guild.manage13)
			say("")
			local s = select(gameforge[pc.get_language()].common.yes, gameforge[pc.get_language()].common.no)
			if s == 1 then
				say_title(gameforge[pc.get_language()].guild.manage1)
				say("")
				say(gameforge[pc.get_language()].guild.manage14)
				say("")
				say(gameforge[pc.get_language()].guild.manage15)
				say("")
				pc.destroy_guild()
				-- pc.setqf("new_disband_time", get_global_time())
				-- pc.setqf("new_withdraw_time", get_global_time())
			end
		end
	end
end
