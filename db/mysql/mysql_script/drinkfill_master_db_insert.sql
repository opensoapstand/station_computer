/*******************************************************
// Drinkfill MySQL Master Script For Database Architecture 
//
// drinkfill_master_db_insert.sql
// insert to the database table structures with keys to define relationships
//
// created: 27-04-2020
// by: Lode Ameije & Ash Singla
//
// copyright 2022 by Drinkfill Beverages Ltd
// all rights reservedname
 ********************************************************/
 /*sample vendor insertion*/
-- insert into `vendors` (`full_address`, `name`, `contact_name`, `contact_phone`,`contact_email`,`connector`) 
-- 				       value 
-- 					  ('vancouver somewhere', 'NYPD', 'Jake Peralta', '909-000-0192', 'xxx@xxx.com', '242');

insert into `vendors` (`id`, `full_address`, `name`, `contact_name`, `contact_phone`,`contact_email`,`connector`) 
				       values ('1', 'vancouver somewhere', 'NYPD', 'Jake Peralta', '909-000-0192', 'xxx@xxx.com', '242')
                       on duplicate key update full_address = 'vancouver la';

select * from `vendors`;
-- ------------------------------------------------------------------------------------------------------------- --
/*Product table shows infomation about the drinks that we used or using or will be using*/
insert into `product` (`name`,`image`,`calibration_const`,`cost_per_litre`) 
			           value
					  ('Elderflower Kombucha', NULL, 2.19, 1.20);
select * from `product`;
-- -------------------------------------------------------------------------------------------------------------- --
/*sample location insertion*/
insert into `location` (`id`, `full_address`,`name`,`in_store_location`,`contact_name`,`contact_phone`,`contact_email`,`connector`,`price_multiplier`)
						value
					   (1, '#303 6th East Ave Vancouver', 'drinkfill', 'room #303', 'Jason Wang', '222-293-2942', 'www@drinkfill.com', '212', 1.23 )
                       on duplicate key update `in_store_location` = 'main entrance';
select * from `location`;
-- --------------------------------------------------------------------------------------------------------------- --
/*sample pricing insertion*/
insert into `pricing` (`product_id`, `price_per_litre`,`location`)
					  value
                      (1, 2.10, 1);   
select * from `pricing`;
-- --------------------------------------------------------------------------------------------------------------- --
/*sample machine insertion*/
insert into `machine` (`id`, `type`,`number_of_drinks`,`last_maintenance`,`vendor_host`,`vendor_provider`)
					   value
					  (1, 1, 9, now(), 'City of vancouver', 'CCPD')
                      on duplicate key update `vendor_host` = 'vancouver la';
select * from `machine`;
-- ---------------------------------------------------------------------------------------------------------------- --
/*sample inventory insertion*/
-- insert into `inventory` (`machine_id`, `drink1`)
-- 			            value
--                         (1, 452.21);
insert into `inventory` (`product_id`, `machine_id`, `volume`) value (1, 1, 452.21); -- inventory should always be logged individually so we can track progress
                        
select * from `inventory`;		
-- ---------------------------------------------------------------------------------------------------------------- --
/*sample sales insertion*/
insert into `sales` (`machine_id`, `date`, `amount`,`pan`, `reference`)
					value
                    (1, now(), 3.3, "****334", "333");	
						

