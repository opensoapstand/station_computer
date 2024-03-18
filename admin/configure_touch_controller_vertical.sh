#!/bin/bash

# configure vertical touch screen for ilitek
# Will not work with WAYLAND. Only tested with xorg display server
# set startup in /etc/gdm3/custom.conf --> disable wayland 

id=$(DISPLAY=:0 xinput | grep -o -E "ILITEK ILITEK-TP\s+id=." |  awk '{print substr($0, length($0))}')
# echo $id

if [ -z "$id" ]; then
    echo "Not an ILITEK ILITEK-TP screen"
else
    echo "ILITEK ILITEK-TP screen found. --> manually adjust touch controller coordinates. "
    xinput set-prop "ILITEK ILITEK-TP" "Coordinate Transformation Matrix" 0 -1 1 1 0 0 0 0 1  # clockwise.
    DISPLAY=:0 xinput list-props $id | grep Matrix
fi


# allll bad
# xinput set-prop 8 "Coordinate Transformation Matrix" 0 1 0 -1 0 1 0 0 1   # vert mirrored horizontal mirrored
# xinput set-prop 8 "Coordinate Transformation Matrix" 0 1 0 1 0 0 0 0 1 # vertical ok, horizontal lmirrored
# xinput set-prop 8 "Coordinate Transformation Matrix" 0 -1 1 -1 0 1 0 0 1 # vertical mirrored, horizontal ok
# xinput set-prop 8 "Coordinate Transformation Matrix" 0 1 0 -1 0 1 0 0 1    # failed to be perfect


# counterclock wise coordinate transformation.  
#  a b c 
#  d e f 
#  g h i   

# default:
# 1 0 0 
# 0 1 0 
# 0 0 1

#xinput set-prop 8 "Coordinate Transformation Matrix" 0 -1 1 1 0 0 0 0 1  # clockwise.
# xinput set-prop "ILITEK ILITEK-TP" "Coordinate Transformation Matrix" 0 -1 1 1 0 0 0 0 1  # clockwise.

# echo "After clockwise transformation:"

# xinput list-props "ILITEK ILITEK-TP" | grep Matrix




