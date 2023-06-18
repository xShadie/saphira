#ifndef __INC_METIN_II_GAME_SHOP_H__
#define __INC_METIN_II_GAME_SHOP_H__

enum
{
	SHOP_MAX_DISTANCE = 1000
};

class CGrid;

/* ---------------------------------------------------------------------------------- */
class CShop
{
	public:
		typedef struct shop_item
		{
			DWORD			vnum;
#ifdef ENABLE_LONG_LONG
			long long		price;
#else
			long			price;
#endif
#ifdef ENABLE_NEW_STACK_LIMIT
			int				count;
#else
			BYTE			count;
#endif
			LPITEM			pkItem;
			int				itemid;
#ifdef ENABLE_BUY_WITH_ITEM
			TShopItemPrice	itemprice[MAX_SHOP_PRICES];
#endif
			shop_item()
			{
				vnum = 0;
				price = 0;
				count = 0;
				itemid = 0;
#ifdef ENABLE_BUY_WITH_ITEM
				memset(itemprice, 0, sizeof(itemprice));
#endif
				pkItem = NULL;
			}
		} SHOP_ITEM;

		CShop();
		virtual ~CShop(); // @fixme139 (+virtual)

		bool	Create(DWORD dwVnum, DWORD dwNPCVnum, TShopItemTable * pItemTable);
		void	SetShopItems(TShopItemTable * pItemTable, BYTE bItemCount);

		virtual void	SetPCShop(LPCHARACTER ch);
		virtual bool	IsPCShop()	{ return m_pkPC ? true : false; }

		// �Խ�Ʈ �߰�/����
		virtual bool	AddGuest(LPCHARACTER ch,DWORD owner_vid, bool bOtherEmpire);
		void	RemoveGuest(LPCHARACTER ch);
#ifdef REGEN_ANDRA
		void	RemoveAllGuests();
#endif

		// ���� ����
#ifdef ENABLE_LONG_LONG
		virtual long long	Buy(LPCHARACTER ch, BYTE pos
#ifdef ENABLE_BUY_STACK_FROM_SHOP
, bool multiple = false
#endif
);
#else
		virtual int	Buy(LPCHARACTER ch, BYTE pos
#ifdef ENABLE_BUY_STACK_FROM_SHOP
, bool multiple = false
#endif
);
#endif
#ifdef ENABLE_BUY_STACK_FROM_SHOP
		virtual uint8_t MultipleBuy(LPCHARACTER ch, uint8_t p, uint8_t c);
#endif
		// �Խ�Ʈ���� ��Ŷ�� ����
		void	BroadcastUpdateItem(BYTE pos);

		// �Ǹ����� �������� ������ �˷��ش�.
		int		GetNumberByVnum(DWORD dwVnum);

		// �������� ������ ��ϵǾ� �ִ��� �˷��ش�.
		virtual bool	IsSellingItem(DWORD itemID);

		DWORD	GetVnum() { return m_dwVnum; }
		DWORD	GetNPCVnum() { return m_dwNPCVnum; }

	protected:
		void	Broadcast(const void * data, int bytes);

	protected:
		DWORD				m_dwVnum;
		DWORD				m_dwNPCVnum;

		CGrid *				m_pGrid;

		typedef TR1_NS::unordered_map<LPCHARACTER, bool> GuestMapType;
		GuestMapType m_map_guest;
		std::vector<SHOP_ITEM>		m_itemVector;	// �� �������� ����ϴ� ���ǵ�

		LPCHARACTER			m_pkPC;
};

#endif
