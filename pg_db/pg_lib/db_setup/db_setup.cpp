/*******************************************************
 // Drinkfill Database thread listener - IMPLEMENTATION
 //
 // db_setup.cpp
 // Create database, users, permissions and tables.
 //
 // created: 07-09-2020
 // by: Li Yan Tong
 //
 // copyright 2020 by Drinkfill Beverages Ltd
 // all rights reserved
 ********************************************************/

#include "db_setup.h"

using namespace std;
using namespace pqxx;

db_setup::db_setup()
{
}

// Connection
bool pg_connect_newDB()
{
}

// Create Database, Groups, Users and Tables
bool pg_db_setup(db_setup connection_details)
{
}

bool pg_user_setup(db_setup connection_details)
{
}

bool pg_table_setup(db_setup connection_details)
{
}

// Dtor
db_setup::~db_setup()
{

}

int main(int argc, char *argv[])
{
   string dbName = "DropME";
   string user = "dfadmin";

   try
   {
      // WILL NEED TO STORE AS SECRET!  connection as template1 required if db from stratch.
      string connection_string("dbname = template1 user = postgres password = * hostaddr=127.0.0.1 port=5432");
      pqxx::connection C(connection_string.c_str());

      if (C.is_open())
      {
         cout << "Opened database successfully: " << C.dbname() << endl;
         pqxx::nontransaction w(C);
         try
         {
            w.exec("CREATE DATABASE " + dbName);
         }
         catch (pqxx::sql_error &e)
         {
            string sqlErrCode = e.sqlstate();
            if (sqlErrCode == "42P04")
            { // catch duplicate_database
               cout << "Database: " << dbName << " exists, proceeding further\n";
               C.close();
               return 1;
            }
            std::cerr << "Database error: " << e.what()
                      << ", error code: " << e.sqlstate()
                      << "SQL Query was: " << e.query() << "\n";
            abort();
         }

         w.exec("GRANT ALL ON DATABASE " + dbName + " TO" + user);
         w.commit();
         C.close();
      }
      else
      {
         cout << "Can't open database" << endl;
         return 1;
      }
   }
   catch (const std::exception &e)
   {
      cerr << e.what() << std::endl;
      return 1;
   }
   return 0;
}