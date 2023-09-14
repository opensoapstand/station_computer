/*******************************************************
 // Drinkfill Postgres Master Script For Database Architecture 
 //
 // df_db_setup.sql
 // Create Database and Tables; Should only run on Factory Reset.
 //
 // created: 07-02-2020
 // by: Li Yan Tong
 //
 // copyright 2023 by Drinkfill Beverages Ltd // all rights reserved
 ********************************************************/
/* 
 * AWS may require '' EG: CREATE TABLE IF NOT EXISTS 'df_transaction.user' ...
 * USE postgres USER ACCOUNT CONNECTION to template1
 * List the name of all the databases in this server
 */
/*
 * Procedural Programming for creating a drinkfill Database
 */
DO $$ BEGIN -- DB Check
CREATE TEMPORARY VIEW dbchk AS (
    SELECT datname
    FROM pg_database
    WHERE datname = 'drinkfill'
);

IF (
    SELECT COUNT(*)
    FROM dbchk
) < 0 THEN
/* -------- Database Creation -------- */
CREATE DATABASE drinkfill;

-- todo set up admin account
/* -------- Role Creation -------- */
CREATE ROLE df_admin_Group WITH NOSUPERUSER NOCREATEDB;

CREATE ROLE machine_group WITH NOSUPERUSER NOCREATEDB NOCREATEROLE;

-- Future role for customer mobile interaction
-- Future role for maintenance mobile interaction
-- Future role for vendor mobile interaction
-- Change Default schema name
-- ALTER DATABASE drinkfill OWNER TO df;
-- ALTER DATABASE drinkfill OWNER TO postgres;
-- Set permissions to groups
GRANT ALL PRIVILEGES ON DATABASE drinkfill TO df_admin_group;

GRANT CONNECT ON DATABASE drinkfill TO machine_group;

-- Can restrict with product catalog

-- REVOKE ALL PRIVILEGES ON ALL TABLES IN SCHEMA df_transaction FROM machine_group;

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

/* -------- MAINTENANCE data storage -------- */
CREATE SCHEMA IF NOT EXISTS df_maintenance;

-- Tables will need machine_id FK for Web.  Do not need reference to machine_id locally.
-- Log and describe maintenance performed
CREATE TABLE IF NOT EXISTS df_maintenance.master_log (
    master_log_id SERIAL,
    -- 'maintenance_user' reference needed...
    maintenance_code VARCHAR(10),
    -- Create contraint or table for this.
    description TEXT,
    maintenance_provider_id last_maintenance TIMESTAMP WITH TIME ZONE NOT NULL,
    PRIMARY KEY(master_log_id) -- TODO FK
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
    flow_sensor_slot INT CHECK(
        flow_sensor_slot > -1
        AND flow_sensor_slot < 8
    ),
    -- 0 to 8 are sensors 1 to 9 respectively.
    flow_contents TEXT,
    flow_amount DECIMAL(2) DEFAULT 0.00,
    flow_timestamp TIMESTAMP WITH TIME ZONE NOT NULL,
    PRIMARY KEY(flow_io_log_id)
);

/* -------- TRANSACTION data storage -------- */
-- Change default schema for sales transaction data
ALTER SCHEMA public
RENAME TO df_transaction;

-- Create User Tables
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
 * FIXME: Find a better way to tie vendors with machine location...better for web.
 */
CREATE TABLE IF NOT EXISTS df_transaction.machine_location (
    machine_location_id SERIAL,
    manager_name VARCHAR(50),
    phone VARCHAR(15),
    contact_email VARCHAR (50),
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
    PRIMARY KEY(sales_id, location_id),
    FOREIGN KEY(location_id) REFERENCES df_transaction.machine_location(machine_location_id)
);

/*
 general information about the machine
 TODO grab serials and version from XML
 */
