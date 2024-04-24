#!/bin/bash
# Get Ubuntu version
UBUNTU_VERSION=$(lsb_release -rs)
CONTROLLER_VERSIONED="station_controller_ubuntu"$UBUNTU_VERSION 
echo "Start make controller... for: $CONTROLLER_VERSIONED" 

g++ -pthread -o $CONTROLLER_VERSIONED fsm.cpp components/*.cpp states/*.cpp objects/*.cpp ../library/tinyxml/*.cpp  ../library/i2c/mcp23017/*.cpp ../library/i2c/smbus/*.c ../library/socket/*.cpp ../library/printer/*.cpp ../library/qr/*.cpp -l sqlite3 -lCppLinuxSerial -lcurl
echo "Controller make complete"
