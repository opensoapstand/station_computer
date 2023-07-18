
#!/bin/bash
#set -e #setting stop script on error

echo "stop soapstand services"
sudo systemctl stop ui_soapstand.service
sudo systemctl stop controller_soapstand.service
sudo systemctl stop check_connectivity.service

echo "take backup of production folder"
cd /home/df-admin/
mv production productionBKP

echo "unzip productionstatic.zip"
unzip productionstatic.zip -d ./production

echo "copy db from production backup"
scp -r productionBKP/db production

echo "copy log files from production backup"
scp -r productionBKP/logging production/logging

echo "change production backup to a name with a date."
# Get the current date in YYYY-MM-DD format
current_date=$(date +%Y%m%d-%H%M%S)
            
# Append the current date to the folder name
name_with_date="production_bkp${current_date}"
mkdir production_archive
mv productionBKP "${name_with_date}"
sudo mv "${name_with_date}" ./production_archive

cd /home/df-admin/production/admin
./copy_and_enable_services.sh