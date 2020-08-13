//#include "i2c/i2c_write.h"
//#include "wire/Wire.h"

//int main()
//{
//    i2c_write i2c;
//    char address[] = "./dev/i2c-0";
//    i2c.open_i2c(address);

//    return 0;
//}



#include "mpc23017/MCP23017.h"
#include <time.h>
#include <iostream>

using namespace std;
// Basic pin reading and pullup test for the MCP23017 I/O expander
// public domain!

// Connect pin #12 of the expander to Analog 5 (i2c clock)
// Connect pin #13 of the expander to Analog 4 (i2c data)
// Connect pins #15, 16 and 17 of the expander to ground (address selection)
// Connect pin #9 of the expander to 5V (power)
// Connect pin #10 of the expander to ground (common ground)
// Connect pin #18 through a ~10kohm resistor to 5V (reset pin, active low)

// Input #0 is on pin 21 so connect a button or switch from there to ground


int main() {

  MCP23017 mcp(1, 0x27);      // use default address 0x20
  mcp.openI2C();
  //cout << mcp.openI2C() << "\n";


  //mcp.pinMode(0, INPUT);

  //                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 mcp.pullUp(0, HIGH);  // turn on a 100K pullup internally

  mcp.pinMode(15, OUTPUT);  // use the p13 LED as debugging
  mcp.pinMode(1, OUTPUT);  // use the p13 LED as debugging
  mcp.pinMode(2, OUTPUT);  // use the p13 LED as debugging

  while(true)
  {
    // The LED will 'echo' the button
    //mcp.digitalWrite(1, mcp.digitalRead(0));
    mcp.digitalWrite(15, HIGH);
    usleep(500000);
    mcp.digitalWrite(15, LOW);
    usleep(500000);

    mcp.digitalWrite(1, HIGH);
    usleep(500000);
    mcp.digitalWrite(1, LOW);
    usleep(500000);

    mcp.digitalWrite(2, HIGH);
    usleep(500000);
    mcp.digitalWrite(2, LOW);
    usleep(500000);

    mcp.writeGPIOAB(HIGH);
  }

  return 0;
}
