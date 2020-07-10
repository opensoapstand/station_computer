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
struct {

};

class db_setup
{
public:
    // Ctor
    db_setup();
    // Dtor
    ~db_setup();
    
    // Connection
    bool pg_connect_newDB();
    // Create Database, Groups, Users and Tables
    bool pg_db_setup(struct connection_details);
    bool pg_user_setup();
    bool pg_table_setup();
    bool pg_alter_setup();
    bool pg_close_setup(struct connection_details);


private:
    connection_details db_setup_connection;
};

#endif // DB_SETUP_H