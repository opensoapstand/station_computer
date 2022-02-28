/*******************************************************
 // Drinkfill Database Utilities - HEADER
 //
 // db_util.h
 // Common libraries and data storage for db queries
 //
 // created: 07-09-2020
 // by: Li Yan Tong
 //
 // copyright 2022 by Drinkfill Beverages Ltd
 // all rights reserved
 ********************************************************/

#ifndef DB_UTIL_H
#define DB_UTIL_H

#include "df_db.h"

class db_utils
{
public:
    // Data type to Query string conversions
    std::string to_string(double double_value); //use for input type double
    std::string to_string(int int_value);
    std::string to_string(bool boolean_value);
};

#endif // DB_UTIL_H