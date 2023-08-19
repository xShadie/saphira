#include "stdafx.h"
#include "constants.h"
#include "utils.h"
#include "desc.h"
#include "char.h"
#include "char_manager.h"
#include "mob_manager.h"
#include "party.h"
#include "regen.h"
#include "p2p.h"
#include "dungeon.h"
#include "db.h"
#include "config.h"
#include "xmas_event.h"
#include "questmanager.h"
#include "questlua.h"
#include "locale_service.h"
#include <boost/bind.hpp>

#ifdef ENABLE_ITEMSHOP
#include "itemshop.h"
#endif

CHARACTER_MANAGER::CHARACTER_MANAGER() :
	m_iVIDCount(0),
	m_pkChrSelectedStone(NULL),
	m_bUsePendingDestroy(false)
{
	RegisterRaceNum(xmas::MOB_XMAS_FIRWORK_SELLER_VNUM);
	RegisterRaceNum(xmas::MOB_SANTA_VNUM);
	RegisterRaceNum(xmas::MOB_XMAS_TREE_VNUM);

	m_iMobItemRate = 100;
	m_iMobDamageRate = 100;
	m_iMobGoldAmountRate = 100;
	m_iMobGoldDropRate = 100;
	m_iMobExpRate = 100;

	m_iMobItemRatePremium = 100;
	m_iMobGoldAmountRatePremium = 100;
	m_iMobGoldDropRatePremium = 100;
	m_iMobExpRatePremium = 100;
	
	m_iUserDamageRate = 100;
	m_iUserDamageRatePremium = 100;
#ifdef ENABLE_REWARD_SYSTEM
	m_rewardData.clear();
#endif
	m_map_pkChrByVID.begin();
}

CHARACTER_MANAGER::~CHARACTER_MANAGER()
{
	Destroy();
}

void CHARACTER_MANAGER::Destroy()
{
	auto it = m_map_pkChrByVID.begin();
	while (it != m_map_pkChrByVID.end()) {
		LPCHARACTER ch = it->second;
		M2_DESTROY_CHARACTER(ch);
		it = m_map_pkChrByVID.begin();
	}
#ifdef ENABLE_REWARD_SYSTEM
	m_rewardData.clear();
#endif
}

void CHARACTER_MANAGER::GracefulShutdown()
{
	auto it = m_map_pkPCChr.begin();

	while (it != m_map_pkPCChr.end())
		(it++)->second->Disconnect("GracefulShutdown");
}

DWORD CHARACTER_MANAGER::AllocVID()
{
	++m_iVIDCount;
	return m_iVIDCount;
}

LPCHARACTER CHARACTER_MANAGER::CreateCharacter(const char * name, DWORD dwPID)
{
	DWORD dwVID = AllocVID();
	LPCHARACTER ch = M2_NEW CHARACTER;
	ch->Create(name, dwVID, dwPID ? true : false);
	m_map_pkChrByVID.insert(std::make_pair(dwVID, ch));

	if (dwPID)
	{
		char szName[CHARACTER_NAME_MAX_LEN + 1];
		str_lower(name, szName, sizeof(szName));

		m_map_pkPCChr.insert(NAME_MAP::value_type(szName, ch));
		m_map_pkChrByPID.insert(std::make_pair(dwPID, ch));
	}

	return (ch);
}

void CHARACTER_MANAGER::DestroyCharacter(LPCHARACTER ch)
{
	if (!ch)
		return;

	auto it = m_map_pkChrByVID.find(ch->GetVID());
	if (it == m_map_pkChrByVID.end()) {
		sys_err("[CHARACTER_MANAGER::DestroyCharacter] <Factor> %d not found", (long)(ch->GetVID()));
		return;
	}

	if (ch->IsNPC() && !ch->IsPet() && ch->GetRider() == NULL)
	{
		if (ch->GetDungeon())
		{
			ch->GetDungeon()->DeadCharacter(ch);
		}
	}

	if (m_bUsePendingDestroy)
	{
		m_set_pkChrPendingDestroy.insert(ch);
		return;
	}

	m_map_pkChrByVID.erase(it);

	if (true == ch->IsPC())
	{
		char szName[CHARACTER_NAME_MAX_LEN + 1];

		str_lower(ch->GetName(), szName, sizeof(szName));

		auto it = m_map_pkPCChr.find(szName);

		if (m_map_pkPCChr.end() != it)
			m_map_pkPCChr.erase(it);
	}

	if (0 != ch->GetPlayerID())
	{
		auto it = m_map_pkChrByPID.find(ch->GetPlayerID());

		if (m_map_pkChrByPID.end() != it)
		{
			m_map_pkChrByPID.erase(it);
		}
	}

	UnregisterRaceNumMap(ch);
	RemoveFromStateList(ch);
	M2_DELETE(ch);
}

LPCHARACTER CHARACTER_MANAGER::Find(DWORD dwVID)
{
	auto it = m_map_pkChrByVID.find(dwVID);

	if (m_map_pkChrByVID.end() == it)
		return NULL;

	LPCHARACTER found = it->second;
	if (found != NULL && dwVID != (DWORD)found->GetVID()) {
		sys_err("[CHARACTER_MANAGER::Find] <Factor> %u != %u", dwVID, (DWORD)found->GetVID());
		return NULL;
	}
	return found;
}

LPCHARACTER CHARACTER_MANAGER::Find(const VID & vid)
{
	LPCHARACTER tch = Find((DWORD) vid);

	if (!tch || tch->GetVID() != vid)
		return NULL;

	return tch;
}

LPCHARACTER CHARACTER_MANAGER::FindByPID(DWORD dwPID)
{
	auto it = m_map_pkChrByPID.find(dwPID);

	if (m_map_pkChrByPID.end() == it)
		return NULL;

	LPCHARACTER found = it->second;
	if (found != NULL && dwPID != found->GetPlayerID()) {
		sys_err("[CHARACTER_MANAGER::FindByPID] <Factor> %u != %u", dwPID, found->GetPlayerID());
		return NULL;
	}
	return found;
}

LPCHARACTER CHARACTER_MANAGER::FindPC(const char * name)
{
	char szName[CHARACTER_NAME_MAX_LEN + 1];
	str_lower(name, szName, sizeof(szName));
	auto it = m_map_pkPCChr.find(szName);

	if (it == m_map_pkPCChr.end())
		return NULL;

	LPCHARACTER found = it->second;
	if (found != NULL && strncasecmp(szName, found->GetName(), CHARACTER_NAME_MAX_LEN) != 0) {
		sys_err("[CHARACTER_MANAGER::FindPC] <Factor> %s != %s", name, found->GetName());
		return NULL;
	}
	return found;
}

