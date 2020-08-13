/*******************************************************
 // Drinkfill SQLDatabase initialization - IMPLEMENTATION
 //
 // sql_db.cpp
 // Create database, users, permissions and tables.
 // PROTOTYPE FOR BNTL SITE LAUNCH VERY BARE BONES!
 //
 // created: 07-30-2020
 // by: Li Yan Tong
 //
 // copyright 2020 by Drinkfill Beverages Ltd
 // all rights reserved
 ********************************************************/

#include <stdio.h>
#include <sqlite3.h> 

int main(int argc, char* argv[]) {

   sqlite3 *db;
   char *zErrMsg = 0;
   int rc;

   rc = sqlite3_open("drinkfill-sqlite.db", &db);

   if( rc ) {
      fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
      return(0);
   } else {
      fprintf(stderr, "Opened database successfully\n");
   }

   sqlite3_close(db);
}