
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
    virtual bool page_init();
    virtual std::vector<uint8_t> readPacket();
    virtual std::vector<uint8_t> readForAck();
    virtual bool sendPacket(std::vector<uint8_t> fullPkt, uint pktSize);
    virtual bool sendAck();
    virtual void clearBuffer();
    virtual void flushSerial();
    virtual bool closeCom();

private:
     int fd;
     uint8_t buffer[1024] = {};
};

#endif // mCommunication_H
