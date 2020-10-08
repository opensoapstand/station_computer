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

select
    *
from
    `inventory`;

select
    *
from
    `machine`;

select
    *
from
    `product`;

select
    *
from
    `temperature_log`;

select
    *
from
    `waste_log`;

select
    `option2`
from
    `machine`;

update
    `machine`
set
    `option1` = 4
where
    `id` = 1;

/ /
Select
    Query for Inventory list Object in FSM
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
WHERE
    inventory.inventory_id = 0;

SELECT
    *
FROM
    vendor
WHERE
    vendor_id = 1;

UPDATE
    inventory
SET
    product_id = 4,
    volume = 18900,
    date_refresh = 2020 -10 -30 24 :00 :00 in_stock = 1,
    has_pump = 0
WHERE
    inventory_id = 1;

/* Empty product for no inventory FK check 
 TODO: Initialize with SEED...
 */
UPDATE
    product
SET
    product_id = 999999
WHERE
    product_id = 8;

UPDATE
    inventory
SET
    product_id = 999999,
    volume = 0,
    date_refresh = 2000 -01 -01 24 :00 :00,
    in_stock = 0,
    has_pump = 0
WHERE
    inventory_id = 2;

UPDATE
    inventory
SET
    product_id = 4,
    volume = 18900,
    date_refresh = 2020 -10 -30 24 :00 :00,
    in_stock = 1,
    has_pump = 0
WHERE
    inventory_id = 1;

