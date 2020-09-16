/*******************************************************
// Drinkfill SQLite Seed Script 
//
// df_SQLITE_Seed.sql
// Seed drinks and ETC for Pilot at BNTL
//
// created: 30-07-2020
// by: Li-Yan Tong
//
// copyright 2020 by Drinkfill Beverages Ltd
// all rights reserved
 ********************************************************/

INSERT OR IGNORE INTO machine(machine_id, type, version, location_id, number_of_drinks)
VALUES
(
	'Bentall-Pilot',
	'DF001',
	'1.0.0',
	1,
	( SELECT COUNT(*) FROM inventory where in_stock = 1)
);

Insert Into 
machine_location(manager_name, phone, contact_email, locale_name, street_address, postal_code, province_state, country, on_site_location)
VALUES
( 
	'Bentall Boss',
	'xxx-xxx-xxxx',
	'email@bentall.com',
	'Bental Tower 5',
	'123 Fake St.',
	'XXX XXX',
	'BC',
	'Canada',
	'Lobby 3'
)

INSERT OR IGNORE INTO 
vendor(name, full_address, contact_name, contact_phone, contact_email)
VALUES
(
	'Drinkfill',
	'312 Main Street, Vancouver, BC',
	'Andy Chou',
	'-',
	'andy@drinkfill.com'
)

UPDATE vendor SET contact_name = "Pilot" WHERE name = 'Bentall';
	
INSERT IF NOT EXISTS INTO vendor (
	full_address = '550 Burrard Street, Vancouver, BC, V6C 3H9',
	name = 'Bentall',
	contact_name = NULL,
	contact_phone = NULL,
	contact_email = NULL
)WHERE name = 'Bentall';

UPDATE inventory
SET
in_stock = 1
WHERE inventory_id = 0;

/* ######## INSERTING A NEW PRODUCT ########*/

INSERT INTO product(name, product_type, product_description, vendor_id, user_type, calibration_const, cost_per_litre, option_slot)
VALUES (
	'JJ Bean Cold brew',
	'Cold Brew coffee',
	1,
	'machine',
	1.3,
	20.00,
	-1
);

update product
SET
option_slot = 0
WHERE product_id = 1;

UPDATE inventory
SET 
	product_id = 1,
	volume = 1000, 
	date_refresh = '2020-09-10 08:00:00',
	in_stock = 0
WHERE inventory_id = 0;

/* ######## INSERTING A SALE ########*/

INSERT INTO sales()

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