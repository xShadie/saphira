
#include "stdafx.h"

#include "config.h"
#include "char.h"
#include "char_manager.h"
#include "affect.h"
#include "packet.h"
#include "buffer_manager.h"
#include "desc_client.h"
#include "battle.h"
#include "guild.h"
#include "utils.h"
#include "locale_service.h"
#include "lua_incl.h"
#include "arena.h"
#include "horsename_manager.h"
#include "item.h"
#include "DragonSoul.h"
#include "../../common/CommonDefines.h"
#ifdef ENABLE_NEW_USE_POTION
#include "party.h"
#endif

#define IS_NO_SAVE_AFFECT(type) ((type) == AFFECT_WAR_FLAG || (type) == AFFECT_REVIVE_INVISIBLE || ((type) >= AFFECT_PREMIUM_START && (type) <= AFFECT_PREMIUM_END))
#define IS_NO_CLEAR_ON_DEATH_AFFECT(type) ((type) == AFFECT_PVM_RACE || (type) == AFFECT_VOTEFORBONUS || (type) == AFFECT_BLOCK_CHAT || ((type) >= 500 && (type) < 600) || ((type) >= 564 && (type) < 566) || ((type) >= NEW_AFFECT_BIOLOGIST_1 && (type) < NEW_AFFECT_BIOLOGIST_14))

void SendAffectRemovePacket(LPDESC d, DWORD pid, DWORD type, BYTE point)
{
	TPacketGCAffectRemove ptoc;
	ptoc.bHeader	= HEADER_GC_AFFECT_REMOVE;
	ptoc.dwType		= type;
	ptoc.bApplyOn	= point;
	d->Packet(&ptoc, sizeof(TPacketGCAffectRemove));

	TPacketGDRemoveAffect ptod;
	ptod.dwPID		= pid;
	ptod.dwType		= type;
	ptod.bApplyOn	= point;
	db_clientdesc->DBPacket(HEADER_GD_REMOVE_AFFECT, 0, &ptod, sizeof(ptod));
}

void SendAffectAddPacket(LPDESC d, CAffect * pkAff)
{
	TPacketGCAffectAdd ptoc;
	ptoc.bHeader		= HEADER_GC_AFFECT_ADD;
	ptoc.elem.dwType		= pkAff->dwType;
	ptoc.elem.bApplyOn		= pkAff->bApplyOn;
	ptoc.elem.lApplyValue	= pkAff->lApplyValue;
	ptoc.elem.dwFlag		= pkAff->dwFlag;
	ptoc.elem.lDuration		= pkAff->lDuration;
	ptoc.elem.lSPCost		= pkAff->lSPCost;
	d->Packet(&ptoc, sizeof(TPacketGCAffectAdd));
}
////////////////////////////////////////////////////////////////////
// Affect
CAffect * CHARACTER::FindAffect(DWORD dwType, BYTE bApply) const
{
	itertype(m_list_pkAffect) it = m_list_pkAffect.begin();

	while (it != m_list_pkAffect.end())
	{
		CAffect * pkAffect = *it++;

		if (pkAffect->dwType == dwType && (bApply == APPLY_NONE || bApply == pkAffect->bApplyOn))
			return pkAffect;
	}

	return NULL;
}

EVENTFUNC(affect_event)
{
	char_event_info* info = dynamic_cast<char_event_info*>( event->info );

	if ( info == NULL )
	{
		sys_err( "affect_event> <Factor> Null pointer" );
		return 0;
	}

	LPCHARACTER ch = info->ch;

	if (ch == NULL) { // <Factor>
		return 0;
	}

	if (!ch->UpdateAffect())
		return 0;
	else
		return passes_per_sec; // 1��
}

