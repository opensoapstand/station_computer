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
/* 
 * AWS may require '' EG: CREATE TABLE IF NOT EXISTS 'df_transaction.user' ...
 * USE postgres USER ACCOUNT CONNECTION to template1
 * List the name of all the databases in this server
*/

/*
 * Procedural Programming for creating a drinkfill Database
*/
DO $$ BEGIN CREATE TEMPORARY VIEW dbchk AS (
    SELECT datname
    FROM pg_database
    WHERE datname = 'drinkfill'
);

IF (
    SELECT COUNT(*)
    FROM dbchk
) < 0 THEN -- Create the Database

-- create database if not exists drinkfill;
CREATE DATABASE drinkfill;

-- todo set up admin account
-- Create the Roles
CREATE ROLE df_admin_Group WITH NOSUPERUSER NOCREATEDB;

CREATE ROLE machine_group WITH NOSUPERUSER NOCREATEDB NOCREATEROLE;

-- Future role for customer mobile interaction
-- Future role for maintenance mobile interaction
-- Future role for vendor mobile interaction
-- Change Default schema name
--ALTER DATABASE drinkfill OWNER TO df;
--ALTER DATABASE drinkfill OWNER TO postgres;
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

/*
 * Create MAINTENANCE data storage
*/

CREATE SCHEMA IF NOT EXISTS df_maintenance;

-- Tables will need machine_id FK for Web.  Do not need reference to machine_id locally.

-- Log and describe maintenance performed
CREATE TABLE IF NOT EXISTS df_maintenance.master_log (
    master_log_id SERIAL,
    -- 'maintenance_user' reference needed...
    maintenance_code VARCHAR(10),
    -- Create contraint or table for this.
    description TEXT,
    last_maintenance TIMESTAMP WITH TIME ZONE NOT NULL,
    PRIMARY KEY(master_log_id)
);

-- Log and describe temperature
CREATE TABLE IF NOT EXISTS df_maintenance.temperature_log (
    temperature_log_id SERIAL,
    -- Sensors in Degrees Celcius
    top_sensor_C DECIMAL(2) DEFAULT NULL,
    bottom_sensor_C DECIMAL(2) DEFAULT NULL,
    temperature_timestamp TIMESTAMP WITH TIME ZONE NOT NULL,
    PRIMARY KEY(temperature_log_id)
);

-- Log and provide warning switch for waste overflow
CREATE TABLE IF NOT EXISTS df_maintenance.waste_log (
    waste_log_id SERIAL,
    -- Sensors are level sensors for liquid overspill in waste
    top_failure_sensor BOOLEAN default FALSE,
    bottom_warning_sensor BOOLEAN default FALSE,
    waste_timestamp TIMESTAMP WITH TIME ZONE NOT NULL,
    PRIMARY KEY(waste_log_id)
);

-- Log and describe dispensed liquids
CREATE TABLE IF NOT EXISTS df_maintenance.flow_io_log (
    flow_io_log_id SERIAL,
    flow_sensor_slot INT CHECK(flow_sensor_slot > 0 AND flow_sensor_slot < 8),
    -- 0 to 8 are sensors 1 to 9 respectively.
    flow_contents TEXT,
    flow_amount DECIMAL(2) DEFAULT 0.00,
    flow_timestamp TIMESTAMP WITH TIME ZONE NOT NULL,
    PRIMARY KEY(flow_io_log_id)
);

/*
 * Change default schema for sales transaction data
*/

ALTER SCHEMA public
RENAME TO df_transaction;

-- Create User Table
CREATE TABLE IF NOT EXISTS df_transaction.user_type (
    user_type_id VARCHAR(3) UNIQUE,
    user_type_description VARCHAR(100),
    PRIMARY KEY(user_type_id)
);

INSERT INTO df_transaction.user_type
VALUES ('ADM', 'Administrator Account'),
    ('MCH', 'Local Machine Account'),
    ('CUS', 'Customer Account'),
    ('VED', 'Vendor Account'),
    ('MTN', 'Maintenance Account');

CREATE TABLE IF NOT EXISTS df_transaction.user (
    user_id SERIAL,
    user_type VARCHAR(3),
    PRIMARY KEY(user_type, user_id),
    FOREIGN KEY(user_type) REFERENCES df_transaction.user_type(user_type_id)
);

-- Check for Customer, Vendor and Maintenance type
CREATE TABLE IF NOT EXISTS df_transaction.user_contact (
    user_contact_id INT NOT NULL,
    user_type VARCHAR(3),
    user_first_name VARCHAR(100),
    user_last_name VARCHAR(100),
    user_email VARCHAR(50),
    user_street_address VARCHAR(255),
    user_phone VARCHAR(15),
    user_country VARCHAR(50),
    PRIMARY KEY(user_contact_id),
    FOREIGN KEY(user_contact_id, user_type) REFERENCES df_transaction.user(user_id, user_type)
);

/*
 * Location of the machine
 */
CREATE TABLE IF NOT EXISTS df_transaction.machine_location (
    machine_location_id SERIAL,
    locale_name VARCHAR(255),
    street_address VARCHAR(255),
    phone VARCHAR(15),
    country VARCHAR(50),
    on_site_location VARCHAR(255),
    user_id_fk INT,
    -- Check for Vendor Type
    user_type_fk VARCHAR(15),
    PRIMARY KEY(machine_location_id),
    FOREIGN KEY(user_id_fk, user_type_fk) REFERENCES df_transaction.user(user_id, user_type)
);

