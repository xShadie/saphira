#include "stdafx.h"
#include "utils.h"
#include "config.h"
#include "char.h"
#include "locale_service.h"
#include "log.h"
#include "desc.h"

#ifdef ENABLE_RENEWAL_ADD_STATS
ACMD(do_stat_val);
#endif

ACMD(do_user_horse_ride);
ACMD(do_user_horse_back);
ACMD(do_user_horse_feed);

ACMD(do_pcbang_update);
ACMD(do_pcbang_check);
#ifdef __NEWPET_SYSTEM__
ACMD(do_CubePetAdd);
ACMD(do_FeedCubePet);
ACMD(do_PetSkill);
ACMD(do_PetEvo);
#ifdef ENABLE_NEW_PET_EDITS
ACMD(do_PetIncreaseSkill);
#endif
#endif
// ADD_COMMAND_SLOW_STUN
ACMD(do_slow);
ACMD(do_stun);
// END_OF_ADD_COMMAND_SLOW_STUN
ACMD(do_daily_reward_get_reward);
ACMD(do_daily_reward_reload);
ACMD(do_warp);
#ifdef REGEN_ANDRA
// reload_regen
ACMD ( do_free_regen );
// reload_regen
#endif
ACMD(do_goto);
ACMD(do_item);
ACMD(do_mob);
ACMD(do_mob_ld);
ACMD(do_mob_aggresive);
ACMD(do_mob_coward);
ACMD(do_mob_map);
ACMD(do_purge);
ACMD(do_weaken);
ACMD(do_item_purge);
ACMD(do_state);
ACMD(do_notice);
ACMD(do_map_notice);
ACMD(do_big_notice);
#ifdef ENABLE_FULL_NOTICE
ACMD(do_notice_test);
ACMD(do_big_notice_test);
ACMD(do_map_big_notice);
#endif
ACMD(do_who);
ACMD(do_user);
ACMD(do_disconnect);
ACMD(do_kill);
ACMD(do_emotion_allow);
ACMD(do_emotion);
ACMD(do_transfer);
ACMD(do_set);
ACMD(do_cmd);
ACMD(do_reset);
ACMD(do_greset);
ACMD(do_fishing);
ACMD(do_refine_rod);

// REFINE_PICK
ACMD(do_max_pick);
ACMD(do_refine_pick);
// END_OF_REFINE_PICK

ACMD(do_fishing_simul);
ACMD(do_console);
ACMD(do_restart);
ACMD(do_advance);
ACMD(do_stat);
ACMD(do_respawn);
ACMD(do_skillup);
ACMD(do_guildskillup);
ACMD(do_pvp);
#ifdef ENABLE_PVP_ADVANCED
ACMD(do_pvp_advanced);
ACMD(do_decline_pvp);
ACMD(do_block_equipment);
#endif
ACMD(do_point_reset);
ACMD(do_safebox_size);
ACMD(do_safebox_close);
ACMD(do_safebox_password);
ACMD(do_safebox_change_password);
ACMD(do_mall_password);
ACMD(do_mall_close);
ACMD(do_ungroup);
ACMD(do_makeguild);
ACMD(do_deleteguild);
ACMD(do_shutdown);
ACMD(do_group);
ACMD(do_group_random);
ACMD(do_invisibility);
ACMD(do_event_flag);
ACMD(do_get_event_flag);
ACMD(do_private);
ACMD(do_qf);
ACMD(do_clear_quest);
ACMD(do_book);
ACMD(do_reload);
ACMD(do_war);
ACMD(do_nowar);
ACMD(do_setskill);
ACMD(do_setskillother);
ACMD(do_level);
ACMD(do_polymorph);
ACMD(do_polymorph_item);
ACMD(do_close_shop);
ACMD(do_set_walk_mode);
ACMD(do_set_run_mode);
ACMD(do_set_skill_group);
ACMD(do_set_skill_point);
ACMD(do_cooltime);
ACMD(do_detaillog);
ACMD(do_monsterlog);

ACMD(do_gwlist);
ACMD(do_stop_guild_war);
ACMD(do_cancel_guild_war);
ACMD(do_guild_state);

ACMD(do_pkmode);
ACMD(do_mobile);
ACMD(do_mobile_auth);
ACMD(do_messenger_auth);

#ifdef ENABLE_SORT_INVEN
ACMD(do_item_check);
ACMD(do_sort_extra_inventory);
#endif

ACMD(do_getqf);
ACMD(do_setqf);
ACMD(do_delqf);
ACMD(do_set_state);

ACMD(do_forgetme);
ACMD(do_aggregate);
ACMD(do_attract_ranger);
ACMD(do_pull_monster);
ACMD(do_setblockmode);
ACMD(do_priv_empire);
ACMD(do_priv_guild);
ACMD(do_mount_test);
ACMD(do_unmount);
ACMD(do_observer);
ACMD(do_observer_exit);
ACMD(do_socket_item);
ACMD(do_stat_minus);
ACMD(do_stat_reset);
ACMD(do_view_equip);
ACMD(do_block_chat);
ACMD(do_vote_block_chat);

// BLOCK_CHAT
ACMD(do_block_chat_list);
// END_OF_BLOCK_CHAT

ACMD(do_party_request);
ACMD(do_party_request_deny);
ACMD(do_party_request_accept);
ACMD(do_build);
ACMD(do_clear_land);

ACMD(do_horse_state);
ACMD(do_horse_level);
ACMD(do_horse_ride);
ACMD(do_horse_summon);
ACMD(do_horse_unsummon);
ACMD(do_horse_set_stat);

