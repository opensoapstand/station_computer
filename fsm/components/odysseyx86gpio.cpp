//***************************************
//
// oddyseyX86gpio.cpp
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

#include "odysseyx86gpio.h"
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

// Default CTOR
oddyseyx86GPIO::oddyseyx86GPIO()
{
}

// oddyseyx86GPIO::oddyseyx86GPIO(messageMediator * message)
//{
//    //debugOutput::sendMessage("stateDispense(messageMediator * message)", MSG_INFO);
// }

/*
 * CTOR with Pin reference
 * Typically for Initializing sensors directly...Flow Sensors for now.
 * SEE Manual, Use last three digits to initialize class
 * i.e. GPIO 335; new oddyseyx86GPIO(335);
 */
oddyseyx86GPIO::oddyseyx86GPIO(int pinNumber)
{
        std::string msg = "------oddyseyx86GPIO------ pin:" + std::to_string(pinNumber);
        debugOutput::sendMessage(msg, MSG_INFO);

        // debugOutput::sendMessage("------oddyseyx86GPIO------", MSG_INFO);
        int fd, len;
        char buf[MAX_BUF];

        m_nPin = pinNumber;

        fd = open(SYSFS_GPIO_DIR "/export", O_WRONLY);
        if (fd < 0)
        {
                debugOutput::sendMessage("~oddyseyx86GPIO could not open the gpio", MSG_ERROR);
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
        flowsensor_most_recent_edge_millis = now;
        flowsensor_stable_state = false;

        /* -------------------------------------------------------------------------------------- */

        return;
}

// DTOR
oddyseyx86GPIO::~oddyseyx86GPIO()
{
        debugOutput::sendMessage("~oddyseyx86GPIO", MSG_INFO);
        int fd, len;
        char buf[MAX_BUF];

        fd = open(SYSFS_GPIO_DIR "/unexport", O_WRONLY);
        if (fd < 0)
        {
                debugOutput::sendMessage("~oddyseyx86GPIO could not close the gpio", MSG_ERROR);
                return;
        }

        len = snprintf(buf, sizeof(buf), "%d", m_nPin);
        write(fd, buf, len);
        close(fd);
        return;
}

// // Setter for flow sensor on Odyssey GPIO Pin
// DF_ERROR oddyseyx86GPIO::setFlowPin(int pinNumber)
// {
//         DF_ERROR df_Ret = ERROR_BAD_PARAMS;

//         //may need a set of array for avilable x86 pins to verify
//         m_nPin = pinNumber;
//         df_Ret = OK;

//         return df_Ret;
// }

// Setter for Direction of flow sensor on Odyssey GPIO Pin
// Writes "in" into a GPIO direction file while
// reading input and "out" otherwise.

DF_ERROR oddyseyx86GPIO::setPinAsInputElseOutput(bool input)
{
        // debugOutput::sendMessage("oddyseyx86GPIO::setPinAsInputElseOutput ", MSG_INFO);
        std::string msg = "oddyseyx86GPIO::setPinAsInputElseOutput " + std::to_string(m_nPin);
        debugOutput::sendMessage(msg, MSG_INFO);

        // debugOutput::sendMessage("oddyseyx86GPIO::setPinAsInputElseOutput ", MSG_INFO);
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

// Checks for Level sensor on Waste tank...
// TODO: Implementation is commented out and needs testing.;
//		 A SPECIFIC function name change REQUIRED. i.e. readWastePinLevel
DF_ERROR oddyseyx86GPIO::readPin(bool *level)
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
DF_ERROR oddyseyx86GPIO::writePin(bool level)
{
        debugOutput::sendMessage("OdysseyX86 writePin", MSG_INFO);
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

void oddyseyx86GPIO::monitorGPIO_Flowsensor(bool *abortLoop)
{
        int fd, len;
        char buf[MAX_BUF];
        struct pollfd pfd;

        string GPIO = "" + to_string(IO_PIN_FLOW_SENSOR);
        string command("/sys/class/gpio/gpio");
        command += GPIO;
        // command += "/edge";

        // input
        fd = open(command.c_str(), O_WRONLY);
        write(fd, "in", 3);
        close(fd);

        // debugOutput::sendMessage("Setup flow input listener" + to_string(*abortLoop), MSG_INFO);

        while (!*abortLoop)
        {

                usleep(1000); // mandatory to relieve processor. If omitted, cpu will race at 100% all the time. (or if four cores, alll the cores all the time). Optimize to make as high as possible

                char flowsensor_state_char;

                command = "/sys/class/gpio/gpio" + GPIO + "/value";
                fd = open(command.c_str(), O_RDONLY);

                if (fd >= 0)
                {
                        read(fd, &flowsensor_state_char, 1);
                        close(fd);
                }
                else
                {
                        debugOutput::sendMessage("gpio PROBLEM (timeout?!)", MSG_ERROR);
                }

                using namespace std::chrono;
                uint64_t now = duration_cast<microseconds>(system_clock::now().time_since_epoch()).count();
                // using namespace std::chrono;
                // uint64_t now = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();

                if (flowsensor_state_memory != flowsensor_state_char)
                {
                        flowsensor_most_recent_edge_millis = now;
                }

                if ((now - flowsensor_most_recent_edge_millis) > FLOWSENSOR_DEJITTER_MICROS) // Dejittering of the sensor.
                {
                        if (flowsensor_state_char == '1')
                        {
                                flowsensor_stable_state = true;
                        }
                        else
                        {
                                flowsensor_stable_state = false;
                        }
                }

                flowsensor_state_memory = flowsensor_state_char;

                if (flowsensor_stable_state_memory != flowsensor_stable_state)
                {
                        // stable edge
                        // debugOutput::sendMessage("Stable edge", MSG_INFO);

                        if (flowsensor_stable_state)
                        {
                                // pos edge
                                m_pDispenser->registerFlowSensorTick(); // trigger the callback
                                // debugOutput::sendMessage("Flow tick received interrupt!", MSG_INFO);
                        }
                        else
                        {
                                // neg edge
                        }
                }
                flowsensor_stable_state_memory = flowsensor_stable_state;
        }

        return;
}

// void oddyseyx86GPIO::monitorGPIO_Buttons_powerAndMaintenance()
// {
//         //        debugOutput::sendMessage("monitorGPIO", MSG_INFO);  //nuke this later it will cause so much spam
//         int fd, len;
//         char buf[MAX_BUF];

//         struct pollfd pfd;

//         // string GPIO = "391";
//         string GPIO = "" + to_string(IO_PIN_BUTTON_MAINTENANCE_SHUTDOWN_EDGE_DETECTOR);
//         string command("/sys/class/gpio/gpio");
//         command += GPIO;
//         command += "/edge";

//         // debugOutput::sendMessage("++++++++++++++++++" + command, MSG_INFO);

//         // set the pin to interrupt
//         fd = open(command.c_str(), O_WRONLY);
//         write(fd, "rising", 4);
//         close(fd);

//         command = "/sys/class/gpio/gpio" + GPIO + "/value";
//         fd = open(command.c_str(), O_RDONLY);
//         pfd.fd = fd;
//         pfd.events = POLLPRI | POLLERR;

//         lseek(fd, 0, SEEK_SET);
//         int ret = poll(&pfd, 1, 100000);
//         char c;
//         read(fd, &c, 1);

//         if (0 == ret)
//         {
//                 // debugOutput::sendMessage("gpioTimeout", MSG_INFO);
//         }
//         else
//         {
//                 if ((c == '1') && (button_state_memory != c))
//                 {
//                         debugOutput::sendMessage("Power button pushed", MSG_INFO);
//                         usleep(1000000); // todo: why one second?!?!?! lode
//                         if (readButtonPin(IO_PIN_BUTTON_SHUTDOWN))
//                         {
//                                 debugOutput::sendMessage("POWER OFF\n", MSG_INFO);
//                                 system("echo D@nkF1ll$ | sudo -S shutdown -h now");
//                         }
//                         else if (readButtonPin(IO_PIN_BUTTON_MAINTENANCE))
//                         {
//                                 debugOutput::sendMessage("Sending MM to UI. Deprecated. UI not listening.\n", MSG_INFO);
//                                 // ENTER MAINTENANCE MODE!
//                                 //m_pMessaging->sendMessageOverIP("MM", true); // send to UI
//                         }
//                         else
//                         {
//                                 debugOutput::sendMessage("Invalid press. Ghost button ?! Buttons not connected?", MSG_INFO);
//                         }
//                         usleep(5000); // Sleep to make sure debug gets chance to print
//                 }
//         }
//         button_state_memory = c;
//         close(fd);
//         return;
// }

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
