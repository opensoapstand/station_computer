/*
 * Demo application using Dallas temperature and OneWire protocol libraries
 * on top of Linux USART "driver".
 *
 * main.c
 *
 *  Created on: Mar 30, 2018
 *      Author: Darau, blė
 *
 *  This file is a part of personal use libraries developed to be used
 *  on various microcontrollers and Linux devices.
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 */

#define MAX_OW_DEV_COUNT 10

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

#include <ow_driver.h>
#include <onewire.h>
#include <dallas.h>
#include <sqlite3.h>

uint8_t scratch_pad[__SCR_LENGTH];
uint8_t dev_count;
uint8_t dev_addr[MAX_OW_DEV_COUNT][8];

ow_driver_ptr driver;
owu_struct_t o2;
int i, s;
float temp;

static void setup_signals(void);

//sqlite3 *db;

int main(void) {

        setup_signals();

        s = init_driver(&driver, 0);
        if (s == OW_OK) {
                //printf("Init completed\n");
        } else {
                printf("Failed to init driver: %d", s);
                return EXIT_FAILURE;
        }
        owu_init(&o2, driver);

        dev_count = 0;
        while(owu_search(&o2, dev_addr[dev_count])) {
                //printf("Device found %d\n", ++dev_count);
                if (dev_count >= MAX_OW_DEV_COUNT) {
                        break;
                }
        }


        ds_convert_all(&o2);
        sleep(1);
        ds_read_scratchpad(&o2, dev_addr[0], scratch_pad);

        temp = ds_get_temp_c(scratch_pad);

        printf("%.1f", temp);
        printf("\n");


        release_driver(&driver);
        return EXIT_SUCCESS;
}


static void setup_signals(void)
{
        struct sigaction s;

        memset(&s, 0, sizeof(s));
        s.sa_flags = 0;
        sigaction(SIGINT, &s, NULL);
        sigaction(SIGTERM, &s, NULL);
        sigaction(SIGUSR1, &s, NULL);
        sigaction(SIGUSR2, &s, NULL);

        s.sa_handler = SIG_IGN;
        sigaction(SIGPIPE, &s, NULL);
}
