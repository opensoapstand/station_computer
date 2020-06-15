//***************************************
//
// <levelsensor.h>
// <Contians functions that monitor the level sensors...
// <... and shows should sent a warning when the sensor is triggered>
//
// created: <04-27-2020>
// by: <Gabriel Leung>
//
// copyright 2020 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#include <iostream>
#include <string>
#include <unistd.h>
using namespace std;

class levelSensor{

  public:

    void readValue(void);
    string command_to_string(string cmd);

};
