#!/bin/bash

export DISPLAY=:0

 echo "Screen setup for UI:"

echo "     - disable window transition animations."
gsettings set org.gnome.desktop.interface enable-animations false

echo "     - disable lock screen"
gsettings set org.gnome.desktop.screensaver lock-enabled false
gsettings set org.gnome.desktop.screensaver ubuntu-lock-on-suspend false

echo "     - decrease reboot time."
user_question="Continue to decrease boot time? Nano will open, change: GRUB_TIMEOUT=1 add: GRUB_RECORDFAIL_TIMEOUT=\$GRUB_TIMEOUT, [y] to continu, other key to skip."
read -p "$user_question" -n 1 -r
echo    # (optional) move to a new line

if [[ $REPLY =~ ^[Yy]$ ]]
then
sudo nano /etc/default/grub
sudo update-grub
fi

 echo "    - Screen touch controller setup:"
# configure vertical touch screen for ilitek
# Will not work with WAYLAND. Only tested with xorg display server
# set startup in /etc/gdm3/custom.conf --> disable wayland 
#!/bin/bash

# Path to the configuration file
CONFIG_FILE="/etc/gdm3/custom.conf"

# Check if WaylandEnable=false is already uncommented
if grep -q "^#.*WaylandEnable=false" "$CONFIG_FILE"; then
    # The line is commented, uncomment it
    sudo sed -i '/^#.*WaylandEnable=false/s/^#//' "$CONFIG_FILE"
    echo "WaylandEnable was commented. It has been uncommented."
else
    if grep -q "^WaylandEnable=false" "$CONFIG_FILE"; then
        echo "WaylandEnable is already uncommented."
    else
        # The line does not exist, add it
        echo "WaylandEnable=false" | sudo tee -a "$CONFIG_FILE" > /dev/null
        echo "WaylandEnable=false was added to the file."
    fi
fi

# WARNING: This code is also executed from ui_execute.sh  Please consider this when making changes.  
echo "WARNING: will not work if script is called from another script over ssh."
#id=$(DISPLAY=:0 xinput | grep -o -E "ILITEK ILITEK-TP\s+id=." |  awk '{print substr($0, length($0))}')
id=$(DISPLAY=:0 xinput | grep -E "ILITEK ILITEK-TP\s+id=" | awk -F"id=" '{print $2}' | awk '{print $1}')

echo $id
if [ -z "$id" ]; then
    echo "Not an ILITEK ILITEK-TP screen"
else
    echo "ILITEK ILITEK-TP screen found. --> manually adjust touch controller coordinates. "
    DISPLAY=:0 xinput set-prop "ILITEK ILITEK-TP" "Coordinate Transformation Matrix" 0 1 0 -1 0 1 0 0 1
    DISPLAY=:0 xinput list-props "ILITEK ILITEK-TP" | grep Matrix
fi

echo "setup script done"
# coordinate transformation matrix
#  a b c 
#  d e f 
#  g h i   

# default:
# 1 0 0 
# 0 1 0 
# 0 0 1

# allll bad
# xinput set-prop 8 "Coordinate Transformation Matrix" 0 1 0 -1 0 1 0 0 1   # vert mirrored horizontal mirrored
# xinput set-prop 8 "Coordinate Transformation Matrix" 0 1 0 1 0 0 0 0 1 # vertical ok, horizontal lmirrored
# xinput set-prop 8 "Coordinate Transformation Matrix" 0 -1 1 -1 0 1 0 0 1 # vertical mirrored, horizontal ok
# xinput set-prop 8 "Coordinate Transformation Matrix" 0 1 0 -1 0 1 0 0 1    # failed to be perfect

#xinput set-prop 8 "Coordinate Transformation Matrix" 0 -1 1 1 0 0 0 0 1  # clockwise.
# xinput set-prop "ILITEK ILITEK-TP" "Coordinate Transformation Matrix" 0 -1 1 1 0 0 0 0 1  # clockwise.

# xinput list-props "ILITEK ILITEK-TP" | grep Matrix