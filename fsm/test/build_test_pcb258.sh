#!/bin/sh
# g++ -o test_8344 test_8344.cpp ../fsm/objects/debugOutput.cpp ../fsm/components/dsed8344.cpp ../fsm/smbus/smbus.c
g++ -g -o EN258_pcb_test test_pcbEN258.cpp ../objects/debugOutput.cpp ../components/pcb.cpp ../smbus/smbus.c ../components/gpio.cpp  -pthread
