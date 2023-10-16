#!/usr/bin/env bash

# SS0.9 (portrait left) and SS1.0 (portrait right) have different screen orientations. 
# this is not retentive. Set in UI --> start->settings->display->orientation for changes to survive a reboot
# sleep 7
# set screen orientation (settings are not saved at reboot)
# DISPLAY=:0 xrandr -o right
database_path="/home/df-admin/production/db/configuration.db"
query="Select payment from products where slot=1"
paymentType=$(sqlite3 "$database_path" "$query")
# Compare the 'paymentType' variable to a string. Use double brackets for the comparison.
if [[ "$paymentType" == "tapTCP" ]]; then
	./tap_payment/enableIPForward.sh  # create a connection between the two LAN controllers with a static ip on the Odyssey. This way, one network can access the internet to which the other network is connected. Used for tap payment
fi
gsettings set org.gnome.desktop.interface enable-animations false  # instant transition from one page to another. If animations are enabled, the quick minimization is a security issue (people can tap during that short timespan and access linux desktop)
echo 
while true
	do

	    DISPLAY=:0 xterm -maximized /home/df-admin/production/bin/DF_UI
        sleep 1
done

