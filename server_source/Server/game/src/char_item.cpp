#include "stdafx.h"

#include <stack>

#include "utils.h"
#include "config.h"
#include "char.h"
#include "char_manager.h"
#include "item_manager.h"
#include "desc.h"
#include "desc_client.h"
#include "desc_manager.h"
#include "packet.h"
#include "affect.h"
#include "skill.h"
#include "start_position.h"
#include "mob_manager.h"
#include "db.h"
#include "log.h"
#include "vector.h"
#include "buffer_manager.h"
#include "questmanager.h"
#include "fishing.h"
#include "party.h"
#include "dungeon.h"
#include "refine.h"
#include "unique_item.h"
#include "war_map.h"
#include "marriage.h"
#include "polymorph.h"
#include "blend_item.h"
#include "BattleArena.h"
#include "arena.h"
#include "dev_log.h"
#include "pcbang.h"

#include "safebox.h"
#include "shop.h"

#ifdef __NEWPET_SYSTEM__
#include "New_PetSystem.h"
#define __NEWPET_SYSTEM_CHECK
#endif
#ifdef __PET_SYSTEM__
#include "PetSystem.h"
#endif
#ifdef ENABLE_NEWSTUFF
#include "pvp.h"
#endif
#ifdef ENABLE_SWITCHBOT
#include "new_switchbot.h"
#endif

#ifdef ENABLE_BATTLE_PASS
#include "battle_pass.h"
#endif

#include "../../common/VnumHelper.h"
#include "DragonSoul.h"
#include "buff_on_attributes.h"
#include "belt_inventory_helper.h"
#include "../../common/CommonDefines.h"

//auction_temp
#ifdef ENABLE_STOLE_COSTUME
#include "../../common/stole_length.h"
#endif

const int ITEM_BROKEN_METIN_VNUM = 28960;
#define ENABLE_EFFECT_EXTRAPOT
#define ENABLE_BOOKS_STACKFIX
#define ENABLE_STONE_STACKFIX //USE ONLY 1 STONE TO ADD
//#define __USE_ADD_WITH_ALL_ITEMS__ //CAN ADD OR SWITH GREEN BONUS WITH ALL ITEMS (MAX LVL 40)

// CHANGE_ITEM_ATTRIBUTES
// const DWORD CHARACTER::msc_dwDefaultChangeItemAttrCycle = 10;
const char CHARACTER::msc_szLastChangeItemAttrFlag[] = "Item.LastChangeItemAttr";
// const char CHARACTER::msc_szChangeItemAttrCycleFlag[] = "change_itemattr_cycle";
// END_OF_CHANGE_ITEM_ATTRIBUTES
const BYTE g_aBuffOnAttrPoints[] = { POINT_ENERGY, POINT_COSTUME_ATTR_BONUS };

#ifdef ENABLE_PVP_ADVANCED
static bool IS_POTION_PVP_BLOCKED(int vnum)
{
	switch (vnum)
	{
		case 72725:
		case 72726:
			return true;
	}
	return false;
}
#endif

struct FFindStone
{
	std::map<DWORD, LPCHARACTER> m_mapStone;

	void operator()(LPENTITY pEnt)
	{
		if (pEnt->IsType(ENTITY_CHARACTER) == true)
		{
			LPCHARACTER pChar = (LPCHARACTER)pEnt;

			if (pChar->IsStone() == true)
			{
				m_mapStone[(DWORD)pChar->GetVID()] = pChar;
			}
		}
	}
};


//��ȯ��, ��ȯ����, ��ȥ����
static bool IS_SUMMON_ITEM(int vnum)
{
	switch (vnum)
	{
		case 22000:
		case 22010:
		case 22011:
		case 22020:
		case ITEM_MARRIAGE_RING:
			return true;
	}

	return false;
}

bool IS_SUMMONABLE_ZONE(int map_index)
{
	switch (map_index)
	{
		case 66 : // ���Ÿ��
		case 71 : // �Ź� ���� 2��
		case 72 : // õ�� ����
		case 73 : // õ�� ���� 2��
		case 193 : // �Ź� ���� 2-1��
#if 0
		case 184 : // õ�� ����(�ż�)
		case 185 : // õ�� ���� 2��(�ż�)
		case 186 : // õ�� ����(õ��)
		case 187 : // õ�� ���� 2��(õ��)
		case 188 : // õ�� ����(����)
		case 189 : // õ�� ���� 2��(����)
#endif
//		case 206 : // �Ʊ͵���
		case 216 : // �Ʊ͵���
		case 217 : // �Ź� ���� 3��
		case 208 : // õ�� ���� (���)

		case 113 : // OX Event ��
			return false;
	}

	if (CBattleArena::IsBattleArenaMap(map_index)) return false;

	// ��� private ������ ���� �Ұ���
	if (map_index > 10000) return false;

	return true;
}

bool IS_BOTARYABLE_ZONE(int nMapIndex)
{
	if (!g_bEnableBootaryCheck) return true;

	switch (nMapIndex)
	{
		case 1 :
		case 3 :
		case 21 :
		case 23 :
		case 41 :
		case 43 :
			return true;
	}

	return false;
}

// item socket �� ������Ÿ�԰� ������ üũ -- by mhh
static bool FN_check_item_socket(LPITEM item)
{
	for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
	{
		if (item->GetSocket(i) != item->GetProto()->alSockets[i])
			return false;
	}

	return true;
}

// item socket ���� -- by mhh
static void FN_copy_item_socket(LPITEM dest, LPITEM src)
{
	for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
	{
		dest->SetSocket(i, src->GetSocket(i));
	}
}
static bool FN_check_item_sex(LPCHARACTER ch, LPITEM item)
{

#ifdef ENABLE_SORT_INVEN
	if (item->GetType() == ITEM_USE && item->GetSubType() == USE_AFFECT)
		return true;
#endif
	
	// ���� ����
	if (IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_MALE))
	{
		if (SEX_MALE==GET_SEX(ch))
			return false;
	}
	// ���ڱ���
	if (IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_FEMALE))
	{
		if (SEX_FEMALE==GET_SEX(ch))
			return false;
	}

	return true;
}


/////////////////////////////////////////////////////////////////////////////
// ITEM HANDLING
/////////////////////////////////////////////////////////////////////////////
bool CHARACTER::CanHandleItem(bool bSkipCheckRefine, bool bSkipObserver)
{
	if (!bSkipObserver)
		if (m_bIsObserver)
			return false;

	if (GetMyShop())
		return false;

	if (!bSkipCheckRefine)
		if (m_bUnderRefine)
			return false;

	if (IsCubeOpen() || NULL != DragonSoul_RefineWindow_GetOpener())
		return false;

#ifdef __ATTR_TRANSFER_SYSTEM__
	if (IsAttrTransferOpen())
		return false;
#endif

	if (IsWarping())
		return false;

#ifdef ENABLE_ACCE_SYSTEM
	if (IsAcceOpen())
		return false;
#endif

	return true;
}

LPITEM CHARACTER::GetInventoryItem(WORD wCell) const
{
	return GetItem(TItemPos(INVENTORY, wCell));
}


#ifdef ENABLE_EXTRA_INVENTORY
LPITEM CHARACTER::GetExtraInventoryItem(WORD wCell) const
{
	return GetItem(TItemPos(EXTRA_INVENTORY, wCell));
}
#endif

LPITEM CHARACTER::GetItem(TItemPos Cell) const
{
	if (!IsValidItemPosition(Cell))
		return NULL;
	WORD wCell = Cell.cell;
	BYTE window_type = Cell.window_type;
	switch (window_type)
	{
	case INVENTORY:
	case EQUIPMENT:
		if (wCell >= INVENTORY_AND_EQUIP_SLOT_MAX)
		{
			sys_err("CHARACTER::GetInventoryItem: invalid item cell %d", wCell);
			return NULL;
		}
		return m_pointsInstant.pItems[wCell];
	case DRAGON_SOUL_INVENTORY:
		if (wCell >= DRAGON_SOUL_INVENTORY_MAX_NUM)
		{
			sys_err("CHARACTER::GetInventoryItem: invalid DS item cell %d", wCell);
			return NULL;
		}
		return m_pointsInstant.pDSItems[wCell];
		
		
#ifdef ENABLE_EXTRA_INVENTORY
	case EXTRA_INVENTORY:
		if (wCell >= EXTRA_INVENTORY_MAX_NUM)
		{
			sys_err("CHARACTER::GetInventoryItem: invalid EXTRA item cell %d", wCell);
			return NULL;
		}
		return m_pointsInstant.pExtraItems[wCell];
#endif
		
#ifdef ENABLE_SWITCHBOT
	case SWITCHBOT:
		if (wCell >= SWITCHBOT_SLOT_COUNT)
		{
			sys_err("CHARACTER::GetInventoryItem: invalid switchbot item cell %d", wCell);
			return NULL;
		}
		return m_pointsInstant.pSwitchbotItems[wCell];
#endif
	default:
		return NULL;
	}
	return NULL;
}

#ifdef __HIGHLIGHT_SYSTEM__
void CHARACTER::SetItem(TItemPos Cell, LPITEM pItem, bool isHighLight)
#else
void CHARACTER::SetItem(TItemPos Cell, LPITEM pItem)
#endif
{
	WORD wCell = Cell.cell;
	BYTE window_type = Cell.window_type;
	if ((unsigned long)((CItem*)pItem) == 0xff || (unsigned long)((CItem*)pItem) == 0xffffffff)
	{
		sys_err("!!! FATAL ERROR !!! item == 0xff (char: %s cell: %u)", GetName(), wCell);
		core_dump();
		return;
	}

	if (pItem && pItem->GetOwner())
	{
		assert(!"GetOwner exist");
		return;
	}
	// �⺻ �κ��丮
	switch(window_type)
	{
	case INVENTORY:
	case EQUIPMENT:
		{
			if (wCell >= INVENTORY_AND_EQUIP_SLOT_MAX)
			{
				sys_err("CHARACTER::SetItem: invalid item cell %d", wCell);
				return;
			}

			LPITEM pOld = m_pointsInstant.pItems[wCell];

			if (pOld)
			{
				if (wCell < INVENTORY_MAX_NUM)
				{
					for (int i = 0; i < pOld->GetSize(); ++i)
					{
						int p = wCell + (i * 5);

						if (p >= INVENTORY_MAX_NUM)
							continue;

						if (m_pointsInstant.pItems[p] && m_pointsInstant.pItems[p] != pOld)
							continue;

						m_pointsInstant.bItemGrid[p] = 0;
					}
				}
				else
					m_pointsInstant.bItemGrid[wCell] = 0;
			}

			if (pItem)
			{
				if (wCell < INVENTORY_MAX_NUM)
				{
					for (int i = 0; i < pItem->GetSize(); ++i)
					{
						int p = wCell + (i * 5);

						if (p >= INVENTORY_MAX_NUM)
							continue;

						// wCell + 1 �� �ϴ� ���� ����� üũ�� �� ����
						// �������� ����ó���ϱ� ����
						m_pointsInstant.bItemGrid[p] = wCell + 1;
					}
				}
				else
					m_pointsInstant.bItemGrid[wCell] = wCell + 1;
			}

			m_pointsInstant.pItems[wCell] = pItem;
		}
		break;
	// ��ȥ�� �κ��丮
	case DRAGON_SOUL_INVENTORY:
		{
			LPITEM pOld = m_pointsInstant.pDSItems[wCell];

			if (pOld)
			{
				if (wCell < DRAGON_SOUL_INVENTORY_MAX_NUM)
				{
					for (int i = 0; i < pOld->GetSize(); ++i)
					{
						int p = wCell + (i * DRAGON_SOUL_BOX_COLUMN_NUM);

						if (p >= DRAGON_SOUL_INVENTORY_MAX_NUM)
							continue;

						if (m_pointsInstant.pDSItems[p] && m_pointsInstant.pDSItems[p] != pOld)
							continue;

						m_pointsInstant.wDSItemGrid[p] = 0;
					}
				}
				else
					m_pointsInstant.wDSItemGrid[wCell] = 0;
			}

			if (pItem)
			{
				if (wCell >= DRAGON_SOUL_INVENTORY_MAX_NUM)
				{
					sys_err("CHARACTER::SetItem: invalid DS item cell %d", wCell);
					return;
				}

				if (wCell < DRAGON_SOUL_INVENTORY_MAX_NUM)
				{
					for (int i = 0; i < pItem->GetSize(); ++i)
					{
						int p = wCell + (i * DRAGON_SOUL_BOX_COLUMN_NUM);

						if (p >= DRAGON_SOUL_INVENTORY_MAX_NUM)
							continue;

						// wCell + 1 �� �ϴ� ���� ����� üũ�� �� ����
						// �������� ����ó���ϱ� ����
						m_pointsInstant.wDSItemGrid[p] = wCell + 1;
					}
				}
				else
					m_pointsInstant.wDSItemGrid[wCell] = wCell + 1;
			}

			m_pointsInstant.pDSItems[wCell] = pItem;
		}
		break;
		
		

#ifdef ENABLE_EXTRA_INVENTORY
	case EXTRA_INVENTORY:
		{
			if (wCell >= EXTRA_INVENTORY_MAX_NUM)
			{
				sys_err("CHARACTER::SetItem: invalid EXTRA item cell %d", wCell);
				return;
			}

			LPITEM pOld = m_pointsInstant.pExtraItems[wCell];

			if (pOld)
			{
				if (wCell < EXTRA_INVENTORY_MAX_NUM)
				{
					for (int i = 0; i < pOld->GetSize(); ++i)
					{
						int p = wCell + (i * EXTRA_INVENTORY_PAGE_COLUMN);

						if (p >= EXTRA_INVENTORY_MAX_NUM)
							continue;

						if (m_pointsInstant.pExtraItems[p] && m_pointsInstant.pExtraItems[p] != pOld)
							continue;

						m_pointsInstant.wExtraItemGrid[p] = 0;
					}
				}
				else
					m_pointsInstant.wExtraItemGrid[wCell] = 0;
			}

			if (pItem)
			{
				if (wCell < EXTRA_INVENTORY_MAX_NUM)
				{
					for (int i = 0; i < pItem->GetSize(); ++i)
					{
						int p = wCell + (i * EXTRA_INVENTORY_PAGE_COLUMN);

						if (p >= EXTRA_INVENTORY_MAX_NUM)
							continue;

						m_pointsInstant.wExtraItemGrid[p] = wCell + 1;
					}
				}
				else
					m_pointsInstant.wExtraItemGrid[wCell] = wCell + 1;
			}

			m_pointsInstant.pExtraItems[wCell] = pItem;
		}
		break;
#endif
		
#ifdef ENABLE_SWITCHBOT
	case SWITCHBOT:
		{
			LPITEM pOld = m_pointsInstant.pSwitchbotItems[wCell];
			if (pItem && pOld)
			{
				return;
			}

			if (wCell >= SWITCHBOT_SLOT_COUNT)
			{
				sys_err("CHARACTER::SetItem: invalid switchbot item cell %d", wCell);
				return;
			}

			if (pItem)
			{
				CSwitchbotManager::Instance().RegisterItem(GetPlayerID(), pItem->GetID(), wCell);
			}
			else
			{
				CSwitchbotManager::Instance().UnregisterItem(GetPlayerID(), wCell);
			}

			m_pointsInstant.pSwitchbotItems[wCell] = pItem;
		}
		break;
#endif
	default:
		sys_err ("Invalid Inventory type %d", window_type);
		return;
	}

	if (GetDesc())
	{
		// Ȯ�� ������: �������� ������ �÷��� ������ ������
		if (pItem)
		{
			TPacketGCItemSet pack;
			pack.header = HEADER_GC_ITEM_SET;
			pack.Cell = Cell;

			pack.count = pItem->GetCount();
#ifdef ATTR_LOCK
			pack.lockedattr = pItem->GetLockedAttr();
#endif
			pack.vnum = pItem->GetVnum();
			pack.flags = pItem->GetFlag();
			pack.anti_flags	= pItem->GetAntiFlag();
#ifdef __HIGHLIGHT_SYSTEM__
			pack.highlight = isHighLight;
#else
			pack.highlight = (Cell.window_type == DRAGON_SOUL_INVENTORY);
#endif

			thecore_memcpy(pack.alSockets, pItem->GetSockets(), sizeof(pack.alSockets));
			thecore_memcpy(pack.aAttr, pItem->GetAttributes(), sizeof(pack.aAttr));

			GetDesc()->Packet(&pack, sizeof(TPacketGCItemSet));
		}
		else
		{
			TPacketGCItemDelDeprecated pack;
			pack.header = HEADER_GC_ITEM_DEL;
			pack.Cell = Cell;
			pack.count = 0;
#ifdef ATTR_LOCK
			pack.lockedattr = -1;
#endif
			pack.vnum = 0;
			memset(pack.alSockets, 0, sizeof(pack.alSockets));
			memset(pack.aAttr, 0, sizeof(pack.aAttr));

			GetDesc()->Packet(&pack, sizeof(TPacketGCItemDelDeprecated));
		}
	}

	if (pItem)
	{
		pItem->SetCell(this, wCell);
		switch (window_type)
		{
		case INVENTORY:
		case EQUIPMENT:
			if ((wCell < INVENTORY_MAX_NUM) || (BELT_INVENTORY_SLOT_START <= wCell && BELT_INVENTORY_SLOT_END > wCell))
				pItem->SetWindow(INVENTORY);
			else
				pItem->SetWindow(EQUIPMENT);
			break;
		case DRAGON_SOUL_INVENTORY:
			pItem->SetWindow(DRAGON_SOUL_INVENTORY);
			break;
#ifdef ENABLE_EXTRA_INVENTORY
		case EXTRA_INVENTORY:
			pItem->SetWindow(EXTRA_INVENTORY);
			break;
#endif
#ifdef ENABLE_SWITCHBOT
		case SWITCHBOT:
			pItem->SetWindow(SWITCHBOT);
			break;
#endif
		}
	}
}

LPITEM CHARACTER::GetWear(BYTE bCell) const
{
	// > WEAR_MAX_NUM : ��ȥ�� ���Ե�.
	if (bCell >= WEAR_MAX_NUM + DRAGON_SOUL_DECK_MAX_NUM * DS_SLOT_MAX)
	{
		sys_err("CHARACTER::GetWear: invalid wear cell %d", bCell);
		return NULL;
	}

	return m_pointsInstant.pItems[INVENTORY_MAX_NUM + bCell];
}

void CHARACTER::SetWear(BYTE bCell, LPITEM item)
{
	// > WEAR_MAX_NUM : ��ȥ�� ���Ե�.
	if (bCell >= WEAR_MAX_NUM + DRAGON_SOUL_DECK_MAX_NUM * DS_SLOT_MAX)
	{
		sys_err("CHARACTER::SetItem: invalid item cell %d", bCell);
		return;
	}

#ifdef __HIGHLIGHT_SYSTEM__
	SetItem(TItemPos (INVENTORY, INVENTORY_MAX_NUM + bCell), item, false);
#else
	SetItem(TItemPos (INVENTORY, INVENTORY_MAX_NUM + bCell), item);
#endif

#ifndef ENABLE_BUG_FIXES
	if (!item && bCell == WEAR_WEAPON) {
		if (IsAffectFlag(AFF_GWIGUM))
			RemoveAffect(SKILL_GWIGEOM);

		if (IsAffectFlag(AFF_GEOMGYEONG))
			RemoveAffect(SKILL_GEOMKYUNG);
	}
#endif
}

void CHARACTER::ClearItem()
{
	int		i;
	LPITEM	item;

	for (i = 0; i < INVENTORY_AND_EQUIP_SLOT_MAX; ++i)
	{
		if ((item = GetInventoryItem(i)))
		{
			item->SetSkipSave(true);
			ITEM_MANAGER::instance().FlushDelayedSave(item);

			item->RemoveFromCharacter();
			M2_DESTROY_ITEM(item);

			SyncQuickslot(QUICKSLOT_TYPE_ITEM, i, 255);
		}
	}
	for (i = 0; i < DRAGON_SOUL_INVENTORY_MAX_NUM; ++i)
	{
		if ((item = GetItem(TItemPos(DRAGON_SOUL_INVENTORY, i))))
		{
			item->SetSkipSave(true);
			ITEM_MANAGER::instance().FlushDelayedSave(item);

			item->RemoveFromCharacter();
			M2_DESTROY_ITEM(item);
		}
	}
	
#ifdef ENABLE_EXTRA_INVENTORY
	for (i = 0; i < EXTRA_INVENTORY_MAX_NUM; ++i)
	{
		if ((item = GetExtraInventoryItem(i)))
		{
			item->SetSkipSave(true);
			ITEM_MANAGER::instance().FlushDelayedSave(item);

			item->RemoveFromCharacter();
			M2_DESTROY_ITEM(item);

			SyncQuickslot(QUICKSLOT_TYPE_ITEM_EXTRA, i, 255);
		}
	}
#endif
	
#ifdef ENABLE_SWITCHBOT
	for (i = 0; i < SWITCHBOT_SLOT_COUNT; ++i)
	{
		if ((item = GetItem(TItemPos(SWITCHBOT, i))))
		{
			item->SetSkipSave(true);
			ITEM_MANAGER::instance().FlushDelayedSave(item);

			item->RemoveFromCharacter();
			M2_DESTROY_ITEM(item);
		}
	}
#endif
}


bool CHARACTER::IsEmptyItemGrid(TItemPos Cell, BYTE bSize, int iExceptionCell) const
{
#ifdef __ENABLE_EXTEND_INVEN_SYSTEM__
	switch (Cell.window_type)
	{
	case INVENTORY:
		{
			BYTE bCell = Cell.cell;

			// bItemCell? 0? false?? ???? ?? + 1 ?? ????.
			// ??? iExceptionCell? 1? ?? ????.
			++iExceptionCell;

			if (Cell.IsBeltInventoryPosition())
			{
				LPITEM beltItem = GetWear(WEAR_BELT);

				if (NULL == beltItem)
					return false;

				if (false == CBeltInventoryHelper::IsAvailableCell(bCell - BELT_INVENTORY_SLOT_START, beltItem->GetValue(0)))
					return false;

				if (m_pointsInstant.bItemGrid[bCell])
				{
					if (m_pointsInstant.bItemGrid[bCell] == iExceptionCell)
						return true;

					return false;
				}

				if (bSize == 1)
					return true;

			}
			//black
			else if (bCell >= Inventory_Size())
				return false;

			if (m_pointsInstant.bItemGrid[bCell])
			{
				if (m_pointsInstant.bItemGrid[bCell] == iExceptionCell)
				{
					if (bSize == 1)
						return true;

					int j = 1;
					BYTE bPage = bCell / (INVENTORY_MAX_NUM / 4);
					do
					{
						BYTE p = bCell + (5 * j);

						if (p >= Inventory_Size())
							return false;

						if (p / (INVENTORY_MAX_NUM / 4) != bPage)
							return false;

						if (m_pointsInstant.bItemGrid[p])
							if (m_pointsInstant.bItemGrid[p] != iExceptionCell)
								return false;
					}
					while (++j < bSize);

					return true;
				}
				else
					return false;
			}

			// ??? 1?? ??? ???? ???? ?? ??
			if (1 == bSize)
				return true;
			else
			{
				int j = 1;
				BYTE bPage = bCell / (INVENTORY_MAX_NUM / 4);

				do
				{
					BYTE p = bCell + (5 * j);

					if (p >= Inventory_Size())
						return false;
					if (p / (INVENTORY_MAX_NUM / 4) != bPage)
						return false;

					if (m_pointsInstant.bItemGrid[p])
						if (m_pointsInstant.bItemGrid[p] != iExceptionCell)
							return false;
				}
				while (++j < bSize);

				return true;
			}
		}
#ifdef ENABLE_EXTRA_INVENTORY
		break;
	case EXTRA_INVENTORY:
	{
		WORD bCell = Cell.cell;

		if (bCell > ExtraInventoryMaxSlots(bCell, true))
			return false;
		
		++iExceptionCell;

		if (m_pointsInstant.wExtraItemGrid[bCell])
		{
			if (m_pointsInstant.wExtraItemGrid[bCell] == iExceptionCell)
			{
				if (bSize == 1)
					return true;

				int j = 1;
				BYTE bPage = bCell / (EXTRA_INVENTORY_MAX_NUM / EXTRA_INVENTORY_PAGE_COUNT);

				do
				{
					int p = bCell + (5 * j);

					if (p > ExtraInventoryMaxSlots(bCell, true))
						return false;

					if (p / (EXTRA_INVENTORY_MAX_NUM / EXTRA_INVENTORY_PAGE_COUNT) != bPage)
						return false;

					if (m_pointsInstant.wExtraItemGrid[p])
						if (m_pointsInstant.wExtraItemGrid[p] != iExceptionCell)
							return false;
				} while (++j < bSize);

				return true;
			}
			else
				return false;
		}

		if (1 == bSize)
			return true;
		else
		{
			int j = 1;
			BYTE bPage = bCell / (EXTRA_INVENTORY_MAX_NUM / EXTRA_INVENTORY_PAGE_COUNT);

			do
			{
				int p = bCell + (5 * j);

				if (p > ExtraInventoryMaxSlots(bCell, true))
					return false;

				if (p / (EXTRA_INVENTORY_MAX_NUM / EXTRA_INVENTORY_PAGE_COUNT) != bPage)
					return false;

				if (m_pointsInstant.wExtraItemGrid[p])
					if (m_pointsInstant.wExtraItemGrid[p] != iExceptionCell)
						return false;
			} while (++j < bSize);

			return true;
		}
	}
#endif
		break;
#else
		switch (Cell.window_type)
		{
		case INVENTORY:
			{
				BYTE bCell = Cell.cell;

				// bItemCell? 0? false?? ???? ?? + 1 ?? ????.
				// ??? iExceptionCell? 1? ?? ????.
				++iExceptionCell;

				if (Cell.IsBeltInventoryPosition())
				{
					LPITEM beltItem = GetWear(WEAR_BELT);

					if (NULL == beltItem)
						return false;

					if (false == CBeltInventoryHelper::IsAvailableCell(bCell - BELT_INVENTORY_SLOT_START, beltItem->GetValue(0)))
						return false;

					if (m_pointsInstant.bItemGrid[bCell])
					{
						if (m_pointsInstant.bItemGrid[bCell] == iExceptionCell)
							return true;

						return false;
					}

					if (bSize == 1)
						return true;

				}
				//black
				else if (bCell >= INVENTORY_MAX_NUM)
					return false;

				if (m_pointsInstant.bItemGrid[bCell])
				{
					if (m_pointsInstant.bItemGrid[bCell] == iExceptionCell)
					{
						if (bSize == 1)
							return true;

						int j = 1;
						BYTE bPage = bCell / (INVENTORY_MAX_NUM / 4);

						do
						{
							BYTE p = bCell + (5 * j);

							if (p >= INVENTORY_MAX_NUM)
								return false;

							if (p / (INVENTORY_MAX_NUM / 4) != bPage)
								return false;

							if (m_pointsInstant.bItemGrid[p])
								if (m_pointsInstant.bItemGrid[p] != iExceptionCell)
									return false;
						}
						while (++j < bSize);

						return true;
					}
					else
						return false;
				}

				// ??? 1?? ??? ???? ???? ?? ??
				if (1 == bSize)
					return true;
				else
				{
					int j = 1;
					BYTE bPage = bCell / (INVENTORY_MAX_NUM / 4);

					do
					{
						BYTE p = bCell + (5 * j);

						if (p >= INVENTORY_MAX_NUM)
							return false;
						if (p / (INVENTORY_MAX_NUM / 4) != bPage)
							return false;

						if (m_pointsInstant.bItemGrid[p])
							if (m_pointsInstant.bItemGrid[p] != iExceptionCell)
								return false;
					}
					while (++j < bSize);

					return true;
				}
			}
#ifdef ENABLE_EXTRA_INVENTORY
			break;
		case EXTRA_INVENTORY:
		{
			WORD bCell = Cell.cell;

			if (bCell >= EXTRA_INVENTORY_MAX_NUM)
				return false;

			++iExceptionCell;

			if (m_pointsInstant.wExtraItemGrid[bCell])
			{
				if (m_pointsInstant.wExtraItemGrid[bCell] == iExceptionCell)
				{
					if (bSize == 1)
						return true;

					int j = 1;
					BYTE bPage = bCell / (EXTRA_INVENTORY_MAX_NUM / EXTRA_INVENTORY_PAGE_COUNT);

					do
					{
						BYTE p = bCell + (5 * j);

						if (p >= EXTRA_INVENTORY_MAX_NUM)
							return false;

						if (p / (EXTRA_INVENTORY_MAX_NUM / EXTRA_INVENTORY_PAGE_COUNT) != bPage)
							return false;

						if (m_pointsInstant.wExtraItemGrid[p])
							if (m_pointsInstant.wExtraItemGrid[p] != iExceptionCell)
								return false;
					} while (++j < bSize);

					return true;
				}
				else
					return false;
			}

			if (1 == bSize)
				return true;
			else
			{
				int j = 1;
				BYTE bPage = bCell / (EXTRA_INVENTORY_MAX_NUM / EXTRA_INVENTORY_PAGE_COUNT);

				do
				{
					BYTE p = bCell + (5 * j);

					if (p >= EXTRA_INVENTORY_MAX_NUM)
						return false;

					if (p / (EXTRA_INVENTORY_MAX_NUM / EXTRA_INVENTORY_PAGE_COUNT) != bPage)
						return false;

					if (m_pointsInstant.wExtraItemGrid[p])
						if (m_pointsInstant.wExtraItemGrid[p] != iExceptionCell)
							return false;
				} while (++j < bSize);

				return true;
			}
		}
#endif
			break;
#endif


#ifdef ENABLE_SWITCHBOT
	case SWITCHBOT:
		{
		WORD wCell = Cell.cell;
		if (wCell >= SWITCHBOT_SLOT_COUNT)
		{
			return false;
		}

		if (m_pointsInstant.pSwitchbotItems[wCell])
		{
			return false;
		}

		return true;
		}
#endif
	case DRAGON_SOUL_INVENTORY:
		{
			WORD wCell = Cell.cell;
			if (wCell >= DRAGON_SOUL_INVENTORY_MAX_NUM)
				return false;

			// bItemCell�� 0�� false���� ��Ÿ���� ���� + 1 �ؼ� ó���Ѵ�.
			// ���� iExceptionCell�� 1�� ���� ���Ѵ�.
			iExceptionCell++;

			if (m_pointsInstant.wDSItemGrid[wCell])
			{
				if (m_pointsInstant.wDSItemGrid[wCell] == iExceptionCell)
				{
					if (bSize == 1)
						return true;

					int j = 1;

					do
					{
						int p = wCell + (DRAGON_SOUL_BOX_COLUMN_NUM * j);

						if (p >= DRAGON_SOUL_INVENTORY_MAX_NUM)
							return false;

						if (m_pointsInstant.wDSItemGrid[p])
							if (m_pointsInstant.wDSItemGrid[p] != iExceptionCell)
								return false;
					}
					while (++j < bSize);

					return true;
				}
				else
					return false;
			}

			// ũ�Ⱑ 1�̸� ��ĭ�� �����ϴ� ���̹Ƿ� �׳� ����
			if (1 == bSize)
				return true;
			else
			{
				int j = 1;

				do
				{
					int p = wCell + (DRAGON_SOUL_BOX_COLUMN_NUM * j);

					if (p >= DRAGON_SOUL_INVENTORY_MAX_NUM)
						return false;

					if (m_pointsInstant.bItemGrid[p])
						if (m_pointsInstant.wDSItemGrid[p] != iExceptionCell)
							return false;
				}
				while (++j < bSize);

				return true;
			}
		}
	}
	return false;
}

int CHARACTER::GetEmptyInventory(BYTE size) const
{
	// NOTE: ���� �� �Լ��� ������ ����, ȹ�� ���� ������ �� �� �κ��丮�� �� ĭ�� ã�� ���� ���ǰ� �ִµ�,
	//		��Ʈ �κ��丮�� Ư�� �κ��丮�̹Ƿ� �˻����� �ʵ��� �Ѵ�. (�⺻ �κ��丮: INVENTORY_MAX_NUM ������ �˻�)
#ifdef __ENABLE_EXTEND_INVEN_SYSTEM__
	for ( int i = 0; i < Inventory_Size(); ++i)
#else
	for ( int i = 0; i < INVENTORY_MAX_NUM; ++i)	
#endif
		if (IsEmptyItemGrid(TItemPos (INVENTORY, i), size))
			return i;
	return -1;
}

#ifdef ENABLE_LOCKED_EXTRA_INVENTORY
int CHARACTER::ExtraInventoryMaxSlots(int iArg1, bool bAuto) const {
	if (bAuto) {
		if ((iArg1 >= 0) && (iArg1 < (EXTRA_INVENTORY_CATEGORY_MAX_NUM * 1)))
			iArg1 = 0;
		else if ((iArg1 >= (EXTRA_INVENTORY_CATEGORY_MAX_NUM * 1)) && (iArg1 < (EXTRA_INVENTORY_CATEGORY_MAX_NUM * 2)))
			iArg1 = 1;
		else if ((iArg1 >= (EXTRA_INVENTORY_CATEGORY_MAX_NUM * 2)) && (iArg1 < (EXTRA_INVENTORY_CATEGORY_MAX_NUM * 3)))
			iArg1 = 2;
		else if ((iArg1 >= (EXTRA_INVENTORY_CATEGORY_MAX_NUM * 3)) && (iArg1 < (EXTRA_INVENTORY_CATEGORY_MAX_NUM * 4)))
			iArg1 = 3;
		else if ((iArg1 >= (EXTRA_INVENTORY_CATEGORY_MAX_NUM * 4)) && (iArg1 < (EXTRA_INVENTORY_CATEGORY_MAX_NUM * 5)))
			iArg1 = 4;
	}
	
	if ((iArg1 < 0) || (iArg1 > 4))
		return 0;
	
	int iUnlock;
	switch (iArg1) {
		case 0: {
			iUnlock = GetQuestFlag("lock_extra.cat1") * 5;
			break;
		}
		case 1: {
			iUnlock = GetQuestFlag("lock_extra.cat2") * 5;
			break;
		}
		case 2: {
			iUnlock = GetQuestFlag("lock_extra.cat3") * 5;
			break;
		}
		case 3: {
			iUnlock = GetQuestFlag("lock_extra.cat4") * 5;
			break;
		}
		case 4: {
			iUnlock = GetQuestFlag("lock_extra.cat5") * 5;
			break;
		}
		default: {
			iUnlock = 0;
			break;
		}
	}

	//int iUnlock = GetPoint(POINT_EXTRA_INVENTORY1 + iArg1) * 5;
	int iMaxUnlock = 25 + EXTRA_INVENTORY_PAGE_SIZE;
	int iStart = EXTRA_INVENTORY_CATEGORY_MAX_NUM * iArg1;
	int iFree = (EXTRA_INVENTORY_PAGE_SIZE * 2) + 20;
	return iUnlock > iMaxUnlock ? iMaxUnlock + iStart + iFree : iUnlock + iStart + iFree;
}

static int NeedKeysForExtraInventory[] = {
											1, // 20-25
											1, // 25-30
											1, // 30-35
											2, // 35-40
											2, // 40-45 : end page 3
											2, // 45-50
											3, // 50-55
											3, // 55-60
											3, // 60-65
											4, // 65-70
											4, // 70-75
											4, // 75-80
											5, // 80-85
											6, // 90-95 : end page 4
};

void CHARACTER::UnlockExtraInventory(BYTE category) {
	if ((category < 0) || (category > 4)) {
		return;
	}

#ifdef ENABLE_SPAM_CHECK
	int32_t time = GetLastUnlock() - get_global_time();
	if (time > 0) {
#ifdef TEXTS_IMPROVEMENT
		ChatPacketNew(CHAT_TYPE_INFO, 234, "%d", time);
#endif
		return;
	}
#endif

	std::string stageName;
	switch (category) {
		case 1: {
			stageName = "lock_extra.cat2";
		} break;
		case 2: {
			stageName = "lock_extra.cat3";
		} break;
		case 3: {
			stageName = "lock_extra.cat4";
		} break;
		case 4: {
			stageName = "lock_extra.cat5";
		} break;
		default: {
			stageName = "lock_extra.cat1";
		} break;
	}

	BYTE stage = GetQuestFlag(stageName.c_str());
	if (stage < 0 || stage >= 14)
		return;

	int needKeys = NeedKeysForExtraInventory[stage];
	if (CountSpecifyItem(72320) >= needKeys) {
		RemoveSpecifyItem(72320, needKeys);
		
		SetQuestFlag(stageName.c_str(), stage + 1);
		PointChange(POINT_EXTRA_INVENTORY1 + category, stage + 1);
		ChatPacket(CHAT_TYPE_COMMAND, "RefreshExpandInventory");
#ifdef ENABLE_SPAM_CHECK
		SetLastUnlock();
#endif
	} else {
		ChatPacket(CHAT_TYPE_COMMAND, "update_envanter_need %d", needKeys - CountSpecifyItem(72320));
	}
}
#endif

#ifdef ENABLE_EXTRA_INVENTORY
int CHARACTER::GetEmptyExtraInventory(LPITEM pItem) const
{
	BYTE category = pItem->GetExtraCategory();
#ifdef ENABLE_LOCKED_EXTRA_INVENTORY
	for (int i = EXTRA_INVENTORY_CATEGORY_MAX_NUM * category; i < ExtraInventoryMaxSlots(category); ++i)
#else
	for (int i = EXTRA_INVENTORY_CATEGORY_MAX_NUM * category; i < EXTRA_INVENTORY_CATEGORY_MAX_NUM * (category + 1); ++i)
#endif
		if (IsEmptyItemGrid(TItemPos(EXTRA_INVENTORY, i), pItem->GetSize()))
			return i;

	return -1;
}

int CHARACTER::GetEmptyExtraInventory(BYTE size, BYTE category) const // needed for offline shop
{
#ifdef ENABLE_LOCKED_EXTRA_INVENTORY
	for (int i = EXTRA_INVENTORY_CATEGORY_MAX_NUM * category; i < ExtraInventoryMaxSlots(category); ++i)
#else
	for (int i = EXTRA_INVENTORY_CATEGORY_MAX_NUM * category; i < EXTRA_INVENTORY_CATEGORY_MAX_NUM * (category + 1); ++i)
#endif
		if (IsEmptyItemGrid(TItemPos(EXTRA_INVENTORY, i), size))
			return i;

	return -1;
}
#endif

int CHARACTER::GetEmptyDragonSoulInventory(LPITEM pItem) const
{
	if (NULL == pItem || !pItem->IsDragonSoul())
		return -1;

	BYTE bSize = pItem->GetSize();
	WORD wBaseCell = DSManager::instance().GetBasePosition(pItem);

	if (WORD_MAX == wBaseCell)
		return -1;

	for (int i = 0; i < DRAGON_SOUL_BOX_SIZE; ++i)
		if (IsEmptyItemGrid(TItemPos(DRAGON_SOUL_INVENTORY, i + wBaseCell), bSize))
			return i + wBaseCell;

	return -1;
}

void CHARACTER::CopyDragonSoulItemGrid(std::vector<WORD>& vDragonSoulItemGrid) const
{
	vDragonSoulItemGrid.resize(DRAGON_SOUL_INVENTORY_MAX_NUM);

	std::copy(m_pointsInstant.wDSItemGrid, m_pointsInstant.wDSItemGrid + DRAGON_SOUL_INVENTORY_MAX_NUM, vDragonSoulItemGrid.begin());
}

int CHARACTER::CountEmptyInventory() const
{
	int	count = 0;

#ifdef __ENABLE_EXTEND_INVEN_SYSTEM__
	for (int i = 0; i < Inventory_Size(); ++i)
#else
	for (int i = 0; i < INVENTORY_MAX_NUM; ++i)	
#endif
		if (GetInventoryItem(i))
			count += GetInventoryItem(i)->GetSize();

#ifdef __ENABLE_EXTEND_INVEN_SYSTEM__
	return (Inventory_Size() - count);
#else
	return (INVENTORY_MAX_NUM - count);
#endif
}

void TransformRefineItem(LPITEM pkOldItem, LPITEM pkNewItem)
{
	// ACCESSORY_REFINE
	if (pkOldItem->IsAccessoryForSocket())
	{
		for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
		{
			pkNewItem->SetSocket(i, pkOldItem->GetSocket(i));
		}
		//pkNewItem->StartAccessorySocketExpireEvent();
	}
	// END_OF_ACCESSORY_REFINE
	else
	{
		// ���⼭ �������� �ڵ������� û�� ��
		for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
		{
			if (!pkOldItem->GetSocket(i))
				break;
			else
				pkNewItem->SetSocket(i, 1);
		}

		// ���� ����
		int slot = 0;

		for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
		{
			long socket = pkOldItem->GetSocket(i);

			if (socket > 2 && socket != ITEM_BROKEN_METIN_VNUM)
				pkNewItem->SetSocket(slot++, socket);
		}

	}

	// ���� ������ ����
	pkOldItem->CopyAttributeTo(pkNewItem);
}

void NotifyRefineSuccess(LPCHARACTER ch, LPITEM item, const char* way)
{
	if (NULL != ch && item != NULL)
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "RefineSuceeded");

		LogManager::instance().RefineLog(ch->GetPlayerID(), item->GetName(), item->GetID(), item->GetRefineLevel(), 1, way);
	}
}

void NotifyRefineFail(LPCHARACTER ch, LPITEM item, const char* way, int success = 0)
{
	if (NULL != ch && NULL != item)
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "RefineFailed");

		LogManager::instance().RefineLog(ch->GetPlayerID(), item->GetName(), item->GetID(), item->GetRefineLevel(), success, way);
	}
}

void CHARACTER::SetRefineNPC(LPCHARACTER ch)
{
	if ( ch != NULL )
	{
		m_dwRefineNPCVID = ch->GetVID();
	}
	else
	{
		m_dwRefineNPCVID = 0;
	}
}

bool CHARACTER::DoRefine(LPITEM item, bool bMoneyOnly)
{
	if (!CanHandleItem(true))
	{
		ClearRefineMode();
		return false;
	}

	//���� �ð����� : upgrade_refine_scroll.quest ���� ������ 5���̳��� �Ϲ� ������
	//�����Ҽ� ����
	if (quest::CQuestManager::instance().GetEventFlag("update_refine_time") != 0)
	{
		if (get_global_time() < quest::CQuestManager::instance().GetEventFlag("update_refine_time") + (60 * 5))
		{
			sys_log(0, "can't refine %d %s", GetPlayerID(), GetName());
			return false;
		}
	}

	const TRefineTable * prt = CRefineManager::instance().GetRefineRecipe(item->GetRefineSet());

	if (!prt)
		return false;

	DWORD result_vnum = item->GetRefinedVnum();
	int cost = ComputeRefineFee(prt->cost);

	if (result_vnum == 0)
	{
#ifdef TEXTS_IMPROVEMENT
		ChatPacketNew(CHAT_TYPE_INFO, 305, "");
#endif
		return false;
	}

	if (item->GetType() == ITEM_USE && item->GetSubType() == USE_TUNING)
		return false;

	TItemTable * pProto = ITEM_MANAGER::instance().GetTable(item->GetRefinedVnum());

	if (!pProto)
	{
#ifdef TEXTS_IMPROVEMENT
		ChatPacketNew(CHAT_TYPE_INFO, 427, "");
#endif
		return false;
	}

	// REFINE_COST
	if (GetGold() < cost)
	{
#ifdef TEXTS_IMPROVEMENT
		ChatPacketNew(CHAT_TYPE_INFO, 232, "");
#endif
		return false;
	}

	if (!bMoneyOnly)
	{
		for (int i = 0; i < prt->material_count; ++i)
		{
			if (CountSpecifyItem(prt->materials[i].vnum) < prt->materials[i].count)
			{
#ifdef TEXTS_IMPROVEMENT
				ChatPacketNew(CHAT_TYPE_INFO, 233, "");
#endif
				return false;
			}
		}

		for (int i = 0; i < prt->material_count; ++i)
			RemoveSpecifyItem(prt->materials[i].vnum, prt->materials[i].count);
	}

	int prob = number(1, 100);


#ifdef ENABLE_FEATURES_REFINE_SYSTEM	
	if (IsRefineThroughGuild() || bMoneyOnly)
	{
		prob -= 10;
	}
	
	int success_prob = prt->prob;
	success_prob += CRefineManager::instance().Result(this);
#else
	if (IsRefineThroughGuild() || bMoneyOnly)
		prob -= 10;

#endif
	// END_OF_REFINE_COST
#ifdef ENABLE_FEATURES_REFINE_SYSTEM	
	if (prob <= success_prob)
#else
	if (prob <= prt->prob)
#endif
	{
		// ����! ��� �������� �������, ���� �Ӽ��� �ٸ� ������ ȹ��
		LPITEM pkNewItem = ITEM_MANAGER::instance().CreateItem(result_vnum, 1, 0, false);

		if (pkNewItem)
		{
			ITEM_MANAGER::CopyAllAttrTo(item, pkNewItem);
			LogManager::instance().ItemLog(this, pkNewItem, "REFINE SUCCESS", pkNewItem->GetName());

			BYTE bCell = item->GetCell();
			

#ifdef ENABLE_BATTLE_PASS
			BYTE bBattlePassId = GetBattlePassId();
			if(bBattlePassId)
			{
				DWORD dwItemVnum, dwCount;
				if(CBattlePass::instance().BattlePassMissionGetInfo(bBattlePassId, REFINE_ITEM, &dwItemVnum, &dwCount))
				{
					if(dwItemVnum == item->GetVnum() && GetMissionProgress(REFINE_ITEM, bBattlePassId) < dwCount)
						UpdateMissionProgress(REFINE_ITEM, bBattlePassId, 1, dwCount);
				}
			}
#endif

			// DETAIL_REFINE_LOG
			NotifyRefineSuccess(this, item, IsRefineThroughGuild() ? "GUILD" : "POWER");
			DBManager::instance().SendMoneyLog(MONEY_LOG_REFINE, item->GetVnum(), -cost);
			ITEM_MANAGER::instance().RemoveItem(item, "REMOVE (REFINE SUCCESS)");
			// END_OF_DETAIL_REFINE_LOG

			pkNewItem->AddToCharacter(this, TItemPos(INVENTORY, bCell));
			ITEM_MANAGER::instance().FlushDelayedSave(pkNewItem);

			sys_log(0, "Refine Success %d", cost);
			pkNewItem->AttrLog();
			//PointChange(POINT_GOLD, -cost);
			sys_log(0, "PayPee %d", cost);
#ifdef ENABLE_FEATURES_REFINE_SYSTEM
			CRefineManager::instance().Reset(this);
#endif
			PayRefineFee(cost);
			sys_log(0, "PayPee End %d", cost);
		}
		else
		{
			// DETAIL_REFINE_LOG
			// ������ ������ ���� -> ���� ���з� ����
			sys_err("cannot create item %u", result_vnum);
			NotifyRefineFail(this, item, IsRefineThroughGuild() ? "GUILD" : "POWER");
			// END_OF_DETAIL_REFINE_LOG
		}
	}
	else
	{
		// ����! ��� �������� �����.
		DBManager::instance().SendMoneyLog(MONEY_LOG_REFINE, item->GetVnum(), -cost);
		NotifyRefineFail(this, item, IsRefineThroughGuild() ? "GUILD" : "POWER");
		item->AttrLog();
		ITEM_MANAGER::instance().RemoveItem(item, "REMOVE (REFINE FAIL)");

		//PointChange(POINT_GOLD, -cost);
#ifdef ENABLE_FEATURES_REFINE_SYSTEM
		CRefineManager::instance().Reset(this);
#endif
		PayRefineFee(cost);
	}

	return true;
}

enum enum_RefineScrolls
{
	CHUKBOK_SCROLL = 0,
	HYUNIRON_CHN   = 1, // �߱������� ���
	YONGSIN_SCROLL = 2,
	MUSIN_SCROLL   = 3,
	YAGONG_SCROLL  = 4,
	MEMO_SCROLL	   = 5,
	BDRAGON_SCROLL	= 6,
#ifdef ENABLE_SOUL_SYSTEM
	SOUL_SCROLL = 9,
#endif
};

bool CHARACTER::DoRefineWithScroll(LPITEM item)
{
	if (!CanHandleItem(true))
	{
		ClearRefineMode();
		return false;
	}

	ClearRefineMode();

	//���� �ð����� : upgrade_refine_scroll.quest ���� ������ 5���̳��� �Ϲ� ������
	//�����Ҽ� ����
	if (quest::CQuestManager::instance().GetEventFlag("update_refine_time") != 0)
	{
		if (get_global_time() < quest::CQuestManager::instance().GetEventFlag("update_refine_time") + (60 * 5))
		{
			sys_log(0, "can't refine %d %s", GetPlayerID(), GetName());
			return false;
		}
	}

	const TRefineTable * prt = CRefineManager::instance().GetRefineRecipe(item->GetRefineSet());

	if (!prt)
		return false;

	LPITEM pkItemScroll;

	// ������ üũ
	if (m_iRefineAdditionalCell < 0)
		return false;

	pkItemScroll = GetInventoryItem(m_iRefineAdditionalCell);

	if (!pkItemScroll)
		return false;

	if (!(pkItemScroll->GetType() == ITEM_USE && pkItemScroll->GetSubType() == USE_TUNING))
		return false;

	if (pkItemScroll->GetVnum() == item->GetVnum())
		return false;

	DWORD result_vnum = item->GetRefinedVnum();
	DWORD result_fail_vnum = item->GetRefineFromVnum();

	if (result_vnum == 0)
	{
#ifdef TEXTS_IMPROVEMENT
		ChatPacketNew(CHAT_TYPE_INFO, 305, "");
#endif
		return false;
	}

	// MUSIN_SCROLL
	if (pkItemScroll->GetValue(0) == MUSIN_SCROLL)
	{
		if (item->GetRefineLevel() >= 4)
		{
#ifdef TEXTS_IMPROVEMENT
			ChatPacketNew(CHAT_TYPE_INFO, 305, "");
#endif
			return false;
		}
	}
	// END_OF_MUSIC_SCROLL

	else if (pkItemScroll->GetValue(0) == MEMO_SCROLL)
	{
		if (item->GetRefineLevel() != pkItemScroll->GetValue(1))
		{
#ifdef TEXTS_IMPROVEMENT
			ChatPacketNew(CHAT_TYPE_INFO, 417, "%s#%s", item->GetName(), pkItemScroll->GetName());
#endif
			return false;
		}
	}
	else if (pkItemScroll->GetValue(0) == BDRAGON_SCROLL)
	{
		if (item->GetType() != ITEM_METIN || item->GetRefineLevel() != 4)
		{
#ifdef TEXTS_IMPROVEMENT
			ChatPacketNew(CHAT_TYPE_INFO, 665, "%s#%s", item->GetName(), pkItemScroll->GetName());
#endif
			return false;
		}
	}

	TItemTable * pProto = ITEM_MANAGER::instance().GetTable(item->GetRefinedVnum());

	if (!pProto)
	{
#ifdef TEXTS_IMPROVEMENT
		ChatPacketNew(CHAT_TYPE_INFO, 427, "");
#endif
		return false;
	}

	if (GetGold() < prt->cost)
	{
#ifdef TEXTS_IMPROVEMENT
		ChatPacketNew(CHAT_TYPE_INFO, 232, "");
#endif
		return false;
	}

	for (int i = 0; i < prt->material_count; ++i)
	{
		if (CountSpecifyItem(prt->materials[i].vnum) < prt->materials[i].count)
		{
#ifdef TEXTS_IMPROVEMENT
			ChatPacketNew(CHAT_TYPE_INFO, 233, "");
#endif
			return false;
		}
	}

	for (int i = 0; i < prt->material_count; ++i)
		RemoveSpecifyItem(prt->materials[i].vnum, prt->materials[i].count);

	int prob = number(1, 100);
	int success_prob = prt->prob;
	bool bDestroyWhenFail = false;

	const char* szRefineType = "SCROLL";

	if (pkItemScroll->GetValue(0) == HYUNIRON_CHN ||
		pkItemScroll->GetValue(0) == YONGSIN_SCROLL ||
		pkItemScroll->GetValue(0) == YAGONG_SCROLL) // ��ö, ����� �ູ��, �߰��� ������  ó��
	{
		const char hyuniron_prob[9] = { 100, 75, 65, 55, 45, 40, 35, 25, 20 };
		const char yagong_prob[9] = { 100, 100, 90, 80, 70, 60, 50, 30, 20 };

		if (pkItemScroll->GetValue(0) == YONGSIN_SCROLL)
		{
			success_prob = hyuniron_prob[MINMAX(0, item->GetRefineLevel(), 8)];
		}
		else if (pkItemScroll->GetValue(0) == YAGONG_SCROLL)
		{
			success_prob = yagong_prob[MINMAX(0, item->GetRefineLevel(), 8)];
		}
		else if (pkItemScroll->GetValue(0) == HYUNIRON_CHN) {} // @fixme121
		else
		{
			sys_err("REFINE : Unknown refine scroll item. Value0: %d", pkItemScroll->GetValue(0));
		}

		if (pkItemScroll->GetValue(0) == HYUNIRON_CHN) // ��ö�� �������� �μ����� �Ѵ�.
			bDestroyWhenFail = true;

		// DETAIL_REFINE_LOG
		if (pkItemScroll->GetValue(0) == HYUNIRON_CHN)
		{
			szRefineType = "HYUNIRON";
		}
		else if (pkItemScroll->GetValue(0) == YONGSIN_SCROLL)
		{
			szRefineType = "GOD_SCROLL";
		}
		else if (pkItemScroll->GetValue(0) == YAGONG_SCROLL)
		{
			szRefineType = "YAGONG_SCROLL";
		}
		// END_OF_DETAIL_REFINE_LOG
	}

	// DETAIL_REFINE_LOG
	if (pkItemScroll->GetValue(0) == MUSIN_SCROLL) // ������ �ູ���� 100% ���� (+4������)
	{
		success_prob = 100;

		szRefineType = "MUSIN_SCROLL";
	}
	// END_OF_DETAIL_REFINE_LOG
	else if (pkItemScroll->GetValue(0) == MEMO_SCROLL)
	{
		success_prob = 100;
		szRefineType = "MEMO_SCROLL";
	}
	else if (pkItemScroll->GetValue(0) == BDRAGON_SCROLL)
	{
		success_prob = 80;
		szRefineType = "BDRAGON_SCROLL";
	}

#ifdef ENABLE_FEATURES_REFINE_SYSTEM	
	success_prob += CRefineManager::instance().Result(this);
#endif
	pkItemScroll->SetCount(pkItemScroll->GetCount() - 1);

	if (prob <= success_prob)
	{
		// ����! ��� �������� �������, ���� �Ӽ��� �ٸ� ������ ȹ��
		LPITEM pkNewItem = ITEM_MANAGER::instance().CreateItem(result_vnum, 1, 0, false);

		if (pkNewItem)
		{
			ITEM_MANAGER::CopyAllAttrTo(item, pkNewItem);
			LogManager::instance().ItemLog(this, pkNewItem, "REFINE SUCCESS", pkNewItem->GetName());

			BYTE bCell = item->GetCell();
			

#ifdef ENABLE_BATTLE_PASS
			BYTE bBattlePassId = GetBattlePassId();
			if(bBattlePassId)
			{
				DWORD dwItemVnum, dwCount;
				if(CBattlePass::instance().BattlePassMissionGetInfo(bBattlePassId, REFINE_ITEM, &dwItemVnum, &dwCount))
				{
					if(dwItemVnum == item->GetVnum() && GetMissionProgress(REFINE_ITEM, bBattlePassId) < dwCount)
						UpdateMissionProgress(REFINE_ITEM, bBattlePassId, 1, dwCount);
				}
			}
#endif

			NotifyRefineSuccess(this, item, szRefineType);
			DBManager::instance().SendMoneyLog(MONEY_LOG_REFINE, item->GetVnum(), -prt->cost);
			ITEM_MANAGER::instance().RemoveItem(item, "REMOVE (REFINE SUCCESS)");

			pkNewItem->AddToCharacter(this, TItemPos(INVENTORY, bCell));
			ITEM_MANAGER::instance().FlushDelayedSave(pkNewItem);
			pkNewItem->AttrLog();
			//PointChange(POINT_GOLD, -prt->cost);
#ifdef ENABLE_FEATURES_REFINE_SYSTEM
			CRefineManager::instance().Reset(this);
#endif
			PayRefineFee(prt->cost);
		}
		else
		{
			// ������ ������ ���� -> ���� ���з� ����
			sys_err("cannot create item %u", result_vnum);
			NotifyRefineFail(this, item, szRefineType);
		}
	}
	else if (!bDestroyWhenFail && result_fail_vnum)
	{
		// ����! ��� �������� �������, ���� �Ӽ��� ���� ����� ������ ȹ��
		LPITEM pkNewItem = ITEM_MANAGER::instance().CreateItem(result_fail_vnum, 1, 0, false);

		if (pkNewItem)
		{
			ITEM_MANAGER::CopyAllAttrTo(item, pkNewItem);
			LogManager::instance().ItemLog(this, pkNewItem, "REFINE FAIL", pkNewItem->GetName());

			BYTE bCell = item->GetCell();
			

#ifdef ENABLE_BATTLE_PASS
			BYTE bBattlePassId = GetBattlePassId();
			if(bBattlePassId)
			{
				DWORD dwItemVnum, dwCount;
				if(CBattlePass::instance().BattlePassMissionGetInfo(bBattlePassId, REFINE_ITEM, &dwItemVnum, &dwCount))
				{
					if(dwItemVnum == item->GetVnum() && GetMissionProgress(REFINE_ITEM, bBattlePassId) < dwCount)
						UpdateMissionProgress(REFINE_ITEM, bBattlePassId, 1, dwCount);
				}
			}
#endif

			DBManager::instance().SendMoneyLog(MONEY_LOG_REFINE, item->GetVnum(), -prt->cost);
			NotifyRefineFail(this, item, szRefineType, -1);
			ITEM_MANAGER::instance().RemoveItem(item, "REMOVE (REFINE FAIL)");

			pkNewItem->AddToCharacter(this, TItemPos(INVENTORY, bCell));
			ITEM_MANAGER::instance().FlushDelayedSave(pkNewItem);

			pkNewItem->AttrLog();

			//PointChange(POINT_GOLD, -prt->cost);
#ifdef ENABLE_FEATURES_REFINE_SYSTEM
			CRefineManager::instance().Reset(this);
#endif
			PayRefineFee(prt->cost);
		}
		else
		{
			// ������ ������ ���� -> ���� ���з� ����
			sys_err("cannot create item %u", result_fail_vnum);
			NotifyRefineFail(this, item, szRefineType);
		}
	}
	else
	{
		NotifyRefineFail(this, item, szRefineType); // ������ ������ ������� ����

#ifdef ENABLE_FEATURES_REFINE_SYSTEM
		CRefineManager::instance().Reset(this);
#endif
		PayRefineFee(prt->cost);
	}

	return true;
}

#ifdef ENABLE_SOUL_SYSTEM
bool CHARACTER::DoRefineItemSoul(LPITEM item)
{
	if (!CanHandleItem(true))
	{
		ClearRefineMode();
		return false;
	}

	ClearRefineMode();

	LPITEM pkItemScroll;

	if (m_iRefineAdditionalCell < 0)
		return false;

	pkItemScroll = GetInventoryItem(m_iRefineAdditionalCell);

	if (!pkItemScroll)
		return false;

	if (!(pkItemScroll->GetType() == ITEM_USE && pkItemScroll->GetSubType() == USE_TUNING))
		return false;

	if (pkItemScroll->GetVnum() == item->GetVnum())
		return false;

	DWORD resultVnum = item->GetRefinedVnum();

	if (resultVnum == 0)
	{
#ifdef TEXTS_IMPROVEMENT
		ChatPacketNew(CHAT_TYPE_INFO, 666, "%s", item->GetName());
#endif
		return false;
	}

	TItemTable * pProto = ITEM_MANAGER::instance().GetTable(item->GetRefinedVnum());

	if (!pProto)
	{
		sys_err("DoRefineWithScroll NOT GET ITEM PROTO %d", item->GetRefinedVnum());
#ifdef TEXTS_IMPROVEMENT
		ChatPacketNew(CHAT_TYPE_INFO, 305, "");
#endif
		return false;
	}

	int prob = number(1, 100);
	int successProb = pkItemScroll->GetValue(1);

	pkItemScroll->SetCount(pkItemScroll->GetCount() - 1);

	if (prob <= successProb)
	{
		LPITEM pkNewItem = ITEM_MANAGER::instance().CreateItem(resultVnum, 1, 0, false);
		if (pkNewItem)
		{
			BYTE bCell = item->GetCell();
			ChatPacket(CHAT_TYPE_COMMAND, "RefineSoulSuceeded");
			ITEM_MANAGER::instance().RemoveItem(item, "REMOVE (REFINE SUCCESS)");

			pkNewItem->AddToCharacter(this, TItemPos(INVENTORY, bCell));
			ITEM_MANAGER::instance().FlushDelayedSave(pkNewItem);
		}
		else
		{
			sys_err("Cannot create item soul %u", resultVnum);
			ChatPacket(CHAT_TYPE_COMMAND, "RefineSoulFailed");
		}
	}
	else
	{
		ChatPacket(CHAT_TYPE_COMMAND, "RefineSoulFailed");
	}

	return true;
}
#endif

bool CHARACTER::RefineInformation(BYTE bCell, BYTE bType, int iAdditionalCell)
{
	if (bCell > INVENTORY_MAX_NUM)
		return false;

	LPITEM item = GetInventoryItem(bCell);

	if (!item)
		return false;

#ifdef ATTR_LOCK
	if (item->GetLockedAttr() != -1)
	{
#ifdef TEXTS_IMPROVEMENT
		ChatPacketNew(CHAT_TYPE_INFO, 784, "");
#endif
		return false;
	}
#endif

	// REFINE_COST
	if (bType == REFINE_TYPE_MONEY_ONLY && !GetQuestFlag("deviltower_zone.can_refine"))
	{
#ifdef TEXTS_IMPROVEMENT
		ChatPacketNew(CHAT_TYPE_INFO, 361, "");
#endif
		return false;
	}
	// END_OF_REFINE_COST

	TPacketGCRefineInformation p;

	p.header = HEADER_GC_REFINE_INFORMATION;
	p.pos = bCell;
	p.src_vnum = item->GetVnum();
	p.result_vnum = item->GetRefinedVnum();
	p.type = bType;

	if (p.result_vnum == 0)
	{
#ifdef TEXTS_IMPROVEMENT
		ChatPacketNew(CHAT_TYPE_INFO, 427, "");
#endif
		return false;
	}

	if (item->GetType() == ITEM_USE && item->GetSubType() == USE_TUNING)
	{
		if (bType == 0)
		{
#ifdef TEXTS_IMPROVEMENT
			ChatPacketNew(CHAT_TYPE_INFO, 424, "");
#endif
			return false;
		}
		else
		{
			LPITEM itemScroll = GetInventoryItem(iAdditionalCell);
			if (!itemScroll || item->GetVnum() == itemScroll->GetVnum())
			{
#ifdef TEXTS_IMPROVEMENT
				ChatPacketNew(CHAT_TYPE_INFO, 229, "");
#endif
				return false;
			}
		}
	}

#ifdef ENABLE_SOUL_SYSTEM
	if (bType == REFINE_TYPE_SOUL)
	{
		LPITEM itemScroll = GetInventoryItem(iAdditionalCell);
		if (!itemScroll)
			return false;
		
		p.cost = 0;
		p.prob = itemScroll->GetValue(1);
		p.material_count = 0;
		memset(p.materials, 0, sizeof(p.materials));
		
		GetDesc()->Packet(&p, sizeof(TPacketGCRefineInformation));
	
		SetRefineMode(iAdditionalCell);
		return true;
	}
#endif

	CRefineManager & rm = CRefineManager::instance();

	const TRefineTable* prt = rm.GetRefineRecipe(item->GetRefineSet());

	if (!prt)
	{
#ifdef TEXTS_IMPROVEMENT
		ChatPacketNew(CHAT_TYPE_INFO, 427, "");
#endif
		return false;
	}

	p.cost = ComputeRefineFee(prt->cost);
	p.prob = prt->prob;
	if (bType == REFINE_TYPE_MONEY_ONLY)
	{
		p.material_count = 0;
		memset(p.materials, 0, sizeof(p.materials));
	}
	else
	{
		p.material_count = prt->material_count;
		thecore_memcpy(&p.materials, prt->materials, sizeof(prt->materials));
	}

	GetDesc()->Packet(&p, sizeof(TPacketGCRefineInformation));

	SetRefineMode(iAdditionalCell);
	return true;
}

bool CHARACTER::RefineItem(LPITEM pkItem, LPITEM pkTarget)
{
	if (!CanHandleItem())
		return false;

#ifdef ENABLE_SOUL_SYSTEM
	DWORD vnum = pkItem->GetVnum();
	if ((vnum == 70602 || vnum == 70603 || vnum == 88958) && pkTarget->GetType() != ITEM_SOUL) {
#ifdef TEXTS_IMPROVEMENT
		ChatPacketNew(CHAT_TYPE_INFO, 1294, "%s", pkItem->GetName());
#endif
		return false;
	}
#endif

	if (pkItem->GetSubType() == USE_TUNING)
	{
		// XXX ����, ���� �������� ��������ϴ�...
		// XXX ���ɰ������� �ູ�� ���� �Ǿ���!
		// MUSIN_SCROLL
		if (pkItem->GetValue(0) == MUSIN_SCROLL)
			RefineInformation(pkTarget->GetCell(), REFINE_TYPE_MUSIN, pkItem->GetCell());
		// END_OF_MUSIN_SCROLL

#ifdef ENABLE_SOUL_SYSTEM
		else if (pkItem->GetValue(0) == SOUL_SCROLL)
			RefineInformation(pkTarget->GetCell(), REFINE_TYPE_SOUL, pkItem->GetCell());
#endif

		else if (pkItem->GetValue(0) == HYUNIRON_CHN)
			RefineInformation(pkTarget->GetCell(), REFINE_TYPE_HYUNIRON, pkItem->GetCell());
		else if (pkItem->GetValue(0) == BDRAGON_SCROLL)
		{
			if (pkTarget->GetRefineSet() != 702) return false;
			RefineInformation(pkTarget->GetCell(), REFINE_TYPE_BDRAGON, pkItem->GetCell());
		}
		else
		{
			if (pkTarget->GetRefineSet() == 501) return false;
			RefineInformation(pkTarget->GetCell(), REFINE_TYPE_SCROLL, pkItem->GetCell());
		}
	}
	else if (pkItem->GetSubType() == USE_DETACHMENT && IS_SET(pkTarget->GetFlag(), ITEM_FLAG_REFINEABLE))
	{
		LogManager::instance().ItemLog(this, pkTarget, "USE_DETACHMENT", pkTarget->GetName());

		bool bHasMetinStone = false;

		for (int i = 0; i < ITEM_SOCKET_MAX_NUM; i++)
		{
			long socket = pkTarget->GetSocket(i);
			if (socket > 2 && socket != ITEM_BROKEN_METIN_VNUM)
			{
				bHasMetinStone = true;
				break;
			}
		}

		if (bHasMetinStone)
		{
			for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
			{
				long socket = pkTarget->GetSocket(i);
				if (socket > 2 && socket != ITEM_BROKEN_METIN_VNUM)
				{
					AutoGiveItem(socket);
					//TItemTable* pTable = ITEM_MANAGER::instance().GetTable(pkTarget->GetSocket(i));
					//pkTarget->SetSocket(i, pTable->alValues[2]);
					// �������� ��ü���ش�
					pkTarget->SetSocket(i, ITEM_BROKEN_METIN_VNUM);
				}
			}
			pkItem->SetCount(pkItem->GetCount() - 1);
			return true;
		}
		else
		{
#ifdef TEXTS_IMPROVEMENT
			ChatPacketNew(CHAT_TYPE_INFO, 360, "");
#endif
			return false;
		}
	}

	return false;
}

EVENTFUNC(kill_campfire_event)
{
	char_event_info* info = dynamic_cast<char_event_info*>( event->info );

	if ( info == NULL )
	{
		sys_err( "kill_campfire_event> <Factor> Null pointer" );
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

bool CHARACTER::GiveRecallItem(LPITEM item)
{
	int idx = GetMapIndex();
	int iEmpireByMapIndex = -1;

	if (idx < 20)
		iEmpireByMapIndex = 1;
	else if (idx < 40)
		iEmpireByMapIndex = 2;
	else if (idx < 60)
		iEmpireByMapIndex = 3;
	else if (idx < 10000)
		iEmpireByMapIndex = 0;

	switch (idx)
	{
		case 66:
		case 216:
			iEmpireByMapIndex = -1;
			break;
	}

	if (iEmpireByMapIndex && GetEmpire() != iEmpireByMapIndex)
	{
#ifdef TEXTS_IMPROVEMENT
		ChatPacketNew(CHAT_TYPE_INFO, 270, "");
#endif
		return false;
	}

	int pos;

	if (item->GetCount() == 1)	// �������� �ϳ���� �׳� ����.
	{
		item->SetSocket(0, GetX());
		item->SetSocket(1, GetY());
	}
	else if ((pos = GetEmptyInventory(item->GetSize())) != -1) // �׷��� �ʴٸ� �ٸ� �κ��丮 ������ ã�´�.
	{
		LPITEM item2 = ITEM_MANAGER::instance().CreateItem(item->GetVnum(), 1);

		if (NULL != item2)
		{
			item2->SetSocket(0, GetX());
			item2->SetSocket(1, GetY());
			item2->AddToCharacter(this, TItemPos(INVENTORY, pos));

			item->SetCount(item->GetCount() - 1);
		}
	}
	else
	{
#ifdef TEXTS_IMPROVEMENT
		ChatPacketNew(CHAT_TYPE_INFO, 366, "");
#endif
		return false;
	}

	return true;
}

void CHARACTER::ProcessRecallItem(LPITEM item)
{
	int idx;

	if ((idx = SECTREE_MANAGER::instance().GetMapIndex(item->GetSocket(0), item->GetSocket(1))) == 0)
		return;

	int iEmpireByMapIndex = -1;

	if (idx < 20)
		iEmpireByMapIndex = 1;
	else if (idx < 40)
		iEmpireByMapIndex = 2;
	else if (idx < 60)
		iEmpireByMapIndex = 3;
	else if (idx < 10000)
		iEmpireByMapIndex = 0;

	switch (idx)
	{
		case 66:
		case 216:
			iEmpireByMapIndex = -1;
			break;
		// �Ƿ決�� �϶�
		case 301:
		case 302:
		case 303:
		case 304:
			if( GetLevel() < 90 )
			{
#ifdef TEXTS_IMPROVEMENT
				ChatPacketNew(CHAT_TYPE_INFO, 325, "%d", 90);
#endif
				return;
			}
			else
				break;
	}

	if (iEmpireByMapIndex && GetEmpire() != iEmpireByMapIndex)
	{
#ifdef TEXTS_IMPROVEMENT
		ChatPacketNew(CHAT_TYPE_INFO, 270, "");
#endif
		item->SetSocket(0, 0);
		item->SetSocket(1, 0);
	}
	else
	{
		sys_log(1, "Recall: %s %d %d -> %d %d", GetName(), GetX(), GetY(), item->GetSocket(0), item->GetSocket(1));
		WarpSet(item->GetSocket(0), item->GetSocket(1));
		item->SetCount(item->GetCount() - 1);
	}
}

void CHARACTER::__OpenPrivateShop(
#ifdef KASMIR_PAKET_SYSTEM
bool bKasmir
#endif
)
{
#ifdef ENABLE_OPEN_SHOP_WITH_ARMOR
#ifdef KASMIR_PAKET_SYSTEM
	if (bKasmir) {
		ChatPacket(CHAT_TYPE_COMMAND, "OpenPrivateShopKasmir");
		return;
	}
#endif
	ChatPacket(CHAT_TYPE_COMMAND, "OpenPrivateShop");
#else
	unsigned bodyPart = GetPart(PART_MAIN);
	switch (bodyPart)
	{
		case 0:
		case 1:
		case 2: {
#ifdef KASMIR_PAKET_SYSTEM
				if (bKasmir) {
					ChatPacket(CHAT_TYPE_COMMAND, "OpenPrivateShopKasmir");
					break;
				}
#endif
				
				ChatPacket(CHAT_TYPE_COMMAND, "OpenPrivateShop");
			}
			break;
		default:
#ifdef TEXTS_IMPROVEMENT
			ChatPacketNew(CHAT_TYPE_INFO, 503, "");
#endif
			break;
	}
#endif
}

// MYSHOP_PRICE_LIST
#ifdef ENABLE_LONG_LONG
void CHARACTER::SendMyShopPriceListCmd(DWORD dwItemVnum, long long dwItemPrice)
{
	char szLine[256];
	snprintf(szLine, sizeof(szLine), "MyShopPriceList %u %lld", dwItemVnum, dwItemPrice);
	ChatPacket(CHAT_TYPE_COMMAND, szLine);
	sys_log(0, szLine);
}
#else
void CHARACTER::SendMyShopPriceListCmd(DWORD dwItemVnum, DWORD dwItemPrice)
{
	char szLine[256];
	snprintf(szLine, sizeof(szLine), "MyShopPriceList %u %u", dwItemVnum, dwItemPrice);
	ChatPacket(CHAT_TYPE_COMMAND, szLine);
	sys_log(0, szLine);
}
#endif

//
// DB ĳ�÷� ���� ���� ����Ʈ�� User ���� �����ϰ� ������ ����� Ŀ�ǵ带 ������.
//
void CHARACTER::UseSilkBotaryReal(const TPacketMyshopPricelistHeader* p)
{
	const TItemPriceInfo* pInfo = (const TItemPriceInfo*)(p + 1);

	if (!p->byCount)
		// ���� ����Ʈ�� ����. dummy �����͸� ���� Ŀ�ǵ带 �����ش�.
		SendMyShopPriceListCmd(1, 0);
	else {
		for (int idx = 0; idx < p->byCount; idx++)
			SendMyShopPriceListCmd(pInfo[ idx ].dwVnum, pInfo[ idx ].dwPrice);
	}
	
#ifdef KASMIR_PAKET_SYSTEM
	__OpenPrivateShop(m_bKasmirPaketDurum);
#else
	__OpenPrivateShop();
#endif
}

//
// �̹� ���� �� ó�� ������ Open �ϴ� ��� ����Ʈ�� Load �ϱ� ���� DB ĳ�ÿ� �������� ����Ʈ ��û ��Ŷ�� ������.
// ���ĺ��ʹ� �ٷ� ������ ����� ������ ������.
//
void CHARACTER::UseSilkBotary(void)
{
	if (m_bNoOpenedShop) {
		DWORD dwPlayerID = GetPlayerID();
		db_clientdesc->DBPacket(HEADER_GD_MYSHOP_PRICELIST_REQ, GetDesc()->GetHandle(), &dwPlayerID, sizeof(DWORD));
		m_bNoOpenedShop = false;
	} else {
#ifdef KASMIR_PAKET_SYSTEM
		__OpenPrivateShop(m_bKasmirPaketDurum);
#else
		__OpenPrivateShop();
#endif
	}
}
// END_OF_MYSHOP_PRICE_LIST


int CalculateConsume(LPCHARACTER ch)
{
	static const int WARP_NEED_LIFE_PERCENT	= 30;
	static const int WARP_MIN_LIFE_PERCENT	= 10;
	// CONSUME_LIFE_WHEN_USE_WARP_ITEM
	int consumeLife = 0;
	{
		// CheckNeedLifeForWarp
		const int curLife		= ch->GetHP();
		const int needPercent	= WARP_NEED_LIFE_PERCENT;
		const int needLife = ch->GetMaxHP() * needPercent / 100;
		if (curLife < needLife)
		{
#ifdef TEXTS_IMPROVEMENT
			if (ch) {
				ch->ChatPacketNew(CHAT_TYPE_INFO, 284, "");
			}
#endif
			return -1;
		}

		consumeLife = needLife;


		// CheckMinLifeForWarp: ���� ���ؼ� ������ �ȵǹǷ� ����� �ּҷ��� �����ش�
		const int minPercent	= WARP_MIN_LIFE_PERCENT;
		const int minLife	= ch->GetMaxHP() * minPercent / 100;
		if (curLife - needLife < minLife)
			consumeLife = curLife - minLife;

		if (consumeLife < 0)
			consumeLife = 0;
	}
	// END_OF_CONSUME_LIFE_WHEN_USE_WARP_ITEM
	return consumeLife;
}

int CalculateConsumeSP(LPCHARACTER lpChar)
{
	static const int NEED_WARP_SP_PERCENT = 30;

	const int curSP = lpChar->GetSP();
	const int needSP = lpChar->GetMaxSP() * NEED_WARP_SP_PERCENT / 100;

	if (curSP < needSP)
	{
#ifdef TEXTS_IMPROVEMENT
		if (lpChar) {
			lpChar->ChatPacketNew(CHAT_TYPE_INFO, 287, "");
		}
#endif
		return -1;
	}

	return needSP;
}

// #define ENABLE_FIREWORK_STUN
#define ENABLE_ADDSTONE_FAILURE
bool CHARACTER::UseItemEx(LPITEM item, TItemPos DestCell)
{
	int iLimitRealtimeStartFirstUseFlagIndex = -1;
	//int iLimitTimerBasedOnWearFlagIndex = -1;

	WORD wDestCell = DestCell.cell;
	BYTE bDestInven = DestCell.window_type;
	for (int i = 0; i < ITEM_LIMIT_MAX_NUM; ++i)
	{
		long limitValue = item->GetProto()->aLimits[i].lValue;

		switch (item->GetProto()->aLimits[i].bType)
		{
			case LIMIT_LEVEL:
				if (GetLevel() < limitValue)
				{
#ifdef TEXTS_IMPROVEMENT
					ChatPacketNew(CHAT_TYPE_INFO, 325, "%d", limitValue);
#endif
					return false;
				}
				break;

			case LIMIT_REAL_TIME_START_FIRST_USE:
				iLimitRealtimeStartFirstUseFlagIndex = i;
				break;

			case LIMIT_TIMER_BASED_ON_WEAR:
				//iLimitTimerBasedOnWearFlagIndex = i;
				break;
		}
	}

	if (test_server)
	{
		sys_log(0, "USE_ITEM %s, Inven %d, Cell %d, ItemType %d, SubType %d", item->GetName(), bDestInven, wDestCell, item->GetType(), item->GetSubType());
	}

	if ( CArenaManager::instance().IsLimitedItem( GetMapIndex(), item->GetVnum() ) == true )
	{
#ifdef TEXTS_IMPROVEMENT
		ChatPacketNew(CHAT_TYPE_INFO, 667, "");
#endif
		return false;
	}
#ifdef ENABLE_NEWSTUFF
	else if (g_NoPotionsOnPVP && CPVPManager::instance().IsFighting(GetPlayerID()) && IsLimitedPotionOnPVP(item->GetVnum()))
	{
#ifdef TEXTS_IMPROVEMENT
		ChatPacketNew(CHAT_TYPE_INFO, 667, "");
#endif
		return false;
	}
#endif

	// @fixme402 (IsLoadedAffect to block affect hacking)
	if (!IsLoadedAffect()) {
		return false;
	}

	// @fixme141 BEGIN
	if (TItemPos(item->GetWindow(), item->GetCell()).IsBeltInventoryPosition())
	{
		LPITEM beltItem = GetWear(WEAR_BELT);

		if (NULL == beltItem)
		{
#ifdef TEXTS_IMPROVEMENT
			ChatPacketNew(CHAT_TYPE_INFO, 785, "");
#endif
			return false;
		}

		if (false == CBeltInventoryHelper::IsAvailableCell(item->GetCell() - BELT_INVENTORY_SLOT_START, beltItem->GetValue(0)))
		{
#ifdef TEXTS_IMPROVEMENT
			ChatPacketNew(CHAT_TYPE_INFO, 786, "");
#endif
			return false;
		}
	}
	// @fixme141 END

	// ������ ���� ��� ���ĺ��ʹ� ������� �ʾƵ� �ð��� �����Ǵ� ��� ó��.
	if (-1 != iLimitRealtimeStartFirstUseFlagIndex)
	{
		// �� ���̶� ����� ���������� ���δ� Socket1�� ���� �Ǵ��Ѵ�. (Socket1�� ���Ƚ�� ���)
		if (0 == item->GetSocket(1))
		{
			// ��밡�ɽð��� Default ������ Limit Value ���� ����ϵ�, Socket0�� ���� ������ �� ���� ����ϵ��� �Ѵ�. (������ ��)
			long duration = (0 != item->GetSocket(0)) ? item->GetSocket(0) : item->GetProto()->aLimits[iLimitRealtimeStartFirstUseFlagIndex].lValue;

			if (0 == duration)
				duration = 60 * 60 * 24 * 7;

			item->SetSocket(0, time(0) + duration);
			item->StartRealTimeExpireEvent();
		}

		if (false == item->IsEquipped())
			item->SetSocket(1, item->GetSocket(1) + 1);
	}

#ifdef __NEWPET_SYSTEM__
	if (item->GetVnum() == 55001) 
	{

		LPITEM item2;

		if (!IsValidItemPosition(DestCell) || !(item2 = GetItem(DestCell)))
			return false;

		if (item2->IsExchanging() || item2->IsEquipped()) // ENABLE_BUG_FIXES
			return false;

		if (item2->GetVnum() > 55711 || item2->GetVnum() < 55701)
			return false;

		
		char szQuery1[1024];
		snprintf(szQuery1, sizeof(szQuery1), "SELECT duration FROM new_petsystem WHERE id = %d LIMIT 1", item2->GetID());
		std::unique_ptr<SQLMsg> pmsg2(DBManager::instance().DirectQuery(szQuery1));
		if (pmsg2->Get()->uiNumRows > 0) {
			MYSQL_ROW row = mysql_fetch_row(pmsg2->Get()->pSQLResult);
			if (atoi(row[0]) > 0) {
				if (GetNewPetSystem()->IsActivePet()) {
#ifdef TEXTS_IMPROVEMENT
					ChatPacketNew(CHAT_TYPE_INFO, 787, "");
#endif
					return false;
				}

				std::unique_ptr<SQLMsg> msg(DBManager::instance().DirectQuery("UPDATE new_petsystem SET duration =(tduration) WHERE id = %d", item2->GetID()));
#ifdef TEXTS_IMPROVEMENT
				ChatPacketNew(CHAT_TYPE_INFO, 788, "");
#endif
			}
			else {
				std::unique_ptr<SQLMsg> msg(DBManager::instance().DirectQuery("UPDATE new_petsystem SET duration =(tduration/2) WHERE id = %d", item2->GetID()));
#ifdef TEXTS_IMPROVEMENT
				ChatPacketNew(CHAT_TYPE_INFO, 788, "");
#endif
			}
			item->SetCount(item->GetCount() - 1);
			return true;
		}
		else
			return false;
	}

	if (item->GetVnum() >= 55701 && item->GetVnum() <= 55711) {
		LPITEM box = GetItem(DestCell);
		if (box) {
			if (item->GetSocket(1) == 0) {
#ifdef TEXTS_IMPROVEMENT
				ChatPacketNew(CHAT_TYPE_INFO, 858, "");
#endif
				return false;
			}

			if (box->GetSocket(0) != 0) {
#ifdef TEXTS_IMPROVEMENT
				ChatPacketNew(CHAT_TYPE_INFO, 853, "%s", box->GetName());
#endif
				return false;
			} else {
				if (item->GetSocket(0) == true) {
#ifdef TEXTS_IMPROVEMENT
					ChatPacketNew(CHAT_TYPE_INFO, 854, "");
#endif
					return false;
				} else {
					char query[1024];
					snprintf(query, sizeof(query), "SELECT level"
#ifdef ENABLE_NEW_PET_EDITS
					", minAge "
#endif
					", evolution, bonus0, bonus1, bonus2, skill0, skill0lv, skill1, skill1lv, skill2, skill2lv, skill3, skill3lv FROM player.new_petsystem WHERE id = %d", item->GetID());
					std::unique_ptr<SQLMsg> pmsg(DBManager::instance().DirectQuery(query));
					if (pmsg->Get()->uiNumRows > 0) 
					{
						MYSQL_ROW row = mysql_fetch_row(pmsg->Get()->pSQLResult);
						DWORD evolution = atoi(row[2]);
						DWORD petVnum = 0;
						switch (item->GetVnum()) {
							case 55701:
								petVnum = evolution == 3 ? 34042 : 34041;
								break;
							case 55702:
								petVnum = evolution == 3 ? 34046 : 34045;
								break;
							case 55703:
								petVnum = evolution == 3 ? 34050 : 34049;
								break;
							case 55704:
								petVnum = evolution == 3 ? 34054 : 34053;
								break;
							case 55705:
								petVnum = evolution == 3 ? 34037 : 34036;
								break;
							case 55706:
								petVnum = evolution == 3 ? 34065 : 34064;
								break;
							case 55707:
								petVnum = evolution == 3 ? 34074 : 34073;
								break;
							case 55708:
								petVnum = evolution == 3 ? 34076 : 34075;
								break;
							case 55709:
								petVnum = evolution == 3 ? 34081 : 34080;
								break;
							case 55710:
								petVnum = evolution == 3 ? 34083 : 34082;
								break;
							case 55711:
								petVnum = evolution == 3 ? 34096 : 34095;
								break;
							default:
								break;
						}

						if (petVnum == 0) {
							return false;
						}

						box->SetSocket(1, item->GetID());
						box->SetSocket(0, petVnum);
						ITEM_MANAGER::instance().RemoveItem(item);
#ifdef ENABLE_NEW_PET_EDITS
						box->SetSocket(2, atoi(row[1]));
#endif
						BYTE res1 = atoi(row[0]);
						BYTE res2 = atoi(row[2]);
						BYTE res3 = atoi(row[3]);
						BYTE res4 = atoi(row[4]);
						box->SetForceAttribute(0, res1, res2);
						box->SetForceAttribute(1, res3, res4);
						BYTE dwskill1 = atoi(row[6]) == -1 ? 255 : atoi(row[6]), dwskilllv1 = atoi(row[7]);
						box->SetForceAttribute(2, atoi(row[5]), dwskill1);
						BYTE dwskill2 = atoi(row[8]) == -1 ? 255 : atoi(row[8]), dwskilllv2 = atoi(row[9]);
						box->SetForceAttribute(3, dwskilllv1, dwskill2);
						BYTE dwskill3 = atoi(row[10]) == -1 ? 255 : atoi(row[10]), dwskilllv3 = atoi(row[11]);
						box->SetForceAttribute(4, dwskilllv2, dwskill3);
						BYTE dwskill4 = atoi(row[12]) == -1 ? 255 : atoi(row[12]), dwskilllv4 = atoi(row[13]);
						box->SetForceAttribute(5, dwskilllv3, dwskill4);
						box->SetForceAttribute(6, dwskilllv4, 1);
#ifdef TEXTS_IMPROVEMENT
						ChatPacketNew(CHAT_TYPE_INFO, 855, "%s", box->GetName());
#endif
						return true;
					}
					else {
						return false;
					}
				}
			}
		}
	} else if (item->GetVnum() == 55002) {
		if (item->GetSocket(0) != 0) {
			DWORD itemVnum = 0;
			switch (item->GetSocket(0)) {
				case 34041:
				case 34042:
					itemVnum = 55701;
					break;
				case 34045:
				case 34046:
					itemVnum = 55702;
					break;
				case 34049:
				case 34050:
					itemVnum = 55703;
					break;
				case 34053:
				case 34054:
					itemVnum = 55704;
					break;
				case 34036:
				case 34037:
					itemVnum = 55705;
					break;
				case 34064:
				case 34065:
					itemVnum = 55706;
					break;
				case 34073:
				case 34074:
					itemVnum = 55707;
					break;
				case 34075:
				case 34076:
					itemVnum = 55708;
					break;
				case 34080:
				case 34081:
					itemVnum = 55709;
					break;
				case 34082:
				case 34083:
					itemVnum = 55710;
					break;
				case 34095:
				case 34096:
					itemVnum = 55711;
					break;
				default:
					break;
			}
			
			if (itemVnum == 0) {
				return false;
			}
			
			LPITEM petItem = AutoGiveItem(itemVnum, 1);
			if (!petItem) {
				return false;
			}

			petItem->SetSocket(0, 0);
			petItem->SetForceAttribute(0, 1, item->GetAttributeType(1));
			petItem->SetForceAttribute(1, 1, item->GetAttributeValue(1));
			petItem->SetForceAttribute(2, 1, item->GetAttributeType(2));
			
			char query[256];
			snprintf(query, sizeof(query), "SELECT tduration FROM player.new_petsystem WHERE id = %ld", item->GetSocket(1));
			std::unique_ptr<SQLMsg> pmsg(DBManager::instance().DirectQuery(query));
			if (pmsg->Get()->uiNumRows > 0)  {
				MYSQL_ROW row = mysql_fetch_row(pmsg->Get()->pSQLResult);
#ifdef ENABLE_NEW_PET_EDITS
				petItem->SetSocket(1, atoi(row[0]));
				petItem->SetSocket(2, atoi(row[0]));
#else
				petItem->SetForceAttribute(3, 1, atoi(row[0]));
				petItem->SetForceAttribute(4, 1, atoi(row[0]));
#endif
			}
#ifdef ENABLE_NEW_PET_EDITS
			petItem->SetForceAttribute(3, 1, item->GetAttributeType(0));
#else
			petItem->SetSocket(1, item->GetAttributeType(0));
#endif
			std::unique_ptr<SQLMsg> msg(DBManager::instance().DirectQuery("UPDATE player.new_petsystem SET id = %d WHERE id = %ld", petItem->GetID(), item->GetSocket(1)));
			ITEM_MANAGER::instance().RemoveItem(item);
#ifdef TEXTS_IMPROVEMENT
			ChatPacketNew(CHAT_TYPE_INFO, 857, "%s", item->GetName());
			return true;
#endif
		} else {
#ifdef TEXTS_IMPROVEMENT
			ChatPacketNew(CHAT_TYPE_INFO, 856, "%s", item->GetName());
#endif
			return false;
		}
	}
#endif

	switch (item->GetType())
	{
#ifdef ENABLE_ITEMSHOP_ITEM
		case ITEM_TYPE_ISHOP:
		{
			DWORD vnum = item->GetSocket(0);
			if (vnum == 0) {
				return false;
			}

			LPITEM reward = AutoGiveItem(vnum, 1);
			if (!reward) {
				return false;
			}

			item->SetCount(item->GetCount() - 1);
			return true;
		}
		break;
#endif
		case ITEM_HAIR:
			return ItemProcess_Hair(item, wDestCell);

		case ITEM_POLYMORPH:
			return ItemProcess_Polymorph(item);

		case ITEM_QUEST:
			if (GetArena() != NULL || IsObserverMode() == true)
			{
				if (item->GetVnum() == 50051 || item->GetVnum() == 50052 || item->GetVnum() == 50053)
				{
#ifdef TEXTS_IMPROVEMENT
					ChatPacketNew(CHAT_TYPE_INFO, 667, "");
#endif
					return false;
				}
			}

			if (!IS_SET(item->GetFlag(), ITEM_FLAG_QUEST_USE | ITEM_FLAG_QUEST_USE_MULTIPLE))
			{
				if (item->GetSIGVnum() == 0)
				{
					quest::CQuestManager::instance().UseItem(GetPlayerID(), item, false);
				}
				else
				{
					quest::CQuestManager::instance().SIGUse(GetPlayerID(), item->GetSIGVnum(), item, false);
				}
			}
			break;

		case ITEM_CAMPFIRE:
			{
				float fx, fy;
				GetDeltaByDegree(GetRotation(), 100.0f, &fx, &fy);

				LPSECTREE tree = SECTREE_MANAGER::instance().Get(GetMapIndex(), (long)(GetX()+fx), (long)(GetY()+fy));

				if (!tree)
				{
#ifdef TEXTS_IMPROVEMENT
					ChatPacketNew(CHAT_TYPE_INFO, 344, "");
#endif
					return false;
				}

				if (tree->IsAttr((long)(GetX()+fx), (long)(GetY()+fy), ATTR_WATER))
				{
#ifdef TEXTS_IMPROVEMENT
					ChatPacketNew(CHAT_TYPE_INFO, 346, "");
#endif
					return false;
				}

#ifdef ENABLE_BUG_FIXES
				if (get_global_time() - GetQuestFlag("kamp.spawned") < 60) {
#ifdef TEXTS_IMPROVEMENT
					ChatPacketNew(CHAT_TYPE_INFO, 1246, "");
#endif
					return false;
				} else {
					SetQuestFlag("kamp.spawned", get_global_time());
				}
#endif

				LPCHARACTER campfire = CHARACTER_MANAGER::instance().SpawnMob(fishing::CAMPFIRE_MOB, GetMapIndex(), (long)(GetX()+fx), (long)(GetY()+fy), 0, false, number(0, 359));

				char_event_info* info = AllocEventInfo<char_event_info>();

				info->ch = campfire;

				campfire->m_pkMiningEvent = event_create(kill_campfire_event, info, PASSES_PER_SEC(40));

				item->SetCount(item->GetCount() - 1);
			}
			break;

		case ITEM_UNIQUE:
			{
				switch (item->GetSubType())
				{
					case USE_ABILITY_UP:
						{
							switch (item->GetValue(0))
							{
								case APPLY_MOV_SPEED:
									AddAffect(AFFECT_UNIQUE_ABILITY, POINT_MOV_SPEED, item->GetValue(2), AFF_MOV_SPEED_POTION, item->GetValue(1), 0, true, true);
									break;

								case APPLY_ATT_SPEED:
									AddAffect(AFFECT_UNIQUE_ABILITY, POINT_ATT_SPEED, item->GetValue(2), AFF_ATT_SPEED_POTION, item->GetValue(1), 0, true, true);
									break;

								case APPLY_STR:
									AddAffect(AFFECT_UNIQUE_ABILITY, POINT_ST, item->GetValue(2), 0, item->GetValue(1), 0, true, true);
									break;

								case APPLY_DEX:
									AddAffect(AFFECT_UNIQUE_ABILITY, POINT_DX, item->GetValue(2), 0, item->GetValue(1), 0, true, true);
									break;

								case APPLY_CON:
									AddAffect(AFFECT_UNIQUE_ABILITY, POINT_HT, item->GetValue(2), 0, item->GetValue(1), 0, true, true);
									break;

								case APPLY_INT:
									AddAffect(AFFECT_UNIQUE_ABILITY, POINT_IQ, item->GetValue(2), 0, item->GetValue(1), 0, true, true);
									break;

								case APPLY_CAST_SPEED:
									AddAffect(AFFECT_UNIQUE_ABILITY, POINT_CASTING_SPEED, item->GetValue(2), 0, item->GetValue(1), 0, true, true);
									break;

								case APPLY_RESIST_MAGIC:
									AddAffect(AFFECT_UNIQUE_ABILITY, POINT_RESIST_MAGIC, item->GetValue(2), 0, item->GetValue(1), 0, true, true);
									break;

								case APPLY_ATT_GRADE_BONUS:
									AddAffect(AFFECT_UNIQUE_ABILITY, POINT_ATT_GRADE_BONUS,
											item->GetValue(2), 0, item->GetValue(1), 0, true, true);
									break;

								case APPLY_DEF_GRADE_BONUS:
									AddAffect(AFFECT_UNIQUE_ABILITY, POINT_DEF_GRADE_BONUS,
											item->GetValue(2), 0, item->GetValue(1), 0, true, true);
									break;
							}
						}

						if (GetWarMap())
							GetWarMap()->UsePotion(this, item);

						item->SetCount(item->GetCount() - 1);
						break;

					default:
						{
							if (item->GetSubType() == USE_SPECIAL)
							{
								sys_log(0, "ITEM_UNIQUE: USE_SPECIAL %u", item->GetVnum());

								switch (item->GetVnum())
								{
									case 71049: // ��ܺ�����
#ifdef KASMIR_PAKET_SYSTEM
									case 88901:
#endif
										if (g_bEnableBootaryCheck)
										{
											if (IS_BOTARYABLE_ZONE(GetMapIndex()) == true)
											{
#ifdef KASMIR_PAKET_SYSTEM
												m_bKasmirPaketDurum = item->GetVnum() == 88901 ? true : false;
#endif
												
												UseSilkBotary();
											}
#ifdef TEXTS_IMPROVEMENT
											else {
												ChatPacketNew(CHAT_TYPE_INFO, 668, "");
											}
#endif
										}
										else
										{
#ifdef KASMIR_PAKET_SYSTEM
											m_bKasmirPaketDurum = item->GetVnum() == 88901 ? true : false;
#endif
											
											UseSilkBotary();
										}
										break;
								}
							}
							else
							{
								if (!item->IsEquipped())
									EquipItem(item);
								else
									UnequipItem(item);
							}
						}
						break;
				}
			}
			break;

		case ITEM_COSTUME:
		case ITEM_WEAPON:
		case ITEM_ARMOR:
		case ITEM_ROD:
		case ITEM_RING:		// �ű� ���� ������
		case ITEM_BELT:		// �ű� ��Ʈ ������
			// MINING
		case ITEM_PICK:
			// END_OF_MINING
			if (!item->IsEquipped())
				EquipItem(item);
			else
				UnequipItem(item);
			break;
			// �������� ���� ��ȥ���� ����� �� ����.
			// �������� Ŭ����, ��ȥ���� ���Ͽ� item use ��Ŷ�� ���� �� ����.
			// ��ȥ�� ������ item move ��Ŷ���� �Ѵ�.
			// ������ ��ȥ���� �����Ѵ�.
		case ITEM_DS:
			{
				if (!item->IsEquipped())
					return false;
				return DSManager::instance().PullOut(this, NPOS, item);
			break;
			}
		case ITEM_SPECIAL_DS:
			if (!item->IsEquipped())
				EquipItem(item);
			else
				UnequipItem(item);
			break;

		case ITEM_FISH:
			{
				if (CArenaManager::instance().IsArenaMap(GetMapIndex()) == true)
				{
#ifdef TEXTS_IMPROVEMENT
					ChatPacketNew(CHAT_TYPE_INFO, 667, "");
#endif
					return false;
				}
#ifdef ENABLE_NEWSTUFF
				else if (g_NoPotionsOnPVP && CPVPManager::instance().IsFighting(GetPlayerID()) && !IsAllowedPotionOnPVP(item->GetVnum()))
				{
#ifdef TEXTS_IMPROVEMENT
					ChatPacketNew(CHAT_TYPE_INFO, 667, "");
#endif
					return false;
				}
#endif

				if (item->GetSubType() == FISH_ALIVE)
					fishing::UseFish(this, item);
			}
			break;

		case ITEM_TREASURE_BOX:
			{
				return false;
			}
			break;

		case ITEM_TREASURE_KEY:
			{
				LPITEM item2;

				if (!GetItem(DestCell) || !(item2 = GetItem(DestCell)))
					return false;

				if (item2->IsExchanging() || item2->IsEquipped()) // @fixme114
					return false;

				if (item2->GetType() != ITEM_TREASURE_BOX)
				{
#ifdef TEXTS_IMPROVEMENT
					ChatPacketNew(CHAT_TYPE_INFO, 408, "");
#endif
					return false;
				}

				if (item->GetValue(0) == item2->GetValue(0))
				{
					DWORD dwBoxVnum = item2->GetVnum();
					std::vector <DWORD> dwVnums;
					std::vector <DWORD> dwCounts;
					std::vector <LPITEM> item_gets(0);
					int count = 0;

					if (GiveItemFromSpecialItemGroup(dwBoxVnum, dwVnums, dwCounts, item_gets, count))
					{
						ITEM_MANAGER::instance().RemoveItem(item);
						ITEM_MANAGER::instance().RemoveItem(item2);

						for (int i = 0; i < count; i++){
							switch (dwVnums[i])
							{
								case CSpecialItemGroup::GOLD:
									break;
								case CSpecialItemGroup::EXP:
									break;
								case CSpecialItemGroup::MOB:
#ifdef TEXTS_IMPROVEMENT
									ChatPacketNew(CHAT_TYPE_INFO, 378, "");
#endif
									break;
								case CSpecialItemGroup::SLOW:
#ifdef TEXTS_IMPROVEMENT
									ChatPacketNew(CHAT_TYPE_INFO, 377, "");
#endif
									break;
								case CSpecialItemGroup::DRAIN_HP:
#ifdef TEXTS_IMPROVEMENT
									ChatPacketNew(CHAT_TYPE_INFO, 373, "");
#endif
									break;
								case CSpecialItemGroup::POISON:
#ifdef TEXTS_IMPROVEMENT
									ChatPacketNew(CHAT_TYPE_INFO, 376, "");
#endif
									break;
#ifdef ENABLE_WOLFMAN_CHARACTER
								case CSpecialItemGroup::BLEEDING:
#ifdef TEXTS_IMPROVEMENT
									ChatPacketNew(CHAT_TYPE_INFO, 379, "");
#endif
									break;
#endif
								case CSpecialItemGroup::MOB_GROUP:
#ifdef TEXTS_IMPROVEMENT
									ChatPacketNew(CHAT_TYPE_INFO, 380, "");
#endif
									break;
								default:
//#ifdef TEXTS_IMPROVEMENT
//									if (item_gets[i]) {
//										if (dwCounts[i] > 1) {
//											ChatPacketNew(CHAT_TYPE_INFO, 374, "%d#%s", dwCounts[i], item_gets[i]->GetName());
//										} else {
//											ChatPacketNew(CHAT_TYPE_INFO, 375, "%s", item_gets[i]->GetName());
//										}
//									}
//#endif
									break;
							}
						}
					}
					else
					{
#ifdef TEXTS_IMPROVEMENT
						ChatPacketNew(CHAT_TYPE_INFO, 408, "");
#endif
						return false;
					}
				}
				else
				{
#ifdef TEXTS_IMPROVEMENT
					ChatPacketNew(CHAT_TYPE_INFO, 408, "");
#endif
					return false;
				}
			}
			break;

		case ITEM_GIFTBOX:
			{
#ifdef ENABLE_NEWSTUFF
				if (0 != g_BoxUseTimeLimitValue)
				{
					if (get_dword_time() < m_dwLastBoxUseTime+g_BoxUseTimeLimitValue)
					{
#ifdef TEXTS_IMPROVEMENT
						ChatPacketNew(CHAT_TYPE_INFO, 510, "");
#endif
						return false;
					}
				}

				m_dwLastBoxUseTime = get_dword_time();
#endif
				DWORD dwBoxVnum = item->GetVnum();
#ifdef ENABLE_CHRISTMAS_2021
				if (item->GetValue(0) == 10)
				{
					if (quest::CQuestManager::instance().GetEventFlag("christmas_event") == 0)
					{
#ifdef TEXTS_IMPROVEMENT
						ChatPacketNew(CHAT_TYPE_INFO, 1306, "");
#endif
						return false;
					}
				}
#endif

				std::vector <DWORD> dwVnums;
				std::vector <DWORD> dwCounts;
				std::vector <LPITEM> item_gets(0);
				int count = 0;

				if (GiveItemFromSpecialItemGroup(dwBoxVnum, dwVnums, dwCounts, item_gets, count))
				{
					item->SetCount(item->GetCount()-1);
#ifdef ENABLE_RANKING
					SetRankPoints(17, GetRankPoints(17) + 1);
#endif
					
					for (int i = 0; i < count; i++){
						switch (dwVnums[i])
						{
						case CSpecialItemGroup::GOLD:
							break;
						case CSpecialItemGroup::EXP:
							break;
						case CSpecialItemGroup::MOB:
#ifdef TEXTS_IMPROVEMENT
							ChatPacketNew(CHAT_TYPE_INFO, 378, "");
#endif
							break;
						case CSpecialItemGroup::SLOW:
#ifdef TEXTS_IMPROVEMENT
							ChatPacketNew(CHAT_TYPE_INFO, 377, "");
#endif
							break;
						case CSpecialItemGroup::DRAIN_HP:
#ifdef TEXTS_IMPROVEMENT
							ChatPacketNew(CHAT_TYPE_INFO, 373, "");
#endif
							break;
						case CSpecialItemGroup::POISON:
#ifdef TEXTS_IMPROVEMENT
							ChatPacketNew(CHAT_TYPE_INFO, 376, "");
#endif
							break;
#ifdef ENABLE_WOLFMAN_CHARACTER
						case CSpecialItemGroup::BLEEDING:
#ifdef TEXTS_IMPROVEMENT
							ChatPacketNew(CHAT_TYPE_INFO, 379, "");
#endif
							break;
#endif
						case CSpecialItemGroup::MOB_GROUP:
#ifdef TEXTS_IMPROVEMENT
							ChatPacketNew(CHAT_TYPE_INFO, 380, "");
#endif
							break;
						default:
//#ifdef TEXTS_IMPROVEMENT
//							if (item_gets[i]) {
//								if (dwCounts[i] > 1) {
//									ChatPacketNew(CHAT_TYPE_INFO, 374, "%d#%s", dwCounts[i], item_gets[i]->GetName());
//								} else {
//									ChatPacketNew(CHAT_TYPE_INFO, 375, "%s", item_gets[i]->GetName());
//								}
//							}
//#endif
							break;
						}
					}
				}
				else
				{
#ifdef TEXTS_IMPROVEMENT
					ChatPacketNew(CHAT_TYPE_INFO, 395, "");
#endif
					return false;
				}
			}
			break;

		case ITEM_SKILLFORGET:
			{
				if (!item->GetSocket(0))
				{
					ITEM_MANAGER::instance().RemoveItem(item);
					return false;
				}

				DWORD dwVnum = item->GetSocket(0);

				if (SkillLevelDown(dwVnum)) {
					ITEM_MANAGER::instance().RemoveItem(item);
#ifdef TEXTS_IMPROVEMENT
					ChatPacketNew(CHAT_TYPE_INFO, 399, "");
#endif
				}
#ifdef TEXTS_IMPROVEMENT
				else {
					ChatPacketNew(CHAT_TYPE_INFO, 400, "");
				}
#endif
			}
			break;

		case ITEM_SKILLBOOK:
			{
				if (item->GetVnum() == 55003 || item->GetVnum() == 55004 || item->GetVnum() == 55005) {
					return false;
				}

				if (IsPolymorphed())
				{
#ifdef TEXTS_IMPROVEMENT
					ChatPacketNew(CHAT_TYPE_INFO, 313, "");
#endif
					return false;
				}

				DWORD dwVnum = 0;
				if (item->GetVnum() == 50300)
				{
					dwVnum = item->GetSocket(0);
				}
				else
				{
					dwVnum = item->GetValue(0);
				}

				dwVnum = item->GetVnum() == 50301 || item->GetVnum() == 50302 || item->GetVnum() == 50303 ? SKILL_LEADERSHIP : dwVnum;

				if (0 == dwVnum)
				{
					ITEM_MANAGER::instance().RemoveItem(item);

					return false;
				}

				if (dwVnum == SKILL_LEADERSHIP) {
					int lv = GetSkillLevel(SKILL_LEADERSHIP);
					if (lv < item->GetValue(0)) {
#ifdef TEXTS_IMPROVEMENT
						ChatPacketNew(CHAT_TYPE_INFO, 429, "");
#endif
						return false;
					}

					if (lv >= item->GetValue(1)) {
#ifdef TEXTS_IMPROVEMENT
						ChatPacketNew(CHAT_TYPE_INFO, 430, "");
#endif
						return false;
					}
				}

				if (true == LearnSkillByBook(dwVnum))
				{
#ifdef ENABLE_BOOKS_STACKFIX
					item->SetCount(item->GetCount() - 1);
#else
					ITEM_MANAGER::instance().RemoveItem(item);
#endif
					int iReadDelay = number(SKILLBOOK_DELAY_MIN, SKILLBOOK_DELAY_MAX);
					SetSkillNextReadTime(dwVnum, dwVnum == SKILL_LEADERSHIP ? get_global_time() + 18000 : get_global_time() + iReadDelay);
				}
			}
			break;
#ifdef ENABLE_NEW_PET_EDITS
		case ITEM_TYPE_PET:
			{
				if (!GetNewPetSystem())
					return false;
				
				if (GetNewPetSystem()->IsActivePet()) {
					GetNewPetSystem()->IncreasePetSkillByBook(item);
				}
#ifdef TEXTS_IMPROVEMENT
				else {
					ChatPacketNew(CHAT_TYPE_INFO, 53, "");
				}
#endif
			}
			break;
#endif
		case ITEM_USE:
			{
				if (item->GetVnum() > 50800 && item->GetVnum() <= 50820)
				{
					if (test_server)
						sys_log (0, "ADD addtional effect : vnum(%d) subtype(%d)", item->GetOriginalVnum(), item->GetSubType());

					int affect_type = AFFECT_EXP_BONUS_EURO_FREE;
					int apply_type = aApplyInfo[item->GetValue(0)].bPointType;
					int apply_value = item->GetValue(2);
					int apply_duration = item->GetValue(1);

					switch (item->GetSubType())
					{
						case USE_ABILITY_UP:
							if (FindAffect(affect_type, apply_type))
							{
#ifdef TEXTS_IMPROVEMENT
								ChatPacketNew(CHAT_TYPE_INFO, 442, "");
#endif
								return false;
							}

							{
								switch (item->GetValue(0))
								{
									case APPLY_MOV_SPEED:
										AddAffect(affect_type, apply_type, apply_value, AFF_MOV_SPEED_POTION, apply_duration, 0, true, true);
										break;

									case APPLY_ATT_SPEED:
										AddAffect(affect_type, apply_type, apply_value, AFF_ATT_SPEED_POTION, apply_duration, 0, true, true);
										break;

									case APPLY_STR:
									case APPLY_DEX:
									case APPLY_CON:
									case APPLY_INT:
									case APPLY_CAST_SPEED:
									case APPLY_RESIST_MAGIC:
									case APPLY_ATT_GRADE_BONUS:
									case APPLY_DEF_GRADE_BONUS:
										AddAffect(affect_type, apply_type, apply_value, 0, apply_duration, 0, true, true);
										break;
								}
							}

							if (GetWarMap())
								GetWarMap()->UsePotion(this, item);

							item->SetCount(item->GetCount() - 1);
							break;

					case USE_AFFECT :
						{
							if (FindAffect(AFFECT_EXP_BONUS_EURO_FREE, aApplyInfo[item->GetValue(1)].bPointType))
							{
#ifdef TEXTS_IMPROVEMENT
								ChatPacketNew(CHAT_TYPE_INFO, 442, "");
#endif
							}
							else
							{
								// PC_BANG_ITEM_ADD
								if (item->IsPCBangItem() == true)
								{
									// PC������ üũ�ؼ� ó��
									if (CPCBangManager::instance().IsPCBangIP(GetDesc()->GetHostName()) == false)
									{
#ifdef TEXTS_IMPROVEMENT
										ChatPacketNew(CHAT_TYPE_INFO, 426, "");
#endif
										return false;
									}
								}
								// END_PC_BANG_ITEM_ADD

								AddAffect(AFFECT_EXP_BONUS_EURO_FREE, aApplyInfo[item->GetValue(1)].bPointType, item->GetValue(2), 0, item->GetValue(3), 0, false, true);
								item->SetCount(item->GetCount() - 1);
							}
						}
						break;
					case USE_POTION_NODELAY:
						{
							if (CArenaManager::instance().IsArenaMap(GetMapIndex()) == true)
							{
								if (quest::CQuestManager::instance().GetEventFlag("arena_potion_limit") > 0)
								{
#ifdef TEXTS_IMPROVEMENT
									ChatPacketNew(CHAT_TYPE_INFO, 303, "");
#endif
									return false;
								}

								switch (item->GetVnum())
								{
									case 70020 :
									case 71018 :
									case 71019 :
									case 71020 :
										if (quest::CQuestManager::instance().GetEventFlag("arena_potion_limit_count") < 10000)
										{
											if (m_nPotionLimit <= 0)
											{
#ifdef TEXTS_IMPROVEMENT
												ChatPacketNew(CHAT_TYPE_INFO, 362, "");
#endif
												return false;
											}
										}
										break;

									default :
#ifdef TEXTS_IMPROVEMENT
										ChatPacketNew(CHAT_TYPE_INFO, 303, "");
#endif
										return false;
										break;
								}
							}
#ifdef ENABLE_NEWSTUFF
							else if (g_NoPotionsOnPVP && CPVPManager::instance().IsFighting(GetPlayerID()) && !IsAllowedPotionOnPVP(item->GetVnum()))
							{
#ifdef TEXTS_IMPROVEMENT
								ChatPacketNew(CHAT_TYPE_INFO, 667, "");
#endif
								return false;
							}
#endif

							bool used = false;

							if (item->GetValue(0) != 0) // HP ���밪 ȸ��
							{
								if (GetHP() < GetMaxHP())
								{
									PointChange(POINT_HP, item->GetValue(0) * (100 + GetPoint(POINT_POTION_BONUS)) / 100);
									EffectPacket(SE_HPUP_RED);
									used = TRUE;
								}
							}

							if (item->GetValue(1) != 0)	// SP ���밪 ȸ��
							{
								if (GetSP() < GetMaxSP())
								{
									PointChange(POINT_SP, item->GetValue(1) * (100 + GetPoint(POINT_POTION_BONUS)) / 100);
									EffectPacket(SE_SPUP_BLUE);
									used = TRUE;
								}
							}

							if (item->GetValue(3) != 0) // HP % ȸ��
							{
								if (GetHP() < GetMaxHP())
								{
									PointChange(POINT_HP, item->GetValue(3) * GetMaxHP() / 100);
									EffectPacket(SE_HPUP_RED);
									used = TRUE;
								}
							}

							if (item->GetValue(4) != 0) // SP % ȸ��
							{
								if (GetSP() < GetMaxSP())
								{
									PointChange(POINT_SP, item->GetValue(4) * GetMaxSP() / 100);
									EffectPacket(SE_SPUP_BLUE);
									used = TRUE;
								}
							}

							if (used)
							{
								if (item->GetVnum() == 50085 || item->GetVnum() == 50086) {
									SetUseSeedOrMoonBottleTime();
								}

								if (GetWarMap())
									GetWarMap()->UsePotion(this, item);

								m_nPotionLimit--;

								//RESTRICT_USE_SEED_OR_MOONBOTTLE
								item->SetCount(item->GetCount() - 1);
								//END_RESTRICT_USE_SEED_OR_MOONBOTTLE
							}
						}
						break;
					}

					return true;
				}


				if (item->GetVnum() >= 27863 && item->GetVnum() <= 27883)
				{
					if (CArenaManager::instance().IsArenaMap(GetMapIndex()) == true)
					{
#ifdef TEXTS_IMPROVEMENT
						ChatPacketNew(CHAT_TYPE_INFO, 667, "");
#endif
						return false;
					}
#ifdef ENABLE_NEWSTUFF
					else if (g_NoPotionsOnPVP && CPVPManager::instance().IsFighting(GetPlayerID()) && !IsAllowedPotionOnPVP(item->GetVnum()))
					{
#ifdef TEXTS_IMPROVEMENT
						ChatPacketNew(CHAT_TYPE_INFO, 667, "");
#endif
						return false;
					}
#endif
				}

				if (test_server)
				{
					 sys_log (0, "USE_ITEM %s Type %d SubType %d vnum %d", item->GetName(), item->GetType(), item->GetSubType(), item->GetOriginalVnum());
				}

#ifdef ENABLE_COSTUME_TIME_EXTENDER
				if (item->IsCostumeTimeExtender())
				{					
					LPITEM item2;
							
					if (!IsValidItemPosition(DestCell) || !(item2 = GetItem(DestCell)))
						return false;
							
					if (item2->IsExchanging() || item2->IsEquipped() || item2->GetType() != ITEM_COSTUME)
					{
						//ChatPacket(CHAT_TYPE_INFO, LC_TEXT("costume_time_cannot_use"));
						ChatPacketNew(CHAT_TYPE_INFO, 3015, "");
						return false;
					}

					switch (item2->GetSubType())
					{
						case COSTUME_BODY:
						case COSTUME_HAIR:
						case COSTUME_WEAPON:
						{
							item2->SetSocket(0, item2->GetSocket(0) + item->GetValue(5));
							item->SetCount(item->GetCount() - 1);
							//ChatPacket(CHAT_TYPE_INFO, LC_TEXT("costume_time_added"));
							ChatPacketNew(CHAT_TYPE_INFO, 3016, "");
							break;
						}
						default:
						{
							//ChatPacket(CHAT_TYPE_INFO, LC_TEXT("costume_time_wrong_item"));
							ChatPacketNew(CHAT_TYPE_INFO, 3017, "");
							break;
						}
					}
				}
#endif

				switch (item->GetSubType())
				{
					case USE_FISH:
					{
						CAffect* pAffect = NULL;
						int type = 0, duration = item->GetValue(0);
						for (int i = 0; i < ITEM_APPLY_MAX_NUM; i++) {
							type = aApplyInfo[item->GetApplyType(i)].bPointType;
							if (type != 0) {
								pAffect = FindAffect(AFFECT_FISH_BONUS, type);
							}
						}

						if (pAffect != NULL) {
#ifdef TEXTS_IMPROVEMENT
							ChatPacketNew(CHAT_TYPE_INFO, 893, "");
#endif
							return false;
						} else {
							item->SetCount(item->GetCount() - 1);

							for (int i = 0; i < ITEM_APPLY_MAX_NUM; i++) {
								type = item->GetApplyType(i);
								if (type != 0) {
									AddAffect(AFFECT_FISH_BONUS, aApplyInfo[type].bPointType, item->GetApplyValue(i), item->GetID(), duration, 0, false, false);
								}
							}
						}
						break;
					}
					case USE_TIME_CHARGE_PER:
						{
							LPITEM pDestItem = GetItem(DestCell);
							if (NULL == pDestItem)
							{
								return false;
							}
							// �켱 ��ȥ���� ���ؼ��� �ϵ��� �Ѵ�.
							if (pDestItem->IsDragonSoul())
							{
#ifdef ENABLE_DS_POTION_DIFFRENT
								if (item->GetCount() > 1) {
									int pos = GetEmptyInventory(item->GetSize());
									if (pos == -1) {
#ifdef TEXTS_IMPROVEMENT
										ChatPacketNew(CHAT_TYPE_INFO, 366, "");
#endif
										return false;
									}
									
									item->SetCount(item->GetCount() - 1);
									LPITEM item2 = ITEM_MANAGER::instance().CreateItem(item->GetVnum(), 1);
									if (!item2)
										return false;
									
									item2->AddToCharacter(this, TItemPos(INVENTORY, pos), false);
									item = item2;
								}

								if (item->GetSocket(0) <= 0) {
									item->RemoveFromCharacter();
									return false;
								} else {
									DWORD duration = DSManager::instance().GetDuration(pDestItem);
									DWORD remain_sec = pDestItem->GetSocket(ITEM_SOCKET_REMAIN_SEC);
									if (remain_sec == duration)
										return false;
									
									DWORD dwBottlePercent = item->GetSocket(0);
									DWORD dwOnePercent = duration / 100;
									DWORD dwRemainPercent = remain_sec / dwOnePercent;
									DWORD dif = 100 - dwRemainPercent;
									dif = dif > dwBottlePercent ? dwBottlePercent : dif;
									DWORD add = dwOnePercent * dif;
									if (remain_sec + add >= 86400) {
										pDestItem->SetSocket(ITEM_SOCKET_REMAIN_SEC, duration);
									} else {
										pDestItem->SetSocket(ITEM_SOCKET_REMAIN_SEC, remain_sec + add);
									}
									
									item->SetSocket(0, dwBottlePercent - dif);
									if (item->GetSocket(0) < 1)
										item->RemoveFromCharacter();

									return true;
								}
#else
								int ret;
								char buf[128];
								if (item->GetVnum() == DRAGON_HEART_VNUM)
								{
									ret = pDestItem->GiveMoreTime_Per((float)item->GetSocket(ITEM_SOCKET_CHARGING_AMOUNT_IDX));
								}
								else
								{
									ret = pDestItem->GiveMoreTime_Per((float)item->GetValue(ITEM_VALUE_CHARGING_AMOUNT_IDX));
								}
								if (ret > 0)
								{
									if (item->GetVnum() == DRAGON_HEART_VNUM)
									{
										sprintf(buf, "Inc %ds by item{VN:%d SOC%d:%ld}", ret, item->GetVnum(), ITEM_SOCKET_CHARGING_AMOUNT_IDX, item->GetSocket(ITEM_SOCKET_CHARGING_AMOUNT_IDX));
									}
									else
									{
										sprintf(buf, "Inc %ds by item{VN:%d VAL%d:%ld}", ret, item->GetVnum(), ITEM_VALUE_CHARGING_AMOUNT_IDX, item->GetValue(ITEM_VALUE_CHARGING_AMOUNT_IDX));
									}

#ifdef TEXTS_IMPROVEMENT
									ChatPacketNew(CHAT_TYPE_INFO, 670, "%s#%d", pDestItem->GetName(), ret);
#endif
									item->SetCount(item->GetCount() - 1);
									LogManager::instance().ItemLog(this, item, "DS_CHARGING_SUCCESS", buf);
									return true;
								}
								else
								{
									if (item->GetVnum() == DRAGON_HEART_VNUM)
									{
										sprintf(buf, "No change by item{VN:%d SOC%d:%ld}", item->GetVnum(), ITEM_SOCKET_CHARGING_AMOUNT_IDX, item->GetSocket(ITEM_SOCKET_CHARGING_AMOUNT_IDX));
									}
									else
									{
										sprintf(buf, "No change by item{VN:%d VAL%d:%ld}", item->GetVnum(), ITEM_VALUE_CHARGING_AMOUNT_IDX, item->GetValue(ITEM_VALUE_CHARGING_AMOUNT_IDX));
									}

#ifdef TEXTS_IMPROVEMENT
									ChatPacketNew(CHAT_TYPE_INFO, 671, "%s", pDestItem->GetName());
#endif
									LogManager::instance().ItemLog(this, item, "DS_CHARGING_FAILED", buf);
									return false;
								}
#endif
							}
							else
								return false;
						}
						break;
					case USE_TIME_CHARGE_FIX:
						{
							LPITEM pDestItem = GetItem(DestCell);
							if (NULL == pDestItem)
							{
								return false;
							}
							// �켱 ��ȥ���� ���ؼ��� �ϵ��� �Ѵ�.
							if (pDestItem->IsDragonSoul())
							{
								int ret = pDestItem->GiveMoreTime_Fix(item->GetValue(ITEM_VALUE_CHARGING_AMOUNT_IDX));
								char buf[128];
								if (ret)
								{
#ifdef TEXTS_IMPROVEMENT
									ChatPacketNew(CHAT_TYPE_INFO, 670, "%s#%d", pDestItem->GetName(), ret);
#endif
									sprintf(buf, "Increase %ds by item{VN:%d VAL%d:%ld}", ret, item->GetVnum(), ITEM_VALUE_CHARGING_AMOUNT_IDX, item->GetValue(ITEM_VALUE_CHARGING_AMOUNT_IDX));
									LogManager::instance().ItemLog(this, item, "DS_CHARGING_SUCCESS", buf);
									item->SetCount(item->GetCount() - 1);
									return true;
								}
								else
								{
#ifdef TEXTS_IMPROVEMENT
									ChatPacketNew(CHAT_TYPE_INFO, 671, "%s", pDestItem->GetName());
#endif
									sprintf(buf, "No change by item{VN:%d VAL%d:%ld}", item->GetVnum(), ITEM_VALUE_CHARGING_AMOUNT_IDX, item->GetValue(ITEM_VALUE_CHARGING_AMOUNT_IDX));
									LogManager::instance().ItemLog(this, item, "DS_CHARGING_FAILED", buf);
									return false;
								}
							}
							else
								return false;
						}
						break;
#ifdef ENABLE_NEW_USE_POTION
					case USE_NEW_POTIION: {
							DWORD dwType = item->GetValue(0);
							if (dwType >= AFFECT_NEW_POTION24 && dwType <= AFFECT_NEW_POTION29 && !marriage::CManager::instance().IsMarried(GetPlayerID())) {
#ifdef TEXTS_IMPROVEMENT
								ChatPacketNew(CHAT_TYPE_INFO, 891, "");
#endif
								return false;
							}

							if (dwType == AFFECT_NEW_POTION31) {
								LPPARTY party = GetParty();
								if ((!party) || (party && GetPlayerID() != party->GetLeaderPID())) {
#ifdef TEXTS_IMPROVEMENT
									ChatPacketNew(CHAT_TYPE_INFO, 902, "");
#endif
									return false;
								}
							}

							if (item->GetCount() > 1)
							{
								int pos = GetEmptyInventory(item->GetSize());
								if (pos == -1) {
#ifdef TEXTS_IMPROVEMENT
									ChatPacketNew(CHAT_TYPE_INFO, 366, "");
#endif
									break;
								}
								
								item->SetCount(item->GetCount() - 1);
								LPITEM item2 = ITEM_MANAGER::instance().CreateItem(item->GetVnum(), 1);
								if (!item2)
									return true;
								
								item2->AddToCharacter(this, TItemPos(INVENTORY, pos), false);
								item = item2;
							}
							
							BYTE bApplyOn = item->GetApplyType(0);
							long lApplyValue = item->GetApplyValue(0);
							CAffect* pAffect = FindAffect(dwType);
							if (pAffect) {
								DWORD dwItemID = pAffect->dwFlag;
								if (item->GetID() == dwItemID) {
									item->Lock(false);
									item->SetSocket(1, 0);
									RemoveAffect(dwType);
#ifdef TEXTS_IMPROVEMENT
									ChatPacketNew(CHAT_TYPE_INFO, 28, "%s", item->GetName());
#endif
								} else {
									LPITEM pkItem = FindItemByID(dwItemID);
									if (pkItem) {
										pkItem->Lock(false);
										pkItem->SetSocket(1, 0);
									}
									
									RemoveAffect(dwType);
									item->Lock(true);
									item->SetSocket(1, 1);
									AddAffect(dwType, aApplyInfo[bApplyOn].bPointType, lApplyValue, item->GetID(), INFINITE_AFFECT_DURATION, 0, true, false);
#ifdef TEXTS_IMPROVEMENT
									ChatPacketNew(CHAT_TYPE_INFO, 29, "%s", item->GetName());
#endif
								}
							}
							else {
								if (dwType == AFFECT_NEW_POTION19) {
									pAffect = FindAffect(AFFECT_NEW_POTION20);
									if (pAffect) {
										LPITEM pkItem = FindItemByID(pAffect->dwFlag);
										if (pkItem) {
											pkItem->Lock(false);
											pkItem->SetSocket(1, 0);
										}
										
										RemoveAffect(AFFECT_NEW_POTION20);
									}
								}
								else if (dwType == AFFECT_NEW_POTION20) {
									pAffect = FindAffect(AFFECT_NEW_POTION19);
									if (pAffect) {
										LPITEM pkItem = FindItemByID(pAffect->dwFlag);
										if (pkItem) {
											pkItem->Lock(false);
											pkItem->SetSocket(1, 0);
										}
										
										RemoveAffect(AFFECT_NEW_POTION19);
									}
								}
								else if (dwType == AFFECT_NEW_POTION21) {
									pAffect = FindAffect(AFFECT_NEW_POTION22);
									if (pAffect) {
										LPITEM pkItem = FindItemByID(pAffect->dwFlag);
										if (pkItem) {
											pkItem->Lock(false);
											pkItem->SetSocket(1, 0);
										}
										
										RemoveAffect(AFFECT_NEW_POTION22);
									}
								}
								else if (dwType == AFFECT_NEW_POTION22) {
									pAffect = FindAffect(AFFECT_NEW_POTION21);
									if (pAffect) {
										LPITEM pkItem = FindItemByID(pAffect->dwFlag);
										if (pkItem) {
											pkItem->Lock(false);
											pkItem->SetSocket(1, 0);
										}
										
										RemoveAffect(AFFECT_NEW_POTION21);
									}
								}
								
								item->Lock(true);
								item->SetSocket(1, 1);
								AddAffect(dwType, aApplyInfo[bApplyOn].bPointType, lApplyValue, item->GetID(), INFINITE_AFFECT_DURATION, 0, true, false);
#ifdef TEXTS_IMPROVEMENT
								ChatPacketNew(CHAT_TYPE_INFO, 29, "%s", item->GetName());
#endif
							}
						}
						break;
#endif
					case USE_SPECIAL:

						switch (item->GetVnum())
						{
							//ũ�������� ����
							case ITEM_NOG_POCKET:
								{
									/*
									���ִɷ�ġ : item_proto value �ǹ�
										�̵��ӵ�  value 1
										���ݷ�	  value 2
										����ġ    value 3
										���ӽð�  value 0 (���� ��)

									*/
									if (FindAffect(AFFECT_NOG_ABILITY))
									{
#ifdef TEXTS_IMPROVEMENT
										ChatPacketNew(CHAT_TYPE_INFO, 442, "");
#endif
										return false;
									}
									long time = item->GetValue(0);
									long moveSpeedPer	= item->GetValue(1);
									long attPer	= item->GetValue(2);
									long expPer			= item->GetValue(3);
									AddAffect(AFFECT_NOG_ABILITY, POINT_MOV_SPEED, moveSpeedPer, AFF_MOV_SPEED_POTION, time, 0, true, true);
									AddAffect(AFFECT_NOG_ABILITY, POINT_MALL_ATTBONUS, attPer, AFF_NONE, time, 0, true, true);
									AddAffect(AFFECT_NOG_ABILITY, POINT_MALL_EXPBONUS, expPer, AFF_NONE, time, 0, true, true);
									item->SetCount(item->GetCount() - 1);
								}
								break;

							//�󸶴ܿ� ����
							case ITEM_RAMADAN_CANDY:
								{
									/*
									�����ɷ�ġ : item_proto value �ǹ�
										�̵��ӵ�  value 1
										���ݷ�	  value 2
										����ġ    value 3
										���ӽð�  value 0 (���� ��)

									*/
									// @fixme147 BEGIN
									if (FindAffect(AFFECT_RAMADAN_ABILITY))
									{
#ifdef TEXTS_IMPROVEMENT
										ChatPacketNew(CHAT_TYPE_INFO, 442, "");
#endif
										return false;
									}
									// @fixme147 END
									long time = item->GetValue(0);
									long moveSpeedPer	= item->GetValue(1);
									long attPer	= item->GetValue(2);
									long expPer			= item->GetValue(3);
									AddAffect(AFFECT_RAMADAN_ABILITY, POINT_MOV_SPEED, moveSpeedPer, AFF_MOV_SPEED_POTION, time, 0, true, true);
									AddAffect(AFFECT_RAMADAN_ABILITY, POINT_MALL_ATTBONUS, attPer, AFF_NONE, time, 0, true, true);
									AddAffect(AFFECT_RAMADAN_ABILITY, POINT_MALL_EXPBONUS, expPer, AFF_NONE, time, 0, true, true);
									item->SetCount(item->GetCount() - 1);
								}
								break;
							case ITEM_MARRIAGE_RING:
								{
									marriage::TMarriage* pMarriage = marriage::CManager::instance().Get(GetPlayerID());
									if (pMarriage)
									{
										if (pMarriage->ch1 != NULL)
										{
											if (CArenaManager::instance().IsArenaMap(pMarriage->ch1->GetMapIndex()) == true)
											{
#ifdef TEXTS_IMPROVEMENT
												ChatPacketNew(CHAT_TYPE_INFO, 672, "");
#endif
												break;
											}
										}

										if (pMarriage->ch2 != NULL)
										{
											if (CArenaManager::instance().IsArenaMap(pMarriage->ch2->GetMapIndex()) == true)
											{
#ifdef TEXTS_IMPROVEMENT
												ChatPacketNew(CHAT_TYPE_INFO, 672, "");
#endif
												break;
											}
										}

										int consumeSP = CalculateConsumeSP(this);

										if (consumeSP < 0)
											return false;

										PointChange(POINT_SP, -consumeSP, false);

										WarpToPID(pMarriage->GetOther(GetPlayerID()));
									}
#ifdef TEXTS_IMPROVEMENT
									else {
										ChatPacketNew(CHAT_TYPE_INFO, 242, "");
									}
#endif
								}
								break;

								//���� ����� ����
							case UNIQUE_ITEM_CAPE_OF_COURAGE:
								//�󸶴� ����� ����� ����
							case 70057:
							case REWARD_BOX_UNIQUE_ITEM_CAPE_OF_COURAGE:
#ifdef __EFFETTO_MANTELLO__
								this->EffectPacket(SE_MANTELLO);
								AggregateMonster();
#endif
								//item->SetCount(item->GetCount()-1);
								break;

							case UNIQUE_ITEM_WHITE_FLAG:
								ForgetMyAttacker();
								item->SetCount(item->GetCount()-1);
								break;

							case UNIQUE_ITEM_TREASURE_BOX:
								break;
#ifdef ENABLE_BATTLE_PASS
							case 70611://79900
								{
									char szQuery[1024];
									snprintf(szQuery, sizeof(szQuery), "SELECT * FROM battle_pass_ranking WHERE player_name = '%s' AND battle_pass_id = %d;", GetName(), 1);
									std::unique_ptr<SQLMsg> pmsg(DBManager::instance().DirectQuery(szQuery));
									if (pmsg->Get()->uiNumRows > 0) {
#ifdef TEXTS_IMPROVEMENT
										ChatPacketNew(CHAT_TYPE_INFO, 6, "");
#endif
										return false;
									}

									int iSeconds = GetSecondsTillNextMonth();
									if(iSeconds < 0) {
#ifdef TEXTS_IMPROVEMENT
										ChatPacketNew(CHAT_TYPE_INFO, 7, "");
#endif
										return false;
									}

									if(FindAffect(AFFECT_BATTLE_PASS)) {
#ifdef TEXTS_IMPROVEMENT
										ChatPacketNew(CHAT_TYPE_INFO, 8, "");
#endif
										return false;
									} else {
										m_dwBattlePassEndTime = get_global_time() + iSeconds;

										AddAffect(AFFECT_BATTLE_PASS, POINT_BATTLE_PASS_ID, 1, 0, iSeconds, 0, true);
										item->SetCount(item->GetCount() - 1);
									}
								}
								break;
#endif
							case 27989: // ����������
							case 76006: // ������ ����������
								{
									LPSECTREE_MAP pMap = SECTREE_MANAGER::instance().GetMap(GetMapIndex());

									if (pMap != NULL)
									{
										item->SetSocket(0, item->GetSocket(0) + 1);

										FFindStone f;

										// <Factor> SECTREE::for_each -> SECTREE::for_each_entity
										pMap->for_each(f);

										if (f.m_mapStone.size() > 0)
										{
											std::map<DWORD, LPCHARACTER>::iterator stone = f.m_mapStone.begin();

											DWORD max = UINT_MAX;
											LPCHARACTER pTarget = stone->second;

											while (stone != f.m_mapStone.end())
											{
												DWORD dist = (DWORD)DISTANCE_SQRT(GetX()-stone->second->GetX(), GetY()-stone->second->GetY());

												if (dist != 0 && max > dist)
												{
													max = dist;
													pTarget = stone->second;
												}
												stone++;
											}

											if (pTarget != NULL)
											{
												int val = 3;

												if (max < 10000) val = 2;
												else if (max < 70000) val = 1;

												ChatPacket(CHAT_TYPE_COMMAND, "StoneDetect %u %d %d", (DWORD)GetVID(), val,
														(int)GetDegreeFromPositionXY(GetX(), pTarget->GetY(), pTarget->GetX(), GetY()));
											}
#ifdef TEXTS_IMPROVEMENT
											else {
												ChatPacketNew(CHAT_TYPE_INFO, 673, "");
											}
#endif
										}
#ifdef TEXTS_IMPROVEMENT
										else {
											ChatPacketNew(CHAT_TYPE_INFO, 673, "");
										}
#endif

										if (item->GetSocket(0) >= 6)
										{
											ChatPacket(CHAT_TYPE_COMMAND, "StoneDetect %u 0 0", (DWORD)GetVID());
											ITEM_MANAGER::instance().RemoveItem(item);
										}
									}
									break;
								}
								break;

							case 27996: // ����
								item->SetCount(item->GetCount() - 1);
								AttackedByPoison(NULL); // @warme008
								break;

							case 27987: // ����
								// 50  ������ 47990
								// 30  ��
								// 10  ������ 47992
								// 7   û���� 47993
								// 3   ������ 47994
								{
									item->SetCount(item->GetCount() - 1);

									int r = number(1, 100);

									if (r <= 50)
									{
#ifdef TEXTS_IMPROVEMENT
										ChatPacketNew(CHAT_TYPE_INFO, 458, "");
#endif
										AutoGiveItem(27990);
									}
									else
									{
										const int prob_table_gb2312[] =
										{
											95, 97, 99
										};

										const int * prob_table = prob_table_gb2312;

										if (r <= prob_table[0]) {
#ifdef TEXTS_IMPROVEMENT
											ChatPacketNew(CHAT_TYPE_INFO, 457, "");
#endif
										}
										else if (r <= prob_table[1])
										{
#ifdef TEXTS_IMPROVEMENT
											ChatPacketNew(CHAT_TYPE_INFO, 459, "");
#endif
											AutoGiveItem(27992);
										}
										else if (r <= prob_table[2])
										{
#ifdef TEXTS_IMPROVEMENT
											ChatPacketNew(CHAT_TYPE_INFO, 460, "");
#endif
											AutoGiveItem(27993);
										}
										else
										{
#ifdef TEXTS_IMPROVEMENT
											ChatPacketNew(CHAT_TYPE_INFO, 461, "");
#endif
											AutoGiveItem(27994);
										}
									}
								}
								break;

							case 71013: // ����������
								CreateFly(number(FLY_FIREWORK1, FLY_FIREWORK6), this);
								item->SetCount(item->GetCount() - 1);
								break;

							case 50100: // ����
							case 50101:
							case 50102:
							case 50103:
							case 50104:
							case 50105:
							case 50106:
								CreateFly(item->GetVnum() - 50100 + FLY_FIREWORK1, this);
								item->SetCount(item->GetCount() - 1);
								break;

							case 50200: // ������
								if (g_bEnableBootaryCheck)
								{
									if (IS_BOTARYABLE_ZONE(GetMapIndex()) == true)
									{
#ifdef KASMIR_PAKET_SYSTEM
										m_bKasmirPaketDurum = false;
#endif
										__OpenPrivateShop();
									}
#ifdef TEXTS_IMPROVEMENT
									else {
										ChatPacketNew(CHAT_TYPE_INFO, 668, "");
									}
#endif
								}
								else
								{
#ifdef KASMIR_PAKET_SYSTEM
									m_bKasmirPaketDurum = false;
#endif
									__OpenPrivateShop();
								}
								break;

							case fishing::FISH_MIND_PILL_VNUM:
								{
#ifdef ENABLE_NEW_FISHING_SYSTEM
									if (FindAffect(AFFECT_FISH_MIND_PILL)) {
#ifdef TEXTS_IMPROVEMENT
										ChatPacketNew(CHAT_TYPE_INFO, 900, "");
#endif
										return false;
									}
#endif

									AddAffect(AFFECT_FISH_MIND_PILL, POINT_NONE, 0, AFF_FISH_MIND, 20*60, 0, true);
									item->SetCount(item->GetCount() - 1);
								}
								break;

							case 50304: // ����� ���ü�
							case 50305:
							case 50306:
								{
									if (IsPolymorphed())
									{
#ifdef TEXTS_IMPROVEMENT
										ChatPacketNew(CHAT_TYPE_INFO, 313, "");
#endif
										return false;

									}
									if (GetSkillLevel(SKILL_COMBO) == 0 && GetLevel() < 30)
									{
#ifdef TEXTS_IMPROVEMENT
										ChatPacketNew(CHAT_TYPE_INFO, 322, "");
#endif
										return false;
									}

									if (GetSkillLevel(SKILL_COMBO) == 1 && GetLevel() < 50)
									{
#ifdef TEXTS_IMPROVEMENT
										ChatPacketNew(CHAT_TYPE_INFO, 323, "");
#endif
										return false;
									}

									if (GetSkillLevel(SKILL_COMBO) >= 2)
									{
#ifdef TEXTS_IMPROVEMENT
										ChatPacketNew(CHAT_TYPE_INFO, 324, "");
#endif
										return false;
									}

									int iPct = item->GetValue(0);

									if (LearnSkillByBook(SKILL_COMBO, iPct))
									{
#ifdef ENABLE_BOOKS_STACKFIX
										item->SetCount(item->GetCount() - 1);
#else
										ITEM_MANAGER::instance().RemoveItem(item);
#endif

										int iReadDelay = number(SKILLBOOK_DELAY_MIN, SKILLBOOK_DELAY_MAX);
										SetSkillNextReadTime(SKILL_COMBO, get_global_time() + iReadDelay);
									}
								}
								break;

#ifdef ENABLE_NEW_SECONDARY_SKILLS
							case 50333:
							case 50334:
							case 50335:
							case 50336: {
									if (IsPolymorphed()) {
#ifdef TEXTS_IMPROVEMENT
										ChatPacketNew(CHAT_TYPE_INFO, 313, "");
#endif
										return false;

									}
									
								#ifdef __ENABLE_BLOCK_EXP__
									if (Block_Exp)
									{
										ChatPacketNew(CHAT_TYPE_INFO, 3035, "");
										return false;
									}
								#endif
									DWORD dwSkillVnum = item->GetValue(0);
									if (GetSkillLevel(dwSkillVnum) >= 10) {
#ifdef TEXTS_IMPROVEMENT
										ChatPacketNew(CHAT_TYPE_INFO, 439, "");
#endif
										return false;
									}
									
									if (LearnSkillByBook(dwSkillVnum, 0)) {
										item->SetCount(item->GetCount() - 1);
										SetSkillNextReadTime(dwSkillVnum, get_global_time() + 10800);
									}
								}
								break;
#endif

							case 50311: // ��� ���ü�
							case 50312:
							case 50313:
								{
									if (IsPolymorphed())
									{
#ifdef TEXTS_IMPROVEMENT
										ChatPacketNew(CHAT_TYPE_INFO, 313, "");
#endif
										return false;

									}
									DWORD dwSkillVnum = item->GetValue(0);
									int iPct = MINMAX(0, item->GetValue(1), 100);
									if (GetSkillLevel(dwSkillVnum)>=20 || dwSkillVnum-SKILL_LANGUAGE1+1 == GetEmpire())
									{
#ifdef TEXTS_IMPROVEMENT
										ChatPacketNew(CHAT_TYPE_INFO, 439, "");
#endif
										return false;
									}

									if (LearnSkillByBook(dwSkillVnum, iPct))
									{
#ifdef ENABLE_BOOKS_STACKFIX
										item->SetCount(item->GetCount() - 1);
#else
										ITEM_MANAGER::instance().RemoveItem(item);
#endif

										int iReadDelay = number(SKILLBOOK_DELAY_MIN, SKILLBOOK_DELAY_MAX);
										SetSkillNextReadTime(dwSkillVnum, get_global_time() + iReadDelay);
									}
								}
								break;

							case 50061 : // �Ϻ� �� ��ȯ ��ų ���ü�
								{
									if (IsPolymorphed())
									{
#ifdef TEXTS_IMPROVEMENT
										ChatPacketNew(CHAT_TYPE_INFO, 313, "");
#endif
										return false;

									}
									DWORD dwSkillVnum = item->GetValue(0);
									int iPct = MINMAX(0, item->GetValue(1), 100);

									if (GetSkillLevel(dwSkillVnum) >= 10)
									{
#ifdef TEXTS_IMPROVEMENT
										ChatPacketNew(CHAT_TYPE_INFO, 306, "");
#endif
										return false;
									}

									if (LearnSkillByBook(dwSkillVnum, iPct))
									{
#ifdef ENABLE_BOOKS_STACKFIX
										item->SetCount(item->GetCount() - 1);
#else
										ITEM_MANAGER::instance().RemoveItem(item);
#endif

										int iReadDelay = number(SKILLBOOK_DELAY_MIN, SKILLBOOK_DELAY_MAX);
										SetSkillNextReadTime(dwSkillVnum, get_global_time() + iReadDelay);
									}
								}
								break;

							case 50314: case 50315: case 50316: // ���� ���ü�
							case 50323: case 50324: // ���� ���ü�
							case 50325: case 50326: // ö�� ���ü�
								{
									if (IsPolymorphed() == true)
									{
#ifdef TEXTS_IMPROVEMENT
										ChatPacketNew(CHAT_TYPE_INFO, 313, "");
#endif
										return false;
									}

									int iSkillLevelLowLimit = item->GetValue(0);
									int iSkillLevelHighLimit = item->GetValue(1);
									int iPct = MINMAX(0, item->GetValue(2), 100);
									int iLevelLimit = item->GetValue(3);
									DWORD dwSkillVnum = 0;

									switch (item->GetVnum())
									{
										case 50314: case 50315: case 50316:
											dwSkillVnum = SKILL_POLYMORPH;
											break;

										case 50323: case 50324:
											dwSkillVnum = SKILL_ADD_HP;
											break;

										case 50325: case 50326:
											dwSkillVnum = SKILL_RESIST_PENETRATE;
											break;

										default:
											return false;
									}

									if (0 == dwSkillVnum)
										return false;

									if (GetLevel() < iLevelLimit)
									{
#ifdef TEXTS_IMPROVEMENT
										ChatPacketNew(CHAT_TYPE_INFO, 431, "%d", iLevelLimit);
#endif
										return false;
									}

									if (GetSkillLevel(dwSkillVnum) >= 40)
									{
#ifdef TEXTS_IMPROVEMENT
										ChatPacketNew(CHAT_TYPE_INFO, 306, "");
#endif
										return false;
									}

									if (GetSkillLevel(dwSkillVnum) < iSkillLevelLowLimit)
									{
#ifdef TEXTS_IMPROVEMENT
										ChatPacketNew(CHAT_TYPE_INFO, 429, "");
#endif
										return false;
									}

									if (GetSkillLevel(dwSkillVnum) >= iSkillLevelHighLimit)
									{
#ifdef TEXTS_IMPROVEMENT
										ChatPacketNew(CHAT_TYPE_INFO, 306, "");
#endif
										return false;
									}

									if (LearnSkillByBook(dwSkillVnum, iPct))
									{
#ifdef ENABLE_BOOKS_STACKFIX
										item->SetCount(item->GetCount() - 1);
#else
										ITEM_MANAGER::instance().RemoveItem(item);
#endif

										int iReadDelay = number(SKILLBOOK_DELAY_MIN, SKILLBOOK_DELAY_MAX);
										SetSkillNextReadTime(dwSkillVnum, get_global_time() + iReadDelay);
									}
								}
								break;

							case 50902:
							case 50903:
							case 50904:
								{
									if (IsPolymorphed())
									{
#ifdef TEXTS_IMPROVEMENT
										ChatPacketNew(CHAT_TYPE_INFO, 313, "");
#endif
										return false;

									}
									DWORD dwSkillVnum = SKILL_CREATE;
									int iPct = MINMAX(0, item->GetValue(1), 100);

									if (GetSkillLevel(dwSkillVnum)>=40)
									{
#ifdef TEXTS_IMPROVEMENT
										ChatPacketNew(CHAT_TYPE_INFO, 306, "");
#endif
										return false;
									}

									if (LearnSkillByBook(dwSkillVnum, iPct))
									{
#ifdef ENABLE_BOOKS_STACKFIX
										item->SetCount(item->GetCount() - 1);
#else
										ITEM_MANAGER::instance().RemoveItem(item);
#endif

										int iReadDelay = number(SKILLBOOK_DELAY_MIN, SKILLBOOK_DELAY_MAX);
										SetSkillNextReadTime(dwSkillVnum, get_global_time() + iReadDelay);
									}
								}
								break;
								// MINING
							case ITEM_MINING_SKILL_TRAIN_BOOK:
								{
									if (IsPolymorphed())
									{
#ifdef TEXTS_IMPROVEMENT
										ChatPacketNew(CHAT_TYPE_INFO, 313, "");
#endif
										return false;

									}
									DWORD dwSkillVnum = SKILL_MINING;
									int iPct = MINMAX(0, item->GetValue(1), 100);

									if (GetSkillLevel(dwSkillVnum)>=40)
									{
#ifdef TEXTS_IMPROVEMENT
										ChatPacketNew(CHAT_TYPE_INFO, 306, "");
#endif
										return false;
									}

									if (LearnSkillByBook(dwSkillVnum, iPct))
									{
#ifdef ENABLE_BOOKS_STACKFIX
										item->SetCount(item->GetCount() - 1);
#else
										ITEM_MANAGER::instance().RemoveItem(item);
#endif

										int iReadDelay = number(SKILLBOOK_DELAY_MIN, SKILLBOOK_DELAY_MAX);
										SetSkillNextReadTime(dwSkillVnum, get_global_time() + iReadDelay);
									}
								}
								break;
								// END_OF_MINING

							case ITEM_HORSE_SKILL_TRAIN_BOOK:
								{
									if (IsPolymorphed())
									{
#ifdef TEXTS_IMPROVEMENT
										ChatPacketNew(CHAT_TYPE_INFO, 313, "");
#endif
										return false;

									}
									DWORD dwSkillVnum = SKILL_HORSE;
									int iPct = MINMAX(0, item->GetValue(1), 100);

									if (GetLevel() < 50)
									{
#ifdef TEXTS_IMPROVEMENT
										ChatPacketNew(CHAT_TYPE_INFO, 404, "%d", 50);
#endif
										return false;
									}

									if (!test_server && get_global_time() < GetSkillNextReadTime(dwSkillVnum))
									{
										if (FindAffect(AFFECT_SKILL_NO_BOOK_DELAY))
										{
											// �־ȼ��� ����߿��� �ð� ���� ����
											RemoveAffect(AFFECT_SKILL_NO_BOOK_DELAY);
#ifdef TEXTS_IMPROVEMENT
											ChatPacketNew(CHAT_TYPE_INFO, 465, "");
#endif
										}
										else
										{
											SkillLearnWaitMoreTimeMessage(GetSkillNextReadTime(dwSkillVnum) - get_global_time());
											return false;
										}
									}

									if (GetPoint(POINT_HORSE_SKILL) >= 20 ||
											GetSkillLevel(SKILL_HORSE_WILDATTACK) + GetSkillLevel(SKILL_HORSE_CHARGE) + GetSkillLevel(SKILL_HORSE_ESCAPE) >= 60 ||
											GetSkillLevel(SKILL_HORSE_WILDATTACK_RANGE) + GetSkillLevel(SKILL_HORSE_CHARGE) + GetSkillLevel(SKILL_HORSE_ESCAPE) >= 60)
									{
#ifdef TEXTS_IMPROVEMENT
										ChatPacketNew(CHAT_TYPE_INFO, 307, "");
#endif
										return false;
									}

									if (number(1, 100) <= iPct)
									{
#ifdef TEXTS_IMPROVEMENT
										ChatPacketNew(CHAT_TYPE_INFO, 394, "");
#endif
										PointChange(POINT_HORSE_SKILL, 1);

										int iReadDelay = number(SKILLBOOK_DELAY_MIN, SKILLBOOK_DELAY_MAX);
										if (!test_server)
											SetSkillNextReadTime(dwSkillVnum, get_global_time() + iReadDelay);
									}
#ifdef TEXTS_IMPROVEMENT
									else {
										ChatPacketNew(CHAT_TYPE_INFO, 393, "");
									}
#endif
#ifdef ENABLE_BOOKS_STACKFIX
									item->SetCount(item->GetCount() - 1);
#else
									ITEM_MANAGER::instance().RemoveItem(item);
#endif
								}
								break;

							case 70102: // ����
							case 70103: // ����
								{
									if (GetAlignment() >= 0)
										return false;

									int delta = MIN(-GetAlignment(), item->GetValue(0));

									sys_log(0, "%s ALIGNMENT ITEM %d", GetName(), delta);

									UpdateAlignment(delta);
									item->SetCount(item->GetCount() - 1);

									if (delta / 10 > 0)
									{
#ifdef TEXTS_IMPROVEMENT
										ChatPacketNew(CHAT_TYPE_INFO, 327, "%d", item->GetValue(0) / 10);
#endif
									}
								}
								break;

							case 71107: // õ��������
								{
									int val = item->GetValue(0);
									int interval = item->GetValue(1);
									quest::PC* pPC = quest::CQuestManager::instance().GetPC(GetPlayerID());
									int last_use_time = pPC->GetFlag("mythical_peach.last_use_time");

									if (get_global_time() - last_use_time < interval * 60 * 60)
									{
#ifdef TEXTS_IMPROVEMENT
										ChatPacketNew(CHAT_TYPE_INFO, 508, "");
#endif
										return false;
									}

									if (GetAlignment() == 200000)
									{
#ifdef TEXTS_IMPROVEMENT
										ChatPacketNew(CHAT_TYPE_INFO, 674, "%d", 200000);
#endif
										return false;
									}

									if (200000 - GetAlignment() < val * 10)
									{
										val = (200000 - GetAlignment()) / 10;
									}

									int old_alignment = GetAlignment() / 10;

									UpdateAlignment(val*10);

									item->SetCount(item->GetCount()-1);
									pPC->SetFlag("mythical_peach.last_use_time", get_global_time());

#ifdef TEXTS_IMPROVEMENT
									ChatPacketNew(CHAT_TYPE_INFO, 327, "%d", val);
#endif

									char buf[256 + 1];
									snprintf(buf, sizeof(buf), "%d %d", old_alignment, GetAlignment() / 10);
									LogManager::instance().CharLog(this, val, "MYTHICAL_PEACH", buf);
								}
								break;

							case 71109: // Ż����
							case 72719:
								{
									LPITEM item2;

									if (!IsValidItemPosition(DestCell) || !(item2 = GetItem(DestCell)))
										return false;

									if (item2->IsExchanging() || item2->IsEquipped()) // @fixme114
										return false;

									if (item2->GetSocketCount() == 0)
										return false;

#ifdef ENABLE_BUG_FIXES
									if (item2->IsEquipped())
										return false;
#endif

									switch( item2->GetType() )
									{
										case ITEM_WEAPON:
											break;
										case ITEM_ARMOR:
											switch (item2->GetSubType())
											{
											case ARMOR_EAR:
											case ARMOR_WRIST:
											case ARMOR_NECK:
#ifdef TEXTS_IMPROVEMENT
												ChatPacketNew(CHAT_TYPE_INFO, 675, "%s", item->GetName());
#endif
												return false;
											}
											break;

										default:
											return false;
									}

									std::stack<long> socket;

									for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
										socket.push(item2->GetSocket(i));

									int idx = ITEM_SOCKET_MAX_NUM - 1;

									while (socket.size() > 0)
									{
										if (socket.top() > 2 && socket.top() != ITEM_BROKEN_METIN_VNUM)
											break;

										idx--;
										socket.pop();
									}

									if (socket.size() == 0)
									{
#ifdef TEXTS_IMPROVEMENT
										ChatPacketNew(CHAT_TYPE_INFO, 675, "%s", item2->GetName());
#endif
										return false;
									}

									LPITEM pItemReward = AutoGiveItem(socket.top());

									if (pItemReward != NULL)
									{
										item2->SetSocket(idx, 1);

										char buf[256+1];
										snprintf(buf, sizeof(buf), "%s(%u) %s(%u)",
												item2->GetName(), item2->GetID(), pItemReward->GetName(), pItemReward->GetID());
										LogManager::instance().ItemLog(this, item, "USE_DETACHMENT_ONE", buf);

										item->SetCount(item->GetCount() - 1);
									}
								}
								break;

							case 70201:   // Ż����
							case 70202:   // ������(���)
							case 70203:   // ������(�ݻ�)
							case 70204:   // ������(������)
							case 70205:   // ������(����)
							case 70206:   // ������(������)
								{
									if (GetPart(PART_HAIR) < 1001)
									{
										quest::CQuestManager& q = quest::CQuestManager::instance();
										quest::PC* pPC = q.GetPC(GetPlayerID());

										if (pPC)
										{
											int last_dye_level = pPC->GetFlag("dyeing_hair.last_dye_level");

											if (last_dye_level == 0 ||
													last_dye_level+3 <= GetLevel() ||
													item->GetVnum() == 70201)
											{
												SetPart(PART_HAIR, item->GetVnum() - 70201);

												if (item->GetVnum() == 70201)
													pPC->SetFlag("dyeing_hair.last_dye_level", 0);
												else
													pPC->SetFlag("dyeing_hair.last_dye_level", GetLevel());

												item->SetCount(item->GetCount() - 1);
												UpdatePacket();
											}
#ifdef TEXTS_IMPROVEMENT
											else {
												ChatPacketNew(CHAT_TYPE_INFO, 97, "%d", last_dye_level+3);
											}
#endif
										}
									}
#ifdef TEXTS_IMPROVEMENT
									else {
										ChatPacketNew(CHAT_TYPE_INFO, 491, "");
									}
#endif
								}
								break;

							case ITEM_NEW_YEAR_GREETING_VNUM:
								{
									DWORD dwBoxVnum = ITEM_NEW_YEAR_GREETING_VNUM;
									std::vector <DWORD> dwVnums;
									std::vector <DWORD> dwCounts;
									std::vector <LPITEM> item_gets;
									int count = 0;

									if (GiveItemFromSpecialItemGroup(dwBoxVnum, dwVnums, dwCounts, item_gets, count))
									{
#ifdef TEXTS_IMPROVEMENT
										for (int i = 0; i < count; i++) {
											if (dwVnums[i] == CSpecialItemGroup::GOLD) {
												ChatPacketNew(CHAT_TYPE_INFO, 102, "%d", dwCounts[i]);
											}
										}
#endif
										item->SetCount(item->GetCount() - 1);
									}
								}
								break;

							case ITEM_VALENTINE_ROSE:
							case ITEM_VALENTINE_CHOCOLATE:
								{
									DWORD dwBoxVnum = item->GetVnum();
									std::vector <DWORD> dwVnums;
									std::vector <DWORD> dwCounts;
									std::vector <LPITEM> item_gets(0);
									int count = 0;
									
									if (item->GetVnum() == ITEM_VALENTINE_ROSE && SEX_MALE == GET_SEX(this)) {
#ifdef TEXTS_IMPROVEMENT
										ChatPacketNew(CHAT_TYPE_INFO, 383, "");
#endif
										return false;
									}
									else if (item->GetVnum() == ITEM_VALENTINE_CHOCOLATE && SEX_FEMALE == GET_SEX(this)) {
#ifdef TEXTS_IMPROVEMENT
										ChatPacketNew(CHAT_TYPE_INFO, 382, "");
#endif
										return false;
									}

									if (GiveItemFromSpecialItemGroup(dwBoxVnum, dwVnums, dwCounts, item_gets, count))
										item->SetCount(item->GetCount()-1);
								}
								break;

							case ITEM_WHITEDAY_CANDY:
							case ITEM_WHITEDAY_ROSE:
								{
									DWORD dwBoxVnum = item->GetVnum();
									std::vector <DWORD> dwVnums;
									std::vector <DWORD> dwCounts;
									std::vector <LPITEM> item_gets(0);
									int count = 0;

									if (item->GetVnum() == ITEM_WHITEDAY_ROSE && SEX_MALE == GET_SEX(this)) {
#ifdef TEXTS_IMPROVEMENT
										ChatPacketNew(CHAT_TYPE_INFO, 383, "");
#endif
										return false;
									}
									else if (item->GetVnum() == ITEM_WHITEDAY_CANDY && SEX_FEMALE == GET_SEX(this)) {
#ifdef TEXTS_IMPROVEMENT
										ChatPacketNew(CHAT_TYPE_INFO, 382, "");
#endif
										return false;
									}

									if (GiveItemFromSpecialItemGroup(dwBoxVnum, dwVnums, dwCounts, item_gets, count))
										item->SetCount(item->GetCount()-1);
								}
								break;

							case 50011: // ��������
								{
									DWORD dwBoxVnum = 50011;
									std::vector <DWORD> dwVnums;
									std::vector <DWORD> dwCounts;
									std::vector <LPITEM> item_gets(0);
									int count = 0;

									if (GiveItemFromSpecialItemGroup(dwBoxVnum, dwVnums, dwCounts, item_gets, count))
									{
										for (int i = 0; i < count; i++)
										{
											char buf[50 + 1];
											snprintf(buf, sizeof(buf), "%u %u", dwVnums[i], dwCounts[i]);
											LogManager::instance().ItemLog(this, item, "MOONLIGHT_GET", buf);

											//ITEM_MANAGER::instance().RemoveItem(item);
											item->SetCount(item->GetCount() - 1);

											switch (dwVnums[i])
											{
											case CSpecialItemGroup::GOLD:
												break;
											case CSpecialItemGroup::EXP:
												break;

											case CSpecialItemGroup::MOB:
#ifdef TEXTS_IMPROVEMENT
												ChatPacketNew(CHAT_TYPE_INFO, 378, "");
#endif
												break;

											case CSpecialItemGroup::SLOW:
#ifdef TEXTS_IMPROVEMENT
												ChatPacketNew(CHAT_TYPE_INFO, 377, "");
#endif
												break;

											case CSpecialItemGroup::DRAIN_HP:
#ifdef TEXTS_IMPROVEMENT
												ChatPacketNew(CHAT_TYPE_INFO, 373, "");
#endif
												break;

											case CSpecialItemGroup::POISON:
#ifdef TEXTS_IMPROVEMENT
												ChatPacketNew(CHAT_TYPE_INFO, 376, "");
#endif
												break;
#ifdef ENABLE_WOLFMAN_CHARACTER
											case CSpecialItemGroup::BLEEDING:
#ifdef TEXTS_IMPROVEMENT
												ChatPacketNew(CHAT_TYPE_INFO, 379, "");
#endif
												break;
#endif
											case CSpecialItemGroup::MOB_GROUP:
#ifdef TEXTS_IMPROVEMENT
												ChatPacketNew(CHAT_TYPE_INFO, 380, "");
#endif
												break;

											default:
//#ifdef TEXTS_IMPROVEMENT
//												if (item_gets[i]) {
//													if (dwCounts[i] > 1) {
//														ChatPacketNew(CHAT_TYPE_INFO, 374, "%d#%s", dwCounts[i], item_gets[i]->GetName());
//													} else {
//														ChatPacketNew(CHAT_TYPE_INFO, 375, "%s", item_gets[i]->GetName());
//													}
//												}
//#endif
												break;
											}
										}
									}
									else
									{
#ifdef TEXTS_IMPROVEMENT
										ChatPacketNew(CHAT_TYPE_INFO, 395, "");
#endif
										return false;
									}
								}
								break;

							case ITEM_GIVE_STAT_RESET_COUNT_VNUM:
								{
									//PointChange(POINT_GOLD, -iCost);
									PointChange(POINT_STAT_RESET_COUNT, 1);
									item->SetCount(item->GetCount()-1);
								}
								break;

							case 50107:
								{
									if (CArenaManager::instance().IsArenaMap(GetMapIndex()) == true)
									{
#ifdef TEXTS_IMPROVEMENT
										ChatPacketNew(CHAT_TYPE_INFO, 667, "");
#endif
										return false;
									}
#ifdef ENABLE_NEWSTUFF
									else if (g_NoPotionsOnPVP && CPVPManager::instance().IsFighting(GetPlayerID()) && !IsAllowedPotionOnPVP(item->GetVnum()))
									{
#ifdef TEXTS_IMPROVEMENT
										ChatPacketNew(CHAT_TYPE_INFO, 667, "");
#endif
										return false;
									}
#endif

									EffectPacket(SE_CHINA_FIREWORK);
#ifdef ENABLE_FIREWORK_STUN
									// ���� ������ �÷��ش�
									AddAffect(AFFECT_CHINA_FIREWORK, POINT_STUN_PCT, 30, AFF_CHINA_FIREWORK, 5*60, 0, true);
#endif
									item->SetCount(item->GetCount()-1);
								}
								break;

							case 50108:
								{
									if (CArenaManager::instance().IsArenaMap(GetMapIndex()) == true)
									{
#ifdef TEXTS_IMPROVEMENT
										ChatPacketNew(CHAT_TYPE_INFO, 667, "");
#endif
										return false;
									}
#ifdef ENABLE_NEWSTUFF
									else if (g_NoPotionsOnPVP && CPVPManager::instance().IsFighting(GetPlayerID()) && !IsAllowedPotionOnPVP(item->GetVnum()))
									{
#ifdef TEXTS_IMPROVEMENT
										ChatPacketNew(CHAT_TYPE_INFO, 667, "");
#endif
										return false;
									}
#endif

									EffectPacket(SE_SPIN_TOP);
#ifdef ENABLE_FIREWORK_STUN
									// ���� ������ �÷��ش�
									AddAffect(AFFECT_CHINA_FIREWORK, POINT_STUN_PCT, 30, AFF_CHINA_FIREWORK, 5*60, 0, true);
#endif
									item->SetCount(item->GetCount()-1);
								}
								break;

							case ITEM_WONSO_BEAN_VNUM:
								PointChange(POINT_HP, GetMaxHP() - GetHP());
								item->SetCount(item->GetCount()-1);
								break;

							case ITEM_WONSO_SUGAR_VNUM:
								PointChange(POINT_SP, GetMaxSP() - GetSP());
								item->SetCount(item->GetCount()-1);
								break;

							case ITEM_WONSO_FRUIT_VNUM:
								PointChange(POINT_STAMINA, GetMaxStamina()-GetStamina());
								item->SetCount(item->GetCount()-1);
								break;

							case 90008: // VCARD
							case 90009: // VCARD
								VCardUse(this, this, item);
								break;

							case ITEM_ELK_VNUM: // ���ٷ���
								{
									int iGold = item->GetSocket(0);
									ITEM_MANAGER::instance().RemoveItem(item);
									PointChange(POINT_GOLD, iGold);
								}
								break;
							case 27995:
								{
								}
								break;

							case 71092 : // ���� ��ü�� �ӽ�
								{
									if (m_pkChrTarget != NULL)
									{
										if (m_pkChrTarget->IsPolymorphed())
										{
											m_pkChrTarget->SetPolymorph(0);
											m_pkChrTarget->RemoveAffect(AFFECT_POLYMORPH);
										}
									}
									else
									{
										if (IsPolymorphed())
										{
											SetPolymorph(0);
											RemoveAffect(AFFECT_POLYMORPH);
										}
									}
								}
								break;

							case 71051 : // ���簡
								{
									// ����, �̰���, ��Ʈ�� ���簡 ������
									LPITEM item2;

									if (!IsValidItemPosition(DestCell) || !(item2 = GetInventoryItem(wDestCell)))
										return false;

									if (ITEM_COSTUME == item2->GetType())
									{
#ifdef TEXTS_IMPROVEMENT
										ChatPacketNew(CHAT_TYPE_INFO, 396, "");
#endif
										return false;
									}

									if (item2->IsExchanging() || item2->IsEquipped()) // @fixme114
										return false;

									if (item2->GetAttributeSetIndex() == -1)
									{
#ifdef TEXTS_IMPROVEMENT
										ChatPacketNew(CHAT_TYPE_INFO, 396, "");
#endif
										return false;
									}

									if (item2->AddRareAttribute() == true)
									{
#ifdef TEXTS_IMPROVEMENT
										ChatPacketNew(CHAT_TYPE_INFO, 389, "");
#endif
										int iAddedIdx = item2->GetRareAttrCount() + 4;
										char buf[21];
										snprintf(buf, sizeof(buf), "%u", item2->GetID());

										LogManager::instance().ItemLog(
												GetPlayerID(),
												item2->GetAttributeType(iAddedIdx),
												item2->GetAttributeValue(iAddedIdx),
												item->GetID(),
												"ADD_RARE_ATTR",
												buf,
												GetDesc()->GetHostName(),
												item->GetOriginalVnum());

										item->SetCount(item->GetCount() - 1);
									}
#ifdef TEXTS_IMPROVEMENT
									else {
										ChatPacketNew(CHAT_TYPE_INFO, 308, "");
									}
#endif
								}
								break;

							case 71052 : // �����
								{
									// ����, �̰���, ��Ʈ�� ���簡 ������
									LPITEM item2;

									if (!IsValidItemPosition(DestCell) || !(item2 = GetItem(DestCell)))
										return false;

									if (ITEM_COSTUME == item2->GetType()) // @fixme124
									{
#ifdef TEXTS_IMPROVEMENT
										ChatPacketNew(CHAT_TYPE_INFO, 396, "");
#endif
										return false;
									}

									if (item2->IsExchanging() || item2->IsEquipped()) // @fixme114
										return false;

									if (item2->GetAttributeSetIndex() == -1)
									{
#ifdef TEXTS_IMPROVEMENT
										ChatPacketNew(CHAT_TYPE_INFO, 396, "");
#endif
										return false;
									}

									if (item2->ChangeRareAttribute() == true)
									{
										char buf[21];
										snprintf(buf, sizeof(buf), "%u", item2->GetID());
										LogManager::instance().ItemLog(this, item, "CHANGE_RARE_ATTR", buf);

										item->SetCount(item->GetCount() - 1);
									}
#ifdef TEXTS_IMPROVEMENT
									else {
										ChatPacketNew(CHAT_TYPE_INFO, 354, "");
									}
#endif
								}
								break;

							case ITEM_AUTO_HP_RECOVERY_S:
							case ITEM_AUTO_HP_RECOVERY_M:
							case ITEM_AUTO_HP_RECOVERY_L:
							case ITEM_AUTO_HP_RECOVERY_X:
							case ITEM_AUTO_SP_RECOVERY_S:
							case ITEM_AUTO_SP_RECOVERY_M:
							case ITEM_AUTO_SP_RECOVERY_L:
							case ITEM_AUTO_SP_RECOVERY_X:
							// ���ù��������� ������ �ϴ� �� ��ġ��� ������...
							// �׷��� �׳� �ϵ� �ڵ�. ���� ���ڿ� �ڵ����� �����۵�.
							case REWARD_BOX_ITEM_AUTO_SP_RECOVERY_XS:
							case REWARD_BOX_ITEM_AUTO_SP_RECOVERY_S:
							case REWARD_BOX_ITEM_AUTO_HP_RECOVERY_XS:
							case REWARD_BOX_ITEM_AUTO_HP_RECOVERY_S:
								{
									if (CArenaManager::instance().IsArenaMap(GetMapIndex()) == true)
									{
#ifdef TEXTS_IMPROVEMENT
										ChatPacketNew(CHAT_TYPE_INFO, 667, "");
#endif
										return false;
									}
#ifdef ENABLE_NEWSTUFF
									else if (g_NoPotionsOnPVP && CPVPManager::instance().IsFighting(GetPlayerID()) && !IsAllowedPotionOnPVP(item->GetVnum()))
									{
#ifdef TEXTS_IMPROVEMENT
										ChatPacketNew(CHAT_TYPE_INFO, 667, "");
#endif
										return false;
									}
#endif

									EAffectTypes type = AFFECT_NONE;
									bool isSpecialPotion = false;

									switch (item->GetVnum())
									{
										case ITEM_AUTO_HP_RECOVERY_X:
											isSpecialPotion = true;

										case ITEM_AUTO_HP_RECOVERY_S:
										case ITEM_AUTO_HP_RECOVERY_M:
										case ITEM_AUTO_HP_RECOVERY_L:
										case REWARD_BOX_ITEM_AUTO_HP_RECOVERY_XS:
										case REWARD_BOX_ITEM_AUTO_HP_RECOVERY_S:
											type = AFFECT_AUTO_HP_RECOVERY;
											break;

										case ITEM_AUTO_SP_RECOVERY_X:
											isSpecialPotion = true;

										case ITEM_AUTO_SP_RECOVERY_S:
										case ITEM_AUTO_SP_RECOVERY_M:
										case ITEM_AUTO_SP_RECOVERY_L:
										case REWARD_BOX_ITEM_AUTO_SP_RECOVERY_XS:
										case REWARD_BOX_ITEM_AUTO_SP_RECOVERY_S:
											type = AFFECT_AUTO_SP_RECOVERY;
											break;
									}

									if (AFFECT_NONE == type)
										break;

									if (item->GetCount() > 1)
									{
										int pos = GetEmptyInventory(item->GetSize());

										if (-1 == pos)
										{
#ifdef TEXTS_IMPROVEMENT
											ChatPacketNew(CHAT_TYPE_INFO, 366, "");
#endif
											break;
										}

										item->SetCount( item->GetCount() - 1 );

										LPITEM item2 = ITEM_MANAGER::instance().CreateItem( item->GetVnum(), 1 );
										item2->AddToCharacter(this, TItemPos(INVENTORY, pos));

										if (item->GetSocket(1) != 0)
										{
											item2->SetSocket(1, item->GetSocket(1));
										}
										
										if (FindAffect(type))
											return true;
										else if (isSpecialPotion) {
											EAffectTypes eType = type == AFFECT_AUTO_HP_RECOVERY ? AFFECT_AUTO_HP_RECOVERY2 : AFFECT_AUTO_SP_RECOVERY2;
											if (FindAffect(eType))
												return true;
										}
										
										item = item2;
									}
									
#ifdef ENABLE_NEW_USE_POTION
									EAffectTypes type2 = AFFECT_NONE;
									CAffect* pAffect2 = NULL;
#endif
									CAffect* pAffect = FindAffect( type );
									
									if (NULL == pAffect)
									{
										EPointTypes bonus = POINT_NONE;
										if (true == isSpecialPotion)
										{
											if (type == AFFECT_AUTO_HP_RECOVERY)
											{
#ifdef ENABLE_NEW_USE_POTION
												type2 = type;
												type = AFFECT_AUTO_HP_RECOVERY2;
#endif
												bonus = POINT_MAX_HP_PCT;
											}
											else if (type == AFFECT_AUTO_SP_RECOVERY)
											{
#ifdef ENABLE_NEW_USE_POTION
												type2 = type;
												type = AFFECT_AUTO_SP_RECOVERY2;
#endif
												bonus = POINT_MAX_SP_PCT;
											}
										}
#ifdef ENABLE_NEW_USE_POTION
										else {
											if (type == AFFECT_AUTO_HP_RECOVERY)
												type2 = AFFECT_AUTO_HP_RECOVERY2;
											else if (type == AFFECT_AUTO_SP_RECOVERY)
												type2 = AFFECT_AUTO_SP_RECOVERY2;
										}
										
										pAffect2 = FindAffect(type2);
										if (pAffect2) {
											if (item->GetID() == pAffect2->dwFlag)
											{
												RemoveAffect(pAffect2);
												item->Lock(false);
												item->SetSocket(0, false);
											}
											else
											{
												LPITEM old = FindItemByID(pAffect2->dwFlag);
												if (NULL != old)
												{
													old->Lock(false);
													old->SetSocket(0, false);
												}
												
												RemoveAffect(pAffect2);
											}
										}
										else if (isSpecialPotion == true) {
											pAffect2 = FindAffect(type);
											if (pAffect2) {
												if (item->GetID() == pAffect2->dwFlag)
												{
													RemoveAffect(pAffect2);
													item->Lock(false);
													item->SetSocket(0, false);
													return true;
												}
												else {
													LPITEM old = FindItemByID(pAffect2->dwFlag);
													if (old)
													{
														old->Lock(false);
														old->SetSocket(0, false);
													}
												}
											}
										}
#endif
										
										AddAffect( type, bonus, 4, item->GetID(), INFINITE_AFFECT_DURATION, 0, true, false);
										item->Lock(true);
										item->SetSocket(0, true);
										AutoRecoveryItemProcess(type);
									}
									else
									{
										if (item->GetID() == pAffect->dwFlag)
										{
											RemoveAffect( pAffect );

											item->Lock(false);
											item->SetSocket(0, false);
										}
										else
										{
											LPITEM old = FindItemByID( pAffect->dwFlag );

											if (NULL != old)
											{
												old->Lock(false);
												old->SetSocket(0, false);
											}

											RemoveAffect( pAffect );

											EPointTypes bonus = POINT_NONE;

											if (true == isSpecialPotion)
											{
												if (type == AFFECT_AUTO_HP_RECOVERY)
												{
#ifdef ENABLE_NEW_USE_POTION
													type2 = type;
													type = AFFECT_AUTO_HP_RECOVERY2;
#endif
													bonus = POINT_MAX_HP_PCT;
												}
												else if (type == AFFECT_AUTO_SP_RECOVERY)
												{
#ifdef ENABLE_NEW_USE_POTION
													type2 = type;
													type = AFFECT_AUTO_SP_RECOVERY2;
#endif
													bonus = POINT_MAX_SP_PCT;
												}
											}
#ifdef ENABLE_NEW_USE_POTION
											else {
												if (type == AFFECT_AUTO_HP_RECOVERY)
													type2 = AFFECT_AUTO_HP_RECOVERY2;
												else if (type == AFFECT_AUTO_SP_RECOVERY)
													type2 = AFFECT_AUTO_SP_RECOVERY2;
											}
											
											pAffect2 = FindAffect(type2);
											if (pAffect2) {
												if (item->GetID() == pAffect2->dwFlag)
												{
													RemoveAffect(pAffect2);
													item->Lock(false);
													item->SetSocket(0, false);
												}
												else
												{
													LPITEM old = FindItemByID(pAffect2->dwFlag);
													if (NULL != old)
													{
														old->Lock(false);
														old->SetSocket(0, false);
													}
													
													RemoveAffect(pAffect2);
												}
											}
#endif

											AddAffect( type, bonus, 4, item->GetID(), INFINITE_AFFECT_DURATION, 0, true, false);

											item->Lock(true);
											item->SetSocket(0, true);

											AutoRecoveryItemProcess( type );
										}
									}
								}
								break;
						}
						break;

					case USE_CLEAR:
						{
							switch (item->GetVnum())
							{
#ifdef ENABLE_WOLFMAN_CHARACTER
								case 27124: // Bandage
									RemoveBleeding();
									break;
#endif
								case 27874: // Grilled Perch
								default:
									RemoveBadAffect();
									break;
							}
							item->SetCount(item->GetCount() - 1);
						}
						break;

					case USE_INVISIBILITY:
						{
							if (item->GetVnum() == 70026)
							{
								quest::CQuestManager& q = quest::CQuestManager::instance();
								quest::PC* pPC = q.GetPC(GetPlayerID());

								if (pPC != NULL)
								{
									int last_use_time = pPC->GetFlag("mirror_of_disapper.last_use_time");

									if (get_global_time() - last_use_time < 10*60)
									{
#ifdef TEXTS_IMPROVEMENT
										ChatPacketNew(CHAT_TYPE_INFO, 508, "");
#endif
										return false;
									}

									pPC->SetFlag("mirror_of_disapper.last_use_time", get_global_time());
								}
							}

							AddAffect(AFFECT_INVISIBILITY, POINT_NONE, 0, AFF_INVISIBILITY, 300, 0, true);
							item->SetCount(item->GetCount() - 1);
						}
						break;

					case USE_POTION_NODELAY:
						{
							if (CArenaManager::instance().IsArenaMap(GetMapIndex()) == true)
							{
								if (quest::CQuestManager::instance().GetEventFlag("arena_potion_limit") > 0)
								{
#ifdef TEXTS_IMPROVEMENT
									ChatPacketNew(CHAT_TYPE_INFO, 303, "");
#endif
									return false;
								}

								switch (item->GetVnum())
								{
									case 70020 :
									case 71018 :
									case 71019 :
									case 71020 :
										if (quest::CQuestManager::instance().GetEventFlag("arena_potion_limit_count") < 10000)
										{
											if (m_nPotionLimit <= 0)
											{
#ifdef TEXTS_IMPROVEMENT
												ChatPacketNew(CHAT_TYPE_INFO, 362, "");
#endif
												return false;
											}
										}
										break;

									default :
#ifdef TEXTS_IMPROVEMENT
										ChatPacketNew(CHAT_TYPE_INFO, 303, "");
#endif
										return false;
								}
							}
#ifdef ENABLE_NEWSTUFF
							else if (g_NoPotionsOnPVP && CPVPManager::instance().IsFighting(GetPlayerID()) && !IsAllowedPotionOnPVP(item->GetVnum()))
							{
#ifdef TEXTS_IMPROVEMENT
								ChatPacketNew(CHAT_TYPE_INFO, 667, "");
#endif
								return false;
							}
#endif

							bool used = false;

							if (item->GetValue(0) != 0) // HP ���밪 ȸ��
							{
								if (GetHP() < GetMaxHP())
								{
									PointChange(POINT_HP, item->GetValue(0) * (100 + GetPoint(POINT_POTION_BONUS)) / 100);
									EffectPacket(SE_HPUP_RED);
									used = TRUE;
								}
							}

							if (item->GetValue(1) != 0)	// SP ���밪 ȸ��
							{
								if (GetSP() < GetMaxSP())
								{
									PointChange(POINT_SP, item->GetValue(1) * (100 + GetPoint(POINT_POTION_BONUS)) / 100);
									EffectPacket(SE_SPUP_BLUE);
									used = TRUE;
								}
							}

							if (item->GetValue(3) != 0) // HP % ȸ��
							{
								if (GetHP() < GetMaxHP())
								{
									PointChange(POINT_HP, item->GetValue(3) * GetMaxHP() / 100);
									EffectPacket(SE_HPUP_RED);
									used = TRUE;
								}
							}

							if (item->GetValue(4) != 0) // SP % ȸ��
							{
								if (GetSP() < GetMaxSP())
								{
									PointChange(POINT_SP, item->GetValue(4) * GetMaxSP() / 100);
									EffectPacket(SE_SPUP_BLUE);
									used = TRUE;
								}
							}

							if (used)
							{
								if (item->GetVnum() == 50085 || item->GetVnum() == 50086) {
									SetUseSeedOrMoonBottleTime();
								}

								if (GetWarMap())
									GetWarMap()->UsePotion(this, item);

								m_nPotionLimit--;

								//RESTRICT_USE_SEED_OR_MOONBOTTLE
								item->SetCount(item->GetCount() - 1);
								//END_RESTRICT_USE_SEED_OR_MOONBOTTLE
							}
						}
						break;

					case USE_POTION:
						if (CArenaManager::instance().IsArenaMap(GetMapIndex()) == true)
						{
							if (quest::CQuestManager::instance().GetEventFlag("arena_potion_limit") > 0)
							{
#ifdef TEXTS_IMPROVEMENT
								ChatPacketNew(CHAT_TYPE_INFO, 303, "");
#endif
								return false;
							}
						}
#ifdef ENABLE_NEWSTUFF
						else if (g_NoPotionsOnPVP && CPVPManager::instance().IsFighting(GetPlayerID()) && !IsAllowedPotionOnPVP(item->GetVnum()))
						{
#ifdef TEXTS_IMPROVEMENT
							ChatPacketNew(CHAT_TYPE_INFO, 667, "");
#endif
							return false;
						}
#endif

						if (item->GetValue(1) != 0)
						{
							if (GetPoint(POINT_SP_RECOVERY) + GetSP() >= GetMaxSP())
							{
								return false;
							}

							PointChange(POINT_SP_RECOVERY, item->GetValue(1) * MIN(200, (100 + GetPoint(POINT_POTION_BONUS))) / 100);
							StartAffectEvent();
							EffectPacket(SE_SPUP_BLUE);
						}

						if (item->GetValue(0) != 0)
						{
							if (GetPoint(POINT_HP_RECOVERY) + GetHP() >= GetMaxHP())
							{
								return false;
							}

							PointChange(POINT_HP_RECOVERY, item->GetValue(0) * MIN(200, (100 + GetPoint(POINT_POTION_BONUS))) / 100);
							StartAffectEvent();
							EffectPacket(SE_HPUP_RED);
						}

						if (GetWarMap())
							GetWarMap()->UsePotion(this, item);

						item->SetCount(item->GetCount() - 1);
						m_nPotionLimit--;
						break;

					case USE_POTION_CONTINUE:
						{
							if (item->GetValue(0) != 0)
							{
								AddAffect(AFFECT_HP_RECOVER_CONTINUE, POINT_HP_RECOVER_CONTINUE, item->GetValue(0), 0, item->GetValue(2), 0, true);
							}
							else if (item->GetValue(1) != 0)
							{
								AddAffect(AFFECT_SP_RECOVER_CONTINUE, POINT_SP_RECOVER_CONTINUE, item->GetValue(1), 0, item->GetValue(2), 0, true);
							}
							else
								return false;
						}

						if (GetWarMap())
							GetWarMap()->UsePotion(this, item);

						item->SetCount(item->GetCount() - 1);
						break;

					case USE_ABILITY_UP:
						{
							switch (item->GetValue(0))
							{
								case APPLY_MOV_SPEED:
									AddAffect(AFFECT_MOV_SPEED, POINT_MOV_SPEED, item->GetValue(2), AFF_MOV_SPEED_POTION, item->GetValue(1), 0, true);
#ifdef ENABLE_EFFECT_EXTRAPOT
									EffectPacket(SE_DXUP_PURPLE);
#endif
									break;

								case APPLY_ATT_SPEED:
									AddAffect(AFFECT_ATT_SPEED, POINT_ATT_SPEED, item->GetValue(2), AFF_ATT_SPEED_POTION, item->GetValue(1), 0, true);
#ifdef ENABLE_EFFECT_EXTRAPOT
									EffectPacket(SE_SPEEDUP_GREEN);
#endif
									break;

								case APPLY_STR:
									AddAffect(AFFECT_STR, POINT_ST, item->GetValue(2), 0, item->GetValue(1), 0, true);
									break;

								case APPLY_DEX:
									AddAffect(AFFECT_DEX, POINT_DX, item->GetValue(2), 0, item->GetValue(1), 0, true);
									break;

								case APPLY_CON:
									AddAffect(AFFECT_CON, POINT_HT, item->GetValue(2), 0, item->GetValue(1), 0, true);
									break;

								case APPLY_INT:
									AddAffect(AFFECT_INT, POINT_IQ, item->GetValue(2), 0, item->GetValue(1), 0, true);
									break;

								case APPLY_CAST_SPEED:
									AddAffect(AFFECT_CAST_SPEED, POINT_CASTING_SPEED, item->GetValue(2), 0, item->GetValue(1), 0, true);
									break;

								case APPLY_ATT_GRADE_BONUS:
									AddAffect(AFFECT_ATT_GRADE, POINT_ATT_GRADE_BONUS,
											item->GetValue(2), 0, item->GetValue(1), 0, true);
									break;

								case APPLY_DEF_GRADE_BONUS:
									AddAffect(AFFECT_DEF_GRADE, POINT_DEF_GRADE_BONUS,
											item->GetValue(2), 0, item->GetValue(1), 0, true);
									break;
							}
						}

						if (GetWarMap())
							GetWarMap()->UsePotion(this, item);

						item->SetCount(item->GetCount() - 1);
						break;

					case USE_TALISMAN:
						{
							const int TOWN_PORTAL	= 1;
							const int MEMORY_PORTAL = 2;


							// gm_guild_build, oxevent �ʿ��� ��ȯ�� ��ȯ���� �� �����ϰ� ����
							if (GetMapIndex() == 200 || GetMapIndex() == 113)
							{
#ifdef TEXTS_IMPROVEMENT
								ChatPacketNew(CHAT_TYPE_INFO, 489, "");
#endif
								return false;
							}

							if (CArenaManager::instance().IsArenaMap(GetMapIndex()) == true)
							{
#ifdef TEXTS_IMPROVEMENT
								ChatPacketNew(CHAT_TYPE_INFO, 667, "");
#endif
								return false;
							}
#ifdef ENABLE_NEWSTUFF
							else if (g_NoPotionsOnPVP && CPVPManager::instance().IsFighting(GetPlayerID()) && !IsAllowedPotionOnPVP(item->GetVnum()))
							{
#ifdef TEXTS_IMPROVEMENT
								ChatPacketNew(CHAT_TYPE_INFO, 667, "");
#endif
								return false;
							}
#endif

							if (m_pkWarpEvent)
							{
#ifdef TEXTS_IMPROVEMENT
								ChatPacketNew(CHAT_TYPE_INFO, 434, "");
#endif
								return false;
							}

							// CONSUME_LIFE_WHEN_USE_WARP_ITEM
							int consumeLife = CalculateConsume(this);

							if (consumeLife < 0)
								return false;
							// END_OF_CONSUME_LIFE_WHEN_USE_WARP_ITEM

							if (item->GetValue(0) == TOWN_PORTAL) // ��ȯ��
							{
								if (item->GetSocket(0) == 0)
								{
									if (!GetDungeon())
										if (!GiveRecallItem(item))
											return false;

									PIXEL_POSITION posWarp;

									if (SECTREE_MANAGER::instance().GetRecallPositionByEmpire(GetMapIndex(), GetEmpire(), posWarp))
									{
										// CONSUME_LIFE_WHEN_USE_WARP_ITEM
										PointChange(POINT_HP, -consumeLife, false);
										// END_OF_CONSUME_LIFE_WHEN_USE_WARP_ITEM

										WarpSet(posWarp.x, posWarp.y);
									}
									else
									{
										sys_err("CHARACTER::UseItem : cannot find spawn position (name %s, %d x %d)", GetName(), GetX(), GetY());
									}
								}
								else
								{
#ifdef TEXTS_IMPROVEMENT
									if (test_server) {
										ChatPacketNew(CHAT_TYPE_INFO, 415, "");
									}
#endif
									ProcessRecallItem(item);
								}
							}
							else if (item->GetValue(0) == MEMORY_PORTAL) // ��ȯ����
							{
								if (item->GetSocket(0) == 0)
								{
									if (GetDungeon())
									{
#ifdef TEXTS_IMPROVEMENT
										ChatPacketNew(CHAT_TYPE_INFO, 310, "%s", item->GetName());
#endif
										return false;
									}

									if (!GiveRecallItem(item))
										return false;
								}
								else
								{
									// CONSUME_LIFE_WHEN_USE_WARP_ITEM
									PointChange(POINT_HP, -consumeLife, false);
									// END_OF_CONSUME_LIFE_WHEN_USE_WARP_ITEM

									ProcessRecallItem(item);
								}
							}
						}
						break;
#ifdef ENABLE_ATTR_COSTUMES
					case USE_CHANGE_ATTR_COSTUME:
						{
							LPITEM item2;
							if ((!IsValidItemPosition(DestCell)) || (!(item2 = GetItem(DestCell))))
								return false;
							
							if (item2->IsEquipped())
								BuffOnAttr_RemoveBuffsFromItem(item2);
							
							if (item2->GetType() != ITEM_COSTUME)
							{
#ifdef TEXTS_IMPROVEMENT
								ChatPacketNew(CHAT_TYPE_INFO, 396, "");
#endif
								return false;
							}
							
							if ((item2->GetSubType() != COSTUME_BODY) && (item2->GetSubType() != COSTUME_HAIR) && (item2->GetSubType() != COSTUME_WEAPON)) {
#ifdef TEXTS_IMPROVEMENT
								ChatPacketNew(CHAT_TYPE_INFO, 396, "");
#endif
								return false;
							}
							
							if ((item2->IsExchanging()) || (item2->IsEquipped()))
								return false;
							
							if (item2->GetAttributeSetIndex() == -1)
							{
#ifdef TEXTS_IMPROVEMENT
								ChatPacketNew(CHAT_TYPE_INFO, 396, "");
#endif
								return false;
							}
							else if (item2->GetAttributeCount() == 0)
							{
#ifdef TEXTS_IMPROVEMENT
								ChatPacketNew(CHAT_TYPE_INFO, 354, "");
#endif
								return false;
							}
							
							item2->ChangeAttribute();
							
							{
								char buf[21];
								snprintf(buf, sizeof(buf), "%u", item2->GetID());
								LogManager::instance().ItemLog(this, item, "CHANGE_COSTUME_ATTR", buf);
							}
							
#ifdef TEXTS_IMPROVEMENT
							ChatPacketNew(CHAT_TYPE_INFO, 392, "");
#endif
							item->SetCount(item->GetCount() - 1);
							break;
						}
					case USE_ADD_ATTR_COSTUME1:
					case USE_ADD_ATTR_COSTUME2:
						{
							LPITEM item2;
							if ((!IsValidItemPosition(DestCell)) || (!(item2 = GetItem(DestCell))))
								return false;
							
							if (item2->IsEquipped())
								BuffOnAttr_RemoveBuffsFromItem(item2);
							
							if (item2->GetType() != ITEM_COSTUME)
							{
#ifdef TEXTS_IMPROVEMENT
								ChatPacketNew(CHAT_TYPE_INFO, 396, "");
#endif
								return false;
							}
							
							if ((item2->GetSubType() != COSTUME_BODY) && (item2->GetSubType() != COSTUME_HAIR) && (item2->GetSubType() != COSTUME_WEAPON)) {
#ifdef TEXTS_IMPROVEMENT
								ChatPacketNew(CHAT_TYPE_INFO, 396, "");
#endif
								return false;
							}
							
							if ((item2->IsExchanging()) || (item2->IsEquipped()))
								return false;
							
							if (item2->GetAttributeSetIndex() == -1)
							{
#ifdef TEXTS_IMPROVEMENT
								ChatPacketNew(CHAT_TYPE_INFO, 396, "");
#endif
								return false;
							}
							
							if ((item2->GetAttributeType(ITEM_ATTRIBUTE_MAX_NUM-2) != 0) && (item2->GetAttributeType(ITEM_ATTRIBUTE_MAX_NUM-1) != 0))
							{
#ifdef TEXTS_IMPROVEMENT
								ChatPacketNew(CHAT_TYPE_INFO, 87, "");
#endif
								return false;
							}
							
							BYTE bAttrSocket = item2->GetAttributeType(ITEM_ATTRIBUTE_MAX_NUM-2) == 0 ? ITEM_ATTRIBUTE_MAX_NUM-2 : ITEM_ATTRIBUTE_MAX_NUM-1;
							BYTE bAttrSocketCheck = bAttrSocket == ITEM_ATTRIBUTE_MAX_NUM-2 ? ITEM_ATTRIBUTE_MAX_NUM-1 : ITEM_ATTRIBUTE_MAX_NUM-2;
							if (item2->GetAttributeType(bAttrSocketCheck) == item->GetSocket(0))
							{
#ifdef TEXTS_IMPROVEMENT
								ChatPacketNew(CHAT_TYPE_INFO, 88, "");
#endif
								return false;
							}
							
							item2->SetForceAttribute(bAttrSocket, item->GetSocket(0), item->GetSocket(1));
							
							{
								char buf[21];
								snprintf(buf, sizeof(buf), "%u", item2->GetID());
								LogManager::instance().ItemLog(this, item, "ADD_COSTUME_ATTR", buf);
							}

#ifdef TEXTS_IMPROVEMENT
							ChatPacketNew(CHAT_TYPE_INFO, 677, "");
#endif
							item->SetCount(item->GetCount() - 1);
							break;
						}
					case USE_REMOVE_ATTR_COSTUME:
						{
							LPITEM item2;
							if ((!IsValidItemPosition(DestCell)) || (!(item2 = GetItem(DestCell))))
								return false;
							
							if (item2->IsEquipped())
								BuffOnAttr_RemoveBuffsFromItem(item2);
							
							if (item2->GetType() != ITEM_COSTUME)
							{
#ifdef TEXTS_IMPROVEMENT
								ChatPacketNew(CHAT_TYPE_INFO, 396, "");
#endif
								return false;
							}
							
							if ((item2->GetSubType() != COSTUME_BODY) && (item2->GetSubType() != COSTUME_HAIR) && (item2->GetSubType() != COSTUME_WEAPON)) {
#ifdef TEXTS_IMPROVEMENT
								ChatPacketNew(CHAT_TYPE_INFO, 396, "");
#endif
								return false;
							}
							
							if ((item2->IsExchanging()) || (item2->IsEquipped()))
								return false;
							
							if (item2->GetAttributeSetIndex() == -1)
							{
#ifdef TEXTS_IMPROVEMENT
								ChatPacketNew(CHAT_TYPE_INFO, 396, "");
#endif
								return false;
							}
							
							if ((item2->GetAttributeType(ITEM_ATTRIBUTE_MAX_NUM-2) == 0) && (item2->GetAttributeType(ITEM_ATTRIBUTE_MAX_NUM-1) == 0))
							{
#ifdef TEXTS_IMPROVEMENT
								ChatPacketNew(CHAT_TYPE_INFO, 89, "");
#endif
								return false;
							}
							
							int iAttrSocket = GetAttrDialogRemove();
							if ((iAttrSocket == 0) && (item2->GetAttributeType(ITEM_ATTRIBUTE_MAX_NUM-1) != 0))
							{
								item2->SetForceAttribute(ITEM_ATTRIBUTE_MAX_NUM-2, item2->GetAttributeType(ITEM_ATTRIBUTE_MAX_NUM-1), item2->GetAttributeValue(ITEM_ATTRIBUTE_MAX_NUM-1));
								item2->SetForceAttribute(ITEM_ATTRIBUTE_MAX_NUM-1, 0, 0);
							}
							else
								item2->SetForceAttribute(ITEM_ATTRIBUTE_MAX_NUM-2+iAttrSocket, 0, 0);
							
							{
								char buf[21];
								snprintf(buf, sizeof(buf), "%u", item2->GetID());
								LogManager::instance().ItemLog(this, item, "REMOVE_COSTUME_ATTR", buf);
							}
							
#ifdef TEXTS_IMPROVEMENT
							ChatPacketNew(CHAT_TYPE_INFO, 90, "");
#endif
							item->SetCount(item->GetCount() - 1);
							break;
						}
#endif
#ifdef ENABLE_STOLE_COSTUME
					case USE_ENCHANT_STOLE: {
						LPITEM item2;
						if ((!IsValidItemPosition(DestCell)) || (!(item2 = GetItem(DestCell))))
							return false;
						
						if ((item2->GetType() != ITEM_COSTUME) || (item2->GetSubType() != COSTUME_STOLE)) {
#ifdef TEXTS_IMPROVEMENT
							ChatPacketNew(CHAT_TYPE_INFO, 22, "%s", item->GetName());
#endif
							return false;
						}
						
						if ((item2->IsExchanging()) || (item2->IsEquipped()) || (item2->isLocked()))
							return false;
						
						BYTE bGrade = item2->GetValue(0);
						if (bGrade < 1)
							return false;
						
						bGrade = bGrade > 4 ? 4 : bGrade;
						BYTE bRandom = (bGrade * 4);
						for (int i = 0; i < MAX_ATTR; i++) {
							item2->SetForceAttribute(i, stoleInfoTable[i][0], stoleInfoTable[i][number(bRandom - 3, bRandom)]);
						}
						
#ifdef TEXTS_IMPROVEMENT
						ChatPacketNew(CHAT_TYPE_INFO, 21, "%s", item2->GetName());
#endif
						item->SetCount(item->GetCount() - 1);
						break;
					}
#endif
#ifdef ENABLE_DS_ENCHANT
					case USE_DS_ENCHANT: {
							LPITEM item2;
							if ((!IsValidItemPosition(DestCell)) || (!(item2 = GetItem(DestCell))))
								return false;
							
							if (!item2->IsDragonSoul()) {
#ifdef TEXTS_IMPROVEMENT
								ChatPacketNew(CHAT_TYPE_INFO, 73, "");
#endif
								return false;
							}
							
							if ((DragonSoul_IsDeckActivated()) && (item2->IsEquipped())) {
#ifdef TEXTS_IMPROVEMENT
								ChatPacketNew(CHAT_TYPE_INFO, 76, "");
#endif
								return false;
							}
							
							if (item2->IsExchanging() /*|| item2->IsEquipped()*/) // ENABLE_BUG_FIXES
								return false;
							
							int iGrade = (item2->GetVnum() / 1000) % 10, iStep = (item2->GetVnum() / 100) % 10;
							if ((iGrade != 
#ifdef ENABLE_DS_GRADE_MYTH
							DRAGON_SOUL_GRADE_MYTH
#else
							DRAGON_SOUL_GRADE_LEGENDARY
#endif
							) || (iStep != DRAGON_SOUL_STEP_HIGHEST)) {
#ifdef TEXTS_IMPROVEMENT
								ChatPacketNew(CHAT_TYPE_INFO, 75, "");
#endif
								return false;
							}
							
							for (int i = 0; i < ITEM_ATTRIBUTE_RARE_END; i++)
								item2->SetForceAttribute(i, 0, 0);
							
							bool bRet = DSManager::instance().PutAttributes(item2);
							if (!bRet)
								return false;
							
							{
								char buf[21];
								snprintf(buf, sizeof(buf), "%u", item2->GetID());
								LogManager::instance().ItemLog(this, item, "USE_DS_ENCHANT", buf);
							}
							
#ifdef TEXTS_IMPROVEMENT
							ChatPacketNew(CHAT_TYPE_INFO, 74, "");
#endif
							item->SetCount(item->GetCount() - 1);
							break;
					}
#endif
#ifdef ENABLE_REMOTE_ATTR_SASH_REMOVE
					case USE_ATTR_SASH_REMOVE: {
							LPITEM item2;
							if ((!IsValidItemPosition(DestCell)) || (!(item2 = GetItem(DestCell))))
								return false;
							
							if ((item2->IsExchanging()) || (item2->IsEquipped()))
								return false;
							
							if ((item2->GetType() == ITEM_COSTUME) && (item2->GetSubType() == COSTUME_ACCE)) {
								if (item2->GetSocket(ACCE_ABSORBED_SOCKET) <= 0) {
#ifdef TEXTS_IMPROVEMENT
									ChatPacketNew(CHAT_TYPE_INFO, 71, "");
#endif
									return false;
								}
								
								bool bClean = CleanAcceAttr(item, item2);
								if (bClean) {
									{
										char buf[21];
										snprintf(buf, sizeof(buf), "%u", item2->GetID());
										LogManager::instance().ItemLog(this, item, "USE_ATTR_SASH_REMOVE", buf);
									}
									
#ifdef TEXTS_IMPROVEMENT
									ChatPacketNew(CHAT_TYPE_INFO, 72, "");
#endif
								}
								
								return bClean;
							}
							else {
#ifdef TEXTS_IMPROVEMENT
								ChatPacketNew(CHAT_TYPE_INFO, 70, "");
#endif
								return false;
							}
					}
#endif
#ifdef ENABLE_NEW_PET_EDITS
					case USE_PET_REVIVE: {
							LPITEM item2;
							if ((!IsValidItemPosition(DestCell)) || (!(item2 = GetItem(DestCell))))
								return false;
							
							if ((item2->GetVnum() < 55701) || (item2->GetVnum() > 55711)) {
#ifdef TEXTS_IMPROVEMENT
								ChatPacketNew(CHAT_TYPE_INFO, 66, "");
#endif
								return false;
							}
							
							if (item2->GetSocket(0) != 0) {
#ifdef TEXTS_IMPROVEMENT
								ChatPacketNew(CHAT_TYPE_INFO, 67, "");
#endif
								return false;
							}
							
							if (item2->GetSocket(2) == 0) {
#ifdef TEXTS_IMPROVEMENT
								ChatPacketNew(CHAT_TYPE_INFO, 64, "");
#endif
								return false;
							}
							
							if (item2->IsExchanging() || item2->IsEquipped()) // ENABLE_BUG_FIXES
								return false;
							
							if (item2->GetSocket(1) > int(1440*365)) {
#ifdef TEXTS_IMPROVEMENT
								ChatPacketNew(CHAT_TYPE_INFO, 69, "");
#endif
								return false;
							}
							
							int iLimit = int(1440*365);
							int iValue = item->GetValue(0);
							int iNewDuration = iValue == 0 ? 1440 * 366 : 1440 * iValue;
							iNewDuration += item2->GetSocket(1);
							if ((iNewDuration >= iLimit) && (item->GetVnum() != 86074)) {
#ifdef TEXTS_IMPROVEMENT
								ChatPacketNew(CHAT_TYPE_INFO, 68, "");
#endif
								return false;
							}
							
							iNewDuration = iNewDuration > iLimit ? iLimit : iNewDuration;
							if (item->GetVnum() == 86074)
								iNewDuration = 1440 * 366;
							
							item2->SetSocket(1, iNewDuration);
							item2->SetSocket(2, iNewDuration);
							std::unique_ptr<SQLMsg> msg(DBManager::instance().DirectQuery("UPDATE player.new_petsystem SET duration = %d, tduration = %d WHERE id = %lu ", iNewDuration, iNewDuration, item2->GetID()));
							
							{
								char buf[21];
								snprintf(buf, sizeof(buf), "%u", item2->GetID());
								LogManager::instance().ItemLog(this, item, "USE_PET_REVIVE", buf);
							}
							
#ifdef TEXTS_IMPROVEMENT
							ChatPacketNew(CHAT_TYPE_INFO, 65, "");
#endif
							item->SetCount(item->GetCount() - 1);
							break;
					}
					case USE_PET_ENCHANT: {
							LPITEM item2;
							if ((!IsValidItemPosition(DestCell)) || (!(item2 = GetItem(DestCell))))
								return false;
							
							if ((item2->GetVnum() < 55701) || (item2->GetVnum() > 55711)) {
#ifdef TEXTS_IMPROVEMENT
								ChatPacketNew(CHAT_TYPE_INFO, 66, "");
#endif
								return false;
							}
							
							if (item2->GetSocket(0) != 0) {
#ifdef TEXTS_IMPROVEMENT
								ChatPacketNew(CHAT_TYPE_INFO, 67, "");
#endif
								return false;
							}
							
							if (item2->IsExchanging() || item2->IsEquipped()) // ENABLE_BUG_FIXES
								return false;
							
							int idx = GetPetEnchant();
							if ((idx < 0) || (idx > 2))
								return false;
							
							int iValue = item2->GetAttributeValue(idx);
							if ((idx == 0) && (iValue >= 150)) {
#ifdef TEXTS_IMPROVEMENT
								ChatPacketNew(CHAT_TYPE_INFO, 63, "");
#endif
								return false;
							}
							
							if ((idx == 1) && (iValue >= 100)) {
#ifdef TEXTS_IMPROVEMENT
								ChatPacketNew(CHAT_TYPE_INFO, 63, "");
#endif
								return false;
							}
							
							if ((idx == 2) && (iValue >= 100)) {
#ifdef TEXTS_IMPROVEMENT
								ChatPacketNew(CHAT_TYPE_INFO, 63, "");
#endif
								return false;
							}
							
							{
								char buf[21];
								snprintf(buf, sizeof(buf), "%u", item2->GetID());
								LogManager::instance().ItemLog(this, item, "USE_PET_ENCHANT", buf);
							}
							
							if (number(1, 100) > 70) {
								int iMax;
								if (idx == 0)
									iMax = iValue + 5 > 150 ? 150 : iValue + 5;
								else
									iMax = iValue + 5 > 100 ? 100 : iValue + 5;
								
								item2->SetForceAttribute(idx, 1, iMax);
								std::unique_ptr<SQLMsg> msg(DBManager::instance().DirectQuery("UPDATE player.new_petsystem SET bonus%d = %d WHERE id = %lu ", idx, iMax, item2->GetID()));
								
#ifdef TEXTS_IMPROVEMENT
								ChatPacketNew(CHAT_TYPE_INFO, 61, "");
#endif
							}
#ifdef TEXTS_IMPROVEMENT
							else {
								ChatPacketNew(CHAT_TYPE_INFO, 62, "");
							}
#endif
							
							item->SetCount(item->GetCount() - 1);
							break;
					}
#endif
					case USE_TUNING:
					case USE_DETACHMENT:
						{
							LPITEM item2;

							if (!IsValidItemPosition(DestCell) || !(item2 = GetItem(DestCell)))
								return false;

							if (item2->IsExchanging() || item2->IsEquipped()) // @fixme114
								return false;

							if (item2->GetVnum() >= 28330 && item2->GetVnum() <= 28343) // ����+3
							{
#ifdef TEXTS_IMPROVEMENT
								ChatPacketNew(CHAT_TYPE_INFO, 678, "%s", item->GetName());
#endif
								return false;
							}

#ifdef ENABLE_BUG_FIXES
							if (item2->IsEquipped())
								return false;
#endif

#ifdef ENABLE_ACCE_SYSTEM
							if (item->GetValue(0) == ACCE_CLEAN_ATTR_VALUE0)
							{
								if (!CleanAcceAttr(item, item2))
									return false;

								return true;
							}
#endif
							if (item2->GetVnum() >= 28430 && item2->GetVnum() <= 28443)  // ����+4
							{
								if (item->GetVnum() == 71056) // û���Ǽ���
								{
									RefineItem(item, item2);
								}
#ifdef TEXTS_IMPROVEMENT
								else {
									ChatPacketNew(CHAT_TYPE_INFO, 679, "%s", item->GetName());
								}
#endif
							}
							else
							{
								RefineItem(item, item2);
							}
						}
						break;
#ifdef ATTR_LOCK						
					case USE_ADD_ATTRIBUTE_LOCK:
						{
							LPITEM item2;
							if (!IsValidItemPosition(DestCell) || !(item2 = GetItem(DestCell)))
								return false;
							
							if (ITEM_COSTUME == item2->GetType() || item2->GetWearFlag() == WEARABLE_PENDANT || item2->IsDragonSoul())
							{
#ifdef TEXTS_IMPROVEMENT
								ChatPacketNew(CHAT_TYPE_INFO, 791, "");
#endif
								return false;
							}
							
							if (item2->GetAttributeCount() < 5)
							{
#ifdef TEXTS_IMPROVEMENT
								ChatPacketNew(CHAT_TYPE_INFO, 792, "");
#endif
								return false;
							}
							
							if (item2->GetLockedAttr() != -1)
							{
#ifdef TEXTS_IMPROVEMENT
								ChatPacketNew(CHAT_TYPE_INFO, 793, "");
#endif
								return false;
							}
							
							if (item2->IsEquipped())
							{
								BuffOnAttr_RemoveBuffsFromItem(item2);
							}

							if (item2->IsExchanging() || item2->IsEquipped()) // @fixme114
								return false;
							
							item2->AddLockedAttr();
							item->SetCount(item->GetCount() - 1);
						}
					break;
					case USE_CHANGE_ATTRIBUTE_LOCK:
						{
							LPITEM item2;
							if (!IsValidItemPosition(DestCell) || !(item2 = GetItem(DestCell)))
								return false;
							
							
							if (item2->GetLockedAttr() == -1)
							{
#ifdef TEXTS_IMPROVEMENT
								ChatPacketNew(CHAT_TYPE_INFO, 795, "");
#endif
								return false;
							}
							
							if (ITEM_COSTUME == item2->GetType() /*|| item2->GetWearFlag() == WEARABLE_PENDANT*/ || item2->IsDragonSoul())
							{
#ifdef TEXTS_IMPROVEMENT
								ChatPacketNew(CHAT_TYPE_INFO, 791, "");
#endif
								return false;
							}
							
							if (item2->IsEquipped())
							{
								BuffOnAttr_RemoveBuffsFromItem(item2);
							}
							

							if (item2->IsExchanging() || item2->IsEquipped()) // @fixme114
								return false;
								
						
							item2->ChangeLockedAttr();
							item->SetCount(item->GetCount() - 1);
						}
					break;
					case USE_DELETE_ATTRIBUTE_LOCK:
						{
							LPITEM item2;
							if (!IsValidItemPosition(DestCell) || !(item2 = GetItem(DestCell)))
								return false;
							
							if (item2->GetLockedAttr() == -1)
							{
#ifdef TEXTS_IMPROVEMENT
								ChatPacketNew(CHAT_TYPE_INFO, 795, "");
#endif
								return false;
							}	
							
							if (item2->IsEquipped())
							{
								BuffOnAttr_RemoveBuffsFromItem(item2);
							}
							
							if (ITEM_COSTUME == item2->GetType()  || item2->IsDragonSoul())
							{
#ifdef TEXTS_IMPROVEMENT
								ChatPacketNew(CHAT_TYPE_INFO, 680, "");
#endif
								return false;
							}

							if (item2->IsExchanging() || item2->IsEquipped()) // @fixme114
								return false;
						
							item2->RemoveLockedAttr();
							item->SetCount(item->GetCount() - 1);
						}
					break;
#endif
					case USE_CHANGE_COSTUME_ATTR:
					case USE_RESET_COSTUME_ATTR:
						{
							LPITEM item2;
							if (!IsValidItemPosition(DestCell) || !(item2 = GetItem(DestCell)))
								return false;

							if (item2->IsEquipped())
							{
								BuffOnAttr_RemoveBuffsFromItem(item2);
							}

							if (ITEM_COSTUME != item2->GetType())
							{
#ifdef TEXTS_IMPROVEMENT
								ChatPacketNew(CHAT_TYPE_INFO, 396, "");
#endif
								return false;
							}
							
							{
								BYTE bSubType = item2->GetSubType();
#ifdef ENABLE_ACCE_SYSTEM
								if (bSubType == COSTUME_ACCE)
									return false;
#endif
							
#ifdef ENABLE_STOLE_COSTUME
								if (bSubType == COSTUME_STOLE)
									return false;
#endif
							}
							
							if (item2->IsExchanging() || item2->IsEquipped()) // @fixme114
								return false;

							if (item2->GetAttributeSetIndex() == -1)
							{
#ifdef TEXTS_IMPROVEMENT
								ChatPacketNew(CHAT_TYPE_INFO, 396, "");
#endif
								return false;
							}

							if (item2->GetAttributeCount() == 0)
							{
#ifdef TEXTS_IMPROVEMENT
								ChatPacketNew(CHAT_TYPE_INFO, 354, "");
#endif
								return false;
							}

							switch (item->GetSubType())
							{
								case USE_CHANGE_COSTUME_ATTR:
									item2->ChangeAttribute();
									{
										char buf[21];
										snprintf(buf, sizeof(buf), "%u", item2->GetID());
										LogManager::instance().ItemLog(this, item, "CHANGE_COSTUME_ATTR", buf);
									}
									break;
								case USE_RESET_COSTUME_ATTR:
									item2->ClearAttribute();
									item2->AlterToMagicItem();
									{
										char buf[21];
										snprintf(buf, sizeof(buf), "%u", item2->GetID());
										LogManager::instance().ItemLog(this, item, "RESET_COSTUME_ATTR", buf);
									}
									break;
							}

#ifdef TEXTS_IMPROVEMENT
							ChatPacketNew(CHAT_TYPE_INFO, 392, "");
#endif
							item->SetCount(item->GetCount() - 1);
							break;
						}

						//  ACCESSORY_REFINE & ADD/CHANGE_ATTRIBUTES
					case USE_PUT_INTO_BELT_SOCKET:
					case USE_PUT_INTO_RING_SOCKET:
					case USE_PUT_INTO_ACCESSORY_SOCKET:
					case USE_ADD_ACCESSORY_SOCKET:
					case USE_CLEAN_SOCKET:
					case USE_CHANGE_ATTRIBUTE:
					case USE_CHANGE_ATTRIBUTE2 :
					case USE_ADD_ATTRIBUTE:
					case USE_ADD_ATTRIBUTE2:
						{
							LPITEM item2;
							if (!IsValidItemPosition(DestCell) || !(item2 = GetItem(DestCell)))
								return false;

							if (item2->IsEquipped())
							{
								BuffOnAttr_RemoveBuffsFromItem(item2);
							}

							// [NOTE] �ڽ�Ƭ �����ۿ��� ������ ���� ������ ���� �Ӽ��� �ο��ϵ�, ����簡 ����� ���ƴ޶�� ��û�� �־���.
							// ���� ANTI_CHANGE_ATTRIBUTE ���� ������ Flag�� �߰��Ͽ� ��ȹ �������� �����ϰ� ��Ʈ�� �� �� �ֵ��� �� �����̾�����
							// �׵��� �ʿ������ ��ġ�� ���� �ش޷��� �׳� ���⼭ ����... -_-
							if (ITEM_COSTUME == item2->GetType())
							{
#ifdef TEXTS_IMPROVEMENT
								ChatPacketNew(CHAT_TYPE_INFO, 396, "");
#endif
								return false;
							}

							if (item2->IsExchanging() || item2->IsEquipped()) // @fixme114
								return false;

							switch (item->GetSubType())
							{
								case USE_CLEAN_SOCKET:
									{
										int i;
										for (i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
										{
											if (item2->GetSocket(i) == ITEM_BROKEN_METIN_VNUM)
												break;
										}

										if (i == ITEM_SOCKET_MAX_NUM)
										{
#ifdef TEXTS_IMPROVEMENT
											ChatPacketNew(CHAT_TYPE_INFO, 480, "");
#endif
											return false;
										}

										int j = 0;

										for (i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
										{
											if (item2->GetSocket(i) != ITEM_BROKEN_METIN_VNUM && item2->GetSocket(i) != 0)
												item2->SetSocket(j++, item2->GetSocket(i));
										}

										for (; j < ITEM_SOCKET_MAX_NUM; ++j)
										{
											if (item2->GetSocket(j) > 0)
												item2->SetSocket(j, 1);
										}

										{
											char buf[21];
											snprintf(buf, sizeof(buf), "%u", item2->GetID());
											LogManager::instance().ItemLog(this, item, "CLEAN_SOCKET", buf);
										}

										item->SetCount(item->GetCount() - 1);

									}
									break;

								case USE_CHANGE_ATTRIBUTE :
								case USE_CHANGE_ATTRIBUTE2 : // @fixme123
									if (item2->GetAttributeSetIndex() == -1)
									{
#ifdef TEXTS_IMPROVEMENT
										ChatPacketNew(CHAT_TYPE_INFO, 396, "");
#endif
										return false;
									}

									if (item2->GetAttributeCount() == 0)
									{
#ifdef TEXTS_IMPROVEMENT
										ChatPacketNew(CHAT_TYPE_INFO, 354, "");
#endif
										return false;
									}

									if ((GM_PLAYER == GetGMLevel()) && (false == test_server) && (g_dwItemBonusChangeTime > 0))
									{
										//
										// Event Flag �� ���� ������ ������ �Ӽ� ������ �� �ð����� ���� ����� �ð��� �귶���� �˻��ϰ�
										// �ð��� ����� �귶�ٸ� ���� �Ӽ����濡 ���� �ð��� ������ �ش�.
										//

										// DWORD dwChangeItemAttrCycle = quest::CQuestManager::instance().GetEventFlag(msc_szChangeItemAttrCycleFlag);
										// if (dwChangeItemAttrCycle < msc_dwDefaultChangeItemAttrCycle)
											// dwChangeItemAttrCycle = msc_dwDefaultChangeItemAttrCycle;
										DWORD dwChangeItemAttrCycle = g_dwItemBonusChangeTime;

										quest::PC* pPC = quest::CQuestManager::instance().GetPC(GetPlayerID());

										if (pPC)
										{
											DWORD dwNowSec = get_global_time();

											DWORD dwLastChangeItemAttrSec = pPC->GetFlag(msc_szLastChangeItemAttrFlag);

											if (dwLastChangeItemAttrSec + dwChangeItemAttrCycle > dwNowSec)
											{
#ifdef TEXTS_IMPROVEMENT
												ChatPacketNew(CHAT_TYPE_INFO, 391, "%d#%d", dwChangeItemAttrCycle, dwChangeItemAttrCycle - (dwNowSec - dwLastChangeItemAttrSec));
#endif
												return false;
											}

											pPC->SetFlag(msc_szLastChangeItemAttrFlag, dwNowSec);
										}
									}
									
#ifdef ENABLE_CHANGE_ATTRIBUTE_RULES
									{
										DWORD dwTargetVnum = item2->GetVnum();
										bool bZodiacItem = (
															((dwTargetVnum >= 19290) && (dwTargetVnum <= 19312)) || 
															((dwTargetVnum >= 19490) && (dwTargetVnum <= 19512)) || 
															((dwTargetVnum >= 19690) && (dwTargetVnum <= 19712)) || 
															((dwTargetVnum >= 19890) && (dwTargetVnum <= 19912)) || 
															((dwTargetVnum >= 300) && (dwTargetVnum <= 319)) || 
															(dwTargetVnum == 329) || 
															(dwTargetVnum == 339) || 
															(dwTargetVnum == 349) || 
															(dwTargetVnum == 359) || 
															(dwTargetVnum == 369) || 
															(dwTargetVnum == 379) || 
															(dwTargetVnum == 389) || 
															(dwTargetVnum == 399) || 
															((dwTargetVnum >= 1180) && (dwTargetVnum <= 1189)) || 
															(dwTargetVnum == 1199) || 
															(dwTargetVnum == 1209) || 
															(dwTargetVnum == 1219) || 
															(dwTargetVnum == 1229) || 
															((dwTargetVnum >= 2200) && (dwTargetVnum <= 2209)) || 
															(dwTargetVnum == 2219) || 
															(dwTargetVnum == 2229) || 
															(dwTargetVnum == 2239) || 
															(dwTargetVnum == 2249) || 
															((dwTargetVnum >= 3220) && (dwTargetVnum <= 3229)) || 
															(dwTargetVnum == 3239) || 
															(dwTargetVnum == 3249) || 
															(dwTargetVnum == 3259) || 
															(dwTargetVnum == 3269) || 
															((dwTargetVnum >= 5160) && (dwTargetVnum <= 5169)) || 
															(dwTargetVnum == 5179) || 
															(dwTargetVnum == 5189) || 
															(dwTargetVnum == 5199) || 
															(dwTargetVnum == 5209) || 
															((dwTargetVnum >= 7300) && (dwTargetVnum <= 7309)) || 
															(dwTargetVnum == 7319) || 
															(dwTargetVnum == 7329) || 
															(dwTargetVnum == 7339) || 
															(dwTargetVnum == 7349)
															) ? true : false;
										if (item->GetVnum() != 86060) {
											if (bZodiacItem) {
#ifdef TEXTS_IMPROVEMENT
												ChatPacketNew(CHAT_TYPE_INFO, 10, "%s", item2->GetName());
#endif
												return false;
											}
										}
										else if (!bZodiacItem) {
#ifdef TEXTS_IMPROVEMENT
											ChatPacketNew(CHAT_TYPE_INFO, 9, "%s", item2->GetName());
#endif
											return false;
										}
									}
#endif
									
#ifdef ENABLE_TALISMAN_ATTR
									if (item->GetVnum() == 86051 || item->GetVnum() == 88965)
									{
										if (item2->GetType() == ITEM_ARMOR && item2->GetSubType() == ARMOR_PENDANT)
										{
											item2->ChangeAttribute();
											item->SetCount(item->GetCount() - 1);
#ifdef ENABLE_RANKING
											SetRankPoints(13, GetRankPoints(13) + 1);
#endif
											return true;
										}
										else
										{
#ifdef TEXTS_IMPROVEMENT
											ChatPacketNew(CHAT_TYPE_INFO, 681, "");
#endif
											return false;
										}
									}
									else if (item2->GetType() == ITEM_ARMOR && item2->GetSubType() == ARMOR_PENDANT)
									{
#ifdef TEXTS_IMPROVEMENT
										ChatPacketNew(CHAT_TYPE_INFO, 850, "");
#endif
										return false;
									}
#endif

									if (item->GetSubType() == USE_CHANGE_ATTRIBUTE2)
									{
										int aiChangeProb[ITEM_ATTRIBUTE_MAX_LEVEL] =
										{
											0, 0, 30, 40, 3
										};

										item2->ChangeAttribute(aiChangeProb);
									}
									else if (item->GetVnum() == 76014)
									{
										int aiChangeProb[ITEM_ATTRIBUTE_MAX_LEVEL] =
										{
											0, 10, 50, 39, 1
										};

										item2->ChangeAttribute(aiChangeProb);
									}
									else
									{
										// ����� Ư��ó��
										// ����� ���簡 �߰� �ȵɰŶ� �Ͽ� �ϵ� �ڵ���.
										if (item->GetVnum() == 71151 || item->GetVnum() == 76023)
										{
											if ((item2->GetType() == ITEM_WEAPON)
												|| (item2->GetType() == ITEM_ARMOR && item2->GetSubType() == ARMOR_BODY)
#ifdef __USE_ADD_WITH_ALL_ITEMS__
												|| (item2->GetType() == ITEM_ARMOR && item2->GetSubType() == ARMOR_HEAD)
												|| (item2->GetType() == ITEM_ARMOR && item2->GetSubType() == ARMOR_SHIELD)
												|| (item2->GetType() == ITEM_ARMOR && item2->GetSubType() == ARMOR_WRIST)
												|| (item2->GetType() == ITEM_ARMOR && item2->GetSubType() == ARMOR_FOOTS)
												|| (item2->GetType() == ITEM_ARMOR && item2->GetSubType() == ARMOR_NECK)
												|| (item2->GetType() == ITEM_ARMOR && item2->GetSubType() == ARMOR_EAR)
#endif
											)
											{
												bool bCanUse = true;
												for (int i = 0; i < ITEM_LIMIT_MAX_NUM; ++i)
												{
#ifdef __ENABLE_GREEN_ITEM_LVL_30__
													if (item2->GetLimitType(i) == LIMIT_LEVEL && item2->GetLimitValue(i) > 30)
#else
													if (item2->GetLimitType(i) == LIMIT_LEVEL && item2->GetLimitValue(i) > 40)
#endif
													{
														bCanUse = false;
														break;
													}
												}
												if (false == bCanUse)
												{
#ifdef TEXTS_IMPROVEMENT
#ifdef __ENABLE_GREEN_ITEM_LVL_30__
													int iLimit = 30;
#else
													int iLimit = 40;
#endif
													ChatPacketNew(CHAT_TYPE_INFO, 682, "%d", iLimit);
#endif
													break;
												}
											}
											else
											{
#ifdef TEXTS_IMPROVEMENT
												ChatPacketNew(CHAT_TYPE_INFO, 683, "");
#endif
												break;
											}
										}
										
										
#ifdef ENABLE_BATTLE_PASS
										BYTE bBattlePassId = GetBattlePassId();
										if(bBattlePassId)
										{
											DWORD dwItemVnum, dwUseCount;
											if(CBattlePass::instance().BattlePassMissionGetInfo(bBattlePassId, USE_ITEM, &dwItemVnum, &dwUseCount))
											{
												if(dwItemVnum == item->GetVnum() && GetMissionProgress(USE_ITEM, bBattlePassId) < dwUseCount)
													UpdateMissionProgress(USE_ITEM, bBattlePassId, 1, dwUseCount);
											}
										}
#endif
										item2->ChangeAttribute();
									}

#ifdef TEXTS_IMPROVEMENT
									ChatPacketNew(CHAT_TYPE_INFO, 392, "");
#endif

									{
										char buf[21];
										snprintf(buf, sizeof(buf), "%u", item2->GetID());
										LogManager::instance().ItemLog(this, item, "CHANGE_ATTRIBUTE", buf);
									}
									
									item->SetCount(item->GetCount() - 1);
#ifdef ENABLE_RANKING
									if (item->GetVnum() == 86051 || item->GetVnum() == 88965)
										SetRankPoints(13, GetRankPoints(13) + 1);
									else
										SetRankPoints(12, GetRankPoints(12) + 1);
#endif
									break;

								case USE_ADD_ATTRIBUTE :
									if (item2->GetAttributeSetIndex() == -1)
									{
#ifdef TEXTS_IMPROVEMENT
										ChatPacketNew(CHAT_TYPE_INFO, 396, "");
#endif
										return false;
									}

									if (item2->GetAttributeCount() < 5)
									{
#ifdef ENABLE_TALISMAN_ATTR
										if (item->GetVnum() == 86050 || item->GetVnum() == 88966) {
											if (item2->GetType() == ITEM_ARMOR && item2->GetSubType() == ARMOR_PENDANT)
											{
#if defined(ENABLE_BUG_FIXES)
												if (item2->GetAttributeCount() == 4)
												{
#if defined(TEXTS_IMPROVEMENT)
													ChatPacketNew(CHAT_TYPE_INFO, 1359, "");
#endif
													return false;
												}
#endif

												item2->AddAttribute();
												item->SetCount(item->GetCount() -1);
												return true;
											}
											else
											{
#ifdef TEXTS_IMPROVEMENT
												ChatPacketNew(CHAT_TYPE_INFO, 681, "");
#endif
												return false;
											}
										}
										else if (item2->GetType() == ITEM_ARMOR && item2->GetSubType() == ARMOR_PENDANT)
										{
#ifdef TEXTS_IMPROVEMENT
											ChatPacketNew(CHAT_TYPE_INFO, 684, "");
#endif
											return false;
										}
#endif

										// ���簡 Ư��ó��
										// ����� ���簡 �߰� �ȵɰŶ� �Ͽ� �ϵ� �ڵ���.
										if (item->GetVnum() == 71152 || item->GetVnum() == 76024)
										{
											if ((item2->GetType() == ITEM_WEAPON)
												|| (item2->GetType() == ITEM_ARMOR && item2->GetSubType() == ARMOR_BODY)
#ifdef __USE_ADD_WITH_ALL_ITEMS__
												|| (item2->GetType() == ITEM_ARMOR && item2->GetSubType() == ARMOR_HEAD)
												|| (item2->GetType() == ITEM_ARMOR && item2->GetSubType() == ARMOR_SHIELD)
												|| (item2->GetType() == ITEM_ARMOR && item2->GetSubType() == ARMOR_WRIST)
												|| (item2->GetType() == ITEM_ARMOR && item2->GetSubType() == ARMOR_FOOTS)
												|| (item2->GetType() == ITEM_ARMOR && item2->GetSubType() == ARMOR_NECK)
												|| (item2->GetType() == ITEM_ARMOR && item2->GetSubType() == ARMOR_EAR)
#endif
											)
											{
												bool bCanUse = true;
												for (int i = 0; i < ITEM_LIMIT_MAX_NUM; ++i)
												{
#ifdef __ENABLE_GREEN_ITEM_LVL_30__
													if (item2->GetLimitType(i) == LIMIT_LEVEL && item2->GetLimitValue(i) > 30)
#else
													if (item2->GetLimitType(i) == LIMIT_LEVEL && item2->GetLimitValue(i) > 40)
#endif
													{
														bCanUse = false;
														break;
													}
												}
												if (false == bCanUse)
												{
#ifdef TEXTS_IMPROVEMENT
#ifdef __ENABLE_GREEN_ITEM_LVL_30__
													int iLimit = 30;
#else
													int iLimit = 40;
#endif
													ChatPacketNew(CHAT_TYPE_INFO, 682, "%d", iLimit);
#endif
													break;
												}
											}
											else
											{
#ifdef TEXTS_IMPROVEMENT
												ChatPacketNew(CHAT_TYPE_INFO, 683, "");
#endif
												break;
											}
										}
										char buf[21];
										snprintf(buf, sizeof(buf), "%u", item2->GetID());
#ifndef ENABLE_ENCHANT_CHANGES
										if (number(1, 100) <= aiItemAttributeAddPercent[item2->GetAttributeCount()])
#endif
										{
#ifdef ENABLE_MAX_ADD_ATTRIBUTE
											short AttributeCount = abs(5 - item->GetAttributeCount());
											for (int i = 0; i < AttributeCount; i++)
												item2->AddAttribute();
#else
											item2->AddAttribute();
#endif
#ifdef TEXTS_IMPROVEMENT
											ChatPacketNew(CHAT_TYPE_INFO, 389, "");
#endif
											int iAddedIdx = item2->GetAttributeCount() - 1;
											LogManager::instance().ItemLog(
													GetPlayerID(),
													item2->GetAttributeType(iAddedIdx),
													item2->GetAttributeValue(iAddedIdx),
													item->GetID(),
													"ADD_ATTRIBUTE_SUCCESS",
													buf,
													GetDesc()->GetHostName(),
													item->GetOriginalVnum());
										}
#ifndef ENABLE_ENCHANT_CHANGES
										else
										{
#ifdef TEXTS_IMPROVEMENT
											ChatPacketNew(CHAT_TYPE_INFO, 390, "");
#endif
											LogManager::instance().ItemLog(this, item, "ADD_ATTRIBUTE_FAIL", buf);
										}
										
										item->SetCount(item->GetCount() - 1);
#endif
									}
#ifdef TEXTS_IMPROVEMENT
									else {
										ChatPacketNew(CHAT_TYPE_INFO, 308, "");
									}
#endif
									break;

								case USE_ADD_ATTRIBUTE2 :
									// �ູ�� ����
									// �簡�񼭸� ���� �Ӽ��� 4�� �߰� ��Ų �����ۿ� ���ؼ� �ϳ��� �Ӽ��� �� �ٿ��ش�.
									if (item2->GetAttributeSetIndex() == -1)
									{
#ifdef TEXTS_IMPROVEMENT
										ChatPacketNew(CHAT_TYPE_INFO, 396, "");
#endif
										return false;
									}

									// �Ӽ��� �̹� 4�� �߰� �Ǿ��� ���� �Ӽ��� �߰� �����ϴ�.
									if (item2->GetAttributeCount() == 4)
									{
#ifdef ENABLE_TALISMAN_ATTR
										if (item->GetVnum() == 86052 || item->GetVnum() == 88964)
										{
											if (item2->GetType() == ITEM_ARMOR && item2->GetSubType() == ARMOR_PENDANT)
											{
												if (number(1, 100) <= 75) // % Successo di inserimeno Sfera Benedetta 75%
												{
													item2->AddAttribute();
													item->SetCount(item->GetCount() - 1);
													return true;
												}
												else
												{
													item->SetCount(item->GetCount() - 1);
#ifdef TEXTS_IMPROVEMENT
													ChatPacketNew(CHAT_TYPE_INFO, 390, "");
#endif
													return false;
												}
											}
											else
											{
#ifdef TEXTS_IMPROVEMENT
												ChatPacketNew(CHAT_TYPE_INFO, 681, "");
#endif
												return false;
											}
										}
										else if (item2->GetType() == ITEM_ARMOR && item2->GetSubType() == ARMOR_PENDANT)
										{
#ifdef TEXTS_IMPROVEMENT
											ChatPacketNew(CHAT_TYPE_INFO, 684, "");
#endif
											return false;
										}
#endif
										
										
										char buf[21];
										snprintf(buf, sizeof(buf), "%u", item2->GetID());

										if (number(1, 100) <= aiItemAttributeAddPercent[item2->GetAttributeCount()])
										{
#ifdef ENABLE_MAX_ADD_ATTRIBUTE
											short AttributeCount = abs(1 - item->GetAttributeCount());
											for (int i = 0; i < AttributeCount; i++)
												item2->AddAttribute();
#else
											item2->AddAttribute();
#endif
#ifdef TEXTS_IMPROVEMENT
											ChatPacketNew(CHAT_TYPE_INFO, 389, "");
#endif
											int iAddedIdx = item2->GetAttributeCount() - 1;
											LogManager::instance().ItemLog(
													GetPlayerID(),
													item2->GetAttributeType(iAddedIdx),
													item2->GetAttributeValue(iAddedIdx),
													item->GetID(),
													"ADD_ATTRIBUTE2_SUCCESS",
													buf,
													GetDesc()->GetHostName(),
													item->GetOriginalVnum());
										}
										else
										{
#ifdef TEXTS_IMPROVEMENT
											ChatPacketNew(CHAT_TYPE_INFO, 390, "");
#endif
											LogManager::instance().ItemLog(this, item, "ADD_ATTRIBUTE2_FAIL", buf);
										}

										item->SetCount(item->GetCount() - 1);
									}
									else if (item2->GetAttributeCount() == 5)
									{
#ifdef TEXTS_IMPROVEMENT
										ChatPacketNew(CHAT_TYPE_INFO, 308, "");
#endif
									}
									else if (item2->GetAttributeCount() < 4)
									{
#ifdef TEXTS_IMPROVEMENT
										ChatPacketNew(CHAT_TYPE_INFO, 339, "%d#%d#%d", 4, item2->GetAttributeCount(), 4);
#endif
									}
									else
									{
										// wtf ?!
										sys_err("ADD_ATTRIBUTE2 : Item has wrong AttributeCount(%d)", item2->GetAttributeCount());
									}
									break;

								case USE_ADD_ACCESSORY_SOCKET:
									{
										char buf[21];
										snprintf(buf, sizeof(buf), "%u", item2->GetID());

										if (item2->IsAccessoryForSocket())
										{
											if (item2->GetAccessorySocketMaxGrade() < ITEM_ACCESSORY_SOCKET_MAX_NUM)
											{
#ifdef ENABLE_ADDSTONE_FAILURE
												if (number(1, 100) <= 50)
#else
												if (1)
#endif
												{
													item2->SetAccessorySocketMaxGrade(item2->GetAccessorySocketMaxGrade() + 1);
#ifdef TEXTS_IMPROVEMENT
													ChatPacketNew(CHAT_TYPE_INFO, 387, "");
#endif
													LogManager::instance().ItemLog(this, item, "ADD_SOCKET_SUCCESS", buf);
												}
												else
												{
#ifdef TEXTS_IMPROVEMENT
													ChatPacketNew(CHAT_TYPE_INFO, 386, "");
#endif
													LogManager::instance().ItemLog(this, item, "ADD_SOCKET_FAIL", buf);
												}

												item->SetCount(item->GetCount() - 1);
											}
#ifdef TEXTS_IMPROVEMENT
											else {
												ChatPacketNew(CHAT_TYPE_INFO, 428, "");
											}
#endif
										}
										else
										{
#ifdef TEXTS_IMPROVEMENT
											ChatPacketNew(CHAT_TYPE_INFO, 425, "");
#endif
										}
									}
									break;

								case USE_PUT_INTO_BELT_SOCKET:
								case USE_PUT_INTO_ACCESSORY_SOCKET:
									if (item2->IsAccessoryForSocket())
									{
										if (item->CanPutInto(item2)) {
#ifdef ENABLE_INFINITE_RAFINES
											if (item2->GetSocket(0) > 86400 || item2->GetSocket(1) > 86400 || item2->GetSocket(2) > 86400) {
#ifdef TEXTS_IMPROVEMENT
												ChatPacketNew(CHAT_TYPE_INFO, 859, "");
#endif
												return false;
											}
#endif
											char buf[21];
											snprintf(buf, sizeof(buf), "%u", item2->GetID());

											if (item2->GetAccessorySocketGrade() < item2->GetAccessorySocketMaxGrade())
											{
												//if (number(1, 100) <= aiAccessorySocketPutPct[item2->GetAccessorySocketGrade()])
												//{
													item2->SetAccessorySocketGrade(item2->GetAccessorySocketGrade() + 1);
#ifdef TEXTS_IMPROVEMENT
													ChatPacketNew(CHAT_TYPE_INFO, 452, "");
#endif
													LogManager::instance().ItemLog(this, item, "PUT_SOCKET_SUCCESS", buf);
												//}
												//else
												//{
//#ifdef TEXTS_IMPROVEMENT
													//ChatPacketNew(CHAT_TYPE_INFO, 453, "");
//#endif
													//LogManager::instance().ItemLog(this, item, "PUT_SOCKET_FAIL", buf);
												//}

												item->SetCount(item->GetCount() - 1);
											}
											else
											{
#ifdef TEXTS_IMPROVEMENT
												if (item2->GetAccessorySocketMaxGrade() == 0 || item2->GetAccessorySocketMaxGrade() < ITEM_ACCESSORY_SOCKET_MAX_NUM) {
													ChatPacketNew(CHAT_TYPE_INFO, 297, "");
													ChatPacketNew(CHAT_TYPE_INFO, 298, "");
												} else {
													ChatPacketNew(CHAT_TYPE_INFO, 337, "");
												}
#endif
											}
										}
#ifdef ENABLE_INFINITE_RAFINES
										else if (item->CanPutInto2(item2)) {
											if ((item2->GetSocket(0) > 5 && item2->GetSocket(0) <= 86400) || (item2->GetSocket(1) > 5 && item2->GetSocket(1) <= 86400) || (item2->GetSocket(2) > 5 && item2->GetSocket(2) <= 86400)) {
#ifdef TEXTS_IMPROVEMENT
												ChatPacketNew(CHAT_TYPE_INFO, 860, "");
#endif
												return false;
											}
											
											char buf[21];
											snprintf(buf, sizeof(buf), "%u", item2->GetID());

											if (item2->GetAccessorySocketGrade() < item2->GetAccessorySocketMaxGrade())
											{
												bool infinite = item->GetValue(0) == 1 ? true : false;
												if (infinite == true)
												{
													item2->SetAccessorySocketGrade(item2->GetAccessorySocketGrade() + 1, infinite);
#ifdef TEXTS_IMPROVEMENT
													ChatPacketNew(CHAT_TYPE_INFO, 452, "");
#endif
													LogManager::instance().ItemLog(this, item, "PUT_SOCKET_SUCCESS", buf);
												}
												else
												{
#ifdef TEXTS_IMPROVEMENT
													ChatPacketNew(CHAT_TYPE_INFO, 453, "");
#endif
													LogManager::instance().ItemLog(this, item, "PUT_SOCKET_FAIL", buf);
												}

												item->SetCount(item->GetCount() - 1);
											}
											else
											{
#ifdef TEXTS_IMPROVEMENT
												if (item2->GetAccessorySocketMaxGrade() == 0 || item2->GetAccessorySocketMaxGrade() < ITEM_ACCESSORY_SOCKET_MAX_NUM) {
													ChatPacketNew(CHAT_TYPE_INFO, 297, "");
													ChatPacketNew(CHAT_TYPE_INFO, 298, "");
												} else {
													ChatPacketNew(CHAT_TYPE_INFO, 337, "");
												}
#endif
											}
										}
#endif
										else {
											ChatPacketNew(CHAT_TYPE_INFO, 425, "");
										}
									}
									else {
#ifdef TEXTS_IMPROVEMENT
										ChatPacketNew(CHAT_TYPE_INFO, 425, "");
#endif
									}
									break;
							}
							if (item2->IsEquipped())
							{
								BuffOnAttr_AddBuffsFromItem(item2);
							}
						}
						break;
						//  END_OF_ACCESSORY_REFINE & END_OF_ADD_ATTRIBUTES & END_OF_CHANGE_ATTRIBUTES

					case USE_BAIT:
						{

							if (m_pkFishingEvent 
#ifdef ENABLE_NEW_FISHING_SYSTEM
							|| m_pkFishingNewEvent
#endif
							)
							{
#ifdef TEXTS_IMPROVEMENT
								ChatPacketNew(CHAT_TYPE_INFO, 277, "");
#endif
								return false;
							}

							LPITEM weapon = GetWear(WEAR_WEAPON);

							if (!weapon || weapon->GetType() != ITEM_ROD)
								return false;

#ifdef TEXTS_IMPROVEMENT
							if (weapon->GetSocket(2)) {
								ChatPacketNew(CHAT_TYPE_INFO, 898, "%s", item->GetName());
							} else {
								ChatPacketNew(CHAT_TYPE_INFO, 282, "%s", item->GetName());
							}
#endif
							weapon->SetSocket(2, item->GetValue(0));
							item->SetCount(item->GetCount() - 1);
						}
						break;

					case USE_MOVE:
					case USE_TREASURE_BOX:
					case USE_MONEYBAG:
						break;

					case USE_AFFECT :
						{
							if (FindAffect(item->GetValue(0), aApplyInfo[item->GetValue(1)].bPointType)) {
#ifdef TEXTS_IMPROVEMENT
								ChatPacketNew(CHAT_TYPE_INFO, 442, "");
#endif
							}
							else
							{
								// PC_BANG_ITEM_ADD
								if (item->IsPCBangItem() == true)
								{
									// PC������ üũ�ؼ� ó��
									if (CPCBangManager::instance().IsPCBangIP(GetDesc()->GetHostName()) == false)
									{
#ifdef TEXTS_IMPROVEMENT
										ChatPacketNew(CHAT_TYPE_INFO, 426, "");
#endif
										return false;
									}
								}
								// END_PC_BANG_ITEM_ADD

								AddAffect(item->GetValue(0), aApplyInfo[item->GetValue(1)].bPointType, item->GetValue(2), 0, item->GetValue(3), 0, false);
								item->SetCount(item->GetCount() - 1);
							}
						}
						break;

					case USE_CREATE_STONE:
						AutoGiveItem(number(28000, 28013));
						item->SetCount(item->GetCount() - 1);
						break;

					// ���� ���� ��ų�� ������ ó��
					case USE_RECIPE :
						{
							LPITEM pSource1 = FindSpecifyItem(item->GetValue(1));
							int dwSourceCount1 = item->GetValue(2);

							LPITEM pSource2 = FindSpecifyItem(item->GetValue(3));
							int dwSourceCount2 = item->GetValue(4);

							if (dwSourceCount1 != 0)
							{
								if (pSource1 == NULL)
								{
#ifdef TEXTS_IMPROVEMENT
									ChatPacketNew(CHAT_TYPE_INFO, 350, "");
#endif
									return false;
								}
							}

							if (dwSourceCount2 != 0)
							{
								if (pSource2 == NULL)
								{
#ifdef TEXTS_IMPROVEMENT
									ChatPacketNew(CHAT_TYPE_INFO, 350, "");
#endif
									return false;
								}
							}

							if (pSource1 != NULL)
							{
								if (pSource1->GetCount() < dwSourceCount1)
								{
#ifdef TEXTS_IMPROVEMENT
									ChatPacketNew(CHAT_TYPE_INFO, 454, "%s#%d#%d", pSource1->GetName(), dwSourceCount1, pSource1->GetCount());
#endif
									return false;
								}
								
								pSource1->SetCount(pSource1->GetCount() - dwSourceCount1);
							}

							if (pSource2 != NULL)
							{
								if (pSource2->GetCount() < dwSourceCount2)
								{
#ifdef TEXTS_IMPROVEMENT
									ChatPacketNew(CHAT_TYPE_INFO, 454, "%s#%d#%d", pSource2->GetName(), dwSourceCount1, pSource2->GetCount());
#endif
									return false;
								}

								pSource2->SetCount(pSource2->GetCount() - dwSourceCount2);
							}

							LPITEM pBottle = FindSpecifyItem(50901);

							if (!pBottle || pBottle->GetCount() < 1)
							{
#ifdef TEXTS_IMPROVEMENT
								ChatPacketNew(CHAT_TYPE_INFO, 359, "");
#endif
								return false;
							}

							pBottle->SetCount(pBottle->GetCount() - 1);

							if (number(1, 100) > item->GetValue(5))
							{
#ifdef TEXTS_IMPROVEMENT
								ChatPacketNew(CHAT_TYPE_INFO, 347, "");
#endif
								return false;
							}

							AutoGiveItem(item->GetValue(0));
						}
						break;
				}
			}
			break;

		case ITEM_METIN:
			{
				LPITEM item2;

				if (!IsValidItemPosition(DestCell) || !(item2 = GetItem(DestCell)))
					return false;

				if (item2->IsExchanging() || item2->IsEquipped()) // @fixme114
					return false;

				if (item2->GetType() == ITEM_PICK) return false;
				if (item2->GetType() == ITEM_ROD) return false;

				int i;

				for (i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
				{
					DWORD dwVnum;

					if ((dwVnum = item2->GetSocket(i)) <= 2)
						continue;

					TItemTable * p = ITEM_MANAGER::instance().GetTable(dwVnum);

					if (!p)
						continue;

					if (item->GetValue(5) == p->alValues[5])
					{
#ifdef TEXTS_IMPROVEMENT
						ChatPacketNew(CHAT_TYPE_INFO, 230, "");
#endif
						return false;
					}
				}

				if (item2->GetType() == ITEM_ARMOR)
				{
					if (!IS_SET(item->GetWearFlag(), WEARABLE_BODY) || !IS_SET(item2->GetWearFlag(), WEARABLE_BODY))
					{
#ifdef TEXTS_IMPROVEMENT
						ChatPacketNew(CHAT_TYPE_INFO, 420, "%s", item->GetName());
#endif
						return false;
					}
				}
				else if (item2->GetType() == ITEM_WEAPON)
				{
					if (!IS_SET(item->GetWearFlag(), WEARABLE_WEAPON))
					{
#ifdef TEXTS_IMPROVEMENT
						ChatPacketNew(CHAT_TYPE_INFO, 419, "%s", item->GetName());
#endif
						return false;
					}
				}
				else
				{
#ifdef TEXTS_IMPROVEMENT
					ChatPacketNew(CHAT_TYPE_INFO, 357, "");
#endif
					return false;
				}

				for (i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
					if (item2->GetSocket(i) >= 1 && item2->GetSocket(i) <= 2 && item2->GetSocket(i) >= item->GetValue(2))
					{
						// �� Ȯ��
#ifdef ENABLE_ADDSTONE_FAILURE
						if (number(1, 100) <= 30)
#else
						if (1)
#endif
						{
#ifdef TEXTS_IMPROVEMENT
							ChatPacketNew(CHAT_TYPE_INFO, 340, "");
#endif
							item2->SetSocket(i, item->GetVnum());
						}
						else
						{
							ChatPacketNew(CHAT_TYPE_INFO, 341, "");
							item2->SetSocket(i, ITEM_BROKEN_METIN_VNUM);
						}

						LogManager::instance().ItemLog(this, item2, "SOCKET", item->GetName());
#ifdef ENABLE_BUG_FIXES
						item->SetCount(item->GetCount() - 1);
#else
#ifdef ENABLE_STONE_STACKFIX
						item->SetCount(item->GetCount() - 1);
#else
						ITEM_MANAGER::instance().RemoveItem(item, "REMOVE (METIN)");
#endif
#endif
						break;
					}

				if (i == ITEM_SOCKET_MAX_NUM)
#ifdef TEXTS_IMPROVEMENT
					ChatPacketNew(CHAT_TYPE_INFO, 357, "%s", item2->GetName());
#endif
			}
			break;

		case ITEM_AUTOUSE:
		case ITEM_MATERIAL:
		case ITEM_SPECIAL:
		case ITEM_TOOL:
		case ITEM_LOTTERY:
			break;

		case ITEM_TOTEM:
			{
				if (!item->IsEquipped())
					EquipItem(item);
			}
			break;

		case ITEM_BLEND:
			// ���ο� ���ʵ�
			sys_log(0,"ITEM_BLEND!!");
			if (Blend_Item_find(item->GetVnum()))
			{
				int		affect_type		= AFFECT_BLEND;
				int		apply_type		= aApplyInfo[item->GetSocket(0)].bPointType;
				int		apply_value		= item->GetSocket(1);
				int		apply_duration	= item->GetSocket(2);

				if (FindAffect(affect_type, apply_type)) {
#ifdef TEXTS_IMPROVEMENT
					ChatPacketNew(CHAT_TYPE_INFO, 442, "");
#endif
				}
				else
				{
					if (FindAffect(AFFECT_EXP_BONUS_EURO_FREE, POINT_RESIST_MAGIC)) {
#ifdef TEXTS_IMPROVEMENT
						ChatPacketNew(CHAT_TYPE_INFO, 442, "");
#endif
					}
					else
					{
#ifdef ENABLE_BUG_FIXES
						if (!m_bIsLoadedAffect) {
							return false;
						}
#endif

						AddAffect(affect_type, apply_type, apply_value, 0, apply_duration, 0, false);
						item->SetCount(item->GetCount() - 1);
					}
				}
			}
			break;
		case ITEM_EXTRACT:
			{
				LPITEM pDestItem = GetItem(DestCell);
				if (NULL == pDestItem)
				{
					return false;
				}
				switch (item->GetSubType())
				{
				case EXTRACT_DRAGON_SOUL:
					if (pDestItem->IsDragonSoul())
					{
						return DSManager::instance().PullOut(this, NPOS, pDestItem, item);
					}
					return false;
				case EXTRACT_DRAGON_HEART:
					if (pDestItem->IsDragonSoul())
					{
						return DSManager::instance().ExtractDragonHeart(this, pDestItem, item);
					}
					return false;
				default:
					return false;
				}
			}
			break;

#ifdef ENABLE_SOUL_SYSTEM
		case ITEM_SOUL:
			{
				int iCurrentMinutes = (item->GetSocket(2) / 10000);
				int iCurrentStrike = (item->GetSocket(2) % 10000);
				
				if(iCurrentMinutes < 60)
				{
#ifdef TEXTS_IMPROVEMENT
					ChatPacketNew(CHAT_TYPE_INFO, 685, "");
#endif
					return false;
				}
				
				if(iCurrentStrike <= 0)
				{
#ifdef TEXTS_IMPROVEMENT
					ChatPacketNew(CHAT_TYPE_INFO, 686, "");
#endif
					return false;
				}
					
				BYTE bSoulType = item->GetSubType();
				if(bSoulType >= SOUL_MAX_NUM)
					return false;
				
				int iAffectID = AFFECT_SOUL_RED + bSoulType;
				int iAffID = AFF_SOUL_RED + bSoulType;
				
				bool blockUse = false;
				const CAffect* pAffect = FindAffect(iAffectID);
				if(pAffect)
				{
					DWORD dwSPCost = pAffect->lSPCost;
					if(item->GetID() == dwSPCost)
					{
						blockUse = true;
					}
					
					LPITEM currentItem = FindItemByID(pAffect->lSPCost);
					if(currentItem)
					{
						currentItem->Lock(false);
						currentItem->SetSocket(1, false);
					}
					
					RemoveAffect(const_cast<CAffect*>(pAffect));
				}
				
				if(!blockUse)
				{
					item->Lock(true);
					item->SetSocket(1, true);
					
					AddAffect(iAffectID, APPLY_NONE, 0, iAffID, INFINITE_AFFECT_DURATION, item->GetID(), true, false);
				}
			}
			break;
#endif

		case ITEM_NONE:
			sys_err("Item type NONE %s", item->GetName());
			break;

		default:
			sys_log(0, "UseItemEx: Unknown type %s %d", item->GetName(), item->GetType());
			return false;
	}

	return true;
}

int g_nPortalLimitTime = 10;

bool CHARACTER::UseItem(TItemPos Cell, TItemPos DestCell)
{
	WORD wCell = Cell.cell;
	BYTE window_type = Cell.window_type;
	//WORD wDestCell = DestCell.cell;
	//BYTE bDestInven = DestCell.window_type;
	LPITEM item;

	if (!CanHandleItem())
		return false;

	if (!IsValidItemPosition(Cell) || !(item = GetItem(Cell)))
			return false;

	LPITEM destItem = GetItem(DestCell);
	if (destItem && item != destItem && destItem->IsStackable() && !IS_SET(destItem->GetAntiFlag(), ITEM_ANTIFLAG_STACK) && destItem->GetVnum() == item->GetVnum())
	{
		if (MoveItem(Cell, DestCell, 0))
			return false;
	}

#ifdef ENABLE_BUG_FIXES
	if (quest::CQuestManager::instance().GetPCForce(GetPlayerID())->IsRunning() == true)
	{
#ifdef TEXTS_IMPROVEMENT
		ChatPacketNew(CHAT_TYPE_INFO, 1247, "");
#endif
		//if (GetDesc()) {
		//	GetDesc()->DelayedDisconnect(3);
		//}
		return false;
	}
#endif

	sys_log(0, "%s: USE_ITEM %s (inven %d, cell: %d)", GetName(), item->GetName(), window_type, wCell);

	if (item->IsExchanging())
		return false;
#ifdef ENABLE_SWITCHBOT
	if (Cell.IsSwitchbotPosition())
	{
		CSwitchbot* pkSwitchbot = CSwitchbotManager::Instance().FindSwitchbot(GetPlayerID());
		if (pkSwitchbot && pkSwitchbot->IsActive(Cell.cell))
		{
			return false;
		}

		int iEmptyCell = GetEmptyInventory(item->GetSize());

		if (iEmptyCell == -1)
		{
#ifdef TEXTS_IMPROVEMENT
			ChatPacketNew(CHAT_TYPE_INFO, 687, "");
#endif
			return false;
		}

		MoveItem(Cell, TItemPos(INVENTORY, iEmptyCell), item->GetCount());
		return true;
	}
#endif
	if (!item->CanUsedBy(this))
	{
#ifdef TEXTS_IMPROVEMENT
		ChatPacketNew(CHAT_TYPE_INFO, 495, "");
#endif
		return false;
	}

	if (IsStun())
		return false;

	if (false == FN_check_item_sex(this, item))
	{
#ifdef TEXTS_IMPROVEMENT
		ChatPacketNew(CHAT_TYPE_INFO, 496, "");
#endif
		return false;
	}

#ifdef ENABLE_PVP_ADVANCED	
	if ((GetDuel("BlockPotion")) && IS_POTION_PVP_BLOCKED(item->GetVnum()))
	{
#ifdef TEXTS_IMPROVEMENT
		ChatPacketNew(CHAT_TYPE_INFO, 516, "");
#endif
		return false;
	}
#endif	

	//PREVENT_TRADE_WINDOW
	if (IS_SUMMON_ITEM(item->GetVnum()))
	{
		if (false == IS_SUMMONABLE_ZONE(GetMapIndex()))
		{
#ifdef TEXTS_IMPROVEMENT
		ChatPacketNew(CHAT_TYPE_INFO, 688, "");
#endif
			return false;
		}

		int iPulse = thecore_pulse();

		//â�� ���� üũ
		if (iPulse - GetSafeboxLoadTime() < PASSES_PER_SEC(g_nPortalLimitTime))
		{
#ifdef TEXTS_IMPROVEMENT
			ChatPacketNew(CHAT_TYPE_INFO, 234, "%d", g_nPortalLimitTime);
#endif
			return false;
		}

		//�ŷ����� â üũ
		if (GetExchange() || GetMyShop() || GetShopOwner() || IsOpenSafebox() || IsCubeOpen())
		{
#ifdef TEXTS_IMPROVEMENT
			ChatPacketNew(CHAT_TYPE_INFO, 235, "");
#endif
			return false;
		}
		
#ifdef __ATTR_TRANSFER_SYSTEM__
		if (IsAttrTransferOpen())
		{
#ifdef TEXTS_IMPROVEMENT
			ChatPacketNew(CHAT_TYPE_INFO, 235, "");
#endif
			return false;
		}
#endif
		
		//PREVENT_REFINE_HACK
		//������ �ð�üũ
		{
			if (iPulse - GetRefineTime() < PASSES_PER_SEC(g_nPortalLimitTime))
			{
#ifdef TEXTS_IMPROVEMENT
				ChatPacketNew(CHAT_TYPE_INFO, 234, "%d", g_nPortalLimitTime);
#endif
				return false;
			}
		}
		//END_PREVENT_REFINE_HACK


		//PREVENT_ITEM_COPY
		{
			if (iPulse - GetMyShopTime() < PASSES_PER_SEC(g_nPortalLimitTime))
			{
#ifdef TEXTS_IMPROVEMENT
				ChatPacketNew(CHAT_TYPE_INFO, 234, "%d", g_nPortalLimitTime);
#endif
				return false;
			}

		}
		//END_PREVENT_ITEM_COPY


		//��ȯ�� �Ÿ�üũ
		if (item->GetVnum() != 70302)
		{
			PIXEL_POSITION posWarp;

			int x = 0;
			int y = 0;

			double nDist = 0;
			const double nDistant = 5000.0;
			//��ȯ����
			if (item->GetVnum() == 22010)
			{
				x = item->GetSocket(0) - GetX();
				y = item->GetSocket(1) - GetY();
			}
			//��ȯ��
			else if (item->GetVnum() == 22000)
			{
				SECTREE_MANAGER::instance().GetRecallPositionByEmpire(GetMapIndex(), GetEmpire(), posWarp);

				if (item->GetSocket(0) == 0)
				{
					x = posWarp.x - GetX();
					y = posWarp.y - GetY();
				}
				else
				{
					x = item->GetSocket(0) - GetX();
					y = item->GetSocket(1) - GetY();
				}
			}

			nDist = sqrt(pow((float)x,2) + pow((float)y,2));
			if (nDistant > nDist) {
#ifdef TEXTS_IMPROVEMENT
				ChatPacketNew(CHAT_TYPE_INFO, 433, "");
#endif
				return false;
			}
		}

		//PREVENT_PORTAL_AFTER_EXCHANGE
		//��ȯ �� �ð�üũ
		if (iPulse - GetExchangeTime()  < PASSES_PER_SEC(g_nPortalLimitTime))
		{
#ifdef TEXTS_IMPROVEMENT
			ChatPacketNew(CHAT_TYPE_INFO, 234, "%d", g_nPortalLimitTime);
#endif
			return false;
		}
		//END_PREVENT_PORTAL_AFTER_EXCHANGE

	}

	//������ ��� ���� �ŷ�â ���� üũ
	if ((item->GetVnum() == 50200) || (item->GetVnum() == 71049)
#ifdef KASMIR_PAKET_SYSTEM
	|| (item->GetVnum() == 88901)
#endif
	)
	{
		if (GetExchange() || GetMyShop() || GetShopOwner() || IsOpenSafebox() || IsCubeOpen())
		{
#ifdef TEXTS_IMPROVEMENT
			ChatPacketNew(CHAT_TYPE_INFO, 237, "");
#endif
			return false;
		}
		
#ifdef __ATTR_TRANSFER_SYSTEM__
		if (IsAttrTransferOpen())
		{
#ifdef TEXTS_IMPROVEMENT
			ChatPacketNew(CHAT_TYPE_INFO, 237, "");
#endif
			return false;
		}
#endif
	}
	//END_PREVENT_TRADE_WINDOW

	if (IS_SET(item->GetFlag(), ITEM_FLAG_LOG)) // ��� �α׸� ����� ������ ó��
	{
		DWORD vid = item->GetVID();
		int oldCount = item->GetCount();
		DWORD vnum = item->GetVnum();

		char hint[ITEM_NAME_MAX_LEN + 48 + 1];
		int len = snprintf(hint, sizeof(hint) - 48, "%s", item->GetName());

		if (len < 0 || len >= (int) sizeof(hint) - 48)
			len = (sizeof(hint) - 48) - 1;

		bool ret = UseItemEx(item, DestCell);

		if (NULL == ITEM_MANAGER::instance().FindByVID(vid)) // UseItemEx���� �������� ���� �Ǿ���. ���� �α׸� ����
		{
			LogManager::instance().ItemLog(this, vid, vnum, "REMOVE", hint);
		}
		else if (oldCount != item->GetCount())
		{
			snprintf(hint + len, sizeof(hint) - len, " %u", oldCount - 1);
			LogManager::instance().ItemLog(this, vid, vnum, "USE_ITEM", hint);
		}
		return (ret);
	}
	else
		return UseItemEx(item, DestCell);
}

bool CHARACTER::DropItem(TItemPos Cell,
#ifdef ENABLE_NEW_STACK_LIMIT
int 
#else
BYTE 
#endif
bCount)
{
	bool stupid = false;
	if (bCount < 0)
	{
		sys_err("I am a stupid hacker 1: %s %d", GetName(), bCount);
		stupid = true;
	}

	bCount = abs(bCount);
	if (stupid)
	{
		sys_err("I am a stupid hacker 2: %s %d", GetName(), bCount);
		return false;
	}

	LPITEM item = NULL;

	if (!CanHandleItem())
	{
#ifdef TEXTS_IMPROVEMENT
		if (NULL != DragonSoul_RefineWindow_GetOpener()) {
			ChatPacketNew(CHAT_TYPE_INFO, 232, "");
		}
#endif

		return false;
	}

#ifdef ENABLE_ANTICHEAT
	if (thecore_pulse() > m_lastdropitem + 25)
	{
		m_dropitemcount = 0;
	}

	if (thecore_pulse() < m_lastdropitem + 25 && m_dropitemcount >= 4)
	{
		m_dropitemcount = 0;
		LPDESC desc = GetDesc();
		if (desc)
		{
			LogManager::instance().HackLog("DROP_HACK", this);
			desc->SetPhase(PHASE_CLOSE);
		}

		return false;
	}
#endif

	if (IsDead())
		return false;

	if (!IsValidItemPosition(Cell) || !(item = GetItem(Cell)))
		return false;

	if (item->isLocked() || item->IsExchanging() || item->IsEquipped())
		return false;

	if (quest::CQuestManager::instance().GetPCForce(GetPlayerID())->IsRunning() == true)
		return false;

	if (IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_DROP | ITEM_ANTIFLAG_GIVE))
	{
#ifdef TEXTS_IMPROVEMENT
		ChatPacketNew(CHAT_TYPE_INFO, 353, "");
#endif
		return false;
	}

	if (bCount == 0 || bCount > item->GetCount())
		bCount = item->GetCount();

#ifdef ENABLE_EXTRA_INVENTORY
	if (item->IsExtraItem()) {
		SyncQuickslot(QUICKSLOT_TYPE_ITEM_EXTRA, Cell.cell, 255);
	} else {
		SyncQuickslot(QUICKSLOT_TYPE_ITEM, Cell.cell, 255);
	}
#else
	SyncQuickslot(QUICKSLOT_TYPE_ITEM, Cell.cell, 255);
#endif

	LPITEM pkItemToDrop;

	if (bCount == item->GetCount())
	{
		item->RemoveFromCharacter();
		pkItemToDrop = item;
	}
	else
	{
		if (bCount == 0)
		{
			if (test_server)
				sys_log(0, "[DROP_ITEM] drop item count == 0");
			return false;
		}

		item->SetCount(item->GetCount() - bCount);
		ITEM_MANAGER::instance().FlushDelayedSave(item);

		pkItemToDrop = ITEM_MANAGER::instance().CreateItem(item->GetVnum(), bCount);

		// copy item socket -- by mhh
		FN_copy_item_socket(pkItemToDrop, item);

		char szBuf[51 + 1];
		snprintf(szBuf, sizeof(szBuf), "%u %u", pkItemToDrop->GetID(), pkItemToDrop->GetCount());
		LogManager::instance().ItemLog(this, item, "ITEM_SPLIT", szBuf);
	}

	PIXEL_POSITION pxPos = GetXYZ();

	if (pkItemToDrop->AddToGround(GetMapIndex(), pxPos))
	{
#ifdef TEXTS_IMPROVEMENT
		ChatPacketNew(CHAT_TYPE_INFO, 321, "%d",
#ifdef ENABLE_NEWSTUFF
		g_aiItemDestroyTime[ITEM_DESTROY_TIME_DROPITEM]
#else
		300
#endif
		);
#endif
		pkItemToDrop->StartDestroyEvent(
#ifdef ENABLE_NEWSTUFF
		g_aiItemDestroyTime[ITEM_DESTROY_TIME_DROPITEM]
#endif
		);

		ITEM_MANAGER::instance().FlushDelayedSave(pkItemToDrop);

		char szHint[32 + 1];
		snprintf(szHint, sizeof(szHint), "%s %u %u", pkItemToDrop->GetName(), pkItemToDrop->GetCount(), pkItemToDrop->GetOriginalVnum());
		LogManager::instance().ItemLog(this, pkItemToDrop, "DROP", szHint);
		//Motion(MOTION_PICKUP);
#ifdef ENABLE_ANTICHEAT
		m_lastdropitem = thecore_pulse();
		m_dropitemcount++;
#endif
	}

	return true;
}

bool CHARACTER::DropGold(int gold)
{
	if (gold <= 0 || gold > GetGold())
		return false;

	if (!CanHandleItem())
		return false;

	if (0 != g_GoldDropTimeLimitValue)
	{
		if (get_dword_time() < m_dwLastGoldDropTime+g_GoldDropTimeLimitValue)
		{
#ifdef TEXTS_IMPROVEMENT
			ChatPacketNew(CHAT_TYPE_INFO, 510, "");
#endif
			return false;
		}
	}

	m_dwLastGoldDropTime = get_dword_time();

	LPITEM item = ITEM_MANAGER::instance().CreateItem(1, gold);

	if (item)
	{
		PIXEL_POSITION pos = GetXYZ();

		if (item->AddToGround(GetMapIndex(), pos))
		{
			//Motion(MOTION_PICKUP);
			PointChange(POINT_GOLD, -gold, true);

			if (gold > 1000) // õ�� �̻� ����Ѵ�.
				LogManager::instance().CharLog(this, gold, "DROP_GOLD", "");

#ifdef ENABLE_NEWSTUFF
			item->StartDestroyEvent(g_aiItemDestroyTime[ITEM_DESTROY_TIME_DROPGOLD]);
#else
			item->StartDestroyEvent();
#endif
#ifdef TEXTS_IMPROVEMENT
			ChatPacketNew(CHAT_TYPE_INFO, 321, "%d", (150/60));
#endif
		}

		Save();
		return true;
	}

	return false;
}

bool CHARACTER::MoveItem(TItemPos Cell, TItemPos DestCell,
#ifdef ENABLE_NEW_STACK_LIMIT
int 
#else
BYTE 
#endif
count)
{
	bool stupid = false;
	if (count < 0)
	{
		sys_err("I am a stupid hacker 3: %s %d", GetName(), count);
		stupid = true;
	}

	count = abs(count);
	if (stupid)
	{
		sys_err("I am a stupid hacker 4: %s %d", GetName(), count);
		return false;
	}

	if (Cell.cell == DestCell.cell && Cell.window_type == DestCell.window_type)
	{
		return false;
	}

	LPITEM item = NULL;
	if (!IsValidItemPosition(Cell))
		return false;
	
	if (!(item = GetItem(Cell)))
		return false;
	
	if (item->IsExchanging())
		return false;
	
	if (item->GetCount() < count)
		return false;
	
	if (INVENTORY == Cell.window_type && Cell.cell >= INVENTORY_MAX_NUM && IS_SET(item->GetFlag(), ITEM_FLAG_IRREMOVABLE))
		return false;
	
	if (true == item->isLocked())
		return false;
	
	if (!IsValidItemPosition(DestCell))
		return false;

	if (!CanHandleItem())
	{
#ifdef TEXTS_IMPROVEMENT
		if (NULL != DragonSoul_RefineWindow_GetOpener()) {
			ChatPacketNew(CHAT_TYPE_INFO, 232, "");
		}
#endif
		
		return false;
	}
	
	// ��ȹ���� ��û���� ��Ʈ �κ��丮���� Ư�� Ÿ���� �����۸� ���� �� �ִ�.
	if (DestCell.IsBeltInventoryPosition() && false == CBeltInventoryHelper::CanMoveIntoBeltInventory(item))
	{
#ifdef TEXTS_IMPROVEMENT
		ChatPacketNew(CHAT_TYPE_INFO, 509, "");
#endif
		return false;
	}
	
#ifdef ENABLE_SWITCHBOT
	if (Cell.IsSwitchbotPosition() && CSwitchbotManager::Instance().IsActive(GetPlayerID(), Cell.cell))
	{
#ifdef TEXTS_IMPROVEMENT
		ChatPacketNew(CHAT_TYPE_INFO, 690, "");
#endif
		return false;
	}
	
	if ((DestCell.IsSwitchbotPosition() && item->IsEquipped()) || (Cell.IsSwitchbotPosition() && DestCell.IsEquipPosition()))
	{
		return false;
	}
	
	if (DestCell.IsSwitchbotPosition() && !SwitchbotHelper::IsValidItem(item))
	{
#ifdef TEXTS_IMPROVEMENT
		ChatPacketNew(CHAT_TYPE_INFO, 691, "");
#endif
		return false;
	}
#endif
	// �̹� �������� �������� �ٸ� ������ �ű�� ���, '��å ����' ������ �� Ȯ���ϰ� �ű�
	
	if (Cell.IsEquipPosition())
	{
		if (!CanUnequipNow(item))
			return false;

#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
		int iWearCell = item->FindEquipCell(this);
		if (iWearCell == WEAR_WEAPON)
		{
			LPITEM costumeWeapon = GetWear(WEAR_COSTUME_WEAPON);
			if (costumeWeapon && !UnequipItem(costumeWeapon))
			{
#ifdef TEXTS_IMPROVEMENT
				ChatPacketNew(CHAT_TYPE_INFO, 366, "");
#endif
				return false;
			}

			if (!IsEmptyItemGrid(DestCell, item->GetSize(), Cell.cell))
				return UnequipItem(item);
		}
#endif
	}
	
#ifdef ENABLE_EXTRA_INVENTORY
	if (!item->IsExtraItem() && DestCell.IsEquipPosition())
#else
	if (DestCell.IsEquipPosition())
#endif
	{
		if (GetItem(DestCell))	// ����� ��� �� ���� �˻��ص� �ȴ�.
		{
#ifdef TEXTS_IMPROVEMENT
			ChatPacketNew(CHAT_TYPE_INFO, 538, "");
#endif
			return false;
		}

		EquipItem(item, DestCell.cell - INVENTORY_MAX_NUM);
	}
	else
	{
		if (item->IsDragonSoul())
		{
			if (item->IsEquipped())
			{
				return DSManager::instance().PullOut(this, DestCell, item);
			}
			else
			{
				if (DestCell.window_type != DRAGON_SOUL_INVENTORY)
				{
					return false;
				}

				if (!DSManager::instance().IsValidCellForThisItem(item, DestCell))
				{
					return false;
				}
			}
		}
		else if (DestCell.window_type == DRAGON_SOUL_INVENTORY)
			return false;
		
		// ��ȥ���� �ƴ� �������� ��ȥ�� �κ��� �� �� ����.
#ifdef ENABLE_EXTRA_INVENTORY
		if (item->IsExtraItem())
		{
			if (DestCell.window_type != EXTRA_INVENTORY)
				return false;

			BYTE category = item->GetExtraCategory();
			if (DestCell.cell < category * EXTRA_INVENTORY_CATEGORY_MAX_NUM || DestCell.cell >= (category + 1) * EXTRA_INVENTORY_CATEGORY_MAX_NUM)
				return false;
		}
		else if (DestCell.window_type == EXTRA_INVENTORY)
			return false;
#else
		else if (DRAGON_SOUL_INVENTORY == DestCell.window_type)
			return false;
#endif

		LPITEM item2;

		if ((item2 = GetItem(DestCell)) && item != item2 && item2->IsStackable() &&
				!IS_SET(item2->GetAntiFlag(), ITEM_ANTIFLAG_STACK) &&
				item2->GetVnum() == item->GetVnum()) // ��ĥ �� �ִ� �������� ���
		{
			for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
				if (item2->GetSocket(i) != item->GetSocket(i))
					return false;

			if (count == 0)
				count = item->GetCount();

			sys_log(0, "%s: ITEM_STACK %s (window: %d, cell : %d) -> (window:%d, cell %d) count %d", GetName(), item->GetName(), Cell.window_type, Cell.cell,
				DestCell.window_type, DestCell.cell, count);

			count = MIN(g_bItemCountLimit - item2->GetCount(), count);

			item->SetCount(item->GetCount() - count);
			item2->SetCount(item2->GetCount() + count);
			return true;
		}

		if (!IsEmptyItemGrid(DestCell, item->GetSize(), Cell.cell))
			return false;

		if (count == 0 || count >= item->GetCount() || !item->IsStackable() || IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_STACK))
		{
			sys_log(0, "%s: ITEM_MOVE %s (window: %d, cell : %d) -> (window:%d, cell %d) count %d", GetName(), item->GetName(), Cell.window_type, Cell.cell,
				DestCell.window_type, DestCell.cell, count);

			item->RemoveFromCharacter();
			SetItem(DestCell, item
#ifdef __HIGHLIGHT_SYSTEM__
			, false
#endif
			);

			if (INVENTORY == Cell.window_type && INVENTORY == DestCell.window_type)
			{
				SyncQuickslot(QUICKSLOT_TYPE_ITEM, Cell.cell, DestCell.cell);
			}
#ifdef ENABLE_EXTRA_INVENTORY
			else if (EXTRA_INVENTORY == Cell.window_type && EXTRA_INVENTORY == DestCell.window_type) {
				SyncQuickslot(QUICKSLOT_TYPE_ITEM_EXTRA, Cell.cell, DestCell.cell);
			}
#endif
		}
		else if (count < item->GetCount())
		{

			sys_log(0, "%s: ITEM_SPLIT %s (window: %d, cell : %d) -> (window:%d, cell %d) count %d", GetName(), item->GetName(), Cell.window_type, Cell.cell,
				DestCell.window_type, DestCell.cell, count);

			item->SetCount(item->GetCount() - count);
			LPITEM item2 = ITEM_MANAGER::instance().CreateItem(item->GetVnum(), count);

			// copy socket -- by mhh
			FN_copy_item_socket(item2, item);

			item2->AddToCharacter(this, DestCell
#ifdef __HIGHLIGHT_SYSTEM__
			, false
#endif
			);

			char szBuf[51+1];
			snprintf(szBuf, sizeof(szBuf), "%u %u %u %u ", item2->GetID(), item2->GetCount(), item->GetCount(), item->GetCount() + item2->GetCount());
			LogManager::instance().ItemLog(this, item, "ITEM_SPLIT", szBuf);
		}
	}

	return true;
}

namespace NPartyPickupDistribute
{
	struct FFindOwnership
	{
		LPITEM item;
		LPCHARACTER owner;

		FFindOwnership(LPITEM item)
			: item(item), owner(NULL)
		{
		}

		void operator () (LPCHARACTER ch)
		{
			if (item->IsOwnership(ch))
				owner = ch;
		}
	};

	struct FCountNearMember
	{
		int		total;
		int		x, y;

		FCountNearMember(LPCHARACTER center )
			: total(0), x(center->GetX()), y(center->GetY())
		{
		}

		void operator () (LPCHARACTER ch)
		{
			if (DISTANCE_APPROX(ch->GetX() - x, ch->GetY() - y) <= PARTY_DEFAULT_RANGE)
				total += 1;
		}
	};

	struct FMoneyDistributor
	{
		int		total;
		LPCHARACTER	c;
		int		x, y;
		int		iMoney;

		FMoneyDistributor(LPCHARACTER center, int iMoney)
			: total(0), c(center), x(center->GetX()), y(center->GetY()), iMoney(iMoney)
		{
		}

		void operator ()(LPCHARACTER ch)
		{
			if (ch!=c)
				if (DISTANCE_APPROX(ch->GetX() - x, ch->GetY() - y) <= PARTY_DEFAULT_RANGE)
				{
					ch->PointChange(POINT_GOLD, iMoney, true);

					if (iMoney > 1000) // õ�� �̻� ����Ѵ�.
					{
						LOG_LEVEL_CHECK(LOG_LEVEL_MAX, LogManager::instance().CharLog(ch, iMoney, "GET_GOLD", ""));
					}
				}
		}
	};
}
#ifdef ENABLE_LONG_LONG
void CHARACTER::GiveGold(long long iAmount)
#else
void CHARACTER::GiveGold(int iAmount)
#endif
{
	if (iAmount <= 0)
		return;
#ifdef ENABLE_LONG_LONG
	sys_log(0, "GIVE_GOLD: %s %lld", GetName(), iAmount);
#else
	sys_log(0, "GIVE_GOLD: %s %d", GetName(), iAmount);
#endif
#ifdef ENABLE_BATTLE_PASS
	BYTE bBattlePassId = GetBattlePassId();
	if(bBattlePassId)
	{
		DWORD dwYangCount, dwNotUsed;
		if(CBattlePass::instance().BattlePassMissionGetInfo(bBattlePassId, FARM_YANG, &dwNotUsed, &dwYangCount))
		{
			if(GetMissionProgress(FARM_YANG, bBattlePassId) < dwYangCount)
				UpdateMissionProgress(FARM_YANG, bBattlePassId, iAmount, dwYangCount);
		}
	}
#endif

	if (GetParty())
	{
		LPPARTY pParty = GetParty();

		// ��Ƽ�� �ִ� ��� ������ ������.
#ifdef ENABLE_LONG_LONG
		long long dwTotal = iAmount;
		long long dwMyAmount = dwTotal;
#else
		DWORD dwTotal = iAmount;
		DWORD dwMyAmount = dwTotal;
#endif

		NPartyPickupDistribute::FCountNearMember funcCountNearMember(this);
		pParty->ForEachOnlineMember(funcCountNearMember);

		if (funcCountNearMember.total > 1)
		{
			DWORD dwShare = dwTotal / funcCountNearMember.total;
			dwMyAmount -= dwShare * (funcCountNearMember.total - 1);

			NPartyPickupDistribute::FMoneyDistributor funcMoneyDist(this, dwShare);

			pParty->ForEachOnlineMember(funcMoneyDist);
		}

		PointChange(POINT_GOLD, dwMyAmount, true);

		if (dwMyAmount > 1000) // õ�� �̻� ����Ѵ�.
		{
			LOG_LEVEL_CHECK(LOG_LEVEL_MAX, LogManager::instance().CharLog(this, dwMyAmount, "GET_GOLD", ""));
		}
	}
	else
	{
		PointChange(POINT_GOLD, iAmount, true);

		if (iAmount > 1000) // õ�� �̻� ����Ѵ�.
		{
			LOG_LEVEL_CHECK(LOG_LEVEL_MAX, LogManager::instance().CharLog(this, iAmount, "GET_GOLD", ""));
		}
	}
}

bool CHARACTER::PickupItem(DWORD dwVID)
{
	if (!IsPC() || IsDead() || IsObserverMode())
	{
		return false;
	}

	LPITEM item = ITEM_MANAGER::instance().FindByVID(dwVID);
	if (!item || !item->GetSectree())
		return false;

#ifdef ENABLE_BATTLE_PASS
	bool bIsBattlePass = item->HaveOwnership();
#endif

	if (item->DistanceValid(this))
	{
		// @fixme150 BEGIN
		if (item->GetType() == ITEM_QUEST)
		{
			if (quest::CQuestManager::instance().GetPCForce(GetPlayerID())->IsRunning() == true)
			{
#ifdef TEXTS_IMPROVEMENT
				ChatPacketNew(CHAT_TYPE_INFO, 692, "");
#endif
				return false;
			}
		}
		// @fixme150 END

		if (item->IsOwnership(this))
		{
			// ���� ������ �ϴ� �������� ��ũ���
			if (item->GetType() == ITEM_ELK)
			{
				GiveGold(item->GetCount());
				item->RemoveFromGround();
#ifdef ENABLE_RANKING
				SetRankPoints(10, GetRankPoints(10) + item->GetCount());
#endif
				M2_DESTROY_ITEM(item);

				Save();
			}
			// ����� �������̶��
			else
			{
#ifdef ENABLE_EXTRA_INVENTORY
				if (item->IsExtraItem() && item->IsStackable() && !IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_STACK))
				{
#ifdef ENABLE_NEW_STACK_LIMIT
					int 
#else
					BYTE 
#endif
					bCount = item->GetCount(); // change type for some

					for (int i = 0; i < EXTRA_INVENTORY_MAX_NUM; ++i)
					{
						LPITEM item2 = GetExtraInventoryItem(i);

						if (!item2)
							continue;

						if (item2->GetVnum() == item->GetVnum())
						{
							int j = 0;

							for (j = 0; j < ITEM_SOCKET_MAX_NUM; ++j)
								if (item2->GetSocket(j) != item->GetSocket(j))
									break;

							if (j != ITEM_SOCKET_MAX_NUM)
								continue;
							
#ifdef ENABLE_NEW_STACK_LIMIT
							int 
#else
							BYTE 
#endif
							bCount2 = MIN(g_bItemCountLimit - item2->GetCount(), bCount); // change type for some
							bCount -= bCount2;
							
#ifdef ENABLE_BATTLE_PASS
							if(bIsBattlePass)
							{
								BYTE bBattlePassId = GetBattlePassId();
								if(bBattlePassId)
								{
									DWORD dwItemVnum, dwCount;
									if(CBattlePass::instance().BattlePassMissionGetInfo(bBattlePassId, COLLECT_ITEM, &dwItemVnum, &dwCount))
									{
										if(dwItemVnum == item->GetVnum() && GetMissionProgress(COLLECT_ITEM, bBattlePassId) < dwCount)
											UpdateMissionProgress(COLLECT_ITEM, bBattlePassId, bCount2, dwCount);
									}
								}
							}
#endif

							item2->SetCount(item2->GetCount() + bCount2);

							if (bCount == 0)
							{
#ifdef TEXTS_IMPROVEMENT
								ChatPacketNew(
#ifdef ENABLE_NEW_CHAT
								CHAT_TYPE_INFO_ITEM
#else
								CHAT_TYPE_INFO
#endif
								, 102, "%s", item2->GetName(GetDesc() ? GetDesc()->GetLanguage() : 0));
#endif
								M2_DESTROY_ITEM(item);
								return true;
							}
						}
					}

					item->SetCount(bCount);
				}
				else if (item->IsStackable() && !IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_STACK))
#else
				if (item->IsStackable() && !IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_STACK))
#endif
				{
#ifdef ENABLE_NEW_STACK_LIMIT
					int 
#else
					BYTE 
#endif
					bCount = item->GetCount();

					for (int i = 0; i < INVENTORY_MAX_NUM; ++i)
					{
						LPITEM item2 = GetInventoryItem(i);

						if (!item2)
							continue;

						if (item2->GetVnum() == item->GetVnum())
						{
							int j;

							for (j = 0; j < ITEM_SOCKET_MAX_NUM; ++j)
								if (item2->GetSocket(j) != item->GetSocket(j))
									break;

							if (j != ITEM_SOCKET_MAX_NUM)
								continue;
							
#ifdef ENABLE_NEW_STACK_LIMIT
							int 
#else
							BYTE 
#endif
							bCount2 = MIN(g_bItemCountLimit - item2->GetCount(), bCount);
							bCount -= bCount2;
#ifdef ENABLE_BATTLE_PASS
							if(bIsBattlePass)
							{
								BYTE bBattlePassId = GetBattlePassId();
								if(bBattlePassId)
								{
									DWORD dwItemVnum, dwCount;
									if(CBattlePass::instance().BattlePassMissionGetInfo(bBattlePassId, COLLECT_ITEM, &dwItemVnum, &dwCount))
									{
										if(dwItemVnum == item->GetVnum() && GetMissionProgress(COLLECT_ITEM, bBattlePassId) < dwCount)
											UpdateMissionProgress(COLLECT_ITEM, bBattlePassId, bCount2, dwCount);
									}
								}
							}
#endif
							item2->SetCount(item2->GetCount() + bCount2);

							if (bCount == 0)
							{
#ifdef TEXTS_IMPROVEMENT
								ChatPacketNew(
#ifdef ENABLE_NEW_CHAT
								CHAT_TYPE_INFO_ITEM
#else
								CHAT_TYPE_INFO
#endif
								, 102, "%s", item2->GetName(GetDesc() ? GetDesc()->GetLanguage() : 0));
#endif
								M2_DESTROY_ITEM(item);
								return true;
							}
						}
					}

					item->SetCount(bCount);
				}

				int iEmptyCell;
				if (item->IsDragonSoul())
				{
					if ((iEmptyCell = GetEmptyDragonSoulInventory(item)) == -1)
					{
#ifdef TEXTS_IMPROVEMENT
						ChatPacketNew(CHAT_TYPE_INFO, 366, "");
#endif
						return false;
					}
				}
#ifdef ENABLE_EXTRA_INVENTORY
				else if (item->IsExtraItem())
				{
					if ((iEmptyCell = GetEmptyExtraInventory(item)) == -1)
					{
#ifdef TEXTS_IMPROVEMENT
						ChatPacketNew(CHAT_TYPE_INFO, 539, "");
#endif
						return false;
					}
				}
#endif
				else
				{
					if ((iEmptyCell = GetEmptyInventory(item->GetSize())) == -1)
					{
#ifdef TEXTS_IMPROVEMENT
						ChatPacketNew(CHAT_TYPE_INFO, 366, "");
#endif
						return false;
					}
				}

				item->RemoveFromGround();

				if (item->IsDragonSoul())
					item->AddToCharacter(this, TItemPos(DRAGON_SOUL_INVENTORY, iEmptyCell));
#ifdef ENABLE_EXTRA_INVENTORY
				else if (item->IsExtraItem())
					item->AddToCharacter(this, TItemPos(EXTRA_INVENTORY, iEmptyCell));
#endif
				else
					item->AddToCharacter(this, TItemPos(INVENTORY, iEmptyCell));

#ifdef ENABLE_BATTLE_PASS
				if(bIsBattlePass)
				{
					BYTE bBattlePassId = GetBattlePassId();
					if(bBattlePassId)
					{
						DWORD dwItemVnum, dwCount;
						if(CBattlePass::instance().BattlePassMissionGetInfo(bBattlePassId, COLLECT_ITEM, &dwItemVnum, &dwCount))
						{
							if(dwItemVnum == item->GetVnum() && GetMissionProgress(COLLECT_ITEM, bBattlePassId) < dwCount)
								UpdateMissionProgress(COLLECT_ITEM, bBattlePassId, item->GetCount(), dwCount);
						}
					}
				}
#endif

				char szHint[32+1];
				snprintf(szHint, sizeof(szHint), "%s %u %u", item->GetName(), item->GetCount(), item->GetOriginalVnum());
				LogManager::instance().ItemLog(this, item, "GET", szHint);
#ifdef TEXTS_IMPROVEMENT
				ChatPacketNew(
#ifdef ENABLE_NEW_CHAT
				CHAT_TYPE_INFO_ITEM
#else
				CHAT_TYPE_INFO
#endif
				, 102, "%s", item->GetName(GetDesc() ? GetDesc()->GetLanguage() : 0));
#endif
			}

			//Motion(MOTION_PICKUP);
			return true;
		}
		else if (!IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_GIVE | ITEM_ANTIFLAG_DROP) && GetParty())
		{
			// �ٸ� ��Ƽ�� ������ �������� �������� �Ѵٸ�
			NPartyPickupDistribute::FFindOwnership funcFindOwnership(item);

			GetParty()->ForEachOnlineMember(funcFindOwnership);

			LPCHARACTER owner = funcFindOwnership.owner;
			// @fixme115
			if (!owner)
				return false;

#ifdef ENABLE_EXTRA_INVENTORY
			if (item->IsExtraItem() && item->IsStackable() && !IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_STACK))
			{
#ifdef ENABLE_NEW_STACK_LIMIT
				int 
#else
				BYTE 
#endif
				bCount = item->GetCount(); // change type for some

				for (int i = 0; i < EXTRA_INVENTORY_MAX_NUM; ++i)
				{
					LPITEM item2 = owner->GetExtraInventoryItem(i);

					if (!item2)
						continue;

					if (item2->GetVnum() == item->GetVnum())
					{
						int j = 0;

						for (j = 0; j < ITEM_SOCKET_MAX_NUM; ++j)
							if (item2->GetSocket(j) != item->GetSocket(j))
								break;

						if (j != ITEM_SOCKET_MAX_NUM)
							continue;
						
#ifdef ENABLE_NEW_STACK_LIMIT
						int 
#else
						BYTE 
#endif
						bCount2 = MIN(g_bItemCountLimit - item2->GetCount(), bCount); // change type for some
						bCount -= bCount2;
#ifdef ENABLE_BATTLE_PASS
							if(bIsBattlePass)
							{
								BYTE bBattlePassId = owner->GetBattlePassId();
								if(bBattlePassId)
								{
									DWORD dwItemVnum, dwCount;
									if(CBattlePass::instance().BattlePassMissionGetInfo(bBattlePassId, COLLECT_ITEM, &dwItemVnum, &dwCount))
									{
										if(dwItemVnum == item->GetVnum() && owner->GetMissionProgress(COLLECT_ITEM, bBattlePassId) < dwCount)
											owner->UpdateMissionProgress(COLLECT_ITEM, bBattlePassId, bCount2, dwCount);
									}
								}
							}
#endif
						item2->SetCount(item2->GetCount() + bCount2);

						if (bCount == 0)
						{
#ifdef TEXTS_IMPROVEMENT
							owner->ChatPacketNew(
#ifdef ENABLE_NEW_CHAT
							CHAT_TYPE_INFO_ITEM
#else
							CHAT_TYPE_INFO
#endif
							, 102, "%s", item2->GetName(GetDesc() ? GetDesc()->GetLanguage() : 0));
#endif
							M2_DESTROY_ITEM(item);
							return true;
						}
					}
				}

				item->SetCount(bCount);
			}
			else if (item->IsStackable() && !IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_STACK))
#else
			if (item->IsStackable() && !IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_STACK))
#endif
			{
#ifdef ENABLE_NEW_STACK_LIMIT
				int 
#else
				BYTE 
#endif
				bCount = item->GetCount();

				for (int i = 0; i < INVENTORY_MAX_NUM; ++i)
				{
					LPITEM item2 = owner->GetInventoryItem(i);

					if (!item2)
						continue;

					if (item2->GetVnum() == item->GetVnum())
					{
						int j;

						for (j = 0; j < ITEM_SOCKET_MAX_NUM; ++j)
							if (item2->GetSocket(j) != item->GetSocket(j))
								break;

						if (j != ITEM_SOCKET_MAX_NUM)
							continue;
							
#ifdef ENABLE_NEW_STACK_LIMIT
						int 
#else
						BYTE 
#endif
						bCount2 = MIN(g_bItemCountLimit - item2->GetCount(), bCount);
						bCount -= bCount2;
#ifdef ENABLE_BATTLE_PASS
						if(bIsBattlePass)
						{
							BYTE bBattlePassId = owner->GetBattlePassId();
							if(bBattlePassId)
							{
								DWORD dwItemVnum, dwCount;
								if(CBattlePass::instance().BattlePassMissionGetInfo(bBattlePassId, COLLECT_ITEM, &dwItemVnum, &dwCount))
								{
									if(dwItemVnum == item->GetVnum() && owner->GetMissionProgress(COLLECT_ITEM, bBattlePassId) < dwCount)
										owner->UpdateMissionProgress(COLLECT_ITEM, bBattlePassId, bCount2, dwCount);
								}
							}
						}
#endif
						item2->SetCount(item2->GetCount() + bCount2);

						if (bCount == 0)
						{
#ifdef TEXTS_IMPROVEMENT
							owner->ChatPacketNew(
#ifdef ENABLE_NEW_CHAT
							CHAT_TYPE_INFO_ITEM
#else
							CHAT_TYPE_INFO
#endif
							, 102, "%s", item2->GetName(GetDesc() ? GetDesc()->GetLanguage() : 0));
#endif
							M2_DESTROY_ITEM(item);
							return true;
						}
					}
				}

				item->SetCount(bCount);
			}

			int iEmptyCell;

			if (item->IsDragonSoul())
			{
				if (!(owner && (iEmptyCell = owner->GetEmptyDragonSoulInventory(item)) != -1))
				{
#ifdef ENABLE_BUG_FIXES
#ifdef TEXTS_IMPROVEMENT
					ChatPacketNew(CHAT_TYPE_INFO, 1248, "%s", owner->GetName());
#endif
					return false;
#else
					owner = this;

					if ((iEmptyCell = GetEmptyDragonSoulInventory(item)) == -1)
					{
#ifdef TEXTS_IMPROVEMENT
						owner->ChatPacketNew(CHAT_TYPE_INFO, 366, "");
#endif
						return false;
					}
#endif
				}
			}
#ifdef ENABLE_EXTRA_INVENTORY
			else if (item->IsExtraItem())
			{
				if (!(owner && (iEmptyCell = owner->GetEmptyExtraInventory(item)) != -1))
				{
#ifdef ENABLE_BUG_FIXES
#ifdef TEXTS_IMPROVEMENT
				ChatPacketNew(CHAT_TYPE_INFO, 1248, "%s", owner->GetName());
#endif
				return false;
#else
					owner = this;

					if ((iEmptyCell = GetEmptyExtraInventory(item)) == -1)
					{
#ifdef TEXTS_IMPROVEMENT
						ChatPacketNew(CHAT_TYPE_INFO, 539, "");
#endif
						return false;
					}
#endif
				}
			}
#endif
			else
			{
				if (!(owner && (iEmptyCell = owner->GetEmptyInventory(item->GetSize())) != -1))
				{
#ifdef ENABLE_BUG_FIXES
#ifdef TEXTS_IMPROVEMENT
					ChatPacketNew(CHAT_TYPE_INFO, 1248, "%s", owner->GetName());
#endif
					return false;
#else
					owner = this;

					if ((iEmptyCell = GetEmptyInventory(item->GetSize())) == -1)
					{
#ifdef TEXTS_IMPROVEMENT
						owner->ChatPacketNew(CHAT_TYPE_INFO, 366, "");
#endif
						return false;
					}
#endif
				}
			}

			item->RemoveFromGround();

			if (item->IsDragonSoul())
				item->AddToCharacter(owner, TItemPos(DRAGON_SOUL_INVENTORY, iEmptyCell));
#ifdef ENABLE_EXTRA_INVENTORY
			else if (item->IsExtraItem())
				item->AddToCharacter(owner, TItemPos(EXTRA_INVENTORY, iEmptyCell));
#endif
			else
				item->AddToCharacter(owner, TItemPos(INVENTORY, iEmptyCell));

#ifdef ENABLE_BATTLE_PASS
			if(bIsBattlePass)
			{
				BYTE bBattlePassId = owner->GetBattlePassId();
				if(bBattlePassId)
				{
					DWORD dwItemVnum, dwCount;
					if(CBattlePass::instance().BattlePassMissionGetInfo(bBattlePassId, COLLECT_ITEM, &dwItemVnum, &dwCount))
					{
						if(dwItemVnum == item->GetVnum() && owner->GetMissionProgress(COLLECT_ITEM, bBattlePassId) < dwCount)
							owner->UpdateMissionProgress(COLLECT_ITEM, bBattlePassId, item->GetCount(), dwCount);
					}
				}
			}
#endif

			char szHint[32+1];
			snprintf(szHint, sizeof(szHint), "%s %u %u", item->GetName(), item->GetCount(), item->GetOriginalVnum());
			LogManager::instance().ItemLog(owner, item, "GET", szHint);

			if (owner == this) {
#ifdef TEXTS_IMPROVEMENT
				ChatPacketNew(
#ifdef ENABLE_NEW_CHAT
				CHAT_TYPE_INFO_ITEM
#else
				CHAT_TYPE_INFO
#endif
				, 102, "%s", item->GetName(GetDesc() ? GetDesc()->GetLanguage() : 0));
#endif
			}
			else
			{
#ifdef TEXTS_IMPROVEMENT
				owner->ChatPacketNew(
#ifdef ENABLE_NEW_CHAT
				CHAT_TYPE_INFO_ITEM
#else
				CHAT_TYPE_INFO
#endif
				, 102, "%s", item->GetName(GetDesc() ? GetDesc()->GetLanguage() : 0));
#endif
#ifdef TEXTS_IMPROVEMENT
				owner->ChatPacketNew(CHAT_TYPE_INFO, 401, "%s", item->GetName());
#endif
			}

			return true;
		}
	}

	return false;
}

bool CHARACTER::SwapItem(BYTE bCell, BYTE bDestCell)
{
	if (!CanHandleItem())
		return false;

	TItemPos srcCell(INVENTORY, bCell), destCell(INVENTORY, bDestCell);

	// �ùٸ� Cell ���� �˻�
	// ��ȥ���� Swap�� �� �����Ƿ�, ���⼭ �ɸ�.
	//if (bCell >= INVENTORY_MAX_NUM + WEAR_MAX_NUM || bDestCell >= INVENTORY_MAX_NUM + WEAR_MAX_NUM)
	if (srcCell.IsDragonSoulEquipPosition() || destCell.IsDragonSoulEquipPosition())
		return false;

	// ���� CELL ���� �˻�
	if (bCell == bDestCell)
		return false;

	// �� �� ���â ��ġ�� Swap �� �� ����.
	if (srcCell.IsEquipPosition() && destCell.IsEquipPosition())
		return false;

	LPITEM item1, item2;

	// item2�� ���â�� �ִ� ���� �ǵ���.
	if (srcCell.IsEquipPosition())
	{
		item1 = GetInventoryItem(bDestCell);
		item2 = GetInventoryItem(bCell);
	}
	else
	{
		item1 = GetInventoryItem(bCell);
		item2 = GetInventoryItem(bDestCell);
	}

	if (!item1 || !item2)
		return false;

	if (item1 == item2)
	{
	    sys_log(0, "[WARNING][WARNING][HACK USER!] : %s %d %d", m_stName.c_str(), bCell, bDestCell);
	    return false;
	}

	// item2�� bCell��ġ�� �� �� �ִ��� Ȯ���Ѵ�.
	if (!IsEmptyItemGrid(TItemPos (INVENTORY, item1->GetCell()), item2->GetSize(), item1->GetCell()))
		return false;

	// �ٲ� �������� ���â�� ������
	if (TItemPos(EQUIPMENT, item2->GetCell()).IsEquipPosition())
	{
		BYTE bEquipCell = item2->GetCell() - INVENTORY_MAX_NUM;
		BYTE bInvenCell = item1->GetCell();

		// �������� �������� ���� �� �ְ�, ���� ���� �������� ���� ������ ���¿��߸� ����
		if (item2->IsDragonSoul() || item2->GetType() == ITEM_BELT) // @fixme117
		{
			if (false == CanUnequipNow(item2) || false == CanEquipNow(item1))
				return false;
		}

		if (bEquipCell != item1->FindEquipCell(this)) // ���� ��ġ�϶��� ���
			return false;

		item2->RemoveFromCharacter();

		if (item1->EquipTo(this, bEquipCell))
		{
			item2->AddToCharacter(this, TItemPos(INVENTORY, bInvenCell)
#ifdef __HIGHLIGHT_SYSTEM__
			, false
#endif
		);
			////item2->ModifyPoints(false);
			////ComputePoints();
		}
		else {
			sys_err("SwapItem cannot equip %s! item1 %s", item2->GetName(), item1->GetName());
		}
	}
	else
	{
		BYTE bCell1 = item1->GetCell();
		BYTE bCell2 = item2->GetCell();

		item1->RemoveFromCharacter();
		item2->RemoveFromCharacter();

#ifdef __HIGHLIGHT_SYSTEM__
		item1->AddToCharacter(this, TItemPos(INVENTORY, bCell2), false);
		item2->AddToCharacter(this, TItemPos(INVENTORY, bCell1), false);
#else
		item1->AddToCharacter(this, TItemPos(INVENTORY, bCell2));
		item2->AddToCharacter(this, TItemPos(INVENTORY, bCell1));
#endif
	}

	return true;
}

bool CHARACTER::UnequipItem(LPITEM item)
{
#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
	int iWearCell = item->FindEquipCell(this);
	if (iWearCell == WEAR_WEAPON)
	{
		LPITEM costumeWeapon = GetWear(WEAR_COSTUME_WEAPON);
		if (costumeWeapon && !UnequipItem(costumeWeapon))
		{
#ifdef TEXTS_IMPROVEMENT
			ChatPacketNew(CHAT_TYPE_INFO, 366, "");
#endif
			return false;
		}
	}
#elif defined(ENABLE_BUG_FIXES)
	int iWearCell = item->FindEquipCell(this);
#endif

	if (false == CanUnequipNow(item))
		return false;

	int pos;
	if (item->IsDragonSoul())
		pos = GetEmptyDragonSoulInventory(item);
	else
		pos = GetEmptyInventory(item->GetSize());

	// HARD CODING
	if (item->GetVnum() == UNIQUE_ITEM_HIDE_ALIGNMENT_TITLE)
		ShowAlignment(true);

	item->RemoveFromCharacter();
	if (item->IsDragonSoul())
#ifdef __HIGHLIGHT_SYSTEM__
		item->AddToCharacter(this, TItemPos(DRAGON_SOUL_INVENTORY, pos), false);
#else
		item->AddToCharacter(this, TItemPos(DRAGON_SOUL_INVENTORY, pos));
#endif
	else
#ifdef __HIGHLIGHT_SYSTEM__
		item->AddToCharacter(this, TItemPos(INVENTORY, pos), false);
#else
		item->AddToCharacter(this, TItemPos(INVENTORY, pos));
#endif

	CheckMaximumPoints();
#ifdef ENABLE_BUG_FIXES
	if (iWearCell == WEAR_WEAPON) {
		if (IsAffectFlag(AFF_GWIGUM)) {
			RemoveAffect(SKILL_GWIGEOM);
		}

		if (IsAffectFlag(AFF_GEOMGYEONG)) {
			RemoveAffect(SKILL_GEOMKYUNG);
		}
	}
#endif
	return true;
}

//
// @version	05/07/05 Bang2ni - Skill ����� 1.5 �� �̳��� ��� ���� ����
//
bool CHARACTER::EquipItem(LPITEM item, int iCandidateCell)
{
	if (item->IsExchanging())
		return false;

	if (false == item->IsEquipable())
		return false;

	if (false == CanEquipNow(item))
		return false;

	int iWearCell = item->FindEquipCell(this, iCandidateCell);

	if (iWearCell < 0)
		return false;

	// ���𰡸� ź ���¿��� �νõ� �Ա� ����
	if (iWearCell == WEAR_BODY && IsRiding() && (item->GetVnum() >= 11901 && item->GetVnum() <= 11904))
	{
#ifdef TEXTS_IMPROVEMENT
		ChatPacketNew(CHAT_TYPE_INFO, 693, "");
#endif
		return false;
	}

	if (iWearCell != WEAR_ARROW && IsPolymorphed()) {
#ifdef TEXTS_IMPROVEMENT
		ChatPacketNew(CHAT_TYPE_INFO, 315, "");
#endif
		return false;
	}

	if (FN_check_item_sex(this, item) == false)
	{
#ifdef TEXTS_IMPROVEMENT
		ChatPacketNew(CHAT_TYPE_INFO, 496, "");
#endif
		return false;
	}

	//�ű� Ż�� ���� ���� �� ��뿩�� üũ
	if(item->IsRideItem() && IsRiding())
	{
#ifdef TEXTS_IMPROVEMENT
		ChatPacketNew(CHAT_TYPE_INFO, 532, "");
#endif
		return false;
	}

#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
	if (iWearCell == WEAR_WEAPON)
	{
		if (item->GetType() == ITEM_WEAPON)
		{
			LPITEM costumeWeapon = GetWear(WEAR_COSTUME_WEAPON);
			if (costumeWeapon && costumeWeapon->GetValue(3) != item->GetSubType() && !UnequipItem(costumeWeapon))
			{
#ifdef TEXTS_IMPROVEMENT
				ChatPacketNew(CHAT_TYPE_INFO, 366, "");
#endif
				return false;
			}
		}
		else //fishrod/pickaxe
		{
			LPITEM costumeWeapon = GetWear(WEAR_COSTUME_WEAPON);
			if (costumeWeapon && !UnequipItem(costumeWeapon))
			{
#ifdef TEXTS_IMPROVEMENT
				ChatPacketNew(CHAT_TYPE_INFO, 366, "");
#endif
				return false;
			}
		}
	}
	else if (iWearCell == WEAR_COSTUME_WEAPON)
	{
		if (item->GetType() == ITEM_COSTUME && item->GetSubType() == COSTUME_WEAPON)
		{
			LPITEM pkWeapon = GetWear(WEAR_WEAPON);
			if (!pkWeapon || pkWeapon->GetType() != ITEM_WEAPON || item->GetValue(3) != pkWeapon->GetSubType())
			{
#ifdef TEXTS_IMPROVEMENT
				ChatPacketNew(CHAT_TYPE_INFO, 694, "");
#endif
				return false;
			}
		}
	}
#endif

	// ��ȥ�� Ư�� ó��
	if (item->IsDragonSoul())
	{
		// ���� Ÿ���� ��ȥ���� �̹� �� �ִٸ� ������ �� ����.
		// ��ȥ���� swap�� �����ϸ� �ȵ�.
		if(GetInventoryItem(INVENTORY_MAX_NUM + iWearCell))
		{
#ifdef TEXTS_IMPROVEMENT
			ChatPacketNew(CHAT_TYPE_INFO, 796, "");
#endif
			return false;
		}

		if (!item->EquipTo(this, iWearCell))
		{
			return false;
		}
	}
	// ��ȥ���� �ƴ�.
	else
	{
		// ������ ���� �������� �ִٸ�,
		if (GetWear(iWearCell) && !IS_SET(GetWear(iWearCell)->GetFlag(), ITEM_FLAG_IRREMOVABLE))
		{
			// �� �������� �ѹ� ������ ���� �Ұ�. swap ���� ���� �Ұ�
			if (item->GetWearFlag() == WEARABLE_ABILITY)
				return false;

			if (false == SwapItem(item->GetCell(), INVENTORY_MAX_NUM + iWearCell))
			{
				return false;
			}
		}
		else
		{
			BYTE bOldCell = item->GetCell();

			if (item->EquipTo(this, iWearCell))
			{
				SyncQuickslot(QUICKSLOT_TYPE_ITEM, bOldCell, iWearCell);
			}
		}
	}

	if (true == item->IsEquipped())
	{
		// ������ ���� ��� ���ĺ��ʹ� ������� �ʾƵ� �ð��� �����Ǵ� ��� ó��.
		if (-1 != item->GetProto()->cLimitRealTimeFirstUseIndex)
		{
			// �� ���̶� ����� ���������� ���δ� Socket1�� ���� �Ǵ��Ѵ�. (Socket1�� ���Ƚ�� ���)
			if (0 == item->GetSocket(1))
			{
				// ��밡�ɽð��� Default ������ Limit Value ���� ����ϵ�, Socket0�� ���� ������ �� ���� ����ϵ��� �Ѵ�. (������ ��)
				long duration = (0 != item->GetSocket(0)) ? item->GetSocket(0) : item->GetProto()->aLimits[(unsigned char)(item->GetProto()->cLimitRealTimeFirstUseIndex)].lValue;

				if (0 == duration)
					duration = 60 * 60 * 24 * 7;

				item->SetSocket(0, time(0) + duration);
				item->StartRealTimeExpireEvent();
			}

			item->SetSocket(1, item->GetSocket(1) + 1);
		}

		if (item->GetVnum() == UNIQUE_ITEM_HIDE_ALIGNMENT_TITLE)
			ShowAlignment(false);

		const DWORD& dwVnum = item->GetVnum();

		// �󸶴� �̺�Ʈ �ʽ´��� ����(71135) ����� ����Ʈ �ߵ�
		if (true == CItemVnumHelper::IsRamadanMoonRing(dwVnum))
		{
			this->EffectPacket(SE_EQUIP_RAMADAN_RING);
		}
		// �ҷ��� ����(71136) ����� ����Ʈ �ߵ�
		else if (true == CItemVnumHelper::IsHalloweenCandy(dwVnum))
		{
			this->EffectPacket(SE_EQUIP_HALLOWEEN_CANDY);
		}
		// �ູ�� ����(71143) ����� ����Ʈ �ߵ�
		else if (true == CItemVnumHelper::IsHappinessRing(dwVnum))
		{
			this->EffectPacket(SE_EQUIP_HAPPINESS_RING);
		}
		// ����� �Ҵ�Ʈ(71145) ����� ����Ʈ �ߵ�
		else if (true == CItemVnumHelper::IsLovePendant(dwVnum))
		{
			this->EffectPacket(SE_EQUIP_LOVE_PENDANT);
		}
		// ITEM_UNIQUE�� ���, SpecialItemGroup�� ���ǵǾ� �ְ�, (item->GetSIGVnum() != NULL)
		//
		else if (ITEM_UNIQUE == item->GetType() && 0 != item->GetSIGVnum())
		{
			const CSpecialItemGroup* pGroup = ITEM_MANAGER::instance().GetSpecialItemGroup(item->GetSIGVnum());
			if (NULL != pGroup)
			{
				const CSpecialAttrGroup* pAttrGroup = ITEM_MANAGER::instance().GetSpecialAttrGroup(pGroup->GetAttrVnum(item->GetVnum()));
				if (NULL != pAttrGroup)
				{
					const std::string& std = pAttrGroup->m_stEffectFileName;
					SpecificEffectPacket(std.c_str());
				}
			}
		}
#ifdef ENABLE_ACCE_SYSTEM
		else if ((item->GetType() == ITEM_COSTUME) && (item->GetSubType() == COSTUME_ACCE))
			this->EffectPacket(SE_EFFECT_ACCE_EQUIP);
#endif
#ifdef ENABLE_STOLE_COSTUME
		else if ((item->GetType() == ITEM_COSTUME) && (item->GetSubType() == COSTUME_STOLE))
			this->EffectPacket(SE_EFFECT_ACCE_EQUIP);
#endif
#ifdef ENABLE_TALISMAN_EFFECT
	else if (/*(item->GetType() == ITEM_ARMOR) && (item->GetWearFlag() ==WEARABLE_PENDANT) && */(item->GetVnum() >= 9600 && item->GetVnum()<= 9800))
		this->EffectPacket(SE_EFFECT_TALISMAN_EQUIP_FIRE);
	else if (/*(item->GetType() == ITEM_ARMOR) && (item->GetWearFlag() ==WEARABLE_PENDANT) && */(item->GetVnum() >= 9830 && item->GetVnum()<= 10030))
		this->EffectPacket(SE_EFFECT_TALISMAN_EQUIP_ICE);
	else if (/*(item->GetType() == ITEM_ARMOR) && (item->GetWearFlag() ==WEARABLE_PENDANT) && */(item->GetVnum() >= 10520 && item->GetVnum()<= 10720))
		this->EffectPacket(SE_EFFECT_TALISMAN_EQUIP_WIND);
	else if (/*(item->GetType() == ITEM_ARMOR) && (item->GetWearFlag() ==WEARABLE_PENDANT) && */(item->GetVnum() >= 10060 && item->GetVnum()<= 10260))
		this->EffectPacket(SE_EFFECT_TALISMAN_EQUIP_EARTH);
	else if (/*(item->GetType() == ITEM_ARMOR) && (item->GetWearFlag() ==WEARABLE_PENDANT) && */(item->GetVnum() >= 10290 && item->GetVnum()<= 10490))
		this->EffectPacket(SE_EFFECT_TALISMAN_EQUIP_DARK);
	else if (/*(item->GetType() == ITEM_ARMOR) && (item->GetWearFlag() ==WEARABLE_PENDANT) && */(item->GetVnum() >= 10750 && item->GetVnum()<= 10950))
		this->EffectPacket(SE_EFFECT_TALISMAN_EQUIP_ELEC);
#endif

		if (
			(ITEM_UNIQUE == item->GetType() && UNIQUE_SPECIAL_RIDE == item->GetSubType() && IS_SET(item->GetFlag(), ITEM_FLAG_QUEST_USE))
			|| (ITEM_UNIQUE == item->GetType() && UNIQUE_SPECIAL_MOUNT_RIDE == item->GetSubType() && IS_SET(item->GetFlag(), ITEM_FLAG_QUEST_USE))
#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
			|| (ITEM_COSTUME == item->GetType() && COSTUME_MOUNT == item->GetSubType())
#endif
		)
		{
			quest::CQuestManager::instance().UseItem(GetPlayerID(), item, false);
		}

	}

	return true;
}

void CHARACTER::BuffOnAttr_AddBuffsFromItem(LPITEM pItem)
{
	for (size_t i = 0; i < sizeof(g_aBuffOnAttrPoints)/sizeof(g_aBuffOnAttrPoints[0]); i++)
	{
		TMapBuffOnAttrs::iterator it = m_map_buff_on_attrs.find(g_aBuffOnAttrPoints[i]);
		if (it != m_map_buff_on_attrs.end())
		{
			it->second->AddBuffFromItem(pItem);
		}
	}
}

void CHARACTER::BuffOnAttr_RemoveBuffsFromItem(LPITEM pItem)
{
	for (size_t i = 0; i < sizeof(g_aBuffOnAttrPoints)/sizeof(g_aBuffOnAttrPoints[0]); i++)
	{
		TMapBuffOnAttrs::iterator it = m_map_buff_on_attrs.find(g_aBuffOnAttrPoints[i]);
		if (it != m_map_buff_on_attrs.end())
		{
			it->second->RemoveBuffFromItem(pItem);
		}
	}
}

void CHARACTER::BuffOnAttr_ClearAll()
{
	for (TMapBuffOnAttrs::iterator it = m_map_buff_on_attrs.begin(); it != m_map_buff_on_attrs.end(); it++)
	{
		CBuffOnAttributes* pBuff = it->second;
		if (pBuff)
		{
			pBuff->Initialize();
		}
	}
}

void CHARACTER::BuffOnAttr_ValueChange(BYTE bType, BYTE bOldValue, BYTE bNewValue)
{
	TMapBuffOnAttrs::iterator it = m_map_buff_on_attrs.find(bType);

	if (0 == bNewValue)
	{
		if (m_map_buff_on_attrs.end() == it)
			return;
		else
			it->second->Off();
	}
	else if(0 == bOldValue)
	{
		CBuffOnAttributes* pBuff = NULL;
		if (m_map_buff_on_attrs.end() == it)
		{
			switch (bType)
			{
			case POINT_ENERGY:
				{
					static BYTE abSlot[] = { WEAR_BODY, WEAR_HEAD, WEAR_FOOTS, WEAR_WRIST, WEAR_WEAPON, WEAR_NECK, WEAR_EAR, WEAR_SHIELD };
					static std::vector <BYTE> vec_slots (abSlot, abSlot + _countof(abSlot));
					pBuff = M2_NEW CBuffOnAttributes(this, bType, &vec_slots);
				}
				break;
			case POINT_COSTUME_ATTR_BONUS:
				{
					static BYTE abSlot[] = {
						WEAR_COSTUME_BODY,
						WEAR_COSTUME_HAIR,
						WEAR_COSTUME_MOUNT,
#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
						WEAR_COSTUME_WEAPON,
#endif
#ifdef ENABLE_STOLE_COSTUME
						WEAR_COSTUME_ACCE,
#endif
						WEAR_COSTUME_ACCE_SLOT,
					};
					
					static std::vector <BYTE> vec_slots (abSlot, abSlot + _countof(abSlot));
					pBuff = M2_NEW CBuffOnAttributes(this, bType, &vec_slots);
				}
				break;
			default:
				break;
			}
			m_map_buff_on_attrs.insert(TMapBuffOnAttrs::value_type(bType, pBuff));

		}
		else
			pBuff = it->second;
		if (pBuff != NULL)
			pBuff->On(bNewValue);
	}
	else
	{
		assert (m_map_buff_on_attrs.end() != it);
		it->second->ChangeBuffValue(bNewValue);
	}
}


LPITEM CHARACTER::FindSpecifyItem(DWORD vnum
#ifdef ENABLE_EXTRA_INVENTORY
, bool reinforce
#endif
) const
{
#ifdef ENABLE_EXTRA_INVENTORY
	if (reinforce) {
		for (int i = 0; i < EXTRA_INVENTORY_MAX_NUM; ++i) {
			if (GetExtraInventoryItem(i) && GetExtraInventoryItem(i)->GetVnum() == vnum) {
				return GetExtraInventoryItem(i);
			}
		}
	} else {
#ifdef __ENABLE_EXTEND_INVEN_SYSTEM__
		for (int i = 0; i < Inventory_Size(); ++i)
#else
		for (int i = 0; i < INVENTORY_MAX_NUM; ++i)	
#endif
		{
			if (GetInventoryItem(i) && GetInventoryItem(i)->GetVnum() == vnum) {
				return GetInventoryItem(i);
			}
		}
	}
#else
#ifdef __ENABLE_EXTEND_INVEN_SYSTEM__
	for (int i = 0; i < Inventory_Size(); ++i)
#else
	for (int i = 0; i < INVENTORY_MAX_NUM; ++i)	
#endif
		if (GetInventoryItem(i) && GetInventoryItem(i)->GetVnum() == vnum)
			return GetInventoryItem(i);
#endif

	return NULL;
}

LPITEM CHARACTER::FindItemByID(DWORD id) const
{
#ifdef __ENABLE_EXTEND_INVEN_SYSTEM__
	for (int i = 0; i < Inventory_Size(); ++i)
#else
	for (int i = 0; i < INVENTORY_MAX_NUM; ++i)	
#endif
	{
		if (NULL != GetInventoryItem(i) && GetInventoryItem(i)->GetID() == id)
			return GetInventoryItem(i);
	}

	for (int i=BELT_INVENTORY_SLOT_START; i < BELT_INVENTORY_SLOT_END ; ++i)
	{
		if (NULL != GetInventoryItem(i) && GetInventoryItem(i)->GetID() == id)
			return GetInventoryItem(i);
	}

	return NULL;
}
int CHARACTER::CountSpecifyItemRenewal(DWORD vnum) const
{

	int	count = 0;
	LPITEM item;


#ifdef ENABLE_EXTRA_INVENTORY
	if (ITEM_MANAGER::instance().IsExtraItem(vnum))
	{
		for (int i = 0; i < EXTRA_INVENTORY_MAX_NUM; ++i)
		{
			item = GetExtraInventoryItem(i);

			if (item && item->GetVnum() == vnum)
			{
				if (item->GetLockedAttr() != -1){
					continue;
				}

				if (m_pkMyShop && m_pkMyShop->IsSellingItem(item->GetID()))
					continue;
				else
					count += item->GetCount();
			}
		}
	}
	else {
#endif
#ifdef __ENABLE_EXTEND_INVEN_SYSTEM__
	for (int i = 0; i < Inventory_Size(); ++i)
#else
	for (int i = 0; i < INVENTORY_MAX_NUM; ++i)	
#endif
	{
		item = GetInventoryItem(i);
		if (NULL != item && item->GetVnum() == vnum)
		{
			// ���� ������ ��ϵ� �����̸� �Ѿ��.
			if (item->GetLockedAttr() != -1){
				continue;
			}

			if (m_pkMyShop && m_pkMyShop->IsSellingItem(item->GetID()))
			{
				continue;
			}
			else
			{
				count += item->GetCount();
			}
		}
	}
#ifdef ENABLE_EXTRA_INVENTORY
	}
#endif
	return count;

}

int CHARACTER::CountSpecifyItem(DWORD vnum) const
{
	int	count = 0;
	LPITEM item;
#ifdef ENABLE_EXTRA_INVENTORY
	if (ITEM_MANAGER::instance().IsExtraItem(vnum))
	{
		for (int i = 0; i < EXTRA_INVENTORY_MAX_NUM; ++i)
		{
			item = GetExtraInventoryItem(i);
			if (item && item->GetVnum() == vnum)
			{
				if (m_pkMyShop && m_pkMyShop->IsSellingItem(item->GetID())) {
					continue;
				}
				else {
					count += item->GetCount();
				}
			}
		}
	}
	else {
#endif


#ifdef __ENABLE_EXTEND_INVEN_SYSTEM__
	for (int i = 0; i < Inventory_Size(); ++i)
#else
	for (int i = 0; i < INVENTORY_MAX_NUM; ++i)	
#endif
	{
		item = GetInventoryItem(i);
		if (NULL != item && item->GetVnum() == vnum)
		{
			// ���� ������ ��ϵ� �����̸� �Ѿ��.
			if (m_pkMyShop && m_pkMyShop->IsSellingItem(item->GetID()))
			{
				continue;
			}
			else {
				count += item->GetCount();
			}
		}
	}
#ifdef ENABLE_EXTRA_INVENTORY
	}
#endif

	return count;
}

void CHARACTER::RemoveSpecifyItem(DWORD vnum, int count, bool cuberenewal)
{
	if (0 == count)
		return;


#ifdef ENABLE_EXTRA_INVENTORY
	if (ITEM_MANAGER::instance().IsExtraItem(vnum))
	{
		for (WORD i = 0; i < EXTRA_INVENTORY_MAX_NUM; ++i)
		{
			LPITEM item = GetExtraInventoryItem(i);

			if (!item)
				continue;

			if (item->GetVnum() != vnum)
				continue;

			if (m_pkMyShop)
			{
				if (m_pkMyShop->IsSellingItem(item->GetID()))
					continue;
			}

			if(cuberenewal){
				if (item->GetLockedAttr() != -1){
					continue;
				}
			}
			
			if (count >= item->GetCount())
			{
				count -= item->GetCount();
				item->SetCount(0);

				if (0 == count)
					return;
			}
			else
			{
				item->SetCount(item->GetCount() - count);
				return;
			}
		}
}
	else
#endif

#ifdef __ENABLE_EXTEND_INVEN_SYSTEM__
	for (int i = 0; i < Inventory_Size(); ++i)
#else
	for (UINT i = 0; i < INVENTORY_MAX_NUM; ++i)
#endif
	{
		if (NULL == GetInventoryItem(i))
			continue;

		if (GetInventoryItem(i)->GetVnum() != vnum)
			continue;

		//���� ������ ��ϵ� �����̸� �Ѿ��. (���� �������� �Ǹŵɶ� �� �κ����� ���� ��� ����!)
		if(m_pkMyShop)
		{
			bool isItemSelling = m_pkMyShop->IsSellingItem(GetInventoryItem(i)->GetID());
			if (isItemSelling)
				continue;
		}

		if(cuberenewal){
			if (GetInventoryItem(i)->GetLockedAttr() != -1){
				continue;
			}
		}

		if (vnum >= 80003 && vnum <= 80007)
			LogManager::instance().GoldBarLog(GetPlayerID(), GetInventoryItem(i)->GetID(), QUEST, "RemoveSpecifyItem");

		if (count >= GetInventoryItem(i)->GetCount())
		{
			count -= GetInventoryItem(i)->GetCount();
			GetInventoryItem(i)->SetCount(0);

			if (0 == count)
				return;
		}
		else
		{
			GetInventoryItem(i)->SetCount(GetInventoryItem(i)->GetCount() - count);
			return;
		}
	}

	// ����ó���� ���ϴ�.
	if (count)
		sys_log(0, "CHARACTER::RemoveSpecifyItem cannot remove enough item vnum %u, still remain %d", vnum, count);
}

int CHARACTER::CountSpecifyTypeItem(BYTE type) const
{
	int	count = 0;

#ifdef __ENABLE_EXTEND_INVEN_SYSTEM__
	for (int i = 0; i < Inventory_Size(); ++i)
#else
	for (UINT i = 0; i < INVENTORY_MAX_NUM; ++i)
#endif
	{
		LPITEM pItem = GetInventoryItem(i);
		if (pItem != NULL && pItem->GetType() == type)
		{
			count += pItem->GetCount();
		}
	}

	return count;
}

void CHARACTER::RemoveSpecifyTypeItem(BYTE type, int count)
{
	if (0 == count)
		return;

#ifdef __ENABLE_EXTEND_INVEN_SYSTEM__
	for (int i = 0; i < Inventory_Size(); ++i)
#else
	for (UINT i = 0; i < INVENTORY_MAX_NUM; ++i)
#endif
	{
		if (NULL == GetInventoryItem(i))
			continue;

		if (GetInventoryItem(i)->GetType() != type)
			continue;

		//���� ������ ��ϵ� �����̸� �Ѿ��. (���� �������� �Ǹŵɶ� �� �κ����� ���� ��� ����!)
		if(m_pkMyShop)
		{
			bool isItemSelling = m_pkMyShop->IsSellingItem(GetInventoryItem(i)->GetID());
			if (isItemSelling)
				continue;
		}

		if (count >= GetInventoryItem(i)->GetCount())
		{
			count -= GetInventoryItem(i)->GetCount();
			GetInventoryItem(i)->SetCount(0);

			if (0 == count)
				return;
		}
		else
		{
			GetInventoryItem(i)->SetCount(GetInventoryItem(i)->GetCount() - count);
			return;
		}
	}
}

void CHARACTER::AutoGiveItem(LPITEM item, bool longOwnerShip
#ifdef __HIGHLIGHT_SYSTEM__
, bool isHighLight
#endif
)
{
	if (NULL == item)
	{
		sys_err ("NULL point.");
		return;
	}
	if (item->GetOwner())
	{
		sys_err ("item %d 's owner exists!",item->GetID());
		return;
	}

#ifdef ENABLE_EXTRA_INVENTORY
	if (item->IsExtraItem() && item->IsStackable() && !IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_STACK))
	{
#ifdef ENABLE_NEW_STACK_LIMIT
		int 
#else
		BYTE 
#endif
		bCount = item->GetCount();
		for (int i = 0; i < EXTRA_INVENTORY_MAX_NUM; ++i)
		{
			LPITEM item2 = GetExtraInventoryItem(i);
			if (!item2)
				continue;

			if (item2->GetVnum() == item->GetVnum())
			{
				int j = 0;
				for (j = 0; j < ITEM_SOCKET_MAX_NUM; ++j)
				if (item2->GetSocket(j) != item->GetSocket(j))
					break;

				if (j != ITEM_SOCKET_MAX_NUM)
					continue;

#ifdef ENABLE_NEW_STACK_LIMIT
				int 
#else
				BYTE 
#endif
				bCount2 = MIN(g_bItemCountLimit - item2->GetCount(), bCount); // change type for some
				bCount -= bCount2;
				item2->SetCount(item2->GetCount() + bCount2);
				if (bCount == 0) {
					M2_DESTROY_ITEM(item);
					item->SetCount(bCount);
					return;
				} else {
					item->SetCount(bCount);
				}
			}
		}
	}
	else if (item->IsStackable() && !IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_STACK))
#else
	if (item->IsStackable() && !IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_STACK))
#endif
	{
#ifdef ENABLE_NEW_STACK_LIMIT
		int 
#else
		BYTE 
#endif
		bCount = item->GetCount();
		for (int i = 0; i < INVENTORY_MAX_NUM; ++i)
		{
			LPITEM item2 = GetInventoryItem(i);
			if (!item2)
				continue;

			if (item2->GetVnum() == item->GetVnum())
			{
				int j = 0;
				for (j = 0; j < ITEM_SOCKET_MAX_NUM; ++j)
				if (item2->GetSocket(j) != item->GetSocket(j))
					break;

				if (j != ITEM_SOCKET_MAX_NUM)
					continue;

#ifdef ENABLE_NEW_STACK_LIMIT
				int 
#else
				BYTE 
#endif
				bCount2 = MIN(g_bItemCountLimit - item2->GetCount(), bCount); // change type for some
				bCount -= bCount2;
				item2->SetCount(item2->GetCount() + bCount2);
				if (bCount == 0) {
					M2_DESTROY_ITEM(item);
					item->SetCount(bCount);
					return;
				} else {
					item->SetCount(bCount);
				}
			}
		}
	}

	int cell;
	if (item->IsDragonSoul())
	{
		cell = GetEmptyDragonSoulInventory(item);
	}
#ifdef ENABLE_EXTRA_INVENTORY
	else if (item->IsExtraItem())
	{
		cell = GetEmptyExtraInventory(item);
	}
#endif
	else
	{
		cell = GetEmptyInventory (item->GetSize());
	}

	if (cell != -1)
	{
		if (item->IsDragonSoul())
			item->AddToCharacter(this, TItemPos(DRAGON_SOUL_INVENTORY, cell)
#ifdef __HIGHLIGHT_SYSTEM__
			, isHighLight
#endif
			);
#ifdef ENABLE_EXTRA_INVENTORY
		else if (item->IsExtraItem())
			item->AddToCharacter(this, TItemPos(EXTRA_INVENTORY, cell)
#ifdef __HIGHLIGHT_SYSTEM__
			, isHighLight
#endif
			);
#endif
		else
			item->AddToCharacter(this, TItemPos(INVENTORY, cell)
#ifdef __HIGHLIGHT_SYSTEM__
			, isHighLight
#endif
			);

		LogManager::instance().ItemLog(this, item, "SYSTEM", item->GetName());

		if (item->GetType() == ITEM_USE && item->GetSubType() == USE_POTION)
		{
			TQuickslot * pSlot;

			if (GetQuickslot(0, &pSlot) && pSlot->type == QUICKSLOT_TYPE_NONE)
			{
				TQuickslot slot;
				slot.type = QUICKSLOT_TYPE_ITEM;
				slot.pos = cell;
				SetQuickslot(0, slot);
			}
		}
	}
	else
	{
		item->AddToGround (GetMapIndex(), GetXYZ());
#ifdef ENABLE_NEWSTUFF
		item->StartDestroyEvent(g_aiItemDestroyTime[ITEM_DESTROY_TIME_AUTOGIVE]);
#else
		item->StartDestroyEvent();
#endif

		if (longOwnerShip)
			item->SetOwnership (this, 300);
		else
			item->SetOwnership (this, 60);
		LogManager::instance().ItemLog(this, item, "SYSTEM_DROP", item->GetName());
	}
}

#ifdef ENABLE_DS_REFINE_ALL
bool CHARACTER::AutoGiveDS(LPITEM item, bool longOwnerShip) {
	if (item == NULL) {
		sys_err ("NULL point.");
		return false;
	}

	if (item->GetOwner()) {
		sys_err ("item %d 's owner exists!", item->GetID());
		return false;
	}

	if (!item->IsDragonSoul()) {
		sys_err ("item %d is not alchemy!", item->GetID());
		return false;
	}

	int cell = GetEmptyDragonSoulInventory(item);
	if (cell != -1)
	{
		item->AddToCharacter(this, TItemPos(DRAGON_SOUL_INVENTORY, cell)
#ifdef __HIGHLIGHT_SYSTEM__
		, true
#endif
		);

		LogManager::instance().ItemLog(this, item, "SYSTEM", item->GetName());
	}
	else
	{
		item->AddToGround(GetMapIndex(), GetXYZ());
#ifdef ENABLE_NEWSTUFF
		item->StartDestroyEvent(g_aiItemDestroyTime[ITEM_DESTROY_TIME_AUTOGIVE]);
#else
		item->StartDestroyEvent();
#endif

		if (longOwnerShip) {
			item->SetOwnership(this, 300);
		} else {
			item->SetOwnership(this, 60);
		}

		LogManager::instance().ItemLog(this, item, "SYSTEM_DROP", item->GetName());
	}

	return true;
}
#endif

LPITEM CHARACTER::AutoGiveItem(DWORD dwItemVnum,
#ifdef ENABLE_NEW_STACK_LIMIT
int 
#else
BYTE 
#endif
bCount, int iRarePct, bool bMsg
#ifdef __HIGHLIGHT_SYSTEM__
, bool isHighLight
#endif
)
{
	TItemTable * p = ITEM_MANAGER::instance().GetTable(dwItemVnum);

	if (!p)
		return NULL;

	DBManager::instance().SendMoneyLog(MONEY_LOG_DROP, dwItemVnum, bCount);


#ifdef ENABLE_EXTRA_INVENTORY
	if (p->dwFlags & ITEM_FLAG_STACKABLE && ITEM_MANAGER::instance().IsExtraItem(dwItemVnum))
	{
		for (int i = 0; i < EXTRA_INVENTORY_MAX_NUM; ++i)
		{
			LPITEM item = GetExtraInventoryItem(i);

			if (!item)
				continue;

			if (item->GetVnum() == dwItemVnum && FN_check_item_socket(item))
			{
				if (IS_SET(p->dwFlags, ITEM_FLAG_MAKECOUNT))
				{
					if (bCount < p->alValues[1])
						bCount = p->alValues[1];
				}
				
#ifdef ENABLE_NEW_STACK_LIMIT
				int 
#else
				BYTE 
#endif
				bCount2 = MIN(g_bItemCountLimit - item->GetCount(), bCount); // change type for some
				bCount -= bCount2;

				item->SetCount(item->GetCount() + bCount2);

				if (bCount == 0)
				{
#ifdef TEXTS_IMPROVEMENT
					if (bMsg) {
						ChatPacketNew(
#ifdef ENABLE_NEW_CHAT
						CHAT_TYPE_INFO_ITEM
#else
						CHAT_TYPE_INFO
#endif
						, 102, "%s", item->GetName(GetDesc() ? GetDesc()->GetLanguage() : 0));
					}
#endif
					
					return item;
				}
			}
		}
	}
	else if (p->dwFlags & ITEM_FLAG_STACKABLE && p->bType != ITEM_BLEND)
#else
	if (p->dwFlags & ITEM_FLAG_STACKABLE && p->bType != ITEM_BLEND)
#endif
	{
		for (int i = 0; i < INVENTORY_MAX_NUM; ++i)
		{
			LPITEM item = GetInventoryItem(i);

			if (!item)
				continue;

#ifdef ENABLE_SORT_INVEN
			if (item->GetOriginalVnum() == dwItemVnum && FN_check_item_socket(item))
#else
			if (item->GetVnum() == dwItemVnum && FN_check_item_socket(item))	
#endif
			{
				if (IS_SET(p->dwFlags, ITEM_FLAG_MAKECOUNT))
				{
					if (bCount < p->alValues[1])
						bCount = p->alValues[1];
				}
				
#ifdef ENABLE_NEW_STACK_LIMIT
				int 
#else
				BYTE 
#endif
				bCount2 = MIN(g_bItemCountLimit - item->GetCount(), bCount);
				bCount -= bCount2;

				item->SetCount(item->GetCount() + bCount2);

				if (bCount == 0)
				{
#ifdef TEXTS_IMPROVEMENT
					if (bMsg) {
						ChatPacketNew(
#ifdef ENABLE_NEW_CHAT
						CHAT_TYPE_INFO_ITEM
#else
						CHAT_TYPE_INFO
#endif
						, 102, "%s", item->GetName(GetDesc() ? GetDesc()->GetLanguage() : 0));
					}
#endif
					
					return item;
				}
			}
		}
	}

	LPITEM item = ITEM_MANAGER::instance().CreateItem(dwItemVnum, bCount, 0, true);

	if (!item)
	{
		sys_err("cannot create item by vnum %u (name: %s)", dwItemVnum, GetName());
		return NULL;
	}

	if (item->GetType() == ITEM_BLEND)
	{
		for (int i=0; i < INVENTORY_MAX_NUM; i++)
		{
			LPITEM inv_item = GetInventoryItem(i);

			if (inv_item == NULL) continue;

			if (inv_item->GetType() == ITEM_BLEND)
			{
				if (inv_item->GetVnum() == item->GetVnum())
				{
					if (inv_item->GetSocket(0) == item->GetSocket(0) &&
							inv_item->GetSocket(1) == item->GetSocket(1) &&
							inv_item->GetSocket(2) == item->GetSocket(2) &&
							inv_item->GetCount() < g_bItemCountLimit)
					{
						inv_item->SetCount(inv_item->GetCount() + item->GetCount());
						return inv_item;
					}
				}
			}
		}
	}

	int iEmptyCell;
	if (item->IsDragonSoul())
	{
		iEmptyCell = GetEmptyDragonSoulInventory(item);
	}
#ifdef ENABLE_EXTRA_INVENTORY
	else if (item->IsExtraItem())
		iEmptyCell = GetEmptyExtraInventory(item);
#endif
	else
		iEmptyCell = GetEmptyInventory(item->GetSize());

	if (iEmptyCell != -1)
	{
#ifdef TEXTS_IMPROVEMENT
		if (bMsg) {
			ChatPacketNew(
#ifdef ENABLE_NEW_CHAT
			CHAT_TYPE_INFO_ITEM
#else
			CHAT_TYPE_INFO
#endif
			, 102, "%s", item->GetName(GetDesc() ? GetDesc()->GetLanguage() : 0));
		}
#endif
		
		if (item->IsDragonSoul())
			item->AddToCharacter(this, TItemPos(DRAGON_SOUL_INVENTORY, iEmptyCell)
#ifdef __HIGHLIGHT_SYSTEM__
			, isHighLight
#endif
			);
#ifdef ENABLE_EXTRA_INVENTORY
		else if (item->IsExtraItem())
			item->AddToCharacter(this, TItemPos(EXTRA_INVENTORY, iEmptyCell)
#ifdef __HIGHLIGHT_SYSTEM__
			, isHighLight
#endif
			);
#endif
		else
			item->AddToCharacter(this, TItemPos(INVENTORY, iEmptyCell)
#ifdef __HIGHLIGHT_SYSTEM__
			, isHighLight
#endif
			);
		LogManager::instance().ItemLog(this, item, "SYSTEM", item->GetName());

		if (item->GetType() == ITEM_USE && item->GetSubType() == USE_POTION)
		{
			TQuickslot * pSlot;

			if (GetQuickslot(0, &pSlot) && pSlot->type == QUICKSLOT_TYPE_NONE)
			{
				TQuickslot slot;
				slot.type = QUICKSLOT_TYPE_ITEM;
				slot.pos = iEmptyCell;
				SetQuickslot(0, slot);
			}
		}
	}
	else
	{
		item->AddToGround(GetMapIndex(), GetXYZ());
#ifdef ENABLE_NEWSTUFF
		item->StartDestroyEvent(g_aiItemDestroyTime[ITEM_DESTROY_TIME_AUTOGIVE]);
#else
		item->StartDestroyEvent();
#endif
		// ��Ƽ ��� flag�� �ɷ��ִ� �������� ���,
		// �κ��� �� ������ ��� ��¿ �� ���� ����Ʈ���� �Ǹ�,
		// ownership�� �������� ����� ������(300��) �����Ѵ�.
		if (IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_DROP))
			item->SetOwnership(this, 300);
		else
			item->SetOwnership(this, 60);
		LogManager::instance().ItemLog(this, item, "SYSTEM_DROP", item->GetName());
	}

	sys_log(0,
		"7: %d %d", dwItemVnum, bCount);
	return item;
}

bool CHARACTER::GiveItem(LPCHARACTER victim, TItemPos Cell)
{
	if (!CanHandleItem())
		return false;

	// @fixme150 BEGIN
	if (quest::CQuestManager::instance().GetPCForce(GetPlayerID())->IsRunning() == true)
	{
#ifdef TEXTS_IMPROVEMENT
		ChatPacketNew(CHAT_TYPE_INFO, 740, "");
#endif
		return false;
	}
	// @fixme150 END

	LPITEM item = GetItem(Cell);

	if (item && !item->IsExchanging())
	{
		if (victim->CanReceiveItem(this, item))
		{
			victim->ReceiveItem(this, item);
			return true;
		}
	}

	return false;
}

bool CHARACTER::CanReceiveItem(LPCHARACTER from, LPITEM item) const
{
	if (IsPC())
		return false;

	// TOO_LONG_DISTANCE_EXCHANGE_BUG_FIX
	if (DISTANCE_APPROX(GetX() - from->GetX(), GetY() - from->GetY()) > 2000)
		return false;
	// END_OF_TOO_LONG_DISTANCE_EXCHANGE_BUG_FIX

	switch (GetRaceNum())
	{
		case fishing::CAMPFIRE_MOB:
			if (item->GetType() == ITEM_FISH &&
					(item->GetSubType() == FISH_ALIVE || item->GetSubType() == FISH_DEAD))
				return true;
			break;

		case fishing::FISHER_MOB:
			if (item->GetType() == ITEM_ROD)
				return true;
			break;

		case BLACKSMITH_WEAPON_MOB:
		case DEVILTOWER_BLACKSMITH_WEAPON_MOB:
			if (item->GetType() == ITEM_WEAPON && item->GetRefinedVnum())
				return true;
			else
				return false;
			break;
		case BLACKSMITH_ARMOR_MOB:
		case DEVILTOWER_BLACKSMITH_ARMOR_MOB:
			if ((item->GetType() == ITEM_BELT || (item->GetType() == ITEM_ARMOR && (item->GetSubType() == ARMOR_BODY || item->GetSubType() == ARMOR_SHIELD || item->GetSubType() == ARMOR_HEAD))) && item->GetRefinedVnum())
				return true;
			else
				return false;
			break;
		case BLACKSMITH_ACCESSORY_MOB:
		case DEVILTOWER_BLACKSMITH_ACCESSORY_MOB:
			if (item->GetType() == ITEM_ARMOR && !(item->GetSubType() == ARMOR_BODY || item->GetSubType() == ARMOR_SHIELD || item->GetSubType() == ARMOR_HEAD
#ifdef ENABLE_PENDANT
			 || item->GetSubType() == ARMOR_PENDANT
#endif
			) && item->GetRefinedVnum())
				return true;
			else
				return false;
			break;
		case BLACKSMITH_MOB:
		case BLACKSMITH2_MOB:
			if (item->GetRefinedVnum() && item->GetRefineSet()) {
				return true;
			} else {
				return false;
			}
		case ALCHEMIST_MOB:
			if (item->GetRefinedVnum())
				return true;
			break;

		case 20101:
		case 20102:
		case 20103:
			// �ʱ� ��
			if (item->GetVnum() == ITEM_REVIVE_HORSE_1)
			{
				if (!IsDead())
				{
#ifdef TEXTS_IMPROVEMENT
					from->ChatPacketNew(CHAT_TYPE_INFO, 467, "");
#endif
					return false;
				}
				return true;
			}
			else if (item->GetVnum() == ITEM_HORSE_FOOD_1)
			{
				if (IsDead())
				{
#ifdef TEXTS_IMPROVEMENT
					from->ChatPacketNew(CHAT_TYPE_INFO, 466, "");
#endif
					return false;
				}
				return true;
			}
			else if (item->GetVnum() == ITEM_HORSE_FOOD_2 || item->GetVnum() == ITEM_HORSE_FOOD_3)
			{
				return false;
			}
			break;
		case 20104:
		case 20105:
		case 20106:
			// �߱� ��
			if (item->GetVnum() == ITEM_REVIVE_HORSE_2)
			{
				if (!IsDead())
				{
#ifdef TEXTS_IMPROVEMENT
					from->ChatPacketNew(CHAT_TYPE_INFO, 467, "");
#endif
					return false;
				}
				return true;
			}
			else if (item->GetVnum() == ITEM_HORSE_FOOD_2)
			{
				if (IsDead())
				{
#ifdef TEXTS_IMPROVEMENT
					from->ChatPacketNew(CHAT_TYPE_INFO, 466, "");
#endif
					return false;
				}
				return true;
			}
			else if (item->GetVnum() == ITEM_HORSE_FOOD_1 || item->GetVnum() == ITEM_HORSE_FOOD_3)
			{
				return false;
			}
			break;
		case 20107:
		case 20108:
		case 20109:
			// ��� ��
			if (item->GetVnum() == ITEM_REVIVE_HORSE_3)
			{
				if (!IsDead())
				{
#ifdef TEXTS_IMPROVEMENT
					from->ChatPacketNew(CHAT_TYPE_INFO, 467, "");
#endif
					return false;
				}
				return true;
			}
			else if (item->GetVnum() == ITEM_HORSE_FOOD_3)
			{
				if (IsDead())
				{
#ifdef TEXTS_IMPROVEMENT
					from->ChatPacketNew(CHAT_TYPE_INFO, 466, "");
#endif
					return false;
				}
				return true;
			}
			else if (item->GetVnum() == ITEM_HORSE_FOOD_1 || item->GetVnum() == ITEM_HORSE_FOOD_2)
			{
				return false;
			}
			break;
	}

	//if (IS_SET(item->GetFlag(), ITEM_FLAG_QUEST_GIVE))
	{
		return true;
	}

	return false;
}

void CHARACTER::ReceiveItem(LPCHARACTER from, LPITEM item)
{
	if (IsPC())
		return;

	switch (GetRaceNum())
	{
		case fishing::CAMPFIRE_MOB:
			if (item->GetType() == ITEM_FISH && (item->GetSubType() == FISH_ALIVE || item->GetSubType() == FISH_DEAD))
				fishing::Grill(from, item);
			else
			{
				// TAKE_ITEM_BUG_FIX
				from->SetQuestNPCID(GetVID());
				// END_OF_TAKE_ITEM_BUG_FIX
				quest::CQuestManager::instance().TakeItem(from->GetPlayerID(), GetRaceNum(), item);
			}
			break;

		// DEVILTOWER_NPC
		case DEVILTOWER_BLACKSMITH_WEAPON_MOB:
		case DEVILTOWER_BLACKSMITH_ARMOR_MOB:
		case DEVILTOWER_BLACKSMITH_ACCESSORY_MOB: {
			int set = item->GetRefineSet();
			if (item->GetRefinedVnum() != 0 && set != 0 /*&& item->GetRefineSet() < 500*/
#ifdef ENABLE_ITEM_EXTRA_PROTO
			 && set != 1021
			 && set != 1022
			 && set != 1023
			 && set != 1024
			 && set != 19
			 && set != 20
			 && set != 21
			 && set != 22
			 && set != 28
			 && set != 29
			 && set != 30
			 && set != 31
			 && set != 32
			 && set != 396
			 && set != 397
			 && set != 398
			 && set != 399
			 && set != 640
			 && set != 641
			 && set != 642
			 && set != 643
			 && set != 370
			 && set != 371
			 && set != 372
			 && set != 373
			 && set != 461
			 && set != 462
			 && set != 463
			 && set != 464
			 && set != 474
			 && set != 475
			 && set != 476
			 && set != 477
			 && set != 487
			 && set != 488
			 && set != 489
			 && set != 490
			 && set != 235
			 && set != 236
			 && set != 237
			 && set != 238
			 && set != 383
			 && set != 384
			 && set != 385
			 && set != 386
			 && set != 769
			 && set != 770
			 && set != 771
			 && set != 772
			 && set != 995
			 && set != 996
			 && set != 997
			 && set != 998
			 && set != 1017
			 && set != 1018
			 && set != 1019
			 && set != 1020
			 && set != 448
			 && set != 449
			 && set != 450
			 && set != 451
			 && set != 430
			 && set != 431
			 && set != 432
			 && set != 433
			 && set != 325
			 && set != 326
			 && set != 327
			 && set != 328
#endif
			)
			{
				from->SetRefineNPC(this);
				from->RefineInformation(item->GetCell(), REFINE_TYPE_MONEY_ONLY);
			}
#ifdef TEXTS_IMPROVEMENT
			else {
				from->ChatPacketNew(CHAT_TYPE_INFO, 427, "");
			}
#endif
			break;
		}
		// END_OF_DEVILTOWER_NPC

		case BLACKSMITH_MOB:
		case BLACKSMITH2_MOB:
		case BLACKSMITH_WEAPON_MOB:
		case BLACKSMITH_ARMOR_MOB:
		case BLACKSMITH_ACCESSORY_MOB:
			if (item->GetRefinedVnum())
			{
				from->SetRefineNPC(this);
				from->RefineInformation(item->GetCell(), REFINE_TYPE_NORMAL);
			}
#ifdef TEXTS_IMPROVEMENT
			else {
				from->ChatPacketNew(CHAT_TYPE_INFO, 427, "");
			}
#endif
			break;
		case 20101:
		case 20102:
		case 20103:
		case 20104:
		case 20105:
		case 20106:
		case 20107:
		case 20108:
		case 20109:
			if (item->GetVnum() == ITEM_REVIVE_HORSE_1 ||
					item->GetVnum() == ITEM_REVIVE_HORSE_2 ||
					item->GetVnum() == ITEM_REVIVE_HORSE_3)
			{
				from->ReviveHorse();
				item->SetCount(item->GetCount()-1);
#ifdef TEXTS_IMPROVEMENT
				from->ChatPacketNew(CHAT_TYPE_INFO, 329, "%s", item->GetName());
#endif
			}
			else if (item->GetVnum() == ITEM_HORSE_FOOD_1 ||
					item->GetVnum() == ITEM_HORSE_FOOD_2 ||
					item->GetVnum() == ITEM_HORSE_FOOD_3)
			{
				from->FeedHorse();
#ifdef TEXTS_IMPROVEMENT
				from->ChatPacketNew(CHAT_TYPE_INFO, 112, "%s", item->GetName());
#endif
				item->SetCount(item->GetCount()-1);
				EffectPacket(SE_HPUP_RED);
			}
			break;

		default:
			sys_log(0, "TakeItem %s %d %s", from->GetName(), GetRaceNum(), item->GetName());
			from->SetQuestNPCID(GetVID());
			quest::CQuestManager::instance().TakeItem(from->GetPlayerID(), GetRaceNum(), item);
			break;
	}
}

bool CHARACTER::IsEquipUniqueItem(DWORD dwItemVnum) const
{
	{
		LPITEM u = GetWear(WEAR_UNIQUE1);

		if (u && u->GetVnum() == dwItemVnum)
			return true;
	}

	{
		LPITEM u = GetWear(WEAR_UNIQUE2);

		if (u && u->GetVnum() == dwItemVnum)
			return true;
	}

	{
		LPITEM u = GetWear(WEAR_COSTUME_MOUNT);

		if (u && u->GetVnum() == dwItemVnum)
			return true;
	}

	// �������� ��� ������(�ߺ�) ������ üũ�Ѵ�.
	if (dwItemVnum == UNIQUE_ITEM_RING_OF_LANGUAGE)
		return IsEquipUniqueItem(UNIQUE_ITEM_RING_OF_LANGUAGE_SAMPLE);

	return false;
}

// CHECK_UNIQUE_GROUP
bool CHARACTER::IsEquipUniqueGroup(DWORD dwGroupVnum) const
{
	{
		LPITEM u = GetWear(WEAR_UNIQUE1);

		if (u && u->GetSpecialGroup() == (int) dwGroupVnum)
			return true;
	}

	{
		LPITEM u = GetWear(WEAR_UNIQUE2);

		if (u && u->GetSpecialGroup() == (int) dwGroupVnum)
			return true;
	}

	{
		LPITEM u = GetWear(WEAR_COSTUME_MOUNT);

		if (u && u->GetSpecialGroup() == (int)dwGroupVnum)
			return true;
	}

	return false;
}
// END_OF_CHECK_UNIQUE_GROUP

void CHARACTER::SetRefineMode(int iAdditionalCell)
{
	m_iRefineAdditionalCell = iAdditionalCell;
	m_bUnderRefine = true;
}

void CHARACTER::ClearRefineMode()
{
	m_bUnderRefine = false;
	SetRefineNPC( NULL );
}

bool CHARACTER::GiveItemFromSpecialItemGroup(DWORD dwGroupNum, std::vector<DWORD> &dwItemVnums,
											std::vector<DWORD> &dwItemCounts, std::vector <LPITEM> &item_gets, int &count)
{
	const CSpecialItemGroup* pGroup = ITEM_MANAGER::instance().GetSpecialItemGroup(dwGroupNum);

	if (!pGroup)
	{
		sys_err("cannot find special item group %d", dwGroupNum);
		return false;
	}

	std::vector <int> idxes;
	int n = pGroup->GetMultiIndex(idxes);

	bool bSuccess;

	for (int i = 0; i < n; i++)
	{
		bSuccess = false;
		int idx = idxes[i];
		DWORD dwVnum = pGroup->GetVnum(idx);
		DWORD dwCount = pGroup->GetCount(idx);
		int	iRarePct = pGroup->GetRarePct(idx);
		LPITEM item_get = NULL;
		switch (dwVnum)
		{
			case CSpecialItemGroup::GOLD:
				PointChange(POINT_GOLD, dwCount);
				LogManager::instance().CharLog(this, dwCount, "TREASURE_GOLD", "");

				bSuccess = true;
				break;
			case CSpecialItemGroup::EXP:
				{
					PointChange(POINT_EXP, dwCount);
					LogManager::instance().CharLog(this, dwCount, "TREASURE_EXP", "");

					bSuccess = true;
				}
				break;

			case CSpecialItemGroup::MOB:
				{
					sys_log(0, "CSpecialItemGroup::MOB %d", dwCount);
					int x = GetX() + number(-500, 500);
					int y = GetY() + number(-500, 500);

					LPCHARACTER ch = CHARACTER_MANAGER::instance().SpawnMob(dwCount, GetMapIndex(), x, y, 0, true, -1);
					if (ch)
						ch->SetAggressive();
					bSuccess = true;
				}
				break;
			case CSpecialItemGroup::SLOW:
				{
					sys_log(0, "CSpecialItemGroup::SLOW %d", -(int)dwCount);
					AddAffect(AFFECT_SLOW, POINT_MOV_SPEED, -(int)dwCount, AFF_SLOW, 300, 0, true);
					bSuccess = true;
				}
				break;
			case CSpecialItemGroup::DRAIN_HP:
				{
					int iDropHP = GetMaxHP()*dwCount/100;
					sys_log(0, "CSpecialItemGroup::DRAIN_HP %d", -iDropHP);
					iDropHP = MIN(iDropHP, GetHP()-1);
					sys_log(0, "CSpecialItemGroup::DRAIN_HP %d", -iDropHP);
					PointChange(POINT_HP, -iDropHP);
					bSuccess = true;
				}
				break;
			case CSpecialItemGroup::POISON:
				{
					AttackedByPoison(NULL);
					bSuccess = true;
				}
				break;
#ifdef ENABLE_WOLFMAN_CHARACTER
			case CSpecialItemGroup::BLEEDING:
				{
					AttackedByBleeding(NULL);
					bSuccess = true;
				}
				break;
#endif
			case CSpecialItemGroup::MOB_GROUP:
				{
					int sx = GetX() - number(300, 500);
					int sy = GetY() - number(300, 500);
					int ex = GetX() + number(300, 500);
					int ey = GetY() + number(300, 500);
					CHARACTER_MANAGER::instance().SpawnGroup(dwCount, GetMapIndex(), sx, sy, ex, ey, NULL, true);

					bSuccess = true;
				}
				break;
			default:
				{
					item_get = AutoGiveItem(dwVnum, dwCount, iRarePct);

					if (item_get)
					{
						bSuccess = true;
					}
				}
				break;
		}

		if (bSuccess)
		{
			dwItemVnums.push_back(dwVnum);
			dwItemCounts.push_back(dwCount);
			item_gets.push_back(item_get);
			count++;

		}
		else
		{
			return false;
		}
	}
	return bSuccess;
}

// NEW_HAIR_STYLE_ADD
bool CHARACTER::ItemProcess_Hair(LPITEM item, int iDestCell)
{
	if (item->CheckItemUseLevel(GetLevel()) == false)
	{
#ifdef TEXTS_IMPROVEMENT
		ChatPacketNew(CHAT_TYPE_INFO, 405, "");
#endif
		return false;
	}

	DWORD hair = item->GetVnum();

	switch (GetJob())
	{
		case JOB_WARRIOR :
			hair -= 72000; // 73001 - 72000 = 1001 ���� ��� ��ȣ ����
			break;

		case JOB_ASSASSIN :
			hair -= 71250;
			break;

		case JOB_SURA :
			hair -= 70500;
			break;

		case JOB_SHAMAN :
			hair -= 69750;
			break;
#ifdef ENABLE_WOLFMAN_CHARACTER
		case JOB_WOLFMAN:
			break; // NOTE: �� ����ڵ�� �� ���̹Ƿ� �н�. (���� ���ý����� �̹� �ڽ�Ƭ���� ��ü �� ������)
#endif
		default :
			return false;
			break;
	}

	if (hair == GetPart(PART_HAIR))
	{
#ifdef TEXTS_IMPROVEMENT
		ChatPacketNew(CHAT_TYPE_INFO, 311, "");
#endif
		return true;
	}

	item->SetCount(item->GetCount() - 1);

	SetPart(PART_HAIR, hair);
	UpdatePacket();

	return true;
}
// END_NEW_HAIR_STYLE_ADD

bool CHARACTER::ItemProcess_Polymorph(LPITEM item)
{

#ifdef ENABLE_PVP_ADVANCED
	if ((GetDuel("BlockPoly")))
	{
#ifdef TEXTS_IMPROVEMENT
		ChatPacketNew(CHAT_TYPE_INFO, 516, "");
#endif
		return false;
	}
#endif

	if (IsPolymorphed()) {
#ifdef TEXTS_IMPROVEMENT
		ChatPacketNew(CHAT_TYPE_INFO, 437, "");
#endif
		return false;
	}

	if (true == IsRiding())
	{
#ifdef TEXTS_IMPROVEMENT
		ChatPacketNew(CHAT_TYPE_INFO, 741, "");
#endif
		return false;
	}

	DWORD dwVnum = item->GetSocket(0);

	if (dwVnum == 0)
	{
#ifdef TEXTS_IMPROVEMENT
		ChatPacketNew(CHAT_TYPE_INFO, 450, "");
#endif
		item->SetCount(item->GetCount()-1);
		return false;
	}

	const CMob* pMob = CMobManager::instance().Get(dwVnum);

	if (pMob == NULL)
	{
#ifdef TEXTS_IMPROVEMENT
		ChatPacketNew(CHAT_TYPE_INFO, 451, "");
#endif
		item->SetCount(item->GetCount()-1);
		return false;
	}

	switch (item->GetVnum())
	{
		case 70104 :
		case 70105 :
		case 70106 :
		case 70107 :
		case 71093 :
			{
				// �а��� ó��
				sys_log(0, "USE_POLYMORPH_BALL PID(%d) vnum(%d)", GetPlayerID(), dwVnum);

				// ���� ���� üũ
				int iPolymorphLevelLimit = MAX(0, 20 - GetLevel() * 3 / 10);
				if (pMob->m_table.bLevel >= GetLevel() + iPolymorphLevelLimit)
				{
#ifdef TEXTS_IMPROVEMENT
					ChatPacketNew(CHAT_TYPE_INFO, 275, "");
#endif
					return false;
				}

				int iDuration = GetSkillLevel(POLYMORPH_SKILL_ID) == 0 ? 5 : (5 + (5 + GetSkillLevel(POLYMORPH_SKILL_ID)/40 * 25));
				iDuration *= 60;

				DWORD dwBonus = 0;

				dwBonus = (2 + GetSkillLevel(POLYMORPH_SKILL_ID)/40) * 100;

				AddAffect(AFFECT_POLYMORPH, POINT_POLYMORPH, dwVnum, AFF_POLYMORPH, iDuration, 0, true);
				AddAffect(AFFECT_POLYMORPH, POINT_ATT_BONUS, dwBonus, AFF_POLYMORPH, iDuration, 0, false);

				item->SetCount(item->GetCount()-1);
			}
			break;

		case 50322:
			{
				// ����

				// �а��� ó��
				// ����0                ����1           ����2
				// �а��� ���� ��ȣ   ��������        �а��� ����
				sys_log(0, "USE_POLYMORPH_BOOK: %s(%u) vnum(%u)", GetName(), GetPlayerID(), dwVnum);

				if (CPolymorphUtils::instance().PolymorphCharacter(this, item, pMob) == true)
				{
					CPolymorphUtils::instance().UpdateBookPracticeGrade(this, item);
				}
				else
				{
				}
			}
			break;

		default :
			sys_err("POLYMORPH invalid item passed PID(%d) vnum(%d)", GetPlayerID(), item->GetOriginalVnum());
			return false;
	}

	return true;
}

bool CHARACTER::CanDoCube() const
{
	if (m_bIsObserver)	return false;
	if (GetShop())		return false;
	if (GetMyShop())	return false;
	if (m_bUnderRefine)	return false;
	if (IsWarping())	return false;

	return true;
}

bool CHARACTER::UnEquipSpecialRideUniqueItem()
{
	LPITEM Unique1 = GetWear(WEAR_UNIQUE1);
	LPITEM Unique2 = GetWear(WEAR_UNIQUE2);
	LPITEM Unique3 = GetWear(WEAR_COSTUME_MOUNT);
/*
#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
	LPITEM MountCostume = GetWear(WEAR_COSTUME_MOUNT);
#endif
*/

	if( NULL != Unique1 )
	{
		if( UNIQUE_GROUP_SPECIAL_RIDE == Unique1->GetSpecialGroup() )
		{
			return UnequipItem(Unique1);
		}
	}

	if( NULL != Unique2 )
	{
		if( UNIQUE_GROUP_SPECIAL_RIDE == Unique2->GetSpecialGroup() )
		{
			return UnequipItem(Unique2);
		}
	}

	if (NULL != Unique3)
	{
		if (UNIQUE_GROUP_SPECIAL_RIDE == Unique3->GetSpecialGroup())
		{
			return UnequipItem(Unique3);
		}
	}

/*#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
	if (MountCostume)
		return UnequipItem(MountCostume);
#endif*/

	return true;
}

#ifdef ENABLE_RECALL
void CHARACTER::AutoRecallProcess()
{
	if (!IsPC())
		return;
	
#ifdef __PET_SYSTEM__
	{
		const CAffect* pAffect = FindAffect(AFFECT_RECALL1);
		if (pAffect) {
			LPITEM pItem = FindItemByID(pAffect->dwFlag);
			if (pItem) {
				if (pItem->GetSocket(2) == false) {
					CPetSystem* petSystem = GetPetSystem();
					if (petSystem) {
						if (petSystem->CountSummoned() < 1) {
							CPetActor* pPet = petSystem->Summon(pItem->GetValue(1), pItem, "", false);
							if (!pPet)
								RemoveAffect(const_cast<CAffect*>(pAffect));
						}
					}
					else
						RemoveAffect(const_cast<CAffect*>(pAffect));
				}
			}
			else
				RemoveAffect(const_cast<CAffect*>(pAffect));
		}
	}
#endif
#ifdef __NEWPET_SYSTEM__
	{
		const CAffect* pAffect = FindAffect(AFFECT_RECALL2);
		if (pAffect) {
			LPITEM pItem = FindItemByID(pAffect->dwFlag);
			if (pItem) {
				if (pItem->GetSocket(0) == false) {
					CNewPetSystem* petSystem = GetNewPetSystem();
					if (petSystem) {
						if (petSystem->CountSummoned() < 1) {
							CNewPetActor* pPet = petSystem->Summon(pItem->GetValue(0), pItem, "", false);
							if (!pPet)
								RemoveAffect(const_cast<CAffect*>(pAffect));
						}
					}
					else
						RemoveAffect(const_cast<CAffect*>(pAffect));
				}
			}
			else
				RemoveAffect(const_cast<CAffect*>(pAffect));
		}
	}
#endif
}
#endif

void CHARACTER::AutoRecoveryItemProcess(const EAffectTypes type)
{
	if (true == IsDead() || true == IsStun())
		return;

	if (false == IsPC())
		return;

#ifdef ENABLE_PVP_ADVANCED	
	if (
#ifdef ENABLE_NEW_USE_POTION
	((type == AFFECT_AUTO_HP_RECOVERY2) ||
#endif
	(type == AFFECT_AUTO_HP_RECOVERY)
#ifdef ENABLE_NEW_USE_POTION
	)
#endif
	&& (GetDuel("BlockPotion")))
		return;
#endif
	
	if ((type != AFFECT_AUTO_HP_RECOVERY) && (type != AFFECT_AUTO_SP_RECOVERY)
#ifdef ENABLE_NEW_USE_POTION
		&& (type != AFFECT_AUTO_HP_RECOVERY2) && (type != AFFECT_AUTO_SP_RECOVERY2)
#endif
	)
		return;
	
	if (NULL != FindAffect(AFFECT_STUN))
		return;

	{
		const DWORD stunSkills[] = { SKILL_TANHWAN, SKILL_GEOMPUNG, SKILL_BYEURAK, SKILL_GIGUNG };

		for (size_t i=0 ; i < sizeof(stunSkills)/sizeof(DWORD) ; ++i)
		{
			const CAffect* p = FindAffect(stunSkills[i]);

			if (NULL != p && AFF_STUN == p->dwFlag)
				return;
		}
	}

	const CAffect* pAffect = FindAffect(type);
	const size_t idx_of_amount_of_used = 1;
	const size_t idx_of_amount_of_full = 2;

	if (NULL != pAffect)
	{
		LPITEM pItem = FindItemByID(pAffect->dwFlag);

		if (NULL != pItem && true == pItem->GetSocket(0))
		{
			if (!CArenaManager::instance().IsArenaMap(GetMapIndex())
#ifdef ENABLE_NEWSTUFF
				&& !(g_NoPotionsOnPVP && CPVPManager::instance().IsFighting(GetPlayerID()) && !IsAllowedPotionOnPVP(pItem->GetVnum()))
#endif
			)
			{
				const long amount_of_used = pItem->GetSocket(idx_of_amount_of_used);
				const long amount_of_full = pItem->GetSocket(idx_of_amount_of_full);

				const int32_t avail = amount_of_full - amount_of_used;

				int32_t amount = 0;
#ifdef ENABLE_NEW_USE_POTION
				if ((type == AFFECT_AUTO_HP_RECOVERY) || (type == AFFECT_AUTO_HP_RECOVERY2))
#else
				if (AFFECT_AUTO_HP_RECOVERY == type)
#endif
				{
					amount = GetMaxHP() - (GetHP() + GetPoint(POINT_HP_RECOVERY));
				}
#ifdef ENABLE_NEW_USE_POTION
				else if ((type == AFFECT_AUTO_SP_RECOVERY) || (type == AFFECT_AUTO_SP_RECOVERY2))
#else
				else if (AFFECT_AUTO_SP_RECOVERY == type)
#endif
				{
					amount = GetMaxSP() - (GetSP() + GetPoint(POINT_SP_RECOVERY));
				}

				if (amount > 0)
				{
					if (avail > amount)
					{
						const int pct_of_used = amount_of_used * 100 / amount_of_full;
						const int pct_of_will_used = (amount_of_used + amount) * 100 / amount_of_full;

						bool bLog = false;
						// ��뷮�� 10% ������ �α׸� ����
						// (��뷮�� %����, ���� �ڸ��� �ٲ� ������ �α׸� ����.)
						if ((pct_of_will_used / 10) - (pct_of_used / 10) >= 1)
							bLog = true;
						
#ifdef ENABLE_NEW_USE_POTION
						if (pItem->GetVnum() != ITEM_AUTO_HP_RECOVERY_X && pItem->GetVnum() != ITEM_AUTO_SP_RECOVERY_X)
							pItem->SetSocket(idx_of_amount_of_used, amount_of_used + amount, bLog);
#else
						pItem->SetSocket(idx_of_amount_of_used, amount_of_used + amount, bLog);
#endif
					}
					else if (pItem->GetVnum() != ITEM_AUTO_HP_RECOVERY_X && pItem->GetVnum() != ITEM_AUTO_SP_RECOVERY_X)
					{
						amount = avail;

						ITEM_MANAGER::instance().RemoveItem( pItem );
					}
					
#ifdef ENABLE_NEW_USE_POTION
					if ((type == AFFECT_AUTO_HP_RECOVERY) || (type == AFFECT_AUTO_HP_RECOVERY2))
#else
					if (AFFECT_AUTO_HP_RECOVERY == type)
#endif
					{
						PointChange( POINT_HP_RECOVERY, amount );
						EffectPacket( SE_AUTO_HPUP );
					}
#ifdef ENABLE_NEW_USE_POTION
					else if ((type == AFFECT_AUTO_SP_RECOVERY) || (type == AFFECT_AUTO_SP_RECOVERY2))
#else
					else if (AFFECT_AUTO_SP_RECOVERY == type)
#endif
					{
						PointChange( POINT_SP_RECOVERY, amount );
						EffectPacket( SE_AUTO_SPUP );
					}
				}
			}
			else
			{
				pItem->Lock(false);
				pItem->SetSocket(0, false);
				RemoveAffect( const_cast<CAffect*>(pAffect) );
			}
		}
		else
		{
			RemoveAffect( const_cast<CAffect*>(pAffect) );
		}
	}
}

bool CHARACTER::IsValidItemPosition(TItemPos Pos) const
{
	BYTE window_type = Pos.window_type;
	WORD cell = Pos.cell;

	switch (window_type)
	{
	case RESERVED_WINDOW:
		return false;

	case INVENTORY:
	case EQUIPMENT:
		return cell < (INVENTORY_AND_EQUIP_SLOT_MAX);

	case DRAGON_SOUL_INVENTORY:
		return cell < (DRAGON_SOUL_INVENTORY_MAX_NUM);
#ifdef ENABLE_SWITCHBOT
	case SWITCHBOT:
		return cell < SWITCHBOT_SLOT_COUNT;
#endif
	case SAFEBOX:
		if (NULL != m_pkSafebox)
			return m_pkSafebox->IsValidPosition(cell);
		else
			return false;

	case MALL:
		if (NULL != m_pkMall)
			return m_pkMall->IsValidPosition(cell);
		else
			return false;
		
#ifdef ENABLE_EXTRA_INVENTORY
	case EXTRA_INVENTORY:
		return cell < (EXTRA_INVENTORY_MAX_NUM);
#endif
	default:
		return false;
	}
}

/// ���� ĳ������ ���¸� �������� �־��� item�� ������ �� �ִ� �� Ȯ���ϰ�, �Ұ��� �ϴٸ� ĳ���Ϳ��� ������ �˷��ִ� �Լ�
bool CHARACTER::CanEquipNow(const LPITEM item, const TItemPos& srcCell, const TItemPos& destCell) /*const*/
{
	const TItemTable* itemTable = item->GetProto();
	//BYTE itemType = item->GetType();
	//BYTE itemSubType = item->GetSubType();

#ifdef ENABLE_PVP_ADVANCED
	if ((GetDuel("BlockChangeItem")))
	{
#ifdef TEXTS_IMPROVEMENT
		ChatPacketNew(CHAT_TYPE_INFO, 516, "");
#endif
		return false;
	}
#endif

	switch (GetJob())
	{
		case JOB_WARRIOR:
			if (item->GetAntiFlag() & ITEM_ANTIFLAG_WARRIOR)
				return false;
			break;

		case JOB_ASSASSIN:
			if (item->GetAntiFlag() & ITEM_ANTIFLAG_ASSASSIN)
				return false;
			break;

		case JOB_SHAMAN:
			if (item->GetAntiFlag() & ITEM_ANTIFLAG_SHAMAN)
				return false;
			break;

		case JOB_SURA:
			if (item->GetAntiFlag() & ITEM_ANTIFLAG_SURA)
				return false;
			break;
#ifdef ENABLE_WOLFMAN_CHARACTER
		case JOB_WOLFMAN:
			if (item->GetAntiFlag() & ITEM_ANTIFLAG_WOLFMAN)
				return false;
			break; // TODO: ������ ������ ���밡�ɿ��� ó��
#endif
	}

	for (int i = 0; i < ITEM_LIMIT_MAX_NUM; ++i)
	{
		long limit = itemTable->aLimits[i].lValue;
		switch (itemTable->aLimits[i].bType)
		{
			case LIMIT_LEVEL:
				if (GetLevel() < limit) {
#ifdef TEXTS_IMPROVEMENT
					ChatPacketNew(CHAT_TYPE_INFO, 325, "%d", limit);
#endif
					return false;
				}
				break;
			case LIMIT_STR:
				if (GetPoint(POINT_ST) < limit) {
#ifdef TEXTS_IMPROVEMENT
					ChatPacketNew(CHAT_TYPE_INFO, 269, "%d", limit);
#endif
					return false;
				}
				break;
			case LIMIT_INT:
				if (GetPoint(POINT_IQ) < limit) {
#ifdef TEXTS_IMPROVEMENT
					ChatPacketNew(CHAT_TYPE_INFO, 468, "%d", limit);
#endif
					return false;
				}
				break;
			case LIMIT_DEX:
				if (GetPoint(POINT_DX) < limit) {
#ifdef TEXTS_IMPROVEMENT
					ChatPacketNew(CHAT_TYPE_INFO, 352, "%d", limit);
#endif
					return false;
				}
				break;

			case LIMIT_CON:
				if (GetPoint(POINT_HT) < limit) {
#ifdef TEXTS_IMPROVEMENT
					ChatPacketNew(CHAT_TYPE_INFO, 481, "%d", limit);
#endif
					return false;
				}
				break;
		}
	}

	if (item->GetWearFlag() & WEARABLE_UNIQUE)
	{
		if ((GetWear(WEAR_UNIQUE1) && GetWear(WEAR_UNIQUE1)->IsSameSpecialGroup(item)) ||
			(GetWear(WEAR_UNIQUE2) && GetWear(WEAR_UNIQUE2)->IsSameSpecialGroup(item)) ||
			(GetWear(WEAR_COSTUME_MOUNT) && GetWear(WEAR_COSTUME_MOUNT)->IsSameSpecialGroup(item)))
		{
#ifdef TEXTS_IMPROVEMENT
			ChatPacketNew(CHAT_TYPE_INFO, 695, "");
#endif
			return false;
		}

		if (marriage::CManager::instance().IsMarriageUniqueItem(item->GetVnum()) &&
			!marriage::CManager::instance().IsMarried(GetPlayerID()))
		{
#ifdef TEXTS_IMPROVEMENT
			ChatPacketNew(CHAT_TYPE_INFO, 696, "");
#endif
			return false;
		}

	}

#ifdef ENABLE_BUG_FIXES
	if (item->GetType() == ITEM_COSTUME && item->GetSubType() == COSTUME_BODY)
	{
		LPITEM atakanxd = GetWear(WEAR_BODY);
		if (atakanxd && (atakanxd->GetVnum() >= 11901 && atakanxd->GetVnum() <= 11914))
		{
#ifdef TEXTS_IMPROVEMENT
			ChatPacketNew(CHAT_TYPE_INFO, 1129, "");
#endif
			return false;
		}
	}

	if (item->GetVnum() >= 11901 && item->GetVnum() <= 11914)
	{
		LPITEM atakan = GetWear(WEAR_COSTUME_BODY);
		if (atakan && (atakan->GetType() == ITEM_COSTUME && atakan->GetSubType() == COSTUME_BODY))
		{
#ifdef TEXTS_IMPROVEMENT
			ChatPacketNew(CHAT_TYPE_INFO, 1129, "");
#endif
			return false;
		}
	}
#endif

#ifdef ENABLE_DS_SET
	if ((DragonSoul_IsDeckActivated()) && (item->IsDragonSoul())) {
#ifdef TEXTS_IMPROVEMENT
		ChatPacketNew(CHAT_TYPE_INFO, 76, "");
#endif
		return false;
	}
#endif
	
	return true;
}

#

/// ���� ĳ������ ���¸� �������� ���� ���� item�� ���� �� �ִ� �� Ȯ���ϰ�, �Ұ��� �ϴٸ� ĳ���Ϳ��� ������ �˷��ִ� �Լ�
bool CHARACTER::CanUnequipNow(const LPITEM item, const TItemPos& srcCell, const TItemPos& destCell) {
	if (ITEM_BELT == item->GetType() && CBeltInventoryHelper::IsExistItemInBeltInventory(this)) {
#ifdef TEXTS_IMPROVEMENT
		ChatPacketNew(CHAT_TYPE_INFO, 366, "");
#endif
		return false;
	}

	// ������ ������ �� ���� ������
	if (IS_SET(item->GetFlag(), ITEM_FLAG_IRREMOVABLE))
		return false;

	// ������ unequip�� �κ��丮�� �ű� �� �� �ڸ��� �ִ� �� Ȯ��
	{
		int pos = -1;

		if (item->IsDragonSoul())
			pos = GetEmptyDragonSoulInventory(item);
		else
			pos = GetEmptyInventory(item->GetSize());

		if (pos == -1) {
#ifdef TEXTS_IMPROVEMENT
			ChatPacketNew(CHAT_TYPE_INFO, 366, "");
#endif
			return false;
		}
	}
	
#ifdef ENABLE_DS_SET
	if ((DragonSoul_IsDeckActivated()) && (item->IsDragonSoul())) {
#ifdef TEXTS_IMPROVEMENT
		ChatPacketNew(CHAT_TYPE_INFO, 76, "");
#endif
		return false;
	}
#endif
	
	return true;
}

#ifdef __ATTR_TRANSFER_SYSTEM__
bool CHARACTER::CanDoAttrTransfer() const
{
	if (m_bIsObserver)
		return false;
	
	if (GetShop())
		return false;
	
	if (GetMyShop())
		return false;
	
	if (m_bUnderRefine)
		return false;
	
	if (IsWarping())
		return false;
	
#ifdef ENABLE_ACCE_SYSTEM
	if ((m_bAcceCombination) || (m_bAcceAbsorption))
		return false;
#endif
	
	return true;
}
#endif

bool CHARACTER::DestroyItem(TItemPos Cell)
{
	LPITEM item = NULL;
	if (!CanHandleItem()) {
#ifdef TEXTS_IMPROVEMENT
		if (NULL != DragonSoul_RefineWindow_GetOpener()) {
			ChatPacketNew(CHAT_TYPE_INFO, 232, "");
		}
#endif
		
		return false;
	}

	if (IsDead())
		return false;

	if (!IsValidItemPosition(Cell) || !(item = GetItem(Cell)))
		return false;

	if (item->IsEquipped())
		return false;

	if (item->IsExchanging())
		return false;

	if (true == item->isLocked())
		return false;

	if (quest::CQuestManager::instance().GetPCForce(GetPlayerID())->IsRunning() == true)
		return false;

	if ((item->GetVnum() >= 55701) && (item->GetVnum() <= 55711)) {
		if (item->GetSocket(0) != 0)
			return false;
	}

#ifdef ENABLE_EXTRA_INVENTORY
	if (item->IsExtraItem()) {
	SyncQuickslot(QUICKSLOT_TYPE_ITEM_EXTRA, Cell.cell, 255);
	} else {
	SyncQuickslot(QUICKSLOT_TYPE_ITEM, Cell.cell, 255);
	}
#else
	SyncQuickslot(QUICKSLOT_TYPE_ITEM, Cell.cell, 255);
#endif

#ifdef ENABLE_BATTLE_PASS
	BYTE bBattlePassId = GetBattlePassId();
	if(bBattlePassId)
	{
		DWORD dwItemVnum, dwCnt;
		if(CBattlePass::instance().BattlePassMissionGetInfo(bBattlePassId, DESTROY_ITEM, &dwItemVnum, &dwCnt))
		{
			if(dwItemVnum == item->GetVnum() && GetMissionProgress(DESTROY_ITEM, bBattlePassId) < dwCnt)
				UpdateMissionProgress(DESTROY_ITEM, bBattlePassId, item->GetCount(), dwCnt);
		}
	}
#endif

#ifdef TEXTS_IMPROVEMENT
	ChatPacketNew(CHAT_TYPE_INFO, 47, "%s", item->GetName());
#endif
	ITEM_MANAGER::instance().RemoveItem(item, "DESTROY");
	return true;
}
