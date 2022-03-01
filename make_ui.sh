#!/bin/bash

cd ui/
echo "Starting to make UI"
qmake
make
echo "UI make complete"
echo "Use make_ui_fsm to also compile FSM"
echo "Done"
