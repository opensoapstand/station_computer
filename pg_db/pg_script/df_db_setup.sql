/*******************************************************
 // Drinkfill Postgres Master Script For Database Architecture 
 //
 // df_db_setup.sql
 // Create Database and Tables; Should only run on Factory Reset.
 //
 // created: 07-02-2020
 // by: Li Yan Tong
 //
 // copyright 2020 by Drinkfill Beverages Ltd
 // all rights reserved
 ********************************************************/
-- USE postgres USER ACCOUNT CONNECTION to template1
-- List the name of all the databases in this server

-- create database if not exists drinkfill;
DO $$ BEGIN CREATE TEMPORARY VIEW dbchk AS (
    SELECT datname
    FROM pg_database
    WHERE datname = 'drinkfill'
);

IF (
    SELECT COUNT(*)
    FROM dbchk
) < 0 THEN -- Create the Database
CREATE DATABASE drinkfill;

-- Create the Roles
CREATE ROLE df_admin_Group WITH NOSUPERUSER NOCREATEDB;
CREATE ROLE machine_group WITH NOSUPERUSER NOCREATEDB NOCREATEROLE;

-- Future role for customer mobile interaction
-- Future role for maintenance mobile interaction
-- Future role for vendor mobile interaction

-- Change Default schema name
ALTER SCHEMA public
RENAME TO df_transaction;

-- Set permissions to groups
GRANT ALL PRIVILEGES ON DATABASE drinkfill TO df_admin_group;

GRANT CONNECT ON DATABASE drinkfill TO machine_group;

-- Can restrict with product catalog
GRANT SELECT,
    INSERT,
    UPDATE,
    TRUNCATE,
    REFERENCES,
    TRIGGER ON ALL TABLES IN SCHEMA df_transaction to machine_group;

-- TODO: Future permissions required for customer mobile interaction
-- TODO: Future permissions required for maintenance mobile interaction
-- TODO: Future permissions required for vendor mobile interaction

-- Create Users

-- Figure out more secure way to create this user
CREATE USER df WITH PASSWORD 'password1234';

GRANT df_admin_group TO df;

CREATE USER local_machine WITH PASSWORD 'machine1234';

GRANT machine_group TO local_machine;

-- Create User Table
CREATE TABLE IF NOT EXISTS 'user_type' (
  'user_type_id' DISTINCT VARCHAR(3),
  'user_type_description' VARCHAR(100),
    PRIMARY KEY ('user_type_id')
);

INSERT INTO 'user_type' VALUES 
('ADM', 'Administrator Account'),
('MCH', 'Local Machine Account'),
('CUS', 'Customer Account'),
('VED', 'Vendor Account'),
('MTN', 'Maintenance Account');

CREATE TABLE IF NOT EXISTS `user` (
    'id' SERIAL,
    'user_type' VARCHAR(3),
    PRIMARY KEY ('user_type','id'),
    FOREIGN KEY ('user_type') REFERENCES user_type(user_type_id)
);

-- Check for Customer, Vendor and Maintenance type
CREATE TABLE IF NOT EXISTS 'user_contact' (
    'id' SERIAL,
    'user_type' VARCHAR(3),
    'user_first_name' VARCHAR(100),
    'user_last_name' VARCHAR (100),
    'user_email' VARCHAR (50),
    'user_street_address' VARCHAR (255),
    'user_phone' VARCHAR (15),
    'user_country' VARCHAR (50),
    PRIMARY KEY ('user_type','id'),
    FOREIGN KEY ('user_type_id','user_type') REFERENCES user('user_type_id', 'user_type')
);

/*Product holds a local catalog of drinks used in the machine*/
CREATE TABLE IF NOT EXISTS 'product_catalog' (
    'id' SERIAL,
    'name' VARCHAR(100) NOT NULL,
    'vendor' INT DEFAULT NULL,
    -- can move this to QT reference in future 
    `image` BLOB,
    `calibration_const` DOUBLE,
    -- cost from vendor
    `cost_per_litre` DOUBLE,
    'option_slot' INT CHECK(numrange(-1,8)),
    'coupon_code' CHAR(6),
    PRIMARY KEY(`id`),
    FOREIGN KEY (`vendor`) REFERENCES vendors(`id`)
);

/*
Location of the machine
*/
CREATE TABLE IF NOT EXISTS `machine_location` (
    `id` SERIAL,
    `locale_name` VARCHAR (255),
    'street_address' VARCHAR (255),
    'phone' VARCHAR (15),
    'country' VARCHAR (50),
    `on_site_location` VARCHAR(255),
    `user_id_fk` VARCHAR (100),
    -- Check for Vendor Type
    `user_type_fk` VARCHAR (15),
    PRIMARY KEY(`id`),
    FOREIGN KEY('user_id_fk', 'user_type_fk') REFERECES users('id', 'user_type')
);

/*product pricing*/
CREATE TABLE IF NOT EXISTS `pricing` (
    'product_id' int,
    -- cost for consumers
    'price_per_litre' double,
    'location' int,
    PRIMARY KEY (`product_id`),
    FOREIGN KEY (`product_id`) references product(`id`),
    FOREIGN KEY (`location`) references location(`id`)
);

