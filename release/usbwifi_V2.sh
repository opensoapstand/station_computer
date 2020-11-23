#!/bin/bash

file="wifi.txt"
while IFS=$'\n\r' read -r line
do
  wifi_array+=("$line")
done < "$file"

echo "${wifi_array[0]}"
echo "${wifi_array[1]}"

# Bash array does not feed the data correctly!  Only finds the Connection.
echo sudo nmcli dev wifi con "${wifi_array[0]}" password ${wifi_array[1]} name "SoapStand Wifi"

# Works hardcoded...
sudo nmcli dev wifi con 312\ Main\ -\ Members password 312@Members name "SoapStand Wifi"

