#ifndef __INC_METIN_II_LENGTH_H__
#define __INC_METIN_II_LENGTH_H__

#include "CommonDefines.h"
#include "service.h"

#define WORD_MAX 0xffff
enum EMisc
{
	ITEM_MAX_COUNT			= 2000,
	MAX_HOST_LENGTH			= 15,
	IP_ADDRESS_LENGTH		= 15,
	LOGIN_MAX_LEN			= 30,
	PASSWD_MAX_LEN			= 16,
#ifdef ENABLE_PLAYER_PER_ACCOUNT5
	PLAYER_PER_ACCOUNT		= 5,
#else
	PLAYER_PER_ACCOUNT		= 4,
#endif
	ACCOUNT_STATUS_MAX_LEN	= 8,
	CHARACTER_NAME_MAX_LEN	= 24,
	SHOP_SIGN_MAX_LEN		= 32,
	INVENTORY_PAGE_COLUMN	= 5, // 5 (default)
	INVENTORY_PAGE_ROW		= 9, // 9 (default)
	INVENTORY_PAGE_SIZE		= INVENTORY_PAGE_COLUMN*INVENTORY_PAGE_ROW,
#ifdef ENABLE_EXTEND_INVEN_SYSTEM
	INVENTORY_PAGE_COUNT	= 4, // 2 (default)
#else
	INVENTORY_PAGE_COUNT	= 2, // 2 (default)
#endif

	INVENTORY_MAX_NUM		= INVENTORY_PAGE_SIZE*INVENTORY_PAGE_COUNT, // 90 (default)
	

#ifdef ENABLE_EXTRA_INVENTORY
	EXTRA_INVENTORY_PAGE_COLUMN = 5,
	EXTRA_INVENTORY_PAGE_ROW = 9,
	EXTRA_INVENTORY_PAGE_SIZE = EXTRA_INVENTORY_PAGE_COLUMN * EXTRA_INVENTORY_PAGE_ROW,
	EXTRA_INVENTORY_PAGE_COUNT = 20,
	EXTRA_INVENTORY_MAX_NUM = EXTRA_INVENTORY_PAGE_SIZE * EXTRA_INVENTORY_PAGE_COUNT,
	EXTRA_INVENTORY_CATEGORY_MAX_NUM = EXTRA_INVENTORY_PAGE_SIZE * 4,
#endif
	
	
	ABILITY_MAX_NUM			= 50,
	EMPIRE_MAX_NUM			= 4,
	BANWORD_MAX_LEN			= 24,
	SMS_MAX_LEN				= 80,
	MOBILE_MAX_LEN			= 32,
	SOCIAL_ID_MAX_LEN		= 18,

	GUILD_NAME_MAX_LEN		= 12,

	SHOP_HOST_ITEM_MAX_NUM	= 40,	/* ȣ��Ʈ�� �ִ� ������ ���� */
	SHOP_GUEST_ITEM_MAX_NUM = 18,	/* �Խ�Ʈ�� �ִ� ������ ���� */

	SHOP_PRICELIST_MAX_NUM	= 40,	///< ���λ��� �������� ����Ʈ���� ������ ���������� �ִ� ����

	CHAT_MAX_LEN			= 512,

	QUICKSLOT_MAX_NUM		= 36,

	JOURNAL_MAX_NUM			= 2,

	QUERY_MAX_LEN			= 8192,

	FILE_MAX_LEN			= 128,
	PLAYER_EXP_TABLE_MAX	= 120,
	PLAYER_MAX_LEVEL_CONST	= 250,

	GUILD_MAX_LEVEL			= 20,
	MOB_MAX_LEVEL			= 100,

	ATTRIBUTE_MAX_VALUE		= 20,
	CHARACTER_PATH_MAX_NUM	= 64,
	SKILL_MAX_NUM			= 255,
	SKILLBOOK_DELAY_MIN		= 64800,
	SKILLBOOK_DELAY_MAX		= 108000,
	SKILL_MAX_LEVEL			= 40,

	APPLY_NAME_MAX_LEN		= 32,
	EVENT_FLAG_NAME_MAX_LEN = 32,

	MOB_SKILL_MAX_NUM		= 5,

	POINT_MAX_NUM = 255,
	DRAGON_SOUL_BOX_SIZE = 32,
	DRAGON_SOUL_BOX_COLUMN_NUM = 8,
	DRAGON_SOUL_BOX_ROW_NUM = DRAGON_SOUL_BOX_SIZE / DRAGON_SOUL_BOX_COLUMN_NUM,
	DRAGON_SOUL_REFINE_GRID_SIZE = 15,
	MAX_AMOUNT_OF_MALL_BONUS	= 20,

#ifdef ENABLE_RUNE_SYSTEM
	WEAR_MAX_NUM = 39,
#else
	WEAR_MAX_NUM = 32,
#endif
#ifdef __ENABLE_NEW_OFFLINESHOP__
	OFFLINE_SHOP_NAME_MAX_LEN			= 40 + CHARACTER_NAME_MAX_LEN +1,
	OFFLINESHOP_DURATION_MAX_MINUTES	= 8* 24 * 60,
	OFFLINESHOP_MAX_SEARCH_RESULT		= 250,
	OFFLINESHOP_SECONDS_PER_SEARCH		= 15,
	//*new check about auction offers
	OFFLINESHOP_SECONDS_PER_OFFER		= 7,
#endif
#ifdef ENABLE_LONG_LONG
	GOLD_MAX = 1000000000000001LL,
#else
	// LIMIT_GOLD
	GOLD_MAX = 2000000000,
#endif
#ifdef ENABLE_GAYA_SYSTEM
	GAYA_MAX = 999999,
#endif
	//END_LIMIT_GOLD

	OPENID_AUTHKEY_LEN = 32,

	SHOP_TAB_NAME_MAX = 32,
	SHOP_TAB_COUNT_MAX = 3,

	BELT_INVENTORY_SLOT_WIDTH = 4,
	BELT_INVENTORY_SLOT_HEIGHT= 4,

	BELT_INVENTORY_SLOT_COUNT = BELT_INVENTORY_SLOT_WIDTH * BELT_INVENTORY_SLOT_HEIGHT,
#ifdef ENABLE_RANKING
	RANKING_MAX_CATEGORIES = 20,
#endif

/**
	 **** ������� �Ҵ� �� ������ ���� ���� (DB�� Item Position) ****
	+------------------------------------------------------+ 0
	| ĳ���� �⺻ �κ��丮 (45ĭ * 2������) 90ĭ           |
	+------------------------------------------------------+ 90 = INVENTORY_MAX_NUM(90)
	| ĳ���� ��� â (�������� ������) 32ĭ                |
	+------------------------------------------------------+ 122 = INVENTORY_MAX_NUM(90) + WEAR_MAX_NUM(32)
	| ��ȥ�� ��� â (�������� ��ȥ��) 12ĭ                |
	+------------------------------------------------------+ 134 = 122 + DS_SLOT_MAX(6) * DRAGON_SOUL_DECK_MAX_NUM(2)
	| ��ȥ�� ��� â ���� (���� �̻��) 18ĭ               |
	+------------------------------------------------------+ 152 = 134 + DS_SLOT_MAX(6) * DRAGON_SOUL_DECK_RESERVED_MAX_NUM(3)
	| ��Ʈ �κ��丮 (��Ʈ ����ÿ��� ��Ʈ ������ ���� Ȱ��)|
	+------------------------------------------------------+ 168 = 152 + BELT_INVENTORY_SLOT_COUNT(16) = INVENTORY_AND_EQUIP_CELL_MAX
	| �̻��                                               |
	+------------------------------------------------------+ ??
*/
#ifdef __ENABLE_NEW_OFFLINESHOP__
#ifdef __ENABLE_CHEQUE_SYSTEM__
	YANG_PER_CHEQUE = 100000000,
#endif
#endif
#ifdef ENABLE_BUY_WITH_ITEM
	MAX_SHOP_PRICES = 5,
#endif
#ifdef ENABLE_HWID
	HWID_LENGTH = 64,
#endif
};

