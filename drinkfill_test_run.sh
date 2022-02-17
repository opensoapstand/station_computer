#!/usr/bin/env bash

while true
	do
	    # sleep 5
	    DISPLAY=:0 xterm -hold ~/drinkfill/fsm/controller & DISPLAY=:0 xterm ~/drinkfill/ui/DF_UI
	    DISPLAY=:0 xterm -maximized ~/drinkfill/ui/DF_UI
	    # sleep 20
	    sleep 2
	    DISPLAY=:0 xterm -hold  ~/drinkfill/fsm/controller 
done

