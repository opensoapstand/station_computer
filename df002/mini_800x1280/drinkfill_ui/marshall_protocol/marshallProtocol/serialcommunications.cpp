//***************************************
//
// gpio.cpp
// debug a log interface
//
// created: 13-June-2019
// by: Kiru Sri
//
// copyright 2019 by drinkfill
// all rights reserved
//***************************************

///usleep is in microns or microseconds
#include "serialcommunications.h"


#define  NUMATO_PORT "/dev/ttyUSB0"
#define  NUMATO_READ "gpio read %c\r"
#define  NUMATO_SET  "gpio set %c\r"
#define  NUMATO_CLEAR "gpio clear %c\r"
#define  NUMATO_PAUSE 5000



serialcommunications::serialcommunications(){
   fd = -1;
}
serialcommunications::~serialcommunications() {
}

bool serialcommunications::init(){

   bool bRet = false;

    fd = open(NUMATO_PORT ,O_RDWR | O_NOCTTY | O_NDELAY);
                                /* O_RDWR Read/Write access to serial port           */
                                /* O_NOCTTY - No terminal will control the process   */
                                /* O_NDELAY -Non Blocking Mode,Does not care about-  */
                                /* -the status of DCD line,Open() returns immediatly */

    if(fd == -1) {
        cout<<"serialclosed"<<endl;
        exit;
    }
    else {

      //set the serial port setting for Numato h/w

      struct termios SerialPortSettings;	/* Create the structure                          */

      tcgetattr(fd, &SerialPortSettings);	/* Get the current attributes of the Serial port */

      cfsetispeed(&SerialPortSettings,B115200); /* Set Read  Speed as 19200                       */
      cfsetospeed(&SerialPortSettings,B115200); /* Set Write Speed as 19200                       */

      SerialPortSettings.c_cflag &= ~PARENB;   /* Disables the Parity Enable bit(PARENB),So No Parity   */
      SerialPortSettings.c_cflag &= ~CSTOPB;   /* CSTOPB = 2 Stop bits,here it is cleared so 1 Stop bit */
      SerialPortSettings.c_cflag &= ~CSIZE;	 /* Clears the mask for setting the data size             */
      SerialPortSettings.c_cflag |=  CS8;      /* Set the data bits = 8                                 */
      SerialPortSettings.c_cflag &= ~CRTSCTS;       /* No Hardware flow Control                         */
      SerialPortSettings.c_cflag |= CREAD | CLOCAL; /* Enable receiver,Ignore Modem Control lines       */
      SerialPortSettings.c_iflag &= (IXON | IXOFF | IXANY);          /* Disable XON/XOFF flow control both i/p and o/p */
      SerialPortSettings.c_iflag &= ~(ICANON | ECHO | ECHOE | ISIG);  /* Non Cannonical mode            */
      SerialPortSettings.c_oflag &= ~OPOST;/*No Output Processing*/

      if((tcsetattr(fd,TCSANOW,&SerialPortSettings)) != 0) {
      } else {
            tcflush(fd,TCIOFLUSH);

         bRet = true;
      }
    }
;
    return bRet;
}

bool serialcommunications::readPacket(){


}

void serialcommunications::sendPacket(){ //Sets pinID on
   char readerEnable[20];
   readerEnable[0] = 0x0B;
   readerEnable[1] = 0x00;
   readerEnable[2] = 0x01;
   readerEnable[3] = 0x2F;
   readerEnable[4] = 0x01;
   readerEnable[5] = 0x53;
   readerEnable[6] = 0x00;
   readerEnable[7] = 0x30;
   readerEnable[8] = 0x80;
   readerEnable[9] = 0x14;
   readerEnable[10] = 0x01;
   readerEnable[11] = 0xCD;
   readerEnable[12] = 0x5D;
   readerEnable[13] = 0x0a;
   readerEnable[14] = 0x00;
   readerEnable[15] = 0x00;
   readerEnable[16] = 0x00;
   readerEnable[17] = 0x00;
   readerEnable[18] = 0x00;
   readerEnable[19] = 0x00;


   int writesize = write(fd, readerEnable, sizeof (readerEnable));
   tcflush(fd, TCIOFLUSH);
   char buffer[128];
   int readsize = -1;
   while (readsize == -1){
       readsize= read(fd, buffer, sizeof (buffer));
       int x;
   }

   sleep(1);
   readerEnable[0] = 0x0c;
   readerEnable[1] = 0x00;
   readerEnable[2] = 0x01;
   readerEnable[3] = 0x01;
   readerEnable[4] = 0x00;
   readerEnable[5] = 0x30;
   readerEnable[6] = 0x01;
   readerEnable[7] = 0x53;
   readerEnable[8] = 0x80;
   readerEnable[9] = 0x03;
   readerEnable[10] = 0xb8;
   readerEnable[11] = 0x0b;
   readerEnable[12] = 0x70;
   readerEnable[13] = 0xd7;
   readerEnable[14] = 0x0a;
   readerEnable[15] = 0x00;
   readerEnable[16] = 0x00;
   readerEnable[17] = 0x00;
   readerEnable[18] = 0x00;
   readerEnable[19] = 0x00;

   writesize = write(fd, readerEnable, sizeof (readerEnable));
   readsize = -1;
      while (readsize == -1){
          readsize= read(fd, buffer, sizeof (buffer));
          int x;
      }
      sleep (1);
   readerEnable[0] = 0x0f;
   readerEnable[1] = 0x00;
   readerEnable[2] = 0x01;
   readerEnable[3] = 0x38;
   readerEnable[4] = 0x01;
   readerEnable[5] = 0x53;
   readerEnable[6] = 0x00;
   readerEnable[7] = 0x30;
   readerEnable[8] = 0x80;
   readerEnable[9] = 0x13;
   readerEnable[10] = 0x00;
   readerEnable[11] = 0x14;
   readerEnable[12] = 0x20;
   readerEnable[13] = 0x06;
   readerEnable[14] = 0x00;
   readerEnable[15] = 0xf4;
   readerEnable[16] = 0x3f;
   readerEnable[17] = 0x0a;
   readerEnable[18] = 0x00;
   readerEnable[19] = 0x00;

   writesize = write(fd, readerEnable, sizeof (readerEnable));
   readsize = -1;
      while (readsize == -1){
          readsize= read(fd, buffer, sizeof (buffer));
          int x;
      }
int y;
y=4;
}

