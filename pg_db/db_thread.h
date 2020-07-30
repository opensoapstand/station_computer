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

#include "./pg_util/db_utils.h"
#include "./db_setup/db_setup.h"

using namespace std;
using namespace pqxx;

class db_thread
{
public:
    // Ctor
    db_thread();
    // Dtor
    ~db_thread();

    // Intepret commands from FSM
    database_result db_command_parse();
    database_result db_command_switch();

    // Permission/User Checks
    connection_details pg_connections();
    // bool pg_connect_machine();
    // bool pg_connect_admin();

    // Machine Functions
    database_result insert_sale(struct connection_details);
    database_result update_inventory(struct connection_details);

    // Admin Functions
    database_result check_product(struct connection_details);
    database_result insert_product(struct connection_details);
    database_result update_product(struct connection_details);
    database_result insert_new_inventory(struct connection_details);
    database_result update_existing_inventory(struct connection_details);

private:
    connection_details db_connection;
};

#endif // LOCAL_DB_LOGGER_H