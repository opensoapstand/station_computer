/*******************************************************
 // Drinkfill Database thread listener - HEADER
 //
 // db_thread.h
 // Listen for function calls from FSM; Route and execute
 // CRUD on DB.
 //
 // created: 07-09-2020
 // by: Li Yan Tong
 //
 // copyright 2020 by Drinkfill Beverages Ltd
 // all rights reserved
 ********************************************************/

#ifndef DB_THREAD_H
#define DB_THREAD_H

#include "../pg_util/db_utils.h"

class db_thread
{
public:
    // Ctor
    db_thread();

    // Permission/User Checks
    bool pg_connect_machine();
    bool pg_connect_admin();

    // Machine Functions
    bool insert_sale();

    // Admin Functions
    bool insert_drink_inventory();
    bool update_machine_option();

    // Dtor
    ~db_thread();

private:
    connection_details db_connection;
};

#endif // LOCAL_DB_LOGGER_H