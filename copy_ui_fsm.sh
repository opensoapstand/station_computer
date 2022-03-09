#!/bin/bash


echo "Stopping service"
sudo systemctl drinkfill stop
echo "Starting copy"
sudo scp fsm/controller /release/fsm/
sudo scp ui/DF_UI /release/ui/
echo "Copy completed"
echo "Starting service"
sudo service drinkfill start
echo "Service started"
echo "Done"


# echo "Stopping service"
# sudo service drinkfill stop
# echo "Starting copy"
# sudo scp fsm/controller /release/fsm/
# sudo scp ui/DF_UI /release/ui/
# echo "Copy completed"
# echo "Starting service"
# sudo service drinkfill start
# echo "Service started"
# echo "Done"
