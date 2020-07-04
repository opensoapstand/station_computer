#ifndef LOCAL_DB_LOGGER_H
#define LOCAL_DB_LOGGER_H

#include <iostream>
#include <sstream>
#include <string>
#include <cstring>

#include <mysql/mysql.h>

using namespace std;

struct connection_details
{
    char const *server;
    char const *user;
    char const *password;
    char const *database;
};

class local_db_logger
{
public:
    local_db_logger();
    MYSQL* mysql_connection_setup(struct connection_details mysql_connection);
    MYSQL_RES* create_machine_inDB(int machine_Number); //creating the machine into the database

    //inset to local database event logging
    MYSQL_RES* insert_drink_inventory(int option, int machine, double volume);
    MYSQL_RES* insert_temperature(int machine, double temp_top, double temp_bottom);
    MYSQL_RES* insert_waste_level(int machine, int top, int bottom);

    //update local database
    MYSQL_RES* update_machine_option(int machine, int oldOption, int newOption);

    //cloud database management

    //need to work on sychronization to the master DB

private:
    MYSQL* local_Connection;
    connection_details local_db;

    string to_string(double double_value); //use for input type double
    string to_string(int int_value);

    string drink_options[9] = {
        "option1","option2","option3",
        "option4","option5","option6",
        "option7","option8","option9"
    };
};

#endif // LOCAL_DB_LOGGER_H
