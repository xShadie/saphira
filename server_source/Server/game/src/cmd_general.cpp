#include "stdafx.h"
#ifdef __FreeBSD__
#include <md5.h>
#else
#include "../../libthecore/include/xmd5.h"
#endif

#include "utils.h"
#include "config.h"
#include "desc_client.h"
#include "desc_manager.h"
#include "char.h"
#include "char_manager.h"
#include "motion.h"
#include "packet.h"
#include "affect.h"
#include "pvp.h"
#include "start_position.h"
#include "party.h"
#include "guild_manager.h"
#include "p2p.h"
#include "dungeon.h"
#include "messenger_manager.h"
#include "war_map.h"
#include "questmanager.h"
#include "item_manager.h"
#include "mob_manager.h"
#include "dev_log.h"
#include "item.h"
#include "arena.h"
#include "buffer_manager.h"
#include "unique_item.h"
#include "log.h"
#include "../../common/VnumHelper.h"
#include "shop.h"
#include "shop_manager.h"
#ifdef __NEWPET_SYSTEM__
#include "New_PetSystem.h"
#endif
#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
#include "MountSystem.h"
#endif

ACMD(do_user_horse_ride)
{
	if (ch->IsObserverMode())
		return;

	if (ch->IsDead() || ch->IsStun())
		return;

	if (ch->IsHorseRiding() == false)
	{
		if (ch->GetMountVnum()) {
#ifdef TEXTS_IMPROVEMENT
			ch->ChatPacketNew(CHAT_TYPE_INFO, 532, "");
#endif
			return;
		}

		if (ch->GetHorse() == NULL)
		{
#ifdef TEXTS_IMPROVEMENT
			ch->ChatPacketNew(CHAT_TYPE_INFO, 332, "");
#endif
			return;
		}

		ch->StartRiding();
	}
	else
	{
		ch->StopRiding();
	}
}
ACMD(do_daily_reward_reload){
	if (!ch) {
		return;
	}

	ch->ChatPacket(CHAT_TYPE_COMMAND, "ManagerGiftSystem DeleteRewards|");
	std::string time = "";
	std::string rewards = "";

	std::unique_ptr<SQLMsg> msg(DBManager::instance().DirectQuery("SELECT UNIX_TIMESTAMP(time), reward FROM player.daily_reward_status WHERE pid = %u", ch->GetPlayerID()));
	if (msg->Get()->uiNumRows > 0) {
		std::unique_ptr<SQLMsg> msg2(DBManager::instance().DirectQuery("SELECT UNIX_TIMESTAMP(time),reward FROM player.daily_reward_status WHERE pid = %u and (time + INTERVAL 1 DAY < NOW()) limit 1;", ch->GetPlayerID()));
		if (msg2->Get()->uiNumRows > 0) {
			std::unique_ptr<SQLMsg>(DBManager::Instance().DirectQuery("DELETE FROM player.daily_reward_status WHERE pid = %u", ch->GetPlayerID()));
			std::unique_ptr<SQLMsg>(DBManager::Instance().DirectQuery("INSERT INTO player.daily_reward_status (pid, time, reward, total_rewards) VALUES(%u, NOW(), 0, 0)", ch->GetPlayerID()));
#ifdef TEXTS_IMPROVEMENT
			ch->ChatPacketNew(CHAT_TYPE_INFO, 721, "");
#endif
			std::unique_ptr<SQLMsg> msg3(DBManager::instance().DirectQuery("SELECT UNIX_TIMESTAMP(time), reward FROM player.daily_reward_status WHERE pid = %u", ch->GetPlayerID()));
			if (msg3->Get()->uiNumRows > 0) {
				MYSQL_ROW row;
				while ((row = mysql_fetch_row(msg3->Get()->pSQLResult)) != NULL) {
					time = row[0];
					rewards = row[1];
				}
			}
		} else {
			std::unique_ptr<SQLMsg> msg3(DBManager::instance().DirectQuery("SELECT UNIX_TIMESTAMP(time), reward FROM player.daily_reward_status WHERE pid = %u", ch->GetPlayerID()));
			if (msg3->Get()->uiNumRows > 0) {
				MYSQL_ROW row;
				while ((row = mysql_fetch_row(msg3->Get()->pSQLResult)) != NULL) {
					time = row[0];
					rewards = row[1];
				}
			}
		}
	} else {
		std::unique_ptr<SQLMsg>(DBManager::Instance().DirectQuery("INSERT INTO player.daily_reward_status (pid, time, reward, total_rewards) VALUES(%u, NOW(), 0, 0)", ch->GetPlayerID()));
		
		std::unique_ptr<SQLMsg> msg2(DBManager::instance().DirectQuery("SELECT UNIX_TIMESTAMP(time), reward FROM player.daily_reward_status WHERE pid = %u", ch->GetPlayerID()));
		if (msg2->Get()->uiNumRows > 0) {
			MYSQL_ROW row;
			while ((row = mysql_fetch_row(msg2->Get()->pSQLResult)) != NULL) {
				time = row[0];
				rewards = row[1];
			}
		}
	}

	std::unique_ptr<SQLMsg> msgend(DBManager::instance().DirectQuery("SELECT items, count FROM player.daily_reward_items WHERE reward = '%s'", rewards.c_str()));
	if (msgend->Get()->uiNumRows > 0) {
		MYSQL_ROW row;
		while ((row = mysql_fetch_row(msgend->Get()->pSQLResult)) != NULL) {
			ch->ChatPacket(CHAT_TYPE_COMMAND, "ManagerGiftSystem SetReward|%s|%s", row[0], row[1]);
		}
	}

	ch->ChatPacket(CHAT_TYPE_COMMAND, "ManagerGiftSystem SetTime|%s", time.c_str());
	ch->ChatPacket(CHAT_TYPE_COMMAND, "ManagerGiftSystem SetDailyReward|%s", rewards.c_str());
	ch->ChatPacket(CHAT_TYPE_COMMAND, "ManagerGiftSystem SetRewardDone|");
}

ACMD(do_daily_reward_get_reward){
	if (!ch)
		return;

	std::string items = "";
	bool reward = false;
	std::string rewards = "";
	// and (NOW() - interval 30 minute > time) 

	std::unique_ptr<SQLMsg> msg(DBManager::instance().DirectQuery("SELECT reward from player.daily_reward_status where (NOW() > time) and pid = %u", ch->GetPlayerID()));
	if (msg->Get()->uiNumRows > 0) {
		MYSQL_ROW row;
		while ((row = mysql_fetch_row(msg->Get()->pSQLResult)) != NULL) {
			rewards = row[0];
		}

		reward = true;
	}
	
	if (reward) {
		std::string counts = "";

		std::unique_ptr<SQLMsg> msg2(DBManager::instance().DirectQuery("SELECT items, count from player.daily_reward_items where reward = '%s' ORDER BY RAND() limit 1", rewards.c_str()));
		if (msg2->Get()->uiNumRows > 0) {
			MYSQL_ROW row;
			while ((row = mysql_fetch_row(msg2->Get()->pSQLResult)) != NULL) {
				items = row[0];
				counts = row[1];
			}
		}

		DWORD item = 0;
		DWORD count = 0;
		
		str_to_number(item, items.c_str());
		str_to_number(count, counts.c_str());
		ch->AutoGiveItem(item, count);
		std::unique_ptr<SQLMsg>(DBManager::Instance().DirectQuery("UPDATE daily_reward_status SET reward = CASE WHEN reward = 0 THEN '1' WHEN reward = 1 THEN '2' WHEN reward = 2 THEN '3' WHEN reward = 3 THEN '4' WHEN reward = 4 THEN '5' WHEN reward = 5 THEN '6' WHEN reward = 6 THEN '0' END, total_rewards = total_rewards +1, time = (NOW() + interval 1 day) WHERE pid = %u", ch->GetPlayerID()));
	}
#ifdef TEXTS_IMPROVEMENT
	else {
		ch->ChatPacketNew(CHAT_TYPE_INFO, 715, "");
	}
#endif
}
ACMD(do_user_horse_back)
{
	if (!ch)
		return;
	
	CMountSystem* mountSystem = ch->GetMountSystem();
	if (mountSystem) {
		if ((mountSystem->CountSummoned() > 0) || (ch->GetMountVnum())) {
			LPITEM pkItem = ch->GetWear(WEAR_COSTUME_MOUNT);
			if (pkItem) {
				ch->UnequipItem(pkItem);
				return;
			}
		}
	}
	
	if (ch->GetHorse() != NULL)
	{
		ch->HorseSummon(false);
#ifdef TEXTS_IMPROVEMENT
		ch->ChatPacketNew(CHAT_TYPE_INFO, 331, "");
#endif
	}
	else if (ch->IsHorseRiding() == true)
	{
#ifdef TEXTS_IMPROVEMENT
		ch->ChatPacketNew(CHAT_TYPE_INFO, 330, "");
#endif
	}
	else
	{
#ifdef TEXTS_IMPROVEMENT
		ch->ChatPacketNew(CHAT_TYPE_INFO, 332, "");
#endif
	}
}

ACMD(do_user_horse_feed)
{
	// ���λ����� �� ���¿����� �� ���̸� �� �� ����.
	if (ch->GetMyShop())
		return;

	if (ch->GetHorse() == NULL)
	{
#ifdef TEXTS_IMPROVEMENT
		if (ch->IsHorseRiding() == false) {
			ch->ChatPacketNew(CHAT_TYPE_INFO, 332, "");
		}
		else {
			ch->ChatPacketNew(CHAT_TYPE_INFO, 336, "");
		}
#endif
		return;
	}

	DWORD dwFood = ch->GetHorseGrade() + 50054 - 1;


	if (ch->CountSpecifyItem(dwFood) > 0)
	{
		ch->RemoveSpecifyItem(dwFood, 1);
		ch->FeedHorse();
#ifdef TEXTS_IMPROVEMENT
		ch->ChatPacketNew(CHAT_TYPE_INFO, 112, "%s", 
#ifdef ENABLE_MULTI_NAMES
		ITEM_MANAGER::instance().GetTable(dwFood)->szLocaleName[ch->GetDesc()->GetLanguage()]
#else
		ITEM_MANAGER::instance().GetTable(dwFood)->szLocaleName
#endif
		);
#endif
	}
	else
	{
#ifdef TEXTS_IMPROVEMENT
		ch->ChatPacketNew(CHAT_TYPE_INFO, 111, "%s", 
#ifdef ENABLE_MULTI_NAMES
		ITEM_MANAGER::instance().GetTable(dwFood)->szLocaleName[ch->GetDesc()->GetLanguage()]
#else
		ITEM_MANAGER::instance().GetTable(dwFood)->szLocaleName
#endif
		);
#endif
	}

}

#define MAX_REASON_LEN		128

EVENTINFO(TimedEventInfo)
{
	DynamicCharacterPtr ch;
	int		subcmd;
	int         	left_second;
	char		szReason[MAX_REASON_LEN];

	TimedEventInfo()
	: ch()
	, subcmd( 0 )
	, left_second( 0 )
	{
		::memset( szReason, 0, MAX_REASON_LEN );
	}
};

struct SendDisconnectFunc
{
	void operator () (LPDESC d)
	{
		if (d->GetCharacter())
		{
			if (d->GetCharacter()->GetGMLevel() == GM_PLAYER)
				d->GetCharacter()->ChatPacket(CHAT_TYPE_COMMAND, "quit Shutdown(SendDisconnectFunc)");
		}
	}
};

struct DisconnectFunc
{
	void operator () (LPDESC d)
	{
		if (d->GetType() == DESC_TYPE_CONNECTOR)
			return;

		if (d->IsPhase(PHASE_P2P))
			return;

		if (d->GetCharacter())
			d->GetCharacter()->Disconnect("Shutdown(DisconnectFunc)");

		d->SetPhase(PHASE_CLOSE);
	}
};

EVENTINFO(shutdown_event_data)
{
	int seconds;

	shutdown_event_data()
	: seconds( 0 )
	{
	}
};

EVENTFUNC(shutdown_event)
{
	shutdown_event_data* info = dynamic_cast<shutdown_event_data*>( event->info );

	if ( info == NULL )
	{
		sys_err( "shutdown_event> <Factor> Null pointer" );
		return 0;
	}

	int * pSec = & (info->seconds);

	if (*pSec < 0)
	{
		sys_log(0, "shutdown_event sec %d", *pSec);

		if (--*pSec == -10)
		{
			const DESC_MANAGER::DESC_SET & c_set_desc = DESC_MANAGER::instance().GetClientSet();
			std::for_each(c_set_desc.begin(), c_set_desc.end(), DisconnectFunc());
			return passes_per_sec;
		}
		else if (*pSec < -10)
			return 0;

		return passes_per_sec;
	}
	else if (*pSec == 0)
	{
		const DESC_MANAGER::DESC_SET & c_set_desc = DESC_MANAGER::instance().GetClientSet();
		std::for_each(c_set_desc.begin(), c_set_desc.end(), SendDisconnectFunc());
		g_bNoMoreClient = true;
		--*pSec;
		return passes_per_sec;
	}
	else
	{
#ifdef TEXTS_IMPROVEMENT
		SendNoticeNew(CHAT_TYPE_NOTICE, 0, 0, 577, "%d", *pSec);
#endif
		--*pSec;
		return passes_per_sec;
	}
}

