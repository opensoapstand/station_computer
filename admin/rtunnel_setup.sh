#!/usr/bin/env bash

# ask for reload of rtunnel script
read -p "Copy ssh rtunnel script to script folder before editing? Warning: this will overwrite the set rtunnel address if already existing. [y] for yes, [q] for quit, other key for no and continue" -n 1 -r
echo    # (optional) move to a new line
if [[ $REPLY =~ ^[Yy]$ ]]
then
    sudo scp /home/df-admin/production/admin/rtunnel.service /etc/systemd/system
elif [[ $REPLY =~ ^[qQ]$ ]]
then
    exit
fi

sudo nano /etc/systemd/system/rtunnel.service
sudo systemctl daemon-reload
sudo service rtunnel restart
echo "done"