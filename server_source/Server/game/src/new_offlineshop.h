#ifndef __INCLUDE_NEW_OFFLINESHOP_HEADER__
#define __INCLUDE_NEW_OFFLINESHOP_HEADER__

#ifdef __ENABLE_NEW_OFFLINESHOP__
#define __USE_PID_AS_GUESTLIST__

#ifdef __USE_PID_AS_GUESTLIST__
#define AS_LPGUEST(some)  (CHARACTER_MANAGER::instance().FindByPID(some)) 
#define AS_GUESTID(some)  (some->GetPlayerID())
#else
#define AS_LPGUEST(some) (some)
#define AS_GUESTID(some) (some)
#endif
#include "../../common/service.h"

template <class T>
void ZeroObject(T& obj){
	obj = {};
}

template <class T>
void CopyObject(T& objDest, const T& objSrc){
	memcpy(&objDest, &objSrc, sizeof(objDest));
}

template <class T>
void CopyContainer(T& objDest, const T& objSrc){
	objDest = objSrc;
}


//copyarray
template <class T, size_t size>
void CopyArray(T (&objDest)[size] , const T (&objSrc)[size]){
	if(size==0)
		return;
	memcpy(&objDest[0] , &objSrc[0], sizeof(T)*size);
}




template <class T>
void DeletePointersContainer(T& obj){
	typename T::iterator it = obj.begin();
	for(; it != obj.end(); it++)
		delete(*it);
}



namespace offlineshop
{
	//patch 08-03-2020
	inline offlineshop::ExpirationType GetItemExpiration(LPITEM item) {
		auto proto = item->GetProto();
#ifdef ENABLE_NEW_USE_POTION
		if (proto->bType == ITEM_USE && proto->bSubType == USE_NEW_POTIION) {
			return offlineshop::ExpirationType::EXPIRE_NONE;
		}
#endif

		for (const auto limit : proto->aLimits) {
			if (limit.bType == LIMIT_REAL_TIME)
				return offlineshop::ExpirationType::EXPIRE_REAL_TIME;
			else if (limit.bType == LIMIT_REAL_TIME_START_FIRST_USE && item->GetSocket(1) != 0)
				return offlineshop::ExpirationType::EXPIRE_REAL_TIME_FIRST_USE;
		} return offlineshop::ExpirationType::EXPIRE_NONE;
	}


	enum eOffshopChatPacket
	{
		CHAT_PACKET_CANNOT_CREATE_SHOP,
		CHAT_PACKET_CANNOT_CHANGE_NAME,
		CHAT_PACKET_CANNOT_FORCE_CLOSE,
		CHAT_PACKET_CANNOT_OPEN_SHOP,
		CHAT_PACKET_CANNOT_OPEN_SHOP_OWNER,

		CHAT_PACKET_CANNOT_ADD_ITEM,
		CHAT_PACKET_CANNOT_BUY_ITEM, //tofix wrong chat packet
		CHAT_PACKET_CANNOT_REMOVE_ITEM,
		CHAT_PACKET_CANNOT_EDIT_ITEM,
		CHAT_PACKET_CANNOT_REMOVE_LAST_ITEM,

		CHAT_PACKET_CANNOT_FILTER,
		CHAT_PACKET_CANNOT_SEARCH_YET,
		CHAT_PACKET_OFFER_CREATE,

		CHAT_PACKET_CANNOT_CREATE_OFFER,
		CHAT_PACKET_CANNOT_ACCEPT_OFFER,

		CHAT_PACKET_CANNOT_OPEN_SAFEBOX,
		CHAT_PACKET_CANNOT_SAFEBOX_GET_ITEM,
		CHAT_PACKET_CANNOT_SAFEBOX_GET_VALUTES,
		CHAT_PACKET_CANNOT_SAFEBOX_CLOSE,


		CHAT_PACKET_RECV_ITEM_SAFEBOX,

		//AUCTION
		CHAT_PACKET_AUCTION_CANNOT_SEND_LIST,
		CHAT_PACKET_AUCTION_CANNOT_OPEN_AUCTION,
		CHAT_PACKET_AUCTION_CANNOT_CREATE_AUCTION,
		CHAT_PACKET_AUCTION_CANNOT_ADD_OFFER,

		//GENERAL
		CHAT_PACKET_CANNOT_DO_NOW,
#ifdef KASMIR_PAKET_SYSTEM
		CHAT_PACKET_CANNOT_DO_STYLE,
#endif
	};

