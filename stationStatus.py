import json
import os
import sqlite3
import logging
import requests

def checkWifiStatus():
    try:
        output = os.popen('iwconfig wlo2').read()
        output_list = output.split('\n\n')

        wifi_dict = {}
        for item in output_list:
            if len(item)>0:
                item_list = item.split("\n")
                ssid = item_list[0].split('ESSID:"')[1].split('"')[0]
                wifi_dict['ssid'] = ssid
                for i in range(1, len(item_list)):
                    if ':' in item_list[i]:
                        key, val = item_list[i].split(':',1)

                        wifi_dict[key.strip()] = val.strip()
        json_output = json.dumps(wifi_dict)
    except:
        json_output = "Wifi is not connected"
    return json_output

def get_machine_id():
    database = sqlite3.connect("/home/df-admin/production/db/drinkfill-sqlite_newlayout.db")
    machineId = database.execute("Select machine_id from machine").fetchone()[0]
    return machineId

def updateStatusToPortalAndLocalFile(machineId, wifiStatus):
    
    logging.basicConfig(filename="/home/df-admin/production/logging/statusLog.txt", level=logging.DEBUG,
                    format="%(asctime)s %(message)s", filemode="a+")
    logging.debug('\n')
    logging.info(wifiStatus) 
    payload = {
        "MachineSerialNumber" : machineId,
        "wifiStrength": wifiStatus
    }
    try:
        response = requests.post("https://soapstandportal.com/api/stationStatus/add", payload)
        if(response.status_code == 200):
            logging.info("Updated the status to portal")
        else:
            logging.error("Update to portal failed")
    except:
        logging.error("Connection not established")
    logging.info("-------------------------------------------------------------------------------------------------------")


updateStatusToPortalAndLocalFile(get_machine_id(), checkWifiStatus())
