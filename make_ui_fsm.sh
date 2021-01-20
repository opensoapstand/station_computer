#!/bin/bash

cd /home/df-admin/drinkfill/ui/
echo "Starting to make UI"
qmake
make
echo "UI make complete"
cd /home/df-admin/drinkfill/fsm/
g++ -pthread -o controller fsm.cpp components/*.cpp states/*.cpp objects/*.cpp ../library/tinyxml/*.cpp  ../library/i2c/mcp23017/*.cpp ../library/i2c/smbus/*.c ../library/socket/*.cpp ../library/printer/*.cpp -l sqlite3 -lCppLinuxSerial
echo "FSM make complete"
echo "Done"
