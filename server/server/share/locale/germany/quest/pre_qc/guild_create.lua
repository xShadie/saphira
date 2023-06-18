quest guild_create begin
	state start begin
		when	11000.chat.gameforge[pc.get_language()].guild.create1 or 
				11002.chat.gameforge[pc.get_language()].guild.create1 or 
				11004.chat.gameforge[pc.get_language()].guild.create1 
				with not pc.hasguild() and not pc.isguildmaster() begin
			say_title(gameforge[pc.get_language()].guild.create2)
			say(gameforge[pc.get_language()].guild.create3)
			say_reward(gameforge[pc.get_language()].guild.create4)
			local x = select(gameforge[pc.get_language()].common.yes,gameforge[pc.get_language()].common.no)
			if x == 1 then
				say(gameforge[pc.get_language()].guild.create5)
				local guild_name = string.gsub(input(), "[^A-Za-z0-9]", "")
				local guild_len_name = string.len(guild_name)
				if pc.get_level() < 40 then
					say_reward(gameforge[pc.get_language()].guild.create6)
					return
				elseif pc.get_gold() < 5000000 then
					say_reward(gameforge[pc.get_language()].guild.create7)
					return
				elseif pc.hasguild() or pc.isguildmaster() then
					say_reward(gameforge[pc.get_language()].guild.create8)
					return
				else
					local ret = pc.make_guild0(guild_name)
					if ret == -2 then
						say_reward(gameforge[pc.get_language()].guild.create9)
					elseif ret == -1 then
						say_reward(gameforge[pc.get_language()].guild.create10)
					elseif ret == 0 then
						say_reward(gameforge[pc.get_language()].guild.create11)
					elseif ret == 1 then
						pc.change_gold(- 5000000 )
						say_reward(gameforge[pc.get_language()].guild.create12)
					elseif ret == 2 then
						say_reward(gameforge[pc.get_language()].guild.create13)
					elseif ret == 3 then
						say_reward(gameforge[pc.get_language()].guild.create14)
					end
				end
			elseif x == 2 then
				return
			end
		end
	end
end