ACMD(do_save_attribute_to_image);
ACMD(do_affect_remove);

ACMD(do_change_attr);
ACMD(do_add_attr);
ACMD(do_add_socket);

ACMD(do_inputall)
{
#ifdef TEXTS_IMPROVEMENT
	ch->ChatPacketNew(CHAT_TYPE_INFO, 342, "");
#endif
}

ACMD(do_show_arena_list);
ACMD(do_end_all_duel);
ACMD(do_end_duel);
ACMD(do_duel);

ACMD(do_stat_plus_amount);

ACMD(do_break_marriage);

ACMD(do_oxevent_show_quiz);
ACMD(do_oxevent_log);
ACMD(do_oxevent_get_attender);

ACMD(do_effect);

//gift notify quest command
ACMD(do_gift);
// 큐브관련
#ifdef __ATTR_TRANSFER_SYSTEM__
ACMD(do_attr_transfer);
#endif
ACMD(do_inventory);
ACMD(do_cube);
// 공성전

ACMD(do_reset_subskill );
ACMD(do_flush);

ACMD(do_eclipse);

ACMD(do_in_game_mall);

ACMD(do_get_mob_count);

ACMD(do_dice);
ACMD(do_special_item);

ACMD(do_click_mall);

ACMD(do_ride);
ACMD(do_get_item_id_list);
ACMD(do_set_socket);

// 코스츔 상태보기 및 벗기
ACMD(do_set_stat);

// 무적
ACMD (do_can_dead);

ACMD (do_full_set);
// 직군과 레벨에 따른 최고 아이템
ACMD (do_item_full_set);
// 직군에 따른 최고 옵션의 속성 셋팅
ACMD (do_attr_full_set);
// 모든 스킬 마스터
ACMD (do_all_skill_master);
// 아이템 착용. icon이 없어 클라에서 확인 할 수 없는 아이템 착용을 위해 만듦.
ACMD (do_use_item);
ACMD (do_dragon_soul);
ACMD (do_clear_affect);
#ifdef __ENABLE_NEW_OFFLINESHOP__
ACMD(do_offshop_change_shop_name);
ACMD(do_offshop_force_close_shop);
#endif

#ifdef ENABLE_NEWSTUFF
ACMD(do_change_rare_attr);
ACMD(do_add_rare_attr);

ACMD(do_click_safebox);
ACMD(do_force_logout);

ACMD(do_poison);
ACMD(do_rewarp);
#endif
#ifdef ENABLE_WOLFMAN_CHARACTER
ACMD(do_bleeding);
#endif
#ifdef ENABLE_GAYA_SYSTEM
ACMD(do_gaya_system);
#endif


#ifdef __ENABLE_RANGE_ALCHEMY__
ACMD(do_extend_range_npc);
#endif
#ifdef __ENABLE_REFINE_ALCHEMY__
ACMD(do_refine_window_alchemy);
#endif


#ifdef ENABLE_CHANNEL_SWITCH_SYSTEM
ACMD(do_change_channel);
#endif
#ifdef __HIDE_COSTUME_SYSTEM__
ACMD(do_hide_costume);
#endif
#ifdef ENABLE_EVENT_MANAGER
ACMD(do_event_manager);
#endif

#ifdef ENABLE_ATTR_COSTUMES
ACMD(do_attrdialog_remove) {
	if (ch->IsObserverMode() || ch->IsDead()) {
		return;
	}

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));
	if (!*arg1) {
		return;
	}

	int iArg = atoi(arg1);
	if (iArg > 1) {
		return;
	}

	ch->SetAttrDialogRemove(iArg);
}
#endif

#ifdef ENABLE_RANKING
ACMD(do_ranking_subcategory)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));
	if (!*arg1)
		return;
	
	ch->RankingSubcategory(atoi(arg1));
}
#endif

ACMD(do_manage_exp)
{
	bool arg = ch->Block_Exp == false ? true : false;
	ch->Block_Exp = arg;
	
	if( true == ch->IsDead() )
	{
		ch->ChatPacketNew(CHAT_TYPE_INFO, 3037, "");
		return;
	}
	
#ifdef TEXTS_IMPROVEMENT
	if (arg)
	{
		ch->ChatPacketNew(CHAT_TYPE_INFO, 77, "");
	}
	else
	{
		ch->ChatPacketNew(CHAT_TYPE_INFO, 78, "");
	}
#endif
	ch->SetQuestFlag("exp.stat", arg == true ? 1 : 0);
	ch->ChatPacket(CHAT_TYPE_COMMAND, "manage_exp_status %d", arg == true ? 1 : 0);
}

#ifdef ENABLE_LOCKED_EXTRA_INVENTORY
ACMD(do_unlock_extra)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));
	if (!*arg1)
		return;
	
	ch->UnlockExtraInventory(atoi(arg1));
}
#endif

#ifdef ENABLE_NEW_PET_EDITS
ACMD(do_petenchant)
{
	if ((ch->IsObserverMode()) || (ch->IsDead()) || (ch->IsStun()))
		return;
	
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));
	if (!*arg1)
		return;
	
	int iArg = atoi(arg1);
	if ((iArg < 0) || (iArg > 2))
		return;
	
	ch->SetPetEnchant(iArg);
}
#endif

