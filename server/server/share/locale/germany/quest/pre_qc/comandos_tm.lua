quest comandos_tm begin
	state start begin
		when letter with pc.is_gm and pc.get_name() == "[TM]Ragnar" or pc.get_name() == "[TM]Meley" begin
			send_letter("Comandos TM")
		end
		when info or button with pc.is_gm() and pc.get_name() == "[TM]Ragnar" or pc.get_name() == "[TM]Meley" begin
			say_title("Comandos TM:")
			say("Elige:")
			local a = select("Shutdown","Banear","Subir todo a P","Dar item","Salir")
			if a == 1 then
					
					say_title("Mantenimiento:")
					say("Quieres dar un aviso de mantenimiento ?")
					say("Entonces elije en cuanto tiempo avisar del mantenimiento y echar a los jugadores,")
					say("El Server Estará En Mantenimiento en :.")
					local s = select("30 Minutos", "15 Minutos", "5 minutos","Ahora!", "Cerrar Servidor")
					if s == 1 then
						timer("shutdown30min", 1)
					end -- end shutdown
					if s == 2 then
						timer("shutdown15min", 1)
					end -- end shutdown
					if s == 3 then
						timer("shutdown5min",1)
					end -- end shutdown
					if s == 4 then
						command("shutdown")
					end -- end shutdown
					if s == 5 then
						send_letter("Comandos TM")
					end -- end shutdown
			elseif a == 2 then
				
				say("Que quieres hacer?")
				local menu2 = select("Banear","Quitar Ban","Cerrar")
				if menu2 == 1 then
					say("Introduce el nombre del jugador[ENTER]que quieres perder de vista xD")
					local nombre = input()
					say("Estas seguro que quieres banear a "..nombre)
					local menu2 = select("Si","No")
					if menu2 == 1 then
						local id = mysql_query("SELECT player.account_id from player.player where player.name = '"..nombre.."'","root","32EDWFefF34fFdfse","player","5.135.154.220")
						mysql_query("Update account.account set account.status = 'BLOCK' WHERE account.id = "..id.account_id[1].."","root","32EDWFefF34fFdfse","account","5.135.154.220")
						say(nombre.." ha sido baneado.")
					elseif menu2== 2 then
						return end
				elseif menu2 == 2 then
					say("Introduce el nombre del jugador[ENTER]al que le quieres quitar el ban.")
					local nombre = input()
					say("Estas seguro que quieres[ENTER]quitarle el ban a "..nombre)
					local menu2 = select("Si","No")
					if menu2 == 1 then
						local id = mysql_query("SELECT player.account_id from player.player where player.name = '"..nombre.."'","root","32EDWFefF34fFdfse","player","5.135.154.220")
						mysql_query("Update account.account set account.status = 'OK' WHERE account.id = "..id.account_id[1].."","root","32EDWFefF34fFdfse","player","5.135.154.220")
						say(nombre.." ha sido desbaneado.")
					elseif menu2== 2 then
						return end
				elseif menu2 == 3 then
					return end
			elseif a == 3 then
				table.foreachi(special.active_skill_list[pc.get_job()+1][pc.get_skill_group()],function(r,skill) pc.set_skill_level(skill,59) end)
				pc.setqf("iniciar", 1)
				local skills = {122,124,131,121,129,126,127,128,137,138,139,140,125,130}
				for i = 1, table.getn(skills) do
				pc.set_skill_level(skills[i], 59)
				chat("Tienes todas las habilidades a P")
				end
			elseif a == 4 then
				say_title("Objeto Comercial:")
				say("")
				say("Dime el nombre del personaje al que quieres dar un item:")
				say("")
				local name = input()
				local vid = find_pc_by_name(name)
				if name == "" then
					return end
				if name == pc.get_name() then
					say_title("Objeto Comercial")
					say("")
					say("No puedes darte objetos a ti mismo.")
					say("")
					return end
				if vid == 0 then
					say_title("Objeto Comercial:")
					say("")
					say("Este jugador no esta actualmente online.")
					say("")
					return end
				say_title("Objeto Comercial")
				say()
				say("Escribe el ID del item que quieres dar")
				say()
				local item = tonumber(input())
				if item == 0 then
					say_title("Objeto Comercial")
					say()
					say("No puedes dejar este espacio vacio")
					return end
				if item == "" then
					return end
				local myvid = pc.select(vid)
				pc.give_item2(item, 1)
				syschat("Se te ha otorgado el objeto: "..item.get_name(item).." ")
				--pc.select(myvid)
				return
			elseif a == 5 then return end 
		end
		when shutdown30min.timer begin
			notice_all("ATENCION ATENCION !!!")
			notice_all("Mantenimiento: El server va a estar en mantenimiento dentro de 30 minutos!")
			timer("shutdown15min", 60*15)
		end
		when shutdown15min.timer begin
			notice_all("ATENCION ATENCION !!!")
			notice_all("Mantenimiento: El server va a estar en mantenimiento dentro de 15 minutos!")
			timer("shutdown1", 60*5)
		end
		when shutdown1.timer begin
			notice_all("ATENCION ATENCION !!!")
			notice_all("Mantenimiento: El server va a estar en mantenimiento dentro de 10 minutos!")
			notice_all("Porfavor , desconectate cuando queden 1 minutos para no reclamar la perdida de items , Gracias Saludos Survival!")
			timer("shutdown5min", 60*5)
		end
		when shutdown5min.timer begin
			notice_all("ATENCION ATENCION !!!")
			notice_all("Mantenimiento: El server va a estar en mantenimiento dentro de 5 minutos!")
			notice_all("Porfavor , desconectate cuando queden 1 minutos para no reclamar la perdida de items , Gracias Saludos Survival!")
			timer("shutdown3", 60*3)
		end
		when shutdown3.timer begin
			notice_all("ATENCION ATENCION !!!")
			notice_all("Mantenimiento: El server va a estar en mantenimiento dentro de 3 minutos!")
			notice_all("Porfavor , desconectate cuando queden 1 minutos para no reclamar la perdida de items , Gracias Saludos Survival!")
			timer("shutdown4", 60)
		end
		when shutdown4.timer begin
			notice_all("ATENCION ATENCION !!!")
			notice_all("Mantenimiento: El server va a estar en mantenimiento dentro de 2 minutos!")
			notice_all("Porfavor , desconectate cuando queden 1 minutos para no reclamar la perdida de items , Gracias Saludos Survival!")
			timer("shutdown5", 30)
		end
		when shutdown5.timer begin
			notice_all("ATENCION ATENCION !!!")
			notice_all("Mantenimiento: El server va a estar en mantenimiento dentro de 1 minutos!")
			notice_all("Porfavor , desconectate ahora para no reclamar la perdida de items , Gracias Saludos Survival!")
			timer("shutdown6", 15)
		end
		when shutdown6.timer begin
			notice_all("ATENCION ATENCION !!!")
			notice_all("Mantenimiento: El server va a estar en mantenimiento ahora!")
			timer("shutdown7", 5)
		end
		when shutdown7.timer begin
			command("shutdown")
		end
	end
end