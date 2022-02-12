//***************************************
//
// oddyseyX86gpio.cpp
// GPIO implementation for
// NATIVE x86 pins on Oddysey board
//
// created: 15-06-2020
// by: Lode Ameije & Ash Singla
//
// copyright 2022 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <poll.h>
#include <iostream>

#include "odysseyx86gpio.h"

#define SYSFS_GPIO_DIR "/sys/class/gpio"
#define MAX_BUF 64

#define INPUT 1
#define OUTPUT 0

#define X20 20
#define X21 21
#define X22 22

#define HIGH = 1
#define LOW = 0

// Default CTOR
oddyseyx86GPIO::oddyseyx86GPIO()
{
}

//oddyseyx86GPIO::oddyseyx86GPIO(messageMediator * message)
//{
//   //debugOutput::sendMessage("stateDispense(messageMediator * message)", INFO);
//}

/* 
 * CTOR with Pin reference
 * Typically for Initializing sensors directly...Flow Sensors for now.
 * SEE Manual, Use last three digits to initialize class
 * i.e. GPIO 335; new oddyseyx86GPIO(335);
 */
oddyseyx86GPIO::oddyseyx86GPIO(int pinNumber)
{
        std::string msg = "------oddyseyx86GPIO------ pin:" + std::to_string(pinNumber);
        debugOutput::sendMessage(msg, INFO);

        // debugOutput::sendMessage("------oddyseyx86GPIO------", INFO);
        int fd, len;
        char buf[MAX_BUF];

        m_nPin = pinNumber;

        fd = open(SYSFS_GPIO_DIR "/export", O_WRONLY);
        if (fd < 0)
        {
                debugOutput::sendMessage("~oddyseyx86GPIO could not open the gpio", ERROR);
                return;
        }

        len = snprintf(buf, sizeof(buf), "%d", m_nPin);
        write(fd, buf, len);
        close(fd);

        /* ------------------------- TODO: THIS NEEDS TO BE DONE BETTER ------------------------ */

        string GPIO = std::to_string(m_nPin);
        string command("echo 'D@nkF1ll$' | sudo -S chmod a+w /sys/class/gpio/gpio");
        command += GPIO;
        string command_dir = command + "/direction";
        string command_edg = command + "/edge";
        string command_val = command + "/value";

        system(command_dir.c_str());
        system(command_edg.c_str());
        system(command_val.c_str());

        /* -------------------------------------------------------------------------------------- */

        return;
}

// DTOR
oddyseyx86GPIO::~oddyseyx86GPIO()
{
        debugOutput::sendMessage("~oddyseyx86GPIO", INFO);
        int fd, len;
        char buf[MAX_BUF];

        fd = open(SYSFS_GPIO_DIR "/unexport", O_WRONLY);
        if (fd < 0)
        {
                debugOutput::sendMessage("~oddyseyx86GPIO could not close the gpio", ERROR);
                return;
        }

        len = snprintf(buf, sizeof(buf), "%d", m_nPin);
        write(fd, buf, len);
        close(fd);
        return;
}

// Setter for flow sensor on Odyssey GPIO Pin
DF_ERROR oddyseyx86GPIO::setFlowPin(int pinNumber)
{
        DF_ERROR df_Ret = ERROR_BAD_PARAMS;

        //may need a set of array for avilable x86 pins to verify
        m_nPin = pinNumber;
        df_Ret = OK;

        return df_Ret;
}

// Setter for Direction of flow sensor on Odyssey GPIO Pin
// Writes "in" into a GPIO direction file while
// reading input and "out" otherwise.
DF_ERROR oddyseyx86GPIO::setDirection(bool input)
{
        // debugOutput::sendMessage("oddyseyx86GPIO::setDirection ", INFO);
        std::string msg = "oddyseyx86GPIO::setDirection " + std::to_string(m_nPin);
        debugOutput::sendMessage(msg, INFO);

        //debugOutput::sendMessage("oddyseyx86GPIO::setDirection ", INFO);
        DF_ERROR df_ret = ERROR_MECH_FS_FAULT;
        int fd, len;
        char syscode;
        char buf[MAX_BUF];
        //char buf2[MAX_BUF];

        //Composes a string with the same text that would be printed if format was used on printf, but instead of being printed,
        //the content is stored as a C string in the buffer pointed by s
        len = snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/direction", m_nPin);
        //len2 = snprintf(buf2, sizeof(buf2), SYSFS_GPIO_DIR "/gpio%d/edge", m_nPin);

        fd = open(buf, O_WRONLY);
        //fd2 = open(buf2, O_WRONLY);
        if (fd >= 0)
        {
                if (INPUT == input)
                {
                        //cout << "direction input set" << endl;
                        write(fd, "in", 3);
                        // cout << "edge set" << endl;
                        // write(fd2, "rising", 7);
                }
                else
                        write(fd, "out", 4);

                close(fd);
                //      close(fd2);
                df_ret = OK;
        }

        return df_ret;
}

