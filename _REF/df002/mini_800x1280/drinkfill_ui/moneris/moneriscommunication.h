//***************************************
//
// moneriscommunication.h
// moneris payment device communication
//
// created: 04-Oct-2019
// by: Kiru Sri & Jason
//
// copyright 2019 by drinkfill
// all rights reserved
//***************************************

#ifndef MONERISCOMMUNICATION_H
#define MONERISCOMMUNICATION_H

#include <stdio.h>
#include <fcntl.h>   /* File Control Definitions           */
#include <termios.h> /* POSIX Terminal Control Definitions */
#include <unistd.h>  /* UNIX Standard Definitions 	   */
#include <errno.h>   /* ERROR Number Definitions           */
#include <fstream>
#include <iostream>
#include <vector>

class monerisCommunication
{
public:
    monerisCommunication();
    virtual ~monerisCommunication();
    virtual bool init();
    virtual std::vector<uint8_t> readPacket();
    virtual std::vector<uint8_t> readForAck();
    virtual bool sendPacket(std::vector<uint8_t> fullPkt, uint pktSize);

private:
     int fd;
};

#endif // MONERISCOMMUNICATION_H