bool CHARACTER::UpdateAffect()
{
#ifdef ENABLE_BUG_FIXES
	if (!GetWear(WEAR_WEAPON)) {
		if (IsAffectFlag(AFF_GEOMGYEONG)) {
			RemoveAffect(SKILL_GEOMKYUNG);
		}

		if (IsAffectFlag(AFF_GWIGUM)) {
			RemoveAffect(SKILL_GWIGEOM);
		}
	}
#endif

	// affect_event ���� ó���� ���� �ƴ�����, 1��¥�� �̺�Ʈ���� ó���ϴ� ����
	// �̰� ���̶� ���⼭ ���� ó���� �Ѵ�.
	if (GetPoint(POINT_HP_RECOVERY) > 0)
	{
		if (GetMaxHP() <= GetHP())
		{
			PointChange(POINT_HP_RECOVERY, -GetPoint(POINT_HP_RECOVERY));
		}
		else
		{
			int iVal = MIN(GetPoint(POINT_HP_RECOVERY), GetMaxHP() * 7 / 100);

			PointChange(POINT_HP, iVal);
			PointChange(POINT_HP_RECOVERY, -iVal);
		}
	}

	if (GetPoint(POINT_SP_RECOVERY) > 0)
	{
		if (GetMaxSP() <= GetSP())
			PointChange(POINT_SP_RECOVERY, -GetPoint(POINT_SP_RECOVERY));
		else
		{
			int iVal = MIN(GetPoint(POINT_SP_RECOVERY), GetMaxSP() * 7 / 100);

			PointChange(POINT_SP, iVal);
			PointChange(POINT_SP_RECOVERY, -iVal);
		}
	}

	if (GetPoint(POINT_HP_RECOVER_CONTINUE) > 0)
	{
		PointChange(POINT_HP, GetPoint(POINT_HP_RECOVER_CONTINUE));
	}

	if (GetPoint(POINT_SP_RECOVER_CONTINUE) > 0)
	{
		PointChange(POINT_SP, GetPoint(POINT_SP_RECOVER_CONTINUE));
	}
	
	AutoRecoveryItemProcess(AFFECT_AUTO_HP_RECOVERY);
	AutoRecoveryItemProcess(AFFECT_AUTO_SP_RECOVERY);
#ifdef ENABLE_NEW_USE_POTION
	AutoRecoveryItemProcess(AFFECT_AUTO_HP_RECOVERY2);
	AutoRecoveryItemProcess(AFFECT_AUTO_SP_RECOVERY2);
#endif
#ifdef ENABLE_RECALL
	AutoRecallProcess();
#endif
	
	// ���׹̳� ȸ��
	if (GetMaxStamina() > GetStamina())
	{
		int iSec = (get_dword_time() - GetStopTime()) / 3000;
		if (iSec)
			PointChange(POINT_STAMINA, GetMaxStamina()/1);
	}


	// ProcessAffect�� affect�� ������ true�� �����Ѵ�.
	if (ProcessAffect())
		if (GetPoint(POINT_HP_RECOVERY) == 0 && GetPoint(POINT_SP_RECOVERY) == 0 && GetStamina() == GetMaxStamina())
		{
			m_pkAffectEvent = NULL;
			return false;
		}

	return true;
}

void CHARACTER::StartAffectEvent()
{
	if (m_pkAffectEvent)
		return;

	char_event_info* info = AllocEventInfo<char_event_info>();
	info->ch = this;
	m_pkAffectEvent = event_create(affect_event, info, passes_per_sec);
	sys_log(1, "StartAffectEvent %s %p %p", GetName(), this, get_pointer(m_pkAffectEvent));
}

#ifdef ENABLE_SKILLS_BUFF_ALTERNATIVE
void CHARACTER::ClearAffectSkills() {
	size_t j = m_list_pkAffectSkills.size();
	if (j < 1)
		return;
	
	m_list_pkAffectSkills.erase(m_list_pkAffectSkills.begin(), m_list_pkAffectSkills.end());
	m_list_pkAffectSkills.shrink_to_fit();
}

void CHARACTER::SaveAffectSkills(DWORD dwType, BYTE bApplyOn, long lApplyValue, DWORD dwFlag, long lDuration, long lSPCost) {
	TAffectSkills t;
	t.dwType = dwType;
	t.bApplyOn = bApplyOn;
	t.lApplyValue = lApplyValue;
	t.dwFlag = dwFlag;
	t.lDuration = lDuration;
	t.lSPCost = lSPCost;
	t.dwTime = get_global_time();
	
	m_list_pkAffectSkills.push_back(t);
}

void CHARACTER::LoadAffectSkills() {
	size_t j = m_list_pkAffectSkills.size();
	if (j < 1)
		return;
	
	long lDuration = 0;
	for (size_t i = 0; i < j; ++i) {
		lDuration = m_list_pkAffectSkills[i].lDuration - (get_global_time() - m_list_pkAffectSkills[i].dwTime);
		if (lDuration > 0)
			AddAffect(m_list_pkAffectSkills[i].dwType, m_list_pkAffectSkills[i].bApplyOn, m_list_pkAffectSkills[i].lApplyValue, m_list_pkAffectSkills[i].dwFlag, lDuration, m_list_pkAffectSkills[i].lSPCost, false);
	}
	
	ClearAffectSkills();
}
#endif

