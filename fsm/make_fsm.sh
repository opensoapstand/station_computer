#!/bin/bash
echo "Start make controller..."
g++ -pthread -o controller fsm.cpp components/*.cpp states/*.cpp objects/*.cpp ../library/tinyxml/*.cpp  ../library/i2c/mcp23017/*.cpp ../library/i2c/smbus/*.c ../library/socket/*.cpp ../library/printer/*.cpp ../library/qr/*.cpp -l sqlite3 -lCppLinuxSerial -lcurl
echo "Controller make complete"