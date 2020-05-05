#include "local_db_logger.h"

local_db_logger::local_db_logger()
{
    local_db.server = "localhost";
    local_db.user = "drinkfill";
    local_db.password = "0000";
    local_db.database = "drinkfill";

    local_Connection = mysql_connection_setup(local_db);

    create_machine_inDB();
}

MYSQL *local_db_logger::mysql_connection_setup(connection_details mysql_connection)
{
    // first of all create a mysql instance and initialize the variables within
   MYSQL *connection = mysql_init(NULL);

   // connect to the database with the details attached.
   if (!mysql_real_connect(connection, mysql_connection.server, mysql_connection.user, mysql_connection.password, mysql_connection.database, 0, NULL, 0)) {
     printf("Conection error : %s\n", mysql_error(connection));

     exit(1);
   }
   else
       std::cout<< "I compiled alright with the proper compile tag `mysql_config --cflags --libs`" << std::endl;

   return connection;
}

MYSQL_RES *local_db_logger::update_drink_inventory_local(int option, double volume)
{
    //int to char* conversion
    string optionStr = to_string(option);

    //double to char* conversion
    string volumeStr = to_string(volume);

    /*sample inventory insertion*/
    string qryString = string("insert into `inventory` (`machine_id`, `volume`) value(") + optionStr + "," + volumeStr + ") "
                              "on duplicate key update `volume` = " + volumeStr + ";";
    int n = qryString.length();

    char qryChar_Array[n+1]; //allocate proper size for qry script
    strcpy(qryChar_Array, qryString.c_str());

    // send the query to the database
    if (mysql_query(local_Connection, qryChar_Array))
    {
       printf("MySQL query error : %s\n", mysql_error(local_Connection));
       exit(1);
    }
    else
    {
        cout << "drink inventroy is updated with option: " << optionStr << " volume: " << volumeStr << endl;
    }
    return mysql_use_result(local_Connection);
}

MYSQL_RES *local_db_logger::create_machine_inDB()
{
    string machineNumStr = to_string(machine_Number);
    string qryString = string("insert into `machine` (`id`, `last_maintenance`) value(") + machineNumStr + ", now())"
                                                       + "on duplicate key update `id` = " + machineNumStr + ";";

    int n = qryString.length();

    char qryChar_Array[n+1]; //allocate proper size for qry script
    strcpy(qryChar_Array, qryString.c_str());

    // send the query to the database
    if (mysql_query(local_Connection, qryChar_Array))
    {
       printf("MySQL query error : %s\n", mysql_error(local_Connection));
       exit(1);
    }
    else
    {
        cout << "drink inventroy is updated with option: " << "1" << endl;
    }

    return mysql_use_result(local_Connection);
}

string local_db_logger::to_string(double double_value)
{
    std::ostringstream ss;
    ss << double_value;
    return ss.str();
}

string local_db_logger::to_string(int int_value)
{
    std::ostringstream ss;
    ss << int_value;
    return ss.str();
}
