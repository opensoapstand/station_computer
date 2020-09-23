//***************************************
//
// oddyseyX86gpio.cpp
// GPIO implementation for 
// NATIVE x86 pins on Oddysey board
//
// created: 15-06-2020
// by: Denis Londry
//
// copyright 2020 by Drinkfill Beverages Ltd
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

/* 
 * CTOR with Pin reference
 * Typically for Initializing sensors directly...Flow Sensors for now.
 * SEE Manual, Use last three digits to initialize class
 * i.e. GPIO 335; new oddyseyx86GPIO(335);
 */
oddyseyx86GPIO::oddyseyx86GPIO(int pinNumber)
{
	debugOutput::sendMessage("------oddyseyx86GPIO------", INFO);
	int fd, len;
	char buf[MAX_BUF];

	m_nPin = pinNumber;

	fd = open(SYSFS_GPIO_DIR "/export", O_WRONLY);
	if (fd < 0) {
		debugOutput::sendMessage("~oddyseyx86GPIO could not open the gpio", ERROR);
		return;
	}

	len = snprintf(buf, sizeof(buf), "%d", m_nPin );
	write(fd, buf, len);
	close(fd);

	return;
}

// DTOR
oddyseyx86GPIO::~oddyseyx86GPIO()
{
	debugOutput::sendMessage("~oddyseyx86GPIO", INFO);
	int fd, len;
	char buf[MAX_BUF];

	fd = open(SYSFS_GPIO_DIR "/unexport", O_WRONLY);
	if (fd < 0) {
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
	debugOutput::sendMessage("setDirection", INFO);
	DF_ERROR df_ret = ERROR_MECH_FS_FAULT;
	int fd, len;
	char buf[MAX_BUF];

	//Composes a string with the same text that would be printed if format was used on printf, but instead of being printed, 
	//the content is stored as a C string in the buffer pointed by s
	len = snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR  "/gpio%d/direction", m_nPin);

	fd = open(buf, O_WRONLY);
	if (fd >= 0) {
		if (INPUT == input){
			cout << "direction input set" << endl;
			write(fd, "in", 3);
		}
		else
			write(fd, "out", 4);

		close(fd);
		df_ret = OK;
	}	
		
	return df_ret;
}

// Checks for Level sensor on Waste tank...
// TODO: Implementation is commented out and needs testing.;
//		 A SPECIFIC function name change REQUIRED. i.e. readWastePinLevel
DF_ERROR oddyseyx86GPIO::readPin(bool * level)
{
	debugOutput::sendMessage("readPin", INFO);
	DF_ERROR df_ret = ERROR_MECH_FS_FAULT;
	int fd, len;
	char buf[MAX_BUF];
	char ch;

	// test to see if level exists
	if (NULL != level) {
		len = snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/value", m_nPin);

		fd = open(buf, O_RDONLY);
		if (fd >= 0) {
			read(fd, &ch, 1);

			if (ch != '0') {
				cout << "ON" << endl;
				*level = true;
			}
			else {
				cout << "OFF" << endl;
				*level = false;
			}

			close(fd);
			df_ret = OK;
		}
	} else {
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
	if (fd >= 0) {
		if (level)
			write(fd, "1", 2);
		else
			write(fd, "0", 2);

		close(fd);
		df_ret = OK;
	}
	
	return df_ret;
}


// Threaded function call to monitor Odyssecy GPIO pin activity.
void oddyseyx86GPIO::monitorGPIO()
{
	debugOutput::sendMessage("monitorGPIO", INFO);  //nuke this later it will cause so much spam
	int fd, len;
	char buf[MAX_BUF];
	string GPIO = std::to_string(m_nPin);
	string command("/sys/class/gpio/gpio");
	command += GPIO;
	command += "/value";

	fd = open(command.c_str(), O_RDONLY);

	pfd.fd = fd;
	pfd.events = POLLPRI;

	lseek(fd, 0, SEEK_SET);    /* consume any prior interrupt */
	read(fd, buf, sizeof buf);

	poll(&pfd, 1, -1);         /* wait for interrupt */

	lseek(fd, 0, SEEK_SET);    /* consume interrupt */
	read(fd, buf, sizeof buf);

	m_pDrink->registerFlowSensorTick();  //trigger the callback

	return;

}

// Utility
// TODO: Should be interpreting for flow sensor buffer...this does not seem to provide value...
string oddyseyx86GPIO::command_to_string(string cmd) {

  string data;
  FILE * stream;
  const int max_buffer = 256;
  char buffer[max_buffer];
  cmd.append(" 2>&1");

  stream = popen(cmd.c_str(), "r");
  if (stream) {
    while (!feof(stream))
      if (fgets(buffer, max_buffer, stream) != NULL) data.append(buffer);
        pclose(stream);
    }
  return data;
}