LPCHARACTER CHARACTER_MANAGER::SpawnMobRandomPosition(DWORD dwVnum, long lMapIndex)
{
	{
		if (dwVnum == 5001 && !quest::CQuestManager::instance().GetEventFlag("japan_regen"))
		{
			sys_log(1, "WAEGU[5001] regen disabled.");
			return NULL;
		}
	}

	{
		if (dwVnum == 5002 && !quest::CQuestManager::instance().GetEventFlag("newyear_mob"))
		{
			sys_log(1, "HAETAE (new-year-mob) [5002] regen disabled.");
			return NULL;
		}
	}

	{
		if (dwVnum == 5004 && !quest::CQuestManager::instance().GetEventFlag("independence_day"))
		{
			sys_log(1, "INDEPENDECE DAY [5004] regen disabled.");
			return NULL;
		}
	}

	const CMob * pkMob = CMobManager::instance().Get(dwVnum);

	if (!pkMob)
	{
		sys_err("no mob data for vnum %u", dwVnum);
		return NULL;
	}

	if (!map_allow_find(lMapIndex))
	{
		sys_err("not allowed map %u", lMapIndex);
		return NULL;
	}

	LPSECTREE_MAP pkSectreeMap = SECTREE_MANAGER::instance().GetMap(lMapIndex);
	if (pkSectreeMap == NULL) {
		return NULL;
	}

	int i;
	long x, y;
	for (i=0; i<2000; i++)
	{
		x = number(1, (pkSectreeMap->m_setting.iWidth / 100)  - 1) * 100 + pkSectreeMap->m_setting.iBaseX;
		y = number(1, (pkSectreeMap->m_setting.iHeight / 100) - 1) * 100 + pkSectreeMap->m_setting.iBaseY;

		LPSECTREE tree = pkSectreeMap->Find(x, y);

		if (!tree)
			continue;

		DWORD dwAttr = tree->GetAttribute(x, y);

		if (IS_SET(dwAttr, ATTR_BLOCK | ATTR_OBJECT))
			continue;

		if (IS_SET(dwAttr, ATTR_BANPK))
			continue;

		break;
	}

	if (i == 2000)
	{
		sys_err("cannot find valid location");
		return NULL;
	}

	LPSECTREE sectree = SECTREE_MANAGER::instance().Get(lMapIndex, x, y);

	if (!sectree)
	{
		sys_log(0, "SpawnMobRandomPosition: cannot create monster at non-exist sectree %d x %d (map %d)", x, y, lMapIndex);
		return NULL;
	}

	LPCHARACTER ch = CHARACTER_MANAGER::instance().CreateCharacter(pkMob->m_table.szLocaleName);

	if (!ch)
	{
		sys_log(0, "SpawnMobRandomPosition: cannot create new character");
		return NULL;
	}

	ch->SetProto(pkMob);

	if (pkMob->m_table.bType == CHAR_TYPE_NPC)
		if (ch->GetEmpire() == 0)
			ch->SetEmpire(SECTREE_MANAGER::instance().GetEmpireFromMapIndex(lMapIndex));

	ch->SetRotation(number(0, 360));

	if (!ch->Show(lMapIndex, x, y, 0, false))
	{
		M2_DESTROY_CHARACTER(ch);
		sys_err(0, "SpawnMobRandomPosition: cannot show monster");
		return NULL;
	}

	char buf[512+1];
	long local_x = x - pkSectreeMap->m_setting.iBaseX;
	long local_y = y - pkSectreeMap->m_setting.iBaseY;
	snprintf(buf, sizeof(buf), "spawn %s[%d] random position at %ld %ld %ld %ld (time: %d)", ch->GetName(), dwVnum, x, y, local_x, local_y, get_global_time());
	
	if (test_server)
		SendNotice(buf);

	sys_log(0, buf);
	return (ch);
}

bool __IsSpawnableInSafeZone(BYTE raceType)
{
	switch (raceType)
	{
	case CHAR_TYPE_NPC:
	case CHAR_TYPE_WARP:
	case CHAR_TYPE_GOTO:
	case CHAR_TYPE_HORSE:
	case CHAR_TYPE_PET:
	case CHAR_TYPE_MOUNT:
		return true;
	default:
		return false;
	}
}

LPCHARACTER CHARACTER_MANAGER::SpawnMob(DWORD dwVnum, long lMapIndex, long x, long y, long z, bool bSpawnMotion, int iRot, bool bShow)
{
	const CMob * pkMob = CMobManager::instance().Get(dwVnum);
	if (!pkMob)
	{
		sys_err("SpawnMob: no mob data for vnum %u", dwVnum);
		return NULL;
	}

	if (!__IsSpawnableInSafeZone(pkMob->m_table.bType) || mining::IsVeinOfOre(dwVnum))
	{
		LPSECTREE tree = SECTREE_MANAGER::instance().Get(lMapIndex, x, y);

		if (!tree)
		{
			sys_log(0, "no sectree for spawn at %d %d mobvnum %d mapindex %d", x, y, dwVnum, lMapIndex);
			return NULL;
		}

		DWORD dwAttr = tree->GetAttribute(x, y);

		bool is_set = false;

		if ( mining::IsVeinOfOre (dwVnum) ) is_set = IS_SET(dwAttr, ATTR_BLOCK);
		else is_set = IS_SET(dwAttr, ATTR_BLOCK | ATTR_OBJECT);

		if ( is_set )
		{
			static bool s_isLog=quest::CQuestManager::instance().GetEventFlag("spawn_block_log");
			static DWORD s_nextTime=get_global_time()+10000;

			DWORD curTime=get_global_time();

			if (curTime>s_nextTime)
			{
				s_nextTime=curTime;
				s_isLog=quest::CQuestManager::instance().GetEventFlag("spawn_block_log");

			}

			if (s_isLog)
				sys_log(0, "SpawnMob: BLOCKED position for spawn %s %u at %d %d (attr %u)", pkMob->m_table.szLocaleName, dwVnum, x, y, dwAttr);
			return NULL;
		}

		if (IS_SET(dwAttr, ATTR_BANPK))
		{
			sys_log(0, "SpawnMob: BAN_PK position for mob spawn %s %u at %d %d", pkMob->m_table.szLocaleName, dwVnum, x, y);
			return NULL;
		}
	}

	LPSECTREE sectree = SECTREE_MANAGER::instance().Get(lMapIndex, x, y);

	if (!sectree)
	{
		sys_log(0, "SpawnMob: cannot create monster at non-exist sectree %d x %d (map %d)", x, y, lMapIndex);
		return NULL;
	}

	LPCHARACTER ch = CHARACTER_MANAGER::instance().CreateCharacter(pkMob->m_table.szLocaleName);

	if (!ch)
	{
		sys_log(0, "SpawnMob: cannot create new character");
		return NULL;
	}

	if (iRot == -1)
		iRot = number(0, 360);

	ch->SetProto(pkMob);

	if (pkMob->m_table.bType == CHAR_TYPE_NPC)
		if (ch->GetEmpire() == 0)
			ch->SetEmpire(SECTREE_MANAGER::instance().GetEmpireFromMapIndex(lMapIndex));

	ch->SetRotation(iRot);

	if (bShow && !ch->Show(lMapIndex, x, y, z, bSpawnMotion))
	{
		M2_DESTROY_CHARACTER(ch);
		sys_log(0, "SpawnMob: cannot show monster");
		return NULL;
	}

	return (ch);
}

LPCHARACTER CHARACTER_MANAGER::SpawnMobRange(DWORD dwVnum, long lMapIndex, int sx, int sy, int ex, int ey, bool bIsException, bool bSpawnMotion, bool bAggressive )
{
	const CMob * pkMob = CMobManager::instance().Get(dwVnum);

	if (!pkMob)
		return NULL;

	if (pkMob->m_table.bType == CHAR_TYPE_STONE)
		bSpawnMotion = true;

	int i = 16;

	while (i--)
	{
		int x = number(sx, ex);
		int y = number(sy, ey);

		LPCHARACTER ch = SpawnMob(dwVnum, lMapIndex, x, y, 0, bSpawnMotion);

		if (ch)
		{
			sys_log(1, "MOB_SPAWN: %s(%d) %dx%d", ch->GetName(), (DWORD) ch->GetVID(), ch->GetX(), ch->GetY());
			if ( bAggressive )
				ch->SetAggressive();
			return (ch);
		}
	}

	return NULL;
}