void Shutdown(int iSec)
{
	if (g_bNoMoreClient)
	{
		thecore_shutdown();
		return;
	}

	CWarMapManager::instance().OnShutdown();
#ifdef TEXTS_IMPROVEMENT
	SendNoticeNew(CHAT_TYPE_NOTICE, 0, 0, 578, "%d", iSec);
#endif
	shutdown_event_data* info = AllocEventInfo<shutdown_event_data>();
	info->seconds = iSec;

	event_create(shutdown_event, info, 1);
}

ACMD(do_shutdown)
{
	if (NULL == ch)
	{
		sys_err("Accept shutdown command from %s.", ch->GetName());
	}
	TPacketGGShutdown p;
	p.bHeader = HEADER_GG_SHUTDOWN;
	P2P_MANAGER::instance().Send(&p, sizeof(TPacketGGShutdown));

	Shutdown(10);
}


#ifdef ENABLE_CHANNEL_SWITCH_SYSTEM
ACMD(do_change_channel)
{
	if (!ch)
		return;

	if (ch->IsWarping())
	{
		return;
	}

	if (!ch->CanWarp())
	{
#ifdef TEXTS_IMPROVEMENT
		ch->ChatPacketNew(CHAT_TYPE_INFO, 234, "10");
#endif
		return;
	}

	if (ch->m_pkTimedEvent)
	{
#ifdef TEXTS_IMPROVEMENT
		ch->ChatPacketNew(CHAT_TYPE_INFO, 482, "");
#endif
		event_cancel(&ch->m_pkTimedEvent);
		return;
	}

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
	{
#ifdef TEXTS_IMPROVEMENT
		ch->ChatPacketNew(CHAT_TYPE_INFO, 716, "");
#endif
		return;
	}

	if (g_bChannel == 99)
	{
#ifdef TEXTS_IMPROVEMENT
		ch->ChatPacketNew(CHAT_TYPE_INFO, 719, "");
#endif
		return;
	}

	int32_t channel;
	str_to_number(channel, arg1);

	if (channel < 1 || channel > 6)
	{
#ifdef TEXTS_IMPROVEMENT
		ch->ChatPacketNew(CHAT_TYPE_INFO, 717, "");
#endif
		return;
	}

	if (channel == g_bChannel)
	{
#ifdef TEXTS_IMPROVEMENT
		ch->ChatPacketNew(CHAT_TYPE_INFO, 718, "%d", g_bChannel);
#endif
		return;
	}

	if (ch->GetDungeon())
	{
#ifdef TEXTS_IMPROVEMENT
		ch->ChatPacketNew(CHAT_TYPE_INFO, 720, "");
#endif
		return;
	}

	TPacketChangeChannel p;
	p.channel = channel;
	p.lMapIndex = ch->GetMapIndex();

	db_clientdesc->DBPacket(HEADER_GD_FIND_CHANNEL, ch->GetDesc()->GetHandle(), &p, sizeof(p));
}
#endif

#ifdef ENABLE_SORT_INVEN
ACMD(do_item_check)
{
	ch->EditMyInven();
}


ACMD(do_sort_extra_inventory)
{
	ch->EditMyExtraInven();
}

#endif

EVENTFUNC(timed_event)
{
	TimedEventInfo * info = dynamic_cast<TimedEventInfo *>( event->info );

	if ( info == NULL )
	{
		sys_err( "timed_event> <Factor> Null pointer" );
		return 0;
	}

	LPCHARACTER	ch = info->ch;
	if (ch == NULL) { // <Factor>
		return 0;
	}
	LPDESC d = ch->GetDesc();

	if (info->left_second <= 0)
	{
		ch->m_pkTimedEvent = NULL;

		switch (info->subcmd)
		{
			case SCMD_LOGOUT:
			case SCMD_QUIT:
			case SCMD_PHASE_SELECT:
				{
					TPacketNeedLoginLogInfo acc_info;
					acc_info.dwPlayerID = ch->GetDesc()->GetAccountTable().id;

					db_clientdesc->DBPacket( HEADER_GD_VALID_LOGOUT, 0, &acc_info, sizeof(acc_info) );

					LogManager::instance().DetailLoginLog( false, ch );
				}
				break;
		}

		switch (info->subcmd)
		{
			case SCMD_LOGOUT:
				if (d)
					d->SetPhase(PHASE_CLOSE);
				break;

			case SCMD_QUIT:
				ch->ChatPacket(CHAT_TYPE_COMMAND, "quit");
				break;

			case SCMD_PHASE_SELECT:
				{
					ch->Disconnect("timed_event - SCMD_PHASE_SELECT");

					if (d)
					{
						d->SetPhase(PHASE_SELECT);
					}
				}
				break;
		}

		return 0;
	}
	else
	{
#ifdef TEXTS_IMPROVEMENT
		ch->ChatPacketNew(CHAT_TYPE_INFO, 103, "%d", info->left_second);
#endif
		--info->left_second;
	}

	return PASSES_PER_SEC(1);
}

ACMD(do_cmd)
{
	if (ch->m_pkTimedEvent)
	{
#ifdef TEXTS_IMPROVEMENT
		ch->ChatPacketNew(CHAT_TYPE_INFO, 482, "");
#endif
		event_cancel(&ch->m_pkTimedEvent);
		return;
	}

#ifdef TEXTS_IMPROVEMENT
	switch (subcmd)
	{
		case SCMD_LOGOUT:
			ch->ChatPacketNew(CHAT_TYPE_INFO, 326, "");
			break;
		case SCMD_QUIT:
			ch->ChatPacketNew(CHAT_TYPE_INFO, 240, "");
			break;
		case SCMD_PHASE_SELECT:
#ifdef TEXTS_IMPROVEMENT
			ch->ChatPacketNew(CHAT_TYPE_INFO, 483, "");
#endif
			break;
	}
#endif

	int nExitLimitTime = 10;

	if (ch->IsHack(false, true, nExitLimitTime) && (!ch->GetWarMap() || ch->GetWarMap()->GetType() == GUILD_WAR_TYPE_FLAG)) {
		return;
	}

	switch (subcmd)
	{
		case SCMD_LOGOUT:
		case SCMD_QUIT:
		case SCMD_PHASE_SELECT:
			{
				TimedEventInfo* info = AllocEventInfo<TimedEventInfo>();

				{
					if (ch->IsPosition(POS_FIGHTING))
						info->left_second = 10;
					else
						info->left_second = 3;
				}

				info->ch		= ch;
				info->subcmd		= subcmd;
				strlcpy(info->szReason, argument, sizeof(info->szReason));

				ch->m_pkTimedEvent	= event_create(timed_event, info, 1);
			}
			break;
	}
}

ACMD(do_fishing)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	ch->SetRotation(atof(arg1));
	ch->fishing();
}

ACMD(do_console)
{
	ch->ChatPacket(CHAT_TYPE_COMMAND, "ConsoleEnable");
}

