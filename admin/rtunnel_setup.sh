#!/usr/bin/env bash
sudo nano /etc/systemd/system/rtunnel.service
sudo systemctl daemon-reload
sudo service rtunnel restart
echo "done"
Setup rtunnel