void CHARACTER::ClearAffect(bool bSave)
{
	TAffectFlag afOld = m_afAffectFlag;
	WORD	wMovSpd = GetPoint(POINT_MOV_SPEED);
	WORD	wAttSpd = GetPoint(POINT_ATT_SPEED);

	itertype(m_list_pkAffect) it = m_list_pkAffect.begin();

	while (it != m_list_pkAffect.end())
	{
		CAffect * pkAff = *it;

		if (bSave)
		{
#ifdef ENABLE_SOUL_SYSTEM
			if ( pkAff->dwType == AFFECT_SOUL_RED || pkAff->dwType == AFFECT_SOUL_BLUE )
			{
				++it;
				continue;
			}
#endif
			
			if ( IS_NO_CLEAR_ON_DEATH_AFFECT(pkAff->dwType) || IS_NO_SAVE_AFFECT(pkAff->dwType) )
			{
				++it;
				continue;
			}
#ifdef ENABLE_SKILLS_BUFF_ALTERNATIVE
			else if ((IsPC()) && (
				(pkAff->dwType == SKILL_JEONGWI) ||	// 3
				(pkAff->dwType == SKILL_GEOMKYUNG) ||	// 4
				(pkAff->dwType == SKILL_CHUNKEON) ||		// 19
				(pkAff->dwType == SKILL_GYEONGGONG) ||	// 49
				(pkAff->dwType == SKILL_GWIGEOM) ||		// 63
				(pkAff->dwType == SKILL_TERROR) ||		// 64
				(pkAff->dwType == SKILL_JUMAGAP) ||		// 65
				(pkAff->dwType == SKILL_MUYEONG) ||		// 78
				(pkAff->dwType == SKILL_MANASHILED) ||	// 79
				(pkAff->dwType == SKILL_HOSIN) ||			// 94
				(pkAff->dwType == SKILL_REFLECT) ||			// 95
				(pkAff->dwType == SKILL_GICHEON) ||		// 96
				(pkAff->dwType == SKILL_KWAESOK) ||		// 110
				(pkAff->dwType == SKILL_JEUNGRYEOK)		// 111
			))
			{
				SaveAffectSkills(pkAff->dwType, pkAff->bApplyOn, pkAff->lApplyValue, pkAff->dwFlag, pkAff->lDuration, pkAff->lSPCost);
				//++it;
				//continue;
			}
#endif
#ifdef ENABLE_BLOCK_MULTIFARM
			else if ((pkAff->dwType == AFFECT_DROP_BLOCK) || (pkAff->dwType == AFFECT_DROP_UNBLOCK)) {
				++it;
				continue;
			}
#endif
			
			if (IsPC())
			{
				SendAffectRemovePacket(GetDesc(), GetPlayerID(), pkAff->dwType, pkAff->bApplyOn);
			}
		}

		ComputeAffect(pkAff, false);

		it = m_list_pkAffect.erase(it);
		CAffect::Release(pkAff);
	}

	if (afOld != m_afAffectFlag ||
			wMovSpd != GetPoint(POINT_MOV_SPEED) ||
			wAttSpd != GetPoint(POINT_ATT_SPEED))
		UpdatePacket();

	CheckMaximumPoints();

	if (m_list_pkAffect.empty())
		event_cancel(&m_pkAffectEvent);
}

int CHARACTER::ProcessAffect()
{
	bool	bDiff	= false;
	CAffect	*pkAff	= NULL;

	//
	// �����̾� ó��
	//
	for (int i = 0; i <= PREMIUM_MAX_NUM; ++i)
	{
		int aff_idx = i + AFFECT_PREMIUM_START;

		pkAff = FindAffect(aff_idx);

		if (!pkAff)
			continue;

		int remain = GetPremiumRemainSeconds(i);

		if (remain < 0)
		{
			RemoveAffect(aff_idx);
			bDiff = true;
		}
		else
			pkAff->lDuration = remain + 1;
	}

#ifdef ENABLE_VOTE_FOR_BONUS
	pkAff = FindAffect(AFFECT_VOTEFORBONUS);
	if (pkAff)
	{
		int32_t remain = pkAff->lDuration - get_global_time();
		if (remain <= 0)
		{
			RemoveAffect(AFFECT_VOTEFORBONUS);
			bDiff = true;
		}
	}
#endif

#ifdef ENABLE_BATTLE_PASS
	pkAff = FindAffect(AFFECT_BATTLE_PASS);
	if (pkAff)
	{
		int remain = GetBattlePassEndTime();
		
		if (remain < 0)
		{
			RemoveAffect(AFFECT_BATTLE_PASS);
			m_dwBattlePassEndTime = 0;
			bDiff = true;
		}
		else
			pkAff->lDuration = remain + 1;
	}
#endif

	////////// HAIR_AFFECT
	pkAff = FindAffect(AFFECT_HAIR);
	if (pkAff)
	{
		// IF HAIR_LIMIT_TIME() < CURRENT_TIME()
		if ( this->GetQuestFlag("hair.limit_time") < get_global_time())
		{
			// SET HAIR NORMAL
			this->SetPart(PART_HAIR, 0);
			// REMOVE HAIR AFFECT
			RemoveAffect(AFFECT_HAIR);
		}
		else
		{
			// INCREASE AFFECT DURATION
			++(pkAff->lDuration);
		}
	}
	////////// HAIR_AFFECT
	//

	CHorseNameManager::instance().Validate(this);

	TAffectFlag afOld = m_afAffectFlag;
	long lMovSpd = GetPoint(POINT_MOV_SPEED);
	long lAttSpd = GetPoint(POINT_ATT_SPEED);

	itertype(m_list_pkAffect) it;

	it = m_list_pkAffect.begin();

	while (it != m_list_pkAffect.end())
	{
		pkAff = *it;

		bool bEnd = false;

		if (pkAff->dwType >= GUILD_SKILL_START && pkAff->dwType <= GUILD_SKILL_END)
		{
			if (!GetGuild() || !GetGuild()->UnderAnyWar())
				bEnd = true;
		}

#ifdef ENABLE_SOUL_SYSTEM
		if (pkAff->lSPCost > 0 && pkAff->dwType != AFFECT_SOUL_RED && pkAff->dwType != AFFECT_SOUL_BLUE)
#else
		if (pkAff->lSPCost > 0)
#endif
		{
			if (GetSP() < pkAff->lSPCost)
				bEnd = true;
			else
				PointChange(POINT_SP, -pkAff->lSPCost);
		}
		
		// AFFECT_DURATION_BUG_FIX
		// ���� ȿ�� �����۵� �ð��� ���δ�.
		// �ð��� �ſ� ũ�� ��� ������ ��� ���� ���̶� ������.
		if ( --pkAff->lDuration <= 0 )
		{
			bEnd = true;
		}
		// END_AFFECT_DURATION_BUG_FIX

		if (bEnd)
		{
			it = m_list_pkAffect.erase(it);
			ComputeAffect(pkAff, false);
			bDiff = true;
			if (IsPC())
			{
				SendAffectRemovePacket(GetDesc(), GetPlayerID(), pkAff->dwType, pkAff->bApplyOn);
			}

			CAffect::Release(pkAff);

			continue;
		}

		++it;
	}

	if (bDiff)
	{
		if (afOld != m_afAffectFlag ||
				lMovSpd != GetPoint(POINT_MOV_SPEED) ||
				lAttSpd != GetPoint(POINT_ATT_SPEED))
		{
			UpdatePacket();
		}

		CheckMaximumPoints();
	}

	if (m_list_pkAffect.empty())
		return true;

	return false;
}

