#ifndef __INC_METIN_II_GAME_ITEM_H__
#define __INC_METIN_II_GAME_ITEM_H__

#include "entity.h"

class CItem : public CEntity
{
	protected:
		// override methods from ENTITY class
		virtual void	EncodeInsertPacket(LPENTITY entity);
		virtual void	EncodeRemovePacket(LPENTITY entity);

	public:
		CItem(DWORD dwVnum);
		virtual ~CItem();

#ifdef ENABLE_COSTUME_TIME_EXTENDER
		bool		IsCostumeTimeExtender();
#endif

		int			GetLevelLimit();

		bool		CheckItemUseLevel(int nLevel);

		bool		IsPCBangItem();

		long		FindApplyValue(BYTE bApplyType);

		bool		IsStackable()		{ return (GetFlag() & ITEM_FLAG_STACKABLE)?true:false; }

		void		Initialize();
		void		Destroy();

		void		Save();

		void		SetWindow(BYTE b)	{ m_bWindow = b; }
		BYTE		GetWindow()		{ return m_bWindow; }

		void		SetID(DWORD id)		{ m_dwID = id;	}
		DWORD		GetID()			{ return m_dwID; }

		void			SetProto(const TItemTable * table);
		TItemTable const *	GetProto()	{ return m_pProto; }

#ifdef ENABLE_ITEM_EXTRA_PROTO
		void		SetExtraProto(TItemExtraProto* Proto);
		TItemExtraProto* GetExtraProto();
		bool		HasExtraProto() { return m_ExtraProto != nullptr; }
#endif

#ifdef ATTR_LOCK
		short	GetLockedAttr() const	{return m_sLockedAttr;}
		void	SetLockedAttr(short sIndex);
		void	AddLockedAttr();
		void	ChangeLockedAttr();
		void	RemoveLockedAttr();
		bool	CheckHumanApply();
#endif
#ifdef ENABLE_LONG_LONG
		long long		GetGold();
		long long		GetShopBuyPrice();
#else
		int		GetGold();
		int		GetShopBuyPrice();
#endif

#ifdef ENABLE_MULTI_NAMES
		const char *	GetName(BYTE Lang=0);
#else
		const char *	GetName()		{ return m_pProto ? m_pProto->szLocaleName : NULL; }
#endif

		const char *	GetBaseName()		{ return m_pProto ? m_pProto->szName : NULL; }
		BYTE		GetSize()		{ return m_pProto ? m_pProto->bSize : 0;	}

		void		SetFlag(long flag)	{ m_lFlag = flag;	}
		long		GetFlag()		{ return m_lFlag;	}

		void		AddFlag(long bit);
		void		RemoveFlag(long bit);

		DWORD		GetWearFlag()		{ return m_pProto ? m_pProto->dwWearFlags : 0; }
		DWORD		GetAntiFlag()		{ return m_pProto ? m_pProto->dwAntiFlags : 0; }
		DWORD		GetImmuneFlag()		{ return m_pProto ? m_pProto->dwImmuneFlag : 0; }

		void		SetVID(DWORD vid)	{ m_dwVID = vid;	}
		DWORD		GetVID()		{ return m_dwVID;	}

		bool		SetCount(int count);
		int			GetCount();

		// GetVnum�� GetOriginalVnum�� ���� comment
		// GetVnum�� Masking �� Vnum�̴�. �̸� ��������ν�, �������� ���� Vnum�� 10������, Vnum�� 20�� ��ó�� ������ �� �ִ� ���̴�.
		// Masking ���� ori_to_new.txt���� ���ǵ� ���̴�.
		// GetOriginalVnum�� ������ ������ Vnum����, �α� ���� ��, Ŭ���̾�Ʈ�� ������ ���� ���� ��, ������ ���� �� Vnum�� ����Ͽ��� �Ѵ�.
		//
		DWORD		GetVnum() const		{ return m_dwMaskVnum ? m_dwMaskVnum : m_dwVnum;	}
		DWORD		GetOriginalVnum() const		{ return m_dwVnum;	}
		BYTE		GetType() const		{ return m_pProto ? m_pProto->bType : 0;	}
		BYTE		GetSubType() const	{ return m_pProto ? m_pProto->bSubType : 0;	}
		BYTE		GetLimitType(DWORD idx) const { return m_pProto ? m_pProto->aLimits[idx].bType : 0;	}
		long		GetLimitValue(DWORD idx) const { return m_pProto ? m_pProto->aLimits[idx].lValue : 0;	}
#ifdef ENABLE_NEW_USE_POTION
		BYTE	GetApplyType(DWORD idx) const { return m_pProto ? m_pProto->aApplies[idx].bType : 0;}
		long	GetApplyValue(DWORD idx) const { return m_pProto ? m_pProto->aApplies[idx].lValue : 0;}
#endif
		long		GetValue(DWORD idx);

