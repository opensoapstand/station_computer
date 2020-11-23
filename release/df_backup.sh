#!/usr/bin/env bash

while true
	do
	    sleep 1
	    DISPLAY=:0 xterm -hold  /release/fsm/controller &
	    DISPLAY=:0 xterm -fullscreen /release/ui/DF_UI
done
