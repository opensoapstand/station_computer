#ifndef _pcbEN134_H
#define _pcbEN134_H

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
#define SLOT_COUNT 4 
#define PCA9534_ADDRESS_SLOT_1  0b0100000
#define PCA9534_ADDRESS_SLOT_2  0b0100001
#define PCA9534_ADDRESS_SLOT_3  0b0100010
#define PCA9534_ADDRESS_SLOT_4  0b0100011

#define PCA9534_TMP_SLOT2_ADDRESS  PCA9534_ADDRESS_SLOT_2

#define PCA9534_PIN_OUT_SOLENOID 0
#define PCA9534_PIN_OUT_PUMP_ENABLE 1
#define PCA9534_PIN_OUT_PUMP_DIR 2
#define PCA9534_PIN_IN_PUMP_FAULT 3
#define PCA9534_PIN_IN_BUTTON 4
#define PCA9534_PIN_OUT_BUTTON_LED_LOW_IS_ON 5
#define PCA9534_PIN_IN_FLOW_SENSOR_TICKS 6
#define PCA9534_PIN_OUT_FLOW_SENSOR_ENABLE 7

#define PIC_ADDRESS      0b0110000
#define MAX31760_ADDRESS 0b1010000
#define DS2485Q_ADDRESS  0b1000000
#define MCP3424T_ADDRESS 0b1101000

class pcbEN134
{
public:
    pcbEN134(void);
    pcbEN134(const char *);
    ~pcbEN134();

    void setup(void);

    unsigned char getPumpPWM(void);
    bool setPumpPWM(uint8_t pwm_val);
    bool setPumpDirection(uint8_t slot, bool forwardElseReverse);

    bool setPumpEnable(unsigned char pump_number);
    bool setPumpsDisableAll();


    bool getDispenseButtonStateDebounced(void);
    bool getDispenseButtonEdge(void);
    void setSingleDispenseButtonLight(uint8_t slot, bool poweron);
    void virtualButtonPressHack(void);
    void virtualButtonUnpressHack(void);
    void dispenseButtonRefresh();
    void setPCA9534Output(uint8_t slot, int posIndex, bool onElseOff);
    
private:
    bool getDispenseButtonState(void);
    bool is_initialized;
    int i2c_handle = -1;
    char *i2c_bus_name;

    bool pic_pwm_found = false;
    
    uint8_t get_PCA9534_address_from_slot(uint8_t slot);

    bool SendByte(unsigned char address, unsigned char reg, unsigned char byte);
    unsigned char ReadByte(unsigned char address, unsigned char reg);
    bool set_i2c_address(unsigned char address);
    void setup_i2c_bus(void);
    bool check_pcb_configuration(void);
    void initialize_pcb(void);

    bool dispenseButtonStateMemory;
    bool dispenseButtonIsDebounced;
    bool dispenseButtonStateDebounced;
    uint64_t dispenseButtonDebounceStartEpoch;

};

#endif // _pcbEN134_H
