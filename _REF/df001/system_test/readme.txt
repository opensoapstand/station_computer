//g++ -o system_test *.cpp

//g++ -o system_test *.cpp mcp23017/mcp23017.cpp smbus/smbus.

g++ -o system_test *.cpp mcp23017/mcp23017.cpp smbus/smbus.c sensors/*.cpp

g++ -o system_test *.cpp sensors/*.cpp  mcp23017/mcp23017.cpp smbus/smbus.c
------------------------------------------------------------
I2c chip address  --> 20, 21, 22 [follow gpios.h for pin out controls] 

