#pragma once
#include "Peer.h"
#include <queue>
#include <utility>

struct TPrivEmpireData
{
    BYTE type;
    int value;
    bool bRemoved;
    BYTE empire;

    time_t end_time_sec;

    TPrivEmpireData(BYTE type, int value, BYTE empire, time_t end_time_sec)
	: type(type), value(value), bRemoved(false), empire(empire), end_time_sec(end_time_sec)
    {}
};

struct TPrivGuildData
{
    BYTE type;
    int value;
    bool bRemoved;
    DWORD guild_id;

    time_t end_time_sec;

    TPrivGuildData(BYTE type, int value, DWORD guild_id, time_t _end_time_sec)
	: type(type), value(value), bRemoved(false), guild_id(guild_id), end_time_sec(_end_time_sec )
    {}
};

struct TPrivCharData
{
    BYTE type;
    int value;
    bool bRemoved;
    DWORD pid;
    TPrivCharData(BYTE type, int value, DWORD pid)
	: type(type), value(value), bRemoved(false), pid(pid)
    {}
};

class CPrivManager : public singleton<CPrivManager>
{
    public: 
	CPrivManager();
	virtual ~CPrivManager();

	void AddGuildPriv(DWORD guild_id, BYTE type, int value, time_t time_sec);
	void AddEmpirePriv(BYTE empire, BYTE type, int value, time_t time_sec);
	void AddCharPriv(DWORD pid, BYTE type, int value);
	void Update();
	void SendPrivOnSetup(CPeer* peer);

    private:
	void SendChangeGuildPriv(DWORD guild_id, BYTE type, int value, time_t end_time_sec);
	void SendChangeEmpirePriv(BYTE empire, BYTE type, int value, time_t end_time_sec);
	void SendChangeCharPriv(DWORD pid, BYTE type, int value);

	typedef std::pair<time_t, TPrivCharData *> stPairChar;
	typedef std::pair<time_t, TPrivGuildData*> stPairGuild;
	typedef std::pair<time_t, TPrivEmpireData*> stPairEmpire;

	std::priority_queue<stPairChar, std::vector<stPairChar>, std::greater<stPairChar> >
	    m_pqPrivChar;
	std::priority_queue<stPairGuild, std::vector<stPairGuild>, std::greater<stPairGuild> > 
	    m_pqPrivGuild;
	std::priority_queue<stPairEmpire, std::vector<stPairEmpire>, std::greater<stPairEmpire> >
	    m_pqPrivEmpire;

	TPrivEmpireData* m_aaPrivEmpire[MAX_PRIV_NUM][EMPIRE_MAX_NUM];
	std::map<DWORD, TPrivGuildData*> m_aPrivGuild[MAX_PRIV_NUM];
	std::map<DWORD, TPrivCharData*> m_aPrivChar[MAX_PRIV_NUM];
};