#ifdef ENABLE_SWITCHBOT

enum SwitchbotValues
{
	SWITCHBOT_SLOT_COUNT = 7,
	SWITCHBOT_ALTERNATIVE_COUNT = 5,
	//1 = Switching Item
	//2 = Yang
	SWITCHBOT_PRICE_TYPE = 1,
	//Amount Swtichting Items the Yang-Price
	SWITCHBOT_PRICE_AMOUNT = 1,
};

const DWORD c_arSwitchingItems[5] =
{
	39028,
	71151,
	71084,
	76014,
	88959
};
#endif

enum EWearPositions
{
	WEAR_BODY,		// 0
	WEAR_HEAD,		// 1
	WEAR_FOOTS,		// 2
	WEAR_WRIST,		// 3
	WEAR_WEAPON,	// 4
	WEAR_NECK,		// 5
	WEAR_EAR,		// 6
	WEAR_UNIQUE1,	// 7
	WEAR_UNIQUE2,	// 8
	WEAR_ARROW,		// 9
	WEAR_SHIELD,	// 10
	WEAR_ABILITY1,  // 11
	WEAR_ABILITY2,  // 12
	WEAR_ABILITY3,  // 13
	WEAR_ABILITY4,  // 14
	WEAR_ABILITY5,  // 15
	WEAR_ABILITY6,  // 16
	WEAR_ABILITY7,  // 17
#ifndef ENABLE_STOLE_REAL
	WEAR_COSTUME_STOLE,
#else
	WEAR_ABILITY8,
#endif
	WEAR_COSTUME_BODY,	// 19
	WEAR_COSTUME_HAIR,	// 20

	WEAR_RING1,			// 21	: �ű� ��������1 (����)
#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
	WEAR_COSTUME_MOUNT = WEAR_RING1, // costume_mount == ring1
#endif

	WEAR_RING2,			// 22	: �ű� ��������2 (������)
#ifdef ENABLE_ACCE_SYSTEM
	WEAR_COSTUME_ACCE = WEAR_RING2, // costume_acce == ring2
#endif

	WEAR_BELT,			// 23	: �ű� ��Ʈ����

#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
	WEAR_COSTUME_WEAPON,
#endif

#ifdef ENABLE_PENDANT
	WEAR_PENDANT,		// 24
#endif
#ifdef ENABLE_COSTUME_PET
	WEAR_COSTUME_PET_SKIN = 26,
#endif
#ifdef ENABLE_COSTUME_MOUNT
	WEAR_COSTUME_MOUNT_SKIN = 27,
#endif
#ifdef ENABLE_COSTUME_EFFECT
	WEAR_COSTUME_EFFECT_BODY = 28,
	WEAR_COSTUME_EFFECT_WEAPON = 29,
#endif
#ifdef ENABLE_RUNE_SYSTEM
	WEAR_RUNE1 = 32,
	WEAR_RUNE2,
	WEAR_RUNE3,
	WEAR_RUNE4,
	WEAR_RUNE5,
	WEAR_RUNE6,
	WEAR_RUNE7,
	WEAR_MAX = 39
#else
	WEAR_MAX = 32
#endif
};

enum EDragonSoulDeckType
{
	DRAGON_SOUL_DECK_0,
	DRAGON_SOUL_DECK_1,
	DRAGON_SOUL_DECK_MAX_NUM = 2,

	DRAGON_SOUL_DECK_RESERVED_MAX_NUM = 3,	// NOTE: �߿�! ���� ��������� ������, 3������ �з��� ���� �� ��. DS DECK�� �ø� ��� �ݵ�� �� ����ŭ RESERVED���� �����ؾ� ��!
};

enum ESex
{
	SEX_MALE,
	SEX_FEMALE
};

enum EDirection
{
	DIR_NORTH,
	DIR_NORTHEAST,
	DIR_EAST,
	DIR_SOUTHEAST,
	DIR_SOUTH,
	DIR_SOUTHWEST,
	DIR_WEST,
	DIR_NORTHWEST,
	DIR_MAX_NUM
};

#define ABILITY_MAX_LEVEL	10  /* ��� �ִ� ���� */

enum EAbilityDifficulty
{
	DIFFICULTY_EASY,
	DIFFICULTY_NORMAL,
	DIFFICULTY_HARD,
	DIFFICULTY_VERY_HARD,
	DIFFICULTY_NUM_TYPES
};

enum EAbilityCategory
{
	CATEGORY_PHYSICAL,	/* ��ü�� �����Ƽ */
	CATEGORY_MENTAL,	/* ������ �����Ƽ */
	CATEGORY_ATTRIBUTE,	/* �ɷ� �����Ƽ */
	CATEGORY_NUM_TYPES
};

enum EJobs
{
	JOB_WARRIOR,
	JOB_ASSASSIN,
	JOB_SURA,
	JOB_SHAMAN,
#ifdef ENABLE_WOLFMAN_CHARACTER
	JOB_WOLFMAN,		// ������. �����ڵ���� WOLFMAN. (��ȹ�ڰ� ���߾�. ���߿� WOLF GIRL ���ܵ� ������ �Ӷ����������Ф�)
#endif
	JOB_MAX_NUM
};

enum ESkillGroups
{
	SKILL_GROUP_MAX_NUM = 2,
};

enum ERaceFlags
{
	RACE_FLAG_ANIMAL	= (1 << 0),
	RACE_FLAG_UNDEAD	= (1 << 1),
	RACE_FLAG_DEVIL		= (1 << 2),
	RACE_FLAG_HUMAN		= (1 << 3),
	RACE_FLAG_ORC		= (1 << 4),
	RACE_FLAG_MILGYO	= (1 << 5),
	RACE_FLAG_INSECT	= (1 << 6),
	RACE_FLAG_FIRE		= (1 << 7),
	RACE_FLAG_ICE		= (1 << 8),
	RACE_FLAG_DESERT	= (1 << 9),
	RACE_FLAG_TREE		= (1 << 10),
#ifdef ELEMENT_TARGET
	RACE_FLAG_ATT_ELEC	= (1 << 11),
	RACE_FLAG_ATT_FIRE	= (1 << 12),
	RACE_FLAG_ATT_ICE	= (1 << 13),
	RACE_FLAG_ATT_WIND	= (1 << 14),
	RACE_FLAG_ATT_EARTH	= (1 << 15),
	RACE_FLAG_ATT_DARK	= (1 << 16),
#endif
#ifdef ENABLE_DS_RUNE
	RACE_FLAG_RUNE = (1 << 17),
#endif
};

enum ELoads
{
	LOAD_NONE,
	LOAD_LIGHT,
	LOAD_NORMAL,
	LOAD_HEAVY,
	LOAD_MASSIVE
};

enum
{
	QUICKSLOT_TYPE_NONE,
	QUICKSLOT_TYPE_ITEM,
	QUICKSLOT_TYPE_SKILL,
	QUICKSLOT_TYPE_COMMAND,
#ifdef ENABLE_EXTRA_INVENTORY
	QUICKSLOT_TYPE_ITEM_EXTRA,
#endif
	QUICKSLOT_TYPE_MAX_NUM,
};

enum EParts
{
	PART_MAIN,
	PART_WEAPON,
	PART_HEAD,
	PART_HAIR,

#ifdef ENABLE_ACCE_SYSTEM
	PART_ACCE,
#endif
#ifdef ENABLE_COSTUME_EFFECT
	PART_EFFECT_BODY,
	PART_EFFECT_WEAPON,
#endif
#ifdef ENABLE_RUNE_SYSTEM
	PART_RUNE,
#endif
	PART_MAX_NUM,
	PART_WEAPON_SUB,
};

