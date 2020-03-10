//***************************************
//
// mcommunication.h
// payment device communication
//
// created: 04-Oct-2019
// by: Kiru Sri & Jason
//
// copyright 2019 by drinkfill
// all rights reserved
//***************************************

#ifndef mCommunication_H
#define mCommunication_H

#include <stdio.h>
#include <fcntl.h>   /* File Control Definitions           */
#include <termios.h> /* POSIX Terminal Control Definitions */
#include <unistd.h>  /* UNIX Standard Definitions 	   */
#include <errno.h>   /* ERROR Number Definitions           */
#include <fstream>
#include <iostream>
#include <vector>
#include <QDebug>

class mCommunication
{
public:
    mCommunication();
    virtual ~mCommunication();
    virtual bool init();
    virtual std::vector<uint8_t> readPacket();
    virtual std::vector<uint8_t> readForAck();
    virtual bool sendPacket(std::vector<uint8_t> fullPkt, uint pktSize);
    virtual bool sendAck();

    virtual void flushSerial();
    virtual bool closeCom();

private:
     int fd;
};

#endif // mCommunication_H