void CHARACTER::SaveAffect()
{
	TPacketGDAddAffect p;

	itertype(m_list_pkAffect) it = m_list_pkAffect.begin();

	while (it != m_list_pkAffect.end())
	{
		CAffect * pkAff = *it++;
		if (IS_NO_SAVE_AFFECT(pkAff->dwType))
			continue;

		sys_log(1, "AFFECT_SAVE: %u %u %d %d", pkAff->dwType, pkAff->bApplyOn, pkAff->lApplyValue, pkAff->lDuration);

		p.dwPID			= GetPlayerID();
		p.elem.dwType		= pkAff->dwType;
		p.elem.bApplyOn		= pkAff->bApplyOn;
		p.elem.lApplyValue	= pkAff->lApplyValue;
		p.elem.dwFlag		= pkAff->dwFlag;
		p.elem.lDuration	= pkAff->lDuration;
		p.elem.lSPCost		= pkAff->lSPCost;
		db_clientdesc->DBPacket(HEADER_GD_ADD_AFFECT, 0, &p, sizeof(p));
	}
}

EVENTINFO(load_affect_login_event_info)
{
	DWORD pid;
	DWORD count;
	char* data;

	load_affect_login_event_info()
	: pid( 0 )
	, count( 0 )
	, data( 0 )
	{
	}
};

EVENTFUNC(load_affect_login_event)
{
	load_affect_login_event_info* info = dynamic_cast<load_affect_login_event_info*>( event->info );

	if ( info == NULL )
	{
		sys_err( "load_affect_login_event_info> <Factor> Null pointer" );
		return 0;
	}

	DWORD dwPID = info->pid;
	LPCHARACTER ch = CHARACTER_MANAGER::instance().FindByPID(dwPID);

	if (!ch)
	{
		M2_DELETE_ARRAY(info->data);
		return 0;
	}

	LPDESC d = ch->GetDesc();

	if (!d)
	{
		M2_DELETE_ARRAY(info->data);
		return 0;
	}

	if (d->IsPhase(PHASE_HANDSHAKE) ||
			d->IsPhase(PHASE_LOGIN) ||
			d->IsPhase(PHASE_SELECT) ||
			d->IsPhase(PHASE_DEAD) ||
			d->IsPhase(PHASE_LOADING))
	{
		return PASSES_PER_SEC(1);
	}
	else if (d->IsPhase(PHASE_CLOSE))
	{
		M2_DELETE_ARRAY(info->data);
		return 0;
	}
	else if (d->IsPhase(PHASE_GAME))
	{
		sys_log(1, "Affect Load by Event");
		ch->LoadAffect(info->count, (TPacketAffectElement*)info->data);
		M2_DELETE_ARRAY(info->data);
		return 0;
	}
	else
	{
		sys_err("input_db.cpp:quest_login_event INVALID PHASE pid %d", ch->GetPlayerID());
		M2_DELETE_ARRAY(info->data);
		return 0;
	}
}

