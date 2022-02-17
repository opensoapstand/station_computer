/*******************************************************
 // Drinkfill Database Utilities - IMPLEMENTATION
 //
 // db_util.cpp
 // Links to common libraries and tools for data type to 
 // query string conversions.
 //
 // created: 07-09-2020
 // by: Li Yan Tong
 //
 // copyright 2022 by Drinkfill Beverages Ltd
 // all rights reserved
 ********************************************************/

#include "db_utils.h"

using namespace std;
using namespace pqxx;

string db_utils::to_string(double double_value)
{
    std::ostringstream ss;
    ss << double_value;
    return ss.str();
}

string db_utils::to_string(int int_value)
{
    std::ostringstream ss;
    ss << int_value;
    return ss.str();
}

string db_utils::to_string(bool boolean_value)
{
    std::ostringstream ss;
    ss.clear();
    if(boolean_value == true) {
        ss << "TRUE";
    } else {
        ss << "FALSE";
    }
    return ss.str();
}