void CHARACTER_MANAGER::SelectStone(LPCHARACTER pkChr)
{
	m_pkChrSelectedStone = pkChr;
}

bool CHARACTER_MANAGER::SpawnMoveGroup(DWORD dwVnum, long lMapIndex, int sx, int sy, int ex, int ey, int tx, int ty, LPREGEN pkRegen, bool bAggressive_)
{
	CMobGroup * pkGroup = CMobManager::Instance().GetGroup(dwVnum);

	if (!pkGroup)
	{
		sys_err("NOT_EXIST_GROUP_VNUM(%u) Map(%u) ", dwVnum, lMapIndex);
		return false;
	}

	LPCHARACTER pkChrMaster = NULL;
	LPPARTY pkParty = NULL;

	const std::vector<DWORD> & c_rdwMembers = pkGroup->GetMemberVector();

	bool bSpawnedByStone = false;
	bool bAggressive = bAggressive_;

	if (m_pkChrSelectedStone)
	{
		bSpawnedByStone = true;
		if (m_pkChrSelectedStone->GetDungeon())
			bAggressive = true;
	}

	for (DWORD i = 0; i < c_rdwMembers.size(); ++i)
	{
		LPCHARACTER tch = SpawnMobRange(c_rdwMembers[i], lMapIndex, sx, sy, ex, ey, true, bSpawnedByStone);

		if (!tch)
		{
			if (i == 0)
				return false;

			continue;
		}

		sx = tch->GetX() - number(300, 500);
		sy = tch->GetY() - number(300, 500);
		ex = tch->GetX() + number(300, 500);
		ey = tch->GetY() + number(300, 500);

		if (m_pkChrSelectedStone)
			tch->SetStone(m_pkChrSelectedStone);
		else if (pkParty)
		{
			pkParty->Join(tch->GetVID());
			pkParty->Link(tch);
		}
		else if (!pkChrMaster)
		{
			pkChrMaster = tch;
			pkChrMaster->SetRegen(pkRegen);

			pkParty = CPartyManager::instance().CreateParty(pkChrMaster);
		}
		if (bAggressive)
			tch->SetAggressive();

		if (tch->Goto(tx, ty))
			tch->SendMovePacket(FUNC_WAIT, 0, 0, 0, 0);
	}

	return true;
}

bool CHARACTER_MANAGER::SpawnGroupGroup(DWORD dwVnum, long lMapIndex, int sx, int sy, int ex, int ey, LPREGEN pkRegen, bool bAggressive_, LPDUNGEON pDungeon)
{
	const DWORD dwGroupID = CMobManager::Instance().GetGroupFromGroupGroup(dwVnum);

	if( dwGroupID != 0 )
	{
		return SpawnGroup(dwGroupID, lMapIndex, sx, sy, ex, ey, pkRegen, bAggressive_, pDungeon);
	}
	else
	{
		sys_err( "NOT_EXIST_GROUP_GROUP_VNUM(%u) MAP(%ld)", dwVnum, lMapIndex );
		return false;
	}
}

LPCHARACTER CHARACTER_MANAGER::SpawnGroup(DWORD dwVnum, long lMapIndex, int sx, int sy, int ex, int ey, LPREGEN pkRegen, bool bAggressive_, LPDUNGEON pDungeon)
{
	CMobGroup * pkGroup = CMobManager::Instance().GetGroup(dwVnum);

	if (!pkGroup)
	{
		sys_err("NOT_EXIST_GROUP_VNUM(%u) Map(%u) ", dwVnum, lMapIndex);
		return NULL;
	}

	LPCHARACTER pkChrMaster = NULL;
	LPPARTY pkParty = NULL;

	const std::vector<DWORD> & c_rdwMembers = pkGroup->GetMemberVector();

	bool bSpawnedByStone = false;
	bool bAggressive = bAggressive_;

	if (m_pkChrSelectedStone)
	{
		bSpawnedByStone = true;

		if (m_pkChrSelectedStone->GetDungeon())
			bAggressive = true;
	}

	LPCHARACTER chLeader = NULL;

	for (DWORD i = 0; i < c_rdwMembers.size(); ++i)
	{
		LPCHARACTER tch = SpawnMobRange(c_rdwMembers[i], lMapIndex, sx, sy, ex, ey, true, bSpawnedByStone);

		if (!tch)
		{
			if (i == 0)
				return NULL;

			continue;
		}

		if (i == 0)
			chLeader = tch;

		tch->SetDungeon(pDungeon);

		sx = tch->GetX() - number(300, 500);
		sy = tch->GetY() - number(300, 500);
		ex = tch->GetX() + number(300, 500);
		ey = tch->GetY() + number(300, 500);

		if (m_pkChrSelectedStone)
			tch->SetStone(m_pkChrSelectedStone);
		else if (pkParty)
		{
			pkParty->Join(tch->GetVID());
			pkParty->Link(tch);
		}
		else if (!pkChrMaster)
		{
			pkChrMaster = tch;
			pkChrMaster->SetRegen(pkRegen);

			pkParty = CPartyManager::instance().CreateParty(pkChrMaster);
		}

		if (bAggressive)
			tch->SetAggressive();
	}

	return chLeader;
}

struct FuncUpdateAndResetChatCounter
{
	void operator () (LPCHARACTER ch)
	{
		ch->ResetChatCounter();
		ch->CFSM::Update();
	}
};

void CHARACTER_MANAGER::Update(int iPulse)
{
	using namespace std;
	BeginPendingDestroy();

	{
		if (!m_map_pkPCChr.empty())
		{
			CHARACTER_VECTOR v;
			v.reserve(m_map_pkPCChr.size());
			transform(m_map_pkPCChr.begin(), m_map_pkPCChr.end(), back_inserter(v), boost::bind(&NAME_MAP::value_type::second, _1));

			if (0 == (iPulse % PASSES_PER_SEC(5)))
			{
				FuncUpdateAndResetChatCounter f;
				for_each(v.begin(), v.end(), f);
			}
			else
			{
				for_each(v.begin(), v.end(), std::bind(&CHARACTER::UpdateCharacter, std::placeholders::_1, iPulse));
			}
		}
	}

	{
		if (!m_set_pkChrState.empty())
		{
			CHARACTER_VECTOR v;
			v.reserve(m_set_pkChrState.size());
			v.insert(v.end(), m_set_pkChrState.begin(), m_set_pkChrState.end());
			for_each(v.begin(), v.end(), std::bind(&CHARACTER::UpdateStateMachine, std::placeholders::_1, iPulse));
		}
	}

	{
		CharacterVectorInteractor i;

		if (CHARACTER_MANAGER::instance().GetCharactersByRaceNum(xmas::MOB_SANTA_VNUM, i))
		{
			for_each(i.begin(), i.end(),
				std::bind(&CHARACTER::UpdateStateMachine, std::placeholders::_1, iPulse));
		}
	}

	if (0 == (iPulse % PASSES_PER_SEC(3600)))
	{
		m_map_dwMobKillCount.clear();
	}

	if (test_server && 0 == (iPulse % PASSES_PER_SEC(60)))
		sys_log(0, "CHARACTER COUNT vid %zu pid %zu", m_map_pkChrByVID.size(), m_map_pkChrByPID.size());

	FlushPendingDestroy();
}

void CHARACTER_MANAGER::ProcessDelayedSave()
{
	auto it = m_set_pkChrForDelayedSave.begin();

	while (it != m_set_pkChrForDelayedSave.end())
	{
		LPCHARACTER pkChr = *it++;
		pkChr->SaveReal();
	}

	m_set_pkChrForDelayedSave.clear();
}