#ifdef ENABLE_BIOLOGIST_UI
void CHARACTER::CheckBiologistReward() {
	int stat = GetQuestFlag("biologist.stat");
	if (stat > 0) {
		for (int i = 0; i < stat; i++) {
			if (FindAffect(biologistMissionInfo[i][14])) {
				continue;
			}

			if (biologistMissionInfo[i][11] == 0) {
				int j = 0;
				for (int w = 0; w < 4; w++) {
					j += 2;
					BYTE bApplyOn = biologistMissionInfo[i][j + 1];
					long lApplyValue = biologistMissionInfo[i][j + 2];
					if (bApplyOn == APPLY_NONE || lApplyValue == 0) {
						continue;
					} else {
						bApplyOn = aApplyInfo[bApplyOn].bPointType;
						AddAffect(biologistMissionInfo[i][14], bApplyOn, lApplyValue, 0, 315360000, 0, false);
					}
				}
			} else {
				BYTE bApplyOn = biologistMissionInfo[i][7];
				long lApplyValue = biologistMissionInfo[i][8];
				if (bApplyOn != APPLY_NONE || lApplyValue != 0) {
					bApplyOn = aApplyInfo[bApplyOn].bPointType;
					AddAffect(biologistMissionInfo[i][14], bApplyOn, lApplyValue, 0, 315360000, 0, false);
				}
			}
		}
	}
}
#endif

void CHARACTER::LoadAffect(DWORD dwCount, TPacketAffectElement * pElements)
{
	m_bIsLoadedAffect = false;

	if (!GetDesc()->IsPhase(PHASE_GAME))
	{
		if (test_server)
			sys_log(0, "LOAD_AFFECT: Creating Event", GetName(), dwCount);

		load_affect_login_event_info* info = AllocEventInfo<load_affect_login_event_info>();

		info->pid = GetPlayerID();
		info->count = dwCount;
		info->data = M2_NEW char[sizeof(TPacketAffectElement) * dwCount];
		thecore_memcpy(info->data, pElements, sizeof(TPacketAffectElement) * dwCount);

		event_create(load_affect_login_event, info, PASSES_PER_SEC(1));

		return;
	}

	ClearAffect(true);

	if (test_server)
		sys_log(0, "LOAD_AFFECT: %s count %d", GetName(), dwCount);

	TAffectFlag afOld = m_afAffectFlag;

	long lMovSpd = GetPoint(POINT_MOV_SPEED);
	long lAttSpd = GetPoint(POINT_ATT_SPEED);

	for (DWORD i = 0; i < dwCount; ++i, ++pElements)
	{
		////// �������� �ε������ʴ´�.
		////if (pElements->dwType == SKILL_MUYEONG)
		////	continue;
		if (AFFECT_AUTO_HP_RECOVERY == pElements->dwType || AFFECT_AUTO_SP_RECOVERY == pElements->dwType)
		{
			LPITEM item = FindItemByID( pElements->dwFlag );
			if (NULL == item)
				continue;
			
			item->Lock(true);
		}
#ifdef ENABLE_NEW_USE_POTION
		else if (AFFECT_AUTO_HP_RECOVERY2 == pElements->dwType || AFFECT_AUTO_SP_RECOVERY2 == pElements->dwType)
		{
			LPITEM item = FindItemByID( pElements->dwFlag );
			if (NULL == item)
				continue;
			
			item->Lock(true);
		}
		else if ((pElements->dwType >= AFFECT_NEW_POTION1) && (pElements->dwType <= AFFECT_NEW_POTION31))
		{
			LPITEM item = FindItemByID(pElements->dwFlag);
			if (item)
				item->Lock(true);
			else
				continue;
		}
		//else if (pElements->dwType == AFFECT_NEW_POTION31)
		//{
		//	LPPARTY party = GetParty();
		//	if ((!party) || (party && GetPlayerID() != party->GetLeaderPID())) {
		//		LPITEM item = FindItemByID(pElements->dwFlag);
		//		if (item) {
		//			item->Lock(false);
		//			item->SetSocket(1, 0);
		//			RemoveAffect(AFFECT_NEW_POTION31);
		//		} else {
		//			continue;
		//		}
		//	}
		//}
#endif
#ifdef ENABLE_RECALL
#ifdef __PET_SYSTEM__
		else if (pElements->dwType == AFFECT_RECALL1)
		{
			LPITEM item = FindItemByID(pElements->dwFlag);
			if (item)
				item->Lock(true);
			else
				continue;
		}
#endif
#ifdef __NEWPET_SYSTEM__
		else if (pElements->dwType == AFFECT_RECALL2)
		{
			LPITEM item = FindItemByID(pElements->dwFlag);
			if (item)
				item->Lock(true);
			else
				continue;
		}
#endif
#endif
		
#ifdef ENABLE_SOUL_SYSTEM
		if(pElements->dwType == AFFECT_SOUL_RED || pElements->dwType == AFFECT_SOUL_BLUE)
		{
			LPITEM item = FindItemByID( pElements->lSPCost );

			if (!item)
				continue;

			item->Lock(true);
		}
#endif

		if (pElements->bApplyOn >= POINT_MAX_NUM)
		{
			sys_err("invalid affect data %s ApplyOn %u ApplyValue %d",
					GetName(), pElements->bApplyOn, pElements->lApplyValue);
			continue;
		}

		if (test_server)
		{
			sys_log(0, "Load Affect : Affect %s %d %d", GetName(), pElements->dwType, pElements->bApplyOn );
		}

		CAffect* pkAff = CAffect::Acquire();
		m_list_pkAffect.push_back(pkAff);

		pkAff->dwType		= pElements->dwType;
		pkAff->bApplyOn		= pElements->bApplyOn;
		pkAff->lApplyValue	= pElements->lApplyValue;
		pkAff->dwFlag		= pElements->dwFlag;
		pkAff->lDuration	= pElements->lDuration;
		pkAff->lSPCost		= pElements->lSPCost;

		SendAffectAddPacket(GetDesc(), pkAff);

		ComputeAffect(pkAff, true);


	}

	if ( CArenaManager::instance().IsArenaMap(GetMapIndex()) == true )
	{
		RemoveGoodAffect();
	}

#ifndef ENABLE_01092021
	RemoveAffect(AFFECT_MOUNT);
#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
	RemoveAffect(AFFECT_MOUNT_BONUS);
	if (GetMapIndex() != 113 && CArenaManager::instance().IsArenaMap(GetMapIndex()) == false) {
		CheckMount();
	}
#endif
#endif

	if (afOld != m_afAffectFlag || lMovSpd != GetPoint(POINT_MOV_SPEED) || lAttSpd != GetPoint(POINT_ATT_SPEED))
	{
		UpdatePacket();
	}

	StartAffectEvent();

	m_bIsLoadedAffect = true;

	// ��ȥ�� ���� �ε� �� �ʱ�ȭ
	DragonSoul_Initialize();

	// @fixme118 (regain affect hp/mp)
	if (!IsDead())
	{
		PointChange(POINT_HP, GetMaxHP() - GetHP());
		PointChange(POINT_SP, GetMaxSP() - GetSP());
	}

#ifdef ENABLE_BLOCK_MULTIFARM
	SetDropStatus();
#endif
#ifdef ENABLE_BIOLOGIST_UI
	CheckBiologistReward();
#endif
}

