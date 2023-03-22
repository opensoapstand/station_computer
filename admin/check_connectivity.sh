#!/bin/bash
# iwconfig wlo2

# Check Wi-Fi connectivity strength
# wifi_strength=$(nmcli dev wifi | grep -o -E '[0-9]+')
wifi_strength=$(nmcli -t dev wifi)

# Append result to log file
echo "$(date +%Y-%m-%d_%k.%M.%S):$(date +%s):$wifi_strength" >> /home/df-admin/production/admin/log.txt