		void		SetCell(LPCHARACTER ch, WORD pos)	{ m_pOwner = ch, m_wCell = pos;	}
		WORD		GetCell()				{ return m_wCell;	}

		LPITEM		RemoveFromCharacter();
#ifdef __HIGHLIGHT_SYSTEM__
		bool	AddToCharacter(LPCHARACTER ch, TItemPos Cell, bool isHighLight = true);
#else
		bool	AddToCharacter(LPCHARACTER ch, TItemPos Cell);
#endif
		LPCHARACTER	GetOwner()		{ return m_pOwner; }

		LPITEM		RemoveFromGround();
		bool		AddToGround(long lMapIndex, const PIXEL_POSITION & pos, bool skipOwnerCheck = false);

		int			FindEquipCell(LPCHARACTER ch, int bCandidateCell = -1);
		bool		IsEquipped() const		{ return m_bEquipped;	}
		bool		EquipTo(LPCHARACTER ch, BYTE bWearCell);
		bool		IsEquipable() const;

		bool		CanUsedBy(LPCHARACTER ch);

		bool		DistanceValid(LPCHARACTER ch);

		void		UpdatePacket();
		void		UsePacketEncode(LPCHARACTER ch, LPCHARACTER victim, struct packet_item_use * packet);

		void		SetExchanging(bool isOn = true);
		bool		IsExchanging()		{ return m_bExchanging;	}

		bool		IsTwohanded();

		bool		IsPolymorphItem();

		void		ModifyPoints(bool bAdd);	// �������� ȿ���� ĳ���Ϳ� �ο� �Ѵ�. bAdd�� false�̸� ������

		bool		CreateSocket(BYTE bSlot, BYTE bGold);
		const long *	GetSockets()		{ return &m_alSockets[0];	}
		long		GetSocket(int i)	{ return m_alSockets[i];	}

		void		SetSockets(const long * al);
		void		SetSocket(int i, long v, bool bLog = true);

		int		GetSocketCount();
		bool		AddSocket();

		const TPlayerItemAttribute* GetAttributes()		{ return m_aAttr;	}
		const TPlayerItemAttribute& GetAttribute(int i)	{ return m_aAttr[i];	}

		BYTE		GetAttributeType(int i)	{ return m_aAttr[i].bType;	}
		short		GetAttributeValue(int i){ return m_aAttr[i].sValue;	}

		void		SetAttributes(const TPlayerItemAttribute* c_pAttribute);

		int		FindAttribute(BYTE bType);
		bool		RemoveAttributeAt(int index);
		bool		RemoveAttributeType(BYTE bType);

		bool		HasAttr(BYTE bApply);
		bool		HasRareAttr(BYTE bApply);

		void		SetDestroyEvent(LPEVENT pkEvent);
		void		StartDestroyEvent(int iSec=300);

		DWORD		GetRefinedVnum()	{ return m_pProto ? m_pProto->dwRefinedVnum : 0; }
		DWORD		GetRefineFromVnum();
		int		GetRefineLevel();

		void		SetSkipSave(bool b)	{ m_bSkipSave = b; }
		bool		GetSkipSave()		{ return m_bSkipSave; }

		bool		IsOwnership(LPCHARACTER ch);
		void		SetOwnership(LPCHARACTER ch, int iSec = 10);
		void		SetOwnershipEvent(LPEVENT pkEvent);

		DWORD		GetLastOwnerPID()	{ return m_dwLastOwnerPID; }
		

#ifdef ENABLE_BATTLE_PASS
		bool		HaveOwnership() { return (m_pkOwnershipEvent ? true : false); }
#endif

		int		GetAttributeSetIndex(); // �Ӽ� �ٴ°��� ������ �迭�� ��� �ε����� ����ϴ��� �����ش�.
		void		AlterToMagicItem();
		void		AlterToSocketItem(int iSocketCount);

		WORD		GetRefineSet()		{ return m_pProto ? m_pProto->wRefineSet : 0;	}