bool CHARACTER::AddAffect(DWORD dwType, BYTE bApplyOn, long lApplyValue, DWORD dwFlag, long lDuration, long lSPCost, bool bOverride, bool IsCube )
{
#ifdef ENABLE_BUG_FIXES
	if (dwType == AFFECT_POLYMORPH) {
		if (IsAffectFlag(AFF_GEOMGYEONG)) {
			RemoveAffect(SKILL_GEOMKYUNG);
		}

		if (IsAffectFlag(AFF_GWIGUM)) {
			RemoveAffect(SKILL_GWIGEOM);
		}
	}
#endif

	// CHAT_BLOCK
	if (dwType == AFFECT_BLOCK_CHAT && lDuration > 1)
	{
#ifdef TEXTS_IMPROVEMENT
		ChatPacketNew(CHAT_TYPE_INFO, 414, "%d", (lDuration / 60));
#endif
	}
	// END_OF_CHAT_BLOCK

	if (lDuration == 0)
	{
		sys_err("Character::AddAffect lDuration == 0 type %d", lDuration, dwType);
		lDuration = 1;
	}

	CAffect * pkAff = NULL;

	if (IsCube)
		pkAff = FindAffect(dwType,bApplyOn);
	else
		pkAff = FindAffect(dwType);

	if (dwFlag == AFF_STUN)
	{
		if (m_posDest.x != GetX() || m_posDest.y != GetY())
		{
			m_posDest.x = m_posStart.x = GetX();
			m_posDest.y = m_posStart.y = GetY();
			battle_end(this);

			SyncPacket();
		}
	}

	// �̹� �ִ� ȿ���� ���� ���� ó��
	if (pkAff && bOverride)
	{
		ComputeAffect(pkAff, false); // �ϴ� ȿ���� �����ϰ�

		if (GetDesc()) {
			SendAffectRemovePacket(GetDesc(), GetPlayerID(), pkAff->dwType, pkAff->bApplyOn);
		}
	}
	else
	{
		//
		// �� ���带 �߰�
		//
		// NOTE: ���� ���� type ���ε� ���� ����Ʈ�� ���� �� �ִ�.
		//
		pkAff = CAffect::Acquire();
		m_list_pkAffect.push_back(pkAff);

	}

	sys_log(1, "AddAffect %s type %d apply %d %d flag %u duration %d", GetName(), dwType, bApplyOn, lApplyValue, dwFlag, lDuration);
	sys_log(0, "AddAffect %s type %d apply %d %d flag %u duration %d", GetName(), dwType, bApplyOn, lApplyValue, dwFlag, lDuration);

	pkAff->dwType	= dwType;
	pkAff->bApplyOn	= bApplyOn;
	pkAff->lApplyValue	= lApplyValue;
	pkAff->dwFlag	= dwFlag;
	pkAff->lDuration	= lDuration;
	pkAff->lSPCost	= lSPCost;

	WORD wMovSpd = GetPoint(POINT_MOV_SPEED);
	WORD wAttSpd = GetPoint(POINT_ATT_SPEED);

	ComputeAffect(pkAff, true);

	if (pkAff->dwFlag || wMovSpd != GetPoint(POINT_MOV_SPEED) || wAttSpd != GetPoint(POINT_ATT_SPEED))
		UpdatePacket();

	StartAffectEvent();

	if (IsPC())
	{
		SendAffectAddPacket(GetDesc(), pkAff);

		if (IS_NO_SAVE_AFFECT(pkAff->dwType))
			return true;

		TPacketGDAddAffect p;
		p.dwPID			= GetPlayerID();
		p.elem.dwType		= pkAff->dwType;
		p.elem.bApplyOn		= pkAff->bApplyOn;
		p.elem.lApplyValue	= pkAff->lApplyValue;
		p.elem.dwFlag		= pkAff->dwFlag;
		p.elem.lDuration	= pkAff->lDuration;
		p.elem.lSPCost		= pkAff->lSPCost;
		db_clientdesc->DBPacket(HEADER_GD_ADD_AFFECT, 0, &p, sizeof(p));
	}

	return true;
}

