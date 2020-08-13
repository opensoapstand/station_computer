#!/bin/sh

xterm -fullscreen ./release/blackscreen/blackScreen &

sleep 10

while true
do
sleep 1
xterm -fullscreen ./release/ui/EV2_Basic 
done