ACMD(do_restart)
{
	if (!ch->IsPC() || ch->GetPosition() != POS_DEAD)
	{
		return;
	}

	if (ch->IsHack())
	{
		if (subcmd == SCMD_RESTART_TOWN)
		{
			return;
		}
		else if (subcmd != SCMD_RESTART_TOWN && (!ch->GetWarMap() || ch->GetWarMap()->GetType() == GUILD_WAR_TYPE_FLAG))
		{
			return;
		}
	}

	ch->ChatPacket(CHAT_TYPE_COMMAND, "CloseRestartWindow");

	ch->GetDesc()->SetPhase(PHASE_GAME);
	ch->SetPosition(POS_STANDING);
	ch->StartRecoveryEvent();


	int32_t mapidx = ch->GetMapIndex();

	if (ch->GetWarMap() && !ch->IsObserverMode())
	{
		CWarMap * pMap = ch->GetWarMap();
		DWORD dwGuildOpponent = pMap ? pMap->GetGuildOpponent(ch) : 0;
		if (dwGuildOpponent)
		{
			switch (subcmd)
			{
				case SCMD_RESTART_TOWN:
					{
						sys_log(0, "do_restart: restart town");

						PIXEL_POSITION pos;
						if (CWarMapManager::instance().GetStartPosition(mapidx, ch->GetGuild()->GetID() < dwGuildOpponent ? 0 : 1, pos))
						{
							ch->Show(mapidx, pos.x, pos.y);
						}
						else
						{
							ch->ExitToSavedLocation();
						}

						ch->PointChange(POINT_HP, ch->GetMaxHP() - ch->GetHP());
						ch->PointChange(POINT_SP, ch->GetMaxSP() - ch->GetSP());
						ch->ReviveInvisible(5);
#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
						ch->CheckMount();
#endif
					}
					break;

				case SCMD_RESTART_HERE:
					{
						sys_log(0, "do_restart: restart here");
						ch->RestartAtSamePos();
						ch->PointChange(POINT_HP, ch->GetMaxHP() - ch->GetHP());
						ch->PointChange(POINT_SP, ch->GetMaxSP() - ch->GetSP());
						ch->ReviveInvisible(5);
#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
						ch->CheckMount();
#endif
#ifdef ENABLE_SKILLS_BUFF_ALTERNATIVE
						ch->LoadAffectSkills();
#endif
					}
					break;
				default:
					{
						sys_err(0, "do_restart: unknown method for %s", ch->GetName());
					}
					break;
			}

			return;
		}
	}

	switch (subcmd)
	{
		case SCMD_RESTART_TOWN:
			{
				sys_log(0, "do_restart: restart town");

				bool wasDungeon = false;
				bool showed = false;
				if (mapidx >= 10000)
				{
					LPDUNGEON dungeon = CDungeonManager::instance().FindByMapIndex(mapidx);
					if (dungeon)
					{
						if (mapidx >= 2160000 && mapidx < 2170000)
						{
							wasDungeon = true;
							int32_t floor = dungeon->GetFlag("floor");
							switch (floor)
							{
								case 1:
									{
										ch->Show(mapidx, 445000, 1228200);
										showed = true;
									}
									break;
								case 2:
									{
										ch->Show(mapidx, 391700, 1293200);
										showed = true;
									}
									break;
								case 3:
									{
										ch->Show(mapidx, 443400, 1269800);
										showed = true;
									}
									break;
								case 4:
									{
										ch->Show(mapidx, 314700, 1318700);
										showed = true;
									}
									break;
								default:
									{
										ch->ExitToSavedLocation();
									}
									break;
							}
						}
						else if (mapidx >= 2080000 && mapidx < 2090000)
						{
							wasDungeon = true;
							int32_t floor = dungeon->GetFlag("floor");
							if (floor != 0)
							{
								ch->Show(mapidx, 843500, 1066800);
								showed = true;
							}
							else
							{
								ch->ExitToSavedLocation();
							}
						}
						else if (mapidx >= 2170000 && mapidx < 2180000)
						{
							wasDungeon = true;
							int32_t floor = dungeon->GetFlag("floor");
							if (floor != 0)
							{
								ch->Show(mapidx, 87900, 614700);
								showed = true;
							}
							else
							{
								ch->ExitToSavedLocation();
							}
						}
						else if (mapidx >= 3550000 && mapidx < 3560000)
						{
							wasDungeon = true;
							int32_t floor = dungeon->GetFlag("floor");
							switch (floor)
							{
								case 1:
									{
										ch->Show(mapidx, 216600, 266700);
										showed = true;
									}
									break;
								case 2:
									{
										ch->Show(mapidx, 218600, 348900);
										showed = true;
									}
									break;
								default:
									{
										ch->ExitToSavedLocation();
									}
									break;
							}
						}
						else if (mapidx >= 3530000 && mapidx < 3540000)
						{
							wasDungeon = true;
							int32_t floor = dungeon->GetFlag("floor");
							if (floor != 0)
							{
								ch->Show(mapidx, 166500, 522100);
								showed = true;
							}
							else
							{
								ch->ExitToSavedLocation();
							}
						}
						else if (mapidx >= 3520000 && mapidx < 3530000)
						{
							wasDungeon = true;
							int32_t floor = dungeon->GetFlag("floor");
							switch (floor)
							{
								case 1:
								case 2:
									{
										ch->Show(mapidx, 588100, 180400);
										showed = true;
									}
									break;
								case 3:
									{
										ch->Show(mapidx, 554000, 207000);
										showed = true;
									}
									break;
								case 4:
								case 5:
									{
										ch->Show(mapidx, 569100, 223000);
										showed = true;
									}
									break;
								case 6:
									{
										ch->Show(mapidx, 586600, 206800);
										showed = true;
									}
									break;
								case 7:
								case 8:
									{
										ch->Show(mapidx, 596900, 222500);
										showed = true;
									}
									break;
								case 9:
									{
										ch->Show(mapidx, 604700, 192600);
										showed = true;
									}
									break;
								default:
									{
										ch->ExitToSavedLocation();
									}
									break;
							}
						}
						else if (mapidx >= 3570000 && mapidx < 3580000)
						{
							wasDungeon = true;
							int32_t floor = dungeon->GetFlag("floor");
							switch (floor)
							{
								case 1:
									{
										ch->Show(mapidx, 905100, 2261700);
										showed = true;
									}
									break;
								case 2:
								case 3:
									{
										ch->Show(mapidx, 926600, 2262100);
										showed = true;
									}
									break;
								case 4:
								case 5:
									{
										ch->Show(mapidx, 953600, 2260800);
										showed = true;
									}
									break;
								case 6:
									{
										ch->Show(mapidx, 913700, 2355800);
										showed = true;
									}
									break;
								case 7:
									{
										ch->Show(mapidx, 975900, 2365500);
										showed = true;
									}
									break;
								default:
									{
										ch->ExitToSavedLocation();
									}
									break;
							}
						}
						else if (mapidx >= 3510000 && mapidx < 3520000)
						{
							wasDungeon = true;
							int32_t floor = dungeon->GetFlag("floor");
							switch (floor)
							{
								case 1:
								case 2:
								case 3:
								case 4:
								case 5:
								case 6:
								case 7:
									{
										ch->Show(mapidx, 776600, 671900);
										showed = true;
									}
									break;
								case 8:
									{
										ch->Show(mapidx, 810900, 686700);
										showed = true;
									}
									break;
								default:
									{
										ch->ExitToSavedLocation();
									}
									break;
							}
						}
						else if (mapidx >= 2180000 && mapidx < 2190000)
						{
							wasDungeon = true;
							int32_t floor = dungeon->GetFlag("floor");
							switch (floor)
							{
								case 1:
									{
										ch->Show(mapidx, 624500, 1415200);
										showed = true;
									}
									break;
								case 2:
									{
										ch->Show(mapidx, 627300, 1446800);
										showed = true;
									}
									break;
								case 3:
									{
										ch->Show(mapidx, 673200, 1444000);
										showed = true;
									}
									break;
								case 4:
									{
										ch->Show(mapidx, 655400, 1421200);
										showed = true;
									}
									break;
								case 5:
									{
										ch->Show(mapidx, 695500, 1421300);
										showed = true;
									}
									break;
								default:
									{
										ch->ExitToSavedLocation();
									}
									break;
							}
						}
						else if (mapidx >= 270000 && mapidx < 280000)
						{
							wasDungeon = true;
							int32_t floor = dungeon->GetFlag("floor");
							if (floor != 0)
							{
								ch->Show(mapidx, 942000, 127700);
								showed = true;
							}
							else
							{
								ch->ExitToSavedLocation();
							}
						}
						else if (mapidx >= 2090000 && mapidx < 2100000)
						{
							wasDungeon = true;
							int32_t floor = dungeon->GetFlag("floor");
							if (floor != 0)
							{
								ch->Show(mapidx, 853700, 1416400);
								showed = true;
							}
							else
							{
								ch->ExitToSavedLocation();
							}
						}
						else if (mapidx >= 2100000 && mapidx < 2110000)
						{
							wasDungeon = true;
							int32_t floor = dungeon->GetFlag("floor");
							if (floor != 0)
							{
								ch->Show(mapidx, 782400, 1502100);
								showed = true;
							}
							else
							{
								ch->ExitToSavedLocation();
							}
						}
						else if (mapidx >= 660000 && mapidx < 670000)
						{
							wasDungeon = true;
							int32_t floor = dungeon->GetFlag("floor");
							switch (floor)
							{
								case 1:
									{
										ch->Show(mapidx, 377400, 2704000);
										showed = true;
									}
									break;
								case 2:
									{
										ch->Show(mapidx, 378200, 2680300);
										showed = true;
									}
									break;
								case 3:
								case 4:
									{
										ch->Show(mapidx, 401700, 2728500);
										showed = true;
									}
									break;
								case 5:
									{
										ch->Show(mapidx, 401700, 2705700);
										showed = true;
									}
									break;
								case 6:
								case 7:
									{
										ch->Show(mapidx, 402200, 2682300);
										showed = true;
									}
									break;
								case 8:
								case 9:
								case 10:
									{
										ch->Show(mapidx, 423800, 2729400);
										showed = true;
									}
									break;
								case 11:
								case 12:
									{
										ch->Show(mapidx, 423800, 2705900);
										showed = true;
									}
									break;
								case 13:
									{
										ch->Show(mapidx, 423800, 2681100);
										showed = true;
									}
									break;
								default:
									{
										ch->ExitToSavedLocation();
									}
									break;
							}
						}
						else if (mapidx >= 3560000 && mapidx < 3570000)
						{
							wasDungeon = true;
							int32_t floor = dungeon->GetFlag("floor");
							if (floor != 0)
							{
								ch->Show(mapidx, 1528200, 2318700);
								showed = true;
							}
							else
							{
								ch->ExitToSavedLocation();
							}
						}
						else if (mapidx >= 2120000 && mapidx < 2130000)
						{
							wasDungeon = true;
							int32_t floor = dungeon->GetFlag("floor");
							if (floor != 0)
							{
								ch->Show(mapidx, 320000, 1529000);
								showed = true;
							}
							else
							{
								ch->ExitToSavedLocation();
							}
						}
						else if (mapidx >= 260000 && mapidx < 270000)
						{
							wasDungeon = true;
							int32_t floor = dungeon->GetFlag("floor");
							switch (floor)
							{
								case 1:
									{
										ch->Show(mapidx, 54500, 2268000);
										showed = true;
									}
									break;
								case 2:
									{
										ch->Show(mapidx, 19400, 2306600);
										showed = true;
									}
									break;
								case 3:
									{
										ch->Show(mapidx, 110500, 2295900);
										showed = true;
									}
									break;
								default:
									{
										ch->ExitToSavedLocation();
									}
									break;
							}
						}
						else if (mapidx >= 250000 && mapidx < 260000)
						{
							wasDungeon = true;
							int32_t floor = dungeon->GetFlag("floor");
							switch (floor)
							{
								case 1:
									{
										ch->Show(mapidx, 2320600, 3077800);
										showed = true;
									}
									break;
								case 2:
									{
										ch->Show(mapidx, 2351500, 3141500);
										showed = true;
									}
									break;
								default:
									{
										ch->ExitToSavedLocation();
									}
									break;
							}
						}
					}
				}

				if (!wasDungeon)
				{
					PIXEL_POSITION pos;
					if (SECTREE_MANAGER::instance().GetRecallPositionByEmpire(mapidx, ch->GetEmpire(), pos))
					{
						ch->WarpSet(pos.x, pos.y);
					}
					else
					{
						ch->WarpSet(EMPIRE_START_X(ch->GetEmpire()), EMPIRE_START_Y(ch->GetEmpire()));
					}
				}

				ch->PointChange(POINT_HP, ch->GetMaxHP() - ch->GetHP());
				ch->PointChange(POINT_SP, ch->GetMaxSP() - ch->GetSP());
				ch->DeathPenalty(1);
				if (showed)
				{
#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
					ch->CheckMount();
#endif
#ifdef ENABLE_SKILLS_BUFF_ALTERNATIVE
					ch->LoadAffectSkills();
#endif
				}
			}
			break;
		case SCMD_RESTART_HERE:
			{
				sys_log(0, "do_restart: restart here");

				ch->RestartAtSamePos();
#ifdef ENABLE_REVIVE_WITH_HALF_HP_IF_MONSTER_KILLED_YOU
				ch->PointChange(POINT_HP, ch->GetDeadByMonster() ? (ch->GetMaxHP() - ch->GetHP()) / 2 : 50 - ch->GetHP());
#else
				ch->PointChange(POINT_HP, 50 - ch->GetHP());
#endif
				ch->PointChange(POINT_SP, ch->GetMaxSP() - ch->GetSP());
				ch->DeathPenalty(0);
				ch->ReviveInvisible(5);
#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
				ch->CheckMount();
#endif
#ifdef ENABLE_SKILLS_BUFF_ALTERNATIVE
				ch->LoadAffectSkills();
#endif
			}
			break;
		default:
			{
				sys_err(0, "do_restart: unknown method for %s", ch->GetName());
			}
			break;
	}
}

#define MAX_STAT g_iStatusPointSetMaxValue

ACMD(do_stat_reset)
{
	ch->PointChange(POINT_STAT_RESET_COUNT, 12 - ch->GetPoint(POINT_STAT_RESET_COUNT));
}

ACMD(do_stat_minus)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	if (ch->IsPolymorphed())
	{
#ifdef TEXTS_IMPROVEMENT
		ch->ChatPacketNew(CHAT_TYPE_INFO, 312, "");
#endif
		return;
	}

	if (ch->GetPoint(POINT_STAT_RESET_COUNT) <= 0)
		return;

	if (!strcmp(arg1, "st"))
	{
		if (ch->GetRealPoint(POINT_ST) <= JobInitialPoints[ch->GetJob()].st)
			return;

		ch->SetRealPoint(POINT_ST, ch->GetRealPoint(POINT_ST) - 1);
		ch->SetPoint(POINT_ST, ch->GetPoint(POINT_ST) - 1);
		ch->ComputePoints();
		ch->PointChange(POINT_ST, 0);
	}
	else if (!strcmp(arg1, "dx"))
	{
		if (ch->GetRealPoint(POINT_DX) <= JobInitialPoints[ch->GetJob()].dx)
			return;

		ch->SetRealPoint(POINT_DX, ch->GetRealPoint(POINT_DX) - 1);
		ch->SetPoint(POINT_DX, ch->GetPoint(POINT_DX) - 1);
		ch->ComputePoints();
		ch->PointChange(POINT_DX, 0);
	}
	else if (!strcmp(arg1, "ht"))
	{
		if (ch->GetRealPoint(POINT_HT) <= JobInitialPoints[ch->GetJob()].ht)
			return;

		ch->SetRealPoint(POINT_HT, ch->GetRealPoint(POINT_HT) - 1);
		ch->SetPoint(POINT_HT, ch->GetPoint(POINT_HT) - 1);
		ch->ComputePoints();
		ch->PointChange(POINT_HT, 0);
		ch->PointChange(POINT_MAX_HP, 0);
	}
	else if (!strcmp(arg1, "iq"))
	{
		if (ch->GetRealPoint(POINT_IQ) <= JobInitialPoints[ch->GetJob()].iq)
			return;

		ch->SetRealPoint(POINT_IQ, ch->GetRealPoint(POINT_IQ) - 1);
		ch->SetPoint(POINT_IQ, ch->GetPoint(POINT_IQ) - 1);
		ch->ComputePoints();
		ch->PointChange(POINT_IQ, 0);
		ch->PointChange(POINT_MAX_SP, 0);
	}
	else
		return;

	ch->PointChange(POINT_STAT, +1);
	ch->PointChange(POINT_STAT_RESET_COUNT, -1);
	ch->ComputePoints();
}

ACMD(do_stat)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	if (ch->IsPolymorphed())
	{
#ifdef TEXTS_IMPROVEMENT
		ch->ChatPacketNew(CHAT_TYPE_INFO, 312, "");
#endif
		return;
	}

	if (ch->GetPoint(POINT_STAT) <= 0)
		return;

	BYTE idx = 0;

	if (!strcmp(arg1, "st"))
		idx = POINT_ST;
	else if (!strcmp(arg1, "dx"))
		idx = POINT_DX;
	else if (!strcmp(arg1, "ht"))
		idx = POINT_HT;
	else if (!strcmp(arg1, "iq"))
		idx = POINT_IQ;
	else
		return;

	if (ch->GetRealPoint(idx) >= MAX_STAT)
		return;

	ch->SetRealPoint(idx, ch->GetRealPoint(idx) + 1);
	ch->SetPoint(idx, ch->GetPoint(idx) + 1);
	ch->ComputePoints();
	ch->PointChange(idx, 0);

	if (idx == POINT_IQ)
	{
		ch->PointChange(POINT_MAX_HP, 0);
	}
	else if (idx == POINT_HT)
	{
		ch->PointChange(POINT_MAX_SP, 0);
	}

	ch->PointChange(POINT_STAT, -1);
	ch->ComputePoints();
}

#ifdef ENABLE_PVP_ADVANCED
#include <string>
#include <algorithm>
#include <boost/algorithm/string/replace.hpp>
const char* szTableStaticPvP[] = {BLOCK_CHANGEITEM, BLOCK_BUFF, BLOCK_POTION, BLOCK_RIDE, BLOCK_PET, BLOCK_POLY, BLOCK_PARTY, BLOCK_EXCHANGE_, BET_WINNER, CHECK_IS_FIGHT};

