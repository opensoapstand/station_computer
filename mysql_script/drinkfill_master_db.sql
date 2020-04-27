/*******************************************************
// Drinkfill MySQL Master Script For Database Architecture 
//
// drinkfill_master_db.sql
// create the database table structures with keys to define relationships
//
// created: 24-04-2020
// by: Jason Wang
//
// copyright 2020 by Drinkfill Beverages Ltd
// all rights reserved
 ********************************************************/
 
SHOW DATABASES;                -- List the name of all the databases in this server
create database if not exists drinkfill;
USE drinkfill;                     -- Set system database 'mysql' as the current database

SET FOREIGN_KEY_CHECKS = 0;
drop table if exists inventory;
drop table if exists coupon;
drop table if exists location;
drop table if exists machine;
drop table if exists pricing;
drop table if exists product;
drop table if exists sales;
drop table if exists vendors;
SET FOREIGN_KEY_CHECKS = 1;

/*Main database gateway*/
create table if not exists `vendors` (`id` int not null auto_increment, 
									   `full_address` varchar (255),
                                       `name` varchar(50),
                                       `contact_name` varchar (100),
                                       `contact_phone` varchar (15),
                                       `contact_email` varchar (50),
                                       `connector` varchar (100),
                                       primary key (`id`)
                                       );
	
/*Product table shows infomation about the drinks that we used or using or will be using*/
/**/
create table if not exists `product` (`id` int not null auto_increment,           -- SERIAL is an alias for BIGINT UNSIGNED NOT NULL AUTO_INCREMENT UNIQUE.
									   `name` varchar(100) NOT NULL,
                                       `vendor` int, -- unique vendor 
                                       `image` blob,
									   `calibration_const` double,
                                       `cost_per_litre` double, -- cost from vendor
                                       primary key(`id`),
                                       foreign key (`vendor`) references vendors(`id`)); -- build index on unique value column

-- if not exists (select * from `product` where `id` = 0)
-- insert into `product`(`id`, `name`, `image`, `calibration_const`, `cost_per_litre`) values
-- 					 (0, 'NULL', 'NULL', 0 , 0); -- the default choice for the machine
-- select * from `product`;
                                       
/*location of the machine alone with customer (host) infomation*/                                       
create table if not exists `location` (`id` int not null auto_increment,
									   `full_address` varchar (255),
                                       `name` varchar(50),
                                       `in_store_location` varchar(100),
                                       `contact_name` varchar (100),
                                       `contact_phone` varchar (15),
                                       `contact_email` varchar (50),
                                       `connector` varchar (100),
                                       `price_multiplier` double default 1,
                                       primary key(`id`)
                                       );
                                       
/*product pricing*/
create table if not exists `pricing` (`product_id` int, -- cost for consumers
									  `price_per_litre` double,
                                      `location` int,
									  KEY (`product_id`),
                                      foreign key (`product_id`) references product(`id`),
                                      foreign key (`location`) references location(`id`));    
                                       
/*general infomation about the machine*/                                       
create table if not exists `machine` (`id` int not null auto_increment,
									   `type` ENUM('V1', 'V2', 'V3') NOT NULL,  -- machine version
									   `location` int, 
									   `number_of_drinks` int,                  -- how many drinks to display on UI
									   `last_maintenance` timestamp NOT NULL,
                                       `option1` int default null,
                                       `option2` int default null, 
                                       `option3` int default null,
                                       `option4` int default null,
                                       `option5` int default null, 
                                       `option6` int default null,
                                       `option7` int default null,
                                       `option8` int default null,
                                       `option9` int default null,
                                       `vendor_host` varchar(100),
                                       `vendor_provider` varchar(100),
                                       foreign key(`option1`) references product(`id`),
                                       foreign key(`option2`) references product(`id`),
                                       foreign key(`option3`) references product(`id`),
                                       foreign key(`option4`) references product(`id`),
                                       foreign key(`option5`) references product(`id`),
                                       foreign key(`option6`) references product(`id`),
                                       foreign key(`option7`) references product(`id`),
                                       foreign key(`option8`) references product(`id`),
                                       foreign key(`option9`) references product(`id`),
									   foreign key(`location`) references location(`id`),
                                       primary key(`id`)
                                       );
                                       
create table if not exists `inventory` (`machine_id` int,
										`drink1` double default null,
                                        `drink2` double default null,
                                        `drink3` double default null,
                                        `drink4` double default null,
                                        `drink5` double default null,
                                        `drink6` double default null,
                                        `drink7` double default null,
                                        `drink8` double default null,
                                        `drink9` double default null,
                                        `co2` double default null,
                                        foreign key (`machine_id`) references machine(id));
                                       
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

create table if not exists `sales` (`id` int not null auto_increment,
									`machine_id` int,
                                    `date` datetime not null,
                                    `amount` varchar(10),
                                    `pan` varchar(20), -- mask pan data
                                    `reference` varchar(30),
                                    primary key (`id`),
                                    FOREIGN KEY (`machine_id`) REFERENCES machine(`id`));


/*future implementation*/
/*coupon code -> active time with 6 characters*/                                      
create table if not exists `coupon` (`id` int primary key,
									 `code` char(6),
                                     `machine_id` int,
                                     `percentage` int,
									 `start_date` timestamp,
									 `end_date` timestamp,
                                     unique(`code`),
                                     foreign key(`machine_id`) references machine(`id`));
                                     
-- drop table sales;

-- create table if not exists `users` (`id` serial,
-- 									);