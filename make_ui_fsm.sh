#!/bin/bash

# cd ui/payment/
# echo "Compiling payment module"
# g++ -o command commands.cpp  setup_Tap.cpp -lssl -lcrypto
# echo "Compilation Completed Payment"
cd ui/
echo "Starting to make UI"
qmake
make
echo "UI make complete"
cd ../fsm/
g++ -pthread -o controller fsm.cpp components/*.cpp states/*.cpp objects/*.cpp ../library/tinyxml/*.cpp  ../library/i2c/mcp23017/*.cpp ../library/i2c/smbus/*.c ../library/socket/*.cpp ../library/printer/*.cpp ../library/qr/*.cpp -l sqlite3 -lCppLinuxSerial -lcurl
echo "FSM make complete"
echo "Done"
