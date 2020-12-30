#!/bin/bash

echo "Stopping service"
sudo service drinkfill stop
echo "Starting copy"
sudo scp /home/df-admin/drinkfill/fsm/controller /release/fsm/
sudo scp /home/df-admin/drinkfill/ui/DF_UI /release/ui/
echo "Copy completed"
echo "Starting service"
sudo service drinkfill start
echo "Service started"
echo "Done"
