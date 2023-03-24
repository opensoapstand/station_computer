#!/bin/bash

echo "stop soapstand services"
sudo systemctl stop ui_soapstand
sudo systemctl stop controller_soapstand

# create production data
sudo ./create_production_data.sh
sudo ./copy_and_enable_services.sh

echo "start soapstand services"
# start service
sudo systemctl start ui_soapstand.service
sudo systemctl start controller_soapstand.service
sudo systemctl start check_connectivity.service

# status
sudo ./status_services.sh

echo "done."