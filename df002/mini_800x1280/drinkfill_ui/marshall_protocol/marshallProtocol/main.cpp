//***************************************
//
// main.cpp
// class for testing marshall protocol
//
// created: 12-Aug-2019
// by: Jason Wang
//
// copyright 2019 by drinkfill
// all rights reserved
//***************************************

#include <iostream>
#include "marshallprotocol.h"
#include "serialcommunications.h"

using namespace std;

int main()

{
    //serialcommunications *s1 = new serialcommunications;
    //s1->init();
    //s1->sendPacket();

    char* reset = "09000000FFFFFFFF016D0F";
    marshallProtocol *packetrecieved = new marshallProtocol;
    packetrecieved->sortMP(reset);


    cout << "Hello World!" << endl;
    return 0;
}
