/*******************************************************
// Drinkfill SQLite Setup Script 
//
// df_SQLITE_setup.sql
// Table creation based on future PG implementation
//
// created: 30-07-2020
// by: Li-Yan Tong
//
// copyright 2020 by Drinkfill Beverages Ltd
// all rights reserved
 ********************************************************/

-- Tables will need machine_id FK for Web.  Do not need reference to machine_id locally-- Log and describe maintenance performed--
CREATE TABLE IF NOT EXISTS master_log (
    master_log_id INTEGER PRIMARY KEY AUTOINCREMENT,
    -- 'maintenance_user' reference needed...
    maintenance_code VARCHAR(10),
    -- Create contraint or table for this.
    description TEXT,
    maintenance_provider_id last_maintenance TIMESTAMP WITH TIME ZONE NOT NULL
);

-- Log and describe temperature
CREATE TABLE IF NOT EXISTS  temperature_log (
    temperature_log_id INTEGER PRIMARY KEY AUTOINCREMENT,
    -- Sensors in Degrees Celcius
    top_sensor_C DECIMAL(2) DEFAULT NULL,
    bottom_sensor_C DECIMAL(2) DEFAULT NULL,
    temperature_timestamp TIMESTAMP WITH TIME ZONE NOT NULL
);

-- Log and provide warning switch for waste overflow
CREATE TABLE IF NOT EXISTS  waste_log (
    waste_log_id INTEGER PRIMARY KEY AUTOINCREMENT,
    -- Sensors are level sensors for liquid overspill in waste
    top_failure_sensor BOOLEAN default FALSE,
    bottom_warning_sensor BOOLEAN default FALSE,
    waste_timestamp TIMESTAMP WITH TIME ZONE NOT NULL
);

-- Log and describe dispensed liquids
CREATE TABLE IF NOT EXISTS  flow_io_log (
    flow_io_log_id INTEGER PRIMARY KEY AUTOINCREMENT,
    flow_sensor_slot INT CHECK(
        flow_sensor_slot > -1
        AND flow_sensor_slot < 8
    ),
    -- 0 to 8 are sensors 1 to 9 respectively.
    flow_contents TEXT,
    flow_amount DECIMAL(2) DEFAULT 0.00,
    flow_timestamp TIMESTAMP WITH TIME ZONE NOT NULL
);

/*
 * Location of the machine
 */
CREATE TABLE IF NOT EXISTS   machine_location (
    machine_location_id INTEGER PRIMARY KEY AUTOINCREMENT,
    locale_name VARCHAR(255),
    street_address VARCHAR(255),
    phone VARCHAR(15),
    country VARCHAR(50),
    on_site_location VARCHAR(255),
    user_id_fk INT,
    -- Check for Vendor Type
    user_type_fk VARCHAR(15),
    FOREIGN KEY(user_id_fk, user_type_fk) REFERENCES   user(user_id, user_type)
);

/*
 *  Sales by location of machine
 */
CREATE TABLE IF NOT EXISTS   sales (
    sales_id INTEGER PRIMARY KEY AUTOINCREMENT,
    location_id INT NOT NULL,
    date TIMESTAMP WITH TIME ZONE NOT NULL,
    amount VARCHAR(10),
    -- pan VARCHAR(20),
    -- mask pan data FOR MONERIS; removed for security
    -- reference VARCHAR(30),
    -- FOR MONERIS; removed for security
    FOREIGN KEY(location_id) REFERENCES   machine_location(machine_location_id)
);

/*
 general information about the machine
 TODO grab serials and version from XML
 */
CREATE TABLE IF NOT EXISTS   machine (
    machine_id VARCHAR(255) UNIQUE NOT NULL,
    type VARCHAR(25),
    -- I.E keggerator vs full unit to be defined...
    version VARCHAR(25) NOT NULL,
    -- machine version
    location_id INT DEFAULT NULL,
    number_of_drinks INT,
    -- Do a count of inventory level
    FOREIGN KEY(location_id) REFERENCES   machine_location(machine_location_id)
);

