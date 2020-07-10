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
   if(pg_connect_newDB()) {
      pg_db_setup(db_setup_connection);
   } else {
      // Error handling for failed connection
   }

   // TODO: DO NOT want postgres to manipulate server
   if(pg_close_setup(db_setup_connection)){

   } else {
      // Error handling for failed connection close
   }
}

/*
 * Establish connection with postgres user account to template1 db
 */
bool pg_connect_newDB()
{

}

/*
 * pg_connect_newDB()
 * Create db here
 *    Call pg_user_setup
 *    Call pg_table_setup
 *    Call pg_alter_setup
 */
bool pg_db_setup()
{
}

/*
 * Called from pg_db_setup
 * Create two users: dfAdmin and local_machine
 * dfAdmin has ALL CRUD and local_machine can CRU with limitations
 */
bool pg_user_setup()
{
}

/*
 * Called from pg_db_setup
 * Create three schema: QT, Maintenance, Transactions
 * Set constraints and triggers
 */
bool pg_table_setup()
{
}

/*
 * Called from pg_db_setup
 * Set constraints, Permissions and triggers once all 
 * tables and users exist
 */
bool pg_alter_setup()
{
}

// Dtor
db_setup::~db_setup()
{
}

// Ignore stratchpad for now...will move functions later.
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