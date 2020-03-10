/***************************************
//
// drinkfill_database.sql
// creating a database with the needed table for the keep record 
// of transactions, inventory count, and machine id
//
// created: 07-08-2019
// by: Jason Wang
//
// copyright 2019 by drinkfill
// all rights reserved
//***************************************/


CREATE DATABASE drinkfill;

USE drinkfill;

CREATE TABLE IF NOT EXISTS `machine_id` (
  `model_id` INT NOT NULL, -- each machine should have their own identified id
  `location` VARCHAR(255) NOT NULL, -- where this machine is installed
  `type` INT NOT NULL -- how many screen or version of the machine 
  );
  
  CREATE TABLE IF NOT EXISTS `inventory` (
  `plu_id` BIGINT, -- can be empty eg. no inventory 
  `machine_id` VARCHAR(20), -- type MAY need to be changed 
  `volume` INT, -- amount of beverage in stock
  `price` FLOAT -- ex. 4.99
  );
  
CREATE TABLE IF NOT EXISTS `beverage` (
  `plu_id` BIGINT, -- can be empty eg. no inventory 
  `n` BIGINT, -- [NEED TO CHECK WHAT THIS IS]type MAY need to be changed (8 byte)
  `image_address` VARCHAR(255),
  `long_text_address` VARCHAR(255)
  );
  
CREATE TABLE IF NOT EXISTS `transaction` (
  `id` BIGINT AUTO_INCREMENT PRIMARY KEY, -- store each transaction into its own id 
  `plu_id` BIGINT, -- can be empty eg. no inventory 
  `machine_id` VARCHAR(20), -- type MAY need to be changed
  `volume_requested` INT,
  `volume_dispensed` INT 
  );
  
CREATE TABLE IF NOT EXISTS `event` (
  `machine_id` VARCHAR(20) NOT NULL, -- type MAY need to be changed [corresponding machine]  
  `time` DATETIME NOT NULL, -- each event that happened should have the corresponding time
  `event_code` INT NOT NULL, -- type of event with its own defined code
  `text` VARCHAR(255), -- discrption of the event
  `inventory` INT
  );

CREATE TABLE IF NOT EXISTS `survey` (
   `time` DATETIME NOT NULL,
   `drinkAnswer` VARCHAR(50),
   `emailAddress` VARCHAR(50),
   `action` VARCHAR(20)
);