/*
 * Limit inserts to Admin; Try to only keep as many rows needed per device
 * Treat inventory id/row/option as same:
 * Select drink 1 > inventory_id = 1 > 
 */
CREATE TABLE IF NOT EXISTS   inventory (
    inventory_id INTEGER PRIMARY KEY,
    product_id INT,
    volume NUMERIC(2) DEFAULT NULL,
    date_refresh TIMESTAMP WITH TIME ZONE NOT NULL,
    in_stock BOOLEAN DEFAULT FALSE,
    FOREIGN KEY(product_id) REFERENCES   product(product_id)
);

INSERT INTO   inventory
VALUES (0, NULL, NULL, current_timestamp, FALSE),
    (1, NULL, NULL, current_timestamp, FALSE),
    (2, NULL, NULL, current_timestamp, FALSE),
    (3, NULL, NULL, current_timestamp, FALSE),
    (4, NULL, NULL, current_timestamp, FALSE),
    (5, NULL, NULL, current_timestamp, FALSE),
    (6, NULL, NULL, current_timestamp, FALSE),
    (7, NULL, NULL, current_timestamp, FALSE),
    (8, NULL, NULL, current_timestamp, FALSE);

/*
 Product holds a local catalog of drinks used in the machine
 Holds references to all products placed in machine before.
 */
CREATE TABLE IF NOT EXISTS   product (
    product_id INTEGER PRIMARY KEY AUTOINCREMENT,
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
    FOREIGN KEY(vendor_id, user_type) REFERENCES   user(user_id, user_type),
    FOREIGN KEY(coupon_code_id) REFERENCES   coupon(coupon_id),
    FOREIGN KEY(option_slot) REFERENCES   inventory(inventory_id)
);

CREATE TABLE IF NOT EXISTS vendor (
    `vendor_id` INTEGER PRIMARY KEY AUTOINCREMENT, 
    `name` varchar(50),
	`full_address` varchar (255),
    `contact_name` varchar (100),
    `contact_phone` varchar (15),
    `contact_email` varchar (50)
);

// XXX: SQLite DOES NOT SUPPORT Constraint modifications!
ALTER TABLE inventory
ADD CONSTRAINT fk_inventory_product FOREIGN KEY(product_id) REFERENCES   product(product_id);

-- DROP TABLE   coupon CASCADE;
/*future implementation*/
/*coupon code -> active time with 6 characters*/
CREATE TABLE IF NOT EXISTS   coupon (
    coupon_id INTEGER PRIMARY KEY AUTOINCREMENT,
    code CHAR(6) UNIQUE,
    percentage INT,
    start_date TIMESTAMP WITH TIME ZONE NOT NULL,
    end_date TIMESTAMP WITH TIME ZONE NOT NULL
);

/*
 * Product pricing reference based on product and location
 */
CREATE TABLE IF NOT EXISTS   pricing (
    product_id INT,
    -- cost for consumers
    price_per_litre MONEY,
    location_id INT,
    FOREIGN KEY(product_id) REFERENCES   product(product_id),
    FOREIGN KEY(location_id) REFERENCES   machine_location(machine_location_id)
);

/*
 Debatable...between storing blob and a file reference link!
 Going with reference link.  Dumps are annoying with big files.
 */
CREATE TABLE IF NOT EXISTS   product_image_QT (
    product_image_id INT,
    -- modify with QT references.
    image_name VARCHAR(100),
    image_path VARCHAR(255),
    -- Upload to standardized folder location!
    FOREIGN KEY(product_image_id) REFERENCES   product_catalog(product_catalog_id)
)

/*
 * Teardown: todo integrate into create database procedural
 */
DO $$ DROP DATABASE IF EXISTS drinkfill;
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