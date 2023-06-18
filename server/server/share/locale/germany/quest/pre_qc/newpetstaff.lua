quest newpetstuff begin
	state start begin
		function book_skill(value)
			return value - 55009
		end
		
		when 55010.use or 55011.use or 55012.use or 55013.use or 55014.use or 55015.use or 55016.use or 55017.use or 55018.use or 55019.use or 55020.use or 55021.use or 55022.use or 55023.use or 55024.use or 55025.use or 55026.use or 55027.use begin
			if not pc.can_warp() then
				syschat(gameforge[pc.get_language()].reset_scroll._35)
				return
			end
			
			if newpet.getlevel() >= 80 and  newpet.getevo() == 3 then
				if newpet.increaseskill(newpetstuff.book_skill(item.get_vnum())) then
					pc.remove_item(item.get_vnum(), 1)
				end
			end
		end
	end
end

