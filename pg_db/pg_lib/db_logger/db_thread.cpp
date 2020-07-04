#include "pg_db.h"

db_thread::db_thread()
{

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

MYSQL_RES *local_db_logger::insert_drink_inventory(int option, int machine, double volume)
{
    //int to char* conversion
    string optionStr = to_string(option);
    string machineStr = to_string(machine);

    //double to char* conversion
    string volumeStr = to_string(volume);

    /*sample inventory insertion*/
    string qryString = string("insert into `inventory` (`product_id`, `machine_id`, `volume`) value(") + optionStr + "," +
                              machineStr + "," + volumeStr + ");";
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

MYSQL_RES* local_db_logger::insert_temperature(int machine, double temp_top, double temp_bottom)
{

    //int to char* conversion
    string machineStr = to_string(machine);

    //double to char* conversion
    string topStr = to_string(temp_top);
    string bottomStr = to_string(temp_bottom);

    /*sample inventory insertion*/
    string qryString = string("insert into `temperature_log` (`machine_id`, `top`, `bottom`) value(") + machineStr + "," +
                              topStr + "," + bottomStr + ");";
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
        cout << "Temperature Log is updated with top: " << topStr << " bottom: " << bottomStr << endl;
    }
    return mysql_use_result(local_Connection);
}

MYSQL_RES* local_db_logger::insert_waste_level(int machine, int top, int bottom)
{

    //int to char* conversion
    string machineStr = to_string(machine);
    string topStr = to_string(top);
    string bottomStr = to_string(bottom);

    /*sample inventory insertion*/
    string qryString = string("insert into `waste_log` (`machine_id`, `top`, `bottom`) value(") + machineStr + "," +
                              topStr + "," + bottomStr + ");";
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
        cout << "Waste Log is updated with top: " << topStr << " bottom: " << bottomStr << endl;
    }
    return mysql_use_result(local_Connection);
}

MYSQL_RES *local_db_logger::update_machine_option(int machine, int oldOption, int newOption)
{
    //int to char* conversion
    string machineStr = to_string(machine);
    string oldStr = drink_options[oldOption-1];
    string newStr = to_string(newOption);

    /*sample machine option update*/
    string qryString = string("update `machine` set `") + oldStr + "` = " +
                              newStr + " where `id` =  " + machineStr + ";";
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
        cout << "machine " << oldStr << " is updated with product: " << newStr << endl;
    }
    return mysql_use_result(local_Connection);
}

MYSQL_RES* local_db_logger::create_machine_inDB(int machine_Number)
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
        cout << "machine is updated with machine number: " << machineNumStr << endl;
    }

    return mysql_use_result(local_Connection);
}

