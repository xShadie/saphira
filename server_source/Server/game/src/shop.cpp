#include "stdafx.h"
#include "../../libgame/include/grid.h"
#include "constants.h"
#include "utils.h"
#include "config.h"
#include "shop.h"
#include "desc.h"
#include "desc_manager.h"
#include "char.h"
#include "char_manager.h"
#include "item.h"
#include "item_manager.h"
#include "buffer_manager.h"
#include "packet.h"
#include "log.h"
#include "db.h"
#include "questmanager.h"
#include "mob_manager.h"
#include "locale_service.h"
#include "battle_pass.h"

//#define ENABLE_SHOP_BLACKLIST
/* ------------------------------------------------------------------------------------ */
CShop::CShop()
	: m_dwVnum(0), m_dwNPCVnum(0), m_pkPC(NULL)
{
	m_pGrid = M2_NEW CGrid(5, 9);
}

CShop::~CShop()
{
	TPacketGCShop pack;

	pack.header		= HEADER_GC_SHOP;
	pack.subheader	= SHOP_SUBHEADER_GC_END;
	pack.size		= sizeof(TPacketGCShop);

	Broadcast(&pack, sizeof(pack));

	GuestMapType::iterator it;

	it = m_map_guest.begin();

	while (it != m_map_guest.end())
	{
		LPCHARACTER ch = it->first;
		ch->SetShop(NULL);
		++it;
	}

	M2_DELETE(m_pGrid);
}

void CShop::SetPCShop(LPCHARACTER ch)
{
	m_pkPC = ch;
}

bool CShop::Create(DWORD dwVnum, DWORD dwNPCVnum, TShopItemTable * pTable)
{
	/*
	   if (NULL == CMobManager::instance().Get(dwNPCVnum))
	   {
	   sys_err("No such a npc by vnum %d", dwNPCVnum);
	   return false;
	   }
	 */
	sys_log(0, "SHOP #%d (Shopkeeper %d)", dwVnum, dwNPCVnum);

	m_dwVnum = dwVnum;
	m_dwNPCVnum = dwNPCVnum;

	BYTE bItemCount;

	for (bItemCount = 0; bItemCount < SHOP_HOST_ITEM_MAX_NUM; ++bItemCount)
		if (0 == (pTable + bItemCount)->vnum)
			break;

	SetShopItems(pTable, bItemCount);
	return true;
}

void CShop::SetShopItems(TShopItemTable * pTable, BYTE bItemCount)
{
	if (bItemCount > SHOP_HOST_ITEM_MAX_NUM)
		return;

	m_pGrid->Clear();

	m_itemVector.resize(SHOP_HOST_ITEM_MAX_NUM);
	memset(&m_itemVector[0], 0, sizeof(SHOP_ITEM) * m_itemVector.size());

	for (int i = 0; i < bItemCount; ++i)
	{
		LPITEM pkItem = NULL;
		const TItemTable * item_table;

		if (m_pkPC)
		{
			pkItem = m_pkPC->GetItem(pTable->pos);

			if (!pkItem)
			{
				sys_err("cannot find item on pos (%d, %d) (name: %s)", pTable->pos.window_type, pTable->pos.cell, m_pkPC->GetName());
				continue;
			}

			item_table = pkItem->GetProto();
		}
		else
		{
			if (!pTable->vnum)
				continue;

			item_table = ITEM_MANAGER::instance().GetTable(pTable->vnum);
		}

		if (!item_table)
		{
			sys_err("Shop: no item table by item vnum #%d", pTable->vnum);
			continue;
		}

		int iPos;

		if (IsPCShop())
		{
			sys_log(0, "MyShop: use position %d", pTable->display_pos);
			iPos = pTable->display_pos;
		}
		else
			iPos = m_pGrid->FindBlank(1, item_table->bSize);

		if (iPos < 0)
		{
			sys_err("not enough shop window");
			continue;
		}

		if (!m_pGrid->IsEmpty(iPos, 1, item_table->bSize))
		{
			if (IsPCShop())
			{
				sys_err("not empty position for pc shop %s[%d]", m_pkPC->GetName(), m_pkPC->GetPlayerID());
			}
			else
			{
				sys_err("not empty position for npc shop");
			}
			continue;
		}

		m_pGrid->Put(iPos, 1, item_table->bSize);

		SHOP_ITEM & item = m_itemVector[iPos];

		item.pkItem = pkItem;
		item.itemid = 0;

		if (item.pkItem)
		{
			item.vnum = pkItem->GetVnum();
			item.count = pkItem->GetCount(); // PC ���� ��� ������ ������ ��¥ �������� �������� �Ѵ�.
#ifndef ENABLE_BUY_WITH_ITEM
			item.price = pTable->price;
#endif
			item.itemid	= pkItem->GetID();
		}
		else
		{
			item.vnum = pTable->vnum;
			item.count = pTable->count;
#ifndef ENABLE_BUY_WITH_ITEM
			if (IS_SET(item_table->dwFlags, ITEM_FLAG_COUNT_PER_1GOLD))
			{
				if (item_table->dwGold == 0)
					item.price = item.count;
				else
					item.price = item.count / item_table->dwGold;
			}
			else
				item.price = item_table->dwGold * item.count;
#endif
		}
		
#ifdef ENABLE_BUY_WITH_ITEM
		item.price = pTable->price;
		for (int i = 0; i < MAX_SHOP_PRICES; i++) {
			item.itemprice[i].vnum = pTable->itemprice[i].vnum;
			item.itemprice[i].count = pTable->itemprice[i].count;
		}
#endif
		
		char name[256];
		snprintf(name, sizeof(name), "%s (v: %d) (c: %d)", item_table->szName, item.vnum, item.count);
		sys_log(0, "SHOP_ITEM: ITEM: %s PRICE: %d", name, item.price);
		++pTable;
	}
}

