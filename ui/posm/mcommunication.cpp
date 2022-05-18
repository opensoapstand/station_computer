//***************************************
//
// mcommunication.cpp
// payment device communication
//
// created: 04-Oct-2019
// by: Kiru Sri & Jason
//
// copyright 2019 by drinkfill
// all rights reserved
//***************************************

#include "mcommunication.h"

#define  MONERIS_PORT "/dev/mpos"
#define  MONERIS_READ "gpio read %c\r"
#define  MONERIS_SET  "gpio set %c\r"
#define  MONERIS_CLEAR "gpio clear %c\r"
#define  MONERIS_PAUSE 5000
#define  MAX_SIZE 1024

#include <QDebug>

mCommunication::mCommunication(){
    fd = -1;
}

mCommunication::~mCommunication() {
}

bool mCommunication::page_init(){

    bool bRet = false;

    fd = open(MONERIS_PORT ,O_RDWR | O_NOCTTY| O_NDELAY);
    /* O_RDWR Read/Write access to serial port           */
    /* O_NOCTTY - No terminal will control the process   */
    /* O_NDELAY -Non Blocking Mode,Does not care about-  */
    /* -the status of DCD line,Open() returns immediatly */

    if(fd == -1) {
        qDebug() << "Serial (mpos) Status: "<< "Failed";
        return bRet;
    }
    else {

        //set the serial port setting for MONERIS h/w

        struct termios SerialPortSettings;	/* Create the structure                          */

        tcgetattr(fd, &SerialPortSettings);	/* Get the current attributes of the Serial port */

        cfsetispeed(&SerialPortSettings, speed_t(B9600)); /* Set Read  Speed as 9600                      */
        cfsetospeed(&SerialPortSettings, speed_t(B9600)); /* Set Write Speed as 9600                       */

        SerialPortSettings.c_cflag &= ~PARENB;   /* Disables the Parity Enable bit(PARENB),So No Parity   */
        SerialPortSettings.c_cflag &= ~CSTOPB;   /* CSTOPB = 2 Stop bits,here it is cleared so 1 Stop bit */
        SerialPortSettings.c_cflag &= ~CSIZE;	 /* Clears the mask for setting the data size             */
        SerialPortSettings.c_cflag |=  CS8;      /* Set the data bits = 8                                 */
        SerialPortSettings.c_cflag &= ~CRTSCTS;       /* No Hardware flow Control                         */
        SerialPortSettings.c_cflag |= CREAD | CLOCAL; /* Enable receiver,Ignore Modem Control lines       */
        SerialPortSettings.c_iflag &= (IXON | IXOFF | IXANY);          /* Disable XON/XOFF flow control both i/p and o/p */
        SerialPortSettings.c_iflag &= ~(ICANON | ECHO | ECHOE | ISIG);  /* Non Cannonical mode            */
        SerialPortSettings.c_oflag &= ~OPOST;/*No Output Processing*/
        SerialPortSettings.c_cc[VMIN]   =  1;                  // read doesn't block
        SerialPortSettings.c_cc[VTIME]  =  5;                  // 0.5 seconds read timeout
        /* Make raw */
        cfmakeraw(&SerialPortSettings);

        if((tcsetattr(fd,TCSANOW,&SerialPortSettings)) != 0) {
        } else {
            tcflush(fd,TCIOFLUSH);

            bRet = true;
        }
        qDebug() << "Serial (mpos) Status: "<< "Success";
        return bRet;
    };

}

