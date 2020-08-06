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

Insert Into 
machine_location(manager_name, phone, contact_email, locale_name, street_address, country, on_site_location)
VALUES
( ""

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

INSERT INTO product(name, product_description, vendor_id, user_type, calibration_const, cost_per_litre, option_slot, coupon_code_id)
VALUES (
	'JJ Bean Cold brew',
	'Cold Brew coffee',
	vendor_id = 1
	user_type
);


INSERT INTO   inventory
VALUES (0, NULL, NULL, current_timestamp, 0),
    (1, NULL, NULL, current_timestamp, 0),
    (2, NULL, NULL, current_timestamp, 0),
    (3, NULL, NULL, current_timestamp, 0),
    (4, NULL, NULL, current_timestamp, 0),
    (5, NULL, NULL, current_timestamp, 0),
    (6, NULL, NULL, current_timestamp, 0),
    (7, NULL, NULL, current_timestamp, 0),
    (8, NULL, NULL, current_timestamp, 0);

UPDATE inventory
SET 
	product_id = 1,
	volume = 1000, 
	date_refresh = '2020-09-10 08:00:00',
	in_stock = 0
WHERE inventory_id = 0;

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