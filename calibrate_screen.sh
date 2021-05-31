#!/usr/bin/env bash

idd=$(DISPLAY=:0 xinput --list | grep 'Elo Touch Solutions ET3243L pCap USB HI' | awk '{print $10}' | cut -d '=' -f2)
#echo $idd
DISPLAY=:0  xinput map-to-output $idd HDMI-1
#sleep 1
