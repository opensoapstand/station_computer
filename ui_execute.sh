#!/usr/bin/env bash

# set screen orientation (settings are not saved at reboot)
DISPLAY=:0 xrandr -o right

while true
	do
	    DISPLAY=:0 xterm -maximized /home/df-admin/production/bin/DF_UI
        sleep 1
done

