#!/usr/bin/env bash

set -e #setting stop script on error

# move files to service folder
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
