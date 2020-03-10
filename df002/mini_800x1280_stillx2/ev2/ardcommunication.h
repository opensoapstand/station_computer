#ifndef ardCOMMUNICATION_H
#define ardCOMMUNICATION_H

#include <stdio.h>
#include <fcntl.h>   /* File Control Definitions           */
#include <termios.h> /* POSIX Terminal Control Definitions */
#include <unistd.h>  /* UNIX Standard Definitions 	   */
#include <errno.h>   /* ERROR Number Definitions           */
#include <fstream>
#include <iostream>
#include <vector>
#include <QtSerialPort/QSerialPort>

class ardCommunication
{
public:
    ardCommunication();
    virtual bool init();
    virtual QByteArray readPacket(int sizeByte);
    virtual void writePacket(QByteArray writeChar);
    virtual bool checkConnected();
    virtual void clearVolume();

private:
    int arduDevice;
    QSerialPort serial;

};

#endif // ardCOMMUNICATION_H
