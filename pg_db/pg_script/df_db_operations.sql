/*******************************************************
 // Drinkfill Postgres operations 
 //
 // df_db_operations.sql
 // Typical function calls on database
 //
 // created: 07-08-2020
 // by: Li Yan Tong
 //
 // copyright 2020 by Drinkfill Beverages Ltd
 // all rights reservedname
 ********************************************************/

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