bool CHARACTER_MANAGER::AddToStateList(LPCHARACTER ch)
{
	assert(ch != NULL);

	auto it = m_set_pkChrState.find(ch);

	if (it == m_set_pkChrState.end())
	{
		m_set_pkChrState.insert(ch);
		return true;
	}

	return false;
}

void CHARACTER_MANAGER::RemoveFromStateList(LPCHARACTER ch)
{
	auto it = m_set_pkChrState.find(ch);

	if (it != m_set_pkChrState.end())
	{
		m_set_pkChrState.erase(it);
	}
}

void CHARACTER_MANAGER::DelayedSave(LPCHARACTER ch)
{
	m_set_pkChrForDelayedSave.insert(ch);
}

bool CHARACTER_MANAGER::FlushDelayedSave(LPCHARACTER ch)
{
	auto it = m_set_pkChrForDelayedSave.find(ch);

	if (it == m_set_pkChrForDelayedSave.end())
		return false;

	m_set_pkChrForDelayedSave.erase(it);
	ch->SaveReal();
	return true;
}

void CHARACTER_MANAGER::KillLog(DWORD dwVnum)
{
	const DWORD SEND_LIMIT = 10000;

	auto it = m_map_dwMobKillCount.find(dwVnum);

	if (it == m_map_dwMobKillCount.end())
		m_map_dwMobKillCount.insert(std::make_pair(dwVnum, 1));
	else
	{
		++it->second;

		if (it->second > SEND_LIMIT)
		{
			m_map_dwMobKillCount.erase(it);
		}
	}
}

void CHARACTER_MANAGER::RegisterRaceNum(DWORD dwVnum)
{
	m_set_dwRegisteredRaceNum.insert(dwVnum);
}

void CHARACTER_MANAGER::RegisterRaceNumMap(LPCHARACTER ch)
{
	DWORD dwVnum = ch->GetRaceNum();

	if (m_set_dwRegisteredRaceNum.find(dwVnum) != m_set_dwRegisteredRaceNum.end())
	{
		m_map_pkChrByRaceNum[dwVnum].insert(ch);
	}
}

void CHARACTER_MANAGER::UnregisterRaceNumMap(LPCHARACTER ch)
{
	DWORD dwVnum = ch->GetRaceNum();

	auto it = m_map_pkChrByRaceNum.find(dwVnum);

	if (it != m_map_pkChrByRaceNum.end())
		it->second.erase(ch);
}

bool CHARACTER_MANAGER::GetCharactersByRaceNum(DWORD dwRaceNum, CharacterVectorInteractor & i)
{
	auto it = m_map_pkChrByRaceNum.find(dwRaceNum);

	if (it == m_map_pkChrByRaceNum.end())
		return false;

	i = it->second;
	return true;
}

#define FIND_JOB_WARRIOR_0	(1 << 3)
#define FIND_JOB_WARRIOR_1	(1 << 4)
#define FIND_JOB_WARRIOR_2	(1 << 5)
#define FIND_JOB_WARRIOR	(FIND_JOB_WARRIOR_0 | FIND_JOB_WARRIOR_1 | FIND_JOB_WARRIOR_2)
#define FIND_JOB_ASSASSIN_0	(1 << 6)
#define FIND_JOB_ASSASSIN_1	(1 << 7)
#define FIND_JOB_ASSASSIN_2	(1 << 8)
#define FIND_JOB_ASSASSIN	(FIND_JOB_ASSASSIN_0 | FIND_JOB_ASSASSIN_1 | FIND_JOB_ASSASSIN_2)
#define FIND_JOB_SURA_0		(1 << 9)
#define FIND_JOB_SURA_1		(1 << 10)
#define FIND_JOB_SURA_2		(1 << 11)
#define FIND_JOB_SURA		(FIND_JOB_SURA_0 | FIND_JOB_SURA_1 | FIND_JOB_SURA_2)
#define FIND_JOB_SHAMAN_0	(1 << 12)
#define FIND_JOB_SHAMAN_1	(1 << 13)
#define FIND_JOB_SHAMAN_2	(1 << 14)
#define FIND_JOB_SHAMAN		(FIND_JOB_SHAMAN_0 | FIND_JOB_SHAMAN_1 | FIND_JOB_SHAMAN_2)
#ifdef ENABLE_WOLFMAN
#define FIND_JOB_WOLFMAN_0 = (1 << 15)
#define FIND_JOB_WOLFMAN_1 = (1 << 16)
#define FIND_JOB_WOLFMAN_2 = (1 << 17)
#define FIND_JOB_WOLFMAN = (FIND_JOB_WOLFMAN_0 | FIND_JOB_WOLFMAN_1 | FIND_JOB_WOLFMAN_2)
#endif

LPCHARACTER CHARACTER_MANAGER::FindSpecifyPC(unsigned int uiJobFlag, long lMapIndex, LPCHARACTER except, int iMinLevel, int iMaxLevel)
{
	LPCHARACTER chFind = NULL;
	int n = 0;

	for (auto it = m_map_pkChrByPID.begin(); it != m_map_pkChrByPID.end(); ++it)
	{
		LPCHARACTER ch = it->second;

		if (ch == except)
			continue;

		if (ch->GetLevel() < iMinLevel)
			continue;

		if (ch->GetLevel() > iMaxLevel)
			continue;

		if (ch->GetMapIndex() != lMapIndex)
			continue;

		if (uiJobFlag)
		{
			unsigned int uiChrJob = (1 << ((ch->GetJob() + 1) * 3 + ch->GetSkillGroup()));

			if (!IS_SET(uiJobFlag, uiChrJob))
				continue;
		}

		if (!chFind || number(1, ++n) == 1)
			chFind = ch;
	}

	return chFind;
}

int CHARACTER_MANAGER::GetMobItemRate(LPCHARACTER ch)	
{ 
	if (ch && ch->GetPremiumRemainSeconds(PREMIUM_ITEM) > 0)
		return m_iMobItemRatePremium;
	return m_iMobItemRate; 
}

int CHARACTER_MANAGER::GetMobDamageRate(LPCHARACTER ch)	
{ 
	return m_iMobDamageRate; 
}

int CHARACTER_MANAGER::GetMobGoldAmountRate(LPCHARACTER ch)
{ 
	if ( !ch )
		return m_iMobGoldAmountRate;

	if (ch && ch->GetPremiumRemainSeconds(PREMIUM_GOLD) > 0)
		return m_iMobGoldAmountRatePremium;
	return m_iMobGoldAmountRate; 
}

int CHARACTER_MANAGER::GetMobGoldDropRate(LPCHARACTER ch)
{
	if ( !ch )
		return m_iMobGoldDropRate;
	
	if (ch && ch->GetPremiumRemainSeconds(PREMIUM_GOLD) > 0)
		return m_iMobGoldDropRatePremium;
	return m_iMobGoldDropRate;
}

int CHARACTER_MANAGER::GetMobExpRate(LPCHARACTER ch)
{ 
	if ( !ch )
		return m_iMobExpRate;

	if (ch && ch->GetPremiumRemainSeconds(PREMIUM_EXP) > 0)
		return m_iMobExpRatePremium;
	return m_iMobExpRate; 
}

int	CHARACTER_MANAGER::GetUserDamageRate(LPCHARACTER ch)
{
	if (!ch)
		return m_iUserDamageRate;

	if (ch && ch->GetPremiumRemainSeconds(PREMIUM_EXP) > 0)
		return m_iUserDamageRatePremium;

	return m_iUserDamageRate;
}

