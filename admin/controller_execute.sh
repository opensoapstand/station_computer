#!/usr/bin/env bash
#!/bin/bash

# Get Ubuntu version
UBUNTU_VERSION=$(lsb_release -rs)
CONTROLLER_VERSIONED="controller_"$UBUNTU_VERSION 

while true
	do
	    
	    #DISPLAY=:0 xterm -hold  /release/fsm/controller &
	    #DISPLAY=:0 xterm /release/ui/DF_UI
	    # DISPLAY=:0 xterm -maximized /release/ui/DF_UI
#	    sleep 10
#	    DISPLAY=:0 xterm -hold  /release/fsm/controller 
		DISPLAY=:0 xterm -maximized /home/df-admin/production/bin/$CONTROLLER_VERSIONED
        sleep 1
done

