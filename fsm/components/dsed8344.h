#ifndef _DSED8344_H
#define _DSED8344_H

#include <cstddef>
#include <cstdint>

// This is a bit of a hack, but on the Odyssey we need to explicitly
// include code to access the I2C bus.  We could probably make this
// work cleanly on the rPi but I got tired of fighting compiler
// warnings...
#ifndef __arm__
#include "../smbus/smbus.h"
#endif 

#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <cstdlib>
#include <cstdio>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "../objects/debugOutput.h"

#define PCA9534_ADDRESS  0b0100000
#define MAX31760_ADDRESS 0b1010000
#define DS2485Q_ADDRESS  0b1000000
#define MCP3424T_ADDRESS 0b1101000

class dsed8344
{
public:
    dsed8344(const char *);
    ~dsed8344();
    
    unsigned char getPumpPWM (void);
    bool setPumpPWM (unsigned char pwm_val);
    bool setPumpDirection (bool direction);
    bool startPump (unsigned char pump_number);
    bool stopPump ();
    unsigned short getPumpTach (void);
    bool getButton (void);
    void setButtonPower (bool poweron);

private:
    int i2c_handle = -1;
    char *i2c_bus_name;
    
    bool SendByte (unsigned char address, unsigned char reg, unsigned char byte);
    unsigned char ReadByte (unsigned char address, unsigned char reg);
    bool set_i2c_address (unsigned char address);
    bool check_8344_configuration (void);
    void initialize_8344 (void);
};


#endif  // _DSED8344_H
