#!/usr/bin/env bash

#ssh -R 43022:localhost:22 -i /home/df-admin/.ssh/DrinkfillAWS.pem ubuntu@ec2-44-225-153-121.us-west-2.compute.amazonaws.com &

while true
	do
	    sleep 5
	    #DISPLAY=:0 xterm -hold  /release/fsm/controller &
	    #DISPLAY=:0 xterm /release/ui/DF_UI
	    DISPLAY=:0 xterm /release/ui/DF_UI &
	    sleep 8
	    DISPLAY=:0 xterm -hold  /release/fsm/controller 
done
