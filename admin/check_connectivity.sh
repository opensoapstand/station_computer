#!/bin/bash

#Get machine id from database
database_path="/home/df-admin/production/db/configuration.db"
query="Select machine_id from machine"
machineId=$(sqlite3 "$database_path" "$query")

getPortalUrl="Select portal_base_url from machine"
portalBaseUrl=$(sqlite3 "$database_path" "$getPortalUrl")

database_temperature="/home/df-admin/production/db/usage.db"
getTemperature="Select temperature_1 from temperature ORDER BY time DESC LIMIT 1"
temperatureValue1=$(sqlite3 "$database_temperature" "$getTemperature")

database_temperature2="/home/df-admin/production/db/usage.db"
getTemperature2="Select temperature_2 from temperature ORDER BY time DESC LIMIT 1"
temperatureValue2=$(sqlite3 "$database_temperature2" "$getTemperature2")

# Check Wi-Fi connectivity strength
wifi_strength=$(nmcli -t -f ssid,mode,freq,rate,signal,security dev wifi | grep '*')
#Check if folder exists or create folder and file if it doesn't exist
folder="/home/df-admin/production/logging/wifi"
mkdir -p "$folder" && touch "$folder"/logging.txt

#initialize soapstandportal POST request variables
url="${portalBaseUrl}api/stationStatus/add"
payload="{\"MachineSerialNumber\" : \"$machineId\",\"wifiStrength\": \"$wifi_strength\",\"temperature_1\": \"$temperatureValue1\",\"temperature_2\": \"$temperatureValue2\"}"
response=$(curl -X POST -H "Content-Type: application/json" -d "$payload" "$url")


# Append result to log file
echo "$(date +%Y-%m-%d_%k.%M.%S):$(date +%s):$wifi_strength" >> "$folder"/logging.txt
echo "Soapstandportal response:$response" >> "$folder"/logging.txt