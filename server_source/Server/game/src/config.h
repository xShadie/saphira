
#ifndef __INC_METIN_II_GAME_CONFIG_H__
#define __INC_METIN_II_GAME_CONFIG_H__

enum
{
	ADDRESS_MAX_LEN = 15
};

enum ItemDestroyTime {ITEM_DESTROY_TIME_AUTOGIVE, ITEM_DESTROY_TIME_DROPGOLD, ITEM_DESTROY_TIME_DROPITEM, ITEM_DESTROY_TIME_MAX};

void config_init(const std::string& st_localeServiceName); // default "" is CONFIG

extern char sql_addr[256];

extern WORD mother_port;
extern WORD p2p_port;

extern char db_addr[ADDRESS_MAX_LEN + 1];
extern WORD db_port;

extern char teen_addr[ADDRESS_MAX_LEN + 1];
extern WORD teen_port;

extern int passes_per_sec;
extern int save_event_second_cycle;
extern int ping_event_second_cycle;
extern int test_server;
extern bool	guild_mark_server;
extern BYTE guild_mark_min_level;

extern bool	g_bNoMoreClient;
extern bool	g_bNoRegen;

// #ifdef ENABLE_NEWSTUFF
extern bool	g_bEmpireShopPriceTripleDisable;
extern bool g_bShoutAddonEnable;
extern bool g_bGlobalShoutEnable;
extern bool g_bDisablePrismNeed;
extern bool g_bDisableEmotionMask;
#ifdef ENABLE_NEW_STACK_LIMIT
extern int g_bItemCountLimit;
#else
extern BYTE g_bItemCountLimit;
#endif
extern DWORD g_dwItemBonusChangeTime;
extern bool	g_bAllMountAttack;
extern bool	g_bEnableBootaryCheck;
extern bool	g_bGMHostCheck;
extern bool	g_bGuildInviteLimit;
extern bool	g_bGuildInfiniteMembers;
extern int g_iStatusPointGetLevelLimit;
extern int g_iStatusPointSetMaxValue;
extern int g_iShoutLimitLevel;
extern DWORD g_dwSkillBookNextReadMin;
extern DWORD g_dwSkillBookNextReadMax;
// extern int g_iShoutLimitTime;
extern int g_iDbLogLevel;
extern int g_iSysLogLevel;
extern int g_aiItemDestroyTime[ITEM_DESTROY_TIME_MAX];
extern bool	g_bDisableEmpireLanguageCheck;
// #endif
extern BYTE	g_bChannel;

extern bool	map_allow_find(int index);
extern void	map_allow_copy(long * pl, int size);
extern bool	no_wander;

extern int		g_iUserLimit;
extern time_t	g_global_time;

const char *	get_table_postfix();

extern std::string	g_stHostname;
extern std::string	g_stLocale;
extern std::string	g_stLocaleFilename;

extern char		g_szPublicIP[16];
extern char		g_szInternalIP[16];

extern int (*is_twobyte) (const char * str);
extern int (*check_name) (const char * str);

extern bool		g_bSkillDisable;

extern int		g_iFullUserCount;
extern int		g_iBusyUserCount;
extern void		LoadStateUserCount();

extern bool	g_bEmpireWhisper;

extern BYTE	g_bAuthServer;
extern BYTE	g_bBilling;

extern BYTE	PK_PROTECT_LEVEL;

extern void	LoadValidCRCList();
extern bool	IsValidProcessCRC(DWORD dwCRC);
extern bool	IsValidFileCRC(DWORD dwCRC);

extern std::string	g_stAuthMasterIP;
extern WORD		g_wAuthMasterPort;

extern std::string	g_stClientVersion;
extern bool		g_bCheckClientVersion;
extern void		CheckClientVersion();

extern std::string	g_stQuestDir;
//extern std::string	g_stQuestObjectDir;
extern std::set<std::string> g_setQuestObjectDir;

extern int g_server_id;
extern std::string g_strWebMallURL;

extern int VIEW_RANGE;
extern int VIEW_BONUS_RANGE;

extern bool g_protectNormalPlayer;      // �����ڰ� "��ȭ���" �� �Ϲ������� �������� ����
extern bool g_noticeBattleZone;         // �߸����뿡 �����ϸ� �ȳ��޼����� �˷���

extern DWORD g_GoldDropTimeLimitValue;
// #ifdef ENABLE_NEWSTUFF
extern DWORD g_BoxUseTimeLimitValue;
extern DWORD g_BuySellTimeLimitValue;
extern bool g_NoDropMetinStone;
extern bool g_NoMountAtGuildWar;
extern bool g_NoPotionsOnPVP;
// #endif

extern int gPlayerMaxLevel;
extern int gShutdownAge;
extern int gShutdownEnable;	// �⺻ 0. config���� �����ؾ���.

extern bool gHackCheckEnable;

extern bool g_BlockCharCreation;
#ifdef __ATTR_TRANSFER_SYSTEM__
extern int gAttrTransferLimit;
#endif
// missing begin
extern int openid_server;
extern std::string g_stBlockDate;

extern int g_iSpamBlockMaxLevel;
extern unsigned int g_uiSpamBlockScore;
extern unsigned int g_uiSpamBlockDuration;
extern unsigned int g_uiSpamReloadCycle;

extern void map_allow_log();
// missing end
#ifdef ENABLE_MAP_TELEPORTER
typedef std::vector<DWORD> MAPITEMS;

struct TMapConfig
{
	// int			iMapIndex;
	long			coord_x;
	long			coord_y;
#ifdef ENABLE_LONG_LONG
	long long	price;
#else
	DWORD	price;
#endif
	MAPITEMS	items;
	int			iLevel;
	int			iLevelMax;
};

typedef std::vector<TMapConfig> MAPCONFIG_VEC;

extern MAPCONFIG_VEC	g_vecMapConf;
#endif

#ifdef EANBLE_PATCH_YOHARA
extern uint8_t g_MaxChampionLevel;
#endif
#endif /* __INC_METIN_II_GAME_CONFIG_H__ */