void CHARACTER_MANAGER::SendScriptToMap(long lMapIndex, const std::string & s)
{
	LPSECTREE_MAP pSecMap = SECTREE_MANAGER::instance().GetMap(lMapIndex);

	if (NULL == pSecMap)
		return;

	struct packet_script p;

	p.header = HEADER_GC_SCRIPT;
	p.skin = 1;
	p.src_size = s.size();

	quest::FSendPacket f;
	p.size = p.src_size + sizeof(struct packet_script);
	f.buf.write(&p, sizeof(struct packet_script));
	f.buf.write(&s[0], s.size());

	pSecMap->for_each(f);
}

bool CHARACTER_MANAGER::BeginPendingDestroy()
{
	if (m_bUsePendingDestroy)
		return false;

	m_bUsePendingDestroy = true;
	return true;
}

void CHARACTER_MANAGER::FlushPendingDestroy()
{
	using namespace std;

	m_bUsePendingDestroy = false;

	if (!m_set_pkChrPendingDestroy.empty())
	{
		sys_log(0, "FlushPendingDestroy size %d", m_set_pkChrPendingDestroy.size());
		
		auto it = m_set_pkChrPendingDestroy.begin(),
			end = m_set_pkChrPendingDestroy.end();
		for ( ; it != end; ++it) {
			M2_DESTROY_CHARACTER(*it);
		}

		m_set_pkChrPendingDestroy.clear();
	}
}

CharacterVectorInteractor::CharacterVectorInteractor(const CHARACTER_SET & r)
{
	using namespace std;

	reserve(r.size());
	insert(end(), r.begin(), r.end());

	if (CHARACTER_MANAGER::instance().BeginPendingDestroy())
		m_bMyBegin = true;
}

CharacterVectorInteractor::~CharacterVectorInteractor()
{
	if (m_bMyBegin)
		CHARACTER_MANAGER::instance().FlushPendingDestroy();
}

#ifdef ENABLE_REWARD_SYSTEM
const char* GetRewardIndexToString(BYTE rewardIndex)
{
	std::map<BYTE, std::string> rewardNames = {
		{REWARD_115,"REWARD_115"},
		{REWARD_PET_115,"REWARD_PET_115"},
		{REWARD_120,"REWARD_120"},
		{REWARD_LEGENDARY_SKILL,"REWARD_LEGENDARY_SKILL"},
		{REWARD_LEGENDARY_SKILL_SET,"REWARD_LEGENDARY_SKILL_SET"},
		{REWARD_THANDRUIL,"REWARD_THANDRUIL"},
		{REWARD_HYDRA,"REWARD_HYDRA"},
		{REWARD_CRYSTAL_DRAGON,"REWARD_CRYSTAL_DRAGON"},
		{REWARD_OFFLINESHOP_SLOT,"REWARD_OFFLINESHOP_SLOT"},
		{REWARD_INVENTORY_SLOT,"REWARD_INVENTORY_SLOT"},
		{REWARD_AVERAGE,"REWARD_AVERAGE"},
		{REWARD_ELEMENT,"REWARD_ELEMENT"},
		{REWARD_BATTLEPASS,"REWARD_BATTLEPASS"},
		{REWARD_CUSTOM_SASH,"REWARD_CUSTOM_SASH"},
		{REWARD_AURA,"REWARD_AURA"},
		{REWARD_ENERGY,"REWARD_ENERGY"},
		{REWARD_112_BIO,"REWARD_112_BIO"},
		{REWARD_120_BIO,"REWARD_120_BIO"},
		{REWARD_LEADER_SHIP,"REWARD_LEADER_SHIP"},
		{REWARD_BUFFI_LEGENDARY_SKILL,"REWARD_BUFFI_LEGENDARY_SKILL"},
	};
	auto it = rewardNames.find(rewardIndex);
	if (it != rewardNames.end())
		return it->second.c_str();
	return 0;
}
BYTE GetRewardIndex(const char* szRewardName)
{
	std::map<std::string, BYTE> rewardNames = {
		{"REWARD_115",REWARD_115},
		{"REWARD_PET_115",REWARD_PET_115},
		{"REWARD_120",REWARD_120},
		{"REWARD_LEGENDARY_SKILL",REWARD_LEGENDARY_SKILL},
		{"REWARD_LEGENDARY_SKILL_SET",REWARD_LEGENDARY_SKILL_SET},
		{"REWARD_THANDRUIL",REWARD_THANDRUIL},
		{"REWARD_HYDRA",REWARD_HYDRA},
		{"REWARD_CRYSTAL_DRAGON",REWARD_CRYSTAL_DRAGON},
		{"REWARD_OFFLINESHOP_SLOT",REWARD_OFFLINESHOP_SLOT},
		{"REWARD_INVENTORY_SLOT",REWARD_INVENTORY_SLOT},
		{"REWARD_AVERAGE",REWARD_AVERAGE},
		{"REWARD_ELEMENT",REWARD_ELEMENT},
		{"REWARD_BATTLEPASS",REWARD_BATTLEPASS},
		{"REWARD_CUSTOM_SASH",REWARD_CUSTOM_SASH},
		{"REWARD_AURA",REWARD_AURA},
		{"REWARD_ENERGY",REWARD_ENERGY},
		{"REWARD_112_BIO",REWARD_112_BIO},
		{"REWARD_120_BIO",REWARD_120_BIO},
		{"REWARD_LEADER_SHIP",REWARD_LEADER_SHIP},
		{"REWARD_BUFFI_LEGENDARY_SKILL",REWARD_BUFFI_LEGENDARY_SKILL},
	};
	auto it = rewardNames.find(szRewardName);
	if (it != rewardNames.end())
		return it->second;
	return 0;
}
void CHARACTER_MANAGER::LoadRewardData()
{
	m_rewardData.clear();

	char szQuery[QUERY_MAX_LEN];
	snprintf(szQuery, sizeof(szQuery), "SELECT rewardIndex, lc_text, playerName, itemVnum0, itemCount0, itemVnum1, itemCount1, itemVnum2, itemCount2 FROM player.reward_table");
	std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery(szQuery));

	if (pMsg->Get()->uiNumRows != 0)
	{
		MYSQL_ROW row = NULL;
		for (int n = 0; (row = mysql_fetch_row(pMsg->Get()->pSQLResult)) != NULL; ++n)
		{
			int col = 0;
			TRewardInfo p;
			p.m_rewardItems.clear();

			char rewardName[50];
			DWORD rewardIndex;
			strlcpy(rewardName, row[col++], sizeof(rewardName));
			rewardIndex = GetRewardIndex(rewardName);

			strlcpy(p.lc_text, row[col++], sizeof(p.lc_text));
			strlcpy(p.playerName, row[col++], sizeof(p.playerName));
			for (BYTE j = 0; j < 3; ++j)
			{
				DWORD itemVnum, itemCount;
				str_to_number(itemVnum, row[col++]);
				str_to_number(itemCount, row[col++]);
				p.m_rewardItems.emplace_back(itemVnum, itemCount);
			}
			m_rewardData.emplace(rewardIndex, p);
		}
	}
}
bool CHARACTER_MANAGER::IsRewardEmpty(BYTE rewardIndex)
{
	auto it = m_rewardData.find(rewardIndex);
	if (it != m_rewardData.end())
	{
		if (strcmp(it->second.playerName, "") == 0)
			return true;
	}
	return false;
}
void CHARACTER_MANAGER::SendRewardInfo(LPCHARACTER ch)
{
	ch->SetProtectTime("RewardInfo", 1);
	std::string cmd = "";//12
	if (m_rewardData.size())
	{
		for (auto it = m_rewardData.begin(); it != m_rewardData.end(); ++it)
		{
			if (strlen(it->second.playerName) > 1)
			{
				char text[60];
				snprintf(text, sizeof(text), "%d|%s#", it->first, it->second.playerName);
				cmd += text;
			}
			if (strlen(cmd.c_str()) + 12 > CHAT_MAX_LEN - 30)
			{
				ch->ChatPacket(CHAT_TYPE_COMMAND, "RewardInfo %s", cmd.c_str());
				cmd = "";
			}
		}
		if (strlen(cmd.c_str()) > 1)
			ch->ChatPacket(CHAT_TYPE_COMMAND, "RewardInfo %s", cmd.c_str());
	}
}

