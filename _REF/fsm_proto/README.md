#GPIO allocation plan:

1) Use UDOO GPIO for flow sensors
2) Use i2c pins for solenoid activation

i2c chip Channel 1 =  MCP23017 mcp20(0, 0x20);
                      mcp20.openI2C();
                      mcp20.pinMode(pin, OUTPUT);

i2c chip Channel 2 =  MCP23017 mcp21(0, 0x21);
                      mcp21.openI2C();
                      mcp21.pinMode(pin, OUTPUT);

i2c chip Channel 1 takes care of all DRINK solenoids
i2c chip Channel 2 takes care of all WATER solenoids

                    Drink1	       Drink2	       Drink3	       Drink 4
                  ------------  ------------  ------------  -------------
Flow sensor   ||      275           280           273           278
Drink (i2c)   ||    CH1 P0        CH1 P1        CH1 P2        CH1 P3
Water (i2c)   ||    CH2 P0        CH2 P1        CH2 P2        CH2 P3
Air   (i2c)   ||    CH3 P0        CH3 P1        CH3 P2        CH3 P3

                    Drink5	       Drink6	       Drink7	       Drink 8
                  ------------  ------------  ------------  -------------
Flow sensor   ||      276           279           307           358
Drink (i2c)   ||    CH1 P4        CH1 P5        CH1 P6        CH1 P7
Water (i2c)   ||    CH2 P4        CH2 P5        CH2 P6        CH2 P7
Air   (i2c)   ||    CH3 P5        CH3 P5        CH3 P6        CH3 P7

                    Drink9
                  ------------
Flow sensor   ||      243
Drink (i2c)   ||    CH1 P8
Water (i2c)   ||    CH2 P8
Air   (i2c)   ||    CH3 P8

Level sensor1: GPIO249
Level sensor2: GPIO246