#ifdef ENABLE_RUNE_SYSTEM
ACMD(do_rune);
ACMD(do_rune_charge);
ACMD(do_rune_shop);
ACMD(do_rune_effect);
#endif
ACMD(do_remove_affect);
// ACMD(do_stat2);
#ifdef ENABLE_BIOLOGIST_UI
ACMD(do_open_biologist);
ACMD(do_delivery_biologist);
ACMD(do_reward_biologist);
ACMD(do_change_biologist);
#endif
ACMD(do_gotoxy);
#ifdef ENABLE_SAVEPOINT_SYSTEM
ACMD(do_open_savepoint);
ACMD(do_empty_savepoint);
ACMD(do_go_savepoint);
ACMD(do_save_savepoint);
#endif
#ifdef ENABLE_WHISPER_ADMIN_SYSTEM
ACMD(do_open_whispersys);
#endif
#ifdef ENABLE_CHOOSE_DOCTRINE_GUI
ACMD(do_doctrine_choose);
#endif
#ifdef ENABLE_HWID
ACMD(do_blockhwid);
ACMD(do_unblockhwid);
#endif
#ifdef ENABLE_BLOCK_MULTIFARM
ACMD(do_drop_block);
ACMD(do_drop_unblock);
#endif
#ifdef ENABLE_CHANGE_RACE
ACMD(do_change_race);
#endif

struct command_info cmd_info[] =
{
	{ "!RESERVED!",	NULL,			0,			POS_DEAD,	GM_IMPLEMENTOR	}, /* 반드시 이 것이 처음이어야 한다. */
	{ "who",		do_who,			0,			POS_DEAD,	GM_IMPLEMENTOR	},
#ifdef REGEN_ANDRA
	{ "free_regens", do_free_regen,	0,			POS_DEAD,	GM_IMPLEMENTOR	},
#endif
	{ "war",		do_war,			0,			POS_DEAD,	GM_PLAYER	},
	{ "warp",		do_warp,		0,			POS_DEAD,	GM_LOW_WIZARD	},
	{ "user",		do_user,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "notice",		do_notice,		0,			POS_DEAD,	GM_LOW_WIZARD	},
	{ "notice_map",	do_map_notice,	0,			POS_DEAD,	GM_LOW_WIZARD	},
	{ "big_notice",	do_big_notice,	0,			POS_DEAD,	GM_IMPLEMENTOR	},
#ifdef ENABLE_RENEWAL_ADD_STATS
	{	"stat_val",	do_stat_val,	0,			POS_DEAD,	GM_PLAYER	},
#endif
#ifdef ENABLE_FULL_NOTICE
	{ "big_notice_map",	do_map_big_notice,	0,	POS_DEAD,	GM_IMPLEMENTOR	},
	{ "notice_test",	do_notice_test,		0,	POS_DEAD,	GM_IMPLEMENTOR	},
	{ "big_notice_test",do_big_notice_test,	0,	POS_DEAD,	GM_IMPLEMENTOR	},
#endif
	{ "nowar",		do_nowar,		0,			POS_DEAD,	GM_PLAYER	},
	{ "purge",		do_purge,		0,			POS_DEAD,	GM_LOW_WIZARD	},
	{ "weaken",		do_weaken,		0,			POS_DEAD,	GM_IMPLEMENTOR		},
	{ "dc",		do_disconnect,		0,			POS_DEAD,	GM_HIGH_WIZARD	},
	{ "transfer",	do_transfer,		0,			POS_DEAD,	GM_LOW_WIZARD	},
	{ "goto",		do_goto,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "level",		do_level,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "eventflag",	do_event_flag,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "geteventflag",	do_get_event_flag,	0,			POS_DEAD,	GM_IMPLEMENTOR	},

	{ "item",		do_item,		0,			POS_DEAD,	GM_IMPLEMENTOR		},

	{ "mob",		do_mob,			0,			POS_DEAD,	GM_HIGH_WIZARD	},
	{ "mob_ld",		do_mob_ld,			0,			POS_DEAD,	GM_IMPLEMENTOR	}, /* 몹의 위치와 방향을 설정해 소환 /mob_ld vnum x y dir */
	{ "ma",		do_mob_aggresive,	0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "mc",		do_mob_coward,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "mm",		do_mob_map,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "kill",		do_kill,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "ipurge",		do_item_purge,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "group",		do_group,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "grrandom",	do_group_random,	0,			POS_DEAD,	GM_IMPLEMENTOR	},

	{ "set",		do_set,			0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "reset",		do_reset,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "greset",		do_greset,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "advance",	do_advance,		0,			POS_DEAD,	GM_IMPLEMENTOR		},
	{ "book",		do_book,		0,			POS_DEAD,	GM_IMPLEMENTOR  },

	{ "console",	do_console,		0,			POS_DEAD,	GM_IMPLEMENTOR	},