// Checks for Level sensor on Waste tank...
// TODO: Implementation is commented out and needs testing.;
//		 A SPECIFIC function name change REQUIRED. i.e. readWastePinLevel
DF_ERROR oddyseyx86GPIO::readPin(bool *level)
{
        debugOutput::sendMessage("readPin", INFO);
        DF_ERROR df_ret = ERROR_MECH_FS_FAULT;
        int fd, len;
        char buf[MAX_BUF];
        char ch;

        // test to see if level exists
        if (NULL != level)
        {
                len = snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/value", m_nPin);

                fd = open(buf, O_RDONLY);
                if (fd >= 0)
                {
                        read(fd, &ch, 1);

                        if (ch != '0')
                        {
                                //cout << "ON" << endl;
                                *level = true;
                        }
                        else
                        {
                                //cout << "OFF" << endl;
                                *level = false;
                        }

                        close(fd);
                        df_ret = OK;
                }
        }
        else
        {
                debugOutput::sendMessage("readPin: Null Level reference", ERROR);
        }

        return df_ret;
}

// Flags Level sensor on Waste tank...
// TODO: A SPECIFIC function name change REQUIRED. i.e. triggersWasteLevel
DF_ERROR oddyseyx86GPIO::writePin(bool level)
{
        debugOutput::sendMessage("OdysseyX86 writePin", INFO);
        DF_ERROR df_ret = ERROR_MECH_FS_FAULT;
        int fd, len;
        char buf[MAX_BUF];

        len = snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/value", m_nPin);

        fd = open(buf, O_WRONLY);
        if (fd >= 0)
        {
                if (level)
                        write(fd, "1", 2);
                else
                        write(fd, "0", 2);

                close(fd);
                df_ret = OK;
        }

        return df_ret;
}

//void oddyseyx86GPIO::monitorGPIO()
//{
//    int fd;
//    char local_buffer[16];
//    struct pollfd pfd;

//    string GPIO = std::to_string(m_nPin);
//    string command("/sys/class/gpio/gpio");
//    command += GPIO;
//    command += "/edge";
//    //debugOutput::sendMessage(command.c_str(), INFO);

//    // Set the pin to interrupt
//    fd = open(command.c_str(), O_WRONLY);
//    write(fd, "rising", 6);
//    close(fd);

//    command = "/sys/class/gpio/gpio" + GPIO + "/value";
//    fd = open(command.c_str(), O_RDONLY);

//    pfd.fd = fd;
//    pfd.events = POLLPRI | POLLERR;

//    // Clear out any old interrupts
//    lseek(fd, 0, SEEK_SET);
//    read (fd, local_buffer, sizeof(local_buffer));

//    // Block waiting for the interrupt
//    int ret = poll(&pfd, 1, 5000);

//    if (0 == ret)
//    {
//        //debugOutput::sendMessage("gpioTimeout", INFO);
//    }
//    else
//    {
//        //debugOutput::sendMessage("FLOW TICK", INFO);
//        m_pProduct->registerFlowSensorTick(); //trigger the callback
//    }

//    close(fd);

//    return;
//}

// Threaded function call to monitor Odyssecy GPIO pin activity.

// Threaded function call to monitor Odyssecy GPIO pin activity.

