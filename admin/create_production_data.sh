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
sudo -u df-admin mkdir /home/df-admin/production/logging/wifi  
sudo -u df-admin mkdir /home/df-admin/production/db
sudo -u df-admin mkdir /home/df-admin/production/admin
sudo -u df-admin mkdir /home/df-admin/production/bin
sudo -u df-admin mkdir /home/df-admin/production/references
sudo -u df-admin mkdir /home/df-admin/production/references/logos
sudo -u df-admin mkdir /home/df-admin/production/admin/tap_payment

# move binary files
scp /home/df-admin/drinkfill/ui/DF_UI /home/df-admin/production/bin/DF_UI
scp /home/df-admin/drinkfill/fsm/controller /home/df-admin/production/bin/controller

# move auxiliary to production folder
sudo -u df-admin scp -r /home/df-admin/drinkfill/ui/references /home/df-admin/production

# move scripts and other administrative stuff
scp /home/df-admin/drinkfill/admin/controller_soapstand.service /home/df-admin/production/admin/controller_soapstand.service
scp /home/df-admin/drinkfill/admin/rtunnel.service /home/df-admin/production/admin/rtunnel.service
scp /home/df-admin/drinkfill/admin/set_aws_port.sh /home/df-admin/production/admin/set_aws_port.sh
scp /home/df-admin/drinkfill/admin/check_connectivity.service /home/df-admin/production/admin/check_connectivity.service
scp /home/df-admin/drinkfill/admin/controller_execute.sh /home/df-admin/production/admin/controller_execute.sh
scp /home/df-admin/drinkfill/admin/ui_soapstand.service /home/df-admin/production/admin/ui_soapstand.service
scp /home/df-admin/drinkfill/admin/ui_execute.sh /home/df-admin/production/admin/ui_execute.sh
scp /home/df-admin/drinkfill/admin/soapstand_manager.sh /home/df-admin/production/admin/soapstand_manager.sh
scp /home/df-admin/drinkfill/admin/status_services.sh /home/df-admin/production/admin/status_services.sh
scp /home/df-admin/drinkfill/admin/check_connectivity.sh /home/df-admin/production/admin/check_connectivity.sh
scp /home/df-admin/drinkfill/admin/developer_tools.sh /home/df-admin/production/admin/developer_tools.sh
scp /home/df-admin/drinkfill/admin/copy_and_enable_services.sh /home/df-admin/production/admin/copy_and_enable_services.sh
scp /home/df-admin/drinkfill/admin/create_and_run_production_data.sh /home/df-admin/production/admin/create_and_run_production_data.sh
scp /home/df-admin/drinkfill/admin/rtunnel_print.sh /home/df-admin/production/admin/rtunnel_print.sh

scp /home/df-admin/drinkfill/admin/aws_operations.sh /home/df-admin/production/admin/aws_operations.sh

scp /home/df-admin/drinkfill/admin/screenshotbot.sh /home/df-admin/production/admin/screenshotbot.sh
scp /home/df-admin/drinkfill/admin/screenshotbot_sequence_printer.txt /home/df-admin/production/admin/screenshotbot_sequence_printer.txt
scp /home/df-admin/drinkfill/admin/screenshotbot_sequence_qr.txt /home/df-admin/production/admin/screenshotbot_sequence_qr.txt
scp /home/df-admin/drinkfill/admin/updateDatabase.py /home/df-admin/production/admin/updateDatabase.py
scp /home/df-admin/drinkfill/admin/missingTransactions.py /home/df-admin/production/admin/missingTransactions.py

scp /home/df-admin/drinkfill/admin/enableIPForward.sh /home/df-admin/production/admin/tap_payment
scp /home/df-admin/drinkfill/ui/payment/private_key.der /home/df-admin/production/admin/tap_payment
scp /home/df-admin/drinkfill/ui/payment/public_key.txt /home/df-admin/production/admin/tap_payment


# database gets copied from the backup folder. aka we do not copy the database from the drinkfill folder. 
sudo -u df-admin rm -r /home/df-admin/production/db
sudo -u df-admin scp -r $BKP_PATH/db /home/df-admin/production

echo "Done. NOTE: the database was not copied from the drinkfill folder. It was preserved from the previous production folder. Copy the db manually from drinkfill if needed."
