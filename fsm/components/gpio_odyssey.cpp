//***************************************
//
// gpio_odyssey.cpp
// GPIO implementation for
// NATIVE x86 pins on Oddysey board
//
// created: 15-06-2020
// by: Lode Ameije, Ash Singla, Jordan Wang & Daniel Delgado
//
// copyright 2023 by Drinkfill Beverages Ltd// all rights reserved
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

#include "gpio_odyssey.h"
#include "../dftypes.h"

#define SYSFS_GPIO_DIR "/sys/class/gpio"
#define MAX_BUF 64

#ifndef INPUT
#define INPUT 1
#endif

#ifndef OUTPUT
#define OUTPUT 0
#endif

#ifndef HIGH
#define HIGH = 1
#endif
#ifndef LOW
#define LOW = 0
#endif

#define X20 20
#define X21 21
#define X22 22

#ifndef HIGH
#define HIGH = 1
#endif
#ifndef LOW
#define LOW = 0
#endif

// Default CTOR
gpio_odyssey::gpio_odyssey()
{
}

// gpio_odyssey::gpio_odyssey(messageMediator * message)
//{
//    //debugOutput::sendMessage("stateDispense(messageMediator * message)", MSG_INFO);
// }

/*
 * CTOR with Pin reference
 * Typically for Initializing sensors directly...Flow Sensors for now.
 * SEE Manual, Use last three digits to initialize class
 * i.e. GPIO 335; new gpio_odyssey(335);
 */
gpio_odyssey::gpio_odyssey(int pinNumber)
{
        setPinNumber(pinNumber);
}
void gpio_odyssey::setPinNumber(int pinNumber)
{
        std::string msg = "------gpio_odyssey------ pin:" + std::to_string(pinNumber);
        debugOutput::sendMessage(msg, MSG_INFO);

        // debugOutput::sendMessage("------gpio_odyssey------", MSG_INFO);
        int fd, len;
        char buf[MAX_BUF];

        m_nPin = pinNumber;

        fd = open(SYSFS_GPIO_DIR "/export", O_WRONLY);
        if (fd < 0)
        {
                debugOutput::sendMessage("~gpio_odyssey could not open the gpio", MSG_ERROR);
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

        using namespace std::chrono;
        uint64_t now = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();

        /* -------------------------------------------------------------------------------------- */
}

// DTOR
gpio_odyssey::~gpio_odyssey()
{
        debugOutput::sendMessage("~gpio_odyssey", MSG_INFO);
        int fd, len;
        char buf[MAX_BUF];

        fd = open(SYSFS_GPIO_DIR "/unexport", O_WRONLY);
        if (fd < 0)
        {
                debugOutput::sendMessage("~gpio_odyssey could not close the gpio", MSG_ERROR);
                return;
        }

        len = snprintf(buf, sizeof(buf), "%d", m_nPin);
        write(fd, buf, len);
        close(fd);
        return;
}

DF_ERROR gpio_odyssey::setPinAsInputElseOutput(bool input)
{
        // debugOutput::sendMessage("gpio_odyssey::setPinAsInputElseOutput ", MSG_INFO);
        std::string msg = "gpio_odyssey::setPinAsInputElseOutput " + std::to_string(m_nPin);
        debugOutput::sendMessage(msg, MSG_INFO);

        // debugOutput::sendMessage("gpio_odyssey::setPinAsInputElseOutput ", MSG_INFO);
        DF_ERROR df_ret = ERROR_MECH_FS_FAULT;
        int fd, len;
        char syscode;
        char buf[MAX_BUF];
        // char buf2[MAX_BUF];

        // Composes a string with the same text that would be printed if format was used on printf, but instead of being printed,
        // the content is stored as a C string in the buffer pointed by s
        len = snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/direction", m_nPin);
        // len2 = snprintf(buf2, sizeof(buf2), SYSFS_GPIO_DIR "/gpio%d/edge", m_nPin);

        fd = open(buf, O_WRONLY);
        // fd2 = open(buf2, O_WRONLY);
        if (fd >= 0)
        {
                if (INPUT == input)
                {
                        // cout << "direction input set" << endl;
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

// TODO: Implementation is commented out and needs testing.;
//		 A SPECIFIC function name change REQUIRED. i.e. readWastePinLevel
DF_ERROR gpio_odyssey::readPin(bool *level)
{
        debugOutput::sendMessage("readPin", MSG_INFO);
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
                                // cout << "ON" << endl;
                                *level = true;
                        }
                        else
                        {
                                // cout << "OFF" << endl;
                                *level = false;
                        }

                        close(fd);
                        df_ret = OK;
                }
        }
        else
        {
                debugOutput::sendMessage("readPin: Null Level reference", MSG_ERROR);
        }

        return df_ret;
}

// Flags Level sensor on Waste tank...
// TODO: A SPECIFIC function name change REQUIRED. i.e. triggersWasteLevel
DF_ERROR gpio_odyssey::writePin(bool level)
{
        debugOutput::sendMessage("OdysseyX86 writePin : " + std::to_string(m_nPin) + " value: " + std::to_string(level), MSG_INFO);
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