ACMD(do_pvp)
{
	if (!ch)
		return;
	
	if (ch->GetArena() != NULL || CArenaManager::instance().IsArenaMap(ch->GetMapIndex()) == true)
	{
#ifdef TEXTS_IMPROVEMENT
		ch->ChatPacketNew(CHAT_TYPE_INFO, 303, "");
#endif
		return;
	}

	char arg1[256], arg2[256], arg3[256], arg4[256], arg5[256], arg6[256], arg7[256], arg8[256], arg9[256], arg10[256];
	
	pvp_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2), arg3, sizeof(arg3), arg4, sizeof(arg4), arg5, sizeof(arg5), arg6, sizeof(arg6), arg7, sizeof(arg7), arg8, sizeof(arg8), arg9, sizeof(arg9), arg10, sizeof(arg10));	
	
	DWORD vid = 0;
	str_to_number(vid, arg1);	
	LPCHARACTER pkVictim = CHARACTER_MANAGER::instance().Find(vid);

	if (!pkVictim)
		return;

	if (pkVictim->IsNPC())
		return;

	if (pkVictim->GetArena() != NULL) {
		return;
	}

	int mytime = pkVictim->GetQuestFlag("pvp.timed");
	int itime = mytime <= 0 ? 0 : mytime - get_global_time();
	if (itime > 0) {
#ifdef TEXTS_IMPROVEMENT
		ch->ChatPacketNew(CHAT_TYPE_DIALOG, 888, "%d", itime);
		pkVictim->ChatPacketNew(CHAT_TYPE_DIALOG, 889, "%s", ch->GetName());
#endif
		return;
	}
	else {
		pkVictim->SetQuestFlag("pvp.timed", get_global_time() + 30);
	}

	if (ch->GetExchange() || pkVictim->GetExchange())
	{
		CPVPManager::instance().Decline(ch, pkVictim);
		CPVPManager::instance().Decline(pkVictim, ch);
		return;
	}
	
	if (*arg2 && !strcmp(arg2, "accept"))
	{	
#ifdef ENABLE_LONG_LONG
		long long chA_nBetMoney = ch->GetQuestFlag(szTableStaticPvP[8]);
		long long  chB_nBetMoney = pkVictim->GetQuestFlag(szTableStaticPvP[8]);
		long long  limit = 2000000000;
#else
		int chA_nBetMoney = ch->GetQuestFlag(szTableStaticPvP[8]);
		int  chB_nBetMoney = pkVictim->GetQuestFlag(szTableStaticPvP[8]);
		int  limit = 2000000000;
#endif

		if ((ch->GetGold() < chA_nBetMoney) || (pkVictim->GetGold() < chB_nBetMoney ) || (chA_nBetMoney > limit) || (chB_nBetMoney > limit)) {
#ifdef TEXTS_IMPROVEMENT
			ch->ChatPacketNew(CHAT_TYPE_INFO, 722, "");
			pkVictim->ChatPacketNew(CHAT_TYPE_INFO, 722, "");
#endif
			CPVPManager::instance().Decline(ch, pkVictim);
			CPVPManager::instance().Decline(pkVictim, ch);
			return;
		}

		ch->SetDuel("IsFight", 1);
		pkVictim->SetDuel("IsFight", 1);
		
		if (chA_nBetMoney > 0 && chA_nBetMoney > 0)
		{
			ch->PointChange(POINT_GOLD, - chA_nBetMoney, true);	
			pkVictim->PointChange(POINT_GOLD, - chB_nBetMoney, true);	
		}
		
		CPVPManager::instance().Insert(ch, pkVictim);
		return;
	}
	
	int m_BlockChangeItem = 0, m_BlockBuff = 0, m_BlockPotion = 0, m_BlockRide = 0, m_BlockPet = 0, m_BlockPoly = 0, m_BlockParty = 0, m_BlockExchange = 0, m_BetMoney = 0;
	
	str_to_number(m_BlockChangeItem, arg2);
	str_to_number(m_BlockBuff, arg3);
	str_to_number(m_BlockPotion, arg4);
	str_to_number(m_BlockRide, arg5);
	str_to_number(m_BlockPet, arg6);
	str_to_number(m_BlockPoly, arg7);
	str_to_number(m_BlockParty, arg8);
	str_to_number(m_BlockExchange, arg9);
	str_to_number(m_BetMoney, arg10);
	
	if (!isdigit(*arg2) && !isdigit(*arg3) && !isdigit(*arg4) && !isdigit(*arg5) && !isdigit(*arg6) && !isdigit(*arg7) && !isdigit(*arg8) && !isdigit(*arg9) && !isdigit(*arg10))
	{
#ifdef TEXTS_IMPROVEMENT
		ch->ChatPacketNew(CHAT_TYPE_DIALOG, 874, "");
#endif
		return;
	}

	if (m_BetMoney < 0)
	{
#ifdef TEXTS_IMPROVEMENT
		ch->ChatPacketNew(CHAT_TYPE_DIALOG, 875, "");
#endif
		return;
	}	
	
	if (m_BetMoney >= GOLD_MAX)
	{
#ifdef TEXTS_IMPROVEMENT
		ch->ChatPacketNew(CHAT_TYPE_DIALOG, 876, "");
#endif
		return;
	}
	
	if (ch->GetGold() < m_BetMoney)
	{
#ifdef TEXTS_IMPROVEMENT
		ch->ChatPacketNew(CHAT_TYPE_DIALOG, 877, "");
#endif
		return;
	}
	
	if ((ch->GetGold() + m_BetMoney) > GOLD_MAX)
	{
#ifdef TEXTS_IMPROVEMENT
		ch->ChatPacketNew(CHAT_TYPE_DIALOG, 878, "");
#endif
		return;
	}
	
	if ((pkVictim->GetGold() + m_BetMoney) > GOLD_MAX)
	{
#ifdef TEXTS_IMPROVEMENT
		ch->ChatPacketNew(CHAT_TYPE_DIALOG, 878, "");
#endif
		return;
	}
	
	if (pkVictim->GetGold() < m_BetMoney)
	{
#ifdef TEXTS_IMPROVEMENT
		ch->ChatPacketNew(CHAT_TYPE_DIALOG, 879, "");
#endif
		return;
	}
	
	if (*arg1 && *arg2 && *arg3 && *arg4 && *arg5 && *arg6 && *arg7 && *arg8 && *arg9 && *arg10)
	{
		ch->SetDuel("BlockChangeItem", m_BlockChangeItem);			ch->SetDuel("BlockBuff", m_BlockBuff);
		ch->SetDuel("BlockPotion", m_BlockPotion);					ch->SetDuel("BlockRide", m_BlockRide);
		ch->SetDuel("BlockPet", m_BlockPet);						ch->SetDuel("BlockPoly", m_BlockPoly);	
		ch->SetDuel("BlockParty", m_BlockParty);					ch->SetDuel("BlockExchange", m_BlockExchange);
		ch->SetDuel("BetMoney", m_BetMoney);

		pkVictim->SetDuel("BlockChangeItem", m_BlockChangeItem);	pkVictim->SetDuel("BlockBuff", m_BlockBuff);
		pkVictim->SetDuel("BlockPotion", m_BlockPotion);			pkVictim->SetDuel("BlockRide", m_BlockRide);
		pkVictim->SetDuel("BlockPet", m_BlockPet);					pkVictim->SetDuel("BlockPoly", m_BlockPoly);	
		pkVictim->SetDuel("BlockParty", m_BlockParty);				pkVictim->SetDuel("BlockExchange", m_BlockExchange);
		pkVictim->SetDuel("BetMoney", m_BetMoney);
			
		CPVPManager::instance().Insert(ch, pkVictim); 
	}
}

ACMD(do_pvp_advanced)
{   
	if (!ch)
		return;

	if (ch->GetArena() != NULL || CArenaManager::instance().IsArenaMap(ch->GetMapIndex()) == true)
	{
#ifdef TEXTS_IMPROVEMENT
		ch->ChatPacketNew(CHAT_TYPE_INFO, 303, "");
#endif
		return;
	}
	
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	DWORD vid = 0;
	str_to_number(vid, arg1);
	LPCHARACTER pkVictim = CHARACTER_MANAGER::instance().Find(vid);

	if (!pkVictim)
		return;
	
	if (pkVictim->IsNPC())
		return;
	
	if (pkVictim->GetArena() != NULL) {
		return;
	}
	
	if (ch->GetQuestFlag(szTableStaticPvP[9]) > 0)
	{
#ifdef TEXTS_IMPROVEMENT
		ch->ChatPacketNew(CHAT_TYPE_DIALOG, 882, "");
#endif
		return;
	}
	
	if (pkVictim->GetQuestFlag(szTableStaticPvP[9]) > 0)
	{
#ifdef TEXTS_IMPROVEMENT
		ch->ChatPacketNew(CHAT_TYPE_DIALOG, 882, "");
#endif
		return;
	}
	
	int statusEq = pkVictim->GetQuestFlag(BLOCK_EQUIPMENT_);
	
	CGuild * g = pkVictim->GetGuild();

	const char* m_Name = pkVictim->GetName();	
	const char* m_GuildName = "-";
		
	int m_Vid = pkVictim->GetVID();	
	int m_Level = pkVictim->GetLevel();
	int m_PlayTime = pkVictim->GetRealPoint(POINT_PLAYTIME);
	int m_MaxHP = pkVictim->GetMaxHP();
	int m_MaxSP = pkVictim->GetMaxSP();
	
	DWORD m_Race = pkVictim->GetRaceNum();
	
	if (g)
	{ 
		ch->ChatPacket(CHAT_TYPE_COMMAND, "BINARY_Duel_GetInfo %d %s %s %d %d %d %d %d", m_Vid, m_Name, g->GetName(), m_Level, m_Race, m_PlayTime, m_MaxHP, m_MaxSP);
		
		if (statusEq < 1)
			pkVictim->SendEquipment(ch);
	}
	else { 
		ch->ChatPacket(CHAT_TYPE_COMMAND, "BINARY_Duel_GetInfo %d %s %s %d %d %d %d %d", m_Vid, m_Name, m_GuildName, m_Level, m_Race, m_PlayTime, m_MaxHP, m_MaxSP);
		
		if (statusEq < 1)
			pkVictim->SendEquipment(ch);
	}
}

ACMD(do_decline_pvp)
{
	if (!ch)
		return;

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));
	
	if (!*arg1)
		return;
	
	DWORD vid = 0;
	str_to_number(vid, arg1);
	
	LPCHARACTER pkVictim = CHARACTER_MANAGER::instance().Find(vid);
	
	if (!pkVictim)
		return;
	
	if (pkVictim->IsNPC())
		return;
	
	CPVPManager::instance().Decline(ch, pkVictim);
	ch->SetQuestFlag("pvp.timed", 0);
	pkVictim->SetQuestFlag("pvp.timed", 0);
}

ACMD(do_block_equipment)
{
	if (!ch)
		return;

	char arg1[256];
	one_argument (argument, arg1, sizeof(arg1));
	
	if (!ch->IsPC() || NULL == ch)
		return;
	
	int statusEq = ch->GetQuestFlag(BLOCK_EQUIPMENT_);
	
	if (!strcmp(arg1, "BLOCK"))
	{
		if (statusEq > 0)
		{
#ifdef TEXTS_IMPROVEMENT
			ch->ChatPacketNew(CHAT_TYPE_INFO, 11, "");
#endif
		}
		else {
			ch->SetQuestFlag(BLOCK_EQUIPMENT_, 1);
			ch->ChatPacket(CHAT_TYPE_COMMAND, "equipview 1");
#ifdef TEXTS_IMPROVEMENT
			ch->ChatPacketNew(CHAT_TYPE_INFO, 12, "");
#endif
		}
	}
	else if (!strcmp(arg1, "UNBLOCK"))
	{
		if (statusEq != 0) {
			ch->SetQuestFlag(BLOCK_EQUIPMENT_, 0);
			ch->ChatPacket(CHAT_TYPE_COMMAND, "equipview 0");
#ifdef TEXTS_IMPROVEMENT
			ch->ChatPacketNew(CHAT_TYPE_INFO, 14, "");
#endif
		}
#ifdef TEXTS_IMPROVEMENT
		else {
			ch->ChatPacketNew(CHAT_TYPE_INFO, 13, "");
		}
#endif
	}
}
#endif

ACMD(do_guildskillup)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	if (!ch->GetGuild())
	{
#ifdef TEXTS_IMPROVEMENT
		ch->ChatPacketNew(CHAT_TYPE_INFO, 138, "");
#endif
		return;
	}

	CGuild* g = ch->GetGuild();
	TGuildMember* gm = g->GetMember(ch->GetPlayerID());
	if (gm->grade == GUILD_LEADER_GRADE)
	{
		DWORD vnum = 0;
		str_to_number(vnum, arg1);
		g->SkillLevelUp(vnum);
	}
#ifdef TEXTS_IMPROVEMENT
	else {
		ch->ChatPacketNew(CHAT_TYPE_INFO, 890, "");
	}
#endif
}

