#!/bin/bash


echo "Starting to make UI"
sudo apt-get install libcrypto++-dev libcrypto++-doc libcrypto++-utils
# qmake "CONFIG+=releaselode" 

mkdir build-ui-cli-debug
cd build-ui-cli-debug
qmake ../ui.pro -spec linux-g++ CONFIG+=debug
cp DF_UI ..
# qmake ../ui.pro -spec linux-g++ CONFIG+=debug CONFIG+=static

make

echo "UI make complete"
