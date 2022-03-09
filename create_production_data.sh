#!/bin/bash

echo "create backup"
sudo mv /home/df-admin/drinkfill/production /home/df-admin/production_"$(date +%Y%m%d-%H%M%S)"

echo "create soapstand production content"
# delete production folder to start anew
sudo rm -r -f production
sudo -u df-admin mkdir production
# chown -R $USER:$USER /home/df-admin/production
cd production 

# create subfolders 
sudo -u df-admin mkdir db
sudo -u df-admin mkdir logging

# move files to production folder
sudo scp -r /home/df-admin/drinkfill/ui/references /home/df-admin/drinkfill/production/references
sudo -u df-admin scp /home/df-admin/drinkfill/db/sqlite/drinkfill-sqlite.db /home/df-admin/drinkfill/production/db/drinkfill-sqlite.db 

sudo scp /home/df-admin/drinkfill/ui/DF_UI /home/df-admin/drinkfill/production/DF_UI
sudo scp /home/df-admin/drinkfill/fsm/controller /home/df-admin/drinkfill/production/controller

sudo scp /home/df-admin/drinkfill/controller_soapstand.service /home/df-admin/drinkfill/production/controller_soapstand.service
sudo scp /home/df-admin/drinkfill/controller_execute.sh /home/df-admin/drinkfill/production/controller_execute.sh
sudo scp /home/df-admin/drinkfill/ui_soapstand.service /home/df-admin/drinkfill/production/ui_soapstand.service
sudo scp /home/df-admin/drinkfill/ui_execute.sh /home/df-admin/drinkfill/production/ui_execute.sh
sudo scp /home/df-admin/drinkfill/soapstand_service_manager_start.sh /home/df-admin/drinkfill/production/soapstand_service_manager_start.sh
sudo scp /home/df-admin/drinkfill/soapstand_service_manager_stop.sh /home/df-admin/drinkfill/production/soapstand_service_manager_stop.sh