/*
general information about the machine
 TODO grab serials and version from XML
*/
CREATE TABLE IF NOT EXISTS `machine` (
    `id` VARCHAR(255) DISTINCT NOT NULL,
    'type' VARCHAR(25),
    -- I.E keggerator vs full unit to be defined...
    'version' NOT NULL VARCHAR(25),
    -- machine version
    `location_id` int default null,
    'inventory_level_id' int default null,
    `number_of_drinks` int,
    -- Do a count of inventory level
    `maintenance_log_id` timestamp NOT NULL,
    `vendor_host` varchar(100),
    `vendor_provider` varchar(100),
    PRIMARY KEY(`id`,'type','version','location_id'),
    FOREIGN KEY(`location_id`) REFERENCES machine_location(`id`),
    FOREIGN KEY('maintenance_log_id') REFERENCES maintenance_log('id')
);

CREATE TABLE IF NOT EXISTS 'maintenance_log' (
    'id' SERIAL,
    -- 'maintenance_user' reference needed...
    'maintenance_code' VARCHAR(3),
    -- Create contraint or table for this.
    `last_maintenance` timestamp NOT NULL,
    'description' TEXT,
    PRIMARY KEY('id')
    )

CREATE TABLE IF NOT EXISTS `inventory_level` (
    `id` SERIAL,
    `product_id` INT NOT NULL,
    -- every time when there is a log, there should be coresponding id for what product and which machine
    `machine_id` INT NOT NULL,
    `volume` double default null,
    'refresh_date' DATETIME NOT NULL,
    PRIMARY KEY ('id') REFERENCES
    FOREIGN KEY (`product_id`) REFERENCES product(`id`),
    FOREIGN KEY (`machine_id`) REFERENCES machine(`id`)
);

create table if not exists `temperature_log` (
    `id` int primary key not null auto_increment,
    `machine_id` int not null,
    `top` double default null,
    `bottom` double default null,
    foreign key (`machine_id`) references machine(`id`)
);

create table if not exists `waste_log` (
    `id` int primary key not null auto_increment,
    `machine_id` int not null,
    `top` int default null,
    `bottom` int default null,
    foreign key (`machine_id`) references machine(`id`)
);

/*Daily update of the revenue from the machine, get summarized revenue daily*/
/*can utilize this table to display histogram of the progress*/
-- create table if not exists `revenue` (`day` DATE NOT NULL,
-- 									  `number_transactions` int,
--                                       `daily_revenue` double,
--                                       `machine_id` serial NOT NULL,
--                                       FOREIGN KEY (`machine_id`) REFERENCES machines(id)
-- 									  );
/*this table info have all the infomation need to be use to generate reciepts*/
/*refer to Appendix E: Transaction Data Field Identifiers from moneris doc*/
/*or simply reduce the security risk by........................................*/
-- create table if not exists `sales` (`id` int,
-- 									`machine_id` int,
--                                     `spdh_transaction_code` varchar (10), -- SPDH transaction type code (e.g. Purchase = “00”)
--                                     `date` datetime not null,
--                                     `amount` varchar(10),
--                                     `pan` varchar(20), -- mask pan data
--                                     `iso` varchar(10), -- iso responose code
--                                     `spdh_response` varchar(10), 
--                                     `approval_number` varchar(10),
--                                     `sequence_number` varchar(50),
--                                     `language` ENUM ('E', 'F'),
--                                     `mop` varchar(10),  -- method of payment (tapped [EMV, MSD, TOKEN)
-- 									`account_type` varchar(15),
--                                     `aid` varchar (30), -- chip application 
--                                     `label` varchar (15), -- chip application
-- 									`card_name` varchar(10) not null,
-- 									`txn_type` varchar(10) not null, -- transaction type
--                                     `tvr_21` varchar(15), -- TVR value after ARQC
--                                     `tvr_22` varchar(15) , -- TVR value after ARQC
--                                     `tsi` varchar(15),
-- 									`disposition` varchar(50),                                    
--                                     FOREIGN KEY (`machine_id`) REFERENCES machine(`id`));
CREATE TABLE IF NOT EXISTS `sales` (
    `id` INT NOT NULL auto_increment,
    `machine_id` INT,
    `date` DATETIME NOT NULL,
    `amount` VARCHAR(10),
    `pan` VARCHAR(20),
    -- mask pan data
    `reference` VARCHAR(30),
    PRIMARY KEY (`id`),
    FOREIGN KEY (`machine_id`) REFERENCES machine(`id`)
);

/*future implementation*/
/*coupon code -> active time with 6 characters*/
CREATE TABLE IF NOT EXISTS `coupon` (
    `id` INT PRIMARY KEY,
    `code` CHAR(6),
    `machine_id` INT,
    `percentage` INT,
    `start_date` TIMESTAMP,
    `end_date` TIMESTAMP,
    UNIQUE(`code`),
    FOREIGN KEY(`machine_id`) references machine(`id`)
);

END IF;

END;
$$ 

-- Teardown
DO $$ DROP DATABASE IF EXISTS drinkfill;

DROP USER localmachine;

DROP USER df;

DROP GROUP dfAdminGroup;

DROP GROUP machineGroup;

DROP TABLE IF EXISTS inventory;

DROP TABLE IF EXISTS coupon;

DROP TABLE IF EXISTS location;

DROP TABLE IF EXISTS machine;

DROP TABLE IF EXISTS pricing;

DROP TABLE IF EXISTS product;

DROP TABLE IF EXISTS sales;

DROP TABLE IF EXISTS vendors;

DROP TABLE IF EXISTS temperature_log;

DROP TABLE IF EXISTS waste_log;

END IF;

END;

$$