#include "stdafx.h"
#include "char.h"
#include "item.h"
#include "desc.h"
#include "DragonSoul.h"
#include "log.h"

// ��ȥ�� �ʱ�ȭ
// ��ȥ�� on/off�� Affect�� ����Ǳ� ������,
// ��ȥ�� Affect�� �ִٸ� ���� �ִ� ��ȥ���� activate�ؾ��Ѵ�.
// ���� ��ȥ�� ��� �ڰ��� QuestFlag�� ������ ���ұ� ������,
// ����Ʈ Flag���� ��ȥ�� ��� �ڰ��� �о�´�.

// ĳ������ affect, quest�� load �Ǳ� ���� DragonSoul_Initialize�� ȣ���ϸ� �ȵȴ�.
// affect�� ���� �������� �ε�Ǿ� LoadAffect���� ȣ����.
void CHARACTER::DragonSoul_Initialize()
{
	for (int i = DRAGON_SOUL_EQUIP_SLOT_START; i < DRAGON_SOUL_EQUIP_SLOT_END; i++)
	{
		LPITEM pItem = GetItem(TItemPos(INVENTORY, i));
		if (NULL != pItem)
			pItem->SetSocket(ITEM_SOCKET_DRAGON_SOUL_ACTIVE_IDX, 0);
	}

	if (FindAffect(AFFECT_DRAGON_SOUL_DECK_0))
	{
		DragonSoul_ActivateDeck(DRAGON_SOUL_DECK_0);
	}
	else if (FindAffect(AFFECT_DRAGON_SOUL_DECK_1))
	{
		DragonSoul_ActivateDeck(DRAGON_SOUL_DECK_1);
	}
}

int CHARACTER::DragonSoul_GetActiveDeck() const
{
	return m_pointsInstant.iDragonSoulActiveDeck;
}

bool CHARACTER::DragonSoul_IsDeckActivated() const
{
	return m_pointsInstant.iDragonSoulActiveDeck >= 0;
}

bool CHARACTER::DragonSoul_ActivateDeck(int deck_idx)
{
	if (deck_idx < DRAGON_SOUL_DECK_0 || deck_idx >= DRAGON_SOUL_DECK_MAX_NUM)
	{
		return false;
	}

	if (DragonSoul_GetActiveDeck() == deck_idx)
		return true;

	DragonSoul_DeactivateAll();

	AddAffect(AFFECT_DRAGON_SOUL_DECK_0 + deck_idx, APPLY_NONE, 0, 0, INFINITE_AFFECT_DURATION, 0, false);
	m_pointsInstant.iDragonSoulActiveDeck = deck_idx;
	
#ifdef ENABLE_DS_SET
	std::vector<int> gradeList, stepList, strengthList;
	bool bExpired = false;
	for (int i = 0; i < DS_SLOT_MAX; i++) {
		gradeList.push_back(0);
		stepList.push_back(0);
		strengthList.push_back(0);
	}
	
	int j = 0;
#endif
	
	for (int i = DRAGON_SOUL_EQUIP_SLOT_START + DS_SLOT_MAX * deck_idx; i < DRAGON_SOUL_EQUIP_SLOT_START + DS_SLOT_MAX * (deck_idx + 1); i++)
	{
		LPITEM pItem = GetInventoryItem(i);
		if (NULL != pItem) {
			DSManager::instance().ActivateDragonSoul(pItem);
#ifdef ENABLE_DS_SET
			if (!DSManager::instance().IsTimeLeftDragonSoul(pItem))
				bExpired = true;
			
			gradeList[j] = (pItem->GetVnum() / 1000) % 10;
			stepList[j] = (pItem->GetVnum() / 100) % 10;
			strengthList[j] = (pItem->GetVnum() / 10) % 10;
			j += 1;
#endif
		}
	}
	
#ifdef ENABLE_DS_SET
	if ((!bExpired) && (gradeList[0] == 
#ifdef ENABLE_DS_GRADE_MYTH
	DRAGON_SOUL_GRADE_MYTH
#else
	DRAGON_SOUL_GRADE_LEGENDARY
#endif
	) && (stepList[0] == DRAGON_SOUL_STEP_HIGHEST) && (strengthList[0] == 6)) {
		if ((std::equal(gradeList.begin() + 1, gradeList.end(), gradeList.begin())) && (std::equal(stepList.begin() + 1, stepList.end(), stepList.begin())) && (std::equal(strengthList.begin() + 1, strengthList.end(), strengthList.begin()))) {
			
			AddAffect(AFFECT_DS_SET, POINT_NONE, 1, 0, INFINITE_AFFECT_DURATION, 0, false);
			AddAffect(AFFECT_DS_BNS1, POINT_ATTBONUS_METIN, 10, 0, INFINITE_AFFECT_DURATION, 0, false);
			AddAffect(AFFECT_DS_BNS2, POINT_ATTBONUS_MONSTER, 10, 0, INFINITE_AFFECT_DURATION, 0, false);
			AddAffect(AFFECT_DS_BNS3, POINT_MAX_HP, 1000, 0, INFINITE_AFFECT_DURATION, 0, false);
		}
		else {
			AddAffect(AFFECT_DS_SET, POINT_NONE, 0, 0, INFINITE_AFFECT_DURATION, 0, false);
			RemoveAffect(AFFECT_DS_BNS1);
			RemoveAffect(AFFECT_DS_BNS2);
			RemoveAffect(AFFECT_DS_BNS3);
		}
	}
	else {
		AddAffect(AFFECT_DS_SET, POINT_NONE, 0, 0, INFINITE_AFFECT_DURATION, 0, false);
		RemoveAffect(AFFECT_DS_BNS1);
		RemoveAffect(AFFECT_DS_BNS2);
		RemoveAffect(AFFECT_DS_BNS3);
	}
#endif
	
	return true;
}