void CHARACTER::RefreshAffect()
{
	itertype(m_list_pkAffect) it = m_list_pkAffect.begin();

	while (it != m_list_pkAffect.end())
	{
		CAffect * pkAff = *it++;
		ComputeAffect(pkAff, true);
	}
}

void CHARACTER::ComputeAffect(CAffect * pkAff, bool bAdd)
{
	if (bAdd && pkAff->dwType >= GUILD_SKILL_START && pkAff->dwType <= GUILD_SKILL_END)
	{
		if (!GetGuild())
			return;

		if (!GetGuild()->UnderAnyWar())
			return;
	}

	if (pkAff->dwFlag)
	{
		if (!bAdd)
			m_afAffectFlag.Reset(pkAff->dwFlag);
		else
			m_afAffectFlag.Set(pkAff->dwFlag);
	}

	if (bAdd)
		PointChange(pkAff->bApplyOn, pkAff->lApplyValue);
	else
		PointChange(pkAff->bApplyOn, -pkAff->lApplyValue);

	if (pkAff->dwType == SKILL_MUYEONG)
	{
		if (bAdd)
			StartMuyeongEvent();
		else
			StopMuyeongEvent();
	}

#ifdef ENABLE_NEW_GYEONGGONG_SKILL
	if (pkAff->dwType == SKILL_GYEONGGONG)
	{
		if (bAdd)
			StartGyeongGongEvent();
		else
			StopGyeongGongEvent();
	}
#endif
}

bool CHARACTER::RemoveAffect(CAffect * pkAff)
{
	if (!pkAff)
		return false;

	// AFFECT_BUF_FIX
	m_list_pkAffect.remove(pkAff);
	// END_OF_AFFECT_BUF_FIX

	ComputeAffect(pkAff, false);

	// ��� ���� ����.
	// ��� ���״� ���� ��ų ����->�а�->��� ���(AFFECT_REVIVE_INVISIBLE) �� �ٷ� ���� �� ��쿡 �߻��Ѵ�.
	// ������ �а��� �����ϴ� ������, ���� ��ų ȿ���� �����ϰ� �а� ȿ���� ����ǰ� �Ǿ��ִµ�,
	// ��� ��� �� �ٷ� �����ϸ� RemoveAffect�� �Ҹ��� �ǰ�, ComputePoints�ϸ鼭 �а� ȿ�� + ���� ��ų ȿ���� �ȴ�.
	// ComputePoints���� �а� ���¸� ���� ��ų ȿ�� �� �������� �ϸ� �Ǳ� �ϴµ�,
	// ComputePoints�� �������ϰ� ���ǰ� �־ ū ��ȭ�� �ִ� ���� ��������.(� side effect�� �߻����� �˱� �����.)
	// ���� AFFECT_REVIVE_INVISIBLE�� RemoveAffect�� �����Ǵ� ��츸 �����Ѵ�.
	// �ð��� �� �Ǿ� ��� ȿ���� Ǯ���� ���� ���װ� �߻����� �����Ƿ� �׿� �Ȱ��� ��.
	//		(ProcessAffect�� ���� �ð��� �� �Ǿ Affect�� �����Ǵ� ���, ComputePoints�� �θ��� �ʴ´�.)
	if (AFFECT_REVIVE_INVISIBLE != pkAff->dwType
#ifdef ENABLE_BUG_FIXES
	&& AFFECT_MOUNT != pkAff->dwType
#endif
	) {
		ComputePoints();
	} else {
		UpdatePacket();
	}

	CheckMaximumPoints();

	if (test_server)
		sys_log(0, "AFFECT_REMOVE: %s (flag %u apply: %u)", GetName(), pkAff->dwFlag, pkAff->bApplyOn);

	if (IsPC())
	{
		SendAffectRemovePacket(GetDesc(), GetPlayerID(), pkAff->dwType, pkAff->bApplyOn);
	}

	CAffect::Release(pkAff);
	return true;
}

bool CHARACTER::RemoveAffect(DWORD dwType)
{
#ifdef TEXTS_IMPROVEMENT
	if (dwType == AFFECT_BLOCK_CHAT) {
		ChatPacketNew(CHAT_TYPE_INFO, 474, "");
	}
#endif

	bool flag = false;

	CAffect * pkAff;

	while ((pkAff = FindAffect(dwType)))
	{
		RemoveAffect(pkAff);
		flag = true;
	}

	return flag;
}