ACMD(do_skillup)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	DWORD vnum = 0;
	str_to_number(vnum, arg1);

	if (true == ch->CanUseSkill(vnum))
	{
		ch->SkillLevelUp(vnum);
	}
	else
	{
		switch(vnum)
		{
			case SKILL_HORSE_WILDATTACK:
			case SKILL_HORSE_CHARGE:
			case SKILL_HORSE_ESCAPE:
			case SKILL_HORSE_WILDATTACK_RANGE:

			case SKILL_7_A_ANTI_TANHWAN:
			case SKILL_7_B_ANTI_AMSEOP:
			case SKILL_7_C_ANTI_SWAERYUNG:
			case SKILL_7_D_ANTI_YONGBI:

			case SKILL_8_A_ANTI_GIGONGCHAM:
			case SKILL_8_B_ANTI_YEONSA:
			case SKILL_8_C_ANTI_MAHWAN:
			case SKILL_8_D_ANTI_BYEURAK:

			case SKILL_ADD_HP:
			case SKILL_RESIST_PENETRATE:
#ifdef ENABLE_NEW_SECONDARY_SKILLS
			case NEW_SUPPORT_SKILL_ATTACK:
			case NEW_SUPPORT_SKILL_YANG:
			case NEW_SUPPORT_SKILL_MONSTERS:
			case NEW_SUPPORT_SKILL_HP:
#endif

#ifdef ENABLE_NEW_PASSIVE_SKILLS
			case SKILL_ANTI_PALBANG:
			case SKILL_ANTI_AMSEOP:
			case SKILL_ANTI_SWAERYUNG:
			case SKILL_ANTI_YONGBI:
			case SKILL_ANTI_GIGONGCHAM:
			case SKILL_ANTI_HWAJO:
			case SKILL_ANTI_MARYUNG:
			case SKILL_ANTI_BYEURAK:
			case SKILL_HELP_PALBANG:
			case SKILL_HELP_AMSEOP:
			case SKILL_HELP_SWAERYUNG:
			case SKILL_HELP_YONGBI:
			case SKILL_HELP_GIGONGCHAM:
			case SKILL_HELP_HWAJO:
			case SKILL_HELP_MARYUNG:
			case SKILL_HELP_BYEURAK:
#endif

				ch->SkillLevelUp(vnum);
				break;
		}
	}
}

//
// @version	05/06/20 Bang2ni - Ŀ�ǵ� ó�� Delegate to CHARACTER class
//
ACMD(do_safebox_close)
{
	ch->CloseSafebox();
}

//
// @version	05/06/20 Bang2ni - Ŀ�ǵ� ó�� Delegate to CHARACTER class
//
ACMD(do_safebox_password)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));
	ch->ReqSafeboxLoad(arg1);
}

ACMD(do_safebox_change_password)
{
	char arg1[256];
	char arg2[256];

	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1 || strlen(arg1)>6)
	{
#ifdef TEXTS_IMPROVEMENT
		ch->ChatPacketNew(CHAT_TYPE_INFO, 188, "");
#endif
		return;
	}

	if (!*arg2 || strlen(arg2)>6)
	{
#ifdef TEXTS_IMPROVEMENT
		ch->ChatPacketNew(CHAT_TYPE_INFO, 188, "");
#endif
		return;
	}

	TSafeboxChangePasswordPacket p;

	p.dwID = ch->GetDesc()->GetAccountTable().id;
	strlcpy(p.szOldPassword, arg1, sizeof(p.szOldPassword));
	strlcpy(p.szNewPassword, arg2, sizeof(p.szNewPassword));

	db_clientdesc->DBPacket(HEADER_GD_SAFEBOX_CHANGE_PASSWORD, ch->GetDesc()->GetHandle(), &p, sizeof(p));
}

ACMD(do_mall_password)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1 || strlen(arg1) > 6)
	{
#ifdef TEXTS_IMPROVEMENT
		ch->ChatPacketNew(CHAT_TYPE_INFO, 188, "");
#endif
		return;
	}

	int iPulse = thecore_pulse();

	if (ch->GetMall())
	{
#ifdef TEXTS_IMPROVEMENT
		ch->ChatPacketNew(CHAT_TYPE_INFO, 189, "");
#endif
		return;
	}

	if (iPulse - ch->GetMallLoadTime() < passes_per_sec * 10) // 10�ʿ� �ѹ��� ��û ����
	{
#ifdef TEXTS_IMPROVEMENT
		ch->ChatPacketNew(CHAT_TYPE_INFO, 190, "");
#endif
		return;
	}

	ch->SetMallLoadTime(iPulse);

	TSafeboxLoadPacket p;
	p.dwID = ch->GetDesc()->GetAccountTable().id;
	strlcpy(p.szLogin, ch->GetDesc()->GetAccountTable().login, sizeof(p.szLogin));
	strlcpy(p.szPassword, arg1, sizeof(p.szPassword));

	db_clientdesc->DBPacket(HEADER_GD_MALL_LOAD, ch->GetDesc()->GetHandle(), &p, sizeof(p));
}

ACMD(do_mall_close)
{
	if (ch->GetMall())
	{
		ch->SetMallLoadTime(thecore_pulse());
		ch->CloseMall();
		ch->Save();
	}
}

ACMD(do_ungroup)
{
	if (!ch->GetParty())
		return;

	if (!CPartyManager::instance().IsEnablePCParty())
	{
#ifdef TEXTS_IMPROVEMENT
		ch->ChatPacketNew(CHAT_TYPE_INFO, 208, "");
#endif
		return;
	}

	if (ch->GetDungeon())
	{
#ifdef TEXTS_IMPROVEMENT
		ch->ChatPacketNew(CHAT_TYPE_INFO, 202, "");
#endif
		return;
	}

	LPPARTY pParty = ch->GetParty();

	if (pParty->GetMemberCount() == 2)
	{
		// party disband
		CPartyManager::instance().DeleteParty(pParty);
	}
	else
	{
#ifdef TEXTS_IMPROVEMENT
		ch->ChatPacketNew(CHAT_TYPE_INFO, 215, "");
#endif
		//pParty->SendPartyRemoveOneToAll(ch);
		pParty->Quit(ch->GetPlayerID());
		//pParty->SendPartyRemoveAllToOne(ch);
	}
}

ACMD(do_close_shop)
{
	if (ch->GetMyShop())
	{
		ch->CloseMyShop();
		return;
	}
}

ACMD(do_set_walk_mode)
{
	ch->SetNowWalking(true);
	ch->SetWalking(true);
}

ACMD(do_set_run_mode)
{
	ch->SetNowWalking(false);
	ch->SetWalking(false);
}

ACMD(do_war)
{
	//�� ��� ������ ������
	CGuild * g = ch->GetGuild();

	if (!g)
		return;

	//���������� üũ�ѹ�!
	if (g->UnderAnyWar())
	{
#ifdef TEXTS_IMPROVEMENT
		ch->ChatPacketNew(CHAT_TYPE_INFO, 167, "");
#endif
		return;
	}

	//�Ķ���͸� �ι�� ������
	char arg1[256], arg2[256];
	DWORD type = GUILD_WAR_TYPE_FIELD; //fixme102 base int modded uint
	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1)
		return;

	if (*arg2)
	{
		str_to_number(type, arg2);
#ifdef ENABLE_BUG_FIXES
		if (type < 0) {
			return;
		}
#endif

		if (type >= GUILD_WAR_TYPE_MAX_NUM)
			type = GUILD_WAR_TYPE_FIELD;
	}

	//����� ������ ���̵� ���µ�
	DWORD gm_pid = g->GetMasterPID();

	//���������� üũ(������ ����常�� ����)
	if (gm_pid != ch->GetPlayerID())
	{
#ifdef TEXTS_IMPROVEMENT
		ch->ChatPacketNew(CHAT_TYPE_INFO, 144, "");
#endif
		return;
	}

	//��� ��带 ������
	CGuild * opp_g = CGuildManager::instance().FindGuildByName(arg1);

	if (!opp_g)
	{
#ifdef TEXTS_IMPROVEMENT
		ch->ChatPacketNew(CHAT_TYPE_INFO, 130, "");
#endif
		return;
	}

	//�������� ���� üũ
	switch (g->GetGuildWarState(opp_g->GetID()))
	{
		case GUILD_WAR_NONE:
			{
				if (opp_g->UnderAnyWar())
				{
#ifdef TEXTS_IMPROVEMENT
					ch->ChatPacketNew(CHAT_TYPE_INFO, 157, "");
#endif
					return;
				}

				int iWarPrice = KOR_aGuildWarInfo[type].iWarPrice;

				if (g->GetGuildMoney() < iWarPrice)
				{
#ifdef TEXTS_IMPROVEMENT
					ch->ChatPacketNew(CHAT_TYPE_INFO, 172, "");
#endif
					return;
				}

				if (opp_g->GetGuildMoney() < iWarPrice)
				{
#ifdef TEXTS_IMPROVEMENT
					ch->ChatPacketNew(CHAT_TYPE_INFO, 160, "");
#endif
					return;
				}
			}
			break;

		case GUILD_WAR_SEND_DECLARE:
			{
#ifdef TEXTS_IMPROVEMENT
				ch->ChatPacketNew(CHAT_TYPE_INFO, 438, "");
#endif
				return;
			}
			break;

		case GUILD_WAR_RECV_DECLARE:
			{
				if (opp_g->UnderAnyWar())
				{
#ifdef TEXTS_IMPROVEMENT
					ch->ChatPacketNew(CHAT_TYPE_INFO, 157, "");
#endif
					g->RequestRefuseWar(opp_g->GetID());
					return;
				}
			}
			break;

		case GUILD_WAR_RESERVE:
			{
#ifdef TEXTS_IMPROVEMENT
				ch->ChatPacketNew(CHAT_TYPE_INFO, 169, "");
#endif
				return;
			}
			break;

		case GUILD_WAR_END:
			return;

		default:
#ifdef TEXTS_IMPROVEMENT
			ch->ChatPacketNew(CHAT_TYPE_INFO, 168, "");
#endif
			g->RequestRefuseWar(opp_g->GetID());
			return;
	}

	if (!g->CanStartWar(type))
	{
		// ������� �� �� �ִ� ������ ���������ʴ´�.
		if (g->GetLadderPoint() == 0)
		{
#ifdef TEXTS_IMPROVEMENT
			ch->ChatPacketNew(CHAT_TYPE_INFO, 159, "");
#endif
			sys_log(0, "GuildWar.StartError.NEED_LADDER_POINT");
		}
		else if (g->GetMemberCount() < GUILD_WAR_MIN_MEMBER_COUNT)
		{
#ifdef TEXTS_IMPROVEMENT
			ch->ChatPacketNew(CHAT_TYPE_INFO, 145, "%d", GUILD_WAR_MIN_MEMBER_COUNT);
#endif
			sys_log(0, "GuildWar.StartError.NEED_MINIMUM_MEMBER[%d]", GUILD_WAR_MIN_MEMBER_COUNT);
		}
		else
		{
			sys_log(0, "GuildWar.StartError.UNKNOWN_ERROR");
		}
		return;
	}

	// �ʵ��� üũ�� �ϰ� ������ üũ�� ������ �³��Ҷ� �Ѵ�.
	if (!opp_g->CanStartWar(GUILD_WAR_TYPE_FIELD))
	{
#ifdef TEXTS_IMPROVEMENT
		if (opp_g->GetLadderPoint() == 0) {
			ch->ChatPacketNew(CHAT_TYPE_INFO, 153, "%s", opp_g->GetName());
		} else if (opp_g->GetMemberCount() < GUILD_WAR_MIN_MEMBER_COUNT) {
			ch->ChatPacketNew(CHAT_TYPE_INFO, 158, "");
		}
#endif
		return;
	}

	do
	{
		if (g->GetMasterCharacter() != NULL)
			break;

		CCI *pCCI = P2P_MANAGER::instance().FindByPID(g->GetMasterPID());

		if (pCCI != NULL)
			break;

#ifdef TEXTS_IMPROVEMENT
		ch->ChatPacketNew(CHAT_TYPE_INFO, 507, "");
#endif
		g->RequestRefuseWar(opp_g->GetID());
		return;

	} while (false);

	do
	{
		if (opp_g->GetMasterCharacter() != NULL)
			break;

		CCI *pCCI = P2P_MANAGER::instance().FindByPID(opp_g->GetMasterPID());

		if (pCCI != NULL)
			break;

#ifdef TEXTS_IMPROVEMENT
		ch->ChatPacketNew(CHAT_TYPE_INFO, 507, "");
#endif
		g->RequestRefuseWar(opp_g->GetID());
		return;

	} while (false);

	g->RequestDeclareWar(opp_g->GetID(), type);
}

ACMD(do_nowar)
{
	CGuild* g = ch->GetGuild();
	if (!g)
		return;

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	DWORD gm_pid = g->GetMasterPID();

	if (gm_pid != ch->GetPlayerID())
	{
#ifdef TEXTS_IMPROVEMENT
		ch->ChatPacketNew(CHAT_TYPE_INFO, 144, "");
#endif
		return;
	}

	CGuild* opp_g = CGuildManager::instance().FindGuildByName(arg1);

	if (!opp_g)
	{
#ifdef TEXTS_IMPROVEMENT
		ch->ChatPacketNew(CHAT_TYPE_INFO, 130, "");
#endif
		return;
	}
	
	g->RequestRefuseWar(opp_g->GetID());
}

ACMD(do_detaillog)
{
	ch->DetailLog();
}

ACMD(do_monsterlog)
{
	ch->ToggleMonsterLog();
}

ACMD(do_pkmode)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	BYTE mode = 0;
	str_to_number(mode, arg1);

	if (mode == PK_MODE_PROTECT)
		return;

	if (ch->GetLevel() < PK_PROTECT_LEVEL && mode != 0)
		return;

	ch->SetPKMode(mode);
}

