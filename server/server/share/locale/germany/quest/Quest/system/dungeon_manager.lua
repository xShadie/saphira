quest dungeon_manager begin
	state start begin
		when login begin
			dungeonlib.update()
		end
		
		when button begin
			cmdchat("GetDungeonInfo INPUT#1")
			local cmd = split(input(cmdchat("GetDungeonInfo CMD#")), "#")
			cmdchat("GetDungeonInfo INPUT#0")
			if cmd[1] == "WARP" then
				--if pc.in_dungeon() and pc.get_map_index() >= 10000 then
				--	syschat(gameforge[pc.get_language()].dungeon_manager._1)
				--	return
				--else
				pc.warp(tonumber(cmd[2]) * 100, tonumber(cmd[3]) * 100)
				--end
			elseif cmd[1] == "RANKING" then
				dungeonlib.update_ranking(tonumber(cmd[2]), tonumber(cmd[3]))
			end
		end
	end
end

