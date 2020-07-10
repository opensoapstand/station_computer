/*******************************************************
 // Drinkfill Database thread listener - IMPLEMENTATION
 //
 // db_thread.cpp
 // Listen for function calls from FSM; Route and execute
 // CRUD on DB.
 //
 // created: 07-09-2020
 // by: Li Yan Tong
 //
 // copyright 2020 by Drinkfill Beverages Ltd
 // all rights reserved
 ********************************************************/

#include "db_thread.h"

/*
 * Ctor: Checks if DB Exists and initiates all connections
 */
db_thread::db_thread()
{
    pg_connections();
}

/*
 * Take FSM command; Parse.
 */
bool db_command_parse(string command_string)
{
    
}

/*
 * Execute CRUD based on FSM command; called from infinite loop in main
 */
bool db_command_switch(char parse_char)
{
    // Will fill with ENUMS later...
    switch (parse_char)
    {
    case 0:
        // insert_product()
        // insert_new_inventory()
        // insert_sale()
        break;
    case 1:
        // update_product()
        // update_existing_inventory()
        break;
    case 2:
        // Database Creation here
        break;
    case 3:
        // Database Maintenance here
        break;
    case 4:
        //
        break;
    case ',':
        // Skip
        break;
    }
}

// ******** Machine Functions ********

/*
 * Insert a new row for a completed sale
 */
bool insert_sale()
{
}

/*
 * Find row of selected drink; Deduct from total amount
 * Call the maintenance function for i/o
 */
bool update_inventory()
{
}

// ******** Admin Functions ********

/*
 * Check for existing product through Query
 */
bool product_check()
{
}

/*
 * Do product_check()
 * If Exists GO TO update_product().
 * Else insert new row with information.
 */
bool insert_product()
{
}

/*
 * Do product_check()
 * If Exists update_product().
 * Else FAIL error with insert notification.
 */
bool update_product()
{
}

/*
* Do product_check()
*   IF False then Fails and request insert_product()
*   Else continue
* IS inventory slot empty? (-1 option slot int value check)
*   If False then fails Slot not available
*   else inserts new row with inventory information
*/
bool insert_new_inventory()
{
}

/*
* Do product_check()
*   IF False then Fails and request insert_product()
*   Else continue
* IS inventory slot empty? (-1 option slot int value check)
*   If False then fails Slot not available
*   else inserts new row with inventory information
*/
bool update_existing_inventory()
{
}

int main(int argc, char *argv[])
{
    // TODO: Integrate FSM mutex.  Fails out on
    // FSM Thread pipes character commands here
    while (1)
    {
        // Call db_command_parse()
    }
}