CREATE TABLE IF NOT EXISTS df_transaction.machine (
    machine_id VARCHAR(255) UNIQUE NOT NULL,
    type VARCHAR(25),
    -- I.E keggerator vs full unit to be defined...
    version VARCHAR(25) NOT NULL,
    -- machine version
    location_id INT DEFAULT NULL,
    number_of_drinks INT,
    -- Do a count of inventory level
    PRIMARY KEY(machine_id),
    FOREIGN KEY(location_id) REFERENCES df_transaction.machine_location(machine_location_id)
);

/*
 * Limit inserts to Admin; Try to only keep as many rows needed per device
 * Treat inventory id/row/option as same:
 * Select drink 1 > inventory_id = 1 > 
 */
CREATE TABLE IF NOT EXISTS df_transaction.inventory (
    inventory_id SERIAL,
    product_id INT,
    volume NUMERIC(2) DEFAULT NULL,
    date_refresh TIMESTAMP WITH TIME ZONE NOT NULL,
    in_stock BOOLEAN DEFAULT FALSE,
    PRIMARY KEY(inventory_id)
);

INSERT INTO df_transaction.inventory
VALUES (0, NULL, NULL, current_timestamp, FALSE),
    (1, NULL, NULL, current_timestamp, FALSE),
    (2, NULL, NULL, current_timestamp, FALSE),
    (3, NULL, NULL, current_timestamp, FALSE),
    (4, NULL, NULL, current_timestamp, FALSE),
    (5, NULL, NULL, current_timestamp, FALSE),
    (6, NULL, NULL, current_timestamp, FALSE),
    (7, NULL, NULL, current_timestamp, FALSE),
    (8, NULL, NULL, current_timestamp, FALSE);

REVOKE INSERT ON df_transaction.inventory FROM machine_group;

-- drop table df_transaction.product CASCADE;
/*
 Product holds a local catalog of drinks used in the machine
 Holds references to all products placed in machine before.
 */
CREATE TABLE IF NOT EXISTS df_transaction.product (
    product_id SERIAL,
    name VARCHAR(100) NOT NULL,
    product_description VARCHAR(100) NOT NULL,
    vendor_id INT DEFAULT NULL,
    user_type VARCHAR(3) DEFAULT 'VED',
    calibration_const NUMERIC(6),
    -- cost from vendor
    cost_per_litre MONEY,
    option_slot INT DEFAULT(-1) CHECK(
        option_slot > -1
        AND option_slot < 8
    ),
    -- -1 means empty; 0 to 8 are slots 1 to 9 respectively.
    coupon_code_id INT,
    PRIMARY KEY(product_id),
    FOREIGN KEY(vendor_id, user_type) REFERENCES df_transaction.user(user_id, user_type),
    FOREIGN KEY(coupon_code_id) REFERENCES df_transaction.coupon(coupon_id),
    FOREIGN KEY(option_slot) REFERENCES df_transaction.inventory(inventory_id)
);

ALTER TABLE df_transaction.inventory
ADD CONSTRAINT fk_inventory_product FOREIGN KEY(product_id) REFERENCES df_transaction.product(product_id);

-- DROP TABLE df_transaction.coupon CASCADE;
/*future implementation*/
/*coupon code -> active time with 6 characters*/
CREATE TABLE IF NOT EXISTS df_transaction.coupon (
    coupon_id SERIAL,
    code CHAR(6) UNIQUE,
    percentage INT,
    start_date TIMESTAMP WITH TIME ZONE NOT NULL,
    end_date TIMESTAMP WITH TIME ZONE NOT NULL,
    PRIMARY KEY(coupon_id)
);

/*
 * Product pricing reference based on product and location
 */
CREATE TABLE IF NOT EXISTS df_transaction.pricing (
    product_id INT,
    -- cost for consumers
    price_per_litre MONEY,
    location_id INT,
    PRIMARY KEY(product_id),
    FOREIGN KEY(product_id) REFERENCES df_transaction.product(product_id),
    FOREIGN KEY(location_id) REFERENCES df_transaction.machine_location(machine_location_id)
);

/* -------- QTCreator data storage -------- */
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