ACMD(do_messenger_auth)
{
	if (ch->GetArena())
	{
#ifdef TEXTS_IMPROVEMENT
		ch->ChatPacketNew(CHAT_TYPE_INFO, 303, "");
#endif
		return;
	}

	char arg1[256], arg2[256];
	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1 || !*arg2)
		return;

	char answer = LOWER(*arg1);
	// @fixme130 AuthToAdd void -> bool
	bool bIsDenied = answer != 'y';
	bool bIsAdded = MessengerManager::instance().AuthToAdd(ch->GetName(), arg2, bIsDenied); // DENY
	if (bIsAdded && bIsDenied)
	{
		LPCHARACTER tch = CHARACTER_MANAGER::instance().FindPC(arg2);
#ifdef TEXTS_IMPROVEMENT
		if (tch) {
			tch->ChatPacketNew(CHAT_TYPE_INFO, 107, "%s", ch->GetName());
		}
#endif
	}
}

ACMD(do_setblockmode)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (*arg1)
	{
		BYTE flag = 0;
		str_to_number(flag, arg1);
		ch->SetBlockMode(flag);
	}
}

ACMD(do_unmount)
{

#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
	if(ch->GetWear(WEAR_COSTUME_MOUNT))
	{
		CMountSystem* mountSystem = ch->GetMountSystem();
		LPITEM mount = ch->GetWear(WEAR_COSTUME_MOUNT);
		DWORD mobVnum = 0;
		
		if (!mountSystem && !mount)
			return;
		
#ifdef __CHANGELOOK_SYSTEM__	
		if(mount->GetTransmutation())
		{
			const TItemTable* itemTable = ITEM_MANAGER::instance().GetTable(mount->GetTransmutation());
			
			if (itemTable)
				mobVnum = itemTable->alValues[1];
			else
				mobVnum = mount->GetValue(1);
		}
		else
			mobVnum = mount->GetValue(1);
#else
		if(mount->GetValue(1) != 0)
			mobVnum = mount->GetValue(1);
#endif

		if (ch->GetMountVnum())
		{
			if(mountSystem->CountSummoned() == 0)
			{
				mountSystem->Unmount(mobVnum);
			}
		}
		return;
	}
#endif

	if (true == ch->UnEquipSpecialRideUniqueItem())
	{
		ch->RemoveAffect(AFFECT_MOUNT);
		ch->RemoveAffect(AFFECT_MOUNT_BONUS);

		if (ch->IsHorseRiding())
		{
			ch->StopRiding();
		}
	}
#ifdef TEXTS_IMPROVEMENT
	else {
		ch->ChatPacketNew(CHAT_TYPE_INFO, 366, "");
	}
#endif
}

ACMD(do_observer_exit)
{
	if (ch->IsObserverMode())
	{
		if (ch->GetWarMap())
			ch->SetWarMap(NULL);

		if (ch->GetArena() != NULL || ch->GetArenaObserverMode() == true)
		{
			ch->SetArenaObserverMode(false);

			if (ch->GetArena() != NULL)
				ch->GetArena()->RemoveObserver(ch->GetPlayerID());

			ch->SetArena(NULL);
			ch->WarpSet(ARENA_RETURN_POINT_X(ch->GetEmpire()), ARENA_RETURN_POINT_Y(ch->GetEmpire()));
		}
		else
		{
			ch->ExitToSavedLocation();
		}
		ch->SetObserverMode(false);
	}
}

ACMD(do_view_equip)
{
	if (ch->GetGMLevel() <= GM_PLAYER)
		return;

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (*arg1)
	{
		DWORD vid = 0;
		str_to_number(vid, arg1);
		LPCHARACTER tch = CHARACTER_MANAGER::instance().Find(vid);

		if (!tch)
			return;

		if (!tch->IsPC())
			return;

		tch->SendEquipment(ch);
	}
}

ACMD(do_party_request)
{
	if (ch->GetArena())
	{
#ifdef TEXTS_IMPROVEMENT
		ch->ChatPacketNew(CHAT_TYPE_INFO, 303, "");
#endif
		return;
	}

	if (ch->GetParty())
	{
#ifdef TEXTS_IMPROVEMENT
		ch->ChatPacketNew(CHAT_TYPE_INFO, 441, "");
#endif
		return;
	}

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	DWORD vid = 0;
	str_to_number(vid, arg1);
	LPCHARACTER tch = CHARACTER_MANAGER::instance().Find(vid);

	if (tch)
		if (!ch->RequestToParty(tch))
			ch->ChatPacket(CHAT_TYPE_COMMAND, "PartyRequestDenied");
}

ACMD(do_party_request_accept)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	DWORD vid = 0;
	str_to_number(vid, arg1);
	LPCHARACTER tch = CHARACTER_MANAGER::instance().Find(vid);

	if (tch)
		ch->AcceptToParty(tch);
}

ACMD(do_party_request_deny)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	DWORD vid = 0;
	str_to_number(vid, arg1);
	LPCHARACTER tch = CHARACTER_MANAGER::instance().Find(vid);

	if (tch)
		ch->DenyToParty(tch);
}

// LUA_ADD_GOTO_INFO
struct GotoInfo
{
	std::string 	st_name;

	BYTE 	empire;
	int 	mapIndex;
	DWORD 	x, y;

	GotoInfo()
	{
		st_name 	= "";
		empire 		= 0;
		mapIndex 	= 0;

		x = 0;
		y = 0;
	}

	GotoInfo(const GotoInfo& c_src)
	{
		__copy__(c_src);
	}

	void operator = (const GotoInfo& c_src)
	{
		__copy__(c_src);
	}

	void __copy__(const GotoInfo& c_src)
	{
		st_name 	= c_src.st_name;
		empire 		= c_src.empire;
		mapIndex 	= c_src.mapIndex;

		x = c_src.x;
		y = c_src.y;
	}
};

#ifdef __ATTR_TRANSFER_SYSTEM__
ACMD(do_attr_transfer)
{
	if (!ch->CanDoAttrTransfer())
		return;
	
	sys_log(1, "%s has used an Attr Transfer command: %s.", ch->GetName(), argument);
	
	int w_index = 0, i_index = 0;
	const char *line;
	char arg1[256], arg2[256], arg3[256];
	line = two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));
	one_argument(line, arg3, sizeof(arg3));
	if (0 == arg1[0]) {
		return;
	}
	
	const std::string& strArg1 = std::string(arg1);
	if (strArg1 == "open")
	{
		AttrTransfer_open(ch);
		return;
	}
	else if (strArg1 == "close")
	{
		AttrTransfer_close(ch);
		return;
	}
	else if (strArg1 == "make")
	{
		AttrTransfer_make(ch);
		return;
	}
	else if (strArg1 == "add")
	{
		if (0 == arg2[0] || !isdigit(*arg2) || 0 == arg3[0] || !isdigit(*arg3))
			return;
		
		str_to_number(w_index, arg2);
		str_to_number(i_index, arg3);
		AttrTransfer_add_item(ch, w_index, i_index);
		return;
	}
	else if (strArg1 == "delete")
	{
		if (0 == arg2[0] || !isdigit(*arg2))
			return;
		
		str_to_number(w_index, arg2);
		AttrTransfer_delete_item(ch, w_index);
		return;
	}
	
	switch (LOWER(arg1[0]))
	{
		case 'o':
			AttrTransfer_open(ch);
			break;
		case 'c':
			AttrTransfer_close(ch);
			break;
		case 'm':
			AttrTransfer_make(ch);
			break;
		case 'a':
			{
				if (0 == arg2[0] || !isdigit(*arg2) || 0 == arg3[0] || !isdigit(*arg3))
					return;
				
				str_to_number(w_index, arg2);
				str_to_number(i_index, arg3);
				AttrTransfer_add_item(ch, w_index, i_index);
			}
			break;
		case 'd':
			{
				if (0 == arg2[0] || !isdigit(*arg2))
					return;
				
				str_to_number(w_index, arg2);
				AttrTransfer_delete_item(ch, w_index);
			}
			break;
		default:
			return;
	}
}
#endif

ACMD(do_inventory)
{
	int	index = 0;
	int	count		= 1;

	char arg1[256];
	char arg2[256];

	LPITEM	item;

	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1) {
		return;
	}

	if (!*arg2)
	{
		index = 0;
		str_to_number(count, arg1);
	}
	else
	{
		str_to_number(index, arg1); index = MIN(index, INVENTORY_MAX_NUM);
		str_to_number(count, arg2); count = MIN(count, INVENTORY_MAX_NUM);
	}

	for (int i = 0; i < count; ++i)
	{
		if (index >= INVENTORY_MAX_NUM)
			break;

		item = ch->GetInventoryItem(index);
#ifdef TEXTS_IMPROVEMENT
		if (item) {
			ch->ChatPacketNew(CHAT_TYPE_INFO, 727, "%d#%s", index, item->GetName());
		} else {
			ch->ChatPacketNew(CHAT_TYPE_INFO, 728, "%d", index);
		}
#endif
		++index;
	}
}

//gift notify quest command
ACMD(do_gift)
{
	ch->ChatPacket(CHAT_TYPE_COMMAND, "gift");
}

#ifdef __NEWPET_SYSTEM__
ACMD(do_CubePetAdd) {

	int pos = 0;
	int invpos = 0;

	const char *line;
	char arg1[256], arg2[256], arg3[256];

	line = two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));
	one_argument(line, arg3, sizeof(arg3));

	if (0 == arg1[0])
		return;
	const std::string& strArg1 = std::string(arg1);
	switch (LOWER(arg1[0]))
	{
	case 'a':	// add cue_index inven_index
	{
		if (0 == arg2[0] || !isdigit(*arg2) ||
			0 == arg3[0] || !isdigit(*arg3))
			return;

		str_to_number(pos, arg2);
		str_to_number(invpos, arg3);

	}
	break;

	default:
		return;
	}

	if (ch->GetNewPetSystem()->IsActivePet())
		ch->GetNewPetSystem()->SetItemCube(pos, invpos);
	else
		return;

}

ACMD(do_PetSkill) {
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));
	if (!*arg1)
		return;

	DWORD skillslot = 0;
	str_to_number(skillslot, arg1);
	if (skillslot > 3 || skillslot < 0)
		return;

	if (ch->GetNewPetSystem()->IsActivePet()) { 
		ch->GetNewPetSystem()->DoPetSkill(skillslot);
	}
#ifdef TEXTS_IMPROVEMENT
	else {
		ch->ChatPacketNew(CHAT_TYPE_INFO, 729, "");
	}
#endif
}

#ifdef ENABLE_NEW_PET_EDITS
ACMD(do_PetIncreaseSkill) {
	char arg1[256], arg2[256];
	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));
	
	if ((!*arg1) || (!*arg2))
		return;
	
	int iSlot = atoi(arg1), iType = atoi(arg2);
	if (!ch->GetNewPetSystem())
		return;
	
	if (ch->GetNewPetSystem()->IsActivePet())
		ch->GetNewPetSystem()->IncreasePetSkill(iSlot, iType);
}
#endif

ACMD(do_FeedCubePet) {
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));
	if (!*arg1)
		return;

	DWORD feedtype = 0;
	str_to_number(feedtype, arg1);
	if (ch->GetNewPetSystem()->IsActivePet()) {
		ch->GetNewPetSystem()->ItemCubeFeed(feedtype);
	}
#ifdef TEXTS_IMPROVEMENT
	else {
		ch->ChatPacketNew(CHAT_TYPE_INFO, 729, "");
	}
#endif
}

ACMD(do_PetEvo) {

	if (ch->GetExchange() || ch->GetMyShop() || ch->GetShopOwner() || ch->IsOpenSafebox() || ch->IsCubeOpen()) {
#ifdef TEXTS_IMPROVEMENT
		ch->ChatPacketNew(CHAT_TYPE_INFO, 730, "");
#endif
		return;
	}
	if (ch->GetNewPetSystem()->IsActivePet()) {
		int tmpevo = ch->GetNewPetSystem()->GetEvolution();
		if (((tmpevo == 0) && (ch->GetNewPetSystem()->GetLevel() >= 40)) || ((tmpevo == 1) && (ch->GetNewPetSystem()->GetLevel() >= 60)) || ((tmpevo == 2) && (ch->GetNewPetSystem()->GetLevel() >= 80))) {
#ifdef ENABLE_NEW_PET_EDITS
			if (ch->GetNewPetSystem()->GetExp() < ch->GetNewPetSystem()->GetNextExpFromMob()) {
#ifdef TEXTS_IMPROVEMENT
				ch->ChatPacketNew(CHAT_TYPE_INFO, 59, "");
#endif
				return;
			}
#endif
			
			bool bRet = false;
			DWORD dwItemVnum1 = 55003 + tmpevo;
			if (ch->CountSpecifyItem(dwItemVnum1) < 10) {
#ifdef TEXTS_IMPROVEMENT
				ch->ChatPacketNew(CHAT_TYPE_INFO, 60, "%d#%s", 10, 
#ifdef ENABLE_MULTI_NAMES
				ITEM_MANAGER::instance().GetTable(dwItemVnum1)->szLocaleName[ch->GetDesc()->GetLanguage()]
#else
				ITEM_MANAGER::instance().GetTable(dwItemVnum1)->szLocaleName
#endif
				);
#endif
				bRet = true;
			}
			
			DWORD dwItemVnum2 = 27992 + tmpevo;
			if (!bRet && ch->CountSpecifyItem(dwItemVnum2) < 10) {
#ifdef TEXTS_IMPROVEMENT
				ch->ChatPacketNew(CHAT_TYPE_INFO, 60, "%d#%s", 10, 
#ifdef ENABLE_MULTI_NAMES
				ITEM_MANAGER::instance().GetTable(dwItemVnum2)->szLocaleName[ch->GetDesc()->GetLanguage()]
#else
				ITEM_MANAGER::instance().GetTable(dwItemVnum2)->szLocaleName
#endif
				);
#endif
				bRet = true;
			}
			
			DWORD dwItemVnum3 = 86056 + tmpevo;
			if (!bRet && ch->CountSpecifyItem(dwItemVnum3) < 3) {
#ifdef TEXTS_IMPROVEMENT
				ch->ChatPacketNew(CHAT_TYPE_INFO, 60, "%d#%s", 3, 
#ifdef ENABLE_MULTI_NAMES
				ITEM_MANAGER::instance().GetTable(dwItemVnum3)->szLocaleName[ch->GetDesc()->GetLanguage()]
#else
				ITEM_MANAGER::instance().GetTable(dwItemVnum3)->szLocaleName
#endif
				);
#endif
				bRet = true;
			}
			
			if (bRet)
				return;
			
			ch->RemoveSpecifyItem(dwItemVnum1, 10);
			ch->RemoveSpecifyItem(dwItemVnum2, 10);
			ch->RemoveSpecifyItem(dwItemVnum3, 3);
			ch->GetNewPetSystem()->IncreasePetEvolution();
		}
		else {
#ifdef TEXTS_IMPROVEMENT
			ch->ChatPacketNew(CHAT_TYPE_INFO, 730, "");
#endif
			return;
		}
	}
#ifdef TEXTS_IMPROVEMENT
	else {
		ch->ChatPacketNew(CHAT_TYPE_INFO, 729, "");
	}
#endif

}