enum EChatType
{
	CHAT_TYPE_TALKING,	/* �׳� ä�� */
	CHAT_TYPE_INFO,	/* ���� (�������� ������, ����ġ�� �����. ��) */
	CHAT_TYPE_NOTICE,	/* �������� */
	CHAT_TYPE_PARTY,	/* ��Ƽ�� */
	CHAT_TYPE_GUILD,	/* ��帻 */
	CHAT_TYPE_COMMAND,	/* �Ϲ� ��� */
	CHAT_TYPE_SHOUT,	/* ��ġ�� */
	CHAT_TYPE_WHISPER,
	CHAT_TYPE_BIG_NOTICE,
#ifdef ENABLE_DICE_SYSTEM
	CHAT_TYPE_DICE_INFO, //11
#endif
#ifdef ENABLE_NEW_CHAT
	CHAT_TYPE_INFO_EXP,
	CHAT_TYPE_INFO_ITEM,
	CHAT_TYPE_INFO_VALUE,
#endif
	CHAT_TYPE_DIALOG,
	CHAT_TYPE_MAX_NUM
};

enum EWhisperType
{
	WHISPER_TYPE_NORMAL = 0,
	WHISPER_TYPE_NOT_EXIST = 1,
	WHISPER_TYPE_TARGET_BLOCKED = 2,
	WHISPER_TYPE_SENDER_BLOCKED = 3,
	WHISPER_TYPE_ERROR = 4,
	WHISPER_TYPE_GM = 5,
#if defined(BL_OFFLINE_MESSAGE)
	WHISPER_TYPE_OFFLINE = 6,
#endif
	WHISPER_TYPE_SYSTEM = 0xFF
};

enum ECharacterPosition
{
	POSITION_GENERAL,
	POSITION_BATTLE,
	POSITION_DYING,
	POSITION_SITTING_CHAIR,
	POSITION_SITTING_GROUND,
	POSITION_INTRO,
	POSITION_MAX_NUM
};

enum EGMLevels
{
	GM_PLAYER,
	GM_LOW_WIZARD,
	GM_WIZARD,
	GM_HIGH_WIZARD,
	GM_GOD,
	GM_IMPLEMENTOR,
	GM_DISABLE,
};

enum EMobRank
{
	MOB_RANK_PAWN,
	MOB_RANK_S_PAWN,
	MOB_RANK_KNIGHT,
	MOB_RANK_S_KNIGHT,
	MOB_RANK_BOSS,
	MOB_RANK_KING,
	MOB_RANK_MAX_NUM
};

enum ECharType
{
	CHAR_TYPE_MONSTER,
	CHAR_TYPE_NPC,
	CHAR_TYPE_STONE,
	CHAR_TYPE_WARP,
	CHAR_TYPE_DOOR,
	CHAR_TYPE_BUILDING,
	CHAR_TYPE_PC,
	CHAR_TYPE_POLYMORPH_PC,
	CHAR_TYPE_HORSE,
	CHAR_TYPE_GOTO
};

enum EBattleType
{
	BATTLE_TYPE_MELEE,
	BATTLE_TYPE_RANGE,
	BATTLE_TYPE_MAGIC,
	BATTLE_TYPE_SPECIAL,
	BATTLE_TYPE_POWER,
	BATTLE_TYPE_TANKER,
	BATTLE_TYPE_SUPER_POWER,
	BATTLE_TYPE_SUPER_TANKER,
	BATTLE_TYPE_MAX_NUM
};

enum EApplyTypes
{
	APPLY_NONE,			// 0
	APPLY_MAX_HP,		// 1
	APPLY_MAX_SP,		// 2
	APPLY_CON,			// 3
	APPLY_INT,			// 4
	APPLY_STR,			// 5
	APPLY_DEX,			// 6
	APPLY_ATT_SPEED,	// 7
	APPLY_MOV_SPEED,	// 8
	APPLY_CAST_SPEED,	// 9
	APPLY_HP_REGEN,		// 10
	APPLY_SP_REGEN,		// 11
	APPLY_POISON_PCT,	// 12
	APPLY_STUN_PCT,		// 13
	APPLY_SLOW_PCT,		// 14
	APPLY_CRITICAL_PCT,		// 15
	APPLY_PENETRATE_PCT,	// 16
	APPLY_ATTBONUS_HUMAN,	// 17
	APPLY_ATTBONUS_ANIMAL,	// 18
	APPLY_ATTBONUS_ORC,		// 19
	APPLY_ATTBONUS_MILGYO,	// 20
	APPLY_ATTBONUS_UNDEAD,	// 21
	APPLY_ATTBONUS_DEVIL,	// 22
	APPLY_STEAL_HP,			// 23
	APPLY_STEAL_SP,			// 24
	APPLY_MANA_BURN_PCT,	// 25
	APPLY_DAMAGE_SP_RECOVER,	// 26
	APPLY_BLOCK,			// 27
	APPLY_DODGE,			// 28
	APPLY_RESIST_SWORD,		// 29
	APPLY_RESIST_TWOHAND,	// 30
	APPLY_RESIST_DAGGER,	// 31
	APPLY_RESIST_BELL,		// 32
	APPLY_RESIST_FAN,		// 33
	APPLY_RESIST_BOW,		// 34
	APPLY_RESIST_FIRE,		// 35
	APPLY_RESIST_ELEC,		// 36
	APPLY_RESIST_MAGIC,		// 37
	APPLY_RESIST_WIND,		// 38
	APPLY_REFLECT_MELEE,	// 39
	APPLY_REFLECT_CURSE,	// 40
	APPLY_POISON_REDUCE,	// 41
	APPLY_KILL_SP_RECOVER,	// 42
	APPLY_EXP_DOUBLE_BONUS,	// 43
	APPLY_GOLD_DOUBLE_BONUS,	// 44
	APPLY_ITEM_DROP_BONUS,	// 45
	APPLY_POTION_BONUS,		// 46
	APPLY_KILL_HP_RECOVER,	// 47
	APPLY_IMMUNE_STUN,		// 48
	APPLY_IMMUNE_SLOW,		// 49
	APPLY_IMMUNE_FALL,		// 50
	APPLY_SKILL,			// 51
	APPLY_BOW_DISTANCE,		// 52
	APPLY_ATT_GRADE_BONUS,	// 53
	APPLY_DEF_GRADE_BONUS,	// 54
	APPLY_MAGIC_ATT_GRADE,	// 55
	APPLY_MAGIC_DEF_GRADE,	// 56
	APPLY_CURSE_PCT,		// 57
	APPLY_MAX_STAMINA,		// 58
	APPLY_ATTBONUS_WARRIOR,	// 59
	APPLY_ATTBONUS_ASSASSIN,	// 60
	APPLY_ATTBONUS_SURA,	// 61
	APPLY_ATTBONUS_SHAMAN,	// 62
	APPLY_ATTBONUS_MONSTER,	// 63
	APPLY_MALL_ATTBONUS,			// 64 ���ݷ� +x%
	APPLY_MALL_DEFBONUS,			// 65 ���� +x%
	APPLY_MALL_EXPBONUS,			// 66 ����ġ +x%
	APPLY_MALL_ITEMBONUS,			// 67 ������ ����� x/10��
	APPLY_MALL_GOLDBONUS,			// 68 �� ����� x/10��
	APPLY_MAX_HP_PCT,				// 69 �ִ� ����� +x%
	APPLY_MAX_SP_PCT,				// 70 �ִ� ���ŷ� +x%
	APPLY_SKILL_DAMAGE_BONUS,		// 71 ��ų ������ * (100+x)%
	APPLY_NORMAL_HIT_DAMAGE_BONUS,	// 72 ��Ÿ ������ * (100+x)%
	APPLY_SKILL_DEFEND_BONUS,		// 73 ��ų ������ ��� * (100-x)%
	APPLY_NORMAL_HIT_DEFEND_BONUS,	// 74 ��Ÿ ������ ��� * (100-x)%
	APPLY_PC_BANG_EXP_BONUS,		// 75 PC�� ������ EXP ���ʽ�
	APPLY_PC_BANG_DROP_BONUS,		// 76 PC�� ������ ����� ���ʽ�

	APPLY_EXTRACT_HP_PCT,			// 77 ���� HP �Ҹ�

	APPLY_RESIST_WARRIOR,			// 78 ���翡�� ����
	APPLY_RESIST_ASSASSIN,			// 79 �ڰ����� ����
	APPLY_RESIST_SURA,				// 80 ���󿡰� ����
	APPLY_RESIST_SHAMAN,			// 81 ���翡�� ����
	APPLY_ENERGY,					// 82 ���
	APPLY_DEF_GRADE,				// 83 ����. DEF_GRADE_BONUS�� Ŭ�󿡼� �ι�� �������� �ǵ��� ����(...)�� �ִ�.
	APPLY_COSTUME_ATTR_BONUS,		// 84 �ڽ�Ƭ �����ۿ� ���� �Ӽ�ġ ���ʽ�
	APPLY_MAGIC_ATTBONUS_PER,		// 85 ���� ���ݷ� +x%
	APPLY_MELEE_MAGIC_ATTBONUS_PER,			// 86 ���� + �и� ���ݷ� +x%

	APPLY_RESIST_ICE,		// 87 �ñ� ����
	APPLY_RESIST_EARTH,		// 88 ���� ����
	APPLY_RESIST_DARK,		// 89 ��� ����

	APPLY_ANTI_CRITICAL_PCT,	//90 ũ��Ƽ�� ����
	APPLY_ANTI_PENETRATE_PCT,	//91 ����Ÿ�� ����

#ifdef ENABLE_WOLFMAN_CHARACTER
	APPLY_BLEEDING_REDUCE			= 92,		//92
	APPLY_BLEEDING_PCT				= 93,		//93
	APPLY_ATTBONUS_WOLFMAN			= 94,		//94 ���������� ����
	APPLY_RESIST_WOLFMAN			= 95,		//95 ���������� ����
	APPLY_RESIST_CLAW				= 96,		//96 CLAW���⿡ ����
#endif

#ifdef ELEMENT_NEW_BONUSES
#ifdef ENABLE_NEW_BONUS_TALISMAN
	APPLY_ATTBONUS_IRR_SPADA		= 97,
	APPLY_ATTBONUS_IRR_SPADONE		= 98,
	APPLY_ATTBONUS_IRR_PUGNALE		= 99,
	APPLY_ATTBONUS_IRR_FRECCIA		= 100,
	APPLY_ATTBONUS_IRR_VENTAGLIO	= 101,
	APPLY_ATTBONUS_IRR_CAMPANA		= 102,
	APPLY_RESIST_MEZZIUOMINI		= 103,
	APPLY_DEF_TALISMAN				= 104,
	APPLY_ATTBONUS_INSECT			= 105,
	APPLY_ATTBONUS_DESERT			= 106,
	APPLY_ATTBONUS_FORT_ZODIAC		= 107,
#endif
	APPLY_ATTBONUS_ELEC				= 108,
	APPLY_ATTBONUS_FIRE				= 109,
	APPLY_ATTBONUS_ICE				= 110,
	APPLY_ATTBONUS_WIND				= 111,
	APPLY_ATTBONUS_EARTH			= 112,
	APPLY_ATTBONUS_DARK				= 113,
#endif

#ifdef ENABLE_ACCE_SYSTEM
	APPLY_ACCEDRAIN_RATE			= 114,			//115
#endif

#ifdef ENABLE_MAGIC_REDUCTION_SYSTEM
	APPLY_RESIST_MAGIC_REDUCTION	= 115,	//116
#endif
#ifdef ENABLE_STRONG_METIN
	APPLY_ATTBONUS_METIN 			= 116,
#endif
#ifdef ENABLE_STRONG_BOSS
	APPLY_ATTBONUS_BOSS 			= 117,
#endif
#ifdef ENABLE_RESIST_MONSTER
	APPLY_RESIST_MONSTER			= 118,
#endif
#ifdef ENABLE_MEDI_PVM
	APPLY_ATTBONUS_MEDI_PVM			= 119,
#endif
	APPLY_PVM_CRITICAL_PCT = 120,
#ifdef ENABLE_DS_RUNE
	APPLY_RUNE_MONSTERS = 121,
#endif
#ifdef ENABLE_NEW_COMMON_BONUSES
	APPLY_DOUBLE_DROP_ITEM = 122,
	APPLY_IRR_WEAPON_DEFENSE = 123,
#endif
	APPLY_DESIGN_1 = 124,
	APPLY_FISHING_RARE = 125,
#ifdef ENABLE_NEW_USE_POTION
	APPLY_PARTY_DROPEXP = 126,
#endif
	MAX_APPLY_NUM,
};

enum EOnClickEvents
{
	ON_CLICK_NONE,
	ON_CLICK_SHOP,
	ON_CLICK_TALK,
	ON_CLICK_MAX_NUM
};

enum EOnIdleEvents
{
	ON_IDLE_NONE,
	ON_IDLE_GENERAL,
	ON_IDLE_MAX_NUM
};

enum EWindows
{
	RESERVED_WINDOW,
	INVENTORY,
	EQUIPMENT,
	SAFEBOX,
	MALL,
	DRAGON_SOUL_INVENTORY,
	BELT_INVENTORY,
#ifdef ENABLE_EXTRA_INVENTORY
	EXTRA_INVENTORY,
#endif
#ifdef ENABLE_SWITCHBOT
	SWITCHBOT,
#endif
	GROUND,
#ifdef __ENABLE_NEW_OFFLINESHOP__
	NEW_OFFSHOP,
	SHOP_SAFEBOX,
#endif
};

enum EMobSizes
{
	MOBSIZE_RESERVED,
	MOBSIZE_SMALL,
	MOBSIZE_MEDIUM,
	MOBSIZE_BIG
};

enum EAIFlags
{
	AIFLAG_AGGRESSIVE	= (1 << 0),
	AIFLAG_NOMOVE	= (1 << 1),
	AIFLAG_COWARD	= (1 << 2),
	AIFLAG_NOATTACKSHINSU	= (1 << 3),
	AIFLAG_NOATTACKJINNO	= (1 << 4),
	AIFLAG_NOATTACKCHUNJO	= (1 << 5),
	AIFLAG_ATTACKMOB = (1 << 6 ),
	AIFLAG_BERSERK	= (1 << 7),
	AIFLAG_STONESKIN	= (1 << 8),
	AIFLAG_GODSPEED	= (1 << 9),
	AIFLAG_DEATHBLOW	= (1 << 10),
	AIFLAG_REVIVE		= (1 << 11),
};

enum EMobStatType
{
	MOB_STATTYPE_POWER,
	MOB_STATTYPE_TANKER,
	MOB_STATTYPE_SUPER_POWER,
	MOB_STATTYPE_SUPER_TANKER,
	MOB_STATTYPE_RANGE,
	MOB_STATTYPE_MAGIC,
	MOB_STATTYPE_MAX_NUM
};

enum EImmuneFlags
{
	IMMUNE_STUN		= (1 << 0),
	IMMUNE_SLOW		= (1 << 1),
	IMMUNE_FALL		= (1 << 2),
	IMMUNE_CURSE	= (1 << 3),
	IMMUNE_POISON	= (1 << 4),
	IMMUNE_TERROR	= (1 << 5),
	IMMUNE_REFLECT	= (1 << 6),
};

enum EMobEnchants
{
	MOB_ENCHANT_CURSE,
	MOB_ENCHANT_SLOW,
	MOB_ENCHANT_POISON,
	MOB_ENCHANT_STUN,
	MOB_ENCHANT_CRITICAL,
	MOB_ENCHANT_PENETRATE,
#if defined(ENABLE_WOLFMAN_CHARACTER) && !defined(USE_MOB_BLEEDING_AS_POISON)
	MOB_ENCHANT_BLEEDING,
#endif
	MOB_ENCHANTS_MAX_NUM
};

