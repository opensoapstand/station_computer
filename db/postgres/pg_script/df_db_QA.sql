/*******************************************************
 // Drinkfill Postgres Quality Assurance 
 //
 // df_db_operations.sql
 // Typical function calls on database
 //
 // created: 07-08-2020
 // by: Li Yan Tong
 //
 // copyright 2022 by Drinkfill Beverages Ltd
 // all rights reserved
 ********************************************************/

-- TEST inventory restriction
-- Machine cannot insert
UPDATE df_transaction.inventory
SET in_stock = TRUE
WHERE inventory_id = 0;

-- DO NOT USE ASSERT!  only for debugging
-- RAISE can be used...will look into unit testing options later...

do $$ begin INSERT INTO df_transaction.inventory
VALUES (99, NULL, NULL, current_timestamp, FALSE);

end;
$$ LANGUAGE plpgsql;