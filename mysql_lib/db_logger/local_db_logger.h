#ifndef LOCAL_DB_LOGGER_H
#define LOCAL_DB_LOGGER_H

#include <iostream>
#include <sstream>
#include <string>
#include <cstring>

#include <mysql/mysql.h>

struct connection_details
{
    char const *server;
    char const *user;
    char const *password;
    char const *database;
};

using namespace std;

class local_db_logger
{
public:
    local_db_logger();
    MYSQL* mysql_connection_setup(struct connection_details mysql_connection);

    //local database event logging
    MYSQL_RES* update_drink_inventory_local(int option, double volume);

private:
    MYSQL_RES *create_machine_inDB();

    MYSQL* local_Connection;
    connection_details local_db;

    string to_string(double double_value); //use for input type double
    string to_string(int int_value);

    int number_of_Drinks = 9; //ideally this should be input for every machine for very first time,
    int machine_Number = 1;
};

#endif // LOCAL_DB_LOGGER_H
