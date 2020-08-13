#ifndef SERIALCOMMUNICATIONS_H
#define SERIALCOMMUNICATIONS_H

#include <stdio.h>
#include <fcntl.h>   /* File Control Definitions           */
#include <termios.h> /* POSIX Terminal Control Definitions */
#include <unistd.h>  /* UNIX Standard Definitions 	   */
#include <errno.h>   /* ERROR Number Definitions           */
#include <fstream>
#include <iostream>
//#include <QByteArray>


using namespace std;





class serialcommunications{


public:
     serialcommunications();
     virtual ~serialcommunications();
     virtual bool init();
     virtual bool readPacket();
     virtual void sendPacket();

private:
     int fd;
};


#endif // SERIALCOMMUNICATIONS_H
