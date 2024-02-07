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

	    DISPLAY=:0 xterm -maximized /home/df-admin/production/bin/DF_UI
        sleep 1
done