	inline void SendChatPacket(LPCHARACTER ch, BYTE type)
	{
#ifdef TEXTS_IMPROVEMENT
		if (!ch)
			return;

		switch (type)
		{
			case CHAT_PACKET_CANNOT_CREATE_SHOP:
				{
					ch->ChatPacketNew(CHAT_TYPE_INFO, 825, "");
				}
				break;
			case CHAT_PACKET_CANNOT_CHANGE_NAME:
				{
					ch->ChatPacketNew(CHAT_TYPE_INFO, 826, "");
				}
				break;
			case CHAT_PACKET_CANNOT_FORCE_CLOSE:
				{
					ch->ChatPacketNew(CHAT_TYPE_INFO, 827, "");
				}
				break;
			case CHAT_PACKET_CANNOT_OPEN_SHOP:
				{
					ch->ChatPacketNew(CHAT_TYPE_INFO, 828, "");
				}
				break;
			case CHAT_PACKET_CANNOT_OPEN_SHOP_OWNER:
				{
					ch->ChatPacketNew(CHAT_TYPE_INFO, 829, "");
				}
				break;
			case CHAT_PACKET_CANNOT_ADD_ITEM:
				{
					ch->ChatPacketNew(CHAT_TYPE_INFO, 830, "");
				}
				break;
			case CHAT_PACKET_CANNOT_BUY_ITEM:
				{
					ch->ChatPacketNew(CHAT_TYPE_INFO, 831, "");
				}
				break;
			case CHAT_PACKET_CANNOT_REMOVE_ITEM:
				{
					ch->ChatPacketNew(CHAT_TYPE_INFO, 832, "");
				}
				break;
			case CHAT_PACKET_CANNOT_EDIT_ITEM:
				{
					ch->ChatPacketNew(CHAT_TYPE_INFO, 833, "");
				}
				break;
			case CHAT_PACKET_CANNOT_REMOVE_LAST_ITEM:
				{
					ch->ChatPacketNew(CHAT_TYPE_INFO, 834, "");
				}
				break;
			case CHAT_PACKET_CANNOT_FILTER:
				{
					ch->ChatPacketNew(CHAT_TYPE_INFO, 835, "");
				}
				break;
			case CHAT_PACKET_CANNOT_SEARCH_YET:
				{
					ch->ChatPacketNew(CHAT_TYPE_INFO, 836, "");
				}
				break;
			case CHAT_PACKET_OFFER_CREATE:
				{
					ch->ChatPacketNew(CHAT_TYPE_INFO, 837, "");
				}
				break;
			case CHAT_PACKET_CANNOT_CREATE_OFFER:
				{
					ch->ChatPacketNew(CHAT_TYPE_INFO, 838, "");
				}
				break;
			case CHAT_PACKET_CANNOT_ACCEPT_OFFER:
				{
					ch->ChatPacketNew(CHAT_TYPE_INFO, 839, "");
				}
				break;
			case CHAT_PACKET_CANNOT_OPEN_SAFEBOX:
				{
					ch->ChatPacketNew(CHAT_TYPE_INFO, 840, "");
				}
				break;
			case CHAT_PACKET_CANNOT_SAFEBOX_GET_ITEM:
				{
					ch->ChatPacketNew(CHAT_TYPE_INFO, 841, "");
				}
				break;
			case CHAT_PACKET_CANNOT_SAFEBOX_GET_VALUTES:
				{
					ch->ChatPacketNew(CHAT_TYPE_INFO, 842, "");
				}
				break;
			case CHAT_PACKET_CANNOT_SAFEBOX_CLOSE:
				{
					ch->ChatPacketNew(CHAT_TYPE_INFO, 843, "");
				}
				break;
			case CHAT_PACKET_RECV_ITEM_SAFEBOX:
				{
					ch->ChatPacketNew(CHAT_TYPE_INFO, 844, "");
				}
				break;
			case CHAT_PACKET_AUCTION_CANNOT_SEND_LIST:
				{
					ch->ChatPacketNew(CHAT_TYPE_INFO, 845, "");
				}
				break;
			case CHAT_PACKET_AUCTION_CANNOT_OPEN_AUCTION:
				{
					ch->ChatPacketNew(CHAT_TYPE_INFO, 846, "");
				}
				break;
			case CHAT_PACKET_AUCTION_CANNOT_CREATE_AUCTION:
				{
					ch->ChatPacketNew(CHAT_TYPE_INFO, 847, "");
				}
				break;
			case CHAT_PACKET_AUCTION_CANNOT_ADD_OFFER:
				{
					ch->ChatPacketNew(CHAT_TYPE_INFO, 848, "");
				}
				break;
			case CHAT_PACKET_CANNOT_DO_NOW:
				{
					ch->ChatPacketNew(CHAT_TYPE_INFO, 849, "");
				}
				break;
		default:
			sys_err("unknown chat packet %d",type);
			break;
		}
#endif
	}

#pragma pack(1)
	class CShopItem
	{
	
