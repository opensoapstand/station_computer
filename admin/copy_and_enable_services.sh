#!/usr/bin/env bash
echo "Copy soapstand services from production. ---- THIS WILL NOT RESTART THEM ----"
# move services to systemd/system folder
sudo scp /home/df-admin/production/admin/controller_soapstand.service /etc/systemd/system
sudo scp /home/df-admin/production/admin/ui_soapstand.service /etc/systemd/system
sudo scp /home/df-admin/production/admin/check_connectivity.service /etc/systemd/system

 #missing Transactions 
# could be done as a service: run python ~/production/admin/missingTransactions.py hourly
crontab -l | grep -q 'missingTransactions.py' && echo 'Missing transactions handler scheduler not reloaded: already exists'  \
|| crontab -l > mycron 
echo "0 * * * *  python ~/production/admin/missingTransactions.py" >> mycron 
crontab mycron
rm mycron

# reload services
sudo systemctl daemon-reload

# enable at startup (disable to undo)
sudo systemctl enable ui_soapstand.service
sudo systemctl enable controller_soapstand.service
sudo systemctl enable check_connectivity.service