std::vector<uint8_t> mCommunication::readPacket(){

    buffer[1024] = {};
    long int readSize = -1;
    long int readSize2 = -1;
    std::vector<uint8_t> pktRead;
    //tcflush(fd, TCIOFLUSH);
    //tcflush(fd, TCIOFLUSH);
    
    readSize = read(fd, buffer, MAX_SIZE);
    
    if (readSize == -1)
    {
        pktRead.clear();
        pktRead.push_back(0xFF);
        std::cout << "Packet read failed...\n";
        return pktRead;
    }
//    else if (int(buffer[2]) < readSize) {
//        std::cout << "buffer[2] = " << int(buffer[3]) << " readSzie: " << readSize ;
//        readPacket();
//    }
    else{

        pktRead.reserve(uint(readSize));
        for (long i = 0; i < readSize; i++){ //store read bytes into vector
            pktRead.push_back(buffer[i]);
            std::cout << "buffer[" << i << "i] = " << int(buffer[i]) << " \n";
        }

//        std::cout << "buffer1 + buffer2 = " << (int(buffer[1]) + int(buffer[2])) << "\n" << "readSize-3 = " << readSize-0x05 << "\n" << "readSize = " << readSize << "\n";

//        std::cout << "pktRead.end()[-2]= " << pktRead.end()[-2] << "\n";
        while ((pktRead.end()[-2] != 0x03) && ((int(buffer[1]) + int(buffer[2])) != readSize-0x05)){
//            std::cout << "True\n";
            readSize2 = read(fd, buffer, MAX_SIZE);
            if (readSize2 == -1)
            {
//                std::cout << "readSize2 = -1\n";
                pktRead.clear();
                pktRead.push_back(0xFF);
                return pktRead;
            }else {
//
                pktRead.reserve(uint(readSize2));
                for (long i = 0; i < readSize2; i++){ //store read bytes into vector
                    pktRead.push_back(buffer[i]);
                    std::cout << "buffer[" << i << "i] = " << int(buffer[i]) << " \n";
                }
        }

        //tcflush(fd, TCIOFLUSH);
        buffer[1024] = {};
        return pktRead;
    }
}
}

void mCommunication::clearBuffer(){
    buffer[1024] = {};
}

std::vector<uint8_t> mCommunication::readForAck()
{

    uint8_t buffer[1] = {};
    long int readSize = -1;
    std::vector<uint8_t> pktRead;
    //tcflush(fd, TCIOFLUSH);
    //tcflush(fd, TCIOFLUSH);
    int readcount = 0;

    while (readcount < 3){
        if (readSize == -1){
            readSize = read(fd, buffer, 1);
            readcount ++;
        }
        if (readSize != -1){
            break;
        }
        usleep(500000);
    }


    if (readSize == -1)
    {
        std::cout << "Read failed -1";
        pktRead.clear();
        pktRead.push_back(0xFF);
        return pktRead;
    }
    else{
        std::cout << "read size" << readSize << endl;
        pktRead.reserve(uint(readSize));
        for (int i = 0; i < readSize; i++){ //store read bytes into vector
            pktRead.push_back(buffer[i]);
        }

 //       tcflush(fd, TCIOFLUSH);
        return pktRead;
    }
}

bool mCommunication::sendPacket(std::vector<uint8_t> fullPkt, uint pktSize){ //Sets pinID on
    bool bRet = false;
    long int writeSize =  -1;
    char sendBuffer[MAX_SIZE]={};

    for (uint i = 0; i < pktSize; ++i){
        sendBuffer[i] = char(fullPkt[i]);
        }

    writeSize= write(fd, sendBuffer, pktSize);
    //writeSize =write(fd, &fullPkt, fullPkt.size());
    //tcflush(fd, TCIOFLUSH);
    //qDebug() <<writeSize<<"\n";
    if (-1 != writeSize)
        bRet = true;

    //usleep();
    return bRet;
}

bool mCommunication::sendAck()
{
    char sendBuffer[1];
    sendBuffer[0] = 0x06;
    write (fd, sendBuffer,1);
    return true;
}


void mCommunication::flushSerial()
{
    std::cout << "Flushing Serial port..." << std::endl;
    tcflush(fd, TCIOFLUSH);
}

bool mCommunication::closeCom()
{
    close(fd);

    return true;
}