	public:


		CShopItem(DWORD dwID);
		CShopItem(const CShopItem& rCopy);
		CShopItem(LPITEM pItem, const TPriceInfo& sPrice, BYTE byWindowType, DWORD dwID=0);

		~CShopItem();

		bool			GetTable(TItemTable** ppTable) const;
		TPriceInfo*		GetPrice() const;
		
		LPITEM			CreateItem() const;
		TItemInfoEx*	GetInfo() const;

		void			SetInfo(LPITEM pItem);
		void			SetInfo(const TItemInfoEx& info);
		void			SetPrice(const TPriceInfo& sPrice);

		void			SetWindow(BYTE byWin);
		BYTE			GetWindow() const;

		DWORD			GetID() const ;
		void			SetOwnerID(DWORD dwOwnerID);
		bool			CanBuy(LPCHARACTER ch);

		void operator = (const CShopItem& rItem);

	protected:
		TItemInfoEx		m_itemInfo;
		TPriceInfo		m_priceInfo;
		BYTE			m_byWindow;
		DWORD			m_dwID;
		DWORD			m_dwOwnerID;

	};
#pragma pack()




	class CShop
	{


	public:
		typedef std::vector<CShopItem>  VECSHOPITEM;
		typedef std::vector<TOfferInfo>	VECSHOPOFFER;
#ifdef __USE_PID_AS_GUESTLIST__
		typedef std::list<DWORD>		LISTGUEST;
#else
		typedef std::list<LPCHARACTER>	LISTGUEST;
#endif



	public:
		CShop();
		CShop(const CShop& rCopy);



		~CShop();

		//get const 
		VECSHOPITEM*		GetItems() const;
		VECSHOPITEM*		GetItemsSold() const;
		VECSHOPOFFER*		GetOffers() const;
		LISTGUEST*			GetGuests() const;

		//duration
		void				SetDuration(DWORD dwDuration);
		DWORD				DecreaseDuration();

		DWORD				GetDuration() const;

		//owner pid
		void				SetOwnerPID(DWORD dwOwnerPID);
		DWORD				GetOwnerPID() const;

		//guests
		bool				AddGuest(LPCHARACTER ch);
		bool				RemoveGuest(LPCHARACTER ch);

		//items
		void				SetItems(VECSHOPITEM* pVec);
		bool				AddItem(CShopItem& rItem);
		bool				AddItemSold(CShopItem& rItem);
		bool				RemoveItem(DWORD dwItemID);
		bool				ModifyItem(DWORD dwItemID, CShopItem& rItem);
		bool				BuyItem(DWORD dwItem);
		bool				GetItem(DWORD dwItem, CShopItem** ppItem);
		bool				GetItemSold(DWORD dwItem, CShopItem** ppItem);

		bool				AddOffer(const TOfferInfo* pOfferInfo);
		bool				AcceptOffer(const TOfferInfo* pOfferInfo);

		void				NotifyOffers(LPCHARACTER ch);
		void				NotifyAcceptedOffers(LPCHARACTER ch);
		LPCHARACTER			FindOwnerCharacter();
		void				Clear();


		const char*			GetName() const;
		void				SetName(const char* pcszName);


		void				RefreshToOwner();
#ifdef KASMIR_PAKET_SYSTEM
		void				SetRace(DWORD dwRace);
		DWORD				GetRace() const;
#endif
	private:
		void				__RefreshItems(LPCHARACTER ch=NULL);
		void				__SendOfferNotify(LPCHARACTER ch, TOfferInfo* pOffer);

	private:
		VECSHOPITEM			m_vecItems;
		VECSHOPITEM			m_vecItemSold;
		VECSHOPOFFER		m_vecOffers;
		LISTGUEST			m_listGuests;

		DWORD				m_dwPID;
		DWORD				m_dwDuration;
		std::string			m_stName;
#ifdef KASMIR_PAKET_SYSTEM
		DWORD				m_Race;
#endif
	};





	class CShopSafebox
	{
	public:
		typedef std::vector<CShopItem> VECITEM;

	public:
#pragma pack(1)
		struct SValuteAmount {
			long long illYang;
#ifdef __ENABLE_CHEQUE_SYSTEM__
			int iCheque;

#endif

			SValuteAmount() : illYang(0)
			{
			}

			SValuteAmount(const TValutesInfo& rCopy)
			{
				illYang = rCopy.illYang;
#ifdef __ENABLE_CHEQUE_SYSTEM__
				iCheque = rCopy.iCheque;
#endif
			}