#endif

#ifdef ENABLE_CUBE_RENEWAL_WORLDARD
ACMD(do_cube)
{

	const char *line;
	char arg1[256], arg2[256], arg3[256];
	line = two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));
	one_argument(line, arg3, sizeof(arg3));

	if (0 == arg1[0])
	{
		return;
	}

	switch (LOWER(arg1[0]))
	{
		case 'o':	// open
			Cube_open(ch);
			break;

		default:
			return;
	}
}
#else
ACMD(do_cube)
{
	if (!ch->CanDoCube())
		return;

	dev_log(LOG_DEB0, "CUBE COMMAND <%s>: %s", ch->GetName(), argument);
	int cube_index = 0, inven_index = 0;
#ifdef ENABLE_EXTRA_INVENTORY
	BYTE window_type = 0;
	char arg1[256], arg2[256], arg3[256], arg4[256];

	two_arguments(two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2)), arg3, sizeof(arg3), arg4, sizeof(arg4));
#else
	const char *line;
	char arg1[256], arg2[256], arg3[256];

	line = two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));
	one_argument(line, arg3, sizeof(arg3));
#endif


	if (0 == arg1[0]) {
		return;
	}

	const std::string& strArg1 = std::string(arg1);

	// r_info (request information)
	// /cube r_info     ==> (Client -> Server) ���� NPC�� ���� �� �ִ� ������ ��û
	//					    (Server -> Client) /cube r_list npcVNUM resultCOUNT 123,1/125,1/128,1/130,5
	//
	// /cube r_info 3   ==> (Client -> Server) ���� NPC�� ����� �ִ� ������ �� 3��° �������� ����� �� �ʿ��� ������ ��û
	// /cube r_info 3 5 ==> (Client -> Server) ���� NPC�� ����� �ִ� ������ �� 3��° �����ۺ��� ���� 5���� �������� ����� �� �ʿ��� ��� ������ ��û
	//					   (Server -> Client) /cube m_info startIndex count 125,1|126,2|127,2|123,5&555,5&555,4/120000@125,1|126,2|127,2|123,5&555,5&555,4/120000
	//
	if (strArg1 == "r_info")
	{
		if (0 == arg2[0])
			Cube_request_result_list(ch);
		else
		{
			if (isdigit(*arg2))
			{
				int listIndex = 0, requestCount = 1;
				str_to_number(listIndex, arg2);

				if (0 != arg3[0] && isdigit(*arg3))
					str_to_number(requestCount, arg3);

				Cube_request_material_info(ch, listIndex, requestCount);
			}
		}

		return;
	}

	switch (LOWER(arg1[0]))
	{
		case 'o':	// open
			Cube_open(ch);
			break;

		case 'c':	// close
			Cube_close(ch);
			break;

		case 'l':	// list
			Cube_show_list(ch);
			break;

		case 'a':	// add cue_index inven_index
			{
#ifdef ENABLE_EXTRA_INVENTORY
				if (arg2[0] == 0 || !isdigit(*arg2) || arg3[0] == 0 || !isdigit(*arg3) || arg4[0] == 0 || !isdigit(*arg4))
#else
				if (0 == arg2[0] || !isdigit(*arg2) ||
					0 == arg3[0] || !isdigit(*arg3))
#endif
					return;

				str_to_number(cube_index, arg2);
				str_to_number(inven_index, arg3);
#ifdef ENABLE_EXTRA_INVENTORY
				str_to_number(window_type, arg4);
				Cube_add_item(ch, cube_index, inven_index, window_type);
#else
				Cube_add_item (ch, cube_index, inven_index);
#endif
			}
			break;

		case 'd':	// delete
			{
				if (0 == arg2[0] || !isdigit(*arg2))
					return;

				str_to_number(cube_index, arg2);
				Cube_delete_item (ch, cube_index);
			}
			break;

		case 'm':	// make
			if (0 != arg2[0])
			{
				while (true == Cube_make(ch))
					dev_log (LOG_DEB0, "cube make success");
			}
			else
				Cube_make(ch);
			break;

		default:
			return;
	}
}
#endif


ACMD(do_in_game_mall)
{
	if (!ch)
		return;
	
	char buf[512+1];
	char sas[33];
	MD5_CTX ctx;
	const char secretKey[] = "base64:vKqgEB0ho5Swmzh+bQTAmBoWpOk8z2yIFaxsIJMOzvE=";
	const char websiteUrl[] = "https://wonder2.org";
	snprintf(buf, sizeof(buf), "%u%s", ch->GetAID(), secretKey);
	MD5Init(&ctx);
	MD5Update(&ctx, (const unsigned char *) buf, strlen(buf));
#ifdef __FreeBSD__
	MD5End(&ctx, sas);
#else
	static const char hex[] = "0123456789abcdef";
	unsigned char digest[16];
	MD5Final(digest, &ctx);
	int i;
	for (i = 0; i < 16; ++i) {
		sas[i+i] = hex[digest[i] >> 4];
		sas[i+i+1] = hex[digest[i] & 0x0f];
	}
	
	sas[i+i] = '\0';
#endif

#ifdef ENABLE_MULTI_LANGUAGE
	BYTE lang = ch->GetDesc() ? ch->GetDesc()->GetLanguage() : 0;
	std::string str_lang;
	switch (lang) {
		case LANGUAGE_RO: {
			str_lang = "ro";
			break;
		}
		case LANGUAGE_IT: {
			str_lang = "it";
			break;
		}
		case LANGUAGE_TR: {
			str_lang = "tr";
			break;
		}
		case LANGUAGE_DE: {
			str_lang = "de";
			break;
		}
		case LANGUAGE_PL: {
			str_lang = "pl";
			break;
		}
		case LANGUAGE_PT: {
			str_lang = "pt";
			break;
		}
		case LANGUAGE_ES: {
			str_lang = "es";
			break;
		}
		case LANGUAGE_CZ: {
			str_lang = "cz";
			break;
		}
		case LANGUAGE_HU: {
			str_lang = "hu";
			break;
		}
		default: {
			str_lang = "en";
			break;
		}
	}

	snprintf(buf, sizeof(buf), "mall %s/in-game-shop?aid=%u&secret=%s&lang=%s", websiteUrl, ch->GetAID(), sas, str_lang.c_str());
#else
	snprintf(buf, sizeof(buf), "mall %s/in-game-shop?aid=%u&secret=%s", websiteUrl, ch->GetAID(), sas);
#endif
	ch->ChatPacket(CHAT_TYPE_COMMAND, buf);
}

// �ֻ���
ACMD(do_dice)
{
#ifdef TEXTS_IMPROVEMENT
	char arg1[256], arg2[256];
	int start = 1, end = 100;

	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (*arg1 && *arg2)
	{
		start = atoi(arg1);
		end = atoi(arg2);
	}
	else if (*arg1 && !*arg2)
	{
		start = 1;
		end = atoi(arg1);
	}

	end = MAX(start, end);
	start = MIN(start, end);

	int n = number(start, end);
	if (ch->GetParty()) {
		ch->GetParty()->ChatPacketToAllMemberNew(
#ifdef ENABLE_DICE_SYSTEM
		CHAT_TYPE_DICE_INFO
#else
		CHAT_TYPE_INFO
#endif
		, 544, "%s#%d#%d#%d", ch->GetName(), n, start, end);
	} else {
		ch->ChatPacketNew(
#ifdef ENABLE_DICE_SYSTEM
		CHAT_TYPE_DICE_INFO
#else
		CHAT_TYPE_INFO
#endif
		, 545, "%d#%d#%d", n, start, end);
	}
#endif
}

#ifdef ENABLE_NEWSTUFF
ACMD(do_click_safebox)
{
	if (ch->GetDungeon() || ch->GetWarMap())
	{
#ifdef TEXTS_IMPROVEMENT
		ch->ChatPacketNew(CHAT_TYPE_INFO, 731, "");
#endif
		return;
	}

	ch->SetSafeboxOpenPosition();
	ch->ChatPacket(CHAT_TYPE_COMMAND, "ShowMeSafeboxPassword");
}
ACMD(do_force_logout)
{
	LPDESC pDesc=DESC_MANAGER::instance().FindByCharacterName(ch->GetName());
	if (!pDesc)
		return;
	pDesc->DelayedDisconnect(0);
}
#endif

ACMD(do_click_mall)
{
	ch->ChatPacket(CHAT_TYPE_COMMAND, "ShowMeMallPassword");
}

ACMD(do_ride)
{
    dev_log(LOG_DEB0, "[DO_RIDE] start");
    if (ch->IsDead() || ch->IsStun())
		return;

	if (ch->GetMapIndex() == 113)
		return;

#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
	if (ch->IsPolymorphed() == true){
#ifdef TEXTS_IMPROVEMENT
		ch->ChatPacketNew(CHAT_TYPE_INFO, 732, "");
#endif
		return;
	}
	if(ch->GetWear(WEAR_COSTUME_MOUNT))
	{
		CMountSystem* mountSystem = ch->GetMountSystem();
		LPITEM mount = ch->GetWear(WEAR_COSTUME_MOUNT);
		DWORD mobVnum = 0;
		
		if (!mountSystem && !mount)
			return;
		
#ifdef __CHANGELOOK_SYSTEM__	
		if(mount->GetTransmutation())
		{
			const TItemTable* itemTable = ITEM_MANAGER::instance().GetTable(mount->GetTransmutation());
			
			if (itemTable)
				mobVnum = itemTable->alValues[1];
			else
				mobVnum = mount->GetValue(1);
		}
		else
			mobVnum = mount->GetValue(1);
#else
		if(mount->GetValue(1) != 0)
			mobVnum = mount->GetValue(1);
#endif

		if (ch->GetMountVnum())
		{
			if(mountSystem->CountSummoned() == 0)
			{
				mountSystem->Unmount(mobVnum);
			}
		}
		else
		{
			if(mountSystem->CountSummoned() == 1)
			{
				mountSystem->Mount(mobVnum, mount);
			}
		}
		
		return;
	}
#endif

	if (ch->IsHorseRiding())
	{
		ch->StopRiding();
		return;
	}

	if (ch->GetHorse() != NULL)
	{
	    ch->StartRiding();
	    return;
	}

	for (BYTE i=0; i<INVENTORY_MAX_NUM; ++i)
	{
		LPITEM item = ch->GetInventoryItem(i);
		if (NULL == item)
			continue;

		if (item->GetType() == ITEM_COSTUME && item->GetSubType() == COSTUME_MOUNT)	{
			ch->UseItem(TItemPos (INVENTORY, i));
			return;
		}
	}
	
#ifdef TEXTS_IMPROVEMENT
	ch->ChatPacketNew(CHAT_TYPE_INFO, 5, "");
#endif
}

#ifdef ENABLE_GAYA_SYSTEM
ACMD(do_gaya_system)
{
	if (quest::CQuestManager::instance().GetEventFlag("gaya_disable") == 1)
	{
#ifdef TEXTS_IMPROVEMENT
		ch->ChatPacketNew(CHAT_TYPE_INFO, 734, "");
#endif
		return;
	}

	char arg1[255];
	char arg2[255];
	char arg3[256];
	three_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2), arg3, sizeof(arg3));

	if (!*arg1)
		return;

	if (0 == arg1[0])
		return;

	const std::string& strArg1 = std::string(arg1);
	if (strArg1 == "craft")
	{
		if (0 == arg2[0])
			return;

		int slot = atoi(arg2);
		ch->CraftGayaItems(slot);
	}
	else if (strArg1 == "market")
	{
		if (0 == arg2[0])
			return;

		int slot = atoi(arg2);
		ch->MarketGayaItems(slot);
	}
	else if (strArg1 == "refresh")
	{
		ch->RefreshGayaItems();
	}
}
#endif