enum EMobResists
{
	MOB_RESIST_SWORD,
	MOB_RESIST_TWOHAND,
	MOB_RESIST_DAGGER,
	MOB_RESIST_BELL,
	MOB_RESIST_FAN,
	MOB_RESIST_BOW,
	MOB_RESIST_FIRE,
	MOB_RESIST_ELECT,
	MOB_RESIST_MAGIC,
	MOB_RESIST_WIND,
	MOB_RESIST_POISON,
#if defined(ENABLE_WOLFMAN_CHARACTER) && !defined(USE_MOB_CLAW_AS_DAGGER)
	MOB_RESIST_CLAW,
#endif
#if defined(ENABLE_WOLFMAN_CHARACTER) && !defined(USE_MOB_BLEEDING_AS_POISON)
	MOB_RESIST_BLEEDING,
#endif
	MOB_RESISTS_MAX_NUM
};

enum
{
	SKILL_ATTR_TYPE_NORMAL = 1,
	SKILL_ATTR_TYPE_MELEE,
	SKILL_ATTR_TYPE_RANGE,
	SKILL_ATTR_TYPE_MAGIC
		/*
		   SKILL_ATTR_TYPE_FIRE,
		   SKILL_ATTR_TYPE_ICE,
		   SKILL_ATTR_TYPE_ELEC,
		   SKILL_ATTR_TYPE_DARK,
		   */
};

enum
{
	SKILL_NORMAL,
	SKILL_MASTER,
	SKILL_GRAND_MASTER,
	SKILL_PERFECT_MASTER,
};

enum EGuildWarType
{
	GUILD_WAR_TYPE_FIELD,
	GUILD_WAR_TYPE_BATTLE,
	GUILD_WAR_TYPE_FLAG,
	GUILD_WAR_TYPE_MAX_NUM
};

enum EGuildWarState
{
	GUILD_WAR_NONE,
	GUILD_WAR_SEND_DECLARE,
	GUILD_WAR_REFUSE,
	GUILD_WAR_RECV_DECLARE,
	GUILD_WAR_WAIT_START,
	GUILD_WAR_CANCEL,
	GUILD_WAR_ON_WAR,
	GUILD_WAR_END,
	GUILD_WAR_OVER,
	GUILD_WAR_RESERVE,

	GUILD_WAR_DURATION = 30*60, // 1�ð�
	GUILD_WAR_WIN_POINT = 1000,
	GUILD_WAR_LADDER_HALF_PENALTY_TIME = 12*60*60,
};

enum EAttributeSet
{
	ATTRIBUTE_SET_WEAPON,
	ATTRIBUTE_SET_BODY,
	ATTRIBUTE_SET_WRIST,
	ATTRIBUTE_SET_FOOTS,
	ATTRIBUTE_SET_NECK,
	ATTRIBUTE_SET_HEAD,
	ATTRIBUTE_SET_SHIELD,
	ATTRIBUTE_SET_EAR,
#if defined(ENABLE_PENDANT) && defined(ENABLE_NEW_BONUS_TALISMAN)
	ATTRIBUTE_SET_PENDANT,
#endif
#ifdef ENABLE_ATTR_COSTUMES
	ATTRIBUTE_SET_COSTUME_BODY,
	ATTRIBUTE_SET_COSTUME_HAIR,
	ATTRIBUTE_SET_COSTUME_WEAPON,
#ifdef ENABLE_STOLE_COSTUME
	ATTRIBUTE_SET_COSTUME_STOLE,
#endif
#endif
	ATTRIBUTE_SET_MAX_NUM
};

enum EPrivType
{
	PRIV_NONE,
	PRIV_ITEM_DROP,
	PRIV_GOLD_DROP,
	PRIV_GOLD10_DROP,
	PRIV_EXP_PCT,
	MAX_PRIV_NUM,
};

enum EMoneyLogType
{
	MONEY_LOG_RESERVED,
	MONEY_LOG_MONSTER,
	MONEY_LOG_SHOP,
	MONEY_LOG_REFINE,
	MONEY_LOG_QUEST,
	MONEY_LOG_GUILD,
	MONEY_LOG_MISC,
	MONEY_LOG_MONSTER_KILL,
	MONEY_LOG_DROP,
	MONEY_LOG_TYPE_MAX_NUM,
};

enum EPremiumTypes
{
	PREMIUM_EXP,		// ����ġ�� 1.2��
	PREMIUM_ITEM,		// ������ ������� 2��
	PREMIUM_SAFEBOX,		// â�� 1ĭ���� 3ĭ
	PREMIUM_AUTOLOOT,		// �� �ڵ� �ݱ�
	PREMIUM_FISH_MIND,		// ��� ����� ���� Ȯ�� ���
	PREMIUM_MARRIAGE_FAST,	// �ݽ� ���� ���� �������մϴ�.
	PREMIUM_GOLD,		// �� ������� 1.5��
	PREMIUM_MAX_NUM = 9
};

enum SPECIAL_EFFECT
{
	SE_NONE,

	SE_HPUP_RED,
	SE_SPUP_BLUE,
	SE_SPEEDUP_GREEN,
	SE_DXUP_PURPLE,
	SE_CRITICAL,
	SE_PENETRATE,
	SE_BLOCK,
	SE_DODGE,
	SE_CHINA_FIREWORK,
	SE_SPIN_TOP,
	SE_SUCCESS,
	SE_FAIL,
	SE_FR_SUCCESS,
	SE_LEVELUP_ON_14_FOR_GERMANY,
	SE_LEVELUP_UNDER_15_FOR_GERMANY,
	SE_PERCENT_DAMAGE1,
	SE_PERCENT_DAMAGE2,
	SE_PERCENT_DAMAGE3,

	SE_AUTO_HPUP,
	SE_AUTO_SPUP,

	SE_EQUIP_RAMADAN_RING,		// �󸶴� �ʽ´��� ����(71135) ������ �� ����Ʈ (�ߵ�����Ʈ��, ��������Ʈ �ƴ�)
	SE_EQUIP_HALLOWEEN_CANDY,		// �ҷ��� ������ ����(-_-;)�� ������ �ߵ��ϴ� ����Ʈ
	SE_EQUIP_HAPPINESS_RING,		// ũ�������� �ູ�� ����(71143) ������ �� ����Ʈ (�ߵ�����Ʈ��, ��������Ʈ �ƴ�)
	SE_EQUIP_LOVE_PENDANT,		// �߷�Ÿ�� ����� �Ҵ�Ʈ(71145) ������ �� ����Ʈ (�ߵ�����Ʈ��, ��������Ʈ �ƴ�)

#ifdef ENABLE_ACCE_SYSTEM
	SE_EFFECT_ACCE_SUCCEDED,
	SE_EFFECT_ACCE_EQUIP,
#endif
#ifdef __VERSION_162__
	SE_EFFECT_HEALER,
#endif

#ifdef ENABLE_TALISMAN_EFFECT
	SE_EFFECT_TALISMAN_EQUIP_FIRE,
	SE_EFFECT_TALISMAN_EQUIP_ICE,
	SE_EFFECT_TALISMAN_EQUIP_WIND,
	SE_EFFECT_TALISMAN_EQUIP_EARTH,
	SE_EFFECT_TALISMAN_EQUIP_DARK,
	SE_EFFECT_TALISMAN_EQUIP_ELEC,
#endif

#ifdef __EFFETTO_MANTELLO__
	SE_MANTELLO,
#endif

};

enum ETeenFlags
{
	TEENFLAG_NONE = 0,
	TEENFLAG_1HOUR,
	TEENFLAG_2HOUR,
	TEENFLAG_3HOUR,
	TEENFLAG_4HOUR,
	TEENFLAG_5HOUR,
};

#include "item_length.h"

// inventory�� position�� ��Ÿ���� ����ü
// int���� �Ͻ��� ����ȯ�� �ִ� ������,
// �κ� ���õ� ��� �Լ��� window_type�� ���� �ʰ�, cell �ϳ��� �޾ұ� ������,(�������� �κ��� �ϳ� ���̾ inventory type�̶��� �ʿ������ ������,)
// �κ� ���� ��� �Լ� ȣ��κ��� �����ϴ� ���� �����ϱ� �����̴�.

