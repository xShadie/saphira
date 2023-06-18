#include "stdafx.h"

#include "../../common/teen_packet.h"
#include "../../common/VnumHelper.h"

#include "char.h"

#include "config.h"
#include "utils.h"
#include "crc32.h"
#include "char_manager.h"
#include "desc_client.h"
#include "desc_manager.h"
#include "buffer_manager.h"
#include "item_manager.h"
#include "motion.h"
#include "vector.h"
#include "packet.h"
#include "cmd.h"
#include "fishing.h"
#include "exchange.h"
#include "battle.h"
#include "affect.h"
#include "shop.h"
#include "shop_manager.h"
#include "safebox.h"
#include "regen.h"
#include "pvp.h"
#include "party.h"
#include "start_position.h"
#include "questmanager.h"
#include "log.h"
#include "p2p.h"
#include "guild.h"
#include "guild_manager.h"
#include "dungeon.h"
#include "messenger_manager.h"
#include "unique_item.h"
#include "priv_manager.h"
#include "war_map.h"
#include "banword.h"
#include "target.h"
#include "wedding.h"
#include "mob_manager.h"
#include "mining.h"
#include "arena.h"
#include "dev_log.h"
#include "horsename_manager.h"
#include "pcbang.h"
#include "gm.h"
#include "map_location.h"
#include "skill_power.h"
#include "buff_on_attributes.h"
#ifdef __ENABLE_NEW_OFFLINESHOP__
#include "new_offlineshop.h"
#include "new_offlineshop_manager.h"
#endif

#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
#include "MountSystem.h"
#endif

#ifdef ENABLE_BATTLE_PASS
#include "battle_pass.h"
#endif

#ifdef __PET_SYSTEM__
#include "PetSystem.h"
#endif
#ifdef __NEWPET_SYSTEM__
#include "New_PetSystem.h"
#endif
#include "DragonSoul.h"
#include "../../common/CommonDefines.h"
#ifdef __SEND_TARGET_INFO__
#include <algorithm>
#include <iterator>
#endif
#ifdef ENABLE_SWITCHBOT
#include "new_switchbot.h"
#endif
#ifdef ENABLE_RUNE_SYSTEM
#include "../../common/rune_length.h"
#endif
#ifdef ENABLE_STOLE_COSTUME
#include "../../common/stole_length.h"
#endif
using namespace std;

extern const BYTE g_aBuffOnAttrPoints;
extern bool RaceToJob(unsigned race, unsigned *ret_job);

extern bool IS_SUMMONABLE_ZONE(int map_index); // char_item.cpp
bool CAN_ENTER_ZONE(const LPCHARACTER& ch, int map_index);

bool CAN_ENTER_ZONE(const LPCHARACTER& ch, int map_index)
{
	switch (map_index)
	{
	case 301:
	case 302:
	case 303:
	case 304:
		if (ch->GetLevel() < 90)
			return false;
	}
	return true;
}

// <Factor> DynamicCharacterPtr member function definitions

LPCHARACTER DynamicCharacterPtr::Get() const {
	LPCHARACTER p = NULL;
	if (is_pc) {
		p = CHARACTER_MANAGER::instance().FindByPID(id);
	} else {
		p = CHARACTER_MANAGER::instance().Find(id);
	}
	return p;
}

DynamicCharacterPtr& DynamicCharacterPtr::operator=(LPCHARACTER character) {
	if (character == NULL) {
		Reset();
		return *this;
	}
	if (character->IsPC()) {
		is_pc = true;
		id = character->GetPlayerID();
	} else {
		is_pc = false;
		id = character->GetVID();
	}
	return *this;
}

CHARACTER::CHARACTER()
{
	m_stateIdle.Set(this, &CHARACTER::BeginStateEmpty, &CHARACTER::StateIdle, &CHARACTER::EndStateEmpty);
	m_stateMove.Set(this, &CHARACTER::BeginStateEmpty, &CHARACTER::StateMove, &CHARACTER::EndStateEmpty);
	m_stateBattle.Set(this, &CHARACTER::BeginStateEmpty, &CHARACTER::StateBattle, &CHARACTER::EndStateEmpty);

	Initialize();
}

CHARACTER::~CHARACTER()
{
	Destroy();
}

void CHARACTER::Initialize()
{
	CEntity::Initialize(ENTITY_CHARACTER);

	m_bNoOpenedShop = true;

	m_bOpeningSafebox = false;

	m_fSyncTime = get_float_time()-3;
	m_dwPlayerID = 0;
#ifdef __NEWPET_SYSTEM__
	m_stImmortalSt = 0;
	m_newpetskillcd[0] = 0;
	m_newpetskillcd[1] = 0;
	m_newpetskillcd[2] = 0;
	m_newpetskillcd[3] = 0;
#endif	
	m_dwKillerPID = 0;
#ifdef __SEND_TARGET_INFO__
	dwLastTargetInfoPulse = 0;
#endif
	m_iMoveCount = 0;

	m_pkRegen = NULL;
	regen_id_ = 0;
	m_posRegen.x = m_posRegen.y = m_posRegen.z = 0;
	m_posStart.x = m_posStart.y = 0;
	m_posDest.x = m_posDest.y = 0;
	m_fRegenAngle = 0.0f;

	m_pkMobData		= NULL;
	m_pkMobInst		= NULL;

	m_pkShop		= NULL;
	m_pkChrShopOwner	= NULL;
	m_pkMyShop		= NULL;
	m_pkExchange	= NULL;
	m_pkParty		= NULL;
	m_pkPartyRequestEvent = NULL;

	m_pGuild = NULL;

	m_pkChrTarget = NULL;

	m_pkMuyeongEvent = NULL;
#ifdef ENABLE_NEW_GYEONGGONG_SKILL	
	m_pkGyeongGongEvent = NULL;
#endif
	m_pkWarpNPCEvent = NULL;
	m_pkDeadEvent = NULL;
	m_pkStunEvent = NULL;
	m_pkSaveEvent = NULL;
	m_pkRecoveryEvent = NULL;
	m_pkTimedEvent = NULL;
	m_pkFishingEvent = NULL;
	m_pkWarpEvent = NULL;

	// MINING
	m_pkMiningEvent = NULL;
	// END_OF_MINING

	m_pkPoisonEvent = NULL;
#ifdef ENABLE_WOLFMAN_CHARACTER
	m_pkBleedingEvent = NULL;
#endif
	m_pkFireEvent = NULL;
	m_pkAffectEvent = NULL;
	m_afAffectFlag = TAffectFlag(0, 0);

	m_pkDestroyWhenIdleEvent = NULL;

	m_pkChrSyncOwner = NULL;

	memset(&m_points, 0, sizeof(m_points));
	memset(&m_pointsInstant, 0, sizeof(m_pointsInstant));
	memset(&m_quickslot, 0, sizeof(m_quickslot));

	m_bCharType = CHAR_TYPE_MONSTER;

	SetPosition(POS_STANDING);

	m_dwPlayStartTime = m_dwLastMoveTime = get_dword_time();

	GotoState(m_stateIdle);
	m_dwStateDuration = 1;

	m_dwLastAttackTime = get_dword_time() - 20000;

	m_bAddChrState = 0;
#if defined(BL_OFFLINE_MESSAGE)
	dwLastOfflinePMTime = 0;
#endif
	m_pkChrStone = NULL;

	m_pkSafebox = NULL;
	m_iSafeboxSize = -1;
	m_iSafeboxLoadTime = 0;

	m_pkMall = NULL;
	m_iMallLoadTime = 0;

	m_posWarp.x = m_posWarp.y = m_posWarp.z = 0;
	m_lWarpMapIndex = 0;

	m_posExit.x = m_posExit.y = m_posExit.z = 0;
	m_lExitMapIndex = 0;

	m_pSkillLevels = NULL;

	m_dwMoveStartTime = 0;
	m_dwMoveDuration = 0;

	m_dwFlyTargetID = 0;

	m_dwNextStatePulse = 0;

	m_dwLastDeadTime = get_dword_time()-180000;

	m_bSkipSave = false;

	m_bItemLoaded = false;

	m_bHasPoisoned = false;
#ifdef ENABLE_WOLFMAN_CHARACTER
	m_bHasBled = false;
#endif
	m_pkDungeon = NULL;
	m_iEventAttr = 0;

	m_kAttackLog.dwVID = 0;
	m_kAttackLog.dwTime = 0;

	m_bNowWalking = m_bWalking = false;
	ResetChangeAttackPositionTime();

	m_bDetailLog = false;
	m_bMonsterLog = false;

	m_bDisableCooltime = false;

	m_iAlignment = 0;
	m_iRealAlignment = 0;

	m_iKillerModePulse = 0;
	m_bPKMode = PK_MODE_PEACE;

	m_dwQuestNPCVID = 0;
	m_dwQuestByVnum = 0;
	m_pQuestItem = NULL;

	m_szMobileAuth[0] = '\0';

	m_dwUnderGuildWarInfoMessageTime = get_dword_time()-60000;

	m_bUnderRefine = false;

	// REFINE_NPC
	m_dwRefineNPCVID = 0;
	// END_OF_REFINE_NPC

	m_dwPolymorphRace = 0;

	m_bStaminaConsume = false;

	ResetChainLightningIndex();

	m_dwMountVnum = 0;
	m_chHorse = NULL;
	m_chRider = NULL;

	m_pWarMap = NULL;
	m_pWeddingMap = NULL;
	m_bChatCounter = 0;

#ifdef __ENABLE_NEW_OFFLINESHOP__
	m_pkOfflineShop = NULL;
	m_pkShopSafebox	= NULL;
	m_pkAuction		= NULL;
	m_pkAuctionGuest= NULL;
	//offlineshop-updated 03/08/19
	m_pkOfflineShopGuest = NULL;

	//offlineshop-updated 05/08/19
	m_bIsLookingOfflineshopOfferList = false;
#endif


	ResetStopTime();
#ifdef ENABLE_GAYA_SYSTEM
	LOAD_GAYA();
#endif
	m_dwLastVictimSetTime = get_dword_time() - 3000;
	m_iMaxAggro = -100;

	m_bSendHorseLevel = 0;
	m_bSendHorseHealthGrade = 0;
	m_bSendHorseStaminaGrade = 0;

	m_dwLoginPlayTime = 0;

	m_pkChrMarried = NULL;

	m_posSafeboxOpen.x = -1000;
	m_posSafeboxOpen.y = -1000;

	// EQUIP_LAST_SKILL_DELAY
	m_dwLastSkillTime = get_dword_time();
	// END_OF_EQUIP_LAST_SKILL_DELAY

	// MOB_SKILL_COOLTIME
	memset(m_adwMobSkillCooltime, 0, sizeof(m_adwMobSkillCooltime));
	// END_OF_MOB_SKILL_COOLTIME

	m_isinPCBang = false;

	// ARENA
	m_pArena = NULL;
	m_nPotionLimit = quest::CQuestManager::instance().GetEventFlag("arena_potion_limit_count");
	// END_ARENA

	//PREVENT_TRADE_WINDOW
	m_isOpenSafebox = 0;
	//END_PREVENT_TRADE_WINDOW

	//PREVENT_REFINE_HACK
	m_iRefineTime = 0;
	//END_PREVENT_REFINE_HACK

	//RESTRICT_USE_SEED_OR_MOONBOTTLE
	m_iSeedTime = 0;
	//END_RESTRICT_USE_SEED_OR_MOONBOTTLE
	//PREVENT_PORTAL_AFTER_EXCHANGE
	m_iExchangeTime = 0;
	//END_PREVENT_PORTAL_AFTER_EXCHANGE
	//
	m_iMyShopTime = 0;

	m_deposit_pulse = 0;

	SET_OVER_TIME(this, OT_NONE);

	m_strNewName = "";

	m_known_guild.clear();

	m_dwLogOffInterval = 0;

	m_bComboSequence = 0;
	m_dwLastComboTime = 0;
	m_bComboIndex = 0;
	m_iComboHackCount = 0;
	m_dwSkipComboAttackByTime = 0;

	m_dwMountTime = 0;

	m_dwLastGoldDropTime = 0;
#ifdef ENABLE_NEWSTUFF
	m_dwLastBoxUseTime = 0;
	m_dwLastBuySellTime = 0;
#endif

	m_bIsLoadedAffect = false;
	cannot_dead = false;

#ifdef __PET_SYSTEM__
	m_petSystem = 0;
	m_bIsPet = false;
#endif

#ifdef __NEWPET_SYSTEM__
	m_newpetSystem = 0;
	m_bIsNewPet = false;
	m_eggvid = 0;
#endif
	m_fAttMul = 1.0f;
	m_fDamMul = 1.0f;

	m_pointsInstant.iDragonSoulActiveDeck = -1;

#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
	m_mountSystem = 0;
	m_bIsMount = false;
#endif

#ifdef ENABLE_ANTI_CMD_FLOOD
	m_dwCmdAntiFloodCount = 0;
	m_dwCmdAntiFloodPulse = 0;
#endif
	memset(&m_tvLastSyncTime, 0, sizeof(m_tvLastSyncTime));
	m_iSyncHackCount = 0;
#ifdef ENABLE_NEW_FISHING_SYSTEM
	m_pkFishingNewEvent = NULL;
	m_bFishCatch = 0;
	m_dwLastCatch = 0;
	m_dwCatchFailed = 0;
#endif
#ifdef ENABLE_RANKING
	for (int i = 0; i < RANKING_MAX_CATEGORIES; ++i)
		m_lRankPoints[i] = 0;
#endif
	
#ifdef ENABLE_ATTR_COSTUMES
	attrdialog_remove = 0;
#endif
#ifdef ENABLE_BATTLE_PASS
	m_listBattlePass.clear();
	m_bIsLoadedBattlePass = false;
	
	m_dwBattlePassEndTime = 0;
	
#ifdef ENABLE_BATTLE_PASS_STAY_ONLINE
	m_pkStayOnlineEvent = NULL;
#endif

#endif
	m_stName = "";


#ifdef __SKILL_COLOR_SYSTEM__
	memset(&m_dwSkillColor, 0, sizeof(m_dwSkillColor));
#endif
#ifdef ENABLE_ACCE_SYSTEM
	m_bAcceCombination	= false;
	m_bAcceAbsorption	= false;
#endif


#ifdef __HIDE_COSTUME_SYSTEM__
	m_bHideBodyCostume = false;
	m_bHideHairCostume = false;
#ifdef ENABLE_ACCE_SYSTEM
	m_bHideAcceCostume = false;
#endif
	m_bHideWeaponCostume = false;
#endif
#ifdef ENABLE_NEW_PET_EDITS
	petenchant = 0;
#endif
#ifdef KASMIR_PAKET_SYSTEM
	m_bKasmirPaketBaslik = 0;
	m_bKasmirPaketDurum = false;
#endif
	isInvincible = false;
	m_iGoToXYTime = 0;
#ifdef ENABLE_SAVEPOINT_SYSTEM
	m_iSavePointTime = 0;
#endif
#ifdef ENABLE_SORT_INVEN
	m_iSortInv1Time = 0;
	m_iSortInv2Time = 0;
#endif
#ifdef ENABLE_LIMIT_BUY_SPEED
	m_iLastBuyTime = 0;
#endif
#ifdef __DUNGEON_INFO_SYSTEM__
	dungeonDamage.clear();
#endif
#ifdef ENABLE_SPAM_CHECK
	m_iLastUnlock = 0;
	m_iLastDSRefine = 0;
#endif
#ifdef ENABLE_ANTICHEAT
	m_firstReward = 0;
	m_rewardCount = 0;
	m_checkRepeated = 0;
	m_dropitemcount = 0;
	m_lastdropitem = 0;
#endif
#ifdef ENABLE_BLOCK_MULTIFARM
	m_pkDropEvent = NULL;
#endif
}

void CHARACTER::Create(const char * c_pszName, DWORD vid, bool isPC)
{
	static int s_crc = 172814;

	char crc_string[128+1];
	snprintf(crc_string, sizeof(crc_string), "%s%p%d", c_pszName, this, ++s_crc);
	m_vid = VID(vid, GetCRC32(crc_string, strlen(crc_string)));
	if (isPC)
		m_stName = c_pszName;
}

void CHARACTER::Destroy()
{
	CloseMyShop();
#ifdef REGEN_ANDRA
	if (m_pkRegen)
	{
		if (m_pkDungeon) {
			// Dungeon regen may not be valid at this point
			if (m_pkDungeon->IsValidRegen(m_pkRegen, regen_id_)) {
				--m_pkRegen->count;
			}
		} else {
			// Is this really safe? //turkmmo///
			if (is_valid_regen(m_pkRegen))
				--m_pkRegen->count;
		}
		m_pkRegen->nextRespawn = m_pkRegen->time ? time(0) + m_pkRegen->time : 0;
		m_pkRegen = NULL;
	}

#else
	if (m_pkRegen)
	{
		if (m_pkDungeon) {
			// Dungeon regen may not be valid at this point
			if (m_pkDungeon->IsValidRegen(m_pkRegen, regen_id_)) {
				--m_pkRegen->count;
			}
		} else {
			// Is this really safe?
			--m_pkRegen->count;
		}
		m_pkRegen = NULL;
	}
#endif

	if (m_pkDungeon)
	{
		SetDungeon(NULL);
	}

#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
	if (m_mountSystem)
	{
		m_mountSystem->Destroy();
		delete m_mountSystem;

		m_mountSystem = 0;
	}
	
	if(GetMountVnum())
	{
		RemoveAffect(AFFECT_MOUNT);
		RemoveAffect(AFFECT_MOUNT_BONUS);
	}
	HorseSummon(false);
#endif
#ifdef __PET_SYSTEM__
	if (m_petSystem)
	{
		m_petSystem->Destroy();
		delete m_petSystem;

		m_petSystem = 0;
	}
#endif

#ifdef __NEWPET_SYSTEM__
	if (m_newpetSystem)
	{
		m_newpetSystem->Destroy();
		delete m_newpetSystem;

		m_newpetSystem = 0;
	}
#endif

	HorseSummon(false);

	if (GetRider())
		GetRider()->ClearHorseInfo();

	if (GetDesc())
	{
		GetDesc()->BindCharacter(NULL);
//		BindDesc(NULL);
	}

	if (m_pkExchange)
		m_pkExchange->Cancel();

	SetVictim(NULL);

	if (GetShop())
	{
		GetShop()->RemoveGuest(this);
		SetShop(NULL);
	}

	ClearStone();
	ClearSync();
	ClearTarget();

	if (NULL == m_pkMobData)
	{
		DragonSoul_CleanUp();
		ClearItem();
	}

	// <Factor> m_pkParty becomes NULL after CParty destructor call!
	LPPARTY party = m_pkParty;
	if (party)
	{
		if (party->GetLeaderPID() == GetVID() && !IsPC())
		{
			M2_DELETE(party);
		}
		else
		{
			party->Unlink(this);

			if (!IsPC())
				party->Quit(GetVID());
		}

		SetParty(NULL); // ���ص� ������ �����ϰ�.
	}

	if (m_pkMobInst)
	{
		M2_DELETE(m_pkMobInst);
		m_pkMobInst = NULL;
	}

	m_pkMobData = NULL;

	if (m_pkSafebox)
	{
		M2_DELETE(m_pkSafebox);
		m_pkSafebox = NULL;
	}

	if (m_pkMall)
	{
		M2_DELETE(m_pkMall);
		m_pkMall = NULL;
	}

	for (TMapBuffOnAttrs::iterator it = m_map_buff_on_attrs.begin();  it != m_map_buff_on_attrs.end(); it++)
	{
		if (NULL != it->second)
		{
			M2_DELETE(it->second);
		}
	}
	m_map_buff_on_attrs.clear();

	m_set_pkChrSpawnedBy.clear();

	StopMuyeongEvent();
#ifdef ENABLE_NEW_GYEONGGONG_SKILL
	StopGyeongGongEvent();
#endif
	event_cancel(&m_pkWarpNPCEvent);
	event_cancel(&m_pkRecoveryEvent);
	event_cancel(&m_pkDeadEvent);
	event_cancel(&m_pkSaveEvent);
	event_cancel(&m_pkTimedEvent);
	event_cancel(&m_pkStunEvent);
	event_cancel(&m_pkFishingEvent);
	event_cancel(&m_pkPoisonEvent);
#ifdef ENABLE_WOLFMAN_CHARACTER
	event_cancel(&m_pkBleedingEvent);
#endif
	event_cancel(&m_pkFireEvent);
	event_cancel(&m_pkPartyRequestEvent);
	//DELAYED_WARP
	event_cancel(&m_pkWarpEvent);
	//END_DELAYED_WARP
#ifdef ENABLE_NEW_FISHING_SYSTEM
	event_cancel(&m_pkFishingNewEvent);
#endif
	// RECALL_DELAY
	//event_cancel(&m_pkRecallEvent);
	// END_OF_RECALL_DELAY

	// MINING
	event_cancel(&m_pkMiningEvent);
	// END_OF_MINING
#ifdef ENABLE_BLOCK_MULTIFARM
	if (m_pkDropEvent) {
		event_cancel(&m_pkDropEvent);
		m_pkDropEvent = NULL;
	}
#endif
#ifdef ENABLE_BATTLE_PASS_STAY_ONLINE
	event_cancel(&m_pkStayOnlineEvent);
#endif

	for (itertype(m_mapMobSkillEvent) it = m_mapMobSkillEvent.begin(); it != m_mapMobSkillEvent.end(); ++it)
	{
		LPEVENT pkEvent = it->second;
		event_cancel(&pkEvent);
	}
	m_mapMobSkillEvent.clear();
#ifdef __DUNGEON_INFO_SYSTEM__
	dungeonDamage.clear();
#endif
	//event_cancel(&m_pkAffectEvent);
	ClearAffect();

	event_cancel(&m_pkDestroyWhenIdleEvent);

	if (m_pSkillLevels)
	{
		M2_DELETE_ARRAY(m_pSkillLevels);
		m_pSkillLevels = NULL;
	}

	CEntity::Destroy();

	if (GetSectree())
		GetSectree()->RemoveEntity(this);

	if (m_bMonsterLog)
		CHARACTER_MANAGER::instance().UnregisterForMonsterLog(this);
}

#ifdef ENABLE_MULTI_LANGUAGE
const char * CHARACTER::GetName(BYTE lang) const
{
	return m_stName.empty() ? (m_pkMobData ? m_pkMobData->m_table.szLocaleName[lang] : "") : m_stName.c_str();
}
#else
const char * CHARACTER::GetName() const
{
	return m_stName.empty() ? (m_pkMobData ? m_pkMobData->m_table.szLocaleName : "") : m_stName.c_str();
}
#endif

#ifdef ENABLE_PVP_ADVANCED
void CHARACTER::DestroyPvP()
{
	if (GetDesc() != NULL)
	{
		const char* szTableStaticPvP[] = {BLOCK_CHANGEITEM, BLOCK_BUFF, BLOCK_POTION, BLOCK_RIDE, BLOCK_PET, BLOCK_POLY, BLOCK_PARTY, BLOCK_EXCHANGE_, BET_WINNER, CHECK_IS_FIGHT};
		
		int moneyBet = GetQuestFlag(szTableStaticPvP[8]);
		int isDuel = GetQuestFlag(szTableStaticPvP[9]);
			
		if (isDuel != 0)
		{
			if (moneyBet > 0)
			{
				PointChange(POINT_GOLD, moneyBet, true);	
			}
			
			char szBuf[CHAT_MAX_LEN + 1];
			snprintf(szBuf, sizeof(szBuf), "BINARY_Duel_Delete");
			ChatPacket(CHAT_TYPE_COMMAND, szBuf);	
		
			for (size_t i = 0; i < _countof(szTableStaticPvP); i++)
			{
				SetQuestFlag(szTableStaticPvP[i], 0);	//codice di merda indovina... ... il ciclo for e sprecato qui o sbaglio?
			}
		}
	}
}
#endif

void CHARACTER::OpenMyShop(const char * c_pszSign, TShopItemTable * pTable, BYTE bItemCount
#ifdef KASMIR_PAKET_SYSTEM
, DWORD KasmirNpc, BYTE KasmirBaslik
#endif
)
{
	if (!CanHandleItem())
	{
#ifdef TEXTS_IMPROVEMENT
		ChatPacketNew(CHAT_TYPE_INFO, 291, "");
#endif
		return;
	}

#ifdef ENABLE_RESTRICT_GM_PERMISSIONS
	if (GetGMLevel() > GM_PLAYER && GetGMLevel() < GM_IMPLEMENTOR) {
		return;
	}
#endif

#ifndef ENABLE_OPEN_SHOP_WITH_ARMOR
	if (GetPart(PART_MAIN) > 2)
	{
#ifdef TEXTS_IMPROVEMENT
		ChatPacketNew(CHAT_TYPE_INFO, 503, "");
#endif
		return;
	}
#endif

	if (GetMyShop())	// �̹� ���� ���� ������ �ݴ´�.
	{
		CloseMyShop();
		return;
	}

	// �������� ����Ʈ�� ������ ������ �� �� ����.
	quest::PC * pPC = quest::CQuestManager::instance().GetPCForce(GetPlayerID());

	// GetPCForce�� NULL�� �� �����Ƿ� ���� Ȯ������ ����
	if (pPC->IsRunning())
		return;

	if (bItemCount == 0)
		return;

#ifdef ENABLE_LONG_LONG
	long long nTotalMoney = 0;

	for (int n = 0; n < bItemCount; ++n)
	{
		nTotalMoney += static_cast<long long>((pTable+n)->price);
	}

	nTotalMoney += static_cast<long long>(GetGold());
#else
	int64_t nTotalMoney = 0;

	for (int n = 0; n < bItemCount; ++n)
	{
		nTotalMoney += static_cast<int64_t>((pTable + n)->price);
	}

	nTotalMoney += static_cast<int64_t>(GetGold());
#endif
	if (GOLD_MAX <= nTotalMoney)
	{
#ifdef TEXTS_IMPROVEMENT
		ChatPacketNew(CHAT_TYPE_INFO, 226, 
#ifdef ENABLE_LONG_LONG
		"%lld"
#else
		"%d"
#endif
		, GOLD_MAX);
#endif
		return;
	}

	char szSign[SHOP_SIGN_MAX_LEN+1];
	strlcpy(szSign, c_pszSign, sizeof(szSign));

	m_stShopSign = szSign;

	if (m_stShopSign.length() == 0)
		return;

	if (CBanwordManager::instance().CheckString(m_stShopSign.c_str(), m_stShopSign.length()))
	{
#ifdef TEXTS_IMPROVEMENT
		ChatPacketNew(CHAT_TYPE_INFO, 358, "");
#endif
		return;
	}
	
#ifdef KASMIR_PAKET_SYSTEM
	m_bKasmirPaketBaslik = KasmirBaslik;
	if (m_bKasmirPaketBaslik < 1 && m_bKasmirPaketBaslik > 6)
	{
#ifdef TEXTS_IMPROVEMENT
		ChatPacketNew(CHAT_TYPE_INFO, 46, "");
#endif
		return;
	}
#endif
	
	// MYSHOP_PRICE_LIST
	std::map<DWORD, DWORD> itemkind;  // ������ ������ ����, first: vnum, second: ���� ���� ����
	// END_OF_MYSHOP_PRICE_LIST

	std::set<TItemPos> cont;
	for (BYTE i = 0; i < bItemCount; ++i)
	{
		if (cont.find((pTable + i)->pos) != cont.end())
		{
			sys_err("MYSHOP: duplicate shop item detected! (name: %s)", GetName());
			return;
		}

		// ANTI_GIVE, ANTI_MYSHOP check
		LPITEM pkItem = GetItem((pTable + i)->pos);

		if (pkItem)
		{
			const TItemTable * item_table = pkItem->GetProto();

			if (item_table && (IS_SET(item_table->dwAntiFlags, ITEM_ANTIFLAG_GIVE | ITEM_ANTIFLAG_MYSHOP)))
			{
#ifdef TEXTS_IMPROVEMENT
				ChatPacketNew(CHAT_TYPE_INFO, 416, "%s", pkItem->GetName());
#endif
				return;
			}

			if (pkItem->IsEquipped() == true)
			{
#ifdef TEXTS_IMPROVEMENT
				ChatPacketNew(CHAT_TYPE_INFO, 541, "");
#endif
				return;
			}

			if (true == pkItem->isLocked())
			{
#ifdef TEXTS_IMPROVEMENT
				ChatPacketNew(CHAT_TYPE_INFO, 656, "");
#endif
				return;
			}

			// MYSHOP_PRICE_LIST
			itemkind[pkItem->GetVnum()] = (pTable + i)->price / pkItem->GetCount();
			// END_OF_MYSHOP_PRICE_LIST
		}

		cont.insert((pTable + i)->pos);
	}

	// MYSHOP_PRICE_LIST
	// ������ ������ ���ҽ�Ų��.
	if (CountSpecifyItem(71049)
#ifdef KASMIR_PAKET_SYSTEM
	|| CountSpecifyItem(88901)
#endif
	) { // ��� �������� ������ �ʰ� ���������� �����Ѵ�.

		//
		// ������ ���������� �����ϱ� ���� ������ �������� ��Ŷ�� ����� DB ĳ�ÿ� ������.
		//
		// @fixme403 BEGIN
		TItemPriceListTable header;
		memset(&header, 0, sizeof(TItemPriceListTable));

		header.dwOwnerID = GetPlayerID();
		header.byCount = itemkind.size();

		size_t idx=0;
		for (itertype(itemkind) it = itemkind.begin(); it != itemkind.end(); ++it)
		{
			header.aPriceInfo[idx].dwVnum = it->first;
			header.aPriceInfo[idx].dwPrice = it->second;
			idx++;
		}

		db_clientdesc->DBPacket(HEADER_GD_MYSHOP_PRICELIST_UPDATE, GetDesc()->GetHandle(), &header, sizeof(TItemPriceListTable));
		// @fixme403 END
	}
	// END_OF_MYSHOP_PRICE_LIST
	else if (CountSpecifyItem(50200))
		RemoveSpecifyItem(50200, 1);
	else
		return; // �������� ������ �ߴ�.

	if (m_pkExchange)
		m_pkExchange->Cancel();

	TPacketGCShopSign p;

	p.bHeader = HEADER_GC_SHOP_SIGN;
	p.dwVID = GetVID();
	strlcpy(p.szSign, c_pszSign, sizeof(p.szSign));
#ifdef KASMIR_PAKET_SYSTEM
	p.bShopKasmirTitle = KasmirBaslik;
#endif
	PacketAround(&p, sizeof(TPacketGCShopSign));

	m_pkMyShop = CShopManager::instance().CreatePCShop(this, pTable, bItemCount);

	if (IsPolymorphed() == true)
	{
		RemoveAffect(AFFECT_POLYMORPH);
	}

	if (GetHorse())
	{
		HorseSummon( false, true );
	}
	// new mount �̿� �߿�, ���� ���� ���� �ڵ� unmount
	// StopRiding���� ������Ʈ���� ó���ϸ� ������ �� �׷��� ���س����� �� �� ����.
	else if (GetMountVnum())
	{
		RemoveAffect(AFFECT_MOUNT);
		RemoveAffect(AFFECT_MOUNT_BONUS);
	}
	
	DWORD dwNpcShop = 30000;
#ifdef KASMIR_PAKET_SYSTEM
	dwNpcShop = KasmirNpc >= 30000 && KasmirNpc <= 30007 ? KasmirNpc : 30000;
#endif
	SetPolymorph(dwNpcShop, true);
}

void CHARACTER::CloseMyShop()
{
	if (GetMyShop())
	{
		m_stShopSign.clear();
		CShopManager::instance().DestroyPCShop(this);
		m_pkMyShop = NULL;
		m_bKasmirPaketBaslik = 0;
		m_bKasmirPaketDurum = false;
		
		TPacketGCShopSign p;

		p.bHeader = HEADER_GC_SHOP_SIGN;
		p.dwVID = GetVID();
#ifdef KASMIR_PAKET_SYSTEM
		p.bShopKasmirTitle = m_bKasmirPaketBaslik;
#endif
		p.szSign[0] = '\0';

		PacketAround(&p, sizeof(p));
#ifdef ENABLE_WOLFMAN_CHARACTER
		SetPolymorph(m_points.job, true);
		// SetPolymorph(0, true);
#else
		SetPolymorph(GetJob(), true);
#endif
	}
}

void EncodeMovePacket(TPacketGCMove & pack, DWORD dwVID, BYTE bFunc, BYTE bArg, DWORD x, DWORD y, DWORD dwDuration, DWORD dwTime, BYTE bRot)
{
	pack.bHeader = HEADER_GC_MOVE;
	pack.bFunc   = bFunc;
	pack.bArg    = bArg;
	pack.dwVID   = dwVID;
	pack.dwTime  = dwTime ? dwTime : get_dword_time();
	pack.bRot    = bRot;
	pack.lX		= x;
	pack.lY		= y;
	pack.dwDuration	= dwDuration;
}

void CHARACTER::RestartAtSamePos()
{
	if (m_bIsObserver)
		return;

	EncodeRemovePacket(this);
	EncodeInsertPacket(this);

	ENTITY_MAP::iterator it = m_map_view.begin();

	while (it != m_map_view.end())
	{
		LPENTITY entity = (it++)->first;

		EncodeRemovePacket(entity);
		if (!m_bIsObserver)
			EncodeInsertPacket(entity);

		if( entity->IsType(ENTITY_CHARACTER) )
		{
			LPCHARACTER lpChar = (LPCHARACTER)entity;
			if( lpChar->IsPC() || lpChar->IsNPC() || lpChar->IsMonster() )
			{
				if (!entity->IsObserverMode())
					entity->EncodeInsertPacket(this);
			}
		}
		else
		{
			if( !entity->IsObserverMode())
			{
				entity->EncodeInsertPacket(this);
			}
		}
	}
}

// #define ENABLE_SHOWNPCLEVEL
// Entity�� ���� ��Ÿ���ٰ� ��Ŷ�� ������.
void CHARACTER::EncodeInsertPacket(LPENTITY entity) {
	LPDESC d;
	if (!(d = entity->GetDesc()))
		return;

	LPCHARACTER ch = (LPCHARACTER) entity;
	ch->SendGuildName(GetGuild());

#ifdef ENABLE_SOUL_SYSTEM
	TAffectFlag sendAffectFlag = m_afAffectFlag;
	if(sendAffectFlag.IsSet(AFF_SOUL_RED) && sendAffectFlag.IsSet(AFF_SOUL_BLUE))
	{
		sendAffectFlag.Reset(AFF_SOUL_RED);
		sendAffectFlag.Reset(AFF_SOUL_BLUE);
		sendAffectFlag.Set(AFF_SOUL_MIX);
	}
#endif

	TPacketGCCharacterAdd pack;
	pack.header = HEADER_GC_CHARACTER_ADD;
	pack.dwVID = m_vid;
	pack.bType = GetCharType();
	pack.angle = GetRotation();
	pack.x = GetX();
	pack.y = GetY();
	pack.z = GetZ();
	pack.wRaceNum = GetRaceNum();
	if ((pack.wRaceNum >= 20101 && pack.wRaceNum <= 20109)|| IsPet()
#ifdef __NEWPET_SYSTEM__
	 || IsNewPet()
#endif
#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
	|| m_bIsMount == true
#endif
	)
	{
#ifdef ENABLE_MULTI_NAMES
		pack.transname = false;
#endif
#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
		if (m_bIsMount == true) {
			pack.bMovingSpeed = GetLimitPoint(POINT_MOV_SPEED);
		} else {
			pack.bMovingSpeed = IsPC() ? GetLimitPoint(POINT_MOV_SPEED) : 150;
		}
#else
		pack.bMovingSpeed	= 150;
#endif
	} else {
#ifdef ENABLE_MULTI_NAMES
		pack.transname = true;
#endif
		pack.bMovingSpeed	= GetLimitPoint(POINT_MOV_SPEED);
	}
	pack.bAttackSpeed	= GetLimitPoint(POINT_ATT_SPEED);
#ifdef ENABLE_SOUL_SYSTEM
	pack.dwAffectFlag[0] = sendAffectFlag.bits[0];
	pack.dwAffectFlag[1] = sendAffectFlag.bits[1];
#else
	pack.dwAffectFlag[0] = m_afAffectFlag.bits[0];
	pack.dwAffectFlag[1] = m_afAffectFlag.bits[1];
#endif

	pack.bStateFlag = m_bAddChrState;

	int iDur = 0;
	if (m_posDest.x != pack.x || m_posDest.y != pack.y) {
		iDur = (m_dwMoveStartTime + m_dwMoveDuration) - get_dword_time();
		if (iDur <= 0) {
			pack.x = m_posDest.x;
			pack.y = m_posDest.y;
		}
	}

	d->Packet(&pack, sizeof(pack));
	if (IsPC() == true || m_bCharType == CHAR_TYPE_NPC) {
		TPacketGCCharacterAdditionalInfo addPacket;
		addPacket.dwLevel = 0;
		addPacket.sAlignment = 0;
		addPacket.dwMountVnum = 0;
#ifdef ENABLE_MULTI_LANGUAGE
		addPacket.bLanguage = 0;
#endif
		if (!IsPC()) {
			memcpy(addPacket.dwSkillColor, GetSkillColor(), sizeof(addPacket.dwSkillColor));
		}
		
		addPacket.header = HEADER_GC_CHAR_ADDITIONAL_INFO;
		addPacket.dwVID = m_vid;
		addPacket.bPKMode = m_bPKMode;
		addPacket.bEmpire = m_bEmpire;
		addPacket.dwGuildID = 0;
		strlcpy(addPacket.name, GetName(), sizeof(addPacket.name));
		addPacket.awPart[CHR_EQUIPPART_ARMOR] = GetPart(PART_MAIN);
		addPacket.awPart[CHR_EQUIPPART_WEAPON] = GetPart(PART_WEAPON);
		addPacket.awPart[CHR_EQUIPPART_HEAD] = GetPart(PART_HEAD);
		addPacket.awPart[CHR_EQUIPPART_HAIR] = GetPart(PART_HAIR);
#ifdef ENABLE_RUNE_SYSTEM
		addPacket.awPart[CHR_EQUIPPART_RUNE] = GetPart(PART_RUNE);
#endif
#ifdef ENABLE_ACCE_SYSTEM
		addPacket.awPart[CHR_EQUIPPART_ACCE] = GetPart(PART_ACCE);
#endif
#ifdef ENABLE_COSTUME_EFFECT
		addPacket.awPart[CHR_EQUIPPART_EFFECT_BODY] = GetPart(PART_EFFECT_BODY);
		addPacket.awPart[CHR_EQUIPPART_EFFECT_WEAPON] = GetPart(PART_EFFECT_WEAPON);
#endif
		if (IsPC()) {
			addPacket.dwLevel = GetLevel();
			addPacket.dwMountVnum = GetMountVnum();
			addPacket.dwGuildID = GetGuild() ? GetGuild()->GetID() : 0;
			addPacket.sAlignment = m_iAlignment / 10;
#ifdef __SKILL_COLOR_SYSTEM__
			memcpy(addPacket.dwSkillColor, GetSkillColor(), sizeof(addPacket.dwSkillColor));
#endif
		}
#ifdef __NEWPET_SYSTEM__
		if (IsNewPet()) {
			addPacket.dwLevel = GetLevel();
		}
#else
#endif
		d->Packet(&addPacket, sizeof(TPacketGCCharacterAdditionalInfo));
	}

	if (iDur) {
		TPacketGCMove pack;
		EncodeMovePacket(pack, GetVID(), FUNC_MOVE, 0, m_posDest.x, m_posDest.y, iDur, 0, (BYTE) (GetRotation() / 5));
		d->Packet(&pack, sizeof(pack));

		TPacketGCWalkMode p;
		p.vid = GetVID();
		p.header = HEADER_GC_WALK_MODE;
		p.mode = m_bNowWalking ? WALKMODE_WALK : WALKMODE_RUN;

		d->Packet(&p, sizeof(p));
	}

	if (entity->IsType(ENTITY_CHARACTER) && GetDesc()) {
		LPCHARACTER ch = (LPCHARACTER) entity;
		if (ch->IsWalking()) {
			TPacketGCWalkMode p;
			p.vid = ch->GetVID();
			p.header = HEADER_GC_WALK_MODE;
			p.mode = ch->m_bNowWalking ? WALKMODE_WALK : WALKMODE_RUN;
			GetDesc()->Packet(&p, sizeof(p));
		}
	}

	if (GetMyShop()) {
		TPacketGCShopSign p;
		p.bHeader = HEADER_GC_SHOP_SIGN;
		p.dwVID = GetVID();
#ifdef KASMIR_PAKET_SYSTEM
		p.bShopKasmirTitle = m_bKasmirPaketBaslik;
#endif
		strlcpy(p.szSign, m_stShopSign.c_str(), sizeof(p.szSign));
		
		d->Packet(&p, sizeof(TPacketGCShopSign));
	}

	if (entity->IsType(ENTITY_CHARACTER)) {
		sys_log(3, "EntityInsert %s (RaceNum %d) (%d %d) TO %s",
				GetName(), GetRaceNum(), GetX() / SECTREE_SIZE, GetY() / SECTREE_SIZE, ((LPCHARACTER)entity)->GetName());
	}
}

void CHARACTER::EncodeRemovePacket(LPENTITY entity)
{
	if (entity->GetType() != ENTITY_CHARACTER)
		return;

	LPDESC d;

	if (!(d = entity->GetDesc()))
		return;

	TPacketGCCharacterDelete pack;

	pack.header	= HEADER_GC_CHARACTER_DEL;
	pack.id	= m_vid;

	d->Packet(&pack, sizeof(TPacketGCCharacterDelete));

	if (entity->IsType(ENTITY_CHARACTER))
		sys_log(3, "EntityRemove %s(%d) FROM %s", GetName(), (DWORD) m_vid, ((LPCHARACTER) entity)->GetName());
}

void CHARACTER::UpdatePacket()
{
	if (GetSectree() == NULL) return;

#ifdef ENABLE_SOUL_SYSTEM
	TAffectFlag sendAffectFlag = m_afAffectFlag;
	if(sendAffectFlag.IsSet(AFF_SOUL_RED) && sendAffectFlag.IsSet(AFF_SOUL_BLUE))
	{
		sendAffectFlag.Reset(AFF_SOUL_RED);
		sendAffectFlag.Reset(AFF_SOUL_BLUE);
		sendAffectFlag.Set(AFF_SOUL_MIX);
	}
#endif

	TPacketGCCharacterUpdate pack;
	TPacketGCCharacterUpdate pack2;

	pack.header = HEADER_GC_CHARACTER_UPDATE;
	pack.dwVID = m_vid;

	pack.awPart[CHR_EQUIPPART_ARMOR] = GetPart(PART_MAIN);
	pack.awPart[CHR_EQUIPPART_WEAPON] = GetPart(PART_WEAPON);
	pack.awPart[CHR_EQUIPPART_HEAD] = GetPart(PART_HEAD);
	pack.awPart[CHR_EQUIPPART_HAIR] = GetPart(PART_HAIR);
#ifdef ENABLE_RUNE_SYSTEM
	pack.awPart[CHR_EQUIPPART_RUNE] = GetPart(PART_RUNE);
#endif
#ifdef ENABLE_ACCE_SYSTEM
	pack.awPart[CHR_EQUIPPART_ACCE] = GetPart(PART_ACCE);
#endif
#ifdef ENABLE_COSTUME_EFFECT
	pack.awPart[CHR_EQUIPPART_EFFECT_BODY] = GetPart(PART_EFFECT_BODY);
	pack.awPart[CHR_EQUIPPART_EFFECT_WEAPON] = GetPart(PART_EFFECT_WEAPON);
#endif
#ifdef __SKILL_COLOR_SYSTEM__
	memcpy(pack.dwSkillColor, GetSkillColor(), sizeof(pack.dwSkillColor));
#endif

	pack.bMovingSpeed	= GetLimitPoint(POINT_MOV_SPEED);
	pack.bAttackSpeed	= GetLimitPoint(POINT_ATT_SPEED);
	pack.bStateFlag	= m_bAddChrState;
#ifdef ENABLE_SOUL_SYSTEM
	pack.dwAffectFlag[0] = sendAffectFlag.bits[0];
	pack.dwAffectFlag[1] = sendAffectFlag.bits[1];
#else
	pack.dwAffectFlag[0] = m_afAffectFlag.bits[0];
	pack.dwAffectFlag[1] = m_afAffectFlag.bits[1];
#endif
	pack.dwGuildID	= 0;
	pack.sAlignment	= m_iAlignment / 10;
#ifdef ENABLE_MULTI_LANGUAGE
	pack.bLanguage = (IsPC() && GetDesc()) ? GetDesc()->GetLanguage() : 0;
#endif
	pack.bPKMode	= m_bPKMode;

	if (GetGuild())
		pack.dwGuildID = GetGuild()->GetID();

	pack.dwMountVnum	= GetMountVnum();

	pack2 = pack;
	pack2.dwGuildID = 0;
	pack2.sAlignment = 0;
	if (false)
	{
		if (m_bIsObserver != true)
		{
			for (ENTITY_MAP::iterator iter = m_map_view.begin(); iter != m_map_view.end(); iter++)
			{
				LPENTITY pEntity = iter->first;

				if (pEntity != NULL)
				{
					if (pEntity->IsType(ENTITY_CHARACTER) == true)
					{
						if (pEntity->GetDesc() != NULL)
						{
							LPCHARACTER pChar = (LPCHARACTER)pEntity;

							if (GetEmpire() == pChar->GetEmpire() || pChar->GetGMLevel() > GM_PLAYER)
							{
								pEntity->GetDesc()->Packet(&pack, sizeof(pack));
							}
							else
							{
								pEntity->GetDesc()->Packet(&pack2, sizeof(pack2));
							}
						}
					}
					else
					{
						if (pEntity->GetDesc() != NULL)
						{
							pEntity->GetDesc()->Packet(&pack, sizeof(pack));
						}
					}
				}
			}
		}

		if (GetDesc() != NULL)
		{
			GetDesc()->Packet(&pack, sizeof(pack));
		}
	}
	else
	{
		PacketAround(&pack, sizeof(pack));
	}
}

LPCHARACTER CHARACTER::FindCharacterInView(const char * c_pszName, bool bFindPCOnly)
{
	ENTITY_MAP::iterator it = m_map_view.begin();

	for (; it != m_map_view.end(); ++it)
	{
		if (!it->first->IsType(ENTITY_CHARACTER))
			continue;

		LPCHARACTER tch = (LPCHARACTER) it->first;

		if (bFindPCOnly && tch->IsNPC())
			continue;

		if (!strcasecmp(tch->GetName(), c_pszName))
			return (tch);
	}

	return NULL;
}

void CHARACTER::SetPosition(int pos)
{
	if (pos == POS_STANDING)
	{
		REMOVE_BIT(m_bAddChrState, ADD_CHARACTER_STATE_DEAD);
		REMOVE_BIT(m_pointsInstant.instant_flag, INSTANT_FLAG_STUN);

		event_cancel(&m_pkDeadEvent);
		event_cancel(&m_pkStunEvent);
	}
	else if (pos == POS_DEAD)
		SET_BIT(m_bAddChrState, ADD_CHARACTER_STATE_DEAD);

	if (!IsStone())
	{
		switch (pos)
		{
			case POS_FIGHTING:
				if (!IsState(m_stateBattle))
					MonsterLog("[BATTLE] �ο�� ����");

				GotoState(m_stateBattle);
				break;

			default:
				if (!IsState(m_stateIdle))
					MonsterLog("[IDLE] ���� ����");

				GotoState(m_stateIdle);
				break;
		}
	}

	m_pointsInstant.position = pos;
}

void CHARACTER::Save()
{
	if (!m_bSkipSave)
		CHARACTER_MANAGER::instance().DelayedSave(this);
}

void CHARACTER::CreatePlayerProto(TPlayerTable & tab)
{
	memset(&tab, 0, sizeof(TPlayerTable));

	if (GetNewName().empty())
	{
		strlcpy(tab.name, GetName(), sizeof(tab.name));
	}
	else
	{
		strlcpy(tab.name, GetNewName().c_str(), sizeof(tab.name));
	}

	strlcpy(tab.ip, GetDesc() ? GetDesc()->GetHostName() : "", sizeof(tab.ip));

	tab.id			= m_dwPlayerID;
	tab.voice		= GetPoint(POINT_VOICE);
	tab.level		= GetLevel();
	tab.level_step	= GetPoint(POINT_LEVEL_STEP);
	tab.exp			= GetExp();
	tab.gold		= GetGold();
#ifdef ENABLE_GAYA_SYSTEM
	tab.gaya		= GetGaya();
#endif
	tab.job			= m_points.job;
	tab.part_base	= m_pointsInstant.bBasePart;
	tab.skill_group	= m_points.skill_group;
#ifdef __ENABLE_EXTEND_INVEN_SYSTEM__
	tab.envanter 		= Inven_Point();
#endif
	DWORD dwPlayedTime = (get_dword_time() - m_dwPlayStartTime);

	if (dwPlayedTime > 60000)
	{
		if (GetSectree() && !GetSectree()->IsAttr(GetX(), GetY(), ATTR_BANPK))
		{
			if (GetRealAlignment() < 0)
			{
				if (IsEquipUniqueItem(UNIQUE_ITEM_FASTER_ALIGNMENT_UP_BY_TIME))
					UpdateAlignment(120 * (dwPlayedTime / 60000));
				else
					UpdateAlignment(60 * (dwPlayedTime / 60000));
			}
			else
				UpdateAlignment(5 * (dwPlayedTime / 60000));
		}

		SetRealPoint(POINT_PLAYTIME, GetRealPoint(POINT_PLAYTIME) + dwPlayedTime / 60000);
		ResetPlayTime(dwPlayedTime % 60000);
	}

	tab.playtime = GetRealPoint(POINT_PLAYTIME);
	tab.lAlignment = m_iRealAlignment;

	if (m_posWarp.x != 0 || m_posWarp.y != 0)
	{
		tab.x = m_posWarp.x;
		tab.y = m_posWarp.y;
		tab.z = 0;
		tab.lMapIndex = m_lWarpMapIndex;
	}
	else
	{
		tab.x = GetX();
		tab.y = GetY();
		tab.z = GetZ();
		tab.lMapIndex	= GetMapIndex();
	}

	if (m_lExitMapIndex == 0)
	{
		tab.lExitMapIndex	= tab.lMapIndex;
		tab.lExitX		= tab.x;
		tab.lExitY		= tab.y;
	}
	else
	{
		tab.lExitMapIndex	= m_lExitMapIndex;
		tab.lExitX		= m_posExit.x;
		tab.lExitY		= m_posExit.y;
	}

	sys_log(0, "SAVE: %s %dx%d", GetName(), tab.x, tab.y);

	tab.st = GetRealPoint(POINT_ST);
	tab.ht = GetRealPoint(POINT_HT);
	tab.dx = GetRealPoint(POINT_DX);
	tab.iq = GetRealPoint(POINT_IQ);

	tab.stat_point = GetPoint(POINT_STAT);
	tab.skill_point = GetPoint(POINT_SKILL);
	tab.sub_skill_point = GetPoint(POINT_SUB_SKILL);
	tab.horse_skill_point = GetPoint(POINT_HORSE_SKILL);

	tab.stat_reset_count = GetPoint(POINT_STAT_RESET_COUNT);

	tab.hp = GetHP();
	tab.sp = GetSP();

	tab.stamina = GetStamina();

	tab.sRandomHP = m_points.iRandomHP;
	tab.sRandomSP = m_points.iRandomSP;

	for (int i = 0; i < QUICKSLOT_MAX_NUM; ++i)
		tab.quickslot[i] = m_quickslot[i];

	if (m_stMobile.length() && !*m_szMobileAuth)
		strlcpy(tab.szMobile, m_stMobile.c_str(), sizeof(tab.szMobile));

	thecore_memcpy(tab.parts, m_pointsInstant.parts, sizeof(tab.parts));
	thecore_memcpy(tab.skills, m_pSkillLevels, sizeof(TPlayerSkill) * SKILL_MAX_NUM);

#ifdef ENABLE_BATTLE_PASS
	tab.dwBattlePassEndTime = m_dwBattlePassEndTime;
#endif
#ifdef ENABLE_RANKING
	for (int i = 0; i < RANKING_MAX_CATEGORIES; ++i) {
		tab.lRankPoints[i] = m_lRankPoints[i];
	}
#endif
	tab.horse = GetHorseData();
}


void CHARACTER::SaveReal()
{
	if (m_bSkipSave)
		return;

	if (!GetDesc())
	{
		sys_err("Character::Save : no descriptor when saving (name: %s)", GetName());
		return;
	}

	TPlayerTable table;
	CreatePlayerProto(table);

	db_clientdesc->DBPacket(HEADER_GD_PLAYER_SAVE, GetDesc()->GetHandle(), &table, sizeof(TPlayerTable));

	quest::PC * pkQuestPC = quest::CQuestManager::instance().GetPCForce(GetPlayerID());

	if (!pkQuestPC)
		sys_err("CHARACTER::Save : null quest::PC pointer! (name %s)", GetName());
	else
	{
		pkQuestPC->Save();
	}

	marriage::TMarriage* pMarriage = marriage::CManager::instance().Get(GetPlayerID());
	if (pMarriage)
		pMarriage->Save();
}

void CHARACTER::FlushDelayedSaveItem()
{
	// ���� �ȵ� ����ǰ�� ���� �����Ų��.
	LPITEM item;

	for (int i = 0; i < INVENTORY_AND_EQUIP_SLOT_MAX; ++i)
		if ((item = GetInventoryItem(i)))
			ITEM_MANAGER::instance().FlushDelayedSave(item);
}

void CHARACTER::Disconnect(const char * c_pszReason)
{
	assert(GetDesc() != NULL);

	sys_log(0, "DISCONNECT: %s (%s)", GetName(), c_pszReason ? c_pszReason : "unset" );
	
	if (GetShop())
	{
		GetShop()->RemoveGuest(this);
		SetShop(NULL);
	}

	if (GetArena() != NULL)
	{
		GetArena()->OnDisconnect(GetPlayerID());
	}

	if (GetParty() != NULL)
	{
		GetParty()->UpdateOfflineState(GetPlayerID());
	}

	marriage::CManager::instance().Logout(this);

	// P2P Logout
	TPacketGGLogout p;
	p.bHeader = HEADER_GG_LOGOUT;
	strlcpy(p.szName, GetName(), sizeof(p.szName));
	P2P_MANAGER::instance().Send(&p, sizeof(TPacketGGLogout));
	LogManager::instance().CharLog(this, 0, "LOGOUT", "");

#ifdef ENABLE_PCBANG_FEATURE // @warme006
	{
		long playTime = GetRealPoint(POINT_PLAYTIME) - m_dwLoginPlayTime;
		LogManager::instance().LoginLog(false, GetDesc()->GetAccountTable().id, GetPlayerID(), GetLevel(), GetJob(), playTime);

		if (0)
			CPCBangManager::instance().Log(GetDesc()->GetHostName(), GetPlayerID(), playTime);
	}
#endif

	if (m_pWarMap)
		SetWarMap(NULL);

	if (m_pWeddingMap)
	{
		SetWeddingMap(NULL);
	}
#ifdef __ENABLE_NEW_OFFLINESHOP__
	offlineshop::GetManager().RemoveSafeboxFromCache(GetPlayerID());
	offlineshop::GetManager().RemoveGuestFromShops(this);
	
	if(m_pkAuctionGuest)
		m_pkAuctionGuest->RemoveGuest(this);

	if (GetOfflineShop())
		SetOfflineShop(NULL);

	SetShopSafebox(NULL);

	m_pkAuction		= NULL;
	m_pkAuctionGuest= NULL;

	//offlineshop-updated 05/08/19
	m_bIsLookingOfflineshopOfferList = false;
#endif

	if (GetGuild())
		GetGuild()->LogoutMember(this);

	quest::CQuestManager::instance().LogoutPC(this);

#ifdef ENABLE_PVP_ADVANCED
	DestroyPvP();
#endif

	if (GetParty())
		GetParty()->Unlink(this);

	// �׾��� �� ���Ӳ����� ����ġ �ٰ� �ϱ�
	if (IsStun() || IsDead())
	{
		DeathPenalty(0);
		PointChange(POINT_HP, 50 - GetHP());
	}


	if (!CHARACTER_MANAGER::instance().FlushDelayedSave(this))
	{
		SaveReal();
	}

	FlushDelayedSaveItem();

	SaveAffect();
	m_bIsLoadedAffect = false;

#ifdef ENABLE_BATTLE_PASS
	itertype(m_listBattlePass) it = m_listBattlePass.begin();
	while (it != m_listBattlePass.end())
	{
		TPlayerBattlePassMission * pkMission = *it++;
		
		if (pkMission->bIsUpdated)
		{
			db_clientdesc->DBPacket(HEADER_GD_SAVE_BATTLE_PASS, 0, pkMission, sizeof(TPlayerBattlePassMission));
		}
		
		if(pkMission)
			M2_DELETE(pkMission);
	}
	m_bIsLoadedBattlePass = false;
#endif

	m_bSkipSave = true; // �� ���Ŀ��� ���̻� �����ϸ� �ȵȴ�.

	quest::CQuestManager::instance().DisconnectPC(this);

	CloseSafebox();

	CloseMall();

	CPVPManager::instance().Disconnect(this);

	CTargetManager::instance().Logout(GetPlayerID());

	MessengerManager::instance().Logout(GetName());

#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
	if(GetMountVnum())
	{
		RemoveAffect(AFFECT_MOUNT);
		RemoveAffect(AFFECT_MOUNT_BONUS);
	}
#endif

	if (g_TeenDesc)
	{
		int		offset = 0;
		char	buf[245] = {0};

		buf[0] = HEADER_GT_LOGOUT;
		offset += 1;

		memset(buf+offset, 0x00, 2);
		offset += 2;

		TAccountTable	&acc_table = GetDesc()->GetAccountTable();
		memcpy(buf+offset, &acc_table.id, 4);
		offset += 4;

		g_TeenDesc->Packet(buf, offset);
	}

	if (GetDesc())
	{
		GetDesc()->BindCharacter(NULL);
//		BindDesc(NULL);
	}

	M2_DESTROY_CHARACTER(this);
}

bool CHARACTER::Show(long lMapIndex, long x, long y, long z, bool bShowSpawnMotion/* = false */)
{
	LPSECTREE sectree = SECTREE_MANAGER::instance().Get(lMapIndex, x, y);

	if (!sectree)
	{
		sys_log(0, "cannot find sectree by %dx%d mapindex %d", x, y, lMapIndex);
		return false;
	}

	SetMapIndex(lMapIndex);

	bool bChangeTree = false;

	if (!GetSectree() || GetSectree() != sectree)
		bChangeTree = true;

	if (bChangeTree)
	{
		if (GetSectree())
			GetSectree()->RemoveEntity(this);

		ViewCleanup();
	}

	if (!IsNPC())
	{
		sys_log(0, "SHOW: %s %dx%dx%d", GetName(), x, y, z);
		if (GetStamina() < GetMaxStamina())
			StartAffectEvent();
	}
	else if (m_pkMobData)
	{
		m_pkMobInst->m_posLastAttacked.x = x;
		m_pkMobInst->m_posLastAttacked.y = y;
		m_pkMobInst->m_posLastAttacked.z = z;
	}

	if (bShowSpawnMotion)
	{
		SET_BIT(m_bAddChrState, ADD_CHARACTER_STATE_SPAWN);
		m_afAffectFlag.Set(AFF_SPAWN);
	}

	SetXYZ(x, y, z);

	m_posDest.x = x;
	m_posDest.y = y;
	m_posDest.z = z;

	m_posStart.x = x;
	m_posStart.y = y;
	m_posStart.z = z;

	if (bChangeTree)
	{
		EncodeInsertPacket(this);
		sectree->InsertEntity(this);

		UpdateSectree();
	}
	else
	{
		ViewReencode();
		sys_log(0, "      in same sectree");
	}

	REMOVE_BIT(m_bAddChrState, ADD_CHARACTER_STATE_SPAWN);

	SetValidComboInterval(0);
	return true;
}

// BGM_INFO
struct BGMInfo
{
	std::string	name;
	float		vol;
};

typedef std::map<unsigned, BGMInfo> BGMInfoMap;

static BGMInfoMap 	gs_bgmInfoMap;
static bool		gs_bgmVolEnable = false;

void CHARACTER_SetBGMVolumeEnable()
{
	gs_bgmVolEnable = true;
	sys_log(0, "bgm_info.set_bgm_volume_enable");
}

void CHARACTER_AddBGMInfo(unsigned mapIndex, const char* name, float vol)
{
	BGMInfo newInfo;
	newInfo.name = name;
	newInfo.vol = vol;

	gs_bgmInfoMap[mapIndex] = newInfo;

	sys_log(0, "bgm_info.add_info(%d, '%s', %f)", mapIndex, name, vol);
}

const BGMInfo& CHARACTER_GetBGMInfo(unsigned mapIndex)
{
	BGMInfoMap::iterator f = gs_bgmInfoMap.find(mapIndex);
	if (gs_bgmInfoMap.end() == f)
	{
		static BGMInfo s_empty = {"", 0.0f};
		return s_empty;
	}
	return f->second;
}

bool CHARACTER_IsBGMVolumeEnable()
{
	return gs_bgmVolEnable;
}
// END_OF_BGM_INFO

void CHARACTER::MainCharacterPacket()
{
	const unsigned mapIndex = GetMapIndex();
	const BGMInfo& bgmInfo = CHARACTER_GetBGMInfo(mapIndex);

	// SUPPORT_BGM
	if (!bgmInfo.name.empty())
	{
		if (CHARACTER_IsBGMVolumeEnable())
		{
			sys_log(1, "bgm_info.play_bgm_vol(%d, name='%s', vol=%f)", mapIndex, bgmInfo.name.c_str(), bgmInfo.vol);
			TPacketGCMainCharacter4_BGM_VOL mainChrPacket;
			mainChrPacket.header = HEADER_GC_MAIN_CHARACTER4_BGM_VOL;
			mainChrPacket.dwVID = m_vid;
			mainChrPacket.wRaceNum = GetRaceNum();
			mainChrPacket.lx = GetX();
			mainChrPacket.ly = GetY();
			mainChrPacket.lz = GetZ();
			mainChrPacket.empire = GetDesc()->GetEmpire();
			mainChrPacket.skill_group = GetSkillGroup();
			strlcpy(mainChrPacket.szChrName, GetName(), sizeof(mainChrPacket.szChrName));

			mainChrPacket.fBGMVol = bgmInfo.vol;
			strlcpy(mainChrPacket.szBGMName, bgmInfo.name.c_str(), sizeof(mainChrPacket.szBGMName));
			GetDesc()->Packet(&mainChrPacket, sizeof(TPacketGCMainCharacter4_BGM_VOL));
		}
		else
		{
			sys_log(1, "bgm_info.play(%d, '%s')", mapIndex, bgmInfo.name.c_str());
			TPacketGCMainCharacter3_BGM mainChrPacket;
			mainChrPacket.header = HEADER_GC_MAIN_CHARACTER3_BGM;
			mainChrPacket.dwVID = m_vid;
			mainChrPacket.wRaceNum = GetRaceNum();
			mainChrPacket.lx = GetX();
			mainChrPacket.ly = GetY();
			mainChrPacket.lz = GetZ();
			mainChrPacket.empire = GetDesc()->GetEmpire();
			mainChrPacket.skill_group = GetSkillGroup();
			strlcpy(mainChrPacket.szChrName, GetName(), sizeof(mainChrPacket.szChrName));
			strlcpy(mainChrPacket.szBGMName, bgmInfo.name.c_str(), sizeof(mainChrPacket.szBGMName));
			GetDesc()->Packet(&mainChrPacket, sizeof(TPacketGCMainCharacter3_BGM));
		}
		//if (m_stMobile.length())
		//		ChatPacket(CHAT_TYPE_COMMAND, "sms");
	}
	// END_OF_SUPPORT_BGM
	else
	{
		sys_log(0, "bgm_info.play(%d, DEFAULT_BGM_NAME)", mapIndex);

		TPacketGCMainCharacter pack;
		pack.header = HEADER_GC_MAIN_CHARACTER;
		pack.dwVID = m_vid;
		pack.wRaceNum = GetRaceNum();
		pack.lx = GetX();
		pack.ly = GetY();
		pack.lz = GetZ();
		pack.empire = GetDesc()->GetEmpire();
		pack.skill_group = GetSkillGroup();
		strlcpy(pack.szName, GetName(), sizeof(pack.szName));
		GetDesc()->Packet(&pack, sizeof(TPacketGCMainCharacter));

		if (m_stMobile.length())
			ChatPacket(CHAT_TYPE_COMMAND, "sms");
	}
}

void CHARACTER::PointsPacket()
{
	if (!GetDesc())
		return;

	TPacketGCPoints pack;

	pack.header	= HEADER_GC_CHARACTER_POINTS;

	pack.points[POINT_LEVEL]		= GetLevel();
	pack.points[POINT_EXP]		= GetExp();
	pack.points[POINT_NEXT_EXP]		= GetNextExp();
	pack.points[POINT_HP]		= GetHP();
	pack.points[POINT_MAX_HP]		= GetMaxHP();
	pack.points[POINT_SP]		= GetSP();
	pack.points[POINT_MAX_SP]		= GetMaxSP();
	pack.points[POINT_GOLD]		= GetGold();
	pack.points[POINT_STAMINA]		= GetStamina();
	pack.points[POINT_MAX_STAMINA]	= GetMaxStamina();
#ifdef __ENABLE_EXTEND_INVEN_SYSTEM__
	pack.points[POINT_INVEN]		= Inven_Point();
#endif

	for (int i = POINT_ST; i < POINT_MAX_NUM; ++i)
		pack.points[i] = GetPoint(i);
#ifdef ENABLE_GAYA_SYSTEM
	pack.points[POINT_GAYA] = GetGaya();
#endif
	GetDesc()->Packet(&pack, sizeof(TPacketGCPoints));
}

bool CHARACTER::ChangeSex()
{
	int src_race = GetRaceNum();

	switch (src_race)
	{
		case MAIN_RACE_WARRIOR_M:
			m_points.job = MAIN_RACE_WARRIOR_W;
			break;

		case MAIN_RACE_WARRIOR_W:
			m_points.job = MAIN_RACE_WARRIOR_M;
			break;

		case MAIN_RACE_ASSASSIN_M:
			m_points.job = MAIN_RACE_ASSASSIN_W;
			break;

		case MAIN_RACE_ASSASSIN_W:
			m_points.job = MAIN_RACE_ASSASSIN_M;
			break;

		case MAIN_RACE_SURA_M:
			m_points.job = MAIN_RACE_SURA_W;
			break;

		case MAIN_RACE_SURA_W:
			m_points.job = MAIN_RACE_SURA_M;
			break;

		case MAIN_RACE_SHAMAN_M:
			m_points.job = MAIN_RACE_SHAMAN_W;
			break;

		case MAIN_RACE_SHAMAN_W:
			m_points.job = MAIN_RACE_SHAMAN_M;
			break;
#ifdef ENABLE_WOLFMAN_CHARACTER
		case MAIN_RACE_WOLFMAN_M:
			m_points.job = MAIN_RACE_WOLFMAN_M;
			break;
#endif
		default:
			sys_err("CHANGE_SEX: %s unknown race %d", GetName(), src_race);
			return false;
	}

	sys_log(0, "CHANGE_SEX: %s (%d -> %d)", GetName(), src_race, m_points.job);
	return true;
}

WORD CHARACTER::GetRaceNum() const
{
	if (m_dwPolymorphRace)
		return m_dwPolymorphRace;

	if (m_pkMobData)
		return m_pkMobData->m_table.dwVnum;

	return m_points.job;
}

void CHARACTER::SetRace(BYTE race)
{
	if (race >= MAIN_RACE_MAX_NUM)
	{
		sys_err("CHARACTER::SetRace(name=%s, race=%d).OUT_OF_RACE_RANGE", GetName(), race);
		return;
	}

	m_points.job = race;
}

BYTE CHARACTER::GetJob() const
{
	unsigned race = m_points.job;
	unsigned job;

	if (RaceToJob(race, &job))
		return job;

	sys_err("CHARACTER::GetJob(name=%s, race=%d).OUT_OF_RACE_RANGE", GetName(), race);
	return JOB_WARRIOR;
}

void CHARACTER::SetLevel(BYTE level)
{
	m_points.level = level;

	if (IsPC())
	{
		if (level < PK_PROTECT_LEVEL)
			SetPKMode(PK_MODE_PROTECT);
		else if (GetGMLevel() != GM_PLAYER)
			SetPKMode(PK_MODE_PROTECT);
		else if (m_bPKMode == PK_MODE_PROTECT)
			SetPKMode(PK_MODE_PEACE);
	}
}

void CHARACTER::SetEmpire(BYTE bEmpire)
{
	m_bEmpire = bEmpire;
}

#define ENABLE_GM_FLAG_IF_TEST_SERVER
#define ENABLE_GM_FLAG_FOR_LOW_WIZARD
void CHARACTER::SetPlayerProto(const TPlayerTable * t)
{
	if (!GetDesc() || !*GetDesc()->GetHostName())
		sys_err("cannot get desc or hostname");
	else
		SetGMLevel();

	m_bCharType = CHAR_TYPE_PC;

	m_dwPlayerID = t->id;

	m_iAlignment = t->lAlignment;
	m_iRealAlignment = t->lAlignment;

	m_points.voice = t->voice;

	m_points.skill_group = t->skill_group;

	m_pointsInstant.bBasePart = t->part_base;
	SetPart(PART_HAIR, t->parts[PART_HAIR]);
#ifdef ENABLE_ACCE_SYSTEM
	SetPart(PART_ACCE, t->parts[PART_ACCE]);
#endif

	m_points.iRandomHP = t->sRandomHP;
	m_points.iRandomSP = t->sRandomSP;

	if (m_pSkillLevels) {
		M2_DELETE_ARRAY(m_pSkillLevels);
	}

	m_pSkillLevels = M2_NEW TPlayerSkill[SKILL_MAX_NUM];
	thecore_memcpy(m_pSkillLevels, t->skills, sizeof(TPlayerSkill) * SKILL_MAX_NUM);
#ifdef ENABLE_BATTLE_PASS
	m_dwBattlePassEndTime = t->dwBattlePassEndTime;
#endif

	if (t->lMapIndex >= 10000)
	{
		m_posWarp.x = t->lExitX;
		m_posWarp.y = t->lExitY;
		m_lWarpMapIndex = t->lExitMapIndex;
	}

	SetRealPoint(POINT_PLAYTIME, t->playtime);
	m_dwLoginPlayTime = t->playtime;
	SetRealPoint(POINT_ST, t->st);
	SetRealPoint(POINT_HT, t->ht);
	SetRealPoint(POINT_DX, t->dx);
	SetRealPoint(POINT_IQ, t->iq);

	SetPoint(POINT_ST, t->st);
	SetPoint(POINT_HT, t->ht);
	SetPoint(POINT_DX, t->dx);
	SetPoint(POINT_IQ, t->iq);

	SetPoint(POINT_STAT, t->stat_point);
	SetPoint(POINT_SKILL, t->skill_point);
	SetPoint(POINT_SUB_SKILL, t->sub_skill_point);
	SetPoint(POINT_HORSE_SKILL, t->horse_skill_point);

	SetPoint(POINT_STAT_RESET_COUNT, t->stat_reset_count);

	SetPoint(POINT_LEVEL_STEP, t->level_step);
	SetRealPoint(POINT_LEVEL_STEP, t->level_step);

	SetRace(t->job);

	SetLevel(t->level);
	SetExp(t->exp);
	SetGold(t->gold);
#ifdef ENABLE_GAYA_SYSTEM
	SetGaya(t->gaya);
#endif
#ifdef __ENABLE_EXTEND_INVEN_SYSTEM__
	Set_Inventory_Point(t->envanter);
#endif

	SetMapIndex(t->lMapIndex);
	SetXYZ(t->x, t->y, t->z);

	ComputePoints();

	SetHP(t->hp);
	SetSP(t->sp);
	SetStamina(t->stamina);

	//GM�϶� ��ȣ���
#ifndef ENABLE_GM_FLAG_IF_TEST_SERVER
	if (!test_server)
#endif
	{
#ifdef ENABLE_GM_FLAG_FOR_LOW_WIZARD
		if (GetGMLevel() > GM_PLAYER)
#else
		if (GetGMLevel() > GM_LOW_WIZARD)
#endif
		{
			m_afAffectFlag.Set(AFF_YMIR);
			m_bPKMode = PK_MODE_PROTECT;
		}
	}




	if (GetLevel() < PK_PROTECT_LEVEL)
		m_bPKMode = PK_MODE_PROTECT;

	m_stMobile = t->szMobile;

	SetHorseData(t->horse);

	if (GetHorseLevel() > 0)
		UpdateHorseDataByLogoff(t->logoff_interval);

	thecore_memcpy(m_aiPremiumTimes, t->aiPremiumTimes, sizeof(t->aiPremiumTimes));

	m_dwLogOffInterval = t->logoff_interval;

	sys_log(0, "PLAYER_LOAD: %s PREMIUM %d %d, LOGGOFF_INTERVAL %u PTR: %p", t->name, m_aiPremiumTimes[0], m_aiPremiumTimes[1], t->logoff_interval, this);

	if (GetGMLevel() != GM_PLAYER)
	{
		LogManager::instance().CharLog(this, GetGMLevel(), "GM_LOGIN", "");
		sys_log(0, "GM_LOGIN(gmlevel=%d, name=%s(%d), pos=(%d, %d)", GetGMLevel(), GetName(), GetPlayerID(), GetX(), GetY());
	}
	
#ifdef ENABLE_RANKING
	for (int i = 0; i < RANKING_MAX_CATEGORIES; ++i)
		m_lRankPoints[i] = t->lRankPoints[i];
#endif

#ifdef __PET_SYSTEM__
	// NOTE: �ϴ� ĳ���Ͱ� PC�� ��쿡�� PetSystem�� ������ ��. ���� �ӽŴ� �޸� ���������� NPC���� �ϱ� ��..
	if (m_petSystem)
	{
		m_petSystem->Destroy();
		delete m_petSystem;
	}

	m_petSystem = M2_NEW CPetSystem(this);
#endif

#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
	if (m_mountSystem)
	{
		m_mountSystem->Destroy();
		delete m_mountSystem;
	}

	m_mountSystem = M2_NEW CMountSystem(this);
#endif

#ifdef __NEWPET_SYSTEM__
	if (m_newpetSystem)
	{
		m_newpetSystem->Destroy();
		delete m_newpetSystem;
	}

	m_newpetSystem = M2_NEW CNewPetSystem(this);
#endif
}

EVENTFUNC(kill_ore_load_event)
{
	char_event_info* info = dynamic_cast<char_event_info*>( event->info );
	if ( info == NULL )
	{
		sys_err( "kill_ore_load_even> <Factor> Null pointer" );
		return 0;
	}

	LPCHARACTER	ch = info->ch;
	if (ch == NULL) { // <Factor>
		return 0;
	}

	ch->m_pkMiningEvent = NULL;
	M2_DESTROY_CHARACTER(ch);
	return 0;
}

void CHARACTER::SetProto(const CMob * pkMob)
{
	if (m_pkMobInst)
		M2_DELETE(m_pkMobInst);

	m_pkMobData = pkMob;
	m_pkMobInst = M2_NEW CMobInstance;

	m_bPKMode = PK_MODE_FREE;

	const TMobTable * t = &m_pkMobData->m_table;

	m_bCharType = t->bType;

	SetLevel(t->bLevel);
	SetEmpire(t->bEmpire);

	SetExp(t->dwExp);
	SetRealPoint(POINT_ST, t->bStr);
	SetRealPoint(POINT_DX, t->bDex);
	SetRealPoint(POINT_HT, t->bCon);
	SetRealPoint(POINT_IQ, t->bInt);

	ComputePoints();

	SetHP(GetMaxHP());
	SetSP(GetMaxSP());

	////////////////////
	m_pointsInstant.dwAIFlag = t->dwAIFlag;
	SetImmuneFlag(t->dwImmuneFlag);

	AssignTriggers(t);

	ApplyMobAttribute(t);

	if (IsStone())
	{
		DetermineDropMetinStone();
		DetermineDropMetinStofa();
		DetermineDropMetinSacca();
#ifdef ENABLE_CHRISTMAS_2021
		if (quest::CQuestManager::instance().GetEventFlag("christmas_event") != 0)
		{
			DetermineDropMetinSock();
		}
#endif
	}

	if (IsWarp() || IsGoto())
	{
		StartWarpNPCEvent();
	}

	CHARACTER_MANAGER::instance().RegisterRaceNumMap(this);

	// XXX CTF GuildWar hardcoding
	if (warmap::IsWarFlag(GetRaceNum()))
	{
		m_stateIdle.Set(this, &CHARACTER::BeginStateEmpty, &CHARACTER::StateFlag, &CHARACTER::EndStateEmpty);
		m_stateMove.Set(this, &CHARACTER::BeginStateEmpty, &CHARACTER::StateFlag, &CHARACTER::EndStateEmpty);
		m_stateBattle.Set(this, &CHARACTER::BeginStateEmpty, &CHARACTER::StateFlag, &CHARACTER::EndStateEmpty);
	}

	if (warmap::IsWarFlagBase(GetRaceNum()))
	{
		m_stateIdle.Set(this, &CHARACTER::BeginStateEmpty, &CHARACTER::StateFlagBase, &CHARACTER::EndStateEmpty);
		m_stateMove.Set(this, &CHARACTER::BeginStateEmpty, &CHARACTER::StateFlagBase, &CHARACTER::EndStateEmpty);
		m_stateBattle.Set(this, &CHARACTER::BeginStateEmpty, &CHARACTER::StateFlagBase, &CHARACTER::EndStateEmpty);
	}

	if (m_bCharType == CHAR_TYPE_HORSE ||
			GetRaceNum() == 20101 ||
			GetRaceNum() == 20102 ||
			GetRaceNum() == 20103 ||
			GetRaceNum() == 20104 ||
			GetRaceNum() == 20105 ||
			GetRaceNum() == 20106 ||
			GetRaceNum() == 20107 ||
			GetRaceNum() == 20108 ||
			GetRaceNum() == 20109
	  )
	{
		m_stateIdle.Set(this, &CHARACTER::BeginStateEmpty, &CHARACTER::StateHorse, &CHARACTER::EndStateEmpty);
		m_stateMove.Set(this, &CHARACTER::BeginStateEmpty, &CHARACTER::StateMove, &CHARACTER::EndStateEmpty);
		m_stateBattle.Set(this, &CHARACTER::BeginStateEmpty, &CHARACTER::StateHorse, &CHARACTER::EndStateEmpty);
	}

	// MINING
	if (mining::IsVeinOfOre (GetRaceNum()))
	{
		char_event_info* info = AllocEventInfo<char_event_info>();

		info->ch = this;

		m_pkMiningEvent = event_create(kill_ore_load_event, info, PASSES_PER_SEC(number(7 * 60, 15 * 60)));
	}
	// END_OF_MINING
}

const TMobTable & CHARACTER::GetMobTable() const
{
	return m_pkMobData->m_table;
}

bool CHARACTER::IsRaceFlag(DWORD dwBit) const
{
	return m_pkMobData ? IS_SET(m_pkMobData->m_table.dwRaceFlag, dwBit) : 0;
}

DWORD CHARACTER::GetMobDamageMin() const
{
	return m_pkMobData->m_table.dwDamageRange[0];
}

DWORD CHARACTER::GetMobDamageMax() const
{
	return m_pkMobData->m_table.dwDamageRange[1];
}

float CHARACTER::GetMobDamageMultiply() const
{
	float fDamMultiply = GetMobTable().fDamMultiply;

	if (IsBerserk())
		fDamMultiply = fDamMultiply * 2.0f; // BALANCE: ����ȭ �� �ι�

	return fDamMultiply;
}

DWORD CHARACTER::GetMobDropItemVnum() const
{
	return m_pkMobData->m_table.dwDropItemVnum;
}

bool CHARACTER::IsSummonMonster() const
{
	return GetSummonVnum() != 0;
}

DWORD CHARACTER::GetSummonVnum() const
{
	return m_pkMobData ? m_pkMobData->m_table.dwSummonVnum : 0;
}

DWORD CHARACTER::GetPolymorphItemVnum() const
{
	return m_pkMobData ? m_pkMobData->m_table.dwPolymorphItemVnum : 0;
}

DWORD CHARACTER::GetMonsterDrainSPPoint() const
{
	return m_pkMobData ? m_pkMobData->m_table.dwDrainSP : 0;
}

BYTE CHARACTER::GetMobRank() const
{
	if (!m_pkMobData)
		return MOB_RANK_KNIGHT;	// PC�� ��� KNIGHT��

	return m_pkMobData->m_table.bRank;
}

BYTE CHARACTER::GetMobSize() const
{
	if (!m_pkMobData)
		return MOBSIZE_MEDIUM;

	return m_pkMobData->m_table.bSize;
}

WORD CHARACTER::GetMobAttackRange() const
{
	switch (GetMobBattleType())
	{
		case BATTLE_TYPE_RANGE:
		case BATTLE_TYPE_MAGIC:
#ifdef __DEFENSE_WAVE__
			if (GetRaceNum() == 3960 || GetRaceNum() == 3961 || GetRaceNum() == 3962)
			{
				return m_pkMobData->m_table.wAttackRange + GetPoint(POINT_BOW_DISTANCE) + 4000;
			}
			else
			{
				return m_pkMobData->m_table.wAttackRange;
			}
#else
			return m_pkMobData->m_table.wAttackRange + GetPoint(POINT_BOW_DISTANCE);
#endif
		default:
#ifdef __DEFENSE_WAVE__
			if ((GetRaceNum() <= 3955 && GetRaceNum() >= 3950 && GetRaceNum() != 3953) || (GetRaceNum() <= 3605 && GetRaceNum() >= 3601 && GetRaceNum() != 3602))
			{
				return m_pkMobData->m_table.wAttackRange + 300;
			}
			else
			{
				return m_pkMobData->m_table.wAttackRange;
			}
#else
			return m_pkMobData->m_table.wAttackRange;
#endif
	}
}

BYTE CHARACTER::GetMobBattleType() const
{
	if (!m_pkMobData)
		return BATTLE_TYPE_MELEE;

	return (m_pkMobData->m_table.bBattleType);
}

void CHARACTER::ComputeBattlePoints()
{
	if (IsPolymorphed())
	{
		DWORD dwMobVnum = GetPolymorphVnum();
		const CMob * pMob = CMobManager::instance().Get(dwMobVnum);
		int iAtt = 0;
		int iDef = 0;

		if (pMob)
		{
			iAtt = GetLevel() * 2 + GetPolymorphPoint(POINT_ST) * 2;
			// lev + con
			iDef = GetLevel() + GetPolymorphPoint(POINT_HT) + pMob->m_table.wDef;
		}

		SetPoint(POINT_ATT_GRADE, iAtt);
		SetPoint(POINT_DEF_GRADE, iDef);
		SetPoint(POINT_MAGIC_ATT_GRADE, GetPoint(POINT_ATT_GRADE));
		SetPoint(POINT_MAGIC_DEF_GRADE, GetPoint(POINT_DEF_GRADE));
	}
	else if (IsPC())
	{
		SetPoint(POINT_ATT_GRADE, 0);
		SetPoint(POINT_DEF_GRADE, 0);
		SetPoint(POINT_CLIENT_DEF_GRADE, 0);
		SetPoint(POINT_MAGIC_ATT_GRADE, GetPoint(POINT_ATT_GRADE));
		SetPoint(POINT_MAGIC_DEF_GRADE, GetPoint(POINT_DEF_GRADE));

		//
		// �⺻ ATK = 2lev + 2str, ������ ���� 2str�� �ٲ� �� ����
		//
		int iAtk = GetLevel() * 2;
		int iStatAtk = 0;

		switch (GetJob())
		{
			case JOB_WARRIOR:
			case JOB_SURA:
				iStatAtk = (2 * GetPoint(POINT_ST));
				break;

			case JOB_ASSASSIN:
				iStatAtk = (4 * GetPoint(POINT_ST) + 2 * GetPoint(POINT_DX)) / 3;
				break;

			case JOB_SHAMAN:
				iStatAtk = (4 * GetPoint(POINT_ST) + 2 * GetPoint(POINT_IQ)) / 3;
				break;
#ifdef ENABLE_WOLFMAN_CHARACTER
			case JOB_WOLFMAN:
				// TODO: ������ ���ݷ� ���� ��ȹ�ڿ��� ��û
				iStatAtk = (2 * GetPoint(POINT_ST));
				break;
#endif
			default:
				sys_err("invalid job %d", GetJob());
				iStatAtk = (2 * GetPoint(POINT_ST));
				break;
		}

		// ���� Ÿ�� �ְ�, �������� ���� ���ݷ��� ST*2 ���� ������ ST*2�� �Ѵ�.
		// ������ �߸� ���� ��� ���ݷ��� �� ���� �ʰ� �ϱ� ���ؼ���.
		if (GetMountVnum() && iStatAtk < 2 * GetPoint(POINT_ST))
			iStatAtk = (2 * GetPoint(POINT_ST));

		iAtk += iStatAtk;

		// �¸�(��) : �˼��� ������ ����
		if (GetMountVnum())
		{
			if (GetJob() == JOB_SURA && GetSkillGroup() == 1)
			{
				iAtk += (iAtk * GetHorseLevel()) / 60;
			}
			else
			{
				iAtk += (iAtk * GetHorseLevel()) / 30;
			}
		}

		//
		// ATK Setting
		//
		iAtk += GetPoint(POINT_ATT_GRADE_BONUS);

		PointChange(POINT_ATT_GRADE, iAtk);

		// DEF = LEV + CON + ARMOR
		int iShowDef = GetLevel() + GetPoint(POINT_HT); // For Ymir(õ��)
		int iDef = GetLevel() + (int) (GetPoint(POINT_HT) / 1.25); // For Other
		int iArmor = 0;

		LPITEM pkItem;

		for (int i = 0; i < WEAR_MAX_NUM; ++i)
			if ((pkItem = GetWear(i)) && pkItem->GetType() == ITEM_ARMOR)
			{
#ifdef ENABLE_PENDANT
				if (pkItem->GetSubType() == ARMOR_BODY || pkItem->GetSubType() == ARMOR_HEAD || pkItem->GetSubType() == ARMOR_FOOTS || pkItem->GetSubType() == ARMOR_SHIELD || pkItem->GetSubType() == ARMOR_PENDANT)
#else
				if (pkItem->GetSubType() == ARMOR_BODY || pkItem->GetSubType() == ARMOR_HEAD || pkItem->GetSubType() == ARMOR_FOOTS || pkItem->GetSubType() == ARMOR_SHIELD)
#endif
				{
					iArmor += pkItem->GetValue(1);
					iArmor += (2 * pkItem->GetValue(5));
				}
			}

		// �� Ÿ�� ���� �� ������ ���� ���� ���º��� ������ ���� �������� ����
		if( true == IsHorseRiding() )
		{
			if (iArmor < GetHorseArmor())
				iArmor = GetHorseArmor();

			const char* pHorseName = CHorseNameManager::instance().GetHorseName(GetPlayerID());

			if (pHorseName != NULL && strlen(pHorseName))
			{
				iArmor += 20;
			}
		}

		iArmor += GetPoint(POINT_DEF_GRADE_BONUS);
		iArmor += GetPoint(POINT_PARTY_DEFENDER_BONUS);

		// INTERNATIONAL_VERSION
		PointChange(POINT_DEF_GRADE, iDef + iArmor);
		PointChange(POINT_CLIENT_DEF_GRADE, (iShowDef + iArmor) - GetPoint(POINT_DEF_GRADE));
		// END_OF_INTERNATIONAL_VERSION

		PointChange(POINT_MAGIC_ATT_GRADE, GetLevel() * 2 + GetPoint(POINT_IQ) * 2 + GetPoint(POINT_MAGIC_ATT_GRADE_BONUS));
		PointChange(POINT_MAGIC_DEF_GRADE, GetLevel() + (GetPoint(POINT_IQ) * 3 + GetPoint(POINT_HT)) / 3 + iArmor / 2 + GetPoint(POINT_MAGIC_DEF_GRADE_BONUS));
	}
	else
	{
		// 2lev + str * 2
		int iAtt = GetLevel() * 2 + GetPoint(POINT_ST) * 2;
		// lev + con
		int iDef = GetLevel() + GetPoint(POINT_HT) + GetMobTable().wDef;

		SetPoint(POINT_ATT_GRADE, iAtt);
		SetPoint(POINT_DEF_GRADE, iDef);
		SetPoint(POINT_MAGIC_ATT_GRADE, GetPoint(POINT_ATT_GRADE));
		SetPoint(POINT_MAGIC_DEF_GRADE, GetPoint(POINT_DEF_GRADE));
	}
}

void CHARACTER::ComputePoints()
{
	long lStat = GetPoint(POINT_STAT);
	long lStatResetCount = GetPoint(POINT_STAT_RESET_COUNT);
	long lSkillActive = GetPoint(POINT_SKILL);
	long lSkillSub = GetPoint(POINT_SUB_SKILL);
	long lSkillHorse = GetPoint(POINT_HORSE_SKILL);
	long lLevelStep = GetPoint(POINT_LEVEL_STEP);

	long lAttackerBonus = GetPoint(POINT_PARTY_ATTACKER_BONUS);
	long lTankerBonus = GetPoint(POINT_PARTY_TANKER_BONUS);
	long lBufferBonus = GetPoint(POINT_PARTY_BUFFER_BONUS);
	long lSkillMasterBonus = GetPoint(POINT_PARTY_SKILL_MASTER_BONUS);
	long lHasteBonus = GetPoint(POINT_PARTY_HASTE_BONUS);
	long lDefenderBonus = GetPoint(POINT_PARTY_DEFENDER_BONUS);

	long lHPRecovery = GetPoint(POINT_HP_RECOVERY);
	long lSPRecovery = GetPoint(POINT_SP_RECOVERY);
#ifdef __ENABLE_EXTEND_INVEN_SYSTEM__
	long envanterim = Inven_Point();
#endif

	memset(m_pointsInstant.points, 0, sizeof(m_pointsInstant.points));
	BuffOnAttr_ClearAll();
	m_SkillDamageBonus.clear();

	SetPoint(POINT_STAT, lStat);
	SetPoint(POINT_SKILL, lSkillActive);
	SetPoint(POINT_SUB_SKILL, lSkillSub);
	SetPoint(POINT_HORSE_SKILL, lSkillHorse);
	SetPoint(POINT_LEVEL_STEP, lLevelStep);
	SetPoint(POINT_STAT_RESET_COUNT, lStatResetCount);

	SetPoint(POINT_ST, GetRealPoint(POINT_ST));
	SetPoint(POINT_HT, GetRealPoint(POINT_HT));
	SetPoint(POINT_DX, GetRealPoint(POINT_DX));
	SetPoint(POINT_IQ, GetRealPoint(POINT_IQ));
#ifdef ENABLE_FIX_LEVELUP_EFFECT
	SetPart(PART_MAIN, GetPart(PART_MAIN));
#else
	SetPart(PART_MAIN, GetOriginalPart(PART_MAIN));
#endif
	SetPart(PART_WEAPON, GetOriginalPart(PART_WEAPON));
	SetPart(PART_HEAD, GetOriginalPart(PART_HEAD));
	SetPart(PART_HAIR, GetOriginalPart(PART_HAIR));
#ifdef ENABLE_RUNE_SYSTEM
	SetPart(PART_RUNE, GetOriginalPart(PART_RUNE));
#endif
#ifdef ENABLE_ACCE_SYSTEM
	SetPart(PART_ACCE, GetOriginalPart(PART_ACCE));
#endif
#ifdef ENABLE_COSTUME_EFFECT
	SetPart(PART_EFFECT_BODY, GetOriginalPart(PART_EFFECT_BODY));
	SetPart(PART_EFFECT_WEAPON, GetOriginalPart(PART_EFFECT_WEAPON));
#endif
	SetPoint(POINT_PARTY_ATTACKER_BONUS, lAttackerBonus);
	SetPoint(POINT_PARTY_TANKER_BONUS, lTankerBonus);
	SetPoint(POINT_PARTY_BUFFER_BONUS, lBufferBonus);
	SetPoint(POINT_PARTY_SKILL_MASTER_BONUS, lSkillMasterBonus);
	SetPoint(POINT_PARTY_HASTE_BONUS, lHasteBonus);
	SetPoint(POINT_PARTY_DEFENDER_BONUS, lDefenderBonus);

	SetPoint(POINT_HP_RECOVERY, lHPRecovery);
	SetPoint(POINT_SP_RECOVERY, lSPRecovery);

	// PC_BANG_ITEM_ADD
	SetPoint(POINT_PC_BANG_EXP_BONUS, 0);
	SetPoint(POINT_PC_BANG_DROP_BONUS, 0);
	// END_PC_BANG_ITEM_ADD

#ifdef __ENABLE_EXTEND_INVEN_SYSTEM__
	SetPoint(POINT_INVEN, envanterim);
#endif

	int iMaxHP, iMaxSP;
	int iMaxStamina;

	if (IsPC())
	{
		// �ִ� �����/���ŷ�
		iMaxHP = JobInitialPoints[GetJob()].max_hp + m_points.iRandomHP + GetPoint(POINT_HT) * JobInitialPoints[GetJob()].hp_per_ht;
		iMaxSP = JobInitialPoints[GetJob()].max_sp + m_points.iRandomSP + GetPoint(POINT_IQ) * JobInitialPoints[GetJob()].sp_per_iq;
		iMaxStamina = JobInitialPoints[GetJob()].max_stamina + GetPoint(POINT_HT) * JobInitialPoints[GetJob()].stamina_per_con;

		{
			CSkillProto* pkSk = CSkillManager::instance().Get(SKILL_ADD_HP);

			if (NULL != pkSk)
			{
				pkSk->SetPointVar("k", 1.0f * GetSkillPower(SKILL_ADD_HP) / 100.0f);

				iMaxHP += static_cast<int>(pkSk->kPointPoly.Eval());
			}
		}

#ifdef ENABLE_NEW_SECONDARY_SKILLS
		{
			long lValue[4][11] = {
								{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10},
								{0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20},
								{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10},
								{0, 200, 400, 800, 1200, 1600, 2000, 2200, 2500, 2700, 3000},
			};
			
			PointChange(POINT_MALL_ATTBONUS, lValue[0][GetSkillLevel(NEW_SUPPORT_SKILL_ATTACK)]);
			PointChange(POINT_MALL_GOLDBONUS, lValue[1][GetSkillLevel(NEW_SUPPORT_SKILL_YANG)]);
			PointChange(POINT_ATTBONUS_MONSTER, lValue[2][GetSkillLevel(NEW_SUPPORT_SKILL_MONSTERS)]);
			iMaxHP += lValue[3][GetSkillLevel(NEW_SUPPORT_SKILL_HP)];
		}
#endif

#ifdef ENABLE_NEW_MOVEMENT_SPEED
		SetPoint(POINT_MOV_SPEED, 200);
#else
		SetPoint(POINT_MOV_SPEED,	100);
#endif
		SetPoint(POINT_ATT_SPEED,	100);
		PointChange(POINT_ATT_SPEED, GetPoint(POINT_PARTY_HASTE_BONUS));
		SetPoint(POINT_CASTING_SPEED,	100);
	}
	else
	{
		iMaxHP = m_pkMobData->m_table.dwMaxHP;
		iMaxSP = 0;
		iMaxStamina = 0;

		SetPoint(POINT_ATT_SPEED, m_pkMobData->m_table.sAttackSpeed);
		SetPoint(POINT_MOV_SPEED, m_pkMobData->m_table.sMovingSpeed);
		SetPoint(POINT_CASTING_SPEED, m_pkMobData->m_table.sAttackSpeed);
	}

	if (IsPC())
	{
		DWORD mountVnum = GetMountVnum();
		if (mountVnum)
		{
			bool horse = mountVnum >= 20101 && mountVnum <= 20107 ? true : false;
			int st = horse == true ? GetHorseST() : 36;
			int dx = horse == true ? GetHorseDX() : 18;
			int ht = horse == true ? GetHorseHT() : 53;
			int iq = horse == true ? GetHorseIQ() : 71;
			if (st > GetPoint(POINT_ST))
				PointChange(POINT_ST, st - GetPoint(POINT_ST));

			if (dx > GetPoint(POINT_DX))
				PointChange(POINT_DX, dx - GetPoint(POINT_DX));

			if (ht > GetPoint(POINT_HT))
				PointChange(POINT_HT, ht - GetPoint(POINT_HT));

			if (iq > GetPoint(POINT_IQ))
				PointChange(POINT_IQ, iq - GetPoint(POINT_IQ));
		}

	}

	ComputeBattlePoints();

	// �⺻ HP/SP ����
	if (iMaxHP != GetMaxHP())
	{
		SetRealPoint(POINT_MAX_HP, iMaxHP); // �⺻HP�� RealPoint�� ������ ���´�.
	}

	PointChange(POINT_MAX_HP, 0);

	if (iMaxSP != GetMaxSP())
	{
		SetRealPoint(POINT_MAX_SP, iMaxSP); // �⺻SP�� RealPoint�� ������ ���´�.
	}

	PointChange(POINT_MAX_SP, 0);

	SetMaxStamina(iMaxStamina);
	// @fixme118 part1
	int iCurHP = this->GetHP();
	int iCurSP = this->GetSP();

	m_pointsInstant.dwImmuneFlag = 0;

	for (int i = 0 ; i < WEAR_MAX_NUM; i++) {
		LPITEM pItem = GetWear(i);
		if (pItem) {
#ifdef ENABLE_RUNE_SYSTEM
			if (pItem->IsRune() && pItem->GetSocket(1) != 1) {
				continue;
			}
#endif

			pItem->ModifyPoints(true);
			SET_BIT(m_pointsInstant.dwImmuneFlag, GetWear(i)->GetImmuneFlag());
		}
	}
#ifdef ENABLE_EVENT_MANAGER
		CHARACTER_MANAGER::Instance().CheckBonusEvent(this);
#endif
	// ��ȥ�� �ý���
	// ComputePoints������ �ɸ����� ��� �Ӽ����� �ʱ�ȭ�ϰ�,
	// ������, ���� � ���õ� ��� �Ӽ����� �����ϱ� ������,
	// ��ȥ�� �ý��۵� ActiveDeck�� �ִ� ��� ��ȥ���� �Ӽ����� �ٽ� ������Ѿ� �Ѵ�.
	if (DragonSoul_IsDeckActivated())
	{
		for (int i = WEAR_MAX_NUM + DS_SLOT_MAX * DragonSoul_GetActiveDeck();
			i < WEAR_MAX_NUM + DS_SLOT_MAX * (DragonSoul_GetActiveDeck() + 1); i++)
		{
			LPITEM pItem = GetWear(i);
			if (pItem)
			{
				if (DSManager::instance().IsTimeLeftDragonSoul(pItem))
					pItem->ModifyPoints(true);
			}
		}
	}

	if (GetHP() > GetMaxHP())
		PointChange(POINT_HP, GetMaxHP() - GetHP());

	if (GetSP() > GetMaxSP())
		PointChange(POINT_SP, GetMaxSP() - GetSP());

	ComputeSkillPoints();

	RefreshAffect();

	CPetSystem * pPetSystem = GetPetSystem();
	if (NULL != pPetSystem) {
		pPetSystem->RefreshBuff();
	}

//#ifdef __NEWPET_SYSTEM__
//	if (m_newpetSystem) {
//		m_newpetSystem->RefreshBuff();
//	}
//#endif

	// @fixme118 part2 (after petsystem stuff)
	if (IsPC())
	{
		if (this->GetHP() != iCurHP)
			this->PointChange(POINT_HP, iCurHP-this->GetHP());
		if (this->GetSP() != iCurSP)
			this->PointChange(POINT_SP, iCurSP-this->GetSP());
	}

	UpdatePacket();
}

// m_dwPlayStartTime�� ������ milisecond��. �����ͺ��̽����� �д����� ����ϱ�
// ������ �÷��̽ð��� ����� �� / 60000 ���� ������ �ϴµ�, �� ������ ���� ����
// �� �� ���⿡ dwTimeRemain���� �־ ����� ���ǵ��� ���־�� �Ѵ�.
void CHARACTER::ResetPlayTime(DWORD dwTimeRemain)
{
	m_dwPlayStartTime = get_dword_time() - dwTimeRemain;
}

const int aiRecoveryPercents[10] = { 1, 5, 5, 5, 5, 5, 5, 5, 5, 5 };

EVENTFUNC(recovery_event)
{
	char_event_info* info = dynamic_cast<char_event_info*>( event->info );
	if ( info == NULL )
	{
		sys_err( "recovery_event> <Factor> Null pointer" );
		return 0;
	}

	LPCHARACTER	ch = info->ch;

	if (ch == NULL) { // <Factor>
		return 0;
	}

	if (!ch->IsPC())
	{
		//
		// ���� ȸ��
		//
		if (ch->IsAffectFlag(AFF_POISON))
			return PASSES_PER_SEC(MAX(1, ch->GetMobTable().bRegenCycle));
			
#ifdef ENABLE_WOLFMAN_CHARACTER
		if (ch->IsAffectFlag(AFF_BLEEDING))
			return PASSES_PER_SEC(MAX(1, ch->GetMobTable().bRegenCycle));
#endif
		
#ifdef ENABLE_DS_RUNE
		if (ch->GetMobTable().dwVnum == 3996) {
			LPDUNGEON target = ch->GetDungeon();
			if (target) {
				if (target->GetFlag("floor") == 5) {
					ch->DistributeSP(ch);
					if (ch->GetMaxHP() <= ch->GetHP())
						return PASSES_PER_SEC(3);
					
					int iPercent = 0;
					int iAmount = 0;
					
					{
						iPercent = 2;
						iAmount = 15 + (ch->GetMaxHP() * iPercent) / 100;
					}
					
					iAmount += (iAmount * ch->GetPoint(POINT_HP_REGEN)) / 100;
					sys_log(1, "RECOVERY_EVENT: %s %d HP_REGEN %d HP +%d", ch->GetName(), iPercent, ch->GetPoint(POINT_HP_REGEN), iAmount);
					ch->PointChange(POINT_HP, iAmount, false);
					return PASSES_PER_SEC(10);
				}
			}
		}
		else if (ch->GetMobTable().dwVnum == 8202) {
			LPDUNGEON target = ch->GetDungeon();
			if (target) {
				if (target->GetFlag("floor") == 1) {
					ch->DistributeSP(ch);
					if (ch->GetMaxHP() <= ch->GetHP())
						return PASSES_PER_SEC(3);
					
					int iPercent = 0;
					int iAmount = 0;
					
					{
						iPercent = 2;
						iAmount = 15 + (ch->GetMaxHP() * iPercent) / 100;
					}
					
					iAmount += (iAmount * ch->GetPoint(POINT_HP_REGEN)) / 100;
					sys_log(1, "RECOVERY_EVENT: %s %d HP_REGEN %d HP +%d", ch->GetName(), iPercent, ch->GetPoint(POINT_HP_REGEN), iAmount);
					ch->PointChange(POINT_HP, iAmount, false);
					return PASSES_PER_SEC(10);
				}
			}
		}
#endif

		if (!ch->IsDoor())
		{
			ch->MonsterLog("HP_REGEN +%d", MAX(1, (ch->GetMaxHP() * ch->GetMobTable().bRegenPercent) / 100));
			ch->PointChange(POINT_HP, MAX(1, (ch->GetMaxHP() * ch->GetMobTable().bRegenPercent) / 100));
		}
		
		if (ch->GetHP() >= ch->GetMaxHP())
		{
			ch->m_pkRecoveryEvent = NULL;
			return 0;
		}

		return PASSES_PER_SEC(MAX(1, ch->GetMobTable().bRegenCycle));
	}
	else
	{
		//
		// PC ȸ��
		//
		ch->CheckTarget();
		//ch->UpdateSectree(); // ���⼭ �̰� ������?
		ch->UpdateKillerMode();

		if (ch->IsAffectFlag(AFF_POISON) == true)
		{
			// �ߵ��� ��� �ڵ�ȸ�� ����
			// �Ĺ����� ��� �ڵ�ȸ�� ����
			return 3;
		}
#ifdef ENABLE_WOLFMAN_CHARACTER
		if (ch->IsAffectFlag(AFF_BLEEDING))
			return 3;
#endif
		int iSec = (get_dword_time() - ch->GetLastMoveTime()) / 3000;

		// SP ȸ�� ��ƾ.
		// �� �̰ɷ� �ؼ� �Լ��� �����°� ?!
		ch->DistributeSP(ch);

		if (ch->GetMaxHP() <= ch->GetHP())
			return PASSES_PER_SEC(3);

		int iPercent = 0;
		int iAmount = 0;

		{
			iPercent = aiRecoveryPercents[MIN(9, iSec)];
			iAmount = 15 + (ch->GetMaxHP() * iPercent) / 100;
		}

		iAmount += (iAmount * ch->GetPoint(POINT_HP_REGEN)) / 100;

		sys_log(1, "RECOVERY_EVENT: %s %d HP_REGEN %d HP +%d", ch->GetName(), iPercent, ch->GetPoint(POINT_HP_REGEN), iAmount);

		ch->PointChange(POINT_HP, iAmount, false);
		return PASSES_PER_SEC(3);
	}
}

void CHARACTER::StartRecoveryEvent()
{
	if (m_pkRecoveryEvent)
		return;

	if (IsDead() || IsStun())
		return;

	if (IsNPC() && GetHP() >= GetMaxHP()) // ���ʹ� ü���� �� �������� ���� ���Ѵ�.
		return;


#ifdef ENABLE_MELEY_LAIR
	int32_t racenum = GetRaceNum();
	if (racenum == 6193 || racenum == 6118)
	{
		return;
	}
#endif

	char_event_info* info = AllocEventInfo<char_event_info>();

	info->ch = this;

	int iSec = IsPC() ? 3 : (MAX(1, GetMobTable().bRegenCycle));
	m_pkRecoveryEvent = event_create(recovery_event, info, PASSES_PER_SEC(iSec));
}

void CHARACTER::Standup()
{
	struct packet_position pack_position;

	if (!IsPosition(POS_SITTING))
		return;

	SetPosition(POS_STANDING);

	sys_log(1, "STANDUP: %s", GetName());

	pack_position.header	= HEADER_GC_CHARACTER_POSITION;
	pack_position.vid		= GetVID();
	pack_position.position	= POSITION_GENERAL;

	PacketAround(&pack_position, sizeof(pack_position));
}

void CHARACTER::Sitdown(int is_ground)
{
	struct packet_position pack_position;

	if (IsPosition(POS_SITTING))
		return;

	SetPosition(POS_SITTING);
	sys_log(1, "SITDOWN: %s", GetName());

	pack_position.header	= HEADER_GC_CHARACTER_POSITION;
	pack_position.vid		= GetVID();
	pack_position.position	= POSITION_SITTING_GROUND;
	PacketAround(&pack_position, sizeof(pack_position));
}

#ifdef ENABLE_ANCIENT_PYRAMID
void CHARACTER::SetRotation(float fRot, bool bForce)
#else
void CHARACTER::SetRotation(float fRot)
#endif
{
	if (!IsPC())
	{
		int32_t vnum = GetRaceNum();
#ifdef ENABLE_ANCIENT_PYRAMID
		if (vnum == PYRAMID_BOSSVNUM && (!bForce))
		{
			return;
		}
#endif

#ifdef __DEFENSE_WAVE__
		if (vnum >= 3960 && vnum <= 3962 && (!bForce))
		{
			return;
		}
#endif
	}

	m_pointsInstant.fRot = fRot;
}

// x, y �������� ���� ����.
void CHARACTER::SetRotationToXY(long x, long y)
{
	SetRotation(GetDegreeFromPositionXY(GetX(), GetY(), x, y));
}

bool CHARACTER::CannotMoveByAffect() const
{
	return (IsAffectFlag(AFF_STUN));
}

bool CHARACTER::CanMove() const
{
	if (CannotMoveByAffect())
		return false;

	if (GetMyShop())	// ���� �� ���¿����� ������ �� ����
		return false;

	// 0.2�� ���̶�� ������ �� ����.
	/*
	   if (get_float_time() - m_fSyncTime < 0.2f)
	   return false;
	 */
	return true;
}

// ������ x, y ��ġ�� �̵� ��Ų��.
bool CHARACTER::Sync(long x, long y)
{
	if (!GetSectree())
		return false;

	LPSECTREE new_tree = SECTREE_MANAGER::instance().Get(GetMapIndex(), x, y);

	if (!new_tree)
	{
		if (GetDesc())
		{
			sys_err("cannot find tree at %d %d (name: %s)", x, y, GetName());
			GetDesc()->SetPhase(PHASE_CLOSE);
		}
		else
		{
			sys_err("no tree: %s %d %d %d", GetName(), x, y, GetMapIndex());
			Dead();
		}

		return false;
	}

	SetRotationToXY(x, y);
	SetXYZ(x, y, 0);

	if (GetDungeon())
	{
		// ������ �̺�Ʈ �Ӽ� ��ȭ
		int iLastEventAttr = m_iEventAttr;
		m_iEventAttr = new_tree->GetEventAttribute(x, y);

		if (m_iEventAttr != iLastEventAttr)
		{
			if (GetParty())
			{
				quest::CQuestManager::instance().AttrOut(GetParty()->GetLeaderPID(), this, iLastEventAttr);
				quest::CQuestManager::instance().AttrIn(GetParty()->GetLeaderPID(), this, m_iEventAttr);
			}
			else
			{
				quest::CQuestManager::instance().AttrOut(GetPlayerID(), this, iLastEventAttr);
				quest::CQuestManager::instance().AttrIn(GetPlayerID(), this, m_iEventAttr);
			}
		}
	}

	if (GetSectree() != new_tree)
	{
		if (!IsNPC())
		{
			SECTREEID id = new_tree->GetID();
			SECTREEID old_id = GetSectree()->GetID();

			const float fDist = DISTANCE_SQRT(id.coord.x - old_id.coord.x, id.coord.y - old_id.coord.y);
			sys_log(0, "SECTREE DIFFER: %s %dx%d was %dx%d dist %.1fm",
					GetName(),
					id.coord.x,
					id.coord.y,
					old_id.coord.x,
					old_id.coord.y,
					fDist);
		}

		new_tree->InsertEntity(this);
	}

	return true;
}

void CHARACTER::Stop()
{
	if (!IsState(m_stateIdle))
		MonsterLog("[IDLE] ����");

	GotoState(m_stateIdle);

	m_posDest.x = m_posStart.x = GetX();
	m_posDest.y = m_posStart.y = GetY();
}

bool CHARACTER::Goto(long x, long y)
{
	// TODO �Ÿ�üũ �ʿ�
	// ���� ��ġ�� �̵��� �ʿ� ���� (�ڵ� ����)
	if (GetX() == x && GetY() == y)
		return false;

	if (!IsPC())
	{
		int32_t vnum = GetRaceNum();
#ifdef ENABLE_MELEY_LAIR
		if (vnum == 6193)
		{
			return false;
		}
#endif

#ifdef ENABLE_ANCIENT_PYRAMID
		if (vnum == PYRAMID_BOSSVNUM)
		{
			return false;
		}
#endif

#ifdef __DEFENSE_WAVE__
		if (vnum >= 3960 && vnum <= 3962)
		{
			return false;
		}
#endif
	}

	if (m_posDest.x == x && m_posDest.y == y)
	{
		if (!IsState(m_stateMove))
		{
			m_dwStateDuration = 4;
			GotoState(m_stateMove);
		}
		return false;
	}

	m_posDest.x = x;
	m_posDest.y = y;

	CalculateMoveDuration();

	m_dwStateDuration = 4;


	if (!IsState(m_stateMove)) {
		MonsterLog("[MOVE] %s", GetVictim() ? "�������" : "�׳��̵�");
	}

	GotoState(m_stateMove);

	return true;
}


DWORD CHARACTER::GetMotionMode() const
{
	DWORD dwMode = MOTION_MODE_GENERAL;

	if (IsPolymorphed())
		return dwMode;

	LPITEM pkItem;

	if ((pkItem = GetWear(WEAR_WEAPON)))
	{
		switch (pkItem->GetProto()->bSubType)
		{
			case WEAPON_SWORD:
				dwMode = MOTION_MODE_ONEHAND_SWORD;
				break;

			case WEAPON_TWO_HANDED:
				dwMode = MOTION_MODE_TWOHAND_SWORD;
				break;

			case WEAPON_DAGGER:
				dwMode = MOTION_MODE_DUALHAND_SWORD;
				break;

			case WEAPON_BOW:
				dwMode = MOTION_MODE_BOW;
				break;

			case WEAPON_BELL:
				dwMode = MOTION_MODE_BELL;
				break;

			case WEAPON_FAN:
				dwMode = MOTION_MODE_FAN;
				break;
#ifdef ENABLE_WOLFMAN_CHARACTER
			case WEAPON_CLAW:
				dwMode = MOTION_MODE_CLAW;
				break;
#endif
		}
	}
	return dwMode;
}

float CHARACTER::GetMoveMotionSpeed() const
{
	DWORD dwMode = GetMotionMode();
	if (!IsPC())
	{
		if (dwMode == 0)
		{
			int32_t vnum = GetRaceNum();
#ifdef ENABLE_MELEY_LAIR
			if (vnum == 6193)
			{
				return 100.0f;
			}
#endif

#ifdef ENABLE_ANCIENT_PYRAMID
			if (vnum == PYRAMID_BOSSVNUM)
			{
				return 100.0f;
			}
#endif

#ifdef __DEFENSE_WAVE__
			if (vnum >= 3960 && vnum <= 3962)
			{
				return 100.0f;
			}
#endif
		}
	}

	const CMotion * pkMotion = NULL;

	if (!GetMountVnum())
		pkMotion = CMotionManager::instance().GetMotion(GetRaceNum(), MAKE_MOTION_KEY(dwMode, (IsWalking() && IsPC()) ? MOTION_WALK : MOTION_RUN));
	else
	{
		pkMotion = CMotionManager::instance().GetMotion(GetMountVnum(), MAKE_MOTION_KEY(MOTION_MODE_GENERAL, (IsWalking() && IsPC()) ? MOTION_WALK : MOTION_RUN));

		if (!pkMotion)
			pkMotion = CMotionManager::instance().GetMotion(GetRaceNum(), MAKE_MOTION_KEY(MOTION_MODE_HORSE, (IsWalking() && IsPC()) ? MOTION_WALK : MOTION_RUN));
	}

	if (pkMotion)
		return -pkMotion->GetAccumVector().y / pkMotion->GetDuration();
	else
	{
		if (test_server) {
			sys_err("cannot find motion (name %s race %d mode %d)", GetName(), GetRaceNum(), dwMode);
		}

		return 300.0f;
	}
}

float CHARACTER::GetMoveSpeed() const
{
	return GetMoveMotionSpeed() * 10000 / CalculateDuration(GetLimitPoint(POINT_MOV_SPEED), 10000);
}

void CHARACTER::CalculateMoveDuration()
{
	m_posStart.x = GetX();
	m_posStart.y = GetY();

	float fDist = DISTANCE_SQRT(m_posStart.x - m_posDest.x, m_posStart.y - m_posDest.y);

	float motionSpeed = GetMoveMotionSpeed();

	m_dwMoveDuration = CalculateDuration(GetLimitPoint(POINT_MOV_SPEED),
			(int) ((fDist / motionSpeed) * 1000.0f));

	if (IsNPC())
		sys_log(1, "%s: GOTO: distance %f, spd %u, duration %u, motion speed %f pos %d %d -> %d %d",
				GetName(), fDist, GetLimitPoint(POINT_MOV_SPEED), m_dwMoveDuration, motionSpeed,
				m_posStart.x, m_posStart.y, m_posDest.x, m_posDest.y);

	m_dwMoveStartTime = get_dword_time();
}

// x y ��ġ�� �̵� �Ѵ�. (�̵��� �� �ִ� �� ���� ���� Ȯ�� �ϰ� Sync �޼ҵ�� ���� �̵� �Ѵ�)
// ������ char�� x, y ���� �ٷ� �ٲ�����,
// Ŭ�󿡼��� ���� ��ġ���� �ٲ� x, y���� interpolation�Ѵ�.
// �Ȱų� �ٴ� ���� char�� m_bNowWalking�� �޷��ִ�.
// Warp�� �ǵ��� ���̶�� Show�� ����� ��.
bool CHARACTER::Move(long x, long y)
{
	// ���� ��ġ�� �̵��� �ʿ� ���� (�ڵ� ����)
	if (GetX() == x && GetY() == y)
		return true;

	if (test_server)
		if (m_bDetailLog)
			sys_log(0, "%s position %u %u", GetName(), x, y);

	OnMove();
	return Sync(x, y);
}

void CHARACTER::SendMovePacket(BYTE bFunc, BYTE bArg, DWORD x, DWORD y, DWORD dwDuration, DWORD dwTime, int iRot)
{
	TPacketGCMove pack;

	if (bFunc == FUNC_WAIT)
	{
		x = m_posDest.x;
		y = m_posDest.y;
		dwDuration = m_dwMoveDuration;
	}

	EncodeMovePacket(pack, GetVID(), bFunc, bArg, x, y, dwDuration, dwTime, iRot == -1 ? (int) GetRotation() / 5 : iRot);
	PacketView(&pack, sizeof(TPacketGCMove), this);
}

int CHARACTER::GetRealPoint(BYTE type) const
{
	return m_points.points[type];
}

void CHARACTER::SetRealPoint(BYTE type, int val)
{
	m_points.points[type] = val;
#ifdef ENABLE_RANKING
	if (type == POINT_PLAYTIME)
		SetRankPoints(15, val);
#endif
}

int CHARACTER::GetPolymorphPoint(BYTE type) const
{
	if (IsPolymorphed() && !IsPolyMaintainStat())
	{
		DWORD dwMobVnum = GetPolymorphVnum();
		const CMob * pMob = CMobManager::instance().Get(dwMobVnum);
		int iPower = GetPolymorphPower();

		if (pMob)
		{
			switch (type)
			{
				case POINT_ST:
					if ((GetJob() == JOB_SHAMAN) || ((GetJob() == JOB_SURA) && (GetSkillGroup() == 2)))
						return pMob->m_table.bStr * iPower / 100 + GetPoint(POINT_IQ);
					return pMob->m_table.bStr * iPower / 100 + GetPoint(POINT_ST);

				case POINT_HT:
					return pMob->m_table.bCon * iPower / 100 + GetPoint(POINT_HT);

				case POINT_IQ:
					return pMob->m_table.bInt * iPower / 100 + GetPoint(POINT_IQ);

				case POINT_DX:
					return pMob->m_table.bDex * iPower / 100 + GetPoint(POINT_DX);
			}
		}
	}

	return GetPoint(type);
}
#ifdef ENABLE_LONG_LONG
long long CHARACTER::GetPoint(BYTE type) const
#else
int CHARACTER::GetPoint(BYTE type) const
#endif
{
	if (type >= POINT_MAX_NUM)
	{
		sys_err("Point type overflow (type %u)", type);
		return 0;
	}
#ifdef ENABLE_LONG_LONG
	long long val = m_pointsInstant.points[type];
	long long max_val = INT_MAX;
#else
	int val = m_pointsInstant.points[type];
	int max_val = INT_MAX;
#endif
	switch (type)
	{
		case POINT_STEAL_HP:
		case POINT_STEAL_SP:
			max_val = 50;
			break;
		case POINT_GOLD:
			max_val = GOLD_MAX;
			break;
	}

	if (val > max_val)
		sys_err("POINT_ERROR: %s type %d val %d (max: %d)", GetName(), type, val, max_val);

	return (val);
}

int CHARACTER::GetLimitPoint(BYTE type) const
{
	if (type >= POINT_MAX_NUM)
	{
		sys_err("Point type overflow (type %u)", type);
		return 0;
	}

	int val = m_pointsInstant.points[type];
	int max_val = INT_MAX;
	int limit = INT_MAX;
	int min_limit = -INT_MAX;

	switch (type)
	{
		case POINT_ATT_SPEED:
			min_limit = 0;

			if (IsPC())
				limit = 170;
			else
				limit = 250;
			break;

		case POINT_MOV_SPEED:
			min_limit = 0;
#ifdef ENABLE_NEW_MOVEMENT_SPEED
			limit = 350;
#else
			if (IsPC())
				limit = 200;
			else
				limit = 250;
#endif
			break;

		case POINT_STEAL_HP:
		case POINT_STEAL_SP:
			limit = 50;
			max_val = 50;
			break;

		case POINT_MALL_ATTBONUS:
		case POINT_MALL_DEFBONUS:
			limit = 20;
			max_val = 50;
			break;
	}

	if (val > max_val)
		sys_err("POINT_ERROR: %s type %d val %d (max: %d)", GetName(), type, val, max_val);

	if (val > limit)
		val = limit;

	if (val < min_limit)
		val = min_limit;

	return (val);
}
#ifdef ENABLE_LONG_LONG
void CHARACTER::SetPoint(BYTE type, long long val)
#else
void CHARACTER::SetPoint(BYTE type, int val)
#endif
{
	if (type >= POINT_MAX_NUM)
	{
		sys_err("Point type overflow (type %u)", type);
		return;
	}

	m_pointsInstant.points[type] = val;

	// ���� �̵��� �� �ȳ����ٸ� �̵� �ð� ����� �ٽ� �ؾ� �Ѵ�.
	if (type == POINT_MOV_SPEED && get_dword_time() < m_dwMoveStartTime + m_dwMoveDuration)
	{
		CalculateMoveDuration();
	}
}
#ifdef ENABLE_LONG_LONG
long long CHARACTER::GetAllowedGold() const
#else
INT CHARACTER::GetAllowedGold() const
#endif
{
	if (GetLevel() <= 10)
		return 100000;
	else if (GetLevel() <= 20)
		return 500000;
	else
		return 50000000;
}

void CHARACTER::CheckMaximumPoints()
{
	if (GetMaxHP() < GetHP())
		PointChange(POINT_HP, GetMaxHP() - GetHP());

	if (GetMaxSP() < GetSP())
		PointChange(POINT_SP, GetMaxSP() - GetSP());
}

#ifdef ENABLE_LONG_LONG
void CHARACTER::PointChange(BYTE type, long long amount, bool bAmount, bool bBroadcast
#ifdef __ENABLE_BLOCK_EXP__
, bool bForceExp
#endif
)
{
	long long val = 0;
#else
void CHARACTER::PointChange(BYTE type, int amount, bool bAmount, bool bBroadcast
#ifdef __ENABLE_BLOCK_EXP__
, bool bForceExp
#endif
)
{
	int val = 0;
#endif

	//sys_log(0, "PointChange %d %d | %d -> %d cHP %d mHP %d", type, amount, GetPoint(type), GetPoint(type)+amount, GetHP(), GetMaxHP());

	switch (type)
	{
		case POINT_NONE:

#ifdef ENABLE_BATTLE_PASS
		case POINT_BATTLE_PASS_ID:
#endif		

			return;

		case POINT_LEVEL:
			if ((GetLevel() + amount) > gPlayerMaxLevel)
				return;

			SetLevel(GetLevel() + amount);
			val = GetLevel();

			sys_log(0, "LEVELUP: %s %d NEXT EXP %d", GetName(), GetLevel(), GetNextExp());
#ifdef ENABLE_WOLFMAN_CHARACTER
			if (GetJob() == JOB_WOLFMAN)
			{
				if ((5 <= val) && (GetSkillGroup()!=1))
				{
					ClearSkill();
					// set skill group
					SetSkillGroup(1);
					// set skill points
					SetRealPoint(POINT_SKILL, GetLevel()-1);
					SetPoint(POINT_SKILL, GetRealPoint(POINT_SKILL));
					PointChange(POINT_SKILL, 0);
					// update points (not required)
					// ComputePoints();
					// PointsPacket();
				}
			}
#endif
			PointChange(POINT_NEXT_EXP,	GetNextExp(), false);
#ifdef ENABLE_ANNOUNCEMENT_LEVELUP
#ifdef TEXTS_IMPROVEMENT
			switch (val) {
				case 30:
				case 50:
				case 75:
				case 90:
				case 100:
				case 110:
				case 120:
					BroadcastNoticeNew(CHAT_TYPE_NOTICE, 0, 0, 546, "%s#%d", GetName(), val);
					break;
				default:
					break;
			}
#endif
#endif
			if (amount)
			{
				quest::CQuestManager::instance().LevelUp(GetPlayerID());

				LogManager::instance().LevelLog(this, val, GetRealPoint(POINT_PLAYTIME) + (get_dword_time() - m_dwPlayStartTime) / 60000);

				if (GetGuild())
				{
					GetGuild()->LevelChange(GetPlayerID(), GetLevel());
				}

				if (GetParty())
				{
					GetParty()->RequestSetMemberLevel(GetPlayerID(), GetLevel());
				}
			}
			break;

		case POINT_NEXT_EXP:
			val = GetNextExp();
			bAmount = false;	// ������ bAmount�� false ���� �Ѵ�.
			break;

		case POINT_EXP:
			{
				DWORD exp = GetExp();
				DWORD next_exp = GetNextExp();

				// exp�� 0 ���Ϸ� ���� �ʵ��� �Ѵ�
				if ((amount < 0) && (exp < (DWORD)(-amount)))
				{
					sys_log(1, "%s AMOUNT < 0 %d, CUR EXP: %d", GetName(), -amount, exp);
					amount = -exp;

					SetExp(exp + amount);
					val = GetExp();
				}
				else
				{
					if (gPlayerMaxLevel <= GetLevel())
						return;
					
#ifdef __ENABLE_BLOCK_EXP__
					if (Block_Exp && !bForceExp)
					{
						return;
					}
#endif
					
//#ifdef TEXTS_IMPROVEMENT
//
//					if (amount > 0) {
//						auto s = std::to_string(amount);
//						int n = s.length() - 3;
//						while (n > 0) {
//							s.insert(n, ".");
//							n -= 3;
//						}
//
//						ChatPacketNew(
//#ifdef ENABLE_NEW_CHAT
//						CHAT_TYPE_INFO_EXP
//#else
//						CHAT_TYPE_INFO
//#endif
//						, 2, "%s", s.c_str());
//					}
//#endif
					DWORD iExpBalance = 0;

					// ���� ��!
					if (exp + amount >= next_exp)
					{
						iExpBalance = (exp + amount) - next_exp;
						amount = next_exp - exp;

						SetExp(0);
						exp = next_exp;
					}
					else
					{
						SetExp(exp + amount);
						exp = GetExp();
					}

					DWORD q = DWORD(next_exp / 4.0f);
					int iLevStep = GetRealPoint(POINT_LEVEL_STEP);

					// iLevStep�� 4 �̻��̸� ������ �ö���� �ϹǷ� ���⿡ �� �� ���� ���̴�.
					if (iLevStep >= 4)
					{
						sys_err("%s LEVEL_STEP bigger than 4! (%d)", GetName(), iLevStep);
						iLevStep = 4;
					}
					
					if (exp >= next_exp && iLevStep < 4)
					{
						for (int i = 0; i < 4 - iLevStep; ++i)
							PointChange(POINT_LEVEL_STEP, 1, false, true);
					}
					else if (exp >= q * 3 && iLevStep < 3)
					{
						for (int i = 0; i < 3 - iLevStep; ++i)
							PointChange(POINT_LEVEL_STEP, 1, false, true);
					}
					else if (exp >= q * 2 && iLevStep < 2)
					{
						for (int i = 0; i < 2 - iLevStep; ++i)
							PointChange(POINT_LEVEL_STEP, 1, false, true);
					}
					else if (exp >= q && iLevStep < 1)
						PointChange(POINT_LEVEL_STEP, 1);
					
					if (iExpBalance)
					{
						PointChange(POINT_EXP, iExpBalance);
					}
					
					val = GetExp();
				}
			}
			break;

		case POINT_LEVEL_STEP:
			if (amount > 0)
			{
				val = GetPoint(POINT_LEVEL_STEP) + amount;

				switch (val)
				{
					case 1:
					case 2:
					case 3:
						{
							int iLvl = GetLevel();
#ifdef ENABLE_STATUS_MAX_344_POINTS
								if (iLvl > 115)
									break;
								else if ((iLvl == 115) && (val == 3))
									break;
								
								PointChange(POINT_STAT, 1);
#else
							if ((iLvl <= g_iStatusPointGetLevelLimit) && (iLvl <= gPlayerMaxLevel))
								PointChange(POINT_STAT, 1);
#endif
						}
						break;

					case 4:
						{
							int iHP = number(JobInitialPoints[GetJob()].hp_per_lv_begin, JobInitialPoints[GetJob()].hp_per_lv_end);
							int iSP = number(JobInitialPoints[GetJob()].sp_per_lv_begin, JobInitialPoints[GetJob()].sp_per_lv_end);

							m_points.iRandomHP += iHP;
							m_points.iRandomSP += iSP;

							if (GetSkillGroup())
							{
								if (GetLevel() >= 5)
									PointChange(POINT_SKILL, 1);

								if (GetLevel() >= 9)
									PointChange(POINT_SUB_SKILL, 1);
							}

							PointChange(POINT_MAX_HP, iHP);
							PointChange(POINT_MAX_SP, iSP);
							PointChange(POINT_LEVEL, 1, false, true);

							val = 0;
						}
						break;
				}

				PointChange(POINT_HP, GetMaxHP() - GetHP());
				PointChange(POINT_SP, GetMaxSP() - GetSP());
				PointChange(POINT_STAMINA, GetMaxStamina() - GetStamina());

				SetPoint(POINT_LEVEL_STEP, val);
				SetRealPoint(POINT_LEVEL_STEP, val);

				Save();
			}
			else
				val = GetPoint(POINT_LEVEL_STEP);

			break;

		case POINT_HP:
			{
				if (IsDead() || IsStun())
					return;

				int prev_hp = GetHP();

				amount = MIN(GetMaxHP() - GetHP(), amount);
				SetHP(GetHP() + amount);
				val = GetHP();

				BroadcastTargetPacket();

				if (GetParty() && IsPC() && val != prev_hp)
					GetParty()->SendPartyInfoOneToAll(this);
			}
			break;

		case POINT_SP:
			{
				if (IsDead() || IsStun())
					return;

				amount = MIN(GetMaxSP() - GetSP(), amount);
				SetSP(GetSP() + amount);
				val = GetSP();
			}
			break;

		case POINT_STAMINA:
			{
				if (IsDead() || IsStun())
					return;

				int prev_val = GetStamina();
				amount = MIN(GetMaxStamina() - GetStamina(), amount);
				SetStamina(GetStamina() + amount);
				val = GetStamina();

				if (val == 0)
				{
					// Stamina�� ������ ����!
					SetNowWalking(true);
				}
				else if (prev_val == 0)
				{
					// ���� ���׹̳��� �������� ���� ��� ����
					ResetWalking();
				}

				if (amount < 0 && val != 0) // ���Ҵ� �������ʴ´�.
					return;
			}
			break;

		case POINT_MAX_HP:
			{
				SetPoint(type, GetPoint(type) + amount);

				//SetMaxHP(GetMaxHP() + amount);
				// �ִ� ����� = (�⺻ �ִ� ����� + �߰�) * �ִ�����%
				int hp = GetRealPoint(POINT_MAX_HP);
				int add_hp = MIN(3500, hp * GetPoint(POINT_MAX_HP_PCT) / 100);
				add_hp += GetPoint(POINT_MAX_HP);
				add_hp += GetPoint(POINT_PARTY_TANKER_BONUS);

				SetMaxHP(hp + add_hp);

				val = GetMaxHP();
			}
			break;

		case POINT_MAX_SP:
			{
				SetPoint(type, GetPoint(type) + amount);

				//SetMaxSP(GetMaxSP() + amount);
				// �ִ� ���ŷ� = (�⺻ �ִ� ���ŷ� + �߰�) * �ִ����ŷ�%
				int sp = GetRealPoint(POINT_MAX_SP);
				int add_sp = MIN(800, sp * GetPoint(POINT_MAX_SP_PCT) / 100);
				add_sp += GetPoint(POINT_MAX_SP);
				add_sp += GetPoint(POINT_PARTY_SKILL_MASTER_BONUS);

				SetMaxSP(sp + add_sp);

				val = GetMaxSP();
			}
			break;
		case POINT_MAX_HP_PCT:
			SetPoint(type, GetPoint(type) + amount);
			val = GetPoint(type);
			PointChange(POINT_MAX_HP, 0);
			break;

		case POINT_MAX_SP_PCT:
			SetPoint(type, GetPoint(type) + amount);
			val = GetPoint(type);
			PointChange(POINT_MAX_SP, 0);
			break;

		case POINT_MAX_STAMINA:
			SetMaxStamina(GetMaxStamina() + amount);
			val = GetMaxStamina();
			break;


#ifdef __ENABLE_EXTEND_INVEN_SYSTEM__
		case POINT_INVEN:
			{
				const int64_t Envantertoplami = static_cast<int64_t>(Inven_Point()) + static_cast<int64_t>(amount);
				if (Envantertoplami > 18)
				{
					sys_err("[ENVANTER ERROR!]");
					return;
				}
				Set_Inventory_Point(Inven_Point() + amount);
				val = Inven_Point();
			}
			break;
#endif

		case POINT_GOLD:
			{
#ifdef ENABLE_LONG_LONG
				const long long nTotalMoney = static_cast<long long>(GetGold()) + static_cast<long long>(amount);
#else
				const int64_t nTotalMoney = static_cast<int64_t>(GetGold()) + static_cast<int64_t>(amount);
#endif
				if (GOLD_MAX <= nTotalMoney)
				{
#ifdef ENABLE_LONG_LONG
					sys_err("[OVERFLOW_GOLD] OriGold %d AddedGold %lld id %u Name %s ", GetGold(), amount, GetPlayerID(), GetName());
#else
					sys_err("[OVERFLOW_GOLD] OriGold %d AddedGold %d id %u Name %s ", GetGold(), amount, GetPlayerID(), GetName());
#endif
					LogManager::instance().CharLog(this, GetGold() + amount, "OVERFLOW_GOLD", "");
					return;
				}

//#ifdef TEXTS_IMPROVEMENT
//				if (amount > 0) {
//					auto s = std::to_string(amount);
//					int n = s.length() - 3;
//					while (n > 0) {
//						s.insert(n, ".");
//						n -= 3;
//					}
//
//					ChatPacketNew(
//#ifdef ENABLE_NEW_CHAT
//					CHAT_TYPE_INFO_VALUE
//#else
//					CHAT_TYPE_INFO
//#endif
//					, 3, "%s", s.c_str());
//				}
//#endif
				SetGold(GetGold() + amount);
				val = GetGold();
			}
			break;

#ifdef ENABLE_GAYA_SYSTEM
		case POINT_GAYA:
		{
			const int64_t nTotalGaya = static_cast<int64_t>(GetGaya()) + static_cast<int64_t>(amount);

			if (GAYA_MAX <= nTotalGaya)
			{
				sys_err("[OVERFLOW_GAYA] Gaya max seviyede %u Name %s ", GetGaya(), GetName());
				return;
			}

			if (nTotalGaya < 0)
			{
				sys_err("Gaya eksiye dusecekti. PID::[%d]", GetPlayerID());
				return;
			}

//#ifdef TEXTS_IMPROVEMENT
//			if (amount > 0) {
//				ChatPacketNew(
//#ifdef ENABLE_NEW_CHAT
//				CHAT_TYPE_INFO_VALUE
//#else
//				CHAT_TYPE_INFO
//#endif
//				, 4,
//#ifdef ENABLE_LONG_LONG
//				"%lld"
//#else
//				"%d"
//#endif
//				, amount);
//			}
//#endif
			
			SetGaya(GetGaya() + amount);
			val = GetGaya();
		}
		break;
#endif


		case POINT_SKILL:
		case POINT_STAT:
		case POINT_SUB_SKILL:
		case POINT_STAT_RESET_COUNT:
		case POINT_HORSE_SKILL:
			{
				long total = GetPoint(type) + amount;
#ifdef ENABLE_STATUS_MAX_344_POINTS
				if (type == POINT_STAT)
					total = total > 344 ? 344 : total;
#endif
				
				SetPoint(type, total);
				val = GetPoint(type);
				
				SetRealPoint(type, val);
			}
			break;
		case POINT_DEF_GRADE:
			SetPoint(type, GetPoint(type) + amount);
			val = GetPoint(type);

			PointChange(POINT_CLIENT_DEF_GRADE, amount);
			break;

		case POINT_CLIENT_DEF_GRADE:
			SetPoint(type, GetPoint(type) + amount);
			val = GetPoint(type);
			break;

		case POINT_ST:
		case POINT_HT:
		case POINT_DX:
		case POINT_IQ:
		case POINT_HP_REGEN:
		case POINT_SP_REGEN:
		case POINT_ATT_SPEED:
		case POINT_ATT_GRADE:
		case POINT_MOV_SPEED:
		case POINT_CASTING_SPEED:
		case POINT_MAGIC_ATT_GRADE:
		case POINT_MAGIC_DEF_GRADE:
		case POINT_BOW_DISTANCE:
		case POINT_HP_RECOVERY:
		case POINT_SP_RECOVERY:

		case POINT_ATTBONUS_HUMAN:	// 42 �ΰ����� ����
		case POINT_ATTBONUS_ANIMAL:	// 43 �������� ������ % ����
		case POINT_ATTBONUS_ORC:		// 44 ���Ϳ��� ������ % ����
		case POINT_ATTBONUS_MILGYO:	// 45 �б����� ������ % ����
		case POINT_ATTBONUS_UNDEAD:	// 46 ��ü���� ������ % ����
		case POINT_ATTBONUS_DEVIL:	// 47 ����(�Ǹ�)���� ������ % ����

		case POINT_ATTBONUS_MONSTER:
		case POINT_ATTBONUS_SURA:
		case POINT_ATTBONUS_ASSASSIN:
		case POINT_ATTBONUS_WARRIOR:
		case POINT_ATTBONUS_SHAMAN:
#ifdef ENABLE_WOLFMAN_CHARACTER
		case POINT_ATTBONUS_WOLFMAN:
#endif

		case POINT_POISON_PCT:
#ifdef ENABLE_WOLFMAN_CHARACTER
		case POINT_BLEEDING_PCT:
#endif
		case POINT_STUN_PCT:
		case POINT_SLOW_PCT:

		case POINT_BLOCK:
		case POINT_DODGE:

		case POINT_CRITICAL_PCT:
		case POINT_PVM_CRITICAL_PCT:
		case POINT_RESIST_CRITICAL:
		case POINT_PENETRATE_PCT:
		case POINT_RESIST_PENETRATE:
		case POINT_CURSE_PCT:

		case POINT_STEAL_HP:		// 48 ����� ���
		case POINT_STEAL_SP:		// 49 ���ŷ� ���

		case POINT_MANA_BURN_PCT:	// 50 ���� ��
		case POINT_DAMAGE_SP_RECOVER:	// 51 ���ݴ��� �� ���ŷ� ȸ�� Ȯ��
		case POINT_RESIST_NORMAL_DAMAGE:
		case POINT_RESIST_SWORD:
		case POINT_RESIST_TWOHAND:
		case POINT_RESIST_DAGGER:
		case POINT_RESIST_BELL:
		case POINT_RESIST_FAN:
		case POINT_RESIST_BOW:
#ifdef ENABLE_WOLFMAN_CHARACTER
		case POINT_RESIST_CLAW:
#endif
		case POINT_RESIST_FIRE:
		case POINT_RESIST_ELEC:
		case POINT_RESIST_MAGIC:
#ifdef ENABLE_ACCE_SYSTEM
		case POINT_ACCEDRAIN_RATE:
#endif
#ifdef ENABLE_MAGIC_REDUCTION_SYSTEM
		case POINT_RESIST_MAGIC_REDUCTION:
#endif
		case POINT_RESIST_WIND:
		case POINT_RESIST_ICE:
		case POINT_RESIST_EARTH:
		case POINT_RESIST_DARK:
		case POINT_REFLECT_MELEE:	// 67 ���� �ݻ�
		case POINT_REFLECT_CURSE:	// 68 ���� �ݻ�
		case POINT_POISON_REDUCE:	// 69 �������� ����
#ifdef ENABLE_WOLFMAN_CHARACTER
		case POINT_BLEEDING_REDUCE:
#endif
		case POINT_KILL_SP_RECOVER:	// 70 �� �Ҹ�� MP ȸ��
		case POINT_KILL_HP_RECOVERY:	// 75
		case POINT_HIT_HP_RECOVERY:
		case POINT_HIT_SP_RECOVERY:
		case POINT_MANASHIELD:
		case POINT_ATT_BONUS:
		case POINT_DEF_BONUS:
		case POINT_SKILL_DAMAGE_BONUS:
		case POINT_NORMAL_HIT_DAMAGE_BONUS:
		case POINT_SKILL_DEFEND_BONUS:
		case POINT_NORMAL_HIT_DEFEND_BONUS:
#ifdef ENABLE_DS_RUNE
		case POINT_RUNE_MONSTERS:
#endif
#ifdef ENABLE_NEW_COMMON_BONUSES
		case POINT_DOUBLE_DROP_ITEM:
		case POINT_IRR_WEAPON_DEFENSE:
#endif
			SetPoint(type, GetPoint(type) + amount);
			val = GetPoint(type);
			break;
#ifdef ELEMENT_NEW_BONUSES
		case POINT_ATTBONUS_ELEC:
		case POINT_ATTBONUS_FIRE:
		case POINT_ATTBONUS_ICE:
		case POINT_ATTBONUS_WIND:
		case POINT_ATTBONUS_EARTH:
		case POINT_ATTBONUS_DARK:
#ifdef ENABLE_NEW_BONUS_TALISMAN
		case POINT_ATTBONUS_IRR_SPADA:
		case POINT_ATTBONUS_IRR_SPADONE:	
		case POINT_ATTBONUS_IRR_PUGNALE:		
		case POINT_ATTBONUS_IRR_FRECCIA:	
		case POINT_ATTBONUS_IRR_VENTAGLIO:	
		case POINT_ATTBONUS_IRR_CAMPANA:		
		case POINT_RESIST_MEZZIUOMINI:		
		case POINT_DEF_TALISMAN:
		case POINT_ATTBONUS_INSECT:
		case POINT_ATTBONUS_DESERT:
		case POINT_ATTBONUS_FORT_ZODIAC:
#endif
		case POINT_FISHING_RARE:
#ifdef ENABLE_NEW_USE_POTION
		case POINT_PARTY_DROPEXP:
#endif
			SetPoint(type, GetPoint(type) + amount);
			val = GetPoint(type);
			break;

#endif
#ifdef ENABLE_STRONG_METIN
		case POINT_ATTBONUS_METIN:
#endif
#ifdef ENABLE_STRONG_BOSS
		case POINT_ATTBONUS_BOSS:
#endif
#ifdef ENABLE_RESIST_MONSTER
		case POINT_RESIST_MONSTER:
#endif
#ifdef ENABLE_MEDI_PVM
		case POINT_ATTBONUS_MEDI_PVM:
#endif

		case POINT_PARTY_ATTACKER_BONUS:
		case POINT_PARTY_TANKER_BONUS:
		case POINT_PARTY_BUFFER_BONUS:
		case POINT_PARTY_SKILL_MASTER_BONUS:
		case POINT_PARTY_HASTE_BONUS:
		case POINT_PARTY_DEFENDER_BONUS:

		case POINT_RESIST_WARRIOR :
		case POINT_RESIST_ASSASSIN :
		case POINT_RESIST_SURA :
		case POINT_RESIST_SHAMAN :
#ifdef ENABLE_WOLFMAN_CHARACTER
		case POINT_RESIST_WOLFMAN :
#endif
			SetPoint(type, GetPoint(type) + amount);
			val = GetPoint(type);
			break;
		case POINT_MALL_EXPBONUS:
		case POINT_MALL_ITEMBONUS:
		case POINT_MALL_GOLDBONUS:
		case POINT_MALL_ATTBONUS:
		case POINT_MALL_DEFBONUS:
		case POINT_MELEE_MAGIC_ATT_BONUS_PER:
			if (GetPoint(type) + amount > 100)
			{
				if (type != POINT_MALL_EXPBONUS && type != POINT_MALL_ITEMBONUS) {
					sys_err("MALL_BONUS exceeded over 100!! point type: %d name: %s amount %d", type, GetName(), amount);
				}

				amount = 100 - GetPoint(type);
			}

			SetPoint(type, GetPoint(type) + amount);
			val = GetPoint(type);
			break;

			// PC_BANG_ITEM_ADD
		case POINT_PC_BANG_EXP_BONUS :
		case POINT_PC_BANG_DROP_BONUS :
		case POINT_RAMADAN_CANDY_BONUS_EXP:
#ifdef ENABLE_LOCKED_EXTRA_INVENTORY
		case POINT_EXTRA_INVENTORY1:
		case POINT_EXTRA_INVENTORY2:
		case POINT_EXTRA_INVENTORY3:
		case POINT_EXTRA_INVENTORY4:
		case POINT_EXTRA_INVENTORY5:
#endif
			SetPoint(type, amount);
			val = GetPoint(type);
			break;
			// END_PC_BANG_ITEM_ADD

		case POINT_EXP_DOUBLE_BONUS:	// 71
		case POINT_GOLD_DOUBLE_BONUS:	// 72
		case POINT_ITEM_DROP_BONUS:	// 73
		case POINT_POTION_BONUS:	// 74
			if (GetPoint(type) + amount > 300)
			{
				if (type != POINT_EXP_DOUBLE_BONUS && type != POINT_GOLD_DOUBLE_BONUS) {
					sys_err("BONUS exceeded over 300!! point type: %d name: %s amount %d", type, GetName(), amount);
				}

				amount = 300 - GetPoint(type);
			}

			SetPoint(type, GetPoint(type) + amount);
			val = GetPoint(type);
			break;

		case POINT_IMMUNE_STUN:		// 76
			SetPoint(type, GetPoint(type) + amount);
			val = GetPoint(type);
			if (val)
			{
				SET_BIT(m_pointsInstant.dwImmuneFlag, IMMUNE_STUN);
			}
			else
			{
				REMOVE_BIT(m_pointsInstant.dwImmuneFlag, IMMUNE_STUN);
			}
			break;

		case POINT_IMMUNE_SLOW:		// 77
			SetPoint(type, GetPoint(type) + amount);
			val = GetPoint(type);
			if (val)
			{
				SET_BIT(m_pointsInstant.dwImmuneFlag, IMMUNE_SLOW);
			}
			else
			{
				REMOVE_BIT(m_pointsInstant.dwImmuneFlag, IMMUNE_SLOW);
			}
			break;

		case POINT_IMMUNE_FALL:	// 78
			SetPoint(type, GetPoint(type) + amount);
			val = GetPoint(type);
			if (val)
			{
				SET_BIT(m_pointsInstant.dwImmuneFlag, IMMUNE_FALL);
			}
			else
			{
				REMOVE_BIT(m_pointsInstant.dwImmuneFlag, IMMUNE_FALL);
			}
			break;

		case POINT_ATT_GRADE_BONUS:
			SetPoint(type, GetPoint(type) + amount);
			PointChange(POINT_ATT_GRADE, amount);
			val = GetPoint(type);
			break;

		case POINT_DEF_GRADE_BONUS:
			SetPoint(type, GetPoint(type) + amount);
			PointChange(POINT_DEF_GRADE, amount);
			val = GetPoint(type);
			break;

		case POINT_MAGIC_ATT_GRADE_BONUS:
			SetPoint(type, GetPoint(type) + amount);
			PointChange(POINT_MAGIC_ATT_GRADE, amount);
			val = GetPoint(type);
			break;

		case POINT_MAGIC_DEF_GRADE_BONUS:
			SetPoint(type, GetPoint(type) + amount);
			PointChange(POINT_MAGIC_DEF_GRADE, amount);
			val = GetPoint(type);
			break;

		case POINT_VOICE:
		case POINT_EMPIRE_POINT:
			//sys_err("CHARACTER::PointChange: %s: point cannot be changed. use SetPoint instead (type: %d)", GetName(), type);
			val = GetRealPoint(type);
			break;

		case POINT_POLYMORPH:
			SetPoint(type, GetPoint(type) + amount);
			val = GetPoint(type);
			SetPolymorph(val);
			break;

		case POINT_MOUNT:
			SetPoint(type, GetPoint(type) + amount);
			val = GetPoint(type);
			MountVnum(val);
			break;

		case POINT_ENERGY:
		case POINT_COSTUME_ATTR_BONUS:
			{
				int old_val = GetPoint(type);
				SetPoint(type, old_val + amount);
				val = GetPoint(type);
				BuffOnAttr_ValueChange(type, old_val, val);
			}
			break;

		default:
			sys_err("CHARACTER::PointChange: %s: unknown point change type %d", GetName(), type);
			return;
	}

	switch (type)
	{
		case POINT_LEVEL:
		case POINT_ST:
		case POINT_DX:
		case POINT_IQ:
		case POINT_HT:
			ComputeBattlePoints();
			break;
		case POINT_MAX_HP:
		case POINT_MAX_SP:
		case POINT_MAX_STAMINA:
			break;
	}

	if (type == POINT_HP && amount == 0)
		return;

	if (GetDesc())
	{
		struct packet_point_change pack;

		pack.header = HEADER_GC_CHARACTER_POINT_CHANGE;
		pack.dwVID = m_vid;
		pack.type = type;
		pack.value = val;

		if (bAmount)
			pack.amount = amount;
		else
			pack.amount = 0;

		if (!bBroadcast)
			GetDesc()->Packet(&pack, sizeof(struct packet_point_change));
		else
			PacketAround(&pack, sizeof(pack));
	}
}

#ifdef __NEWPET_SYSTEM__
void CHARACTER::SendPetLevelUpEffect(int vid, int type, int value, int amount) {
	struct packet_point_change pack;

	pack.header = HEADER_GC_CHARACTER_POINT_CHANGE;
	pack.dwVID = vid;
	pack.type = type;
	pack.value = value;
	pack.amount = amount;
	PacketAround(&pack, sizeof(pack));
}
#endif

void CHARACTER::ApplyPoint(BYTE bApplyType, int iVal)
{
	switch (bApplyType)
	{
		case APPLY_NONE:			// 0
			break;;

		case APPLY_CON:
			PointChange(POINT_HT, iVal);
			PointChange(POINT_MAX_HP, (iVal * JobInitialPoints[GetJob()].hp_per_ht));
			PointChange(POINT_MAX_STAMINA, (iVal * JobInitialPoints[GetJob()].stamina_per_con));
			break;

		case APPLY_INT:
			PointChange(POINT_IQ, iVal);
			PointChange(POINT_MAX_SP, (iVal * JobInitialPoints[GetJob()].sp_per_iq));
			break;

		case APPLY_SKILL:
			// SKILL_DAMAGE_BONUS
			{
				// �ֻ��� ��Ʈ �������� 8��Ʈ vnum, 9��Ʈ add, 15��Ʈ change
				// 00000000 00000000 00000000 00000000
				// ^^^^^^^^  ^^^^^^^^^^^^^^^^^^^^^^^^^
				// vnum     ^ add       change
				BYTE bSkillVnum = (BYTE) (((DWORD)iVal) >> 24);
				int iAdd = iVal & 0x00800000;
				int iChange = iVal & 0x007fffff;

				sys_log(1, "APPLY_SKILL skill %d add? %d change %d", bSkillVnum, iAdd ? 1 : 0, iChange);

				if (0 == iAdd)
					iChange = -iChange;

				boost::unordered_map<BYTE, int>::iterator iter = m_SkillDamageBonus.find(bSkillVnum);

				if (iter == m_SkillDamageBonus.end())
					m_SkillDamageBonus.insert(std::make_pair(bSkillVnum, iChange));
				else
					iter->second += iChange;
			}
			// END_OF_SKILL_DAMAGE_BONUS
			break;

		// NOTE: �����ۿ� ���� �ִ�HP ���ʽ��� ����Ʈ ���� ���ʽ��� �Ȱ��� ����� ����ϹǷ�
		// �׳� MAX_HP�� ����ϸ� ����Ʈ ������ ��� ������ ����. ��� ���� ������ �ո����̱⵵ �ϰ�..
		// �ٲ� ������ ���� �ִ� hp�� ���� hp�� ������ ���� �� �ٲ� �ִ� hp�� �������� hp�� �����Ѵ�.
		// ���� PointChange���� �ϴ°� ������ ������ ���� ������ ������� skip..
		// SP�� �Ȱ��� ����Ѵ�.
		// Mantis : 101460			~ ity ~
		case APPLY_MAX_HP:
		case APPLY_MAX_HP_PCT:
			{
				int i = GetMaxHP();
				if (i == 0) {
					break;
				}

				PointChange(aApplyInfo[bApplyType].bPointType, iVal);
				float fRatio = (float)GetMaxHP() / (float)i;
				PointChange(POINT_HP, GetHP() * fRatio - GetHP());
			}
			break;
		case APPLY_MAX_SP:
		case APPLY_MAX_SP_PCT:
			{
				int i = GetMaxSP();
				if (i == 0) {
					break;
				}

				PointChange(aApplyInfo[bApplyType].bPointType, iVal);
				float fRatio = (float)GetMaxSP() / (float)i;
				PointChange(POINT_SP, GetSP() * fRatio - GetSP());
			}
			break;
		case APPLY_STR:
		case APPLY_DEX:
		case APPLY_ATT_SPEED:
		case APPLY_MOV_SPEED:
		case APPLY_CAST_SPEED:
		case APPLY_HP_REGEN:
		case APPLY_SP_REGEN:
		case APPLY_POISON_PCT:
#ifdef ENABLE_WOLFMAN_CHARACTER
		case APPLY_BLEEDING_PCT:
#endif
		case APPLY_STUN_PCT:
		case APPLY_SLOW_PCT:
		case APPLY_CRITICAL_PCT:
		case APPLY_PENETRATE_PCT:
		case APPLY_ATTBONUS_HUMAN:
		case APPLY_ATTBONUS_ANIMAL:
		case APPLY_ATTBONUS_ORC:
		case APPLY_ATTBONUS_MILGYO:
		case APPLY_ATTBONUS_UNDEAD:
		case APPLY_ATTBONUS_DEVIL:
		case APPLY_ATTBONUS_WARRIOR:	// 59
		case APPLY_ATTBONUS_ASSASSIN:	// 60
		case APPLY_ATTBONUS_SURA:	// 61
		case APPLY_ATTBONUS_SHAMAN:	// 62
#ifdef ENABLE_WOLFMAN_CHARACTER
		case APPLY_ATTBONUS_WOLFMAN:
#endif
		case APPLY_ATTBONUS_MONSTER:	// 63
		case APPLY_STEAL_HP:
		case APPLY_STEAL_SP:
		case APPLY_MANA_BURN_PCT:
		case APPLY_DAMAGE_SP_RECOVER:
		case APPLY_BLOCK:
		case APPLY_DODGE:
		case APPLY_RESIST_SWORD:
		case APPLY_RESIST_TWOHAND:
		case APPLY_RESIST_DAGGER:
		case APPLY_RESIST_BELL:
		case APPLY_RESIST_FAN:
		case APPLY_RESIST_BOW:
#ifdef ENABLE_WOLFMAN_CHARACTER
		case APPLY_RESIST_CLAW:
#endif
		case APPLY_RESIST_FIRE:
		case APPLY_RESIST_ELEC:
		case APPLY_RESIST_MAGIC:
		case APPLY_RESIST_WIND:
		case APPLY_RESIST_ICE:
		case APPLY_RESIST_EARTH:
		case APPLY_RESIST_DARK:
		case APPLY_REFLECT_MELEE:
		case APPLY_REFLECT_CURSE:
		case APPLY_ANTI_CRITICAL_PCT:
		case APPLY_ANTI_PENETRATE_PCT:
		case APPLY_POISON_REDUCE:
#ifdef ENABLE_WOLFMAN_CHARACTER
		case APPLY_BLEEDING_REDUCE:
#endif
		case APPLY_KILL_SP_RECOVER:
		case APPLY_EXP_DOUBLE_BONUS:
		case APPLY_GOLD_DOUBLE_BONUS:
		case APPLY_ITEM_DROP_BONUS:
		case APPLY_POTION_BONUS:
		case APPLY_KILL_HP_RECOVER:
		case APPLY_IMMUNE_STUN:
		case APPLY_IMMUNE_SLOW:
		case APPLY_IMMUNE_FALL:
		case APPLY_BOW_DISTANCE:
		case APPLY_ATT_GRADE_BONUS:
		case APPLY_DEF_GRADE_BONUS:
		case APPLY_MAGIC_ATT_GRADE:
		case APPLY_MAGIC_DEF_GRADE:
		case APPLY_CURSE_PCT:
		case APPLY_MAX_STAMINA:
		case APPLY_MALL_ATTBONUS:
		case APPLY_MALL_DEFBONUS:
		case APPLY_MALL_EXPBONUS:
		case APPLY_MALL_ITEMBONUS:
		case APPLY_MALL_GOLDBONUS:
		case APPLY_SKILL_DAMAGE_BONUS:
		case APPLY_NORMAL_HIT_DAMAGE_BONUS:

			// DEPEND_BONUS_ATTRIBUTES
		case APPLY_SKILL_DEFEND_BONUS:
		case APPLY_NORMAL_HIT_DEFEND_BONUS:
			// END_OF_DEPEND_BONUS_ATTRIBUTES

		case APPLY_PC_BANG_EXP_BONUS :
		case APPLY_PC_BANG_DROP_BONUS :

		case APPLY_RESIST_WARRIOR :
		case APPLY_RESIST_ASSASSIN :
		case APPLY_RESIST_SURA :
		case APPLY_RESIST_SHAMAN :
#ifdef ENABLE_WOLFMAN_CHARACTER
		case APPLY_RESIST_WOLFMAN :
#endif
		case APPLY_ENERGY:					// 82 ���
		case APPLY_DEF_GRADE:				// 83 ����. DEF_GRADE_BONUS�� Ŭ�󿡼� �ι�� �������� �ǵ��� ����(...)�� �ִ�.
		case APPLY_COSTUME_ATTR_BONUS:		// 84 �ڽ�Ƭ �����ۿ� ���� �Ӽ�ġ ���ʽ�
		case APPLY_MAGIC_ATTBONUS_PER:		// 85 ���� ���ݷ� +x%
		case APPLY_MELEE_MAGIC_ATTBONUS_PER:			// 86 ���� + �и� ���ݷ� +x%
#ifdef ENABLE_ACCE_SYSTEM
		case APPLY_ACCEDRAIN_RATE:			//97
#endif
#ifdef ENABLE_MAGIC_REDUCTION_SYSTEM
		case APPLY_RESIST_MAGIC_REDUCTION:	//98
#endif
#ifdef ELEMENT_NEW_BONUSES
		case APPLY_ATTBONUS_ELEC:			//99
		case APPLY_ATTBONUS_FIRE:			//100
		case APPLY_ATTBONUS_ICE:			//101
		case APPLY_ATTBONUS_WIND:			//102
		case APPLY_ATTBONUS_EARTH:			//103
		case APPLY_ATTBONUS_DARK:			//104
#ifdef ENABLE_NEW_BONUS_TALISMAN
		case APPLY_ATTBONUS_IRR_SPADA:
		case APPLY_ATTBONUS_IRR_SPADONE:	
		case APPLY_ATTBONUS_IRR_PUGNALE:		
		case APPLY_ATTBONUS_IRR_FRECCIA:	
		case APPLY_ATTBONUS_IRR_VENTAGLIO:	
		case APPLY_ATTBONUS_IRR_CAMPANA:		
		case APPLY_RESIST_MEZZIUOMINI:		
		case APPLY_DEF_TALISMAN:
		case APPLY_ATTBONUS_INSECT:
		case APPLY_ATTBONUS_DESERT:
		case APPLY_ATTBONUS_FORT_ZODIAC:
#endif		
#endif
#ifdef ENABLE_STRONG_METIN
		case APPLY_ATTBONUS_METIN:
#endif
#ifdef ENABLE_STRONG_BOSS
		case APPLY_ATTBONUS_BOSS:
#endif
#ifdef ENABLE_RESIST_MONSTER
		case APPLY_RESIST_MONSTER:
#endif
#ifdef ENABLE_MEDI_PVM
		case APPLY_ATTBONUS_MEDI_PVM:
#endif
		case APPLY_PVM_CRITICAL_PCT:
#ifdef ENABLE_DS_RUNE
		case APPLY_RUNE_MONSTERS:
#endif
#ifdef ENABLE_NEW_COMMON_BONUSES
		case APPLY_DOUBLE_DROP_ITEM:
		case APPLY_IRR_WEAPON_DEFENSE:
#endif
		case APPLY_FISHING_RARE:
#ifdef ENABLE_NEW_USE_POTION
		case APPLY_PARTY_DROPEXP:
#endif
			PointChange(aApplyInfo[bApplyType].bPointType, iVal);
			break;

		default:
			sys_err("Unknown apply type %d name %s", bApplyType, GetName());
			break;
	}
}

void CHARACTER::MotionPacketEncode(BYTE motion, LPCHARACTER victim, struct packet_motion * packet)
{
	packet->header	= HEADER_GC_MOTION;
	packet->vid		= m_vid;
	packet->motion	= motion;

	if (victim)
		packet->victim_vid = victim->GetVID();
	else
		packet->victim_vid = 0;
}

void CHARACTER::Motion(BYTE motion, LPCHARACTER victim)
{
	struct packet_motion pack_motion;
	MotionPacketEncode(motion, victim, &pack_motion);
	PacketAround(&pack_motion, sizeof(struct packet_motion));
}

EVENTFUNC(save_event)
{
	char_event_info* info = dynamic_cast<char_event_info*>( event->info );
	if ( info == NULL )
	{
		sys_err( "save_event> <Factor> Null pointer" );
		return 0;
	}

	LPCHARACTER ch = info->ch;

	if (ch == NULL) { // <Factor>
		return 0;
	}
	sys_log(1, "SAVE_EVENT: %s", ch->GetName());
	ch->Save();
	ch->FlushDelayedSaveItem();
	return (save_event_second_cycle);
}

void CHARACTER::StartSaveEvent()
{
	if (m_pkSaveEvent)
		return;

	char_event_info* info = AllocEventInfo<char_event_info>();

	info->ch = this;
	m_pkSaveEvent = event_create(save_event, info, save_event_second_cycle);
}

void CHARACTER::MonsterLog(const char* format, ...)
{
	if (!test_server)
		return;

	if (IsPC())
		return;

	char chatbuf[CHAT_MAX_LEN + 1];
	int len = snprintf(chatbuf, sizeof(chatbuf), "%u)", (DWORD)GetVID());

	if (len < 0 || len >= (int) sizeof(chatbuf))
		len = sizeof(chatbuf) - 1;

	va_list args;

	va_start(args, format);

	int len2 = vsnprintf(chatbuf + len, sizeof(chatbuf) - len, format, args);

	if (len2 < 0 || len2 >= (int) sizeof(chatbuf) - len)
		len += (sizeof(chatbuf) - len) - 1;
	else
		len += len2;

	// \0 ���� ����
	++len;

	va_end(args);

	TPacketGCChat pack_chat;

	pack_chat.header    = HEADER_GC_CHAT;
	pack_chat.size		= sizeof(TPacketGCChat) + len;
	pack_chat.type      = CHAT_TYPE_TALKING;
	pack_chat.id        = (DWORD)GetVID();
	pack_chat.bEmpire	= 0;

	TEMP_BUFFER buf;
	buf.write(&pack_chat, sizeof(TPacketGCChat));
	buf.write(chatbuf, len);

	CHARACTER_MANAGER::instance().PacketMonsterLog(this, buf.read_peek(), buf.size());
}

void CHARACTER::ChatPacket(BYTE type, const char * format, ...)
{
	LPDESC d = GetDesc();
	if (!d || !format)
		return;

	char chatbuf[CHAT_MAX_LEN + 1];
	va_list args;
	va_start(args, format);
	int len = vsnprintf(chatbuf, sizeof(chatbuf), format, args);
	va_end(args);

	struct packet_chat pack_chat;

	pack_chat.header    = HEADER_GC_CHAT;
	pack_chat.size      = sizeof(struct packet_chat) + len;
	pack_chat.type      = type;
	pack_chat.id        = 0;
	pack_chat.bEmpire   = d->GetEmpire();

	TEMP_BUFFER buf;
	buf.write(&pack_chat, sizeof(struct packet_chat));
	buf.write(chatbuf, len);

	d->Packet(buf.read_peek(), buf.size());

	if (type == CHAT_TYPE_COMMAND && test_server)
		sys_log(0, "SEND_COMMAND %s %s", GetName(), chatbuf);
}

// MINING
void CHARACTER::mining_take()
{
	m_pkMiningEvent = NULL;
}

void CHARACTER::mining_cancel()
{
	if (m_pkMiningEvent)
	{
		sys_log(0, "XXX MINING CANCEL");
		event_cancel(&m_pkMiningEvent);
#ifdef TEXTS_IMPROVEMENT
		ChatPacketNew(CHAT_TYPE_INFO, 472, "");
#endif
	}
}

void CHARACTER::mining(LPCHARACTER chLoad)
{
	if (m_pkMiningEvent)
	{
		mining_cancel();
		return;
	}

	if (!chLoad)
		return;

	// @fixme128
	if (GetMapIndex() != chLoad->GetMapIndex() || DISTANCE_APPROX(GetX() - chLoad->GetX(), GetY() - chLoad->GetY()) > 1000)
		return;

	if (mining::GetRawOreFromLoad(chLoad->GetRaceNum()) == 0)
		return;

	LPITEM pick = GetWear(WEAR_WEAPON);

	if (!pick || pick->GetType() != ITEM_PICK)
	{
#ifdef TEXTS_IMPROVEMENT
		ChatPacketNew(CHAT_TYPE_INFO, 252, "");
#endif
		return;
	}

	int count = number(5, 15); // ���� Ƚ��, �� ���۴� 2��

	// ä�� ������ ������
	TPacketGCDigMotion p;
	p.header = HEADER_GC_DIG_MOTION;
	p.vid = GetVID();
	p.target_vid = chLoad->GetVID();
	p.count = count;

	PacketAround(&p, sizeof(p));

	m_pkMiningEvent = mining::CreateMiningEvent(this, chLoad, count);
}
// END_OF_MINING

void CHARACTER::fishing()
{
	if (m_pkFishingEvent)
	{
		fishing_take();
		return;
	}

	// ���� �Ӽ����� ���ø� �õ��Ѵ�?
	{
		LPSECTREE_MAP pkSectreeMap = SECTREE_MANAGER::instance().GetMap(GetMapIndex());

		int	x = GetX();
		int y = GetY();

		LPSECTREE tree = pkSectreeMap->Find(x, y);
		DWORD dwAttr = tree->GetAttribute(x, y);

		if (IS_SET(dwAttr, ATTR_BLOCK))
		{
#ifdef TEXTS_IMPROVEMENT
			ChatPacketNew(CHAT_TYPE_INFO, 657, "");
#endif
			return;
		}
	}

	LPITEM rod = GetWear(WEAR_WEAPON);

	// ���ô� ����
	if (!rod || rod->GetType() != ITEM_ROD)
	{
#ifdef TEXTS_IMPROVEMENT
		ChatPacketNew(CHAT_TYPE_INFO, 281, "");
#endif
		return;
	}

	if (0 == rod->GetSocket(2))
	{
#ifdef TEXTS_IMPROVEMENT
		ChatPacketNew(CHAT_TYPE_INFO, 351, "");
#endif
		return;
	}

	float fx, fy;
	GetDeltaByDegree(GetRotation(), 400.0f, &fx, &fy);

	m_pkFishingEvent = fishing::CreateFishingEvent(this);
}

void CHARACTER::fishing_take()
{
	LPITEM rod = GetWear(WEAR_WEAPON);
	if (rod && rod->GetType() == ITEM_ROD)
	{
		using fishing::fishing_event_info;
		if (m_pkFishingEvent)
		{
			struct fishing_event_info* info = dynamic_cast<struct fishing_event_info*>(m_pkFishingEvent->info);

			if (info)
				fishing::Take(info, this);
		}
	}
#ifdef TEXTS_IMPROVEMENT
	else {
		ChatPacketNew(CHAT_TYPE_INFO, 278, "");
	}
#endif

	event_cancel(&m_pkFishingEvent);
}

bool CHARACTER::StartStateMachine(int iNextPulse)
{
	if (CHARACTER_MANAGER::instance().AddToStateList(this))
	{
		m_dwNextStatePulse = thecore_heart->pulse + iNextPulse;
		return true;
	}

	return false;
}

void CHARACTER::StopStateMachine()
{
	CHARACTER_MANAGER::instance().RemoveFromStateList(this);
}

void CHARACTER::UpdateStateMachine(DWORD dwPulse)
{
	if (dwPulse < m_dwNextStatePulse)
		return;

	if (IsDead())
		return;

	Update();
	m_dwNextStatePulse = dwPulse + m_dwStateDuration;
}

void CHARACTER::SetNextStatePulse(int iNextPulse)
{
	CHARACTER_MANAGER::instance().AddToStateList(this);
	m_dwNextStatePulse = iNextPulse;

	if (iNextPulse < 10)
		MonsterLog("�������·ξ����");
}


// ĳ���� �ν��Ͻ� ������Ʈ �Լ�.
void CHARACTER::UpdateCharacter(DWORD dwPulse)
{
	CFSM::Update();
}

void CHARACTER::SetShop(LPSHOP pkShop)
{
	if ((m_pkShop = pkShop)) {
		SET_BIT(m_pointsInstant.instant_flag, INSTANT_FLAG_SHOP);
	}
	else
	{
		REMOVE_BIT(m_pointsInstant.instant_flag, INSTANT_FLAG_SHOP);
		SetShopOwner(NULL);
	}
}

void CHARACTER::SetExchange(CExchange * pkExchange)
{
	m_pkExchange = pkExchange;
}

void CHARACTER::SetPart(BYTE bPartPos, WORD wVal)
{
	assert(bPartPos < PART_MAX_NUM);
	m_pointsInstant.parts[bPartPos] = wVal;
}

WORD CHARACTER::GetPart(BYTE bPartPos) const
{
	assert(bPartPos < PART_MAX_NUM);
	
#ifdef __HIDE_COSTUME_SYSTEM__
	if (bPartPos == PART_MAIN && GetWear(WEAR_COSTUME_BODY) && IsBodyCostumeHidden() == true) {
		if (const LPITEM pArmor = GetWear(WEAR_BODY))
#ifdef __CHANGE_LOOK_SYSTEM__
			return pArmor->GetTransmutation() != 0 ? pArmor->GetTransmutation() : pArmor->GetVnum();
#else
			return pArmor->GetVnum();
#endif
		else
			return 0;
	}
	else if (bPartPos == PART_HAIR && GetWear(WEAR_COSTUME_HAIR) && IsHairCostumeHidden() == true)
		return 0;
#ifdef ENABLE_STOLE_COSTUME
	else if (bPartPos == PART_ACCE && GetWear(WEAR_COSTUME_ACCE) && IsAcceCostumeHidden() == true) {
		LPITEM pAcce = GetWear(WEAR_COSTUME_ACCE_SLOT);
		if (pAcce) {
			DWORD toSetValue = pAcce->GetVnum();
			toSetValue -= 85000;
			if (pAcce->GetSocket(ACCE_ABSORPTION_SOCKET) >= ACCE_EFFECT_FROM_ABS)
				toSetValue += 1000;
			
			return toSetValue;
		}
		else
			return 0;
	}
#else
	else if (bPartPos == PART_ACCE && GetWear(WEAR_COSTUME_ACCE_SLOT) && IsAcceCostumeHidden() == true)
		return 0;
#endif
	else if (bPartPos == PART_WEAPON && GetWear(WEAR_COSTUME_WEAPON) && IsWeaponCostumeHidden() == true)
	{
		if (const LPITEM pWeapon = GetWear(WEAR_WEAPON))
#ifdef __CHANGE_LOOK_SYSTEM__
			return pWeapon->GetTransmutation() != 0 ? pWeapon->GetTransmutation() : pWeapon->GetVnum();
#else
			return pWeapon->GetVnum();
#endif
		else
			return 0;
	}
#endif
	
	return m_pointsInstant.parts[bPartPos];
}

WORD CHARACTER::GetOriginalPart(BYTE bPartPos) const
{
	switch (bPartPos)
	{
		case PART_MAIN:
			{
				if (!IsPC())
					return GetPart(PART_MAIN);
				
#ifdef __HIDE_COSTUME_SYSTEM__
				if (GetWear(WEAR_COSTUME_BODY) && IsBodyCostumeHidden() == true) {
					if (const LPITEM pArmor = GetWear(WEAR_BODY))
						return pArmor->GetVnum();
				}
#endif
				
				return m_pointsInstant.bBasePart;
			}
		case PART_HAIR:
			{
#ifdef __HIDE_COSTUME_SYSTEM__
				if (GetWear(WEAR_COSTUME_HAIR) && IsHairCostumeHidden() == true)
					return 0;
#endif
				
				return GetPart(PART_HAIR);
			}
#ifdef ENABLE_ACCE_SYSTEM
		case PART_ACCE:
			{
#ifdef __HIDE_COSTUME_SYSTEM__
#ifdef ENABLE_STOLE_COSTUME
				if (GetWear(WEAR_COSTUME_ACCE) && IsAcceCostumeHidden() == true) {
					LPITEM pAcce = GetWear(WEAR_COSTUME_ACCE_SLOT);
					if (pAcce) {
						DWORD toSetValue = pAcce->GetVnum();
						toSetValue -= 85000;
						if (pAcce->GetSocket(ACCE_ABSORPTION_SOCKET) >= ACCE_EFFECT_FROM_ABS)
							toSetValue += 1000;
						
						return toSetValue;
					}
					else
						return 0;
				}
#else
				if (GetWear(WEAR_COSTUME_ACCE_SLOT) && IsAcceCostumeHidden() == true)
					return 0;
#endif
#else
				if (GetWear(WEAR_COSTUME_ACCE_SLOT))
					return 0;
#endif
				return GetPart(PART_ACCE);
			}
#endif
#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
		case PART_WEAPON:
			{
#ifdef __HIDE_COSTUME_SYSTEM__
				if (GetWear(WEAR_COSTUME_WEAPON) && IsWeaponCostumeHidden() == true) {
					if (const LPITEM pWeapon = GetWear(WEAR_WEAPON))
						return pWeapon->GetVnum();
				}
#endif
				return GetPart(PART_WEAPON);
#endif
			}
		default:
			return 0;
	}
}

BYTE CHARACTER::GetCharType() const
{
	return m_bCharType;
}

bool CHARACTER::SetSyncOwner(LPCHARACTER ch, bool bRemoveFromList)
{
	// TRENT_MONSTER
	if (IS_SET(m_pointsInstant.dwAIFlag, AIFLAG_NOMOVE))
		return false;
	// END_OF_TRENT_MONSTER
	
	if (ch) // @fixme131
	{
		if (!battle_is_attackable(ch, this))
		{
			SendDamagePacket(ch, 0, DAMAGE_BLOCK);
			return false;
		}
	}

	if (ch == this)
	{
		sys_err("SetSyncOwner owner == this (%p)", this);
		return false;
	}

	if (!ch)
	{
		if (bRemoveFromList && m_pkChrSyncOwner)
		{
			m_pkChrSyncOwner->m_kLst_pkChrSyncOwned.remove(this);
		}

		if (m_pkChrSyncOwner)
			sys_log(1, "SyncRelease %s %p from %s", GetName(), this, m_pkChrSyncOwner->GetName());

		// ����Ʈ���� �������� �ʴ��� �����ʹ� NULL�� ���õǾ�� �Ѵ�.
		m_pkChrSyncOwner = NULL;
	}
	else
	{
		if (!IsSyncOwner(ch))
			return false;

		// �Ÿ��� 200 �̻��̸� SyncOwner�� �� �� ����.
		if (DISTANCE_APPROX(GetX() - ch->GetX(), GetY() - ch->GetY()) > 250)
		{
			sys_log(1, "SetSyncOwner distance over than 250 %s %s", GetName(), ch->GetName());

			// SyncOwner�� ��� Owner�� ǥ���Ѵ�.
			if (m_pkChrSyncOwner == ch)
				return true;

			return false;
		}

		if (m_pkChrSyncOwner != ch)
		{
			if (m_pkChrSyncOwner)
			{
				sys_log(1, "SyncRelease %s %p from %s", GetName(), this, m_pkChrSyncOwner->GetName());
				m_pkChrSyncOwner->m_kLst_pkChrSyncOwned.remove(this);
			}

			m_pkChrSyncOwner = ch;
			m_pkChrSyncOwner->m_kLst_pkChrSyncOwned.push_back(this);

			// SyncOwner�� �ٲ�� LastSyncTime�� �ʱ�ȭ�Ѵ�.
			static const timeval zero_tv = {0, 0};
			SetLastSyncTime(zero_tv);

			sys_log(1, "SetSyncOwner set %s %p to %s", GetName(), this, ch->GetName());
		}

		m_fSyncTime = get_float_time();
	}

	// TODO: Sync Owner�� ������ ��� ��Ŷ�� ������ �����Ƿ�,
	//       ����ȭ �� �ð��� 3�� �̻� ������ �� Ǯ���ִ� ��Ŷ��
	//       ������ ������� �ϸ� ��Ŷ�� ���� �� �ִ�.
	TPacketGCOwnership pack;

	pack.bHeader	= HEADER_GC_OWNERSHIP;
	pack.dwOwnerVID	= ch ? ch->GetVID() : 0;
	pack.dwVictimVID	= GetVID();

	PacketAround(&pack, sizeof(TPacketGCOwnership));
	return true;
}

struct FuncClearSync
{
	void operator () (LPCHARACTER ch)
	{
		assert(ch != NULL);
		ch->SetSyncOwner(NULL, false);	// false �÷��׷� �ؾ� for_each �� ����� ����.
	}
};

void CHARACTER::ClearSync()
{
	SetSyncOwner(NULL);

	// �Ʒ� for_each���� ���� m_pkChrSyncOwner�� ���� �ڵ��� �����͸� NULL�� �Ѵ�.
	std::for_each(m_kLst_pkChrSyncOwned.begin(), m_kLst_pkChrSyncOwned.end(), FuncClearSync());
	m_kLst_pkChrSyncOwned.clear();
}

bool CHARACTER::IsSyncOwner(LPCHARACTER ch) const
{
	if (m_pkChrSyncOwner == ch)
		return true;

	// ���������� ����ȭ �� �ð��� 3�� �̻� �����ٸ� �������� �ƹ����Ե�
	// ����. ���� �ƹ��� SyncOwner�̹Ƿ� true ����
	if (get_float_time() - m_fSyncTime >= 3.0f)
		return true;

	return false;
}

void CHARACTER::SetParty(LPPARTY pkParty)
{
	if (pkParty == m_pkParty)
		return;

	if (pkParty && m_pkParty)
		sys_err("%s is trying to reassigning party (current %p, new party %p)", GetName(), get_pointer(m_pkParty), get_pointer(pkParty));

	sys_log(1, "PARTY set to %p", get_pointer(pkParty));

	//if (m_pkDungeon && IsPC())
	//SetDungeon(NULL);

#ifdef ENABLE_BUG_FIXES
	if (m_pkDungeon && IsPC() && !pkParty) {
		SetDungeon(NULL);
	}
#endif

#ifdef ENABLE_NEW_USE_POTION
	if (IsPC() && m_pkParty && pkParty == NULL && m_pkParty->GetLeaderPID() == GetPlayerID()) {
		CAffect* pAffect = FindAffect(AFFECT_NEW_POTION31);
		if (pAffect) {
			LPITEM pkItem = FindItemByID(pAffect->dwFlag);
			if (pkItem) {
				pkItem->Lock(false);
				pkItem->SetSocket(1, 0);
			}

			RemoveAffect(AFFECT_NEW_POTION31);
		}
	}
#endif

	m_pkParty = pkParty;

	if (IsPC())
	{
		if (m_pkParty)
			SET_BIT(m_bAddChrState, ADD_CHARACTER_STATE_PARTY);
		else
			REMOVE_BIT(m_bAddChrState, ADD_CHARACTER_STATE_PARTY);

		UpdatePacket();
	}
}

// PARTY_JOIN_BUG_FIX
/// ��Ƽ ���� event ����
EVENTINFO(TPartyJoinEventInfo)
{
	DWORD	dwGuestPID;		///< ��Ƽ�� ������ ĳ������ PID
	DWORD	dwLeaderPID;		///< ��Ƽ ������ PID

	TPartyJoinEventInfo()
	: dwGuestPID( 0 )
	, dwLeaderPID( 0 )
	{
	}
} ;

EVENTFUNC(party_request_event)
{
	TPartyJoinEventInfo * info = dynamic_cast<TPartyJoinEventInfo *>(  event->info );

	if ( info == NULL )
	{
		sys_err( "party_request_event> <Factor> Null pointer" );
		return 0;
	}

	LPCHARACTER ch = CHARACTER_MANAGER::instance().FindByPID(info->dwGuestPID);

	if (ch)
	{
		sys_log(0, "PartyRequestEvent %s", ch->GetName());
		ch->ChatPacket(CHAT_TYPE_COMMAND, "PartyRequestDenied");
		ch->SetPartyRequestEvent(NULL);
	}

	return 0;
}

bool CHARACTER::RequestToParty(LPCHARACTER leader)
{
	if (leader->GetParty())
		leader = leader->GetParty()->GetLeaderCharacter();

	if (!leader)
	{
#ifdef TEXTS_IMPROVEMENT
		ChatPacketNew(CHAT_TYPE_INFO, 488, "");
#endif
		return false;
	}

	if (m_pkPartyRequestEvent)
		return false;

	if (!IsPC() || !leader->IsPC())
		return false;

	if (leader->IsBlockMode(BLOCK_PARTY_REQUEST))
		return false;

	PartyJoinErrCode errcode = IsPartyJoinableCondition(leader, this);

	switch (errcode)
	{
		case PERR_NONE:
			break;

		case PERR_SERVER:
#ifdef TEXTS_IMPROVEMENT
			ChatPacketNew(CHAT_TYPE_INFO, 208, "");
#endif
			return false;

		case PERR_DIFFEMPIRE:
#ifdef TEXTS_IMPROVEMENT
			ChatPacketNew(CHAT_TYPE_INFO, 196, "");
#endif
			return false;

		case PERR_DUNGEON:
#ifdef TEXTS_IMPROVEMENT
			ChatPacketNew(CHAT_TYPE_INFO, 200, "");
#endif
			return false;
		case PERR_OBSERVER:
#ifdef TEXTS_IMPROVEMENT
			ChatPacketNew(CHAT_TYPE_INFO, 195, "");
#endif
			return false;

		case PERR_LVBOUNDARY:
#ifdef TEXTS_IMPROVEMENT
			ChatPacketNew(CHAT_TYPE_INFO, 194, "");
#endif
			return false;

		case PERR_LOWLEVEL:
#ifdef TEXTS_IMPROVEMENT
			ChatPacketNew(CHAT_TYPE_INFO, 214, "");
#endif
			return false;

		case PERR_HILEVEL:
#ifdef TEXTS_IMPROVEMENT
			ChatPacketNew(CHAT_TYPE_INFO, 214, "");
#endif
			return false;

		case PERR_ALREADYJOIN:
			return false;

		case PERR_PARTYISFULL:
#ifdef TEXTS_IMPROVEMENT
			ChatPacketNew(CHAT_TYPE_INFO, 199, "");
#endif
			return false;

		default:
			sys_err("Do not process party join error(%d)", errcode);
			return false;
	}

	TPartyJoinEventInfo* info = AllocEventInfo<TPartyJoinEventInfo>();

	info->dwGuestPID = GetPlayerID();
	info->dwLeaderPID = leader->GetPlayerID();

	SetPartyRequestEvent(event_create(party_request_event, info, PASSES_PER_SEC(10)));

	leader->ChatPacket(CHAT_TYPE_COMMAND, "PartyRequest %u", (DWORD) GetVID());
#ifdef TEXTS_IMPROVEMENT
	ChatPacketNew(CHAT_TYPE_INFO, 106, "%s", leader->GetName());
#endif
	return true;
}

void CHARACTER::DenyToParty(LPCHARACTER member)
{
	sys_log(1, "DenyToParty %s member %s %p", GetName(), member->GetName(), get_pointer(member->m_pkPartyRequestEvent));

	if (!member->m_pkPartyRequestEvent)
		return;

	TPartyJoinEventInfo * info = dynamic_cast<TPartyJoinEventInfo *>(member->m_pkPartyRequestEvent->info);

	if (!info)
	{
		sys_err( "CHARACTER::DenyToParty> <Factor> Null pointer" );
		return;
	}

	if (info->dwGuestPID != member->GetPlayerID())
		return;

	if (info->dwLeaderPID != GetPlayerID())
		return;

	event_cancel(&member->m_pkPartyRequestEvent);

	member->ChatPacket(CHAT_TYPE_COMMAND, "PartyRequestDenied");
}

void CHARACTER::AcceptToParty(LPCHARACTER member)
{
	sys_log(1, "AcceptToParty %s member %s %p", GetName(), member->GetName(), get_pointer(member->m_pkPartyRequestEvent));

	if (!member->m_pkPartyRequestEvent)
		return;

	TPartyJoinEventInfo * info = dynamic_cast<TPartyJoinEventInfo *>(member->m_pkPartyRequestEvent->info);

	if (!info)
	{
		sys_err( "CHARACTER::AcceptToParty> <Factor> Null pointer" );
		return;
	}

	if (info->dwGuestPID != member->GetPlayerID())
		return;

	if (info->dwLeaderPID != GetPlayerID())
		return;

	event_cancel(&member->m_pkPartyRequestEvent);

	if (GetParty())
	{
		if (GetPlayerID() != GetParty()->GetLeaderPID())
			return;

		PartyJoinErrCode errcode = IsPartyJoinableCondition(this, member);
		switch (errcode)
		{
			case PERR_NONE: 		member->PartyJoin(this); return;
			case PERR_SERVER:
#ifdef TEXTS_IMPROVEMENT
				member->ChatPacketNew(CHAT_TYPE_INFO, 208, "");
#endif
				break;
			case PERR_DUNGEON:
#ifdef TEXTS_IMPROVEMENT
				member->ChatPacketNew(CHAT_TYPE_INFO, 200, "");
#endif
				break;
			case PERR_OBSERVER:
#ifdef TEXTS_IMPROVEMENT
				member->ChatPacketNew(CHAT_TYPE_INFO, 195, "");
#endif
				break;
			case PERR_LOWLEVEL:
			case PERR_LVBOUNDARY:
#ifdef TEXTS_IMPROVEMENT
				member->ChatPacketNew(CHAT_TYPE_INFO, 194, "");
#endif
				break;
			case PERR_HILEVEL:
#ifdef TEXTS_IMPROVEMENT
				member->ChatPacketNew(CHAT_TYPE_INFO, 214, "");
#endif
				break;
			case PERR_ALREADYJOIN: 	break;
			case PERR_PARTYISFULL:
			{
#ifdef TEXTS_IMPROVEMENT
				ChatPacketNew(CHAT_TYPE_INFO, 199, "");
				member->ChatPacketNew(CHAT_TYPE_INFO, 220, "");
#endif
				break;
				
			}
			default:
				sys_err("Do not process party join error(%d)", errcode);
		}
	}

	member->ChatPacket(CHAT_TYPE_COMMAND, "PartyRequestDenied");
}

/**
 * ��Ƽ �ʴ� event callback �Լ�.
 * event �� �ߵ��ϸ� �ʴ� ������ ó���Ѵ�.
 */
EVENTFUNC(party_invite_event)
{
	TPartyJoinEventInfo * pInfo = dynamic_cast<TPartyJoinEventInfo *>(  event->info );

	if ( pInfo == NULL )
	{
		sys_err( "party_invite_event> <Factor> Null pointer" );
		return 0;
	}

	LPCHARACTER pchInviter = CHARACTER_MANAGER::instance().FindByPID(pInfo->dwLeaderPID);

	if (pchInviter)
	{
		sys_log(1, "PartyInviteEvent %s", pchInviter->GetName());
		pchInviter->PartyInviteDeny(pInfo->dwGuestPID);
	}

	return 0;
}

void CHARACTER::PartyInvite(LPCHARACTER pchInvitee)
{
	if (GetParty() && GetParty()->GetLeaderPID() != GetPlayerID())
	{
#ifdef TEXTS_IMPROVEMENT
		ChatPacketNew(CHAT_TYPE_INFO, 218, "");
#endif
		return;
	}
	else if (pchInvitee->IsBlockMode(BLOCK_PARTY_INVITE))
	{
#ifdef TEXTS_IMPROVEMENT
		ChatPacketNew(CHAT_TYPE_INFO, 192, "%s", pchInvitee->GetName());
#endif
		return;
	}

#ifdef ENABLE_PVP_ADVANCED
	else if ((GetDuel("BlockParty")))
	{
#ifdef TEXTS_IMPROVEMENT
		ChatPacketNew(CHAT_TYPE_INFO, 516, "");
#endif
		return;
	}
	
	else if ((pchInvitee->GetDuel("BlockParty")))
	{
#ifdef TEXTS_IMPROVEMENT
		ChatPacketNew(CHAT_TYPE_INFO, 517, "%s", pchInvitee->GetName());
#endif
		return;
	}
#endif

	PartyJoinErrCode errcode = IsPartyJoinableCondition(this, pchInvitee);

	switch (errcode)
	{
		case PERR_NONE:
			break;

		case PERR_SERVER:
#ifdef TEXTS_IMPROVEMENT
			ChatPacketNew(CHAT_TYPE_INFO, 208, "");
#endif
			return;
		case PERR_DIFFEMPIRE:
#ifdef TEXTS_IMPROVEMENT
			ChatPacketNew(CHAT_TYPE_INFO, 196, "");
#endif
			return;
		case PERR_DUNGEON:
#ifdef TEXTS_IMPROVEMENT
			ChatPacketNew(CHAT_TYPE_INFO, 200, "");
#endif
			return;
		case PERR_OBSERVER:
#ifdef TEXTS_IMPROVEMENT
			ChatPacketNew(CHAT_TYPE_INFO, 195, "");
#endif
			return;
		case PERR_LVBOUNDARY:
#ifdef TEXTS_IMPROVEMENT
			ChatPacketNew(CHAT_TYPE_INFO, 194, "");
#endif
			return;
		case PERR_LOWLEVEL:
#ifdef TEXTS_IMPROVEMENT
			ChatPacketNew(CHAT_TYPE_INFO, 214, "");
#endif
			return;

		case PERR_HILEVEL:
#ifdef TEXTS_IMPROVEMENT
			ChatPacketNew(CHAT_TYPE_INFO, 214, "");
#endif
			return;
		case PERR_ALREADYJOIN:
#ifdef TEXTS_IMPROVEMENT
			ChatPacketNew(CHAT_TYPE_INFO, 210, "%s", pchInvitee->GetName());
#endif
			return;
		case PERR_PARTYISFULL:
#ifdef TEXTS_IMPROVEMENT
			ChatPacketNew(CHAT_TYPE_INFO, 199, "");
#endif
			return;
		default:
			sys_err("Do not process party join error(%d)", errcode);
			return;
	}

	if (m_PartyInviteEventMap.end() != m_PartyInviteEventMap.find(pchInvitee->GetPlayerID()))
		return;

	//
	// EventMap �� �̺�Ʈ �߰�
	//
	TPartyJoinEventInfo* info = AllocEventInfo<TPartyJoinEventInfo>();

	info->dwGuestPID = pchInvitee->GetPlayerID();
	info->dwLeaderPID = GetPlayerID();

	m_PartyInviteEventMap.insert(EventMap::value_type(pchInvitee->GetPlayerID(), event_create(party_invite_event, info, PASSES_PER_SEC(10))));

	//
	// �ʴ� �޴� character ���� �ʴ� ��Ŷ ����
	//

	TPacketGCPartyInvite p;
	p.header = HEADER_GC_PARTY_INVITE;
	p.leader_vid = GetVID();
	pchInvitee->GetDesc()->Packet(&p, sizeof(p));
}

void CHARACTER::PartyInviteAccept(LPCHARACTER pchInvitee)
{
	EventMap::iterator itFind = m_PartyInviteEventMap.find(pchInvitee->GetPlayerID());

	if (itFind == m_PartyInviteEventMap.end())
	{
		sys_log(1, "PartyInviteAccept from not invited character(%s)", pchInvitee->GetName());
		return;
	}

	event_cancel(&itFind->second);
	m_PartyInviteEventMap.erase(itFind);

	if (GetParty() && GetParty()->GetLeaderPID() != GetPlayerID())
	{
#ifdef TEXTS_IMPROVEMENT
		ChatPacketNew(CHAT_TYPE_INFO, 218, "");
#endif
		return;
	}

	PartyJoinErrCode errcode = IsPartyJoinableMutableCondition(this, pchInvitee);

	switch (errcode)
	{
		case PERR_NONE:
			break;
		case PERR_SERVER:
#ifdef TEXTS_IMPROVEMENT
			pchInvitee->ChatPacketNew(CHAT_TYPE_INFO, 208, "");
#endif
			return;
		case PERR_DUNGEON:
#ifdef TEXTS_IMPROVEMENT
			pchInvitee->ChatPacketNew(CHAT_TYPE_INFO, 201, "");
#endif
			return;
		case PERR_OBSERVER:
#ifdef TEXTS_IMPROVEMENT
			pchInvitee->ChatPacketNew(CHAT_TYPE_INFO, 195, "");
#endif
			return;
		case PERR_LVBOUNDARY:
#ifdef TEXTS_IMPROVEMENT
			pchInvitee->ChatPacketNew(CHAT_TYPE_INFO, 194, "");
#endif
			return;
		case PERR_LOWLEVEL:
#ifdef TEXTS_IMPROVEMENT
			pchInvitee->ChatPacketNew(CHAT_TYPE_INFO, 214, "");
#endif
			return;
		case PERR_HILEVEL:
#ifdef TEXTS_IMPROVEMENT
			pchInvitee->ChatPacketNew(CHAT_TYPE_INFO, 214, "");
#endif
			return;
		case PERR_ALREADYJOIN:
#ifdef TEXTS_IMPROVEMENT
			pchInvitee->ChatPacketNew(CHAT_TYPE_INFO, 212, "");
#endif
			return;
		case PERR_PARTYISFULL:
#ifdef TEXTS_IMPROVEMENT
			ChatPacketNew(CHAT_TYPE_INFO, 199, "");
			pchInvitee->ChatPacketNew(CHAT_TYPE_INFO, 220, "");
#endif
			return;
		default:
			sys_err("ignore party join error(%d)", errcode);
			return;
	}

	//
	// ��Ƽ ���� ó��
	//

	if (GetParty())
		pchInvitee->PartyJoin(this);
	else
	{
		LPPARTY pParty = CPartyManager::instance().CreateParty(this);

		pParty->Join(pchInvitee->GetPlayerID());
		pParty->Link(pchInvitee);
		pParty->SendPartyInfoAllToOne(this);
	}
}

void CHARACTER::PartyInviteDeny(DWORD dwPID)
{
	EventMap::iterator itFind = m_PartyInviteEventMap.find(dwPID);

	if (itFind == m_PartyInviteEventMap.end())
	{
		sys_log(1, "PartyInviteDeny to not exist event(inviter PID: %d, invitee PID: %d)", GetPlayerID(), dwPID);
		return;
	}

	event_cancel(&itFind->second);
	m_PartyInviteEventMap.erase(itFind);
#ifdef TEXTS_IMPROVEMENT
	LPCHARACTER pchInvitee = CHARACTER_MANAGER::instance().FindByPID(dwPID);
	if (pchInvitee) {
		ChatPacketNew(CHAT_TYPE_INFO, 192, "%s", pchInvitee->GetName());
	}
#endif
}

void CHARACTER::PartyJoin(LPCHARACTER pLeader) {
	if (pLeader && pLeader->GetParty()) {
#ifdef TEXTS_IMPROVEMENT
		pLeader->ChatPacketNew(CHAT_TYPE_INFO, 1249, "%s", GetName());
		ChatPacketNew(CHAT_TYPE_INFO, 193, "%s", pLeader->GetName());
#endif
		pLeader->GetParty()->Join(GetPlayerID());
		pLeader->GetParty()->Link(this);
	}
}

CHARACTER::PartyJoinErrCode CHARACTER::IsPartyJoinableCondition(const LPCHARACTER pchLeader, const LPCHARACTER pchGuest)
{
	if (pchLeader->GetEmpire() != pchGuest->GetEmpire())
		return PERR_DIFFEMPIRE;

	return IsPartyJoinableMutableCondition(pchLeader, pchGuest);
}

static bool __party_can_join_by_level(LPCHARACTER leader, LPCHARACTER quest)
{
	int	level_limit = 30;
	return (abs(leader->GetLevel() - quest->GetLevel()) <= level_limit);
}

CHARACTER::PartyJoinErrCode CHARACTER::IsPartyJoinableMutableCondition(const LPCHARACTER pchLeader, const LPCHARACTER pchGuest)
{
	if (!CPartyManager::instance().IsEnablePCParty())
		return PERR_SERVER;
	else if (pchLeader->GetDungeon())
		return PERR_DUNGEON;
	else if (pchGuest->IsObserverMode())
		return PERR_OBSERVER;
	else if (false == __party_can_join_by_level(pchLeader, pchGuest))
		return PERR_LVBOUNDARY;
	else if (pchGuest->GetParty())
		return PERR_ALREADYJOIN;
	else if (pchLeader->GetParty())
   	{
	   	if (pchLeader->GetParty()->GetMemberCount() == PARTY_MAX_MEMBER)
			return PERR_PARTYISFULL;
	}

	return PERR_NONE;
}
// END_OF_PARTY_JOIN_BUG_FIX

void CHARACTER::SetDungeon(LPDUNGEON pkDungeon)
{
	if (pkDungeon && m_pkDungeon)
	{
		sys_err("%s is trying to reassigning dungeon (current %p, new party %p)", GetName(), get_pointer(m_pkDungeon), get_pointer(pkDungeon));
	}

	if (m_pkDungeon)
	{
		if (IsPC())
		{
			if (GetParty())
				m_pkDungeon->DecPartyMember(GetParty(), this);
			else
				m_pkDungeon->DecMember(this);
		}
	}

	m_pkDungeon = pkDungeon;

	if (pkDungeon)
	{
		//sys_log(0, "%s DUNGEON set to %p, PARTY is %p", GetName(), get_pointer(pkDungeon), get_pointer(m_pkParty));

		if (IsPC())
		{
			if (GetParty())
				m_pkDungeon->IncPartyMember(GetParty(), this);
			else
				m_pkDungeon->IncMember(this);
		}
		else if (IsMonster() || IsStone())
		{
			m_pkDungeon->IncMonster();
		}
	}
}

void CHARACTER::SetWarMap(CWarMap * pWarMap)
{
	if (m_pWarMap)
		m_pWarMap->DecMember(this);

	m_pWarMap = pWarMap;

	if (m_pWarMap)
		m_pWarMap->IncMember(this);
}

void CHARACTER::SetWeddingMap(marriage::WeddingMap* pMap)
{
	if (m_pWeddingMap)
		m_pWeddingMap->DecMember(this);

	m_pWeddingMap = pMap;

	if (m_pWeddingMap)
		m_pWeddingMap->IncMember(this);
}

void CHARACTER::SetRegen(LPREGEN pkRegen)
{
	m_pkRegen = pkRegen;
	if (pkRegen != NULL) {
		regen_id_ = pkRegen->id;
	}
	m_fRegenAngle = GetRotation();
	m_posRegen = GetXYZ();
}

bool CHARACTER::OnIdle()
{
	return false;
}

void CHARACTER::OnMove(bool bIsAttack)
{
	m_dwLastMoveTime = get_dword_time();

	if (bIsAttack)
	{
		m_dwLastAttackTime = m_dwLastMoveTime;

		if (IsAffectFlag(AFF_REVIVE_INVISIBLE))
			RemoveAffect(AFFECT_REVIVE_INVISIBLE);

		if (IsAffectFlag(AFF_EUNHYUNG))
		{
			RemoveAffect(SKILL_EUNHYUNG);
			SetAffectedEunhyung();
		}
		else
		{
			ClearAffectedEunhyung();
		}

		/*if (IsAffectFlag(AFF_JEONSIN))
		  RemoveAffect(SKILL_JEONSINBANGEO);*/
	}

	/*if (IsAffectFlag(AFF_GUNGON))
	  RemoveAffect(SKILL_GUNGON);*/

	// MINING
	mining_cancel();
	// END_OF_MINING
}

void CHARACTER::OnClick(LPCHARACTER pkChrCauser)
{
	if (!pkChrCauser)
	{
		sys_err("OnClick %s by NULL", GetName());
		return;
	}

	DWORD vid = GetVID();
	sys_log(0, "OnClick %s[vnum: %d vid: %d] by %s", GetName(), GetRaceNum(), vid, pkChrCauser->GetName());

	// ������ �����·� ����Ʈ�� ������ �� ����.
	{
		// ��, �ڽ��� �ڽ��� ������ Ŭ���� �� �ִ�.
		if (pkChrCauser->GetMyShop() && pkChrCauser != this)
		{
			sys_err("OnClick Fail (%s->%s) - pc has shop", pkChrCauser->GetName(), GetName());
			return;
		}
	}

	// ��ȯ���϶� ����Ʈ�� ������ �� ����.
	{
		if (pkChrCauser->GetExchange())
		{
			sys_err("OnClick Fail (%s->%s) - pc is exchanging", pkChrCauser->GetName(), GetName());
			return;
		}
	}

	if (IsPC())
	{
		// Ÿ������ ������ ���� PC�� ���� Ŭ���� ����Ʈ�� ó���ϵ��� �մϴ�.
		if (!CTargetManager::instance().GetTargetInfo(pkChrCauser->GetPlayerID(), TARGET_TYPE_VID, GetVID()))
		{
			// 2005.03.17.myevan.Ÿ���� �ƴ� ���� ���� ���� ó�� ����� �۵���Ų��.
			if (GetMyShop())
			{
				if (pkChrCauser->IsDead() == true) return;

				//PREVENT_TRADE_WINDOW
				if (pkChrCauser == this) // �ڱ�� ����
				{
					if ((GetExchange() || IsOpenSafebox() || GetShopOwner()) || IsCubeOpen())
					{
#ifdef TEXTS_IMPROVEMENT
						pkChrCauser->ChatPacketNew(CHAT_TYPE_INFO, 291, "");
#endif
						return;
					}

#ifdef __ATTR_TRANSFER_SYSTEM__
					if (IsAttrTransferOpen())
					{
#ifdef TEXTS_IMPROVEMENT
						pkChrCauser->ChatPacketNew(CHAT_TYPE_INFO, 291, "");
#endif
						return;
					}
#endif
				}
				else // �ٸ� ����� Ŭ��������
				{
					// Ŭ���� ����� ��ȯ/â��/���λ���/�����̿����̶�� �Ұ�
					if ((pkChrCauser->GetExchange() || pkChrCauser->IsOpenSafebox() || pkChrCauser->GetMyShop() || pkChrCauser->GetShopOwner()) || pkChrCauser->IsCubeOpen() )
					{
#ifdef TEXTS_IMPROVEMENT
						pkChrCauser->ChatPacketNew(CHAT_TYPE_INFO, 291, "");
#endif
						return;
					}

#ifdef __ATTR_TRANSFER_SYSTEM__
					if (pkChrCauser->IsAttrTransferOpen())
					{
#ifdef TEXTS_IMPROVEMENT
						pkChrCauser->ChatPacketNew(CHAT_TYPE_INFO, 291, "");
#endif
						return;
					}
#endif

					// Ŭ���� ����� ��ȯ/â��/�����̿����̶�� �Ұ�
					//if ((GetExchange() || IsOpenSafebox() || GetShopOwner()))
					if ((GetExchange() || IsOpenSafebox() || IsCubeOpen()))
					{
#ifdef TEXTS_IMPROVEMENT
						pkChrCauser->ChatPacketNew(CHAT_TYPE_INFO, 369, "%s", GetName());
#endif
						return;
					}

#ifdef __ATTR_TRANSFER_SYSTEM__
					if (IsAttrTransferOpen())
					{
#ifdef TEXTS_IMPROVEMENT
						pkChrCauser->ChatPacketNew(CHAT_TYPE_INFO, 369, "%s", GetName());
#endif
						return;
					}
#endif
				}
				//END_PREVENT_TRADE_WINDOW

				if (pkChrCauser->GetShop())
				{
					pkChrCauser->GetShop()->RemoveGuest(pkChrCauser);
					pkChrCauser->SetShop(NULL);
				}

				GetMyShop()->AddGuest(pkChrCauser, GetVID(), false);
				pkChrCauser->SetShopOwner(this);
				return;
			}

			if (test_server)
				sys_err("%s.OnClickFailure(%s) - target is PC", pkChrCauser->GetName(), GetName());

			return;
		}
	}

	pkChrCauser->SetQuestNPCID(GetVID());

	if (quest::CQuestManager::instance().Click(pkChrCauser->GetPlayerID(), this))
	{
		return;
	}

	// NPC ���� ��� ���� : ���� ���� ��
	if (!IsPC())
	{
		if (!m_triggerOnClick.pFunc)
		{
			// NPC Ʈ���� �ý��� �α� ����
			//sys_err("%s.OnClickFailure(%s) : triggerOnClick.pFunc is EMPTY(pid=%d)",
			//			pkChrCauser->GetName(),
			//			GetName(),
			//			pkChrCauser->GetPlayerID());
			return;
		}

		m_triggerOnClick.pFunc(this, pkChrCauser);
	}

}

BYTE CHARACTER::GetGMLevel() const
{
	if (test_server)
		return GM_IMPLEMENTOR;
	return m_pointsInstant.gm_level;
}

void CHARACTER::SetGMLevel()
{
	if (GetDesc())
	{
	    m_pointsInstant.gm_level =  gm_get_level(GetName(), GetDesc()->GetHostName(), GetDesc()->GetAccountTable().login);
	}
	else
	{
	    m_pointsInstant.gm_level = GM_PLAYER;
	}
}

BOOL CHARACTER::IsGM() const
{
	if (m_pointsInstant.gm_level != GM_PLAYER)
		return true;
	if (test_server)
		return true;
	return false;
}

void CHARACTER::SetStone(LPCHARACTER pkChrStone)
{
	m_pkChrStone = pkChrStone;

	if (m_pkChrStone)
	{
		if (pkChrStone->m_set_pkChrSpawnedBy.find(this) == pkChrStone->m_set_pkChrSpawnedBy.end())
			pkChrStone->m_set_pkChrSpawnedBy.insert(this);
	}
}

struct FuncDeadSpawnedByStone
{
	void operator () (LPCHARACTER ch)
	{
		ch->Dead(NULL);
		ch->SetStone(NULL);
	}
};

void CHARACTER::ClearStone()
{
	if (!m_set_pkChrSpawnedBy.empty())
	{
		// ���� ������Ų ���͵��� ��� ���δ�.
		FuncDeadSpawnedByStone f;
		std::for_each(m_set_pkChrSpawnedBy.begin(), m_set_pkChrSpawnedBy.end(), f);
		m_set_pkChrSpawnedBy.clear();
	}

	if (!m_pkChrStone)
		return;

	m_pkChrStone->m_set_pkChrSpawnedBy.erase(this);
	m_pkChrStone = NULL;
}

void CHARACTER::ClearTarget()
{
	if (m_pkChrTarget)
	{
		m_pkChrTarget->m_set_pkChrTargetedBy.erase(this);
		m_pkChrTarget = NULL;
	}

	TPacketGCTarget p;

	p.header = HEADER_GC_TARGET;
	p.dwVID = 0;
	p.bHPPercent = 0;
#ifdef __VIEW_TARGET_DECIMAL_HP__
	p.iMinHP = 0;
	p.iMaxHP = 0;
#endif

	CHARACTER_SET::iterator it = m_set_pkChrTargetedBy.begin();

	while (it != m_set_pkChrTargetedBy.end())
	{
		LPCHARACTER pkChr = *(it++);
		pkChr->m_pkChrTarget = NULL;

		if (!pkChr->GetDesc())
		{
			sys_err("%s %p does not have desc", pkChr->GetName(), get_pointer(pkChr));
			abort();
		}

		pkChr->GetDesc()->Packet(&p, sizeof(TPacketGCTarget));
	}

	m_set_pkChrTargetedBy.clear();
}

void CHARACTER::SetTarget(LPCHARACTER pkChrTarget)
{
	if (m_pkChrTarget == pkChrTarget)
		return;

	if (m_pkChrTarget)
		m_pkChrTarget->m_set_pkChrTargetedBy.erase(this);

	m_pkChrTarget = pkChrTarget;

	TPacketGCTarget p;

	p.header = HEADER_GC_TARGET;

	if (m_pkChrTarget)
	{
		m_pkChrTarget->m_set_pkChrTargetedBy.insert(this);

		p.dwVID	= m_pkChrTarget->GetVID();

#ifdef __VIEW_TARGET_PLAYER_HP__
		if ((m_pkChrTarget->GetMaxHP() <= 0))
		{
			p.bHPPercent = 0;
#ifdef __VIEW_TARGET_DECIMAL_HP__
			p.iMinHP = 0;
			p.iMaxHP = 0;
#endif
		}
		else if (m_pkChrTarget->IsPC() && !m_pkChrTarget->IsPolymorphed())
		{
			p.bHPPercent = MINMAX(0, (m_pkChrTarget->GetHP() * 100) / m_pkChrTarget->GetMaxHP(), 100);
#ifdef __VIEW_TARGET_DECIMAL_HP__
			p.iMinHP = m_pkChrTarget->GetHP();
			p.iMaxHP = m_pkChrTarget->GetMaxHP();
#endif
		}
#else
		if ((m_pkChrTarget->IsPC() && !m_pkChrTarget->IsPolymorphed()) || (m_pkChrTarget->GetMaxHP() <= 0))
			p.bHPPercent = 0;
#endif
		else
		{
			if (m_pkChrTarget->GetRaceNum() == 20101 ||
					m_pkChrTarget->GetRaceNum() == 20102 ||
					m_pkChrTarget->GetRaceNum() == 20103 ||
					m_pkChrTarget->GetRaceNum() == 20104 ||
					m_pkChrTarget->GetRaceNum() == 20105 ||
					m_pkChrTarget->GetRaceNum() == 20106 ||
					m_pkChrTarget->GetRaceNum() == 20107 ||
					m_pkChrTarget->GetRaceNum() == 20108 ||
					m_pkChrTarget->GetRaceNum() == 20109)
			{
				LPCHARACTER owner = m_pkChrTarget->GetVictim();

				if (owner)
				{
					int iHorseHealth = owner->GetHorseHealth();
					int iHorseMaxHealth = owner->GetHorseMaxHealth();
#ifdef __VIEW_TARGET_DECIMAL_HP__
					if (iHorseMaxHealth)
					{
						p.bHPPercent = MINMAX(0,  iHorseHealth * 100 / iHorseMaxHealth, 100);
						p.iMinHP = 100;
						p.iMaxHP = 100;
					}
					else
					{
						p.bHPPercent = 100;
						p.iMinHP = 100;
						p.iMaxHP = 100;
					}
				}
				else
				{
					p.bHPPercent = 100;
					p.iMinHP = 100;
						p.iMaxHP = 100;
				}
			}
			else
			{
				if (m_pkChrTarget->GetMaxHP() <= 0) // @fixme136
				{
					p.bHPPercent = 0;
					p.iMinHP = 0;
					p.iMaxHP = 0;
				}
				else
				{
					p.bHPPercent = (m_pkChrTarget->GetHP() * 100) / m_pkChrTarget->GetMaxHP() > 100 ? 100 : (m_pkChrTarget->GetHP() * 100) / m_pkChrTarget->GetMaxHP();
					p.iMinHP = m_pkChrTarget->GetHP();
					p.iMaxHP = m_pkChrTarget->GetMaxHP();
				}
			}
		}
	}
	else
	{
		p.dwVID = 0;
		p.bHPPercent = 0;
		p.iMinHP = 0;
		p.iMaxHP = 0;
	}
#else
					if (iHorseMaxHealth)
						p.bHPPercent = MINMAX(0,  iHorseHealth * 100 / iHorseMaxHealth, 100);

					else
						p.bHPPercent = 100;
				}
				else
					p.bHPPercent = 100;
			}
			else
			{
				if (m_pkChrTarget->GetMaxHP() <= 0) // @fixme136
					p.bHPPercent = 0;
				else
					p.bHPPercent = MINMAX(0, (m_pkChrTarget->GetHP() * 100) / m_pkChrTarget->GetMaxHP(), 100);
			}
		}
	}
	else
	{
		p.dwVID = 0;
		p.bHPPercent = 0;
	}
#endif
#ifdef ELEMENT_TARGET
	p.bElement = 0;
	if (m_pkChrTarget) {
		if (m_pkChrTarget->IsPC()) {
			LPITEM item = m_pkChrTarget->GetWear(WEAR_PENDANT);
			if (item) {
				DWORD vnum = item->GetVnum();
				if (vnum >= 10750 && vnum <= 10950) {
					p.bElement = 1;
				} else if (vnum >= 9600 && vnum <= 9800) {
					p.bElement = 2;
				} else if (vnum >= 9830 && vnum <= 10030) {
					p.bElement = 3;
				} else if (vnum >= 10520 && vnum <= 10720) {
					p.bElement = 4;
				} else if (vnum >= 10060 && vnum <= 10260) {
					p.bElement = 5;
				} else if (vnum >= 10290 && vnum <= 10490) {
					p.bElement = 6;
				}
			}
		} else if (m_pkChrTarget->IsMonster() || m_pkChrTarget->IsStone()) {
			if (m_pkChrTarget->IsRaceFlag(RACE_FLAG_ATT_ELEC)) {
				p.bElement = 1;
			} else if (m_pkChrTarget->IsRaceFlag(RACE_FLAG_ATT_FIRE)) {
				p.bElement = 2;
			} else if (m_pkChrTarget->IsRaceFlag(RACE_FLAG_ATT_ICE)) {
				p.bElement = 3;
			} else if (m_pkChrTarget->IsRaceFlag(RACE_FLAG_ATT_WIND)) {
				p.bElement = 4;
			} else if (m_pkChrTarget->IsRaceFlag(RACE_FLAG_ATT_EARTH)) {
				p.bElement = 5;
			} else if (m_pkChrTarget->IsRaceFlag(RACE_FLAG_ATT_DARK)) {
				p.bElement = 6;
			}
		}
	}
#endif
	GetDesc()->Packet(&p, sizeof(TPacketGCTarget));
}

void CHARACTER::BroadcastTargetPacket()
{
	if (m_set_pkChrTargetedBy.empty())
		return;

	TPacketGCTarget p;

	p.header = HEADER_GC_TARGET;
	p.dwVID = GetVID();

#ifdef __VIEW_TARGET_DECIMAL_HP__
	if (GetMaxHP() <= 0) // @fixme136
	{
		p.bHPPercent = 0;
		p.iMinHP = 0;
		p.iMaxHP = 0;
	}
	else
	{
		p.bHPPercent = (GetHP() * 100) / GetMaxHP() > 100 ? 100 : (GetHP() * 100) / GetMaxHP();
		p.iMinHP = GetHP();
		p.iMaxHP = GetMaxHP();
	}
#else
	if (IsPC())
		p.bHPPercent = 0;
	else if (GetMaxHP() <= 0) // @fixme136
		p.bHPPercent = 0;
	else
		p.bHPPercent = MINMAX(0, (GetHP() * 100) / GetMaxHP(), 100);
#endif

	CHARACTER_SET::iterator it = m_set_pkChrTargetedBy.begin();

	while (it != m_set_pkChrTargetedBy.end())
	{
		LPCHARACTER pkChr = *it++;

		if (!pkChr->GetDesc())
		{
			sys_err("%s %p does not have desc", pkChr->GetName(), get_pointer(pkChr));
			abort();
		}

		pkChr->GetDesc()->Packet(&p, sizeof(TPacketGCTarget));
	}
}

void CHARACTER::CheckTarget()
{
	if (!m_pkChrTarget)
		return;

	if (DISTANCE_APPROX(GetX() - m_pkChrTarget->GetX(), GetY() - m_pkChrTarget->GetY()) >= 4800)
		SetTarget(NULL);
}

void CHARACTER::SetWarpLocation(long lMapIndex, long x, long y)
{
	m_posWarp.x = x * 100;
	m_posWarp.y = y * 100;
	m_lWarpMapIndex = lMapIndex;
}

void CHARACTER::SaveExitLocation()
{
	m_posExit = GetXYZ();
	m_lExitMapIndex = GetMapIndex();
}

void CHARACTER::ExitToSavedLocation()
{
	sys_log (0, "ExitToSavedLocation");
	WarpSet(m_posWarp.x, m_posWarp.y, m_lWarpMapIndex);

	m_posExit.x = m_posExit.y = m_posExit.z = 0;
	m_lExitMapIndex = 0;
}

// fixme
// ���ݱ��� privateMapIndex �� ���� �� �ε����� ������ üũ �ϴ� ���� �ܺο��� �ϰ�,
// �ٸ��� warpset�� �ҷ��µ�
// �̸� warpset ������ ����.
bool CHARACTER::WarpSet(long x, long y, long lPrivateMapIndex)
{
	if (!IsPC())
		return false;

	long lAddr;
	long lMapIndex;
	WORD wPort;

#ifdef ENABLE_GENERAL_CH
	BYTE ch = GetDesc() ? GetDesc()->GetAccountTable().bChannel : 0;
	if (!CMapLocation::instance().Get(ch, x, y, lMapIndex, lAddr, wPort)) {
		sys_err("cannot find map location index %d x %d y %d name %s", lMapIndex, x, y, GetName());
		return false;
	}

	if (lPrivateMapIndex >= 10000) {
		if (lPrivateMapIndex / 10000 != lMapIndex) {
			sys_err("Invalid map index %d, must be child of %d", lPrivateMapIndex, lMapIndex);
			return false;
		}

		lMapIndex = lPrivateMapIndex;
	}
#else
	if (!CMapLocation::instance().Get(x, y, lMapIndex, lAddr, wPort))
	{
		sys_err("cannot find map location index %d x %d y %d name %s", lMapIndex, x, y, GetName());
		return false;
	}

	if (lPrivateMapIndex >= 10000)
	{
		if (lPrivateMapIndex / 10000 != lMapIndex)
		{
			sys_err("Invalid map index %d, must be child of %d", lPrivateMapIndex, lMapIndex);
			return false;
		}

		lMapIndex = lPrivateMapIndex;
	}
#endif

	Stop();
	Save();

	if (GetSectree())
	{
		GetSectree()->RemoveEntity(this);
		ViewCleanup();

		EncodeRemovePacket(this);
	}

	m_lWarpMapIndex = lMapIndex;
	m_posWarp.x = x;
	m_posWarp.y = y;

	sys_log(0, "WarpSet %s %d %d current map %d target map %d", GetName(), x, y, GetMapIndex(), lMapIndex);

	TPacketGCWarp p;

	p.bHeader	= HEADER_GC_WARP;
	p.lX	= x;
	p.lY	= y;
	p.lAddr	= lAddr;
	p.wPort	= wPort;

#ifdef ENABLE_SWITCHBOT
	CSwitchbotManager::Instance().SetIsWarping(GetPlayerID(), true);

	if (p.wPort != mother_port)
	{
		CSwitchbotManager::Instance().P2PSendSwitchbot(GetPlayerID(), p.wPort);
	}
#endif

	GetDesc()->Packet(&p, sizeof(TPacketGCWarp));

	char buf[256];
	snprintf(buf, sizeof(buf), "%s MapIdx %ld DestMapIdx%ld DestX%ld DestY%ld Empire%d", GetName(), GetMapIndex(), lPrivateMapIndex, x, y, GetEmpire());
	LogManager::instance().CharLog(this, 0, "WARP", buf);

	return true;
}

#define ENABLE_GOHOME_IF_MAP_NOT_ALLOWED
void CHARACTER::WarpEnd()
{
	if (test_server)
		sys_log(0, "WarpEnd %s", GetName());

	if (m_posWarp.x == 0 && m_posWarp.y == 0)
		return;

	int index = m_lWarpMapIndex;

	if (index > 10000)
		index /= 10000;

	if (!map_allow_find(index))
	{
		// �� ������ ������ �� �����Ƿ� �����ϱ� �� ��ǥ�� �ǵ�����.
		sys_err("location %d %d not allowed to login this server", m_posWarp.x, m_posWarp.y);
#ifdef ENABLE_GOHOME_IF_MAP_NOT_ALLOWED
		GoHome();
#else
		GetDesc()->SetPhase(PHASE_CLOSE);
#endif
		return;
	}

	sys_log(0, "WarpEnd %s %d %u %u", GetName(), m_lWarpMapIndex, m_posWarp.x, m_posWarp.y);

	Show(m_lWarpMapIndex, m_posWarp.x, m_posWarp.y, 0);
	Stop();

	m_lWarpMapIndex = 0;
	m_posWarp.x = m_posWarp.y = m_posWarp.z = 0;

	{
		// P2P Login
		TPacketGGLogin p;

		p.bHeader = HEADER_GG_LOGIN;
		strlcpy(p.szName, GetName(), sizeof(p.szName));
		p.dwPID = GetPlayerID();
		p.bEmpire = GetEmpire();
		p.lMapIndex = SECTREE_MANAGER::instance().GetMapIndex(GetX(), GetY());
		p.bChannel = g_bChannel;

		P2P_MANAGER::instance().Send(&p, sizeof(TPacketGGLogin));
	}
}

bool CHARACTER::Return()
{
	if (!IsNPC())
		return false;

	int x, y;
	/*
	   float fDist = DISTANCE_SQRT(m_pkMobData->m_posLastAttacked.x - GetX(), m_pkMobData->m_posLastAttacked.y - GetY());
	   float fx, fy;
	   GetDeltaByDegree(GetRotation(), fDist, &fx, &fy);
	   x = GetX() + (int) fx;
	   y = GetY() + (int) fy;
	 */
	SetVictim(NULL);

	x = m_pkMobInst->m_posLastAttacked.x;
	y = m_pkMobInst->m_posLastAttacked.y;

	SetRotationToXY(x, y);

	if (!Goto(x, y))
		return false;

	SendMovePacket(FUNC_WAIT, 0, 0, 0, 0);

	if (test_server)
		sys_log(0, "%s %p �����ϰ� ���ư���! %d %d", GetName(), this, x, y);

	if (GetParty())
		GetParty()->SendMessage(this, PM_RETURN, x, y);

	return true;
}

bool CHARACTER::Follow(LPCHARACTER pkChr, float fMinDistance)
{
	if (IsPC())
	{
		sys_err("CHARACTER::Follow : PC cannot use this method", GetName());
		return false;
	}

	// TRENT_MONSTER
	if (IS_SET(m_pointsInstant.dwAIFlag, AIFLAG_NOMOVE))
	{
		if (pkChr->IsPC()) // �Ѿư��� ��밡 PC�� ��
		{
			// If i'm in a party. I must obey party leader's AI.
			if (!GetParty() || !GetParty()->GetLeader() || GetParty()->GetLeader() == this)
			{
				if (get_dword_time() - m_pkMobInst->m_dwLastAttackedTime >= 15000) // ���������� ���ݹ����� 15�ʰ� ������
				{
					// ������ ���� ������ ���� 50���� �̻� ���̳��� �����ϰ� ���ư���.
					if (m_pkMobData->m_table.wAttackRange < DISTANCE_APPROX(pkChr->GetX() - GetX(), pkChr->GetY() - GetY()))
						if (Return())
							return true;
				}
			}
		}
		return false;
	}
	// END_OF_TRENT_MONSTER

	long x = pkChr->GetX();
	long y = pkChr->GetY();

	if (pkChr->IsPC()) // �Ѿư��� ��밡 PC�� ��
	{
		// If i'm in a party. I must obey party leader's AI.
		if (!GetParty() || !GetParty()->GetLeader() || GetParty()->GetLeader() == this)
		{
			if (get_dword_time() - m_pkMobInst->m_dwLastAttackedTime >= 15000) // ���������� ���ݹ����� 15�ʰ� ������
			{
				// ������ ���� ������ ���� 50���� �̻� ���̳��� �����ϰ� ���ư���.
				if (5000 < DISTANCE_APPROX(m_pkMobInst->m_posLastAttacked.x - GetX(), m_pkMobInst->m_posLastAttacked.y - GetY()))
					if (Return())
						return true;
			}
		}
	}

#ifndef ENABLE_BUG_FIXES
	if (IsGuardNPC())
	{
		if (5000 < DISTANCE_APPROX(m_pkMobInst->m_posLastAttacked.x - GetX(), m_pkMobInst->m_posLastAttacked.y - GetY()))
			if (Return())
				return true;
	}
#endif

#ifdef __NEWPET_SYSTEM__
	if (pkChr->IsState(pkChr->m_stateMove) &&
		GetMobBattleType() != BATTLE_TYPE_RANGE &&
		GetMobBattleType() != BATTLE_TYPE_MAGIC &&
		false == IsPet() && false == IsNewPet()
#else
	if (pkChr->IsState(pkChr->m_stateMove) && 
		GetMobBattleType() != BATTLE_TYPE_RANGE && 
		GetMobBattleType() != BATTLE_TYPE_MAGIC &&
		false == IsPet()
#endif
		)


	{
		// ����� �̵����̸� ���� �̵��� �Ѵ�
		// ���� ������ �ӵ����� �Ÿ��κ��� ���� �ð��� ������ ��
		// ������ �� �ð����� �������� �̵��Ѵٰ� �����Ͽ� �ű�� �̵��Ѵ�.
		float rot = pkChr->GetRotation();
		float rot_delta = GetDegreeDelta(rot, GetDegreeFromPositionXY(GetX(), GetY(), pkChr->GetX(), pkChr->GetY()));

		float yourSpeed = pkChr->GetMoveSpeed();
		float mySpeed = GetMoveSpeed();

		float fDist = DISTANCE_SQRT(x - GetX(), y - GetY());
		float fFollowSpeed = mySpeed - yourSpeed * cos(rot_delta * M_PI / 180);

		if (fFollowSpeed >= 0.1f)
		{
			float fMeetTime = fDist / fFollowSpeed;
			float fYourMoveEstimateX, fYourMoveEstimateY;

			if( fMeetTime * yourSpeed <= 100000.0f )
			{
				GetDeltaByDegree(pkChr->GetRotation(), fMeetTime * yourSpeed, &fYourMoveEstimateX, &fYourMoveEstimateY);

				x += (long) fYourMoveEstimateX;
				y += (long) fYourMoveEstimateY;

				float fDistNew = sqrt(((double)x - GetX())*(x-GetX())+((double)y - GetY())*(y-GetY()));
				if (fDist < fDistNew)
				{
					x = (long)(GetX() + (x - GetX()) * fDist / fDistNew);
					y = (long)(GetY() + (y - GetY()) * fDist / fDistNew);
				}
			}
		}
	}

	// ������ ��ġ�� �ٶ���� �Ѵ�.
	SetRotationToXY(x, y);

	float fDist = DISTANCE_SQRT(x - GetX(), y - GetY());

	if (fDist <= fMinDistance)
		return false;

	float fx, fy;

	if (IsChangeAttackPosition(pkChr) && GetMobRank() < MOB_RANK_BOSS)
	{
		// ���� �ֺ� ������ ������ �̵�
		SetChangeAttackPositionTime();

		int retry = 16;
		int dx, dy;
		int rot = (int) GetDegreeFromPositionXY(x, y, GetX(), GetY());

		while (--retry)
		{
			if (fDist < 500.0f)
				GetDeltaByDegree((rot + number(-90, 90) + number(-90, 90)) % 360, fMinDistance, &fx, &fy);
			else
				GetDeltaByDegree(number(0, 359), fMinDistance, &fx, &fy);

			dx = x + (int) fx;
			dy = y + (int) fy;

			LPSECTREE tree = SECTREE_MANAGER::instance().Get(GetMapIndex(), dx, dy);

			if (NULL == tree)
				break;

			if (0 == (tree->GetAttribute(dx, dy) & (ATTR_BLOCK | ATTR_OBJECT)))
				break;
		}

		//sys_log(0, "��ó ��򰡷� �̵� %s retry %d", GetName(), retry);
		if (!Goto(dx, dy))
			return false;
	}
	else
	{
		// ���� ���󰡱�
		float fDistToGo = fDist - fMinDistance;
		GetDeltaByDegree(GetRotation(), fDistToGo, &fx, &fy);

		//sys_log(0, "�������� �̵� %s", GetName());
		if (!Goto(GetX() + (int) fx, GetY() + (int) fy))
			return false;
	}

	SendMovePacket(FUNC_WAIT, 0, 0, 0, 0);
	//MonsterLog("�Ѿư���; %s", pkChr->GetName());
	return true;
}


float CHARACTER::GetDistanceFromSafeboxOpen() const
{
	return DISTANCE_APPROX(GetX() - m_posSafeboxOpen.x, GetY() - m_posSafeboxOpen.y);
}

void CHARACTER::SetSafeboxOpenPosition()
{
	m_posSafeboxOpen = GetXYZ();
}

CSafebox * CHARACTER::GetSafebox() const
{
	return m_pkSafebox;
}

void CHARACTER::ReqSafeboxLoad(const char* pszPassword)
{
	if (!*pszPassword || strlen(pszPassword) > SAFEBOX_PASSWORD_MAX_LEN)
	{
#ifdef TEXTS_IMPROVEMENT
		ChatPacketNew(CHAT_TYPE_INFO, 188, "");
#endif
		return;
	}
	else if (m_pkSafebox)
	{
#ifdef TEXTS_IMPROVEMENT
		ChatPacketNew(CHAT_TYPE_INFO, 189, "");
#endif
		return;
	}

	int iPulse = thecore_pulse();

	if (iPulse - GetSafeboxLoadTime()  < PASSES_PER_SEC(10))
	{
#ifdef TEXTS_IMPROVEMENT
		ChatPacketNew(CHAT_TYPE_INFO, 190, "");
#endif
		return;
	}
#ifndef __OPEN_SAFEBOX_CLICK__
	else if (GetDistanceFromSafeboxOpen() > 1000)
	{
#ifdef TEXTS_IMPROVEMENT
		ChatPacketNew(CHAT_TYPE_INFO, 185, "");
#endif
		return;
	}
#endif
	else if (m_bOpeningSafebox)
	{
		sys_log(0, "Overlapped safebox load request from %s", GetName());
		return;
	}

	SetSafeboxLoadTime();
	m_bOpeningSafebox = true;

	TSafeboxLoadPacket p;
	p.dwID = GetDesc()->GetAccountTable().id;
	strlcpy(p.szLogin, GetDesc()->GetAccountTable().login, sizeof(p.szLogin));
	strlcpy(p.szPassword, pszPassword, sizeof(p.szPassword));

	db_clientdesc->DBPacket(HEADER_GD_SAFEBOX_LOAD, GetDesc()->GetHandle(), &p, sizeof(p));
}

void CHARACTER::LoadSafebox(int iSize, DWORD dwGold, int iItemCount, TPlayerItem * pItems)
{
	bool bLoaded = false;

	//PREVENT_TRADE_WINDOW
	SetOpenSafebox(true);
	//END_PREVENT_TRADE_WINDOW

	if (m_pkSafebox)
		bLoaded = true;

	if (!m_pkSafebox)
		m_pkSafebox = M2_NEW CSafebox(this, iSize, dwGold);
	else
		m_pkSafebox->ChangeSize(iSize);

	m_iSafeboxSize = iSize;

	TPacketCGSafeboxSize p;

	p.bHeader = HEADER_GC_SAFEBOX_SIZE;
	p.bSize = iSize;

	GetDesc()->Packet(&p, sizeof(TPacketCGSafeboxSize));

	if (!bLoaded)
	{
		for (int i = 0; i < iItemCount; ++i, ++pItems)
		{
			if (!m_pkSafebox->IsValidPosition(pItems->pos))
				continue;

			LPITEM item = ITEM_MANAGER::instance().CreateItem(pItems->vnum, pItems->count, pItems->id);

			if (!item)
			{
				sys_err("cannot create item vnum %d id %u (name: %s)", pItems->vnum, pItems->id, GetName());
				continue;
			}

			item->SetSkipSave(true);
			item->SetSockets(pItems->alSockets);
			item->SetAttributes(pItems->aAttr);

			if (!m_pkSafebox->Add(pItems->pos, item))
			{
				M2_DESTROY_ITEM(item);
			}
			else
				item->SetSkipSave(false);
		}
	}
}

void CHARACTER::ChangeSafeboxSize(BYTE bSize)
{
	//if (!m_pkSafebox)
	//return;

	TPacketCGSafeboxSize p;

	p.bHeader = HEADER_GC_SAFEBOX_SIZE;
	p.bSize = bSize;

	GetDesc()->Packet(&p, sizeof(TPacketCGSafeboxSize));

	if (m_pkSafebox)
		m_pkSafebox->ChangeSize(bSize);

	m_iSafeboxSize = bSize;
}

void CHARACTER::CloseSafebox()
{
	if (!m_pkSafebox)
		return;

	//PREVENT_TRADE_WINDOW
	SetOpenSafebox(false);
	//END_PREVENT_TRADE_WINDOW

	m_pkSafebox->Save();

	M2_DELETE(m_pkSafebox);
	m_pkSafebox = NULL;

	ChatPacket(CHAT_TYPE_COMMAND, "CloseSafebox");

	SetSafeboxLoadTime();
	m_bOpeningSafebox = false;

	Save();
}

CSafebox * CHARACTER::GetMall() const
{
	return m_pkMall;
}

void CHARACTER::LoadMall(int iItemCount, TPlayerItem * pItems)
{
	bool bLoaded = false;

	if (m_pkMall)
		bLoaded = true;

	if (!m_pkMall)
		m_pkMall = M2_NEW CSafebox(this, 3 * SAFEBOX_PAGE_SIZE, 0);
	else
		m_pkMall->ChangeSize(3 * SAFEBOX_PAGE_SIZE);

	m_pkMall->SetWindowMode(MALL);

	TPacketCGSafeboxSize p;

	p.bHeader = HEADER_GC_MALL_OPEN;
	p.bSize = 3 * SAFEBOX_PAGE_SIZE;

	GetDesc()->Packet(&p, sizeof(TPacketCGSafeboxSize));

	if (!bLoaded)
	{
		for (int i = 0; i < iItemCount; ++i, ++pItems)
		{
			if (!m_pkMall->IsValidPosition(pItems->pos))
				continue;

			LPITEM item = ITEM_MANAGER::instance().CreateItem(pItems->vnum, pItems->count, pItems->id);

			if (!item)
			{
				sys_err("cannot create item vnum %d id %u (name: %s)", pItems->vnum, pItems->id, GetName());
				continue;
			}

			item->SetSkipSave(true);
			item->SetSockets(pItems->alSockets);
			item->SetAttributes(pItems->aAttr);

			if (!m_pkMall->Add(pItems->pos, item))
				M2_DESTROY_ITEM(item);
			else
				item->SetSkipSave(false);
		}
	}
}

void CHARACTER::CloseMall()
{
	if (!m_pkMall)
		return;

	m_pkMall->Save();

	M2_DELETE(m_pkMall);
	m_pkMall = NULL;

	ChatPacket(CHAT_TYPE_COMMAND, "CloseMall");
}

bool CHARACTER::BuildUpdatePartyPacket(TPacketGCPartyUpdate & out)
{
	if (!GetParty())
		return false;

	memset(&out, 0, sizeof(out));

	out.header		= HEADER_GC_PARTY_UPDATE;
	out.pid		= GetPlayerID();
	if (GetMaxHP() <= 0) // @fixme136
		out.percent_hp	= 0;
	else
		out.percent_hp	= MINMAX(0, GetHP() * 100 / GetMaxHP(), 100);
	out.role		= GetParty()->GetRole(GetPlayerID());

	sys_log(1, "PARTY %s role is %d", GetName(), out.role);

	LPCHARACTER l = GetParty()->GetLeaderCharacter();

	if (l && DISTANCE_APPROX(GetX() - l->GetX(), GetY() - l->GetY()) < PARTY_DEFAULT_RANGE)
	{
		out.affects[0] = GetParty()->GetPartyBonusExpPercent();
		out.affects[1] = GetPoint(POINT_PARTY_ATTACKER_BONUS);
		out.affects[2] = GetPoint(POINT_PARTY_TANKER_BONUS);
		out.affects[3] = GetPoint(POINT_PARTY_BUFFER_BONUS);
		out.affects[4] = GetPoint(POINT_PARTY_SKILL_MASTER_BONUS);
		out.affects[5] = GetPoint(POINT_PARTY_HASTE_BONUS);
		out.affects[6] = GetPoint(POINT_PARTY_DEFENDER_BONUS);
	}

	return true;
}

int CHARACTER::GetLeadershipSkillLevel() const
{
	return GetSkillLevel(SKILL_LEADERSHIP);
}

void CHARACTER::QuerySafeboxSize()
{
	if (m_iSafeboxSize == -1)
	{
		DBManager::instance().ReturnQuery(QID_SAFEBOX_SIZE,
				GetPlayerID(),
				NULL,
				"SELECT size FROM safebox%s WHERE account_id = %u",
				get_table_postfix(),
				GetDesc()->GetAccountTable().id);
	}
}

void CHARACTER::SetSafeboxSize(int iSize)
{
	sys_log(1, "SetSafeboxSize: %s %d", GetName(), iSize);
	m_iSafeboxSize = iSize;
	DBManager::instance().Query("UPDATE safebox%s SET size = %d WHERE account_id = %u", get_table_postfix(), iSize / SAFEBOX_PAGE_SIZE, GetDesc()->GetAccountTable().id);
}

int CHARACTER::GetSafeboxSize() const
{
	return m_iSafeboxSize;
}

void CHARACTER::SetNowWalking(bool bWalkFlag)
{
	//if (m_bNowWalking != bWalkFlag || IsNPC())
	if (m_bNowWalking != bWalkFlag)
	{
		if (bWalkFlag)
		{
			m_bNowWalking = true;
			m_dwWalkStartTime = get_dword_time();
		}
		else
		{
			m_bNowWalking = false;
		}

		//if (m_bNowWalking)
		{
			TPacketGCWalkMode p;
			p.vid = GetVID();
			p.header = HEADER_GC_WALK_MODE;
			p.mode = m_bNowWalking ? WALKMODE_WALK : WALKMODE_RUN;

			PacketView(&p, sizeof(p));
		}

		if (IsNPC())
		{
			if (m_bNowWalking)
				MonsterLog("�ȴ´�");
			else
				MonsterLog("�ڴ�");
		}

		//sys_log(0, "%s is now %s", GetName(), m_bNowWalking?"walking.":"running.");
	}
}

void CHARACTER::StartStaminaConsume()
{
	if (m_bStaminaConsume)
		return;
	PointChange(POINT_STAMINA, 0);
	m_bStaminaConsume = true;
	//ChatPacket(CHAT_TYPE_COMMAND, "StartStaminaConsume %d %d", STAMINA_PER_STEP * passes_per_sec, GetStamina());
	if (IsStaminaHalfConsume())
		ChatPacket(CHAT_TYPE_COMMAND, "StartStaminaConsume %d %d", STAMINA_PER_STEP * passes_per_sec / 2, GetStamina());
	else
		ChatPacket(CHAT_TYPE_COMMAND, "StartStaminaConsume %d %d", STAMINA_PER_STEP * passes_per_sec, GetStamina());
}

void CHARACTER::StopStaminaConsume()
{
	if (!m_bStaminaConsume)
		return;
	PointChange(POINT_STAMINA, 0);
	m_bStaminaConsume = false;
	ChatPacket(CHAT_TYPE_COMMAND, "StopStaminaConsume %d", GetStamina());
}

bool CHARACTER::IsStaminaConsume() const
{
	return m_bStaminaConsume;
}

bool CHARACTER::IsStaminaHalfConsume() const
{
	return IsEquipUniqueItem(UNIQUE_ITEM_HALF_STAMINA);
}

void CHARACTER::ResetStopTime()
{
	m_dwStopTime = get_dword_time();
}

DWORD CHARACTER::GetStopTime() const
{
	return m_dwStopTime;
}

void CHARACTER::ResetPoint(int iLv)
{
	BYTE bJob = GetJob();

	PointChange(POINT_LEVEL, iLv - GetLevel());

	SetRealPoint(POINT_ST, JobInitialPoints[bJob].st);
	SetPoint(POINT_ST, GetRealPoint(POINT_ST));

	SetRealPoint(POINT_HT, JobInitialPoints[bJob].ht);
	SetPoint(POINT_HT, GetRealPoint(POINT_HT));

	SetRealPoint(POINT_DX, JobInitialPoints[bJob].dx);
	SetPoint(POINT_DX, GetRealPoint(POINT_DX));

	SetRealPoint(POINT_IQ, JobInitialPoints[bJob].iq);
	SetPoint(POINT_IQ, GetRealPoint(POINT_IQ));

	SetRandomHP((iLv - 1) * number(JobInitialPoints[GetJob()].hp_per_lv_begin, JobInitialPoints[GetJob()].hp_per_lv_end));
	SetRandomSP((iLv - 1) * number(JobInitialPoints[GetJob()].sp_per_lv_begin, JobInitialPoints[GetJob()].sp_per_lv_end));

	// @fixme104
	int iLvl = iLv;
#ifdef ENABLE_STATUS_MAX_344_POINTS
	if (iLvl > 0)
		iLvl -= 1;
#endif
	PointChange(POINT_STAT, (MINMAX(1, iLvl, g_iStatusPointGetLevelLimit) * 3) + GetPoint(POINT_LEVEL_STEP) - GetPoint(POINT_STAT));

	ComputePoints();

	// ȸ��
	PointChange(POINT_HP, GetMaxHP() - GetHP());
	PointChange(POINT_SP, GetMaxSP() - GetSP());

	PointsPacket();

	LogManager::instance().CharLog(this, 0, "RESET_POINT", "");
}

bool CHARACTER::IsChangeAttackPosition(LPCHARACTER target) const
{
	if (!IsNPC())
		return true;

	DWORD dwChangeTime = AI_CHANGE_ATTACK_POISITION_TIME_NEAR;

	if (DISTANCE_APPROX(GetX() - target->GetX(), GetY() - target->GetY()) >
		AI_CHANGE_ATTACK_POISITION_DISTANCE + GetMobAttackRange())
		dwChangeTime = AI_CHANGE_ATTACK_POISITION_TIME_FAR;

	return get_dword_time() - m_dwLastChangeAttackPositionTime > dwChangeTime;
}

void CHARACTER::GiveRandomSkillBook()
{
	LPITEM item = AutoGiveItem(50300);

	if (NULL != item)
	{
		extern const DWORD GetRandomSkillVnum(BYTE bJob = JOB_MAX_NUM);
		DWORD dwSkillVnum = 0;
		// 50% of getting random books or getting one of the same player's race
		if (!number(0, 1))
			dwSkillVnum = GetRandomSkillVnum(GetJob());
		else
			dwSkillVnum = GetRandomSkillVnum();
		item->SetSocket(0, dwSkillVnum);
	}
}

void CHARACTER::ReviveInvisible(int iDur)
{
	AddAffect(AFFECT_REVIVE_INVISIBLE, POINT_NONE, 0, AFF_REVIVE_INVISIBLE, iDur, 0, true);
}

void CHARACTER::ToggleMonsterLog()
{
	m_bMonsterLog = !m_bMonsterLog;

	if (m_bMonsterLog)
	{
		CHARACTER_MANAGER::instance().RegisterForMonsterLog(this);
	}
	else
	{
		CHARACTER_MANAGER::instance().UnregisterForMonsterLog(this);
	}
}

void CHARACTER::SetGuild(CGuild* pGuild)
{
	if (m_pGuild != pGuild)
	{
		m_pGuild = pGuild;
		UpdatePacket();
	}
}

void CHARACTER::SendGreetMessage()
{
	__typeof(DBManager::instance().GetGreetMessage()) v = DBManager::instance().GetGreetMessage();

	for (itertype(v) it = v.begin(); it != v.end(); ++it)
	{
		ChatPacket(CHAT_TYPE_NOTICE, it->c_str());
	}
}

void CHARACTER::BeginStateEmpty()
{
	MonsterLog("!");
}

void CHARACTER::EffectPacket(int enumEffectType)
{
	TPacketGCSpecialEffect p;

	p.header = HEADER_GC_SEPCIAL_EFFECT;
	p.type = enumEffectType;
	p.vid = GetVID();

	PacketAround(&p, sizeof(TPacketGCSpecialEffect));
}

void CHARACTER::SpecificEffectPacket(const char filename[MAX_EFFECT_FILE_NAME])
{
	TPacketGCSpecificEffect p;

	p.header = HEADER_GC_SPECIFIC_EFFECT;
	p.vid = GetVID();
	memcpy (p.effect_file, filename, MAX_EFFECT_FILE_NAME);

	PacketAround(&p, sizeof(TPacketGCSpecificEffect));
}

void CHARACTER::SetQuestNPCID(DWORD vid)
{
	m_dwQuestNPCVID = vid;
}

LPCHARACTER CHARACTER::GetQuestNPC() const
{
	return CHARACTER_MANAGER::instance().Find(m_dwQuestNPCVID);
}

void CHARACTER::SetQuestItemPtr(LPITEM item)
{
	m_pQuestItem = item;
}

void CHARACTER::ClearQuestItemPtr()
{
	m_pQuestItem = NULL;
}

LPITEM CHARACTER::GetQuestItemPtr() const
{
	return m_pQuestItem;
}

LPDUNGEON CHARACTER::GetDungeonForce() const
{
	if (m_lWarpMapIndex > 10000)
		return CDungeonManager::instance().FindByMapIndex(m_lWarpMapIndex);

	return m_pkDungeon;
}

void CHARACTER::SetBlockMode(BYTE bFlag)
{
	m_pointsInstant.bBlockMode = bFlag;

	ChatPacket(CHAT_TYPE_COMMAND, "setblockmode %d", m_pointsInstant.bBlockMode);

	SetQuestFlag("game_option.block_exchange", bFlag & BLOCK_EXCHANGE ? 1 : 0);
	SetQuestFlag("game_option.block_party_invite", bFlag & BLOCK_PARTY_INVITE ? 1 : 0);
	SetQuestFlag("game_option.block_guild_invite", bFlag & BLOCK_GUILD_INVITE ? 1 : 0);
	SetQuestFlag("game_option.block_whisper", bFlag & BLOCK_WHISPER ? 1 : 0);
	SetQuestFlag("game_option.block_messenger_invite", bFlag & BLOCK_MESSENGER_INVITE ? 1 : 0);
	SetQuestFlag("game_option.block_party_request", bFlag & BLOCK_PARTY_REQUEST ? 1 : 0);
}

void CHARACTER::SetBlockModeForce(BYTE bFlag)
{
	m_pointsInstant.bBlockMode = bFlag;
	ChatPacket(CHAT_TYPE_COMMAND, "setblockmode %d", m_pointsInstant.bBlockMode);
}

bool CHARACTER::IsGuardNPC() const
{
	return IsNPC() && (GetRaceNum() == 11000 || GetRaceNum() == 11002 || GetRaceNum() == 11004);
}

int CHARACTER::GetPolymorphPower() const
{
	if (test_server)
	{
		int value = quest::CQuestManager::instance().GetEventFlag("poly");
		if (value)
			return value;
	}
	return aiPolymorphPowerByLevel[MINMAX(0, GetSkillLevel(SKILL_POLYMORPH), 40)];
}

void CHARACTER::SetPolymorph(DWORD dwRaceNum, bool bMaintainStat)
{
#ifdef ENABLE_WOLFMAN_CHARACTER
	if (dwRaceNum < MAIN_RACE_MAX_NUM)
#else
	if (dwRaceNum < JOB_MAX_NUM)
#endif
	{
		dwRaceNum = 0;
		bMaintainStat = false;
	}

	if (m_dwPolymorphRace == dwRaceNum)
		return;

	m_bPolyMaintainStat = bMaintainStat;
	m_dwPolymorphRace = dwRaceNum;

	sys_log(0, "POLYMORPH: %s race %u ", GetName(), dwRaceNum);

	if (dwRaceNum != 0)
		StopRiding();

	SET_BIT(m_bAddChrState, ADD_CHARACTER_STATE_SPAWN);
	m_afAffectFlag.Set(AFF_SPAWN);

	ViewReencode();

	REMOVE_BIT(m_bAddChrState, ADD_CHARACTER_STATE_SPAWN);

	if (!bMaintainStat)
	{
		PointChange(POINT_ST, 0);
		PointChange(POINT_DX, 0);
		PointChange(POINT_IQ, 0);
		PointChange(POINT_HT, 0);
	}

	// �������� ���¿��� �״� ���, ���������� Ǯ���� �Ǵµ�
	// ���� ���� ���ķ� valid combo interval�� �ٸ��� ������
	// Combo �� �Ǵ� Hacker�� �ν��ϴ� ��찡 �ִ�.
	// ���� ���������� Ǯ�ų� �������� �ϰ� �Ǹ�,
	// valid combo interval�� reset�Ѵ�.
	SetValidComboInterval(0);
	SetComboSequence(0);

	ComputeBattlePoints();
}

int CHARACTER::GetQuestFlag(const std::string& flag) const
{
	int ret = 0;
	quest::CQuestManager& q = quest::CQuestManager::instance();
	quest::PC* pPC = q.GetPC(GetPlayerID());
	if (pPC)
		ret = pPC->GetFlag(flag);
	
	return ret;
}

void CHARACTER::SetQuestFlag(const std::string& flag, int value)
{
	quest::CQuestManager& q = quest::CQuestManager::instance();
	quest::PC* pPC = q.GetPC(GetPlayerID());
	pPC->SetFlag(flag, value);
}

void CHARACTER::DetermineDropMetinStone()
{
#ifdef ENABLE_NEWSTUFF
	if (g_NoDropMetinStone)
	{
		m_dwDropMetinStone = 0;
		return;
	}
#endif

	static const DWORD c_adwMetin[] =
	{
#if defined(ENABLE_WOLFMAN_CHARACTER) && defined(USE_WOLFMAN_STONES)
		28012,
#endif
		28030,
		28031,
		28032,
		28033,
		28034,
		28035,
		28036,
		28037,
		28038,
		28039,
		28040,
		28041,
		28042,
		28043,
#if defined(ENABLE_MAGIC_REDUCTION_SYSTEM) && defined(USE_MAGIC_REDUCTION_STONES)
		28044,
		28045,
#endif
	};
	DWORD stone_num = GetRaceNum();
	int idx = std::lower_bound(aStoneDrop, aStoneDrop+STONE_INFO_MAX_NUM, stone_num) - aStoneDrop;
	if (idx >= STONE_INFO_MAX_NUM || aStoneDrop[idx].dwMobVnum != stone_num)
	{
		m_dwDropMetinStone = 0;
	}
	else
	{
		const SStoneDropInfo & info = aStoneDrop[idx];
		m_bDropMetinStonePct = info.iDropPct;
		{
			m_dwDropMetinStone = c_adwMetin[number(0, sizeof(c_adwMetin)/sizeof(DWORD) - 1)];
			int iGradePct = number(1, 100);
			for (int iStoneLevel = 0; iStoneLevel < STONE_LEVEL_MAX_NUM; iStoneLevel ++)
			{
				int iLevelGradePortion = info.iLevelPct[iStoneLevel];
				if (iGradePct <= iLevelGradePortion)
				{
					break;
				}
				else
				{
					iGradePct -= iLevelGradePortion;
					m_dwDropMetinStone += 100; // �� +a -> +(a+1)�� �ɶ����� 100�� ����
				}
			}
		}
	}
}

void CHARACTER::DetermineDropMetinStofa()
{
	static const DWORD c_adwMetin[STOFA_MAX_NUM] = {
										80019,
										80022,
										80023,
										80024,
										80025,
										80026,
										80027,
	};
	
	DWORD stone_num = GetRaceNum();
	int idx = std::lower_bound(aStofaDrop, aStofaDrop+STONE_STOFA_INFO_MAX_NUM, stone_num) - aStofaDrop;
	if ((idx >= STONE_STOFA_INFO_MAX_NUM) || (aStofaDrop[idx].dwMobVnum != stone_num)) {
		m_dwDropMetinStofa = 0;
	} else {
		const SStofaDropInfo & info = aStofaDrop[idx];
		int random = number(0, STOFA_MAX_NUM-1);
		m_dwDropMetinStofa = c_adwMetin[random];
		m_bDropMetinStofaPct = info.iChance[random];
	}
}

void CHARACTER::DetermineDropMetinSacca()
{
	static const DWORD c_adwMetin[SACCA_MAX_NUM] = {
										30094,
										30095,
										30096,
	};
	
	DWORD stone_num = GetRaceNum();
	int idx = std::lower_bound(aSaccaDrop, aSaccaDrop+STONE_SACCA_INFO_MAX_NUM, stone_num) - aSaccaDrop;
	if ((idx >= STONE_SACCA_INFO_MAX_NUM) || (aSaccaDrop[idx].dwMobVnum != stone_num)) {
		m_dwDropMetinSacca = 0;
	} else {
		const SSaccaDropInfo & info = aSaccaDrop[idx];
		int random = number(0, SACCA_MAX_NUM-1);
		m_dwDropMetinSacca = c_adwMetin[random];
		m_bDropMetinSaccaPct = info.iChance[random];
	}
}

#ifdef ENABLE_CHRISTMAS_2021
void CHARACTER::DetermineDropMetinSock()
{
	static const DWORD c_adwSock[SOCKS_MAX_NUM] = {
										78201,
										78202,
										78203,
										78204,
	};

	DWORD stone_num = GetRaceNum();
	int idx = std::lower_bound(aSockDrop, aSockDrop+SOCKS_INFO_MAX_NUM, stone_num) - aSockDrop;
	if (idx >= SOCKS_INFO_MAX_NUM || aSockDrop[idx].dwMobVnum != stone_num)
	{
		m_dwDropMetinSock = 0;
	}
	else
	{
		int random = number(0, SOCKS_MAX_NUM-1);
		m_dwDropMetinSock = c_adwSock[random];
	}
}
#endif

void CHARACTER::SendEquipment(LPCHARACTER ch)
{
	TPacketViewEquip p;
	p.header = HEADER_GC_VIEW_EQUIP;
	p.vid    = GetVID();

#ifdef EQUIP_ENABLE_VIEW_SASH
	int pos[23] = {	WEAR_BODY, WEAR_HEAD, WEAR_FOOTS, WEAR_WRIST, WEAR_WEAPON, WEAR_NECK, WEAR_EAR,	WEAR_UNIQUE1,
					WEAR_UNIQUE2, WEAR_ARROW, WEAR_SHIELD, WEAR_COSTUME_BODY, WEAR_COSTUME_HAIR, WEAR_RING1, WEAR_RING2, WEAR_BELT, WEAR_COSTUME_ACCE_SLOT, WEAR_COSTUME_ACCE, WEAR_COSTUME_WEAPON, WEAR_COSTUME_PET_SKIN, WEAR_COSTUME_MOUNT_SKIN, WEAR_COSTUME_EFFECT_BODY, WEAR_COSTUME_EFFECT_WEAPON};
	for (int i = 0; i < 23; i++)
#else
	int pos[16] = {	WEAR_BODY, WEAR_HEAD, WEAR_FOOTS, WEAR_WRIST, WEAR_WEAPON, WEAR_NECK, WEAR_EAR, WEAR_UNIQUE1,
					WEAR_UNIQUE2, WEAR_ARROW, WEAR_SHIELD, WEAR_COSTUME_BODY, WEAR_COSTUME_HAIR, WEAR_RING1, WEAR_RING2, WEAR_BELT};
	for (int i = 0; i < 16; i++)	
#endif
	{
		LPITEM item = GetWear(pos[i]);
		if (item) {
			p.equips[i].vnum = item->GetVnum();
			p.equips[i].count = item->GetCount();

			thecore_memcpy(p.equips[i].alSockets, item->GetSockets(), sizeof(p.equips[i].alSockets));
			thecore_memcpy(p.equips[i].aAttr, item->GetAttributes(), sizeof(p.equips[i].aAttr));
		}
		else {
			p.equips[i].vnum = 0;
		}
	}
	ch->GetDesc()->Packet(&p, sizeof(p));
}

bool CHARACTER::CanSummon(int iLeaderShip)
{
	return ((iLeaderShip >= 20) || ((iLeaderShip >= 12) && ((m_dwLastDeadTime + 180) > get_dword_time())));
}

#ifdef ENABLE_PVP_ADVANCED	
int CHARACTER::GetDuel(const char* type) const
{
	const char* szTableStaticPvP[] = {BLOCK_CHANGEITEM, BLOCK_BUFF, BLOCK_POTION, BLOCK_RIDE, BLOCK_PET, BLOCK_POLY, BLOCK_PARTY, BLOCK_EXCHANGE_, BET_WINNER, CHECK_IS_FIGHT};
	
	int m_nDuelTable[] = {(GetQuestFlag(szTableStaticPvP[0])), (GetQuestFlag(szTableStaticPvP[1])), (GetQuestFlag(szTableStaticPvP[2])), (GetQuestFlag(szTableStaticPvP[3])), (GetQuestFlag(szTableStaticPvP[4])), (GetQuestFlag(szTableStaticPvP[5])), (GetQuestFlag(szTableStaticPvP[6])), (GetQuestFlag(szTableStaticPvP[7])), (GetQuestFlag(szTableStaticPvP[8])), (GetQuestFlag(szTableStaticPvP[9]))};
	
	if (!strcmp(type, "BlockChangeItem") && m_nDuelTable[0] > 0) {
		return true;
	}	
	if (!strcmp(type, "BlockBuff") && m_nDuelTable[1] > 0) {
		return true;
	}	
	if (!strcmp(type, "BlockPotion") && m_nDuelTable[2] > 0) {
		return true;
	}	
	if (!strcmp(type, "BlockRide") && m_nDuelTable[3] > 0) {
		return true;
	}	
	if (!strcmp(type, "BlockPet") && m_nDuelTable[4] > 0) {
		return true;
	}	
	if (!strcmp(type, "BlockPoly") && m_nDuelTable[5] > 0) {
		return true;
	}	
	if (!strcmp(type, "BlockParty") && m_nDuelTable[6] > 0) {
		return true;
	}	
	if (!strcmp(type, "BlockExchange") && m_nDuelTable[7] > 0) {
		return true;
	}	
	if (!strcmp(type, "BetMoney") && m_nDuelTable[8] > 0) {
		return true;
	}	
	if (!strcmp(type, "IsFight") && m_nDuelTable[9] > 0) {
		return true;
	}	
	return false;
}

void CHARACTER::SetDuel(const char* type, int value)
{	
	const char* szTableStaticPvP[] = {BLOCK_CHANGEITEM, BLOCK_BUFF, BLOCK_POTION, BLOCK_RIDE, BLOCK_PET, BLOCK_POLY, BLOCK_PARTY, BLOCK_EXCHANGE_, BET_WINNER, CHECK_IS_FIGHT};

	if (!strcmp(type, "BlockChangeItem")) {
		SetQuestFlag(szTableStaticPvP[0], value);
	}	
	if (!strcmp(type, "BlockBuff")) {
		SetQuestFlag(szTableStaticPvP[1], value);
	}	
	if (!strcmp(type, "BlockPotion")) {
		SetQuestFlag(szTableStaticPvP[2], value);
	}	
	if (!strcmp(type, "BlockRide")) {
		SetQuestFlag(szTableStaticPvP[3], value);
	}	
	if (!strcmp(type, "BlockPet")) {
		SetQuestFlag(szTableStaticPvP[4], value);
	}	
	if (!strcmp(type, "BlockPoly")) {
		SetQuestFlag(szTableStaticPvP[5], value);
	}	
	if (!strcmp(type, "BlockParty")) {
		SetQuestFlag(szTableStaticPvP[6], value);
	}	
	if (!strcmp(type, "BlockExchange")) {
		SetQuestFlag(szTableStaticPvP[7], value);
	}	
	if (!strcmp(type, "BetMoney")) {
		SetQuestFlag(szTableStaticPvP[8], value);
	}	
	if (!strcmp(type, "IsFight")) {
		SetQuestFlag(szTableStaticPvP[9], value);
	}			
}
#endif

void CHARACTER::MountVnum(DWORD vnum)
{
	if (m_dwMountVnum == vnum)
		return;
	if ((m_dwMountVnum != 0)&&(vnum!=0)) //@fixme108 set recursively to 0 for eventuality
		MountVnum(0);

	m_dwMountVnum = vnum;
	m_dwMountTime = get_dword_time();

	if (m_bIsObserver)
		return;

	//NOTE : Mount�Ѵٰ� �ؼ� Client Side�� ��ü�� �������� �ʴ´�.
	//�׸��� ����Side���� ������ ��ġ �̵��� ���� �ʴ´�. �ֳ��ϸ� Client Side���� Coliision Adjust�� �Ҽ� �ִµ�
	//��ü�� �Ҹ���״ٰ� ������ġ�� �̵���Ű�� �̶� collision check�� ������ �����Ƿ� ��濡 ���ų� �հ� ������ ������ �����Ѵ�.
	m_posDest.x = m_posStart.x = GetX();
	m_posDest.y = m_posStart.y = GetY();
	//EncodeRemovePacket(this);
	EncodeInsertPacket(this);

	ENTITY_MAP::iterator it = m_map_view.begin();

	while (it != m_map_view.end())
	{
		LPENTITY entity = (it++)->first;

		//Mount�Ѵٰ� �ؼ� Client Side�� ��ü�� �������� �ʴ´�.
		//EncodeRemovePacket(entity);
		//if (!m_bIsObserver)
		EncodeInsertPacket(entity);

		//if (!entity->IsObserverMode())
		//	entity->EncodeInsertPacket(this);
	}

	SetValidComboInterval(0);
	SetComboSequence(0);

	ComputePoints();
}

namespace {
	class FuncCheckWarp
	{
		public:
			FuncCheckWarp(LPCHARACTER pkWarp)
			{
				m_lTargetY = 0;
				m_lTargetX = 0;

				m_lX = pkWarp->GetX();
				m_lY = pkWarp->GetY();

				m_bInvalid = false;
				m_bEmpire = pkWarp->GetEmpire();

				char szTmp[64];

				if (3 != sscanf(pkWarp->GetName(), " %s %ld %ld ", szTmp, &m_lTargetX, &m_lTargetY))
				{
					if (number(1, 100) < 5)
						sys_err("Warp NPC name wrong : vnum(%d) name(%s)", pkWarp->GetRaceNum(), pkWarp->GetName());

					m_bInvalid = true;

					return;
				}

				m_lTargetX *= 100;
				m_lTargetY *= 100;

				m_bUseWarp = true;

				if (pkWarp->IsGoto())
				{
					LPSECTREE_MAP pkSectreeMap = SECTREE_MANAGER::instance().GetMap(pkWarp->GetMapIndex());
					m_lTargetX += pkSectreeMap->m_setting.iBaseX;
					m_lTargetY += pkSectreeMap->m_setting.iBaseY;
					m_bUseWarp = false;
				}
			}

			bool Valid()
			{
				return !m_bInvalid;
			}

			void operator () (LPENTITY ent)
			{
				if (!Valid())
					return;

				if (!ent->IsType(ENTITY_CHARACTER))
					return;

				LPCHARACTER pkChr = (LPCHARACTER) ent;

				if (!pkChr->IsPC())
					return;

				int iDist = DISTANCE_APPROX(pkChr->GetX() - m_lX, pkChr->GetY() - m_lY);

				if (iDist > 300)
					return;

				if (m_bEmpire && pkChr->GetEmpire() && m_bEmpire != pkChr->GetEmpire())
					return;

				if (pkChr->IsHack())
					return;

				if (!pkChr->CanHandleItem(false, true))
					return;

				if (m_bUseWarp)
					pkChr->WarpSet(m_lTargetX, m_lTargetY);
				else
				{
					pkChr->Show(pkChr->GetMapIndex(), m_lTargetX, m_lTargetY);
					pkChr->Stop();
				}
			}

			bool m_bInvalid;
			bool m_bUseWarp;

			long m_lX;
			long m_lY;
			long m_lTargetX;
			long m_lTargetY;

			BYTE m_bEmpire;
	};
}

EVENTFUNC(warp_npc_event)
{
	char_event_info* info = dynamic_cast<char_event_info*>( event->info );
	if ( info == NULL )
	{
		sys_err( "warp_npc_event> <Factor> Null pointer" );
		return 0;
	}

	LPCHARACTER	ch = info->ch;

	if (ch == NULL) { // <Factor>
		return 0;
	}

	if (!ch->GetSectree())
	{
		ch->m_pkWarpNPCEvent = NULL;
		return 0;
	}

	FuncCheckWarp f(ch);
	if (f.Valid())
		ch->GetSectree()->ForEachAround(f);

	return passes_per_sec / 2;
}


void CHARACTER::StartWarpNPCEvent()
{
	if (m_pkWarpNPCEvent)
		return;

	if (!IsWarp() && !IsGoto())
		return;

	char_event_info* info = AllocEventInfo<char_event_info>();

	info->ch = this;

	m_pkWarpNPCEvent = event_create(warp_npc_event, info, passes_per_sec / 2);
}

void CHARACTER::SyncPacket()
{
	TEMP_BUFFER buf;

	TPacketCGSyncPositionElement elem;

	elem.dwVID = GetVID();
	elem.lX = GetX();
	elem.lY = GetY();

	TPacketGCSyncPosition pack;

	pack.bHeader = HEADER_GC_SYNC_POSITION;
	pack.wSize = sizeof(TPacketGCSyncPosition) + sizeof(elem);

	buf.write(&pack, sizeof(pack));
	buf.write(&elem, sizeof(elem));

	PacketAround(buf.read_peek(), buf.size());
}

LPCHARACTER CHARACTER::GetMarryPartner() const
{
	return m_pkChrMarried;
}

void CHARACTER::SetMarryPartner(LPCHARACTER ch)
{
	m_pkChrMarried = ch;
}

int CHARACTER::GetMarriageBonus(DWORD dwItemVnum, bool bSum)
{
	if (IsNPC())
		return 0;

	marriage::TMarriage* pMarriage = marriage::CManager::instance().Get(GetPlayerID());

	if (!pMarriage)
		return 0;

	return pMarriage->GetBonus(dwItemVnum, bSum, this);
}

void CHARACTER::ConfirmWithMsg(const char* szMsg, int iTimeout, DWORD dwRequestPID)
{
	if (!IsPC())
		return;

	TPacketGCQuestConfirm p;

	p.header = HEADER_GC_QUEST_CONFIRM;
	p.requestPID = dwRequestPID;
	p.timeout = iTimeout;
	strlcpy(p.msg, szMsg, sizeof(p.msg));

	GetDesc()->Packet(&p, sizeof(p));
}

int CHARACTER::GetPremiumRemainSeconds(BYTE bType) const
{
	if (bType >= PREMIUM_MAX_NUM)
		return 0;

	return m_aiPremiumTimes[bType] - get_global_time();
}

bool CHARACTER::WarpToPID(DWORD dwPID)
{
	LPCHARACTER victim;
	if ((victim = (CHARACTER_MANAGER::instance().FindByPID(dwPID))))
	{
		int mapIdx = victim->GetMapIndex();
		if (IS_SUMMONABLE_ZONE(mapIdx))
		{
			if (CAN_ENTER_ZONE(this, mapIdx))
			{
				WarpSet(victim->GetX(), victim->GetY());
			}
			else
			{
#ifdef TEXTS_IMPROVEMENT
				ChatPacketNew(CHAT_TYPE_INFO, 372, "");
#endif
				return false;
			}
		}
		else
		{
#ifdef TEXTS_IMPROVEMENT
			ChatPacketNew(CHAT_TYPE_INFO, 372, "");
#endif
			return false;
		}
	}
	else
	{
		// �ٸ� ������ �α��ε� ����� ���� -> �޽��� ���� ��ǥ�� �޾ƿ���
		// 1. A.pid, B.pid �� �Ѹ�
		// 2. B.pid�� ���� ������ �Ѹ��������� A.pid, ��ǥ �� ����
		// 3. ����
		CCI * pcci = P2P_MANAGER::instance().FindByPID(dwPID);

		if (!pcci)
		{
#ifdef TEXTS_IMPROVEMENT
			ChatPacketNew(CHAT_TYPE_INFO, 371, "");
#endif
			return false;
		}

		if (pcci->bChannel != g_bChannel)
		{
#ifdef TEXTS_IMPROVEMENT
			ChatPacketNew(CHAT_TYPE_INFO, 367, "%d#%d", g_bChannel, pcci->bChannel);
#endif
			return false;
		}
		else if (false == IS_SUMMONABLE_ZONE(pcci->lMapIndex))
		{
#ifdef TEXTS_IMPROVEMENT
			ChatPacketNew(CHAT_TYPE_INFO, 372, "");
#endif
			return false;
		}
		else
		{
			if (!CAN_ENTER_ZONE(this, pcci->lMapIndex))
			{
#ifdef TEXTS_IMPROVEMENT
				ChatPacketNew(CHAT_TYPE_INFO, 372, "");
#endif
				return false;
			}

			TPacketGGFindPosition p;
			p.header = HEADER_GG_FIND_POSITION;
			p.dwFromPID = GetPlayerID();
			p.dwTargetPID = dwPID;
			pcci->pkDesc->Packet(&p, sizeof(TPacketGGFindPosition));

			if (test_server)
				ChatPacket(CHAT_TYPE_PARTY, "sent find position packet for teleport");
		}
	}
	return true;
}

// ADD_REFINE_BUILDING
CGuild* CHARACTER::GetRefineGuild() const
{
	LPCHARACTER chRefineNPC = CHARACTER_MANAGER::instance().Find(m_dwRefineNPCVID);

	return (chRefineNPC ? chRefineNPC->GetGuild() : NULL);
}

bool CHARACTER::IsRefineThroughGuild() const
{
	return GetRefineGuild() != NULL;
}

int CHARACTER::ComputeRefineFee(int iCost, int iMultiply) const
{
	CGuild* pGuild = GetRefineGuild();
	if (pGuild)
	{
		if (pGuild == GetGuild())
			return iCost * iMultiply * 9 / 10;

		// �ٸ� ���� ����� �õ��ϴ� ��� �߰��� 3�� ��
		LPCHARACTER chRefineNPC = CHARACTER_MANAGER::instance().Find(m_dwRefineNPCVID);
		if (chRefineNPC && chRefineNPC->GetEmpire() != GetEmpire())
			return iCost * iMultiply * 3;

		return iCost * iMultiply;
	}
	else
		return iCost;
}

void CHARACTER::PayRefineFee(int iTotalMoney)
{
	int iFee = iTotalMoney / 10;
	CGuild* pGuild = GetRefineGuild();

	int iRemain = iTotalMoney;

	if (pGuild)
	{
		// �ڱ� ����̸� iTotalMoney�� �̹� 10%�� ���ܵǾ��ִ�
		if (pGuild != GetGuild())
		{
			pGuild->RequestDepositMoney(this, iFee);
			iRemain -= iFee;
		}
	}

	PointChange(POINT_GOLD, -iRemain);
}
// END_OF_ADD_REFINE_BUILDING

//Hack ������ ���� üũ.
bool CHARACTER::IsHack(bool bSendMsg, bool bCheckShopOwner, int limittime)
{
	const int iPulse = thecore_pulse();

	if (test_server)
		bSendMsg = true;

	if (iPulse - GetSafeboxLoadTime() < PASSES_PER_SEC(limittime))
	{
#ifdef TEXTS_IMPROVEMENT
		if (bSendMsg) {
			ChatPacketNew(CHAT_TYPE_INFO, 234, "%d", limittime);
		}
#endif
		return true;
	}

	//�ŷ����� â üũ
	if (bCheckShopOwner)
	{
		if (GetExchange() || GetMyShop() || GetShopOwner() || IsOpenSafebox() || IsCubeOpen()
#if defined(ENABLE_CHRISTMAS_WHEEL_OF_DESTINY)
		 || GetWheelDestiny()
#endif
		)
		{
#ifdef TEXTS_IMPROVEMENT
			if (bSendMsg) {
				ChatPacketNew(CHAT_TYPE_INFO, 236, "");
			}
#endif
			return true;
		}
	}
	else
	{
		if (GetExchange() || GetMyShop() || IsOpenSafebox() || IsCubeOpen()
#if defined(ENABLE_CHRISTMAS_WHEEL_OF_DESTINY)
		 || GetWheelDestiny()
#endif
		)
		{
#ifdef TEXTS_IMPROVEMENT
			if (bSendMsg) {
				ChatPacketNew(CHAT_TYPE_INFO, 236, "");
			}
#endif
			return true;
		}
	}

	//PREVENT_PORTAL_AFTER_EXCHANGE
	//��ȯ �� �ð�üũ
	if (iPulse - GetExchangeTime()  < PASSES_PER_SEC(limittime))
	{
#ifdef TEXTS_IMPROVEMENT
		if (bSendMsg) {
			ChatPacketNew(CHAT_TYPE_INFO, 234, "%d", limittime);
		}
#endif
		return true;
	}
	//END_PREVENT_PORTAL_AFTER_EXCHANGE

	//PREVENT_ITEM_COPY
	if (iPulse - GetMyShopTime() < PASSES_PER_SEC(limittime))
	{
#ifdef TEXTS_IMPROVEMENT
		if (bSendMsg) {
			ChatPacketNew(CHAT_TYPE_INFO, 234, "%d", limittime);
		}
#endif
		return true;
	}

	if (iPulse - GetRefineTime() < PASSES_PER_SEC(limittime))
	{
#ifdef TEXTS_IMPROVEMENT
		if (bSendMsg) {
			ChatPacketNew(CHAT_TYPE_INFO, 234, "%d", limittime);
		}
#endif
		return true;
	}
	//END_PREVENT_ITEM_COPY

	return false;
}

void CHARACTER::Say(const std::string & s)
{
	struct ::packet_script packet_script;

	packet_script.header = HEADER_GC_SCRIPT;
	packet_script.skin = 1;
	packet_script.src_size = s.size();
	packet_script.size = packet_script.src_size + sizeof(struct packet_script);

	TEMP_BUFFER buf;

	buf.write(&packet_script, sizeof(struct packet_script));
	buf.write(&s[0], s.size());

	if (IsPC())
	{
		GetDesc()->Packet(buf.read_peek(), buf.size());
	}
}

//------------------------------------------------
void CHARACTER::UpdateDepositPulse()
{
	m_deposit_pulse = thecore_pulse() + PASSES_PER_SEC(60*5);	// 5��
}

bool CHARACTER::CanDeposit() const
{
	return (m_deposit_pulse == 0 || (m_deposit_pulse < thecore_pulse()));
}
//------------------------------------------------

ESex GET_SEX(LPCHARACTER ch)
{
	switch (ch->GetRaceNum())
	{
		case MAIN_RACE_WARRIOR_M:
		case MAIN_RACE_SURA_M:
		case MAIN_RACE_ASSASSIN_M:
		case MAIN_RACE_SHAMAN_M:
#ifdef ENABLE_WOLFMAN_CHARACTER
		case MAIN_RACE_WOLFMAN_M:
#endif
			return SEX_MALE;

		case MAIN_RACE_ASSASSIN_W:
		case MAIN_RACE_SHAMAN_W:
		case MAIN_RACE_WARRIOR_W:
		case MAIN_RACE_SURA_W:
			return SEX_FEMALE;
	}

	/* default sex = male */
	return SEX_MALE;
}

int CHARACTER::GetHPPct() const
{
	if (GetMaxHP() <= 0) // @fixme136
		return 0;
	return (GetHP() * 100) / GetMaxHP();
}

bool CHARACTER::IsBerserk() const
{
	if (m_pkMobInst != NULL)
		return m_pkMobInst->m_IsBerserk;
	else
		return false;
}

void CHARACTER::SetBerserk(bool mode)
{
	if (m_pkMobInst != NULL)
		m_pkMobInst->m_IsBerserk = mode;
}

bool CHARACTER::IsGodSpeed() const
{
	if (m_pkMobInst != NULL)
	{
		return m_pkMobInst->m_IsGodSpeed;
	}
	else
	{
		return false;
	}
}

void CHARACTER::SetGodSpeed(bool mode)
{
	if (m_pkMobInst != NULL)
	{
		m_pkMobInst->m_IsGodSpeed = mode;

		if (mode == true)
		{
			SetPoint(POINT_ATT_SPEED, 250);
		}
		else
		{
			SetPoint(POINT_ATT_SPEED, m_pkMobData->m_table.sAttackSpeed);
		}
	}
}

bool CHARACTER::IsDeathBlow() const
{
	if (number(1, 100) <= m_pkMobData->m_table.bDeathBlowPoint)
	{
		return true;
	}
	else
	{
		return false;
	}
}

struct FFindReviver
{
	FFindReviver()
	{
		pChar = NULL;
		HasReviver = false;
	}

	void operator() (LPCHARACTER ch)
	{
		if (ch->IsMonster() != true)
		{
			return;
		}

		if (ch->IsReviver() == true && pChar != ch && ch->IsDead() != true)
		{
			if (number(1, 100) <= ch->GetMobTable().bRevivePoint)
			{
				HasReviver = true;
				pChar = ch;
			}
		}
	}

	LPCHARACTER pChar;
	bool HasReviver;
};

bool CHARACTER::HasReviverInParty() const
{
	LPPARTY party = GetParty();

	if (party != NULL)
	{
		if (party->GetMemberCount() == 1) return false;

		FFindReviver f;
		party->ForEachMemberPtr(f);
		return f.HasReviver;
	}

	return false;
}

bool CHARACTER::IsRevive() const
{
	if (m_pkMobInst != NULL)
	{
		return m_pkMobInst->m_IsRevive;
	}

	return false;
}

void CHARACTER::SetRevive(bool mode)
{
	if (m_pkMobInst != NULL)
	{
		m_pkMobInst->m_IsRevive = mode;
	}
}

void CHARACTER::GoHome()
{
	WarpSet(EMPIRE_START_X(GetEmpire()), EMPIRE_START_Y(GetEmpire()));
}

void CHARACTER::SendGuildName(CGuild* pGuild)
{
	if (NULL == pGuild) return;

	DESC	*desc = GetDesc();

	if (NULL == desc) return;
	if (m_known_guild.find(pGuild->GetID()) != m_known_guild.end()) return;

	m_known_guild.insert(pGuild->GetID());

	TPacketGCGuildName	pack;
	memset(&pack, 0x00, sizeof(pack));

	pack.header		= HEADER_GC_GUILD;
	pack.subheader	= GUILD_SUBHEADER_GC_GUILD_NAME;
	pack.size		= sizeof(TPacketGCGuildName);
	pack.guildID	= pGuild->GetID();
	memcpy(pack.guildName, pGuild->GetName(), GUILD_NAME_MAX_LEN);

	desc->Packet(&pack, sizeof(pack));
}

void CHARACTER::SendGuildName(DWORD dwGuildID)
{
	SendGuildName(CGuildManager::instance().FindGuild(dwGuildID));
}

EVENTFUNC(destroy_when_idle_event)
{
	char_event_info* info = dynamic_cast<char_event_info*>( event->info );
	if ( info == NULL )
	{
		sys_err( "destroy_when_idle_event> <Factor> Null pointer" );
		return 0;
	}

	LPCHARACTER ch = info->ch;
	if (ch == NULL) { // <Factor>
		return 0;
	}

	if (ch->GetVictim())
	{
		return PASSES_PER_SEC(300);
	}

	sys_log(1, "DESTROY_WHEN_IDLE: %s", ch->GetName());

	ch->m_pkDestroyWhenIdleEvent = NULL;
	M2_DESTROY_CHARACTER(ch);
	return 0;
}

void CHARACTER::StartDestroyWhenIdleEvent()
{
	if (m_pkDestroyWhenIdleEvent)
		return;

	char_event_info* info = AllocEventInfo<char_event_info>();

	info->ch = this;

	m_pkDestroyWhenIdleEvent = event_create(destroy_when_idle_event, info, PASSES_PER_SEC(300));
}

void CHARACTER::SetComboSequence(BYTE seq)
{
	m_bComboSequence = seq;
}

BYTE CHARACTER::GetComboSequence() const
{
	return m_bComboSequence;
}

void CHARACTER::SetLastComboTime(DWORD time)
{
	m_dwLastComboTime = time;
}

DWORD CHARACTER::GetLastComboTime() const
{
	return m_dwLastComboTime;
}

void CHARACTER::SetValidComboInterval(int interval)
{
	m_iValidComboInterval = interval;
}

int CHARACTER::GetValidComboInterval() const
{
	return m_iValidComboInterval;
}

BYTE CHARACTER::GetComboIndex() const
{
	return m_bComboIndex;
}

void CHARACTER::IncreaseComboHackCount(int k)
{
	m_iComboHackCount += k;

	if (m_iComboHackCount >= 10)
	{
		if (GetDesc())
			if (GetDesc()->DelayedDisconnect(number(2, 7)))
			{
				sys_log(0, "COMBO_HACK_DISCONNECT: %s count: %d", GetName(), m_iComboHackCount);
				LogManager::instance().HackLog("Combo", this);
			}
	}
}

void CHARACTER::ResetComboHackCount()
{
	m_iComboHackCount = 0;
}

void CHARACTER::SkipComboAttackByTime(int interval)
{
	m_dwSkipComboAttackByTime = get_dword_time() + interval;
}

DWORD CHARACTER::GetSkipComboAttackByTime() const
{
	return m_dwSkipComboAttackByTime;
}

void CHARACTER::ResetChatCounter()
{
	m_bChatCounter = 0;
	m_bMountCounter = 0;
}

BYTE CHARACTER::IncreaseChatCounter()
{
	return ++m_bChatCounter;
}

BYTE CHARACTER::GetChatCounter() const
{
	return m_bChatCounter;
}

BYTE CHARACTER::GetMountCounter() const
{
	return m_bMountCounter;
}

void CHARACTER::ResetMountCounter()
{
	m_bMountCounter = 0;
}

BYTE CHARACTER::IncreaseMountCounter()
{
	return ++m_bMountCounter;
}

// ���̳� �ٸ����� Ÿ�� �ֳ�?
bool CHARACTER::IsRiding() const
{
	return IsHorseRiding() || GetMountVnum();
}

bool CHARACTER::CanWarp() const
{
	const int iPulse = thecore_pulse();
	const int limit_time = PASSES_PER_SEC(g_nPortalLimitTime);

	if ((iPulse - GetSafeboxLoadTime()) < limit_time)
		return false;

	if ((iPulse - GetExchangeTime()) < limit_time)
		return false;

	if ((iPulse - GetMyShopTime()) < limit_time)
		return false;

	if ((iPulse - GetRefineTime()) < limit_time)
		return false;

	if (GetExchange() || GetMyShop() || GetShopOwner() || IsOpenSafebox() || IsCubeOpen()
#ifdef ENABLE_ACCE_SYSTEM
		 || IsAcceOpen()
#endif
#ifdef __ATTR_TRANSFER_SYSTEM__
		 || IsAttrTransferOpen()
#endif
#if defined(ENABLE_CHRISTMAS_WHEEL_OF_DESTINY)
		 || GetWheelDestiny()
#endif
	)
		return false;

#ifdef __ENABLE_NEW_OFFLINESHOP__
	if (GetOfflineShopGuest() || GetAuctionGuest())
		return false;

	if (iPulse - GetOfflineShopUseTime() < limit_time)
		return false;
#endif

	return true;
}

DWORD CHARACTER::GetNextExp() const
{
	if (PLAYER_MAX_LEVEL_CONST < GetLevel())
		return 2500000000u;
	else
		return exp_table[GetLevel()];
}

#ifdef __NEWPET_SYSTEM__
DWORD CHARACTER::PetGetNextExp() const
{
	if (IsNewPet()) {
		if (120 < GetLevel())
			return 2500000000;
		else
			return exppet_table[GetLevel()];
	} return 0;
}
#endif

#ifdef ENABLE_RANKING
long long CHARACTER::GetRankPoints(int iArg)
{
	if ((iArg < 0) || (iArg >= RANKING_MAX_CATEGORIES))
		return 0;
	
	return m_lRankPoints[iArg];
}

void CHARACTER::SetRankPoints(int iArg, long long lPoint)
{
	if ((iArg < 0) || (iArg >= RANKING_MAX_CATEGORIES))
		return;
	
	/*
	iArg : 0	- chi ha vinto piu duelli -
	iArg : 1	- chi ha perso piu duelli -
	iArg : 2	- m. m. di uccisioni in guerra -
	iArg : 3	- d. medi maggiori vs player -
	iArg : 4	- d. abilita maggiore vs player -
	iArg : 5	- chi ha distrutto piu metin -
	iArg : 6	- chi ha ucciso piu mostri -
	iArg : 7	- chi ha ucciso piu boss -
	iArg : 8	- d. medi maggiori vs boss -
	iArg : 9	- d. abilita maggiori vs boss -
	iArg : 10	- chi ha raccolto piu yang -
	iArg : 11	- chi ha raccolto piu gaya -
	iArg : 12	- chi ha usato piu i. oggetto -
	iArg : 13	- chi ha usato piu i. talismani -
	iArg : 14	- chi ha pescato piu pesci -
	iArg : 15	- m. numero di tempo in game -
	iArg : 16	- piu dungeon completati -
	iArg : 17	- chi ha aperto piu forzieri -
	iArg : 18	- d. medio mas. vs mob -
	iArg : 19	- d. abilita mas. vs mob -
	*/
	m_lRankPoints[iArg] = lPoint;
	Save();
}

void CHARACTER::RankingSubcategory(int iArg)
{
	if (!GetDesc())
		return;
	
	if ((iArg < 0) || (iArg >= RANKING_MAX_CATEGORIES))
		return;
	
	TPacketGCRankingTable p;
	int j = 0;
	
	char szQuery1[1024] = {0};
	snprintf(szQuery1, sizeof(szQuery1), "SELECT account_id, level, name, r%d FROM player.player%s WHERE account_id=(SELECT id FROM account.account%s WHERE status='OK' AND id=account_id) AND name not in(SELECT mName FROM common.gmlist%s) ORDER BY r%d desc, level desc, name asc LIMIT 50", iArg, get_table_postfix(), get_table_postfix(), get_table_postfix(), iArg);
	std::unique_ptr<SQLMsg> pRes1(DBManager::instance().DirectQuery(szQuery1));
	int iRes = pRes1->Get()->uiNumRows;
	if (iRes > 0) {
		MYSQL_ROW data;
		while ((data = mysql_fetch_row(pRes1->Get()->pSQLResult))) {
			int col = 1;
			p.list[j].iPosition = j;
			p.list[j].iRealPosition = 0;
			p.list[j].iLevel = atoi(data[col++]);
			strlcpy(p.list[j].szName, data[col++], sizeof(p.list[j].szName));
			p.list[j].iPoints = atoi(data[col]);
			j += 1;
		}
	}
	
	if (j < MAX_RANKING_LIST) {
		for (int i = j; i < MAX_RANKING_LIST; i++) {
			p.list[i].iPosition = i;
			p.list[i].iRealPosition = 0;
			p.list[i].iLevel = 0;
			p.list[i].iPoints = 0;
			strlcpy(p.list[i].szName, "", sizeof(p.list[i].szName));
		}
	}
	
	char szQuery2[1024] = {0};
	if (GetGMLevel() > GM_PLAYER) {
		snprintf(szQuery2, sizeof(szQuery2), "SELECT * FROM (SELECT @rank:=0) a, (SELECT @rank:=@rank+1 r, r%d, name, level FROM player.player%s AS res ORDER BY r%d desc, level desc, name asc) as custom WHERE name='%s'", iArg, get_table_postfix(), iArg, GetName());
	} else {
		snprintf(szQuery2, sizeof(szQuery2), "SELECT * FROM (SELECT @rank:=0) a, (SELECT @rank:=@rank+1 r, r%d, name, level FROM player.player%s AS res WHERE name not in(SELECT mName FROM common.gmlist) ORDER BY r%d desc, level desc, name asc) as custom WHERE name='%s'", iArg, get_table_postfix(), iArg, GetName());
	}
	std::unique_ptr<SQLMsg> pRes2(DBManager::instance().DirectQuery(szQuery2));
	iRes = pRes2->Get()->uiNumRows;
	if (iRes > 0) {
		j = MAX_RANKING_LIST-1;
		MYSQL_ROW data = mysql_fetch_row(pRes2->Get()->pSQLResult);
		p.list[j].iPosition = j;
		p.list[j].iRealPosition = atoi(data[1]);
		p.list[j].iLevel = atoi(data[4]);
		p.list[j].iPoints = atoi(data[2]);
		strlcpy(p.list[j].szName, GetName(), sizeof(p.list[j].szName));
	}
	
	GetDesc()->Packet(&p, sizeof(p));
}
#endif

int	CHARACTER::GetSkillPowerByLevel(int level, bool bMob) const
{
	return CTableBySkill::instance().GetSkillPowerByLevelFromType(GetJob(), GetSkillGroup(), MINMAX(0, level, SKILL_MAX_LEVEL), bMob);
}
#ifdef __ENABLE_NEW_OFFLINESHOP__
void CHARACTER::SetShopSafebox(offlineshop::CShopSafebox* pk) 
{
	if(m_pkShopSafebox && pk==NULL)
		m_pkShopSafebox->SetOwner(NULL);

	else if(m_pkShopSafebox==NULL && pk)
		pk->SetOwner(this);

	m_pkShopSafebox  = pk;
}
#endif



#ifdef ENABLE_CHANNEL_SWITCH_SYSTEM
bool CHARACTER::SwitchChannel(long newAddr, WORD newPort)
{
	if (!IsPC() || !GetDesc() || !CanWarp())
		return false;

	long x = GetX();
	long y = GetY();

	long lAddr = newAddr;
	long lMapIndex = GetMapIndex();
	WORD wPort = newPort;

	// If we currently are in a dungeon.
	if (lMapIndex >= 10000)
	{
		sys_err("Invalid change channel request from dungeon %d!", lMapIndex);
		return false;
	}

	// If we are on CH99.
	if (g_bChannel == 99)
	{
		sys_err("%s attempted to change channel from CH99, ignoring req.", GetName());
		return false;
	}

	Stop();
	Save();

	if (GetSectree())
	{
		GetSectree()->RemoveEntity(this);
		ViewCleanup();
		EncodeRemovePacket(this);
	}

	m_lWarpMapIndex = lMapIndex;
	m_posWarp.x = x;
	m_posWarp.y = y;

	// TODO: This log message should mention channel we are changing to instead of port.
	sys_log(0, "ChangeChannel %s, %ld %ld map %ld to port %d", GetName(), x, y, GetMapIndex(), wPort);

	TPacketGCWarp p;

	p.bHeader = HEADER_GC_WARP;
	p.lX = x;
	p.lY = y;
	p.lAddr = lAddr;
	p.wPort = wPort;

	GetDesc()->Packet(&p, sizeof(p));

	char buf[256];
	// TODO: This log message should mention channel we are changing to instead of port
	snprintf(buf, sizeof(buf), "%s Port%d Map%ld x%ld y%ld", GetName(), wPort, GetMapIndex(), x, y);
	LogManager::instance().CharLog(this, 0, "CHANGE_CH", buf);

	return true;
}

EVENTINFO(switch_channel_info)
{
	DynamicCharacterPtr ch;
	int secs;
	long newAddr;
	WORD newPort;
	switch_channel_info()
		: ch(),
		secs(0),
		newAddr(0),
		newPort(0)
	{
	}
};

EVENTFUNC(switch_channel)
{
	switch_channel_info* info = dynamic_cast<switch_channel_info*>(event->info);
	if (!info)
	{
		sys_err("No switch channel event info!");
		return 0;
	}

	LPCHARACTER	ch = info->ch;
	if (!ch)
	{
		sys_err("No char to work on for the switch.");
		return 0;
	}

	if (!ch->GetDesc())
		return 0;

	if (info->secs > 0)
	{
#ifdef TEXTS_IMPROVEMENT
		ch->ChatPacketNew(CHAT_TYPE_INFO, 658, "%d", info->secs);
#endif
		--info->secs;
		return PASSES_PER_SEC(1);
	}

	ch->SwitchChannel(info->newAddr, info->newPort);
	ch->m_pkTimedEvent = nullptr;
	return 0;
}

bool CHARACTER::StartChannelSwitch(long newAddr, WORD newPort)
{
	if (IsHack(false, true, 10))
		return false;

	switch_channel_info* info = AllocEventInfo<switch_channel_info>();
	info->ch = this;
	info->secs = CanWarp() && !IsPosition(POS_FIGHTING) ? 3 : 10;
	info->newAddr = newAddr;
	info->newPort = newPort;

	m_pkTimedEvent = event_create(switch_channel, info, 1);
	return true;
}
#endif




#ifdef ENABLE_ACCE_SYSTEM
void CHARACTER::OpenAcce(bool bCombination)
{
	if (isAcceOpened(bCombination))
	{
#ifdef TEXTS_IMPROVEMENT
		ChatPacketNew(CHAT_TYPE_INFO, 659, "");
#endif
		return;
	}

	if (bCombination)
	{
		if (m_bAcceAbsorption)
		{
#ifdef TEXTS_IMPROVEMENT
			ChatPacketNew(CHAT_TYPE_INFO, 660, "");
#endif
			return;
		}

		m_bAcceCombination = true;
	}
	else
	{
		if (m_bAcceCombination)
		{
#ifdef TEXTS_IMPROVEMENT
			ChatPacketNew(CHAT_TYPE_INFO, 661, "");
#endif
			return;
		}

		m_bAcceAbsorption = true;
	}

	TItemPos tPos;
	tPos.window_type = INVENTORY;
	tPos.cell = 0;

	TPacketAcce sPacket;
	sPacket.header = HEADER_GC_ACCE;
	sPacket.subheader = ACCE_SUBHEADER_GC_OPEN;
	sPacket.bWindow = bCombination;
	sPacket.dwPrice = 0;
	sPacket.bPos = 0;
	sPacket.tPos = tPos;
	sPacket.dwItemVnum = 0;
	sPacket.dwMinAbs = 0;
	sPacket.dwMaxAbs = 0;
	GetDesc()->Packet(&sPacket, sizeof(TPacketAcce));

	ClearAcceMaterials();
}

void CHARACTER::CloseAcce()
{
	if ((!m_bAcceCombination) && (!m_bAcceAbsorption))
		return;

	bool bWindow = (m_bAcceCombination == true ? true : false);

	TItemPos tPos;
	tPos.window_type = INVENTORY;
	tPos.cell = 0;

	TPacketAcce sPacket;
	sPacket.header = HEADER_GC_ACCE;
	sPacket.subheader = ACCE_SUBHEADER_GC_CLOSE;
	sPacket.bWindow = bWindow;
	sPacket.dwPrice = 0;
	sPacket.bPos = 0;
	sPacket.tPos = tPos;
	sPacket.dwItemVnum = 0;
	sPacket.dwMinAbs = 0;
	sPacket.dwMaxAbs = 0;
	GetDesc()->Packet(&sPacket, sizeof(TPacketAcce));

	if (bWindow)
		m_bAcceCombination = false;
	else
		m_bAcceAbsorption = false;

	ClearAcceMaterials();
}

void CHARACTER::ClearAcceMaterials()
{
	LPITEM * pkItemMaterial;
	pkItemMaterial = GetAcceMaterials();
	for (int i = 0; i < ACCE_WINDOW_MAX_MATERIALS; ++i)
	{
		if (!pkItemMaterial[i])
			continue;

		pkItemMaterial[i]->Lock(false);
		pkItemMaterial[i] = NULL;
	}
}

bool CHARACTER::AcceIsSameGrade(long lGrade)
{
	LPITEM * pkItemMaterial;
	pkItemMaterial = GetAcceMaterials();
	if (!pkItemMaterial[0])
		return false;

	bool bReturn = (pkItemMaterial[0]->GetValue(ACCE_GRADE_VALUE_FIELD) == lGrade ? true : false);
	return bReturn;
}

DWORD CHARACTER::GetAcceCombinePrice(long lGrade
#ifdef ENABLE_STOLE_COSTUME
, bool isCostume
#endif
)
{
	DWORD dwPrice = 0;
	switch (lGrade)
	{
	case 2:
	{
#ifdef ENABLE_STOLE_COSTUME
		dwPrice = isCostume ? COSTUME_STOLE_GRADE_2_PRICE : ACCE_GRADE_2_PRICE;
#else
		dwPrice = ACCE_GRADE_2_PRICE;
#endif
	}
	break;
	case 3:
	{
#ifdef ENABLE_STOLE_COSTUME
		dwPrice = isCostume ? COSTUME_STOLE_GRADE_3_PRICE : ACCE_GRADE_3_PRICE;
#else
		dwPrice = ACCE_GRADE_2_PRICE;
#endif
	}
	break;
	case 4:
	{
#ifdef ENABLE_STOLE_COSTUME
		dwPrice = isCostume ? 0 : ACCE_GRADE_4_PRICE;
#else
		dwPrice = ACCE_GRADE_2_PRICE;
#endif
	}
	break;
	default:
	{
#ifdef ENABLE_STOLE_COSTUME
		dwPrice = isCostume ? COSTUME_STOLE_GRADE_1_PRICE : ACCE_GRADE_1_PRICE;
#else
		dwPrice = ACCE_GRADE_1_PRICE;
#endif
	}
	break;
	}

	return dwPrice;
}

BYTE CHARACTER::CheckEmptyMaterialSlot()
{
	LPITEM * pkItemMaterial;
	pkItemMaterial = GetAcceMaterials();
	for (int i = 0; i < ACCE_WINDOW_MAX_MATERIALS; ++i)
	{
		if (!pkItemMaterial[i])
			return i;
	}

	return 255;
}

void CHARACTER::GetAcceCombineResult(DWORD & dwItemVnum, DWORD & dwMinAbs, DWORD & dwMaxAbs)
{
	LPITEM * pkItemMaterial;
	pkItemMaterial = GetAcceMaterials();

	if (m_bAcceCombination)
	{
		if ((pkItemMaterial[0]) && (pkItemMaterial[1]))
		{
			long lVal = pkItemMaterial[0]->GetValue(ACCE_GRADE_VALUE_FIELD);
			if (lVal == 4)
			{
				dwItemVnum = pkItemMaterial[0]->GetOriginalVnum();
				dwMinAbs = pkItemMaterial[0]->GetSocket(ACCE_ABSORPTION_SOCKET);
				DWORD dwMaxAbsCalc = (dwMinAbs + ACCE_GRADE_4_ABS_RANGE > ACCE_GRADE_4_ABS_MAX ? ACCE_GRADE_4_ABS_MAX : (dwMinAbs + ACCE_GRADE_4_ABS_RANGE));
				dwMaxAbs = dwMaxAbsCalc;
			}
			else
			{
				DWORD dwMaskVnum = pkItemMaterial[0]->GetOriginalVnum();
				TItemTable * pTable = ITEM_MANAGER::instance().GetTable(dwMaskVnum + 1);
				if (pTable)
					dwMaskVnum += 1;

				dwItemVnum = dwMaskVnum;
				switch (lVal)
				{
				case 2:
				{
					dwMinAbs = ACCE_GRADE_3_ABS;
					dwMaxAbs = ACCE_GRADE_3_ABS;
				}
				break;
				case 3:
				{
					dwMinAbs = ACCE_GRADE_4_ABS_MIN;
					dwMaxAbs = ACCE_GRADE_4_ABS_MAX_COMB;
				}
				break;
				default:
				{
					dwMinAbs = ACCE_GRADE_2_ABS;
					dwMaxAbs = ACCE_GRADE_2_ABS;
				}
				break;
				}
			}
		}
		else
		{
			dwItemVnum = 0;
			dwMinAbs = 0;
			dwMaxAbs = 0;
		}
	}
	else
	{
		if ((pkItemMaterial[0]) && (pkItemMaterial[1]))
		{
			dwItemVnum = pkItemMaterial[0]->GetOriginalVnum();
			dwMinAbs = pkItemMaterial[0]->GetSocket(ACCE_ABSORPTION_SOCKET);
			dwMaxAbs = dwMinAbs;
		}
		else
		{
			dwItemVnum = 0;
			dwMinAbs = 0;
			dwMaxAbs = 0;
		}
	}
}

void CHARACTER::AddAcceMaterial(TItemPos tPos, BYTE bPos)
{
	if (bPos >= ACCE_WINDOW_MAX_MATERIALS)
	{
		if (bPos == 255)
		{
			bPos = CheckEmptyMaterialSlot();
			if (bPos >= ACCE_WINDOW_MAX_MATERIALS)
				return;
		}
		else
			return;
	}

	LPITEM pkItem = GetItem(tPos);
	if (!pkItem)
		return;
	else if ((pkItem->GetCell() >= INVENTORY_MAX_NUM) || (pkItem->IsEquipped()) || (tPos.IsBeltInventoryPosition()) || (pkItem->IsDragonSoul()))
		return;
	else if ((pkItem->GetType() != ITEM_COSTUME) && (m_bAcceCombination))
		return;
	else if ((pkItem->GetType() != ITEM_COSTUME) && (bPos == 0) && (m_bAcceAbsorption))
		return;
	else if (pkItem->isLocked())
	{
#ifdef TEXTS_IMPROVEMENT
		ChatPacketNew(CHAT_TYPE_INFO, 519, ""); 
#endif
		return;
	}
#ifdef __SOULBINDING_SYSTEM__
	else if ((pkItem->IsBind()) || (pkItem->IsUntilBind()))
	{
#ifdef TEXTS_IMPROVEMENT
		ChatPacketNew(CHAT_TYPE_INFO, 519, ""); 
#endif
		return;
	}
#endif
#ifdef ENABLE_STOLE_COSTUME
	else if (m_bAcceAbsorption && bPos == 0 && pkItem->GetSubType() != COSTUME_ACCE)
	{
		return;
	}
#endif
	else if ((m_bAcceCombination) && (bPos == 1) && (!AcceIsSameGrade(pkItem->GetValue(ACCE_GRADE_VALUE_FIELD))))
	{
#ifdef TEXTS_IMPROVEMENT
		ChatPacketNew(CHAT_TYPE_INFO, 662, "");
#endif
		return;
	}
#ifdef ENABLE_STOLE_COSTUME
	else if ((m_bAcceCombination) && (pkItem->GetSubType() == COSTUME_STOLE) && (pkItem->GetValue(0) == 4))
	{
#ifdef TEXTS_IMPROVEMENT
		ChatPacketNew(CHAT_TYPE_INFO, 20, "%s", pkItem->GetName());
#endif
		return;
	}
#endif
	else if ((m_bAcceCombination) && (pkItem->GetSocket(ACCE_ABSORPTION_SOCKET) >= ACCE_GRADE_4_ABS_MAX))
	{
#ifdef TEXTS_IMPROVEMENT
		ChatPacketNew(CHAT_TYPE_INFO, 663, "%d", ACCE_GRADE_4_ABS_MAX);
#endif
		return;
	}
	else if ((bPos == 1) && (m_bAcceAbsorption))
	{
		if ((pkItem->GetType() != ITEM_WEAPON) && (pkItem->GetType() != ITEM_ARMOR))
		{
#ifdef TEXTS_IMPROVEMENT
			ChatPacketNew(CHAT_TYPE_INFO, 520, ""); 
#endif
			return;
		}
		else if ((pkItem->GetType() == ITEM_ARMOR) && (pkItem->GetSubType() != ARMOR_BODY))
		{
#ifdef TEXTS_IMPROVEMENT
			ChatPacketNew(CHAT_TYPE_INFO, 520, ""); 
#endif
			return;
		}
	}
	else if
#ifdef ENABLE_STOLE_COSTUME
	(
#endif
	((pkItem->GetSubType() != COSTUME_ACCE) 
#ifdef ENABLE_STOLE_COSTUME
	&& (pkItem->GetSubType() != COSTUME_STOLE)) 
#endif
	&& (m_bAcceCombination))
		return;
	else if 
#ifdef ENABLE_STOLE_COSTUME
	(
#endif
	((pkItem->GetSubType() != COSTUME_ACCE) 
#ifdef ENABLE_STOLE_COSTUME
	&& (pkItem->GetSubType() != COSTUME_STOLE)) 
#endif
	&& (bPos == 0) && (m_bAcceAbsorption))
		return;
	else if ((pkItem->GetSocket(ACCE_ABSORBED_SOCKET) > 0) && (bPos == 0) && (m_bAcceAbsorption))
		return;

	LPITEM * pkItemMaterial;
	pkItemMaterial = GetAcceMaterials();
	if ((bPos == 1) && (!pkItemMaterial[0]))
		return;

#ifdef ENABLE_STOLE_COSTUME
	if ((!m_bAcceAbsorption) && (bPos == 1) && (pkItemMaterial[0]->GetSubType() != pkItem->GetSubType())) {
#ifdef TEXTS_IMPROVEMENT
		if (pkItemMaterial[0]->GetSubType() == COSTUME_STOLE) {
			ChatPacketNew(CHAT_TYPE_INFO, 18, "");
		}
		else {
			ChatPacketNew(CHAT_TYPE_INFO, 822, "");
		}
#endif
		return;
	}
	else if (!m_bAcceAbsorption && bPos == 1 && pkItemMaterial[0]->GetSubType() == COSTUME_STOLE && pkItemMaterial[0]->GetVnum() != pkItem->GetVnum()) {
#ifdef TEXTS_IMPROVEMENT
		ChatPacketNew(CHAT_TYPE_INFO, 1293, "");
#endif
		return;
	}
#endif

	if (pkItemMaterial[bPos])
		return;

	pkItemMaterial[bPos] = pkItem;
	pkItemMaterial[bPos]->Lock(true);

	DWORD dwItemVnum, dwMinAbs, dwMaxAbs;
	GetAcceCombineResult(dwItemVnum, dwMinAbs, dwMaxAbs);

	TPacketAcce sPacket;
	sPacket.header = HEADER_GC_ACCE;
	sPacket.subheader = ACCE_SUBHEADER_GC_ADDED;
	sPacket.bWindow = m_bAcceCombination == true ? true : false;
	sPacket.dwPrice = GetAcceCombinePrice(pkItem->GetValue(ACCE_GRADE_VALUE_FIELD)
#ifdef ENABLE_STOLE_COSTUME
	, pkItem->GetSubType() == COSTUME_STOLE ? true : false
#endif
	);
	sPacket.bPos = bPos;
	sPacket.tPos = tPos;
	sPacket.dwItemVnum = dwItemVnum;
	sPacket.dwMinAbs = dwMinAbs;
	sPacket.dwMaxAbs = dwMaxAbs;
	GetDesc()->Packet(&sPacket, sizeof(TPacketAcce));
}

void CHARACTER::RemoveAcceMaterial(BYTE bPos)
{
	if (bPos >= ACCE_WINDOW_MAX_MATERIALS)
		return;

	LPITEM * pkItemMaterial;
	pkItemMaterial = GetAcceMaterials();

	DWORD dwPrice = 0;

	if (bPos == 1)
	{
		if (pkItemMaterial[bPos])
		{
			pkItemMaterial[bPos]->Lock(false);
			pkItemMaterial[bPos] = NULL;
		}

		if (pkItemMaterial[0]) {
			dwPrice = GetAcceCombinePrice(pkItemMaterial[0]->GetValue(ACCE_GRADE_VALUE_FIELD)
#ifdef ENABLE_STOLE_COSTUME
			, pkItemMaterial[0]->GetSubType() == COSTUME_STOLE ? true : false
#endif
		);
		}
	}
	else
		ClearAcceMaterials();

	TItemPos tPos;
	tPos.window_type = INVENTORY;
	tPos.cell = 0;

	TPacketAcce sPacket;
	sPacket.header = HEADER_GC_ACCE;
	sPacket.subheader = ACCE_SUBHEADER_GC_REMOVED;
	sPacket.bWindow = m_bAcceCombination == true ? true : false;
	sPacket.dwPrice = dwPrice;
	sPacket.bPos = bPos;
	sPacket.tPos = tPos;
	sPacket.dwItemVnum = 0;
	sPacket.dwMinAbs = 0;
	sPacket.dwMaxAbs = 0;
	GetDesc()->Packet(&sPacket, sizeof(TPacketAcce));
}

BYTE CHARACTER::CanRefineAcceMaterials()
{
	if(GetOfflineShopGuest() || GetAuctionGuest())
		return 0;

	if (GetExchange() || GetMyShop() || GetShopOwner() || IsOpenSafebox() || IsCubeOpen()
#ifdef __ATTR_TRANSFER_SYSTEM__
		|| IsAttrTransferOpen()
#endif
	)
		return 0;

	BYTE bReturn = 0;
	LPITEM * pkItemMaterial;
	pkItemMaterial = GetAcceMaterials();
	if (m_bAcceCombination)
	{
		for (int i = 0; i < ACCE_WINDOW_MAX_MATERIALS; ++i)
		{
			if (pkItemMaterial[i])
			{
				if ((pkItemMaterial[i]->GetType() == ITEM_COSTUME) && (pkItemMaterial[i]->GetSubType() == COSTUME_ACCE))
					bReturn = 1;
#ifdef ENABLE_STOLE_COSTUME
				else if ((pkItemMaterial[i]->GetType() == ITEM_COSTUME) && (pkItemMaterial[i]->GetSubType() == COSTUME_STOLE))
					bReturn = 1;
#endif
				else
				{
					bReturn = 0;
					break;
				}
			}
			else
			{
				bReturn = 0;
				break;
			}
		}
	}
	else if (m_bAcceAbsorption)
	{
		if ((pkItemMaterial[0]) && (pkItemMaterial[1]))
		{
			if ((pkItemMaterial[0]->GetType() == ITEM_COSTUME) && (pkItemMaterial[0]->GetSubType() == COSTUME_ACCE))
				bReturn = 2;
			else
				bReturn = 0;

			if ((pkItemMaterial[1]->GetType() == ITEM_WEAPON) || ((pkItemMaterial[1]->GetType() == ITEM_ARMOR) && (pkItemMaterial[1]->GetSubType() == ARMOR_BODY)))
				bReturn = 2;
#ifdef ATTR_LOCK
			if ((pkItemMaterial[1]->GetType() == ITEM_WEAPON) || ((pkItemMaterial[1]->GetType() == ITEM_ARMOR) && (pkItemMaterial[1]->GetSubType() == ARMOR_BODY)))
			{
				if (pkItemMaterial[1]->GetLockedAttr() != -1)
				{
					bReturn= 0;
#ifdef TEXTS_IMPROVEMENT
					ChatPacketNew(CHAT_TYPE_INFO, 783, "");
#endif
				}
			}
#endif
			else
				bReturn = 0;

			if (pkItemMaterial[0]->GetSocket(ACCE_ABSORBED_SOCKET) > 0)
				bReturn = 0;
		}
		else
			bReturn = 0;
	}

	return bReturn;
}

void CHARACTER::RefineAcceMaterials()
{
	BYTE bCan = CanRefineAcceMaterials();
	if (bCan == 0)
		return;

	LPITEM * pkItemMaterial;
	pkItemMaterial = GetAcceMaterials();

	DWORD dwItemVnum, dwMinAbs, dwMaxAbs;
	GetAcceCombineResult(dwItemVnum, dwMinAbs, dwMaxAbs);
#ifdef ENABLE_LONG_LONG
	long long dwPrice = GetAcceCombinePrice(pkItemMaterial[0]->GetValue(ACCE_GRADE_VALUE_FIELD)
#ifdef ENABLE_STOLE_COSTUME
	, pkItemMaterial[0]->GetSubType() == COSTUME_STOLE ? true : false
#endif
	);
#else
	DWORD dwPrice = GetAcceCombinePrice(pkItemMaterial[0]->GetValue(ACCE_GRADE_VALUE_FIELD)
#ifdef ENABLE_STOLE_COSTUME
	, pkItemMaterial[0]->GetSubType() == COSTUME_STOLE ? true : false
#endif
	);
#endif

	if (bCan == 1)
	{
#ifdef ENABLE_STOLE_COSTUME
		bool bStole = pkItemMaterial[0]->GetSubType() == COSTUME_STOLE ? true : false;
#endif
		int iSuccessChance = 0;
		long lVal = pkItemMaterial[0]->GetValue(ACCE_GRADE_VALUE_FIELD);
		switch (lVal)
		{
		case 2:
		{
#ifdef ENABLE_STOLE_COSTUME
			if (bStole) {
				iSuccessChance = STOLA_COMBINE_GRADE_2;
				break;
			}
#endif
			iSuccessChance = ACCE_COMBINE_GRADE_2;
		}
		break;
		case 3:
		{
#ifdef ENABLE_STOLE_COSTUME
			if (bStole) {
				iSuccessChance = STOLA_COMBINE_GRADE_3;
				break;
			}
#endif
			iSuccessChance = ACCE_COMBINE_GRADE_3;
		}
		break;
		case 4:
		{
			iSuccessChance = ACCE_COMBINE_GRADE_4;
		}
		break;
		default:
		{
#ifdef ENABLE_STOLE_COSTUME
			if (bStole) {
				iSuccessChance = STOLA_COMBINE_GRADE_1;
				break;
			}
#endif
			iSuccessChance = ACCE_COMBINE_GRADE_1;
		}
		break;
		}

		if (GetGold() < dwPrice)
		{
#ifdef TEXTS_IMPROVEMENT
			ChatPacketNew(CHAT_TYPE_INFO, 232, ""); 
#endif
			return;
		}

		int iChance = number(1, 100);
		bool bSucces = (iChance <= iSuccessChance ? true : false);
		if (bSucces)
		{
			LPITEM pkItem = ITEM_MANAGER::instance().CreateItem(dwItemVnum, 1, 0, false);
			if (!pkItem)
			{
				sys_err("%d can't be created.", dwItemVnum);
				return;
			}
			
#ifdef ENABLE_STOLE_COSTUME
			if (pkItem->GetSubType() != COSTUME_STOLE)
				ITEM_MANAGER::CopyAllAttrTo(pkItemMaterial[0], pkItem);
#else
			ITEM_MANAGER::CopyAllAttrTo(pkItemMaterial[0], pkItem);
#endif
			LogManager::instance().ItemLog(this, pkItem, "COMBINE SUCCESS", pkItem->GetName());
			DWORD dwAbs = (dwMinAbs == dwMaxAbs ? dwMinAbs : number(dwMinAbs + 1, dwMaxAbs));
			pkItem->SetSocket(ACCE_ABSORPTION_SOCKET, dwAbs);
			pkItem->SetSocket(ACCE_ABSORBED_SOCKET, pkItemMaterial[0]->GetSocket(ACCE_ABSORBED_SOCKET));

			PointChange(POINT_GOLD, -dwPrice);
			DBManager::instance().SendMoneyLog(MONEY_LOG_REFINE, pkItemMaterial[0]->GetVnum(), -dwPrice);

			WORD wCell = pkItemMaterial[0]->GetCell();
			ITEM_MANAGER::instance().RemoveItem(pkItemMaterial[0], "COMBINE (REFINE SUCCESS)");
			ITEM_MANAGER::instance().RemoveItem(pkItemMaterial[1], "COMBINE (REFINE SUCCESS)");

			pkItem->AddToCharacter(this, TItemPos(INVENTORY, wCell));
			ITEM_MANAGER::instance().FlushDelayedSave(pkItem);
			pkItem->AttrLog();

#ifdef TEXTS_IMPROVEMENT
			if (lVal == 4) {
				ChatPacketNew(CHAT_TYPE_INFO, 521, "%d", dwAbs); 
			}
			else {
				ChatPacketNew(CHAT_TYPE_INFO, 389, ""); 
			}
#endif
			EffectPacket(SE_EFFECT_ACCE_SUCCEDED);
			LogManager::instance().AcceLog(GetPlayerID(), GetX(), GetY(), dwItemVnum, pkItem->GetID(), 1, dwAbs, 1);

			ClearAcceMaterials();
		}
		else
		{
			PointChange(POINT_GOLD, -dwPrice);
			DBManager::instance().SendMoneyLog(MONEY_LOG_REFINE, pkItemMaterial[0]->GetVnum(), -dwPrice);
			ITEM_MANAGER::instance().RemoveItem(pkItemMaterial[1], "COMBINE (REFINE FAIL)");
#ifdef TEXTS_IMPROVEMENT
			ChatPacketNew(CHAT_TYPE_INFO, 390, ""); 
#endif
			LogManager::instance().AcceLog(GetPlayerID(), GetX(), GetY(), dwItemVnum, 0, 0, 0, 0);
			pkItemMaterial[1] = NULL;
		}

		TItemPos tPos;
		tPos.window_type = INVENTORY;
		tPos.cell = 0;

		TPacketAcce sPacket;
		sPacket.header = HEADER_GC_ACCE;
		sPacket.subheader = ACCE_SUBHEADER_CG_REFINED;
		sPacket.bWindow = m_bAcceCombination == true ? true : false;
		sPacket.dwPrice = dwPrice;
		sPacket.bPos = 0;
		sPacket.tPos = tPos;
		sPacket.dwItemVnum = 0;
		sPacket.dwMinAbs = 0;
		if (bSucces)
			sPacket.dwMaxAbs = 100;
		else
			sPacket.dwMaxAbs = 0;

		GetDesc()->Packet(&sPacket, sizeof(TPacketAcce));
	}
	else
	{
		pkItemMaterial[1]->CopyAttributeTo(pkItemMaterial[0]);
		LogManager::instance().ItemLog(this, pkItemMaterial[0], "ABSORB (REFINE SUCCESS)", pkItemMaterial[0]->GetName());
		pkItemMaterial[0]->SetSocket(ACCE_ABSORBED_SOCKET, pkItemMaterial[1]->GetOriginalVnum());
		for (int i = 0; i < ITEM_ATTRIBUTE_MAX_NUM; ++i)
		{
			if (pkItemMaterial[0]->GetAttributeValue(i) < 0)
				pkItemMaterial[0]->SetForceAttribute(i, pkItemMaterial[0]->GetAttributeType(i), 0);
		}

		ITEM_MANAGER::instance().RemoveItem(pkItemMaterial[1], "ABSORBED (REFINE SUCCESS)");

		ITEM_MANAGER::instance().FlushDelayedSave(pkItemMaterial[0]);
		pkItemMaterial[0]->AttrLog();

#ifdef TEXTS_IMPROVEMENT
		ChatPacketNew(CHAT_TYPE_INFO, 629, "");
#endif
		ClearAcceMaterials();

		TItemPos tPos;
		tPos.window_type = INVENTORY;
		tPos.cell = 0;

		TPacketAcce sPacket;
		sPacket.header = HEADER_GC_ACCE;
		sPacket.subheader = ACCE_SUBHEADER_CG_REFINED;
		sPacket.bWindow = m_bAcceCombination == true ? true : false;
		sPacket.dwPrice = dwPrice;
		sPacket.bPos = 255;
		sPacket.tPos = tPos;
		sPacket.dwItemVnum = 0;
		sPacket.dwMinAbs = 0;
		sPacket.dwMaxAbs = 1;
		GetDesc()->Packet(&sPacket, sizeof(TPacketAcce));
	}
}

bool CHARACTER::CleanAcceAttr(LPITEM pkItem, LPITEM pkTarget)
{
	if (!CanHandleItem())
		return false;
	else if ((!pkItem) || (!pkTarget))
		return false;
	else if ((pkTarget->GetType() != ITEM_COSTUME) && (pkTarget->GetSubType() != COSTUME_ACCE))
		return false;

	if (pkTarget->GetSocket(ACCE_ABSORBED_SOCKET) <= 0)
		return false;

	pkTarget->SetSocket(ACCE_ABSORBED_SOCKET, 0);
	for (int i = 0; i < ITEM_ATTRIBUTE_MAX_NUM; ++i)
		pkTarget->SetForceAttribute(i, 0, 0);

	pkItem->SetCount(pkItem->GetCount() - 1);
	LogManager::instance().ItemLog(this, pkTarget, "USE_DETACHMENT (CLEAN ATTR)", pkTarget->GetName());
	return true;
}
#endif

#ifdef __HIDE_COSTUME_SYSTEM__
void CHARACTER::SetBodyCostumeHidden(bool hidden, bool pass)
{
	m_bHideBodyCostume = hidden;
	ChatPacket(CHAT_TYPE_COMMAND, "SetBodyCostumeHidden %d", m_bHideBodyCostume ? 1 : 0);
	if (!pass) {
		SetQuestFlag("costume_option.hide_body", m_bHideBodyCostume ? 1 : 0);
	}
}

void CHARACTER::SetHairCostumeHidden(bool hidden, bool pass)
{
	m_bHideHairCostume = hidden;
	ChatPacket(CHAT_TYPE_COMMAND, "SetHairCostumeHidden %d", m_bHideHairCostume ? 1 : 0);
	if (!pass) {
		SetQuestFlag("costume_option.hide_hair", m_bHideHairCostume ? 1 : 0);
	}
}

#ifdef ENABLE_ACCE_SYSTEM
void CHARACTER::SetAcceCostumeHidden(bool hidden, bool pass)
{
	m_bHideAcceCostume = hidden;
	ChatPacket(CHAT_TYPE_COMMAND, "SetAcceCostumeHidden %d", m_bHideAcceCostume ? 1 : 0);
	if (!pass) {
		SetQuestFlag("costume_option.hide_acce", m_bHideAcceCostume ? 1 : 0);
	}
}
#endif

#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
void CHARACTER::SetWeaponCostumeHidden(bool hidden, bool pass)
{
	m_bHideWeaponCostume = hidden;
	ChatPacket(CHAT_TYPE_COMMAND, "SetWeaponCostumeHidden %d", m_bHideWeaponCostume ? 1 : 0);
	if (!pass) {
		SetQuestFlag("costume_option.hide_weapon", m_bHideWeaponCostume ? 1 : 0);
	}
}
#endif
#endif



//__ENABLE_NEW_OFFLINESHOP__
bool CHARACTER::CanTakeInventoryItem(LPITEM item, TItemPos* cell)
{
	// DONT TOUCH MY iEmpty integer THANKS.
	int iEmpty = -1;

	if (item->IsDragonSoul())
	{
		cell->window_type = DRAGON_SOUL_INVENTORY;
		cell->cell = iEmpty = GetEmptyDragonSoulInventory(item);
	}

#ifdef ENABLE_EXTRA_INVENTORY
	else if (item->IsExtraItem())
	{
		cell->window_type = EXTRA_INVENTORY;
		cell->cell = iEmpty = GetEmptyExtraInventory(item);
	}
#endif


	else
	{
		cell->window_type = INVENTORY;
		cell->cell = iEmpty = GetEmptyInventory(item->GetSize());
	}

	return iEmpty != -1;
}


#ifdef ENABLE_SORT_INVEN
static bool SortMyItems(const LPITEM & s1, const LPITEM & s2)
{
	// Sort By Name
	std::string name(s1->GetName());
	std::string name2(s2->GetName());
	
	//Sort by Vnum
	// DWORD name(s1->GetVnum());
	// DWORD name2(s2->GetVnum());
	
	return name < name2;
	// return s1->GetVnum() < s2->GetVnum();
}
void CHARACTER::EditMyInven()
{
	return;

	int iPulse = thecore_pulse() - GetSortInv1Time();
	if (iPulse < PASSES_PER_SEC(30)) {
#ifdef TEXTS_IMPROVEMENT
		ChatPacketNew(CHAT_TYPE_INFO, 1290, "");
#endif
		return;
	}

	if (IsDead() || GetExchange() || GetShopOwner() || GetMyShop() || IsOpenSafebox() || IsCubeOpen())
	{
#ifdef TEXTS_IMPROVEMENT
		ChatPacketNew(CHAT_TYPE_INFO, 540, "");
#endif
		return;
	}

#ifdef __ENABLE_NEW_OFFLINESHOP__
	if(GetOfflineShopGuest() || GetAuctionGuest())
	{
#ifdef TEXTS_IMPROVEMENT
		ChatPacketNew(CHAT_TYPE_INFO, 782, "");
#endif
		return;
	}
#endif

	static std::vector<LPITEM> v;
	LPITEM myitems;
	
	//FIXING QUICKSLOT SYNC
	std::map<DWORD, BYTE> mapOldPosition;

	//clear vector
	v.clear();
	
#ifdef __ENABLE_EXTEND_INVEN_SYSTEM__
	int size = Inventory_Size();
#else
	int size = INVENTORY_MAX_NUM;
#endif


	for (int i = 0; i < size; ++i)
	{
		if (!(myitems = GetInventoryItem(i))) 
			continue;
		
		//add all items inven to vector
		v.push_back(myitems);
		
		//FIXING QUICKSLOT SYNC
		mapOldPosition.insert(std::make_pair(myitems->GetID() , myitems->GetCell()));

		//delete items from inven
		myitems->RemoveFromCharacter();
	}
	//Sort items
	std::sort(v.begin(), v.end(), SortMyItems);
	

	//FIXING QUICKSLOT SYNC
	std::vector<TQuickslot*> vecItemQuickslot;
	for(auto& quick : m_quickslot)
		if (quick.type == QUICKSLOT_TYPE_ITEM)
			vecItemQuickslot.push_back(&quick);


	//making a lambda to check if the item position was in quickslots
	auto lambdaChecker = [&vecItemQuickslot,&mapOldPosition] (LPITEM pItemLocal)
	{
		auto iter = mapOldPosition.find(pItemLocal->GetID());
		if (iter == mapOldPosition.end())
			return (TQuickslot*)nullptr;
		
		auto itemPos = iter->second;

		for (auto it = vecItemQuickslot.begin(); it!=vecItemQuickslot.end(); it++)
		{
			TQuickslot* pQuick = *it;

			if (pQuick && pQuick->pos == itemPos)
			{
				vecItemQuickslot.erase(it);
				return pQuick;
			}
		}
		return (TQuickslot*)nullptr;
	};


	//ENDFIX

	//Send vector items to inven
	itertype(v) it = v.begin();
	while (it != v.end()) {	
		LPITEM item = *(it++);
		if (item) 
		{
			//FIXING QUICKSLOT SYNC
			TQuickslot* pQuickSlot = lambdaChecker(item);
			bool isQuickSlotItem = pQuickSlot != nullptr;

			LPITEM newItem = item;
			//END

			TItemTable * p = ITEM_MANAGER::instance().GetTable(item->GetVnum());
			// isn't same function !
			if (p && p->dwFlags & ITEM_FLAG_STACKABLE && p->bType != ITEM_BLEND
#ifdef ENABLE_NEW_USE_POTION
			&& (p->bType != ITEM_USE && p->bSubType != USE_NEW_POTIION)
#endif
			)
				newItem = AutoGiveItem(item->GetVnum(), item->GetCount(), -1, false
#ifdef __HIGHLIGHT_SYSTEM__
				, false
#endif
				); // create new item for stackable items
				//changed 
				//AutoGiveItem(item->GetVnum(), item->GetCount(), -1, false); // create new item for stackable items
			
			else
				AutoGiveItem(item, false
#ifdef __HIGHLIGHT_SYSTEM__
				, false
#endif
				); // ressign again owner


			//FINAL CHECKING TO SYNC QUICKSLOT
			if (isQuickSlotItem)
				SyncQuickslot(QUICKSLOT_TYPE_ITEM, pQuickSlot->pos, newItem->GetCell());

		}
	}
	
	//message
	ChatPacket(CHAT_TYPE_COMMAND, "inv_sort_done");
	SetSortInv1Time();
}






static bool SortMyExtraItems(const LPITEM & s1, const LPITEM & s2)
{
	// Sort By Name
	// std::string name(s1->GetName());
	// std::string name2(s2->GetName());
	
	//Sort by Vnum
	DWORD name(s1->GetVnum());
	DWORD name2(s2->GetVnum());
	
	return name < name2;
	// return s1->GetVnum() < s2->GetVnum();
}
void CHARACTER::EditMyExtraInven()
{
	return;

	int iPulse = thecore_pulse() - GetSortInv2Time();
	if (iPulse < PASSES_PER_SEC(30)) {
#ifdef TEXTS_IMPROVEMENT
		ChatPacketNew(CHAT_TYPE_INFO, 1290, "");
#endif
		return;
	}

	if (IsDead() || GetExchange() || GetShopOwner() || GetMyShop() || IsOpenSafebox() || IsCubeOpen())
	{
#ifdef TEXTS_IMPROVEMENT
		ChatPacketNew(CHAT_TYPE_INFO, 540, "");
#endif
		return;
	}

#ifdef __ENABLE_NEW_OFFLINESHOP__
	if(GetOfflineShopGuest() || GetAuctionGuest())
	{
#ifdef TEXTS_IMPROVEMENT
		ChatPacketNew(CHAT_TYPE_INFO, 782, "");
#endif
		return;
	}
#endif

	static std::vector<LPITEM> v;
	LPITEM myitems;
	
	//FIXING QUICKSLOT SYNC
	std::map<DWORD, BYTE> mapOldPosition;

	//clear vector
	v.clear();
	

	int size = EXTRA_INVENTORY_MAX_NUM;


	for (int i = 0; i < size; ++i)
	{
		if (!(myitems = GetExtraInventoryItem(i))) 
			continue;
		
		//add all items inven to vector
		v.push_back(myitems);
		
		//FIXING QUICKSLOT SYNC
		mapOldPosition.insert(std::make_pair(myitems->GetID() , myitems->GetCell()));

		//delete items from inven
		myitems->RemoveFromCharacter();
	}
	//Sort items
	std::sort(v.begin(), v.end(), SortMyExtraItems);
	

	//FIXING QUICKSLOT SYNC
	std::vector<TQuickslot*> vecItemQuickslot;
	for(auto& quick : m_quickslot)
		if (quick.type == QUICKSLOT_TYPE_ITEM)
			vecItemQuickslot.push_back(&quick);


	//making a lambda to check if the item position was in quickslots
	auto lambdaChecker = [&vecItemQuickslot,&mapOldPosition] (LPITEM pItemLocal)
	{
		auto iter = mapOldPosition.find(pItemLocal->GetID());
		if (iter == mapOldPosition.end())
			return (TQuickslot*)nullptr;
		
		auto itemPos = iter->second;

		for (auto it = vecItemQuickslot.begin(); it!=vecItemQuickslot.end(); it++)
		{
			TQuickslot* pQuick = *it;

			if (pQuick && pQuick->pos == itemPos)
			{
				vecItemQuickslot.erase(it);
				return pQuick;
			}
		}
		return (TQuickslot*)nullptr;
	};


	//ENDFIX

	//Send vector items to inven
	itertype(v) it = v.begin();
	while (it != v.end()) {	
		LPITEM item = *(it++);
		if (item) 
		{
			//FIXING QUICKSLOT SYNC
			TQuickslot* pQuickSlot = lambdaChecker(item);
			bool isQuickSlotItem = pQuickSlot != nullptr;

			LPITEM newItem = item;
			//END

			TItemTable * p = ITEM_MANAGER::instance().GetTable(item->GetVnum());
			// isn't same function !
			if (p && p->dwFlags & ITEM_FLAG_STACKABLE && p->bType != ITEM_BLEND)
				newItem = AutoGiveItem(item->GetVnum(), item->GetCount(), -1, false
#ifdef __HIGHLIGHT_SYSTEM__
				, false
#endif
				); // create new item for stackable items
				//changed 
				//AutoGiveItem(item->GetVnum(), item->GetCount(), -1, false); // create new item for stackable items
			
			else
				AutoGiveItem(item, false
#ifdef __HIGHLIGHT_SYSTEM__
				, false
#endif
				); // ressign again owner


			//FINAL CHECKING TO SYNC QUICKSLOT
			if (isQuickSlotItem)
				SyncQuickslot(QUICKSLOT_TYPE_ITEM, pQuickSlot->pos, newItem->GetCell());

		}
	}
	
	//message
	ChatPacket(CHAT_TYPE_COMMAND, "ext_sort_done");
	SetSortInv2Time();
}







#endif




#ifdef __ENABLE_EXTEND_INVEN_SYSTEM__
static int NeedKeys[] = {2,2,2,2,3,3,4,4,4,5,5,5,6,6,6,7,7,7};
bool CHARACTER::Update_Inven()
{
#ifdef ENABLE_SPAM_CHECK
	int32_t time = GetLastUnlock() - get_global_time();
	if (time > 0) {
#ifdef TEXTS_IMPROVEMENT
		ChatPacketNew(CHAT_TYPE_INFO, 234, "%d", time);
#endif
		return false;
	}
#endif

	#define key2 72320
	int needkey = NeedKeys[Inven_Point()];
	if (CountSpecifyItem(key2) >= needkey) {
		RemoveSpecifyItem(key2, needkey);
		PointChange(POINT_INVEN, 1, false);
		ChatPacket(CHAT_TYPE_COMMAND, "refreshinven");
		UpdatePacket();
#ifdef ENABLE_SPAM_CHECK
		SetLastUnlock();
#endif
		return true;
	} else {
		int need_key = needkey - CountSpecifyItem(key2);
		ChatPacket(CHAT_TYPE_COMMAND, "update_envanter_need %d", need_key);
		return false;
	}
}
#endif

#ifdef ENABLE_SOUL_SYSTEM
int CHARACTER::GetSoulItemDamage(LPCHARACTER pkVictim, int iDamage, BYTE bSoulType)
{
	if(!pkVictim)
		return 0;
	
	if(!IsPC() || IsPolymorphed() || pkVictim->IsPC())
		return 0;
	
	if(bSoulType >= SOUL_MAX_NUM)
		return 0;
	
	const CAffect* pAffect = FindAffect(AFFECT_SOUL_RED + bSoulType);
	int iDamageAdd = 0;
	if (pAffect)
	{
		LPITEM soulItem = FindItemByID(pAffect->lSPCost);
		if(soulItem)
		{
			int iCurrentMinutes = (soulItem->GetSocket(2) / 10000);
			int iCurrentStrike = (soulItem->GetSocket(2) % 10000);
			
			int valueIndex = MINMAX(3, 2 + (iCurrentMinutes / 60), 5);
			float fDamageIncrease = float(soulItem->GetValue(valueIndex) / 10.0f);
		
			iDamageAdd = (fDamageIncrease * iDamage) - iDamage;
			int iNextStrikes = iCurrentStrike - 1;
			if(iNextStrikes <= 0)
			{
				iCurrentMinutes = MINMAX(0, iCurrentMinutes - 60, 180);
				iNextStrikes = soulItem->GetValue(2);
				
				if(iCurrentMinutes < 60)
				{
					soulItem->Lock(false);
					soulItem->SetSocket(1, false);
					RemoveAffect(const_cast<CAffect*>(pAffect));
				}

				soulItem->SetSocket(2, 0);
				soulItem->StartSoulItemEvent();		
			}
			
			soulItem->SetSocket(2, (iCurrentMinutes * 10000 + iNextStrikes));
		}
	}
	
	return iDamageAdd;
}
#endif

#ifdef __SKILL_COLOR_SYSTEM__
void CHARACTER::SetSkillColor(DWORD * dwSkillColor) {
	memcpy(m_dwSkillColor, dwSkillColor, sizeof(m_dwSkillColor));
	UpdatePacket();
}
#endif

#ifdef ENABLE_BATTLE_PASS_STAY_ONLINE
EVENTFUNC(stay_online_event)
{
	char_event_info* info = dynamic_cast<char_event_info*>( event->info );
	if ( info == NULL )
	{
		sys_err( "<stay_online_event> <Factor> Null pointer" );
		return 0;
	}

	LPCHARACTER	ch = info->ch;
	if (ch == NULL) { // <Factor>
		return 0;
	}

	BYTE bBattlePassId = ch->GetBattlePassId();
	if(bBattlePassId)
	{
		DWORD dwMinutes, dwNotUsed;
		if(CBattlePass::instance().BattlePassMissionGetInfo(bBattlePassId, STAY_ONLINE_MINUTES, &dwNotUsed, &dwMinutes))
		{
			if(ch->GetMissionProgress(STAY_ONLINE_MINUTES, bBattlePassId) < dwMinutes)
			{
				ch->UpdateMissionProgress(STAY_ONLINE_MINUTES, bBattlePassId, 1, dwMinutes);
				return PASSES_PER_SEC(1);
			}
		}
	}
	
	ch->m_pkStayOnlineEvent = NULL;
	return 0;
}

void CHARACTER::LoadStayActiveBattlePass()
{
	DWORD dwMinutes = 0;
	//if(CBattlePass::instance().BattlePassMissionGetInfo(GetBattlePassId(), STAY_ONLINE_MINUTES, &dwNotUsed, &dwMinutes))
	if(GetMissionProgress(STAY_ONLINE_MINUTES, GetBattlePassId()) < dwMinutes && !m_pkStayOnlineEvent)
	{
		char_event_info* info = AllocEventInfo<char_event_info>();
		info->ch = this;
		m_pkStayOnlineEvent = event_create(stay_online_event, info, PASSES_PER_SEC(1));
	}
}

#endif

#ifdef ENABLE_BATTLE_PASS
void CHARACTER::LoadBattlePass(DWORD dwCount, TPlayerBattlePassMission * data)
{
	m_bIsLoadedBattlePass = false;
	
	for (size_t i = 0; i < dwCount; ++i, ++data)
	{
		TPlayerBattlePassMission * newMission = new TPlayerBattlePassMission;
		newMission->dwPlayerId = data->dwPlayerId;
		newMission->dwMissionId = data->dwMissionId;
		newMission->dwBattlePassId = data->dwBattlePassId;
		newMission->dwExtraInfo = data->dwExtraInfo;
		newMission->bCompleted = data->bCompleted;
		newMission->bIsUpdated = data->bIsUpdated;
	
#ifdef ENABLE_BATTLE_PASS_STAY_ONLINE
		//if(newMission->dwMissionId == STAY_ONLINE_MINUTES)
		{
			DWORD dwMinutes, dwNotUsed;
			if(CBattlePass::instance().BattlePassMissionGetInfo(GetBattlePassId(), STAY_ONLINE_MINUTES, &dwNotUsed, &dwMinutes))
			{
				if(newMission->dwExtraInfo < dwMinutes && !m_pkStayOnlineEvent)
				{
					char_event_info* info = AllocEventInfo<char_event_info>();
					info->ch = this;
					m_pkStayOnlineEvent = event_create(stay_online_event, info, PASSES_PER_SEC(1));
				}
			}
		}
#endif
	
		m_listBattlePass.push_back(newMission);
	}
	
	m_bIsLoadedBattlePass = true;
}

#ifdef ENABLE_BATTLE_PASS_STAY_ONLINE
void CHARACTER::CancelStayOnlineEvent()
{
	if (m_pkStayOnlineEvent)
	{
		event_cancel(&m_pkStayOnlineEvent);
		m_pkStayOnlineEvent = NULL;
	}
}
#endif

DWORD CHARACTER::GetMissionProgress(DWORD dwMissionID, DWORD dwBattlePassID)
{
	itertype(m_listBattlePass) it = m_listBattlePass.begin();
	while (it != m_listBattlePass.end())
	{
		TPlayerBattlePassMission * pkMission = *it++;
		if(pkMission->dwMissionId == dwMissionID && pkMission->dwBattlePassId == dwBattlePassID)
			return pkMission->dwExtraInfo;
	}

	return 0;
}

bool CHARACTER::IsCompletedMission(BYTE bMissionType)
{
	itertype(m_listBattlePass) it = m_listBattlePass.begin();
	while (it != m_listBattlePass.end())
	{
		TPlayerBattlePassMission * pkMission = *it++;
		if(pkMission->dwMissionId == bMissionType)
			return (pkMission->bCompleted ? true : false);
	}

	return false;
}

void CHARACTER::UpdateMissionProgress(DWORD dwMissionID, DWORD dwBattlePassID, DWORD dwUpdateValue, DWORD dwTotalValue, bool isOverride)
{
	if(!m_bIsLoadedBattlePass)
		return;
	
	bool foundMission = false;
	DWORD dwSaveProgress = 0;
	
	itertype(m_listBattlePass) it = m_listBattlePass.begin();
	while (it != m_listBattlePass.end())
	{
		TPlayerBattlePassMission * pkMission = *it++;
		
		if(pkMission->dwMissionId == dwMissionID && pkMission->dwBattlePassId == dwBattlePassID)
		{
			pkMission->bIsUpdated = 1;
			if(isOverride)
				pkMission->dwExtraInfo = dwUpdateValue;
			else
				pkMission->dwExtraInfo += dwUpdateValue;
			
			if(pkMission->dwExtraInfo >= dwTotalValue)
			{
				pkMission->dwExtraInfo = dwTotalValue;
				pkMission->bCompleted = 1;
				
#ifdef ENABLE_BATTLE_PASS_STAY_ONLINE
				if(pkMission->dwMissionId == STAY_ONLINE_MINUTES)
				{
					CancelStayOnlineEvent();
				}
#endif
				CBattlePass::instance().BattlePassRewardMission(this, dwMissionID, dwBattlePassID);
				//CBattlePass::instance().CheckBattlePassCompleted(this);
			}
			
			dwSaveProgress = pkMission->dwExtraInfo;
			foundMission = true;
			break;
		}
	}
	
	if(!foundMission)
	{
		TPlayerBattlePassMission * newMission = new TPlayerBattlePassMission;
		newMission->dwPlayerId = GetPlayerID();
		newMission->dwMissionId = dwMissionID;
		newMission->dwBattlePassId = dwBattlePassID;
		
		if(dwUpdateValue >= dwTotalValue)
		{
			newMission->dwExtraInfo = dwTotalValue;
			newMission->bCompleted = 1;
#ifdef ENABLE_BATTLE_PASS_STAY_ONLINE
			if(newMission->dwMissionId == STAY_ONLINE_MINUTES)
			{
				CancelStayOnlineEvent();
			}
#endif		
			CBattlePass::instance().BattlePassRewardMission(this, dwMissionID, dwBattlePassID);
			
			dwSaveProgress = dwTotalValue;
		}
		else
		{
			newMission->dwExtraInfo = dwUpdateValue;
			newMission->bCompleted = 0;
			
			dwSaveProgress = dwUpdateValue;
		}
		
		newMission->bIsUpdated = 1;

		m_listBattlePass.push_back(newMission);
	}

	if(!GetDesc())
		return;
	
	TPacketGCBattlePassUpdate packet;
	packet.bHeader = HEADER_GC_BATTLE_PASS_UPDATE;
	packet.bMissionType = dwMissionID;
	packet.dwNewProgress = dwSaveProgress;
	GetDesc()->Packet(&packet, sizeof(TPacketGCBattlePassUpdate));
}

BYTE CHARACTER::GetBattlePassId()
{
	CAffect	* pAffect = FindAffect(AFFECT_BATTLE_PASS, POINT_BATTLE_PASS_ID);
	
	if (!pAffect)
		return 0;
	
	return pAffect->lApplyValue;
}

int CHARACTER::GetSecondsTillNextMonth()
{
	time_t iTime;
	time(&iTime);
	struct tm endTime = *localtime(&iTime);

	int iCurrentMonth = endTime.tm_mon;

	endTime.tm_hour = 0; 
	endTime.tm_min = 0; 
	endTime.tm_sec = 0;
	endTime.tm_mday = 1;

	if (iCurrentMonth == 12)
	{
		endTime.tm_mon = 0;
		endTime.tm_year = endTime.tm_year + 1;
	}
	else
	{
		endTime.tm_mon = iCurrentMonth + 1;
    }

	int seconds = difftime(mktime(&endTime), iTime);
	
	return seconds;
}
#endif

#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
void CHARACTER::MountSummon(LPITEM mountItem)
{
#define MOUNT_SYSTEM_FIX_POLY
#ifdef MOUNT_SYSTEM_FIX_POLY
	if (IsPolymorphed() == true)
	{
#ifdef TEXTS_IMPROVEMENT
		ChatPacketNew(CHAT_TYPE_INFO, 732, "");
#endif
		return; 
	}
#endif	
	if (GetMapIndex() == 113)
		return;
	
	if (CArenaManager::instance().IsArenaMap(GetMapIndex()) == true)
		return;

	CMountSystem* mountSystem = GetMountSystem();
	DWORD mobVnum = 0;
	
	if (!mountSystem || !mountItem)
		return;
				
#ifdef __CHANGELOOK_SYSTEM__	
	if(mountItem->GetTransmutation())
	{
		const TItemTable* itemTable = ITEM_MANAGER::instance().GetTable(mountItem->GetTransmutation());
		
		if (itemTable)
			mobVnum = itemTable->alValues[1];
		else
			mobVnum = mountItem->GetValue(1);
	}
	else
		mobVnum = mountItem->GetValue(1);
#else
	if(mountItem->GetValue(1) != 0)
		mobVnum = mountItem->GetValue(1);
#endif
	
	if (IsHorseRiding())
		StopRiding();
	
	if (GetHorse())
		HorseSummon(false);
	
	mountSystem->Summon(mobVnum, mountItem, false);
}

void CHARACTER::MountUnsummon(LPITEM mountItem)
{
	CMountSystem* mountSystem = GetMountSystem();
	DWORD mobVnum = 0;
	
	if (!mountSystem || !mountItem)
		return;
	
#ifdef __CHANGELOOK_SYSTEM__	
	if(mountItem->GetTransmutation())
	{
		const TItemTable* itemTable = ITEM_MANAGER::instance().GetTable(mountItem->GetTransmutation());
		
		if (itemTable)
			mobVnum = itemTable->alValues[1];
		else
			mobVnum = mountItem->GetValue(1);
	}
	else
		mobVnum = mountItem->GetValue(1);
#else
	if(mountItem->GetValue(1) != 0)
		mobVnum = mountItem->GetValue(1);
#endif
	
	if (GetMountVnum() == mobVnum)
		mountSystem->Unmount(mobVnum);

	mountSystem->Unsummon(mobVnum);
}

void CHARACTER::CheckMount()
{
	CMountSystem* mountSystem = GetMountSystem();
	LPITEM mountItem = GetWear(WEAR_COSTUME_MOUNT);
	DWORD mobVnum = 0;
	
	if (!mountSystem || !mountItem)
		return;
	
#ifdef __CHANGELOOK_SYSTEM__	
	if(mountItem->GetTransmutation())
	{
		const TItemTable* itemTable = ITEM_MANAGER::instance().GetTable(mountItem->GetTransmutation());
		
		if (itemTable)
			mobVnum = itemTable->alValues[1];
		else
			mobVnum = mountItem->GetValue(1);
	}
	else
		mobVnum = mountItem->GetValue(1);
#else
	if(mountItem->GetValue(1) != 0)
		mobVnum = mountItem->GetValue(1);
#endif
	
	if (mountSystem->CountSummoned() == 0)
	{
		mountSystem->Summon(mobVnum, mountItem, false);
	}
}

bool CHARACTER::IsRidingMount()
{
	return (GetWear(WEAR_COSTUME_MOUNT) || FindAffect(AFFECT_MOUNT));
}
#endif

#ifdef ENABLE_COSTUME_PET
void CHARACTER::UpdatePetSkin() {
	if (!m_newpetSystem)
		return;
	
	m_newpetSystem->UpdatePetSkin();
}

DWORD CHARACTER::GetPetSkinVnum() {
	LPITEM item = GetWear(WEAR_COSTUME_PET_SKIN);
	return item != NULL ? item->GetValue(0) : 0;
}
#endif

#ifdef ENABLE_COSTUME_MOUNT
void CHARACTER::UpdateMountSkin() {
	if (!m_mountSystem)
		return;
	
	m_mountSystem->UpdateMountSkin();
	
	if (IsRiding()) {
		LPITEM item = GetWear(WEAR_COSTUME_MOUNT);
		if (!item)
			return;
		
		DWORD mobVnum = 0;
#ifdef __CHANGELOOK_SYSTEM__
		if(item->GetTransmutation())
		{
			const TItemTable* itemTable = ITEM_MANAGER::instance().GetTable(item->GetTransmutation());
			if (itemTable)
				mobVnum = itemTable->alValues[1];
			else
				mobVnum = item->GetValue(1);
		}
		else
			mobVnum = item->GetValue(1);
#else
		if (item->GetValue(1) != 0)
			mobVnum = item->GetValue(1);
#endif
		
		m_mountSystem->Unmount(mobVnum);
		m_mountSystem->Mount(mobVnum, item);
	}
}

DWORD CHARACTER::GetMountSkinVnum() {
	LPITEM item = GetWear(WEAR_COSTUME_MOUNT_SKIN);
	return item != NULL ? item->GetValue(0) : 0;
}
#endif

#ifdef ENABLE_WHISPER_ADMIN_SYSTEM
std::string CHARACTER::GetLang(){

	auto language = GetDesc()->GetLanguage();
	std::string langs[] = {"en","en","ro","it","tr","de","pl","pt","es","cz","hu"};
	if(language == 0)
		return langs[language+1];
	else
		return langs[language];
}
#endif

long CHARACTER::SetInvincible(bool arg) {
	isInvincible = arg;
	return 1;
}

bool CHARACTER::GetInvincible() {
	return isInvincible;
}

long CHARACTER::IncreaseMobHP(long lArg) {
	long t = GetMaxHP() + lArg;
	SetMaxHP(t);
	SetHP(t);
	PointChange(POINT_HP, t, true);
	return 1;
}

long CHARACTER::IncreaseMobRigHP(long lArg) {
	PointChange(POINT_HP_REGEN, GetPoint(POINT_HP_REGEN) + lArg, true);
	return 1;
}

#ifdef ENABLE_BLOCK_MULTIFARM
void CHARACTER::BlockProcessed() {
	if (!m_pkDropEvent) {
		sys_err("<drop_event> process failed, event is null.");
	} else {
#ifdef TEXTS_IMPROVEMENT
		ChatPacketNew(CHAT_TYPE_INFO, 42, "");
#endif
		event_cancel(&m_pkDropEvent);
		m_pkDropEvent = NULL;
		sys_log(0, "<drop_event> processed.");
	}
}

EVENTFUNC(drop_event)
{
	drop_event_info* info = dynamic_cast<drop_event_info*>(event->info);
	if (!info) {
		sys_err("<drop_event> event is null.");
		return 0;
	}

	LPCHARACTER	ch = info->ch;
	if (!ch) {
		sys_err("<drop_event> ch is null.");
		return 0;
	}

	LPDESC d = ch->GetDesc();
	if (!d) {
		sys_err("<drop_event> %s have no desc connector.", ch->GetName());
		return 0;
	}

	time_t diff = info->time - get_global_time();
	if (diff > 0) {
#ifdef TEXTS_IMPROVEMENT
		ch->ChatPacketNew(CHAT_TYPE_INFO, 43, "%d", diff);
#endif
	} else {
		std::string login = ch->GetDesc()->GetAccountTable().login;
		std::unique_ptr<SQLMsg> msg(DBManager::instance().DirectQuery("SELECT status FROM account.antifarm WHERE login='%s'", login.c_str()));
		if (msg->Get()->uiNumRows > 0) {
			MYSQL_ROW row = mysql_fetch_row(msg->Get()->pSQLResult);
			int iStatus = atoi(row[0]);
			bool already = false;
			if (info->drop) {
				if (iStatus == 1) {
					already = true;
#ifdef TEXTS_IMPROVEMENT
					ch->ChatPacketNew(CHAT_TYPE_INFO, 38, "");
#endif
				} else {
					int c = 0;
					std::unique_ptr<SQLMsg> msg2(DBManager::instance().DirectQuery("SELECT COUNT(*) FROM account.antifarm WHERE hwid='%s' and status=1", d->GetHwid()));
					if (msg2->Get()->uiNumRows > 0) {
						MYSQL_ROW row2 = mysql_fetch_row(msg2->Get()->pSQLResult);
						c = atoi(row2[0]);
					}

					if (c >= 2) {
						already = true;
#ifdef TEXTS_IMPROVEMENT
						ch->ChatPacketNew(CHAT_TYPE_INFO, 37, "");
#endif
					}
					else {
						ch->RemoveAffect(AFFECT_DROP_BLOCK);
						ch->AddAffect(AFFECT_DROP_UNBLOCK, APPLY_NONE, 0, 0, 31536000, 0, true, false);
#ifdef TEXTS_IMPROVEMENT
						ch->ChatPacketNew(CHAT_TYPE_INFO, 40, "");
#endif
					}
				}
			} else {
				if (iStatus == 0) {
					already = true;
#ifdef TEXTS_IMPROVEMENT
					ch->ChatPacketNew(CHAT_TYPE_INFO, 39, "");
#endif
				} else {
					ch->RemoveAffect(AFFECT_DROP_UNBLOCK);
					ch->AddAffect(AFFECT_DROP_BLOCK, APPLY_NONE, 0, 0, 31536000, 0, true, false);
#ifdef TEXTS_IMPROVEMENT
					ch->ChatPacketNew(CHAT_TYPE_INFO, 41, "");
#endif
				}
			}

			if (!already) {
				iStatus = iStatus == 1 ? 0 : 1;
				std::unique_ptr<SQLMsg>(DBManager::instance().DirectQuery("UPDATE account.antifarm SET status=%d WHERE login='%s'", iStatus, login.c_str()));
			}
		}

		ch->BlockProcessed();
	}

	return PASSES_PER_SEC(1);
}

void CHARACTER::BlockDrop() {
	if (!IsPC()) {
		return;
	}

	if (GetMapIndex() != 358 && GetMapIndex() != 359 && GetMapIndex() != 360 && GetMapIndex() != 361) {
#ifdef TEXTS_IMPROVEMENT
		ChatPacketNew(CHAT_TYPE_INFO, 36, "");
#endif
		return;
	}

	if (m_pkDropEvent) {
#ifdef TEXTS_IMPROVEMENT
		ChatPacketNew(CHAT_TYPE_INFO, 44, "");
#endif
		return;
	}

	drop_event_info* info = AllocEventInfo<drop_event_info>();
	info->ch = this;
	info->time = get_global_time() + 5;
	info->drop = false;
	m_pkDropEvent = event_create(drop_event, info, PASSES_PER_SEC(1));
#ifdef TEXTS_IMPROVEMENT
	ChatPacketNew(CHAT_TYPE_INFO, 43, "%d", 5);
#endif
}

void CHARACTER::UnblockDrop() {
	if (GetMapIndex() != 358 && GetMapIndex() != 359 && GetMapIndex() != 360 && GetMapIndex() != 361) {
#ifdef TEXTS_IMPROVEMENT
		ChatPacketNew(CHAT_TYPE_INFO, 36, "");
#endif
		return;
	}
	
	if (m_pkDropEvent) {
#ifdef TEXTS_IMPROVEMENT
		ChatPacketNew(CHAT_TYPE_INFO, 44, "");
#endif
		return;
	}
	
	drop_event_info* info = AllocEventInfo<drop_event_info>();
	info->ch = this;
	info->time = get_global_time() + 5;
	info->drop = true;
	m_pkDropEvent = event_create(drop_event, info, PASSES_PER_SEC(1));
#ifdef TEXTS_IMPROVEMENT
	ChatPacketNew(CHAT_TYPE_INFO, 43, "%d", 5);
#endif
}

void CHARACTER::SetDropStatus() {
	if (!IsPC())
		return;

	std::string login = GetDesc()->GetAccountTable().login;
	std::unique_ptr<SQLMsg> msg(DBManager::instance().DirectQuery("SELECT status FROM account.antifarm WHERE login='%s'", login.c_str()));
	if (msg->Get()->uiNumRows != 0) {
		MYSQL_ROW row = mysql_fetch_row(msg->Get()->pSQLResult);
		int32_t r = atoi(row[0]);
		if (r == 1) {
			RemoveAffect(AFFECT_DROP_BLOCK);
			AddAffect(AFFECT_DROP_UNBLOCK, APPLY_NONE, 0, 0, 31536000, 0, true, false);
		} else {
			RemoveAffect(AFFECT_DROP_UNBLOCK);
			AddAffect(AFFECT_DROP_BLOCK, APPLY_NONE, 0, 0, 31536000, 0, true, false);
		}
	}
}
#endif

#if defined(BL_OFFLINE_MESSAGE)
void CHARACTER::SendOfflineMessage(const char* To, const char* Message)
{
	if (!GetDesc())
		return;

	if (strlen(To) < 1)
		return;

	TPacketGDSendOfflineMessage p;
	strlcpy(p.szFrom, GetName(), sizeof(p.szFrom));
	strlcpy(p.szTo, To, sizeof(p.szTo));
	strlcpy(p.szMessage, Message, sizeof(p.szMessage));
	db_clientdesc->DBPacket(HEADER_GD_SEND_OFFLINE_MESSAGE, GetDesc()->GetHandle(), &p, sizeof(p));

	SetLastOfflinePMTime();
}

void CHARACTER::ReadOfflineMessages()
{
	if (!GetDesc())
		return;

	TPacketGDReadOfflineMessage p;
	strlcpy(p.szName, GetName(), sizeof(p.szName));
	db_clientdesc->DBPacket(HEADER_GD_REQUEST_OFFLINE_MESSAGES, GetDesc()->GetHandle(), &p, sizeof(p));
}
#endif

#ifdef ENABLE_RUNE_SYSTEM
WORD CHARACTER::GetRuneEffect() {
	if (!IsPC())
		return 0;
	
	if (GetQuestFlag("rune.hide_effect") == 1)
		return 0;
	
	WORD r = 1;
	LPITEM pkItem = NULL;
	int iMaxSubTypes = RUNE_SUBTYPES - 1;
	long lMaxTime = 0;
	long lOnePercent = 0;
	long lRemainPercent = 0;
	
	for (int i = 0; i < iMaxSubTypes; i++) {
		pkItem = GetWear(WEAR_RUNE1 + i);
		if (!pkItem) {
			r = 0;
			break;
		}
		else {
			if (pkItem->GetSocket(1) != 1) {
				r = 0;
				break;
			}
			else {
				lMaxTime = pkItem->GetValue(0);
				lOnePercent = lMaxTime / 100;
				lRemainPercent = pkItem->GetSocket(ITEM_SOCKET_REMAIN_SEC) / lOnePercent;
				if (lRemainPercent < RUNE_EFFECT_FROM) {
					r = 0;
					break;
				}
			}
		}
	}
	
	return r;
}
#endif
