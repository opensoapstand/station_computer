#!/bin/bash

#Get machine id from database
database_path="/home/df-admin/production/db/configuration.db"
query="Select machine_id from machine"
machineId=$(sqlite3 "$database_path" "$query")

database_temperature="/home/df-admin/production/db/usage.db"
getTemperature="Select temperature from temperature ORDER BY time DESC LIMIT 1"
temperatureValue=$(sqlite3 "$database_temperature" "$getTemperature")


# Check Wi-Fi connectivity strength
wifi_strength=$(nmcli -t dev wifi|grep '*')
#Check if folder exists or create folder and file if it doesn't exist
folder="/home/df-admin/production/logging/wifi"
mkdir -p "$folder" && touch "$folder"/logging.txt

#initialize soapstandportal POST request variables
url="https://soapstandportal.com/api/stationStatus/add"
payload="{\"MachineSerialNumber\" : \"$machineId\",\"wifiStrength\": \"$wifi_strength\",\"temperature\": \"$temperatureValue\"}"
response=$(curl -X POST -H "Content-Type: application/json" -d "$payload" "$url")


# Append result to log file
echo "$(date +%Y-%m-%d_%k.%M.%S):$(date +%s):$wifi_strength" >> "$folder"/logging.txt
echo "Soapstandportal response:$response" >> "$folder"/logging.txt
