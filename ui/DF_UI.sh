UBUNTU_VERSION=$(lsb_release -rs)
UI_VERSIONED="station_ui_ubuntu"$UBUNTU_VERSION 

DISPLAY=:0 xterm -maximized ./$UI_VERSIONED