bool CHARACTER::IsAffectFlag(DWORD dwAff) const
{
	return m_afAffectFlag.IsSet(dwAff);
}

void CHARACTER::RemoveGoodAffect()
{
	RemoveAffect(AFFECT_MOV_SPEED);
	RemoveAffect(AFFECT_ATT_SPEED);
	RemoveAffect(AFFECT_STR);
	RemoveAffect(AFFECT_DEX);
	RemoveAffect(AFFECT_INT);
	RemoveAffect(AFFECT_CON);
	RemoveAffect(AFFECT_CHINA_FIREWORK);
	RemoveAffect(SKILL_JEONGWI);
	RemoveAffect(SKILL_GEOMKYUNG);
	RemoveAffect(SKILL_GYEONGGONG);
	RemoveAffect(SKILL_GWIGEOM);
	RemoveAffect(SKILL_TERROR);
	RemoveAffect(SKILL_JUMAGAP);
	RemoveAffect(SKILL_MANASHILED);
	RemoveAffect(SKILL_HOSIN);
	RemoveAffect(SKILL_REFLECT);
	RemoveAffect(SKILL_GICHEON);
	RemoveAffect(SKILL_KWAESOK);
	RemoveAffect(SKILL_JEUNGRYEOK);
	RemoveAffect(SKILL_CHUNKEON);
	RemoveAffect(SKILL_EUNHYUNG);
#ifdef ENABLE_WOLFMAN_CHARACTER
	// ������(WOLFMEN) ���� �߰�
	RemoveAffect(SKILL_JEOKRANG);
	RemoveAffect(SKILL_CHEONGRANG);
#endif
}

bool CHARACTER::IsGoodAffect(BYTE bAffectType) const
{
	switch (bAffectType)
	{
		case (AFFECT_MOV_SPEED):
		case (AFFECT_ATT_SPEED):
		case (AFFECT_STR):
		case (AFFECT_DEX):
		case (AFFECT_INT):
		case (AFFECT_CON):
		case (AFFECT_CHINA_FIREWORK):

		case (SKILL_JEONGWI):
		case (SKILL_GEOMKYUNG):
		case (SKILL_CHUNKEON):
		case (SKILL_EUNHYUNG):
		case (SKILL_GYEONGGONG):
		case (SKILL_GWIGEOM):
		case (SKILL_TERROR):
		case (SKILL_JUMAGAP):
		case (SKILL_MANASHILED):
		case (SKILL_HOSIN):
		case (SKILL_REFLECT):
		case (SKILL_KWAESOK):
		case (SKILL_JEUNGRYEOK):
		case (SKILL_GICHEON):
#ifdef ENABLE_WOLFMAN_CHARACTER
		// ������(WOLFMEN) ���� �߰�
		case (SKILL_JEOKRANG):
		case (SKILL_CHEONGRANG):
#endif
			return true;
	}
	return false;
}

void CHARACTER::RemoveBadAffect()
{
	sys_log(0, "RemoveBadAffect %s", GetName());
	// ��
	RemovePoison();
#ifdef ENABLE_WOLFMAN_CHARACTER
	RemoveBleeding();
#endif
	RemoveFire();

	// ����           : Value%�� ������ 5�ʰ� �Ӹ� ���� ���� ���ư���. (������ 1/2 Ȯ���� Ǯ��)               AFF_STUN
	RemoveAffect(AFFECT_STUN);

	// ���ο�         : Value%�� ������ ����/�̼� ��� ��������. ���õ��� ���� �޶��� ����� ��� �� ��쿡   AFF_SLOW
	RemoveAffect(AFFECT_SLOW);

	// ���Ӹ���
	RemoveAffect(SKILL_TUSOK);

	// ����
	//RemoveAffect(SKILL_CURSE);

	// �Ĺ���
	//RemoveAffect(SKILL_PABUP);

	// ����           : Value%�� ������ ������Ų��. 2��                                                       AFF_FAINT
	//RemoveAffect(AFFECT_FAINT);

	// �ٸ�����       : Value%�� ������ �̵��ӵ��� ����Ʈ����. 5�ʰ� -40                                      AFF_WEB
	//RemoveAffect(AFFECT_WEB);

	// ����         : Value%�� ������ 10�ʰ� ������. (������ Ǯ��)                                        AFF_SLEEP
	//RemoveAffect(AFFECT_SLEEP);

	// ����           : Value%�� ������ ����/��� ��� ����Ʈ����. ���õ��� ���� �޶��� ����� ��� �� ��쿡 AFF_CURSE
	//RemoveAffect(AFFECT_CURSE);

	// ����           : Value%�� ������ 4�ʰ� �����Ų��.                                                     AFF_PARA
	//RemoveAffect(AFFECT_PARALYZE);

	// �ε��ں�       : ���� ���
	//RemoveAffect(SKILL_BUDONG);
}

