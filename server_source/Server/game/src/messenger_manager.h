#ifndef __INC_MESSENGER_MANAGER_H
#define __INC_MESSENGER_MANAGER_H

#include "db.h"

class MessengerManager : public singleton<MessengerManager>
{
	public:
		typedef std::string keyT;
		typedef const std::string & keyA;

		MessengerManager();
		virtual ~MessengerManager();

	public:
		void	P2PLogin(keyA account);
		void	P2PLogout(keyA account);

		void	Login(keyA account);
		void	Logout(keyA account);
#ifdef ENABLE_MESSENGER_TEAM
		void	SendTeamLogin(keyA account, keyA companion);
		void	SendTeamLogout(keyA account, keyA companion);
		void	LoadTeamList(SQLMsg * pmsg);
		void	SendTeamList(keyA account);
#endif
#ifdef ENABLE_MESSENGER_HELPER
		void	SendHelperLogin(keyA account, keyA companion);
		void	SendHelperLogout(keyA account, keyA companion);
		void	LoadHelperList(SQLMsg * pmsg);
		void	SendHelperList(keyA account);
#endif
		void	RequestToAdd(LPCHARACTER ch, LPCHARACTER target);
		bool	AuthToAdd(keyA account, keyA companion, bool bDeny); // @fixme130 void -> bool

		void	__AddToList(keyA account, keyA companion);	// ���� m_Relation, m_InverseRelation �����ϴ� �޼ҵ�
		void	AddToList(keyA account, keyA companion);

		void	__RemoveFromList(keyA account, keyA companion); // ���� m_Relation, m_InverseRelation �����ϴ� �޼ҵ�
		void	RemoveFromList(keyA account, keyA companion);

		void	RemoveAllList(keyA account);

		void	Initialize();

	private:
		void	SendList(keyA account);
		void	SendLogin(keyA account, keyA companion);
		void	SendLogout(keyA account, keyA companion);

		void	LoadList(SQLMsg * pmsg);

		void	Destroy();

		std::set<keyT>			m_set_loginAccount;
		std::map<keyT, std::set<keyT> >	m_Relation;
		std::map<keyT, std::set<keyT> >	m_InverseRelation;
		std::set<DWORD>			m_set_requestToAdd;
#ifdef ENABLE_MESSENGER_TEAM
		std::map<keyT, std::set<keyT> >	m_TeamRelation;
		std::map<keyT, std::set<keyT> >	m_InverseTeamRelation;
#endif
#ifdef ENABLE_MESSENGER_HELPER
		std::map<keyT, std::set<keyT> >	m_HelperRelation;
		std::map<keyT, std::set<keyT> >	m_InverseHelperRelation;
#endif
};

#endif
