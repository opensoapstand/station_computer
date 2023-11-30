#!/bin/sh
# g++ -o test_8344 test_8344.cpp ../fsm/objects/debugOutput.cpp ../fsm/components/dsed8344.cpp ../fsm/smbus/smbus.c
g++ -g -o test_generic_pcb test_generic_pcb.cpp ../objects/debugOutput.cpp ../components/pcb.cpp ../components/gpio_odyssey.cpp ../components/gpio.cpp ../smbus/smbus.c -pthread
