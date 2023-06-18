#include "stdafx.h"
#include "refine.h"


#include "char.h"
#include "item_manager.h"
#include "item.h"

#include "desc.h"

CRefineManager::CRefineManager()
{
}

CRefineManager::~CRefineManager()
{
}



#ifdef ENABLE_FEATURES_REFINE_SYSTEM
	int EXTRA_REFINE_POTIONS_GRADE[3] = 
	{
		REFINE_VNUM_POTION_LOW,
		REFINE_VNUM_POTION_MEDIUM,
		REFINE_VNUM_POTION_EXTRA,
	};

	int CRefineManager::Result(LPCHARACTER ch)
	{
		int uninitialized = 0;
		int flag = ch->GetQuestFlag(REFINE_INCREASE);

		if (flag > 0)
			return flag;
		else
			return uninitialized;
	}

	bool CRefineManager::GetPercentage(LPCHARACTER ch, BYTE lLow, BYTE lMedium, BYTE lExtra, BYTE lTotal, LPITEM item)
	{
		if (!item) {
			return false;
		}

		BYTE ar_ListType[3] = {lLow, lMedium, lExtra};

		for (int it = 0; it <= JOURNAL_MAX_NUM; it++)
		{
			if (ar_ListType[it] > 0)
			{
				//@fix 12.01.2017
				if (item->GetType() == ITEM_METIN)
				{
					return false;
				}
				if (ch->CountSpecifyItem(EXTRA_REFINE_POTIONS_GRADE[it]) < 1)
				{
#ifdef TEXTS_IMPROVEMENT
					ch->ChatPacketNew(CHAT_TYPE_INFO, 620, "%s", 
#ifdef ENABLE_MULTI_NAMES
					ITEM_MANAGER::instance().GetTable(EXTRA_REFINE_POTIONS_GRADE[it])->szLocaleName[ch->GetDesc()->GetLanguage()]
#else
					ITEM_MANAGER::instance().GetTable(EXTRA_REFINE_POTIONS_GRADE[it])->szLocaleName)
#endif
					);
#endif
					return false;
				}
			}
		}

		if (lTotal > 100 )
		{
#ifdef TEXTS_IMPROVEMENT
			ch->ChatPacketNew(CHAT_TYPE_INFO, 621, "");
#endif
			return false;
		}
		
		return true;
	}

	void CRefineManager::Reset(LPCHARACTER ch)
	{
		for (int it = 0; it <= JOURNAL_MAX_NUM; it++)
		{
			char buf[MAX_HOST_LENGTH + 1];
			snprintf(buf, sizeof(buf), "refine.mode_%d", it);
			
			if (ch->GetQuestFlag(buf) > 0)
			{
				ch->RemoveSpecifyItem(EXTRA_REFINE_POTIONS_GRADE[it], 1);
				ch->SetQuestFlag(REFINE_INCREASE, 0);
				ch->SetQuestFlag(buf, 0);
			}
		}
	}	

	void CRefineManager::Increase(LPCHARACTER ch, BYTE lLow, BYTE lMedium, BYTE lExtra)
	{
		int calcPercentage = 0;

		BYTE ar_ListType[3] = {lLow, lMedium, lExtra};
		int ar_ListPercentage[3] = {REFINE_PERCENTAGE_LOW, REFINE_PERCENTAGE_MEDIUM, REFINE_PERCENTAGE_EXTRA};
		
		for (int it = 0; it <= JOURNAL_MAX_NUM; it++)
		{
			if (ar_ListType[it] > 0)
			{
				char buf[MAX_HOST_LENGTH + 1];
				snprintf(buf, sizeof(buf), "refine.mode_%d", it);
				ch->SetQuestFlag(buf, 1);

				calcPercentage += ar_ListPercentage[it];		
			}
		}
		
		if (ch->GetQuestFlag(REFINE_INCREASE) < 1)
		{
			ch->SetQuestFlag(REFINE_INCREASE, calcPercentage);
		}
#ifdef TEXTS_IMPROVEMENT
		else {
			ch->ChatPacketNew(CHAT_TYPE_INFO, 622, "");
		}
#endif
	}
#endif


#ifdef REGEN_ANDRA
bool CRefineManager::Initialize(TRefineTable * table, int size)
{
	m_map_RefineRecipe.clear();

	for (int i = 0; i < size; ++i, ++table)
	{
		sys_log(0, "REFINE %d prob %d cost %d", table->id, table->prob, table->cost);
		m_map_RefineRecipe.insert(std::make_pair(table->id, *table));
	}

	sys_log(0, "REFINE: COUNT %d", m_map_RefineRecipe.size());
	return true;
}
#else
bool CRefineManager::Initialize(TRefineTable * table, int size)
{
	for (int i = 0; i < size; ++i, ++table)
	{
		sys_log(0, "REFINE %d prob %d cost %d", table->id, table->prob, table->cost);
		m_map_RefineRecipe.insert(std::make_pair(table->id, *table));
	}

	sys_log(0, "REFINE: COUNT %d", m_map_RefineRecipe.size());
	return true;
}
#endif

const TRefineTable* CRefineManager::GetRefineRecipe(DWORD vnum)
{
	if (vnum == 0)
		return NULL;

	itertype(m_map_RefineRecipe) it = m_map_RefineRecipe.find(vnum);
	sys_log(0, "REFINE: FIND %u %s", vnum, it == m_map_RefineRecipe.end() ? "FALSE" : "TRUE");

	if (it == m_map_RefineRecipe.end())
	{
		return NULL;
	}

	return &it->second;
}
