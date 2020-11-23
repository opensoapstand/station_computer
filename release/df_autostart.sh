#!/bin/bash

echo "start\n"
echo "D@nkF1ll$" | sudo chmod 777 /sys/class/gpio/export
echo "D@nkF1ll$" | sudo chmod 777 /sys/class/gpio/unexport
echo "end"
