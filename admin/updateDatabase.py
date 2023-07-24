#!/usr/bin/python

import sys
import requests
import sqlite3
import json
import logging



logging.basicConfig(filename="production/logging/dbLog.txt", level=logging.DEBUG,
                    format="%(asctime)s %(message)s", filemode="a+")

configurationDatabase = sqlite3.connect("/home/df-admin/production/db/configuration.db")
machineId = configurationDatabase.execute("Select machine_id from machine").fetchone()[0]
url = 'https://soapstandportal.com/api/machine/get_machine_info/'
logging.info("Updating the local station configurationDatabase ")
response = requests.get(url+ machineId)

data = response.json()
products = data['Products']
print(data["is_enabled_slot_1"])
for product in products:
   logging.info("Product " + product['name'] + " is updated to " + str(product)) 
   try:
        productUpdate = configurationDatabase.execute("Update products set price_small=?, price_medium=?, price_large=?, price_custom=?, size_small=?, size_medium=?, size_large=?, soapstand_product_serial=? where productId=?",\
                (product['price_small'],product['price_medium'], product['price_large'], product['price_custom'], product['size_small'], product['size_medium'], product['size_large'],\
                product['soapstand_product_serial'], product['productId'] ));
        
   except:
        logging.error("configurationDatabase values are not matching the data type");
logging.info("-------------------------------------------------------------------------------------------------------")
machineUpdate = configurationDatabase.execute("Update machine set is_enabled_slot_1=?, is_enabled_slot_2=?, is_enabled_slot_3=?,is_enabled_slot_4=?",\
                (data["is_enabled_slot_1"],data["is_enabled_slot_2"],data["is_enabled_slot_3"],data["is_enabled_slot_4"]));
configurationDatabase.commit()

