#pragma once

#include "Packet.h"

/*
 *	���� ó��
 *
 *	2003-01-16 anoa	���� �Ϸ�
 *	2003-12-26 levites ����
 *
 *	2012-10-29 rtsummit ���ο� ȭ�� ���� �� tab ��� �߰��� ���� shop Ȯ��.
 *
 */
typedef enum
{
	SHOP_COIN_TYPE_GOLD, // DEFAULT VALUE
	SHOP_COIN_TYPE_SECONDARY_COIN,
} EShopCoinType;

class CPythonShop : public CSingleton<CPythonShop>
{
	public:
		CPythonShop(void);
		virtual ~CPythonShop(void);

		void Clear();

		void SetItemData(DWORD dwIndex, const TShopItemData & c_rShopItemData);
		BOOL GetItemData(DWORD dwIndex, const TShopItemData ** c_ppItemData);

		void SetItemData(BYTE tabIdx, DWORD dwSlotPos, const TShopItemData & c_rShopItemData);
		BOOL GetItemData(BYTE tabIdx, DWORD dwSlotPos, const TShopItemData ** c_ppItemData);

		void SetTabCount(BYTE bTabCount) { m_bTabCount = bTabCount; }
		BYTE GetTabCount() { return m_bTabCount; }

		void SetTabCoinType(BYTE tabIdx, BYTE coinType);
		BYTE GetTabCoinType(BYTE tabIdx);

		void SetTabName(BYTE tabIdx, const char* name);
		const char* GetTabName(BYTE tabIdx);


		//BOOL GetSlotItemID(DWORD dwSlotPos, DWORD* pdwItemID);

		void Open(BOOL isPrivateShop, BOOL isMainPrivateShop);
		void Close();
		BOOL IsOpen();
		BOOL IsPrivateShop();
		BOOL IsMainPlayerPrivateShop();

		void ClearPrivateShopStock();
#ifdef ENABLE_LONG_LONG
		void AddPrivateShopItemStock(TItemPos ItemPos, BYTE byDisplayPos, long long dwPrice);
		long long GetPrivateShopItemPrice(TItemPos ItemPos);
#else
		void AddPrivateShopItemStock(TItemPos ItemPos, BYTE byDisplayPos, DWORD dwPrice);
		int GetPrivateShopItemPrice(TItemPos ItemPos);
#endif
		void DelPrivateShopItemStock(TItemPos ItemPos);
#ifdef ENABLE_OFFLINE_SHOP
		void BuildPrivateShop(const char* c_szName, bool isOfflineShopMode);
		void SetGoldAmount(uGoldType gold) { m_gold = gold; }
		uGoldType GetGoldAmount() { return m_gold; }
		void SetOwner(bool isOwner) { m_bIsOwner = isOwner; }
		bool IsOwner() { return m_bIsOwner; };
		void SetLock(bool lock) { m_bLocked = lock; }
		bool IsLocked() { return m_bLocked; }
#else
		void BuildPrivateShop(const char* c_szName);
#endif
//#ifdef KASMIR_PAKET_SYSTEM
//		, DWORD dwKasmirNpc, BYTE bKasmirBaslik
//#endif


	protected:
		BOOL	CheckSlotIndex(DWORD dwIndex);

	protected:
		BOOL				m_isShoping;
		BOOL				m_isPrivateShop;
		BOOL				m_isMainPlayerPrivateShop;

		struct ShopTab
		{
			ShopTab()
			{
				coinType = SHOP_COIN_TYPE_GOLD;
			}
			BYTE				coinType;
			std::string			name;
			TShopItemData		items[SHOP_HOST_ITEM_MAX_NUM];
		};

		BYTE m_bTabCount;
		ShopTab m_aShoptabs[SHOP_TAB_COUNT_MAX];

		typedef std::map<TItemPos, TShopItemTable> TPrivateShopItemStock;
		TPrivateShopItemStock	m_PrivateShopItemStock;
#ifdef ENABLE_OFFLINE_SHOP
		uGoldType				m_gold;
		bool					m_bIsOwner;
		bool					m_bLocked;
#endif
};