enum EDragonSoulRefineWindowSize
{
	DRAGON_SOUL_REFINE_GRID_MAX = 15,
};

enum EMisc2
{
	DRAGON_SOUL_EQUIP_SLOT_START = INVENTORY_MAX_NUM + WEAR_MAX_NUM, // 212
	DRAGON_SOUL_EQUIP_SLOT_END = DRAGON_SOUL_EQUIP_SLOT_START + (DS_SLOT_MAX * DRAGON_SOUL_DECK_MAX_NUM), // 224
	DRAGON_SOUL_EQUIP_RESERVED_SLOT_END = DRAGON_SOUL_EQUIP_SLOT_END + (DS_SLOT_MAX * DRAGON_SOUL_DECK_RESERVED_MAX_NUM),

	BELT_INVENTORY_SLOT_START = DRAGON_SOUL_EQUIP_RESERVED_SLOT_END,
	BELT_INVENTORY_SLOT_END = BELT_INVENTORY_SLOT_START + BELT_INVENTORY_SLOT_COUNT,

	INVENTORY_AND_EQUIP_SLOT_MAX = BELT_INVENTORY_SLOT_END,
};

#pragma pack(push, 1)

typedef struct SItemPos
{
	BYTE window_type;
	WORD cell;
    SItemPos ()
    {
        window_type = INVENTORY;
		cell = WORD_MAX;
    }

	SItemPos (BYTE _window_type, WORD _cell)
    {
        window_type = _window_type;
        cell = _cell;
    }

	bool IsValidItemPosition() const
	{
		switch (window_type)
		{
		case RESERVED_WINDOW:
			return false;
		case INVENTORY:
		case EQUIPMENT:
		case BELT_INVENTORY:
			return cell < INVENTORY_AND_EQUIP_SLOT_MAX;
		case DRAGON_SOUL_INVENTORY:
			return cell < (DRAGON_SOUL_INVENTORY_MAX_NUM);
		// �������� ũ�Ⱑ �������� window�� valid üũ�� �� ���� ����.
		case SAFEBOX:
		case MALL:
			return false;
#ifdef ENABLE_EXTRA_INVENTORY
		case EXTRA_INVENTORY:
			return cell < (EXTRA_INVENTORY_MAX_NUM);
#endif
#ifdef ENABLE_SWITCHBOT
		case SWITCHBOT:
			return cell < SWITCHBOT_SLOT_COUNT;
#endif
		default:
			return false;
		}
		return false;
	}

	bool IsEquipPosition() const
	{
		return ((INVENTORY == window_type || EQUIPMENT == window_type) && cell >= INVENTORY_MAX_NUM && cell < INVENTORY_MAX_NUM + WEAR_MAX_NUM)
			|| IsDragonSoulEquipPosition();
	}

	bool IsDragonSoulEquipPosition() const
	{
#ifdef ENABLE_EXTRA_INVENTORY
		if (window_type == EXTRA_INVENTORY)
			return false;
#endif
		
		return (DRAGON_SOUL_EQUIP_SLOT_START <= cell) && (DRAGON_SOUL_EQUIP_SLOT_END > cell);
	}

	bool IsBeltInventoryPosition() const
	{
#ifdef ENABLE_EXTRA_INVENTORY
		if (window_type == EXTRA_INVENTORY)
			return false;
#endif
		
		return (BELT_INVENTORY_SLOT_START <= cell) && (BELT_INVENTORY_SLOT_END > cell);
	}
	bool IsDefaultInventoryPosition() const
	{
		return INVENTORY == window_type && cell < INVENTORY_MAX_NUM;
	}
#ifdef ENABLE_SWITCHBOT
	bool IsSwitchbotPosition() const
	{
		return SWITCHBOT == window_type && cell < SWITCHBOT_SLOT_COUNT;
	}
#endif
	bool operator==(const struct SItemPos& rhs) const
	{
		return (window_type == rhs.window_type) && (cell == rhs.cell);
	}
	bool operator<(const struct SItemPos& rhs) const
	{
		return (window_type < rhs.window_type) || ((window_type == rhs.window_type) && (cell < rhs.cell));
	}
} TItemPos;

const TItemPos NPOS (RESERVED_WINDOW, WORD_MAX);

typedef enum
{
	SHOP_COIN_TYPE_GOLD, // DEFAULT VALUE
	SHOP_COIN_TYPE_SECONDARY_COIN,
} EShopCoinType;

#ifdef __SKILL_COLOR_SYSTEM__
namespace ESkillColorLength
{
	enum ESkillColorLength
	{
		MAX_SKILL_COUNT = 6,
		MAX_EFFECT_COUNT = 5,
		MAX_BUFF_COUNT = 5,
		BUFF_BEGIN = MAX_SKILL_COUNT,
	};
}
#endif

#ifdef ENABLE_BATTLE_PASS
enum EBattlePassInfo
{
	MISSION_REWARD_COUNT = 3,
};

enum EBattlePassType
{
	MISSION_TYPE_NONE,
	
	MONSTER_KILL,
	PLAYER_KILL,
	
	MONSTER_DAMAGE,
	PLAYER_DAMAGE,
	
	USE_ITEM,
	SELL_ITEM,
	CRAFT_ITEM,
	REFINE_ITEM,
	DESTROY_ITEM,
	COLLECT_ITEM,

	FRY_FISH,
	CATCH_FISH,
	
	SPENT_YANG,
	FARM_YANG,
	
	COMPLETE_DUNGEON,
	COMPLETE_MINIGAME,
	
#ifdef ENABLE_BATTLE_PASS_STAY_ONLINE
	STAY_ONLINE_MINUTES,
#endif

#ifdef ENABLE_BATTLE_PASS_CHAT_CNT
	COUNTER_CHAT,
#endif

	MISSION_TYPE_MAX,
};
#endif

#ifdef ENABLE_STOLE_REAL
#define WEAR_COSTUME_ACCE_SLOT	18
#else
#define WEAR_COSTUME_ACCE_SLOT	22
#endif




#ifdef ENABLE_MULTI_LANGUAGE
enum ELanguages
{
	LANGUAGE_DEFAULT,		// 0
	LANGUAGE_EN,            // 1
	LANGUAGE_RO,            // 2   
	LANGUAGE_IT,            // 3   
	LANGUAGE_TR,            // 4   
	LANGUAGE_DE,            // 5   
	LANGUAGE_PL,            // 6   
	LANGUAGE_PT,            // 7
	LANGUAGE_ES,			// 8
	LANGUAGE_CZ,			// 9
	LANGUAGE_HU,			// 10
	LANGUAGE_MAX_NUM,		// MAX NUM 10
	DEFAULT_LANGUAGE = LANGUAGE_EN,
};
#endif

#ifdef ENABLE_BIOLOGIST_UI
enum biologAffect {
	AFFECT_BIOLOGIST_1 = 651,
	AFFECT_BIOLOGIST_2 = 652,
	AFFECT_BIOLOGIST_3 = 653,
	AFFECT_BIOLOGIST_4 = 654,
	AFFECT_BIOLOGIST_5 = 655,
	AFFECT_BIOLOGIST_6 = 656,
	AFFECT_BIOLOGIST_7 = 657,
	AFFECT_BIOLOGIST_8 = 658,
	AFFECT_BIOLOGIST_9 = 659,
	AFFECT_BIOLOGIST_10 = 660,
	AFFECT_BIOLOGIST_11 = 661,
	AFFECT_BIOLOGIST_12 = 662,
	AFFECT_BIOLOGIST_13 = 663,
	AFFECT_BIOLOGIST_14 = 664
};