struct RewardForEach
{
	void operator() (LPDESC d)
	{
		LPCHARACTER ch = d->GetCharacter();
		if (ch == NULL)
			return;
		else if (ch->GetProtectTime("RewardInfo") != 1)
			return;
		CHARACTER_MANAGER::Instance().SendRewardInfo(ch);
	}
};
void CHARACTER_MANAGER::SetRewardData(BYTE rewardIndex, const char* playerName, bool isP2P)
{
	if (!IsRewardEmpty(rewardIndex))
		return;

	auto it = m_rewardData.find(rewardIndex);
	if (it == m_rewardData.end())
		return;
	TRewardInfo& rewardInfo = it->second;
	strlcpy(rewardInfo.playerName, playerName, sizeof(rewardInfo.playerName));

	if (isP2P)
	{
		LPCHARACTER ch = FindPC(playerName);
		if (ch)
		{
			for (DWORD j = 0; j < rewardInfo.m_rewardItems.size(); ++j)
				ch->AutoGiveItem(rewardInfo.m_rewardItems[j].first, rewardInfo.m_rewardItems[j].second);
		}

		char msg[CHAT_MAX_LEN + 1];
		//snprintf(msg, sizeof(msg), LC_TEXT(it->second.lc_text), playerName);
		snprintf(msg, sizeof(msg), rewardInfo.lc_text, playerName);
		BroadcastNotice(msg);

		TPacketGGRewardInfo p;
		p.bHeader = HEADER_GG_REWARD_INFO;
		p.rewardIndex = rewardIndex;
		strlcpy(p.playerName, playerName, sizeof(p.playerName));
		P2P_MANAGER::Instance().Send(&p, sizeof(p));

		char szQuery[1024];
		snprintf(szQuery, sizeof(szQuery), "UPDATE player.reward_table SET playerName = '%s' WHERE lc_text = '%s'", playerName, rewardInfo.lc_text);
		std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery(szQuery));
	}
	const DESC_MANAGER::DESC_SET& c_ref_set = DESC_MANAGER::instance().GetClientSet();
	std::for_each(c_ref_set.begin(), c_ref_set.end(), RewardForEach());
}
#endif

#ifdef ENABLE_ITEMSHOP
void CHARACTER_MANAGER::SendItemshopSingleRemoveItem(TItemshopItemTable* item)
{
	for (const auto& [player_id, ch] : m_map_pkChrByPID)
	{
		if (!ch)
			continue;

		ch->SendItemshopSingleItemRemovePacket(item);
	}
}

void CHARACTER_MANAGER::SendItemshopSingleAddItem(TItemshopItemTable* item)
{
	const auto& categories = CItemshopManager::instance().GetItemshopCategories();

	const auto& category_info = categories.find(item->byCategory);

	if (category_info == categories.end())
	{
		// This could NEVER happen but it seems like it does
		return;
	}

	for (const auto& [player_id, ch] : m_map_pkChrByPID)
	{
		if (!ch)
			continue;

		ch->SendItemshopSingleItemAddPacket(item, category_info->second);
	}
}

void CHARACTER_MANAGER::SendItemshopSingleItemRefresh(TItemshopItemTable* item)
{
	for (const auto& [player_id, ch] : m_map_pkChrByPID)
	{
		if (!ch)
			continue;

		ch->SendItemshopSingleItemRefreshPacket(item);
	}
}

void CHARACTER_MANAGER::SendItemshopItems()
{
	const auto& items = CItemshopManager::instance().GetItemshopItems();
	const auto& categories = CItemshopManager::instance().GetItemshopCategories();

	for (const auto& [player_id, ch] : m_map_pkChrByPID)
	{
		if (!ch)
			continue;

		ch->SendItemshopItemsPacket(items, categories);
	}
}
#endif

#ifdef ENABLE_MULTI_FARM_BLOCK
void CHARACTER_MANAGER::RemoveMultiFarm(const char* szIP, const DWORD playerID, const bool isP2P)
{
	if (!isP2P)
	{
		TPacketGGMultiFarm p;
		p.header = HEADER_GG_MULTI_FARM;
		p.subHeader = MULTI_FARM_REMOVE;
		p.playerID = playerID;
		strlcpy(p.playerIP, szIP, sizeof(p.playerIP));
		P2P_MANAGER::Instance().Send(&p, sizeof(TPacketGGMultiFarm));
	}

	auto it = m_mapmultiFarm.find(szIP);
	if (it != m_mapmultiFarm.end())
	{
		for (auto itVec = it->second.begin(); itVec != it->second.end(); ++itVec)
		{
			if (itVec->playerID == playerID)
			{
				it->second.erase(itVec);
				break;
			}
		}
		if (!it->second.size())
			m_mapmultiFarm.erase(szIP);
	}
}
void CHARACTER_MANAGER::SetMultiFarm(const char* szIP, const DWORD playerID, const bool bStatus, const BYTE affectType, const int affectTime)
{
	const auto it = m_mapmultiFarm.find(szIP);
	if (it != m_mapmultiFarm.end())
	{
		for (auto itVec = it->second.begin(); itVec != it->second.end(); ++itVec)
		{
			if (itVec->playerID == playerID)
			{
				itVec->farmStatus = bStatus;
				itVec->affectType = affectType;
				itVec->affectTime = affectTime;
				return;
			}
		}
		it->second.emplace_back(TMultiFarm(playerID, bStatus, affectType, affectTime));
	}
	else
	{
		std::vector<TMultiFarm> m_vecFarmList;
		m_vecFarmList.emplace_back(TMultiFarm(playerID, bStatus, affectType, affectTime));
		m_mapmultiFarm.emplace(szIP, m_vecFarmList);
	}
}
int CHARACTER_MANAGER::GetMultiFarmCount(const char* playerIP)
{
	int accCount = 0;
	bool affectTimeHas = false;
	BYTE affectType = 0;
	const auto it = m_mapmultiFarm.find(playerIP);
	if (it != m_mapmultiFarm.end())
	{
		for (auto itVec = it->second.begin(); itVec != it->second.end(); ++itVec)
		{
			if (itVec->farmStatus)
				accCount++;
			if (itVec->affectTime > get_global_time())
				affectTimeHas = true;
			if (itVec->affectType > affectType)
				affectType = itVec->affectType;
		}
	}

	if (affectTimeHas && affectType > 0)
		accCount -= affectType;
	if (accCount < 0)
		accCount = 0;

	return accCount;
}
void CHARACTER_MANAGER::CheckMultiFarmAccount(const char* szIP, const DWORD playerID, const bool bStatus, BYTE affectType, int affectDuration, bool isP2P)
{
	LPCHARACTER ch = FindByPID(playerID);

	if (ch && bStatus)
	{
		affectDuration = ch->FindAffect(AFFECT_MULTI_FARM_PREMIUM) ? get_global_time() + ch->FindAffect(AFFECT_MULTI_FARM_PREMIUM)->lDuration : 0;
		affectType = ch->FindAffect(AFFECT_MULTI_FARM_PREMIUM) ? ch->FindAffect(AFFECT_MULTI_FARM_PREMIUM)->lApplyValue : 0;
	}

	if (bStatus)
	{
		const int farmPlayerCount = GetMultiFarmCount(szIP);
		if (farmPlayerCount >= 2)
		{
			CheckMultiFarmAccount(szIP, playerID, false);
			return;
		}
	}

	if (!isP2P)
	{
		TPacketGGMultiFarm p;
		p.header = HEADER_GG_MULTI_FARM;
		p.subHeader = MULTI_FARM_SET;
		p.playerID = playerID;
		strlcpy(p.playerIP, szIP, sizeof(p.playerIP));
		p.farmStatus = bStatus;
		p.affectType = affectType;
		p.affectTime = affectDuration;
		P2P_MANAGER::Instance().Send(&p, sizeof(TPacketGGMultiFarm));
	}

	if (ch)
	{
		ch->SetRewardStatus(bStatus);
		ch->ChatPacket(CHAT_TYPE_COMMAND, "UpdateMultiFarmAffect %d", bStatus);
	}

	SetMultiFarm(szIP, playerID, bStatus, affectType, affectDuration);
}
#endif

