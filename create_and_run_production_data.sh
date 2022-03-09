#!/bin/bash

echo "stop soapstand services"
sudo systemctl stop ui_soapstand
sudo systemctl stop controller_soapstand

# create production data
sudo ./create_production_data.sh

echo "start soapstand services"

# move services to systemd/system folder
sudo scp /home/df-admin/drinkfill/production/controller_soapstand.service /etc/systemd/system
sudo scp /home/df-admin/drinkfill/production/ui_soapstand.service /etc/systemd/system

# reload services
sudo systemctl daemon-reload

# enable at startup (disable to undo)
sudo systemctl enable ui_soapstand.service
sudo systemctl enable controller_soapstand.service


# start service
sudo systemctl start ui_soapstand.service
sudo systemctl start controller_soapstand.service

echo "done."