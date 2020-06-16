//***************************************
//
// oddyseyX86gpio.cpp
// implementation of GPIO for native x86 pins on Oddysey board
//
// created: 15-06-2020
// by: Denis Londry
//
// copyright 2020 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#include "oddyseyx86GPIO.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <poll.h>
#include <iostream>


#define SYSFS_GPIO_DIR "/sys/class/gpio"
#define MAX_BUF 64


oddyseyx86GPIO::oddyseyx86GPIO(int address)
{
	debugOutput::sendMessage("oddyseyx86GPIO", INFO);
	int fd, len;
	char buf[MAX_BUF];

	m_nAddress = address;

	fd = open(SYSFS_GPIO_DIR "/export", O_WRONLY);
	if (fd < 0) {
		debugOutput::sendMessage("~oddyseyx86GPIO could not open the gpio", ERROR);
		return;
	}

	len = snprintf(buf, sizeof(buf), "%d", m_nAddress);
	write(fd, buf, len);
	close(fd);

	return;
}


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

	len = snprintf(buf, sizeof(buf), "%d", m_nAddress);
	write(fd, buf, len);
	close(fd);
	return;
}

DF_ERROR oddyseyx86GPIO::setDirection(bool input)
{
	debugOutput::sendMessage("setDirection", INFO);
	DF_ERROR df_ret = ERROR_FS_FAULT;
	int fd, len;
	char buf[MAX_BUF];

	len = snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR  "/gpio%d/direction", gpio);

	fd = open(buf, O_WRONLY);
	if (fd >= 0) {
		if (input)
			write(fd, "in", 3);
		else
			write(fd, "out", 4);

		close(fd);
		df_ret = OK;
	}	
		
	return df_ret;
}

DF_ERROR oddyseyx86GPIO::readPin(bool * level)
{
	debugOutput::sendMessage("readPin", INFO);
	DF_ERROR df_ret = ERROR_FS_FAULT;
	int fd, len;
	char buf[MAX_BUF];
	char ch;

	len = snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/value", m_nAddress);

	fd = open(buf, O_RDONLY);
	if (fd >= 0) {
		read(fd, &ch, 1);

		if (ch != '0') {
			*level = 1;
		}
		else {
			*level = 0;
		}

		close(fd);
		df_ret = OK;
	}
	
	return df_ret;
}

DF_ERROR oddyseyx86GPIO::writePin(bool level)
{
	debugOutput::sendMessage("writePin", INFO);
	DF_ERROR df_ret = ERROR_FS_FAULT;
	int fd, len;
	char buf[MAX_BUF];

	len = snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/value", m_nAddress);

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

void oddyseyx86GPIO::monitorGPIO()
{
	debugOutput::sendMessage("monitorGPIO", INFO);  //nuke this later it will cause so much spam
	int fd, len;
	char buf[MAX_BUF];
	string GPIO = std::to_string(m_nAddress);
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

	this->m_pf();  //trigger the callback

	return;

}
