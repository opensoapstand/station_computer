#!/bin/bash

echo "stop soapstand services"
sudo systemctl stop ui_soapstand
sudo systemctl stop controller_soapstand

# create production data
sudo ./create_production_data.sh

echo "start soapstand services"

# move services to systemd/system folder
sudo scp /home/df-admin/production/admin/controller_soapstand.service /etc/systemd/system
sudo scp /home/df-admin/production/admin/ui_soapstand.service /etc/systemd/system

# reload services
sudo systemctl daemon-reload

# enable at startup (disable to undo)
sudo systemctl enable ui_soapstand.service
sudo systemctl enable controller_soapstand.service


# start service
sudo systemctl start ui_soapstand.service
sudo systemctl start controller_soapstand.service

# status
sudo ./status_services.sh

echo "done."