#ifdef ENABLE_LONG_LONG
long long CShop::Buy(LPCHARACTER ch, BYTE pos
#ifdef ENABLE_BUY_STACK_FROM_SHOP
, bool multiple
#endif
)
#else
int CShop::Buy(LPCHARACTER ch, BYTE pos
#ifdef ENABLE_BUY_STACK_FROM_SHOP
, bool multiple
#endif
)
#endif
{
#ifdef ENABLE_BUY_STACK_FROM_SHOP
	bool ismultiple = multiple;
#else
	bool ismultiple = false;
#endif

	if (!ismultiple) {
#ifdef ENABLE_RESTRICT_GM_PERMISSIONS
		if (ch->GetGMLevel() > GM_PLAYER && ch->GetGMLevel() < GM_IMPLEMENTOR) {
			return SHOP_SUBHEADER_GC_OK;
		}
#endif

#ifdef ENABLE_LIMIT_BUY_SPEED
		int iPulse = thecore_pulse() - ch->GetLastBuyTime();
		if (iPulse < PASSES_PER_SEC(1)) {
			return SHOP_SUBHEADER_GC_OK;
		}
#endif
	}
#ifdef ENABLE_BUY_STACK_FROM_SHOP
	else
	{
		if (m_pkPC)
		{
			return SHOP_SUBHEADER_GC_OK;
		}
	}
#endif

	if (pos >= m_itemVector.size())
	{
		sys_log(0, "Shop::Buy : invalid position %d : %s", pos, ch->GetName());
		return SHOP_SUBHEADER_GC_INVALID_POS;
	}

	GuestMapType::iterator it = m_map_guest.find(ch);
	if (it == m_map_guest.end()) {
		return SHOP_SUBHEADER_GC_END;
	}

	SHOP_ITEM& r_item = m_itemVector[pos];
	if (!ismultiple) {
		LPITEM pkSelectedItem = ITEM_MANAGER::instance().Find(r_item.itemid);

		if (IsPCShop()) {
			if (!pkSelectedItem) {
				sys_log(0, "Shop::Buy : Critical: This user seems to be a hacker : invalid pcshop item : BuyerPID:%d SellerPID:%d", ch->GetPlayerID(), m_pkPC->GetPlayerID());
				return SHOP_SUBHEADER_GC_SOLD_OUT;
			} else if ((pkSelectedItem->GetOwner() != m_pkPC)) {
				sys_log(0, "Shop::Buy : Critical: This user seems to be a hacker : invalid pcshop item : BuyerPID:%d SellerPID:%d", ch->GetPlayerID(), m_pkPC->GetPlayerID());
				return SHOP_SUBHEADER_GC_SOLD_OUT;
			}
		}
	}

#ifdef ENABLE_LONG_LONG
	long long dwPrice = r_item.price;
	if (ch->GetGold() < (long long)dwPrice)
#else
	DWORD dwPrice = r_item.price;
	if (ch->GetGold() < (int)dwPrice)
#endif
	{
		return SHOP_SUBHEADER_GC_NOT_ENOUGH_MONEY;
	}

#ifdef ENABLE_BUY_WITH_ITEM
	DWORD dwPriceVnum = 0, dwPriceCount = 0, dwHaveCount = 0;
	for (int i = 0; i < MAX_SHOP_PRICES; i++) {
		dwPriceVnum = r_item.itemprice[i].vnum;
		if (dwPriceVnum > 0) {
			dwPriceCount = r_item.itemprice[i].count;
			dwHaveCount = ch->CountSpecifyItem(dwPriceVnum);
			if (dwHaveCount < dwPriceCount) {
				sys_log(1, "Shop::Buy : Not enough item : %s has %d, price %d.", ch->GetName(), dwHaveCount, dwPriceCount);
				return SHOP_SUBHEADER_GC_NOT_ENOUGH_ITEM;
			}
		}
	}
#endif

	LPITEM item = m_pkPC ? r_item.pkItem : ITEM_MANAGER::instance().CreateItem(r_item.vnum, r_item.count, 0, true);
	if (!item) {
		return SHOP_SUBHEADER_GC_SOLD_OUT;
	}

#ifdef ENABLE_SHOP_BLACKLIST
	if (!m_pkPC)
	{
		if (quest::CQuestManager::instance().GetEventFlag("hivalue_item_sell") == 0)
		{
			//�ູ�� ���� && ������ö �̺�Ʈ
			if (item->GetVnum() == 70024 || item->GetVnum() == 70035)
			{
				return SHOP_SUBHEADER_GC_END;
			}
		}
	}
#endif

	int iEmptyPos;
	if (item->IsDragonSoul())
	{
		iEmptyPos = ch->GetEmptyDragonSoulInventory(item);
	}
#ifdef ENABLE_EXTRA_INVENTORY
	else if (item->IsExtraItem())
	{
		iEmptyPos = ch->GetEmptyExtraInventory(item);
	}
#endif
	else
	{
		iEmptyPos = ch->GetEmptyInventory(item->GetSize());
	}

	if (iEmptyPos < 0)
	{
		if (m_pkPC)
		{
			sys_log(1, "Shop::Buy at PC Shop : Inventory full : %s size %d", ch->GetName(), item->GetSize());
			return SHOP_SUBHEADER_GC_INVENTORY_FULL;
		}
		else
		{
			sys_log(1, "Shop::Buy : Inventory full : %s size %d", ch->GetName(), item->GetSize());
			M2_DESTROY_ITEM(item);
			return SHOP_SUBHEADER_GC_INVENTORY_FULL;
		}
	}

	if (dwPrice > 0) {
		ch->PointChange(POINT_GOLD, -dwPrice, false);
	}

#ifdef ENABLE_BUY_WITH_ITEM
	for (int i = 0; i < MAX_SHOP_PRICES; i++) {
		dwPriceVnum = r_item.itemprice[i].vnum;
		if (dwPriceVnum > 0) {
			dwPriceCount = r_item.itemprice[i].count;
			if (dwPriceCount > 0) {
				ch->RemoveSpecifyItem(dwPriceVnum, r_item.itemprice[i].count);
			}
		}
	}
#endif

	if (!ismultiple) {
		DWORD dwTax = 0;
		int iVal = 0;

		{
			iVal = quest::CQuestManager::instance().GetEventFlag("personal_shop");

			if (0 < iVal)
			{
				if (iVal > 100)
					iVal = 100;

				dwTax = dwPrice * iVal / 100;
				dwPrice = dwPrice - dwTax;
			}
			else
			{
				iVal = 0;
				dwTax = 0;
			}
		}
	}

	// ���� �ý��� : ���� ¡��
	if (m_pkPC)
	{
#ifdef ENABLE_EXTRA_INVENTORY
		if (item->IsExtraItem()) {
			m_pkPC->SyncQuickslot(QUICKSLOT_TYPE_ITEM_EXTRA, item->GetCell(), 255);
		} else {
			m_pkPC->SyncQuickslot(QUICKSLOT_TYPE_ITEM, item->GetCell(), 255);
		}
#else
		m_pkPC->SyncQuickslot(QUICKSLOT_TYPE_ITEM, item->GetCell(), 255);
#endif

		if (item->GetVnum() == 90008 || item->GetVnum() == 90009) // VCARD
		{
			VCardUse(m_pkPC, ch, item);
			item = NULL;
		}
		else
		{
			char buf[512];

			if (item->GetVnum() >= 80003 && item->GetVnum() <= 80007)
			{
				snprintf(buf, sizeof(buf), "%s FROM: %u TO: %u PRICE: %lld", item->GetName(), ch->GetPlayerID(), m_pkPC->GetPlayerID(), dwPrice);
				LogManager::instance().GoldBarLog(ch->GetPlayerID(), item->GetID(), SHOP_BUY, buf);
				LogManager::instance().GoldBarLog(m_pkPC->GetPlayerID(), item->GetID(), SHOP_SELL, buf);
			}

			item->RemoveFromCharacter();
			
			if (item->IsDragonSoul()) {
				item->AddToCharacter(ch, TItemPos(DRAGON_SOUL_INVENTORY, iEmptyPos));
			}
#ifdef ENABLE_EXTRA_INVENTORY
			else if (item->IsExtraItem()) {
#ifdef ENABLE_25082021
				if (item->IsStackable() && !IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_STACK)) {
#ifdef ENABLE_NEW_STACK_LIMIT
					int 
#else
					BYTE 
#endif
					bCount = item->GetCount();
					for (int i = 0; i < EXTRA_INVENTORY_MAX_NUM; ++i) {
						LPITEM item2 = ch->GetExtraInventoryItem(i);
						if (!item2)
							continue;

						if (item2->GetVnum() == item->GetVnum()) {
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
							bCount2 = MIN(g_bItemCountLimit - item2->GetCount(), bCount);
							bCount -= bCount2;

							item2->SetCount(item2->GetCount() + bCount2);
							if (bCount == 0) {
								M2_DESTROY_ITEM(item);
								item = NULL;
								break;
							}
						}
					}

					if (item != NULL) {
						item->SetCount(bCount);
						item->AddToCharacter(ch, TItemPos(EXTRA_INVENTORY, iEmptyPos));
					}
				} else {
					item->AddToCharacter(ch, TItemPos(EXTRA_INVENTORY, iEmptyPos));
				}
#else
				item->AddToCharacter(ch, TItemPos(EXTRA_INVENTORY, iEmptyPos));
#endif
			}
#endif
			else {
#ifdef ENABLE_25082021
				if (item->IsStackable() && !IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_STACK)) {
#ifdef ENABLE_NEW_STACK_LIMIT
					int 
#else
					BYTE 
#endif
					bCount = item->GetCount();
					for (int i = 0; i < INVENTORY_MAX_NUM; ++i) {
						LPITEM item2 = ch->GetInventoryItem(i);
						if (!item2)
							continue;

						if (item2->GetVnum() == item->GetVnum()) {
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
							bCount2 = MIN(g_bItemCountLimit - item2->GetCount(), bCount);
							bCount -= bCount2;

							item2->SetCount(item2->GetCount() + bCount2);
							if (bCount == 0) {
								M2_DESTROY_ITEM(item);
								item = NULL;
								break;
							}
						}
					}

					if (item != NULL) {
						item->SetCount(bCount);
						item->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyPos));
					}
				} else {
					item->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyPos));
				}