static const long biologistMissionInfo[14][15] =
{
	// missioneID: 0
	{
		30006,						// il vnum del item richiesto per la missione.
		10,							// il contaggio dei item richiesti per completare la missione.
		600,
		APPLY_MAX_HP,				// il primo bonus ricompensa.
		1500,						// il primo valore del bonus ricompensa.
		APPLY_NONE,					// il secondo bonus ricompensa.
		0,							// il secondo valore del bonus ricompensa.
		APPLY_NONE,					// il terzo bonus ricompensa.
		0,							// il terzo valore del bonus ricompensa.
		APPLY_NONE,					// il quarto bonus ricompensa.
		0,							// il quarto valore del bonus ricompensa.
		0,							// 0 = tutti i bonus verrano dati al player | 1 = il player deve scegliere uno bonus (piu avra la posibilita di cambiarlo tramite: 164401).
		30,							// il livello minimo per fare la missione.
		75,							// % di successo per la consegna.
		AFFECT_BIOLOGIST_1,			// l'affetto su cui va stoccato il bonus.
	},
	// missioneID: 1
	{
		30047,						// il vnum del item richiesto per la missione.
		15,							// il contaggio dei item richiesti per completare la missione.
		600,
		APPLY_DEF_GRADE_BONUS,		// il primo bonus ricompensa.
		70,							// il primo valore del bonus ricompensa.
		APPLY_NONE,					// il secondo bonus ricompensa.
		0,							// il secondo valore del bonus ricompensa.
		APPLY_NONE,					// il terzo bonus ricompensa.
		0,							// il terzo valore del bonus ricompensa.
		APPLY_NONE,					// il quarto bonus ricompensa.
		0,							// il quarto valore del bonus ricompensa.
		0,							// 0 = tutti i bonus verrano dati al player | 1 = il player deve scegliere uno bonus (piu avra la posibilita di cambiarlo tramite: 164401).
		40,							// il livello minimo per fare la missione.
		70,							// % di successo per la consegna.
		AFFECT_BIOLOGIST_2,			// l'affetto su cui va stoccato il bonus.
	},
	// missioneID: 2
	{
		30015,						// il vnum del item richiesto per la missione.
		15,							// il contaggio dei item richiesti per completare la missione.
		600,						// il tempo d'attessa per consegnare l'item (in secondi).
		APPLY_ATT_GRADE_BONUS,		// il primo bonus ricompensa.
		50,							// il primo valore del bonus ricompensa.
		APPLY_NONE,					// il secondo bonus ricompensa.
		0,							// il secondo valore del bonus ricompensa.
		APPLY_NONE,					// il terzo bonus ricompensa.
		0,							// il terzo valore del bonus ricompensa.
		APPLY_NONE,					// il quarto bonus ricompensa.
		0,							// il quarto valore del bonus ricompensa.
		0,							// 0 = tutti i bonus verrano dati al player | 1 = il player deve scegliere uno bonus (piu avra la posibilita di cambiarlo tramite: 164401).
		50,							// il livello minimo per fare la missione.
		65,							// % di successo per la consegna.
		AFFECT_BIOLOGIST_3,			// l'affetto su cui va stoccato il bonus.
	},
	// missioneID: 3
	{
		30050,						// il vnum del item richiesto per la missione.
		20,							// il contaggio dei item richiesti per completare la missione.
		600,						// il tempo d'attessa per consegnare l'item (in secondi).
#ifdef ENABLE_MEDI_PVM
		APPLY_ATTBONUS_MEDI_PVM,	// il primo bonus ricompensa.
#else
		APPLY_MAX_HP,				// il primo bonus ricompensa.
#endif
		8,							// il primo valore del bonus ricompensa.
		APPLY_NONE,					// il secondo bonus ricompensa.
		0,							// il secondo valore del bonus ricompensa.
		APPLY_NONE,					// il terzo bonus ricompensa.
		0,							// il terzo valore del bonus ricompensa.
		APPLY_NONE,					// il quarto bonus ricompensa.
		0,							// il quarto valore del bonus ricompensa.
		0,							// 0 = tutti i bonus verrano dati al player | 1 = il player deve scegliere uno bonus (piu avra la posibilita di cambiarlo tramite: 164401).
		60,							// il livello minimo per fare la missione.
		50,							// % di successo per la consegna.
		AFFECT_BIOLOGIST_4,			// l'affetto su cui va stoccato il bonus.
	},
	// missioneID: 4
	{
		30165,						// il vnum del item richiesto per la missione.
		20,							// il contaggio dei item richiesti per completare la missione.
		900,						// il tempo d'attessa per consegnare l'item (in secondi).
		APPLY_ATTBONUS_MONSTER,		// il primo bonus ricompensa.
		10,							// il primo valore del bonus ricompensa.
		APPLY_NONE,					// il secondo bonus ricompensa.
		0,							// il secondo valore del bonus ricompensa.
		APPLY_NONE,					// il terzo bonus ricompensa.
		0,							// il terzo valore del bonus ricompensa.
		APPLY_NONE,					// il quarto bonus ricompensa.
		0,							// il quarto valore del bonus ricompensa.
		0,							// 0 = tutti i bonus verrano dati al player | 1 = il player deve scegliere uno bonus (piu avra la posibilita di cambiarlo tramite: 164401).
		70,							// il livello minimo per fare la missione.
		50,							// % di successo per la consegna.
		AFFECT_BIOLOGIST_5,			// l'affetto su cui va stoccato il bonus.
	},
	// missioneID: 5
	{
		30166,						// il vnum del item richiesto per la missione.
		25,							// il contaggio dei item richiesti per completare la missione.
		900,						// il tempo d'attessa per consegnare l'item (in secondi).
#ifdef ENABLE_STRONG_METIN
		APPLY_ATTBONUS_METIN,		// il primo bonus ricompensa.
#else
		APPLY_MAX_HP,				// il primo bonus ricompensa.
#endif
		10,							// il primo valore del bonus ricompensa.
		APPLY_NONE,					// il secondo bonus ricompensa.
		0,							// il secondo valore del bonus ricompensa.
		APPLY_NONE,					// il terzo bonus ricompensa.
		0,							// il terzo valore del bonus ricompensa.
		APPLY_NONE,					// il quarto bonus ricompensa.
		0,							// il quarto valore del bonus ricompensa.
		0,							// 0 = tutti i bonus verrano dati al player | 1 = il player deve scegliere uno bonus (piu avra la posibilita di cambiarlo tramite: 164401).
		80,							// il livello minimo per fare la missione.
		50,							// % di successo per la consegna.
		AFFECT_BIOLOGIST_6,			// l'affetto su cui va stoccato il bonus.
	},
	// missioneID: 6
	{
		30167,						// il vnum del item richiesto per la missione.
		30,							// il contaggio dei item richiesti per completare la missione.
		1800,						// il tempo d'attessa per consegnare l'item (in secondi).
#ifdef ENABLE_STRONG_BOSS
		APPLY_ATTBONUS_BOSS,		// il primo bonus ricompensa.
#else
		APPLY_MAX_HP,				// il primo bonus ricompensa.
#endif
		10,							// il primo valore del bonus ricompensa.
		APPLY_NONE,					// il secondo bonus ricompensa.
		0,							// il secondo valore del bonus ricompensa.
		APPLY_NONE,					// il terzo bonus ricompensa.
		0,							// il terzo valore del bonus ricompensa.
		APPLY_NONE,					// il quarto bonus ricompensa.
		0,							// il quarto valore del bonus ricompensa.
		0,							// 0 = tutti i bonus verrano dati al player | 1 = il player deve scegliere uno bonus (piu avra la posibilita di cambiarlo tramite: 164401).
		85,							// il livello minimo per fare la missione.
		50,							// % di successo per la consegna.
		AFFECT_BIOLOGIST_7,			// l'affetto su cui va stoccato il bonus.
	},
	// missioneID: 7
	{
		30168,						// il vnum del item richiesto per la missione.
		30,							// il contaggio dei item richiesti per completare la missione.
		1800,						// il tempo d'attessa per consegnare l'item (in secondi).
#ifdef ENABLE_RESIST_MONSTER
		APPLY_RESIST_MONSTER,		// il primo bonus ricompensa.
#else
		APPLY_MAX_HP,				// il primo bonus ricompensa.
#endif
		10,							// il primo valore del bonus ricompensa.
		APPLY_NONE,					// il secondo bonus ricompensa.
		0,							// il secondo valore del bonus ricompensa.
		APPLY_NONE,					// il terzo bonus ricompensa.
		0,							// il terzo valore del bonus ricompensa.
		APPLY_NONE,					// il quarto bonus ricompensa.
		0,							// il quarto valore del bonus ricompensa.
		0,							// 0 = tutti i bonus verrano dati al player | 1 = il player deve scegliere uno bonus (piu avra la posibilita di cambiarlo tramite: 164401).
		90,							// il livello minimo per fare la missione.
		50,							// % di successo per la consegna.
		AFFECT_BIOLOGIST_8,			// l'affetto su cui va stoccato il bonus.
	},
	// missioneID: 8
	{
		30251,						// il vnum del item richiesto per la missione.
		30,							// il contaggio dei item richiesti per completare la missione.
		3000,						// il tempo d'attessa per consegnare l'item (in secondi).
		APPLY_ATTBONUS_WARRIOR,		// il primo bonus ricompensa.
		10,							// il primo valore del bonus ricompensa.
		APPLY_ATTBONUS_ASSASSIN,	// il secondo bonus ricompensa.
		10,							// il secondo valore del bonus ricompensa.
		APPLY_ATTBONUS_SURA,		// il terzo bonus ricompensa.
		10,							// il terzo valore del bonus ricompensa.
		APPLY_ATTBONUS_SHAMAN,		// il quarto bonus ricompensa.
		10,							// il quarto valore del bonus ricompensa.
		0,							// 0 = tutti i bonus verrano dati al player | 1 = il player deve scegliere uno bonus (piu avra la posibilita di cambiarlo tramite: 164401).
		95,							// il livello minimo per fare la missione.
		50,							// % di successo per la consegna.
		AFFECT_BIOLOGIST_9,			// l'affetto su cui va stoccato il bonus.
	},
	// missioneID: 9
	{
		30252,						// il vnum del item richiesto per la missione.
		35,							// il contaggio dei item richiesti per completare la missione.
		3000,						// il tempo d'attessa per consegnare l'item (in secondi).
		APPLY_RESIST_WARRIOR,		// il primo bonus ricompensa.
		10,							// il primo valore del bonus ricompensa.
		APPLY_RESIST_ASSASSIN,		// il secondo bonus ricompensa.
		10,							// il secondo valore del bonus ricompensa.
		APPLY_RESIST_SURA,			// il terzo bonus ricompensa.
		10,							// il terzo valore del bonus ricompensa.
		APPLY_RESIST_SHAMAN,		// il quarto bonus ricompensa.
		10,							// il quarto valore del bonus ricompensa.
		0,							// 0 = tutti i bonus verrano dati al player | 1 = il player deve scegliere uno bonus (piu avra la posibilita di cambiarlo tramite: 164401).
		100,						// il livello minimo per fare la missione.
		50,							// % di successo per la consegna.
		AFFECT_BIOLOGIST_10,		// l'affetto su cui va stoccato il bonus.
	},
	// missioneID: 10
	{
		30002,						// il vnum del item richiesto per la missione.
		35,							// il contaggio dei item richiesti per completare la missione.
		7200,						// il tempo d'attessa per consegnare l'item (in secondi).
		APPLY_MAX_HP,				// il primo bonus ricompensa.
		1200,						// il primo valore del bonus ricompensa.
		APPLY_DEF_GRADE_BONUS,		// il secondo bonus ricompensa.
		100,						// il secondo valore del bonus ricompensa.
		APPLY_ATT_GRADE_BONUS,		// il terzo bonus ricompensa.
		60,							// il terzo valore del bonus ricompensa.
		APPLY_NONE,					// il quarto bonus ricompensa.
		0,							// il quarto valore del bonus ricompensa.
		1,							// 0 = tutti i bonus verrano dati al player | 1 = il player deve scegliere uno bonus (piu avra la posibilita di cambiarlo tramite: 164401).
		105,						// il livello minimo per fare la missione.
		50,							// % di successo per la consegna.
		AFFECT_BIOLOGIST_11,		// l'affetto su cui va stoccato il bonus.
	},
	// missioneID: 11
	{
		30003,						// il vnum del item richiesto per la missione.
		35,							// il contaggio dei item richiesti per completare la missione.
		7200,						// il tempo d'attessa per consegnare l'item (in secondi).
		APPLY_MAX_HP,				// il primo bonus ricompensa.
		1500,						// il primo valore del bonus ricompensa.
		APPLY_DEF_GRADE_BONUS,		// il secondo bonus ricompensa.
		140,						// il secondo valore del bonus ricompensa.
		APPLY_ATT_GRADE_BONUS,		// il terzo bonus ricompensa.
		80,							// il terzo valore del bonus ricompensa.
		APPLY_NONE,					// il quarto bonus ricompensa.
		0,							// il quarto valore del bonus ricompensa.
		1,							// 0 = tutti i bonus verrano dati al player | 1 = il player deve scegliere uno bonus (piu avra la posibilita di cambiarlo tramite: 164401).
		110,						// il livello minimo per fare la missione.
		50,							// % di successo per la consegna.
		AFFECT_BIOLOGIST_12,		// l'affetto su cui va stoccato il bonus.
	},
	// missioneID: 12
	{
		30004,						// il vnum del item richiesto per la missione.
		40,							// il contaggio dei item richiesti per completare la missione.
		10800,						// il tempo d'attessa per consegnare l'item (in secondi).
		APPLY_ATTBONUS_HUMAN,		// il primo bonus ricompensa.
		10,							// il primo valore del bonus ricompensa.
		APPLY_PVM_CRITICAL_PCT,		// il secondo bonus ricompensa.
		10,							// il secondo valore del bonus ricompensa.
		APPLY_NONE,					// il terzo bonus ricompensa.
		0,							// il terzo valore del bonus ricompensa.
		APPLY_NONE,					// il quarto bonus ricompensa.
		0,							// il quarto valore del bonus ricompensa.
		0,							// 0 = tutti i bonus verrano dati al player | 1 = il player deve scegliere uno bonus (piu avra la posibilita di cambiarlo tramite: 164401).
		115,						// il livello minimo per fare la missione.
		50,							// % di successo per la consegna.
		AFFECT_BIOLOGIST_13,		// l'affetto su cui va stoccato il bonus.
	},
	// missioneID: 13
	{
		30005,						// il vnum del item richiesto per la missione.
		45,							// il contaggio dei item richiesti per completare la missione.
		14400,						// il tempo d'attessa per consegnare l'item (in secondi).
		APPLY_ATTBONUS_MONSTER,		// il primo bonus ricompensa.
		10,							// il primo valore del bonus ricompensa.
#ifdef ENABLE_NEW_BONUS_TALISMAN
		APPLY_RESIST_MEZZIUOMINI,	// il secondo bonus ricompensa.
#else
		APPLY_MAX_HP,				// il secondo bonus ricompensa.
#endif
		10,							// il secondo valore del bonus ricompensa.
		APPLY_NONE,					// il terzo bonus ricompensa.
		0,							// il terzo valore del bonus ricompensa.
		APPLY_NONE,					// il quarto bonus ricompensa.
		0,							// il quarto valore del bonus ricompensa.
		0,							// 0 = tutti i bonus verrano dati al player | 1 = il player deve scegliere uno bonus (piu avra la posibilita di cambiarlo tramite: 164401).
		120,						// il livello minimo per fare la missione.
		50,							// % di successo per la consegna.
		AFFECT_BIOLOGIST_14,		// l'affetto su cui va stoccato il bonus.
	}
};
#endif

#pragma pack(pop)

#endif
