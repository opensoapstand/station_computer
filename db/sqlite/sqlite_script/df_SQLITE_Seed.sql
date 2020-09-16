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


/* ######## INSERTING A NEW PRODUCT ########*/

/* If we know the supplier or Vendor */
INSERT OR IGNORE INTO vendor(
	name,
	full_address,
	contact_name,
	contact_phone,
	contact_email
) VALUES 
(
	'Oddity Kombucha',
	'1863 Ontario St., Vancouver, BC',
	NULL,
	NULL,
	NULL
);

INSERT OR IGNORE INTO product(
name, 
product_brand, 
product_type, 
product_flavour, 
product_description,
product_ingredients,
product_requires_pump,
vendor_id,
calibration_const, 
cost_per_litre)
VALUES (
	'Oddity''s Ginger Kombucha',
	'Oddity Kombucha',
	'Kombucha',
	'Ginger',
	'Oddity''s Ginger Kombucha is made with green and black tea and blended with fresh ginger juice and lemongrass for a crisp and refreshing taste',
	'Black tea, green tea, cane sugar, ginger juice, lemongrass, water and kombucha culture',
	0,
	(SELECT vendor_id from vendor
    WHERE name = "Oddity Kombucha"),
	1.3,
	0.00
);

/* FIXME: Hardcoded, should select query these values */
INSERT OR IGNORE INTO pricing (
	product_id,
	price_per_litre,
	small_price,
	medium_price,
	large_price,
	location_id
) VALUES (
	1,
	0.00,
	3.00,
	3.50,
	4.00,
	1
)

update product
SET
option_slot = 0
WHERE product_id = 1;

/* FIXME: Hardcoded, should do a query to link with Product */
UPDATE inventory
SET 
	product_id = 1,
	volume = 18900, 
	date_refresh = '2020-09-31 08:00:00',
	in_stock = 1
WHERE inventory_id = 0;


UPDATE inventory
SET
in_stock = 1
WHERE inventory_id = 0;

/* FOR BENTALL */

INSERT OR IGNORE INTO vendor(
	name,
	full_address,
	contact_name,
	contact_phone,
	contact_email
) VALUES 
(
	'Bucha Brew',
	NULL,
	'Julia Moss',
	NULL,
	"chantal@buchabrew.ca"
);

INSERT OR IGNORE INTO product(
name, 
product_brand, 
product_type, 
product_flavour, 
product_description,
product_ingredients,
product_requires_pump,
vendor_id,
calibration_const, 
cost_per_litre)
VALUES (
	'Bucha Brew Mango Kombucha',
	'Bucha Brew',
	'Kombucha',
	'Mango',
	'Mouthwatering mango that will save you from the melting point.  Just when you think you can''t take the heat, strip, take a sip and show that sun of a bucha who''s hotter now.',
	'Water, organic kombucha culture, organic cane sugar, oragnic green tea, organic black tea, mango',
	0,
	(SELECT vendor_id from vendor
    WHERE name = "Bucha Brew"),
	1.3,
	0.00
);

/* FIXME: Hardcoded, should select query these values */
INSERT OR IGNORE INTO pricing (
	product_id,
	price_per_litre,
	small_price,
	medium_price,
	large_price,
	location_id
) VALUES (
	(SELECT product_id from product where product_brand = "Bucha Brew" AND product_flavour == "Mango"),
	0.00,
	3.00,
	3.50,
	4.00,
	1
);

update product
SET
option_slot = 0
WHERE product_id = 1;

/* FIXME: Hardcoded, should do a query to link with Product */
UPDATE inventory
SET 
	product_id = 1,
	volume = 1000, 
	date_refresh = '2020-09-10 08:00:00',
	in_stock = 1
WHERE inventory_id = 0;

UPDATE inventory
SET
in_stock = 1
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