/*											*/
/*  	  mysql utility for Metin2			*/
/*			   	 by BlackYuko				*/
/*	   please, don't remove the credits		*/
/*											*/

#include "mysql_handler.h"
#include <iostream>
#include <fstream>

using namespace std;

bool load_conf(string&, string&, string&, int&);
void WriteOutputFile(char *, const string&);

int main(int argc, char *argv[])
{
	char *NameDB, *Query, *OutputFile;
	int port_db;
	MYSQL_Handler *mysql;
	string host_db, user_db, pass_db, query_output;
	if (argc != 4)
		return EXIT_FAILURE;	
	// Read parameters
	NameDB = argv[1];
	Query = argv[2];
	OutputFile = argv[3];
	// Begin
	if (!load_conf(host_db, user_db, pass_db, port_db))	
	{
		WriteOutputFile(OutputFile, "-1,\"Error: unable to load 'conf' file\"");
		return EXIT_FAILURE;
	}		
	mysql = new MYSQL_Handler(host_db, user_db, pass_db, port_db);
	if (mysql->init() == MYSQL_Handler::ERROR)
	{
		WriteOutputFile(OutputFile, "1,\"" + mysql->get_error() + "\"");
		return EXIT_FAILURE;
	}
	if (mysql->connect(NameDB) == MYSQL_Handler::ERROR)
	{
		WriteOutputFile(OutputFile, "2,\"" + mysql->get_error() + "\"");
		mysql->close();
		delete mysql;
		return EXIT_FAILURE;
	}
	if (mysql->ex_query(Query, query_output) == MYSQL_Handler::ERROR)
	{
		WriteOutputFile(OutputFile, "3,\"" + mysql->get_error() + "\"");
		mysql->close();
		delete mysql;
		return EXIT_FAILURE;
	}
	WriteOutputFile(OutputFile, "0," + query_output);
	mysql->close();
	delete mysql;
	return EXIT_SUCCESS;
}

// Load 'conf' file
bool load_conf(string& host, string& user, string& pass, int& port)
{
	// Default values
	host = "";
	user = "";
	pass = "";
	port = 0;
	// Read file
	ifstream conf_file ("conf");
	if (!conf_file.is_open())
		return false;
	// Read host
	conf_file >> host;	
	// Read user
	conf_file >> user;	
	// Read password
	conf_file >> pass;	
	// Read port
	conf_file >> port;	
	// Close file
	conf_file.close();
	// If some information has not been read (except for port that is not strictly required), return error
	if (host == "" || user == "" || pass == "")
		return false;
	return true;
}

// Write output file
void WriteOutputFile(char * Name, const string& Output)
{
	ofstream OutFile (Name);
	if (!OutFile.is_open())
		return;
	OutFile << "return {" << Output << "}";
	OutFile.close();
}
