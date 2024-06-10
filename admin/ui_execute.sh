#!/usr/bin/env bash

# SS0.9 (portrait left) and SS1.0 (portrait right) have different screen orientations. 
# this is not retentive. Set in UI --> start->settings->display->orientation for changes to survive a reboot
# set screen orientation (settings are not saved at reboot)
# DISPLAY=:0 xrandr -o right

sleep 6 # absolutely mandatory. If omitted, it will seem like the app will work without an issue. But e.g. popup windows as modal can still disappear in the background. The reason being that the app is started before all ubuntu windows managers were started up. 

gsettings set org.gnome.desktop.interface enable-animations false  # instant transition from one page to another. If animations are enabled, the quick minimization is a security issue (people can tap during that short timespan and access linux desktop)
echo 
while true
	do

		# set vertical screen touch coordinate transformation right for the ILITEK screens. Ubuntu looses the setting (at least) at reboot. (see also: setup Ubuntu script) 
		id=$(DISPLAY=:0 xinput | grep -E "ILITEK ILITEK-TP\s+id=" | awk -F"id=" '{print $2}' | awk '{print $1}')
		if [ -z "$id" ]; then
			echo "Not an ILITEK ILITEK-TP screen"
		else
			echo "ILITEK ILITEK-TP screen found. --> manually adjust touch controller coordinates. "
			DISPLAY=:0 xinput set-prop "ILITEK ILITEK-TP" "Coordinate Transformation Matrix" 0 1 0 -1 0 1 0 0 1
			DISPLAY=:0 xinput list-props "ILITEK ILITEK-TP" | grep Matrix
		fi

		UBUNTU_VERSION=$(lsb_release -rs)
		CONTROLLER_VERSIONED="station_controller_ubuntu"$UBUNTU_VERSION 
		UI_VERSIONED="station_ui_ubuntu"$UBUNTU_VERSION 
	    DISPLAY=:0 xterm -maximized /home/df-admin/production/bin/$UI_VERSIONED
        sleep 1
done