#ifdef ENABLE_BATTLE_PASS
BYTE GetBattlePassMissionIndex(const char* szMissionName)
{
	const std::map<std::string, BYTE> eventNames = {
		{"MISSION_BOSS",MISSION_BOSS},
		{"MISSION_CATCH_FISH",MISSION_CATCH_FISH},
		{"MISSION_CRAFT_ITEM",MISSION_CRAFT_ITEM},
		{"MISSION_CRAFT_GAYA",MISSION_CRAFT_GAYA},
		{"MISSION_DESTROY_ITEM",MISSION_DESTROY_ITEM},
		{"MISSION_DUNGEON",MISSION_DUNGEON},
		{"MISSION_EARN_MONEY",MISSION_EARN_MONEY},
		{"MISSION_FEED_PET",MISSION_FEED_PET},
		{"MISSION_LEVEL_UP",MISSION_LEVEL_UP},
		{"MISSION_MONSTER",MISSION_MONSTER},
		{"MISSION_MOUNT_TIME",MISSION_MOUNT_TIME},
		{"MISSION_OPEN_OFFLINESHOP",MISSION_OPEN_OFFLINESHOP},
		{"MISSION_PLAYTIME",MISSION_PLAYTIME},
		{"MISSION_REFINE_ITEM",MISSION_REFINE_ITEM},
		{"MISSION_REFINE_ALCHEMY",MISSION_REFINE_ALCHEMY},
		{"MISSION_SASH",MISSION_SASH},
		{"MISSION_SELL_ITEM",MISSION_SELL_ITEM},
		{"MISSION_SPEND_MONEY",MISSION_SPEND_MONEY},
		{"MISSION_SPRITE_STONE",MISSION_SPRITE_STONE},
		{"MISSION_STONE",MISSION_STONE},
		{"MISSION_USE_EMOTICON",MISSION_USE_EMOTICON},
		{"MISSION_WHISPER",MISSION_WHISPER},
		{"MISSION_SHOUT_CHAT",MISSION_SHOUT_CHAT},
		{"MISSION_KILLPLAYER",MISSION_KILLPLAYER},
	};
	const auto it = eventNames.find(szMissionName);
	if (it != eventNames.end())
		return it->second;
	return 0;
}
void CHARACTER_MANAGER::DoMission(LPCHARACTER ch, BYTE missionIndex, long long value, DWORD subValue)
{
	for (BYTE j = 0; j < BATTLE_PASS_TYPE_MAX; ++j)
	{
		if (!ch->IsBattlePassActive(j))
			continue;
		else if (!IsBattlePassMissionHave(j, ch, missionIndex))
			continue;

		const DWORD missionSubValue = GetMissionSubValue(j, missionIndex);
		if (missionSubValue != 0)
			if (subValue != missionSubValue)
				continue;

		const long long missionMaxValue = GetMissionMaxValue(j, missionIndex);
		const long long selfValue = ch->GetBattlePassMissonValue(j, missionIndex);
		if (selfValue >= missionMaxValue)
			continue;

		long long newValue = selfValue + std::abs(value);
		if (newValue >= missionMaxValue)
		{
			newValue = missionMaxValue;
			ch->SetBattlePassMissonValue(j, missionIndex, newValue);
			ch->ChatPacket(CHAT_TYPE_INFO, "1108");
			GiveLowMissionReward(j, ch, missionIndex);
			if (CheckBattlePassDone(j, ch))
			{
				char noticetext[256];
				snprintf(noticetext, sizeof(noticetext), "1107 %s", ch->GetName());
				BroadcastNotice(noticetext);
				ch->ChatPacket(CHAT_TYPE_INFO, "1109");
			}
		}
		else
			ch->SetBattlePassMissonValue(j, missionIndex, newValue);
		if (ch->GetProtectTime("battlePassOpen") == 1)
			ch->ChatPacket(CHAT_TYPE_COMMAND, "BattlePassSetMission %d %d %lld", j, missionIndex, newValue);
	}
}
bool CHARACTER_MANAGER::IsBattlePassMissionHave(BYTE battlePassType, LPCHARACTER ch, BYTE missionIndex)
{
	const time_t cur_Time = time(NULL);
	const struct tm vKey = *localtime(&cur_Time);
	const auto it = m_BattlePassData[battlePassType].find(vKey.tm_mon);
	if (it != m_BattlePassData[battlePassType].end())
	{
		if (it->second.second.missionData.find(missionIndex) != it->second.second.missionData.end())
			return true;
	}
	return false;
}
void CHARACTER_MANAGER::GiveLowMissionReward(BYTE battlePassType, LPCHARACTER ch, BYTE missionIndex)
{
	const time_t cur_Time = time(NULL);
	const struct tm vKey = *localtime(&cur_Time);

	const auto it = m_BattlePassData[battlePassType].find(vKey.tm_mon);
	if (it != m_BattlePassData[battlePassType].end())
	{
		const auto lowReward = it->second.second.subReward.find(missionIndex);
		if (lowReward != it->second.second.subReward.end())
		{
			for (DWORD j = 0; j < lowReward->second.size(); ++j)
				ch->AutoGiveItem(lowReward->second[j].first, lowReward->second[j].second);
		}
	}
}
long long CHARACTER_MANAGER::GetMissionMaxValue(BYTE battlePassType, BYTE missionIndex)
{
	const time_t cur_Time = time(NULL);
	const struct tm vKey = *localtime(&cur_Time);

	const auto it = m_BattlePassData[battlePassType].find(vKey.tm_mon);
	if (it != m_BattlePassData[battlePassType].end())
	{
		const auto itEx = it->second.second.missionData.find(missionIndex);
		if (itEx != it->second.second.missionData.end())
			return itEx->second.first;
	}
	return 0;
}
DWORD CHARACTER_MANAGER::GetMissionSubValue(BYTE battlePassType, BYTE missionIndex)
{
	const time_t cur_Time = time(NULL);
	const struct tm vKey = *localtime(&cur_Time);

	const auto it = m_BattlePassData[battlePassType].find(vKey.tm_mon);
	if (it != m_BattlePassData[battlePassType].end())
	{
		const auto itEx = it->second.second.missionData.find(missionIndex);
		if (itEx != it->second.second.missionData.end())
			return itEx->second.second;
	}
	return 0;
}
bool CHARACTER_MANAGER::CheckBattlePassDone(BYTE battlePassType, LPCHARACTER ch)
{
	if (!ch->IsBattlePassActive(battlePassType))
		return false;
	const time_t cur_Time = time(NULL);
	const struct tm vKey = *localtime(&cur_Time);

	const auto it = m_BattlePassData[battlePassType].find(vKey.tm_mon);
	if (it != m_BattlePassData[battlePassType].end())
	{
		const auto missionData = it->second.second.missionData;
		if (missionData.size())
		{
			for (auto itMission = missionData.begin(); itMission != missionData.end(); ++itMission)
			{
				if (ch->GetBattlePassMissonValue(battlePassType, itMission->first) < itMission->second.first)
					return false;
			}
		}
	}
	return true;
}
void CHARACTER_MANAGER::CheckBattlePassReward(BYTE battlePassType, LPCHARACTER ch)
{
	if (!CheckBattlePassDone(battlePassType, ch))
		return;
	const time_t cur_Time = time(NULL);
	const struct tm vKey = *localtime(&cur_Time);

	const auto it = m_BattlePassData[battlePassType].find(vKey.tm_mon);
	if (it != m_BattlePassData[battlePassType].end())
	{
		const auto rewardData = it->second.second.rewardData;
		if (rewardData.size())
		{
			for (auto itReward = rewardData.begin(); itReward != rewardData.end(); ++itReward)
				ch->AutoGiveItem(itReward->first, itReward->second);
		}
	}
	ch->ClearBattlePassData(battlePassType);
	if (battlePassType == BATTLE_PASS_TYPE_NORMAL)
		ch->SetQuestFlag("battlePass.status", 2);
	else if (battlePassType == BATTLE_PASS_TYPE_ELITE);
	ch->SetQuestFlag("battlePass.status_elite", 2);
	ch->ChatPacket(CHAT_TYPE_COMMAND, "BattlePassSetStatusEx %d 2", battlePassType);
}
void CHARACTER_MANAGER::LoadBattlePassData(BYTE battlePassType, LPCHARACTER ch)
{
	const time_t cur_Time = time(NULL);
	const struct tm vKey = *localtime(&cur_Time);

	const auto it = m_BattlePassData[battlePassType].find(vKey.tm_mon);
	if (it != m_BattlePassData[battlePassType].end())
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "BattlePassClear %d", battlePassType);
		std::string rewardText = "";
		const auto missionData = it->second.second.missionData;
		if (missionData.size())
		{
			for (auto itMission = missionData.begin(); itMission != missionData.end(); ++itMission)
			{
				const auto subReward = it->second.second.subReward;
				const auto rewardList = subReward.find(itMission->first);
				if (rewardList != subReward.end())
				{
					const auto itRewardList = rewardList->second;
					if (itRewardList.size() == 0)
						rewardText = "Empty";
					else
					{
						for (BYTE j = 0; j < itRewardList.size(); ++j)
						{
							rewardText += std::to_string(itRewardList[j].first);
							rewardText += "|";
							rewardText += std::to_string(itRewardList[j].second);
							rewardText += "#";
						}
					}
				}

				int vecList = 0;
				const auto missinvecIndexList = it->second.second.missionIndex;
				const auto missinvecIndex = missinvecIndexList.find(itMission->first);
				if (missinvecIndex != missinvecIndexList.end())
					vecList = missinvecIndex->second;

				ch->ChatPacket(CHAT_TYPE_COMMAND, "BattlePassAppendMission %d %u %s %lld %lld %u %d", battlePassType, itMission->first, rewardText.c_str(), ch->GetBattlePassMissonValue(battlePassType, itMission->first), itMission->second.first, itMission->second.second, vecList);
				rewardText = "";
			}
		}
		const auto rewardData = it->second.second.rewardData;
		for (DWORD j = 0; j < rewardData.size(); ++j)
		{
			rewardText += std::to_string(rewardData[j].first);
			rewardText += "|";
			rewardText += std::to_string(rewardData[j].second);
			rewardText += "#";
		}
		ch->ChatPacket(CHAT_TYPE_COMMAND, "BattlePassSetStatus %d %d %d %s", battlePassType, ch->GetBattlePassStatus(battlePassType), it->second.first - time(0), rewardText.c_str());
	}
}

