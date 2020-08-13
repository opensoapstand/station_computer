//***************************************
//
// <gpio.h>
// <Defines gpio class and functions>
//
// created: <04-27-2020>
// by: <Gabriel Leung>
//
// copyright 2020 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#include <iostream>
using namespace std;

class gpio{

  public:

    void activateGPIO(int gpio);
    void setGPIO_input(int gpio);
    void setGPIO_output(int gpio);
    void setGPIO_high(int gpio);
    void setGPIO_low(int gpio);
    void setGPIO_edge_both(int gpio);
    void setGPIO_edge_rising(int gpio);
    void setGPIO_edge_falling(int gpio);

};
