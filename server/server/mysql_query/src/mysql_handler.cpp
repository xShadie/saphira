/*											*/
/*  	  mysql utility for Metin2			*/
/*			   	 by BlackYuko				*/
/*	   please, don't remove the credits		*/
/*											*/

#include "mysql_handler.h"

// Constructor
MYSQL_Handler::MYSQL_Handler(const std::string& host, const std::string& user, const std::string& pass, int port)
{
	HostDB = host;
	UserDB = user;
	PassDB = pass;
	PortDB = port;
}

// Initialize mysql module
int MYSQL_Handler::init()
{
	if (!mysql_init (&mysql))
		return ERROR;
	return NO_ERROR;
}

// Connect to database
int MYSQL_Handler::connect(char *NameDB)
{
	if (!mysql_real_connect (&mysql, HostDB.c_str(), UserDB.c_str(), PassDB.c_str(), NameDB, PortDB, NULL, 0))
		return ERROR;
	return NO_ERROR;
}

// Execute a query
int MYSQL_Handler::ex_query(char *query, std::string& output_res)
{
	MYSQL_RES* query_result;
	// Execute query
	if (mysql_query(&mysql, query))
		return ERROR;  
	// Check result
    query_result = mysql_store_result (&mysql);
	// if it's not a SELECT query, the result will be NULL
	if (query_result == NULL)
	{
		output_res = "\"Query successfully executed\"";
		return NO_ERROR;
	}
	try
	{
		// if it's a SELECT query
		int fields_number, rows_number;
		MYSQL_ROW curr_row;        		
		std::stringstream output_stream;
		fields_number = mysql_num_fields(query_result);
		rows_number = mysql_num_rows(query_result);		
		if (rows_number == 0)
			// If no results, return an empty string
			output_stream << "{{\"\"}}";
		else
		{	
			// Otherwise read results and build the output string
			output_stream << "{";
			// For each row
			for(int cnt_row = 0;(curr_row = mysql_fetch_row(query_result)) != NULL;)
			{
				output_stream << "{";
				// For each field within the current row
				for (int i = 0; i < fields_number;) 
				{
					if (curr_row[i] == NULL)
						output_stream << "\"NULL\"";
					else
						output_stream << "\"" << curr_row[i] << "\"";
					if (++i != fields_number)
						output_stream << ",";
				}
				output_stream << "}";
				if (++cnt_row != rows_number)
					output_stream << ",";
			}
			output_stream << "}";
		}
		mysql_free_result(query_result);
		output_res = output_stream.str();
		return NO_ERROR;
	}
	catch (std::exception &e)
	{
		if (e.what() != NULL)
			output_res = "\"Error: " + (std::string) e.what() + "\"";
		else
			output_res = "\"Error while retrieving results\"";
		mysql_free_result(query_result);
		return ERROR;		
	}
}

// Get the last mysql error
std::string MYSQL_Handler::get_error(void)
{
	std::stringstream error_msg;
	error_msg << "Error (" << mysql_errno(&mysql) << "): " << mysql_error(&mysql);
	return error_msg.str();
}

// Close mysql module and free resources
void MYSQL_Handler::close()
{
	mysql_close(&mysql);
}