#else
				item->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyPos));
#endif
			}

			if (item != NULL) {
				ITEM_MANAGER::instance().FlushDelayedSave(item);
			}
		}

		r_item.pkItem = NULL;
		BroadcastUpdateItem(pos);

		m_pkPC->PointChange(POINT_GOLD, dwPrice, false);
	}
	else
	{
		if (item->IsDragonSoul()) {
			item->AddToCharacter(ch, TItemPos(DRAGON_SOUL_INVENTORY, iEmptyPos));
		}
#ifdef ENABLE_EXTRA_INVENTORY
		else if (item->IsExtraItem()) {
#ifdef ENABLE_25082021
			if (item->IsStackable() && !IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_STACK)) {
#ifdef ENABLE_NEW_STACK_LIMIT
				int 
#else
				BYTE 
#endif
				bCount = item->GetCount();
				for (int i = 0; i < EXTRA_INVENTORY_MAX_NUM; ++i) {
					LPITEM item2 = ch->GetExtraInventoryItem(i);
					if (!item2)
						continue;

					if (item2->GetVnum() == item->GetVnum()) {
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
						bCount2 = MIN(g_bItemCountLimit - item2->GetCount(), bCount);
						bCount -= bCount2;

						item2->SetCount(item2->GetCount() + bCount2);
						if (bCount == 0) {
							M2_DESTROY_ITEM(item);
							item = NULL;
							break;
						}
					}
				}

				if (item != NULL) {
					item->SetCount(bCount);
					item->AddToCharacter(ch, TItemPos(EXTRA_INVENTORY, iEmptyPos));
				}
			} else {
				item->AddToCharacter(ch, TItemPos(EXTRA_INVENTORY, iEmptyPos));
			}
#else
			item->AddToCharacter(ch, TItemPos(EXTRA_INVENTORY, iEmptyPos));
#endif
		}