			SValuteAmount(const TPriceInfo& rCopy)
			{
				illYang = rCopy.illYang;
#ifdef __ENABLE_CHEQUE_SYSTEM__
				iCheque = rCopy.iCheque;
#endif
			}

			long long GetTotalYangAmount() const {
				long long total = illYang;
#ifdef __ENABLE_CHEQUE_SYSTEM__
				total += YANG_PER_CHEQUE*iCheque;
#endif
				return total;
			}

			bool operator < (const SValuteAmount& rVal)
			{
				return GetTotalYangAmount() < rVal.GetTotalYangAmount();
			}

			void operator -= (const SValuteAmount& rVal)
			{
				illYang -= rVal.illYang;
#ifdef __ENABLE_CHEQUE_SYSTEM__
				iCheque -= rVal.iCheque;
#endif
			}

			void operator += (const SValuteAmount& rVal)
			{
				illYang += rVal.illYang;
#ifdef __ENABLE_CHEQUE_SYSTEM__
				iCheque += rVal.iCheque;
#endif
			}
		};
#pragma pack()


	public:
		CShopSafebox(LPCHARACTER chOwner);
		CShopSafebox();
		CShopSafebox(const CShopSafebox& rCopy);
		~CShopSafebox();

		void			SetOwner(LPCHARACTER ch);
		void			SetItems(VECITEM* pVec);
		void			SetValuteAmount(SValuteAmount val);

		bool			AddItem(CShopItem* pItem);
		bool			RemoveItem(DWORD dwItemID);

		void			AddValute(SValuteAmount val);
		bool			RemoveValute(SValuteAmount val);

		VECITEM*		GetItems();
		SValuteAmount	GetValutes();

		bool			GetItem(DWORD dwItemID, CShopItem** ppItem);
		LPCHARACTER		GetOwner();


		bool			RefreshToOwner(LPCHARACTER ch= nullptr);



	private:
		VECITEM			m_vecItems;
		LPCHARACTER		m_pkOwner;
		SValuteAmount	m_valutes;

	};





	class CAuction
	{
	public:
		typedef std::vector<TAuctionOfferInfo> AUCTION_OFFERVEC;
#ifdef __USE_PID_AS_GUESTLIST__
		typedef std::list<DWORD>		GUESTLIST;
#else
		typedef std::list<LPCHARACTER>	GUESTLIST;
#endif

	public:
		CAuction();
		~CAuction();


		void SetInfo(const TAuctionInfo& auction);
		void SetOffers(const std::vector<TAuctionOfferInfo>& vec);

		bool AddOffer(const TAuctionOfferInfo& offer);
		bool AddGuest(LPCHARACTER ch);
		bool RemoveGuest(LPCHARACTER ch);
		void DecreaseDuration();
		//adding 1 minute time when new offer is done in the last minute
		void IncreaseDuration();
		
		CShop::LISTGUEST&			GetGuests();
		const DWORD					GetBestBuyer() const;
		const TAuctionInfo&			GetInfo() const;
		const AUCTION_OFFERVEC&		GetOffers() const;
		const TPriceInfo&			GetBestOffer() const;

	private:
		void __RefreshToGuests();
		bool __SetBestOffer();

	private:
		TAuctionInfo		m_info;
		AUCTION_OFFERVEC	m_offersVec;
		TPriceInfo			m_bestOffer;
		DWORD				m_dwBestBuyer;
		GUESTLIST			m_guestsList;

	};



#ifdef ENABLE_NEW_SHOP_IN_CITIES
	class ShopEntity : public CEntity
	{
	public:
		ShopEntity();

		//overriding virtual CEntity methods
		void	EncodeInsertPacket(LPENTITY entity);
		void	EncodeRemovePacket(LPENTITY entity);

		
		const char*			GetShopName() const;
		void				SetShopName(const char* name);
#ifdef KASMIR_PAKET_SYSTEM
		void				SetShopRace(DWORD dwRace);
		DWORD				GetShopRace();
#endif
		int					GetShopType();
		void				SetShopType(int iType);
		void				SetShop(offlineshop::CShop* pOfflineShop);

		offlineshop::CShop* GetShop() {return m_pkShop;}

		static DWORD		AllocID(){
			static DWORD dwID=1;
			return dwID++;
		}

		DWORD				GetVID() {return m_dwVID;}
		void				Destroy() { CEntity::Destroy(); }
	private:
		char					m_szName[OFFLINE_SHOP_NAME_MAX_LEN];
#ifdef KASMIR_PAKET_SYSTEM
		DWORD					m_Race;
#endif
		int						m_iType;
		DWORD					m_dwVID;
		offlineshop::CShop*		m_pkShop;
	};

#endif







}

#endif //__ENABLE_NEW_OFFLINESHOP__

#endif //__include


