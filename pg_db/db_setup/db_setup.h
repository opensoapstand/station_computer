/*******************************************************
 // Drinkfill Database thread listener - HEADER
 //
 // db_setup.h
 // Create database, users, permissions and tables.
 //
 // created: 07-09-2020
 // by: Li Yan Tong
 //
 // copyright 2020 by Drinkfill Beverages Ltd
 // all rights reserved
 ********************************************************/

#ifndef DB_SETUP_H
#define DB_SETUP_H

#include "../pg_util/db_utils.h"

// Define struct that matches tiny XML machine values
// struct {

// };

class db_setup
{
public:
    // Ctor
    db_setup();
    // Connection
    database_result pg_connect_newDB(connection_details *db_conn);
    // Create Database, Groups, Users and Tables
    void pg_user_setup(pqxx::connection *conn, database_result *pg_setup_result);
    void pg_table_setup(pqxx::connection *conn, database_result *pg_setup_result);
    void pg_alter_setup(pqxx::connection *conn, database_result *pg_setup_result);
    database_result pg_db_setup(connection_details *db_conn);
    database_result pg_close_setup(connection_details *db_conn);
    // Dtor
    ~db_setup();
    connection_details db_setup_connection;
    database_result db_setup_result;

private:
};

#endif // DB_SETUP_H