#endif
		else {
#ifdef ENABLE_25082021
			if (item->IsStackable() && !IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_STACK)) {
#ifdef ENABLE_NEW_STACK_LIMIT
				int 
#else
				BYTE 
#endif
				bCount = item->GetCount();
				for (int i = 0; i < INVENTORY_MAX_NUM; ++i) {
					LPITEM item2 = ch->GetInventoryItem(i);
					if (!item2)
						continue;

					if (item2->GetVnum() == item->GetVnum()) {
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
						bCount2 = MIN(g_bItemCountLimit - item2->GetCount(), bCount);
						bCount -= bCount2;

						item2->SetCount(item2->GetCount() + bCount2);
						if (bCount == 0) {
							M2_DESTROY_ITEM(item);
							item = NULL;
							break;
						}
					}
				}

				if (item != NULL) {
					item->SetCount(bCount);
					item->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyPos));
				}
			} else {
				item->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyPos));
			}
#else
			item->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyPos));
#endif
		}

		if (item != NULL) {
			ITEM_MANAGER::instance().FlushDelayedSave(item);
		}
	}

#ifdef ENABLE_BATTLE_PASS
	{
		BYTE bBattlePassId = ch->GetBattlePassId();
		if(bBattlePassId)
		{
			DWORD dwYangCount, dwNotUsed;
			if(CBattlePass::instance().BattlePassMissionGetInfo(bBattlePassId, SPENT_YANG, &dwNotUsed, &dwYangCount))
			{
				if(ch->GetMissionProgress(SPENT_YANG, bBattlePassId) < dwYangCount)
					ch->UpdateMissionProgress(SPENT_YANG, bBattlePassId, dwPrice, dwYangCount);
			}
		}
	}
