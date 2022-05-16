#ifndef _DSED8344_H
#define _DSED8344_H

#include <cstddef>
#include <cstdint>

// This is a bit of a hack, but on the Odyssey we need to explicitly
// include code to access the I2C bus.  We could probably make this
// work cleanly on the rPi but I got tired of fighting compiler
// warnings...
#ifdef __arm__
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <i2c/smbus.h>
#else
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
#include "../dftypes.h"

#include "../objects/debugOutput.h"

#define PCA9534_ADDRESS  0b0100000
#define PIC_ADDRESS      0b0110000
#define MAX31760_ADDRESS 0b1010000
#define DS2485Q_ADDRESS  0b1000000
#define MCP3424T_ADDRESS 0b1101000

class dsed8344
{
public:
    dsed8344(void);
    dsed8344(const char *);
    ~dsed8344();

    unsigned char getPumpPWM(void);
    bool setPumpPWM(uint8_t pwm_val);
    bool setPumpDirection(bool forwardElseReverse);
    bool setPumpEnable(unsigned char pump_number);
    bool setPumpsDisableAll();
    unsigned short getPumpSpeed(void);
    bool getDispenseButtonStateDebounced(void);
    bool getDispenseButtonEdge(void);
    void setDispenseButtonLight(bool poweron);
    void setup(void);
    void virtualButtonPressHack(void);
    void virtualButtonUnpressHack(void);
    void dispenseButtonRefresh();
    void setPCA9534Output(int posIndex, bool onElseOff);

private:
    bool getDispenseButtonState(void);
    bool is_initialized;
    int i2c_handle = -1;
    char *i2c_bus_name;

    bool pic_pwm_found = false;
    bool max31760_pwm_found = false;

    bool SendByte(unsigned char address, unsigned char reg, unsigned char byte);
    unsigned char ReadByte(unsigned char address, unsigned char reg);
    bool set_i2c_address(unsigned char address);
    void setup_i2c_bus(void);
    bool check_8344_configuration(void);
    void initialize_8344(void);

    bool dispenseButtonStateMemory;
    bool dispenseButtonIsDebounced;
    bool dispenseButtonStateDebounced;
    uint64_t dispenseButtonDebounceStartEpoch;

};

#endif // _DSED8344_H
