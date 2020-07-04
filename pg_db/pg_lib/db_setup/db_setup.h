#include "df_db.h"

class local_db_setup
{
public:
    local_db_setup();
    MYSQL* mysql_connection_setup(struct connection_details mysql_connection);
    MYSQL_RES* create_machine_inDB(int machine_Number);


    create_users();
    MYSQL_RES* create_tables();

    //cloud database management

    //need to work on sychronization to the master DB

private:
    connection temp_local_Connection;
    connection_details temp_local_db;

    string to_string(double double_value); //use for input type double
    string to_string(int int_value);
};