/*
 *  Sales by location of machine
 */
CREATE TABLE IF NOT EXISTS df_transaction.sales (
    sales_id SERIAL,
    location_id INT NOT NULL,
    date TIMESTAMP WITH TIME ZONE NOT NULL,
    amount VARCHAR(10),
    -- pan VARCHAR(20),
    -- mask pan data FOR MONERIS; removed for security
    -- reference VARCHAR(30),
    -- FOR MONERIS; removed for security
    PRIMARY KEY(sales_id),
    FOREIGN KEY(location_id) REFERENCES df_transaction.machine_location(machine_location_id)
);



/*
 general information about the machine
 TODO grab serials and version from XML
 */
CREATE TABLE IF NOT EXISTS machine (
    id VARCHAR(255) DISTINCT NOT NULL,
    type VARCHAR(25),
    -- I.E keggerator vs full unit to be defined...
    version NOT NULL VARCHAR(25),
    -- machine version
    location_id int DEFAULT NULL,
    inventory_level_id int DEFAULT NULL,
    number_of_drinks int,
    -- Do a count of inventory level
    maintenance_log_id timestamp NOT NULL,
    vendor_host varchar(100),
    vendor_provider varchar(100),
    PRIMARY KEY(id, type, version, location_id),
    FOREIGN KEY(location_id) REFERENCES machine_location(id),
    FOREIGN KEY(maintenance_log_id) REFERENCES maintenance_log(id)
);

/*future implementation*/
/*coupon code -> active time with 6 characters*/
CREATE TABLE IF NOT EXISTS coupon (
    coupon_id INT PRIMARY KEY,
    code CHAR(6) UNIQUE,
    machine_id INT,
    percentage INT,
    start_date TIMESTAMP,
    end_date TIMESTAMP,
    FOREIGN KEY(machine_id) references machine(id)
);

/*
 Product holds a local catalog of drinks used in the machine
 Holds references to all products placed in machine before.
 */
CREATE TABLE IF NOT EXISTS df_transaction.product_catalog (
    product_catalog_id SERIAL,
    name VARCHAR(100) NOT NULL,
    vendor_id INT DEFAULT NULL,
    calibration_const DOUBLE,
    -- cost from vendor
    cost_per_litre DOUBLE,
    option_slot INT CHECK(numrange(-1, 8)),
    -- -1 means empty; 0 to 8 are slots 1 to 9 respectively.
    coupon_code_id INT,
    coupon_code CHAR(6),
    PRIMARY KEY(product_catalog_id),
    FOREIGN KEY(vendor_id) REFERENCES df_transaction.users(user_id) FOREIGN KEY(coupon_code, coupon_code_id) REFERENCES df_transaction.coupon
);

/*
 Create Schema for QT elements
 WHY?...can potentially be accessed with QT thread directly with no transaction information required
 */
CREATE SCHEMA IF NOT EXISTS df_QT;

/*
 Debatable...between storing blob and a file reference link!
 Going with reference link.  Dumps are annoying with big files.
 */
CREATE TABLE IF NOT EXISTS df_QT.product_image (
    product_image_id INT,
    -- modify with QT references.
    image_name VARCHAR(100),
    image_path VARCHAR(255),
    -- Upload to standardized folder location!
    PRIMARY KEY(product_image_id),
    FOREIGN KEY(product_image_id) REFERENCES df_transaction.product_catalog(product_catalog_id)
)
/*product pricing*/
CREATE TABLE IF NOT EXISTS df_transaction.pricing (
    product_id int,
    -- cost for consumers
    price_per_litre double,
    location int,
    PRIMARY KEY(product_id),
    FOREIGN KEY(product_id) REFERENCES product(id),
    FOREIGN KEY(location) REFERENCES location(id)
);

CREATE TABLE IF NOT EXISTS inventory_level (
    inventory_level_id SERIAL,
    product_id INT NOT NULL,
    -- every time when there is a log, there should be coresponding id for what product and which machine
    machine_id INT NOT NULL,
    volume double default null,
    refresh_date DATETIME NOT NULL,
    PRIMARY KEY (id) REFERENCES FOREIGN KEY (product_id) REFERENCES product(id),
    FOREIGN KEY (machine_id) REFERENCES machine(id)
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
END IF;

END;

$$ 

/*
 * Teardown: todo integrate into create database procedural
*/
DO $$ DROP DATABASE IF EXISTS drinkfill;

DROP USER IF EXISTS localmachine;

DROP USER IF EXISTS df;

DROP GROUP IF EXISTS dfAdminGroup;

DROP GROUP IF EXISTS machineGroup;

-- DROP TABLE IF EXISTS inventory;
-- DROP TABLE IF EXISTS coupon;
-- DROP TABLE IF EXISTS location;
-- DROP TABLE IF EXISTS machine;
-- DROP TABLE IF EXISTS pricing;
-- DROP TABLE IF EXISTS product;
-- DROP TABLE IF EXISTS sales;
-- DROP TABLE IF EXISTS vendors;
-- DROP TABLE IF EXISTS temperature_log;
-- DROP TABLE IF EXISTS waste_log;

END IF;

END;

$$