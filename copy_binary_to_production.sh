#!/bin/bash



# # create subfolders 
# if [ -d "/home/df-admin/production/db" ] 
# then
#     echo "Directory db exists. Will not create" 
# else
#     echo "Create db folder and add template database"
#     sudo -u df-admin mkdir db
#     sudo -u df-admin scp /home/df-admin/drinkfill/db/sqlite/drinkfill-sqlite.db /home/df-admin/production/db/drinkfill-sqlite.db 
# fi
echo "Stop app as service" 
sudo ./soapstand_service_manager_stop.sh

echo "Copy soapstand application to production folder" 
sudo scp /home/df-admin/drinkfill/ui/DF_UI /home/df-admin/production/bin/DF_UI
sudo scp /home/df-admin/drinkfill/fsm/controller /home/df-admin/production/bin/controller

echo "Start app as service" 
sudo ./soapstand_service_manager_start.sh