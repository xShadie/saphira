quest rune begin
	state start begin
		when login or levelup with pc.get_level() >= 70 begin
			local lang = pc.get_language()
			syschat(gameforge[lang].rune._1)
			syschat(gameforge[lang].rune._2)
			cmdchat("rune 2")
			set_state(gotoinfo)
		end
	end

	state gotoremind begin
		when login or levelup with pc.get_level() >= 70 begin
			local lang = pc.get_language()
			syschat(gameforge[lang].rune._1)
			syschat(gameforge[lang].rune._2)
			cmdchat("rune 2")
			set_state(gotoinfo)
		end
	end

	state gotoinfo begin
		when login begin
			if pc.get_level() >= 70 then
				cmdchat("rune 2")
			else
				set_state(gotoremind)
			end
		end

		when letter begin
			send_letter(gameforge[pc.get_language()].rune._1)
		end

		when button or info begin
			local lang = pc.get_language()
			say_title(gameforge[lang].rune._4)
			say("")
			say(string.format(gameforge[lang].rune._5, pc.get_name()))
			say(gameforge[lang].rune._6)
			say(gameforge[lang].rune._7)
			say(gameforge[lang].rune._8)
			say(gameforge[lang].rune._9)
			say("")
			say(string.format(gameforge[lang].rune._10))
			say(string.format(gameforge[lang].rune._11, item_name(88977)))
			say(gameforge[lang].rune._12)
			wait()
			say_title(gameforge[lang].rune._4)
			say("")
			say(string.format(gameforge[lang].rune._15, item_name(88980)))
			say(string.format(gameforge[lang].rune._13, item_name(88977)))
			say(gameforge[lang].rune._14)
			cmdchat("rune 1")
			--pc.give_item2(88970, 1)
			--pc.give_item2(88971, 1)
			--pc.give_item2(88972, 1)
			--pc.give_item2(88973, 1)
			--pc.give_item2(88974, 1)
			--pc.give_item2(88975, 1)
			--pc.give_item2(88976, 1)
			clear_letter()
			set_state(__COMPLETE__)
		end
	end

	state __COMPLETE__ begin
		when login begin
			if pc.get_level() >= 70 then
				cmdchat("rune 1")
			else
				set_state(gotoinfo)
			end
		end
	end
end

