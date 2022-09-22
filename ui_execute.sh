#!/usr/bin/env bash

# SS0.9 (portrait left) and SS1.0 (portrait right) have different screen orientations. 
# sleep 7
# set screen orientation (settings are not saved at reboot)
# DISPLAY=:0 xrandr -o right

while true
	do
	    DISPLAY=:0 xterm -maximized /home/df-admin/production/bin/DF_UI
        sleep 1
done

