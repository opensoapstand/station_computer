#!/usr/bin/env bash

while true
	do
	    
	    #DISPLAY=:0 xterm -hold  /release/fsm/controller &
	    #DISPLAY=:0 xterm /release/ui/DF_UI
	    DISPLAY=:0 xterm -maximized /release/ui/DF_UI
#	    sleep 10
#	    DISPLAY=:0 xterm -hold  /release/fsm/controller 
            sleep 5
done

