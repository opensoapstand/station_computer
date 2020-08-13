#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include "i2c/i2c.h"

#include <errno.h>

void print_i2c_data(const unsigned char *data, size_t len)
{
    int i = 0;

    for (i = 0; i < static_cast<int>(len); i++) {

        if (i % 16 == 0) {

            fprintf(stdout, "\n"); //when printed to address 16 start next line
        }

        fprintf(stdout, "%02x ", data[i]);  //2 digits in hex
    }

    fprintf(stdout, "\n");
}


//int main(int argc, char **argv)
int main()
{
        char i2c_dev_desc[128] = "i2c expansion with MCP23017";
        I2C_READ_HANDLE i2c_read_handle = i2c_read;
        I2C_WRITE_HANDLE i2c_write_handle = i2c_write;
        unsigned int addr = 0x27, iaddr_bytes = 1, page_bytes = 8, bus_num = 0;

        /* Open i2c bus */
        int bus;
        char bus_name[32] = "./dev/i2c-0";
        memset(bus_name, 0, sizeof(bus_name));

        if (snprintf(bus_name, sizeof(bus_name), "/dev/i2c-%u", bus_num) < 0) {

            fprintf(stderr, "Format i2c bus name error -> %s!\n", strerror(errno));
            exit(-3);
        }

        if ((bus = i2c_open(bus_name)) == -1) {

            fprintf(stderr, "Open i2c bus:%s error -> %s!\n", bus_name, strerror(errno));
            exit(-3);
        }

        /* Init i2c device */
        I2CDevice device;
        memset(&device, 0, sizeof(device));
        i2c_init_device(&device);

        device.bus = bus;
        device.addr = addr & 0x3ff;
        device.page_bytes = page_bytes;
        device.iaddr_bytes = iaddr_bytes;

        /* Print i2c device description */
        fprintf(stdout, "%s\n", i2c_dev_desc);
        fprintf(stdout, "%s\n", i2c_get_device_desc(&device, i2c_dev_desc, sizeof(i2c_dev_desc))); //The fprintf() function is used to write set of characters into file. It sends formatted output to a stream.

        size_t i;
        unsigned char buf[16];
        size_t buf_size = sizeof(buf);
        memset(buf, 0, buf_size);

        /* I/O r/w 0x00 - 0xff */
        if (i2c_read_handle == i2c_read) {

            for (i = 0; i < buf_size; i++) {
                buf[i] = 255;
                //buf[i] = i;
                //std::cout << static_cast<unsigned int>(buf[i]) << " "; //0 to 255
            }
            //std::cout << std::endl;
        }
        /* ioctl r/w 0xff - 0x0 */
        else {

            for (i = 0; i < buf_size; i++) {

                buf[i] = 0xff - i;
                //std::cout << static_cast<unsigned int>(buf[i]) << " "; //255 to 0
            }
            //std::cout << std::endl;
        }

        /* Print before write */
        fprintf(stdout, "Write data:\n");
        print_i2c_data(buf, buf_size);

        if (i2c_write_handle(&device, 0x0, buf, buf_size) != buf_size) {

            fprintf(stderr, "Write i2c error -> %s!\n", strerror(errno));
            exit(-4);
        }

        fprintf(stdout, "\nWrite success, prepare read....\n");

        /* Read */
        usleep(100000);
        memset(buf, 0, buf_size);

        if (i2c_read_handle(&device, 0x0, buf, buf_size) != buf_size) {

            fprintf(stderr, "Read i2c error -> %s!\n", strerror(errno));
            exit(-5);
        }

        /* Print read result */
        fprintf(stdout, "Read data:\n");
        print_i2c_data(buf, buf_size);

        i2c_close(bus);
        return 0;
}
