#!/bin/bash

echo "create backup"
BKP_FOLDER=production_bkp"$(date +%Y%m%d-%H%M%S)"
BKP_PATH=/home/df-admin/production_archive/$BKP_FOLDER
mkdir -p /home/df-admin/production_archive
mv /home/df-admin/production /home/df-admin/production_archive
mv /home/df-admin/production_archive/production $BKP_PATH

echo "create soapstand production content"
# delete production folder to start anew
sudo rm -r -f production
sudo -u df-admin mkdir /home/df-admin/production
# chown -R $USER:$USER /home/df-admin/production

cd /home/df-admin/production 

# create subfolders 
sudo -u df-admin mkdir /home/df-admin/production/logging  
sudo -u df-admin mkdir /home/df-admin/production/logging/ui  
sudo -u df-admin mkdir /home/df-admin/production/logging/controller  
sudo -u df-admin mkdir /home/df-admin/production/logging/transactions  
sudo -u df-admin mkdir /home/df-admin/production/db
sudo -u df-admin mkdir /home/df-admin/production/admin
sudo -u df-admin mkdir /home/df-admin/production/bin
sudo -u df-admin mkdir /home/df-admin/production/references
sudo -u df-admin mkdir /home/df-admin/production/references/logos
# sudo chmod 777 /home/df-admin/production/references

# move binary files
scp /home/df-admin/drinkfill/ui/DF_UI /home/df-admin/production/bin/DF_UI
scp /home/df-admin/drinkfill/fsm/controller /home/df-admin/production/bin/controller

# move auxiliary to production folder
sudo -u df-admin scp -r /home/df-admin/drinkfill/ui/references /home/df-admin/production

# move scripts and other administrative stuff
scp /home/df-admin/drinkfill/controller_soapstand.service /home/df-admin/production/admin/controller_soapstand.service
scp /home/df-admin/drinkfill/controller_execute.sh /home/df-admin/production/admin/controller_execute.sh
scp /home/df-admin/drinkfill/ui_soapstand.service /home/df-admin/production/admin/ui_soapstand.service
scp /home/df-admin/drinkfill/ui_execute.sh /home/df-admin/production/admin/ui_execute.sh
scp /home/df-admin/drinkfill/soapstand_service_manager_start.sh /home/df-admin/production/admin/soapstand_service_manager_start.sh
scp /home/df-admin/drinkfill/soapstand_service_manager_stop.sh /home/df-admin/production/admin/soapstand_service_manager_stop.sh
scp /home/df-admin/drinkfill/status_services.sh /home/df-admin/production/admin/status_services.sh
scp /home/df-admin/drinkfill/scheduler.sh /home/df-admin/production/admin/scheduler.sh
scp /home/df-admin/drinkfill/screenshotbot.sh /home/df-admin/production/admin/screenshotbot.sh
scp /home/df-admin/drinkfill/screenshotbot_sequence_printer.txt /home/df-admin/production/admin/screenshotbot_sequence_printer.txt
scp /home/df-admin/drinkfill/screenshotbot_sequence_qr.txt /home/df-admin/production/admin/screenshotbot_sequence_qr.txt
scp /home/df-admin/drinkfill/missingTransactions.py /home/df-admin/production/admin/missingTransactions.py

# ask for db to be written. By default: NO! 
read -p "Copy database? [y] for yes. [enter,anykey] for no:" -n 1 -r
echo    # (optional) move to a new line
if [[ $REPLY =~ ^[Yy]$ ]]
then
    sudo -u df-admin scp /home/df-admin/drinkfill/db/sqlite/drinkfill-sqlite_newlayout.db /home/df-admin/production/db/drinkfill-sqlite_newlayout.db 
else
    sudo -u df-admin rm -r /home/df-admin/production/db
    sudo -u df-admin scp -r $BKP_PATH/db /home/df-admin/production
fi
