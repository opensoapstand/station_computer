#!/usr/bin/env bash

# move files to service folder
sudo scp /home/df-admin/drinkfill/controller_soapstand.service /etc/systemd/system
sudo scp /home/df-admin/drinkfill/ui_soapstand.service /etc/systemd/system

# reload services
sudo systemctl daemon-reload

# enable at startup (disable to undo)
sudo systemctl enable ui_soapstand.service
sudo systemctl enable controller_soapstand.service


# start service
sudo systemctl start ui_soapstand.service
sudo systemctl start controller_soapstand.service
