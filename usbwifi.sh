#!/bin/bash

# Mount and Read the USB into an array
sudo mount /dev/sda1 /media/usb

wget -q --spider http://google.com

if [ $? -eq 0 ];

then

echo "ALREADY CONNECTED"

else

echo "NOT CONNECTED"

if grep -qs '/media/usb' /proc/mounts;

then

echo "Going in!"

file="/media/usb/wifi.txt"
while IFS=$'\n\r' read -r line
do
  wifi_array+=("$line")
done < "$file"

echo "${wifi_array[0]}"
echo "${wifi_array[1]}"

# Alternative Array Read
readarray -t wifi_array2 < /media/usb/wifi.txt

#echo ${wifi_array2[0]}
#echo ${wifi_array2[1]}

# Turn off Wifi and delete previous connection...

sudo nmcli radio wifi off

#sudo nmcli connection delete "SoapStand Wifi"

#sudo nmcli connection delete "HardCode WIFI"
#sudo nmcli dev wifi con "312 Main - Members" password 312@Members name "HardCode WIFI"

sudo nmcli radio wifi on

sleep 3

nmcli connection show

# Add with more detailed network settings...
#sudo nmcli con add type wifi con-name "SoapStand Wifi" ifname wlo2 ssid "${wifi_array[0]}"
#sudo nmcli con add type wifi con-name "SoapStand Wifi" ifname wlo2 ssid "312 Main - Members"

# Bash array does not feed the data correctly!  Only finds the Connection.
sudo nmcli dev wifi con "${wifi_array[0]}" password ${wifi_array[1]}

# Works hardcoded...
#sudo nmcli dev wifi con "312 Main - Members" password 312@Members name "SoapStand Wifi"

#sudo nmcli -p con up "SoapStand Wifi"

sleep 2

nmcli connection show

else
 echo "USB wifi failed"
fi
fi
sudo umount /media/usb