void oddyseyx86GPIO::monitorGPIO()
{
        //debugOutput::sendMessage("monitorGPIO", INFO);  //nuke this later it will cause so much spam
        int fd, len;
        char buf[MAX_BUF];
        char compareChar;
        struct pollfd pfd;

        //string GPIO = std::to_string(m_nPin);
        string GPIO = "364";
        string command("/sys/class/gpio/gpio");
        command += GPIO;
        command += "/edge";

        // set the pin to interrupt
        fd = open(command.c_str(), O_WRONLY);
        write(fd, "rising", 4);
        close(fd);

        command = "/sys/class/gpio/gpio" + GPIO + "/value";
        fd = open(command.c_str(), O_RDONLY);
        pfd.fd = fd;
        pfd.events = POLLPRI | POLLERR;

        lseek(fd, 0, SEEK_SET);
        int ret = poll(&pfd, 1, 100000);
        char c;
        read(fd, &c, 1);

        if (0 == ret)
        {
                //debugOutput::sendMessage("gpioTimeout", INFO);
        }
        else
        {
                if (('1' == c) && (compareChar == '0'))
                {
                        //                        debugOutput::sendMessage("HIGH Triggered Flow", INFO);
                        //  usleep(500000);						// Sleep to make sure debug gets chance to print
                        m_pProduct->registerFlowSensorTick(); //trigger the callback
                                                            //                        cout << "Registered Tick" << endl;
                }
                else if (('0' == c) && (compareChar == '1'))
                {
                        //                         debugOutput::sendMessage("LOW Triggered Flow", INFO);
                        //   usleep(500000); // Sleep to make sure debug gets chance to print
                }
                compareChar = c;
        }
        close(fd);
        return;
}

void oddyseyx86GPIO::monitorGPIO_PWR()
{
        //        debugOutput::sendMessage("monitorGPIO", INFO);  //nuke this later it will cause so much spam
        int fd, len;
        char buf[MAX_BUF];

        struct pollfd pfd;

        string GPIO = ("391");
        string command("/sys/class/gpio/gpio");
        command += GPIO;
        command += "/edge";

        // set the pin to interrupt
        fd = open(command.c_str(), O_WRONLY);
        write(fd, "rising", 4);
        close(fd);

        command = "/sys/class/gpio/gpio" + GPIO + "/value";
        fd = open(command.c_str(), O_RDONLY);
        pfd.fd = fd;
        pfd.events = POLLPRI | POLLERR;

        lseek(fd, 0, SEEK_SET);
        int ret = poll(&pfd, 1, 100000);
        char c;
        read(fd, &c, 1);

        if (0 == ret)
        {
                //debugOutput::sendMessage("gpioTimeout", INFO);
        }
        else
        {
                if ((c == '1') && (compareChar2 != c))
                {
                        debugOutput::sendMessage("Power button pushed", INFO);
                        usleep(1000000);
                        if (readButtonPin(341))
                        {
                                debugOutput::sendMessage("POWER OFF\n", INFO);
                                system("echo D@nkF1ll$ | sudo -S shutdown -h now");
                        }
                        else if (readButtonPin(340))
                        {
                                debugOutput::sendMessage("MM\n", INFO);
                                // ENTER MAINTENANCE MODE!
                                m_pMessaging->sendMessage("MM");
                        }
                        else
                        {
                                debugOutput::sendMessage("Nothing", INFO);
                        }
                        usleep(5000); // Sleep to make sure debug gets chance to print
                }
        }
        compareChar2 = c;
        close(fd);
        return;
}

bool oddyseyx86GPIO::readButtonPin(int pin)
{

        int fd, len;
        char buf[MAX_BUF];
        char ch;
        len = snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/value", pin);

        fd = open(buf, O_RDONLY);
        if (fd >= 0)
        {
                read(fd, &ch, 1);

                if (ch == '0')
                {

                        close(fd);
                        return true;
                }
                else if (ch == '1')
                {

                        close(fd);
                        return false;
                }
        }
}

// Utility
// TODO: Should be interpreting for flow sensor buffer...this does not seem to provide value...
string oddyseyx86GPIO::command_to_string(string cmd)
{

        string data;
        FILE *stream;
        const int max_buffer = 256;
        char buffer[max_buffer];
        cmd.append(" 2>&1");

        stream = popen(cmd.c_str(), "r");
        if (stream)
        {
                while (!feof(stream))
                        if (fgets(buffer, max_buffer, stream) != NULL)
                                data.append(buffer);
                pclose(stream);
        }
        return data;
}