		void		StartUniqueExpireEvent();
		void		SetUniqueExpireEvent(LPEVENT pkEvent);

		void		StartTimerBasedOnWearExpireEvent();
		void		SetTimerBasedOnWearExpireEvent(LPEVENT pkEvent);

		void		StartRealTimeExpireEvent();
		bool		IsRealTimeItem();
		bool		IsRealTimeFirstUseItem();
		bool		IsUnlimitedTimeUnique();

		void		StopUniqueExpireEvent();
		void		StopTimerBasedOnWearExpireEvent();
		void		StopAccessorySocketExpireEvent();

		//			�ϴ� REAL_TIME�� TIMER_BASED_ON_WEAR �����ۿ� ���ؼ��� ����� ������.
		int			GetDuration();

		int		GetAttributeCount();
		void		ClearAttribute();
		void		ChangeAttribute(const int* aiChangeProb=NULL);
		void		AddAttribute();
		void		AddAttribute(BYTE bType, short sValue);

		void		ApplyAddon(int iAddonType);

		int		GetSpecialGroup() const;
		bool	IsSameSpecialGroup(const LPITEM item) const;

		// ACCESSORY_REFINE
		// �׼������� ������ ���� ������ �߰�
		bool		IsAccessoryForSocket();

		int		GetAccessorySocketGrade();
		int		GetAccessorySocketMaxGrade();
		int		GetAccessorySocketDownGradeTime();

		void		SetAccessorySocketGrade(int iGrade
#ifdef ENABLE_INFINITE_RAFINES
		, bool infinite = false
#endif
		);
		void		SetAccessorySocketMaxGrade(int iMaxGrade);
		void		SetAccessorySocketDownGradeTime(DWORD time);

		void		AccessorySocketDegrade();

		// �Ǽ��縮 �� �����ۿ� �۾����� Ÿ�̸� ���ư��°�( ����, �� )
		void		StartAccessorySocketExpireEvent();
		void		SetAccessorySocketExpireEvent(LPEVENT pkEvent);

		bool		CanPutInto(LPITEM item);
#ifdef ENABLE_INFINITE_RAFINES
		bool		CanPutInto2(LPITEM item);
#endif
		// END_OF_ACCESSORY_REFINE

		void		CopyAttributeTo(LPITEM pItem);
		void		CopySocketTo(LPITEM pItem);

		int			GetRareAttrCount();
		bool		AddRareAttribute();
		bool		ChangeRareAttribute();

		void		AttrLog();

		void		Lock(bool f) { m_isLocked = f; }
		bool		isLocked() const { return m_isLocked; }

	private :
		void		SetAttribute(int i, BYTE bType, short sValue);
	public:
		void		SetForceAttribute(int i, BYTE bType, short sValue);

	protected:
		bool		EquipEx(bool is_equip);
		bool		Unequip();

		void		AddAttr(BYTE bApply, BYTE bLevel);
		void		PutAttribute(const int * aiAttrPercentTable);
		void		PutAttributeWithLevel(BYTE bLevel);

	public:
		void		AddRareAttribute2(const int * aiAttrPercentTable = NULL);
	protected:
		void		AddRareAttr(BYTE bApply, BYTE bLevel);
		void		PutRareAttribute(const int * aiAttrPercentTable);
		void		PutRareAttributeWithLevel(BYTE bLevel);

	protected:
		friend class CInputDB;
		bool		OnAfterCreatedItem();			// ������ �������� ��� ������ �Բ� ������ ����(�ε�)�� �� �Ҹ���� �Լ�.

	public:
		bool		IsRideItem();
		bool		IsRamadanRing();

		void		ClearMountAttributeAndAffect();
		bool		IsNewMountItem();

#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
		bool		IsMountItem();
#endif

		// ���Ͽ��� ���� ĳ�� �����۰� ������, ��ȯ ������ ĳ�� �������� ����ٰ� �Ͽ�,
		// �������� �����ۿ�, ��ȯ ���� �÷��׸� ������ ���ο� �����۵��� ���ο� ������ �뿪�� �Ҵ��Ͽ���.
		// ������ ���ο� �����۵� �������� �����۰� ���� ȿ���� �����ϴµ�,
		// ������, Ŭ���, vnum ������� �Ǿ��־�
		// ���ο� vnum�� �˴� ������ ���� �� �ھƾ��ϴ� ��Ÿ��� ��Ȳ�� �´�Ҵ�.
		// �׷��� �� vnum�� �������̸�, �������� ���ư� ���� �������� ������ vnum���� �ٲ㼭 ���� �ϰ�,
		// ������ ���� ���� vnum���� �ٲ��ֵ��� �Ѵ�.

