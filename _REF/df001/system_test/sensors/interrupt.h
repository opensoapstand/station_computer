//***************************************
//
// <interrupt.h>
// <Defines interrupt class and functions>
//
// created: <04-27-2020>
// by: <Gabriel Leung>
//
// copyright 2020 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>
#include <poll.h>
#include <iostream>
using namespace std;

class interrupt{

  public:

    int countEDGE_LowVolume(int gpio, int calibration);
    int countEDGE_HighVolume(int gpio, int calibration);

};