#ifdef __ENABLE_RANGE_ALCHEMY__
ACMD(do_extend_range_npc)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));
	
	if (!*arg1)
		return;
	
	
	DWORD vnum = 0;
	str_to_number(vnum, arg1);
	
	if (ch->IsDead())
		return;
	
	if (ch->IsDead() || ch->GetExchange() || ch->GetMyShop() || ch->IsOpenSafebox() || ch->IsCubeOpen())
	{
#ifdef TEXTS_IMPROVEMENT
		ch->ChatPacketNew(CHAT_TYPE_INFO, 735, "");
#endif
		return;
	}
	
	LPSHOP shop = CShopManager::instance().Get(vnum);
	
	if(!shop)
		return;
	
	ch->SetShopOwner(ch);
	shop->AddGuest(ch, 0, false);
	
}
#endif



#ifdef __ENABLE_REFINE_ALCHEMY__
ACMD(do_refine_window_alchemy) {
	ch->DragonSoul_RefineWindow_Open(ch);
}
#endif



#ifdef __HIDE_COSTUME_SYSTEM__
ACMD(do_hide_costume)
{
	char arg1[256], arg2[256];
	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1)
		return;

	bool hidden = true;
	BYTE bPartPos = 0;
	BYTE bHidden = 0;

	str_to_number(bPartPos, arg1);

	if (*arg2)
	{
		str_to_number(bHidden, arg2);

		if (bHidden == 0)
			hidden = false;
	}

	if (bPartPos == 1)
		ch->SetBodyCostumeHidden(hidden);
	else if (bPartPos == 2)
		ch->SetHairCostumeHidden(hidden);
	else if (bPartPos == 3)
		ch->SetAcceCostumeHidden(hidden);
	else if (bPartPos == 4)
		ch->SetWeaponCostumeHidden(hidden);
	else
		return;

	ch->UpdatePacket();
}
#endif

#ifdef ENABLE_CHANGE_RACE
ACMD(do_change_race)
{
	char arg1[256], arg2[256];
	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));
	// init
	bool bIsSetSkillGroup = false;
	DWORD dwRace = MAIN_RACE_MAX_NUM;
	DWORD dwSkillGroup = 0;
	// check arg1
	if (!*arg1)
	{
		goto USAGE;
		return;
	}
	// check&analyze arg2
	if (*arg2)
	{
		str_to_number(dwSkillGroup, arg2);
		dwSkillGroup = MINMAX(0, dwSkillGroup, 2);
		bIsSetSkillGroup = true;
	}
	// analyze arg1
	str_to_number(dwRace, arg1);
	if (dwRace >= MAIN_RACE_MAX_NUM)
	{
		goto USAGE;
		return;
	}
	// skip if same race
	if (dwRace==ch->GetRaceNum())
		return;
	// process change race
	ch->ChatPacket(CHAT_TYPE_INFO, "Old Race=%u, Group=%u", ch->GetRaceNum(), ch->GetSkillGroup());
	ch->SetRace(dwRace);
	ch->ClearSkill();
	// ch->ClearSubSkill();
	if (bIsSetSkillGroup)
	{
		ch->SetSkillGroup(dwSkillGroup);
	}
	// quick mesh change workaround begin
	ch->SetPolymorph(101);
	ch->SetPolymorph(0);
	// quick mesh change workaround end
	ch->ChatPacket(CHAT_TYPE_INFO, "New Race=%u, Group=%u", ch->GetRaceNum(), ch->GetSkillGroup());
	return;
	// usage
USAGE:
	ch->ChatPacket(CHAT_TYPE_INFO, "Usage: /change_race <race_id> <&skill_group>");
	// race list
	ch->ChatPacket(CHAT_TYPE_INFO, "RACE-LIST");
	ch->ChatPacket(CHAT_TYPE_INFO, "\tWarrior M = %d", MAIN_RACE_WARRIOR_M);
	ch->ChatPacket(CHAT_TYPE_INFO, "\tNinja F = %d", MAIN_RACE_ASSASSIN_W);
	ch->ChatPacket(CHAT_TYPE_INFO, "\tSura M = %d", MAIN_RACE_SURA_M); 
	ch->ChatPacket(CHAT_TYPE_INFO, "\tShaman F = %d", MAIN_RACE_SHAMAN_W);
	ch->ChatPacket(CHAT_TYPE_INFO, "\tWarrior W = %d", MAIN_RACE_WARRIOR_W);
	ch->ChatPacket(CHAT_TYPE_INFO, "\tAssassin M = %d", MAIN_RACE_ASSASSIN_M);
	ch->ChatPacket(CHAT_TYPE_INFO, "\tSura W = %d", MAIN_RACE_SURA_W);
	ch->ChatPacket(CHAT_TYPE_INFO, "\tShaman M = %d", MAIN_RACE_SHAMAN_M);
#ifdef ENABLE_WOLFMAN_CHARACTER
	ch->ChatPacket(CHAT_TYPE_INFO, "\tWolfman M = %d", MAIN_RACE_WOLFMAN_M);
#endif
	ch->ChatPacket(CHAT_TYPE_INFO, "\tRACE_MAX_NUM = %d", MAIN_RACE_MAX_NUM);
	// group list
	ch->ChatPacket(CHAT_TYPE_INFO, "GROUP-LIST");
	ch->ChatPacket(CHAT_TYPE_INFO, "\tNone = 0");
	ch->ChatPacket(CHAT_TYPE_INFO, "\tFirst = 1");
	ch->ChatPacket(CHAT_TYPE_INFO, "\tSecond = 2");
	return;
}
#endif

#ifdef ENABLE_RUNE_SYSTEM
#include "shop.h"
#include "shop_manager.h"
#include "../../common/rune_length.h"

ACMD(do_rune)
{
	if (!ch)
		return;
	
	char arg1[512];
	const char* rest = one_argument(argument, arg1, sizeof(arg1));
	switch (arg1[0])
	{
		case 'a':
			{
				one_argument(rest, arg1, sizeof(arg1));
				int slot;
				if (str_to_number(slot, arg1) == false)
					return;
				
				if (slot == WEAR_RUNE7)
					return;
				
				LPITEM pkItem = ch->GetWear(slot);
				if (pkItem)
					pkItem->ActivateRune();
			}
			break;
		case 'd':
			{
				one_argument(rest, arg1, sizeof(arg1));
				int slot;
				if (str_to_number(slot, arg1) == false)
					return;
				
				if (slot == WEAR_RUNE7)
					return;
				
				LPITEM pkItem = ch->GetWear(slot);
				if (pkItem)
					pkItem->DeactivateRune();
			}
			break;
		case 'l':
			{
				one_argument(rest, arg1, sizeof(arg1));
				int w;
				if (str_to_number(w, arg1) == false)
					return;
				
				int iMaxSubTypes = RUNE_SUBTYPES - 1;
				LPITEM pkItem = NULL;
				for (int i = 0; i < iMaxSubTypes; i++) {
					pkItem = ch->GetWear(WEAR_RUNE1 + i);
					if (pkItem) {
						if (w == 0)
							pkItem->DeactivateRune();
						else
							pkItem->ActivateRune();
					}
				}
			}
			break;
	}
}

ACMD(do_rune_charge)
{
	if (!ch)
		return;
	
	char arg1[256], arg2[256];
	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));
	
	if ((!*arg1) || (!*arg2))
		return;
	
	int iArg1 = 0;
	if (str_to_number(iArg1, arg1) == false)
		return;
	
	int iArg2 = 0;
	if (str_to_number(iArg2, arg2) == false)
		return;
	
	LPITEM pkRune = ch->GetWear(iArg1);
	if (!pkRune)
		return;
	
	if (!pkRune->IsRune())
		return;
	else if (pkRune->GetSubType() == RUNE_SLOT7)
		return;
	
	LPITEM pkBottle = ch->GetInventoryItem(iArg2);
	if (!pkBottle)
		return;
	
	if ((pkBottle->GetType() != ITEM_USE) && (pkBottle->GetSubType() != USE_RUNE_PERC_CHARGE))
		return;
	
	if (pkBottle->GetCount() > 1) {
		int pos = ch->GetEmptyInventory(pkBottle->GetSize());
		if (pos == -1) {
#ifdef TEXTS_IMPROVEMENT
			ch->ChatPacketNew(CHAT_TYPE_INFO, 366, "");
#endif
			return;
		}
		
		pkBottle->SetCount(pkBottle->GetCount() - 1);
		LPITEM item2 = ITEM_MANAGER::instance().CreateItem(pkBottle->GetVnum(), 1);
		if (!item2)
			return;
		
		item2->AddToCharacter(ch, TItemPos(INVENTORY, pos), false);
		pkBottle = item2;
	}
	
	long lBottlePercent = pkBottle->GetSocket(0);
	if (lBottlePercent < 1)
		return;
	
	long lMaxTime = pkRune->GetValue(0);
	long lOnePercent = lMaxTime / 100;
	long lRemainPercent = pkRune->GetSocket(ITEM_SOCKET_REMAIN_SEC) / lOnePercent;
	if (lRemainPercent > 99) {
#ifdef TEXTS_IMPROVEMENT
		ch->ChatPacketNew(CHAT_TYPE_INFO, 33, "%s", pkRune->GetName());
#endif
		return;
	}
	
	long dif = 100 - lRemainPercent;
	dif = dif > lBottlePercent ? lBottlePercent : dif;
	long add = lOnePercent * dif;
	long lValue = pkRune->GetSocket(ITEM_SOCKET_REMAIN_SEC) + add;
	pkRune->SetSocket(ITEM_SOCKET_REMAIN_SEC, lValue);
#ifdef TEXTS_IMPROVEMENT
	ch->ChatPacketNew(CHAT_TYPE_INFO, 34, "%s#%d", pkRune->GetName(), dif);
#endif
	pkBottle->SetSocket(0, lBottlePercent-dif);
	if (pkBottle->GetSocket(0) < 1)
		pkBottle->RemoveFromCharacter();
	
	pkRune->ChangeRuneAttr(lValue);
	if ((!ch->FindAffect(AFFECT_RUNE2)) && (pkRune->GetSocket(1) == 1)) {
		if (long(lValue / (pkRune->GetValue(0) / 100)) >= 50) {
			pkRune->ActivateRuneBonus();
		}
	}
}

ACMD(do_rune_shop)
{
	if (!ch)
		return;
	
	if (ch->IsOpenSafebox() || ch->GetExchange() || ch->GetMyShop() || ch->IsCubeOpen())
	{
#ifdef TEXTS_IMPROVEMENT
		ch->ChatPacketNew(CHAT_TYPE_INFO, 294, "");
#endif
		return;
	}
	
	LPSHOP pkShop = CShopManager::instance().Get(RUNE_SHOP);
	if (pkShop) {
		pkShop->AddGuest(ch, 0, false);
		ch->SetShopOwner(NULL);
	}
}

ACMD(do_rune_effect)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));
	
	if (!*arg1)
		return;
	
	int iArg1 = 0;
	if (str_to_number(iArg1, arg1) == false)
		return;
	
	if ((iArg1 != 0) && (iArg1 != 1))
		return;
	
	if (ch->GetQuestFlag("rune.hide_effect") == iArg1)
		return;
	
	ch->SetQuestFlag("rune.hide_effect", iArg1);
	ch->ComputePoints();
	ch->UpdatePacket();
	ch->ChatPacket(CHAT_TYPE_COMMAND, "rune_affect %d", iArg1);
}
#endif
#ifdef ENABLE_EVENT_MANAGER
ACMD(do_event_manager)
{
	std::vector<std::string> vecArgs;
	split_argument(argument, vecArgs);
	if (vecArgs.size() < 2) { return; }
	else if (vecArgs[1] == "info")
	{
		CHARACTER_MANAGER::Instance().SendDataPlayer(ch);
	}
	else if (vecArgs[1] == "remove")
	{
		if (!ch->IsGM())
			return;

		if (vecArgs.size() < 3) { 
			
			ch->ChatPacket(CHAT_TYPE_INFO, "Put the event index!!");
			return; 
		}

		BYTE removeIndex;
		str_to_number(removeIndex, vecArgs[2].c_str());

		if(CHARACTER_MANAGER::Instance().CloseEventManuel(removeIndex))
			ch->ChatPacket(CHAT_TYPE_INFO, "Successfuly remove!");
		else
			ch->ChatPacket(CHAT_TYPE_INFO, "Don't has any event!");
	}
	else if (vecArgs[1] == "update")
	{
		if (!ch->IsGM())
			return;
		const BYTE subHeader = EVENT_MANAGER_UPDATE;
		//db_clientdesc->DBPacketHeader(HEADER_GD_EVENT_MANAGER, 0, sizeof(BYTE));
		//db_clientdesc->Packet(&subHeader, sizeof(BYTE));
		db_clientdesc->DBPacket(HEADER_GD_EVENT_MANAGER, 0, &subHeader, sizeof(BYTE));

		ch->ChatPacket(CHAT_TYPE_INFO, "Successfully update!");
	}
}
#endif
