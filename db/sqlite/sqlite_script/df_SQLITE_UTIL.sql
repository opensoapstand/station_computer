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

// Select Query for Inventory list Object in FSM

SELECT 
inventory.inventory_id, 
product.product_id, 
product.name, 
product.calibration_const,
pricing.small_price,
pricing.large_price
FROM
inventory
INNER JOIN product ON inventory.product_id = product.product_id
INNER JOIN pricing ON inventory.product_id = pricing.product_id
WHERE inventory.inventory_id = 0;


SELECT * FROM vendor WHERE vendor_id = 1;