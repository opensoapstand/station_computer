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


/*
 * Establish connection with postgres user account to template1 db
 */
database_result pg_connect_newDB(connection_details *db_conn)
{
   database_result pg_connect_result = {
       ERROR_TRANSACTION_UNDEFINED,
       ERROR_CREATE_DB_UNDEFINED,
   };

   cout << "start connection" << endl;
   try
   {
      // WILL NEED TO STORE AS SECRET!  connection as template1 required if db from stratch.
      string connection_string("hostname=localhost dbname = drinkfill user = local_machine password = machine1234 port=5432");
      db_conn->local_connection = pqxx::connection(connection_string.c_str());
      // pqxx::connection C(connection_string.c_str());

      if (db_conn->local_connection.is_open())
      {
         cout << "Opened database successfully: " << db_conn->local_connection.dbname() << db_conn->local_connection.username() << endl;
         db_conn->DB_User_code = POSTGRES_SU;
      }
      else
      {
         cout << "Can't open database" << endl;
      }
   }
   catch (const std::exception &e)
   {
      pg_connect_result.DB_setup_code = ERROR_CREATE_CONNECTION_FAULT;
      pg_connect_result.error_message = e.what();
      cerr << e.what() << std::endl;
   }
   return pg_connect_result;
}

/*
 * Called from pg_db_setup
 * Create two users: dfAdmin and local_machine
 * dfAdmin has ALL CRUD and local_machine can CRU with limitations
 */
void pg_user_setup(pqxx::connection *conn, database_result *pg_setup_result)
{
   string df_admin_create_group;
   string df_admin_permission;
   string machine_create_group;

   try
   {
      df_admin_create_group = "CREATE ROLE df_admin_Group WITH NOSUPERUSER NOCREATEDB";
      machine_create_group = "CREATE ROLE machine_group WITH NOSUPERUSER NOCREATEDB NOCREATEROLE;";

      /* Create a transactional object. */
      work W(*conn);

      /* Execute SQL query */
      W.exec(df_admin_create_group);
      W.commit();
      cout << "Table created successfully" << endl;
   }
   catch (const std::exception &e)
   {
      cerr << e.what() << std::endl;
      return;
   }
}

/*
 * Called from pg_db_setup
 * Create three schema: QT, Maintenance, Transactions
 * Set constraints and triggers
 */
void pg_table_setup(pqxx::connection *conn, database_result *pg_setup_result)
{
}

/*
 * Called from pg_db_setup
 * Set constraints, Permissions and triggers once all 
 * tables and users exist
 */
void pg_alter_setup(pqxx::connection *conn, database_result *pg_setup_result)
{
}

/*
 * Called from pg_db_setup
 * Set constraints, Permissions and triggers once all 
 * tables and users exist
 */
database_result pg_close_setup(connection_details *db_conn)
{
   database_result pg_connect_result = {
       ERROR_TRANSACTION_UNDEFINED,
       ERROR_CREATE_DB_UNDEFINED,
   };

   try
   {
      if (db_conn->local_connection.is_open())
      {
         cout << "Opened database successfully: " << db_conn->local_connection.dbname() << endl;

         cout << "NOW CLOSING " << db_conn->local_connection.dbname() << endl;
         db_conn->DB_User_code = NO_USER;
         db_conn->local_connection.close();
      }
      else
      {
         cout << "Can't open database" << endl;
      }
   }
   catch (const std::exception &e)
   {
      pg_connect_result.DB_setup_code = ERROR_CREATE_CONNECTION_FAULT;
      pg_connect_result.error_message = e.what();
      cerr << e.what() << std::endl;
   }
   return pg_connect_result;
}

/*
 * pg_connect_newDB()
 * Create db here
 *    Call pg_user_setup
 *    Call pg_table_setup
 *    Call pg_alter_setup
 */
database_result pg_db_setup(connection_details *db_conn)
{

   string dbName = "drinkfill";

   database_result pg_create_result = {
       ERROR_MACHINE_CONNECTION_FAULT,
       ERROR_CREATE_DB_UNDEFINED,
   };

   try
   {
      connection *C = &db_conn->local_connection;

      if (C->is_open())
      {
         std::cout << "Opened database successfully: " << C->dbname() << endl;
         pqxx::nontransaction w(*C);
         try
         {
            w.exec("CREATE DATABASE " + dbName);
            pg_user_setup(C, &pg_create_result);
            // pg_table_setup(db_conn, &pg_create_result);
            // pg_alter_setup(db_conn, &pg_create_result);
         }
         catch (pqxx::sql_error &e)
         {
            string sqlErrCode = e.sqlstate();
            if (sqlErrCode == "42P04")
            { // catch duplicate_database
               std::cout << "Database: " << dbName << " exists, proceeding further\n";
               pg_create_result.DB_setup_code = ERROR_CREATE_DUPLICATE_DB_FAULT;
            }
            std::cerr << "Database error: " << e.what()
                      << ", error code: " << e.sqlstate()
                      << "SQL Query was: " << e.query() << "\n";
            w.abort();
         }
      }
      else
      {
         std::cout << "Can't open database" << endl;
      }
   }
   catch (const std::exception &e)
   {
      cerr << e.what() << std::endl;
   }
   return pg_create_result;
}

db_setup::db_setup()
{
   // db_setup_result = pg_connect_newDB(db_setup_connection);

   // if (db_setup_result.DB_setup_code != ERROR_CREATE_CONNECTION_FAULT)
   // {
   //    db_setup_result = pg_db_setup(db_setup_connection);
   // }
   // else
   // {
   //    // FSM Error handling for failed connection
   // }

   // if (db_setup_result.DB_setup_code == DB_CREATE_SUCESS)
   // {
   // }
   // else
   // {
   // }

   // db_setup_result = pg_close_setup(db_setup_connection);
}

// Dtor
db_setup::~db_setup()
{
}

//Ignore stratchpad for now...will move functions later.
int main(int argc, char *argv[])
{
   db_setup *db = new db_setup();
   cout << db->db_setup_connection.DB_User_code << endl;

   // connection_details *dbConn = db.db_setup_connection.local_connection;
   connection_details *dbConn = &db->db_setup_connection;
   database_result dbResult = db->db_setup_result;

   dbResult = pg_connect_newDB(dbConn);
   dbResult = pg_close_setup(dbConn);

   // cout << db->db_setup_connection.local_connection->is_open() << endl;

   // db->db_setup_connection.local_connection->close();

   // cout << db->db_setup_connection.local_connection->is_open() << endl;


   // if (db_setup_result.DB_setup_code != ERROR_CREATE_CONNECTION_FAULT)
   // {
   //    db_setup_result = pg_db_setup(db_setup_connection);
   // }
   // else
   // {
   //    // FSM Error handling for failed connection
   // }

   // if (db_setup_result.DB_setup_code == DB_CREATE_SUCESS)
   // {
   // }
   // else
   // {
   // }

   // db_setup_result = pg_close_setup(db_setup_connection);

   return 0;
}