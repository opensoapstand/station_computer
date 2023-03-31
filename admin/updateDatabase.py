#!/usr/bin/python

import sys
import requests
import sqlite3
import json
import logging



logging.basicConfig(filename="production/logging/dbLog.txt", level=logging.DEBUG,
                    format="%(asctime)s %(message)s", filemode="a+")

database = sqlite3.connect("/home/df-admin/production/db/drinkfill-sqlite_newlayout.db")
machineId = database.execute("Select machine_id from machine").fetchone()[0]
print(machineId)
url = 'https://soapstandportal.com/api/machine/get_machine_info/'
logging.info("Updating the local station database ")
response = requests.get(url+ machineId)

data = response.json()
products = data['Products']
for product in products:
   logging.info("Product " + product['name'] + " is updated to " + str(product)) 
   try:
        productUpdate = database.execute("Update products set price_small=?, price_medium=?, price_large=?, price_custom=?, size_small=?, size_medium=?, size_large=?, soapstand_product_serial=? where productId=?",\
                (product['price_small'],product['price_medium'], product['price_large'], product['price_custom'], product['size_small'], product['size_medium'], product['size_large'],\
                product['soapstand_product_serial'], product['productId'] ));
   except:
        logging.error("Database values are not matching the data type");
logging.info("-------------------------------------------------------------------------------------------------------")
database.commit()