		// Mask vnum�� � ����(ex. ���� ��Ȳ)�� ���� vnum�� �ٲ�� ���ư��� �������� ���� �ִ�.
		void		SetMaskVnum(DWORD vnum)	{	m_dwMaskVnum = vnum; }
		DWORD		GetMaskVnum()			{	return m_dwMaskVnum; }
		bool		IsMaskedItem()	{	return m_dwMaskVnum != 0;	}

		// ��ȥ��
		bool		IsDragonSoul();
		int		GiveMoreTime_Per(float fPercent);
		int		GiveMoreTime_Fix(DWORD dwTime);
#ifdef ENABLE_SOUL_SYSTEM
	public:
		void		StartSoulItemEvent();
		void		SetSoulItemEvent(LPEVENT pkEvent);
#endif

	private:
		TItemTable const * m_pProto;		// ������ Ÿ��

		DWORD		m_dwVnum;
		LPCHARACTER	m_pOwner;

		BYTE		m_bWindow;		// ���� �������� ��ġ�� ������
		DWORD		m_dwID;			// ������ȣ
		bool		m_bEquipped;	// ���� �Ǿ��°�?
		DWORD		m_dwVID;		// VID
		WORD		m_wCell;		// ��ġ
		DWORD		m_dwCount;		// ����
#ifdef ATTR_LOCK
		DWORD		m_sLockedAttr;
#endif
#ifdef ENABLE_ITEM_EXTRA_PROTO
		TItemExtraProto* m_ExtraProto;
#endif
		long		m_lFlag;		// �߰� flag
		DWORD		m_dwLastOwnerPID;	// ������ ������ �־��� ����� PID

		bool		m_bExchanging;	///< ���� ��ȯ�� ����

		long		m_alSockets[ITEM_SOCKET_MAX_NUM];	// ������ ��Ĺ
		TPlayerItemAttribute	m_aAttr[ITEM_ATTRIBUTE_MAX_NUM];

		LPEVENT		m_pkDestroyEvent;
		LPEVENT		m_pkExpireEvent;
#ifdef ENABLE_SOUL_SYSTEM
		LPEVENT		m_pkSoulItemEvent;
#endif
		LPEVENT		m_pkUniqueExpireEvent;
		LPEVENT		m_pkTimerBasedOnWearExpireEvent;
		LPEVENT		m_pkRealTimeExpireEvent;
		LPEVENT		m_pkAccessorySocketExpireEvent;
		LPEVENT		m_pkOwnershipEvent;
		DWORD		m_dwOwnershipPID;

		bool		m_bSkipSave;

		bool		m_isLocked;

		DWORD		m_dwMaskVnum;
		DWORD		m_dwSIGVnum;
	public:
		void SetSIGVnum(DWORD dwSIG)
		{
			m_dwSIGVnum = dwSIG;
		}
		DWORD	GetSIGVnum() const
		{
			return m_dwSIGVnum;
		}
#ifdef ENABLE_EXTRA_INVENTORY
		bool	IsExtraItem();
		BYTE	GetExtraCategory();
#endif
#ifdef ENABLE_RUNE_SYSTEM
	public:
		bool	IsRune();
		long	GetRuneAttrType(int c);
		long	GetRuneAttrValue(int c, long lTime);
		void	InitializeRune();
		void	ChangeRuneAttr(long lTime);
		void	ActivateRuneBonus();
		void	DeactivateRuneBonus();
		void	DeactivateRuneBonusRefresh();
		void	ActivateRune();
		void	DeactivateRune();
#endif
};

EVENTINFO(item_event_info)
{
	LPITEM item;
	char szOwnerName[CHARACTER_NAME_MAX_LEN];

	item_event_info()
	: item( 0 )
	{
		::memset( szOwnerName, 0, CHARACTER_NAME_MAX_LEN );
	}
};

EVENTINFO(item_vid_event_info)
{
	DWORD item_vid;
#ifdef ENABLE_NEW_USE_POTION
	bool newpotion;
#endif

	item_vid_event_info()
	: item_vid( 0 )
#ifdef ENABLE_NEW_USE_POTION
	, newpotion(false)
#endif
	{
	}
};

#endif
