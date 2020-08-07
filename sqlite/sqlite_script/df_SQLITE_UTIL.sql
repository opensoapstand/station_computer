/*******************************************************
// Drinkfill SQLite UTIL Script 
//
// df_SQLITE_UTIL.sql
// Common SQL queries required for for Pilot at BNTL
//
// created: 30-07-2020
// by: Li-Yan Tong
//
// copyright 2020 by Drinkfill Beverages Ltd
// all rights reserved
 ********************************************************/

Use drinkfill;
select * from `inventory`;
select * from `machine`;
select * from `product`;
select * from `temperature_log`;
select * from `waste_log`;
select `option2` from `machine`;
update `machine` set `option1` = 4 where `id` = 1;


SELECT * FROM vendor WHERE vendor_id = 1;