#endif

	if (!ismultiple) {
#ifdef ENABLE_LIMIT_BUY_SPEED
		ch->SetLastBuyTime();
#endif
		ch->Save();
	}

	return (SHOP_SUBHEADER_GC_OK);
}

#ifdef ENABLE_BUY_STACK_FROM_SHOP
uint8_t CShop::MultipleBuy(LPCHARACTER ch, uint8_t p, uint8_t c) {
	if (p < 0 || c <= 0 || c > MULTIPLE_BUY_LIMIT) {
		return SHOP_SUBHEADER_GC_OK;
	}

#ifdef ENABLE_RESTRICT_GM_PERMISSIONS
	if (ch->GetGMLevel() > GM_PLAYER && ch->GetGMLevel() < GM_IMPLEMENTOR) {
		return SHOP_SUBHEADER_GC_OK;
	}
#endif

#ifdef ENABLE_LIMIT_BUY_SPEED
	int32_t iPulse = thecore_pulse() - ch->GetLastBuyTime();
	if (iPulse < PASSES_PER_SEC(1)) {
		return SHOP_SUBHEADER_GC_OK;
	}
#endif

	if (IsPCShop()) {
		return SHOP_SUBHEADER_GC_OK;
	}

	if (p >= m_itemVector.size()) {
		sys_log(0, "Shop::MultipleBuy: invalid position %d : %s", p, ch->GetName());
		return SHOP_SUBHEADER_GC_INVALID_POS;
	}

	GuestMapType::iterator it = m_map_guest.find(ch);
	if (it == m_map_guest.end()) {
		return SHOP_SUBHEADER_GC_END;
	}

	SHOP_ITEM& r_item = m_itemVector[p];
#ifdef ENABLE_LONG_LONG
	int64_t price = r_item.price * c;
#else
	int32_t price = r_item.price * c;
#endif
	if (ch->GetGold() < price) {
		return SHOP_SUBHEADER_GC_NOT_ENOUGH_MONEY;
	}

#ifdef ENABLE_BUY_WITH_ITEM
	int32_t price_vnum = 0, price_count = 0, have_count = 0;
	for (int32_t i = 0; i < MAX_SHOP_PRICES; i++) {
		price_vnum = r_item.itemprice[i].vnum;
		if (price_vnum > 0) {
			price_count = r_item.itemprice[i].count * c;
			have_count = ch->CountSpecifyItem(price_vnum);
			if (have_count < price_count) {
				sys_log(1, "Shop::MultipleBuy: Not enough item : %s has %d, price %d.", ch->GetName(), have_count, price_count);
				return SHOP_SUBHEADER_GC_NOT_ENOUGH_ITEM;
			}
		}
	}
#endif

#ifdef ENABLE_LONG_LONG
	int32_t r;
#else
	int64_t r;
#endif
	while (c > 0) {
		r = Buy(ch, p, true);
		if (r == SHOP_SUBHEADER_GC_NOT_ENOUGH_MONEY || 
#ifdef ENABLE_BUY_WITH_ITEM
			r == SHOP_SUBHEADER_GC_NOT_ENOUGH_ITEM || 
#endif
			r == SHOP_SUBHEADER_GC_INVENTORY_FULL || 
			r == SHOP_SUBHEADER_GC_END) {
			break;
		} else {
			c--;
		}
	}

#ifdef ENABLE_LIMIT_BUY_SPEED
	ch->SetLastBuyTime();
#endif
	ch->Save();
	return c <= 0 ? SHOP_SUBHEADER_GC_OK : r;
}
#endif

