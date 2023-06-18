/*											*/
/*  	  mysql utility for Metin2			*/
/*			   	 by BlackYuko				*/
/*	   please, don't remove the credits		*/
/*											*/

#ifndef MYSQL_HANDLER_H
#define MYSQL_HANDLER_H

#include <mysql/mysql.h>
#include <string>
#include <sstream>

class MYSQL_Handler
{
	private:
		int PortDB;
		MYSQL mysql;
		std::string HostDB, UserDB, PassDB;
	public:
		static const int NO_ERROR = 0, ERROR = 1;
		MYSQL_Handler(const std::string&, const std::string&, const std::string&, int);
		int init(void);
		int connect(char *);
		int ex_query(char *, std::string&);
		std::string get_error(void);
		void close(void);
};

#endif