void CHARACTER::DragonSoul_DeactivateAll()
{
	for (int i = DRAGON_SOUL_EQUIP_SLOT_START; i < DRAGON_SOUL_EQUIP_SLOT_END; i++)
	{
		DSManager::instance().DeactivateDragonSoul(GetInventoryItem(i), true);
	}
	m_pointsInstant.iDragonSoulActiveDeck = -1;
	RemoveAffect(AFFECT_DRAGON_SOUL_DECK_0);
	RemoveAffect(AFFECT_DRAGON_SOUL_DECK_1);
	RemoveAffect(AFFECT_DS_SET);
	RemoveAffect(AFFECT_DS_BNS1);
	RemoveAffect(AFFECT_DS_BNS2);
	RemoveAffect(AFFECT_DS_BNS3);
}

void CHARACTER::DragonSoul_CleanUp()
{
	for (int i = DRAGON_SOUL_EQUIP_SLOT_START; i < DRAGON_SOUL_EQUIP_SLOT_END; i++)
	{
		DSManager::instance().DeactivateDragonSoul(GetInventoryItem(i), true);
	}
}

bool CHARACTER::DragonSoul_RefineWindow_Open(LPENTITY pEntity)
{
	if (NULL == m_pointsInstant.m_pDragonSoulRefineWindowOpener)
	{
		m_pointsInstant.m_pDragonSoulRefineWindowOpener = pEntity;
	}

	TPacketGCDragonSoulRefine PDS;
	PDS.header = HEADER_GC_DRAGON_SOUL_REFINE;
	PDS.bSubType = DS_SUB_HEADER_OPEN;
	LPDESC d = GetDesc();

	if (NULL == d)
	{
		sys_err ("User(%s)'s DESC is NULL POINT.", GetName());
		return false;
	}

	d->Packet(&PDS, sizeof(PDS));
	return true;
}

bool CHARACTER::DragonSoul_RefineWindow_Close()
{
	m_pointsInstant.m_pDragonSoulRefineWindowOpener = NULL;
	return true;
}

bool CHARACTER::DragonSoul_RefineWindow_CanRefine()
{
	return NULL != m_pointsInstant.m_pDragonSoulRefineWindowOpener;
}