	{ "shutdow",	do_inputall,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "shutdown",	do_shutdown,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	
	{ "stat",		do_stat,		0,			POS_DEAD,	GM_PLAYER	},
	{ "stat-",		do_stat_minus,		0,			POS_DEAD,	GM_PLAYER	},
	{ "stat_reset",	do_stat_reset,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "state",		do_state,		0,			POS_DEAD,	GM_IMPLEMENTOR	},

	// ADD_COMMAND_SLOW_STUN
	{ "stun",		do_stun,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "slow",		do_slow,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	// END_OF_ADD_COMMAND_SLOW_STUN

	{ "respawn",	do_respawn,		0,			POS_DEAD,	GM_IMPLEMENTOR	},

	{ "makeguild",	do_makeguild,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "deleteguild",	do_deleteguild,		0,			POS_DEAD,	GM_IMPLEMENTOR	},

	{ "restart_here",	do_restart,		SCMD_RESTART_HERE,	POS_DEAD,	GM_PLAYER	},
	{ "restart_town",	do_restart,		SCMD_RESTART_TOWN,	POS_DEAD,	GM_PLAYER	},
	{ "phase_selec",	do_inputall,		0,			POS_DEAD,	GM_PLAYER	},
	{ "phase_select",	do_cmd,			SCMD_PHASE_SELECT,	POS_DEAD,	GM_PLAYER	},
	{ "qui",		do_inputall,		0,			POS_DEAD,	GM_PLAYER	},
	{ "quit",		do_cmd,			SCMD_QUIT,		POS_DEAD,	GM_PLAYER	},
	{ "logou",		do_inputall,		0,			POS_DEAD,	GM_PLAYER	},
	{ "logout",		do_cmd,			SCMD_LOGOUT,		POS_DEAD,	GM_PLAYER	},
	{ "skillup",	do_skillup,		0,			POS_DEAD,	GM_PLAYER	},
	{ "gskillup",	do_guildskillup,	0,			POS_DEAD,	GM_PLAYER	},
	{ "pvp",		do_pvp,			0,			POS_DEAD,	GM_PLAYER	},

#ifdef ENABLE_PVP_ADVANCED
	{ "pvp_advanced",	do_pvp_advanced,	0,	POS_DEAD,	GM_PLAYER	},	
	{ "decline_pvp",	do_decline_pvp,		0,	POS_DEAD,	GM_PLAYER	},
	{ "pvp_block_equipment",	do_block_equipment,		0,	POS_DEAD,	GM_PLAYER	},
#endif

	{ "safebox",	do_safebox_size,	0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "safebox_close",	do_safebox_close,	0,			POS_DEAD,	GM_PLAYER	},
	{ "safebox_passwor",do_inputall,		0,			POS_DEAD,	GM_PLAYER	},
	{ "safebox_password",do_safebox_password,	0,			POS_DEAD,	GM_PLAYER	},
	{ "safebox_change_passwor", do_inputall,	0,			POS_DEAD,	GM_PLAYER	},
	{ "safebox_change_password", do_safebox_change_password,	0,	POS_DEAD,	GM_PLAYER	},
	{ "mall_passwor",	do_inputall,		0,			POS_DEAD,	GM_PLAYER	},
	{ "mall_password",	do_mall_password,	0,			POS_DEAD,	GM_PLAYER	},
	{ "mall_close",	do_mall_close,		0,			POS_DEAD,	GM_PLAYER	},

	// Group Command
	{ "ungroup",	do_ungroup,		0,			POS_DEAD,	GM_PLAYER	},

	// REFINE_ROD_HACK_BUG_FIX
	{ "refine_rod",	do_refine_rod,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	// END_OF_REFINE_ROD_HACK_BUG_FIX

	// REFINE_PICK
	{ "refine_pick",	do_refine_pick,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "max_pick",	do_max_pick,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	// END_OF_REFINE_PICK

	{ "fish_simul",	do_fishing_simul,	0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "invisible",	do_invisibility,	0,			POS_DEAD,	GM_LOW_WIZARD	},
	{ "qf",		do_qf,			0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "clear_quest",	do_clear_quest,		0,			POS_DEAD,	GM_IMPLEMENTOR	},

	{ "close_shop",	do_close_shop,		0,			POS_DEAD,	GM_PLAYER	},

	{ "set_walk_mode",	do_set_walk_mode,	0,			POS_DEAD,	GM_PLAYER	},
	{ "set_run_mode",	do_set_run_mode,	0,			POS_DEAD,	GM_PLAYER	},
	{ "setjob",do_set_skill_group,	0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "setskill",	do_setskill,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "setskillother",	do_setskillother,	0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "setskillpoint",  do_set_skill_point,	0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "reload",		do_reload,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "cooltime",	do_cooltime,		0,			POS_DEAD,	GM_IMPLEMENTOR	},

	{ "gwlist",		do_gwlist,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "gwstop",		do_stop_guild_war,	0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "gwcancel",	do_cancel_guild_war, 0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "gstate",		do_guild_state,		0,			POS_DEAD,	GM_IMPLEMENTOR	},

	{ "pkmode",		do_pkmode,		0,			POS_DEAD,	GM_PLAYER	},
	{ "messenger_auth",	do_messenger_auth,	0,			POS_DEAD,	GM_PLAYER	},

#ifdef ENABLE_SORT_INVEN
	{ "item_check",		do_item_check,		0,			POS_DEAD,	GM_PLAYER	},
	{ "sort_extra_inventory",		do_sort_extra_inventory,		0,	POS_DEAD,	GM_PLAYER },
#endif	

	{ "getqf",		do_getqf,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "setqf",		do_setqf,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "delqf",		do_delqf,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "set_state",	do_set_state,		0,			POS_DEAD,	GM_IMPLEMENTOR	},

//	{ "로그를보여줘",	do_detaillog,		0,			POS_DEAD,	GM_IMPLEMENTOR	},//@fixme105
//	{ "몬스터보여줘",	do_monsterlog,		0,			POS_DEAD,	GM_IMPLEMENTOR	},//@fixme105

	{ "detaillog",	do_detaillog,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "monsterlog",	do_monsterlog,		0,			POS_DEAD,	GM_IMPLEMENTOR	},

	{ "forgetme",	do_forgetme,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "aggregate",	do_aggregate,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "attract_ranger",	do_attract_ranger,	0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "pull_monster",	do_pull_monster,	0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "setblockmode",	do_setblockmode,	0,			POS_DEAD,	GM_PLAYER	},
	{ "polymorph",	do_polymorph,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "polyitem",	do_polymorph_item,	0,			POS_DEAD,	GM_IMPLEMENTOR },
	{ "priv_empire",	do_priv_empire,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "priv_guild",	do_priv_guild,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "mount_test",	do_mount_test,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "unmount",	do_unmount,		0,			POS_DEAD,	GM_PLAYER	},
	{ "private",	do_private,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "party_request",	do_party_request,	0,			POS_DEAD,	GM_PLAYER	},
	{ "party_request_accept", do_party_request_accept,0,		POS_DEAD,	GM_PLAYER	},
	{ "party_request_deny", do_party_request_deny,0,			POS_DEAD,	GM_PLAYER	},
	{ "observer",	do_observer,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "observer_exit",	do_observer_exit,	0,			POS_DEAD,	GM_PLAYER	},
	{ "socketitem",	do_socket_item,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "saveati",	do_save_attribute_to_image, 0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "view_equip",	do_view_equip,		0,			POS_DEAD,	GM_PLAYER   	},
	{ "jy",				do_block_chat,		0,			POS_DEAD,	GM_IMPLEMENTOR	},

	// BLOCK_CHAT
	{ "vote_block_chat", do_vote_block_chat,		0,			POS_DEAD,	GM_PLAYER	},
	{ "block_chat",		do_block_chat,		0,			POS_DEAD,	GM_PLAYER	},
	{ "block_chat_list",do_block_chat_list,	0,			POS_DEAD,	GM_PLAYER	},
	// END_OF_BLOCK_CHAT

	{ "build",		do_build,		0,		POS_DEAD,	GM_PLAYER	},
	{ "clear_land", do_clear_land,	0,		POS_DEAD,	GM_IMPLEMENTOR	},

	{ "affect_remove",	do_affect_remove,	0,			POS_DEAD,	GM_IMPLEMENTOR	},

	{ "horse_state",	do_horse_state,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "horse_level",	do_horse_level,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "horse_ride",	do_horse_ride,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "horse_summon",	do_horse_summon,	0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "horse_unsummon",	do_horse_unsummon,	0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "horse_set_stat", do_horse_set_stat,	0,			POS_DEAD,	GM_IMPLEMENTOR	},

	{ "pcbang_update", 	do_pcbang_update,	0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "pcbang_check", 	do_pcbang_check,	0,			POS_DEAD,	GM_IMPLEMENTOR	},

	{ "emotion_allow",	do_emotion_allow,	0,			POS_FIGHTING,	GM_PLAYER	},
	{ "kiss",		do_emotion,		0,			POS_FIGHTING,	GM_PLAYER	},
	{ "slap",		do_emotion,		0,			POS_FIGHTING,	GM_PLAYER	},
	{ "french_kiss",	do_emotion,		0,			POS_FIGHTING,	GM_PLAYER	},
	{ "clap",		do_emotion,		0,			POS_FIGHTING,	GM_PLAYER	},
	{ "cheer1",		do_emotion,		0,			POS_FIGHTING,	GM_PLAYER	},
	{ "cheer2",		do_emotion,		0,			POS_FIGHTING,	GM_PLAYER	},

	// DANCE
	{ "dance1",		do_emotion,		0,			POS_FIGHTING,	GM_PLAYER	},
	{ "dance2",		do_emotion,		0,			POS_FIGHTING,	GM_PLAYER	},
	{ "dance3",		do_emotion,		0,			POS_FIGHTING,	GM_PLAYER	},
	{ "dance4",		do_emotion,		0,			POS_FIGHTING,	GM_PLAYER	},
	{ "dance5",		do_emotion,		0,			POS_FIGHTING,	GM_PLAYER	},
	{ "dance6",		do_emotion,		0,			POS_FIGHTING,	GM_PLAYER	},
	// END_OF_DANCE

	{ "congratulation",	do_emotion,	0,	POS_FIGHTING,	GM_PLAYER	},
	{ "forgive",		do_emotion,	0,	POS_FIGHTING,	GM_PLAYER	},
	{ "angry",		do_emotion,	0,	POS_FIGHTING,	GM_PLAYER	},
	{ "attractive",	do_emotion,	0,	POS_FIGHTING,	GM_PLAYER	},
	{ "sad",		do_emotion,	0,	POS_FIGHTING,	GM_PLAYER	},
	{ "shy",		do_emotion,	0,	POS_FIGHTING,	GM_PLAYER	},
	{ "cheerup",	do_emotion,	0,	POS_FIGHTING,	GM_PLAYER	},
	{ "banter",		do_emotion,	0,	POS_FIGHTING,	GM_PLAYER	},
	{ "joy",		do_emotion,	0,	POS_FIGHTING,	GM_PLAYER	},

	{ "selfie",		do_emotion,	0,	POS_FIGHTING,	GM_PLAYER	},
	{ "dance7",		do_emotion,		0,			POS_FIGHTING,	GM_PLAYER	},
	{ "doze",		do_emotion,		0,			POS_FIGHTING,	GM_PLAYER	},
	{ "exercise",		do_emotion,		0,			POS_FIGHTING,	GM_PLAYER	},
	{ "pushup",		do_emotion,		0,			POS_FIGHTING,	GM_PLAYER	},

	{ "change_attr",	do_change_attr,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "add_attr",	do_add_attr,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "add_socket",	do_add_socket,		0,			POS_DEAD,	GM_IMPLEMENTOR	},

	{ "user_horse_ride",	do_user_horse_ride,		0,		POS_FISHING,	GM_PLAYER	},
	{ "user_horse_back",	do_user_horse_back,		0,		POS_FISHING,	GM_PLAYER	},
	{ "user_horse_feed",	do_user_horse_feed,		0,		POS_FISHING,	GM_PLAYER	},

	{ "show_arena_list",	do_show_arena_list,		0,		POS_DEAD,	GM_IMPLEMENTOR	},
	{ "end_all_duel",		do_end_all_duel,		0,		POS_DEAD,	GM_IMPLEMENTOR	},
	{ "end_duel",			do_end_duel,			0,		POS_DEAD,	GM_IMPLEMENTOR	},
	{ "duel",				do_duel,				0,		POS_DEAD,	GM_IMPLEMENTOR	},

	{ "con+",			do_stat_plus_amount,	POINT_HT,	POS_DEAD,	GM_IMPLEMENTOR	},
	{ "int+",			do_stat_plus_amount,	POINT_IQ,	POS_DEAD,	GM_IMPLEMENTOR	},
	{ "str+",			do_stat_plus_amount,	POINT_ST,	POS_DEAD,	GM_IMPLEMENTOR	},
	{ "dex+",			do_stat_plus_amount,	POINT_DX,	POS_DEAD,	GM_IMPLEMENTOR	},

	{ "break_marriage",	do_break_marriage,		0,			POS_DEAD,	GM_IMPLEMENTOR	},

	{ "show_quiz",			do_oxevent_show_quiz,	0,	POS_DEAD,	GM_IMPLEMENTOR	},
	{ "log_oxevent",		do_oxevent_log,			0,	POS_DEAD,	GM_IMPLEMENTOR	},
	{ "get_oxevent_att",	do_oxevent_get_attender,0,	POS_DEAD,	GM_IMPLEMENTOR	},

	{ "effect",				do_effect,				0,	POS_DEAD,	GM_IMPLEMENTOR	},
#ifdef __ATTR_TRANSFER_SYSTEM__
	{"attrtransfer", do_attr_transfer, 0, POS_DEAD, GM_PLAYER},
#endif
	{ "inventory",			do_inventory,			0,	POS_DEAD,	GM_IMPLEMENTOR	},
	{ "cube",				do_cube,				0,	POS_DEAD,	GM_PLAYER	},
	{ "reset_subskill",		do_reset_subskill,		0,	POS_DEAD,	GM_IMPLEMENTOR },
	{ "flush",				do_flush,				0,	POS_DEAD,	GM_IMPLEMENTOR },
	{ "gift",				do_gift,				0,  POS_DEAD,   GM_PLAYER	},	//gift
	{ "daily_reward_reload",	do_daily_reward_reload,			0,			POS_DEAD,	GM_PLAYER	},
	{ "daily_reward_get_reward",	do_daily_reward_get_reward,			0,			POS_DEAD,	GM_PLAYER	},
	{ "eclipse",			do_eclipse,				0,	POS_DEAD,	GM_IMPLEMENTOR	},

	{ "in_game_mall",		do_in_game_mall,		0,	POS_DEAD,	GM_PLAYER	},

	{ "get_mob_count",		do_get_mob_count,		0,	POS_DEAD,	GM_IMPLEMENTOR	},

	{ "dice",				do_dice,				0,	POS_DEAD,	GM_PLAYER		},
//	{ "주사위",				do_dice,				0,	POS_DEAD,	GM_PLAYER		},//@fixme105
	{ "special_item",			do_special_item,	0,	POS_DEAD,	GM_IMPLEMENTOR		},

	{ "click_mall",			do_click_mall,			0,	POS_DEAD,	GM_PLAYER		},

	{ "ride",				do_ride,				0,	POS_DEAD,	GM_PLAYER	},

	{ "item_id_list",	do_get_item_id_list,	0,	POS_DEAD,	GM_IMPLEMENTOR	},
	{ "set_socket",		do_set_socket,			0,	POS_DEAD,	GM_IMPLEMENTOR	},

	{ "tcon",			do_set_stat,	POINT_HT,	POS_DEAD,	GM_IMPLEMENTOR	},
	{ "tint",			do_set_stat,	POINT_IQ,	POS_DEAD,	GM_IMPLEMENTOR	},
	{ "tstr",			do_set_stat,	POINT_ST,	POS_DEAD,	GM_IMPLEMENTOR	},
	{ "tdex",			do_set_stat,	POINT_DX,	POS_DEAD,	GM_IMPLEMENTOR	},

	{ "cannot_dead",			do_can_dead,	1,	POS_DEAD,		GM_IMPLEMENTOR},
	{ "can_dead",				do_can_dead,	0,	POS_DEAD,		GM_IMPLEMENTOR},

	{ "full_set",	do_full_set, 0, POS_DEAD,		GM_IMPLEMENTOR},
	{ "item_full_set",	do_item_full_set, 0, POS_DEAD,		GM_IMPLEMENTOR},
	{ "attr_full_set",	do_attr_full_set, 0, POS_DEAD,		GM_IMPLEMENTOR},
	{ "all_skill_master",	do_all_skill_master,	0,	POS_DEAD,	GM_IMPLEMENTOR},
	{ "use_item",		do_use_item,	0, POS_DEAD,		GM_IMPLEMENTOR},

	{ "dragon_soul",				do_dragon_soul,				0,	POS_DEAD,	GM_PLAYER	},
	{ "do_clear_affect", do_clear_affect, 	0, POS_DEAD,		GM_IMPLEMENTOR},
#ifdef __NEWPET_SYSTEM__
	{ "cubepetadd", 		do_CubePetAdd,	0,			POS_DEAD,	GM_PLAYER },
	{ "feedcubepet", 		do_FeedCubePet,	0,			POS_DEAD,	GM_PLAYER },
	{ "petskills", 			do_PetSkill,	0,			POS_DEAD,	GM_PLAYER },
	{ "petvoincrease",			do_PetEvo,	0,			POS_DEAD,	GM_PLAYER },
#ifdef ENABLE_NEW_PET_EDITS
	{"petincskill", do_PetIncreaseSkill, 0, POS_DEAD, GM_PLAYER},
#endif
#endif
#ifdef ENABLE_NEWSTUFF
	//item
	{ "add_rare_attr",		do_add_rare_attr,			0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "change_rare_attr",	do_change_rare_attr,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	//player
	{ "click_safebox",		do_click_safebox,			0,			POS_DEAD,	GM_PLAYER	},
	{ "force_logout",		do_force_logout,			0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "poison",				do_poison,					0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "rewarp",				do_rewarp,					0,			POS_DEAD,	GM_IMPLEMENTOR	},
#endif
#ifdef __ENABLE_NEW_OFFLINESHOP__
	{ "offshop_change_shop_name", do_offshop_change_shop_name, 0,  POS_DEAD, GM_IMPLEMENTOR },
	{ "offshop_force_close_shop", do_offshop_force_close_shop, 0,  POS_DEAD, GM_IMPLEMENTOR },
#endif
#ifdef ENABLE_WOLFMAN_CHARACTER
	{ "bleeding",			do_bleeding,				0,			POS_DEAD,	GM_IMPLEMENTOR	},
#endif
#ifdef ENABLE_GAYA_SYSTEM
	{ "w_gaya",							do_gaya_system,					0,	POS_DEAD,	GM_PLAYER },
#endif

#ifdef __ENABLE_RANGE_ALCHEMY__
	{ "open_range_npc",		do_extend_range_npc,			0,			POS_DEAD,	GM_PLAYER		},
#endif

#ifdef __ENABLE_REFINE_ALCHEMY__
	{ "open_refine_window_alchemy",	do_refine_window_alchemy,	0,		POS_DEAD,	GM_PLAYER		},
#endif

#ifdef ENABLE_CHANNEL_SWITCH_SYSTEM
	{"channel", 					do_change_channel, 			0, 		POS_DEAD, 	GM_PLAYER		},
#endif


#ifdef __HIDE_COSTUME_SYSTEM__
	{ "hide_costume", do_hide_costume, 0, POS_DEAD, GM_PLAYER },
#endif
#ifdef ENABLE_ATTR_COSTUMES
	{"attrdialog_remove", do_attrdialog_remove, 0, POS_DEAD, GM_PLAYER},
#endif
#ifdef ENABLE_RANKING
	{"ranking_subcategory", do_ranking_subcategory, 0, POS_DEAD, GM_PLAYER},
#endif
	{"manage_exp", do_manage_exp, 0, POS_DEAD, GM_PLAYER},
#ifdef ENABLE_LOCKED_EXTRA_INVENTORY
	{"unlock_extra", do_unlock_extra, 0, POS_DEAD, GM_PLAYER},
#endif
#ifdef ENABLE_NEW_PET_EDITS
	{"petenchant", do_petenchant, 0, POS_DEAD, GM_PLAYER},
#endif
#ifdef ENABLE_BLOCK_MULTIFARM
	{"drop_block", do_drop_block, 0, POS_DEAD, GM_PLAYER},
	{"drop_unblock", do_drop_unblock, 0, POS_DEAD, GM_PLAYER},
#endif
#ifdef ENABLE_RUNE_SYSTEM
	{"rune", do_rune, 0, POS_DEAD, GM_PLAYER},
	{"rune_charge", do_rune_charge, 0, POS_DEAD, GM_PLAYER},
	{"rune_shop", do_rune_shop, 0, POS_DEAD, GM_PLAYER},
	{"rune_effect", do_rune_effect, 0, POS_DEAD, GM_PLAYER},
#endif
	{"remove_affect", do_remove_affect, 0, POS_DEAD, GM_PLAYER},
	// {"stat2", do_stat2, 0, POS_DEAD, GM_PLAYER},
#ifdef ENABLE_BIOLOGIST_UI
	{"open_biologist", do_open_biologist, 0, POS_DEAD, GM_PLAYER},
	{"delivery_biologist", do_delivery_biologist, 0, POS_DEAD, GM_PLAYER},
	{"reward_biologist", do_reward_biologist, 0, POS_DEAD, GM_PLAYER},
	{"change_biologist", do_change_biologist, 0, POS_DEAD, GM_PLAYER},
#endif
	{"gotoxy", do_gotoxy, 0, POS_DEAD, GM_PLAYER},
#ifdef ENABLE_SAVEPOINT_SYSTEM
	{"open_savepoint", do_open_savepoint, 0, POS_DEAD, GM_PLAYER},
	{"empty_savepoint", do_empty_savepoint, 0, POS_DEAD, GM_PLAYER},
	{"go_savepoint", do_go_savepoint, 0, POS_DEAD, GM_PLAYER},
	{"save_savepoint", do_save_savepoint, 0, POS_DEAD, GM_PLAYER},
#endif
#ifdef ENABLE_WHISPER_ADMIN_SYSTEM
	{"open_whispersys", do_open_whispersys, 0, POS_DEAD, GM_IMPLEMENTOR},
#endif
#ifdef ENABLE_CHOOSE_DOCTRINE_GUI
	{"doctrine_choose", do_doctrine_choose, 0, POS_DEAD, GM_PLAYER},
#endif
#ifdef ENABLE_HWID
	{"blockhwid", do_blockhwid, 0, POS_DEAD, GM_HIGH_WIZARD},
	{"unblockhwid", do_unblockhwid, 0, POS_DEAD, GM_IMPLEMENTOR},
#endif
#ifdef ENABLE_CHANGE_RACE
	{ "change_race",	do_change_race,	0,	POS_DEAD,	GM_HIGH_WIZARD },
#endif
#ifdef ENABLE_EVENT_MANAGER
	{ "event_manager",	do_event_manager,		0,		POS_DEAD,	GM_PLAYER },
#endif
	{"\n", NULL, 0, POS_DEAD, GM_IMPLEMENTOR}
};

void interpreter_set_privilege(const char *cmd, int lvl)
{
	int i;

	for (i = 0; *cmd_info[i].command != '\n'; ++i)
	{
		if (!str_cmp(cmd, cmd_info[i].command))
		{
			cmd_info[i].gm_level = lvl;
			sys_log(0, "Setting command privilege: %s -> %d", cmd, lvl);
			break;
		}
	}
}

void double_dollar(const char *src, size_t src_len, char *dest, size_t dest_len)
{
	const char * tmp = src;
	size_t cur_len = 0;

	// \0 넣을 자리 확보
	dest_len -= 1;

	while (src_len-- && *tmp)
	{
		if (*tmp == '$')
		{
			if (cur_len + 1 >= dest_len)
				break;

			*(dest++) = '$';
			*(dest++) = *(tmp++);
			cur_len += 2;
		}
		else
		{
			if (cur_len >= dest_len)
				break;

			*(dest++) = *(tmp++);
			cur_len += 1;
		}
	}

	*dest = '\0';
}
// #define ENABLE_BLOCK_CMD_SHORTCUT
void interpret_command(LPCHARACTER ch, const char * argument, size_t len)
{
#ifdef ENABLE_ANTI_CMD_FLOOD
	if (ch && !ch->IsGM())
	{
		if (thecore_pulse() > ch->GetCmdAntiFloodPulse() + PASSES_PER_SEC(1))
		{
			ch->SetCmdAntiFloodCount(0);
			ch->SetCmdAntiFloodPulse(thecore_pulse());
		}
		if (ch->IncreaseCmdAntiFloodCount()>=50)
		{
			ch->GetDesc()->DelayedDisconnect(0);
			return;
		}
	}
#endif
	if (NULL == ch)
	{
		sys_err ("NULL CHRACTER");
		return ;
	}

	char cmd[128 + 1];  // buffer overflow 문제가 생기지 않도록 일부러 길이를 짧게 잡음
	char new_line[256 + 1];
	const char * line;
	int icmd;

	if (len == 0 || !*argument)
		return;

	double_dollar(argument, len, new_line, sizeof(new_line));

	size_t cmdlen;
	line = first_cmd(new_line, cmd, sizeof(cmd), &cmdlen);

	for (icmd = 1; *cmd_info[icmd].command != '\n'; ++icmd)
	{
		if (cmd_info[icmd].command_pointer == do_cmd)
		{
			if (!strcmp(cmd_info[icmd].command, cmd)) // do_cmd는 모든 명령어를 쳐야 할 수 있다.
				break;
		}
#ifdef ENABLE_BLOCK_CMD_SHORTCUT
		else if (!strcmp(cmd_info[icmd].command, cmd))
#else
		else if (!strncmp(cmd_info[icmd].command, cmd, cmdlen))
#endif
			break;
	}

	if (ch->GetPosition() < cmd_info[icmd].minimum_position)
	{
		switch (ch->GetPosition())
		{
			case POS_MOUNTING:
				break;
			case POS_DEAD:
				break;
			case POS_SLEEPING:
				break;
			case POS_RESTING:
			case POS_SITTING:
				break;
				/*
				   case POS_FIGHTING:
				   break;
				 */
			default:
				sys_err("unknown position %d", ch->GetPosition());
				break;
		}

		return;
	}

	if (*cmd_info[icmd].command == '\n')
	{
#ifdef TEXTS_IMPROVEMENT
		ch->ChatPacketNew(CHAT_TYPE_INFO, 266, "");
#endif
		return;
	}

	if (cmd_info[icmd].gm_level && (cmd_info[icmd].gm_level > ch->GetGMLevel() || cmd_info[icmd].gm_level == GM_DISABLE))
	{
#ifdef TEXTS_IMPROVEMENT
		ch->ChatPacketNew(CHAT_TYPE_INFO, 266, "");
#endif
		return;
	}

	if (strncmp("phase", cmd_info[icmd].command, 5) != 0) // 히든 명령어 처리
		sys_log(0, "COMMAND: %s: %s", ch->GetName(), cmd_info[icmd].command);

	((*cmd_info[icmd].command_pointer) (ch, line, icmd, cmd_info[icmd].subcmd));

	if (ch->GetGMLevel() >= GM_IMPLEMENTOR)
	{
		if (cmd_info[icmd].gm_level >= GM_IMPLEMENTOR)
		{
			char buf[1024];
			snprintf( buf, sizeof(buf), "%s", argument );

			LogManager::instance().GMCommandLog(ch->GetPlayerID(), ch->GetName(), ch->GetDesc()->GetHostName(), g_bChannel, buf);
		}
	}
}

