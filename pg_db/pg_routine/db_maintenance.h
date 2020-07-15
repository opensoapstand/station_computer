/*******************************************************
 // Drinkfill Database Maintenance - HEADER
 //
 // db_maintenance.h
 // Backup and Synchronization functionality.
 //
 // created: 07-09-2020
 // by: Li Yan Tong
 //
 // copyright 2020 by Drinkfill Beverages Ltd
 // all rights reserved
 ********************************************************/

#ifndef DB_MAINTENANCE_H
#define DB_MAINTENANCE_H

#include "../pg_util/db_utils.h"

class db_utils
{
public: 
    // Maintenance Functions
    bool insert_maintenance_log();
    bool insert_temperature(int machine, double temp_top, double temp_bottom);
    bool insert_waste_level(int machine, int top, int bottom);

    // TODO: cloud database management

    // TODO: Web master DB synchronization
};

#endif // DB_UTIL_H

