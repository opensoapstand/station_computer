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

#include "../../fsm/dftypes.h"

typedef enum DF_DB_USER_TYPE
{
   POSTGRES_SU = 0,
   DFADMIN,
   LOCALMACHINE
} DF_DB_USER_TYPE;

typedef enum DF_DB_TRANSACTION
{
   DB_TRANSACTION_SUCESS = 0,
   ERROR_MACHINE_CONNECTION_FAULT,
   ERROR_MACHINE_SEARCH_FAULT,
   ERROR_MACHINE_INSERT_FAULT,
   ERROR_MACHINE_UPDATE_FAULT,
   ERROR_ADMIN_CONNECTION_FAULT,
   ERROR_ADMIN_SEARCH_FAULT,
   ERROR_ADMIN_INSERT_FAULT,
   ERROR_ADMIN_UPDATE_FAULT,
} DF_DB_TRANSACTION;

typedef enum DF_DB_CREATION
{
   DB_CREATE_SUCESS = 0,
   ERROR_CREATE_CONNECTION_FAULT,
   ERROR_CREATE_USER_FAULT,
   ERROR_CREATE_TABLE_FAULT,
} DF_DB_CREATION;

struct database_result
{
   DF_DB_TRANSACTION DB_transaction_code;
   DF_DB_CREATION DB_setup_code;
   std::exception error_message;
   // pqxx::failure error_message;
};

struct connection_details
{
   DF_DB_USER_TYPE DB_User_code;
   // TODO:set up 3 admin, machine, postgres
   pqxx::connection *local_connection;
};

#endif