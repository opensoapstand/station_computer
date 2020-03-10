#!/bin/sh

xterm -fullscreen ./release/blkscreen/blackScreen &

sleep 120

while true
do
sleep 1
xterm -fullscreen ./release/ui/EV2_Basic 
done
