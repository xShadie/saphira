// vim: ts=8 sw=4
#ifndef __INC_MONEY_LOG
#define __INC_MONEY_LOG

#include <map>
#include "../../common/CommonDefines.h"

class CMoneyLog : public singleton<CMoneyLog>
{
    public:
	CMoneyLog();
	virtual ~CMoneyLog();

	void Save();
#ifdef ENABLE_LONG_LONG
	void AddLog(BYTE utype, DWORD dwVnum, long long iGold);
#else
	void AddLog(BYTE bType, DWORD dwVnum, int iGold);
#endif
    private:
	std::map<DWORD, int> m_MoneyLogContainer[MONEY_LOG_TYPE_MAX_NUM];
};

#endif
