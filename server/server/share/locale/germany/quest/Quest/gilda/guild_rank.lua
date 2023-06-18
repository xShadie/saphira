quest guild_rank begin
	state start begin
		when login begin
			if pc.has_guild() then
				if  guild.get_wins() == 0 then
					pc.setqf("guild_rewards", 0)
				end
				
				if guild.get_wins() >= 5 and pc.getqf("guild_rewards") == 0 then
					pc.setqf("guild_rewards", 1)
					pc.give_gaya(1)
					syschat(gameforge[pc.get_language()].guild_rank._1)
				end
				
				if guild.get_wins() >= 10 and pc.getqf("guild_rewards") == 1 then
					pc.setqf("guild_rewards", 2)
					pc.give_gaya(3)
					syschat(gameforge[pc.get_language()].guild_rank._2)
				end
				
				if guild.get_wins() >= 15 and pc.getqf("guild_rewards") == 2 then
					pc.setqf("guild_rewards", 3)
					pc.give_gaya(15)
					syschat(gameforge[pc.get_language()].guild_rank._3)
				end
			end
			
			cmdchat("callGuildRankId "..q.getcurrentquestindex())
		end

		when button or info begin
			cmdchat("clearGuildRank")
			local rank = game.mysql_query("SELECT name, win, draw, loss, trophies FROM guild WHERE trophies >= 5 ORDER BY trophies DESC, win DESC, draw DESC, loss ASC, name ASC LIMIT 39")
			local arg = 0
			local trophies = 0
			local argento = 0
			for i = 1, table.getn(rank) do
				trophies = tonumber(rank[i][5])
				
				if i <= 7 then
					if trophies < 30 then
						arg = 0
					elseif trophies < 50 then
						arg = 1
						argento = argento + 1
					else
						arg = 2
					end
				end
				
				if i > 7 and i <= 19 then
					if trophies < 30 then
						arg = 0
					else
						if tonumber(i+argento) > 19 then
							arg = 0
						else
							arg = 1
						end
					end
				end
				
				if i > 19 then
					arg = 0
				end
				
				cmdchat(string.format("addGuildRank %d|%s|%s|%s|%s|%d", arg, rank[i][1], rank[i][2], rank[i][3], rank[i][4], trophies))
			end
			
			cmdchat("openGuildRank")
		end
	end
end

