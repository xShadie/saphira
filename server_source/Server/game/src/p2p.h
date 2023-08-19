#pragma once
#include <boost/unordered_map.hpp>
#include "input.h"
#include "../../common/stl.h"

typedef struct _CCI
{
	char	szName[CHARACTER_NAME_MAX_LEN + 1];
	DWORD	dwPID;
	BYTE	bEmpire;
	long	lMapIndex;
	BYTE	bChannel;
#ifdef ENABLE_ANTI_MULTIPLE_FARM
	char	cMAIf[MA_LENGTH + 1];
#endif
	LPDESC	pkDesc;
} CCI;

class P2P_MANAGER : public singleton<P2P_MANAGER>
{
	public:
		P2P_MANAGER();
		~P2P_MANAGER();

		void			RegisterAcceptor(LPDESC d);
		void			UnregisterAcceptor(LPDESC d);
		void			RegisterConnector(LPDESC d);
		void			UnregisterConnector(LPDESC d);
		void			EraseUserByDesc(LPDESC d);
		void			FlushOutput();
		void			Boot(LPDESC d);
		void			Send(const void * c_pvData, int iSize, LPDESC except = NULL);
		void			Login(LPDESC d, const TPacketGGLogin * p);
#ifdef ENABLE_ANTI_MULTIPLE_FARM
		void			Logout(const char* c_pszName, bool is_warping = false);
#else
		void			Logout(const char* c_pszName);
#endif
		CCI *			Find(const char * c_pszName);
		CCI *			FindByPID(DWORD pid);
		int				GetCount();
		int				GetEmpireUserCount(int idx);
		int				GetDescCount();
		void			GetP2PHostNames(std::string& hostNames);

	private:
#ifdef ENABLE_ANTI_MULTIPLE_FARM
		void			Logout(CCI* pkCCI, bool is_warping = false);
#else
		void			Logout(CCI* pkCCI);
#endif
		CInputProcessor *	m_pkInputProcessor;
		int			m_iHandleCount;
		typedef boost::unordered_map<std::string, CCI *, stringhash> TCCIMap;
		typedef boost::unordered_map<DWORD, CCI*> TPIDCCIMap;
		boost::unordered_set<LPDESC> m_set_pkPeers;
		TCCIMap			m_map_pkCCI;
		TPIDCCIMap		m_map_dwPID_pkCCI;
		int			m_aiEmpireUserCount[EMPIRE_MAX_NUM];
};
