/*******************************************************
 // Drinkfill Database library reference - HEADER
 //
 // df_db.h
 // Reference to libraries.
 //
 // created: 07-09-2020
 // by: Li Yan Tong
 //
 // copyright 2020 by Drinkfill Beverages Ltd
 // all rights reserved
 ********************************************************/

#ifndef DF_DB_H
#define DF_DB_H

#include <iostream>
#include <sstream>
#include <string>
#include <cstring>

#include <pqxx/pqxx>

struct connection_details
{
    char const *connection_type;
    // TODO:set up 3 admin, machine, postgres
    pqxx::connection *local_connection;
};

#endif