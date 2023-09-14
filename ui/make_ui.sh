#!/bin/bash


echo "Starting to make UI"
sudo apt-get install libcrypto++-dev libcrypto++-doc libcrypto++-utils
qmake
make
echo "UI make complete"