bool CShop::AddGuest(LPCHARACTER ch, DWORD owner_vid, bool bOtherEmpire)
{
	if (!ch)
		return false;

	if (ch->GetExchange())
		return false;

	if (ch->GetShop())
		return false;

	ch->SetShop(this);

	m_map_guest.insert(GuestMapType::value_type(ch, bOtherEmpire));

	TPacketGCShop pack;

	pack.header		= HEADER_GC_SHOP;
	pack.subheader	= SHOP_SUBHEADER_GC_START;

	TPacketGCShopStart pack2;

	memset(&pack2, 0, sizeof(pack2));
	pack2.owner_vid = owner_vid;

	for (DWORD i = 0; i < m_itemVector.size() && i < SHOP_HOST_ITEM_MAX_NUM; ++i)
	{
		const SHOP_ITEM & item = m_itemVector[i];

#ifdef ENABLE_SHOP_BLACKLIST
		//HIVALUE_ITEM_EVENT
		if (quest::CQuestManager::instance().GetEventFlag("hivalue_item_sell") == 0)
		{
			//�ູ�� ���� && ������ö �̺�Ʈ
			if (item.vnum == 70024 || item.vnum == 70035)
			{
				continue;
			}
		}
#endif
		//END_HIVALUE_ITEM_EVENT
		if (m_pkPC && !item.pkItem)
			continue;

		pack2.items[i].vnum = item.vnum;

		// REMOVED_EMPIRE_PRICE_LIFT
#ifdef ENABLE_NEWSTUFF
		if (bOtherEmpire && !g_bEmpireShopPriceTripleDisable) // no empire price penalty for pc shop
#else
		if (bOtherEmpire) // no empire price penalty for pc shop
#endif
		{
			pack2.items[i].price = item.price * 3;
		}
		else
			pack2.items[i].price = item.price;
		
#ifdef ENABLE_BUY_WITH_ITEM
		for (int j = 0; j < MAX_SHOP_PRICES; j++) {
			pack2.items[i].itemprice[j].vnum = item.itemprice[j].vnum;
			pack2.items[i].itemprice[j].count = item.itemprice[j].count;
		}
#endif
		// END_REMOVED_EMPIRE_PRICE_LIFT

		pack2.items[i].count = item.count;

		if (item.pkItem)
		{
			thecore_memcpy(pack2.items[i].alSockets, item.pkItem->GetSockets(), sizeof(pack2.items[i].alSockets));
			thecore_memcpy(pack2.items[i].aAttr, item.pkItem->GetAttributes(), sizeof(pack2.items[i].aAttr));
#ifdef ATTR_LOCK
			pack2.items[i].lockedattr = item.pkItem->GetLockedAttr();
#endif
		}
	}

	pack.size = sizeof(pack) + sizeof(pack2);

	ch->GetDesc()->BufferedPacket(&pack, sizeof(TPacketGCShop));
	ch->GetDesc()->Packet(&pack2, sizeof(TPacketGCShopStart));
	return true;
}