void CHARACTER_MANAGER::InitializeBattlePass()
{
	for (BYTE j = 0; j < BATTLE_PASS_TYPE_MAX; ++j)
		m_BattlePassData[j].clear();

	std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery("SELECT * FROM player.battle_pass"));
	if (pMsg->Get()->uiNumRows != 0)
	{
		time_t cur_Time = time(NULL);
		struct tm vKey = *localtime(&cur_Time);
		MYSQL_ROW row = NULL;

		for (int n = 0; (row = mysql_fetch_row(pMsg->Get()->pSQLResult)) != NULL; ++n)
		{
			int c = 0;
			int monthIndex;
			str_to_number(monthIndex, row[c++]);
			int isElite;
			str_to_number(isElite, row[c++]);

			int year = vKey.tm_year + 1900;
			int month = monthIndex;
			if (month >= 12)
			{
				year += 1;
				month = 0;
			}

			struct tm t;
			t.tm_year = year - 1900;
			t.tm_mon = month;
			t.tm_mday = 1;
			t.tm_isdst = 0;
			t.tm_hour = 0;
			t.tm_min = 0;
			t.tm_sec = 0;
			const time_t lastTime = mktime(&t);

			//sys_err("year %d month %d lasttime %d realTime %d", year, month, lastTime, lastTime-time(0));

			BattlePassData p;
			for (DWORD j = 0; j < BATTLE_MISSION_MAX; ++j)
			{
				char missionName[100];
				strlcpy(missionName, row[c++], sizeof(missionName));
				BYTE missionIndex = GetBattlePassMissionIndex(missionName);
				DWORD missionMaxValue, missionSubValue;
				str_to_number(missionMaxValue, row[c++]);
				str_to_number(missionSubValue, row[c++]);

				std::vector<std::pair<DWORD, DWORD>> m_vec;
				m_vec.clear();
				for (DWORD j = 0; j < BATTLE_SUB_REWARD; ++j)
				{
					DWORD itemVnum, itemCount;
					str_to_number(itemVnum, row[c++]);
					str_to_number(itemCount, row[c++]);
					if (itemVnum > 0)
						m_vec.emplace_back(itemVnum, itemCount);
				}
				if (missionIndex > 0)
				{
					auto it = p.missionData.find(missionIndex);
					if (it == p.missionData.end())
					{
						p.subReward.emplace(missionIndex, m_vec);
						p.missionIndex.emplace(missionIndex, j + 1);
						p.missionData.emplace(missionIndex, std::make_pair(missionMaxValue, missionSubValue));
					}
					else
						sys_err("battlepass: duplicate mission in same month! monthIndex %d missionIndex %d", monthIndex, missionIndex);
				}
			}
			for (DWORD j = 0; j < BATTLE_REWARD_MAX; ++j)
			{
				DWORD itemVnum, itemCount;
				str_to_number(itemVnum, row[c++]);
				str_to_number(itemCount, row[c++]);
				if (itemVnum != 0 && itemCount != 0)
					p.rewardData.emplace_back(itemVnum, itemCount);
			}

			m_BattlePassData[isElite ? BATTLE_PASS_TYPE_ELITE : BATTLE_PASS_TYPE_NORMAL].emplace(monthIndex - 1, std::make_pair(lastTime, p));
		}
	}
}
#endif