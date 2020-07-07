/*******************************************************
 // Drinkfill Postgres Master Script For Database Maintenance 
 //
 // df_db_maintenance.sql
 // Common Queries for troubleshooting and backing up Database.
 //
 // created: 02-07-2020
 // by: Li Yan Tong
 //
 // copyright 2020 by Drinkfill Beverages Ltd
 // all rights reserved
 ********************************************************/

-- Postgres is connection based; Need different connections for switching DB.
-- USE drinkfill;

-- Check Contents
SELECT * FROM `inventory`;
SELECT * FROM `machine`;
SELECT * FROM `product`;
SELECT * FROM `temperature_log`;
SELECT * FROM `waste_log`;
SELECT `option2` FROM `machine`;

-- User roles checking
SELECT * FROM pg_roles;

-- Peek all DB's on Server
SELECT *
FROM pg_database;

-- Alter for machine testing
UPDATE `machine` SET `option1` = 4 WHERE `id` = 1;
-- ALTER TABLE `coupon` ADD COLUMN `testing` VARCHAR(120);
-- ALTER TABLE `coupon` DROP COLUMN `testing`
-- EXPLAIN SELECT * FROM `coupon`;

-- Backup
-- User PG Dump all through command line for now
