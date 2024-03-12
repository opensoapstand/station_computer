#!/usr/bin/env bash
#!/bin/bash

# Get Ubuntu version
UBUNTU_VERSION=$(lsb_release -rs)
CONTROLLER_VERSIONED="station_controller_ubuntu"$UBUNTU_VERSION 

while true
	do
		DISPLAY=:0 xterm -maximized /home/df-admin/production/bin/$CONTROLLER_VERSIONED
        sleep 1
done

