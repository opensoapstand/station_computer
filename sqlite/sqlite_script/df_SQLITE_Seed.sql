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

INSERT OR IGNORE INTO 
vendor(name, full_address, contact_name, contact_phone, contact_email)
VALUES
(
	'550 Burrard Street, Vancouver, BC, V6C 3H9',
	'Bentall',
	NULL,
	NULL,
	NULL
) WHERE name = 'Bentall';

UPDATE vendor SET contact_name WHERE name = 'Bentall';
	


INSERT IF NOT EXISTS INTO vendor (
	full_address = '550 Burrard Street, Vancouver, BC, V6C 3H9',
	name = 'Bentall',
	contact_name = NULL,
	contact_phone = NULL,
	contact_email = NULL
)WHERE name = 'Bentall';

INSERT IF NOT EXISTS INTO product (
	name = 'JJ Bean Cold brew',
	product_description = 'Cold Brew coffee',
	vendor_id = 1

);

UPDATE inventory
SET 
	product_id = 1,
	volume = 1000, 
	date_refresh = '2020-09-10 08:00:00',
	in_stock = 0
WHERE inventory_id = 0;