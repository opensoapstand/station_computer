#!/usr/bin/env bash

# move files to service folder
sudo scp /home/df-admin/drinkfill/controller_soapstand.service /etc/systemd/system
sudo scp /home/df-admin/drinkfill/ui_soapstand.service /etc/systemd/system

# reload services
sudo systemctl daemon-reload

# start service
sudo systemctl start controller_soapstand.service
sudo systemctl start ui_soapstand.service