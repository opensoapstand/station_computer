#!/bin/bash

echo "create backup"
BKP_FOLDER=production_bkp"$(date +%Y%m%d-%H%M%S)"
mkdir -p /home/df-admin/production_archive
mv /home/df-admin/production /home/df-admin/production_archive
mv /home/df-admin/production_archive/production /home/df-admin/production_archive/$BKP_FOLDER

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
sudo -u df-admin scp /home/df-admin/drinkfill/db/sqlite/drinkfill-sqlite.db /home/df-admin/production/db/drinkfill-sqlite.db 
sudo -u df-admin scp /home/df-admin/drinkfill/db/sqlite/drinkfill-sqlite_newlayout.db /home/df-admin/production/db/drinkfill-sqlite_newlayout.db 
sudo -u df-admin scp -r /home/df-admin/drinkfill/ui/references /home/df-admin/production

# move scripts and other administrative stuff
scp /home/df-admin/drinkfill/controller_soapstand.service /home/df-admin/production/admin/controller_soapstand.service
scp /home/df-admin/drinkfill/controller_execute.sh /home/df-admin/production/admin/controller_execute.sh
scp /home/df-admin/drinkfill/ui_soapstand.service /home/df-admin/production/admin/ui_soapstand.service
scp /home/df-admin/drinkfill/ui_execute.sh /home/df-admin/production/admin/ui_execute.sh
scp /home/df-admin/drinkfill/soapstand_service_manager_start.sh /home/df-admin/production/admin/soapstand_service_manager_start.sh
scp /home/df-admin/drinkfill/soapstand_service_manager_stop.sh /home/df-admin/production/admin/soapstand_service_manager_stop.sh
scp /home/df-admin/drinkfill/status_services.sh /home/df-admin/production/admin/status_services.sh