void CShop::RemoveGuest(LPCHARACTER ch)
{
	if (ch->GetShop() != this)
		return;

	m_map_guest.erase(ch);
	ch->SetShop(NULL);

	TPacketGCShop pack;

	pack.header		= HEADER_GC_SHOP;
	pack.subheader	= SHOP_SUBHEADER_GC_END;
	pack.size		= sizeof(TPacketGCShop);

	ch->GetDesc()->Packet(&pack, sizeof(pack));
}

void CShop::Broadcast(const void * data, int bytes)
{
	sys_log(1, "Shop::Broadcast %p %d", data, bytes);

	GuestMapType::iterator it;

	it = m_map_guest.begin();

	while (it != m_map_guest.end())
	{
		LPCHARACTER ch = it->first;

		if (ch->GetDesc())
			ch->GetDesc()->Packet(data, bytes);

		++it;
	}
}

void CShop::BroadcastUpdateItem(BYTE pos)
{
	TPacketGCShop pack;
	TPacketGCShopUpdateItem pack2;

	TEMP_BUFFER	buf;

	pack.header		= HEADER_GC_SHOP;
	pack.subheader	= SHOP_SUBHEADER_GC_UPDATE_ITEM;
	pack.size		= sizeof(pack) + sizeof(pack2);

	pack2.pos		= pos;

	if (m_pkPC && !m_itemVector[pos].pkItem)
		pack2.item.vnum = 0;
	else
	{
		pack2.item.vnum	= m_itemVector[pos].vnum;
		if (m_itemVector[pos].pkItem)
		{
			thecore_memcpy(pack2.item.alSockets, m_itemVector[pos].pkItem->GetSockets(), sizeof(pack2.item.alSockets));
			thecore_memcpy(pack2.item.aAttr, m_itemVector[pos].pkItem->GetAttributes(), sizeof(pack2.item.aAttr));
		}
		else
		{
			memset(pack2.item.alSockets, 0, sizeof(pack2.item.alSockets));
			memset(pack2.item.aAttr, 0, sizeof(pack2.item.aAttr));
		}
	}

	pack2.item.price	= m_itemVector[pos].price;
	pack2.item.count	= m_itemVector[pos].count;
#ifdef ENABLE_BUY_WITH_ITEM
	for (int i = 0; i < MAX_SHOP_PRICES; i++) {
		pack2.item.itemprice[i].vnum = m_itemVector[pos].itemprice[i].vnum;
		pack2.item.itemprice[i].count = m_itemVector[pos].itemprice[i].count;
	}
#endif
	
	buf.write(&pack, sizeof(pack));
	buf.write(&pack2, sizeof(pack2));
	Broadcast(buf.read_peek(), buf.size());
}

int CShop::GetNumberByVnum(DWORD dwVnum)
{
	int itemNumber = 0;

	for (DWORD i = 0; i < m_itemVector.size() && i < SHOP_HOST_ITEM_MAX_NUM; ++i)
	{
		const SHOP_ITEM & item = m_itemVector[i];

		if (item.vnum == dwVnum)
		{
			itemNumber += item.count;
		}
	}

	return itemNumber;
}

bool CShop::IsSellingItem(DWORD itemID)
{
	bool isSelling = false;

	for (DWORD i = 0; i < m_itemVector.size() && i < SHOP_HOST_ITEM_MAX_NUM; ++i)
	{
		if ((unsigned int)(m_itemVector[i].itemid) == itemID)
		{
			isSelling = true;
			break;
		}
	}

	return isSelling;

}

#ifdef REGEN_ANDRA
void CShop::RemoveAllGuests()
{
	if (m_map_guest.empty())
		return;
	for (GuestMapType::iterator it = m_map_guest.begin(); it != m_map_guest.end(); it++)
	{
		LPCHARACTER ch = it->first;
		if (ch)
		{
			if (ch->GetDesc() && ch->GetShop() == this)
			{
				ch->SetShop(NULL);

				TPacketGCShop pack;

				pack.header = HEADER_GC_SHOP;
				pack.subheader = SHOP_SUBHEADER_GC_END;
				pack.size = sizeof(TPacketGCShop);

				ch->GetDesc()->Packet(&pack, sizeof(pack));
			}
		}
	}
	m_map_guest.clear();
}
#endif
