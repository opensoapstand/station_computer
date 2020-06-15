//***************************************
//
// <interrupt.cpp>
// <Contains functions that enables GPIO interrupt function...
// <... and utilizes it for flow sensor edge counting>
//
// created: <04-27-2020>
// by: <Gabriel Leung>
//
// copyright 2020 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#include "interrupt.h"

int interrupt::countEDGE_LowVolume(int gpio, int calibration){

    struct pollfd pfd;
    int fd;
    int counter = 0;
    int DISPENSE_VOLUME = 350;
    int DISPENSE_VOLUME_CALIBRATED = DISPENSE_VOLUME*calibration;
    char buf[8];

    std::cout << "\nSystem ready for dispense sequence...\n\n";

    std::string GPIO = std::to_string(gpio);
    string command("/sys/class/gpio/gpio");
    command += GPIO;
    command += "/value";

    fd = open(command.c_str(), O_RDONLY);
    //fd = open("/sys/class/gpio/gpio275/value", O_RDONLY);

    pfd.fd = fd;
    pfd.events = POLLPRI;

    while(counter < DISPENSE_VOLUME_CALIBRATED){

		lseek(fd, 0, SEEK_SET);    /* consume any prior interrupt */
		read(fd, buf, sizeof buf);

		poll(&pfd, 1, -1);         /* wait for interrupt */

		lseek(fd, 0, SEEK_SET);    /* consume interrupt */
		read(fd, buf, sizeof buf);

		counter = counter + 1;
		cout << counter << endl;
   }
    return counter;
  }

int interrupt::countEDGE_HighVolume(int gpio, int calibration){

    struct pollfd pfd;
    int fd;
    int counter = 0;
    int DISPENSE_VOLUME = 550;
    int DISPENSE_VOLUME_CALIBRATED = DISPENSE_VOLUME*calibration;
    char buf[8];

    std::cout << "\nSystem ready for dispense sequence...\n\n";

    std::string GPIO = std::to_string(gpio);
    string command("/sys/class/gpio/gpio");
    command += GPIO;
    command += "/value";

    fd = open(command.c_str(), O_RDONLY);
    //fd = open("/sys/class/gpio/gpio275/value", O_RDONLY);

    pfd.fd = fd;
    pfd.events = POLLPRI;

    while(counter < DISPENSE_VOLUME_CALIBRATED){

        lseek(fd, 0, SEEK_SET);    /* consume any prior interrupt */
        read(fd, buf, sizeof buf);

        poll(&pfd, 1, -1);         /* wait for interrupt */

        lseek(fd, 0, SEEK_SET);    /* consume interrupt */
        read(fd, buf, sizeof buf);

        counter = counter + 1;
        cout << counter << endl;
    }
    return counter;
  }
